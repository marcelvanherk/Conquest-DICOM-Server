-- Syntax  of the lua api in Conquest DICOM server - Marcel van Herk 20130522
-- 20130702: added more descriptions for 1.4.17a release
-- 20130808: added genuid, command_line
-- 20130812: updated comments, sleep, removed unused TransmitCompression
-- 20130813: added addimage(image) and DicomObject:AddImage()
-- 20140128: added copydicom(DDO) and DicomObject:Copy(); small fixes for 1.4.17d
-- 20140309: added compressdicom(), DicomObject:Compress(), mkdir and dicomquery2
-- 20140618: Fixed dicommove
-- 20160319: 1.4.19beta, added newdeletedicom, newmodifydicom, newcopydicom, extended changeuid/back
-- 20160827: 1.4.19beta3, added reject, retry, destroy and tempfile functions, returnfile variable
-- 20161120: 1.4.19, ConvertBinaryData
-- 20170430: 1.4.19a, Fix dicommove; extend servercommand
-- 20181110: 1.4.19c, Added C-Get counters, update servercommand info, DGATEVERSION
-- 20181111: 1.4.19c, Fixed typo
-- 20181216: 1.4.19d, Added listitems, Serialize and serialize
-- 20181222: 1.4.19d, Added dicomprint and dicomget
-- 20190101: 1.5.0-alpha, Added dicomstore
-- 20190103: Added tickcount
-- 20200109: 1.5.0-beta, Added dicomecho, listoldestpatients, crc, CGI()
-- 20200125: Added new parameters to Serialize and dicommove, updated text a bit
-- 20200314: release 1.5.0
-- 20220322: Added sort parameter to dbquery
-- 20220718: Updated documentation for json interfaces and new features

