#include <iostream>
#include <gst/gst.h>

#include <gst/rtp/rtp.h>
//#include <time.h>

#include <map>
#include <fstream>
#include "server_class.h"
using namespace std;

/*
 * Sender example setup
 * sends the output of v4l2src as h264 encoded RTP on port 5000, RTCP is sent on
 * port 5001. The destination is 127.0.0.1.
 * the video receiver RTCP reports are received on port 5005
 *
 * .-------.    .-------.    .-------.      .----------.     .-------.
 * |v4l2   |    |x264enc|    |h264pay|      | rtpbin   |     |udpsink|  RTP
 * |      src->sink    src->sink    src->send_rtp send_rtp->sink     | port=5000
 * '-------'    '-------'    '-------'      |          |     '-------'
 *                                          |          |
 *                                          |          |     .-------.
 *                                          |          |     |udpsink|  RTCP
 *                                          |    send_rtcp->sink     | port=5001
 *                           .--------.     |          |     '-------' sync=false
 *                RTCP       |udpsrc  |     |          |               async=false
 *                port=5005  |       src->recv_rtcp    |
 *                           '--------'     '----------'
 */




int main(int argc, char *argv[])
{
    RTPServer srv("config.json");
    return 0;
}
