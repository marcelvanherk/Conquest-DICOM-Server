-------------------------------------------------------------------------------
-- pure lua weasis (V3.0 up) connector for conquest dicom server
-- to use, copy into webserver/cgi-bin/newweb and modify dicom.ini there as follows:
-- lines 18, 19
-- viewer   = weasis_starter
-- studyviewer = weasis_starter
-- line 26 (for newweb only, not for the classical interface)
-- exceptions=start,listpatients,liststudies,listseries,listimageswiththumbs,listimages,wadostudyviewer,wadoseriesviewer,wadoviewerhelp,slice,listtrials,listtrialpatient,weasis_starter
--
-- To update weasis version just replace the jnlp and xml text block indicated in the code
--
-- This file runs as one of the following four modes, 2 create the jnlp starter, 2 create xml manifests
-- dgate.exe?mode=weasis_starter&study=patid:studyuid
-- dgate.exe?mode=weasis_starter&series=patid:seriesuid
-- dgate.exe?mode=weasis_starter&accession=accessionnumber
-- dgate.exe?mode=weasis_starter&parameter=xml&study=patid:studyuid
-- dgate.exe?mode=weasis_starter&parameter=xml&series=patid:seriesuid
-- dgate.exe?mode=weasis_starter&parameter=xml&accession=accessionnumber
--
-- optional parameters:
--   compress, WebScriptAddress and WebCodebase are taken from dicom.ini
-------------------------------------------------------------------------------
-- mvh 20181123 Created for 1.4.19c
-- mvh 20181124 Fixed server links; Removed PatientID for study xml
-- mvh 20181213 server_name does not copy port use relative link and WebScriptAddress;
--              moved WadoTransferSyntaxUID to series level
-- mvh 20181214 Made to work with help from lyakh92; uses compress and WebScriptAddress from dicom.ini
-- mvh 20181227 Use remotequery to limit contralize access to server; use gpps WebScriptAddress
-- mvh 20181229 Fixed section of WebScriptAddress
-- mvh 20180112 Fix to allow : in patientID
-- mvh 20180302 Added accession= access for external use
-- mvh 20180303 Some fixes in that code

local source_server = Global.WebCodeBase

local parameter = CGI('parameter', 'jnlp')
local study = CGI('study', '')
local series = CGI('series', '')
local accession = CGI('accession', '')
local level = 'study'
local ident = study
if study=='' then
  level = 'series'
  ident = series
end
if series=='' then
  level = 'accession'
  ident = accession
end

-- for testing
-- parameter='xml' 
-- script_name=script_name or 'aap'

-------------------------------------------------------------------------------------------------------------
-- this text between [[ ]] is the JNLP sample file from the weasis website without change, it is adapted by the lua code below
-- I had to remove line <extension href="http://localhost:8080/weasis/substance.jnlp" />, for weasis_portable3.0

