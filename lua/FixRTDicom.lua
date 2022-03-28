-- This module attempts to fix UID's in a CT/RTSTRUCT/RTPLAN/RTIMAGE/RTDOSE set
-- Marcel van Herk, 20120817
-- mvh+bs: prints with ***; note: do not use with DBF driver
-- Some cleanup after fixes of Barbara Stam, Marcel van Herk, 20120920
-- Marcel van Herk, 20210422: fix for Varian, use '' in dbase access; attaches to MIP
-- 20220321: allow CT to be in different study (assumes on CT per patient ID)
-- 20220322: evo+mvh search UIDs on slice location to ensure correct order

-- gets sop of rtplan for this study - note assumes there is only one
function getrtplan(data)
  local StudyUID = data.StudyInstanceUID;
  local SeriesInst = dbquery('DICOMSeries', 'SeriesInst', "StudyInsta = '"..StudyUID.."' and Modality = 'RTPLAN'")[1][1]
  local PlanSOP = dbquery('DICOMImages', 'SOPInstanc', "SeriesInst = '"..SeriesInst..";")[1][1]
  return PlanSOP
end;

-- gets sop of rtstruct for this study - note assumes there is only one
function getrtstruct(data)
  local StudyUID = data.StudyInstanceUID;
  local SeriesInst = dbquery('DICOMSeries', 'SeriesInst', "StudyInsta = '"..StudyUID.."' and Modality = 'RTSTRUCT'")[1][1]
  local StructSOP = dbquery('DICOMImages', 'SOPInstanc', "SeriesInst = '"..SeriesInst..'"')[1][1]
  return StructSOP
end;

-- gets CT series for this study - note assumes there is only one
local ctseries=''
function getctseries(data)
  local StudyUID = data.StudyInstanceUID;
  if ctseries=='' then
    local db = dbquery('DICOMSeries', 'SeriesInst', "StudyInsta = '"..StudyUID.."' and Modality = 'CT' and SeriesDesc like '%'")
    if #db>0 then 
      ctseries = db[1][1]
    else
      db = dbquery('DICOMSeries', 'SeriesInst', "SeriesPat = '"..data.PatientID.."' and Modality = 'CT' and SeriesDesc like '%'")
      ctseries = db[1][1]
    end
  end
  return ctseries;
end;

-- gets CT frame of reference  for this study - note assumes there is only one
local ctframeofref='';
function getctframeofref(data)
  local StudyUID = data.StudyInstanceUID;
  if ctframeofref=='' then
    local db = dbquery('DICOMSeries', 'FrameOfRef', "StudyInsta = '"..StudyUID.."' and Modality = 'CT' and SeriesDesc like '%'")
    if #db>0 then 
      ctframeofref=db[1][1]
    else
      db = dbquery('DICOMSeries', 'FrameOfRef', "SeriesPat = '"..data.PatientID.."' and Modality = 'CT' and SeriesDesc like '%'")  
      ctframeofref=db[1][1]
    end
  end
  return ctframeofref;
end;

-- gets study for this study
function getstudy(data)
  local StudyUID = data.StudyInstanceUID;
  return StudyUID;
end;

-- returns correct CT slice sop for incorrect CT slice sop in RTSTRUCT
-- input: data = RTSTRUCT
--        sops = table of incorrect sops extracted from RTSTRUCT
--        sop  = incorrect sop
-- assumption: number and order of incorrect CT slice sops matches those of correct CT slice sops
-- note: if this is not true, the RTSTRUCT will still be readable, but syntactically incorrect\
local CTSOPS={}
function getsopfromloc(data, sops, location)
--  local t=1;
--  for i=1, #sops do
--    if sops[i]==sop then
--      t = i;
--    end;
--  end;
  
  local StudyUID = data.StudyInstanceUID;
  local SeriesInst = getctseries(data);
  if #CTSOPS==0 then
    CTSOPS = dbquery('DICOMImages', 'SOPInstanc,ImageNumbe,SliceLocat', "SeriesInst = '"..SeriesInst.."'", 'cast(ImageNumbe as numeric)');
  end
  
  local t=1;
  for CTi=1,#CTSOPS do
    if math.abs(location- tonumber(CTSOPS[CTi][3])) < 0.001 then
      t = CTi
    end
  end

  if t > #CTSOPS then
    t = #CTSOPS;
  end;
  return CTSOPS[t][1];  
