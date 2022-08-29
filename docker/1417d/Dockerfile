# ******************************************************************
# 
# This is a Docker image, implementing a working ConQuest DICOM Server
# 
# Full information on the fabulous ConQuest DICOM Server can be found
# here: https://ingenium.home.xs4all.nl/dicom.html
# 
# ******************************************************************

# Pull base image.
FROM ubuntu:14.04

# Install wget
RUN apt-get -y update && apt-get install -y wget

# Get the conquest server files
RUN mkdir conquest; cd conquest; \
	wget http://natura-ingenium.nl/dicomserver/conquestlinux1417d.tar.gz; \
	tar -xvf conquestlinux1417d.tar.gz

# Next run the pre-requisites for installation
RUN sudo apt-get -y update && \
	apt-get install -y \
		g++ \
		apache2 \
		make

# Create missing directory prior to make (otherwise we'll get errors)
RUN mkdir /usr/local/man/man1

# Install the jpeg-6c library
RUN cd conquest; \
	cd jpeg-6c; \
	./configure; \
	sudo make; \
	sudo make install;

# Install the JasPer image libraries
RUN cd conquest; \ 
	cd jasper-1.900.1-6ct; \
	./configure; \
	sudo make; \
	sudo make install;

# Compile and install the web access
RUN cd conquest; \
	./maklinux

# Enable CGI scripts on the Apache Server
RUN ln -s /etc/apache2/mods-available/cgi.load /etc/apache2/mods-enabled/cgi.load

# Enable 7za, so that data can be zipped from the "push" page
RUN apt-get install p7zip-full

# Copy across conquest.jpg file
RUN cp /var/www/conquest.jpg /var/www/html

# Expose port 80 (http) and 5678 (for DICOM query/retrieve/send)
EXPOSE 5678 80

# Regenerate the database
RUN cd conquest; ./dgate -v -r

#Generate the autostart script which will be used to initialise the server
RUN echo "#!/bin/bash" > startConquest.sh; \
	echo "service apache2 restart" >> startConquest.sh; \
	echo "cd /conquest" >> startConquest.sh; \
	echo "./dgate -v" >> startConquest.sh; \
	chmod +x startConquest.sh

# Start apache and ConQuest
# The server should then be running and localhost/cgi-bin/dgate should provide a working web interface.
CMD ["/startConquest.sh"]
