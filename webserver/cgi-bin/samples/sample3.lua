HTML('Content-type: text/html\n\n');

HTML('<H1>Welcome to Conquest DICOM server lua page, version:'..version..'</H1>')

HTML'<H2>Will query database defined by SQLServer in dicom.ini:</H2>'
HTML'<br>'
HTML(dbquery('DICOMImages', 'SOPInstanc')[1][1])
HTML'<hr>'

HTML'<H2>Will send command to WebServerFor:TCPPort in dicom.ini:</H2>'
HTML'<br>'
HTML(servercommand('display_status:'))
HTML'<hr>'

HTML'<H2>Will read [sscscp] section in dicom.ini:</H2>'
HTML'<br>'
HTML(Global.SQLServer)
HTML'<hr>'

HTML'<H2>Global lua variables</H2>'
HTML(query_string)
HTML'<br>'
HTML(server_name)     
HTML'<br>'
HTML(script_name)     
HTML'<br>'
HTML(path_translated)
HTML'<br>'
HTML(port)  
HTML'<br>'
HTML(address)
HTML'<br>'
HTML(webcodebase)
HTML'<br>'
HTML(webscriptadress)
HTML'<br>'
HTML(extra)
HTML'<br>'
HTML(version)
HTML'<br>'
HTML(mode)      

HTML'<H2>Read ACRNEMA.map (define Dictionary and ACRNemaMap in dicom.ini)</H2>'
HTML'<br>'
-- note: HTML will only print first return value of 4
print(get_amap(0))
HTML'<br>'

HTML'<H2>Query dicom server(define Dictionary and ACRNemaMap in dicom.ini)</H2>'
b=newdicomobject();
b.PatientName = '*'; 
b.PatientID = ''; 
a=dicomquery('CONQUESTSRV1', 'PATIENT', b);
print(a[0].PatientID)
