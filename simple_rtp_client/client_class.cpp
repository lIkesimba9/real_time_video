#include "client_class.h"
#include <string>
#include <map>
#include <fstream>
#include "../common/json.h"
using namespace std;

gboolean RTPClient::BusCallback (GstBus     *bus,
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


map<string,RTPClient::ConfigData *> RTPClient::LoadFromJson(istream& input) {
    using namespace Json;
    map<string, ConfigData*> res;
    Document doc = Load(input);
    const Node& root = doc.GetRoot().AsMap().at("RTPServer");
    {
        auto node = root.AsMap().at("network");
        NetworkParams* params = new NetworkParams;
        params->host_addr = node.AsMap().at("host_addr").AsString();
        params->client_addr = node.AsMap().at("client_addr").AsString();
        params->host_rtp_port = node.AsMap().at("host_rtp_port").AsInt();
        params->host_rtcp_port = node.AsMap().at("host_rtcp_port").AsInt();
        params->client_rtcp_port = node.AsMap().at("client_rtcp_port").AsInt();
        res["network"] = params;
    }


    return res;
}
void RTPClient::LoadParamFromConfigFile(std::string path){

    ifstream in(path);
    if (in.is_open())
    {
        params_ = LoadFromJson(in);
        in.close();
    } else
    {
        params_["network"] = new NetworkParams();

    }
}
RTPClient::RTPClient(std::string pathToParams)
{
    registry_ = gst_registry_get();
    gst_registry_scan_path(registry_,"gstgccanalysis/");
    LoadParamFromConfigFile(pathToParams);
    pipeline_ = CreatePipeline();
    if (pipeline_ == NULL)
        throw invalid_argument("Error create pipeline");

    bus_ = gst_pipeline_get_bus (GST_PIPELINE (pipeline_));
    watch_id_ = gst_bus_add_watch (bus_, BusCallback, loop_);
    gst_object_unref (bus_);


    GstStateChangeReturn ret;
    ret = gst_element_set_state(pipeline_, GST_STATE_PLAYING);

    g_main_loop_run (loop_);
}
GstElement* RTPClient::CreatePipeline() {
   GstElement *pipeline,*udpSrcRtp,*videconverter,
        *x264decoder,*rtph264depay,*xvimagesink,
        *rtpbin,*udpSrcRtcp,*udpSinkRtcp,*tee,
        *gccanalysis,*queueBeforeH264depay,*queueForCallback;

    pipeline = gst_pipeline_new("rtpStreamerRecv");

    // Создаю udpsrc для приема rtp пакетов.
    udpSrcRtp = gst_element_factory_make("udpsrc","source");
    // Создаю элемент управющий rtp сесией
    rtpbin = gst_element_factory_make("rtpbin","rtpbin");

    // Создаю udp сток для приема rtсp пакетов
    udpSinkRtcp = gst_element_factory_make("udpsink","udpSinkRtcp");
    // Создаю udp источник для отправки rtcp пакетов.
    udpSrcRtcp = gst_element_factory_make("udpsrc","udpSrcRtcp");

    // Создаю элемент который распакуют данные из rtp пакетов.
    rtph264depay = gst_element_factory_make("rtph264depay","rtpdepay");
    // Создаю декодер
    x264decoder = gst_element_factory_make("avdec_h264","decoder");

    //Создаю тройник
    tee = gst_element_factory_make("tee","tee");

    //создаю просто элемент
    gccanalysis = gst_element_factory_make("gccanalysis","gccanalysis");

    // Создаю очереди для синхронизации
    queueBeforeH264depay = gst_element_factory_make("queue","queueBeforeH264depay");
    queueForCallback = gst_element_factory_make("queue","queueForCallback");




    // Создаю элелмент который преобразует данные с кодера для воспроизведения.
    videconverter = gst_element_factory_make("videoconvert","converter");
    // Создаю для показа видео входящего видео потока.
    xvimagesink = gst_element_factory_make("xvimagesink","video");


    if (!pipeline || !udpSrcRtp || !x264decoder || !rtph264depay || !rtpbin ||
        !udpSrcRtp || !udpSinkRtcp || !udpSrcRtcp || !xvimagesink || !videconverter ||
        !tee || !gccanalysis || !queueBeforeH264depay || !queueForCallback)
    {
        cerr << "Not all elements could be created.\n";
        return NULL;
    }
    // Задаю свойство udpsrt для приема RTP пакетов с которог захватывать видео
    g_object_set(G_OBJECT(udpSrcRtp),"caps",gst_caps_from_string("application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264"),NULL);
    g_object_set(G_OBJECT(udpSrcRtp),"port",5000,NULL);


    // Устанавливаю параметры для upd сойденений.
     NetworkParams *networkParams = (NetworkParams *)params_["network"];

     g_object_set(G_OBJECT(udpSrcRtcp),"address",networkParams->client_addr.c_str(),NULL);
     g_object_set(G_OBJECT(udpSrcRtcp),"port",networkParams->host_rtcp_port,NULL);
    g_object_set(G_OBJECT (udpSrcRtcp), "caps", gst_caps_from_string("application/x-rtcp"), NULL);
    g_object_set(G_OBJECT(udpSinkRtcp),"host",networkParams->host_addr.c_str(),NULL);
    g_object_set(G_OBJECT(udpSinkRtcp),"port",networkParams->client_rtcp_port,NULL);
//    g_object_set(G_OBJECT(udpSinkRtcp),"sync",FALSE,NULL);
//    g_object_set(G_OBJECT(udpSinkRtcp),"async",FALSE,NULL);

    // Добавляю элементы в контейнер
    gst_bin_add_many(GST_BIN(pipeline),udpSrcRtp,udpSrcRtcp,rtpbin,udpSinkRtcp,
                     rtph264depay,x264decoder,videconverter,xvimagesink,tee,
                     gccanalysis,queueForCallback,queueBeforeH264depay,NULL);

    // Сойденяю PADы.

    if (!LinkStaticAndRequestPads(udpSrcRtp,rtpbin,"src","recv_rtp_sink_%u"))
    {
        cerr << "Error create link, beetwen udpSrcRtp and rtpbin\n";
        return NULL;
    }



    if (!LinkStaticAndRequestPads(udpSrcRtcp,rtpbin,"src","recv_rtcp_sink_%u"))
    {
        cerr << "Error create link, beetwen udpSrcRtcp and rtpbin\n";
        return NULL;
    }



    if (!LinkRequestAndStaticPads(rtpbin,udpSinkRtcp,"send_rtcp_src_%u","sink"))
    {
        cerr << "Error create link, beetwen rtpbin and udpSinkRtcp\n";
        return NULL;
    }

    // сойденяю остальные элементы
    // Подключаю сигнал для ПАДа, который доступен иногда.
    g_signal_connect (rtpbin, "pad-added", G_CALLBACK (LinkDynamicAndOtherPads),tee);

    if (!LinkRequestAndStaticPads(tee,queueBeforeH264depay,"src_%u","sink"))
    {
        cerr << "Error create link, beetwen tee and queueBeforeH264depay\n";
        return NULL;
    }

    if (!LinkRequestAndStaticPads(tee,queueForCallback,"src_%u","sink"))
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
