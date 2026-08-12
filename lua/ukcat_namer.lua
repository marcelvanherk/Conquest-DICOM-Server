-- 20260119: added date_perturbation, loaded in to 9999,1235
-- 20260803: responds to MoveOriginatorMessageID==0xcafe to forward data for clinical trials with hashed patient ID
-- 20260806: Also responds to MessageID==0xcafe

-- test setup for debugging
--  Data["9999,1234"]='test'
--  Data["9999,1235"]='-100'
--  if true then return end

if Command.MoveOriginatorMessageID=='51966' or Command.MessageID=='51966' then 
  Data["9999,cafe"]='trial'
  local patid = string.gsub(Data.PatientID, 'RBV', '')
  Data["9999,1234"]='trial_'..md5(patid) 
  return
end
-- add  memo={} to association event in dicom.ini
memo = memo or {}
if memo.ukcatID and memo.christieID and Data.PatientID==memo.christieID then
  Data["9999,1234"]=memo.ukcatID
  Data["9999,1235"]=memo.datePerturbation
  return
end
local patid = string.gsub(Data.PatientID, 'RBV', '')
local l = require("luasql_postgres")
local env = l.postgres()
local con = env:connect("key_db", "key_admin", "**********************")
local cur = con:execute("SELECT patient_id, date_perturbation from key_value where key_value = '"..patid.."'")
local row = cur:fetch ({}, "a") -- the rows will be indexed by field names
if not row then
  cur:close()
  con:close()
  env:close()
  print('*** No UKCAT ID found for '..Data.PatientID)
  reject()
  return
else
  memo.ukcatID           = tostring(row.patient_id)
  memo.datePerturbation  = tostring(row.date_perturbation)
  Data["9999,1234"]      = memo.ukcatID
  Data["9999,1235"]      = memo.datePerturbation
  memo.christieID  = Data.PatientID
end
cur:close()
con:close()
env:close()
