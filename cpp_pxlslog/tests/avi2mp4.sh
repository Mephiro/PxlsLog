#!/bin/bash

ffmpeg -i timelapse.avi -vf "pad=ceil(iw/2)*2:ceil(ih/2)*2" -c:v libx264 -crf 19 -preset slower timelapse.mp4 -y
