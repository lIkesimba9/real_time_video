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
    GObject *session;
    g_signal_emit_by_name (rtpbin, "get-internal-session", 0, &session);
    g_signal_connect_after (session, "on-sending-rtcp",
                           G_CALLBACK (CallBackOnSendRtcp), NULL);
    g_object_unref(session);
    return pipeline;
}
gboolean RTPClient::CallBackOnSendRtcp(GstElement *rtpsession, GstBuffer *buf,   gboolean early, gpointer data){


    GstRTCPBuffer rtcpBuffer = GST_RTCP_BUFFER_INIT;
    GstRTCPPacket *rtcpPacket = (GstRTCPPacket*)malloc(sizeof(GstRTCPPacket));
    KmsRTCPPSFBAFBREMBPacket remb_packet;

    gboolean ret = FALSE;
    guint packet_ssrc;
    // AddSsrcsData data;

    GST_LOG_OBJECT (rtpsession, "Signal \"RTPSession::on-sending-rtcp\" ...");

    // elapsed = current_time - self->last_sent_time;
    /*  if (self->last_sent_time != 0 && (elapsed < REMB_MAX_INTERVAL * GST_MSECOND)) {
        GST_LOG_OBJECT (rtpsession, "... Not sending: Interval < %u ms", REMB_MAX_INTERVAL);
        return ret;
    }
*/
    if (!gst_rtcp_buffer_map (buf, GST_MAP_READWRITE, &rtcpBuffer)) {
        GST_WARNING_OBJECT (rtpsession, "... Cannot map RTCP buffer");
        return ret;
    }

    if (!gst_rtcp_buffer_add_packet (&rtcpBuffer, GST_RTCP_TYPE_PSFB, rtcpPacket)) {
        GST_WARNING_OBJECT (rtpsession, "... Cannot add RTCP packet");
        gst_rtcp_buffer_unmap (&rtcpBuffer);
        return false;
    }

    // Update the REMB bitrate estimations
    /*  if (!kms_remb_local_update (self)) {
        GST_LOG_OBJECT (rtpsession, "... Not sending: Stats not updated");
        gst_rtcp_packet_remove (&packet);
        goto end;
    }*/

    //const guint32 old_bitrate = self->remb_sent;
    guint32 new_bitrate = 5000;

    /* if (self->event_manager != NULL) {
        guint remb_local_max;

    remb_local_max = kms_utils_remb_event_manager_get_min (self->event_manager);
    if (remb_local_max > 0) {
        GST_TRACE_OBJECT (rtpsession, "Local max: %" G_GUINT32_FORMAT,
                         remb_local_max);
        new_bitrate = MIN (new_bitrate, remb_local_max);
    }
}*/



    remb_packet.bitrate = new_bitrate;
remb_packet.n_ssrcs = 0;
//data.rl = self;
//data.remb_packet = &remb_packet;
//g_slist_foreach (self->remote_sessions, (GFunc) add_ssrcs, &data);

g_object_get (rtpsession, "internal-ssrc", &packet_ssrc, NULL);
if (!RtcpPsfbAfbRembMarshallPacket(rtcpPacket, &remb_packet,
                                   packet_ssrc)) {
    gst_rtcp_packet_remove (rtcpPacket);
}

//self->last_sent_time = current_time;
ret = TRUE;

    gst_rtcp_buffer_unmap (&rtcpBuffer);
return ret;
}

/* Inspired in The WebRTC project */
static gboolean
compute_mantissa_and_6_bit_base_2_expoonent (guint32 input_base10,
                                            guint8 bits_mantissa, guint32 * mantissa, guint8 * exp)
{
    guint32 mantissa_max;
    guint8 exponent = 0;
    gint i;

    /* input_base10 = mantissa * 2^exp */
    if (bits_mantissa > 32) {
        GST_ERROR ("bits_mantissa must be <= 32");
        return FALSE;
    }

    mantissa_max = (1 << bits_mantissa) - 1;

    for (i = 0; i < 64; ++i) {
        if (input_base10 <= (mantissa_max << i)) {
            exponent = i;
            break;
        }
    }

    *exp = exponent;
    *mantissa = input_base10 >> exponent;

    return TRUE;
}
gboolean RTPClient::RtcpPsfbAfbRembMarshallPacket (GstRTCPPacket * rtcp_packet,
                                                  KmsRTCPPSFBAFBREMBPacket * remb_packet, guint32 sender_ssrc)
{
    guint8 *fci_data;
    guint16 len;
    guint32 mantissa = 0;
    guint8 exp = 0;
    int i;

    if (!compute_mantissa_and_6_bit_base_2_expoonent (remb_packet->bitrate, 18,
                                                     &mantissa, &exp)) {
        GST_ERROR ("Cannot calculate mantissa and exp)");
        return FALSE;
    }

    gst_rtcp_packet_fb_set_type (rtcp_packet, GST_RTCP_PSFB_TYPE_AFB);
    gst_rtcp_packet_fb_set_sender_ssrc (rtcp_packet, sender_ssrc);
    gst_rtcp_packet_fb_set_media_ssrc (rtcp_packet, 0);

    len = gst_rtcp_packet_fb_get_fci_length (rtcp_packet);
    len += 2 + remb_packet->n_ssrcs;
    if (!gst_rtcp_packet_fb_set_fci_length (rtcp_packet, len)) {
        GST_ERROR ("Cannot increase FCI length (%d)", len);
        return FALSE;
    }

    fci_data = gst_rtcp_packet_fb_get_fci (rtcp_packet);
    memmove (fci_data, "REMB", 4);
    fci_data[4] = remb_packet->n_ssrcs;

    fci_data[5] = (exp << 2) + ((mantissa >> 16) & 0x03);
    fci_data[6] = mantissa >> 8;
    fci_data[7] = mantissa;
    fci_data += 8;

    for (i = 0; i < remb_packet->n_ssrcs; i++) {
        *(guint32 *) fci_data = g_htonl (remb_packet->ssrcs[i]);
        fci_data += 4;
    }
    return TRUE;
}
