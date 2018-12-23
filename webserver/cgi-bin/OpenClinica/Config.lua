-- This web page shows relevant configuration for the Conquest - OpenClinica link
-- mvh 20120826

-- To let this work put this in dicom.ini in the cgi-bin folder
--[[
# DefaultPage
[DefaultPage]
source = *.lua

[OpenClinica]
Source=(local)
PatientIDkeep=cookie
AnonymizeOn=transmission
TargetServer=m.v.herk@ftp-rt.nki.nl:
password=XXXXXXXXXXX
]]--

local source = gpps('OpenClinica', 'Source', '(local)');
local patientidwhere = gpps('OpenClinica', 'PatientIDkeep', 'cookie');
local anonymizeon = gpps('OpenClinica', 'AnonymizeOn', 'transmission');
local targetserver = gpps('OpenClinica', 'TargetServer', 'user@server.domain:');
local password = gpps('OpenClinica', 'Password', 'XXXXXXXXXXX');

local serverversion = servercommand('display_status:');
if serverversion==nil then
  HTML('Content-type: text/html\n\n');
  print([[
  <HEAD><TITLE>OpenClinica Conquest Link Configuration</TITLE>
  </HEAD>
  <BODY BGCOLOR='FFCFCF'>
  <H3>The web server is up, but the DICOM server is not accessible</H3>
  </BODY>
  ]])
  return
end
serverversion = string.sub(serverversion, string.find(serverversion, 'version........'));

HTML('Content-type: text/html\n\n');

print([[
<HEAD><TITLE>OpenClinica Conquest Link Configuration</TITLE>
</HEAD>
<BODY BGCOLOR='CFDFCF'>
<H3>Conquest ]]..version..[[ OpenClinica Conquest Link Configuration</H3>
<TABLE border="1">
<TR>
<TH>Item</TH><TH>Value</TH><TH>Location</TH><TH>Section</TH>
</TR>
<TR>
<TD>CGI version</TD><TD>]]..version..[[</TD><TD>cgi-bin/dgate</TD><TD>n/a</TD>
</TR>
<TR>
<TD>Source</TD><TD>]]..source..[[</TD><TD>cgi-bin/dicom.ini</TD><TD>OpenClinica</TD>
</TR>
<TR>
<TD>Patient ID stored in</TD><TD>]]..patientidwhere..[[</TD><TD>cgi-bin/dicom.ini</TD><TD>OpenClinica</TD>
</TR>
<TR>
<TR>
<TD>Anonymize on</TD><TD>]]..anonymizeon..[[</TD><TD>cgi-bin/dicom.ini</TD><TD>OpenClinica</TD>
</TR>
<TR>
<TD>SFTP target server</TD><TD>]]..targetserver..[[</TD><TD>cgi-bin/dicom.ini</TD><TD>OpenClinica</TD>
</TR>
<TR>
<TD>SFTP password</TD><TD>]]..'#######'..[[</TD><TD>cgi-bin/dicom.ini</TD><TD>OpenClinica</TD>
</TR>
<TR>
<TR>
</TR>

<TR>
</TR>
<TD>Server Version</TD><TD>]]..serverversion..[[</TD><TD>dicomserver/dgate</TD><TD>n/a</TD>
</TR>
</TR>
<TD>Server AE</TD><TD>]]..servercommand('get_param:MyACRNema')..[[</TD><TD>dicomserver/dicom.ini</TD><TD>sscscp</TD>
</TR>
<TR>
<TD>Server Port</TD><TD>]]..servercommand('get_param:TCPPort')..[[</TD><TD>dicomserver/dicom.ini</TD><TD>sscscp</TD>
</TR>
<TR>
</TR>
<TR>
</TR>
]])

local i=1;
repeat
  local a = servercommand('get_amap:'..i);
  if a~=nil then
    print([[
    <TR>
    <TD>Known DICOM ]]..i..[[</TD><TD>]]..a..[[</TD><TD>dicomserver/acrnema.map</TD><TD>n/a</TD>
    ]])
  end;
  i = i+1;
until a==nil;

print([[
<TABLE>
</BODY>
]])


