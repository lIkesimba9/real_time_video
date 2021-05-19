#ifndef RTPCLIENT_H
#define RTPCLIENT_H
#pragma once
#include "../common/abstractgstapp.h"
#include <map>
#define KMS_RTCP_PSFB_AFB_REMB_MAX_SSRCS_COUNT   255
class RTPClient : public AbstractGstApp
{

    struct NetworkParams : public AbstractGstApp::ConfigData{
        std::string host_addr = "127.0.0.1";
        std::string client_addr = "127.0.0.1";
        int host_rtp_port = 5000;
        int host_rtcp_port = 5001;
        int client_rtcp_port = 5005;
    };



    struct KmsRTCPPSFBAFBREMBPacket
    {
        guint32 bitrate;
        guint8 n_ssrcs;
        guint32 ssrcs[KMS_RTCP_PSFB_AFB_REMB_MAX_SSRCS_COUNT];
    };

    static gboolean BusCallback (GstBus     *bus,GstMessage *msg, gpointer    data);
    void LoadParamFromConfigFile(std::string path);
    std::map<std::string,AbstractGstApp::ConfigData *> params_;
    GstElement* CreatePipeline() override;
    std::map<std::string,AbstractGstApp::ConfigData *> LoadFromJson(std::istream& input);
    GstRegistry *registry_;
    static gboolean CallBackOnSendRtcp(GstElement *rtpsession, GstBuffer *buf,   gboolean early, gpointer data);
    static gboolean RtcpPsfbAfbRembMarshallPacket (GstRTCPPacket * rtcp_packet,
                                           KmsRTCPPSFBAFBREMBPacket * remb_packet, guint32 sender_ssrc);


public:
    explicit RTPClient(std::string pathToParams = "");
};

#endif // RTPCLIENT_H
