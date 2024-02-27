@ECHO OFF
CLS
SET SCRIPT_LOCATION=%~dp0
cd %SCRIPT_LOCATION%
echo Flashe Firmware und LITTLEFS
esptool.exe --chip esp8266 erase_flash
esptool.exe --chip esp8266 --baud 921600 write_flash 0x000000 Spundomat.ino.bin 0x200000 Spundomat.mklittlefs.bin
echo Enter zum Beenden
pause
exit
