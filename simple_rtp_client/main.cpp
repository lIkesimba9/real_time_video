#include <iostream>
#include <gst/gst.h>

#include <gst/rtp/rtp.h>
#include <sys/time.h>
#include <queue>
#include <thread>
#include <future>
#include <utility>
#include <fstream>
#include <cmath>
#include <map>
#include "client_class.h"

using namespace std;

/*
 * Receiver setup
 *
 *  receives H264 encoded RTP video on port 5000, RTCP is received on  port 5001.
 *  the receiver RTCP reports are sent to port 5005
 *
 *
 *
 *
 *
 *
 *                                                       .---------.    .---------------.
 *                                                       |queue    |    |gstgccanalysis |
 *                                                   .->sink      src->sink             |
 *                                                   |   '---------'    '---------------'
 *                                                   |
 *             .-------.      .----------.     .----src---.    .---------.   .---------.   .-------.   .------------.   .-----------.
 *  RTP        |udpsrc |      | rtpbin   |     |tee       |    |queue    |   |h264depay|   |h264dec|   |videoconvert|   |xvimagesink|
 *  port=5000  |      src->recv_rtp recv_rtp->sink       src->sink      src->sink     src->sink   src->sink        src->sink        |
 *             '-------'      |          |     '----------'    '---------'   '---------'   '-------'   '------------'   '-----------'
 *                            |          |
 *                            |          |     .-------.
 *                            |          |     |udpsink|  RTCP
 *                            |    send_rtcp->sink     | port=5005
 *             .-------.      |          |     '-------' sync=false
 *  RTCP       |udpsrc |      |          |               async=false
 *  port=5001  |     src->recv_rtcp      |
 *             '-------'      '----------'
 */


