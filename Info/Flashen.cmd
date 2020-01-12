esptool.exe -cp COM3 -cd nodemcu -ce 
esptool.exe -cp COM3 -cd nodemcu -ca 0x000000 -cf Spundomat.ino.bin -ca 0x200000 -cf Spundomat.spiffs.bin
