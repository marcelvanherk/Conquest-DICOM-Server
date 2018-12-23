Hi,

I just tested with Weasis 2.0.7 and all works well.

Only to newbies, to install weasis locally in your server, download Weasis portable zip file for sourceforge, and unpack it in your web server directory.
After configure your webserver, change your settings as posted previously by Mr Marcel.

Luiz

Here are updates for the latest Weasis V2.0.7

Install Java
Download weasis-portable.zip (14.2 MB) and unzip it into e.g. folder c:\xampp\htdocs\weasis

Enable the viewers as such:

[webdefaults]
...
viewer = weasis
studyviewer = studyweasis

[weasis]
source = viewers\LaunchWeasis.cq
header = Content-Type: application/x-java-jnlp-file\

[studyweasis]
source = viewers\LaunchWeasisStudy.cq
header = Content-Type: application/x-java-jnlp-file\

[weasisseriesxml]
source = viewers\weasisseriesxml.lua

[weasisstudyxml]
source = viewers\weasisstudyxml.lua

It then works locally for me. To make it work remotely, replace all instances of 127.0.0.1 in the weasis*.cq files with the server's address

Marcel