/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2021 simba9 <<user@hostname.org>>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-gccanalysis
 *
 * FIXME:Describe gccanalysis here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! gccanalysis ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#include "gstgccanalysis.h"

#include <gst/gst.h>
#include <gst/rtp/rtp.h>

#include <sys/time.h>
#include <time.h>






GST_DEBUG_CATEGORY_STATIC(gst_gcc_analysis_debug);
#define GST_CAT_DEFAULT gst_gcc_analysis_debug

/* Filter signals and args */
enum {
    /* FILL ME */
    LAST_SIGNAL
};

enum { PROP_0, PROP_SILENT };

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE(
    "sink", GST_PAD_SINK, GST_PAD_ALWAYS, GST_STATIC_CAPS("ANY"));

#define gst_gcc_analysis_parent_class parent_class
G_DEFINE_TYPE(GstGccAnalysis, gst_gcc_analysis, GST_TYPE_ELEMENT);

static void gst_gcc_analysis_set_property(GObject *object, guint prop_id,
                                          const GValue *value,
                                          GParamSpec *pspec);
static void gst_gcc_analysis_get_property(GObject *object, guint prop_id,
                                          GValue *value, GParamSpec *pspec);

static gboolean gst_gcc_analysis_sink_event(GstPad *pad, GstObject *parent,
                                            GstEvent *event);
static GstFlowReturn gst_gcc_analysis_chain(GstPad *pad, GstObject *parent,
                                            GstBuffer *buf);



/* GObject vmethod implementations */

