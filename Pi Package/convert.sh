#!/bin/bash

for vid in ~/*.h264;
do
	MP4Box -fps 20 -add $vid $vid'.mp4';
done
