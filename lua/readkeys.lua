-- mvh 20260803	read csv name mapping hospital ID to trial ID; first with item names is ignored
-- usage 
-- [lua]
-- association = dofile('lua/readkeys/lua') k_01=readkeys('keys/earrad.csv')
-- ImportConverter0 = newid = k_01['trial_'..md5(Data.PatientID)]; if newid then Data.PatientID=newid end

require('csv')
function readkeys(filename)
  local res = {}
  local k = readCSV(filename, false)
  for i=2, #k do
    res['trial_'..md5(k[i][1])]=k[i][2]
  end
  return res
end
