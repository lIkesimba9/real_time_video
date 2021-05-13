#ifndef RTPCLIENT_H
#define RTPCLIENT_H
#pragma once
#include "../common/abstractgstapp.h"
#include <map>

class RTPClient : public AbstractGstApp
{

    struct NetworkParams : public AbstractGstApp::ConfigData{
        std::string host_addr = "127.0.0.1";
        std::string client_addr = "127.0.0.1";
        int host_rtp_port = 5000;
        int host_rtcp_port = 5001;
        int client_rtcp_port = 5005;
    };



    static gboolean BusCallback (GstBus     *bus,GstMessage *msg, gpointer    data);
    void LoadParamFromConfigFile(std::string path);
    std::map<std::string,AbstractGstApp::ConfigData *> params_;
    GstElement* CreatePipeline() override;
    std::map<std::string,AbstractGstApp::ConfigData *> LoadFromJson(std::istream& input);
    GstRegistry *registry_;



public:
    explicit RTPClient(std::string pathToParams = "");
};

#endif // RTPCLIENT_H
