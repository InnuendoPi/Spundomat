@ECHO OFF
CLS
SET SCRIPT_LOCATION=%~dp0
cd %SCRIPT_LOCATION%
echo Flashe Firmware und LITTLEFS
esptool.exe -cp COM3 -cd nodemcu -ce 
esptool.exe -cp COM3 -cd nodemcu -ca 0x000000 -cf Spundomat.ino.bin -ca 0x200000 -cf Spundomat.mklittlefs.bin
echo Enter zum Beenden
pause
exit