/* initialize the gccanalysis's class */
static void gst_gcc_analysis_class_init(GstGccAnalysisClass *klass) {
    GObjectClass *gobject_class;
    GstElementClass *gstelement_class;

    gobject_class = (GObjectClass *)klass;
    gstelement_class = (GstElementClass *)klass;

    gobject_class->set_property = gst_gcc_analysis_set_property;
    gobject_class->get_property = gst_gcc_analysis_get_property;

    g_object_class_install_property(
        gobject_class, PROP_SILENT,
        g_param_spec_boolean("silent", "Silent", "Produce verbose output ?",
                             FALSE, G_PARAM_READWRITE));

    gst_element_class_set_details_simple(gstelement_class, "GccAnalysis",
                                         "For analysis time delay.", "PGSD",
                                         "simba9 ostilia@mail.ru");

    gst_element_class_add_pad_template(
        gstelement_class, gst_static_pad_template_get(&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void gst_gcc_analysis_init(GstGccAnalysis *gcc) {
    gcc->sinkpad = gst_pad_new_from_static_template(&sink_factory, "sink");
    gst_pad_set_event_function(gcc->sinkpad,
                               GST_DEBUG_FUNCPTR(gst_gcc_analysis_sink_event));
    gst_pad_set_chain_function(gcc->sinkpad,
                               GST_DEBUG_FUNCPTR(gst_gcc_analysis_chain));
    GST_PAD_SET_PROXY_CAPS(gcc->sinkpad);
    gst_element_add_pad(GST_ELEMENT(gcc), gcc->sinkpad);
    // gcc->a.resize(4);
    gcc->rbe = rbe_create();

    //  gcc->proxy = estimator_proxy_create(SIM_SEGMENT_HEADER_SIZE,0);

    rbe_set_min_bitrate(gcc->rbe, MIN_BITRATE);
    rbe_set_max_bitrate(gcc->rbe, MAX_BITRATE);
    gcc->last_update_time = GET_SYS_MS();


    //  gcc->out = fopen("../samples/datanew.csv","wt");
    /*  fprintf(gcc->out,"RecvTime,RecvTimeHightBits,RecvTimeLowBits,"
                      "oldRecvTime,oldRecvTimeHightBits,oldRecvTimeLowBits,"
                      "SendTime,SendTimeHightBits,SendTimeLowBits,"
                      "oldSendTime,oldSendTimeHightBits,oldSendTimeLowBits\n");

    gcc->silent = FALSE;
    gcc->fFirstPacketGroup = true;
    gcc->fForMeasTime = true;
    */
}

static void gst_gcc_analysis_set_property(GObject *object, guint prop_id,
                                          const GValue *value,
                                          GParamSpec *pspec) {
    GstGccAnalysis *gcc = GST_GCCANALYSIS(object);

    switch (prop_id) {
    case PROP_SILENT:
        gcc->silent = g_value_get_boolean(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void gst_gcc_analysis_get_property(GObject *object, guint prop_id,
                                          GValue *value, GParamSpec *pspec) {
    GstGccAnalysis *gcc = GST_GCCANALYSIS(object);

    switch (prop_id) {
    case PROP_SILENT:
        g_value_set_boolean(value, gcc->silent);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean gst_gcc_analysis_sink_event(GstPad *pad, GstObject *parent,
                                            GstEvent *event) {
    GstGccAnalysis *gcc;
    gboolean ret;

    gcc = GST_GCCANALYSIS(parent);

    GST_LOG_OBJECT(gcc, "!!!!!!!!!!!!Received %s event: %" GST_PTR_FORMAT,
                   GST_EVENT_TYPE_NAME(event), event);

    switch (GST_EVENT_TYPE(event)) {
    case GST_EVENT_CAPS: {
        GstCaps *caps;

        gst_event_parse_caps(event, &caps);
        /* do something with the caps */

        /* and forward */
        ret = gst_pad_event_default(pad, parent, event);
        break;
    }
    default:
        ret = gst_pad_event_default(pad, parent, event);
        break;
    }
    return ret;
}

/* chain function
 * this function does the actual processing
 */

static GstFlowReturn gst_gcc_analysis_chain(GstPad *pad, GstObject *parent,
                                            GstBuffer *buf) {
    GstGccAnalysis *gcc;
    gcc = GST_GCCANALYSIS(parent);

    GstRTPBuffer rtpBufer = GST_RTP_BUFFER_INIT;
    gst_rtp_buffer_map(buf, GST_MAP_READ, &rtpBufer);
    // ?????????????? ??????????, ?????????????? ?????????????? ???? ??????????????.
    gpointer miliSec;
    int64_t timestamp;
    guint size_timestamp = sizeof(timestamp);
  gst_rtp_buffer_get_extension_onebyte_header(&rtpBufer, 1, 0, &miliSec,
                                                &size_timestamp);
    timestamp = *((long long *)miliSec);
    int64_t now_ts = GET_SYS_MS();
    int64_t size_data = gst_buffer_get_size(buf);
    //printf("timestamp is %lld *** now_ts is %lld *** size_data is %lld\n",timestamp,now_ts,size_data);
    rbe_incoming_packet(gcc->rbe, timestamp, now_ts, size_data, now_ts);
    uint32_t remb = 0;
    if(now_ts - gcc->last_update_time > 5){
    if (rbe_heartbeat(gcc->rbe, now_ts, &remb) == 0)
        printf("bitrate %lld\n",remb);
    gcc->last_update_time = now_ts;
    }
    return GST_FLOW_OK;
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean gccanalysis_init(GstPlugin *gccanalysis) {
    /* debug category for fltering log messages
   *
   * exchange the string 'Template gccanalysis' with your description
   */
    GST_DEBUG_CATEGORY_INIT(gst_gcc_analysis_debug, "gccanalysis", 0,
                            "Template gccanalysis");

    return gst_element_register(gccanalysis, "gccanalysis", GST_RANK_NONE,
                                GST_TYPE_GCCANALYSIS);
}

/* PACKAGE: this is usually set by meson depending on some _INIT macro
 * in meson.build and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use meson to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "gccanalysis"
#endif

/* gstreamer looks for this structure to register gccanalysiss
 *
 * exchange the string 'Template gccanalysis' with your gccanalysis description
 */
GST_PLUGIN_DEFINE(GST_VERSION_MAJOR, GST_VERSION_MINOR, gccanalysis,
                  "Template gccanalysis", gccanalysis_init, "0.1", "LGPL",
                  "GStreamer GCC Plugin", "ostilia@mail.ru")

