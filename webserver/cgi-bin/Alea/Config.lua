-- This web page shows relevant configuration for the Conquest - Alea link
-- mvh  20120828
-- rvdb 20121017
-- mvh+evd 20130905: simplified parameters, default to curl for submit2:
-- mvh 20140629: Show VirtualServerFor0 as well

-- To let this work put this in dicom.ini in the cgi-bin folder
--[[
# DefaultPage
[DefaultPage]
source = *.lua

[Alea]
Source=(local)
TargetServer=curl -k -T %s https://www.tenalea.com/pacsupload
]]--

local source = gpps('Alea', 'Source', '(local)');
local targetserver = gpps('Alea', 'TargetServer', 'curl -k -T %s https://www.tenalea.com/pacsupload');

local serverversion = servercommand('display_status:');
if serverversion==nil then
  HTML('Content-type: text/html\n\n');
  print([[
  <HEAD><TITLE>Alea Conquest Link Configuration</TITLE>
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
<HEAD><TITLE>Alea Conquest Link Configuration</TITLE>
</HEAD>
<BODY BGCOLOR='CFDFCF'>
<H3>Conquest ]]..version..[[ Alea Conquest Link Configuration</H3>
<TABLE border="1">
<TR>
<TH>Item</TH><TH>Value</TH><TH>Location</TH><TH>Section</TH>
</TR>
<TR>
<TD>CGI version</TD><TD>]]..version..[[</TD><TD>cgi-bin/dgate</TD><TD>n/a</TD>
</TR>
<TR>
<TD>Source</TD><TD>]]..source..[[</TD><TD>cgi-bin/dicom.ini</TD><TD>Alea</TD>
</TR>
<TR>
<TD>Upload command</TD><TD>]]..targetserver..[[</TD><TD>cgi-bin/dicom.ini</TD><TD>Alea</TD>
</TR>
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
<TD>Virtual Server</TD><TD>]]..servercommand('get_param:VirtualServerFor0')..[[</TD><TD>dicomserver/dicom.ini</TD><TD>sscscp</TD>
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
</BODY>
]])