local jnlp = [[
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE jnlp PUBLIC "-//Sun Microsystems, Inc//DTD JNLP Descriptor 6.0//EN" "http://java.sun.com/dtd/JNLP-6.0.dtd">
  <jnlp spec="1.6+" codebase="http://localhost:8080/weasis" href="">
  <information>
    <title>Weasis</title>
    <vendor>Weasis Team</vendor>
    <description>DICOM images viewer</description>
    <description kind="short">An application to visualize and analyze DICOM images.</description>
    <description kind="one-line">DICOM images viewer</description>
    <description kind="tooltip">Weasis</description>
  </information>
  <security>
    <all-permissions />
  </security>

  <resources>
    <!-- Requires Java SE 8 for Weasis 2.5 and superior -->
	<java version="9+" href="http://java.sun.com/products/autodl/j2se" java-vm-args="--add-modules java.xml.bind --add-exports=java.base/sun.net.www.protocol.http=ALL-UNNAMED --add-exports=java.base/sun.net.www.protocol.https=ALL-UNNAMED --add-exports=java.base/sun.net.www.protocol.file=ALL-UNNAMED --add-exports=java.base/sun.net.www.protocol.ftp=ALL-UNNAMED --add-exports=java.base/sun.net.www.protocol.jar=ALL-UNNAMED --add-exports=jdk.unsupported/sun.misc=ALL-UNNAMED --add-opens=java.base/java.net=ALL-UNNAMED --add-opens=java.base/java.lang=ALL-UNNAMED --add-opens=java.base/java.security=ALL-UNNAMED --add-opens=java.base/java.io=ALL-UNNAMED --add-opens=java.desktop/javax.imageio.stream=ALL-UNNAMED --add-opens=java.desktop/javax.imageio=ALL-UNNAMED --add-opens=java.desktop/com.sun.awt=ALL-UNNAMED" initial-heap-size="128m" max-heap-size="768m" />
	<java version="9+" java-vm-args="--add-modules java.xml.bind --add-exports=java.base/sun.net.www.protocol.http=ALL-UNNAMED --add-exports=java.base/sun.net.www.protocol.https=ALL-UNNAMED --add-exports=java.base/sun.net.www.protocol.file=ALL-UNNAMED --add-exports=java.base/sun.net.www.protocol.ftp=ALL-UNNAMED --add-exports=java.base/sun.net.www.protocol.jar=ALL-UNNAMED --add-exports=jdk.unsupported/sun.misc=ALL-UNNAMED --add-opens=java.base/java.net=ALL-UNNAMED --add-opens=java.base/java.lang=ALL-UNNAMED --add-opens=java.base/java.security=ALL-UNNAMED --add-opens=java.base/java.io=ALL-UNNAMED --add-opens=java.desktop/javax.imageio.stream=ALL-UNNAMED --add-opens=java.desktop/javax.imageio=ALL-UNNAMED --add-opens=java.desktop/com.sun.awt=ALL-UNNAMED" initial-heap-size="128m" max-heap-size="768m" />
    <j2se version="1.8+" href="http://java.sun.com/products/autodl/j2se" initial-heap-size="128m" max-heap-size="768m" />
    <j2se version="1.8+" initial-heap-size="128m" max-heap-size="768m" />

    <jar href="http://localhost:8080/weasis/weasis-launcher.jar" main="true" />
    <jar href="http://localhost:8080/weasis/felix.jar" />

    <!-- Optional library (Substance Look and feel, only since version 1.0.8). Requires the new Java Plug-in introduced in the Java SE 6 update 10 release.For previous JRE 6, substance.jnlp needs a static codebase URL -->

    <!-- Allows to get files in pack200 compression, only since Weasis 1.1.2 -->
    <property name="jnlp.packEnabled" value="true" />

    <!-- ================================================================================================================= -->
    <!-- Security Workaround. Add prefix "jnlp.weasis" for having a fully trusted application without signing jnlp (only since weasis 1.2.9), http://bugs.sun.com/bugdatabase/view_bug.do?bug_id=6653241 -->
    <!-- Required parameter. Define the location of config.properties (the OSGI configuration and the list of plug-ins to install/start) -->
    <property name="jnlp.weasis.felix.config.properties" value="http://localhost:8080/weasis/conf/config.properties" />
    <!-- Optional parameter. Define the location of ext-config.properties (extend/override config.properties) -->
    <property name="jnlp.weasis.felix.extended.config.properties" value="http://localhost:8080/weasis-ext/conf/ext-config.properties" />
    <!-- Required parameter. Define the code base of Weasis for the JNLP -->
    <property name="jnlp.weasis.weasis.codebase.url" value="http://localhost:8080/weasis" />
    <!-- Optional parameter. Define the code base ext of Weasis for the JNLP -->
    <property name="jnlp.weasis.weasis.codebase.ext.url" value="http://localhost:8080/weasis-ext" />
    <!-- Required parameter. OSGI console parameter -->
    <property name="jnlp.weasis.gosh.args" value="-sc telnetd -p 17179 start" />
    <!-- Optional parameter. Allows to have the Weasis menu bar in the top bar on Mac OS X (works only with the native Aqua look and feel) -->
    <property name="jnlp.weasis.apple.laf.useScreenMenuBar" value="true" />
    <!-- Optional parameter. Allows to get plug-ins translations -->
    <property name="jnlp.weasis.weasis.i18n" value="http://localhost:8080/weasis-i18n" />
    <!-- Optional Weasis Documentation -->
    <!-- <property name="jnlp.weasis.weasis.help.url" value="${cdb}/../weasis-doc" /> -->
    <!-- ================================================================================================================= -->
  </resources>

  <application-desc main-class="org.weasis.launcher.WebstartLauncher">
    <!-- Example for opening dicom files from remote xml file -->
    <argument>$dicom:get -w "http://localhost:8080/dcm4chee-web/wadoQueries/wado_query3888637380.xml.gz"</argument>

    <!-- Example for opening dicom files from a local folder -->
        <argument>$dicom:get -l "/home/Images/MRIX LUMBAR/"</argument>

    <!-- Example for opening dicom files by embedding the xml file encoded in gzip and then in base64, it must be in one line without space at the beginning -->
    <argument>$dicom:get -i "H4sIAAAAAAAAALVV7Y+aMBz+vr+i6XdLXwDBHHdxpy4mvgX0dvtkOqnaBOGO1qn//RUQnW5uy3JHCC2/1+d5Wsrdw36TgB8iVzJLA0gQhkCkiyyW6SqAW71seBA83H+62/E4m79uR..."</argument>

    <!-- Example for opening dicom files from URLs -->
    <argument>$dicom:get -r "http://server/images/img1.dcm http://server/images/img2.dcm"</argument>
  </application-desc>
  </jnlp>
]]
-------------------------------------------------------------------------------------------------------------

