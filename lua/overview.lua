--[[

#Conquest DICOM server scripting overview

Brief demo of _all_ scripting options in **Conquest Dicom Server**.
If you run this script from **ZeroBrane Studio**, you can put 
breakpoints on any line, single step, inspect data with the mouse, 
and display arbitrary data and enter arbitrary commands to the 
DICOM Server in the `Local console` window.

In version 1.4.19 for Windows the following modules are 
embedded in the executable:
-- `require('socket')`
-- `require('pack')`
-- and string:reshape(string, i, j) is embedded

-- for 1.4.19beta3; decompress sample data before processing; distributed in jpeg format
-- for 1.5.0c; add dicomget, dicomread, json stuff
]]

-- for demo fill the global variable Data which normally contains
-- the incoming DICOM object. You can read from disk or from the 
-- server using a PatientID:SOPInstanceUID format. For reading from 
-- virtual server use 'StudyUID\SeriesUID\SOPInstanceUID format.
-- readdicom('c:\\t.dcm') -- from disk
readdicom('0009703828:1.3.46.670589.5.2.10.2156913941.892665340.475317')
script('compression un')
-- Note that in this demo Data is undefined until after this call
-- I.e., Data:Read() is not allowed as first call

-- association info available from lua:
print(Association.Calling, Association.Called, Association.Thread, Association.ConnectedIP)

-- all counters available from lua:
print('------ All counters --------')
print(Global.StartTime)
print(Global.TotalTime)
print(Global.LoadTime)
print(Global.ProcessTime)
print(Global.SaveTime)   
print(Global.ImagesSent)      
print(Global.ImagesReceived)
print(Global.ImagesSaved)      
print(Global.ImagesSaved)      
print(Global.ImagesForwarded)   
print(Global.ImagesExported)   
print(Global.ImagesCopied)      
print(Global.IncomingAssociations)   
print(Global.EchoRequest)   
print(Global.C_Find_PatientRoot)
print(Global.C_Move_PatientRootNKI)
print(Global.C_Move_PatientRoot)
print(Global.C_Find_StudyRoot)
print(Global.C_Move_StudyRootNKI)
print(Global.C_Move_StudyRoot)
print(Global.C_Find_PatientStudyOnly)
print(Global.C_Find_ModalityWorkList)
print(Global.C_Move_PatientStudyOnlyNKI)
print(Global.C_Move_PatientStudyOnly)
print(Global.UnknownRequest)
print(Global.CreateBasicFilmSession)
print(Global.DeleteBasicFilmSession)
print(Global.ActionBasicFilmSession)
print(Global.SetBasicFilmSession)
print(Global.CreateBasicFilmBox)
print(Global.ActionBasicFilmBox)
print(Global.SetBasicFilmBox)
print(Global.DeleteBasicFilmBox)
print(Global.SetBasicGrayScaleImageBox)
print(Global.SetBasicColorImageBox)
print(Global.GuiRequest)
print(Global.ImagesToGifFromGui)
print(Global.ImagesToDicomFromGui)
print(Global.ExtractFromGui)
print(Global.QueryFromGui)
print(Global.DeleteImageFromGui)
print(Global.DeletePatientFromGui)
print(Global.DeleteStudyFromGui)
print(Global.DeleteStudiesFromGui)
print(Global.DeleteSeriesFromGui)
print(Global.MovePatientFromGui)
print(Global.MoveStudyFromGui)
print(Global.MoveStudiesFromGui)
print(Global.MoveSeriesFromGui)
print(Global.AddedFileFromGui)
print(Global.DumpHeaderFromGui)
print(Global.ForwardFromGui)
print(Global.GrabFromGui)
print(Global.DatabaseOpen)
print(Global.DatabaseClose)
print(Global.DatabaseQuery)
print(Global.DatabaseAddRecord)
print(Global.DatabaseDeleteRecord)
print(Global.DatabaseNextRecord)
print(Global.DatabaseCreateTable)
print(Global.DatabaseUpdateRecords)
print(Global.QueryTime)
print(Global.SkippedCachedUpdates)
print(Global.UpdateDatabase)
print(Global.AddedDatabase)
print(Global.NKIPrivateCompress)
print(Global.NKIPrivateDecompress)
print(Global.DownSizeImage)
print(Global.DecompressJpeg)
print(Global.CompressJpeg)
print(Global.DecompressJpeg2000)
print(Global.CompressJpeg2000)
print(Global.DecompressedRLE)
print(Global.DePlaned)
print(Global.DePaletted)
print(Global.RecompressTime)
print(Global.gpps)
print(Global.gppstime)