--[[
-- read/write data, create sequences, and write into sequences (if [] not passed, [0] is assumed)
print('------ test read/write Data --------')
Data.PatientName = Data.PatientID
Data.ReferencedStudySequence = {}
print('Number of elements in sequence' .. #Data.ReferencedStudySequence);
print('This is a sequence: ', Data.ReferencedStudySequence);
Data.ReferencedStudySequence[0].StudyInstanceUID = Data.StudyInstanceUID
Data.ReferencedStudySequence.StudyInstanceUID = Data.StudyInstanceUID
Data.ReferencedStudySequence[1].StudyInstanceUID = Data.StudyInstanceUID
print(#Data.ReferencedStudySequence);
print('This is a sequence item: ', Data.ReferencedStudySequence[0].StudyInstanceUID);

--delete items
print('------ test delete item --------')
Data.ReferencedStudySequence = nil
print('This was a sequence: ', Data.ReferencedStudySequence);
]]--

return {
  Filename = {
  type = "value",
  description = "Name of dropped file",
  },
  command_line = {
  type = "value",
  description = "command passed from e.g. script('process series after 10 by file.lua command')",
  },
  returnfile = {
  type = "value",
  description = "if set in lua server command, file contents are returned to client",
  },
  Association = {
    type = "class",
    description = "Information of current connection",
    childs = {
        ["Calling"] = { type ='value', description = "calling AE", valuetype = nil, },
        ["Called"] = { type ='value', description = "called AE", valuetype = nil, },
        ["Thread"] = { type ='value', description = "thread number", valuetype = nil, },
        ["ConnectedIP"] = { type ='value', description = "IP address of connected client", valuetype = nil, },
              }
  },
  Global = {
    type = "value",
    description = "Server status (counters) and configuration",
    childs = {
        ["StartTime"] = { type ='value', description = "int (s since 1970)", valuetype = nil, },
        ["TotalTime"] = { type ='value', description = "int (total processing time in s)", valuetype = nil, },
        ["LoadTime"] = { type ='value', description = "int (load time in s)", valuetype = nil, },
        ["ProcessTime"] = { type ='value', description = "int (image processing time in s)", valuetype = nil, },
        ["SaveTime"] = { type ='value', description = "int (image save time s)", valuetype = nil, },

        ["ImagesSent"] = { type ='value', description = "int (counter, images sent)", valuetype = nil, },
        ["ImagesReceived"] = { type ='value', description = "int (counter, images received)", valuetype = nil, },
        ["ImagesSaved"] = { type ='value', description = "int (counter, images saved)", valuetype = nil, },
        ["ImagesForwarded"] = { type ='value', description = "int (counter, images forwarded)", valuetype = nil, },
        ["ImagesExported"] = { type ='value', description = "int (counter, images processed by export converter executable)", valuetype = nil, },
        ["ImagesCopied"] = { type ='value', description = "int (counter, images copied by export converter)", valuetype = nil, },

        ["IncomingAssociations"] = { type ='value', description = "int (counter, number of associations)", valuetype = nil, },
        ["EchoRequest"] = { type ='value', description = "int (counter, number of c-echo requests)", valuetype = nil, },

        ["C_Find_PatientRoot"] = { type ='value', description = "int (counter, queries)", valuetype = nil, },
        ["C_Move_PatientRootNKI"] = { type ='value', description = "int (counter, moves)", valuetype = nil, },
        ["C_Get_PatientRoot"] = { type ='value', description = "int (counter, queries)", valuetype = nil, },
        ["C_Move_PatientRoot"] = { type ='value', description = "int (counter, moves)", valuetype = nil, },
        ["C_Find_StudyRoot"] = { type ='value', description = "int (counter, queries)", valuetype = nil, },
        ["C_Move_StudyRootNKI"] = { type ='value', description = "int (counter, moves)", valuetype = nil, },
        ["C_Move_StudyRoot"] = { type ='value', description = "int (counter, moves)", valuetype = nil, },
        ["C_Get_StudyRoot"] = { type ='value', description = "int (counter, moves)", valuetype = nil, },
        ["C_Find_PatientStudyOnly"] = { type ='value', description = "int (counter, queries)", valuetype = nil, },
        ["C_Find_ModalityWorkList"] = { type ='value', description = "int (counter, queries)", valuetype = nil, },
        ["C_Move_PatientStudyOnlyNKI"] = { type ='value', description = "int (counter, moves)", valuetype = nil, },
        ["C_Move_PatientStudyOnly"] = { type ='value', description = "int (counter, moves)", valuetype = nil, },
        ["C_Get_PatientStudyOnly"] = { type ='value', description = "int (counter, moves)", valuetype = nil, },

        ["UnknownRequest"] = { type ='value', description = "int (counter, unknown DICOM commands)", valuetype = nil, },

        ["CreateBasicFilmSession"] = { type ='value', description = "int (counter, print service)", valuetype = nil, },
        ["DeleteBasicFilmSession"] = { type ='value', description = "int (counter, print service)", valuetype = nil, },
        ["ActionBasicFilmSession"] = { type ='value', description = "int (counter, print service)", valuetype = nil, },
        ["SetBasicFilmSession"] = { type ='value', description = "int (counter, print service)", valuetype = nil, },
        ["CreateBasicFilmBox"] = { type ='value', description = "int (counter, print service)", valuetype = nil, },
        ["ActionBasicFilmBox"] = { type ='value', description = "int (counter, print service)", valuetype = nil, },
        ["SetBasicFilmBox"] = { type ='value', description = "int (counter, print service)", valuetype = nil, },
        ["DeleteBasicFilmBox"] = { type ='value', description = "int (counter, print service)", valuetype = nil, },
        ["SetBasicGrayScaleImageBox"] = { type ='value', description = "int (counter, print service)", valuetype = nil, },
        ["SetBasicColorImageBox"] = { type ='value', description = "int (counter, print service)", valuetype = nil, },

        ["GuiRequest"] = { type ='value', description = "int (counter, dgate -- commands)", valuetype = nil, },
        ["ImagesToGifFromGui"] = { type ='value', description = "int (counter, conversions)", valuetype = nil, },
        ["ImagesToDicomFromGui"] = { type ='value', description = "int (counter, conversions)", valuetype = nil, },
        ["ExtractFromGui"] = { type ='value', description = "int (counter, database extracts for GUI)", valuetype = nil, },
        ["QueryFromGui"] = { type ='value', description = "int (counter, database queries from lua or GUI)", valuetype = nil, },
        ["DeleteImageFromGui"] = { type ='value', description = "int (counter, deletions from GUI)", valuetype = nil, },
        ["DeletePatientFromGui"] = { type ='value', description = "int (counter, deletions from GUI)", valuetype = nil, },
        ["DeleteStudyFromGui"] = { type ='value', description = "int (counter, deletions from GUI)", valuetype = nil, },
        ["DeleteStudiesFromGui"] = { type ='value', description = "int (counter, deletions from GUI)", valuetype = nil, },
        ["DeleteSeriesFromGui"] = { type ='value', description = "int (counter, deletions from GUI)", valuetype = nil, },
        ["MovePatientFromGui"] = { type ='value', description = "int (counter, moves from web page)", valuetype = nil, },
        ["MoveStudyFromGui"] = { type ='value', description = "int (counter, moves from web page)", valuetype = nil, },
        ["MoveStudiesFromGui"] = { type ='value', description = "int (counter, moves from web page)", valuetype = nil, },
        ["MoveSeriesFromGui"] = { type ='value', description = "int (counter, moves from web page)", valuetype = nil, },
        ["AddedFileFromGui"] = { type ='value', description = "int (counter, added images by GUI)", valuetype = nil, },
        ["DumpHeaderFromGui"] = { type ='value', description = "int (counter, web page operation)", valuetype = nil, },
        ["ForwardFromGui"] = { type ='value', description = "int (counter, forward)", valuetype = nil, },
        ["GrabFromGui"] = { type ='value', description = "int (counter, grab missing images)", valuetype = nil, },

        ["DatabaseOpen"] = { type ='value', description = "int (counter, database operations)", valuetype = nil, },
        ["DatabaseClose"] = { type ='value', description = "int (counter, database operations)", valuetype = nil, },
        ["DatabaseQuery"] = { type ='value', description = "int (counter, database operations)", valuetype = nil, },
        ["DatabaseAddRecord"] = { type ='value', description = "int (counter, database operations)", valuetype = nil, },
        ["DatabaseDeleteRecord"] = { type ='value', description = "int (counter, database operations)", valuetype = nil, },
        ["DatabaseNextRecord"] = { type ='value', description = "int (counter, database operations)", valuetype = nil, },
        ["DatabaseCreateTable"] = { type ='value', description = "int (counter, database operations)", valuetype = nil, },
        ["DatabaseUpdateRecords"] = { type ='value', description = "int (counter, database operations)", valuetype = nil, },

        ["QueryTime"] = { type ='value', description = "int (time spent querying database in s)", valuetype = nil, },
        ["SkippedCachedUpdates"] = { type ='value', description = "int (counter, avoided duplicate updates during storage)", valuetype = nil, },
        ["UpdateDatabase"] = { type ='value', description = "int (counter, database updates due to image storage)", valuetype = nil, },
        ["AddedDatabase"] = { type ='value', description = "int (counter, database adds due to image storage)", valuetype = nil, },
        ["NKIPrivateCompress"] = { type ='value', description = "int (counter, compression/decompression)", valuetype = nil, },
        ["NKIPrivateDecompress"] = { type ='value', description = "int (counter, compression/decompression)", valuetype = nil, },
        ["DownSizeImage"] = { type ='value', description = "int (counter, images downsized)", valuetype = nil, },
        ["DecompressJpeg"] = { type ='value', description = "int (counter, compression/decompression)", valuetype = nil, },
        ["CompressJpeg"] = { type ='value', description = "int (counter, compression/decompression)", valuetype = nil, },
        ["DecompressJpeg2000"] = { type ='value', description = "int (counter, compression/decompression)", valuetype = nil, },
        ["CompressJpeg2000"] = { type ='value', description = "int (counter, compression/decompression)", valuetype = nil, },
        ["DecompressedRLE"] = { type ='value', description = "int (counter, compression/decompression)", valuetype = nil, },
        ["DePlaned"] = { type ='value', description = "int (counter, compression/decompression)", valuetype = nil, },
        ["DePaletted"] = { type ='value', description = "int (counter, compression/decompression)", valuetype = nil, },
        ["RecompressTime"] = { type ='value', description = "int (compression/decompression time in s)", valuetype = nil, },

        ["gpps"] = { type ='value', description = "int (counter, dicom.ini reads)", valuetype = nil, },
        ["gppstime"] = { type ='value', description = "int (dicom.ini read time in s)", valuetype = nil, },
        ["ThreadCount"] = { type ='value', description = "int (counter, number of run threads)", valuetype = nil, },
        ["OpenThreadCount"] = { type ='value', description = "int (counter, number of now open threads)", valuetype = nil, },

        ["StatusString"] = { type ='value', description = "string (read only status of submission operation)", valuetype = nil, },

        ["NoDicomCheck"] = { type ='value', description = "int (config, ignore parse errors if set)", valuetype = nil, },
        ["DebugLevel"] = { type ='value', description = "int (config, 0..4 is more and more info)", valuetype = nil, },
        ["EnableReadAheadThread"] = { type ='value', description = "int (config #images read before needed)", valuetype = nil, },
        ["WorkListMode"] = { type ='value', description = "int (1=optional worklist update, 2=compulsory worklist update)", valuetype = nil, },
        ["StorageFailedErrorCode"] = { type ='value', description = "int (returned DICOM error code)", valuetype = nil, },
        ["FailHoldOff"] = { type ='value', description = "int (config, time s exportconverter is hold of after failure)", valuetype = nil, },
        ["RetryDelay"] = { type ='value', description = "int (config, time s after which failed exportconverter is retried)", valuetype = nil, },
        ["QueueSize"] = { type ='value', description = "int (read only, size of exportconverter queue)", valuetype = nil, },
        ["ForwardCollectDelay"] = { type ='value', description = "int (s wait before sending on e.g. forward series to)", valuetype = nil, },
        ["CacheVirtualData"] = { type ='value', description = "bool (defautl 1, if 0 virtualserver data deleted after use)", valuetype = nil, },
        ["gJpegQuality"] = { type ='value', description = "int (Default lossy jpeg quality setting 0..100)", valuetype = nil, },
        ["gUseOpenJpeg"] = { type ='value', description = "int (If set, uses different jpeg library, not available on most systems)", valuetype = nil, },
        ["FixKodak"] = { type ='value', description = "int (If 1, removes leading zero of 8 number patid)", valuetype = nil, },
        ["NumIndexing"] = { type ='value', description = "int (read only, <0 when dbaseiii indexing done)", valuetype = nil, },
        ["DoubleBackSlashToDB"] = { type ='value', description = "int (backslash handling for db)", valuetype = nil, },
        ["UseEscapeStringConstants"] = { type ='value', description = "bool (enable escape string E'' for db)", valuetype = nil, },
        ["EnableComputedFields"] = { type ='value', description = "bool (if set, server will compute e.g. NumberofSeriesRelatedInstances if queried)", valuetype = nil, },
	["AllowEmptyPatientID"] = { type ='value', description = "bool (if set, server will gracefulyl handle images without patientid)", valuetype = nil, },

        ["FileCompressMode"] = { type ='value', description = "string (obsolete, 1..4 is selects NKI compression)", valuetype = nil, },
        ["BaseDir"] = { type ='value', description = "string (folder of server with trailing dir separator)", valuetype = nil, },
        ["DGATEVERSION"] = { type ='value', description = "string (version of conquest core)", valuetype = nil, },
        ["RootConfig"] = { type ='value', description = "string (configuration section used)", valuetype = nil, },
        ["ConfigFile"] = { type ='value', description = "string (read only, full path of dicom.ini)", valuetype = nil, },
        ["DicomDict"] = { type ='value', description = "string (read only, full path of dgate.dic)", valuetype = nil, },
        ["AutoRoutePipe"] = { type ='value', description = "string (obsolete)", valuetype = nil, },
        ["AutoRouteExec"] = { type ='value', description = "string (obsolete)", valuetype = nil, },
        ["DroppedFileCompression"] = { type ='value', description = "string (config, compression code for dropped files)", valuetype = nil, },
        ["IncomingCompression"] = { type ='value', description = "string (config, compression code for incoming files)", valuetype = nil, },
        ["ArchiveCompression"] = { type ='value', description = "string (config, compression code when archiving files)", valuetype = nil, },
        ["TestMode"] = { type ='value', description = "string (config, string will be appended to each dicom file name generated)", valuetype = nil, },
	}
  },
  Command = {
    type = "value",
    description = "Command object of current DICOM command",
    childs = {
	["AffectedSOPClassUID"] = { type ='value', description = "string", valuetype = nil, },
	["AffectedSOPInstanceUID"] = { type ='value', description = "string", valuetype = nil, },
	["CommandField"] = { type ='value', description = "int", valuetype = nil, },
	["MessageID"] = { type ='value', description = "int", valuetype = nil, },
	["MessageIDBeingRespondedTo"] = { type ='value', description = "int", valuetype = nil, },
        ["DataSetType"] = { type ='value', description = "int", valuetype = nil, },
	["MoveDestination"] = { type ='value', description = "string", valuetype = nil, },
        ["TransferSyntaxUID"] = { type ='value', description = "string", valuetype = nil, },
	["MoveOriginatorApplicationEntityTitle"] = { type ='value', description = "string", valuetype = nil, },
	["MoveOriginatorMessageID"] = { type ='value', description = "int", valuetype = nil, },
        Write = { args = "(filename: string)", description = "write dicom object",  returns = "()",  type = "function", },
        Dump = { args = "(filename: string)", description = "write dicom object header as text file",  returns = "()",  type = "function", },
    }
  },
  Data = {
    type = "value",
    description = "Data object of current DICOM command",
    valuetype = "DicomObject",
  },
  DicomObject = {
    type = "class",
    description = "DICOM Object (e.g., image)",
    childs = {
	new       = { type = 'method', description = "constructor", args = "()", returns = "(DicomObject)", valuetype = "DicomObject",},
	newarray  = { type = 'method', description = "constructor", args = "()", returns = "(DicomArray)", valuetype = "DicomArray",},
	free      = { type = 'method', description = "destructor", args = "()", returns = "()", valuetype = nil,},
        ["Storage"] = { type ='value', description = "string e.g. 'MAG0'; preferred storage device - works only in importconverters", valuetype = nil, },
	["Script"] = { type = 'method', description = "execute conquest style script on Data", args = "(script_code: string)", returns = "()", },

        GetPixel = { args = "(x: int, y: int, frame: int)", description = "get pixel from dicom object",  returns = "(int: one or more pixel values)", type = "method", },
        SetPixel = { args = "(x: int, y: int, frame: int, pixel: int, ...)", description = "set pixel in dicom object",  returns = "()",  type = "method", },
        GetRow = { args = "(y: int, frame: int)", description = "get row of pixels from dicom object",  returns = "(table: array of pixel values)", type = "method", },
        SetRow = { args = "(y: int, frame: int, a: table)", description = "set row of pixels in dicom object",  returns = "()",  type = "method", },
        GetColumn = { args = "(x: int, frame: int)", description = "get column of pixels from dicom object",  returns = "(table: array of pixel values)", type = "method", },
        SetColumn = { args = "(x: int, frame: int, a: table)", description = "set column of pixels in dicom object",  returns = "()",  type = "method", },
        GetImage = { args = "(frame: int)", description = "get entire image from dicom object",  returns = "(string: binary pixel data)", type = "method", },
        SetImage = { args = "(frame: int, a: string)", description = "set 2D image in dicom object",  returns = "()",  type = "method", },
        Read = { args = "(filename: string)", description = "read dicom object; NOTE: Data:Read not allowed! use readdicom() instead",  returns = "()",  type = "method", },
        Write = { args = "(filename: string)", description = "write dicom object",  returns = "()",  type = "method", },
        Dump = { args = "(filename: string)", description = "write dicom object header as text file",  returns = "()",  type = "method", },
        GetVR = { args = "(group: int, element: int, asstring: boolean)", description = "get vr as byte sequence from dicom object",  returns = "(DicomArray/table/string: sequence/array of vr values/binary data)", type = "method", },
        SetVR = { args = "(group: int, element: int, a: table/string)", description = "set vr from DicomArray/table of bytes/binary string/json string(for sequence)",  returns = "()",  type = "method", },
	AddImage = { args = "()", description = "add object into dicom server",  returns = "()",  type = "method", },
	Copy = { args = "(json_string)", description = "returns copy of dicom object optionally augmented with json string",  returns = "(DicomObject)",  type = "method", },
	Compress = { args = "(string)", description = "returns compressed copy of dicom object",  returns = "(DicomObject)",  type = "method", },
	DeleteFromSequence = { args = "(name: string, n: integer (starts at 0))", description = "delete item from named sequence",  returns = "()",  type = "method", },
	ListItems = { args = "()", description = "List all VRs in dicom object",  returns = "(names, types, groups, elements: string(| separated) )",  type = "method", },
	Serialize = { args = "(json: boolean=false, includepixeldata: boolean=false)", description = "return object in lua or json syntax, e.g. loadstring('return '..a:Serialize()) converts object to table; or in json format of argument true (default false)",  returns = "(code: string)",  type = "method", },

        -- mini DICOM dictionary
	QueryRetrieveLevel = { type ='value', description = "string", valuetype = nil, },

	PatientID  = { type ='value', description = "string", valuetype = nil, },
	PatientName  = { type ='value', description = "string", valuetype = nil, },
	PatientSex  = { type ='value', description = "string", valuetype = nil, },
	PatientBirthDate  = { type ='value', description = "string", valuetype = nil, },
	StudyInstanceUID = { type ='value', description = "string", valuetype = nil, },
	SeriesInstanceUID = { type ='value', description = "string", valuetype = nil, },
	SOPInstanceUID = { type ='value', description = "string", valuetype = nil, },
	FrameOfReferenceUID = { type ='value', description = "string", valuetype = nil, },
	Modality = { type ='value', description = "string", valuetype = nil, },
	ModalitiesInStudy = { type ='value', description = "string", valuetype = nil, },
	BodyPartExamined = { type ='value', description = "string", valuetype = nil, },
	Manufacturer = { type ='value', description = "string", valuetype = nil, },
	ManufacturerModelName = { type ='value', description = "string", valuetype = nil, },
	StudyDate = { type ='value', description = "string", valuetype = nil, },
	StudyTime = { type ='value', description = "string", valuetype = nil, },
	StudyDescription = { type ='value', description = "string", valuetype = nil, },
	SeriesDate = { type ='value', description = "string", valuetype = nil, },
	SeriesTime = { type ='value', description = "string", valuetype = nil, },
	SeriesNumber = { type ='value', description = "string", valuetype = nil, },
	SeriesDescription = { type ='value', description = "string", valuetype = nil, },
	ImageNumber = { type ='value', description = "string", valuetype = nil, },
	ImageDescription = { type ='value', description = "string", valuetype = nil, },
	AccessionNumber = { type ='value', description = "string", valuetype = nil, },
	InstitutionName = { type ='value', description = "string", valuetype = nil, },
	ReferringPhysicianName = { type ='value', description = "string", valuetype = nil, },
	PerformingPhysicianName = { type ='value', description = "string", valuetype = nil, },
	StudyID = { type ='value', description = "string", valuetype = nil, },
	InstanceCreatorUID = { type ='value', description = "string", valuetype = nil, },
	InstitutionAddress = { type ='value', description = "string", valuetype = nil, },
	StationName = { type ='value', description = "string", valuetype = nil, },
	InstitutionalDepartmentName = { type ='value', description = "string", valuetype = nil, },
	PhysiciansOfRecord = { type ='value', description = "string", valuetype = nil, },
	PhysicianReadingStudy = { type ='value', description = "string", valuetype = nil, },
	OperatorName = { type ='value', description = "string", valuetype = nil, },
	AdmittingDiagnosisDescription = { type ='value', description = "string", valuetype = nil, },
	DerivationDescription = { type ='value', description = "string", valuetype = nil, },
	OtherPatientID = { type ='value', description = "string", valuetype = nil, },
	OtherPatientName = { type ='value', description = "string", valuetype = nil, },
	DeviceSerialNumber = { type ='value', description = "string", valuetype = nil, },
	ProtocolName = { type ='value', description = "string", valuetype = nil, },
	ImageComments = { type ='value', description = "string", valuetype = nil, },
	PatientComments = { type ='value', description = "string", valuetype = nil, },
	StudyComments = { type ='value', description = "string", valuetype = nil, },
	ScheduledPerformingPhysicianName = { type ='value', description = "string", valuetype = nil, },
	ScheduledPerformingPhysicianName = { type ='value', description = "string", valuetype = nil, },
	InstitutionCodeSequence = { type ='value', description = "string", valuetype = "DicomObject", },
	ReferencedStudySequence = { type ='value', description = "string", valuetype = "DicomObject", },
    }
  },
  DicomArray = {
    type = "class",
    description = "Returns DICOM sequence = array of DICOM objects, indexed from 0",
    childs = {
	free      = { type = 'method', description = "destructor", args = "()", returns = "()", valuetype = nil,},
	Delete    = { type = 'method', description = "delete sequence item", args = "(n: integer (starts at 0))", returns = "()", valuetype = nil,},
	Serialize = { args = "(json=false, includepixeldata=false, dicomweb=false)", description = "return array in lua or json syntax, e.g. loadstring('return '..a:Serialize()) converts array to table",  returns = "DicomArray",  type = "method", },
	[0]       = { description = "element", valuetype = "DicomObject",},
    }
  },
  newdicomobject = {
    type = "function",
    description = "Returns an empty DICOM object or convert json string to DICOM object",
    args = "(nil|json_string)",
    returns = "DicomObject",
    valuetype = "DicomObject",
  },
  deletedicomobject = {
    type = "function",
    description = "Forces manual freeing of DICOM object (optional)",
    args = "(DicomObject)",
    returns = "()",
  },
  copydicomobject = {
    type = "function",
    description = "return copy of dicom object, optionally augmented with json object",
    args = "(DicomObject,json_string)",
    returns = "(DicomObject)",
  },
  compressdicomobject = {
    type = "function",
    description = "return compressed copy of dicom object",
    args = "(DicomObject, string)",
    returns = "(DicomObject)",
  },
  newdicomarray = {
    type = "function",
    description = "Return an empty DICOM sequence, counting from [0]",
    args = "()",
    returns = "(DicomArray)",
    valuetype = "DicomArray",
  },
  print = {
    type = "function",
    description = "Receives any number of arguments and prints their values to the server console, using the tostring function to convert each argument to a string.\n\nprint is not intended for formatted output, but only as a quick way to show a value, for instance for debugging. For complete control over the output, use string.format and io.write.",
    args = "(...)",
    returns = "()",
  },
  debuglog = {
    type = "function",
    description = "Receives any number of arguments and prints their values to the server console if debug logging enabled.",
    args = "(...)",
    returns = "()",
  },
  gpps = {
  args = "(section: string, key: string, default: string)",
  description = "Reads any item from configfile DICOM.INI.",
  returns = "(string)",
  type = "function"
  },
  dictionary = {
  args = "(group: int, element: in) or (name: string)",
  description = "Access DICOM dictionary DGATE.DIC.",
  returns = "(name or group,element)",
  type = "function"
  },
  get_sqldef = {
  args = "(database: integer, field: integer)",
  description = "Reads any item from configfile DICOM.SQL.",
  returns = "(group, element, FieldName, Length, SQLType, DicomType)",
  type = "function"
  },
  script = {
  args = "(script_code: string)",
  description = "Sends conquest style script to dicom server, e.g. 'forward to AE'. " ..
                "Special functions are: script('retry') for RejectedImageWorkListConverter0 and RejectedImageConverter0; will re-attempt to store the object after the script is done, "..
		"script('defer') for ExportConverter: will cause later retry, and script('destroy') for query/store or move: will cancel operation.",
  returns = "()",
  type = "function"
  },
  destroy = {
  args = "()",
  description = "For query/store or move: will cancel operation or delete incoming or returned image or query record.",
  returns = "()",
  type = "function"
  },
  retry = {
  args = "()",
  description = "For RejectedImageConverter0: will retry with modified image.",
  returns = "()",
  type = "function"
  },
  reject = {
  args = "()",
  description = "For ImportConverters: will reject incoming image.",
  returns = "()",
  type = "function"
  },
  tempfile = {
  args = "(string)",
  description = "Create temp filename with string as extension (pass . as well).",
  returns = "(string)",
  type = "function"
  },
  servercommand = {
  args = "(command_code: string, mode:string(e.g. 'cgi', 'cgibinary', 'cgihtml', 'binary', '<filename' {upload}, '>filename' {download}",
  description = "Sends conquest server command, e.g. 'display_status:', 'get_param:MyACRNema' or 'lua:chunk'; For binary data write to tempfile and set returnfile to its name.",
  returns = "(string)",
  type = "function"
  },
  system = {
  args = "(program: string)",
  description = "run program in the background",
  returns = "()",
  type = "function"
  },
  mkdir = {
  args = "(path: string)",
  description = "make directories (recursive)",
  returns = "()",
  type = "function"
  },
  sleep = {
  args = "(ms: integer)",
  description = "sleep a number of ms",
  returns = "()",
  type = "function"
  },
  tickcount = {
  args = "()",
  description = "returns elapsed ms",
  returns = "(integer)",
  type = "function"
  },
  get_amap = {
  args = "(entry: int)",
  description = "Reads item from acrnema.map",
  returns = "(AE: string, IP: string, port: string, compression: string)",
  type = "function"
  },
  dbquery = {
  args = "(database: string, fields: string, query: string, sort: string)",
  description = "Executes SQL query on database",
  returns = "(table of records from 1 with table of fields from 1)",
  type = "function"
  },
  getpixel = {
  args = "(x: int, y: int, frame: int)",
  description = "get pixel from dicom object",
  returns = "(int: one or more pixel values)",
  type = "function"
  },
  setpixel = {
  args = "(x: int, y: int, frame: int, pixel: int, ...)",
  description = "set pixel in dicom object",
  returns = "()",
  type = "function"
  },
  getrow = {
  args = "(y: int, frame: int)",
  description = "get row of pixels from dicom object",
  returns = "(table: array of pixel values)",
  type = "function",
  },
  setrow = {
  args = "(y: int, frame: int, a: table)",
  description = "set row of pixels in dicom object",
  returns = "()",
  type = "function"
  },
  getcolumn = {
  args = "(x: int, frame: int)",
  description = "get column of pixels from dicom object",
  returns = "(table: array of pixel values)",
  type = "function",
  },
  setcolumn = {
  args = "(x: int, frame: int, a: table)",
  description = "set column of pixels in dicom object",
  returns = "()",
  type = "function"
  },
  getimage = {
  args = "( frame: int)",
  description = "get image from dicom object",
  returns = "(string: binary image data)",
  type = "function",
  },
  setimage = {
  args = "(frame: int, a: string)",
  description = "set entire image in dicom object",
  returns = "()",
  type = "function"
  },
  getvr = {
  args = "(group: int, element: int, asstring:boolean)",
  description = "get VR as DicomArray/byte array/binary string from dicom object",
  returns = "(DicomArray/table/string of VR values)",
  type = "function",
  },
  setvr = {
  args = "(group: int, element: int, a: DicomArray/table/string/json string(for sequence))",
  description = "set VR as sequence or binary in dicom object",
  returns = "()",
  type = "function"
  },
  readdicom = {
  args = "(filename: string)",
  description = "read dicom object",
  returns = "()",
  type = "function"
  },
  writedicom = {
  args = "(filename: string)",
  description = "write dicom object",
  returns = "()",
  type = "function"
  },
  writeheader = {
  args = "(filename: string)",
  description = "write dicom object header as text file",
  returns = "()",
  type = "function"
  },
  dicomquery = {
  args = "(AE: string, level: string, query: userdata)",
  description = "query DICOM server",
  returns = "(DicomArray, counting from 0)",
  valuetype = "DicomArray",
  type = "function"
  },
  dicomquery2 = {
  args = "(AE: string, level: string, query: userdata)",
  description = "query DICOM server old",
  returns = "(DicomArray, counting from 0)",
  valuetype = "DicomArray",
  type = "function"
  },
  dicommove = {
  args = "(AE: string, dest:string, query: DicomObject, patientroot: number(1=yes), callback: string, extraparameters: DicomObject)",
  description = "move data from DICOM server to DICOM server, extraparameters can replace or add any command tags",
  returns = "error string or nil",
  type = "function"
  },
  dicomget = {
  args = "(AE: string, level: string, query: DicomObject)",
  description = "get objects from DICOM server",
  returns = "(DicomArray, counting from 0)",
  valuetype = "DicomArray",
  type = "function"
  },
  dicomread = {
  args = "(query: DicomObject)",
  description = "get objects from local DICOM server",
  returns = "(DicomArray, counting from 0)",
  valuetype = "DicomArray",
  type = "function"
  },
  dicomecho = {
  args = "(AE: string, extra: DicomObject)",
  description = "echo DICOM server by AE or ip:port; return null if failed",
  returns = "(DicomObject, raw response from echo)",
  type = "function"
  },
  dicomdelete = {
  args = "(query: DicomObject)",
  description = "delete data from local DICOM server",
  returns = "()",
  type = "function"
  },
  dicomprint = {
  args = "(image(s): DicomObject/DicomArray, AE: string, annotation_format: string, callback)",
  description = "print object(s) on DICOM printer",
  returns = "error string",
  type = "function"
  },
  dicomstore = {
  args = "(AE: string, image(s): DicomObject/DicomArray)",
  description = "store object(s) on DICOM archive",
  returns = "dicom result array",
  type = "function"
  },
  newdicomdelete = {
  args = "(query: DicomObject, threadnum: integer)",
  description = "delete data from local DICOM server, threadnum only used for progress information",
  returns = "()",
  type = "function"
  },
  newdicommodify = {
  args = "(query: DicomObject, script:string; threadnum: integer)",
  description = "modify data in local DICOM server, script e.g. lua:Data.PatientID='aap', threadnum only used for progress information",
  returns = "()",
  type = "function"
  },
  newdicomcopy = {
  args = "(query: DicomObject, script:string; threadnum: integer)",
  description = "copies data in local DICOM server, script e.g. newuids;lua:Data.PatientID='aap', threadnum only used for progress information",
  returns = "()",
  type = "function"
  },
  heapinfo = {
  args = "()",
  description = "string listing allocated data in server",
  returns = "(string)",
  type = "function"
  },
  sql = {
  args = "(statement: string)",
  description = "execute SQL statement",
  returns = "()",
  type = "function"
  },
  genuid = {
  args = "()",
  description = "returns new UID",
  returns = "(string)",
  type = "function"
  },
  changeuid = {
  args = "(olduid: string[, proposeduid: string][, stage: string][, type: string])",
  description = "Consistently modify an UID, returns mapped UID, stage names anonymizer (#name for MD5), type stored in database",
  returns = "(string)",
  type = "function"
  },
  changeuidback = {
  args = "(newuid: string[, stage: string][, type: string])",
  description = "For a modified UID, returns the original if exists",
  returns = "(string or nil)",
  type = "function"
  },
  md5 = 
  {
  args = "(data: string)",
  description = "returns md5 hash of string",
  returns = "(string)",
  type = "function"
  },
  addimage = {
  args = "(image: DicomObject)",
  description = "add image to local DICOM server",
  returns = "()",
  type = "function"
  },
  listoldestpatients = {
  args = "(max: integer, device:string, sort:string)",
  description = "list ID of N=max patients with oldest parameter 'sort', optionally on device",
  returns = "(table)",
  type = "function"
  },
  ConvertBinaryData = {
  args = "(format: string, data: string or table)",
  description = "convert table to binary string or vice-verse",
  returns = "(string or table)",
  type = "function"
  },
  HTML = {
  args = "(text: string, ...)",
  description = "web server only: output HTML text from CGI script",
  returns = "()",
  type = "function"
  },
  CGI = {
  args = "(key: string, default: string='')",
  description = "web server only: read url entry from CGI script, CGI() reads posted data",
  returns = "(string)",
  type = "function"
  },
  crc = {
  args = "(value: string)",
  description = "calculate CRC of passed string",
  returns = "(string)",
  type = "function"
  },
  serialize = { 
  args = "(object: DicomObject or DicomArray, json: boolean=false, includepixeldata: boolean=false, dicomweb: boolean=false", 
  description = "return object in lua or json syntax, e.g. loadstring('return '..serialize(a)) converts object to table",
  returns = "(code: string)",  
  type = "method", 
  },
  listitems = { 
  args = "(object: DicomObject)", 
  description = "List all VRs in dicom object",  
  returns = "(names, types, groups, elements: string(| separated) )",  
  type = "method", 
  },
}