end;


function getsopfromsop(data, sops, sop)
  local t=1;
  for i=1, #sops do
    if sops[i]==sop then
      t = i;
    end;
  end;
  
  local StudyUID = data.StudyInstanceUID;
  local SeriesInst = getctseries(data);
  if #CTSOPS==0 then
    CTSOPS = dbquery('DICOMImages', 'SOPInstanc,ImageNumbe,SliceLocat', "SeriesInst = '"..SeriesInst.."'", 'cast(ImageNumbe as numeric)');
  end

  if t > #CTSOPS then
    t = #CTSOPS;
  end;
  return CTSOPS[t][1];  
end;

-- process RTSTRUCT, prints UIDS, referenced UIDS and new referenced UIDS
if Data.Modality=='RTSTRUCT' then
  local i, j, k;

  -- print sops
  print('*** Processing RTSTRUCT for patient ' .. Data.PatientName .. ' ' .. Data.PatientID);
  print('*** Date of RTSTRUCT is ' .. Data.StructureSetDate);
  print('*** study', Data.StudyInstanceUID);
  print('*** series', Data.SeriesInstanceUID);
  print('*** sop', Data.SOPInstanceUID)

  -- print references
  --print('*** r study', Data.ReferencedStudySequence[0].ReferencedSOPInstanceUID);
  print('*** r frameofref', Data.ReferencedFrameOfReferenceSequence[0].FrameOfReferenceUID)
  print('*** r study', Data.ReferencedFrameOfReferenceSequence[0].RTReferencedStudySequence[0].ReferencedSOPInstanceUID)
  print('*** r series', Data.ReferencedFrameOfReferenceSequence[0].RTReferencedStudySequence[0].RTReferencedSeriesSequence[0].SeriesInstanceUID) --]]

  --for i=0, #Data.ReferencedFrameOfReferenceSequence[0].RTReferencedStudySequence[0].RTReferencedSeriesSequence[0].ContourImageSequence-1 do
  --  print('*** r sop', i, Data.ReferencedFrameOfReferenceSequence[0].RTReferencedStudySequence[0].RTReferencedSeriesSequence[0].ContourImageSequence[i].ReferencedSOPInstanceUID)
  --end;    

  --[[
  for i=0, #Data.StructureSetROISequence-1 do
    print('*** frameofref', i, Data.StructureSetROISequence[i].ReferencedFrameOfReferenceUID)
  end;
  for i=0, #Data.ROIContourSequence-1 do
    for j=0, #Data.ROIContourSequence[i].ContourSequence-1 do
      for k=0, #Data.ROIContourSequence[i].ContourSequence[j].ContourImageSequence-1 do
        print('*** r sop', i, j, k, Data.ROIContourSequence[i].ContourSequence[j].ContourImageSequence[k].ReferencedSOPInstanceUID)
       end
    end
  end;
  --]]

  -- build table of incorrect sops
  local sops = {}
  for i=0, #Data.ReferencedFrameOfReferenceSequence[0].RTReferencedStudySequence[0].RTReferencedSeriesSequence[0].ContourImageSequence-1 do
    sops[i+1] = Data.ReferencedFrameOfReferenceSequence[0].RTReferencedStudySequence[0].RTReferencedSeriesSequence[0].ContourImageSequence[i].ReferencedSOPInstanceUID
  end;    
  --table.sort(sops);
    
  -- fix references
  --Data.ReferencedStudySequence[0].ReferencedSOPInstanceUID = getstudy(Data);
  Data.ReferencedFrameOfReferenceSequence[0].FrameOfReferenceUID = getctframeofref(Data);
  Data.ReferencedFrameOfReferenceSequence[0].RTReferencedStudySequence[0].ReferencedSOPInstanceUID = getstudy(Data);
  Data.ReferencedFrameOfReferenceSequence[0].RTReferencedStudySequence[0].RTReferencedSeriesSequence[0].SeriesInstanceUID = getctseries(Data);

  for i=0, #Data.StructureSetROISequence-1 do
    Data.StructureSetROISequence[i].ReferencedFrameOfReferenceUID = getctframeofref(Data);
  end;
  
  for i=0, #Data.ReferencedFrameOfReferenceSequence[0].RTReferencedStudySequence[0].RTReferencedSeriesSequence[0].ContourImageSequence-1 do
    Data.ReferencedFrameOfReferenceSequence[0].RTReferencedStudySequence[0].RTReferencedSeriesSequence[0].ContourImageSequence[i].ReferencedSOPInstanceUID =
      getsopfromsop(Data, sops, Data.ReferencedFrameOfReferenceSequence[0].RTReferencedStudySequence[0].RTReferencedSeriesSequence[0].ContourImageSequence[i].ReferencedSOPInstanceUID);
  end;    

  if Data.ROIContourSequence then
    for i=0, #Data.ROIContourSequence-1 do
      if Data.ROIContourSequence[i].ContourSequence then
        for j=0, #Data.ROIContourSequence[i].ContourSequence-1 do
     if Data.ROIContourSequence[i].ContourSequence[j].ContourImageSequence then
            for k=0, #Data.ROIContourSequence[i].ContourSequence[j].ContourImageSequence-1 do
              if tonumber(Data.ROIContourSequence[i].ContourSequence[j].NumberOfContourPoints) > 0 then
                local loc = tonumber( string.match(Data.ROIContourSequence[i].ContourSequence[j]:GetVR(0x3006,0x0050,true), ".-\\.-\\(.-)\\.*") )
                Data.ROIContourSequence[i].ContourSequence[j].ContourImageSequence[k].ReferencedSOPInstanceUID =
                  getsopfromloc(Data, sops, loc);
              end
        end
          end   
        end
      end
    end
  end;
  
  
  -- print new fixed references
  --print('*** new study', Data.ReferencedStudySequence[0].ReferencedSOPInstanceUID);
  print('*** new frameofref', Data.ReferencedFrameOfReferenceSequence[0].FrameOfReferenceUID)
  print('*** new study', Data.ReferencedFrameOfReferenceSequence[0].RTReferencedStudySequence[0].ReferencedSOPInstanceUID)
  print('*** new series', Data.ReferencedFrameOfReferenceSequence[0].RTReferencedStudySequence[0].RTReferencedSeriesSequence[0].SeriesInstanceUID)--]]

  --for i=0, #Data.ReferencedFrameOfReferenceSequence[0].RTReferencedStudySequence[0].RTReferencedSeriesSequence[0].ContourImageSequence-1 do
  --  print('*** new sop', i, Data.ReferencedFrameOfReferenceSequence[0].RTReferencedStudySequence[0].RTReferencedSeriesSequence[0].ContourImageSequence[i].ReferencedSOPInstanceUID)
  --end;    

  --[[
  for i=0, #Data.StructureSetROISequence-1 do
    print('new frameofref', i, Data.StructureSetROISequence[i].ReferencedFrameOfReferenceUID)
  end; 

  if Data.ROIContourSequence then
    for i=0, #Data.ROIContourSequence-1 do
      if Data.ROIContourSequence[i].ContourSequence then
        for j=0, #Data.ROIContourSequence[i].ContourSequence-1 do
     if Data.ROIContourSequence[i].ContourSequence[j].ContourImageSequence then
       for k=0, #Data.ROIContourSequence[i].ContourSequence[j].ContourImageSequence-1 do
         print('new sop', i, j, k, Data.ROIContourSequence[i].ContourSequence[j].ContourImageSequence[k].ReferencedSOPInstanceUID)
       end
     end
   end
      end
    end
  end;
  --]]
  