/*
static gboolean bus_call (GstBus     *bus,
                         GstMessage *msg,
                         gpointer    data)
{
    GMainLoop *loop = (GMainLoop *)data;

    switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
        g_print ("End-of-stream\n");
        g_main_loop_quit (loop);
        break;
    case GST_MESSAGE_ERROR: {
        gchar *debug = NULL;
        GError *err = NULL;


        gst_message_parse_error (msg, &err, &debug);

        g_print ("Error: %s\n", err->message);
        g_error_free (err);

        if (debug) {
            g_print ("Debug details: %s\n", debug);
            g_free (debug);
        }


        g_main_loop_quit (loop);
        break;
    }
    case GST_MESSAGE_INFO: {
        gchar *debug = NULL;
        GError *err = NULL;

        gst_message_parse_info(msg, &err, &debug);

        g_print ("INFO: %s\n", err->message);
        g_error_free (err);

        if (debug) {
            g_print ("Debug details: %s\n", debug);
            g_free (debug);
        }


        break;
    }
    case GST_MESSAGE_WARNING: {
        gchar *debug = NULL;
        GError *err = NULL;

        gst_message_parse_warning(msg, &err, &debug);

        g_print ("WARNING: %s\n", err->message);
        g_error_free (err);

        if (debug) {
            g_print ("Debug details: %s\n", debug);
            g_free (debug);
        }


        break;
    }



    default:
        break;
    }

    return TRUE;
}



static void cb_new_rtp_recv_src_pad (GstElement *element,
                                    GstPad     *pad,
                                    gpointer    data)
{

    GstElement *secondElement = (GstElement *) data;
    GstPad *sinkPad;
    sinkPad = gst_element_get_static_pad (secondElement, "sink");
    gst_pad_link (pad, sinkPad);
    gst_object_unref (sinkPad);

}

bool linkStaticAndRequestPads(GstElement *sourse,GstElement *sink,gchar *nameSrcPad,gchar *nameSinkPad)
{

    GstPad *srcPad = gst_element_get_static_pad(sourse,nameSrcPad);
    GstPad *sinkPad = gst_element_get_request_pad(sink,nameSinkPad);
    GstPadLinkReturn ret_link = gst_pad_link(srcPad,sinkPad);
    if (ret_link != GST_PAD_LINK_OK)
    {
        cerr << "Error create link, static and request pad\n";
        return false;
    }
    gst_object_unref(GST_OBJECT(srcPad));
    gst_object_unref(GST_OBJECT(sinkPad));
    return true;
}
bool linkRequestAndStaticPads(GstElement *sourse,GstElement *sink,gchar *nameSrcPad,gchar *nameSinkPad)
{

    GstPad *srcPad = gst_element_get_request_pad(sourse,nameSrcPad);
    GstPad *sinkPad = gst_element_get_static_pad(sink,nameSinkPad);
    GstPadLinkReturn ret_link = gst_pad_link(srcPad,sinkPad);
    if (ret_link != GST_PAD_LINK_OK)
    {
        cerr << "Error create link, request and statitc pad\n";
        return false;
    }
    gst_object_unref(GST_OBJECT(srcPad));
    gst_object_unref(GST_OBJECT(sinkPad));
    return true;
}




GstElement *create_pipeline(map<string,string> &params){

        GstElement *pipeline,*udpSrcRtp,*videconverter,
        *x264decoder,*rtph264depay,*xvimagesink,
        *rtpbin,*udpSrcRtcp,*udpSinkRtcp,*tee,
        *gccanalysis,*queueBeforeH264depay,*queueForCallback;

    pipeline = gst_pipeline_new("rtpStreamerRecv");

    // ???????????? udpsrc ?????? ???????????? rtp ??????????????.
    udpSrcRtp = gst_element_factory_make("udpsrc","source");
    // ???????????? ?????????????? ?????????????????? rtp ????????????
    rtpbin = gst_element_factory_make("rtpbin","rtpbin");

    // ???????????? udp ???????? ?????? ???????????? rt??p ??????????????
    udpSinkRtcp = gst_element_factory_make("udpsink","udpSinkRtcp");
    // ???????????? udp ???????????????? ?????? ???????????????? rtcp ??????????????.
    udpSrcRtcp = gst_element_factory_make("udpsrc","udpSrcRtcp");

    // ???????????? ?????????????? ?????????????? ?????????????????? ???????????? ???? rtp ??????????????.
    rtph264depay = gst_element_factory_make("rtph264depay","rtpdepay");
    // ???????????? ??????????????
    x264decoder = gst_element_factory_make("avdec_h264","decoder");

    //???????????? ??????????????
    tee = gst_element_factory_make("tee","tee");

    //???????????? ???????????? ??????????????
    gccanalysis = gst_element_factory_make("gccanalysis","gccanalysis");

    // ???????????? ?????????????? ?????? ??????????????????????????
    queueBeforeH264depay = gst_element_factory_make("queue","queueBeforeH264depay");
    queueForCallback = gst_element_factory_make("queue","queueForCallback");




    // ???????????? ???????????????? ?????????????? ?????????????????????? ???????????? ?? ???????????? ?????? ??????????????????????????????.
    videconverter = gst_element_factory_make("videoconvert","converter");
    // ???????????? ?????? ???????????? ?????????? ?????????????????? ?????????? ????????????.
    xvimagesink = gst_element_factory_make("xvimagesink","video");


    if (!pipeline || !udpSrcRtp || !x264decoder || !rtph264depay || !rtpbin ||
        !udpSrcRtp || !udpSinkRtcp || !udpSrcRtcp || !xvimagesink || !videconverter ||
        !tee || !gccanalysis || !queueBeforeH264depay || !queueForCallback)
    {
        cerr << "Not all elements could be created.\n";
        return NULL;
    }
    // ?????????? ???????????????? udpsrt ?????? ???????????? RTP ?????????????? ?? ?????????????? ?????????????????????? ??????????
    g_object_set(G_OBJECT(udpSrcRtp),"caps",gst_caps_from_string("application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264"),NULL);
    g_object_set(G_OBJECT(udpSrcRtp),"port",5000,NULL);


    // ???????????????????????? ?????????????????? ?????? upd ????????????????????.
    g_object_set(G_OBJECT(udpSrcRtcp),"address",params["client"].c_str(),NULL); // for localhost
   // g_object_set(G_OBJECT(udpSrcRtcp),"address","192.168.2.2",NULL);
    g_object_set(G_OBJECT(udpSrcRtcp),"port",5001,NULL);
    g_object_set(G_OBJECT (udpSrcRtcp), "caps", gst_caps_from_string("application/x-rtcp"), NULL);


   g_object_set(G_OBJECT(udpSinkRtcp),"host",params["server"].c_str(),NULL); // for localhost
   //  g_object_set(G_OBJECT(udpSinkRtcp),"host","10.2.1.2",NULL);
    g_object_set(G_OBJECT(udpSinkRtcp),"port",5005,NULL);
    g_object_set(G_OBJECT(udpSinkRtcp),"sync",FALSE,NULL);
    g_object_set(G_OBJECT(udpSinkRtcp),"async",FALSE,NULL);


   // g_object_set(G_OBJECT (rtpbin), "latency", 500, NULL);
    // ???????????????? ???????????????? ?? ??????????????????
    gst_bin_add_many(GST_BIN(pipeline),udpSrcRtp,udpSrcRtcp,rtpbin,udpSinkRtcp,
                     rtph264depay,x264decoder,videconverter,xvimagesink,tee,
                     gccanalysis,queueForCallback,queueBeforeH264depay,NULL);

    // ???????????????? PAD??.

    if (!linkStaticAndRequestPads(udpSrcRtp,rtpbin,"src","recv_rtp_sink_%u"))
    {
        cerr << "Error create link, beetwen udpSrcRtp and rtpbin\n";
        return NULL;
    }



    if (!linkStaticAndRequestPads(udpSrcRtcp,rtpbin,"src","recv_rtcp_sink_%u"))
    {
        cerr << "Error create link, beetwen udpSrcRtcp and rtpbin\n";
        return NULL;
    }



    if (!linkRequestAndStaticPads(rtpbin,udpSinkRtcp,"send_rtcp_src_%u","sink"))
    {
        cerr << "Error create link, beetwen rtpbin and udpSinkRtcp\n";
        return NULL;
    }

    // ???????????????? ?????????????????? ????????????????
    // ?????????????????? ???????????? ?????? ????????, ?????????????? ???????????????? ????????????.
    g_signal_connect (rtpbin, "pad-added", G_CALLBACK (cb_new_rtp_recv_src_pad),tee);

    if (!linkRequestAndStaticPads(tee,queueBeforeH264depay,"src_%u","sink"))
    {
        cerr << "Error create link, beetwen tee and queueBeforeH264depay\n";
        return NULL;
    }

    if (!linkRequestAndStaticPads(tee,queueForCallback,"src_%u","sink"))
    {
        cerr << "Error create link, beetwen tee and queueForCallback\n";
        return NULL;
    }

    if (!gst_element_link_many(queueBeforeH264depay,rtph264depay,x264decoder,videconverter,xvimagesink,NULL))
    {
        cerr << "Elements could not be linked other.\n";
        return NULL;

    }
    if (!gst_element_link_many(queueForCallback,gccanalysis,NULL))
    {
        cerr << "Elements could not be linked other.\n";
        return NULL;

    }
    return pipeline;

}
bool LoadParam(string fileName,map<string,string> &dict)
{
    ifstream in(fileName);
    if (!in.is_open())
    {
        return false;
    }
    in >> dict["server"];
    in >> dict["client"];
    in.close();
    return true;
}*/

int main(int argc, char *argv[])
{
/*
    map<string,string> params;
    if(!LoadParam("../../simple_rtp_server/config.conf",params))
    {
        cerr << "Not Found params!\n";
        return -1;
    }
    gst_init(0,0);

    GstRegistry *registry;

    registry = gst_registry_get();
    gst_registry_scan_path(registry,"gstgccanalysis/");


    GMainLoop *loop;
    loop = g_main_loop_new(NULL,FALSE);
    GstBus *bus;


    GstElement *pipeline = create_pipeline(params);
    if (pipeline == NULL)
    {
        cerr << "Error create pipeline!" << endl;
        return -1;
    }

    guint watch_id;
    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    watch_id = gst_bus_add_watch (bus, bus_call, loop);
    gst_object_unref (bus);


    GstStateChangeReturn ret;
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);

    g_main_loop_run (loop);

    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
    g_source_remove (watch_id);
    g_main_loop_unref (loop);

*/
RTPClient clt;
    return 0;
}
