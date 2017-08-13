#!/bin/sh

cd /data/images

while [ 1 ]
do 
	#gphoto2 --port usb: --capture-tethered --filename TET_%y_%m_%d-%H_%M_%S-%n.%C 
	#gphoto2 --port usb: --capture-tethered --keep --set-config capturetarget=card --filename TET_%y_%m_%d-%H_%M_%S-%n.%C 
	gphoto2 --port usb: --capture-tethered --keep --set-config capturetarget=card 
	sleep 1
done

# enable script execution from the desktor/filemanager: 
# gsettings set org.gnome.nautilus.preferences executable-text-activation ask

# disable automatic mount:
# gsettings set org.gnome.desktop.media-handling automount false