-- split string into pieces, return as table
function split(str, pat)
   local t = {} 
   local fpat = "(.-)" .. pat
   local last_end = 1
   local s, e, cap = str:find(fpat, 1)
   while s do
      if s ~= 1 or cap ~= "" then
	      table.insert(t,cap)
      end
      last_end = e+1
      s, e, cap = str:find(fpat, last_end)
   end
   if last_end <= #str then
      cap = str:sub(last_end)
      table.insert(t, cap)
   end
   return t
end

function remotequery(ae, level, q)
  local remotecode =
[[
  local ae=']]..ae..[[';
  local level=']]..level..[[';
  local q=]]..q:Serialize()..[[;
  local q2=DicomObject:new(); for k,v in pairs(q) do q2[k]=v end;
  local r = dicomquery(ae, level, q2):Serialize();
  local s=tempfile('txt') local f=io.open(s, "wb") f:write(r) returnfile=s f:close();
]]
  local f = loadstring('return '..servercommand('lua:'..remotecode));
  if f then return f() end
end

-----------------------------------
-- render jnlp file if requested --
-----------------------------------

if parameter=='jnlp' then
HTML("Content-Type: application/x-java-jnlp-file\n")
  local xmlline = string.format([[<argument>$dicom:get -w "%s?mode=weasis_starter&parameter=xml&compress=%s&%s&dum=.xml"</argument>]],
    gpps('sscscp', 'WebScriptAddress', ''), gpps('webdefaults', 'compress', 'un'), level..'='..ident)
  jnlp = split(jnlp, '\n')
  for k,v in ipairs(jnlp) do
    v = string.gsub(v, 'http://localhost:8080/', source_server)
    if string.find(v, 'dicom:get -r', 1, true) then v='' end
    if string.find(v, 'dicom:get -i', 1, true) then v='' end
    if string.find(v, 'dicom:get -l', 1, true) then v='' end
    if string.find(v, 'dicom:get -w', 1, true) then v=xmlline end
    print(v)
  end
  return
