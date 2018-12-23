This folder contains all script allowing integration of OpenClinica
and a PACS using Conquest (and its web server) as a local Gateway.

--------------------------------------
To let this work put these items in dicom.ini in the cgi-bin folder
(e.g., c:\xampp\cgi-bin)


[DefaultPage]

source = *.lua



[OpenClinica]

Source=(local)

TargetServer=user@server.domain:
password=XXXXXXXXXXX


Where Source is the AE of the PACS to connect to or (local)
to enable transfer of data in the local server only

Targetserver is a sftp (ssh) server to be accessed by pscp
password is its password

--------------------------------------
Then initiate the pscp key hash:

cd c:\dicomserver
pscp dicom.ini user@server.domain:

-------------------------------------
In the dicomserver folder (e.g. c:\dicomserver) check that these files are present:

anonymize_script.cq
lua/anonymize_script.lua

Your PACS admin may want to inspect lua/anonymize_script.lua

-------------------------------------
In the openclinica (Linux) installation put marcel.js, e.g.:
sudo cp Desktop/marcel.js /usr/local/tomcat/webapps/OpenClinica/includes
/

See further file marcel.js and the Conquest forum
