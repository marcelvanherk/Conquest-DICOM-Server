--[[
 (c) 2017, Vladimir Agafonkin
 Simplify.js, a high-performance JS polyline simplification library
 mourner.github.io/simplify-js
 Translation into lua by mvh
]]

-- to suit your point format, run search/replace for '.x' and '.y';
-- for 3D version, see 3d branch (configurability would draw significant performance overhead)

-- square distance between 2 points
function getSqDist(p1, p2)

    local dx = p1.x - p2.x
    local dy = p1.y - p2.y

    return dx * dx + dy * dy
end

-- square distance from a point to a segment
function getSqSegDist(p, p1, p2)

    local x = p1.x
    local y = p1.y
    local dx = p2.x - x
    local dy = p2.y - y

    if dx ~= 0 or dy ~= 0 then
        local t = ((p.x - x) * dx + (p.y - y) * dy) / (dx * dx + dy * dy)
        if t > 1 then
            x = p2.x
            y = p2.y
        elseif t > 0 then
            x = x + dx * t
            y = y + dy * t
        end
    end

    dx = p.x - x
    dy = p.y - y
    return dx * dx + dy * dy
end

-- rest of the code doesn't care about point format

local tableinsert = table.insert

-- basic distance-based simplification
function simplifyRadialDist(points, sqTolerance)
    local prevPoint = points[1]
    local newPoints = {prevPoint}
    local point

    for i = 2, #points do
        point = points[i]

        if getSqDist(point, prevPoint) > sqTolerance then
            tableinsert(newPoints, point)
            prevPoint = point
        end
    end

    if getSqDist(prevPoint,point)~=0 then 
        tableinsert(newPoints, point)
    end

    return newPoints
end

function simplifyDPStep(points, first, last, sqTolerance, simplified)
    local maxSqDist = sqTolerance
    local index

    for i = first + 1, last do
        local sqDist = getSqSegDist(points[i], points[first], points[last])

        if sqDist > maxSqDist then
            index = i;
            maxSqDist = sqDist;
        end
    end

    if maxSqDist > sqTolerance then
        if index - first > 1 then simplifyDPStep(points, first, index, sqTolerance, simplified) end
        tableinsert(simplified, points[index])
        if last - index > 1 then simplifyDPStep(points, index, last, sqTolerance, simplified) end
    end
end

-- simplification using Ramer-Douglas-Peucker algorithm
function simplifyDouglasPeucker(points, sqTolerance) 
    local last = #points;

    local simplified = {points[1]}
    simplifyDPStep(points, 1, last, sqTolerance, simplified)
    tableinsert(simplified, points[last])

    return simplified
end

-- both algorithms combined for awesome performance
function simplify(points, tolerance) 

    if #points <= 2 then return points end

    local sqTolerance = (tolerance or 1) * (tolerance or 1)

    local pnts = simplifyRadialDist(points, sqTolerance)
    pnts = simplifyDouglasPeucker(points, sqTolerance)

    return pnts
end

function split(s)
    result = {}
    for match in (s..'\\'):gmatch("(.-)\\") do
      tableinsert(result, match)
    end
    return result
end

function toPoints(s)
  local data = split(s)
  local points = {}
  for i=1, #data, 3 do
    point = {x=data[i], y=data[i+1], z=data[i+2]}
    tableinsert(points, point)
  end
  return points
end

function toContourData(points)
  local cnt={}
  for i=1, #points do
    cnt[i]=string.format("%.1f\\%.1f\\%s", points[i].x, points[i].y, points[i].z)
  end
  return table.concat(cnt, '\\')
end

-- test

local d = DicomObject:new()
--d:Read(":1.2.826.0.1.3680043.8.691.4.311900395635779.1171.1450365426175.8") --ROI
--d:Read(":1.2.826.0.1.3680043.2.135.737032.51197242.7.1557329997.662.30") --MRL1
d:Read(":1.2.826.0.1.3680043.2.135.737059.58899160.7.1563455506.468.34" ) -- ATP2019 breast
local st=os.time()
local total1, total2, total3, total4=0,0,0,0
local s1=d:GetVR(0x3006, 0x0039)
for i=0, #s1-1 do
  local s2 = s1[i]:GetVR(0x3006, 0x0040)
  for j=0, #s2-1 do
    local cnt = s2[j]:GetVR(0x3006, 0x0050, true)
    local points = toPoints(cnt)
    local simple = simplify(points, 0.5)
    local s = toContourData(simple)
    s2[j].NumberOfContourPoints = #simple
    s2[j]:SetVR(0x3006, 0x0050, s)
    print(#cnt, #points, #s, #simple)
    total1=total1+#cnt
    total2=total2+#points
    total3=total3+#s
    total4=total4+#simple
  end
  --s1[i]:SetVR(0x3006, 0x0040, s2)
end
print('-------------------------')
print(total1, total2, total3, total4)
print(os.time()-st)
--d:SetVR(0x3006, 0x0039, s1)
