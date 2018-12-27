--------------------------------------------------------
-- Support Lua code for conquestdicomserver
-- Marcel van Herk; 20181219
-- Code can be configured by editing this file
--------------------------------------------------------
-- mvh 20181228: note, this code is not yet used, it documents lua code hardcoded in the GUI 

local advancedDisplayConversion = true
local advancedPrintConversion = true

----------------------------
-- installation functions --
----------------------------

-- update UIDmods table for version 1.4.19 up
function updateUIDmods()
  local a=dbquery("UIDMODS", "Stage", "Stage=1")
  if a==nil then 
    sql("ALTER TABLE UIDMODS ADD COLUMN Stage varchar(32)")
    sql("ALTER TABLE UIDMODS ADD COLUMN Annotation varchar(64)")
    print("updated UIDMODS table"); 
  end
end

-----------------------
-- browser functions --
-----------------------

-- create bitmap file for display in browser
function createBitmapForDisplay(infile, outfile, frame)
  if advancedDisplayConversion then
    local x=DicomObject:new()
    x:Read(infile)
    local c = (x.WindowCenter or 0)     if c=='' then c=0 end
    local w = (x.WindowWidth or  0)     if w=='' then w=0 end
    local s = (x.RescaleSlope or 1)     if s=='' then s=1 end
    local i = (x.RescaleIntercept or 0) if i=='' then i=0 end

    if string.find(c, '%\\') then
      c = string.match(c, '(.-)\\')
    end
    if string.find(w, '%\\') then
      w = string.match(w, '(.-)\\')
    end
    w = tonumber(w)/tonumber(s)
    c = tonumber(c)/tonumber(s)

    x:Script('save bmp level '..math.floor(c)..' window '..(math.ceil(w))..' size 1600 frame '..frame..' to '..outfile)
  else
    ServerTask('', 'convert_to_bmp:'..infile..',1600,'..outfile..',//'..frame);
  end
end

-- create header dump 
function headerDump(infile, outfile)
  local x=DicomObject:new()
  x:Read(infile)
  x:Dump(outfile)
end