end;

-- idem for RTPLAN
if Data.Modality=='RTPLAN' then
  print('*** Processing RTPLAN for patient ' .. Data.PatientName .. ' ' .. Data.PatientID);
  print('*** Date of RTPLAN is ' .. Data.RTPlanDate);
  print('*** study', Data.StudyInstanceUID);
  print('*** series', Data.SeriesInstanceUID);
  print('*** sop', Data.SOPInstanceUID)

  print('*** r study', Data.ReferencedStudySequence[0].ReferencedSOPInstanceUID);
  print('*** r rtstruct', Data.ReferencedStructureSetSequence[0].ReferencedSOPInstanceUID)
  Data.ReferencedStudySequence[0].ReferencedSOPInstanceUID = getstudy(Data);
  Data.ReferencedStructureSetSequence[0].ReferencedSOPInstanceUID = getrtstruct(Data)

  print('*** new study', Data.ReferencedStudySequence[0].ReferencedSOPInstanceUID);
  print('*** new rtstruct', Data.ReferencedStructureSetSequence[0].ReferencedSOPInstanceUID)
end

-- idem for RTDOSE
if Data.Modality=='RTDOSE' then
  print('*** Processing RTDOSE for patient ' .. Data.PatientName .. ' ' .. Data.PatientID);
  print('*** Date of RTDOSE is ' .. Data.ImageDate);
  print('*** study', Data.StudyInstanceUID);
  print('*** series', Data.SeriesInstanceUID);
  print('*** sop', Data.SOPInstanceUID)

  print('*** r study', Data.ReferencedStudySequence[0].ReferencedSOPInstanceUID);
  print('*** r rtplan', Data.ReferencedRTPlanSequence[0].ReferencedSOPInstanceUID)
  Data.ReferencedStudySequence[0].ReferencedSOPInstanceUID = getstudy(Data);
  Data.ReferencedRTPlanSequence[0].ReferencedSOPInstanceUID = getrtplan(Data);

  print('*** new study', Data.ReferencedStudySequence[0].ReferencedSOPInstanceUID);
  print('*** new rtplan', Data.ReferencedRTPlanSequence[0].ReferencedSOPInstanceUID)
