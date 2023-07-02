#########
#
# Create a running conquest 1.5.0d with web access and bash in Docker. 
# Note that this container DOES NOT RETAIN DATA after restart, 
# needs to be configured differently to achieve this.
#
# to build:
#    docker build -t itest .
#
# to run the container interactively with bash:
#    docker run -it -p 8081:80 itest bash
#    # systemctl restart apache2
#    # cd Conquest-DICOM-Server/install
#    # ./linux.sh
#
# The container then offers this webpage:
# http://localhost:8081/cgi-bin/service/dgate
#
##########


FROM debian

#get tools and libraries needed
RUN apt update
RUN apt -y install make sudo nano systemctl gettext-base git apache2 php libapache2-mod-php git g++ lua5.1 liblua5.1-0 lua-socket p7zip-full
RUN ln -s /usr/lib/x86_64-linux-gnu/liblua5.1.so.0 /usr/lib/x86_64-linux-gnu/liblua5.1.so
#RUN ln -s /usr/lib/aarch64-linux-gnu/liblua5.1.so.0 /usr/lib/aarch64-linux-gnu/liblua5.1.so

#configure apache and restart it
RUN a2enmod rewrite
RUN a2enmod cgi
RUN sed -i 's/AllowOverride None/AllowOverride All/g' /etc/apache2/apache2.conf
RUN sed -i 's/memory_limit = 128M/memory_limit = 512M/g' /etc/php/7.4/apache2/php.ini
RUN sed -i 's/upload_max_filesize = 2M/upload_max_filesize = 250M/g' /etc/php/7.4/apache2/php.ini
RUN sed -i 's/post_max_size = 8M/post_max_size = 250M/g' /etc/php/7.4/apache2/php.ini
RUN systemctl restart apache2

#get conquest source code
COPY Dockerfile .
RUN git clone https://github.com/marcelvanherk/Conquest-DICOM-Server
#COPY Conquest-DICOM-Server Conquest-DICOM-Server

#mark scripts as executable
RUN cd Conquest-DICOM-Server && chmod 777 maklinux
RUN cd Conquest-DICOM-Server && chmod 777 linux/regen
RUN cd Conquest-DICOM-Server && chmod 777 linux/updatelinux
RUN cd Conquest-DICOM-Server && chmod 777 linux/restart
RUN cd Conquest-DICOM-Server && chmod 777 install/linux.sh

#CMD ["/Conquest-DICOM-Server/install/linux.sh"]

###### additional docker commands:
#
#  to cleanup docker:
#  docker system prune -a
#
######