#########
#
# Create a running conquest 1.5.0d with web access and bash in Docker. 
# Note that this container DOES NOT RETAIN DATA after restart, 
# needs to be configured differently to achieve this.
#
# to build:
#    docker build -t ctest .
#
# to run the container interactively with bash:
#    docker run -it -p 8082:80 ctest conquest
#
# The container then offers these webpages:
# http://localhost:8082/app/ohif
# http://localhost:8082/app/luiz
# http://localhost:8082/app/newweb
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
RUN sed -i 's/AllowOverride None/AllowOverride All/g' /etc/apache2/apache2.conf
RUN sed -i 's/memory_limit = 128M/memory_limit = 512M/g' /etc/php/7.4/apache2/php.ini
RUN sed -i 's/upload_max_filesize = 2M/upload_max_filesize = 250M/g' /etc/php/7.4/apache2/php.ini
RUN sed -i 's/post_max_size = 8M/post_max_size = 250M/g' /etc/php/7.4/apache2/php.ini
RUN systemctl restart apache2

#get conquest source code
RUN git clone https://github.com/marcelvanherk/Conquest-DICOM-Server
#COPY Conquest-DICOM-Server Conquest-DICOM-Server

#mark scripts as executable
RUN cd Conquest-DICOM-Server && chmod 777 maklinux
RUN cd Conquest-DICOM-Server && chmod 777 linux/regen
RUN cd Conquest-DICOM-Server && chmod 777 linux/updatelinux
RUN cd Conquest-DICOM-Server && chmod 777 linux/restart

#compile, update dicom.ini if next to Dockerfile, and regenerate the SQLite database
RUN cd Conquest-DICOM-Server && linux/updatelinux
#COPY *dicom.ini Conquest-DICOM-Server
RUN cd Conquest-DICOM-Server && linux/regen

#finalise the web server
RUN chown -R www-data /var/www/html
RUN chmod 777 /var/www/html/api/dicom/servertask

#make the startup script executable
RUN cp Conquest-DICOM-Server/linux/restart /usr/bin/conquest

###### additional docker commands:
#
#  to cleanup docker:
#  docker system prune -a
#
######