-- all configuration items are available from lua (read/write)
print(Global.NoDicomCheck)
print(Global.DebugLevel)
print(Global.ThreadCount)
print(Global.OpenThreadCount)
print(Global.EnableReadAheadThread)
print(Global.WorkListMode)
print(Global.StorageFailedErrorCode)
print(Global.FailHoldOff)
print(Global.RetryDelay)
print(Global.QueueSize)
print(Global.ForwardCollectDelay)
print(Global.CacheVirtualData)
print(Global.gJpegQuality)
print(Global.gUseOpenJpeg)
print(Global.FixKodak)
print(Global.NumIndexing)
print(Global.DoubleBackSlashToDB)
print(Global.UseEscapeStringConstants)
print(Global.EnableComputedFields)
print(Global.FileCompressMode)
print(Global.RootConfig)
print(Global.BaseDir)
print(Global.ConfigFile)
print(Global.DicomDict)
print(Global.AutoRoutePipe)
print(Global.AutoRouteExec)
print(Global.DroppedFileCompression)
print(Global.IncomingCompression)
print(Global.TransmitCompression)
print(Global.DefaultNKITransmitCompression)
print(Global.ArchiveCompression)
print(Global.TestMode)
print(Global.StatusString)

print('------ Set a config item --------')
Global.DebugLevel = 4

print('----- Read any dicom.ini item -----')
section = 'sscscp'; item = 'TCPPort'; default='';
print(gpps(section, item, default))

-- all command items are available from lua (read only)
print('------ testing debug log (typically not shown) --------', 1234)
debuglog('Command priority', Command.Priority)

-- set Data storage
print('------ test setting storage --------')
Data.Storage = 'MAG2'
print('You can only read/write storage in an import converter', Data.Storage);

