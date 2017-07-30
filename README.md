# Photobox
A little software for a Photobox Slideshow.

Photobox is monitoring a folder for new images. If a new image is detected it will be displayed 
imediately. After a specified time the Slideshow will continue displaying all the images in the
monitored folder. 


To automatically copy photos from a camera into a folder a tethered shooting software can be used:
*Under Windows EOS Uitility can be used for example.
*In Linix gphoto2 turned out a good choice. It can be started like this:
 gphoto2 --port usb: --capture-tethered --filename "%y_%m_%d-%H_%M_%S-%n.%C

 
#Build
 qmake Photobox_Widget.pro
 make
 ./Photobox_Widget
 