end

-------------------------------------------------------------------------------------------------------------
-- this between [[ and ]] is the XML sample file from the weasis website without change, 
-- optionally all items may be reduced to one, for the rest it is adapted by the lua code below

local xml=
[[<?xml version="1.0" encoding="UTF-8" ?>
<manifest xmlns="http://www.weasis.org/xsd/2.5" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
    <arcQuery additionnalParameters="" arcId="1001" baseUrl="http://archive-weasis.rhcloud.com/archive/wado" requireOnlySOPInstanceUID="false">
        <Patient PatientID="H13885_9M" PatientName="TEST NON SQUARE PIXELS" PatientSex="F">
            <Study AccessionNumber="" ReferringPhysicianName="" StudyDate="20130711" StudyDescription="TEST NON SQUARE PIXELS" StudyID="PKD" StudyInstanceUID="2.16.756.5.5.100.397184556.14391.1373576413.1508" StudyTime="170013">
                <Series Modality="US" SeriesDescription="NON SQUARE PIXELS: PIXEL ASPECT RATIO" SeriesInstanceUID="1.2.40.0.13.1.1.87878503032592846377547034671833520632" SeriesNumber="2">
                    <Instance InstanceNumber="107" SOPInstanceUID="1.2.40.0.13.1.1.126082073005720329436273995268222863740"/>
                </Series>
            </Study>
        </Patient>
    </arcQuery>
</manifest>
]]
-------------------------------------------------------------------------------------------------------------

-----------------------------------
-- render xml file if requested ---
-----------------------------------

