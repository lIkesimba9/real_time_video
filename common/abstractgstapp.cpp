#include "abstractgstapp.h"

using namespace std;


bool AbstractGstApp::LinkStaticAndRequestPads(GstElement* sourse, GstElement* sink, gchar* nameSrcPad, gchar* nameSinkPad)
{

    GstPad* srcPad = gst_element_get_static_pad(sourse, nameSrcPad);
    GstPad* sinkPad = gst_element_get_request_pad(sink, nameSinkPad);
    GstPadLinkReturn ret_link = gst_pad_link(srcPad, sinkPad);
    if (ret_link != GST_PAD_LINK_OK)
    {
        cerr << "Error create link, static and request pad\n";
        return false;
    }
    gst_object_unref(GST_OBJECT(srcPad));
    gst_object_unref(GST_OBJECT(sinkPad));
    return true;
}
bool AbstractGstApp::LinkRequestAndStaticPads(GstElement* sourse, GstElement* sink, gchar* nameSrcPad, gchar* nameSinkPad)
{


    GstPad* srcPad = gst_element_get_request_pad(sourse, nameSrcPad);
    GstPad* sinkPad = gst_element_get_static_pad(sink, nameSinkPad);
    GstPadLinkReturn ret_link = gst_pad_link(srcPad, sinkPad);
    if (ret_link != GST_PAD_LINK_OK)
    {
        cerr << "Error create link, request and statitc pad\n";
        return false;
    }
    gst_object_unref(GST_OBJECT(srcPad));
    gst_object_unref(GST_OBJECT(sinkPad));
    return true;
}
void AbstractGstApp::LinkDynamicAndOtherPads(GstElement* elementWithDynamicPad, GstPad* dynamicPads, gpointer data)
{

    GstElement* secondElement = (GstElement*)data;
    GstPad* sinkPad;
    sinkPad = gst_element_get_static_pad(secondElement, "sink");
    gst_pad_link(dynamicPads, sinkPad);
    gst_object_unref(sinkPad);

}
AbstractGstApp::~AbstractGstApp()
{
    gst_element_set_state(pipeline_, GST_STATE_NULL);
    gst_object_unref(pipeline_);
    g_source_remove(watch_id_);
    g_main_loop_unref(loop_);

}
AbstractGstApp::AbstractGstApp()
{
    gst_init(0, 0);
    loop_ = g_main_loop_new(NULL, FALSE);
}
