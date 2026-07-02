--20260115	mvh	UKCAT version of anonymise_script.lua that also processes Elekta zip file in 0021,103a
--20260119	mvh	Fixed \n.-Date= matching: matched multiple names, replaced by \n%a-Date=.-\n
--20260122	mvh	Default dataoffset must be ''

local scriptversion = "1.5; date 20260122"

local DirSep      = '/'
if string.find(Global.BaseDir, '\\') then DirSep = '\\' end

require('anonymizer')

local config=dofile(Global.BaseDir..'lua/ukcat.cfg')
local newid, newname
local dateoffset = ''

if Data["9999,1234"] then
  newid = Data["9999,1234"]
  newname = Data["9999,1234"]
  Data["9999,1234"]=nil
else
  newid = CRC32(Data.PatientID)..'.'..CRC32(Data.PatientBirthDate or '')
  newname = 'PAT'..CRC32(Data.PatientID)
end

if Data["9999,1235"] then
  dateoffset = Data["9999,1235"]
  Data["9999,1235"]=nil
end

-- process Elekta XVI zip file
local content
if Data.Modality=='REG' then 
  s= Data:GetVR(0x0021,0x103a, true)
  if s and s~='' then
    local inflate=require('lua/inflate')
    local stream = inflate.new(s)
    for name, offset, size, packed, crc in stream:files() do 
      if string.find(name, '.XVI') then
        if packed then 
          content = stream:inflate(offset, crc)
        else          
          content = stream:extract(offset, size) 
        end
      
        content = string.gsub(content, '\r\n', '\n')
        content = string.gsub(content, '\n\r', '\n')
        content = string.gsub(content, '\n%a-Date=.-\n', '\n')
        content = string.gsub(content, '\n%a-DateTime=.-\n', '\n')
        content = string.gsub(content, '\n%[REFERENCE%..-%]', '\n[REFERENCE_Date_]')
        content = string.gsub(content, '\n%[ALIGNMENT%..-%]', '\n[ALIGNMENT_Date_]')
      end
    end
  end
end

stage = '#ukcat'
local logfile2 = config.logroot .. Data.PatientID .. '\\' .. config.logname(Data, stage, Data.PatientID)
print("Anomymisation stage              : ", 'ukcat', "\n") 
anonymize(config, newid, newname, stage, dateoffset)

if content then
  print(config.logroot .. logfile2)
  local f = io.open(logfile2, "a")
  f:write('Anonymised XVI data\n')
  f:write(content)
  f:write('\n\n\n')
  f:close()
  Data:SetVR(0x0021,0x103a, content)
end

-- for testing only
--Data:Dump(string.sub(logfile2, 1, -5)..'_'..crc(Data.SOPInstanceUID)..'.dmp')
--Data:Write(string.sub(logfile2, 1, -5)..'_'..crc(Data.SOPInstanceUID)..'.dcm')
