#########
#
# Create a prebuild conquest dgate
#
# to build:
#    docker build -t prebuild .
#
# to run the container interactively with bash:
#    docker run -it prebuild bash
#
# docker cp CONTAINER:/Conquest-DICOM-Server/dgate .
# docker cp CONTAINER:/Conquest-DICOM-Server/src/servertask/servertask .
#
##########

FROM ubuntu:14.04

#get tools and libraries needed
RUN apt update
RUN apt -y install make sudo nano gettext-base git g++ lua5.1 liblua5.1-0
RUN ln -s /usr/lib/x86_64-linux-gnu/liblua5.1.so.0 /usr/lib/x86_64-linux-gnu/liblua5.1.so
#RUN ln -s /usr/lib/aarch64-linux-gnu/liblua5.1.so.0 /usr/lib/aarch64-linux-gnu/liblua5.1.so

#get conquest source code
COPY Dockerfile .
RUN git clone https://github.com/marcelvanherk/Conquest-DICOM-Server
#COPY Conquest-DICOM-Server Conquest-DICOM-Server

#mark scripts as executable
RUN cd Conquest-DICOM-Server && chmod 777 maklinux
RUN cd Conquest-DICOM-Server && chmod 777 linux/regen
RUN cd Conquest-DICOM-Server && chmod 777 linux/updatelinux
RUN cd Conquest-DICOM-Server && chmod 777 linux/restart
RUN cd Conquest-DICOM-Server && chmod 777 linux/maklinux_precompiled

#compile, update dicom.ini if next to Dockerfile, and regenerate the SQLite database
RUN cd Conquest-DICOM-Server && linux/maklinux_precompiled
RUN cd Conquest-DICOM-Server/src/servertask && chmod 777 make.sh && ./make.sh

###### additional docker commands:
#
#  to cleanup docker:
#  docker system prune -a
#
######