end

-- idem for RTIMAGE
if Data.Modality=='RTIMAGE' then
  print('*** Processing RTIMAGE for patient ' .. Data.PatientName .. ' ' .. Data.PatientID);
  print('*** Date of RTIMAGE is ' .. Data.ImageDate);
  print('*** study', Data.StudyInstanceUID);
  print('*** series', Data.SeriesInstanceUID);
  print('*** sop', Data.SOPInstanceUID)

  print('*** r study', Data.ReferencedStudySequence[0].ReferencedSOPInstanceUID);
  print('*** r rtplan', Data.ReferencedRTPlanSequence[0].ReferencedSOPInstanceUID);
  Data.ReferencedStudySequence[0].ReferencedSOPInstanceUID = getstudy(Data);
  Data.ReferencedRTPlanSequence[0].ReferencedSOPInstanceUID = getrtplan(Data);

  print('*** new study', Data.ReferencedStudySequence[0].ReferencedSOPInstanceUID);
  print('*** new rtplan', Data.ReferencedRTPlanSequence[0].ReferencedSOPInstanceUID);
end

-- CT does not require processing, just print sops
if Data.Modality=='CT' then
  print('*** Processing CT for patient ' .. Data.PatientName .. ' ' .. Data.PatientID);
  print('*** study', Data.StudyInstanceUID);
  print('*** series', Data.SeriesInstanceUID);
  print('*** sop', Data.SOPInstanceUID)
  print('*** frame', Data.FrameOfReferenceUID)--]]
end
