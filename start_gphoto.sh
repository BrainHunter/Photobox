#!/bin/sh

cd /data/images

#gphoto2 --port usb: --capture-tethered --filename TET_%y_%m_%d-%H_%M_%S-%n.%C 
gphoto2 --port usb: --capture-tethered --keep --filename TET_%y_%m_%d-%H_%M_%S-%n.%C 

