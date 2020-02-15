## !/bin/bash
# mvh 20170301 Web-based installer for 1.4.19 on linux
#              Needs: install/service.lua
#                     install/dicom.ini
# mvh 20170501 For 1.4.19a; dgate/dgatesmall mixup set rights to web folders
# mvh 20181117 For 1.4.19c; add luasocket; copy of anonymize_script
# mvh 20181115 For 1.4.19c; copy dgate.dic to install, use define for CGI
# mvh 20181223 For 1.4.19d; Added luasocket; use sensible-browser
# mvh 20181230 For 1.4.19d; Added setenforce line for Fedora
# mvh 20200110 For 1.5.0beta; Fix pushd use
# mvh 20200215 For 1.5.0beta2; Added Wno-format-overflow

SRC=../src/dgate;
CGI=/usr/lib/cgi-bin; # /var/www/cgi-bin for Fedora

sudo fuser -k 5912/tcp;

#sudo setenforce Permissive; # unblock for Fedora

cp ../dgate.dic .
mkdir lua
cp ../lua/anonymize_script.lua lua

chmod 777 $SRC/luasocket/amake.sh;
pushd $SRC/luasocket;
./amake.sh;
popd;

gcc -o $SRC/lua.o -c $SRC/lua_5.1.5/all.c -I$SRC/lua_5.1.5 -DLUA_USE_DLOPEN -DLUA_USE_POSIX;
g++ -std=c++11 -DUNIX -DNATIVE_ENDIAN=1 -DNOINTJPEG -Wno-write-strings $SRC/lua.o $SRC/luasocket/luasocket.a -o dgatesmall -lpthread -ldl -I$SRC/src $SRC/src/total.cpp -I$SRC/dicomlib -I$SRC/lua_5.1.5 -Wno-multichar -Wno-format-overflow;

chmod 777 $SRC/jpeg-6c/configure;
chmod 777 dgatesmall;
rm -f nohup.out;
nohup ./dgatesmall -v &

sudo mkdir -p $CGI/service;
sudo chmod 777 $CGI/service;
sudo cp dgatesmall $CGI/service/dgate;
sudo cp service.lua $CGI/service;
sudo cp ../dgate.dic $CGI/service;
sudo cp dicom.ini $CGI/service;

sudo mkdir -p $CGI/newweb;
sudo chmod 777 $CGI/newweb;

sensible-browser http://127.0.0.1/cgi-bin/service/dgate;
