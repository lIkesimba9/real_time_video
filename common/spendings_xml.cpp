#include <algorithm>
#include <iostream>
#include <sstream>
#include <map>
#include "json.h"
#include <fstream>
using namespace std;

struct ConfigParam {

};
struct NetworkParams : public ConfigParam
{
    string host_addr;
    string client_addr;
    int host_rtp_port;
    int host_rtcp_port;
    int client_rtcp_port;
};
struct CodecParams : public ConfigParam {
    int bitrate;
    int tune;
    int count_threads;
};
struct SourceParams : public ConfigParam {
    string dev;
};

map<string,ConfigParam *> LoadFromJson(istream& input) {
    using namespace Json;
    map<string, ConfigParam*> res;
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
            params->count_threads = node.AsMap().at("count_threads").AsInt();
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


int main() {
    ifstream in("config.json");
    if (in.is_open()) {
        LoadFromJson(in);
    }
    else {
        cout << "not open file!";
    }
    
 
}
