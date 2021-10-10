-- GUI indepedent printer code, mvh 20210820
-- Integrated as follows in dicom.ini
--
-- [lua]
-- association = local canvas, canvas_w, canvas_h
-- printserver = dofile('lua/print.lua')

print(command_line)
require"cdlua"
require"imlua"
require"cdluaim"

if command_line=='createbasicfilmbox' then
  canvas = cd.CreateCanvas(cd.PRINTER, "test5")
  canvas_w, canvas_h = canvas:GetSize() 
  print(canvas_w, canvas_h)
end

if string.find(command_line, '%.dcm') then
  local c = (Data.WindowCenter or 0)     if c=='' then c=0 end
  local w = (Data.WindowWidth or 0)      if w=='' then w=0 end
  local s = (Data.RescaleSlope or 1)     if s=='' then s=1 end
  local i = (Data.RescaleIntercept or 0) if i=='' then i=0 end
  
  if string.find(c, '%\\') then
    c = string.match(c, '(.-)\\')
  end

  if string.find(w, '%\\') then
    w = string.match(w, '(.-)\\')
  end

  w = tonumber(w)/tonumber(s)
  c = tonumber(c)/tonumber(s)
  
  local vr = Data:GetVR(0x2020, 0x0110)
  vr[0]:Script('save jpg level '..math.floor(c)..' window '..(math.ceil(w/2)) .. ' to x.jpg')
  local image = im.FileImageLoad('x.jpg')
  local iw = image:Width()
  local ih = image:Height()
  
  local rows, cols, num = command_line:match('.*%.(.-)%.(.-)%.(.-)%.(.-)$')
  rows = tonumber(rows)
  cols = tonumber(cols)
  num = tonumber(num)
  local margin=100
  
  local row = num % rows
  local col = math.floor(num/rows)
  local sizex = (canvas_w-2*margin)/rows
  local sizey = (canvas_h-2*margin)/cols
  local posx = margin + row*sizex
  local posy = (canvas_h-sizey) - (margin + col*sizey)
  
  if iw>ih then sizey = sizex*ih/iw
  else sizex = sizey*iw/ih end
  
  image:cdCanvasPutImageRect(canvas, posx, posy, sizex, sizey, 0,0,0,0)
  canvas:Text(posx, posy-40, num) 
end

if command_line=='printbasicfilmbox' then
  canvas:Kill()
end

