#ifndef ABSTRACTGSTAPP_H
#define ABSTRACTGSTAPP_H

#include <iostream>
#include <gst/gst.h>
#include <gst/rtp/rtp.h>
#include "utils.h"
class AbstractGstApp
{
public:
	AbstractGstApp();
    ~AbstractGstApp();

protected:

    struct ConfigData {

    };
    bool LinkStaticAndRequestPads(GstElement* sourse, GstElement* sink, gchar* nameSrcPad, gchar* nameSinkPad) ;
    bool LinkRequestAndStaticPads(GstElement* sourse, GstElement* sink, gchar* nameSrcPad, gchar* nameSinkPad) ;
    static void LinkDynamicAndOtherPads(GstElement* element, GstPad* pad, gpointer data) ;
GstElement* pipeline_;
virtual GstElement* CreatePipeline() = 0;

GMainLoop* loop_;
GstBus* bus_;
guint watch_id_;
GstStateChangeReturn ret_;

private:


};

#endif // ABSTRACTGSTAPP_H
