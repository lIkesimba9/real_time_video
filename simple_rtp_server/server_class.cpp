#include "server_class.h"
#include "../common/json.h"
#include <exception>
#include <fstream>
using namespace std;



gboolean RTPServer::BusCallback (GstBus     *bus,
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
    default:
        break;
    }

    return TRUE;
}

gboolean RTPServer::ProcessRtcpPacket(GstRTCPPacket *packet){
    guint32 ssrc;
    guint count, i;

    count = gst_rtcp_packet_get_rb_count(packet);
    cerr << "    count " << count;
    for (i=0; i<count; i++) {
        guint32 exthighestseq, jitter, lsr, dlsr;
        guint8 fractionlost;
        gint32 packetslost;

        gst_rtcp_packet_get_rb(packet, i, &ssrc, &fractionlost,
                               &packetslost, &exthighestseq, &jitter, &lsr, &dlsr);

        cerr << "    block " << i;
        cerr << "    ssrc " << ssrc;
        cerr << "    highest seq " << exthighestseq;
        cerr << "    jitter " << jitter;
        cerr << "    fraction lost " << fractionlost;
        cerr << "    packet lost " << packetslost;
        cerr << "    lsr " << lsr;
        cerr << "    dlsr " << dlsr;

        //        rtcp_pkt->fractionlost = fractionlost;
        //        rtcp_pkt->jitter = jitter;
        //        rtcp_pkt->packetslost = packetslost;
    }

    //cerr << "Received rtcp packet");

    return TRUE;
}
gboolean RTPServer::CallBackOnReceiveRtcp(GstElement *rtpsession, GstBuffer *buf, gpointer data){

    GstRTCPBuffer rtcpBuffer = GST_RTCP_BUFFER_INIT;
    //    GstRTCPBuffer *rtcpBuffer = (GstRTCPBuffer*)malloc(sizeof(GstRTCPBuffer));
    //    rtcpBuffer->buffer = nullptr;
    GstRTCPPacket *rtcpPacket = (GstRTCPPacket*)malloc(sizeof(GstRTCPPacket));


    if (!gst_rtcp_buffer_validate(buf))
    {
        cerr << "Received invalid RTCP packet" << endl;
    }

    cerr << "Received rtcp packet" << "\n";


    gst_rtcp_buffer_map (buf,(GstMapFlags)(GST_MAP_READ),&rtcpBuffer);
    gboolean more = gst_rtcp_buffer_get_first_packet(&rtcpBuffer,rtcpPacket);
    while (more) {
        GstRTCPType type;

        type = gst_rtcp_packet_get_type(rtcpPacket);
        switch (type) {
        case GST_RTCP_TYPE_RR:
            ProcessRtcpPacket(rtcpPacket);
            //   gst_rtcp_buffer_unmap (&rtcpBuffer);
            //g_debug("RR");
            //send_event_to_encoder(venc, &rtcp_pkt);
            break;
        default:
            cerr << "Other types" << endl;
            break;
        }
        more = gst_rtcp_packet_move_to_next(rtcpPacket);
    }

    free(rtcpPacket);
    return TRUE;
}
GstPadProbeReturn RTPServer::CallBackAddTimeToRtpPacket(GstPad *pad, GstPadProbeInfo *info, gpointer *data) {

    GstBuffer *buffer = GST_PAD_PROBE_INFO_BUFFER(info);
    GstRTPBuffer rtp_buffer;
    memset(&rtp_buffer, 0, sizeof(GstRTPBuffer));

    if (buffer != NULL) {
        // Замеряю время. И отправляю его.
        cerr << "time " << (int64_t)(*data) << '\n';
        int64_t time = GET_SYS_MS() ;//-  (int64_t)(*data);
        gconstpointer pointData = &time;
        if (gst_rtp_buffer_map(buffer, (GstMapFlags)GST_MAP_READWRITE, &rtp_buffer)) {
            gst_rtp_buffer_add_extension_onebyte_header(&rtp_buffer, 1, pointData, sizeof(time));
            //cerr << "milisec: " << millis << '\n';
        }
        else {
            cerr << "RTP buffer not mapped\n";
        }
    }
    else {
        cerr << "Gst buffer is NULL\n";
    }
    gst_rtp_buffer_unmap(&rtp_buffer);
}
map<string,RTPServer::ConfigData *> RTPServer::LoadFromJson(istream& input) {
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
    {
        auto node = root.AsMap().at("codec");
        CodecParams* params = new CodecParams;
        params->bitrate = node.AsMap().at("bitrate").AsInt();
        params->tune = node.AsMap().at("tune").AsInt();
        params->thread_count = node.AsMap().at("thread_count").AsInt();
        res["codec"] = params;
    }

    {
        auto node = root.AsMap().at("source");
        SourceParams* params = new SourceParams;
        params->dev = node.AsMap().at("dev").AsString();
        res["source"] = params;
    }

    return res;
}
void RTPServer::LoadParamFromConfigFile(std::string path){

    ifstream in(path);
    if (in.is_open())
    {
        params_ = LoadFromJson(in);
        in.close();
    } else
    {
        params_["network"] = new NetworkParams();
        params_["codec"] = new CodecParams();
        params_["source"] = new SourceParams();
    }
}
RTPServer::RTPServer(std::string pathToParams)
{
    LoadParamFromConfigFile(pathToParams);
    pipeline_ = CreatePipeline();
    first_ts = GET_SYS_MS();
    if (pipeline_ == NULL)
        throw invalid_argument("Error create pipeline");

    bus_ = gst_pipeline_get_bus (GST_PIPELINE (pipeline_));
    watch_id_ = gst_bus_add_watch (bus_, BusCallback, loop_);
    gst_object_unref (bus_);



    GstStateChangeReturn ret;
    ret = gst_element_set_state(pipeline_, GST_STATE_PLAYING);

    g_main_loop_run (loop_);
}
GstElement* RTPServer::CreatePipeline() {
    GstElement *pipeline,*source,*videconverter,
        *x264encoder,*rtph264pay,
        *rtpbin,*udpSinkRtp;
    GstElement *udpSrcRtcp,*udpSinkRtcp;

    pipeline = gst_pipeline_new("rtpStreamer");

    // Создаю элемент захвата видео с камеры.
    source = gst_element_factory_make("v4l2src","source");
    // Создаю элелмент который преобразует данные с камеры, к данным которые поддерживает кодировщик.
    videconverter = gst_element_factory_make("videoconvert","converter");
    // Создаю кодек
    x264encoder = gst_element_factory_make("x264enc","encoder");
    // Создаю элемент который упакуют донные от кодера, в rtp пакеты
    rtph264pay = gst_element_factory_make("rtph264pay","rtppay");
    // Создаю элемент управющий rtp сесией
    rtpbin = gst_element_factory_make("rtpbin","rtpbin");
    // Создаю udp сток для отпраки rtp пакетов
    udpSinkRtp = gst_element_factory_make("udpsink","udpSinkRtp");
    // Создаю upd сток для отпраки rtcp пакетов
    udpSinkRtcp = gst_element_factory_make("udpsink","udpSinkRtcp");
    // Создаю udp источник для принятия rtcp пакетов.
    udpSrcRtcp = gst_element_factory_make("udpsrc","udpSrcRtcp");

    if (!pipeline || !source || !videconverter || !x264encoder || !rtph264pay || !rtpbin || !udpSinkRtp || !udpSinkRtcp || !udpSrcRtcp)
    {
        cerr << "Not all elements could be created.\n";
        return NULL;
    }
    // Задаю устройство с которог захватывать видео
    SourceParams *sorceParams = (SourceParams *)params_["source"];
    g_object_set(G_OBJECT(source),"device",sorceParams->dev.c_str(),NULL);

    // Устанавливаю параметры кодека..
    CodecParams *codecParams = (CodecParams *)params_["codec"];
    g_object_set(G_OBJECT(x264encoder),"tune",codecParams->tune,NULL);
    g_object_set(G_OBJECT(x264encoder),"bitrate",codecParams->bitrate,NULL);
    g_object_set(G_OBJECT(x264encoder),"threads",codecParams->thread_count,NULL);

    // Устанавливаю параметры для upd сойденений.
    NetworkParams *networkParams = (NetworkParams *)params_["network"];
    g_object_set(G_OBJECT(udpSinkRtp),"host",networkParams->client_addr.c_str(),NULL);
    g_object_set(G_OBJECT(udpSinkRtp),"port",networkParams->host_rtp_port,NULL);
    g_object_set(G_OBJECT(udpSinkRtcp),"host",networkParams->client_addr.c_str(),NULL);
    g_object_set(G_OBJECT(udpSinkRtcp),"port",networkParams->host_rtcp_port,NULL);
    g_object_set(G_OBJECT(udpSrcRtcp),"address",networkParams->host_addr.c_str(),NULL);
    g_object_set(G_OBJECT(udpSrcRtcp),"port",networkParams->client_rtcp_port,NULL);
    g_object_set(G_OBJECT (udpSrcRtcp), "caps", gst_caps_from_string("application/x-rtcp"), NULL);


    // Добавляю элементы в контейнер
    gst_bin_add_many(GST_BIN(pipeline),source,videconverter,
                     x264encoder,rtph264pay,rtpbin,
                     udpSinkRtp,udpSinkRtcp,udpSrcRtcp,NULL);


    // Создаю caps для того, чтобы согласовать параметры захвата видео с камеры и кодировщика.
    GstCaps *capV4l2VideoConverter;
    capV4l2VideoConverter = gst_caps_new_simple ("video/x-raw",
                                                "format",G_TYPE_STRING,"YUY2",
                                                "framerate", GST_TYPE_FRACTION, 30, 1,
                                                NULL);
    if (!capV4l2VideoConverter){
        cerr << "Error create caps\n";
        return NULL;
    }
    GstCaps *capVideoConverterEncoder;
    capVideoConverterEncoder  = gst_caps_new_simple ("video/x-raw",
                                                   "format",G_TYPE_STRING,"I420",
                                                   "framerate", GST_TYPE_FRACTION, 30, 1,
                                                   NULL);
    if (!capVideoConverterEncoder){
        cerr << "Error create caps\n";
        return NULL;
    }


    // Связваю все элементы.
    if (!gst_element_link_filtered(source,videconverter,capV4l2VideoConverter))
    {
        cerr << "Elements could not be linked source and videoconv.\n";
        return NULL;
    }

    if (!gst_element_link_filtered(videconverter,x264encoder,capVideoConverterEncoder))
    {
        cerr << "Elements could not be linked videoconv and x264.\n";
        return NULL;
    }



    if (!gst_element_link_many(x264encoder,rtph264pay,NULL))
    {
        cerr << "Elements could not be linked other.\n";
        return NULL;

    }

    if (!LinkStaticAndRequestPads(rtph264pay,rtpbin,"src","send_rtp_sink_%u"))
    {
        cerr << "Error create link, beetwen rtph264pay and rtpbin\n";
        return NULL;
    }

    if (!gst_element_link(rtpbin,udpSinkRtp))
    {
        cerr << "Elements could not be linked rtpbin and udpSinkRtp.\n";
    }

    if (!LinkRequestAndStaticPads(rtpbin,udpSinkRtcp,"send_rtcp_src_%u","sink"))
    {
        cerr << "Error create link, beetwen rtpbin and udpSinkRtcp\n";
        return NULL;
    }


    if (!LinkStaticAndRequestPads(udpSrcRtcp,rtpbin,"src","recv_rtcp_sink_%u"))
    {
        cerr << "Error create link, beetwen udpSrcRtcp and rtpbin\n";
        return NULL;
    }
    //Пад для расширения заголовка RTP.
    GstPad *rtpPaySrcPad = gst_element_get_static_pad(rtph264pay, "src");
    gst_pad_add_probe(rtpPaySrcPad, GST_PAD_PROBE_TYPE_BUFFER, (GstPadProbeCallback)CallBackAddTimeToRtpPacket, (gpointer)&first_ts, NULL);
    gst_object_unref(GST_OBJECT(rtpPaySrcPad));

    // Подключаю сигнал по обработке принятых rtcp пакетов.
    GObject *session;
    g_signal_emit_by_name (rtpbin, "get-internal-session", 0, &session);
    g_signal_connect_after (session, "on-receiving-rtcp",
                           G_CALLBACK (CallBackOnReceiveRtcp), NULL);
    g_object_unref(session);




    return pipeline;
}