-- read/write data, create sequences, and write into sequences (if [] not passed, [0] is assumed)
print('------ test read/write Data --------')
Data.PatientName = Data.PatientID
Data.ReferencedStudySequence = {}
print('Number of elements in sequence', #Data.ReferencedStudySequence);
print('This is a sequence: ', Data.ReferencedStudySequence);
Data.ReferencedStudySequence[0].StudyInstanceUID = Data.StudyInstanceUID
Data.ReferencedStudySequence.StudyInstanceUID = Data.StudyInstanceUID
Data.ReferencedStudySequence[1].StudyInstanceUID = Data.StudyInstanceUID
print(#Data.ReferencedStudySequence);
print('This is a sequence item: ', Data.ReferencedStudySequence[0].StudyInstanceUID);

-- list items in Object, returns names,types,groups,elements
print(Data.ReferencedStudySequence[1]:ListItems())

-- Delete sequence item:
-- Object:DeleteFromSequence(name, item)
-- Array:Delete(item)
Data:DeleteFromSequence('ReferencedStudySequence', 0);
Data.ReferencedStudySequence:Delete(0)

-- delete items
print('------ test delete item --------')
Data.ReferencedStudySequence = nil
print('This was a sequence: ', Data.ReferencedStudySequence);

-- inspect dictionary (results in 16, 32 vs PatientID)
print(dictionary('PatientID'))
print(dictionary(16, 32))

-- inspect sql definition (database, row) results in 16, 32 PatientID 64 SQL_STR DT_STR)
print(get_sqldef(0, 0))

-- send a script to conquest
print('------ test conquest script call --------')
script('nop this is an ImportConverter script') -- only works when Data defined
Data:Script('nop this is an ImportConverter script running on a specified DICOM object')

-- generate a tempfile
print(tempfile('.txt'))

-- send a script to conquest
print('------ test conquest script call --------')
script('nop this is an ImportConverter script') -- only works when Data defined
Data:Script('nop this is an ImportConverter script running on a specified DICOM object')

-- send a servercommand to conquest and read its result
print('------ test conquest command call --------')
print(servercommand('display_status:'))
servercommand('display_status:', 'cgibinary') -- for web interface; also allows value 'cgihtml' or <filename to upload and >filename to download or 'binary' without processing

-- run executable in the background (avoids window opened by os.execute)
system('dgate.exe -?')

-- get an item from ACRNEMA.MAP
print('------ test reading ACRNEMA.MAP --------')
print(get_amap(0))
-- results in 'CONQUESTSRV1    127.0.0.1       5678    un'

-- remap UIDs (in 1.4.17)
print(changeuid('12jgkfjgfkgjax', 'aapnootmies'))
print(changeuidback('aapnootmies'))
print(changeuid('1.1'))
print(changeuidback('1.2.826.0.1.3680043.2.135.734877.42238624.7.1359125302.31.0'))
print(genuid())
-- results in:
-- [CONQUESTSRV1] aapnootmies
-- [CONQUESTSRV1] 12jgkfjgfkgjax
-- [CONQUESTSRV1] 1.2.826.0.1.3680043.2.135.734877.42238624.7.1359125302.31.0
-- [CONQUESTSRV1] 1.1
-- [CONQUESTSRV1] .... a new uid here ....
-- To remap all uids use script('newuids') and reverse (1.4.17) with script('olduids')

-- staged remap UIDs (in 1.4.19)
print(changeuid('12jgkfjgfkgjax', 'aapnootmies', 'stage1'))
print(changeuidback('aapnootmies', 'stage1'))
print(changeuid('1.1', 'stage1'))
print(changeuidback('1.2.826.0.1.3680043.2.135.734877.42238624.7.1359125302.31.0', 'stage1'))
print(genuid())

-- MD5 has based uid remapping (forward only):
print(changeuid('1.1', '#stage1'))

-- query the local database (also possible from CGI interface, if the database is setup in the CGI dicom.ini)
print('------ test quering a database --------')
print((dbquery('DICOMPatients', 'PatientNam', 'PatientID LIKE \'2%\'')[1] or {})[1])

-- set and get pixels in the current image or any loaded image (MUST BE UNCOMPRESSED!)
print('------ test reading and writing pixels --------')
x=0; y=0; frame=0;
print(getpixel(x, y, frame));
setpixel(x, y, frame, getpixel(x, y, frame)*2+10);
print(getpixel(x, y, frame));
print(Data:GetPixel(x, y, frame))

-- set and get rows and colums in the image
print('------ test reading and writing rows and columns --------')
a = getrow(Data.Rows / 2)
a = Data:GetRow(Data.Rows / 2)
print(a[1], a[2], a[3], a[4], a[128]);
setcolumn(Data.Columns / 2, frame, a)
Data:SetColumn(Data.Columns / 3, frame, a)

-- get / set image as binary string, also allow efficient binary image conversion (1.4.17)
a = getimage(frame); a = Data:GetImage(frame)
setimage(frame, a)
Data:SetImage(frame, a)

-- get / set image as binary string with floats (1.4.17a)
-- read a from a floating point image, i.e., it has 4 bytes per pixel, scale passed
-- setimage(frame, a, 1000); Data:SetImage(frame, a, 1000)

-- copy a dicom object
local c = Data:Copy()
c:Write('c:\\data\\copy.dcm')
c:free()

-- compress a dicom object
local c = Data:Compress('jk')
c:Write('c:\\data\\compressed_jk.dcm')
c:free()

-- create/read/write/destroy a dicom object
print('------ test create/read/write dicom object --------')
a = newdicomobject()
a = DicomObject:new() -- preferred notation in 1.4.17
a.PatientID = 'test'
writedicom(a, 'c:\\data\\file1.dcm')
b = newdicomobject()
readdicom(b, 'c:\\data\\file1.dcm')
writeheader(b, 'c:\\data\\file1.txt')
a:Write('c:\\data\\file2.dcm')
a:Read('c:\\data\\file2.dcm')
-- document read here 
a:Dump('c:\\data\\file2.txt')
a:GetPixel(x, y, z)
a:SetPixel(x, y, z, value)
a:GetRow(x, y, z)
a:SetRow(x, y, z, table)
a:GetColumn(x, y, z)
a:SetColumn(x, y, z, table)
deletedicomobject(a) -- not required: will be freed automatically
-- a:free() -- also allowed in 1.4.17: 

-- add json object to dicom object, where the json_string can have elements by name or number.
local c = Data:Copy(json_string)

-- convert json object to dicom object, where the json_string can have elements by name or number.
local c = DicomObject:new(json_string)

-- query a dicomserver (returns a dicomobjectarray)
print('------ test query a dicom server --------')
b=newdicomobject(); b.PatientName = '*'; a=dicomquery('CONQUESTSRV1', 'PATIENT', b);
print ('First query result has this patientname:', a[0].PatientName);
-- deletedicomobject(a) -- not required: will be freed automatically

-- delete data from local dicomserver (use with care)
print('------ delete from dicom server --------')
b=newdicomobject(); b.PatientID = 'hopedoesntexist'; dicomdelete(b);
local threadno_forprogressinfo=123
b=newdicomobject(); b.PatientID = 'hopedoesntexist'; newdicomdelete(b, threadno_forprogressinfo);

-- modify data from local dicomserver (use with care)
local threadno_forprogressinfo=123
local copy=1
b=newdicomobject(); b.PatientID = 'hopedoesntexist'; newdicommodify(b, 'nop', threadno_forprogressinfo, copy);

-- create a dicomobjectarray
print('------ test creating dicom array --------')
a=newdicomarray(); a[0].PatientID='a'; a[1].PatientID='b';
-- in 1.4.17 also: a = DicomObject:newarray()

-- read the filename of a dropped file if any
print('------ test Filename variable --------')
print('Is there a file dropped?', Filename)

-- access to long and sequence VRs
print('------ test reading / writing long VRs --------')
y = Data:GetVR(0x7fe0, 0x10);
print('Length of y', #y);
y = getvr(0x7fe0, 0x10);
setvr(0x7fe0, 0x10, y);
Data:SetVR(0x7fe0, 0x10, y);
-- where y is either a table starting at 1, or a dicomobjectarray for a sequence
-- In 1.4.17 these command can also return a more efficient binary string:
y = Data:GetVR(0x7fe0, 0x10, true);
Data:SetVR(0x7fe0, 0x10, y);

-- memory allocation debugging
print('------ memory alloc check NOTE: CALL IS NOT THREAD SAFE --------')
print(heapinfo());

-- move
print('------ testing a C-MOVE --------')
AE = 'CONQUESTSRV1';
b=newdicomobject(); b.PatientName = 'HEAD EXP2'; b.QueryRetrieveLevel = 'STUDY'; dicommove('CONQUESTSRV1', AE, b);
b=newdicomobject(); b.PatientName = 'HEAD EXP2'; b.QueryRetrieveLevel = 'STUDY'; dicommove('CONQUESTSRV1', AE, b, 0, 'print(Global.StatusString)');
b=newdicomobject(); b.PatientName = 'HEAD EXP2'; b.QueryRetrieveLevel = 'PATIENT'; dicommove('CONQUESTSRV1', AE, b, 1);

--get
b=newdicomobject(); b.PatientName = 'HEAD EXP2'; b.QueryRetrieveLevel = 'IMAGE'
array=dicomget('CONQUESTSRV1', 'IMAGE', b);

--read array of objects
b=newdicomobject(); b.PatientName = 'HEAD EXP2'; array=dicomread(b);

-- advanced move; 1.5.0 up
print('------ testing advanced C-MOVE; example setting max sent slices --------')
b=newdicomobject();
c=newdicomobject(); c.ConquestMaxSlices=1
b.PatientName = 'HEAD EXP2'; b.QueryRetrieveLevel = 'PATIENT'; dicommove('CONQUESTSRV1', AE, b, 1, c);

-- convert DICOM object to string
c=newdicomobject(); c.ConquestMaxSlices=1
print(c:Serialize()) -- lua format
print(c:Serialize(true)) -- json format
print(c:Serialize(true, true)) -- json format with pixel data
print(c:Serialize(true, true, true)) -- dicomweb json format with pixel data

-- sql
print('------ testing an SQL statement --------')
sql("CREATE TABLE UserTable (CounterID varchar(99), Val integer)");
sql("INSERT INTO UserTable (CounterID,Val) VALUES ('CT',1) ON DUPLICATE KEY UPDATE Val=Val+1");

-- sleep, delay in ms
sleep(1000)

-- mkdir, make folder recursively
mkdir('c:\\temp\\test')

-- enter object into server:
x = DicomObject:new()
x:Read('c:\\data\\t.dcm'); x:AddImage() -- or addimage(x)

-- special script command 'retry'
print('------ testing retry script command --------')
script('retry') -- when used in RejectedImageWorkListConverter0 and RejectedImageConverter0; will re-attempt to store the object after the script is done
retry() -- also reject() and destroy() for importconverter implemented

-- special script command 'defer'
print('------ testing defer script command --------')
script('defer') -- when used in an ExportConverter, the convert will re-attempt to process or forward the object later
