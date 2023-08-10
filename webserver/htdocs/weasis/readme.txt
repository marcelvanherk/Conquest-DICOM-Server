To start a recent Weasis from newweb, update webserver files from Git, install Weasis, and add these lines to your app/newweb/dicom.ini under section [webdefaults].

studylink = '<TD><A href=weasis://%24dicom%3Ars%20--url%20%22http%3A%2F%2F127.0.0.1%2Fapi%2Fdicom%2Frs%22%20-r%20%22studyUID={StudyInstanceUID}%22>Weasis</a>'

serieslink = '<TD><A href=weasis://%24dicom%3Ars%20--url%20%22http%3A%2F%2F127.0.0.1%2Fapi%2Fdicom%2Frs%22%20-r%20%22seriesUID={SeriesInstanceUID}%22>Weasis</a>'

Note that the links inside the Weasis command must be absolute and must therefore be edited to replace e.g. 

http://127.0.0.1 

with 

http://127.0.0.1:8086 (for Ladle server) 

or 

http://yourhostname

