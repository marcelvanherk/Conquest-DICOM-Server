-- 20200126	mvh	Fixed findData
-- 20200127	mvh	Added collateteacher command; writes jpg and rtstruct header for now
-- 20200201	mvh	Added collateobservers, both write json per structure and per slice
-- 20200202	mvh	Use 9999,9000=patientID.observer.session to load observer data
--                      Added session and slice info to contours; todo: color
-- 20200204	mvh	Fixed session number in collateobservers; documentation
--                      Store as observer.json; fix one remaining ImageLocationPatient
--                      Convert ImagePositionPatient, ImageOrientationPatient, PixelSpacing into arrays
--			Do not write slice jsons if converting single observer
--			Added goldobserver mode: upload gold standard (input data) as observer
-- 20200205	mvh	Todo: add observerstandard mode: upload observer as gold standard
--                      Todo: check when uploading rtstruct where it comes from: input, server or observer
-- 20200211	mvh	Empty ContourSequence inside - instead of ROIContourSequence
-- 20200224 evo added list of RTStructs.  TO DO: remove .txt and .json of erased observers from collated folder.
-- 20200302 mvh target and ds are different for linux
-- 20200303 evo added code to generate json for template.

-- call with
-- patient|study|mode|session|extra; 		--  e.g. (only with CAPS are used)
-- PATIENT|study|template 			--> generates template from Data and stores in correct place
-- PATIENT|study|goldstandard|session|GSNAME 	--> stores gold standard in Data in correct place with gsname
-- PATIENT|study|goldobserver|session|OBSNAME 	--> stores observer (fits input CT) in Data in correct place with observer name
-- patient|study|ANY|session|(obs.ses)          --> stores observer (fits observer CT) in correct place 
--                                              (all other info encoded in Data, but can override observername.session)
-- PATIENT|study|collateteacher|SESSION 	--> collates all teacher data (CT and goldstandards)
-- PATIENT|study|collateobservers|SESSION|(name)--> collates all or one (name) observer data



local JSON = require('json')

function fileexists(f)
  local h = io.open(f)
  if h==nil then 
    return false
  else
    h:close()
    return true
  end
end

function filesize(f)
  local h = io.open(f)
  if h==nil then 
    return 0
  else
    local s = h:seek('end', 0)
    h:close()
    return s
  end
end

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

function strSplit(str)
    local t = {}
    local i = 1;
    local p = {0, 0, 0}
    for substr in string.gmatch(str, "[^\\]+") do
      p[i]= tonumber(substr)
      i   = i+1;
      if i==4 then
        table.insert(t,p)
	i = 1;
	p = {0, 0, 0}
      end
    end
    return t
end

function findData(patid, description)
  local x = DicomObject:new()
  print('search', patid, description)
  x.PatientID = patid
  x.SeriesDescription = description
  x.SeriesInstanceUID = ''
  x.StudyInstanceUID = ''
  x.QueryRetrieveLevel = 'SERIES'
  local r = dicomquery('CONQUESTSRV1', 'SERIES', x)
  if r==nil then return genuid(),genuid() end
  if #r==0 then return genuid(),genuid() end
  return r[0].SeriesInstanceUID, r[0].StudyInstanceUID
end

function toLua(im)
  local s= im:Serialize()
  return loadstring('return '..s)()
end

local inputs = split(command_line, '|')
print(inputs[1])
print(inputs[2])
print(inputs[3])
print(inputs[4])
print(inputs[5])

local target, ds
if string.find(Global.basedir, '/') then
  target = [[/var/www/html/inholland/data/]]
  ds = '/'
else
  target = [[c:\xampp\htdocs\inholland\data\]]
  ds = '\\'
end


if inputs[3]=='template' then
  Data.StudyDescription = 'Source data'
  Data.SeriesDescription = 'Template'
  Data.SeriesInstanceUID = findData(inputs[1], 'Template')
  --while #Data.ROIContourSequence>0 do Data.ROIContourSequence:Delete(0) end
  for i=0, #Data.ROIContourSequence-1 do
    while #Data.ROIContourSequence[i].ContourSequence>0 do 
      Data.ROIContourSequence[i].ContourSequence:Delete(0) 
    end
  end
  -- to allow downsizing the CT data; need to check if
  -- ReferencedFrameOfReferenceSequence[].RTReferencedStudySequence[].RTReferencedSeriesSequence[].ContourImageSequence[].ReferencedSOPInstanceUID
  -- exists in CT data in study; if not delete ContourImageSequence[]
  Data.SOPInstanceUID = genuid()

