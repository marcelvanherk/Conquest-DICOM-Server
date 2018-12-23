--[[
#Basic Nifty/DICOM functions
for [Conquest](http://ingenium.home.xs4all.nl/dicom.html) dicom server, version 1.4.17beta or higher expected

 function `write_nifty(imagefiles, filenames, replicateslices)`
   saves a DICOM series in nifty1.1 s-form format
   `imagefiles` is a table of strings (dicom filenames), must be in correct order
   if it is a table of tables, only [1] or .name is used to get the filename
   `filenames` is the name of the output nifty1.1 file or a table of output files 
   to create nifty files of individual slices, which may be replicated if 
   parameter `replicateslices` is passed and set to >1

 function `reread_nifty(imagefiles, filename, usecoords, replacevalues)`
   take a nifty1.1 file and puts its pixel value into new dicom series
   `imagefiles` is a table of strings (filenames) or tables of the original dicom data
   `filename` is the name of the input nifty1.1 file (must be same dimensions as dicom data)
   `usecoords` (optional) if set, nifty s-form changes ImagePosition/OrientationPatient
   `replacevalues` is an (optional) table with replacement dicom tags, e.g., {SeriesDescription = 'ReOriented'}

 function `append_nifty(to, from)`
   Appends nifty file `from` to nifty file `to`, making `to` 4D if needed.
   This routine assumes `to` and `from` have the same dimensions, type etc.
   `append_nifty` is not DICOM server specific, it may be run from any lua interpreter
 
 function `slice_nifty(from, to, slice)`
   Slices timeframe `slice` out of nifty file `from` to nifty file `to`.
   `slice_nifty` is not DICOM server specific, it may be run from any lua interpreter

 function `reread_interfile(imagefiles, headerfilename, replacevalues, shift, scale)`
   take a SIRT interfile file and puts its pixel value into new dicom series
   `imagefiles` is a table of strings (filenames) or tables of the original dicom data
   `filename` is the name of the input SIRT interfile header file. Coordinates and 
   dimensions are taken from the interfile. `replacevalues` is an (optional) table 
   with replacement dicom tags, e.g., {SeriesDescription = 'ReOriented'}.
   `shift` is the optional magic z-axis shift (must be 136?), `scale` is used if the
   interfile data is float, prior to conversion to short. Note 1: requires conquest
   1.4.17a to be able to change image dimensions. Note 2: need to pass 
   AcquisitionTime, SeriesTime, FrameReferenceTime, ActualFrameDuration,
   RescaleIntercept, RescaleSlope in `replacevalues` to make a valid PET
]]   

-- 20130621  mvh  Fix for linux
-- 20130628  mvh  Fix to allow reading of 4D DICOM data; 
--                accept table as filenames for reread_nifty
--                Fix for reading and wrting DICOM series that are 4D volumes
--                Reject series without SliceLocation and color series
-- 20130701  mvh  imagefiles may be table of string or table of tables with t[1] or t.name
--                Fix ndim and added slice_nifty
--                Todo: support multiframe object
-- 20130702  mvh  Fix matrices in 4D data 
-- 20130703  mvh  Fixed dimz for replicateslices, extended error messages
-- 20130708  mvh  Reject invalid 4D data
-- 20130709  mvh  Added reread_interfile to convert SIRT recons back to dicom
-- 20130711  mvh  Made it to work - required conquest 1.4.17a
-- 20130813  mvh  Use DicomObject:addimage from 1.4.17b
-- 20150507  mvh  Implemented REG object - ABAS specific for now; write_nifty returns info table
-- 20150927  mvh  Support images without SliceLocation
-- 20160710  mvh  Added write_nrrd and sample test code
-- 20161121  mvh  Merged REG stuff
-- 20161204  afg  Merge two versions of this file

require('niftyutil')

function write_nifty(imagefiles, filenames, replicateslices)
  require "pack"
  
  function image(i)
    local t=imagefiles[i]
    if type(t)=='table' then
      return t[1] or t.name
    else
      return t
    end
  end
  
--  if #imagefiles<2 then
--    error('*** write_nifty: requires at least 2 images ')
--    return
--  end
  
  replicateslices = replicateslices or 1

  local headerkey = string.pack('iAAihAA', 
    348,                        -- sizeof header
    string.rep('\000', 10),     -- data_type
    string.rep('\000', 18),     -- db_name
    16384,                      -- extents
    0,				-- session_error
    'r',			-- regular
    '\000'			-- hkey_un0
    )
   
  -- need two images to compute Z direction from ImagePositionPatient, 3rd image to check on 4D or inconsistent data
  local i1,i2,i3 = DicomObject:new(),DicomObject:new(),DicomObject:new(); i1:Read(image(1))
  if #imagefiles>1 then
    i2:Read(image(2))
    i3:Read(image(#imagefiles))
  end
  
  if (i1.SliceLocation or '') == '' then
    i1.SliceLocation = split(i1.ImagePositionPatient or '', '\\')[3]
    if #imagefiles>1 then
      i2.SliceLocation = split(i2.ImagePositionPatient or '', '\\')[3]
      i3.SliceLocation = split(i3.ImagePositionPatient or '', '\\')[3]
    end
  end
  
  if i1.Modality~='REG' then
    if (i1.SliceLocation or '') == '' then
      print('*** write_nifty does not support images without slice location')
      return
    end
    
    vl = (i1.SamplesPerPixel or '1') + 0
    if vl~=1 then
      print('*** write_nifty does not support color images')
      return
    end
  end
  
  local slicespacing
  
  if i1.Modality=='REG' then
    local x8 = i1.DeformableRegistrationSequence[0].DeformableRegistrationGridSequence[0]:GetVR(0x64, 0x8)
    i1.PixelSpacing = string.format('%f\\%f', x8[1], x8[2])
    slicespacing = x8[3]
  else
    slicespacing = ((i3.SliceLocation+0) - (i1.SliceLocation+0)) / (#imagefiles-1)
  end
  
  local ps = i1.PixelSpacing or '1\\1'
  local p1 = string.gsub(ps, '.*\\', '') + 0
  local p2 = string.gsub(ps, '\\.*', '') + 0
  
  local ndim=3
  local dimz
  if i1.Modality=='REG' then
    local x7 = i1.DeformableRegistrationSequence[0].DeformableRegistrationGridSequence[0]:GetVR(0x64, 0x7)
    dimz = x7[3]
    i1.Rows = x7[2]
    i1.Columns = x7[1]
    i1.ImagePositionPatient = i1.DeformableRegistrationSequence[0].DeformableRegistrationGridSequence[0].ImagePositionPatient
    i1.ImageOrientationPatient = i1.DeformableRegistrationSequence[0].DeformableRegistrationGridSequence[0].ImageOrientationPatient
  else
    dimz=#imagefiles
  end
  local dimt=1
  local info = {}
  
  -- is there a slicing inconsistency or is the data is 4D ?
  if i1.Modality~='REG' then
    if math.abs((i2.SliceLocation+0) - (i1.SliceLocation+0) - slicespacing) > 0.01 then
      -- compute #timeframes from the slicelocation range
      local lastfirst = (i3.SliceLocation+0) - (i1.SliceLocation+0)
      local scndfirst = (i2.SliceLocation+0) - (i1.SliceLocation+0)
      local dimt2 = #imagefiles / ((lastfirst / scndfirst) + 1)
      
      -- more than one --> 4D
      if math.floor(dimt2)+0.5 > 1 then
        dimt = math.floor(#imagefiles / ((lastfirst / scndfirst) + 1) + 0.5)
        dimz = math.floor(dimz/dimt + 0.5)
        print('detected 4D data', dimt, dimz, ((lastfirst / scndfirst) + 1))
        if dimt*dimz ~= #imagefiles then
          print('*** rejected  4D data, because there are ', #imagefiles, ' slices while the dimensions require ', dimt*dimz, 'slices')
          dimz=#imagefiles
          dimt=1
          return
        else
          ndim = 4
          slicespacing = ((i3.SliceLocation+0) - (i1.SliceLocation+0)) / (dimz-1)
        end
      else
        print('*** Slice spacing inconsistent, is this a localizer?')
        print('*** There are', #imagefiles, 'slices and the last slice number is ', i3.InstanceNumber)
      end
    end
  end
  
  local mz = 1
  if slicespacing < 0 then
    mz = -1
    slicespacing = -slicespacing
  end
  
  local tdimz=dimz
  if type(filenames)=='table' then 
    tdimz=replicateslices
  end
  
  local typ, bitpix=4, 16
  if i1.Modality=='REG' then typ, bitpix, vl, ndim = 16, 32, 3, 5 end

  local imagedimensions = string.pack(
    'hhhhhhhh hhhhhhh hhh ffffffff ffffffff ii',
    ndim, i1.Columns, i1.Rows, tdimz, dimt, vl, 0, 0, -- dim
    0, 0, 0, 0, 0, 0, 0,                              -- unused
    typ, bitpix, 0,                                   -- short, 16 bits, unused
    mz, p1, p2, slicespacing, 0, 0, 0, 0,             -- pixdim
    352, 0, 0, 0, 0, 0, 0, 0,                         -- voxoffset
    0, 0)
  
  -- calculate that which is the same for each slice (if writing loose slices)
  local t
  t = split(i1.ImagePositionPatient, '\\')
  local xs1, ys1, zs1 = unpack(t);
  t = split(i1.ImageOrientationPatient, '\\')
  local rx1, rx2, rx3, ry1, ry2, ry3 = unpack(t);
  local rz1, rz2, rz3
  
  info.SeriesInstanceUID = i1.SeriesInstanceUID

  if i1.Modality~='REG' then
    t = split(i3.ImagePositionPatient, '\\')
    local xs2, ys2, zs2 = unpack(t);
    rz1 = (xs2-xs1) / (dimz-1)
    rz2 = (ys2-ys1) / (dimz-1)
    rz3 = (zs2-zs1) / (dimz-1)
  else
    rz1 = 0
    rz2 = 0
    rz3 = slicespacing
    info.ReferencedSeries1 = i1.ReferencedSeriesSequence[0].SeriesInstanceUID
    info.ReferencedSeries2 = i1.ReferencedSeriesSequence[1].SeriesInstanceUID
  end

  if type(filenames)~='table' then
    filenames = {filenames}
  end
    
  collectgarbage()
  local f
  for i=1, #imagefiles do        -- write pixel values or nifty image of all images
    if i1.Modality~='REG' then i1:Read(image(i)) end
    
    if i <= #filenames then
      xs1, ys1, zs1 = unpack(split(i1.ImagePositionPatient, '\\'));
      local df = (replicateslices-1)/2
      local sx={p1*rx1, p2*ry1, rz1, xs1 - df*rz1}
      local sy={p1*rx2, p2*ry2, rz2, ys1 - df*rz2}
      local sz={p1*rx3, p2*ry3, rz3, zs1 - df*rz3}
      local nifty_extensions= string.pack(
        'AA hh ffffff ffff ffff ffff A A',
        'hoi '..string.rep('\000', 76),	     -- descrip
        string.rep('\000', 24),	     -- aux_file
        0, 1,			     -- qform_code, sform_code
        0, 0, 0, 0, 0, 0,	     -- quaternion (unused)
        -sx[1],-sx[2],-sx[3],-sx[4], -- srow_x
        -sy[1],-sy[2],-sy[3],-sy[4], -- srow_y
         sz[1], sz[2], sz[3], sz[4], -- srow_z
        string.rep('\000', 16),	     -- intent_name
        'n+1\000'		     -- magic (nifty 1.1, embedded data)
        )
      f = io.open(filenames[i], 'wb')
      if f==nil then
        error('nifty_write: cannot write file '..filenames[i])
        return
      end
      f:write(headerkey)
      f:write(imagedimensions)
      f:write(nifty_extensions)
      f:write(string.rep('\000', 4)) -- empty minimal extension
    end

    if i1.Modality~='REG' then
      local y=i1:GetImage(0)
      for j=1, replicateslices do
        f:write(y)
      end
    else
--      Alternative code to using reshape below; is very slow but allows coordinate system fixup here
--      local a=i1.DeformableRegistrationSequence[0].DeformableRegistrationGridSequence[0]:GetVR(0x64, 0x9, false)
--      local b = {}
--      local cnt = i1.Rows*i1.Columns*dimz
--      local off = 0*cnt+1
--      for i=0, cnt-1 do
--        b[i+off] = -a[i*3+1]
--      end
--      collectgarbage()
--      local off = 1*cnt+1
--      for i=0, cnt-1 do
--        b[i+off] = -a[i*3+2]
--      end
--      collectgarbage()
--      local off = 2*cnt+1
--      for i=0, cnt-1 do
--        b[i+off] = a[i*3+3]
--      end
--      collectgarbage()
--      i1.DeformableRegistrationSequence[0].DeformableRegistrationGridSequence[0]:SetVR(0x64, 0x9, b)
      
      local a=i1.DeformableRegistrationSequence[0].DeformableRegistrationGridSequence[0]:GetVR(0x64, 0x9, true)
      local b = a:reshape(4, 3)
      i1.DeformableRegistrationSequence[0].DeformableRegistrationGridSequence[0]:SetVR(0x64, 0x9, b)
      -- after reshape needs fixing up where x and y are negated; below code sits in extrapolate.lua
        -- fix coordinate system issue between DICOM and Nifty: X and Y need a minus, not Z
        --f = field:new()
        --AVS:FIELD_EXTRACT_SCALAR(deform.Data, f, 2)
        --AVS:FIELD_MULC(deform.Data, deform.Data, -1)
        --AVS:FIELD_INSERT_SCALAR(deform.Data, f, deform.Data, 2)

      local y=i1.DeformableRegistrationSequence[0].DeformableRegistrationGridSequence[0]:GetVR(0x64, 0x9, true)
      for j=1, replicateslices do
        f:write(y)
      end
    end
      
    if #filenames>1 or i==#imagefiles then
      f:close()
    end
  end
  
  return info
end

function reread_nifty(imagefiles, filename, usecoords, replacevalues)
  require "pack"
  
  if imagefiles==nil then
    error('cannot convert nifti to DICOM without a template')
  end

  function image(i)
    local t=imagefiles[i]
    if type(t)=='table' then
      return t[1] or t.name
    else
      return t
    end
  end
  
  local f = io.open(filename, 'rb')
  local suid = genuid()
  local header, nxt, dim0, dim1, dim2, dim3, dim4;
  
  usecoords = usecoords or false
  replacevalues = replacevalues or {}
  
  if f==nil then
    error('pixelread_nifty: cannot read file '..filename)
    return
  end
  
  local i1 = DicomObject:new(); 
  i1:Read(image(1))
  local ps = i1.PixelSpacing
  local p1 = string.gsub(ps, '.*\\', '') + 0
  local p2 = string.gsub(ps, '\\.*', '') + 0

  header = f:read(352)     -- read some parts of nifty header only
  nxt = string.unpack(header, 'iA10A18ihA1A1')
  nxt, dim0, dim1, dim2, dim3, dim4 = string.unpack(header, 'hhhhhhhh hhhhhhh hhh ffffffff ffffffff ii', nxt);
  local nex,dsc,aux,q,s,q1,q2,q3,q4,q5,q6,sx1,sx2,sx3,sx4,sy1,sy2,sy3,sy4,sz1,sz2,sz3,sz4
       = string.unpack(header, 'A80A24 hh ffffff ffff ffff ffff A16 A4', nxt);
  local rx={-sx1/p1, -sy1/p1, sz1/p1}
  local ry={-sx2/p2, -sy2/p2, sz2/p2}
  
  local k,v
 
  for i=1, #imagefiles do
    i1:Read(image(i))
    i1.SeriesInstanceUID = suid
    i1.SOPInstanceUID = genuid()
    for k,v in pairs(replacevalues) do
      i1[k] = v
    end
    i1:SetImage(0, f:read(i1.Rows*i1.Columns*(i1.BitsAllocated/8)))
    if usecoords then
      local z = math.fmod(i, dim4)
      i1.ImagePositionPatient    = string.format('%.8g\\%.8g\\%.8g', -sx4-(z-1)*sx3, -sy4-(z-1)*sy3, sz4+(z-1)*sz3)
      i1.ImageOrientationPatient = string.format('%.8g\\%.8g\\%.8g\\%.8g\\%.8g\\%.8g', rx[1], rx[2], rx[3], ry[1], ry[2], ry[3])
    end
    i1:AddImage()
  end
  
  f:close()
end

function append_nifty(to, from)
  require "pack"

  function pack(...)
    return arg
  end
  
  local f = io.open(from, 'rb')
  if f==nil then
    error('append_nifty: cannot read file '..from)
  end
  local g = io.open(to, 'r+b')
  if g==nil then
    error('append_nifty: cannot update file '..to)
  end
  
  local header = g:read(352) -- read some parts of nifty header only
  local headerkey        = pack(string.unpack(header, 'iA10A18ihA1A1'))
  local imagedimensions  = pack(string.unpack(header, 'hhhhhhhh hhhhhhh hhh ffffffff ffffffff ii', headerkey[1]));
  local nifty_extensions = pack(string.unpack(header, 'A80A24 hh ffffff ffff ffff ffff A16 A4', imagedimensions[1]))
  table.remove(headerkey,        1)
  table.remove(imagedimensions,  1)
  table.remove(nifty_extensions, 1)
  
  imagedimensions[1] = 4
  if imagedimensions[5]==0 then
    imagedimensions[5] = 2
  else
    imagedimensions[5] = imagedimensions[5]+1
  end
  
  f:read(352)
  local data = f:read('*all')
  
  g:seek('set', 0)
  g:write(string.pack('iAAihAA', unpack(headerkey)))
  g:write(string.pack('hhhhhhhh hhhhhhh hhh ffffffff ffffffff ii', unpack(imagedimensions)))
  g:seek('end', 0)
  g:write(data)
  
  f:close()
  g:close()
end

function slice_nifty(from, to, slice)
  require "pack"

  function pack(...)
    return arg
  end
  
  local f = io.open(from, 'rb')
  if f==nil then
    error('slice_nifty: cannot read file '..from)
  end
  
  local header = f:read(352)
  local headerkey        = pack(string.unpack(header, 'iA10A18ihA1A1'))
  local imagedimensions  = pack(string.unpack(header, 'hhhhhhhh hhhhhhh hhh ffffffff ffffffff ii', headerkey[1]));
  local nifty_extensions = pack(string.unpack(header, 'A80A24 hh ffffff ffff ffff ffff A16 A4', imagedimensions[1]))
  table.remove(headerkey,        1)
  table.remove(imagedimensions,  1)
  table.remove(nifty_extensions, 1)
  imagedimensions[1] = 3
  imagedimensions[5] = 1
  
  f:seek('set', 352+imagedimensions[2]*imagedimensions[3]*imagedimensions[4]*2*slice)
  local data = f:read(imagedimensions[2]*imagedimensions[3]*imagedimensions[4]*2)
  f:close()
  
  local g = io.open(to, 'wb')
  if g==nil then
    error('slice_nifty: cannot write file '..to)
  end
  g:write(string.pack('iAAihAA', unpack(headerkey)))
  g:write(string.pack('hhhhhhhh hhhhhhh hhh ffffffff ffffffff ii', unpack(imagedimensions)))
  g:write(string.pack('AA hh ffffff ffff ffff ffff A A', unpack(nifty_extensions)))
  g:write(string.rep('\000', 4)) -- empty minimal extension
  g:write(data)
  
  g:close()
end

function reread_interfile(imagefiles, headerfilename, replacevalues, shift, scale)
  function image(i)
    local t=imagefiles[i]
    if type(t)=='table' then
      return t[1] or t.name
    else
      return t
    end
  end
  local function get(s, item)
    return string.match(s, item .. ' := (.-)\n')
  end
    
  -- default parameters
  replacevalues = replacevalues or {}
  scale = scale or 1
  shift = shift or 0
  
  local f = io.open(headerfilename, 'rt')
  if f==nil then
    error('pixelread_interfile: cannot read header file '..headerfilename)
    return
  end
  local h = f:read('*all')
  f:close()
  
  -- read SIRT specific interfile header, % is escape character
  local datafilename          = get(h, 'name of data file')
  local numberformat          = get(h, 'number format') or 'float'
  local numberofbytesperpixel = get(h, 'number of bytes per pixel') or 4
  local numberofdimensions    = get(h, 'number of dimensions') or 3
  local matrixsize = {}
  matrixsize[1]               = get(h, 'matrix size %[1%]')
  matrixsize[2]               = get(h, 'matrix size %[2%]')
  matrixsize[3]               = get(h, 'matrix size %[3%]')
  scalingfactor = {}
  scalingfactor[1]            = get(h, 'scaling factor %(mm/pixel%) %[1%]')
  scalingfactor[2]            = get(h, 'scaling factor %(mm/pixel%) %[2%]')
  scalingfactor[3]            = get(h, 'scaling factor %(mm/pixel%) %[3%]')
  firstpixeloffset = {}
  firstpixeloffset[1]         = get(h, 'first pixel offset %(mm%) %[1%]')
  firstpixeloffset[2]         = get(h, 'first pixel offset %(mm%) %[2%]')
  firstpixeloffset[3]         = get(h, 'first pixel offset %(mm%) %[3%]')
  
  local fulldatafilename = string.match(headerfilename, '^(.*\\)') .. datafilename
  local suid = genuid()
  
  local i1 = DicomObject:new(); 
  local i2 = DicomObject:new(); 
  i1:Read(image(1))
  i2:Read(image(#imagefiles))
  
  local a, b
  local ps     = {}
  local center = {}
  local pos1   = {}
  local pos2   = {}
  a, b, ps[1], ps[2] = string.find(i1.PixelSpacing, '^(.-)\\(.-)$')
  a, b, pos1[1], pos1[2], pos1[3] = string.find(i1.ImagePositionPatient, '^(.-)\\(.-)\\(.-)$')
  a, b, pos2[1], pos2[2], pos2[3] = string.find(i2.ImagePositionPatient, '^(.-)\\(.-)\\(.-)$')
  
  center[1] =  pos1[1] + ((i1.Rows   -1)/2) * ps[1]
  center[2] =  pos1[2] + ((i1.Columns-1)/2) * ps[2]
  center[3] = (pos1[3] + pos2[3])/2
  
  for i=1, #imagefiles do
    i1.SeriesInstanceUID = suid
    i1.SOPInstanceUID    = genuid()
    i1.PixelSpacing      = string.format('%f\\%f', scalingfactor[1], scalingfactor[2])
    i1.Rows              = matrixsize[1]
    i1.Columns           = matrixsize[2]
    for k,v in pairs(replacevalues) do
      i1[k] = v
    end
    
    -- slice and write input data 
    local f = io.open(fulldatafilename, 'rb')
    f:seek('set', matrixsize[1]*matrixsize[2]*numberofbytesperpixel*(i-1))
    i1:SetImage(0, f:read(matrixsize[1]*matrixsize[2]*numberofbytesperpixel), scale)
    f:close()
    
    i1.ImagePositionPatient = string.format('%.8g\\%.8g\\%.8g', 
                                  firstpixeloffset[1] + 0*center[1], 
                                  firstpixeloffset[2] + 0*center[2],
                                  firstpixeloffset[3] +   center[3] + shift - scalingfactor[3] * (i-1))
    i1.SliceLocation        = string.format('%.8g', 
                                  firstpixeloffset[3] +   center[3] + shift - scalingfactor[3] * (i-1))
    i1.InstanceNumber       = i

    i1:AddImage()
  end
end

function write_nrrd(imagefiles, filename)
  function image(i)
    local t=imagefiles[i]
    if type(t)=='table' then
      return t[1] or t.name
    else
      return t
    end
  end
  
  if #imagefiles<2 then
    error('*** write_nrrd: requires at least 2 images ')
    return
  end
  
  -- need two images to compute Z direction from ImagePositionPatient, 3rd image to check on 4D or inconsistent data
  local i1 = DicomObject:new(); i1:Read(image(1))
  local i2 = DicomObject:new(); i2:Read(image(2))
  local i3 = DicomObject:new(); i3:Read(image(#imagefiles))
  
  if (i1.SliceLocation or '') == '' then
    print('*** write_nrrd does not support images without slice location')
    return
  end
  
  vl = (i1.SamplesPerPixel or '1') + 0
  if vl~=1 then
    print('*** write_nrrd does not support color images')
    return
  end
  
  local ps = i1.PixelSpacing or '1\\1'
  local p1 = string.gsub(ps, '.*\\', '') + 0
  local p2 = string.gsub(ps, '\\.*', '') + 0
  local slicespacing = ((i3.SliceLocation+0) - (i1.SliceLocation+0)) / (#imagefiles-1)
  
  local ndim=3
  local dimz=#imagefiles
  local dimt=1
  
  -- is there a slicing inconsistency
  if math.abs((i2.SliceLocation+0) - (i1.SliceLocation+0) - slicespacing) > 0.01 then
    print('*** Slice spacing inconsistent, is this a localizer?')
    print('*** There are', #imagefiles, 'slices and the last slice number is ', i3.InstanceNumber)
	return
  end
  
  local t
  t = split(i1.ImagePositionPatient, '\\')
  local xs1, ys1, zs1 = unpack(t);
  t = split(i3.ImagePositionPatient, '\\')
  local xs2, ys2, zs2 = unpack(t);
  t = split(i1.ImageOrientationPatient, '\\')
  local rx1, rx2, rx3, ry1, ry2, ry3 = unpack(t);
  local rz1 = (xs2-xs1) / (dimz-1)
  local rz2 = (ys2-ys1) / (dimz-1)
  local rz3 = (zs2-zs1) / (dimz-1)
   
  local f
  f = io.open(filename, 'wb')
  if f==nil then
    error('write_nrrd: cannot write file '..filename)
    return
  end

  xs1, ys1, zs1 = unpack(split(i1.ImagePositionPatient, '\\'));
  local sx={p1*rx1, p2*ry1, rz1, 0+xs1}
  local sy={p1*rx2, p2*ry2, rz2, 0+ys1}
  local sz={p1*rx3, p2*ry3, rz3, 0+zs1}

  f:write("NRRD0004\n")
  f:write("# Complete NRRD file format specification at:\n")
  f:write("# http://teem.sourceforge.net/nrrd/format.html\n")
  f:write("type: short\n")
  f:write("dimension: 3\n")
  f:write("space: left-posterior-superior\n")
  f:write(string.format("sizes: %d %d %d\n", i1.Columns, i1.Rows, dimz))
  f:write(string.format("space directions: (%f,%f,%f) (%f,%f,%f) (%f,%f,%f)\n",sx[1],sx[2],sx[3],sy[1],sy[2],sy[3],sz[1],sz[2],sz[3]))
  f:write("kinds: domain domain domain\n")
  f:write("endian: little\n")
  f:write("encoding: raw\n")
  f:write(string.format("space origin: (%f,%f,%f)\n",sx[4],sy[4],sz[4]))
  f:write("\n")

  for i=1, #imagefiles do        -- write pixel values or nifty image of all images
    i1:Read(image(i))
    local y=i1:GetImage(0)
    f:write(y)
  end
  f:close()
end

--[[
q = DicomObject:new()
q.PatientID = "HERK18032001"
q.SeriesInstanceUID = "1.3.12.2.1107.5.2.4.7640.20010318151948000002"
q.SOPInstanceUID = ""
q.InstanceNumber = ""
images = dicomquery("CONQUESTSRV1", "IMAGE", q)
imaget={}
for k=0,#images-1 do
  if (images[k].InstanceNumber or '') == '' then
    images[k].InstanceNumber = '0'
  end
  imaget[k+1]={}
  imaget[k+1].SOPInstanceUID=images[k].SOPInstanceUID
  imaget[k+1].InstanceNumber= tonumber(images[k].InstanceNumber or '0')
  imaget[k+1].name = images[k].PatientID .. ':' .. images[k].SOPInstanceUID
end
table.sort(imaget, function(a,b) return a.InstanceNumber<b.InstanceNumber end)
write_nrrd(imaget, 'test.nrrd')
]]
