## !/bin/bash
# mvh 20170301 Web-based installer for 1.4.19 on linux
#              Needs: install/service.lua
#                     install/dicom.ini
# mvh 20170501 For 1.4.19a; dgate/dgatesmall mixup set rights to web folders
# mvh 20181117 For 1.4.19c; add luasocket; copy of anonymize_script

SRC=../src/dgate;
sudo fuser -k 5912/tcp

mkdir lua
cp ../lua/anonymize_script.lua lua

chmod 777 ../src/dgate/luasocket/amake.sh;
cd ../src/dgate/luasocket;
./amake.sh;
cd ../../../install

gcc -o $SRC/lua.o -c $SRC/lua_5.1.5/all.c -I$SRC/lua_5.1.5 -DLUA_USE_DLOPEN -DLUA_USE_POSIX;
g++ -std=c++11 -DUNIX -DNATIVE_ENDIAN=1 -DNOINTJPEG -Wno-write-strings $SRC/lua.o $SRC/luasocket/luasocket.a -o dgatesmall -lpthread -ldl -I$SRC/src $SRC/src/total.cpp -I$SRC/dicomlib -I$SRC/lua_5.1.5 -Wno-multichar;

chmod 777 ../src/dgate/jpeg-6c/configure;
chmod 777 dgatesmall;
rm -f nohup.out;
nohup ./dgatesmall -v &

sudo mkdir -p /usr/lib/cgi-bin/service;
sudo chmod 777 /usr/lib/cgi-bin/service;
sudo cp dgatesmall /usr/lib/cgi-bin/service/dgate;
sudo cp service.lua /usr/lib/cgi-bin/service;
sudo cp ../dgate.dic /usr/lib/cgi-bin/service;
sudo cp dicom.ini /usr/lib/cgi-bin/service;

sudo mkdir -p /usr/lib/cgi-bin/newweb;
sudo chmod 777 /usr/lib/cgi-bin/newweb;

firefox http://127.0.0.1/cgi-bin/service/dgate;

