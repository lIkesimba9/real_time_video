#ifndef RTPSERVER_H
#define RTPSERVER_H
#pragma once
#include "../common/abstractgstapp.h"
#include <string>
#include <map>
class RTPServer : public AbstractGstApp
{

    struct NetworkParams : public AbstractGstApp::ConfigData{
        std::string host_addr = "127.0.0.1";
        std::string client_addr = "127.0.0.1";
        int host_rtp_port = 5000;
        int host_rtcp_port = 5001;
        int client_rtcp_port = 5005;
    };
    struct CodecParams : public AbstractGstApp::ConfigData{
        int tune = 4;
        int bitrate = 500;
        int thread_count = 2;

    };
    struct SourceParams: public AbstractGstApp::ConfigData{
        std::string dev = "/dev/video0";
    };

    static gboolean BusCallback (GstBus     *bus,GstMessage *msg, gpointer    data);
    static gboolean ProcessRtcpPacket(GstRTCPPacket *packet);
    static gboolean CallBackOnReceiveRtcp(GstElement *rtpsession, GstBuffer *buf, gpointer data);
    static GstPadProbeReturn CallBackAddTimeToRtpPacket(GstPad *pad, GstPadProbeInfo *info, gpointer *data);
    void LoadParamFromConfigFile(std::string path);
    std::map<std::string,AbstractGstApp::ConfigData *> params_;
    GstElement* CreatePipeline() override;
    std::map<std::string,AbstractGstApp::ConfigData *> LoadFromJson(std::istream& input);
    int64_t first_ts;
public:


    explicit RTPServer(std::string pathToParams = "");


};

#endif // RTPSERVER_H
