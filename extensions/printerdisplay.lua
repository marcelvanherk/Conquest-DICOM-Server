-- adapted to 1.4.19b; added gamma clause to converter

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
