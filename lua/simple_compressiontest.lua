-- for 1.5.0-alpha; use new tickcount function, and repeat a few times

images = {'ok', 't1', 't2', 't3'}

testcompression = {
  'jk jpeg2000 LL', 
  'js jpegLS   LL', 
  'n4 NKI      LL', 
  'j2 jpeg     LL', 
  'j3 jpeg     LS', 
  'j7 jpegLS   LS', 
  'jl jpeg2000 LS',
  'j710 jpegLS LS'
  }

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

for i,v in ipairs(images) do
	testimage = [[e:\software\dicomserver\problem images\]]..v..[[.dcm]]
  print(v..'.dcm', filesize(testimage)/1024, 'kb')
  for k,compression in ipairs(testcompression) do
    d = DicomObject:new()
    d:Read(testimage)
    d1 = d:GetImage(0)
    local t = tickcount()
    local elapsed = 0
    local count=0
    while elapsed < 300 do
      d:Script('compression '..compression)
    -- d:Write(testimage..compression..'.dcm')
      d:Script('compression '..'un')
      elapsed = tickcount()-t
      count=count+1
    end
    e1 = d:GetImage(0)
    print('compression', compression,'lossless = ', d1==e1, 'time', string.format("%.1f", elapsed/count), 'ratio', filesize(testimage..compression..'.dcm')/filesize(testimage))
  end
end