-- move item by uid identifiers; identifiers is patid to patid,uid,uid,uid
function moveItem(ae, target, identifiers)
  servercommand(move:'..ae..','..target..','..identifiers);
end

-- zip item
function ZipItem(item, filename)
  servercommand('export:'..item..','..filename..','..'compression UN')
end

-- anonymize and zip 
function anonymizeAndZipItem(item, newid, filename)
  servercommand('export:'..item..','..filename..','..
    'compression UN;lua/anonymize_script.lua('..newid..')')
end

-- change patient ID
function changePatientIDForItem(item, newid)
  servercommand('modifier:'..item..',lua:script("newuids");Data.PatientID="'..newid..'"')
end  

-- anonymize
function anonymizeItem(item, newid)
  servercommand('modifier:'..item..',lua/anonymize_script.lua('..newid..')')
end  

-- modifying by generic script
function modifyItem(item, script)
  servercommand('modifier:'..item..','..script)
end  

-- merging
function mergeItem(what, filenames)
  servercommand(merge'..what:' .. filenames);
end

-- delete patient, study, series or image (what), item is PatientID of PatientId:uid
function deleteItem(what, item)
  if (item or '')~='' then
    servercommand('delete'..what+':'..item)
  end
end

-------------------------------
-- query/move page functions --
-------------------------------

-- query for query/move page, pass Lua table as query
-- returns filename of file with formatted results
function queryForGui(ae, level, q, writeheader)
  local q2=DicomObject:new()
  for k,v in pairs(q) do q2[k]=v end
  local r2=dicomquery(ae, level, q2)
  local s=tempfile("txt") 
  f=io.open(s, "wb")
  if r2==nil then 
    f:write("no connection with "..ae.."\n") f:close() return s 
  end
  local r = loadstring("return "..r2:Serialize())()
  r[1].QueryRetrieveLevel=nil; r[1].TransferSyntaxUID=nil
  local keys={} 
  for k,v in pairs(r[1]) do 
    keys[#keys+1]=k 
  end
  table.sort(keys, 
    function(a, b) return string.sub(a, 1, 7)<string.sub(b, 1, 7) end
  )
  if writeheader then 
    for k,v in ipairs(keys) do f:write(v.."    ") end f:write("\n") end
    f:write("---------------------------------------------------------------------------------------------------------------------------------------------------------------\n")
  end
  for k,v in ipairs(r) do
    for k2,v2 in ipairs(keys) do 
      f:write("["..v[v2].."]    ") 
    end f:write("\n")
  end
  f:close() 
  return s
end

-- find missing data for query/move page, returns name of file with information
function findmissingdata(localae, remoteae, q)
  local q2=DicomObject:new()
  for k,v in pairs(q) do q2[k]=v end
  local s=tempfile("txt") 
  f=io.open(s, "wb")
  local r1 = dicomquery(localae, "IMAGE", q2)
  if r1==nil then 
    f:write("\nno connection with "..localae.."\n") f:close() return s
  end
  local r2 = dicomquery(remoteae, "IMAGE", q2)
  if r2==nil then 
    f:write("\nno connection with "..remoteae.."\n") f:close() return s
  end
  
  local sops = {}
  for i=0, #r1-1 do sops[r1[i].SOPInstanceUID]=r1[i].PatientID end
  for i=0, #r2-1 do sops[r2[i].SOPInstanceUID]=nil end
  
  local pats = {}; 
  for k,v in pairs(sops) do 
    pats[v] = (pats[v] or 0) + 1 
  end

  for k,v in pairs(pats) do f:write(k .. ",") end; f:write("\n")
  f:write("images in "..localae .. " = " .. #r1 .. "\n")
  f:write("images in "..remoteae .. " = " .. #r2 .. "\n")
  local total=0
  for k,v in pairs(pats) do 
    total=total+v; f:write(k .. " misses " .. v .. " images\n") 
  end
  f:write("total missing images = " .. total .. "\n")
  f:close()
  return s 
end

-- move by query, returns error code if any
function moveByQuery(ae, target, q, jobno)
  local q2=DicomObject:new()
  for k,v in pairs(q) do q2[k]=v end
  return dicommove(ae, target, q2, 0, jobno)
end

------------------------
-- printing functions --
------------------------

-- create bitmap file for print server
function createBitmapForPrinting(infile, outfile)
  if advancedPrintConversion then
    local x=DicomObject:new()
    x:Read(infile)

    local c = (x.WindowCenter or 0)     if c=='' then c=0 end
    local w = (x.WindowWidth or 0)      if w=='' then w=0 end
    local s = (x.RescaleSlope or 1)     if s=='' then s=1 end
    local i = (x.RescaleIntercept or 0) if i=='' then i=0 end
    frame = frame or 0

    if string.find(c, '%\\') then
      c = string.match(c, '(.-)\\')
    end

    if string.find(w, '%\\') then
      w = string.match(w, '(.-)\\')
    end

    w = tonumber(w)/tonumber(s)
    c = tonumber(c)/tonumber(s)

    x:Script('save bmp level '..math.floor(c)..' window '..(math.ceil(w/2))..' size 9600 gamma 1 frame '..frame..' to '..outfile)
  else
    servercommand('', 'convert_to_bmp:'..infile..',9600,'..outfile)
  end
end

-- print one image, return error string
function printOneImage(filename, target)
  local image=DicomObject:new()
  image:Read(file)
  image:Compress("un")
  local r=dicomprint(image, target)
  return r
end

-- print array of images (filenames separated by ;), return error string
function printArrayOfImages(filenames, target, formatstring)
  local files = {}
  filenames:gsub("([^;]*);", function(c) table.insert(files, c) end)
  addo = DicomObject:newarray()
  for k, v in ipairs(files) do
    local o = DicomObject:new() 
    o:Read(v) 
    o:Compress("un")
    addo:Add(o)
  end
  local r=dicomprint(addo, target, "", formatstring, "99999");' +
  addo=nil
  collectgarbage()
  return r
end
