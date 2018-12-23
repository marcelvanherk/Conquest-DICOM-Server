-- adapted to 1.4.19

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

-- c = (tonumber(c) - tonumber(i))/tonumber(s)

x:Script('save bmp level '..math.floor(c)..' window '..(math.ceil(w))..' size 1600 frame '..frame..' to '..outfile)