if parameter=='xml' then
  -- utility functions first
  
  -- make DICOM query from sample xml line
  function xmltoquery(line)
    local q = DicomObject:new()
    local level = string.match(line, '<(.-) ')
    if level=='Instance' then level='Image' end
    q.QueryRetrieveLevel = level
    string.gsub(line, "([%w:]+)=([\"'])(.-)%2", function (w, _, a)
      q[w]=''
    end)
    return q
  end
  
  -- print level xml open statement
  function xmlopen(level, t)
    local s
    if level=='PATIENT' then
      s='      <Patient '
    elseif level=='STUDY' then
      s='          <Study '
    elseif level=='SERIES' then
      s='              <Series '
    elseif level=='INSTANCE' then
      s='                  <Instance '
    end
    for k,v in pairs(t) do
      if k:find('Name')        then v=v:gsub('%W','_') end
      if k:find('Description') then v=v:gsub('%W','_') end
      if k:find('Number')      then v=v:gsub(' ','') end
      s = s .. string.format('%s="%s" ', k, v)
    end
    if level=='INSTANCE' then 
      print(s .. '/>')
    else
      print(s .. '>')
    end
  end
  
  -- print level xml close statement
  function xmlclose(level, t)
    if level=='PATIENT' then
      print('      </Patient>')
    elseif level=='STUDY' then
      print('          </Study>')
    elseif level=='SERIES' then
      print('              </Series>')
    end
  end

  -- start of xml generator
  -- split query information
  local patid = string.gsub(ident, ':[^:]-$', '')
  local uid = string.gsub(ident, '^.*:', '')
  local compress = CGI('compression', 'un')
  local source = servercommand('get_param:MyACRNema')
  local paq, stq, seq, inq
  
  -- create compression transfer syntax
  local wt = ''
  if compress=='J5' or compress=='j5' then wt = '1.2.840.10008.1.2.4.53' end
  if compress=='J4' or compress=='j4' then wt = '1.2.840.10008.1.2.4.51' end
  if compress=='J3' or compress=='j3' then wt = '1.2.840.10008.1.2.4.50' end
  if compress=='J1' or compress=='j1' then wt = '1.2.840.10008.1.2.4.55' end
  if compress=='J2' or compress=='j2' then wt = '1.2.840.10008.1.2.4.57' end
  if compress=='J1' or compress=='j1' then wt = '1.2.840.10008.1.2.4.70' end
  if compress=='JK' or compress=='jk' then wt = '1.2.840.10008.1.2.4.90' end
  if compress=='JL' or compress=='jl' then wt = '1.2.840.10008.1.2.4.91' end

  -- first pass: set baseUrl and build default queries 
  xml = split(xml, '\n')
  for k,v in ipairs(xml) do
    if string.find(v, 'baseUrl=') then 
      xml[k] = string.gsub(v, 'baseUrl=".-"', 'baseUrl="'..gpps('sscscp', 'WebScriptAddress', '')..'"')
    end
    if string.find(v, '<Patient', 1, true) and paq==nil then
      paq = xmltoquery(v)
    end
    if string.find(v, '<Study', 1, true) and stq==nil then
      stq = xmltoquery(v)
    end
    if string.find(v, '<Series', 1, true) and seq==nil then
      seq = xmltoquery(v)
    end
    if string.find(v, '<Instance', 1, true) and inq==nil then
      inq = xmltoquery(v)
    end
  end

  -- print preamble
  print('Content-Type: application/xml\n')
  for k,v in ipairs(xml) do
    if string.find(v, '<Patient', 1, true) then 
      break
    end
    print(v)
  end
  
  -- set start query parameters
  local par, str, ser, inr
  paq.PatientID = patid
  if level=='series' then 
    seq.SeriesInstanceUID = uid
    seq.StudyInstanceUID = ''
    ser = remotequery(source, 'SERIES', seq)
    stq.StudyInstanceUID = ser[1].StudyInstanceUID
  end
  if level=='study' then
    seq.StudyInstanceUID = uid
    stq.StudyInstanceUID = uid
  end
  if level=='accession' then
    stq.AccessionNumber = ident
    stq.StudyInstanceUID = ''
    stq.PatientID = ''
    str = remotequery(source, 'STUDY', stq)
    paq.PatientID = str[1].PatientID
    seq.StudyInstanceUID = str[1].StudyInstanceUID
    stq.StudyInstanceUID = str[1].StudyInstanceUID
  end
  
  -- loop over levels and print xml patient data
  par = remotequery(source, 'PATIENT', paq)
  for i=1, #par do
    par[i].QueryRetrieveLevel=nil
    par[i].TransferSyntaxUID=nil
    xmlopen('PATIENT', par[i])
    stq.PatientID=par[i].PatientID
    str = remotequery(source, 'STUDY', stq)
    for j=1, #str do
      str[j].QueryRetrieveLevel=nil
      str[j].TransferSyntaxUID=nil
      str[j].PatientID=nil
      xmlopen('STUDY', str[j])
      seq.StudyInstanceUID=str[j].StudyInstanceUID
      ser = remotequery(source, 'SERIES', seq)
      for k=1, #ser do
        ser[k].QueryRetrieveLevel=nil
        ser[k].TransferSyntaxUID=nil
        ser[k].StudyInstanceUID=nil
	local r=ser[k]
        if wt~='' then r.WadoTransferSyntaxUID = wt end
        xmlopen('SERIES', r)
	inq.SeriesInstanceUID=ser[k].SeriesInstanceUID
        inr = remotequery(source, 'IMAGE', inq)
        for L=1, #inr do
          local t = inr[L]
	  t.QueryRetrieveLevel=nil
          t.TransferSyntaxUID=nil
          t.SeriesInstanceUID=nil
          xmlopen('INSTANCE', t)
	  xmlclose('INSTANCE')
        end
        xmlclose('SERIES')
      end
      xmlclose('STUDY')
    end
    xmlclose('PATIENT')
  end

  -- print last lines
  local inlast=false
  for k,v in ipairs(xml) do
    if string.find(v, '</Patient', 1, true) then 
      inlast=true
    elseif inlast then
      print(v)
    end
  end
end
