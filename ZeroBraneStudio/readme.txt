This folder contains all files necessary to enable editing and debugging [Lua](http://www.lua.org) 
scripts for [Conquest Dicom Server](http://ingenium.home.xs4all.nl/dicom.html) (`V 1.4.1c up`) in the fantastic [ZeroBrane Studio](http://studio.zerobrane.com) (`V 1.70 up`) development environment. 

Use as follows: 

Open **ZeroBrane Studio** (use sudo in Linux to be able to add to its files)
Load the install.lua file with     `File - Open`
click the --> [install now] <-- link

After this - ZeroBrane Studio will reopen ready to run demo scripts 
and develop for **Conquest Dicom Server**. As installed, Zerobrane Studio
communicates with a running Conquest Dicom Server on port 5678 and offers code 
completion and full debugging facilities. 

To change the port it uses, open system preferences and add, e.g.:

conquest = {}
conquest.port = 104