elseif inputs[3]=='goldstandard' then
  Data.StudyDescription = 'Analysis'
  Data.SeriesDescription = 'Gold standard'
  Data.SeriesInstanceUID, Data.StudyInstanceUID = findData(inputs[1], 'Gold standard')
  Data.ImageID = inputs[5] or 'Unknown'
  Data.StructureSetLabel = inputs[5] or 'Unknown'

elseif inputs[3]=='goldobserver' then
  Data.StudyDescription = 'Analysis'
  Data.SeriesDescription = 'Observers'
  Data.SeriesInstanceUID, Data.StudyInstanceUID = findData(inputs[1], 'Observers')
  Data.ImageID = inputs[5] or 'Unknown'
  Data.StructureSetLabel = inputs[5] or 'Unknown'

elseif inputs[3]=='collateteacher' or inputs[3]=='collateobservers' then
  command_line = "Done";
  print(inputs[3], inputs[1])

  local folder = 'Observers'
  if inputs[3]=='collateteacher' then folder = 'GS' end

  -- search images
  local x = DicomObject:new()
  x.PatientID = inputs[1]
  x.SOPInstanceUID = ''
  x.ImageID = ''
  x.InstanceNumber = ''
  x.SeriesDescription = ''
  x.QueryRetrieveLevel = 'IMAGE'

  -- get list of actual images for patient (can be subset of original slices)
  local images={}
  x.Modality = 'CT'
  local cts = dicomquery('CONQUESTSRV1', 'IMAGE', x, 'InstanceNumber')
  for i=0, #cts-1 do 
    images[cts[i].SOPInstanceUID]=1
  end
    
  -- convert CT images to jpeg
  if inputs[3]=='collateteacher' then
    local sliceinfo = {}
    for k,v in pairs(images) do 
      im = DicomObject:new()
      im:Read(':'..k)
      im:Script([[mkdir ]]..target..inputs[1]..ds..[[CT]])
      im:Script([[save jpg level 0 window 1024 to ]]..target..inputs[1]..ds..[[CT]]..ds..[[%VSOPInstanceUID.jpg]])
      local location = im.SliceLocation or split(im.ImagePositionPatient, '\\')[3]
      table.insert(sliceinfo, 
        {SOPInstanceUID=k, SliceLocation=location, 
	 InstanceNumber=tonumber(im.InstanceNumber) or 0,
	 ImageOrientationPatient=strSplit(im.ImageOrientationPatient),
	 ImagePositionPatient=strSplit(im.ImagePositionPatient)[1],
	 PixelSpacing=strSplit(im.PixelSpacing..'\\0')[1]
	})
      table.sort(sliceinfo, function(a, b) return a.InstanceNumber<b.InstanceNumber end)
    end
    print(target)
    local f = io.open(target..inputs[1]..ds..[[CT]]..ds..[[sliceinfo.json]], 'w')
    f:write(JSON:encode(sliceinfo))
    f:close()
  end
  
  -- name and create folder
  local im = DicomObject:new()
  im:Script([[mkdir ]]..target..inputs[1]..ds..folder)

  -- remove files where contour data is collated and start new ones
  for k,v in pairs(images) do 
    os.remove(target..inputs[1]..ds..folder..ds..'.'..k..'.json')
    local g=io.open(target..inputs[1]..ds..folder..ds..k..'.json', 'w')
    g:write('[')
    g:close()
  end

  -- search all RTSTRUCTS (or limit to one)
  x.Modality = 'RTSTRUCT'
  if inputs[5] then x.ImageID = inputs[5] end

  -- add an independent file listing all delineations: either GS or Observers.
  local listOfOutputs = io.open(target..inputs[1]..ds..folder..ds..[[list.json]],'w');
  listOfOutputs:write('[');
  local datenow = os.date();
  
  local r = dicomquery('CONQUESTSRV1', 'IMAGE', x, 'InstanceNumber')
  for i=0,#r-1 do 
    print(r[i].SeriesDescription, r[i].ImageID)
    if (inputs[3]=='collateteacher' and r[i].SeriesDescription=='Gold standard') or 
       (inputs[3]=='collateteacher' and r[i].SeriesDescription=='Template') or 
       (inputs[3]=='collateobservers' and r[i].SeriesDescription=='Observers') then
      a = tickcount()
      if( r[i].SeriesDescription=='Template' ) then
        r[i].ImageID = 'Template'; -- this is not settled before.
      end

      -- read and dump RTSTRUCT
      im = DicomObject:new()
      im:Read(':'..r[i].SOPInstanceUID)
      im:Dump(target..inputs[1]..ds..folder..ds..r[i].ImageID..'.txt')
      if (r[i].SeriesDescription~='Template') then
        listOfOutputs:write( '{"Date":"' .. datenow ..'","Observer":"' .. r[i].ImageID .. '"},'); -- consider writing the date of upload for observers.
      end
      
      -- create list of structure names
      local strs=toLua(im:GetVR(0x3006,0x0020))
      local structures={}
      for k,v in ipairs(strs) do
        structures[v.ROINumber] = v.ROIName        
      end

      -- json file for observer
      local f=io.open(target..inputs[1]..ds..folder..ds..r[i].ImageID..'.json', 'w')
      f:write('[')

      -- get coordinates and write to json file(s)
      local s0=im:GetVR(0x3006,0x0039)
      for j=0, #s0-1 do
	local l = toLua(s0[j])
	local N = l.ReferencedROINumber  
  
	if (r[i].SeriesDescription=='Template') then -- template doesn't have any conotur data.
		f:write('{')
		  f:write('"Structure":"'..(structures[N] or '')..'",')
		  f:write('"Observer":"'..(r[i].ImageID or '')..'",')
		  f:write('"Session":"'..(inputs[4] or '0')..'",')
		  f:write('"Color":['..table.concat(split(l.ROIDisplayColor, '\\'), ',')..']},')
	end
  
	if l.ContourSequence then
	  for k,v in ipairs(l.ContourSequence or {}) do
	    if images[v.ContourImageSequence[1].ReferencedSOPInstanceUID] then
	      local a=strSplit(v.ContourData)
	      for k, v in ipairs(a) do
	        v[2] = table.concat(v, ',')
	        v[1] = '['
	        v[3] = ']'
	        a[k] = table.concat(v)
	      end

              -- other contour information goes here
	      f:write('{')
	      f:write('"Structure":"'..(structures[N] or '')..'",')
	      f:write('"Observer":"'..(r[i].ImageID or '')..'",')
	      f:write('"Session":"'..(inputs[4] or '0')..'",')
	      f:write('"Slice":"'..v.ContourImageSequence[1].ReferencedSOPInstanceUID..'",')
	      --f:write('"Color":"'..('')..'",')
	      
              -- write contours as array of arrays in json format (extra space for if 0 contours)
	      f:write('"Contours":[ ')
	      f:write('[')		-- json per observer
	      f:write(table.concat(a,','))
	      f:write(']]},')

              -- append to json file per slice if converting all observers
              if inputs[5]==nil then
                local g=io.open(target..inputs[1]..ds..folder..ds..
	          v.ContourImageSequence[1].ReferencedSOPInstanceUID..'.json', 'a')
	        g:write('{')
	        g:write('"Structure":"'..(structures[N] or '')..'",')
	        g:write('"Observer":"'..(r[i].ImageID or '')..'",')
	        g:write('"Session":"'..(inputs[4] or '0')..'",')
	        g:write('"Slice":"'..v.ContourImageSequence[1].ReferencedSOPInstanceUID..'",')
	        --g:write('"Color":"'..('')..'",')
                
                -- write contours as array of arrays in json format (extra space for if 0 contours)
	        g:write('"Contours":[ ')
	        g:write('[')		-- json per slice
	        g:write(table.concat(a,','))
	        g:write(']]},')
                g:close()
	      end
	    end
	  end
	end
      end

      -- remove extra comma and close json per observer
      f:seek("end",-1) 
      f:write(']')
      f:close()

      print(tickcount()-a)
    end
  end
  listOfOutputs:write('{}]')
  listOfOutputs:close()

  -- close json per slice (seek after append has issue, therefore write dummy object)
  for k,v in pairs(images) do 
    local g=io.open(target..inputs[1]..ds..folder..ds..k..'.json', 'a')
    --g:seek("end",-1) 
    g:write('{}]')
    g:close()
  end
  
-- convert observer data after de-anonymisation
elseif inputs[3]~='nil' then
  local s = split(Data["9999,9000"] or 'x.x.x', '%.') --PatientID before de-anonymisation
  Data.StudyDescription = 'Analysis'
  Data.SeriesDescription = 'Observers'
  Data.PatientName=Data.PatientID
  local a, b = findData(Data.PatientID, 'Gold standard')
  Data.SeriesInstanceUID = findData(Data.PatientID, 'Observers')
  Data.StudyInstanceUID = b or genuid()
  Data.SOPInstanceUID = genuid()
  Data.ImageID = inputs[5] or (s[2]..'.'..s[3])
  Data.StructureSetLabel = inputs[5] or (s[2]..'.'..s[3])
end
