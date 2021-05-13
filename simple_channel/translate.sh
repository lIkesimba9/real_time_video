#!/bin/bash
com="ip netns exec fServer bash"
# ffmpeg -sdp_file sdp.sdp  -f x11grab -i :0.0+100,200  -c:v libx264 -crf:v 20 -f rtp rtp://10.0.0.1:1234

#ffmpeg -sdp_file sdp.sdp -f v4l2 -input_format mjpeg -framerate 30 -video_size 960x720 -i /dev/video0 -c:v libx264 -f rtp rtp://10.0.0.1:1234

$com
