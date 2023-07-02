/*
MvH 19980327: Disable logfile for speed and save disk
MvH 19980327: -! Enables logging to Personal pacs console; RunServer works; -p breaks
MvH 19980327: All printf's to systemdebug
MvH 19980320: Integrated dumppacs code here for convenience
MvH 19980320: Fixed -p option; SaveToDisk returns filename, printed in stats
MvH 19980320: Documented and added print of usage
MvH 19980404: Added test options, SQL config option and command line specified console pipe/udp
MvH 19980409: added FileCompressMode option; use local save to disk code instead of pdu member
MvH+LJZ 19980409: Return on failed GenerateFileName in SaveToDisk
MvH 19980409: Cleaned up nki private compression code and allow >15 bit differences
MvH 19980410: Added run length encoding for equal pixels to compressor; fix 3-byte case
MvH 19980415: Cleaned up the messages; -! does not enable debug; do not enable accessupdates (use aroute)
MvH 19980618: Some work on manual and database layout listing
MvH 19980621: Added -z option: zap patient
MvH 19980624: Comm test also produces systemdebug and long text output; added \n to some output
MvH 19980625: SaveDicomDataObject in loadddo.cpp supports NKI compression
MvH 19980704: Added some test code for counting disk space and renaming device for patient
MvH 19980705: Added CACHE devices in free store check; added FindPhysicalDevice (searches for file)
MvH 19980709: Added archiving options: -as, -au, -ab, -ac, -ad
MvH 19981217: Temporary use PDU.Save... code for testing DICOM objects (does not compress)
MvH 19981218: Temp use Pdu.Load as well (no compression)
MvH 19981228: Disabled ljz's code until it has been completed
ljz 19990108: Reenabled ljz's code: Replace uncompressed by compressed pixeldata VR in
              SaveToDisk (it seemed to work anyway...)
MvH 19990109: Removed some commented out code; added comments for alternative load and save routines
	      Added Regen of specified device
MvH 19990110: Added n, for MAGn, as parameter in -as and -ab archiving options (default all)
	      exit(1) on failure
MvH 19990111: Fixed exit code on -ar option
MvH 19990117: If the filename is *.dcm (depends on FileNameSyntax) use uncompressed chapter 10 format
ljz 19990317: Added NKI-private stuff
MvH 19990628: Temporary enabled debug in -!nnnn option
MvH 19990630: Added MIRROR option; disabled debug in -!nnnn option
MvH 19990707: Fixed MIRROR option; added MIRROR message; added check on FindPhysicalDevice result
ljz 19990817: Added ability to spawn ExportConverters at the end of SaveToDisk
MvH 19990827: Optimized read of slices: cache a DB to pass to GetFileName (10% faster)
ljz 19991117: Added parameter FileCompressMode to in nki_private_compress call
mvh 20000318: Added private command object 9999, 0300 = print to console
              Added private command object 9999, 0400 = force C-Echo to be 'silent' (no log)
              Display C-Move Destination; print time info about threads
ljz 20000323: Serious fix: MainThread 'hangs' when several associations occur simultaneously, caused
	      by compiler-optimisation. Solution: DriverApp.Lock should be 'volatile'.
mvh 20000329: On request by ljz: designed crash of server if silent VR receives 'DEAD'
ljz 20000501: Compare dicom modality with UPPER-case ini-file setting (ExportConverters)
ljz 20000221: Initialize TroubleLogFile and UserLogFile; log C-STORE and C-MOVE
ljz 20000703: Logging of trouble now starts with '***'
mvh 20000703: Exclude archival messages from trouble log; removed force dcm code
mvh 20001104: Added WINAPI to thread routines
mvh 20001105: Replaced malloc by new where a mix could occur (vr->data)
              Made compilable for MSC and BC and WC using some #ifdefs
              Changed order of verification and store; better localized new/delete DDO more
mvh 20001106: Run-time load odbcinst routine for BC and WC, use spawn... i/s/o _spawn...           
mvh 20001106: Use delete [] operation for vr->Data
mvh 20010328: Added -av: verify mirror disk
mvh 20010415: Added DeleteSeries, DeleteStudy and DeleteImage routines and -f options
mvh 20010416: Added -f options: -feFile = enter MAGN file into DB, -fdFile = delete file from DB
	      -faFile = add (also makes copy) file into server, -f?=ask UID of file, 
	      -fu = generate UID, -fcNEWPATID,file = modify patient ID of image file (moves file!)
	      NOTE: -fc is irreversible in the sense that if a portion of a study/series is changed
              and changed back it winds up as a different study/series. This occurs because the 
	      UIDs must be changed for each image modification. This problem does not occur when 
	      a whole series or study is changed
mvh 20010417: Simplified code to change uids to allow easy extension to referenced SOPs
              Check after load on common UIDs to avoid accepting dropped non-dicom files
              NOTE: ChangeUIDinDDO should also change sequences and referenced SOPs (0008,1155)
mvh 20010418: Fix in above change
mvh 20010418: Changed names of database fields in UIDMODS - did not work on msaccess
mvh 20010419: Added -fk option: copy image file to other patient ID (as -fc but no delete)
              Added version number 1.3.9 to command line help
mvh 20010429: Added -fz option: zap file from server and database
mvh 20010430: Some work on read ahead thread: note that thread must not use any pointer copy!
mvh 20010502: Made read ahead thread working; fixed thread counter; fix odbc option: strings too short
mvh 20020506: Fixed handle leak in read ahead thread
mvh 20020507: Fixed memory leak in read ahead thread; kbhit() test #ifdef DO_LEAK_DETECTION
mvh 20010509: Added RenameDevice is -ae option
ljz 20010730: Replaced parameter argv[0] of ExportConverters to proper value
mvh 20010829: Added ExportStationName as export filter; default Export... value set to "*"
              Made export comparisons case insensitive; added special 'forward to AE' ExportConverter
              Version to 1.3.10
mvh 20010831: Added extended syntax for exportconverters with %f=filename, %m=modality, 
              %s=stationname, %b=file base name, %p=file path; 
              Also allows 'copy %f destination', but for windows NT only (uses cmd.exe)
              Added ExportFilter option that uses an SQL statement to filter.
mvh 20010901: Added a simple queue to process exportconverters asynchroniously
mvh 20010901: Create a queue for each converter; queue mirror copy requests
mvh 20010902: Added %o = SOP; %n = newline; added the following hard coded converters: 
              append "text" to file; write "text" to file; copy file to file; copy file to dir
ljz 20010904: Re-entered change of 20010730
ljz 20010905: Fix: After 'SaveToDisk', do not free pDDO
mvh 20011109: Fix: made ExportConverter stuff case insensitive
              Note: empty filter tests for empty contents - this is not the same as '*'
              Released 1.3.10
mvh 20011110: Note: in release 1.3.10, the " to " in e.g., write "%f%n" to file must be lowercase
mvh 20011115: Made " to" case insensitive; Added *** before some error messages for better logging
              Removed cache db: would hang the server if SQL has gone down and up.
              Now use one DB per thread which is passed to the read ahead thread = fast too!
ljz 20011207: Added extra errormessage when creation of datasource fails
mvh 20020109: Disabled this code for watcom or borland (instead of run-time loading an address)
mvh 20020314: Implemented ignore of C-CANCEL request for ADAC NM (was error before)
              Blocked out SQLInstallerError call; does not compile on clinical MS4.2
mvh 20020413: Added some notes about thread safety and db's: The serverchild is not thread 
              safe when reading and writing is done on the same association.
mvh 20020415: Added -atDEVICE option: sanity check of images on disk; version to 1.3.11
mvh 20020416: Made UID generator configurable through UIDPrefix in dicom.ini; made time print unsigned
mvh 20020422: Sleazy fix of ModifyPatID: time(null) created same UID: now wait 1 s per changed file
mvh 20020426: Generate filename no longer gives an error: is done at higher level
mvh 20020428: Only serious error messages use *** in message
mvh 20020428: Added FAILSAFE_STORAGE: tries to save incoming files even when DB errors occur
mvh 20020429: Put SaveToDisk in myUnknownStorage: pass DB failures to caller
              Disabled FAILSAFE_STORAGE: any storage failures should now be detected by client
mvh 20020508: Do not rewrite image not on MAG but it is not an error (warning is generated earlier)
mvh 20020520: Added -gSERVER,DATE option: grab images on date from remote server
mvh 20020521: fix displaying junk filename when rewriting image not on MAG 
ljz 20020522: Fix in SetStringVR: strings must be padded with a space
mvh 20020529: -i and -r now have mode parameter (0=normal and default, 1=simple, e.g., for DBASEIII)
ljz 20020531: In 'QualifyOn', support case-sensitive call to 'GetACRNema', but make AE-Title
              uppercase after retrieving IP and port.
ljz 20020613: Better handling of Compression ON/OFF in 'SaveToDisk'
	      Fixed crash in DeleteImageFile (twice free of VR->Data)
mvh 20020613: compress to specified mode, check (de)compress result, decompresses data before 
              forward; added 'export compressed to AE' export; -fa has optional ,NEWPATID syntax; 
mvh 20020802: Simplified rmdir code and allow multiple subdirectories in rmdir and mkdir code
              Version to 1.3.12 (unreleased)
mvh 20020804: Use PATHSEPCHAR where possible
mvh 20020807: Changed SaveToDisk order: only make patientdir when DB save succeeded
mvh 20020812: Added skeleton of printer support
mvh 20020816: Added color imagebox; Save incoming print DDO as file
mvh 20020819: UIDs of imageboxes end on rows.cols.position for easy print handling
mvh 20020819: Fix in delete unexistent mirror files for changepatientUID
              Extract pixel data from sequence in ImageBox; added sleep(10) in -fk option
mvh 20020819: Added better UID generation (but gets close to 64 chars)
mvh 20020821: Fix printer UID generation; display page orientation and film #; shorter info
mvh 20021016: Added (indexed) patid to exportfilter query and getfilename for speed; 
              The option tests for rev5 db or higher; increase 64 strings to 66;
              Note: the GrabImagesFromServer option does not yet pass patid to the query 
	      (i.e., will be slow with non-odbc driver)
mvh 20021017: Control NeedPack from here; added -c option: set UID counter at startup
              Removed Sleep from anonimize options; do not re-create UIDMODS
mvh 20021018: Pass NoKill through GenerateFilename for interactive DGATE
mvh 20021028: Fix query on study in PatientStudyOnlyQuery (did an image query)
mvh 20021105: Revamped read-ahead thread logic to try fix rare crash where ratd->DDOOutPtr became NULL
mvh 20021115: Use Generic style retrieve classes with NKIqrsop code instead of older qrsop code
              Advantages: read ahead thread used, and allows print of error messages from C-MOVE
              Required changes such that ratd->pcdo can be NULL.
mvh 20021121: Disabled read-ahead thread for time being
mvh 20021212: Also loop ServerApp.Server(Port) in threaded mode server
mvh 20021213: Re-entered fix by Hanne Kooy: compressed forwarding had extra ;
mvh 20021213: Re-enabled use of generic query classes with read-ahead thread; 
	      but made read-ahead thread configurable with "EnableReadAheadThread"
              Made grab query faster by including date - note grab is independent of FixPhilips
mvh 20021215: Found read-ahead thread problem reported by Aaron Cann - missing Sleep in wait for slices
mvh 20021223: Version to 1.4.1 pre-release; added in dgatefn forbidden filename chars suggest by Thomas Buerer
mvh 20030113: Removed loop ServerApp.Server(Port): gave endless loop error messages
ljz 20030120: Added cleanup-code when DO_LEAK_DETECTION is defined for easy leak checking
mvh+ljz 20030205: Fix bug found by Frank-Andre Siebert: read ahead thread crashed when ID or SOP is missing
                  Also fix 2 other potential missing VR problems
mvh 20030228: Fixed bug reported by Jeff Bacbcock: series retrieve from E-film crashed (patid not passed)
ljz 20030424: Version to 1.3.12
	      Changed calls to obsolete PDU.Write(&DDO)
	      In 'SaveToDisk' the call to PDU.SaveDICOMDataObject now has parameterDICOM_CHAPTER_10_EXPLICIT
	      (which in writes IMPLICIT files iff not compressed)
ljz 20030606: Fixed incremental counter when creating UIDs or generating filenames
	      Fix: QueryRetrieveLevel is mandatory for a QueryResponse
mvh 20030625: Fix NGET of BasicPrinterSOPClass
ljz 20030625: Fix in above fix
mvh 20030627: Adapted for MSC4.2; Finally really fix NGET of BasicPrinterSOPClass (wrong UID used)
mvh 20030629: Added -nd, -nc, -jd and -jc options: (de)compress NKI; (de)compress JPEG (used OFFIS executables)
mvh 20030630: Started on compression for dropped, incoming, transmitted; NKI vr 0x9999,0x700=custom compression
              Added 'forward compressed as xx to ...'
mvh 20030701: QualifyOn now also has compression parameter
mvh 20030703: Moved compression code to nkiqrsop; dgate -j-xx and -j*xx = test compression options
	      Removed "forward compressed to" because "forward compressed as .. to" is better.
	      Note that FileCompressMode is now obsolete. KNOWN BUG: JPEG compressed images may be 
	      rewritten as V2 (when written originally as NKI) which are then not correctly read.
mvh 20030704: Made that FileCompressMode parameter still works even though is obsolete
              Moved faulty delete DDO after SOPUnknownStorage.Read
mvh 20030705: Added check of Tranfer Syntax next to extension for save type; ProcessDDO takes **DDO
mvh 20030706: Use dgate.dic; Attach VRType to PDU's for implicit little endian support; small fix in dump code
mvh 20030709: Printer support fix; version to 1.4.2; added SendUpperCaseAE parameter (default 0)
ljz 20030730: Force ImplicitLittleEndian, ExplicitLittleEndian and ExplicitBigEndian objects that must
              be stored in Chapter10 format, to ImplicitLittleEndian.
mvh 20030815: Version to 1.4.3
mvh 20030905: Allow longer filenames
mvh 20030910: Added check on rights to write (trouble) log file and printer_files in MAG0
mvh 20030921: Allow write to any MIRROR if MAG is full; added -ff# option: delete until # MB free
mvh 20030922: Added StorageFailedErrorCode (default 0x110 = 272 decimal = failed processing)
              Added ExportCallingAE and ExportCalledAE export filters (also %u=SCU, %c=calledAE in converter)
mvh 20040401: Version to 1.4.4; pass status pointer to compress routines
mvh 20040402: Delete DDOptr when SaveToDisk cannot write but does not give an error
              Added "Cannot rewrite jpeg/rle image in v2 format" check
	      Use system DSN for ODBC (easier login)
mvh 20040406: Always use EXPLICIT syntax for jpeg, fix v2 always use DUMP
mvh 20040426: Version to 1.4.4a
mvh 20040520: Version to 1.4.4b
mvh 20040523: Added patient ID to speed GetFileName for single slice move
ljz 20040602: Fix: 'forward compressed as' always failed
mvh 20040528: Set NeedPack (controls DbaseIII indexing) for regen, archive, maintenance; Version to 1.4.4c
mvh 20040528: Added built-in maintenance commands through silent VR in ECHO; pass patid in DeleteImageFile
mvh 20040530: extract (DBF only) to database like XDicomImages in same directory; Version to 1.4.5
mvh 20040601: Added deletestudy, deleteseries and packdbf maintenance commands
mvh 20040605: packdbf maintenance command sets NeedPack to 3: forces pack
mvh 20040606: fix rewrite to not MAG (JUKEBOX) was NOT ignored
mvh 20040607: Merged fix ljz 20040602; do not allow rename and delete with empty name
mvh 20040610: Added maintenance server tasks for archival and regeneration
mvh 20040612: Added indexdbf maintenance server task
mvh 20040614: dgate command line maintenance options use NeedPack=5: non-threaded index generation
	      Added -am option: move (all) device data; to move selected data use -amMAG0.Archiving,MAG1
mvh 20040624: Version to 1.4.5a
mvh 20040626: Pass study and series UID to getfilename; version to 1.4.5b; Fixed leak of pats array
mvh 20040713: Version to 1.4.6
mvh 20040805: Fix in C-MOVE for single file (fix from ljz); version to 1.4.6b
ljz 20040909: Increased length of ExportFilter (was 64, now 510)
mvh 20041020: Added mergeseries(file) and mergestudy(file) maintenance commands, silenttext to 1024
mvh 20041101: Added deletesopfromdb maintenance command; version to 1.4.7
mvh 20041103: called, e.g., AE~nj overrides IncomingCompression to nj
mvh 20041108: Added $c test compression mode (alt-drop files with $c as patient ID tests compression modes)
mvh 20041109: Added $tSERVER test compressed forward; some fixes in forward compressed to (server name read wrong)
              $s tests all filename syntaxes
mvh 20041112: Added $rN N times repeated entry test, and $tN which enters image N time each on its own thread
mvh 20041128: Added -^file: log to file (timestamped) instead of to server console; -#file: same with debug
mvh 20041129: Added silent commands debuglog_on and log_on to set logging to UDP or file
	      Added silent command read_amap to re-read acrnema.map without restarting the server
mvh 20050102: Small modifications to allow compile through total.cxx
mvh 20050107: Added code to hold and retry queued forwards and mirror copies when failed:
              After failed export/copy, start storing entries in file ExportFailuresN (CopyFailures)
	      if a new export/copy is requested >1 minute after last failed one, try again
              if queue is empty, >100 seconds after last fail, try export/copy stored ones
              processing queued entries has priority over processing stored entries
mvh 20050107: Version to 1.4.8; modifications for linux compile
mvh 20050108: Added server PORT# to fail files, so that multiple servers on different ports do not share files
mvh 20050109: Added configurable TCPIPTimeOut, FailHoldOff, RetryDelay and QueueSize
mvh 20050109: Adapted thread code for linux; still need to implement WaitForSingleObject
mvh 20050111: Added ctype.h (for mingw)
mvh 20050118: Linux: crudely implemented WaitForSingleObject; detach pthreads to avoid leaks; 
              crudely implemented export converter apps
mvh 20050119: Added grabimagesfromserver, regendir and makespace server commands
	      dgate -- sends server commands to other running server; adapted manual
mvh 20050121: Small updates to manual
mvh 20050124: Replaced BackGroundExec with simpler system() for Linux; added query server command, e.g.,
	      dgate "--query:DICOMpatients|patientid,patientname,patientsex|patientsex='F'|%s %s %s|t.t"
              Added read_ini: server command to reread dicom.ini; changes most config except:
              TCPPort (busy), UseBuiltInDecompressor (cached) and FileNameSyntax (cached)
mvh 20050130: added get_param, get_freestore, get_amap, get_sqldef that respond through echo-response
	      also: get_sop, get_transfer, get_application, get_localae, get_remoteae, get_dic, forward
	      added: dump_header (to file), convert_to_gif (to file); adapted query (to file) 
	      where file="cgi" for stdout in cgi/mime mode, file="" to send to stdout as is
	      most GUI commands can now pass PatientID|SOPInstanceUID instead of filename of dicom object
              added put_amap, write_amap, put_sop, put_transfer, put_application, put_localae, put_remoteae, 
	      put_param. Note: get/put_param work through disk (read_ini activates), get/put_amap through 
              memory (write_amap writes to disk), get/put_sop etc read from memory and write to mem and disk.
mvh 20050204: Added delete_param, delete_amap and fix cgi output (double \n)
mvh 20050204  most GUI commands can now pass PatientID|?*:SOPInstanceUID; added delete sop etc commands
	      More fixes in cgi data passing VR vr() is mostly wrong; use VR *vr=new VR()
mvh 20050205: Allow up to 16 output fields in query:, repeat queried fields over all 16 outputs
	      Added counters for server status; start on display_status server command
mvh 20050206: More work at display_status, renamed testtable to xtesttable: default not opened
	      Access ObjectFile and DeviceName through query instead of with GetFileName up to 3 x faster!!
	      Also fixes potential leak in read ahead thread: pats, series and studies not always freed
	      Allow up to 32 cgi query outputs, tidied dump layout, start on web interface (put dgate and ini in cgi-bin)
mvh 20050207: Added query2 option: limits number of output rows as asked; gui command not counted as association
              Some more work for web interface
mvh 20050210: Adapted web interface queries such that they run on sql server
mvh 20050212: Removed need for thread local storage; automatically adapt dgate extension in cgi scripts depending in OS
mvh 20050213: Added more parameters to server configuration web page
mvh 20050214: Added update button to server config (updateconfig page to be implemented)
ljz 20050215: Fix in LoadForGUI: Drag 'n' drop of files in the server did not work anymore
              Fixed dgate crash in case of failure in 'recompress()' (pDDO=NULL when OFFIS failes)
	      mvh merged
mvh 20050327: Added rudimentary gz unzip support for dropped files (uses 7za.exe)
mvh 20050401: Start on Modality WorkList Support
mvh 20050404: List End/Start sequence db types (only supported for worklist)
mvh 20050411: Fix incorrect space after filename and device in C-MOVE internal query
mvh 20050414: Added addrecord server command: for now only option to enter worklist entries
mvh 20050603: Fix servercommand query(2) for empty where part
mvh 20050825: Fix web interface queries for sql server (were OK for dbase)
mvh 20050826: More work on web interface and added worklist there; deleterecord command
mvh 20050827: Fixes in addrecord and query commands, editworklist,saveworklist,deleteworklist
mvh 20050829: More fixes in web interface, disable caching for most/all? of it
mvh 20050831: Allow command line length of up to 4096, allow edit all worklist database fields
mvh 20050901: Removed invalid cache-control: cache; added WorkListMode:0= off, 1=optional, 2=compulsory
mvh 20050902: Read db config for worklist edit page in web interface; use generic webpages for db edit
	      Added HL7Tag in DumpDD
mvh 20050903: Added 'simple' loadhl7: server task implementation and HL7->DICOM translation
              Also addimagefile will enter HL7 files into the server, i.e., drag and drop works
              Fix some web queries for sql server with denormalized tables
mvh 20050904: Some more tweaks to hl7 code; max query formats to 48; max format to 8192
mvh 20050905: Auto split HL7 subfields and repeats without using a list
mvh 20050907: Merged change by Hans-Peter Hellemann: fixes extension problem for linux web 
              interface for some browsers
mvh 20050908: Protect convert_to_gif when file not found
mvh 20051121: Recompute SQLResultLength because it does not work in PostGres BindField
mvh 20051123: put in #ifdef POSTGRES
mvh 20051208: put in #if 0 (fixed in odbci.cpp); start on virtual server: will grab images from 
	      other server as needed in read-ahead thread: when in db but not on disk; for now 
	      use rat also for single image access to avoid code duplication
mvh 20051209: Start on virtual query: image (!) query is passed to other server, results populate db
mvh 20051210: Added "vX" incomingcompression (stores dbf but not image; forwarding disabled)
              Added virtual queries on all queries and moves: now works as transparant file cache
	      Added VirtualServerFor0..9 configuration entries: server merges own data with these AE's
	      Added CacheVirtualData control; if set data not deleted after use (db kept for now)
mvh 20051211: Fix: override query level in virtual query
mvh 20051212: Store virtual server # in device; saved on query, used when retrieving, stripped when storing
mvh 20051214: Use different sleep lenghts under linux - not all sleep(0); small fix in virtual server
mvh 20051215: Implemented a linux version of Sleep using nanosleep

mvh 20051217: Use new VR::ReAlloc method to replace data in VR; added quit: command
mvh 20051218: Use ReplaceVR instead of Push in anonymize etc; also changes in sequences
              Version to 1.4.10; show version and exe date in startup log
              Do not start mirror copy queue unless it is needed; show some thread startup info
              Added -b = NOTHREAD: run server as single process, useful for debugging and profiling
              Fixed doc of --regendir: command
mvh 20051219: Fixed several small leaks - as tested with Linux malloc_stats
mvh 20051222: Started on bypass server for virtual query - not functional yet (cleanup required)
              Removed debugVR logic: errors should now list DCO
mvh 20051228: Fix virtual device logic for VirtualServerFor0; 
              Cleanup vr's and duplicates when bypassing server for virtual query, seems functional
mvh 20051229: NonDestructiveDumpDICOMObject now also list sequences; 
	      DebugLevel>0 lists DCO, >1 lists query DDO; fixed leaks in virtual query and some others
mvh 20051230: Added --debuglevel: command, small fix in NonDestructiveDumpDICOMObject
mvh 20060103: Small fix in virtual query; disabled virtual query for worklist; debuglevel 3 shows some query results
mvh 20060123: Added patient/study/seriesfinder (server parameter ignored for now, later for virtual servers etc)
mvh 20060125: Added serieslister (server parameter ignored for now, later for virtual servers etc)
mvh 20060128: Renamed to imagelister, added serieslister
mvh 20060131: Sort filenames from imagelister on ImageNumber (0020,0013)
mvh 20060226: Version to 1.4.11
mvh 20060228: Small fix in isxdigit thanks to ljz
mvh 20060311: Use ReplaceVR in virtual query - level was sent twice (thanks David Lang)
	      Added WEBReadOnly flag
mvh 20060317: Support calledae and callingae in filenamesyntax - for virtualquery called is always MYACRNEMA
mvh 20060324: Strip group 2 from outgoing c-stores if not compression "as" or "is"
mvh 20060328: Improved forwarder, always propose Verification and use IsAbstractSyntaxAccepted(iUID) to test if host accepts image
mvh 20060402: Fix hang condition when a file failed to read in the read-ahead thread
mvh 20060402: Do not remove transfer syntax prior to dcmdjpeg/dcmcjpeg (here in test recompress only)
mvh 20060405: Tried fix for multiple moves on same assoc (thanks Andrej Savelov)
mvh 20060405: Added message when IOD fails to read
mvh 20060603: Fix crash when started as dgate -v: empty statbuf
mvh 20060607: Finally fixed multiple moves on same assoc (thanks Andrej Savelov and Fred Roener)
mvh 20060618: Renamed dfilecopy and define ODBC_ADD_SYS_DSN if needed
mvh 20060619: Set version to 1.4.12alpha1
mvh 20060621: Added clonedb: command; transfer FULL db from other server to this one using virtualquery
mvh 20060628: AddToDatabase has JustAdd parameter: skip one unnecessary query
mvh 20060701: Speeded filter string handling
mvh 20060702: Pass DB to GenerateFilename to avoid zillions of db open and closes
mvh 20060704: Print number of records found in all queries
mvh 20060708: Set version to 1.4.12alpha
mvh 20061126: Moved servertask temporary files to MAG0/printer_files; protected query: and query2 against empty input
mvh 20061126: Experimental forwarding on same association: close unused association after 5 seconds
              Clean collected sopclasses after one hour of inactivity
	      Note 1: there is a problem with AReleaseRQ: do not use in this task
	      Note 2: maximal 20 forwarders have a remaining association
mvh 20061127: Added ForwardAssociationLevel (PATIENT, SERIES, STUDY, IMAGE, SOPCLASS; default STUDY),
	      ForwardAssociationRefreshDelay (default 3600 s), and ForwardAssociationCloseDelay (default 5 s)
              Limit maximum number of ExportConverters to MAXExportConverters=20 (for static arrays used in forwarders)
	      Forwarders keep association open as long as UID at ForwardAssociationLevel does not change.
              Show filename in forward log; document where fixkodak should operate for virtual queries
mvh 20061128: Added ForwardAssociationLevel GLOBAL: use association until timout or incompatible
              Close ForwardAssociation after send when ForwardAssociationLevel is IMAGE
mvh 20061129: VirtualServerForN may be "server,FIXKODAK" to clean queries (add 0) and their response (remove 0)
	      Hardcoded filters for exportconverters (modality, called, calling, station) can now match e.g., RT*
mvh 20061130: Pass series and study in virtualserver outgoing c-move
mvh 20061213: Fixed nasty bug in query2 and TestFilter: Query without bindfield overwrites previous bound strings!
	      Maybe was problem causing crash on linux with query2: and problem with option -g reportedly stopping incorrectly
mvh 20061219: Version to 1.4.12
mvh 20061231: Added ForwardAssociationRelease to allow testing reconnect problem (default 0 = just hangup)
mvh 20070103: FixKodak blocks *xxxx wildcard virtual queries on date (not allowed in Kodak)
mvh 20070103: Version to 1.4.12b; start on ImportConverters e.g., <set 0008,1040 to "%V0008,0080">
mvh 20070104: ImportConverters %QP %QS %QE %QW; <destroy> <delete xxxx,yyyy> <save to filename>
              Added %% and %i to Export and Import converters
mvh 20070105: <set 0008,1040 if "...">, <setifempty 0008,1040 to "...">, <setifempty 0008,1040 if "...">
              Import/export converters only report when syntax OK and report when action is void
	      Added %Vxxxx,yyyy to export converters and %d(date/time) to import and export converters
              An import/export converter can thus be used to extensively log incoming images
              Added import converters: <stop>, <if "string">, <ifempty "string">, <nop> (also export)
              Added general purpose variables x, y, z (read as %x, %y, %z) to import converters
	      Definition of number of ImportConverters now unnecessary.
              Note: importconverter and exportconverter strings may be changed run-time in dicom.ini
mvh 20070105: Some fixes in dgate --archiving options manual; fixed dgate --restoremagflags:
mvh 20070107: Fix web access broken by change in odbci for dbaseIII; query must now always include spaces around =
mvh 20070113: Do not store NKI compressed data in v2 format: decompress and give error message
              Recompress to NKI format now enables StripGroup2; Tested with $c server debug command
mvh 20070117: Allow multiple rules per Export or ImportConverter separated by ; and zero or more spaces
              <nop> statement prints itself on console: useful for logging anything that follows
mvh 20070117: Now maintain multiple connections per ExportConverter line (max 20x20 forwards)
	      Added <stop>, <silentstop>, <if> and <ifempty> to ExportConverters; but they only affect single rule 
	      (while these would cross over to the subsequent rules in ImportConverters)
mvh 20070122: Now also show free space on mirror devices
mvh 20070123: Fixed TestFilter for patientid with '
mvh 20070125: Removed <if> and added <ifnotempty>, <ifequal>, <ifnotequal>, <ifmatch>, <ifnotmatch>, 
	      <ifnumequal>, <ifnumnotequal>, <ifnumgreater>, <ifnumless> as im/exportconverters
              Added %xxx[first,last] substring operator; and protect it against overrange
              <ifxxx> no longer crosses over importconverters; added {} block for im/exportconverters
              E.g.: ifequal "%V0008,0020[0,3]", "2004"; { forward to CONQUESTSRV3; forward to CONQUESTSRV4; } nop test
	      Note: 1) all (also ifxxxx) statements end with ; 2) statements may begin with { or }; 
              3) extra spaces only allowed after ;{} and single space after ,
mvh 20070127: Fix {} in exportconverters.
              Release 1.4.12b to fix more or less critical odbci error
mvh 20070131: Fix in TestFilter (quotes double); Version to 1.4.12c
mvh 20070201: Added <storage MAGn> import converter: sets device to store this (new) image to (default MAG0)
              Fixed importconverters stop and destroy: did not break current line
mvh 20070204: Added extra format starting with # to ImageFileLister: e.g. #%s:%s prints MAG0:filename
mvh 20070206: Added create database: dgate -esapw name user password 
mvh 20070207: Fix query string bug on applyworklist found by capolan1
mvh 20070207: dgate -e name root <blank> works for mysql: root is used as admin and login
              Also implemented dgate -esapw name sa sapw where sapw may be empty
mvh 20070210: Added CheckDuplicates to UpdateOrAddToTable; added @ in imagefilelister: replace \ by /
              Start on ocx based viewer integrated in cgi interface; used dicom over http:
              Small change in gif export; added * as separator in query/2
mvh 20070211: Added WebScriptAddress (default http://server/cgi-bin/dgate), WebMAG0Address (defaults http://server/mag0)
              and WebServerFor (default 127.0.0.1). Fix in cgi query parser: now loads dsize and size correctly
              Version to 1.4.12d
mvh 20070212: Added WebCodeBase, show version in status_display, added config of which dicom server the web server
	      connects to, added [webdefaults] section, added iconsize parameter
mvh 20070213: Added : as format in imagefilelister: use to list pat:sopuid
              Fixed sorting problem by using adapted imagefilelister for seriesviewer
	      Fixed default for WebCodebase; Version to 1.4.12e; 
	      Use correct case of sql table names in cgi interface: required by mysql on linux
	      Unsolved: n3 does not always decompress correctly in dview.pas see p20401977:i47, changed default to n4
mvh 20070215: Fixed logging and added *** Association rejected message when association is rejected
mvh 20070304: Added <defer> exportconverter: retry later as if connection unavailable
mvh 20070305: Note: n3 problem fixed in dview.pas, version to 1.4.13alpha
	      Allow merging of up to 1000 UIDs: set SilentText to 64000 and *uids[1000] (thanks mp)
              Allow named pipes for log_on and debuglog_on
mvh 20070307: Work on extending extract: allow e.g. SQlite -> dbaseIII
bcb 20070308: Made changes for big endian and Apple's DARWIN
mvh 20070314: Fisished extract: allow e.g. SQlite -> dbaseIII in directory MAGO dbase
mvh 20070315: Merged DARWIN stuff; Set TCPIPTimeOut for PDU that moves images
mvh 20070316: Display defer import converter; Clumsily added <compression> import converter and 
              improved <storage> import converter; Made <storage> thread safe
	      Failed recompress() no longer reason to fail drag and drop; add convert_to_bmp
	      Use | instead of , to separate patientid from file in addimagefile:
              Distributed , code to each of the server commands to make it more , safe
              Note: it is NOT a good idea to use a , in a flexible filenamesyntax
	      Added level & window to convert_to_gif and convert_to_bmp; longer log lines for dump VR
mvh 20070317: <prefetch> using new prefetch_queue, mayprocess and into_queue_unique, <preforward>, <preget>
mvh 20070324: preget now <preretrieve> and avoids looping by checking orgmessageid!=0xfbad
              I.e., data collected by <preretrieve> does not go through import/exportconverters
              Added delayed exportconverters <forward patient>, <forward study>, <forward series> with optional compression
              Removed "forward compressed to" (without as section) as is never used
	      Version to 1.4.13beta; delay to 10 minutes
mvh 20070326: Fixed leak in <forward patient> etc. preretrieve also checking orgmoveae
mvh 20070330: Change by Mark Pearson: read dicom.ini from same dir as exe in unix mode; merged more Darwin stuff
	      Also taken over bcb's fix for the finder: studymodality was asked at patient level
              Also put in a more ENxxxx as command and messageid
ljz 20070404: Added CommaInFilenameWorkAround()
mvh 20070407: Use reentrant ctime_r when available; moved static SOPPatientRootQuery etc to stack to fix 
	      reentrancy problem: concurrent outgoing c-move of CT and MRI on different associations would 
	      share PDU causing sopclass not accepted on one. Also seems to reduce linux crash problems on 
	      multiple inbound (4 or more) C-stores. Also moved CreateBasicFilmSessionRequest etc to fix 
	      potential reentrance problem for printing.
mvh 20070410: Merged CommaInFilenameWorkAround()
mvh 20070413: "extract:" will just create dbase III clone of full patient DB; added "prefetch" server command
mvh 20070415: Added Prefetcher option to dicom.ini: is set will aggresively preread queried patient or studies
mvh 20070416: Prefetcher prints more information; use windows calls in windows for faster preread
mvh 20070705: extract: uses DT_DBASEIIINOINDEX to disable indexing and packing when extracting from e.g. sqlite to dbaseiii
              Note: extract: non dbaseIII goes to MAG0\dbase, dbaseIII goes to DataSource
mvh 20070706: Added <between "9", "17"> (9:00 to 16:59) or <between "17", "9"> (17:00 to 08:59) import/exportconverter
mvh 20070709: Fixed print of clone db done
mvh 20070720: Fixed problem where waiting "forward ... to " task caused high CPU load
mvh 20070901: Added dgate --checklargestmalloc:
mvh 20071027: Added ForwardCollectDelay for delayed forwarding (default 600 sec)
mvh 20071030: Fill in called and calling AE for SaveToDisk for e.g., dropped files etc
	      Fix %u and %c in Export and ImportConverters: called and calling AE
	      Log flow control Export and ImportConverters only in debug mode
	      Added retrying to delayed forwards or prefetches
mvh 20071102: SetDicomErrorHandler: orderly shutdown of server on serious DICOM library errors
	      Fixed a bug in ForwardAssociationLevel: last UID was lost when reading INI file to szTemp
	      Added MaximumExportRetries and MaximumDelayedFetchForwardRetries (default=0 is no maximum)
mvh 20071103: Version to 1.4.13; defer resets queue fails -> keeps on retrying
mvh 20071104: Use local as AE for imagelister in web page instead of CONQUESTSRV1
	      Enable remote servers (use AE instead of 'local') in finder and lister commands
	      Fix crash when dgate -- option gets empty filename: now lists to stdout
mvh 20071114: Put --checklargestmalloc: in dgate -?
mvh 20071118: Adapted for 64 bits (use SQLLEN for BindField)
mvh 20071120: Fixed KodakFixer for empty patient ID (crashed on 64 bits only)
              Added [modality xx] and [date yyyymmdd-yyyymmdd] clauses to export [patient|study|series] to AE
mvh 20071121: Added %V*GGGG,EEEE: search VR in any sequence; version to 1.4.13a
              Added [now -ddd+ddd] and [age -ddd+ddd] clauses to export [patient|study|series] to AE
	      No retry on remote dicom error in forward xxx to: would retry zero records response
              Use study date for data, age and now clauses
mvh 20071123: Made forward command case insensitive; added forward image; added get [patient|study|series|image] from AE
	      The latter option makes preretrieve obsolete
mvh 20071124: Added [sop xxx] clause to [forward|get]
              Reorganized exportconverter code: all converters now do % substitution; reduced code duplication
              Added reading of sequence items like in %V/300c,0060.0/0008,1155 or %V/300c,0060/0008,1155
	      Real life example (must all be one one line, max 512 characters): 
              ifnotequal "%m", "RTPLAN"; stop; 						nop "tests modality = RTPLAN";
	      ifnotequal "%V*300a,00b2[0,1]", "A2"; stop; 				nop "test machine name = A2"
	      forward to XVI_A2; 							nop "forwards the current recieved RTPLAN";
	      get study modality CT from NKIPACS; 					nop "collects associated CT";
	      get study modality RTSTRUCT sop %V/300c,0060.0/0008,1155 from NKIPACS; 	nop "collects associated RTSTRUCT";
	      forward study modality CT imagetype *AXIAL* to XVI_A2; 			nop "forwards associated CT";
	      forward study modality CT series %V(/300c,0060/0008,1155)/3006,0010/3006,0012/3006,0014/0020,000e     
	      forward study modality RTSTRUCT sop %V/300c,0060.0/0008,1155 to XVI_A2; 	nop "forwards associated RTSTUCT";
mvh 20071125: Fixed syntax for %V/gggg,eeee.i/ *gggg,eeee (search all groups under a specified one)
mvh 20071126: Start on StorageCommitment
mvh 20080103: Switched to ms7 compiler; fixed forward and get date, modality, sop filters: would not supress the UIDs
mvh 20080107: Fixed malformed web viewer header: worked for IE but not for other browsers
	      Check presence of dicom.sql in web pages that need it
	      Deal with spaces in patient IDs in web pages
mvh 20080126: Check presence of dgate.dic when starting server
mvh 20080129: Fix KodakFixer: wrote one byte outside VR data (pr)
mvh 20080129: Added [imagetype xxxx] clause to [forward|get] (pr)
mvh 20080130: Small fix in above
mvh 20080205: Added experimental PadAEWithZeros flag; version to 1.4.14alpha
mvh 20080210: Added experimental IgnoreOutOfMemoryErrors;
	      Added "delete [patient|study|series|image] [date yyyymmdd-yyyymmdd] [now -ddd+ddd] [age -ddd+ddd] 
	      [modality mm] [sop xxxx] [imagetype xxxx]" export converter
mvh 20080221: Added [seriesdesc xxxxx] filter and "process [patient|study|series|image] [by command]"
mvh 20080222: Small fix in imagetype filter code; process by clause not part of unique check
mvh 20080302: Fixed case of DICOM table names
mvh 20080316: Added dgate --deletestudies:date(range) and dgate --movepatient/study/studies/series
mvh 20080318: Added [series xxxx] and [study xxxxx] filters; work on web page
	      --deleteseries/study and --moveseries/study allow patid:uid as parameter
	      Web interface can now push and delete on series and study level, but
	      delete is disabled in readonly web interface
	      Version to 1.4.14beta
mvh 20080319: Delete in browsers fixed?; added %v(desc)desc: search VR in given sop of same patient
	      Example: %V(/300c,0060/0008,1155)/3006,0010/3006,0012/3006,0014/0020,000e
              Gives referenced CT series UID in referenced structure set from forwarded RTPLAN
mvh 20080321: Fixes in browser; added graphic web setting; may be gif or bmp for now; 
	      Fix %V for non-existing items
mvh 20080322: Added dgate --anonymize that uses general scripting
mvh 20080323: Small fix in (local) for DcmMove: overwrote destination
mvh+pr 20080404: Fixes in series and study filter (rethink naming)
	      pr added value* match for in match() routine
mvh 20080618: Added extra logging in IARQ()
mvh 20080620: Only show extra logging in IARQ when assoc rejected
mvh 20080816: IARQ shows which sops are accepted; and differentiates no sop from connection terminated
mvh 20080817: Small layout fixes; <process with> import converter; show PDU length in log
	      Check saving of file succeeded (thanks Jeff Bellamy and Alberto Smulders)
mvh 20080819: Added experimental DT_MSTR: multi-value entry, e.g. query on 'PET' matches 'PET\CT'
map 20080819: added --get_ini_param (same as get_param) and get_ini_num to list parameter by index
map 20080819: Added get_ini to list all parameters in DICOM.INI
mvh 20080819: Added code by map but kept original get_param for old web cgi client compatibility
              Version to 1.4.14
mvh 20080820: Solaris 10 fixes, and general UNIX fixes (map): configfiles may not be in current dir
mvh 20080821: Default of ForwardAssociationRelease to 1; added sleep export converter
mvh 20080823: Added study UID search to PatientStudyFinder; fixed DcmMove: would call local when remote adressed
	      Added rudimentary query/move web pages
mvh 20080825: query/move looks better; added "thumbs" column to local series lister; config patched to readonly
	      added background color and display of icon (to be placed in WebCodeBase)
mvh 20080826: One more fix in DcmMove, still called wrong server sometimes
	      Do not use 0xfbad as message ID for DcmMove from web page; webreadonly defaults to true
              CGI viewer configurable: seriesviewer, seriesviewer2, noviewer, aiviewer
mvh 20080827: Added slice number and repeat output data in ImagefileLister and SeriesUIDLister for more flexible formatting
              Added flexviewer: reads code from dicom.ini in web server directory
mvh 20080831: Translate yyyy and yyyymm to date range in finder; further only accept valid date and date ranges
              Added dgate --get_amaps: lists all non wildcard entries from acrnema.map
              Fixed handling of patient ID with spaces in ImageFileLister mode :
              All undefined cgi modes can come from the cgi dicom.ini; added special and cgi script variables
	      Lines can start with -- to start dgate, with # as comment, any other passed as HTML
	      Added source parameter: flex pages can be read from script file
              Added serversideviewer: requires javascript only, nothing to install
mvh 20080901: Fixed some leaks in printing and --commands; fixed --get_ini: formatting
mvh 20080902: Added header option for web scripts; added some CGI environment variables
bcb 20080905: Made new changes for big endian and undid some old ones.
mvh 20080908: Index in filelister format is now a string (%s), and added next string that gives index+1 (is1)
              Fixed manual of --modifypatid: and --anonymize:; dicom mime to application/dicom
	      Added modes imagelisthttp, imagelisturls and imagelistfiles; added ? format for imagelister: gives #files
mvh 20080909: Added studyuid option to moveseries; pass it studyuid in seriesmover call from seriesfinder
	      Added --modifyimage:file,script (uses import converter scripting language, where you can use ' instead of ")
mvh 20080910: In modifyimage only replace ' with " conditionally; version to 1.4.14a; 
mvh 20080910: Forbid patid, studuid and seruid change in modifyimage; medged bcb Mac stuff
mvh 20080913: Modified and use SetStringVR to correctly send UIDs with padded 0
mvh 20081016: Show bitness on startup
mvh 20081104: Attach dictionary to DcmMove PDU
mvh 20081105: Fixed display of bitness in web page
mvh 20081116: Adapted for very large objects (use unsigned int for length); remove extra 0 byte in HTML generator; fixed CheckLargestMalloc
mvh 20081121: Added create database for PostGres
mvh 20081123: Skip export converters for dropped, modpatid, modimage, merging
mvh 20081124: show database type on startup; Temp version to 1.4.14a1; fix many scripting crashes
mvh 20081129: start on better patient ID change: process all UIDs; fix drag/drop error introduced in 1.4.14a1
mvh 20081129: Removed double access of SOPInstanceUID in SaveToDisk; fix: do not free ddo after SaveToDisk; 
              added <newuids> and <newuids except > import converters; added --genuid, --changeuid and --checksum commands
	      Version back to 1.4.14a; modifyimage can change patient ID and study/seriesUID if newuids used as well
              set can now also work in exactly one deep sequence: set gggg,eeee/gggg,eeee to "string"
mvh 20081130: Added --attachrtplantortstruct:planfile,structfile command using script; also --attachanytopatient/study/series
              stop now stops one importconverter- not the chain; added <call file> and <return> statements to return from file
	      Added optional anonymize_script.cq: overrules fixed anonymizer; 
	      added %A = %V but then gives CRC32 of data and %E = %V but gives new UID for given UID
mvh 20081201: added --convert_to_jpg and web page use (experimental)
mvh 20081203: If jpg graphic: list options as jpg, yes, no; fix slice number display is serversideviewer
              Added frame control to convert_to_jpg etc as l/w/frame; add optional frame control to 
	      convert_to_dicom; added frame control in serversideviewer
mvh 20090203: Removed DNS reversing (you can use wildcard AE instead); work on zip loading
mvh 20090204: Finished zip loading (to be tested on linux), uses now waiting BackgroundExec
mvh 20090206: Added QueryConverter0, WorkListQueryConverter0, RetrieveConverter0: can read called (%c), 
              calling (%u), and c-move destination for retrieve (in %s), as well as all data in data object
mvh 20090209: Made compiling under linux
mvh 20090211: Export MyDcmError; only DCM_ERROR_MEMORY is fatal
mvh 20090212: Recoded virtual server (kept old code), to provide better grouping of move requests
mvh 20090215: Added VirtualServerPerSeries..9 flags: determines if virtual gets are per image (default) or per entire series
mvh 20090216: Added %Rgggg,eeee: gives restored UID for newly generated one
              Prepare for RetrieveResultConverter0
mvh 20090411: Added simple wildcard matching in match(), accept *text*
mvh 20090411: Version to 1.4.15alpha
mvh 20090513: Added CompressionConverter0 .. 9: called from recompress to 's0' to 's9'
	      Added <fixkodak> and <unfixkodak> import converters; added <after NN> clause to <forward study> etc
	      Blocked out StorageCommitment
mvh 20090514: %QXgggg,eeee reads aliasfileqX.txt with lines like old<tab>new<return> to map VR to new value 
mvh 20090616: Add ^/~ after % command to convert to upper/lowercase, %^ %~ %[; fixed leak when save to sql fails
20090616	jf	Include file stuff; fixes to Unix 7z stuff
20090618	mvh	VirtualServerPerSeries is now a limit: if set to N, more than N images go per series
20090620	mvh	Version to 1.4.15beta
20090802	mvh	Support DCM_ERROR_DEBUG (debug messages from library) that print only if debuglevel>0
20090821	mvh	Removed need to specify "delete series modality *" instead of "delete series"
			Fixed "process by " command; now uses all command string upto ;
20090824	mvh	Version to 1.4.15beta1
20090921	mvh	Added extract_frames: command
20090922	mvh	Tried to fix <compression> import converter; added <reject> import converter
20090924	mvh	Restructured manual; fix crash of studyfinder without search string
			Note: on vista, commands like dgate --imagelister wont give large lists (does when piping)
20090926	mvh	%t = tab in scripts; fixed failed storage error message; added RetryForwardFailed flag (default 0)
20090929	mvh	Added ImportExportDragAndDrop
20090929	mvh	Version to 1.4.15
20090930	mvh	Version to 1.4.15a
20091005	mvh	Version to 1.4.15b
20091012	mvh	Optimized preprocessing of read-ahead thread by saving tt
20091108	mvh	Started on <merge study> import converter
20091120	mvh	Started on <forward> import(!) converter
20091229	mvh	Version to 1.4.15c
20091230	mvh	Merged bcb changes: DGATE_VERSION, PATH_MAX, const char, bigendian fixes
20091231	bcb	Defined PATH_MAX for Apple's Snow Leopard,endian fixes, &
			fixed gcc4.2 warnings (char to const char, VERSION, DEBUG and char* to program memory)
			Added jpeg2k stuff (HAVE_LIBJASPER)
20100111	mvh	Merged; but keep cgi formats local (less changes)
20100113	mvh	Fixed one signed/unsigned mix in dump routines
20100119	mvh	cast %d print to (int), %ud to (unsigned int); one more const issue
20100120	mvh	Fixed two const issues detected with ms8
20100122	mvh	fixed: %ud should be %u
20100122	mvh	DcmMove uses study-root query when UID passed to fix move to osirix from web interface
20100123	mvh	Fixed rare crash in function VirtualServer2; started on DT_FL and DT_FD; version to 1.4.16alpha
20100123	mvh	Added counters for jpeg2000; put info about jpeg libraries into serverstatus display
20100124	mvh	Blocked old VirtualServer(); rephrased cancel message; changeuidback and count_frames commands
			Optimized DcmError call to gpps
20100124	mvh	Use much faster MyGetPrivateProfileString: profiled as followed:
			Installed C:\Program Files\Microsoft SDKs\Windows\v6.1\Bin\xperf_x64.msi
			set _NT_SYMBOL_PATH=e:\quirt\comps\exe\dgate;srv*c:\symbols*http://msdl.microsoft.com/download/symbols
			set _NT_SYMCACHE_PATH=C:\symbols
			xperf -on latency -stackwalk profile; run dgate; xperf -d out4.etl
			xperfview out4.etl; select time range; select trace-load symbols; 
			right click selection; summary table (takes very long the first time - downloads all symbols!)
20100207	mvh	Added uncompress command; version to 1.4.16alpha2
20100209	mvh	Fixed another rare crash in function VirtualServer2 (thanks PR)
20100227	mvh	Fixed Scotts modifyimage problem: http://forum.image-systems.biz/viewtopic.php?f=33&t=2143
20100227	mvh	Added -q option to set target address of dgate -- command; version to 1.4.16alpha3
			pass DCO also for non-nki clients: used in %s for RetrieveResultConverter
			Added more extensive printer activity logging
20100228	bcb	Changed HAVE_LIBJASPER to HAVE_J2K so Jasper or OpenJPEG can be used
20100309	bcb	Added double parentheses (gcc4.2 Warnings)
20100309	bcb	Changed int to unsigned int, commented out unused variables and routine (gcc4.2 Warnings)
20100619	bcb	Added #ifndefs and fixed shadowed variables(gcc4.0 Warnings)
20100723	mvh	Merged: Reread all changed; agreed with all but two in checklargestmalloc and cgi; 1.4.16alpha4
20100728	bcb	Added delete vr's after ->ReplaceVR's
20100802	jd	Added scheduletransfer
20100815	mvh	Merged; version to 1.4.16alpha5
20100815	mvh	Fixed bug 32 in t=2127: seriesdesc filter
20100816	mvh	Added QueryResultConverter and ModalityWorkListQueryResultConverter
20100816	mvh	Fixed bug in VirtualQuery introduced by only partly renaming shadowed variable
20100816	bcb	Allow dicom.ini and other files to be moved; fixed / as cmd line option
20100816	mvh	removed (a) from #ifndef EnterCriticalSection(a) etc
20100819	mvh	Added system export converter; added "get study" etc import converter. 
			Allow "set" in sequence items; Allow "set" to add sequence items.
20100823	mvh	Extended virtualservermask; pass server directly to VirtualServer to override server list
20100823	mvh	Merged and simplified bcbs basedir code; use exe dir as default basedir
			Fixed overlap in ImportConverter number; Made %xyz variables local; Version to 1.4.16beta
20100824	mvh	Use dFileSize to test if file is written correctly: catched 0 byte files with full disk
20100827	mvh	Fixed two typos in warning fix extract: command
20100901	mvh	Fixed empty virtual query sent if no images there
20100905	mvh	Removed file functions: now use RecompressFile; document -j* and -j-
			WIP: added testmode: function to allow internal file comparison
20100914	bcb	Fixed leak in PrinterSupport
20101017	mvh	Added MergeSeriesConverter and MergeStudyConverter
20100918	mvh	Merged; version to 1.4.16beta2; testmode does not affect LoadForGUI direct filenames
			Fixed that changing patid etc in importconverter breaks exportconverter data
20100919	mvh	Added <format> clause to <set> importconverter command
20100920	mvh	Fixed [,fromend] operator
20100924	mvh	Started on error handling in virtualserver
20100925	mvh	Fixed problem in <forward> IMPORT converter; only worked as first command in line
			SearchDicomObject now checks on buffer overflow; ; # lines are comment in script file
			note: <system> converter should run hidden; <call> now passed called and calling
20100925	mvh	Version to 1.4.16beta3
20100926	mvh	Added <mkdir> and <rm> IE converters
20100927	mvh	Added <save frame> and <save bmp/gif/jpg> IE converters, <save to> E converter
20100928	mvh	Fixed in delayed <merge>, <delete>; fixed MergeSeriesConverter and MergeStudyConverter crash
			Fixed 1.4.16beta2 introduced bug: patientid was passed incorrectly into importconverter
20101003	mvh	Added seriesdesc filter and script to DcmMerge, script to DcmMove
20101003	mvh	--browsepatient command; fixed that split/merge series generated new study
20101003	mvh	Version to 1.4.16beta4; merged jd scheduletransfer
20101003	mvh	ImageFileLister now internally also takes study and sop
20101003	mvh	Added dgate --submit:patid,studyuid,seriesuid,sopuid,username@machine:folder,password
			Requires 7za.exe and pscp.exe
20101003	mvh	Modified dgate --scheduletransfer:patid,studyuid,seriesuid,sopuid,username@machine:folder,password
			Added IE <submit> command with target and password clause
			Added next slice and frame buttons in serversideviewer
20101005	mvh	Fixed ? imagefilelister (count), added $ imagefilelister (#frames), started on passing frames 
			to serversideviewer
20101005	mvh	Started on WADO
20101006	mvh	Enabled WADO; do not try to remove images from failed virtualserver request (double messages)
20101008	mvh	Started on rudimentary WADO client (PatientStudyFinder now also lists images)
20101009	mvh	Enabled rudimentary VirtualServer for WADO client: loadforgui will try virtualservers
20101009	mvh	Tried to speedup query result processsing; implemented POST method (no multipart yet)
20101009	mvh	WADO server and client function; upload button, not functional yet as requires multipart parser
20101010	mvh	Set default WADO correct for conquest; finished (single file) upload fuctionality; 
20101010	mvh	Scan incoming folder once per second (thread  starts when incoming folder exists on startup); 
			Some fixes in serversideviewer; started on VirtualQueryCached
20101013	ljz	Fix : CreateDatabase "root", used UserName as Password for postgres
20101014	mvh	Fix in virtualserver for LoadForGUI
20101016	mvh	Improved virtualserver for LoadForGUI; Cached queries start to work; virtual queries now return computed fields
			use e.g., VirtualServerFor0 = AE,CACHESERIES or AE,CACHESTUDIES
20101017	mvh	Added OverlapVirtualGet parameter: controls interleaving of recieve and send of virtualserver (suggest: 16)
20101018	mvh	Limited memory use of OverlapVirtualGet, tried out of order processing but this failed; dated folders for querycache
			Submit and scheduletransfer call submit.cq if exists to anonymize
20101020	mvh	Small fix in querycache: query file sometimes not stored; and study cache did not read date
20101108	mvh	Added --loadanddeletedir command
20101116	bcb	Warnings
20101120	mvh	Fix reading of FIXKODAK in virtualserver if e.g., CACHESERIES appended; 
			merged bcb warnings (and fixed introduced errors); version to 1.4.16releasecandidate
			Added warning if cached virtualserver does not provide computed fields
			Added NONVIRTUAL flag to allow block recursive virtualserver connections
			Do not forward transfer syntax to virtual queries data object
20101121	mvh	Fixed NONVIRTUAL flag for non-cached queries if caching is ON
			Added dgate --export: command to zip/7z/tar selected data
			Added dgate --safequit: command and OpenThreadCount; prepare for ArchiveConverter0 (1900)
			Show in log when a file is rewritten; Added [wadoservers] section in cgi config
			Document ul, ub, and ue in acrnema.map
20101122	mvh	Pass script to export: command; added ZIP thread that runs when the system is a service
			Fix for spaces in zipfilename for export; background exe for 7za
20101124	mvh	Fix compile and 2 warnings for linux; 
			drop zip file extracts with paths to avoid overlapping filenames
20101209	mvh	Fixed FIXKODAK parameter detection of virtualservers
20101212	mvh	Warn if FIXKODAK paramater set in virtualquery does not match FixKodak global
			Fix rewrite message for virtual server data
20101212	mvh	Correct file formats listed and accepted for 7za decompression
20101212	mvh	Version to 1.4.16
20101213	mvh	Implemented NightlyCleanThreshhold here for Linux and Windows (if logging to file)
20101220	mvh	newuids will change empty uids into a new one
20101222	mvh	Fix for empty cached query that crashed
20101227	mvh	Fixed some delete NULL crashes on failures
20101227	mvh	Added org clause to forward <compressed as xx> to AE org AE
20101227	mvh	ImportConverters now also use ForwardAssociationLevel
20101228	mvh	Added \n after cloning db message
20110105	mvh	Use MakeSafeStringValues for exact match; Added MoveDeviceScript0
20110105	mvh	Added --echo and --selectseriestomove commands
20110105	mvh	Version to 1.4.16rc1
20110106	mvh	Added --moveseriestodevice; changed --selectseriestomove syntax
20110106	mvh	Todo: NightlyMoveSource, NightlyMoveDest, NightlyMoveAmount, NightlyMoveAge, NightlyTestAge
20110106	mvh	Version to 1.4.16rc2
20110110	mvh	Note: mergestudiesconverter called for series and other way around
20110111	mvh	Check for empty patient ID in virtual query and for other empty VRs
20110111	mvh	merge study converter merges series: now calls MergeSeriesConverter. 
			Merging studies now calls MergeStudiesConverter
20110111	mvh	changed logic of converters passing study or series uid to be consistent
20110111	mvh	reject or destroy in merge script stops object from being merged
20110113	mvh	fix script clause in merge study, call also tries line in [scripts] in dicom.ini
20110113	mvh	Allow script clause without "" as well; unrecognized line attempts call
20110114	mvh	"" in script string; script command evaluates string as script
20110115	mvh	delete changed UIDs after merge study (remerge will create new series)
20110115	mvh	fixed "script" converter and substitution of ""; experimental "lua" command
20110116	mvh	fixed error message for incorrect script commands; first lua functions built-in
20110116	mvh	SearchDicomObject now supports %VName; allow setting/reading more than 10 sequence items
20110116	mvh	The set command now accepts names, e.g., set PatientID to "9999"; nop %VPatientID
20110116	mvh	Lua now allows, e.g., Data.PatientID = 'aap'; print(Data.PatientID)
20110117	mvh	Started on lua Global, Association, Command
20110118	mvh	Primary PDU is Extended to allow embedding lua state in it
20110119	mvh	Added a global PDU as script context; associations add new contexts
20110120	mvh	Fix in Association.Calling/Called; for now each thread has an isolated lua state
			Fix in speed of query results (were all processed by ImportConverter). Added lua 
			startup/command/QueryConverter0 etc; count all threads including gui threads
20110121	mvh	Added prefetcherPDU for delayed forwards and such
20110122	mvh	Fixed lua print, added lua error handling; added %f %b %p to ImportConverter
20110122	mvh	Fixed SearchDicomObject for US,SS,UL,SL tags (note: 'set to' not fixed yet)
20110124	mvh	bug fixes in new code; implemented reject in lua
20110125	mvh	Put UID changes into debuglog
20110127	mvh	Added lua getpixel and setpixel (takes 3-5 microseconds)
20110129	mvh	Added lua getrow, setrow, getcolumn, setcolumn, readdicom, writedicom; sd.ddo leaks
20110201	mvh	Lua: R/W sequence e.g., print('0 implied', Data.InstitutionCodeSequence.InstitutionName) 
			or Data.InstitutionCodeSequence[2].InstitutionName = 'write for now max one level deep'
20110203	mvh	Lua: Unified reading and writing of Data/Command/Sequences; 
			Allow write at any depth in existing sequences
20110204	mvh	Allow creating sequence in script and in Lua: Data.xxxx = {}
20110205	mvh	Added newdicomobject and deletedicomobject
20110206	mvh	Fix for set .. to ""; added set .. to nil for simple VR; 
			create sequence will not overwrite existing sequence; 
			lua web page creation: CGI(), HTML(), <?lua ?> <%= %> source=xx.lua
20110207	mvh	Added lua: get_amap(N) -- N starts at 1, dbquery(db, flds, query, sort) { returns table of tables starting at 1)
20110208	mvh	lua gc for dicomobject; lua dicomarray and dicomquery; allow virtualquery without setting level in DDO
20110208	mvh	1.4.16rc4
20110214	mvh	Fixed lua Data access; fixed "" in exportconverter; 1.4.16rc4a
20110216	mvh	Fixed calling and called in ExportConverter; fixed %f etc in ImportConverter
			mkdir IE converter also creates subdirs; 1.4.16rc4b
20110228	mvh	Added Write, Read and Dump methods to lua dicomobject as closure with passed method name
20110320	mvh	Fixed Association.Calling in lua; Added RejectedImageConverter0; added t_1500 checks to all queries
20110320	mvh	1.4.16rc5
20110326	mvh	1.4.16rc6
20110328	mvh	Fix crash when setting lua Filename for import file
20110331	mvh	Added NoDICOMCheck option: if set, do not stop parsing DICOM stream on error
			FlushPrivateProfileStringCache after writing to dicom.ini
20110331	mvh	1.4.16
20110404	mvh	Fix in CGI behavior for IIS (check REQUEST_METHOD); version to 1.4.16a (unreleased)
20110413	mvh	NoDICOMCheck also forces use of sequence enabled reader (allows JPEG compressed V2 file)
20110419	mvh	Added lua getvr(Object, g, e), Object:GetVR(g, e), setvr(Object, g, e, data), Object:SetVR(g, e, data)
			full lua webpage now basically works and gives error messages to web page
20110419	mvh	version 1.4.16a
20110501	mvh	added heapinfo() in lua; version 1.4.16b
20110502	mvh	Made DcmMove almost always work on study level
			added lua dicommove(source, dest, obj, patroot) and sql(statement)
20110603	mvh	loadkfactor now required for dgate -a commands; added -h option (wait for getkey)
20110604	mvh	lua endassociation and clienterror events; added retry command for rejectedImageConverter0
20110604	mvh	implemented/fixed size (may be %)/dsize for sliceviewer/serversideviewer/seriesviewer; version 1.4.16c
			also allow size='': use original size or 80% for those requiring a fixed size
20110605	mvh	Added zip download of series/study through push page
20110606	mvh	Added --modifystudy and --modifyseries commands, e.g., dgate "--modifystudy:20000321,,compression j2"
20110628	lsp	Set PDU.Link.Connected to FALSE on !PDU.Read(&DCO)) in 
			StorageApp::ServerChild() to avoid subsequent use of a closed socket in
			AReleaseRQ::Write() and AReleaseRP::Read() in PDU_Service::Close()
			(called in destructor of PDU_Service)
20110710	mvh	Added RejectedImageWorkListConverter0; fixed retry for RejectedImageWorkList and RejectedImage
20110710	mvh	Fixed WADO application/dicom mime header
20110710	mvh	zip from web page calls zip.cq script on each file; also forces DCM extension
20110710	mvh	Fix compile of getch and heapinfo linux
20110710	mvh	Version to 1.4.16d
20110904	mvh	Clear address (0010,1040) in default anonymizer; Fixed lua deletedicmobject, leak in readdicomobject
			Create empty sequence is really empty, # operator for DICOM sequence in lua
			Version to 1.4.16e
20110904	mvh	Fix read of %i, %s, %m in lua not from ImportConverter; 
			protect getpixel etc against missing pixel data
			Fix crash of %QX if file does not exist
20110905	mvh	Fixed leak in %QP etc; fixed leaks in ADDO cleaning in web interface
20110906	mvh	Added optional script instead of submit.cq to dgate --scheduletransfer: and dgate --submit:
20110907	mvh	Added script clause to forward; e.g., forward to PACS001fir org SELF script fixkodak
20110927	mvh	Added "crop x1,x2,y1,y2" and "tomono" importconverters
20110928	mvh	Added the DEST clause to e.g., "forward to AE org CALLING dest CALLED script nop"
20110930	mvh	Fixed crash forwarding from lua
			Version to 1.4.16f
20111009	mvh	Fixed sequence writing problem; make it one long when first writing
			Fixed AliasFile reading issue: i index into command updated incorrectly
			Fixed lua dbquery issue due to typo
			Added channel clause to forward import converter; use PDU[19,channel] from lua
			fix forward org dest clauses; fixed forward to HOST:PORT
20111010	mvh	Protect channel clause againt incorrect values
			Version to 1.4.16g
20111015	mvh	Fixed wado dcm compression (incorrect extension)
20111018	mvh	Added TempDir ini file option
20111114	mvh	Version to 1.4.16h
20111120	mvh	Default size in convert_to_gif etc is now 0: as is
20120112	mvh+jd	Fix in passing script to submit command
20120211	mvh	Allow passing optional DDO as first arg to set/getpixel set/getrow
			Object:SetPixel, Object:GetPixel, Object:SetRow, Object:GetRow
20120212	mvh	Fixed uncompress WADO; fix POST code for SOAP serving; export globals to lua in CGI mode
20120212	mvh	Fixed POST for IIS (read len, not len+1)
20120213	mvh	Small fix in x:GetRow etc; Setting Dictionary and ACRNemaMap for CGI enables moves and queries
20120213	mvh	Moved some set and virtualquery logging to SystemDebug
20120214	mvh	Allow any file to be POSTed, saves as xxx.dat; Added DefaultPage and AnyPage items
20120215	mvh	Reversed CGI key test to allow query string where e.g., mode= appears twice, hit on first
20120215	mvh	Added optional markseries and markstudy pages if passing value 'key' down into them
20120217	mvh	Added shoppingcart option; enabled if configured and passing value 'key' 
20120218	mvh	Added DefaultPage to make own top and AnyPage to totally disable built-in CGI server
20120219	mvh	Disabled WEB push to self, and zip from remote server (which was invalid anyway)
20120220	mvh	Added WebPush configuration to allow disabling push options; show more config items
20120220	mvh	Version to 1.4.16i
20120221	mvh	lua can run script on any dicom object; empty tag does not return NIL to lua; lua dictionary(); fix uploadedfile
20120222	mvh	Fix webpush and crash on IIS for empty QUERY_STRING
20120229	mvh	Extended number of possible columns in PatientStudyFinder
20120302	mvh	Added gpps lua function; DefaultPage may be e.g., *.lua; any mode is that mapped to mode.lua
20120304	mvh	Implemented region and anonymize options to wado
20120304	mvh	Made DefaultPage and AnyPage sections; Fixed WADO text output; Version to 1.4.16j
20120305	mvh	Added statusstring for simple web feedback of dicommove and submit (not multiuser safethough)
20120306	mvh	Added lua callback to dcmmove2; added statusstring: server command
20120306	mvh	A failed virtualserver request will now not wait until a timeout and log error messages
20120307	mvh	Added bridge=AE option to make local WADO server a proper bridge (see MyBridgeStorage and LoadForBridge and 0x2bad)
20120317	mvh	Documented imagefinder; added process study|series with xxxx.lua (just command_line is passed)
20120318	mvh	Allow "file.lua" directly as importconverter instead of lua "dofile('file.lua')"
20120319	mvh	Implemented lua: filenamesyntax; e.g. lua:dofile('generatefilename.lua') or lua:Data.SOPInstanceUID..'.dcm'
20120320	mvh	Added [wadoservers] bridge = AE to force wadoserver by default to bridge for one given AE
20120325	mvh	"lua" and [lua] ExportConverter, "copy" ImportConverter and "defer" ImportConverter (for lua ExportConverter)
20120327	mvh	Document status_string: command
20120402	mvh	Improved OverlapVirtualGet: allow to reverse operation - give slices out mainly in incoming order
			Avoid deadlock by conflicting requirements; SystemDebug some status messages in OverlapVirtualGet and ReadAheadThread
20120402	mvh	Released 1.4.16j
20120525	mvh	When retrieve from virtualserver is ready cancel outstanding c-move requests
			Increased virtualserver timeout to 200 sec
20120601	mvh	Implemented WORKLIST query from lua
20120621	ljz	Fix readahead crash when client hangs up in the middle of the transfer 
20120624	mvh	Limit virtualserver to 64 per move; virtualserverperseries obsolete; fixes crash when series larger than move
20120624	mvh	move mkdir etc defines in dgate.cpp up to line 2220
20120624	mvh	Keep move object intact in lua dicommove and dicomquery; report fail for lua sql and readdicom functions
20120624	mvh	Prerelease 1.4.16k
20120630	mvh	Allow compression to be e.g. JL etc or JLNN, J3NN, J4NN, J5NN, J6NN: NN is quality factor
20120630	mvh	Reverse .dcm check: all is assumed .DCM except .V2
20120701	mvh	Added dicomdelete; note: tried to make heapinfo() thread safe but failed; 
			added [lua]nightly script that runs at 03:00; and [lua]background that runs every second
20120701	mvh	release 1.4.16k
20120702	mvh	Protect lua_setvar against NULL string
20120721	mvh 	Fixed CGI code that got extension wrong on unix with . in folder name
20120722	mvh 	Added LUA_5_2 compatibility by bcb; removed unneeded buf[1024]
20120723	mvh	Fixed *count-- --> (*count)--
20120829	mvh	"mkdir " script command  no longer requires a trailing / or \
20120829	mvh	Force even text/plain output; first pad ' '; then strip when odd
20120829	mvh	Version to 1.4.17
20120905	mvh	Fixed bug found by Bruce Barton in [lua] ExportConverter
20120908	mvh	Added fix for virtualserver on imagetype and NOIMAGETYPEFIX flag
			VirtualServerForN may be e.g., "server,NOIMAGETYPEFIX,FIKKODAK"
20120914	mvh	Changed default of above fix to off: flag is now IMAGETYPEFIX
20120914	mvh	Attempt to use VirtualQueryCached to build list of images to be retrieved in VirtualQuery
			Filter response in virtualquery(for virtualserver) for 
			incorrect response of carestream to IMAGETYPE query
20120916	mvh	Cleaned up code; split in VirtualQuery and VirtualQueryToDB (that uses VirtualQueryCached)
			Renamed option IMAGETYPEFIX to send only 3rd element of ImageType to IMAGETYPEFIX3
			IMAGETYPEFIX just tests each received record for a match against queried ImageType
20120917	mvh	Move dumps response in case of error; finished IMAGETYPEFIX
20120922	mvh	IMAGETYPEFIX also works on EchoNumbers
20121015	mvh	Fixed check on V2 filename extension to enable NKI compression again
20121016	mvh	1.4.17alpha release
20121201	mvh	Added and disabled code to link optional socket.core library
20121208	mvh	Added __tostring method to lua representation of dicom objects; 
			fix luaGlobalNewIndex: writes dicom.ini as well as direct variables
			Blocked delayed export converters of non-existent objects
			Added WatchFolder parameter (second monitored incoming folder)
20121214	mvh	Enabled socket.core; use .zip for submit version to 1.4.17beta
			Fixed 'forward compressed as ' in import and export converter (viewtopic.php?f=33&t=15588)
			Added lua Association.ConnectedIP member (WIP)
20121216	mvh	Added DicomObject as class factory with new and newarray, added :free(), fixed ConnectedIP
20130125	mvh	Added changeuid and changeuidback to lua interface; 
			Anonymizer accepts anonymize_script.lua as well as anonymize_script.cq
			Added ImportConverters "olduids" and "olduids except"
20130127	mvh	Added dgate --luastart: (returns immediate)
20130128	mvh	Added crc to lua interface
20130202	mvh	Added getimage, setimage, :GetImage, :SetImage to string
			GetVr has extra parameter (binary), SetVR accepts (binary) string
			Added "pack" library hardcoded; lua genuid; protect lua :Read filename
20130210	mvh	Fixed e.g. process study after 5 with test.lua %i:%o
			Note: there must be arguments passed to command_line
20130215	mvh	Fixed source = xxx.lua cgi starter
20130218	mvh	re-allow readdicom on empty Data object; 
			header now works for lua cgi script (default disabled)
20130218	mvh	Report move failure in dgate -- commands 
20130218	mvh	Added studyviewer (default disabled); added dgate --dolua:
			Documented lua command line options; socket.core only for win32 now
20130219	mvh	1.4.17beta released as update
20130320	mvh	Added lua system() call; fix BackGroundExec with spaces in path
20130321	mvh	Add BaseDir\clibs to path; added missing logs for converters such as rm
20130417	mvh	fix lua dicommove parameter 5; fixed 'webscriptad(d)ress'
20130417	mvh	Unescape % in luaSeqnewindex generated "set ..@ script
20130502	mvh	Added \nAccess-Control-Allow-Origin: * or \nAccess-Control-Allow-Origin:*
			to all WADO generated messages, the space is omitted to force header EVEN
20130502	mvh	1.4.17beta2 update
20130522	mvh	Added options to pass command line to submit data (dgate --submit2:, IE submit2 ..) 
20130522	mvh	Fixed incorrect case of dicomworklist name found by marek
20130523	mvh	Reset dummyddo to avoid incorrect freeing of what it points to
20130523	mvh	Fixed bug introduced 20130417 with unescaping Data.xxx = nil or {} no longer worked
20130523	mvh	In ModifyPATIDofImageFile change PatientID after running script (assumed changes uids)
20130523	mvh	1.4.17 release
20130606	mvh	Fixed crash in headerdump of RTSTRUCT (buffer overrun)
20130709	mvh	Added float to short conversion in luasetimage
20130709	mvh	Added support for setting simple US and UL items with e.g. set Rows to 256
20130711	mvh	Fixed luasetimage, fixed some lua return values, version to 1.4.17a
20130716	mvh	Added dgate --moveaccession: using 7777 magic number
20130804	mvh	dgate --lua: and --luastart: now run in local environment
20130806	mvh	dcmmove callback and process with lua now thread safe
20130806	mvh	fixed setimage(frame, string), version to 1.4.17b
20130808	mvh	Enabled logging of process series by command
20130808	mvh	Fixed importconverter command that used patid in memory; failed after anonymization
20130808	mvh	Added import_critical and export_critical to make "forward to xxx" thread safe
20130810	mvh	Removed export_critical (is serialized), added channel * to importconverter forward to
20130810	mvh	Added closing of importconverter forward to PDU after ForwardAssociationCloseDelay
20130811	mvh	Fixed that missing images were removed from db even of no virtualserver is defined
20130812	mvh	Fixed deadlock of closing of importconverter
20130812	mvh	added lua sleep() in ms; removed unused TransmitCompression
20130813	mvh	Removed unnecesary double N checks in forward import converter; 
			added lua addimage() and :AddImage()
			added VirtualServerQueryConverter and VirtualServerQueryResultConverter
			Pass file.lua(command) and patient ID of anonymizer to command_line
20130820	mvh	Fixed forward compressed as XX to IP:PORT
20130826	mvh	Test presence of lua file with same name as exe, if exists run that passing arg[]
				Pass arg[] for dgate --dolua:; allow dgate --dolua:filename, searched in . and lua/.
20130829	mvh	Version to 1.4.17c
20130903	mvh	Greatly improved read speed for very large c-moves by reordering if statement in search loop
20130912	mvh	Documented where the merge of virtual query computed fields should be added
				The field should also not be stripped and there are other issues as well
20131013	mvh	Set rc to 1 for [lua] converters; should fix QueryResultConverter0; added quality to ToJPG
20131030	mvh	Fix in virtual server if c-move callback occurs less than once per image; 
				test did not generate proper filenames and error message was given improperly
20131103	mvh	Fix in VirtualServerPerSeries when exact #images, would omit SOP tag but set level to IMAGE
20131104	mvh	Reversed logic: 1 image gives IMAGE move and SOP uid
20131107	mvh	Added quality clause to save jpg; Release 1.4.17c
20131219	mvh	Fixed that MIRRORDevice0 setting starts mirror copy thread
20140128	mvh	added copydicom(source) DicomObject:Copy(); DicomObject:Compress(string)
20140209	mvh	Added dgate --compress: command
Spectra0008 Tue, 4 Feb 2014 17:40:17 -0200: Fix cppcheck bug #6 'items[4]' index 4 out of bounds
Spectra0012 Wed, 5 Feb 2014 16:37:14 -0200: Fix cppcheck bug #5.5 printf format string has 1 parameters but only 0 are given
Spectra0013 Wed, 5 Feb 2014 16:57:49 -0200: Fix cppcheck bugs #8 e #9
20140215	mvh	Processed Pablo's cppcheck issues
20140219	mvh	Added generic decompressor dgate -nu file_in file_out (works for XDR and DCM)
20140304	mvh	Detect Data:Read() and do not crash server on it (just fails)
20140309	mvh	fixed lua/file.lua by returning status rc; newuids no longer generates or changes empty uids
			lua mkdir(), dicomquery() now returns raw; fixed luacompress and luacopy; 
			kept dicomquery2 as old one; fixed some linux warnings
20140311	mvh	Started adding response codes to dgate -- commands; 
			Stop LoadForGui accessing non-configured virtual servers
20140314	mvh	1.4.17d release 
20140319	mvh	Updated command line documentation
20140417	mvh	Changed the array initialization loop in RemoveQueryDuplicates
20140506	mvh	merged release comment
20140509	mvh	Note: DicomObject:Read('p:sop') does not access virtual servers while DicomObject:Read('stu\ser\sop') does
20140519	lsp	Avoid uninitialized index in RemoveQueryDuplicates(), removed duplicate includes and
			corrected check on "vr" in getptr(), corrected "testmode" message in CallExportConverterN()
20140527	mvh	Fixed exit codes: 0 is OK, 1 is ERROR; block filename containing .partial from incoming import
20140527	mvh	1.4.17e1 prerelease
20140910	mvh	Fixed moveaccession; call RejectedImageConverter for failed load in incoming; 1.4.17e2 prerelease
20140916	mvh	Fix buffer overflow in html processing of long cgi query results
20140916	mvh	Fixed mirrorcopy: it failed to perform mkdir
20150416	mvh	Header lister, SearchDicomObject, and lua GetVR and SetVR now support types:
			UL, US, SL, SS, FD, OF, FL and OW. Lua GetVR and SetVR tables now start at 1
20150504	mvh	Added lua string:reshape to reorder large strings representing binary data
20150629	mvh	Keep quality spec in "forward compressed as"
20150719	mvh	Version to 1.4.19alpha
20150720	mvh	Fixed case of DICOMWorklist in web interface; 
			holding message in queue no longer warning
			Start work on luaexport; todo: get filename
20150730	mvh	Expose DeleteFile option in ModifyData; split ServerTask from main loop; add --modifier: option
			Allow redirects of logging in --dolua call; -lfile and -Lfile logging
20150902	mvh	Added -x#: progress port; added NewDeleteDICOM and NewModifyDICOM lua interface, added Progress info to DcmMove2
20151023	mvh	Fixed wado region for non-rectangular images
20151026	mvh	Fixed anypage and exceptions logic in web interface
20151108	mvh	Added extract:n,query mode to allow extraction from individual tables
20151108	mvh	1.4.19beta; protect thread count with critical section; 
			Object:ListItems and DeleteFromSequence method calls; Array:Delete command
20151123	mvh	Added progress information to: delete, move, grab, loadfromdir, modify and prefetch
20160108	mvh	Updated ChangeUID calls to support staged anonymization, added newuids stage and olduids stage commands
20160113	mvh	WADO Anonymize=string other than yes passed to lua/anonymize_script.lua(string)
20160118	mvh	Export: now names files in zip file with SopInstanceUID; addeed changeuid fourth parameter: Type string
20160119	mvh	Fixed named anonymizer in wado request
20160124	mvh	Pass Type to ChangeUIDBack; name zip entries to SopInstanceUID; fix wado named anonymizer
20160220	mvh	Allow forward slash in code detection autostart lua file
20160306	mvh	Added sop to modifier: command; DeleteImage can take patient:sopinstanceuid; 
			Added empty study and series to deleters where needed; deleteimage: command
20160306	mvh	Added move: command
20160306	mvh	Fix progress info of DicomMove function
20160316	mvh	Added progress to export: - note does not uncompress data unless scripted so; extract:-n,query sorts 
20160317	mvh	Added todbf: command; removed convoluted extract:n and extract:-n
20160319	mvh     Decompress images in WADO jpg client, forgotten when cropping
20160319	mvh     1.4.19beta release for windows
20160424	mvh     Linux fixes
20160516	mvh	Added JpegLS to WADO interface
20160516	pm	Small fix in cgi status display get_remotea --> get_remoteae; do not use hardcoded "acrnema.map"
20160516	bcb	Split images counter for Jpg and Bmp (e.g. ImagesToBmpFromGui)
20160716	mvh	Added lua tempfile and returnfile; added n0 and nN transferSyntax to wado server: simplify and NKI compression
20160718	mvh	WADO now supports image/bmp and experimental bmp strip (set frameNumber<0)
20160723	mvh	Added experimental jpeg strip function; allow multiple columns for jpeg and bmp (set frameNumber to -columns)
20160827	mvh	Added Lua destroy, reject and retry functions; version to 1.4.19beta3
20160828	mvh	properly copy Called and Calling, fix ExportConverter script passing; version to 1.4.19beta4
20160830	mvh	Fix in importconverter change above, version to 1.4.19beta3a
20160901	mvh	Fix passed windowwidth from CGI WADO request to wadorequest, was divided by 2
20160905	mvh	Fix stripping of space from called and calling: crashed when empty e.g. all spaces
20160905	mvh	version to 1.4.19beta3b
20160913	mvh	Added lua ConvertBinaryData to allow fast R interface without vstruct
20160917	mvh	Completed it with s conversion
20161120	mvh	dbquery returns nil if query fails not {}; version to 1.4.19
20161204	mvh	Catch image VR null in getptr (lua image access), test return of recompress in scripting
20161212	mvh	Retry after 5s in Linux LoadAndDeleteDir after discussion with snalbansed
			This is because there is no other way of findibg if a file is finished
20161214	mvh	Loop retry 30 times
20170203	mvh	Replace ' with _ in loadhl7
20170208	mvh	Release 1.4.19
20170226	mvh	Increased buffer size for servercommand return data to 5120
20170312        mvh     Added cgi style options or upload file as second argument to Lua servercommand
20170316        mvh     Started on 'binary' mode for returning large objects
20170419	mvh	Made binary mode compiling; use (merge) stage for merging UIDs, max merge# to 1000; version 1.4.19a
20170430	mvh	Release 1.4.19a; note that 'binary' mode of lua servercommand is non functional
20170511	mvh	Fix overrun affecting ModalityWorkListQueryResultConverter0
20170528	mvh	Fix ModifyPATIDofImageFile and MergeUIDofImageFile set KeepImages to FALSE
			version to 1.4.19b
20170530	mvh	Add empty StudyUID to deleteseries()
20170921	mvh	Added gamma on display functions, e.g. save bmp
20171009	mvh     Fix for crash when c-move is canceled, thanks Tetraib (Thibaut)
20171010        mvh     Added Content-length to html header
20180122        mvh     Fix servercommand binary mode, added <upload and >download file mode; add output file option (r7) to wadorequest
20180122        mvh     Version to 1.4.9c; added Global.DGATEVERSION
20180204        mvh     Report save jpg etc to SystemDebug
20180301        mvh     into_queue_unique resets time stamp for every save; silent destroy2 method
20180315	mvh	started on C-GET
20180315	mvh	C-GET seems to work OK; Added RetryForwardRemoteDICOMError config
20181115	mvh     Fixed default of ImportExportDragAndDrop to 1
20181117	mvh     Added luasocket to linux compile
20181124	mvh     Made it 1.4.19c1
20181215	mvh     Adjusted and simplified servercommand to allow return of long strings
20181215	mvh     Version to 1.4.19d; added Serialize for DicomObject and DicomArray
20181216	mvh     Improved Serialize to escape and ignore unknowns; added sequences; comment if too long
20181216	mvh     Changed callback to progress port of dicommove to conform to key=value standard
20181216	mvh     Added PATIENTP and STUDYP levels to dicomquery for PatientStudyOnly queries
20181217	mvh     Added AddToSequence, DicomArray:Add() and dicomprint; 
			better callback and return error string in dicommove
20181218	mvh     Silence conquest ImportConverters run from Lua; added callback to dicomprint
20181220	mvh     Added \n after all Progress.printf output; Progress port uses lossless TCP
20181221	mvh     Send printer information [lua]printserver or to Progress port 
20181222	mvh     Added luadicomget; removed superfluous DCO from luadicomquery
20181223	mvh     Put progress port back to TCP until new GUI committed
20181227	mvh     -x!port uses TCP; -xport uses UDP
20181229	mvh     Added luadicomstore
20181231	mvh     Create all threads with 2MB stack space, Intel compiler seems to be less forgiving
20181231	mvh     Allow luaservercommand to return NIL; increased HTML command buffer space
20181231	mvh     Version to 1.5.0-alpha
20190101	mvh     Fix some leaks in new lua functions; all calls to NewDeleteDICOM get patid & 3 UIDs, can be empty
20190102	mvh     Improved error handling in lua functions; removes superfluous DCO from virtualquery
20190103	mvh     Fixed dicomstore; protect ImportConverters (e.g. move:) against crash
			Fix deletestudies: command; added Lua tickcount() function
			Fix leaks in deletepatient: dicomstore() and dicomget()
20190112	mvh     Use strrchr to find ':' in patientID:uid pair; allows : in patientID
20190318	mvh     Made it test2
20190501	mvh     Fixed definition of HandleError to BOOL
20190501	mvh     Made it test3
20191017	mvh     Added attachfile server command and web page
20191019	mvh     Set ConfigFile and BaseDir in cgi mode, chdir to it, added -hAE
20191019	mvh     Retired scheduletransfer and some unused web interfaces bacause of compiler limit
20191019	mvh     Allow numbers inside Dicom names; fix querycache for linux; 1.5.0-alpha-t4
20191209        mvh     attachfile will also process zip file; version to 1.5.0-beta
20191212        mvh     Added lua CGI() call; returns post_buf as is
20191215        mvh     Fix in above; also read post_buf for other requests (GET, PUT), unlink uploadedfile at end
20191219        mvh     Fix zip in attachfile for Linux
20191221        mvh     Added lua listoldestpatients(max, device, sort)
20191227        mvh     Fix in reading POST parameters with CGI(); set filename for export: and convert_to_dicom:
20200107        mvh     attachfile: now reads embedded file if available; added lua dicomecho()
20200108        mvh     Allow e.g. servercommand('Command["9999,0401"]="val"') as alternative to returnfile
20200109        mvh     Removed double Content-length in download header; fix %c format in querycache; cast in serialize
20200118        mvh     Added rudimentary json support call DicomObject:Serialize(true)
20200119        mvh     Fix escaping in it and fix recursion
20200120        mvh     1.5.0-beta1: added logging to ForwardAssociationLevel close; Added DelayedForwarderThreads
			Added experimental 'split ' clause to delayed forward (cannot be used with imagetype clause)
20200122        mvh     Set Command for many (not all) converters, passing DCO to SaveToDisk, CheckObject and CallImportConverterN
20200124        mvh     Added lua: Im/ExportConverter; must be last in line as it may contain ;  " { or }; Note e.g. 
			%f is still substituted, beware of using % in lua code, escape to %%; added %g generate uid
20200125        mvh     Added DCOextra to lua dicommove and DcmMove2, will be added to DCO to control move 
20200126        mvh     Added --mk_binary_dic: command; Simplified servercommand convert_to_xxx
20200203	mvh	1.5.0-beta2: lsp removed DataPointer stuff to support lsp array change
			Added LUA51EXTERN option for dynamic loading of lua5.1.dll
20200211	mvh	Take luasocket out when LUA51EXTERN defined
20200215	mvh	Make sure globalPDU.L is created in dolua:
20200216	mvh	1.5.0beta3; remove static linked luasocket
20200302        mvh     Open lua scripts in im/exportconverters at Global.basedir .. name (relative paths)
			Note: web contents and process require absolute paths
20200307        mvh     Instead of tempfile now can use Command:SetVR(0x9999,0x0403,"val")
			This allows two way data communication: input and output using 0x9999,0x0401
20200311        mvh	1.5.0-beta4 version, fix attachfile of zip under Linux 
20200311        mvh	Make sure progress closes on failure of commands 
20200314        mvh	Release 1.5.0; Update dgate -?
20200316        mvh	Added dogloballua: servercommand - runs in global context in critical section
			Also added critical section for nightly, background and dgate.lua
20200528        mvh	Version to 1.5.0a; changed truncation of AnyPageExceptions from 255 to 511
                        Use single DB per folder in LoadAndDeleteDir
20200704        mvh	Added missing command to submit2 importconverter; added command [xxx] to imp/exp
20200704        mvh	Renamed dogloballua to globallua; added to dgate -?
20200722        mvh	Version to 1.5.0b
20201012        mvh	Fixed initialization of HL7 parser, and allow Unix EOL
20201029	mvh	Pass db into ChangeUID(Back) and New/OldUIDsInDICOMObject
20201101	mvh	Added 3x5s retry on AttachFile and AddImageFile in LoadAndDeleteDir
20201116	mvh	Added global_query_string and -y option
20201224	mvh	Added regen and regenFail converter; luacompress returns nil on failure
20210325	mvh	Version 1.5.0c
20210415	mvh	luadicomstore returns an dicomarray of results
20210501	mvh	Allow use of 0x0008,0x3001 sequence to return servercommand result
20210509	mvh	Added UT in header dump; simplified overflow check in luaserialize, upped limit
20210928        mvh     pivoglot: Removed max(), do not use strstr()>0; allow host:port for dicomecho and dicomget
20210930        mvh     Added md5() for lua
20220131        mvh     Added object:Serialize(true, true): json with pixel data included
20220210        mvh     Fix object:Serialize(true, false); add convert_to_json, -jj and -js options
20220227        mvh     Added json input o=DicomObject:new(json), o:SetVR(g, e, json), o:Copy(json) (latter merges)
20220307	mvh	-ar has optional dir folder; e.g. dgate -arMAG0,a*.* regens all folders with 'a' 
20220308	mvh	Fix buffer overflow in luaserialize 
20220718	mvh	-$ sets stdout to binary; lua servercommand2 WIP
20220729	mvh	serialize has a third parameter: creates dicomweb style json output
20220729	mvh	fix dicomweb formatting of empty VRs
20220803	mvh	Fix serializing of empty sequences; fix json parsing dealing with sequences and undefined items
20220805	mvh	Added "binary" mode to luaservercommand; will not remove trailing zero/space
                        Set 0x9999,0x0404 if payload file has odd length and process when recieving to lua (only for now)
20220807	mvh	Set e.g. Rows (US and UL elements) to "" to make it empty instead of 0
20220809	mvh	Fix -$ argument for Linux (ignored)
20220810	mvh	Added support for json item names like "00100020" as well as tag names
20220814	mvh	Added and use wadoparse server command in cgi exe mode, keep wadorequest
20220815	mvh	Most output formats that recognise cgi now accept binary 
20220816	mvh	Fix Serialize in dicomweb format for sequences; use uppercase FX for dicomweb
20220818	mvh	Added e.g. scrub -Private,7FEO,00100020 (delete group,vr) or scrub +0010 (keep)
20220819	mvh	Code IS and DS as proper array in dicomweb mode; fixed AT
20220819	mvh	Removed trailing space of ST items, encode OB if includepixeldata
			If json format and not encodepixeldata do nothing (was taken as string)
			Control OB OW and OF with includepixeldata
20220820	mvh	Fixed bug in serialising empty object missing opening bracket, and empty DS and IS
20220820	mvh	Fix when a sequence with just OW data was serialized without includepixeldata
20220821        mvh     Added dicomread(query); local replacement for dicomget with scrub; bit slower though..
			Another fix for empty US elements
20220823        mvh     CGI("_filename_) to get uploaded file name; protect writes there
20220824    	mvh	Second parameter of dicomread allows header truncation
20220828    	mvh	CGI allows dgate -pPORT -qIP -hAE -ycommand (with -y last!)
20220830        mvh     Reject __MACOSX folder, and files starting with ._ in LoadAndDeleteDir; 
			pass DB to LoadAndDeleteDir to avoid DB activity each second in monitorthread
20220830        mvh     ---- RELEASE 1.5.0c -----
20221014        mvh     Allow response from servertask <file
20221016        mvh     Handle odd length return data from servercommand; and odd length upload
			Note: dgate.dic must be updated to read the conquest items properly
20221220        mvh     Added DelayIncomingFileOpen (ms) for incoming files to avoid reading partial
20230612        mvh     Allow () for command in process by clause; by clause is tested for uniqueness
20230701        mvh     Pass ConnectedIP as argument to make it thread safe
20230701        mvh     ---- RELEASE 1.5.0d -----

ENDOFUPDATEHISTORY
*/

#define DGATE_VERSION "1.5.0d"

//#define DO_LEAK_DETECTION	1
//#define DO_VIOLATION_DETECTION	1
//#define FAILSAFE_STORAGE

// max ExportConverters lines AND max forwards per line
#define MAXExportConverters 20
#define DEFAULT_LOSSY "95"

#ifndef UNUSED_ARGUMENT
#define UNUSED_ARGUMENT(x) (void)x
#endif

#include <stdarg.h>

#	include	"dgate.hpp"
#	include	"nkiqrsop.hpp"
#	include	<time.h>
#ifdef WIN32
#	include <io.h>
#	include <process.h>
#	include <conio.h>
#else
#	include <unistd.h>
#	include <pthread.h>
#       include <dirent.h>
#ifndef DARWIN
#	include <malloc.h>
#endif //DARWIN
#endif
#	include <stdio.h>
#	include <ctype.h>
#	include <fcntl.h>
#	include <sys/stat.h>

#ifdef WIN32
#if !defined(__BORLANDC__) && !defined(__WATCOMC__)
#	include	<odbcinst.h>
#endif

#if defined(DO_LEAK_DETECTION) || defined(DO_VIOLATION_DETECTION)
#	include <crtdbg.h>
#endif

#endif

#ifndef ODBC_ADD_SYS_DSN
#define  ODBC_ADD_SYS_DSN 4         // add a system DSN
#endif

BOOL	NoThread=FALSE;					// debug non-threaded
char *DGATEVERSION = DGATE_VERSION;

#ifndef UNIX  	// Win32...
#	define THREAD_RETURN_FALSE FALSE
#	define THREAD_RETURN_TRUE TRUE
	typedef BOOL ThreadRoutineType;
	typedef int ThreadRoutineArgType;
#else		// UNIX...
#	include <pthread.h>
#	include <unistd.h>
#	include <stdlib.h>
#	include <string.h>
#	include <signal.h>
#	include <sys/types.h>
#	include <sys/stat.h>
//#	include <fstream.h>
#	include "npipe.hpp"
#ifndef PATH_MAX
#   define PATH_MAX FILENAME_MAX
#endif
#	define THREAD_RETURN_FALSE ((void *)NULL)
#	define THREAD_RETURN_TRUE ((void *)1)
//#	define closesocket(s) close(s)
#	define CloseHandle(h)
void Sleep(int h)
{ struct timespec t;
  if (NoThread) return;
  t.tv_sec =(h/1000);
  t.tv_nsec=(h%1000)*1000000;
  nanosleep(&t, NULL);
}
#	define eof(h) FALSE
	typedef void *	ThreadRoutineType;
	typedef void * ThreadRoutineArgType;
#endif

#include "gpps.hpp" //DUCKHEAD92

// This file compiles into two different "modes".  The first (DEBUG_MODE) defined will
// compile into a Windows NT console application.  Just run this exactly like
// any other NT console app (it provides everything the service does).  The second
// mode (DEBUG_MODE undefined) is the service.

#ifndef DEBUG_MODE
#	define	DEBUG_MODE
#endif

// IPC Block is a block of data which is passed from parent to child with the socket#
#define IPCBlockMagic 0xafaced0a
typedef	struct	_IPCBlock
	{
#ifdef	UNIX
	int			Magic;
#endif
	int			Socketfd;
	}	IPCBlock;

BOOL	RunServer = TRUE;				// reset for regen etc

char	RootConfig[64]	=	"sscscp";		// main entry in ini file

// Later make everything MAX_PATH or similar?
char    BaseDir[256] 	=	"";			// where the files are

#ifndef	UNIX
char	ConfigFile[256]	=	".\\dicom.ini";		// name ini file
char	DicomDict[256]	=	".\\dgate.dic";		// name dicomdictionary file
#else
char	ConfigFile[512]	=	"dicom.ini";
char	DicomDict[256]	=	"dgate.dic";
#endif

#ifndef UNIX						// for routing IODs
char	AutoRoutePipe[128] = "\\\\.\\pipe\\AutoRoutePipe";
char	AutoRouteExec[128] = "aroute.exe";
#else
char	AutoRoutePipe[128] = "/tmp/AutoRoutePipe";
char	AutoRouteExec[128] = "aroute";
#endif

extern	Array < ACRNemaAddress * > ACRNemaAddressArray; // for -a listing
int	DebugLevel = 0;					// increased after error
RTC	VRType(TRUE);					// VR translation table
extern	int FileCompressMode;
extern unsigned int gl_iFileCounter;

int	ThreadCount = 0;				// Counts total number of associations
int	OpenThreadCount = 0;				// Counts total number of open associations
int	EnableReadAheadThread=1;			// to enable/disable read ahead thread
int	WorkListMode = 0;				// use worklist to update incoming data

char	DroppedFileCompression[16];			// compression of dropped files
char	IncomingCompression[16];			// compression of incoming images
char	ArchiveCompression[16];				// compression of images to be archived
char	TestMode[16]="";				// if set, appended to filename for comparison
char	StatusString[256]="";				// status of submission and move for web i/f

int	StorageFailedErrorCode = 0x110;			// returned when C-STORE fails (default failed processing)
int	FailHoldOff = 60;				// holdoff in seconds aftre last export/copy failure
int	RetryDelay = 100;				// delay in seconds aftre last export/copy failure for retry
int	QueueSize = 128;				// number of entries in export/copy queue (2k per entry in memory)
int	ForwardCollectDelay = 600;			// delay for forward patient to etc ....

char	VirtualServerFor[11][48];			// AE's of servers providing data for this one (cache function) + parameters
BOOL	VirtualServerPerSeries[11];			// If set, get images at SERIES level instead of at IMAGE level
int	CacheVirtualData=1;				// if set, keep (cache) passed through data

// jpeg, jpeg2k global stuff
int	gJpegQuality=95;				// The quality of the lossy jpeg or j2k image
int	gUseOpenJpeg=0;					// If we have both Jasper and OpenJPEG, it lets the dicom.ini choose.

extern int FixKodak;
extern int NumIndexing;		// from odbci.cpp: is -1 on startup, >0 during DBF indexing
extern int DoubleBackSlashToDB;
extern int UseEscapeStringConstants;
extern int EnableComputedFields;
extern int FileCompressMode;

// counters for status display
int	StartTime;					// when dgate is started
int	TotalTime=0;					// total run time in s
int	LoadTime=0;					// total time loading
int	ProcessTime=0;					// total time processing (downsize)
int	SaveTime=0;					// total time saving

int	ImagesSent=0;					// how many images were sent
int	ImagesReceived=0;				// idem recieved
int	ImagesSaved=0;					// idem saved
int	ImagesForwarded=0;				// idem forwarded
int	ImagesExported=0;				// executable as export converted
int	ImagesCopied=0;					// copy as export converter

int	IncomingAssociations=0;				// accepted incoming associations (start thread)
int	EchoRequest=0;					// c-echo requests
int	C_Find_PatientRoot=0;
int	C_Move_PatientRootNKI=0;
int	C_Move_PatientRoot=0;
int	C_Get_PatientRoot=0;
int	C_Find_StudyRoot=0;
int	C_Move_StudyRootNKI=0;
int	C_Move_StudyRoot=0;
int	C_Get_StudyRoot=0;
int	C_Find_PatientStudyOnly=0;
int	C_Find_ModalityWorkList=0;
int	C_Move_PatientStudyOnlyNKI=0;
int	C_Move_PatientStudyOnly=0;
int	C_Get_PatientStudyOnly=0;
int	UnknownRequest=0;

int	CreateBasicFilmSession=0;			// printer actions
int	DeleteBasicFilmSession=0;
int	ActionBasicFilmSession=0;
int	SetBasicFilmSession=0;
int	CreateBasicFilmBox=0;
int	ActionBasicFilmBox=0;
int	SetBasicFilmBox=0;
int	DeleteBasicFilmBox=0;
int	SetBasicGrayScaleImageBox=0;
int	SetBasicColorImageBox=0;

int	GuiRequest=0;					// server command requests from GUI
int	ImagesToGifFromGui=0;
int	ImagesToBmpFromGui=0;
int	ImagesToJpgFromGui=0;
int	ImagesToDicomFromGui=0;
int	ExtractFromGui=0;
int	QueryFromGui=0;
int	DeleteImageFromGui=0;
int	DeletePatientFromGui=0;
int	DeleteStudyFromGui=0;
int	DeleteStudiesFromGui=0;
int	DeleteSeriesFromGui=0;
int	MovePatientFromGui=0;
int	MoveStudyFromGui=0;
int	MoveStudiesFromGui=0;
int	MoveSeriesFromGui=0;
int	AddedFileFromGui=0;
int	DumpHeaderFromGui=0;
int	ForwardFromGui=0;
int	GrabFromGui=0;
int	NoDicomCheck=0;

extern int	DatabaseOpen;				// database activity
extern int	DatabaseClose;
extern int	DatabaseQuery;
extern int	DatabaseAddRecord;
extern int	DatabaseDeleteRecord;
extern int	DatabaseNextRecord;
extern int	DatabaseCreateTable;
extern int	DatabaseUpdateRecords;
extern int	QueryTime;

extern int	SkippedCachedUpdates;			// entering into database levels
extern int	UpdateDatabase;
extern int	AddedDatabase;

extern int	NKIPrivateCompress;			// compression activity
extern int	NKIPrivateDecompress;
extern int	DownSizeImage;
extern int	DecompressJpeg;
extern int	CompressJpeg;
extern int	DecompressJpeg2000;
extern int	CompressJpeg2000;
extern int	DecompressedRLE;
extern int	DePlaned;
extern int	DePaletted;
extern int	RecompressTime;
extern int	gpps, gppstime;

//These tables define parameters to be exported to lua for a global object

#define def(a) #a, &a, 
#define def2(a) #a, a, sizeof(a),

struct iglobal {char *name; int  *data; };
struct sglobal {char *name; char *data; int len; };

struct iglobal iglobals[]=
{ 
def(StartTime)
def(TotalTime)
def(LoadTime)
def(ProcessTime)
def(SaveTime)	
def(ImagesSent)		
def(ImagesReceived)
def(ImagesSaved)		
def(ImagesForwarded)	
def(ImagesExported)	
def(ImagesCopied)		
def(IncomingAssociations)	
def(EchoRequest)	
def(C_Find_PatientRoot)
def(C_Move_PatientRootNKI)
def(C_Move_PatientRoot)
def(C_Get_PatientRoot)
def(C_Find_StudyRoot)
def(C_Move_StudyRootNKI)
def(C_Move_StudyRoot)
def(C_Get_StudyRoot)
def(C_Find_PatientStudyOnly)
def(C_Find_ModalityWorkList)
def(C_Move_PatientStudyOnlyNKI)
def(C_Move_PatientStudyOnly)
def(C_Get_PatientStudyOnly)
def(UnknownRequest)
def(CreateBasicFilmSession)
def(DeleteBasicFilmSession)
def(ActionBasicFilmSession)
def(SetBasicFilmSession)
def(CreateBasicFilmBox)
def(ActionBasicFilmBox)
def(SetBasicFilmBox)
def(DeleteBasicFilmBox)
def(SetBasicGrayScaleImageBox)
def(SetBasicColorImageBox)
def(GuiRequest)
def(ImagesToGifFromGui)
def(ImagesToBmpFromGui)
def(ImagesToJpgFromGui)
def(ImagesToDicomFromGui)
def(ExtractFromGui)
def(QueryFromGui)
def(DeleteImageFromGui)
def(DeletePatientFromGui)
def(DeleteStudyFromGui)
def(DeleteStudiesFromGui)
def(DeleteSeriesFromGui)
def(MovePatientFromGui)
def(MoveStudyFromGui)
def(MoveStudiesFromGui)
def(MoveSeriesFromGui)
def(AddedFileFromGui)
def(DumpHeaderFromGui)
def(ForwardFromGui)
def(GrabFromGui)
def(DatabaseOpen)
def(DatabaseClose)
def(DatabaseQuery)
def(DatabaseAddRecord)
def(DatabaseDeleteRecord)
def(DatabaseNextRecord)
def(DatabaseCreateTable)
def(DatabaseUpdateRecords)
def(QueryTime)
def(SkippedCachedUpdates)
def(UpdateDatabase)
def(AddedDatabase)
def(NKIPrivateCompress)
def(NKIPrivateDecompress)
def(DownSizeImage)
def(DecompressJpeg)
def(CompressJpeg)
def(DecompressJpeg2000)
def(CompressJpeg2000)
def(DecompressedRLE)
def(DePlaned)
def(DePaletted)
def(RecompressTime)
def(gpps)
def(gppstime)
def(NoDicomCheck)

def(DebugLevel)
def(ThreadCount)
def(OpenThreadCount)
def(EnableReadAheadThread)
def(WorkListMode)
def(StorageFailedErrorCode)
def(FailHoldOff)
def(RetryDelay)
def(QueueSize)
def(ForwardCollectDelay)
def(CacheVirtualData)

def(gJpegQuality)
def(gUseOpenJpeg)

def(FixKodak)
def(NumIndexing)
def(DoubleBackSlashToDB)
def(UseEscapeStringConstants)
def(EnableComputedFields)
def(FileCompressMode)

"", NULL
};

struct sglobal sglobals[]=
{ 
def2(RootConfig)
def2(BaseDir)
def2(ConfigFile)
def2(DicomDict)
def2(AutoRoutePipe)
def2(AutoRouteExec)

def2(DroppedFileCompression)
def2(IncomingCompression)
def2(ArchiveCompression)
def2(TestMode)
def2(StatusString)
def2(DGATEVERSION)

"", NULL
};

#ifndef ctime_r
#define ctime_r(a, b) ctime(a)
#endif

#ifndef localtime_r
#define localtime_r(a, b) memcpy(b, localtime(a), sizeof(struct tm))
#endif

void ConfigDgate(void);
BOOL ToGif(DICOMDataObject* pDDO, char *filename, int size, int append, int level, int window, unsigned int frame, float gamma);
BOOL ToBMP(DICOMDataObject* pDDO, char *filename, int size, int append, int level, int window, unsigned int frame, float gamma);
BOOL ToJPG(DICOMDataObject* pDDO, char *filename, int size, int append, int level, int window, unsigned int frame, int quality, float gamma);
int MaybeDownsize(DICOMDataObject* pDDO, DICOMCommandObject* pDCO, int size);
BOOL ExtractFrame(DICOMDataObject* pDDO, unsigned int Frame);
BOOL ExtractFrames(DICOMDataObject* pDDO, unsigned int FirstFrame, unsigned int LastFrame, int skip);
int GetNumberOfFrames(DICOMDataObject* pDDO);

/* error handling for out of memory and such */

void MyDcmError(int error, const char *message, int count)
	{
	char buffer[300];
	char szRootSC[64];

	if (error==DCM_ERROR_DEBUG)
		{ 
		if (DebugLevel==0) return;
	  	sprintf(buffer, message, count);
		strcat(buffer, "\n");
		SystemDebug.printf(buffer);
		}
	else
		{
	 	strcpy(buffer, "***");
		sprintf(buffer+3, message, count);
		strcat(buffer, "\n");
		OperatorConsole.printf(buffer);
		}

        if (error==DCM_ERROR_MEMORY)
		{
		if (MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile))
			{
			MyGetPrivateProfileString(szRootSC, "IgnoreOutOfMemoryErrors", "0", buffer, 64, ConfigFile);
			if (atoi(buffer)) return;
			}
		OperatorConsole.printf("***A fatal error occurred (out of memory) - closing server\n");
		exit(1);
		}
	}

void StatusDisplay(FILE *f)
{ char TimeString[80], buf[64];
  time_t T=StartTime;
  Database DB;
  unsigned int i;
  int libjasper=0;
  int libjpeg=0;
  int intjpeg=1;
	
  UNUSED_ARGUMENT(buf);// Added for unused variable warning. bcb

#ifdef HAVE_J2K
  libjasper=1;
#endif
#ifdef HAVE_LIBJPEG
  libjpeg=1;
  intjpeg=0;
#endif
#ifdef NOINTJPEG
  intjpeg=0;
#endif

  strcpy(TimeString, ctime_r(&T, buf));
  TimeString[strlen(TimeString)-1] = '\0';

  fprintf(f, "DICOM server '%s' (version %s, port %s, bits %d) was started on %s\n", MYACRNEMA, DGATE_VERSION, Port, (int)(8*sizeof(void *)), TimeString);
  fprintf(f, "Old JPEG decoder=%d, JPEGLIB jpeg codec=%d, LIBJASPER jpeg2000 codec=%d\n", intjpeg, libjpeg, libjasper);

  fprintf(f, "Run time (s) total %d, query %d, load %d, save %d, compress %d, process %d, gpps %d\n", TotalTime, QueryTime/1000, LoadTime, SaveTime, RecompressTime, ProcessTime, gppstime);
  fprintf(f, "Associations=%d; Threads=%d (%d open); Images sent=%d, recieved=%d, forwarded=%d\n", IncomingAssociations, ThreadCount, OpenThreadCount, ImagesSent, ImagesReceived, ImagesForwarded);
  fprintf(f, "Images printed=%d, in color=%d\n", SetBasicGrayScaleImageBox+SetBasicColorImageBox, SetBasicColorImageBox);
  fprintf(f, "Activity: Echo:%d, Find:%d, Move:%d, Unknown:%d, gpps:%d\n", EchoRequest, C_Find_PatientRoot+C_Find_StudyRoot+C_Find_PatientStudyOnly+C_Find_ModalityWorkList,
  	C_Move_PatientRootNKI+C_Move_PatientRoot+C_Move_StudyRootNKI+C_Move_StudyRoot+C_Move_PatientStudyOnlyNKI+C_Move_PatientStudyOnly, UnknownRequest, gpps);
  fprintf(f, "Images (de)compressed: NKI %d, JPEG %d, JPEG2000 %d, RLE %d, Planes removed %d, Palettes removed %d, Downsize %d\n",
	NKIPrivateCompress+NKIPrivateDecompress, DecompressJpeg+CompressJpeg, DecompressJpeg2000+CompressJpeg2000,
	DecompressedRLE, DePlaned, DePaletted, DownSizeImage);
  for (i=0; i<MAGDevices; i++)
    fprintf (f, "Space on MAG%d  : %d MByte\n", i, CheckFreeStoreOnMAGDevice(i));
  for (i=0; i<CACHEDevices; i++)
    fprintf (f, "Space on CACHE%d: %d MByte\n", i, CheckFreeStoreOnCACHEDevice(i));
  if (DB.Open ( DataSource, UserName, Password, DataHost ) )
  { switch(DB.db_type)
    { case DT_ODBC:             fprintf(f, "Database type: ODBC connection\n"); break;
      case DT_POSTGRES:         fprintf(f, "Database type: native PostGres connection\n"); break;
      case DT_MYSQL:            fprintf(f, "Database type: native MySQL connection\n"); break;
      case DT_SQLITE:           fprintf(f, "Database type: built-in SQLite driver\n"); break;
      case DT_DBASEIII:         fprintf(f, "Database type: built-in Dbase driver\n"); break;
      case DT_DBASEIIINOINDEX:  fprintf(f, "Database type: built-in Dbase driver (indices disabled)\n"); break;
      case DT_NULL:             fprintf(f, "Database type: NULL driver (black hole)\n"); break;
    }
    for (i=0; i<10; i++)
      if (DB.dbase_handles[i]) 
        fprintf(f, "Database %s has %d records\n", DB.dbase_handles[i]->filename, DB.dbase_handles[i]->reccount);
  };
}

// Dump an association request for the status display

static BOOL AbstractSyntaxEnabled(CheckedPDU_Service &p, AbstractSyntax	&AbsSyntax)
	{
	UINT	Index;

	if ( ! p.SOPUIDListCount )
		return ( TRUE );

	Index = 0;
	while ( Index < p.SOPUIDListCount )
		{
		if ( UID ( p.SOPUIDList [ Index ] ) ==
			AbsSyntax.AbstractSyntaxName)
			return ( TRUE );
		++Index;
		}
	return ( FALSE );
	}


void
IARQ (AAssociateRQ	&ARQ, BOOL showall)
	{
	char	s[128];

	SystemDebug.printf("A-ASSOCIATE-RQ Packet Dump\n");

	memset(s, 0, 64);
	memcpy(s, ARQ.CallingApTitle, 16);
	OperatorConsole.printf("\tCalling Application Title : \"%s\"\n", s);

	memset(s, 0, 64);
	memcpy(s, ARQ.CalledApTitle, 16);
	OperatorConsole.printf("\tCalled Application Title : \"%s\"\n", s);

	memset(s, 0, 64);
	memcpy(s, ARQ.AppContext.ApplicationContextName.GetBuffer(1),
		ARQ.AppContext.ApplicationContextName.GetSize() );
	OperatorConsole.printf("\tApplication Context : \"%s\", PDU length: %d\n", s, ARQ.UserInfo.GetMax());

	SystemDebug.printf("\tNumber of Proposed Presentation Contexts: %d\n",
		ARQ.PresContexts.GetSize());

	unsigned int	Index = 0;
	while (Index < ARQ.PresContexts.GetSize() )
		{
		PresentationContext	PC;

		PC = ARQ.PresContexts.Get(Index);
		PC.TrnSyntax.ClearType = FALSE;

		memset(s, 0, 64);
		memcpy(s,
		PC.AbsSyntax.AbstractSyntaxName.GetBuffer(1),
			PC.AbsSyntax.AbstractSyntaxName.GetSize()%64 );

		OperatorConsole.printf("\tPresentation Context %d \"%s\" %d\n", Index, s, AbstractSyntaxEnabled(*(CheckedPDU_Service*)(&ARQ), PC.AbsSyntax));
		++Index;

		if (showall)
			{
			unsigned int IndexTrn = 0;
			while ( IndexTrn < PC.TrnSyntax.GetSize())
				{
				UID uid;

				TransferSyntax TrnSyntax = PC.TrnSyntax.Get(IndexTrn);
				memset(s, 0, 64);
				memcpy(s,
					TrnSyntax.TransferSyntaxName.GetBuffer(1),
					TrnSyntax.TransferSyntaxName.GetSize()%64 );

				uid = TrnSyntax.TransferSyntaxName;

				OperatorConsole.printf("\t\tTransfer Syntax %d \"%s\" %d\n", IndexTrn, s,
					((CheckedPDU_Service*)&ARQ)->CanYouHandleTransferSyntax(TrnSyntax));

				IndexTrn++;
				}
			}
		}
	}

// Help routines to allow dumping of logs after an error occurs
// Test if character is alphanumerical

BOOL	IsAlpha(BYTE	c)
	{
	if((c >= ' ')&&(c <= 125))
		return ( TRUE );
	if(!c)
		return ( TRUE );
	return ( FALSE );
	}

// Test if all of string is alphanumerical

BOOL	IsDataAlpha(UINT	Size, BYTE	*data)
	{
	if(Size > 16)
		Size = 16;
	if(!Size)
		return ( FALSE );
	--Size;
	while(Size)
		{
		if(!IsAlpha(data[Size]))
			return ( FALSE );
		--Size;
		}
	return ( TRUE );
	}

static void TranslateText(char* pSrc, char* pDest, int iSize)
{
	int i;

	/* Translate CR, LF and FF to escape sequences */
	for (i=0; i<iSize; i++)
		switch (*pSrc)
		{
		case '\\':
			*pDest++ = *pSrc;
			*pDest++ = *pSrc++;
			break;
		case '\n':
			*pDest++ = '\\';
			*pDest++ = 'n';
			pSrc++;
			break;
		case '\r':
			*pDest++ = '\\';
			*pDest++ = 'r';
			pSrc++;
			break;
		case '\f':
			*pDest++ = '\\';
			*pDest++ = 'f';
			pSrc++;
			break;
		default:
			*pDest++ = *pSrc++;
			break;
		}
	*pDest = 0;
}

// Print data from a single dicom object
BOOL	PrintUDATA(UINT	Size, void *data, UINT16 TypeCode, char *dest)
	{
	UINT	Index = 0;
	unsigned char	*c;
	UINT16	tint16;
	UINT32	tint32;
	BYTE	*b;
	LE_UINT16	*w;
	char	Str[512];
	LE_UINT32	*l32 = (LE_UINT32*)data;
	LE_UINT16	*l16 = (LE_UINT16*)data;
	LE_INT32	*s32 = (LE_INT32*)data;
	LE_INT16	*s16 = (LE_INT16*)data;
	float		*f32 = (float*)data;
	double		*f64 = (double*)data;
	
	dest[0]=0;

	if(!data)
		{
		if ( TypeCode == 'SQ' )
			{
			sprintf(dest, "Sequence Item");
			return ( TRUE );
			}
	
		sprintf(dest, "(empty)");
		return ( TRUE );
		}

	if ( ! TypeCode || TypeCode == 'UN' )
		{ 			
		switch ( Size )
			{
			case	2:
				tint16 = (*((UINT16 *) data));
				sprintf(dest, "(%d|%x)", (UINT) tint16, (UINT) tint16);
				break;
			case	4:
				tint32 = (*((UINT32 *) data));
				sprintf(dest, "[%d|%x]", (UINT) tint32, (UINT) tint32);
				break;
			default:
				if(IsDataAlpha(Size, (BYTE*) data))
					{
					if(Size > 256) Size = 256;
					TranslateText((char*)data, Str, Size);
					sprintf(dest, "\"%s\"", Str);
					break;
					}
				Index = 0;
				while(Index < Size)
					{
					if(Index >= 16)
						break;
					c = (unsigned char*)data;
					c += Index;
					if (isprint(*c))
						sprintf(dest+strlen(dest), "%2.2x(%c) ", *c, *c);
					else
						sprintf(dest+strlen(dest), "%2.2x ", *c);
					++Index;
					}
			}
		}
	else
		{
		switch ( TypeCode )
			{
			case	'AE':
			case	'AS':
			case	'CS':
			case	'DA':
			case	'DS':
			case	'DT':
			case	'IS':
			case	'LO':
			case	'LT':
			case	'PN':
			case	'SH':
			case	'ST':
			case	'TM':
			case	'UI':
			case	'UT':
				if(Size > 256) Size = 256;
				TranslateText((char*)data, Str, Size);
				sprintf(dest, "\"%s\" ", Str);
				break;

			case	'AT':
				Index = 0;
				while ( Index < Size )
					{
					UINT16	Grp, Elm;
					Grp = (*l16);++l16;
					Elm = (*l16);++l16;
					sprintf(dest+strlen(dest), "(%4.4x, %4.4x) ", Grp, Elm);
					Index += 4;
					}
				break;

			case	'OB':
				b = (BYTE*)data;
				while ( Index < Size )
					{
					if ( Index > 10 ) break;
					sprintf(dest+strlen(dest), "%2.2x ", *b);
					++b;
					++Index;
					}
				break;

			case	'OW':
				w = (LE_UINT16*)data;				
				while(Index < Size )
					{
					if ( Index > 20 ) break;
					sprintf(dest+strlen(dest), "%4.4x ", (UINT16)(*w));
					++w;
					++Index;
					++Index;
					}
				break;
					
			case	'UL':
				while ( Index < Size )
					{
					if ( Index > 40 ) break;
					sprintf(dest+strlen(dest), "%u ", (unsigned int)(*l32));
					++l32;
					Index += 4;
					}
				break;

			case	'US':
				while ( Index < Size )
					{
					if ( Index > 20 ) break;			
					sprintf(dest+strlen(dest), "%u ", (unsigned int)(*l16));
					++l16;
					Index += 2;
					}
				break;

			case	'SL':
				while ( Index < Size )
					{
					if ( Index > 40 ) break;
					sprintf(dest+strlen(dest), "%d ", (int)(*s32));
					++s32;
					Index += 4;
					}
				break;

			case	'SS':
				while ( Index < Size )
					{
					if ( Index > 20 ) break;
					sprintf(dest+strlen(dest), "%d ", (int)(*s16));
					++s16;
					Index += 2;
					}
				break;

			case	'FL':
			case	'OF':
				while ( Index < Size )
					{
					if ( Index > 40 ) break;
					sprintf(dest+strlen(dest), "%f ", (double)(*f32));
					++f32;
					Index += 4;
					}
				break;

			case	'FD':
				while ( Index < Size )
					{
					if ( Index > 40 ) break;
					sprintf(dest+strlen(dest), "%f ", (double)(*f64));
					++f64;
					Index += 8;
					}
				break;
			}
		}
	return ( TRUE );
	}

// Print a single dicom object
BOOL	DumpVR(VR *vr, FILE *f, int iDepth)
	{
	INT	Index;
	UINT16	TypeCode;
	char	b[256];
	char	s[256];
	char	d[514]; // "\n\n\n...."

	if (DebugLevel<1 && f==NULL)
		return ( TRUE );

	if (vr->Element==0) return TRUE;

	for (Index=0; Index<iDepth; Index++) b[Index]='>';
        b[iDepth]=0;

	s[0] = '\0';
	TypeCode = VRType.RunTimeClass(vr->Group, vr->Element, s);

	PrintUDATA ( vr->Length, vr->Data, TypeCode, d );

	if (f)
		{
		if(TypeCode)
			fprintf(f, "%s%4.4x,%4.4x %6u %c%c %-20.20s   %s\n",
				b, vr->Group, vr->Element, vr->Length,
				TypeCode>>8, TypeCode&0x00ff, s, d);
		else
			fprintf(f, "%s%4.4x,%4.4x %6d UN %-20.20s   %s\n",
				b, vr->Group, vr->Element, vr->Length, "", d);
		}
	else
		{
		if(TypeCode)
			SystemDebug.printf("%s%4.4x,%4.4x %6u %c%c %-20.20s   %s\n",
				b, vr->Group, vr->Element, vr->Length,
				TypeCode>>8, TypeCode&0x00ff, s, d);
		else
			SystemDebug.printf("%s%4.4x,%4.4x %6u UN %-20.20s   %s\n",
				b, vr->Group, vr->Element, vr->Length, "", d);
		}
	return(TRUE);
	}


// Print a composite dicom object + sequences
BOOL
NonDestructiveDumpDICOMObject(
	DICOMObject	*DO, FILE *f = NULL, int iDepth=0)
	{
	DICOMObject	DO2;
	VR		*vr;
	int		i;
	unsigned int 	Index;
		
	if(DebugLevel<1 && f==NULL)
		return ( TRUE );

	while((vr=DO->Pop()))
		{
		DumpVR(vr, f, iDepth);
		if ( vr->SQObjectArray )
			{
			Array < DICOMDataObject * > *ADDO =
				(Array<DICOMDataObject*>*) vr->SQObjectArray;
			
			Index = 0;
			while ( Index < ADDO->GetSize() )
				{
				NonDestructiveDumpDICOMObject(ADDO->Get(Index), f, iDepth+1);

				++Index;
				if (Index == ADDO->GetSize()) break;

				for (i=0; i<iDepth+1; i++)
					if (f)	fprintf(f, "-");
					else	SystemDebug.printf("-");
				if (f)	fprintf(f, "\n");
				else	SystemDebug.printf("\n");
				}
			}
		DO2.Push(vr);
		}
	DO->Reset();
	while((vr=DO2.Pop()))
		{
		DO->Push(vr);
		}
	return ( TRUE );
	}



// Check code from dumppacs pasted here for convenience
// List status of mag devices (free space)
BOOL
PrintFreeSpace()
	{
	unsigned int		Index;

	OperatorConsole.printf("** MAG DEVICE CHECK\n");
	OperatorConsole.printf("\n");
	Index = 0;
	while ( Index < MAGDevices )
		{
		OperatorConsole.printf ( "Free space on MAG   Device %d := %d MByte\n", Index,
			CheckFreeStoreOnMAGDevice(Index));
		++Index;
		}

	Index = 0;
	while ( Index < MIRRORDevices )
		{
		OperatorConsole.printf ( "Free space on MIRROR Device %d := %d MByte\n", Index,
			CheckFreeStoreOnMIRRORDevice(Index));
		++Index;
		}

	Index = 0;
	while ( Index < CACHEDevices )
		{
		OperatorConsole.printf ( "Free space on CACHE Device %d := %d MByte\n", Index,
			CheckFreeStoreOnCACHEDevice(Index));
		++Index;
		}

	return ( TRUE );
	}

// Print Out A-MAP data struct (used for outgoing C-MOVES)
BOOL
PrintAMap()
	{
	UINT	Index = 0;
	OperatorConsole.printf( "**	AE / IP-PORT Map dump\n");
	OperatorConsole.printf("\n");


	while ( Index < ACRNemaAddressArray.GetSize() )
		{
		ACRNemaAddress	*AAPtr = ACRNemaAddressArray.Get(Index);

		OperatorConsole.printf("%-17s %-30s %-10s %-16s\n",
			AAPtr->Name,
			AAPtr->IP,
			AAPtr->Port,
			AAPtr->Compress);
		++Index;
		}
	return ( TRUE );
	}

// Help routine for listing SQL database configuation; sql field type string
const char	*
SQLTypeSymName(
	int	SQLType)
	{
	switch ( SQLType )
		{
		case	SQL_C_CHAR:	return("SQL_C_CHAR");
		case	SQL_C_DATE:	return("SQL_C_DATE");
		default:		return("--bad--");
		}
	return("--bad--");
	}

// Help routine for listing SQL database configuration; dicom object type string
const char	*
DICOMTypeSymName(
	int	DICOMType)
	{
	switch ( DICOMType )
		{
		case	DT_STR:			return("DT_STR");
		case	DT_MSTR:		return("DT_MSTR");
		case	DT_DATE:		return("DT_DATE (DA)");	
		case	DT_UINT16:		return("DT_UINT16 (US)");
		case	DT_UINT32:		return("DT_UINT32 (UL)");
		case	DT_UI:			return("DT_UI (UI)");
		case	DT_FL:			return("DT_FL (FL)");
		case	DT_FD:			return("DT_FD (FD)");
		case	DT_STARTSEQUENCE:	return("Start sequence");
		case	DT_ENDSEQUENCE:		return("End sequence");
		default:			return ("--bad--");
		}
	return("--bad--");
	}

// Help routine for listing SQL database configuration; list one table
BOOL
DumpDD (
	DBENTRY	*DBE)
	{
	UINT	Index = 0;

	while ( DBE[Index].Group )
		{
		OperatorConsole.printf("0x%4.4x,0x%4.4x  %10s  %4d  %10s  %14s  %10s\n",
			DBE[Index].Group,
			DBE[Index].Element,
			DBE[Index].SQLColumn,
			DBE[Index].SQLLength,
			SQLTypeSymName(DBE[Index].SQLType),
			DICOMTypeSymName(DBE[Index].DICOMType),
			DBE[Index].HL7Tag);
		++Index;
		}
	return ( TRUE );
	}
	
// List full SQL database configuration
BOOL
PrintKFactorFile()
	{
//	UINT	Index = 0;
	OperatorConsole.printf( "**	Database Description dump\n");
	OperatorConsole.printf("\n");

	OperatorConsole.printf ("--Patient Database: %s\n", PatientTableName);
	DumpDD(PatientDB);
	OperatorConsole.printf ("--Study Database: %s\n", StudyTableName);
	DumpDD(StudyDB);
	OperatorConsole.printf ("--Series Database: %s\n", SeriesTableName);
	DumpDD(SeriesDB);
	OperatorConsole.printf ("--Image Database: %s\n", ImageTableName);
	DumpDD(ImageDB);
	OperatorConsole.printf ("--WorkList Database: %s\n", WorkListTableName);
	DumpDD(WorkListDB);
	return ( TRUE );
	}


// Delete everything of selected patient, optionally DB only
BOOL
DeletePatient(char *PID, BOOL KeepImages, int Thread)
	{
	VR		*vr;
	DICOMDataObject	DDO;

	if (PID==NULL || *PID==0) return FALSE;

	if (!KeepImages)
		OperatorConsole.printf("Deleting patient: %s\n", PID);
	else
		OperatorConsole.printf("Deleting database entries for patient: %s\n", PID);
	SetStringVR(&vr, 0x0010, 0x0020, PID); 
	DDO.Push(vr);
	SetStringVR(&vr, 0x0020, 0x000d, ""); 
	DDO.Push(vr);
	SetStringVR(&vr, 0x0020, 0x000e, ""); 
	DDO.Push(vr);
	SetStringVR(&vr, 0x0008, 0x0018, ""); 
	DDO.Push(vr);
	if (!KeepImages)
		NewDeleteDICOM(&DDO, Thread);
	else
		RemoveFromPACS(&DDO, KeepImages);

	return ( TRUE );
	}


void SetStringVR(VR **vr, int g, int e, const char *String);

// Delete study
BOOL
DeleteStudy(char *ID, BOOL KeepImages, int Thread)
	{
	VR		*vr;
	DICOMDataObject	DDO;

	if (ID==NULL || *ID==0) return FALSE;

	char *p = strrchr(ID, ':');

	if (p==0)
		{
		SetStringVR(&vr, 0x0010, 0x0020, ""); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0020, 0x000d, ID); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0020, 0x000e, ""); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0008, 0x0018, ""); 
		DDO.Push(vr);
		}
	else
		{
		*p=0;
		SetStringVR(&vr, 0x0010, 0x0020, ID); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0020, 0x000d, p+1); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0020, 0x000e, ""); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0008, 0x0018, ""); 
		DDO.Push(vr);
		}
		
	
	if (!KeepImages)
		OperatorConsole.printf("Deleting study from: %s\n", ID);
	else
		OperatorConsole.printf("Deleting database entries for study: %s\n", ID);

	if (!KeepImages)
		NewDeleteDICOM(&DDO, Thread);
	else
		RemoveFromPACS(&DDO, KeepImages);

	return ( TRUE );
	}

// Delete studies on date (wildcards or range allowed)
BOOL
DeleteStudies(char *date, BOOL KeepImages, int Thread)
	{

	VR		*vr;
	DICOMDataObject	DDO;

	if (date==NULL || *date==0) return FALSE;

	if (!KeepImages)
		OperatorConsole.printf("Deleting studies on date: %s\n", date);
	else
		OperatorConsole.printf("Deleting database entries for studies on date: %s\n", date);

	SetStringVR(&vr, 0x0008, 0x0020, date); 
	DDO.Push(vr);
	SetStringVR(&vr, 0x0010, 0x0020, ""); 
	DDO.Push(vr);
	SetStringVR(&vr, 0x0020, 0x000d, ""); 
	DDO.Push(vr);
	SetStringVR(&vr, 0x0020, 0x000e, ""); 
	DDO.Push(vr);
	SetStringVR(&vr, 0x0008, 0x0018, ""); 
	DDO.Push(vr);
	if (!KeepImages)
		NewDeleteDICOM(&DDO, Thread);
	else
		RemoveFromPACS(&DDO, KeepImages);

	return ( TRUE );
	}

// Delete series
BOOL
DeleteSeries(char *ID, BOOL KeepImages, int Thread)
	{
	VR		*vr;
	DICOMDataObject	DDO;

	if (ID==NULL || *ID==0) return FALSE;

	char *p = strrchr(ID, ':');

	if (p==0)
		{
		SetStringVR(&vr, 0x0010, 0x0020, ""); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0020, 0x000d, ""); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0020, 0x000e, ID); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0008, 0x0018, ""); 
		DDO.Push(vr);
		}
	else
		{
		*p=0;
		SetStringVR(&vr, 0x0010, 0x0020, ID); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0020, 0x000d, ""); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0020, 0x000e, p+1); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0008, 0x0018, ""); 
		DDO.Push(vr);
		}
		
	if (!KeepImages)
		OperatorConsole.printf("Deleting series from: %s\n", ID);
	else
		OperatorConsole.printf("Deleting database entries for series: %s\n", ID);
	
	if (!KeepImages)
		NewDeleteDICOM(&DDO, Thread);
	else
		RemoveFromPACS(&DDO, KeepImages);

	return ( TRUE );
	}

// Delete image
// todo: ***[NewDeleteFromDB] -FAILED: DDO does not contain correct UIDs if passing pat:sop
BOOL
DeleteImage(char *ID, BOOL KeepImages, int Thread)
	{
	VR		*vr;// = new VR(0x0008, 0x0018, (strlen(ID)+1)&0xfe, (void*)ID, (BOOL) FALSE );
	DICOMDataObject	DDO;

	if (ID==NULL || *ID==0) return FALSE;

	char *p = strrchr(ID, ':');

	if (p==0)
		{
		SetStringVR(&vr, 0x0010, 0x0020, ""); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0020, 0x000d, ""); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0020, 0x000e, ""); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0008, 0x0018, ID); 
		DDO.Push(vr);
		}
	else
		{
		*p=0;
		SetStringVR(&vr, 0x0010, 0x0020, ID); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0020, 0x000d, ""); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0020, 0x000e, ""); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0008, 0x0018, p+1); 
		DDO.Push(vr);
		}

	if (!KeepImages)
		OperatorConsole.printf("Deleting image from: %s\n", ID);
	else
		OperatorConsole.printf("Deleting database entry for image: %s\n", ID);

	if (!KeepImages)
		NewDeleteDICOM(&DDO, Thread);
	else
		RemoveFromPACS(&DDO, KeepImages);

	return ( TRUE );
	}

// Compute size of a file
static unsigned int 
DFileSize(char *Path)
	{
	FILE *f;
	unsigned int res;

	f = fopen(Path, "rb");
	if (f==NULL) return 0;

	fseek(f, 0, SEEK_END);
  	res = ftell(f); /* ask position */
	fclose(f);

	return res;
	}

// Test file exists
static BOOL
DFileExists(char *Path)
	{
	FILE *f;

	f = fopen(Path, "rb");
	if (f==NULL) return FALSE;

	fclose(f);
	return TRUE;
	}

static int DcmMove(const char *patid, char* pszSourceAE, char* pszDestinationAE, const char *studyuid, const char *seriesuid, const char *compress, const char *modality, const char *date, 
		    const char *sop, const char *imagetype, const char *seriesdesc, int id, char *script, int Thread);

// Load DICOM object for GUI given with filename or patientID|sopInstanceUID or studyInstanceUID\seriesInstanceUID\sopInstanceUID
DICOMDataObject *LoadForGUI(char *filename, UINT trunc=0)
	{
	char FullFilename[1024], Filename[1024];
	char Device[32];
	char *sop, *pat, *stud, *ser;
	PDU_Service		PDU;
	DICOMDataObject		DDO;
	VR	*vr;
	
	PDU.AttachRTC(&VRType);

	          sop = strrchr(filename, '|');
	if (!sop) sop = strrchr(filename, '?');
	if (!sop) sop = strrchr(filename, '*');
	if (!sop) sop = strrchr(filename, ':');
	if (sop && (sop[1] != '\\'))
		{
		*sop++= 0;
                pat   = filename;

		// vr = new VR(0x0008, 0x0018, (strlen(sop)+1)&0xfe, (void*)sop, FALSE );
		SetStringVR(&vr, 0x0008, 0x0018, sop); 

		if(!GetFileName(vr, Filename, Device, TRUE, pat))
			{
			delete vr;
 			return NULL;	// not in database ?
			}

		if (!FindPhysicalDevice(Device, FullFilename, Filename))
			{
			OperatorConsole.printf("***GUI could not find file:%s\n", Filename);
			delete vr;
			return NULL;
			}
	
		strcat(FullFilename, Filename);

		delete vr;
		if (*TestMode) strcat(FullFilename, TestMode);
		if (trunc) return PDU.LoadDICOMDataObjectTrunc(FullFilename, trunc);
		return PDU.LoadDICOMDataObject(FullFilename);
		}

	sop = NULL;
	ser = strchr(filename, '\\');
	if (ser) sop = strchr(ser+1, '\\');
	if (ser && sop && (sop-ser)>8 && !DFileExists(filename))
		{
		*sop++= 0;
		*ser++= 0;
                stud  = filename;

		SetStringVR(&vr, 0x0008, 0x0018, sop); 
		if(!GetFileName(vr, Filename, Device, TRUE, NULL, stud, ser))
			{
			delete vr;
			if (*VirtualServerFor[0])
 			{ SystemDebug.printf("Attempting to locate %s on virtualservers\n", Filename);
			  DcmMove("", (char *)MYACRNEMA, (char *)MYACRNEMA, stud, ser, "", "", "", sop, "", "", 7, "", 0);
			}

			/*for (int i=0; i<10; i++)
				{
				if (*VirtualServerFor[i])
					{ 
					char *q, tmp[64];
					strcpy(tmp, VirtualServerFor[i]);
					q = strchr(tmp, ',');
					if (q) *q=0;
					if (!DcmMove("", tmp, (char *)MYACRNEMA, stud, ser, "", "", "", sop, "", "", 7, "", 0)) break;
					}
				}
			*/
	
			SetStringVR(&vr, 0x0008, 0x0018, sop); 
			if(!GetFileName(vr, Filename, Device, TRUE, NULL, stud, ser))
 				{
 				SystemDebug.printf("Object %s is still not in database\n", filename);
 				delete vr;
				return NULL;	// still not in database
				}
			}

		if (!FindPhysicalDevice(Device, FullFilename, Filename))
			{
			OperatorConsole.printf("***GUI could not find file:%s\n", Filename);
			delete vr;
			return NULL;
			}
	
		strcat(FullFilename, Filename);

		delete vr;
		if (*TestMode) strcat(FullFilename, TestMode);
		return PDU.LoadDICOMDataObject(FullFilename);
		}

	strcpy(FullFilename, filename);
	return PDU.LoadDICOMDataObject(filename);
	}
	
// external reference from nkiqrsop.cxx
void SaveDICOMDataObject(char *Filename, DICOMDataObject* pDDO);

#ifndef WIN32
#ifndef EnterCriticalSection
#	define EnterCriticalSection(a) pthread_mutex_lock(a)
#endif
#ifndef LeaveCriticalSection
#	define LeaveCriticalSection(a) pthread_mutex_unlock(a)
#endif
#ifndef CRITICAL_SECTION
#	define CRITICAL_SECTION pthread_mutex_t
#endif
#ifndef InitializeCriticalSection
#	define InitializeCriticalSection(a) pthread_mutex_init(a, NULL);
#endif
#define mkdir(a) mkdir(a, 0777)
#endif

#ifndef __SearchDICOMObject
int SearchDICOMObject(DICOMObject *DDO, const char *desc, char *result, Array < DICOMDataObject * > **A = NULL, DICOMDataObject **O = NULL);
#endif

// This class handles temporary storage of a recieved object named with SOP
class	MyBridgeStorage	:	public UnknownStorage
	{
	public:
				// called for each incoming DDO
		UINT16 CheckObject(DICOMCommandObject *DCO, DICOMDataObject *DDO, PDU_Service *PDU) 
			{
			char Filename[1024];
			char Called[20], Calling[20];
			char *Compression;
			int rc, i;

			ImagesReceived++;

			// If the called AE looks like SERVER~j2, then the last 2 characters override IncomingCompression
			memset(Called, 0, 20);
			strcpy(Called, (char *)(((AAssociateAC *)PDU)->CalledApTitle));
		        if (Called[0]>32)
			  while (Called[strlen(Called)-1]==' ') Called[strlen(Called)-1] = 0;

			memset(Calling, 0, 20);
			strcpy(Calling, (char *)(((AAssociateAC *)PDU)->CallingApTitle));
		        if (Calling[0]>32)
  			  while (Calling[strlen(Calling)-1]==' ') Calling[strlen(Calling)-1] = 0;

                        Compression = strchr(Called, '~');
			if (!Compression) Compression = IncomingCompression; else Compression++;
					
			// Recompress (e.g., leave as is or decompress) incoming files
			recompress(&DDO, Compression, "", Compression[0]=='n' || Compression[0]=='N', (ExtendedPDU_Service *)PDU);

			char szTemp[256], szRootSC[256];
			MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile);
			MyGetPrivateProfileString(szRootSC, "TempDir", "", szTemp, 64, ConfigFile);
			if (szTemp[0]==0)
				{
				Filename[0]=0;
				GetPhysicalDevice("MAG0", Filename);
				strcat(Filename, "printer_files");
				}
			else
				strcpy(Filename, szTemp);

			mkdir(Filename);

			i = strlen(Filename);
			Filename[i]   = PATHSEPCHAR;
			Filename[i+1] = 0;
			SearchDICOMObject(DDO, "0008,0018", Filename+i+1);

			SaveDICOMDataObject(Filename, DDO);
			DDO->Reset();

			return 0;
			}
	};

DICOMDataObject *LoadForBridge(char *stsesop, char *ae)
	{
	char *sop, *stud, *ser;
	char Filename[1024];
	DICOMDataObject *p;
	PDU_Service PDU;
	PDU.AttachRTC(&VRType);
	int i;
	
	sop = NULL;
	ser = strchr(stsesop, '\\');
	if (ser) sop = strchr(ser+1, '\\');
	if (ser && sop)
		{
		*sop++= 0;
		*ser++= 0;
                stud  = stsesop;
 		SystemDebug.printf("Attempting to locate %s for bridge from %s\n", stsesop, ae);
		DcmMove("", ae, (char *)MYACRNEMA, stud, ser, "", "", "", sop, "", "", 0x2bad, "", 0);
		char szTemp[256], szRootSC[256];
		MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile);
		MyGetPrivateProfileString(szRootSC, "TempDir", "", szTemp, 64, ConfigFile);
		if (szTemp[0]==0)
			{
			Filename[0]=0;
			GetPhysicalDevice("MAG0", Filename);
			strcat(Filename, "printer_files");
			}
		else
			strcpy(Filename, szTemp);
		
		i = strlen(Filename);
		Filename[i]   = PATHSEPCHAR;
		Filename[i+1] = 0;
		strcat(Filename, sop);
		p=PDU.LoadDICOMDataObject(Filename);
		unlink(Filename);
		return p;
		}
	return NULL;
	}

// Delete image from database, where only filename is given
BOOL
DeleteImageFile(char *filename, BOOL KeepImages)
	{
	DICOMDataObject		*pDDO;
	VR			*vrSOPInstanceUID, *vrPat;
	DICOMDataObject		DDO;
	VR			*pVR;

	pDDO = LoadForGUI(filename);
	if(!pDDO)
		{
		OperatorConsole.printf("***[DeleteImageFile] %s -FAILED: Error on Load\n", filename);
		return ( FALSE );
		}

	if (!pDDO->GetVR(0x0010, 0x0020) || 
            !pDDO->GetVR(0x0020, 0x000d) ||
            !pDDO->GetVR(0x0020, 0x000e) ||
            !pDDO->GetVR(0x0008, 0x0018))
		{
		OperatorConsole.printf("***[DeleteImageFile] %s -FAILED: file does not contain correct UIDs\n", filename);
		delete pDDO;
		return ( FALSE );
		}

	pVR = pDDO->GetVR(0x0008, 0x0018);
	vrSOPInstanceUID = new VR(0x0008, 0x0018, pVR->Length, pVR->Data, (BOOL) FALSE );
	DDO.Push(vrSOPInstanceUID);

	pVR = pDDO->GetVR(0x0010, 0x0020);
	vrPat = new VR(0x0010, 0x0020, pVR->Length, pVR->Data, (BOOL) FALSE );
	DDO.Push(vrPat);

	OperatorConsole.printf("Deleting database entry for image: %s\n", filename);
	RemoveFromPACS(&DDO, KeepImages);

	delete pDDO;

	return ( TRUE );
	}

// Ask image UID
BOOL
GetImageFileUID(char *filename, char *UID)
	{
	DICOMDataObject*	pDDO;
	VR			*vrSOPInstanceUID;

	pDDO = LoadForGUI(filename);
	if(!pDDO)
		{
		OperatorConsole.printf("***[GetImageFileUID] %s -FAILED: Error on Load\n", filename);
		return ( FALSE );
		}

	if (!pDDO->GetVR(0x0010, 0x0020) || 
            !pDDO->GetVR(0x0020, 0x000d) ||
            !pDDO->GetVR(0x0020, 0x000e) ||
            !pDDO->GetVR(0x0008, 0x0018))
		{
		OperatorConsole.printf("***[GetImageFileUID] %s -FAILED: file does not contain correct UIDs\n", filename);
		return ( FALSE );
		}

	vrSOPInstanceUID = pDDO->GetVR(0x0008, 0x0018);
	memset(UID, 0, 255);
	memcpy(UID, vrSOPInstanceUID->Data, vrSOPInstanceUID->Length);

	delete pDDO;

	return ( TRUE );
	}

// Change UID of given DDO
/* Commented out everywhere it was used, why build?
static BOOL 
ChangeUIDinDDO(DICOMDataObject *pDDO, int group, int element, char *name)
	{
	char s[255], NewUID[255];
	VR   *vr;
	int  len;

	vr = pDDO->GetVR(group, element);
	if (!vr) return ( FALSE);

	memset(s, 0, 255);
	memcpy(s, vr->Data, vr->Length);

	if (!ChangeUID(s, name, NewUID))
		{
		OperatorConsole.printf("***[ChangeUIDinDDO] FAILED to change %s\n", name);
		return ( FALSE );
		}

	len = strlen(NewUID); if (len&1) len++;
        vr->ReAlloc(len);
	memcpy(vr->Data, NewUID, len);

	return ( TRUE );
	}
*/
// Change any VR of given DDO
/* Not used anywhere, why build?
static BOOL 
ChangeVRinDDO(DICOMDataObject *pDDO, int group, int element, char *text)
	{
	char s[255];
	VR   *vr;
	int  len;

	vr = pDDO->GetVR(group, element);
	if (!vr) 
		{
		vr = new VR(group, element, 10, TRUE);
		}

	memset(s, 0, 255);
	memcpy(s, vr->Data, vr->Length);

	len = strlen(text); if (len&1) len++;
        vr->ReAlloc(len);
	memcpy(vr->Data, text, len);

	pDDO->ReplaceVR(vr);	// also works in sequences
	delete vr;

	return ( TRUE );
	}
*/


// Modify all UIDs in a DICOM object, except those in the passed string as given; and replace given VR's
// UID|UID|UID|g,e|UID|g,e|g,e=PASSUID|

static BOOL
NewUIDsInDICOMObject(DICOMObject *DO, const char *Exceptions, const char *Reason=NULL, char *Stage=NULL, Database *db=NULL)
	{
	DICOMObject	DO2;
	VR		*vr;
	int		TypeCode;//, i;
	unsigned int	Index;
	char		desc[64], name[20];
	char		*p;
	char 		s[66], NewUID[255];
	int 		len;
	const char	*cp, *Desc;
	Database        DB;

	if (!db)
        { if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		{
		SystemDebug.printf("***Unable to connect to SQL\n");
		return ( FALSE );
		}
	  db = &DB;
	}

	while((vr=DO->Pop()))
		{
		sprintf(name, "%04x,%04x=", vr->Group, vr->Element);
		cp = strstr(Exceptions, name);
		if (cp)
			{
			strncpy(NewUID, cp+10, 65);
			p = strchr(NewUID, '|');
			if (p) *p=0;
			OperatorConsole.printf("[NewUIDsInDICOMObject] setting %04x,%04x\n", vr->Group, vr->Element);
			len = strlen(NewUID); if (len&1) len++;
		        vr->ReAlloc(len);
			memcpy(vr->Data, NewUID, len);
			}
		else
			{
			TypeCode = VRType.RunTimeClass(vr->Group, vr->Element, desc);
			if (Reason) Desc = Reason; else Desc = desc;

    			if (TypeCode=='UI' && strstr(desc, "Class")==NULL && strcmp(desc, "TransferSyntaxUID"))
				{
				memcpy(s, vr->Data, vr->Length);
				s[vr->Length]=0;

				// 20140309: blocked
				// if (strlen(s)==0) GenUID(s);

				strcat(s, ";");
				sprintf(name, "%04x,%04x|", vr->Group, vr->Element);

				if (!strstr(Exceptions, s))
				if (!strstr(Exceptions, name))
					{
					s[strlen(s)-1]=0;
					if (!ChangeUID(s, Desc, NewUID, Stage, db))
						OperatorConsole.printf("***[NewUIDsInDICOMObject] FAILED to change %04x,%04x (%s)\n", vr->Group, vr->Element, desc);
					else
						{
						SystemDebug.printf("[NewUIDsInDICOMObject] changing %04x,%04x (%s)\n", vr->Group, vr->Element, desc);
						len = strlen(NewUID); if (len&1) len++;
					        vr->ReAlloc(len);
						memcpy(vr->Data, NewUID, len);
						}
					}
				}
			}
			
		if ( vr->SQObjectArray )
			{
			Array < DICOMDataObject * > *ADDO =
				(Array<DICOMDataObject*>*) vr->SQObjectArray;
			
			Index = 0;
			while ( Index < ADDO->GetSize() )
				{
				NewUIDsInDICOMObject(ADDO->Get(Index), Exceptions, Reason, Stage, db);
				++Index;
				}
			}
		DO2.Push(vr);
		}
	DO->Reset();
	while((vr=DO2.Pop()))
		{
		DO->Push(vr);
		}
	return ( TRUE );
	}


// restore all UIDs in a DICOM object, except those in the passed string as given; and replace given VR's
// UID|UID|UID|g,e|UID|g,e|g,e=PASSUID|

// From dbsql.cpp

BOOL ChangeUIDBack(char *NewUID, char *OldUID, char *Stage, char *Type, Database *db);
BOOL ChangeUIDTo(char *OldUID, char *Type, char *NewUID, char *Stage, Database *db);
void KodakFixer(DICOMDataObject	*DDOPtr, BOOL tokodak);

static BOOL
OldUIDsInDICOMObject(DICOMObject *DO, const char *Exceptions, char *Stage, Database *db)
	{
	DICOMObject	DO2;
	VR		*vr;
	int		TypeCode;//, i;
	unsigned int	Index;
	char		desc[64], name[20];
	char		*p;
	char 		s[66], NewUID[255];
	int 		len;
	const char	*cp, *Desc;
	Database        DB;

	if (!db)
        { if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		{
		SystemDebug.printf("***Unable to connect to SQL\n");
		return ( FALSE );
		}
	  db = &DB;
	}

	while((vr=DO->Pop()))
		{
		sprintf(name, "%04x,%04x=", vr->Group, vr->Element);
		cp = strstr(Exceptions, name);
		if (cp)
			{
			strncpy(NewUID, cp+10, 65);
			p = strchr(NewUID, '|');
			if (p) *p=0;
			OperatorConsole.printf("[OldUIDsInDICOMObject] setting %04x,%04x\n", vr->Group, vr->Element);
			len = strlen(NewUID); if (len&1) len++;
		        vr->ReAlloc(len);
			memcpy(vr->Data, NewUID, len);
			}
		else
			{
			TypeCode = VRType.RunTimeClass(vr->Group, vr->Element, desc);

    			if (TypeCode=='UI' && strstr(desc, "Class")==NULL && strcmp(desc, "TransferSyntaxUID"))
				{
				memcpy(s, vr->Data, vr->Length);
				s[vr->Length]=0;
				if (strlen(s)==0) GenUID(s);
				strcat(s, ";");
				sprintf(name, "%04x,%04x|", vr->Group, vr->Element);

				if (!strstr(Exceptions, s))
				if (!strstr(Exceptions, name))
					{
					s[strlen(s)-1]=0;
					if (!ChangeUIDBack(s, NewUID, Stage, NULL, db))
						OperatorConsole.printf("***[OldUIDsInDICOMObject] FAILED to change %04x,%04x (%s)\n", vr->Group, vr->Element, desc);
					else
						{
						SystemDebug.printf("[OldUIDsInDICOMObject] restoring %04x,%04x (%s)\n", vr->Group, vr->Element, desc);
						len = strlen(NewUID); if (len&1) len++;
					        vr->ReAlloc(len);
						memcpy(vr->Data, NewUID, len);
						}
					}
				}
			}
			
		if ( vr->SQObjectArray )
			{
			Array < DICOMDataObject * > *ADDO =
				(Array<DICOMDataObject*>*) vr->SQObjectArray;
			
			Index = 0;
			while ( Index < ADDO->GetSize() )
				{
				OldUIDsInDICOMObject(ADDO->Get(Index), Exceptions, Stage, db);
				++Index;
				}
			}
		DO2.Push(vr);
		}
	DO->Reset();
	while((vr=DO2.Pop()))
		{
		DO->Push(vr);
		}
	return ( TRUE );
	}

// Change patient ID of pDDO in memory
static BOOL
ModifyPATIDofDDO(DICOMDataObject *pDDO, char *NewPATID, char *Reason=NULL)
	{
	int			len;
	PDU_Service		PDU;
	DICOMDataObject		DDO;
	Database		DB;
	char			s[255];
	BOOL			ok;
	VR   			*vr;

	PDU.AttachRTC(&VRType);


	if (NewPATID==NULL || *NewPATID==0)
		{
		OperatorConsole.printf("***[ModifyPATIDofDDO] Empty patient ID not allowed\n");
		return ( FALSE );
		}

	if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		{
		OperatorConsole.printf("***[ModifyPATIDofDDO] Error Connecting to SQL\n");
		return ( FALSE );
		}

	// Just to make sure the table exists
	// DB.CreateTable ( "UIDMODS", "MODTime int, OldUID varchar(64), MODType varchar(32), NewUID varchar(64)" );

	// check image in memory
	if(!pDDO)
		{
		OperatorConsole.printf("***[ModifyPATIDofDDO] -FAILED: No image passed\n");
		return ( FALSE );
		}

	if (!pDDO->GetVR(0x0010, 0x0020) || 
            !pDDO->GetVR(0x0020, 0x000d) ||
            !pDDO->GetVR(0x0020, 0x000e) ||
            !pDDO->GetVR(0x0008, 0x0018))
		{
		OperatorConsole.printf("***[ModifyPATIDofDDO] -FAILED: image does not contain correct UIDs\n");
		return ( FALSE );
		}

/*	ok  = ChangeUIDinDDO(pDDO, 0x0020, 0x000d, "StudyUID");
	ok &= ChangeUIDinDDO(pDDO, 0x0020, 0x000e, "SeriesUID");
	ok &= ChangeUIDinDDO(pDDO, 0x0008, 0x0018, "SOPInstanceUID");
*/
	// let off of the transfer syntax
	ok = NewUIDsInDICOMObject(pDDO, "0002,0010|", Reason, NULL, &DB);

	if (!ok)
		{
		OperatorConsole.printf("***[ModifyPATIDofDDO] -FAILED: could not change all UIDs\n");
		return ( FALSE );
		}

	// change patient ID
	vr = pDDO->GetVR(0x0010, 0x0020);
	memset(s, 0, 255);
	memcpy(s, vr->Data, vr->Length);

	len = strlen(NewPATID); if (len&1) len++;
        vr->ReAlloc(len);
	memcpy(vr->Data, NewPATID, len);

	OperatorConsole.printf("Modified patient ID to: %s\n", NewPATID);
	return ( TRUE );
	}

// Merge (SeriesUID or StudyUID {given in type}) of pDDO in memory
BOOL
MergeUIDofDDO(DICOMDataObject *pDDO, const char *type, const char *Reason)
	{
//	int			len;
	PDU_Service		PDU;
	DICOMDataObject		DDO;
	Database		DB;
//	char			s[255];
	BOOL			ok;
//	VR   			*vr;

	PDU.AttachRTC(&VRType);

	if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		{
		OperatorConsole.printf("***[MergeUIDofDDO] Error Connecting to SQL\n");
		return ( FALSE );
		}

	// check image in memory
	if(!pDDO)
		{
		OperatorConsole.printf("***[MergeUIDofDDO] -FAILED: No image passed\n");
		return ( FALSE );
		}

	if (!pDDO->GetVR(0x0010, 0x0020) || 
            !pDDO->GetVR(0x0020, 0x000d) ||
            !pDDO->GetVR(0x0020, 0x000e) ||
            !pDDO->GetVR(0x0008, 0x0018))
		{
		OperatorConsole.printf("***[MergeUIDofDDO -FAILED: image does not contain correct UIDs\n");
		return ( FALSE );
		}

//	ok  = TRUE;
//	if (strcmp(type, "StudyUID")==0) 
//		ok &= ChangeUIDinDDO(pDDO, 0x0020, 0x000d, "StudyUID");
//	ok  = ChangeUIDinDDO(pDDO, 0x0020, 0x000e, "SeriesUID");
//	ok &= ChangeUIDinDDO(pDDO, 0x0008, 0x0018, "SOPInstanceUID");

	if (strcmp(type, "SeriesUID")==0) 
		// let off of the transfer syntax and study UID
		ok = NewUIDsInDICOMObject(pDDO, "0002,0010|0020,000d|", Reason, "(merge)");
	else
		// let off of the transfer syntax
		ok = NewUIDsInDICOMObject(pDDO, "0002,0010|", Reason, "(merge)");

	if (!ok)
		{
		OperatorConsole.printf("***[MergeUIDofDDO] -FAILED: could not change all UIDs\n");
		return ( FALSE );
		}

	return ( TRUE );
	}

// forward references
int
SaveToDisk(Database	*DB, DICOMCommandObject *DCO, DICOMDataObject	*DDOPtr, char 	*Filename, BOOL NoKill, ExtendedPDU_Service *PDU, int Syntax=0, BOOL nopreget=FALSE);

void TestCompress(char *filename, const char *modes, ExtendedPDU_Service *PDU);
void TestForward(char *filename, const char *mode, char *server, ExtendedPDU_Service *PDU);
void TestSyntax(char *filename, int syntax, ExtendedPDU_Service *PDU);
void TestThreadedSave(char *filename);
void ProcessHL7Data(char *data);
static void NewTempFile(char *name, const char *ext);
BOOL AddImageFile(char *filename, char *NewPatid, ExtendedPDU_Service *PDU, Database *PDB=NULL);

static BOOL dgate_IsDirectory(char *TempPath)
{
   struct stat results;
   if (stat(TempPath, &results) == 0)
     {
#ifdef WIN32
     return (results.st_mode & S_IFDIR)!=0;
#else
     return S_ISDIR(results.st_mode);
#endif
     }
   else
     return 0;
}

int CallImportConverterN(DICOMCommandObject *DCO, DICOMDataObject *DDO, int N, char *pszModality, char *pszStationName, char *pszSop, char *patid, ExtendedPDU_Service *PDU, char *Storage, char *Script);
extern "C" void lua_setvar(ExtendedPDU_Service *pdu, char *name, char *value);
BOOL AttachFile(char *filename, char *script, char *rFilename, ExtendedPDU_Service *PDU);

#ifdef WIN32
BOOL LoadAndDeleteDir(char *dir, char *NewPatid, ExtendedPDU_Service *PDU, int Thread, char *script, Database *DB)
	{
	HANDLE		fdHandle;
	WIN32_FIND_DATA	FileData;
	char		TempPath[512];
	int             count=0, sleep=0;
        char            szRootSC[64];
        char            buffer[64];

	strcpy(TempPath, dir);
	strcat(TempPath, "*.*");

	// Traverse first level of subdirectories	

	fdHandle = FindFirstFile(TempPath, &FileData);
	if(fdHandle == INVALID_HANDLE_VALUE)
		return ( FALSE );
        
        if (MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile))
                {
                MyGetPrivateProfileString(szRootSC, "DelayIncomingFileOpen", "0", buffer, 64, ConfigFile);
                sleep = atoi(buffer);
                }

	if (Thread) Progress.printf("Process=%d, Type='loadanddeletedir', Active=1\n", Thread);
	while ( TRUE )
		{
		if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
			if 
				(	
				strcmp(FileData.cFileName, ".") != 0 &&
				strcmp(FileData.cFileName, "..") != 0 &&
				strstr(FileData.cFileName, ".partial") == NULL
		  		)
				{
				strcpy(TempPath, dir);
				strcat(TempPath, FileData.cFileName);
				strcat(TempPath, "\\");
				LoadAndDeleteDir(TempPath, NewPatid, PDU, Thread==0?0:Thread+1, script, DB);
				rmdir(TempPath);
				}
			}
		else
			{
			if (memcmp(FileData.cFileName, "._", 2) == 0)
				{
			  	strcpy(TempPath, dir);
				strcat(TempPath, FileData.cFileName);
				unlink(TempPath);
				}

			else if (strstr(FileData.cFileName, ".partial") == NULL)
				{
			  	strcpy(TempPath, dir);
				strcat(TempPath, FileData.cFileName);
                                
                                if (sleep) Sleep(sleep); // give sending program time to write
                                
				FILE *f = fopen(TempPath, "at");
				if (f)
					{
					fclose(f);
					if (script)
						{
						char dum[1024];
						if (!AttachFile(TempPath, script, dum, PDU))
							for (int i=0; i<3; i++)
								{ 
								Sleep(5000);
								if (AttachFile(TempPath, script, dum, PDU)) 
									break;
								}
						}
					else if (!AddImageFile (TempPath, NewPatid, PDU, DB))
						{
						BOOL success = FALSE;
						for (int i=0; i<3; i++)
							{ 
							Sleep(5000);
							if (AddImageFile (TempPath, NewPatid, PDU, DB)) 
								{ 
								success = TRUE;
								break;
								}
							}
						if (!success)
							{
							DICOMDataObject	DDO;
							lua_setvar(PDU, "Filename", TempPath);
							int rc = CallImportConverterN(NULL, &DDO, 2100, NULL, NULL, NULL, NULL, PDU, NULL, NULL);
							}
						}
					unlink(TempPath);
					if (Thread) Progress.printf("Process=%d, Total=%d, Current=%d\n", Thread, 100, count++);
					}
				}
			}
			
		if(!FindNextFile(fdHandle, &FileData))
			break;
		}

		FindClose(fdHandle);

	if (Thread) Progress.printf("Process=%d, Active=0\n", Thread);
	return TRUE;
	}

#else
BOOL LoadAndDeleteDir(char *dir, char *NewPatid, ExtendedPDU_Service *PDU, int Thread, char *script, Database *DB)
      {
      DIR             *dird;
      dirent          *diren;

      char            TempPath[PATH_MAX];
      char            *n;

      strcpy(TempPath, dir);
      TempPath[strlen(TempPath)-1]=0;

      // Traverse first level of subdirectories       

      dird = opendir(TempPath);
      if(dird == NULL)
            return ( FALSE );

      while ( TRUE )
            {
            diren = readdir(dird);
            if (diren)
                  {
                  n = diren->d_name;
                  strcpy(TempPath, dir);
                  strcat(TempPath, n);
                  
                  if (dgate_IsDirectory(TempPath))
                        {
                        if
                        (       
                        strcmp (n, "."            ) != 0 &&
                        strcmp (n, ".."           ) != 0 &&
                        strstr (n, ".partial"     ) == NULL &&
			memcmp (n, "._", 2        ) != 0
#ifdef DARWIN //A Macintosh OS X file.
                        && stricmp(n, ".DS_Store"    ) != 0
#endif //DARWIN
                        )
                              {
                              long length=strlen(TempPath);
                              TempPath[length] = PATHSEPCHAR;
                              TempPath[length+1] = '\0';
                              LoadAndDeleteDir(TempPath, NewPatid, PDU, Thread==0?0:Thread+1, script, DB);
                              rmdir(TempPath);
                              }
                        }
                  else
                        {
                        if (memcmp(n, "._", 2) == 0)
				{
				unlink(TempPath);
				}
			
			else if (strstr (n, ".partial"     ) == NULL)
				{
				if (script)
					{
					char dum[1024];
					AttachFile(TempPath, script, dum, PDU);
					}
				else if (!AddImageFile (TempPath, NewPatid, PDU, DB))
                                    { 
			            BOOL success = FALSE;
                                    for (int i=0; i<30; i++)
                                          { 
				          Sleep(5000);
                                          if (AddImageFile (TempPath, NewPatid, PDU, DB)) 
                                              { success = TRUE;
                                                break;
                                              }
                                          }
                                    
                                    if (!success)
                                          {
                                          DICOMDataObject DDO;
                                          lua_setvar(PDU, "Filename", TempPath);
                                          int rc = CallImportConverterN(NULL, &DDO, 2100, NULL, NULL, NULL, NULL, PDU, NULL, NULL);
                                          }
                                    }
                              unlink(TempPath);
                              }
                        }
                  }
            else
                  break;
            }

            closedir(dird);

      return TRUE;
      }
#endif

BOOL BackgroundExec(char *ProcessBinary, char *Args);

// Add image file to server (also copies file!); optional changes patient ID before entering file
BOOL
AddImageFile(char *filename, char *NewPatid, ExtendedPDU_Service *PDU, Database	*PDB)
	{
	DICOMDataObject*	pDDO;
	int			i;//, len;
//	VR			*vrSOPInstanceUID;
	DICOMDataObject		DDO;
	Database		DB, *pDB;
	char			rFilename[1024];
	BOOL			rc;
	char			szRootSC[64], szTemp[64];
        
        pDB = &DB;
        if (PDB) 
          pDB=PDB;
	else if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		{
		OperatorConsole.printf("***Error Connecting to SQL\n");
		return ( FALSE );
		}

	MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile);
  	MyGetPrivateProfileString(szRootSC, "ImportExportDragAndDrop", "1", szTemp, 64, ConfigFile);

	if (NewPatid)
		{
		if (NewPatid[0]=='$' && NewPatid[1]=='c')
			{
#ifdef HAVE_J2K
			TestCompress(filename, "unasn1n2n3n4j1j2j3j4j5j6jkjlk1k2k4k8", PDU);
#else
			TestCompress(filename, "unasn1n2n3n4j1j2j3j4j5j6k1k2k4k8", PDU);
#endif
			return TRUE;
			}
		if (NewPatid[0]=='$' && NewPatid[1]=='t')
			{
			TestForward(filename, "un", NewPatid+2, PDU);
			TestForward(filename, "as", NewPatid+2, PDU);
			TestForward(filename, "n1", NewPatid+2, PDU);
			TestForward(filename, "n2", NewPatid+2, PDU);
			TestForward(filename, "n3", NewPatid+2, PDU);
			TestForward(filename, "n4", NewPatid+2, PDU);
			TestForward(filename, "j1", NewPatid+2, PDU);
			TestForward(filename, "j2", NewPatid+2, PDU);
			TestForward(filename, "j3", NewPatid+2, PDU);
			TestForward(filename, "j4", NewPatid+2, PDU);
			TestForward(filename, "j5", NewPatid+2, PDU);
			TestForward(filename, "j6", NewPatid+2, PDU);
#ifdef HAVE_J2K
			TestForward(filename, "jk", NewPatid+2, PDU);
			TestForward(filename, "jl", NewPatid+2, PDU);
#endif
			TestForward(filename, "k1", NewPatid+2, PDU);
			TestForward(filename, "k2", NewPatid+2, PDU);
			TestForward(filename, "k4", NewPatid+2, PDU);
			TestForward(filename, "k8", NewPatid+2, PDU);
			return TRUE;
			}
		if (NewPatid[0]=='$' && NewPatid[1]=='s')
			{
			TestSyntax(filename, 1, PDU);
			TestSyntax(filename, 2, PDU);
			TestSyntax(filename, 3, PDU);
			TestSyntax(filename, 4, PDU);
			TestSyntax(filename, 5, PDU);
			TestSyntax(filename, 6, PDU);
			TestSyntax(filename, 7, PDU);
			TestSyntax(filename, 8, PDU);
			TestSyntax(filename, 9, PDU);
			TestSyntax(filename, 10, PDU);
			return TRUE;
			}
		if (NewPatid[0]=='$' && NewPatid[1]=='r')
			{
			for (i=0; i<atoi(NewPatid+2); i++)
				TestSyntax(filename, 0, PDU);
			return TRUE;
			}
		if (NewPatid[0]=='$' && NewPatid[1]=='t')
			{
			for (i=0; i<atoi(NewPatid+2); i++)
				TestThreadedSave(filename);
			return TRUE;
			}
		}

	// rudimentary hl7 support
	if (strlen(filename)>4 && stricmp(filename+strlen(filename)-4, ".hl7")==0)
		{
		unsigned int len = DFileSize(filename);
		if (len)
			{
			char *p=(char*) malloc(len+1);
			FILE *f;
			f = fopen(filename, "rb");
			fread(p, 1, len, f);
			p[len]=0;
      	               	fclose(f); 
			ProcessHL7Data(p);
			free(p);
			OperatorConsole.printf("[AddImageFile] loaded HL7 file: %s\n", filename);
			return TRUE;
			}
		else
			{
			OperatorConsole.printf("*** [AddImageFile] could not open HL7 file: %s\n", filename);
			return FALSE;
			}
		}

	char *p = strrchr(filename, '.');

	// compressed file support using 7za.exe
	if (p && strstr(".gz.GZ.zip.ZIP.tar.TAR.7z.7Z", p))
		{ 
		char line[1000], dir[512];

		NewTempFile(dir, "");
		mkdir(dir);

#ifdef WIN32
		sprintf(line, "-y -o\"%s\" x \"%s\"", dir, filename);
  		BackgroundExec("7za.exe", line);
		strcat(dir, "\\");
#else
		sprintf(line, "7za -y -o\"%s\" x \"%s\"", dir, filename);
		system(line);
		strcat(dir, "/");
#endif
		LoadAndDeleteDir(dir, NewPatid, PDU, 0, NULL, pDB);
		rmdir(dir);

		return TRUE;
		}


	pDDO = LoadForGUI(filename);
	if(!pDDO)
		{
		OperatorConsole.printf("***[AddImageFile] %s -FAILED: Error on Load\n", filename);
		return ( FALSE );
		}
		
        lua_setvar(PDU, "Filename", filename);

	if (NewPatid)
		{
		if (!ModifyPATIDofDDO(pDDO, NewPatid))
			{
			OperatorConsole.printf("***[AddImageFile] Error changing patient ID for file: %s\n", filename);
			delete pDDO;
			return FALSE;
			}
		}

	if (!pDDO->GetVR(0x0010, 0x0020) || 
            !pDDO->GetVR(0x0020, 0x000d) ||
            !pDDO->GetVR(0x0020, 0x000e) ||
            !pDDO->GetVR(0x0008, 0x0018))
		{
		OperatorConsole.printf("***[AddImageFile] %s -FAILED: file does not contain correct UIDs\n", filename);
		delete pDDO;
		return ( FALSE );
		}

        // recompress dropped files
	rc = recompress(&pDDO, DroppedFileCompression, "", DroppedFileCompression[0]=='n' || DroppedFileCompression[0]=='N', PDU);
        rc = TRUE;	// failed compression leaves original object

	// if ((rc == FALSE) || (!SaveToDisk(DB, pDDO, rFilename, TRUE, (unsigned char *)"dropped", (unsigned char *)"dropped", 0, !atoi(szTemp))))
	if ((rc == FALSE) || (!SaveToDisk(pDB, NULL, pDDO, rFilename, TRUE, PDU, 0, !atoi(szTemp))))
		{
		OperatorConsole.printf("***[AddImageFile] Error entering object into server: %s\n", filename);
//		if (pDDO)
//			delete pDDO;
		return FALSE;
		}

	OperatorConsole.printf("Added file: %s\n", rFilename);
	return ( TRUE );
	}


// Make a new UID for external use (prefix configurable via UIDPrefix in dicom.ini)
extern int UIDPostfix;
static char UIDPrefix[65] = "1.2.826.0.1.3680043.2.135.1066.6";		// default value
static BOOL dgLoadRoot = TRUE;
BOOL
GenUID(char	*oString)
	{ 
	if (dgLoadRoot)
		{
		char szRootSC[64];
  		if (MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile))
			{
			if (MyGetPrivateProfileString(szRootSC, "UIDPrefix", "1.2.826.0.1.3680043.2.135.1066", UIDPrefix, 64, ConfigFile))
				strcat(UIDPrefix, ".6");
			}
		dgLoadRoot = FALSE;
		}

#ifdef WIN32
	sprintf(oString, "%s.%u.%d.%u", UIDPrefix, (unsigned int)time(NULL), GetTickCount()%1000, (UIDPostfix++)%100);
#else
	sprintf(oString, "%s.%u.%u", UIDPrefix, (unsigned int)time(NULL), (UIDPostfix++)%10000);
#endif
	return ( TRUE );
	}


// Change patient ID of image file (must already be loaded in the database); or similar run script: if script is number use defined scripts
BOOL
ModifyPATIDofImageFile(char *filename, char *NewPATID, BOOL DelFile, char *script, ExtendedPDU_Service *PDU)
	{
	DICOMDataObject*	pDDO;
	int			sIndex, len;
	unsigned int		Index;
	DICOMDataObject		DDO;
	Database		DB;
	char			rFilename[1024], s[255];
	char			MirrorFile[1024], Device[255], Physical[1024];
//	BOOL			ok;
//	VR			*vr;
	int			devlen, mirrordevlen, rc;

#ifdef __GNUC__ //Warnings
	devlen = 0;
	mirrordevlen = 0;
#endif

	if (NewPATID!=NULL && *NewPATID==0)
		{
		OperatorConsole.printf("***[ModifyPATIDofImageFile] Empty patient ID not allowed\n");
		return ( FALSE );
		}

	// try to locate the mirror file because that one has to change too
	MirrorFile[0] = 0;
	Index = 0;
	while ( Index < MAGDevices )
		{
		sprintf(Device, "MAG%d", Index);
		GetPhysicalDevice(Device, Physical);
		devlen = strlen(Physical);
		if (memicmp(Physical, filename, strlen(Physical))==0)
			{
			sprintf(Device, "MIRROR%d", Index);
			if (GetPhysicalDevice(Device, MirrorFile))
				{
				mirrordevlen = strlen(MirrorFile);
				strcat(MirrorFile, filename + strlen(Physical));
				}
			break;
			}
		++Index;

		if (Index == MAGDevices)
			{
			OperatorConsole.printf("***File to modify does not exist or is not on a MAG device\n");
			return ( FALSE );
			}
		}

	if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		{
		OperatorConsole.printf("***Error Connecting to SQL\n");
		return ( FALSE );
		}

	// Just to make sure the table exists
	// DB.CreateTable ( "UIDMODS", "MODTime int, OldUID varchar(64), MODType varchar(32), NewUID varchar(64)" );

	// load image into memory
	pDDO = LoadForGUI(filename);
	if(!pDDO)
		{
		OperatorConsole.printf("***[ModifyPATIDofImageFile] %s -FAILED: Error on Load\n", filename);
		return ( FALSE );
		}

	// no script: change patient ID of object file in memory - changes UIDs
	if (NewPATID && script==NULL) 
	  if (!ModifyPATIDofDDO(pDDO, NewPATID))
		{
		delete pDDO;
		return FALSE;
		}

	// assume script changes UIDs; leave modifying patientID to script
	if (script)
		{
		if (atoi(script)>1000)
		  rc = CallImportConverterN(NULL, pDDO, atoi(script), NULL, NULL, NULL, NULL, PDU, NULL, NULL);
		else
		  rc = CallImportConverterN(NULL, pDDO, -1,           NULL, NULL, NULL, NULL, PDU, NULL, script);
		  
		if(rc==2 || rc==6)
			{
			OperatorConsole.printf("[ModifyPATIDofImageFile] %s rejected by script\n", filename);
			return ( FALSE );
			}

		/*if (NewPATID)
			{
			// change patient ID
			VR *vr = pDDO->GetVR(0x0010, 0x0020);
			memset(s, 0, 255);
			memcpy(s, vr->Data, vr->Length);

			len = strlen(NewPATID); if (len&1) len++;
			vr->ReAlloc(len);
			memcpy(vr->Data, NewPATID, len);
			}
		*/
		}

	// remove image file from database
	if (DelFile)
		if (!DeleteImageFile(filename, FALSE)) // 20170528
			{
			OperatorConsole.printf("***Could not remove image file from database\n");
			return ( FALSE );
			}

	// add the image in memory to the server, also makes a copy of the image
	// if (!SaveToDisk(DB, pDDO, rFilename, TRUE, (unsigned char *)"modpatid", (unsigned char *)"modpatid", 0, 1))
	if (!SaveToDisk(&DB, NULL, pDDO, rFilename, TRUE, PDU, 0, 1))
		{
		//delete pDDO;
		OperatorConsole.printf("***Error entering object into server: %s\n", filename);
		return ( FALSE );
		}
	OperatorConsole.printf("Modified image: %s\n", rFilename);

	if (!DelFile) 
		{
		return ( TRUE );
		}

	// delete the original image since it would show up with original PATID after regen
	if (stricmp(filename, rFilename))
		{
		unlink(filename);
		OperatorConsole.printf("Deleting file: %s\n", filename);
		}

	// delete the mirror image since it would show up with original PATID after regen
	if (strlen(MirrorFile))
		if (stricmp(MirrorFile, rFilename))
			{
			unlink(MirrorFile);
			OperatorConsole.printf("Deleting mirror file: %s\n", MirrorFile);
			}


	// remove the source directory and any required subdirectories (just fails quietly if directory not yet empty)
	strcpy(s, filename);
	for (sIndex = strlen(s)-1; sIndex>=devlen; sIndex--)
		if (s[sIndex]==PATHSEPCHAR)
		{
		s[sIndex]='\0';
		rmdir(s);
		}

	// remove the mirror source directory and any required subdirectories (just fails quietly if directory not yet empty)
	if (strlen(MirrorFile))
		{
		strcpy(s, MirrorFile);
		for (sIndex = strlen(s)-1; sIndex>=mirrordevlen; sIndex--)
			if (s[sIndex]==PATHSEPCHAR)
			{
			s[sIndex]='\0';
			rmdir(s);
			}
		}

	return ( TRUE );
	}

// Change image file through a script (warning: do NOT change patient ID or uids without newuids command)
// works on images in the database
BOOL
ModifyImageFile(char *filename, char *script, ExtendedPDU_Service *PDU, BOOL DeleteFile)
	{
//	DICOMDataObject*	pDDO;
	Database		DB;
//	char			rFilename[1024];

	if (script==NULL) return FALSE;
        if (script[0]==0) return FALSE;

	return ModifyPATIDofImageFile(filename, NULL, DeleteFile, script, PDU);
	}

BOOL	ImageFileLister(const char *server, char *pat, char *study, char *series, char *sop, char *fmt, FILE *f);

// modify all data of selected patient, study or series
int	ModifyData(char *pat, char *study, char *series, char *sop, char *script, ExtendedPDU_Service *PDU, BOOL DeleteFile, int Thread)
	{	
	char tempfile[512], temp[512];
	FILE *f;
	int count=0, current=0;

	if (Thread) Progress.printf("Process=%d, Type='modifydata', Active=1\n", Thread);
	NewTempFile(tempfile, ".txt");

        f = fopen(tempfile, "wt");
        char formt[]="%s\n";
        ImageFileLister("local", pat, study, series, sop, formt, f);
	fclose(f);

	f = fopen(tempfile, "rt");
        while(fgets(temp, sizeof(temp), f) != NULL) count++;
	fclose(f);

	f = fopen(tempfile, "rt");
        while(fgets(temp, sizeof(temp), f) != NULL)
  	{ if (temp[strlen(temp)-1]=='\n') temp[strlen(temp)-1]=0;
	  ModifyImageFile(temp, script, PDU, DeleteFile);
  	  if (Thread) Progress.printf("Process=%d, Total=%d, Current=%d\n", Thread, count, current++);
	}
	fclose(f);
	unlink(tempfile);
  	if (Thread) Progress.printf("Process=%d, Active=0\n", Thread);

	return TRUE;
	}

BOOL BMPStrip(DICOMDataObject** pDDO, char *filename, int size, int append, int level, int window, unsigned int frame, int cols, float gamma);
BOOL ToJPGStrip(DICOMDataObject** pDDO, char *filename, int size, int append, int level, int window, unsigned int frame, int quality, int cols, float gamma);

// Export strip of bitmaps for selected series
int	ImageStripforSeries(char *study, char *series, char *filename, int size, int append, int level, int window, int mode, int cols)
	{	
	char tempfile[512], temp[512];
	FILE *f;
	int count=0, current=0, i;
	DICOMDataObject **pDDO;

	NewTempFile(tempfile, ".txt");

        f = fopen(tempfile, "wt");
        char formt[]="%s\n";
        ImageFileLister("local", "", study, series, "", formt, f);
	fclose(f);

	f = fopen(tempfile, "rt");
        while(fgets(temp, sizeof(temp), f) != NULL) count++;
	fclose(f);
	
	pDDO = (DICOMDataObject**)malloc((count+1) * sizeof(DICOMDataObject*));
	pDDO[count]=NULL;
	
	i = 0;
	f = fopen(tempfile, "rt");
        while(fgets(temp, sizeof(temp), f) != NULL)
  	{ if (temp[strlen(temp)-1]=='\n') temp[strlen(temp)-1]=0;
          pDDO[i++] = LoadForGUI(temp);
	}
	fclose(f);
	unlink(tempfile);
	if (mode==0)
          BMPStrip(pDDO, filename, 8192, append, level, window, 0, cols, 1);
        else
	  ToJPGStrip(pDDO, filename, 8192, append, level, window, 0, 95, cols, 1);
	for (i=0; i<count; i++)	delete pDDO[i];
	return TRUE;
	}

// The attach functions load a (copy) the input (plan/any) data into the dicom server
BOOL AttachFile(char *filename, char *script, char *rFilename, ExtendedPDU_Service *PDU)
        {
	DICOMDataObject*	pDDO;
	Database		DB;
	int			rc;
	char			*p;

	if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		{
		OperatorConsole.printf("***Error Connecting to SQL\n");
		return ( FALSE );
		}

	// compressed file support using 7za.exe
	p = strrchr(filename, '.');
	if (p && strstr(".gz.GZ.zip.ZIP.tar.TAR.7z.7Z", p))
		{ 
		char line[1000], dir[512];

		NewTempFile(dir, "");
		mkdir(dir);

#ifdef WIN32
		sprintf(line, "-y -o\"%s\" x \"%s\"", dir, filename);
  		BackgroundExec("7za.exe", line);
		strcat(dir, "\\");
#else
		sprintf(line, "7za -y -o\"%s\" x \"%s\"", dir, filename);
		system(line);
		strcat(dir, "/");
#endif
		LoadAndDeleteDir(dir, NULL, PDU, 0, script, &DB);
		rmdir(dir);

		return TRUE;
		}

	// load image into memory
	pDDO = LoadForGUI(filename);
	if(!pDDO)
		{
		OperatorConsole.printf("***[AttachFile] %s -FAILED: Error on Load (input) \n", filename);
		return ( FALSE );
		}

	rc = CallImportConverterN(NULL, pDDO, -1, NULL, NULL, NULL, NULL, PDU, NULL, script);

	if(rc==2 || rc==6)
		{
		delete pDDO;
		OperatorConsole.printf("[AttachFile] %s rejected by script\n", filename);
		return ( FALSE );
		}

	//if (!SaveToDisk(DB, pDDO, rFilename, TRUE, (unsigned char *)"AttachFile", (unsigned char *)"AttachFile", 0, 1))
	if (!SaveToDisk(&DB, NULL, pDDO, rFilename, TRUE, PDU, 0, 1))
		{
		//delete pDDO;
		OperatorConsole.printf("***[AttachFile] Error entering object into server: %s\n", filename);
		return ( FALSE );
		}
	OperatorConsole.printf("Attached image: %s\n", rFilename);

	return ( TRUE );
	}

BOOL
AttachRTPLANToRTSTRUCT(char *planfilename, char *structfilename, ExtendedPDU_Service *PDU)
	{
	DICOMDataObject*	pDDO2;
	char			rFilename[1024];
	char			Sop[66], Study[66], Pat[66], Name[66], Sex[66], Birth[66], script[1024];

	// load image into memory
	pDDO2 = LoadForGUI(structfilename);
	if(!pDDO2)
		{
		OperatorConsole.printf("***[AttachRTPLANToRTSTRUCT] %s -FAILED: Error on Load (struct)\n", structfilename);
		return ( FALSE );
		}

        SearchDICOMObject(pDDO2, "0010,0010", Name);
        SearchDICOMObject(pDDO2, "0010,0020", Pat);
        SearchDICOMObject(pDDO2, "0010,0030", Birth);
        SearchDICOMObject(pDDO2, "0010,0040", Sex);
        SearchDICOMObject(pDDO2, "0020,000d", Study);
        SearchDICOMObject(pDDO2, "0008,0018", Sop);

        sprintf(script, "newuids except 0020,000d; set 0010,0010 to \"%s\"; set 0010,0020 to \"%s\"; set 0010,0030 to \"%s\"; set 0010,0040 to \"%s\"; "
                        "set 0020,000d to \"%s\"; set 0008,1110/0008,1155 to \"%s\"; "
                        "set 300c,0060/0008,1155 to \"%s\"",
          Name, Pat, Birth, Sex, Study, Study, Sop);

	AttachFile(planfilename, script, rFilename, PDU);
	OperatorConsole.printf("Attached rtplan to rtstruct: %s\n", rFilename);
	delete pDDO2;

	return ( TRUE );
	}

BOOL
AttachAnyToPatient(char *anyfilename, char *samplefilename, ExtendedPDU_Service *PDU)
	{
	DICOMDataObject*	pDDO2;
	char			rFilename[1024], Pat[66], Name[66], Birth[66], Sex[66], script[1024];//, Study[66], Sop[66];

	// load image into memory
	pDDO2 = LoadForGUI(samplefilename);
	if(!pDDO2)
		{
		OperatorConsole.printf("***[AttachAnyToPatient] %s -FAILED: Error on Load (sample)\n", samplefilename);
		return ( FALSE );
		}

        SearchDICOMObject(pDDO2, "0010,0010", Name);
        SearchDICOMObject(pDDO2, "0010,0020", Pat);
        SearchDICOMObject(pDDO2, "0010,0030", Birth);
        SearchDICOMObject(pDDO2, "0010,0040", Sex);

        sprintf(script, "newuids; set 0010,0010 to \"%s\"; set 0010,0020 to \"%s\"; set 0010,0030 to \"%s\"; set 0010,0040 to \"%s\"",
          Name, Pat, Birth, Sex);

	AttachFile(anyfilename, script, rFilename, PDU);
	OperatorConsole.printf("Attached image to patient: %s\n", rFilename);
        delete pDDO2;

	return ( TRUE );
	}

BOOL
AttachAnyToStudy(char *anyfilename, char *samplefilename, ExtendedPDU_Service *PDU)
	{
	DICOMDataObject*	pDDO2;
	char			rFilename[1024], Study[66], Pat[66], Name[66], Birth[66], Sex[66], script[1024];//, Sop[66];

	// load image into memory
	pDDO2 = LoadForGUI(samplefilename);
	if(!pDDO2)
		{
		OperatorConsole.printf("***[AttachAnyToStudy] %s -FAILED: Error on Load (sample)\n", samplefilename);
		return ( FALSE );
		}

        SearchDICOMObject(pDDO2, "0010,0010", Name);
        SearchDICOMObject(pDDO2, "0010,0020", Pat);
        SearchDICOMObject(pDDO2, "0010,0030", Birth);
        SearchDICOMObject(pDDO2, "0010,0040", Sex);
        SearchDICOMObject(pDDO2, "0020,000d", Study);

        sprintf(script, "newuids except 0020,000d; set 0010,0010 to \"%s\"; set 0010,0020 to \"%s\"; set 0010,0030 to \"%s\"; set 0010,0040 to \"%s\"; "
	                "set 0020,000d to \"%s\"",
          Name, Pat, Birth, Sex, Study);

	AttachFile(anyfilename, script, rFilename, PDU);
	OperatorConsole.printf("Attached image to study: %s\n", rFilename);
        delete pDDO2;

	return ( TRUE );
	}

BOOL
AttachAnyToSeries(char *anyfilename, char *samplefilename, ExtendedPDU_Service *PDU)
	{
	DICOMDataObject*	pDDO2;
	char			rFilename[1024], Study[66], Series[66], Pat[66], Name[66], Birth[66], Sex[66], script[1024];//, Sop[66];

        // load image into memory
	pDDO2 = LoadForGUI(samplefilename);
	if(!pDDO2)
		{
		OperatorConsole.printf("***[AttachAnyToSeries] %s -FAILED: Error on Load (sample)\n", samplefilename);
		return ( FALSE );
		}

        SearchDICOMObject(pDDO2, "0010,0010", Name);
        SearchDICOMObject(pDDO2, "0010,0020", Pat);
        SearchDICOMObject(pDDO2, "0010,0030", Birth);
        SearchDICOMObject(pDDO2, "0010,0040", Sex);
        SearchDICOMObject(pDDO2, "0020,000d", Study);
        SearchDICOMObject(pDDO2, "0020,000e", Series);

        sprintf(script, "newuids except 0020,000d|0020,000e; set 0010,0010 to \"%s\"; set 0010,0020 to \"%s\"; set 0010,0030 to \"%s\"; set 0010,0040 to \"%s\"; "
	                "set 0020,000d to \"%s\"; set 0020,000e to \"%s\"",
          Name, Pat, Birth, Sex, Study, Series);

	AttachFile(anyfilename, script, rFilename, PDU);
	OperatorConsole.printf("Attached image to series: %s\n", rFilename);
        delete pDDO2;

	return ( TRUE );
	}

// Merge SeriesUID or StudyUID (passed in type) of image file
static BOOL
MergeUIDofImageFile(char *filename, BOOL DelFile, const char *type, char *script, char *Reason, ExtendedPDU_Service *PDU)
	{
	DICOMDataObject*	pDDO;
	int			sIndex;// len;
	unsigned int		Index;
	DICOMDataObject		DDO;
	Database		DB;
	char			rFilename[1024], s[255];
	char			MirrorFile[1024], Device[255], Physical[1024];
//	BOOL			ok;
//	VR			*vr;
	int			devlen, mirrordevlen, rc;

#ifdef __GNUC__ //Warnings
	devlen = 0;
	mirrordevlen = 0;
#endif

	// try to locate the mirror file because that one has to change too
	MirrorFile[0] = 0;
	Index = 0;
	while ( Index < MAGDevices )
		{
		sprintf(Device, "MAG%d", Index);
		GetPhysicalDevice(Device, Physical);
		devlen = strlen(Physical);
		if (memicmp(Physical, filename, strlen(Physical))==0)
			{
			sprintf(Device, "MIRROR%d", Index);
			if (GetPhysicalDevice(Device, MirrorFile))
				{
				mirrordevlen = strlen(MirrorFile);
				strcat(MirrorFile, filename + strlen(Physical));
				}
			break;
			}
		++Index;

		if (Index == MAGDevices)
			{
			OperatorConsole.printf("***File to merge does not exist or is not on a MAG device\n");
			return ( FALSE );
			}
		}

	if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
		{
		OperatorConsole.printf("***Error Connecting to SQL\n");
		return ( FALSE );
		}

	// load image into memory
	pDDO = LoadForGUI(filename);
	if(!pDDO)
		{
		OperatorConsole.printf("***[MergeUIDofImageFile] %s -FAILED: Error on Load\n", filename);
		return ( FALSE );
		}

	// change patient ID of object file in memory
	if (!MergeUIDofDDO(pDDO, type, Reason))
		{
		OperatorConsole.printf("***[MergeUIDofImageFile] %s -FAILED: merging UID\n", filename);
		delete pDDO;
		return FALSE;
		}

	if (strcmp(type, "SeriesUID")==0)
	  	//rc=CallImportConverterN(NULL, pDDO, 1700, NULL, NULL, NULL, NULL, "merging", "merging", NULL, NULL, VariableVRs);
	  	rc=CallImportConverterN(NULL, pDDO, 1700, NULL, NULL, NULL, NULL, PDU, NULL, NULL);
	else
  		//rc=CallImportConverterN(NULL, pDDO, 1800, NULL, NULL, NULL, NULL, "merging", "merging", NULL, NULL, VariableVRs);
  		rc=CallImportConverterN(NULL, pDDO, 1800, NULL, NULL, NULL, NULL, PDU, NULL, NULL);

	if (rc==2 || rc==6)
		{
		OperatorConsole.printf("[MergeUIDofImageFile] %s script rejected merge\n", filename);
		delete pDDO;
		return FALSE;
		}

	if (*script)
	  	//rc=CallImportConverterN(NULL, pDDO, -1, NULL, NULL, NULL, NULL, "merging", "merging", NULL, script, VariableVRs);
	  	rc=CallImportConverterN(NULL, pDDO, -1, NULL, NULL, NULL, NULL, PDU, NULL, script);

	if (rc==2 || rc==6)
		{
		OperatorConsole.printf("[MergeUIDofImageFile] %s script rejected merge\n", filename);
		delete pDDO;
		return FALSE;
		}

	// remove image file from database
	if (DelFile)
		if (!DeleteImageFile(filename, FALSE)) // 20170528
			{
			OperatorConsole.printf("***Could not remove image file from database\n");
			return ( FALSE );
			}

	// add the image in memory to the server, also makes a copy of the image
	//if (!SaveToDisk(DB, pDDO, rFilename, TRUE, (unsigned char *)"merging", (unsigned char *)"merging", 0, 1))
	if (!SaveToDisk(&DB, NULL, pDDO, rFilename, TRUE, PDU, 0, 1))
		{
		//delete pDDO;
		OperatorConsole.printf("***Error entering object into server: %s\n", filename);
		return ( FALSE );
		}
	OperatorConsole.printf("Merged image: %s\n", rFilename);

	if (!DelFile) 
		return ( TRUE );

	// delete the original image since it would show up with original series UID after regen
	if (stricmp(filename, rFilename))
		{
		unlink(filename);
		OperatorConsole.printf("Deleting file: %s\n", filename);
		}

	// delete the mirror image since it would show up with original series UID after regen
	if (strlen(MirrorFile))
		if (stricmp(MirrorFile, rFilename))
			{
			unlink(MirrorFile);
			OperatorConsole.printf("Deleting mirror file: %s\n", MirrorFile);
			}


	// remove the source directory and any required subdirectories (just fails quietly if directory not yet empty)
	strcpy(s, filename);
	for (sIndex = strlen(s)-1; sIndex>=devlen; sIndex--)
		if (s[sIndex]==PATHSEPCHAR)
		{
		s[sIndex]='\0';
		rmdir(s);
		}

	// remove the mirror source directory and any required subdirectories (just fails quietly if directory not yet empty)
	if (strlen(MirrorFile))
		{
		strcpy(s, MirrorFile);
		for (sIndex = strlen(s)-1; sIndex>=mirrordevlen; sIndex--)
			if (s[sIndex]==PATHSEPCHAR)
			{
			s[sIndex]='\0';
			rmdir(s);
			}
		}

	return ( TRUE );
	}

// Help routine for spawning a process on WIN32 or UNIX

#ifndef UNIX	// Win32...

BOOL
BackgroundExec(
	char	*ProcessBinary,				// name of executable
	char	*Args)					// arguments
	{
	PROCESS_INFORMATION	PINFO;
	STARTUPINFO		SINFO;
	SECURITY_ATTRIBUTES	SA;
	HANDLE			hMap;
	char			CommandLine[1024];
	char			quote[10];

	memset((void*)&SINFO, 0, sizeof(STARTUPINFO));
	SINFO.cb = sizeof(STARTUPINFO);

	SA.nLength = sizeof (SECURITY_ATTRIBUTES);
	SA.lpSecurityDescriptor = NULL;
	SA.bInheritHandle = TRUE;
	
	if (strchr(ProcessBinary, ' '))
	{ quote[0]='"';
	  quote[1]=0;
	}
	else
	  quote[0]=0;

	if (Args[0])
		sprintf(CommandLine, "%s%s%s %s", quote, ProcessBinary, quote, Args);
	else
		sprintf(CommandLine, "%s%s%s", quote, ProcessBinary, quote);

	if(!CreateProcess(ProcessBinary, CommandLine, NULL, NULL, TRUE,
		DETACHED_PROCESS, NULL, NULL, &SINFO, &PINFO))
		{
		OperatorConsole.printf("***Failed to create process %s: %d\n", ProcessBinary, GetLastError());
		return FALSE;
		}
	else
		{
	  	WaitForSingleObject(PINFO.hProcess, INFINITE);
		CloseHandle(PINFO.hThread);
		CloseHandle(PINFO.hProcess);
		}

	return ( TRUE );
	}

#else		// UNIX...

BOOL
BackgroundExec(
	char	*theExecName,
	char	*theArg)
	{

	if (theArg[0]==0)
		{
		system(theExecName);
		return TRUE;
		}

	// fork and exec a new process... (just replicate 1 thread...)

	pid_t aPID = fork();

	if(aPID == 0)
	{
		// this is the child process...
		// lose controlling tty

		setsid();

		// close all open files, except standard I/O
		// (could close those also, and attach them to /dev/console)
		// like:
		// fd = open("/dev/console", 1);
        //	if(fd != 1)
        //	{
        //	    dup2(fd, 1);
        //	    close(fd);
        //	    fd = 1;
        //	}
        //	dup2(1, 2);

		int anIndex;
		int anOpenFileMax;

		for(anIndex = 3, anOpenFileMax = sysconf(_SC_OPEN_MAX); anIndex < anOpenFileMax; ++anIndex)
			{
			close(anIndex);
			}

		// reset signals

		for(anIndex = 1; anIndex <= SIGUSR2; ++anIndex)
			{
			(void)signal(anIndex, SIG_DFL);	
			}

		// do the exec()

		if(execlp(theExecName, theExecName, theArg, NULL))
			{
			SystemDebug.printf(
				"DriverApp::ServerChildInit(): failed to execlp(%s)\n",
				theExecName);
			//_exit(255);
			}
		}
	return ( TRUE );
	}

#endif

////////////////////////////////////////////////////////////////////////
// copy a file (possibly opening as shared file)

static BOOL
DFileCopy2(char *source, char *target, int smode)
{
	BOOL		err_status = TRUE; 	/* Error status                 */
	char		*copybuffer;        	/* The copy buffer              */
	int		inhandle, outhandle; 	/* Handle of input, output files */
	unsigned 	bufsize;            	/* Size of copy buffer          */
	unsigned	bytes;              	/* Actual transferred bytes     */

	/* Open input file for read-only. parser error if operation fails.  */

	if (!smode)
		inhandle = open( source, O_RDONLY | O_BINARY, S_IREAD);
	else
 		inhandle = sopen( source, O_RDONLY | O_BINARY, SH_DENYNO, S_IREAD);

	if( inhandle == -1) return FALSE;

	/* Ok, open output file. Parser error on permission failure: */

	outhandle = open( target, O_CREAT | O_TRUNC | O_BINARY | O_RDWR, 0666);
	if( outhandle == -1)
 		{ 
		close(inhandle);
		return FALSE;
		}

	/* get a large buffer */
	bufsize = 0xF000;

	copybuffer = (char *)malloc(bufsize);
	if (copybuffer == NULL)
   		{
		close(inhandle);
		close(outhandle);
		return FALSE;
		}

	while( !eof(inhandle) && err_status)
		{
		bytes= (unsigned)read( inhandle, copybuffer, bufsize);

#ifndef WIN32
		if (bytes == 0)
       			break;
#endif

		if (bytes == (unsigned) -1)
			{ 
			err_status= FALSE;
       			break;
			}

		if (bytes != (unsigned)write(outhandle, copybuffer, bytes))
			{
			err_status= FALSE;
       			break;
			}
		}

	free(copybuffer);


	close(outhandle);
	close(inhandle);

	/* Clean up mess in case of error : */
	if(err_status != TRUE) 
		unlink(target);

	return err_status;
	}

// Special RunTime-Classing Storage mechanism.  Used for "Generic"
// Unknown outgoing C-Store requests from the Query / Retrieve Engine
// Defined here because needed in forward code

class	RunTimeClassStorage	:
	public	StandardStorage
	{
		UID		MyUID;

	public:
		// is not used anymore and would not be thread safe (db's may not be shared)
		// Database	DB;
		BOOL	uGetUID(UID &uid) { return ( GetUID(uid) ); };
		inline	BOOL Read ( PDU_Service *PDU, DICOMCommandObject *DCO,
					DICOMDataObject *DDO )
			{ return ( StandardStorage :: Read ( PDU, DCO, DDO ) ); };
		inline	BOOL Write ( PDU_Service *PDU, DICOMDataObject *DDO)
			{ return ( StandardStorage :: Write ( PDU, DDO ) ); };
		inline	BOOL Write ( PDU_Service *PDU, DICOMDataObject *DDO, DICOMCommandObject *DCO)
			{ return ( StandardStorage :: Write ( PDU, DDO, NULL, NULL, DCO) ); };
		BOOL	GetUID(UID	&);
		BOOL	SetUID(UID	&);
		BOOL	SetUID(VR	*);
		BOOL	SetUID(DICOMDataObject	*);
#ifdef __GNUC__
		RunTimeClassStorage():MyUID() {};
#endif
		
	};

// This routine is used as a filter for export converter.
// It works by combining a given filter string with a query 
// for a given SOP, and returning TRUE if a record is found.
// maxname is used to truncate fieldnames to fit with the DB spec.
// limitations: PatientID, SeriesInstanceUID and StudyInstanceUID 
// cannot be queried. All other DB fields can be queried.

BOOL DICOM2SQLValue (char *s);

int TestFilter(char *query, char *sop, int maxname, char *patid=NULL)
{	Database aDB;
	char Tables[512], QueryString[512], Dum[256];
        int result, instring=0, inword=0, i, L, Index;
//	char *p;
	SQLLEN sdword;

	if (*query)
	 	sprintf(Tables, "%s, %s, %s, %s",
			ImageTableName, SeriesTableName, StudyTableName, PatientTableName);
	else
	 	sprintf(Tables, "%s",
			ImageTableName);

	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Filter: Unable to open database %s as user %s on %s\n",
					DataSource, UserName, DataHost);
		return 0;
		}

	// truncate the identifiers in the query to 10 characters
        // this is our default maximum field name length for the DB
        L = strlen(query);
	for (i=0; i<L; i++)
	{ if (instring)
	  { if (query[i]=='\'')
	    { instring = 0;
	    }
	    continue;
	  }
	  else if (inword)
	  { if (query[i]==' ')
	    { inword = 0;
	    }
	    else
	    { inword++;
  	      if (inword>maxname) query[i] = ' ';
	    }
	  }
	  else
	  { if (query[i]=='\'')
	    { instring = 1;
	    }
	    else if (isalpha(query[i]))
	    { inword = 1;
	    }
	  }
	}

	if (*query)
		sprintf(QueryString, 	"(%s) and "
                                "DICOMImages.SopInstanc = '%s' and "
				"DICOMStudies.PatientID = DICOMPatients.PatientID and "
				"DICOMImages.SeriesInst = DICOMSeries.SeriesInst and "
				"DICOMSeries.StudyInsta = DICOMStudies.StudyInsta",
				query, sop);
	else
		sprintf(QueryString, "DICOMImages.SopInstanc = '%s'", sop);

        // include patid (indexed) for speed; test for rev5 or higher of the database
	if (patid)
		{
		Index = 1;
        	while (ImageDB[Index].Element)
			{
			if (ImageDB[Index].Group==0x0010 && ImageDB[Index].Element==0x0020)
				{ 
				char newpatid[128];
				strcpy(newpatid, patid);
				DICOM2SQLValue(newpatid);	// allow exact match only
				sprintf(QueryString+strlen(QueryString), " AND DICOMImages.ImagePat = %s", newpatid);
				break;
        			}
			Index++;
			}
		}

	if (!aDB.Query(Tables, "DICOMImages.ObjectFile", QueryString, NULL))
		{
		OperatorConsole.printf("***Filter: Unable to query for image record\n");
		aDB.Close();
		return 0;
		}

	aDB.BindField (1, SQL_C_CHAR, Dum, 255, &sdword);
	result = aDB.NextRecord();

	aDB.Close();	

	return result;
	}

/* The following function is called at the end of 'SaveToDisk' or through a queue
   that is filled from SaveToDisk. After the arrival and storage of each DicomObject, 
   several executables may be invoked, to perform further processing.
*/

#ifdef WIN32
#include "shellapi.h"
#endif

// forward references
static int htoin(const char *value, int len);
static int isxdig(char ch);
BOOL prefetch_queue(const char *operation, char *patientid, const char *server, const char *studyuid, const char *seriesuid, const char *compress, 
		    const char *modality, const char *date, const char *sop, const char *imagetype, const char *seriesdesc, int delay=0, const char *script=NULL);
void reset_queue_fails(int N);

static BOOL match(const char *source, char *target)
{ if (strlen(target)==1 && target[0]=='*') return TRUE;

  char Source[512];
  strcpy(Source, source);
  while (strlen(Source)>0 && Source[strlen(Source)-1]==' ') Source[strlen(Source)-1] = 0;

  if (strlen(target)> 1 && target[0]=='*' && target[strlen(target)-1]=='*')
  { char sub[512];

    strcpy(sub, target+1);
    sub[strlen(sub)-1]=0;
    strlwr(sub);

    strlwr(Source);

    if (strstr(Source, sub)==0) return FALSE;
  }
  else
  if (strlen(target)> 1 && target[strlen(target)-1]=='*')
  { if (memicmp(Source, target, strlen(target)-1)) return FALSE;
  }
  else
  if (strlen(target)> 1 && target[0]=='*')
  { if (memicmp(Source+strlen(Source)-strlen(target)+1, target+1, strlen(target)-1)) return FALSE;
  }
  else
    if (stricmp(Source, target)) return FALSE;

  return TRUE;
}

// Search a composite dicom object:
// desc=gggg,eeee 	plain element
// desc=PatientID 	plain element (name from dgate.dic may be used everywhere instead of gggg,eeee)
// desc=*gggg,eeee 	search in all sequences
// desc=/*gggg,eeee 	idem
// desc=/gggg,eeee.i..  search specified in sequence, ith element (recursive)
// desc=/gggg,eeee..    idem, first element
// e.g. /gggg,eeee.0/gggg,eeee/*gggg,eeee: to search specified sequences
//
// desc=(desc1)desc2 search desc1 in DICOM object which sop is specified in (desc1), patient ID taken from 0010,0020
// e.g. %V(/300c,0060.0/0008,1155)/gggg,eeee.0/gggg,eeee
//
// returns length of parsed desc string
// if passed *A is set with the sequence array or
// if passed *O is set with the embedding dicom object

int SearchDICOMObject(DICOMObject *DDO, const char *desc, char *result, Array < DICOMDataObject * > **A, DICOMDataObject **O)
	{
	DICOMObject	DDO2;
	VR		*vr;
	int		i, g, e, len;
	unsigned int	Index;
//	char		*p;
	BOOL		seq=FALSE, wild=FALSE;

	if (A) *A = NULL;
	if (O) *O = NULL;

	result[0]=0;
	result[1]=0;	// indicates VR not found
	i      =0;

        if (desc[i]=='(')
		{
		i++;
		char r[300];
		DICOMObject *pDDO;

		SearchDICOMObject(DDO, "0010,0020", r);
		strcat(r, ":");
		i += SearchDICOMObject(DDO, desc+i, r+strlen(r));
		if (desc[i]==')') i++;
		//else OperatorConsole.printf("***Syntax error in %V(desc)desc statement %s at %d\n", desc, i);
		pDDO = LoadForGUI(r);
		i += SearchDICOMObject(pDDO, desc+i, result); // no use to allow write to A or O
		delete pDDO;
		return i;
		}

	if (desc[i]=='/') i++, seq=TRUE;
	if (desc[i]=='*') i++, seq=FALSE, wild=TRUE;

	if (isxdig(desc[i]) && desc[i+4]==',') 
	{ g = htoin(desc+i,   4);
  	  e = htoin(desc+i+5, 4);
	  i +=9;
	}
	else if (isalpha(desc[i]))
	{ char d[256]; 
	  int j;
          RTCElement Entry;
	  strcpy(d, desc+i);
	  j=0; while (isalnum(d[j])) j++; //20191019 isalpha-->isalnum
	  d[j]=0;
	  Entry.Description = d;
          if (VRType.GetGroupElement(&Entry))
	  { g = Entry.Group;
            e = Entry.Element;
	    i += j;
	  }
	  else
	    return i;
	}
	else
	  return i;  

	// get plain element
	if (!seq && !wild)
	    	{ 
		if (!DDO) return i;
		vr = DDO->GetVR(g, e);
		if (vr)
			{ 
			if (vr->SQObjectArray)
				{
				if (A) *A = (Array<DICOMDataObject*>*) vr->SQObjectArray;
				strcpy(result, "(sequence)");
				}
			else if (vr->Length>255)
				strcpy(result, "(too long)");
			else
				{
				UINT16 TypeCode = VRType.RunTimeClass(g, e, NULL);
				if      (TypeCode== 'UL')
				  for (int i=0;i<vr->Length/4;i++) sprintf(result+strlen(result), "%u\\", (unsigned int)(((UINT32*)(vr->Data))[i]));
				else if (TypeCode== 'US') //sprintf(result, "%u", (unsigned int)(*(UINT16*)(vr->Data)));
				  for (int i=0;i<vr->Length/2;i++) sprintf(result+strlen(result), "%u\\", (unsigned int)(((UINT16*)(vr->Data))[i]));
				else if (TypeCode== 'SL') //sprintf(result, "%d", (int)(*(INT32*)(vr->Data)));
				  for (int i=0;i<vr->Length/4;i++) sprintf(result+strlen(result), "%d\\", (int)(((INT32*)(vr->Data))[i]));
				else if (TypeCode== 'SS') //sprintf(result, "%d", (int)(*(INT16*)(vr->Data)));
				  for (int i=0;i<vr->Length/2;i++) sprintf(result+strlen(result), "%d\\", (int)(((INT16*)(vr->Data))[i]));
				else if (TypeCode== 'FD')
				  for (int i=0;i<vr->Length/8;i++) sprintf(result+strlen(result), "%f\\", (double)(((double*)(vr->Data))[i]));
				else if (TypeCode== 'OF')
				  for (int i=0;i<vr->Length/4;i++) sprintf(result+strlen(result), "%f\\", (double)(((float*)(vr->Data))[i]));
				else if (TypeCode== 'FL')
				  for (int i=0;i<vr->Length/4;i++) sprintf(result+strlen(result), "%f\\", (double)(((float*)(vr->Data))[i]));
				else if (TypeCode== 'OW')
				  for (int i=0;i<vr->Length/2;i++) sprintf(result+strlen(result), "%d\\", (int)(((INT16*)(vr->Data))[i]));
				else
				{ strncpy(result, (char*)vr->Data, vr->Length);
  	                	  result[vr->Length] = 0;
  	                	  if (vr->Length==0) result[vr->Length+1] = 1; // indicates empty string
				}
				if (result[strlen(result)-1] == '\\')
  	                  	  result[strlen(result)-1] = 0;
  	                	len = vr->Length - 1;
                        	while(len>0)
					{ 
					if (result[len] == ' ')
  	                  			result[len] = 0;
                          		else
                            			break;
                          		len--;
                        		}
				}
                      	}

		return i;
		}

	// get specified sequence element
	else if (seq)
	    	{ 
		int j=0;

		if (desc[i]=='.') 
			{
			i++;
			Index = atoi(desc+i);
			while (isdigit(desc[i])) i++;
			}
		else
			Index = 0;

		vr = NULL;
		if (DDO) vr = DDO->GetVR(g, e);
		if (vr)
			{ 
			if ( vr->SQObjectArray )
				{
				Array < DICOMDataObject * > *ADDO =
					(Array<DICOMDataObject*>*) vr->SQObjectArray;

				j = 0;
				if (desc[i]==0)
					{
					strcpy(result, "(sequence)");
					if (A) *A = ADDO;
					}
				else if (Index<ADDO->GetSize())
					{
					if (O) *O = ADDO->Get(Index);
					j = SearchDICOMObject(ADDO->Get(Index), desc+i, result, A, O);
					}
				}
			else
				{
				if (vr->Length>255)
					strcpy(result, "(too long)");
				else
					{
					UINT16 TypeCode = VRType.RunTimeClass(g, e, NULL);
					if      (TypeCode== 'UL') sprintf(result, "%u", (unsigned int)(*(UINT32*)(vr->Data)));
					else if (TypeCode== 'US') sprintf(result, "%u", (unsigned int)(*(UINT16*)(vr->Data)));
					else if (TypeCode== 'SL') sprintf(result, "%d", (int)(*(INT32*)(vr->Data)));
					else if (TypeCode== 'SS') sprintf(result, "%d", (int)(*(INT16*)(vr->Data)));
					else
					{ strncpy(result, (char*)vr->Data, vr->Length);
  	                		  result[vr->Length] = 0;
  	                	  	  if (vr->Length==0) result[vr->Length+1] = 1;  // indicates empty string
					}
  	        	        	len = vr->Length - 1;
                        		while(len>0)
						{ 
						if (result[len] == ' ')
  		                  			result[len] = 0;
                	          		else
                        	    			break;
                          			len--;
                        			}
					}
                      		}
			}
		else
			{
			// ignore rest of desc
			char dum[1024];
			j = SearchDICOMObject(DDO, desc+i, dum);
			}

		return i+j;
		}
	
	// wild: search all sequences
	if (!DDO) return i;
	while((vr=DDO->Pop()))
		{
		if (*result==0)
			{ 
			if ( vr->SQObjectArray )
				{
				Array < DICOMDataObject * > *ADDO =
					(Array<DICOMDataObject*>*) vr->SQObjectArray;
				
				Index = 0;
				while ( Index < ADDO->GetSize() )
					{
					SearchDICOMObject(ADDO->Get(Index), desc, result);
					if (*result) break;
					++Index;
					}
				}
			else if (vr->Group==g && vr->Element==e)
				{
				if (vr->Length>255)
					strcpy(result, "(too long)");
				else
					{
					UINT16 TypeCode = VRType.RunTimeClass(g, e, NULL);
					if      (TypeCode== 'UL') sprintf(result, "%u", (unsigned int)(*(UINT32*)(vr->Data)));
					else if (TypeCode== 'US') sprintf(result, "%u", (unsigned int)(*(UINT16*)(vr->Data)));
					else if (TypeCode== 'SL') sprintf(result, "%d", (int)(*(INT32*)(vr->Data)));
					else if (TypeCode== 'SS') sprintf(result, "%d", (int)(*(INT16*)(vr->Data)));
					else
					{ strncpy(result, (char*)vr->Data, vr->Length);
  	                		  result[vr->Length] = 0;
	  	                	  if (vr->Length==0) result[vr->Length+1] = 1;  // indicates empty string
					}
  	                		len = vr->Length - 1;
	                        	while(len>0)
						{ 
						if (result[len] == ' ')
  	                	  			result[len] = 0;
                          			else
                            				break;
	                          		len--;
        	                		}
					}
				}
			}

		DDO2.Push(vr);
		}

	DDO->Reset();
	while((vr=DDO2.Pop()))
		{
		DDO->Push(vr);
		}

	return i;
	}


struct scriptdata
{ ExtendedPDU_Service *PDU;
  DICOMCommandObject *DCO;
  DICOMDataObject *DDO;
  int  N;
  char *pszModality;
  char *pszStationName;
  char *pszSop;
  char *patid;
  char *Storage;
  int  rc;
  unsigned int ConnectedIP;
} sd1;

const char *do_lua(lua_State **L, char *cmd, struct scriptdata *sd);

// operates asynchronously on images AFTER it they are stored in the database
// return TRUE if it worthwile to retry (connection or write failed)
// For general scripting use:
// CallExportConverterN(FileName, -1, NULL, NULL, NULL,NULL, NULL, NULL, NULL, NULL, NULL, script);

BOOL CallExportConverterN(char *pszFileName, int N, char *pszModality, char *pszStationName, char *pszSop, char *patid, ExtendedPDU_Service *PDU, char *ForwardLastUID, char *calling, char *called, char *script)
{ char		szRootSC[64];
  char		szEntry[32];
  char		szTemp[66];
  int		i1, rc, MaxLength, part, skipping;
  char		szExecName[512], szNext[512];
  char		szExecModality[66];
  char		szExecStationName[66];
  char		szExecFilter[512];
  char		ExportCalledAE[18], ExportCallingAE[18], Called[18], Calling[18];
  BOOL		StripGroup2;

  PDU->SetLocalAddress ( (BYTE *)calling );
  PDU->SetRemoteAddress ( (BYTE *)called );

  DICOMDataObject  *DDO = NULL;
  ExtendedPDU_Service PDU2;  // not used as script context

  PDU2.AttachRTC(&VRType);

  if (!script || *script==0)
  { if (!MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig,
		  szRootSC, 64, ConfigFile)) return FALSE;

    // [lua] ExportConverterN: has all ImportConverter functionality + script("defer")
    sprintf(szEntry, "ExportConverter%d", N);
    MyGetPrivateProfileString("lua", szEntry, "", szExecName, 512, ConfigFile);
    if (szExecName[0])
    { if (!DDO) DDO = PDU2.LoadDICOMDataObject(pszFileName);
      struct scriptdata sd1 = {PDU, NULL, DDO, N, pszModality, pszStationName, pszSop, patid, NULL, 0, 0};

      do_lua(&(PDU->L), szExecName, &sd1);
      if (sd1.rc==8)
      { SystemDebug.printf("Exportconverter%d.%d: defer\n", N, -1);
	reset_queue_fails(N);
	if (DDO) delete DDO;
	return TRUE; // retry later
      }

      if (DDO) delete DDO;
      return FALSE;
    }

    /* filter using modality, stationname, calling and called */

    if (pszModality)
    { sprintf(szEntry, "ExportModality%d", N);
      if (!MyGetPrivateProfileString(szRootSC, szEntry, "*", szExecModality, 64, ConfigFile)) return FALSE;
      if (!match(pszModality, szExecModality)) return FALSE;
    }

    if (pszStationName)
    { sprintf(szEntry, "ExportStationName%d", N);
      if (!MyGetPrivateProfileString(szRootSC, szEntry, "*", szExecStationName, 64, ConfigFile)) return FALSE;
      if (!match(pszStationName, szExecStationName)) return FALSE;
    }

    if (calling)
    { sprintf(szEntry, "ExportCallingAE%d", N);
      if (!MyGetPrivateProfileString(szRootSC, szEntry, "*", ExportCallingAE, 18, ConfigFile)) return FALSE;
      if (!match(calling, ExportCallingAE)) return FALSE;
    }

    if (called)
    { sprintf(szEntry, "ExportCalledAE%d", N);
      if (!MyGetPrivateProfileString(szRootSC, szEntry, "*", ExportCalledAE, 18, ConfigFile)) return FALSE;
      if (!match(called, ExportCalledAE)) return FALSE;
    }

    /* apply database filter */

    if (called && calling)
    { sprintf(szEntry, "ExportFilter%d", N);
      if (!MyGetPrivateProfileString(szRootSC, szEntry, "*", szExecFilter, 510, ConfigFile)) return FALSE;
      if (szExecFilter[0]!='*') 
      { if (!MyGetPrivateProfileString(szRootSC, "TruncateFieldNames", "255",	szTemp, 32, ConfigFile)) return FALSE;
	MaxLength = atoi(szTemp);
    
	if (!TestFilter(szExecFilter, pszSop, MaxLength, patid)) return FALSE;
      }
    }

    /* get the export converter name: empty is none */

    if (called && calling)
    { sprintf(szEntry, "ExportConverter%d", N);
      if (!MyGetPrivateProfileString(szRootSC, szEntry, "",
	    szExecName, 512, ConfigFile)) return FALSE;
      if (szExecName[0]==0) return FALSE;
    }
    else
      strcpy(szExecName, ForwardLastUID);
  }
  else
    strcpy(szExecName, script);	/* script passed directly */

  rc       = 1;	// status for if statement and stop
  part     = 0;
  skipping = 0;

  while(1)
  { szNext[0] = 0;

    if (skipping || rc==4)			// {} block being skipped
    { if      (szExecName[0]=='{') skipping++, rc=0;
      else if (szExecName[0]=='}') skipping--, rc=0;
    }

    // remove {} for all export converters (not only those with a %)
    i1 = 0;
    if      (szExecName[0] == '{') {i1++; while(szExecName[i1]==' ')i1++; memmove(szExecName, szExecName+i1, strlen(szExecName+i1)+1); }
    else if (szExecName[0] == '}') {i1++; while(szExecName[i1]==' ')i1++; memmove(szExecName, szExecName+i1, strlen(szExecName+i1)+1); }
        
    // find ; not in string: splits export commands
    int instring=0;
    int L1 = strlen(szExecName);
    for (int k=0; k<L1; k++) 
    { if (szExecName[k]=='"') instring = !instring;
      if (szExecName[k]==';' && instring==0)
      { szExecName[k] = 0;			// split command
        while (szExecName[k+1]==' ') k++;	// skip spaces after ;
        strcpy(szNext, szExecName+k+1);		// remaining for next
        break;
      }
      if (memicmp(szExecName+k, "lua:", 4)==0 && instring==0)
	break;
    }
  
    if (skipping)	// {} block being skipped
    { SystemDebug.printf("Exportconverter%d.%d skipped\n", N, part);
    }

    else if (szExecName[0]==0)	// leftover of { or }
    { 
    }

    else if (rc==4)	// ifxxxx statement causes a skip
    { rc = 0;
      SystemDebug.printf("Exportconverter%d.%d not executed because of previous statement\n", N, part);
    }

    /* for all export converters: first substitute % variables */
  
    else
    { char line[1000];

      if (1 || strchr(szExecName, '%'))
      { int i=0, l, L;// len;
        char *p;
//        VR   *pVR;
//        char vr[256];
   
        /* substitute %x variables and remove possible { and } at begin of statement */
  
        line[0] = 0;
        instring = 0;
  
        L = strlen(szExecName);
        while (i<L)
        { if      (i==0 && szExecName[i] == '{') {i++; while(szExecName[i]==' ')i++;}
          else if (i==0 && szExecName[i] == '}') {i++; while(szExecName[i]==' ')i++;}
          else if (szExecName[i] == '"')
          { if (instring && szExecName[i+1]=='"')
            { strcat(line, """"); 
              i+=2;
            }
            else
            { l = strlen(line);
              line[l]   = szExecName[i++];
              line[l+1] = 0;
            }
            instring = !instring;
          }
          else if (szExecName[i] == '%')
          { char *ps = line + strlen(line);

            switch(tolower(szExecName[i+1]))
            { case 'f': strcat(line, pszFileName);	// %f=filename
                        break;
              case 'b': p = strrchr(pszFileName, PATHSEPCHAR);
                        if (p)				// %b=file base name
                        { strcat(line, p+1);
                          p = strrchr(line, '.');
                          if (p) *p=0;
                        }
                        break;
              case 'p': strcat(line, pszFileName);	// %p=file path
                        p = strrchr(line, PATHSEPCHAR);
                        if (p) *p=0;
                        break;
              case 'm': if (pszModality)
                          strcat(line, pszModality);	// %m=modality
                        else
		        { char result[256];
                          if (!DDO) DDO = PDU2.LoadDICOMDataObject(pszFileName);
	      	          SearchDICOMObject(DDO, "0008,0060", result);
                          strcat(line, result);
                        }
                        break;
              case 's': if (pszStationName)
                          strcat(line, pszStationName);	// %s=stationname
                        else
		        { char result[256];
                          if (!DDO) DDO = PDU2.LoadDICOMDataObject(pszFileName);
	      	          SearchDICOMObject(DDO, "0008,1010", result);
                          strcat(line, result);
                        }
                        break;
              case 'o': if (pszSop)
                          strcat(line, pszSop);		// %o=object instance
                        else
		        { char result[256];
                          if (!DDO) DDO = PDU2.LoadDICOMDataObject(pszFileName);
	      	          SearchDICOMObject(DDO, "0008,0018", result);
                          strcat(line, result);
                        }
                        break;
              case 'i': if (patid)
                          strcat(line, patid);		// %i=patient ID
                        else
		        { char result[256];
                          if (!DDO) DDO = PDU2.LoadDICOMDataObject(pszFileName);
	      	          SearchDICOMObject(DDO, "0010,0020", result);
                          strcat(line, result);
                        }
                        break;
              case 'u': Calling[0]=0;
		        if (calling) strcpy(Calling, calling);
		        if (Calling[0]>32)
        	          while (strlen(Calling)>0 && Calling[strlen(Calling)-1]==' ') Calling[strlen(Calling)-1] = 0;
		        strcat(line, Calling);		// %u=SCU
                        break;
              case 'c': Called[0]=0;
		        if (called) strcpy(Called, called);
		        if (Called[0]>32)
			  while (strlen(Called)>0 && Called[strlen(Called)-1]==' ') Called[strlen(Called)-1] = 0;
		        strcat(line, Called);		// %c=calledAE = scp
                        break;
              case 'd': time_t TimeOfDay;		// %d=date and time
                        char TimeString[128], buf[64];
                        UNUSED_ARGUMENT(buf);//Stop gcc4.2 warning bcb
                        TimeOfDay = time(NULL);
                        strcpy(TimeString, ctime_r(&TimeOfDay, buf));
                        TimeString[strlen(TimeString)-1] = '\0';
                        strcat(line, TimeString);
                        break;
              case 'n': strcat(line, "\n");		// %n=newline
                        break;
              case 't': strcat(line, "\t");		// %t=tab
                        break;
              case '%': strcat(line, "%");		// %%=%
                        break;
              case '^': strcat(line, "^");		// %^=^
                        break;
              case '~': strcat(line, "~");		// %~=~
                        break;
              case '[': strcat(line, "[");		// %[=[
                        break;
              case 'v': 				// %Vxxxx,yyyy=any vr (group and element must have 4 digits)
		        char result[256];		// %V*xxxx,yyyy=any vr in any sequence
                        if (!DDO) DDO = PDU2.LoadDICOMDataObject(pszFileName);
	      	        i += SearchDICOMObject(DDO, szExecName+i+2, result);
                        strcat(line, result);
                        break;
	      case 'a': {				// %Axxxx,yyyy=crc of any vr (group and element must have 4 digits)
		        char result2[256];
                        if (!DDO) DDO = PDU2.LoadDICOMDataObject(pszFileName);
			i += SearchDICOMObject(DDO, szExecName+i+2, result2);
		        unsigned int crc = ComputeCRC(result2, strlen(result2));
		        sprintf(result2, "%u", crc);
                        strcat(line, result2);
                        break;
			}
	      case 'e': {				// %Exxxx,yyyy= changed UID for any UID
		        char result2[256], result3[256];
                        if (!DDO) DDO = PDU2.LoadDICOMDataObject(pszFileName);
			i += SearchDICOMObject(DDO, szExecName+i+2, result2);
			ChangeUID(result2, "percente", result3, NULL, NULL);
                        strcat(line, result3);
                        break;
			}
	      case 'r': {				// %Rxxxx,yyyy= old UID for any changed UID
		        char result2[256], result3[256];
                        if (!DDO) DDO = PDU2.LoadDICOMDataObject(pszFileName);
			i += SearchDICOMObject(DDO, szExecName+i+2, result2);
			ChangeUIDBack(result2, result3, NULL, NULL, NULL);
                        strcat(line, result3);
                        break;
			}
	      case 'g': {				// %g generate UID
		        char result2[256];
			GenUID(result2);
                        strcat(line, result2);
                        break;
			}

              // case 'q': not (yet) supported for export converters - probably no need
            }
            i += 2;

  	    // get substring of variable [start, end] or [,fromend]
            if (szExecName[i]=='[')
            { int a=0, b=-1;
	      if (szExecName[i+1]==',')
                sscanf(szExecName+i, "[,%d]", &b), a=-1;
	      else
                sscanf(szExecName+i, "[%d,%d]", &a, &b);
              for (;i<L;i++) if (szExecName[i-1]==']') break;
              if (a>(signed int)strlen(ps)) a=strlen(ps);
	      if (a<0) { a = strlen(ps)-b; b = strlen(ps); }
	      if (a<0) a = 0;
              if (b>(signed int)strlen(ps)) b=strlen(ps);
              if (b>=a) {memmove(ps, ps+a, b-a+1); ps[b-a+1]=0;}
            }

	    // convert to uppercase
            if (szExecName[i]=='^')
            { i++;
	      strupr(ps);
            }
	    // convert to lowercase
            else if (szExecName[i]=='~')
            { i++;
	      strlwr(ps);
            }
          }
          else
          { l = strlen(line);
            line[l]   = szExecName[i++];
            line[l+1] = 0;
          }
        }
  
        // if (DDO) delete DDO;
      }
      else
	strcpy(line, szExecName);

      /* converter: direct forward of received object */

      if (memicmp(line, "forward to", 10)==0 ||
          memicmp(line, "forward compressed", 18)==0
         )
      { char host[512], port[64], compress[64], dummy[64], Level[64], Temp[66];
        char *p, *q, *c;
        RunTimeClassStorage	RTCStorage;
        UID			AppUID ("1.2.840.10008.3.1.1.1");
        UID			iUID, uid;
        VR			*vr;
        int			offset = 11, level;
	BYTE			org[512], dest[512];
	char			script[512];
        
        // fallback if none passed: forces one association per image
        if (!PDU) PDU = &PDU2;
  
        // preprocess the forwarder string
        if (memicmp(line, "forward compressed as ", 22)==0) offset = 28;
  					      // xx to	where xx = 'is', 'un', 'n0'..'n4', or 'jl', 'jk', 'j0'..'j6'
					      // xxNN same as above but NN is lossy quality

        // offer transfer syntax (compression) from forward string
	compress[0]=0; compress[1]=0; compress[3]=0; 
        if (offset==28)
        { compress[0] = line[22];
          compress[1] = line[23];
          compress[2] = 0;
          if (line[24] != ' ') 
	  { compress[2] = line[24];
	    compress[3] = line[25];
            compress[4] = 0;
	    offset = 30;
	  }
        }

	// get the script clause (to process), must be last
        script[0] = 0;
        p = strstr(line + offset, " script ");
        if (p)
        { strcpy((char *)script, p+8);
          p[8] = 0;
        }

        // get the org clause (originator)
        strcpy((char *)org, (char *)MYACRNEMA);
        p = strstr(line + offset, " org ");
        if (p)
        { strcpy((char *)org, p+5);
          q = strchr((char *)org, ' '); if (q) *q=0;
        }

        // get the dest clause (called AE)
        dest[0] = 0;
        p = strstr(line + offset, " dest ");
        if (p)
        { strcpy((char *)dest, p+6);
          q = strchr((char *)dest, ' '); if (q) *q=0;
        }
  
        // get the to AE
	q = strchr(line+offset, ' '); if (q) *q=0;
	c=compress; 
	if (compress[0]) c=dummy;
	if(!GetACRNema(line+offset, (char *)host, (char *)port, (char *)c))
        { // it does not exist: get host:port
	  strcpy(host, line+offset);
          p = strchr(host, ':');
          if (p) 
          { *p=0;
            strcpy(port, p+1);
          }
          else 
            strcpy(port, "5678");
          strcpy(compress, "UN");
        }
   
        OperatorConsole.printf("ExportConverter%d.%d: forward %s to %s\n", N, part, pszFileName, line+offset);
  
        // load and check the data
        if (!DDO) 
          DDO = PDU2.LoadDICOMDataObject(pszFileName);

        if (!DDO)
        { OperatorConsole.printf("*** ExportConverter%d.%d: Forward failed to load DICOM object %s\n", N, part, pszFileName);
          return FALSE;
        }
  
        // get UID at selected ForwardAssociationLevel into szTemp
        vr = NULL;
        level = 0;
        MyGetPrivateProfileString(szRootSC, "ForwardAssociationLevel", "STUDY", Level, 64, ConfigFile);
        if      (strncmp(Level, "PATIENT", 7)==0) level=1;
        else if (strncmp(Level, "STUDY",   5)==0) level=2;
        else if (strncmp(Level, "SERIES",  6)==0) level=3;
        else if (strncmp(Level, "IMAGE",   5)==0) level=4;
        else if (strncmp(Level, "SOPCLASS",8)==0) level=5, PDU[part].ClearAbstractSyntaxs();
        else if (strncmp(Level, "GLOBAL",  6)==0) level=6;
        szTemp[0] = 0;
        if (level==1) vr = DDO->GetVR(0x0010, 0x0020);
        if (level==2) vr = DDO->GetVR(0x0020, 0x000d);
        if (level==3) vr = DDO->GetVR(0x0020, 0x000e);
        if (level==4) vr = DDO->GetVR(0x0008, 0x0018);
        if (level==5) vr = DDO->GetVR(0x0008, 0x0016);
        if (vr) memcpy(szTemp, vr->Data, vr->Length);
	if (vr) szTemp[vr->Length]=0;
  
        // get sopclass (to check whether it is accepted at the current connection)
        vr = DDO -> GetVR(0x0008, 0x0016);
        if (!vr)
        { OperatorConsole.printf("*** ExportConverter%d.%d: Forward failed because SopClass is missing in %s\n", N, part, pszFileName);
          delete DDO;
          return FALSE;
        }
  
        // does the existing link accept images of this type - if not then hang up to force a reconnect
        // also hang up when UID at ForwardAssociationLevel changes to start a new association
        if (PDU[part].Link.Connected)
        { SetUID ( iUID, vr );
          if (!PDU[part].IsAbstractSyntaxAccepted(iUID) || strcmp(ForwardLastUID+part*66, szTemp)!=0 )
          { //OperatorConsole.printf("!!! ExportConverter%d.%d: attempt to reconnect %s \n", N, part, szTemp);
	    OperatorConsole.printf("ExportConverter%d.%d: forward association closed by %s\n", 
	      N, part, strcmp(ForwardLastUID+part*66, szTemp)!=0?Level:"SOPCLASS");
            MyGetPrivateProfileString(szRootSC, "ForwardAssociationRelease", "1", Temp, 64, ConfigFile);
            if (atoi(Temp)) PDU[part].Close();
  	    else            PDU[part].Link.Close();
          }
        }
  
        // for new check of UID at ForwardAssociationLevel
        strcpy(ForwardLastUID+part*66, szTemp);
  
        // (re)connect
        if (!PDU[part].Link.Connected)
        { PDU[part].AttachRTC(&VRType);
          PDU[part].SetRequestedCompressionType(compress);
  
          PDU[part].SetApplicationContext ( AppUID );
          PDU[part].SetLocalAddress ( org );
	  if (dest[0])
            PDU[part].SetRemoteAddress ( (unsigned char *)(dest) );
	  else
            PDU[part].SetRemoteAddress ( (unsigned char *)(line+offset) );
          PDU[part].SetTimeOut(TCPIPTimeOut);
  
          vr = DDO -> GetVR(0x0008, 0x0016);
          SetUID ( iUID, vr );
          PDU[part].AddAbstractSyntax ( iUID );		// adds type of this image to presentation contexts
    
          uid.Set("1.2.840.10008.1.1");		// This one should always accept: verification
          PDU[part].AddAbstractSyntax(uid);		// assures connect will not return FALSE because image is not accepted
    
          if (!PDU[part].Connect((unsigned char *)&host, (unsigned char *)&port))
          { OperatorConsole.printf("*** ExportConverter%d.%d: Forward failed to connect to host %s\n", N, part, line+offset);
            delete DDO;
            return TRUE; // may be good idea to retry later
          }
        }
  
        vr = DDO -> GetVR(0x0008, 0x0016);
        SetUID ( iUID, vr );
        if (!PDU[part].IsAbstractSyntaxAccepted(iUID))
        { OperatorConsole.printf("*** ExportConverter%d.%d: DICOM server %s does not accept type of forwarded image\n", N, part, line+offset);
          PDU[part].Close();
          delete DDO;
          return FALSE;
        }
  
	// process data
        if (script)
          CallImportConverterN(NULL, DDO, -1, NULL, NULL, NULL, NULL, PDU+part, NULL, script);

        // recompress data to be forwarded here according to accepted compression mode; strip group 2 unless "as" or "is"
        p = PDU[part].GetAcceptedCompressionType(iUID);
        StripGroup2 = memicmp(p, "as", 2)!=0 && memicmp(p, "is", 2)!=0;

	// reuse characters in compress to keep NN quality bit (in compress[2] and [3])
	compress[0] = p[0];
	compress[1] = p[1];
        recompress(&DDO, compress, "", StripGroup2, PDU);
  
        RTCStorage.SetUID(iUID);
  
        if (!RTCStorage.Write(PDU+part, DDO))
        { OperatorConsole.printf("*** ExportConverter%d.%d: Forward failed to send DICOM image to %s\n", N, part, line+offset);
          PDU[part].Close();
          delete DDO;
	  MyGetPrivateProfileString(szRootSC, "RetryForwardFailed", "0", Temp, 64, ConfigFile);
          if (atoi(Temp)) return TRUE;
	  else            return FALSE; // NOT a good idea to retry later
        }
      
        ImagesForwarded++;
        if (level==4) 
        { MyGetPrivateProfileString(szRootSC, "ForwardAssociationRelease", "1", Temp, 64, ConfigFile);
          if (atoi(Temp)) PDU[part].Close();
          else		  PDU[part].Link.Close();
        }

	delete DDO;	// write was destructive
        DDO = NULL;
      }
  
      /* converter: write "string" to file - uses text file, use %n for newline */
  
      else if (memicmp(line, "write \"", 7)==0)
      { char string[256];
        char *file;
        FILE *f;
        char *p;
  
        strcpy(string, line+7);
        p = strstr(string, "\" to ");
        if (!p) p = strstr(string, "\" TO ");
        if (p)
        { *p=0;
  
          OperatorConsole.printf("Exportconverter%d.%d executes: %s\n", N, part, line);
  
          file = p+5;
          f = fopen(file, "wt");
          if (f)
          { fputs(string, f);
            fclose(f);
          }
          else
            OperatorConsole.printf("*** Exportconverter%d.%d: Failed to write to file %s\n", N, part, file);
        }
      }
  
      /* converter: append "string" to file - uses text file, use %n for newline */
  
      else if (memicmp(line, "append \"", 8)==0)
      { char string[256];
        char *file;
        FILE *f;
        char *p;
  
        strcpy(string, line+8);
        p = strstr(string, "\" to ");
        if (!p) p = strstr(string, "\" TO ");
        if (p)
        { *p=0;
  
          OperatorConsole.printf("Exportconverter%d.%d executes: %s\n", N, part, line);
  
          file = p+5;
          f = fopen(file, "at");
          if (f)
          { fputs(string, f);
            fclose(f);
          }
          else
            OperatorConsole.printf("*** Exportconverter%d.%d: Failed to append to file %s\n", N, part, file);
        }
      }
  
      /* converter: copy file to file; copy file to directory */
  
      else if (memicmp(line, "copy ", 5)==0)
      { char string[1024];
        char *file;
        struct stat statbuf;
        char *p;
          
	strcpy(string, line+5);
        p = strstr(string, " to ");
        if (!p) p = strstr(string, " TO ");
        if (p)
        { *p=0;
  
          OperatorConsole.printf("Exportconverter%d.%d executes: %s\n", N, part, line);
  
          file = p+4;
  
  	  /* if the destination a directory then append the source filename to it */
  
          stat(file, &statbuf);
          if (statbuf.st_mode & S_IFDIR)
          { p = strrchr(pszFileName, PATHSEPCHAR);
            if (p)
            { if (file[strlen(file)-1]==PATHSEPCHAR) 
                file[strlen(file)-1]=0;
              strcat(file, p);
            }
          }
  
          if (!DFileCopy2(string, file, 0))
            OperatorConsole.printf("*** Exportconverter%d.%d: Failed to copy %s to %s\n", N, part, string, file);
  	else
  	  ImagesCopied++;
        }
      }

      /* converter: save to filename (filename can be generated using all % tricks) */

      else if (memicmp(line, "save to ", 8)==0)
      { char *file = line+8;

        OperatorConsole.printf("%sconverter%d.%d executes: %s\n", "Export", N, part, line);
 
        if (!DDO) DDO = PDU2.LoadDICOMDataObject(pszFileName);
        SaveDICOMDataObject(file, DDO);
        delete DDO;
	DDO=NULL;
      }

      /* converter: save frame N to filename (filename can be generated using all % tricks) */

      else if (memicmp(line, "save frame ", 11)==0)
      { char *p1 = strstr(line, " to ");
        char *file = p1 + 4;
        int frame = atoi(line+11);

        if (p1)
        { OperatorConsole.printf("%sconverter%d.%d executes: %s\n", "Export", N, part, line);

          if (!DDO) DDO = PDU2.LoadDICOMDataObject(pszFileName);
          ExtractFrame(DDO, frame);
          SaveDICOMDataObject(file, DDO);
          delete DDO;
  	  DDO=NULL;
        }
      }

      /* converter: save bmp [frame N] [size N] filename (filename can be generated using all % tricks) */

      else if (memicmp(line, "save bmp ", 9)==0 ||
               memicmp(line, "save gif ", 9)==0 ||
               memicmp(line, "save jpg ", 9)==0)
      { int level=0, window=0, frame=0, size=0, quality=95;
        float gamma=1;
        char *file=NULL, *p1;
        p1 = strstr(line, " to ");
        if (p1) file = p1 + 4;
        p1 = strstr(line, " as ");
        if (p1) file = p1 + 4;
        p1 = strstr(line, " level ");
        if (p1) level = atoi(p1+7);
        p1 = strstr(line, " window ");
        if (p1) window = atoi(p1+8);
        p1 = strstr(line, " quality ");
        if (p1) quality = atoi(p1+9);
        p1 = strstr(line, " frame ");
        if (p1) frame = atoi(p1+7);
        p1 = strstr(line, " size ");
        if (p1) size = atoi(p1+6);
        p1 = strstr(line, " gamma ");
        if (p1) gamma = atof(p1+7);
        if (size<10) size = 4096;

        if (file)
        { SystemDebug.printf("%sconverter%d.%d executes: %s\n", "Export", N, part, line);
          if (!DDO) DDO = PDU2.LoadDICOMDataObject(pszFileName);

	       if (memicmp(line + 5, "bmp ", 4)==0) ToBMP(DDO, file, size, 0, level, window, frame, gamma);
	  else if (memicmp(line + 5, "gif ", 4)==0) ToGif(DDO, file, size, 0, level, window, frame, gamma);
	  else                                      ToJPG(DDO, file, size, 0, level, window, frame, quality, gamma);
        }
      }

      // lua "" export converter: has all ImportConverter functionality + script("defer")
      // also allow lua: -- must be last

      else if (memicmp(line, "lua \"", 5)==0 || memicmp(line, "lua:", 4)==0)
      { char cmd[1024];
        if (!DDO) DDO = PDU2.LoadDICOMDataObject(pszFileName);

        // note; threadnum and dco not implemented
        struct scriptdata sd = {PDU, NULL, DDO, N, pszModality, pszStationName, pszSop, patid, NULL, 0, 0};
        if (memicmp(line, "lua:", 4)==0) 
	  strcpy(cmd, line+4);
	else                             
	{ strcpy(cmd, line+5);
          cmd[strlen(cmd)-1]=0;
	}
        do_lua(&(PDU->L), cmd, &sd);
        rc = sd.rc;
	if (rc==8)
	{ SystemDebug.printf("Exportconverter%d.%d: defer\n", N, part);
          reset_queue_fails(N);
          if (DDO) delete DDO;
          return TRUE; // retry later
        }
      }

      /* converter: ifnotempty "string" (only execute next converter if string not empty) */
    
      else if (memicmp(line, "ifnotempty \"", 12)==0)	
      { char string[1024];
        char *p;
    
        strcpy(string, line+12);
        p = strrchr(string, '"');
        if (p)
        { *p=0;

          SystemDebug.printf("Exportconverter%d.%d executes: %s\n", N, part, line);
    
          int len = strlen(string);
          if (len==0) rc = 4;
        }
      }
    
      /* converter: ifempty "string" (only execute next converter if string empty) */
    
      else if (memicmp(line, "ifempty \"", 9)==0)	
      { char string[1024];
        char *p;
    
        strcpy(string, line+9);
        p = strrchr(string, '"');
        if (p)
        { *p=0;
    
          SystemDebug.printf("Exportconverter%d.%d executes: %s\n", N, part, line);
    
          int len = strlen(string);
          if (len!=0) rc = 4;
        }
      }
  
      /* converter: ifequal "string1","string2" (only execute next converter if string1 equals string2) */
    
      else if (memicmp(line, "ifequal \"", 9)==0)	
      { char string[1024];
        char *p, *string2;
    
        strcpy(string, line+9);
        p = strrchr(string, '"');
        if (p)
        { *p=0;

          p = strstr(string, "\",\"");
          if (!p) p = strstr(string, "\", \"");
	  if (p) 
          { string2 = strchr(p+1, '"')+1;
            *p=0;
            SystemDebug.printf("Exportconverter%d.%d executes: %s\n", N, part, line);
            int cmp = strcmp(string, string2);
            if (cmp!=0) rc = 4;
          }
        }
      }
 
      /* converter: ifnotequal "string1","string2" (only execute next converter if string1 not equals string2) */
    
      else if (memicmp(line, "ifnotequal \"", 12)==0)	
      { char string[1024];
        char *p, *string2;
    
        strcpy(string, line+12);
        p = strrchr(string, '"');
        if (p)
        { *p=0;

          p = strstr(string, "\",\"");
          if (!p) p = strstr(string, "\", \"");
	  if (p) 
          { string2 = strchr(p+1, '"')+1;
            *p=0;
            SystemDebug.printf("Exportconverter%d.%d executes: %s\n", N, part, line);
            int cmp = strcmp(string, string2);
            if (cmp==0) rc = 4;
          }
        }
      }

      /* converter: ifmatch "string1","string2" (only execute next converter if string1 matches string2 - case insensitive) */
    
      else if (memicmp(line, "ifmatch \"", 9)==0)	
      { char string[1024];
        char *p, *string2;
    
        strcpy(string, line+9);
        p = strrchr(string, '"');
        if (p)
        { *p=0;

          p = strstr(string, "\",\"");
          if (!p) p = strstr(string, "\", \"");
	  if (p) 
          { string2 = strchr(p+1, '"')+1;
            *p=0;
            SystemDebug.printf("Exportconverter%d.%d executes: %s\n", N, part, line);
            if (!match(string, string2)) rc = 4;
          }
        }
      }

      /* converter: ifnotmatch "string1","string2" (only execute next converter if string1 not matches string2 - case insensitive) */
    
      else if (memicmp(line, "ifnotmatch \"", 12)==0)	
      { char string[1024];
        char *p, *string2;
    
        strcpy(string, line+12);
        p = strrchr(string, '"');
        if (p)
        { *p=0;

          p = strstr(string, "\",\"");
          if (!p) p = strstr(string, "\", \"");
	  if (p) 
          { string2 = strchr(p+1, '"')+1;
            *p=0;
            SystemDebug.printf("Exportconverter%d.%d executes: %s\n", N, part, line);
            if (match(string, string2)) rc = 4;
          }
        }
      }

      /* converter: ifnumequal "string1","string2" (only execute next converter if string1 num equals string2 - integer) */
    
      else if (memicmp(line, "ifnumequal \"", 12)==0)	
      { char string[1024];
        char *p, *string2;
    
        strcpy(string, line+12);
        p = strrchr(string, '"');
        if (p)
        { *p=0;

          p = strstr(string, "\",\"");
          if (!p) p = strstr(string, "\", \"");
	  if (p) 
          { string2 = strchr(p+1, '"')+1;
            *p=0;
            SystemDebug.printf("Exportconverter%d.%d executes: %s\n", N, part, line);
            if (atoi(string)!=atoi(string2)) rc = 4;
          }
        }
      }

      /* converter: ifnumnotequal "string1","string2" (only execute next converter if string1 num not equals string2 - integer) */
    
      else if (memicmp(line, "ifnumnotequal \"", 15)==0)	
      { char string[1024];
        char *p, *string2;
    
        strcpy(string, line+15);
        p = strrchr(string, '"');
        if (p)
        { *p=0;

          p = strstr(string, "\",\"");
          if (!p) p = strstr(string, "\", \"");
	  if (p) 
          { string2 = strchr(p+1, '"')+1;
            *p=0;
            SystemDebug.printf("Exportconverter%d.%d executes: %s\n", N, part, line);
            if (atoi(string)==atoi(string2)) rc = 4;
          }
        }
      }

      /* converter: ifnumgreater "string1","string2" (only execute next converter if string1 num greater than string2 num - integer) */
    
      else if (memicmp(line, "ifnumgreater \"", 14)==0)	
      { char string[1024];
        char *p, *string2;
    
        strcpy(string, line+14);
        p = strrchr(string, '"');
        if (p)
        { *p=0;

          p = strstr(string, "\",\"");
          if (!p) p = strstr(string, "\", \"");
	  if (p) 
          { string2 = strchr(p+1, '"')+1;
            *p=0;
            SystemDebug.printf("Exportconverter%d.%d executes: %s\n", N, part, line);
            if (atoi(string)<=atoi(string2)) rc = 4;
          }
        }
      }

      /* converter: ifnumless "string1","string2" (only execute next converter if string1 num less than string2 num - integer) */
    
      else if (memicmp(line, "ifnumless \"", 11)==0)	
      { char string[1024];
        char *p, *string2;
    
        strcpy(string, line+11);
        p = strrchr(string, '"');
        if (p)
        { *p=0;

          p = strstr(string, "\",\"");
          if (!p) p = strstr(string, "\", \"");
	  if (p) 
          { string2 = strchr(p+1, '"')+1;
            *p=0;
            SystemDebug.printf("Exportconverter%d.%d executes: %s\n", N, part, line);
            if (atoi(string)>=atoi(string2)) rc = 4;
          }

        }
      }

      /* converter: between "string1","string2" (execute next converter on time in hours: between "9", "17"; defer) */
    
      else if (memicmp(line, "between \"", 9)==0)	
      { char string[1024];
        char *p, *string2;
        int h1, h2, h;
	struct tm tmbuf;
        time_t t = time(NULL);

        localtime_r(&t, &tmbuf);
        h = tmbuf.tm_hour;
	
        strcpy(string, line+9);
        h1 = atoi(string);
        p = strrchr(string, '"');
        if (p)
        { *p=0;

          p = strstr(string, "\",\"");
          if (!p) p = strstr(string, "\", \"");
	  if (p) 
          { string2 = strchr(p+1, '"')+1;
            *p=0;
            h2 = atoi(string2);
            SystemDebug.printf("Exportconverter%d.%d executes: %s\n", N, part, line);
            if (h2>=h1 && (h <h1 || h>=h2)) rc = 4;	// between "9", "17": skip if h<9 or h>=17
            if (h2< h1 && (h <h1 && h>=h2)) rc = 4;	// between "17", "9": skip if h<=17 and h>=9
          }
        }
      }

      /* converter: prefetch */
  
      else if (memicmp(line, "prefetch", 8)==0)
      { if (prefetch_queue("prefetch", patid, "", "", "", "", "", "", "", "", "", 0, ""))
          OperatorConsole.printf("Exportconverter%d.%d: queued prefetch %s\n", N, part, patid);
      }
  
      /* converter: delayed "[forward|get] [patient|study|series|image] [after nn] [compressed as xx] 
                             [date yyyymmdd-yyyymmdd] [now -ddd+ddd] [age -ddd+ddd] 
                             [modality mm] [sop xxxx] [imagetype xxxx] [seriesdesc xxxx] [to/from AE]" 
         and: delayed "delete [patient|study|series|image] [date yyyymmdd-yyyymmdd] [now -ddd+ddd] 
			      [age -ddd+ddd] [modality mm] [sop xxxx] [imagetype xxxx] [seriesdesc xxxx]"
         and: delayed "process [patient|study|series|image] [by command]"
         and: delayed "merge study [modality mm] [after nn]"

         Will process data only from the current patient, but filters 'now, age, modality, sop, imagetype, seriesdesc' 
	 will read from other studies and series
      */
  
      else if (memicmp(line, "forward patient ",  16)==0  ||
               memicmp(line, "forward study ",    14)==0  ||
               memicmp(line, "forward series ",   15)==0  ||
               memicmp(line, "forward image ",    14)==0  ||
               memicmp(line, "get patient ",      12)==0  ||
               memicmp(line, "get study ",        10)==0  ||
               memicmp(line, "get series ",       11)==0  ||
               memicmp(line, "get image ",        10)==0  ||
               memicmp(line, "delete patient ",   15)==0  ||
               memicmp(line, "delete study ",     13)==0  ||
               memicmp(line, "delete series ",    14)==0  ||
               memicmp(line, "delete image ",     13)==0  ||
               memicmp(line, "delete patient",    15)==0  ||
               memicmp(line, "delete study",      12)==0  ||
               memicmp(line, "delete series",     13)==0  ||
               memicmp(line, "delete image",      12)==0  ||
               memicmp(line, "process patient ",  16)==0  ||
               memicmp(line, "process study ",    14)==0  ||
               memicmp(line, "process series ",   15)==0  ||
               memicmp(line, "process image ",    14)==0  ||
               memicmp(line, "merge study ",      12)==0  ||
               memicmp(line, "submit patient ",   15)==0  ||
               memicmp(line, "submit study ",     13)==0  ||
               memicmp(line, "submit series ",    14)==0  ||
               memicmp(line, "submit image ",     13)==0  ||
               memicmp(line, "submit2 patient ",  16)==0  ||
               memicmp(line, "submit2 study ",    14)==0  ||
               memicmp(line, "submit2 series ",   15)==0  ||
               memicmp(line, "submit2 image ",    14)==0)
      { char *p = strchr(line, ' ')+1; const char *compress="", *dest="", *date="", *modality="", *sop="", *imagetype="", *seriesdesc="", *script="";
        char studyuid[65], seriesuid[65], vr[200], dat[200];
        VR *pVR;
        int len, delay=0;

        studyuid[0]=0;
        seriesuid[0]=0;

        if (!DDO) 
          DDO = PDU2.LoadDICOMDataObject(pszFileName);

        const char *level=p;
        p = strchr(p, ' ');
        while(p)
        { *p=0;
          p++;
  
          if (memicmp(p, "compressed as ", 14)==0) 
          { compress = p+14; 
            p = strchr(p+14, ' '); 
          }
          else if (memicmp(p, "to ", 3)==0) 
          { dest = p+3; 
            p = strchr(p+3, ' '); 
          }
          else if (memicmp(p, "after ", 6)==0) 
          { delay = atoi(p+6); 
            p = strchr(p+6, ' '); 
          }
          else if (memicmp(p, "by ", 3)==0) 
          { dest = p+3; 
            p = NULL; // strchr(p+3, ' '); 
          }
          else if (memicmp(p, "from ", 5)==0) 
          { dest = p+5; 
            p = strchr(p+5, ' '); 
          }
          else if (memicmp(p, "date ", 5)==0) 	// all series date yyyymmdd or yyyymmdd-yyyymmdd
          { date = p+5; 
            p = strchr(p+5, ' '); 
            level = "selection";		// causes that no study or series UID are passed to query
          }
          else if (memicmp(p, "now ", 4)==0) 	// now -365+365: within one year from now (+/- compulsory)
	  { struct tm tmbuf1, tmbuf2;		// also works on all series within date range
	    char *q;
            int from=0, to=0;
	    from=atoi(p+4);
            q = strchr(p+5, '+'); if (!q) q = strchr(p+5, '-'); if (q) to=atoi(q);
		
	    time_t t1 = time(NULL)+24*3600*from;
	    time_t t2 = time(NULL)+24*3600*to;
            localtime_r(&t1, &tmbuf1);
            localtime_r(&t2, &tmbuf2);

	    sprintf(dat, "%04d%02d%02d-%04d%02d%02d", tmbuf1.tm_year+1900, tmbuf1.tm_mon, tmbuf1.tm_mday, tmbuf2.tm_year+1900, tmbuf2.tm_mon, tmbuf2.tm_mday);
	    date = dat;

            p = strchr(p+4, ' '); 
            level = "selection";		// causes that no study or series UID passed to query
          }
          else if (memicmp(p, "age ", 4)==0) 	// age -365+365: within one year from series date (+/- compulsory)
	  { struct tm tmbuf1, tmbuf2;		// also works on all series within date range
	    char *q;
            int from=0, to=0;
	    from=atoi(p+4);
            q = strchr(p+5, '+'); if (!q) q = strchr(p+5, '-'); if (q) to=atoi(q);

            SearchDICOMObject(DDO, "0008,0020", dat);
            memset(&tmbuf1, 0, sizeof(tmbuf1));
	    sscanf(dat, "%04d%02d%02d", &tmbuf1.tm_year, &tmbuf1.tm_mon, &tmbuf1.tm_mday);
            tmbuf1.tm_year -= 1900;

            time_t t = mktime(&tmbuf1);
	    time_t t1 = t+24*3600*from;
	    time_t t2 = t+24*3600*to;
            localtime_r(&t1, &tmbuf1);
            localtime_r(&t2, &tmbuf2);

	    sprintf(dat, "%04d%02d%02d-%04d%02d%02d", tmbuf1.tm_year+1900, tmbuf1.tm_mon, tmbuf1.tm_mday, tmbuf2.tm_year+1900, tmbuf2.tm_mon, tmbuf2.tm_mday);
	    date = dat;

            p = strchr(p+4, ' '); 
            level = "selection";		// causes that no study or series UID passed to query
          }
          else if (memicmp(p, "modality ", 9)==0) 
          { modality = p+9; 
            p = strchr(p+9, ' '); 
            level = "selection";		// causes that no study or series UID is passed to query
          }
          else if (memicmp(p, "sop ", 4)==0) 
          { sop = p+4; 
            p = strchr(p+4, ' '); 
            level = "selection";		// causes that no study or series UID is passed to query except the given SOP
          }
          else if (memicmp(p, "imagetype ", 10)==0) 
          { imagetype = p+10; 
            p = strchr(p+10, ' '); 
            level = "study";			// causes that no series UID is passed to query
          }
          else if (memicmp(p, "split ", 6)==0) 
          { imagetype = p+6; 
            p = strchr(p+6, ' '); 
          }
          else if (memicmp(p, "target ", 7)==0) // for DcmSubmit only
          { imagetype = p+7; 
            p = strchr(p+7, ' '); 
          }
          else if (memicmp(p, "seriesdesc ", 11)==0) 
          { seriesdesc = p+11; 
            p = strchr(p+11, ' '); 
            level = "study";			// causes that no series UID is passed to query
          }
          else if (memicmp(p, "password ", 9)==0) // for submit only
          { seriesdesc = p+9; 
            p = strchr(p+9, ' '); 
          }
          else if (memicmp(p, "command [", 9)==0) // for submit2 only
          { seriesdesc = p+9; 
            p = strchr(p+9, ']'); 
          }
          else if (memicmp(p, "command ", 8)==0) // for submit2 only
          { seriesdesc = p+8; 
            p = strchr(p+8, ' '); 
          }
          else if (memicmp(p, "script \"", 8)==0) 
          { if (p[strlen(p)-1]=='"') p[strlen(p)-1]=0;
	    script = p+8; 			// must be last parameter
	    p = NULL;
            break;
          }
          else if (memicmp(p, "script ", 7)==0) // script without ""
          { script = p+7; 			// must be last parameter
	    p = NULL;
            break;
          }
          else if (memicmp(p, "series ", 7)==0) 
          { char *a=p+7;
            p = strchr(p+7, ' ');
	    *p=0;
	    strcpy(seriesuid, a);
            level = "selection";		// causes that no study UID is passed and given series
          }
          else if (memicmp(p, "study ", 6)==0) 
          { char *a=p+6;
            p = strchr(p+6, ' ');
            *p=0;
	    strcpy(studyuid, a);
            level = "selection";		// causes that no series UID is passed and give study
          }
        }

        if (DDO)
        { pVR = DDO->GetVR(0x0020, 0x000d);
          if (pVR && (memicmp(level, "stu", 3)==0 || memicmp(level, "ser", 3)==0) || memicmp(level, "ima", 3)==0)
          { strncpy(vr, (char*)pVR->Data, pVR->Length);
            vr[pVR->Length] = 0;
            len = pVR->Length - 1;
            while(len>0)
            { if (vr[len] == ' ')
                vr[len] = 0;
              else
                break;
              len--;
            }
            strcpy(studyuid, vr);
          }

          pVR = DDO->GetVR(0x0020, 0x000e);
          if (pVR && (memicmp(level, "ser", 3)==0 || memicmp(level, "ima", 3)==0))
          { strncpy(vr, (char*)pVR->Data, pVR->Length);
            vr[pVR->Length] = 0;
            len = pVR->Length - 1;
            while(len>0)
            { if (vr[len] == ' ')
                vr[len] = 0;
              else
                break;
              len--;
            }
            strcpy(seriesuid, vr);
          }

          pVR = DDO->GetVR(0x0008, 0x0018);
          if (pVR && memicmp(level, "ima", 3)==0)
          { strncpy(vr, (char*)pVR->Data, pVR->Length);
            vr[pVR->Length] = 0;
            len = pVR->Length - 1;
            while(len>0)
            { if (vr[len] == ' ')
                vr[len] = 0;
              else
                break;
              len--;
            }
            sop = vr;
          }
        }

	line[11]=0;
	if (sop) level = "single object";
	if (DDO)
	  if (prefetch_queue(line, patid, dest, studyuid, seriesuid, compress, modality, date, sop, imagetype, seriesdesc, delay, script))
            OperatorConsole.printf("Exportconverter%d.%d: queued %s - (%s %s %s of %s) to %s\n", N, part, line, level, modality, date, patid, dest);
      }
  
      /* converter: preretrieve */
  
      else if (memicmp(line, "preretrieve ", 12)==0)
      { if (prefetch_queue("preretrieve", patid, line+12, "", "", "", "", "", "", "", "", 0, ""))
          OperatorConsole.printf("Exportconverter%d.%d: queued preretrieve of patient %s from %s\n", N, part, patid, line+12);
      }
  
      /* converter: system (command line can be generated using all % tricks) */

      else if (memicmp(line, "system ", 7)==0)
      { char cline[512];
        strcpy(cline, line+7);
        system(cline);
        //BackgroundExec(cline, "");
	SystemDebug.printf("Exportconverter%d.%d: %s\n", N, part, line);
      }

      /* converter: mkdir (command line can be generated using all % tricks) */

      else if (memicmp(line, "mkdir ", 6)==0)
      { char cline[512], s[512];
        strcpy(cline, line+6);
	for (int sIndex = 0; sIndex<=strlen(cline); sIndex++)
	  if (cline[sIndex]==PATHSEPCHAR)
	  { strcpy(s, cline);
	    s[sIndex]='\0';
	    mkdir(s);
	  }
        mkdir(cline); // 20120829
	SystemDebug.printf("Exportconverter%d.%d: %s\n", N, part, line);
      }

      /* converter: rm (command line can be generated using all % tricks) */

      else if (memicmp(line, "rm ", 3)==0)
      { char cline[512];
        strcpy(cline, line+3);
        unlink(cline);
      	SystemDebug.printf("Exportconverter%d.%d: %s\n", N, part, line);
      }

      else if (memicmp(line, "testmode ", 9)==0)
      { OperatorConsole.printf("Exportconverter%d.%d: sets testmode to %s\n", N, part, line+9);
        strcpy(TestMode, line+9);
      }

      /* converter: nop with % item */

      else if (memicmp(line, "nop", 3)==0)
      { OperatorConsole.printf("Exportconverter%d.%d: %s\n", N, part, line);
      }

      else if (memicmp(line, "stop", 4)==0)
      { SystemDebug.printf("Exportconverter%d.%d: stop\n", N, part);
        rc = 3;	
      }

      else if (memicmp(line, "return", 6)==0)
      { SystemDebug.printf("Exportconverter%d.%d: return\n", N, part);
        rc = 5;	
      }

      else if (memicmp(line, "call ", 5)==0)
      { FILE *f;
        int ret;
        char cmd[1024];
        
	if (!DDO) DDO = PDU2.LoadDICOMDataObject(pszFileName);

        cmd[0]=0;
        MyGetPrivateProfileString ( "scripts", line+5, cmd, cmd, 1024, ConfigFile);
        if (*cmd)
          ret = CallImportConverterN(NULL, DDO, -1, NULL, NULL, NULL, NULL, PDU, NULL, cmd);
        else
        { f = fopen(line+5, "rt");
          if (f)
          { SystemDebug.printf("%sconverter%d.%d: %s\n", "Export", N, part, line);
            while(fgets(cmd, sizeof(cmd), f) != NULL)
            { if (cmd[strlen(cmd)-1]=='\n') cmd[strlen(cmd)-1]=0;
	      if (cmd[0]!='#' && cmd[0]!=';')
	      { // ret = CallImportConverterN(NULL, DDO, -1, NULL, NULL, NULL, NULL, NULL, NULL, Storage, cmd, vars);
	        ret = CallImportConverterN(NULL, DDO, -1, NULL, NULL, NULL, NULL, PDU, NULL, cmd);
                if (ret==5 || ret==2 || ret==6) break; // return or destroy or reject
	      }
            }
	    fclose(f);
          }
          else
            OperatorConsole.printf("*** %sconverter%d.%d script not found: %s\n", "Export", N, part, line+5);
        }
        rc = ret;
      }

      else if (memicmp(line, "silentstop", 10)==0)
      { rc = 3;	
      }

      else if (memicmp(line, "defer", 5)==0)
      { SystemDebug.printf("Exportconverter%d.%d: defer\n", N, part);
        reset_queue_fails(N);
        if (DDO) delete DDO;
        return TRUE; // retry later
      }

      /* converter: prefetch */

      else if (memicmp(line, "prefetch", 8)==0)
      { if (prefetch_queue("prefetch", patid, "", "", "", "", "", "", "", "", "", 0, ""))
          OperatorConsole.printf("Exportconverter%d.%d: queued prefetch %s\n", N, part, patid);
      }

      /* default: execute the command */

      else if (strchr(szExecName, '%'))
      {
        OperatorConsole.printf("Exportconverter%d.%d executes: %s\n", N, part, line);
  
  #ifdef WIN32
        WinExec(line, SW_MINIMIZE);     
  #else
  //      char *s=strchr(line, ' ');
  //      *s=0;
  //      BackgroundExec(line, s+1);
        system(line);
  #endif
        ImagesExported++;
      }

      /* for backwards compatibility: execute executable with single parameter */
  
      else
#ifdef WIN32
      { int r = spawnlpe(_P_WAIT, szExecName,
                    szExecName,		/* argv[0] */
                    pszFileName,		/* argv[1] */
                    NULL,			/* Allways NULL */
                    NULL);		/* Environment */
        if (r == -1)
          OperatorConsole.printf("***Exportconverter%d: Spawning '%s' failed (argv=%s)\n", N, szExecName, pszFileName);
        ImagesExported++;
      }
#else
  //    BackgroundExec(szExecName, pszFileName);
      { char line1[1000];
        sprintf(line1, "%s %s", szExecName, pszFileName);
        system(line1);
        ImagesExported++;
      }   
#endif
    }

    // process next command
    if (szNext[0]==0) break;
    strcpy(szExecName, szNext);
    part++;

    if (rc==3 || rc==5) break;
  }

  if (DDO) delete DDO;
  return FALSE;
}

/*
void CallExportConverters(char *pszFileName, char *pszModality, char *pszStationName, char *pszSop, char *patid, char *calling, char *called)
{ char		szRootSC[64];
  char		szTemp[32];
  int		i, iNbConverters;

  if (!MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig,
		szRootSC, 64, ConfigFile)) return;
  if (!MyGetPrivateProfileString(szRootSC, "ExportConverters", "0",
		szTemp, 128, ConfigFile)) return;
  iNbConverters = atoi(szTemp);
  if (iNbConverters>MAXExportConverters) iNbConverters=MAXExportConverters;

  // Loop over all converters
  for(i=0; i<iNbConverters; i++)
    CallExportConverterN(pszFileName, i, pszModality, pszStationName, pszSop, patid, calling, called, NULL, NULL);
}
*/

/////////////////////////////////////////////////////////////////////////////////////////////////
// lua integration
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef LUA51EXTERN
#include "lua.hpp"
#endif

char *heapinfo( void );
char *DcmMove2(char* pszSourceAE, const char* pszDestinationAE, BOOL patroot, int id, DICOMDataObject *DDO, DICOMDataObject * DCOextra, lua_State *L=NULL);
DICOMDataObject *dummyddo;

static int SendServerCommand(const char *NKIcommand1, const char *NKIcommand2, int con, char *buf=NULL, BOOL html=TRUE, BOOL upload=FALSE, lua_State *L=NULL);

#ifndef VirtualQuery
int VirtualQuery(DICOMDataObject *DDO, const char *Level, int N, Array < DICOMDataObject  *> *pADDO, char *ae=NULL);
#endif

#ifdef LUA51EXTERN
#include "lua_dyn.h"
lua_All_functions LuaFunctions;

int loadLua()
{
	/* Load lua.dll dynamically */
#ifdef WIN32
	HMODULE module = LoadLibrary("lua5.1.dll");
#else
	void* module = dlopen("liblua5.1.so", RTLD_LAZY);
#endif
	if(!luaL_loadfunctions(module, &LuaFunctions, sizeof(LuaFunctions)))
	{
		printf("Error loading Lua DLL!\n");
		return (1);
	}
	return 0;
}
#endif

extern "C"
{ struct scriptdata *getsd(lua_State *L)
  { lua_getglobal(L, "scriptdata");
    struct scriptdata *sd = (struct scriptdata *)lua_topointer(L, -1);
    lua_pop(L, 1);
    return sd;
  }
  static void luaCreateObject(lua_State *L, DICOMDataObject *pDDO, Array < DICOMDataObject *> *A, BOOL owned);
  
  static int _luaprint(lua_State *L, Debug *d)
  { int i, n=lua_gettop(L);
    char text[2000]; text[0]=0;
    for (i=1; i<=n; i++)
    { if (i>1)                     sprintf(text + strlen(text), "\t");
      if (lua_isstring(L,i))       sprintf(text + strlen(text), "%s",lua_tostring(L,i));
      else if (lua_isnil(L,i))     sprintf(text + strlen(text), "%s","nil");
      else if (lua_isboolean(L,i)) sprintf(text + strlen(text), "%s",lua_toboolean(L,i) ? "true" : "false");
      else                         sprintf(text + strlen(text), "%s:%p",luaL_typename(L,i),lua_topointer(L,i));
    }
    d->printf("%s\n", text);
    return 0;
  }
  static int luaprint(lua_State *L)    { return _luaprint(L, &OperatorConsole); }
  static int luadebuglog(lua_State *L) { return _luaprint(L, &SystemDebug);     }

int console;

  static int luaservercommand(lua_State *L)
  { int n=lua_gettop(L);
    lua_State *L2=NULL;
    char *b = NULL; // buffer to SendServerCommand (only used to pass filename for upload)
    char *t = NULL; // mode
    int c   = 0;     // filehandle or console
    int rc = 1;
    BOOL html=FALSE; // convert to html
    BOOL upload=FALSE; // upload
    BOOL download=FALSE; // download
    if (lua_isstring(L,2)) 
    { t = (char *)lua_tostring(L,2);
      if      (strcmp(t, "cgi"      )==0) {c=console; }
      else if (strcmp(t, "cgibinary")==0) {c=console; }
      else if (strcmp(t, "cgihtml"  )==0) {c=console; html=TRUE;}
      else if (t[0]=='<') {b=t+1; html=FALSE; upload=TRUE; L2=L;}
      else if (t[0]=='>') {c=open(t+1, O_CREAT | O_TRUNC | O_BINARY | O_RDWR, 0666); html=FALSE; download=FALSE;}
      else if (strcmp(t, "binary"   )==0) {b="binary"; L2=L;}
      else { L2=L; }
    }
    else
      L2=L;
    if (lua_isstring(L,1)) 
      rc = SendServerCommand("", lua_tostring(L,1), c, b, html, upload, L2);
    if (download) close(c);
    if (L2 && rc==-1) return 1;
    return 0;
  }

  static int luadictionary(lua_State *L)
  { if (lua_gettop(L)==2)
    { int g = lua_tointeger(L,1);
      int e = lua_tointeger(L,2);
      char s[256]; s[0] = '\0';
      if (VRType.RunTimeClass(g, e, s))
      { lua_pushstring(L, s);
      	return 1;
      }
    }
    else if (lua_gettop(L)==1)
    { RTCElement Entry;
      char s[256];
      strcpy(s, lua_tostring(L, 1));
      Entry.Description = s;
      if (VRType.GetGroupElement(&Entry))
      { lua_pushinteger(L, Entry.Group);
        lua_pushinteger(L, Entry.Element);
        return 2;
      }
    }
    return 0;
  }
  static int luachangeuid(lua_State *L)
  { if (lua_gettop(L)==1)
    { char from[256], to [256];
      strcpy(from, lua_tostring(L, 1));
      if (ChangeUID(from, "lua", to, NULL, NULL))
      { lua_pushstring(L, to);
        return 1;
      }
    }
    if (lua_gettop(L)==2)
    { char from[256], to[256], result[256];
      strcpy(from, lua_tostring(L, 1));
      strcpy(to, lua_tostring(L, 2));
      if (ChangeUIDTo(from, "lua", to, NULL, NULL))
      { lua_pushstring(L, to);
        return 1;
      }
    }
    if (lua_gettop(L)>=3)
    { char from[256], to[256], stage[256], result[256], type[256];
      strcpy(from, lua_tostring(L, 1));
      strcpy(to, lua_tostring(L, 2));
      strcpy(stage, lua_tostring(L, 3));
      strcpy(type, "lua");
      if (lua_gettop(L)>=4)
        strcpy(type, lua_tostring(L, 4));
      if (ChangeUIDTo(from, type, to, stage, NULL))
      { lua_pushstring(L, to);
        return 1;
      }
    }
    lua_pushnil(L);
    return 1;
  }
  static int luachangeuidback(lua_State *L)
  { if (lua_gettop(L)==1)
    { char from[256], to [256];
      strcpy(from, lua_tostring(L, 1));
      if (ChangeUIDBack(from, to, NULL, NULL, NULL))
      { lua_pushstring(L, to);
        return 1;
      }
    }
    if (lua_gettop(L)==2)
    { char from[256], to [256], stage[256];
      strcpy(from, lua_tostring(L, 1));
      strcpy(stage, lua_tostring(L, 2));
      if (ChangeUIDBack(from, to, stage, NULL, NULL))
      { lua_pushstring(L, to);
        return 1;
      }
    }
    if (lua_gettop(L)==3)
    { char from[256], to [256], stage[256], type[256];
      strcpy(from, lua_tostring(L, 1));
      strcpy(stage, lua_tostring(L, 2));
      strcpy(type, lua_tostring(L, 3));
      if (ChangeUIDBack(from, to, stage, type, NULL))
      { lua_pushstring(L, to);
        return 1;
      }
    }
    lua_pushnil(L);
    return 1;
  }
  static int luacrc(lua_State *L)
  { if (lua_gettop(L)==1)
    { char from[256];
      strcpy(from, lua_tostring(L, 1));
      unsigned int crc = ComputeCRC(from, strlen(from));
      sprintf(from, "%u", crc);
      lua_pushstring(L, from);
      return 1;
    }
    lua_pushnil(L);
    return 1;
  }
  //extern void MD5_compute(char *in, char *out);
  static int luamd5(lua_State *L)
  { if (lua_gettop(L)==1)
    { unsigned char result[16];
      char s[33];
      MD5_compute((char *)lua_tostring(L, 1), result);
      for (int i=0; i<16; i++) sprintf(s+i*2, "%02x", result[i]);
      lua_pushstring(L, s);
      return 1;
    }
    lua_pushnil(L);
    return 1;
  }  
  static int luagenuid(lua_State *L)
  { char n[80];
    GenUID(n);
    lua_pushstring(L, n);
    return 1;
  }
  static int luascript(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    int n=lua_gettop(L);
    char cmd[2000]="";
    DICOMDataObject *pDDO;
    DICOMCommandObject *pDCO;
    pDDO = sd->DDO;
    pDCO = sd->DCO;
    if (n>0)
    { if (lua_isuserdata(L, 1)) 
      { lua_getmetatable(L, 1);
          lua_getfield(L, -1, "DDO");  pDDO = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
        lua_pop(L, 1);
      }
      else if (lua_isstring(L,1)) 
      	strcpy(cmd, lua_tostring(L,1));
    }
    if (n>1 && lua_isstring(L,2)) 
      strcpy(cmd, lua_tostring(L,2));
    sd->rc = CallImportConverterN(pDCO, pDDO, -2, sd->pszModality, sd->pszStationName, sd->pszSop, sd->patid, sd->PDU, sd->Storage, cmd);
    return 0;
  }
  static int luadestroy(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    sd->rc = 2;
    return 0;
  }
  static int luareject(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    sd->rc = 6;
    return 0;
  }
  static int luaretry(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    sd->rc = 7;
    return 0;
  }

void export_queueN(struct conquest_queue **q, char *pszFileName, int N, char *pszModality, char *pszStationName, char *pszSop, char *patid, char *calling, char *called, char *script=NULL);
static struct conquest_queue **sqN = NULL;
static ExtendedPDU_Service ScriptForwardPDU[1][MAXExportConverters];	// max 20*20 with remaining association

  static int luaexport(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    int n=lua_gettop(L);
    char cmd[2000]="";
    DICOMDataObject *pDDO;
    pDDO = sd->DDO;
    if (n>0)
    { if (lua_isuserdata(L, 1)) 
      { lua_getmetatable(L, 1);
          lua_getfield(L, -1, "DDO");  pDDO = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
        lua_pop(L, 1);
      }
      else if (lua_isstring(L,1)) 
      	strcpy(cmd, lua_tostring(L,1));
    }
    if (n>1 && lua_isstring(L,2)) 
      strcpy(cmd, lua_tostring(L,2));
    export_queueN(sqN, "", -1, NULL, NULL, NULL, NULL, NULL, NULL, cmd);
    return 0;
  }
  static int luaget_amap(lua_State *L)
  { int r = lua_tointeger(L, 1);
    if (r < ACRNemaAddressArray.GetSize())
    { ACRNemaAddress	*AAPtr = ACRNemaAddressArray.Get(r);
      lua_pushstring(L, AAPtr->Name);
      lua_pushstring(L, AAPtr->IP);
      lua_pushstring(L, AAPtr->Port);
      lua_pushstring(L, AAPtr->Compress);
      return 4;
    }
    return 0;
  }
  static int luaget_sqldef(lua_State *L)
  { int r = lua_tointeger(L, 1);
    int s = lua_tointeger(L, 2);
    DBENTRY *DBE = NULL;
    if (r==0) DBE = PatientDB;
    if (r==1) DBE = StudyDB;
    if (r==2) DBE = SeriesDB;
    if (r==3) DBE = ImageDB;
    if (r==4) DBE = WorkListDB;
    if (DBE)
    { if (DBE[s].Group)
      { lua_pushinteger(L, DBE[s].Group);
        lua_pushinteger(L, DBE[s].Element);
	lua_pushstring(L, DBE[s].SQLColumn);
	lua_pushinteger(L, DBE[s].SQLLength);
	lua_pushstring(L, SQLTypeSymName(DBE[s].SQLType));
	lua_pushstring(L, DICOMTypeSymName(DBE[s].DICOMType));
	return 6;
      }
    }
    return 0;
  }

  static int luadbquery(lua_State *L)
  { unsigned int i, N, flds=1;
    const char *items[4];
    SQLLEN SQLResultLength;
    for (i=0; i<4; i++) items[i]=lua_tostring(L,i+1);
    if (items[1]) for (i=0; i<strlen(items[1]); i++) if (items[1][i]==',') flds++;
    if (items[2]) if (*items[2]==0) items[2]=NULL;
    if (items[3]) if (*items[3]==0) items[3]=NULL;

    Database DB;
    if (DB.Open ( DataSource, UserName, Password, DataHost ) )
    { char fld[48][256]; memset(fld, 0, sizeof(fld));
      if (DB.Query(items[0], items[1], items[2], items[3]))
      { for (i=0; i<flds; i++) DB.BindField (i+1, SQL_C_CHAR, fld[i], 255, &SQLResultLength);
        N = 1; lua_newtable(L);
        while (DB.NextRecord())
        { lua_newtable(L);
          for (i=0; i<flds; i++) { lua_pushstring(L, fld[i]); lua_rawseti(L, -2, i+1); }
	  lua_rawseti(L, -2, N++);
	  memset(fld, 0, sizeof(fld));
        }
        DB.Close();
        QueryFromGui++;
        return 1;
      }
      return 0;
    }
    return 0;
  }

  static int luasql(lua_State *L)
  { const char *sql = lua_tostring(L,1);
    BOOL f=FALSE;

    Database DB;
    if (DB.Open ( DataSource, UserName, Password, DataHost ) )
    { f = DB.Exec(sql);
      DB.Close();
      QueryFromGui++;
    }
    if (f)
    { lua_pushinteger(L, 1);
      return 1;
    }
    else
      return 0;
  }

  static int luadicomquery2(lua_State *L)
  { const char *ae    = lua_tostring(L,1);
    const char *level = lua_tostring(L,2);
    if (lua_isuserdata(L, 3)) 
    { DICOMDataObject *O = NULL;
      lua_getmetatable(L, 3);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);
      Array < DICOMDataObject * > *A = new Array < DICOMDataObject * >;
      luaCreateObject(L, NULL, A, TRUE); 
      if (O) 
      { DICOMDataObject *P = MakeCopy(O);
        VirtualQuery(P, level, 0, A, (char *)ae);
        delete P;
      }
      return 1;
    }
    return 0;
  }
  
  static int luadicomread(lua_State *L)
  { if (lua_isuserdata(L, 1)) 
    { int limit=lua_tointeger(L, 2);
      DICOMDataObject *O = NULL;
      lua_getmetatable(L, 1);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);
      Array < DICOMDataObject * > *A = new Array < DICOMDataObject * >;
      if (O) 
      { DICOMDataObject *P = MakeCopy(O);
        VirtualQuery(P, "IMAGE", 0, A, (char *)MYACRNEMA);
        delete P;
        int N = A->GetSize();
        for (int i=0; i<N; i++)
        { char fn[100];
          memset(fn , 0, 100);
          fn[0]=':';
	  VR *vr = A->Get(i)->GetVR(0x0008, 0x0018);
	  memcpy(fn+1, (char *)vr->Data, vr->Length);
	  DICOMDataObject *DDO = LoadForGUI(fn, limit);
	  if (limit && !DDO->GetUINT16(0x0028, 0x0010)) // only limit images, check or Rows
	  { delete DDO;
            DDO = LoadForGUI(fn);
	  }
	  MaybeScrub(DDO, (DICOMCommandObject *)O);
          A->Add(DDO);
        }
        for (int i=0; i<N; i++)
	  A->RemoveAt(0);
        luaCreateObject(L, NULL, A, TRUE); 
        return 1;
      }
    }
    return 0;
  }

  //int MakeListOfOldestPatientsOnDevice(char **PatientIDList, int Max, const char *Device, char *Sort);
  static int lualistoldestpatients(lua_State *L)
  { int max = lua_tointeger(L, 1);
    const char *Device = lua_tostring(L,2);
    char *Sort   = (char *)lua_tostring(L,3);
    char *PatientIDList;
    int total = MakeListOfOldestPatientsOnDevice(&PatientIDList, max, Device, Sort);
    lua_newtable(L);
    for (int i=0; i<total; i++) 
    { lua_pushstring(L, PatientIDList+i*256); 
      lua_rawseti(L, -2, i+1); 
    }
    free(PatientIDList);
    return 1;
  }

  static int luadicomquery(lua_State *L);
  static int luadicomprint(lua_State *L);
  static int luadicomget(lua_State *L);
  static int luadicomstore(lua_State *L);
  static int luatickcount(lua_State *L);
  static int luadicomecho(lua_State *L);
  
  static int luadicommove(lua_State *L)
  { const char *source = lua_tostring(L,1);
    const char *dest   = lua_tostring(L,2);
    DICOMDataObject *DCOextra = NULL;
    DICOMDataObject *O = NULL;
      
    if (lua_isuserdata(L, 5)) 
    { lua_getmetatable(L, 5);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);
      DCOextra = MakeCopy(O);
    }
    if (lua_isuserdata(L, 3)) 
    { lua_getmetatable(L, 3);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);
      if (O)
      { DICOMDataObject *P = MakeCopy(O);
        char *r = DcmMove2((char *)source, dest, lua_tointeger(L,4), 0x555, P, DCOextra, L);
        delete P;
	if (DCOextra) delete DCOextra;
	if (r[0])
	{ lua_pushstring(L, r);
	  return 1;
	}
	return 0;
      }
    }
    return 0;
  }
  
  static int luadicomdelete(lua_State *L)
  { if (lua_isuserdata(L, 1)) 
    { DICOMDataObject *O = NULL;
      lua_getmetatable(L, 1);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);
      if (O) 
      { DICOMDataObject *P = MakeCopy(O);
	RemoveFromPACS(P, FALSE);
        delete P;
      }
      lua_pushboolean(L, true);
      return 1;
    }
    return 0;
  }

  static int luanewdicomdelete(lua_State *L)
  { if (lua_isuserdata(L, 1)) 
    { DICOMDataObject *O = NULL;
      lua_getmetatable(L, 1);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);
      if (O) NewDeleteDICOM(O, lua_tonumber(L, 2));
    }
    return 0;
  }

  static int luanewdicommodify(lua_State *L)
  { if (lua_isuserdata(L, 1)) 
    { DICOMDataObject *O = NULL;
      lua_getmetatable(L, 1);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);
      if (O) NewModifyDICOM(O, lua_tostring(L, 2), lua_tonumber(L, 3), FALSE);
    }
    return 0;
  }

  static int luanewdicomcopy(lua_State *L)
  { if (lua_isuserdata(L, 1)) 
    { DICOMDataObject *O = NULL;
      lua_getmetatable(L, 1);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);
      if (O) NewModifyDICOM(O, lua_tostring(L, 2), lua_tonumber(L, 3), TRUE);
    }
    return 0;
  }

  static int getptr(lua_State *L, unsigned int *ptr, int *veclen, int *bytes, int *count, int *step, VR **vr, int mode)
  { struct scriptdata *sd = getsd(L); //mode:0,1,2,3=pixel,row,col,image
    int pars=lua_gettop(L), n, x=0, y=0, z=0, rows, cols, frames, vec;
    Array < DICOMDataObject *> *pADDO;
    DICOMDataObject *pDDO;
    *veclen = 0;
    n=1;
    pDDO = sd->DDO;
      
    if (pars>0)
    { if (lua_isuserdata(L, n)) 
      { lua_getmetatable(L, n);
          lua_getfield(L, -1, "DDO");  pDDO = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
        lua_pop(L, 1);
        n++; pars--;
      }
    }

    if (pars>0 && (mode==0 || mode==2)) x = lua_tointeger(L,n++), pars--;
    if (pars>0 && (mode==0 || mode==1)) y = lua_tointeger(L,n++), pars--;
    if (pars>0                        ) z = lua_tointeger(L,n++), pars--;

    vec   = pDDO->GetUINT16(0x0028,0x0002);
    rows  = pDDO->GetUINT16(0x0028, 0x0010); if (!rows) return 0;
    cols  = pDDO->GetUINT16(0x0028, 0x0011); if (!cols) return 0;
    if (pDDO->GetUINT16(0x0028, 0x0006)>1)              return 0;
    frames= pDDO->Getatoi(0x0028, 0x0008);
    *bytes = (pDDO->GetUINT16(0x0028, 0x0101)+7)/8;
    *ptr   = (y * cols + x) * vec;
    DecompressNKI(pDDO);
    *vr    = pDDO->GetVR(0x7fe0, 0x0010); if (!*vr)   return 0;
    pADDO = (Array<DICOMDataObject*>*)(*vr)->SQObjectArray;
    if (pADDO==NULL)
    { *ptr += z*rows * cols * vec;
      if (*ptr * *bytes > (*vr)->Length) return 0;
    }
    else
    { if (z>=pADDO->GetSize()-1) return 0;
      *vr = pADDO->Get(z+1)->GetVR(0x7fe0, 0x0010);
      if (*vr==NULL) return 0;
      if (*ptr * *bytes > (*vr)->Length) return 0;
    }
    if (mode!=2) *step  = 1;   else *step = cols;
    if (mode>=2) *count= rows; else *count = cols;
    if (mode==3) *count *= cols;
    *veclen = vec;
    return n;
  }
  static int luagetpixel(lua_State *L)
  { int veclen, count, step, bytes;
    unsigned int ptr;
    VR *vr;
    getptr(L, &ptr, &veclen, &bytes, &count, &step, &vr, 0);
    if (veclen==0) return 0;
    switch(bytes)
    { case 1: for (int i=0; i<veclen; i++) lua_pushinteger(L, *((BYTE   *)(vr->Data)+ptr+i*step)); break;
      case 2: for (int i=0; i<veclen; i++) lua_pushinteger(L, *((UINT16 *)(vr->Data)+ptr+i*step)); break;
      case 4: for (int i=0; i<veclen; i++) lua_pushinteger(L, *((INT32  *)(vr->Data)+ptr+i+step)); break;
      default:for (int i=0; i<veclen; i++) lua_pushinteger(L,                                  0); break;
    }
    return veclen;
  }
  static int luasetpixel(lua_State *L)
  { int pars=lua_gettop(L), veclen, count, step, bytes, n;
    unsigned int ptr;
    VR *vr;
    n=getptr(L, &ptr, &veclen, &bytes, &count, &step, &vr, 0);
    if (veclen==0) return 0;
    if (veclen>pars-n+1) veclen=pars-n+1;
    switch(bytes)
    { case 1: for (int i=0; i<veclen; i++) *((BYTE   *)(vr->Data)+ptr+i*step) = lua_tointeger(L,n++); break;
      case 2: for (int i=0; i<veclen; i++) *((UINT16 *)(vr->Data)+ptr+i*step) = lua_tointeger(L,n++); break;
      case 4: for (int i=0; i<veclen; i++) *((INT32  *)(vr->Data)+ptr+i*step) = lua_tointeger(L,n++); break;
    }
    return 0;
  }
  static int luagetrow(lua_State *L)
  { int veclen, count, step, bytes;
    unsigned int ptr;
    VR *vr;
    getptr(L, &ptr, &veclen, &bytes, &count, &step, &vr, 1);
    if (veclen==0) return 0;
    lua_newtable(L);
    for(int x=0; x<count; x++)
    { switch(bytes)
      { case 1: for (int i=0; i<veclen; i++) {lua_pushinteger(L, *((BYTE   *)(vr->Data)+ptr+i)); lua_rawseti(L, -2, x*veclen+i);} break;
        case 2: for (int i=0; i<veclen; i++) {lua_pushinteger(L, *((UINT16 *)(vr->Data)+ptr+i)); lua_rawseti(L, -2, x*veclen+i);} break;
        case 4: for (int i=0; i<veclen; i++) {lua_pushinteger(L, *((INT32  *)(vr->Data)+ptr+i)); lua_rawseti(L, -2, x*veclen+i);} break;
      }
      ptr += step*veclen;
    }
    return 1;
  }
  static int luagetcolumn(lua_State *L)
  { int veclen, count, step, bytes;
    unsigned int ptr;
    VR *vr;
    getptr(L, &ptr, &veclen, &bytes, &count, &step, &vr, 2);
    if (veclen==0) return 0;
    lua_newtable(L);
    for(int x=0; x<count; x++)
    { switch(bytes)
      { case 1: for (int i=0; i<veclen; i++) {lua_pushinteger(L, *((BYTE   *)(vr->Data)+ptr+i)); lua_rawseti(L, -2, x*veclen+i);} break;
        case 2: for (int i=0; i<veclen; i++) {lua_pushinteger(L, *((UINT16 *)(vr->Data)+ptr+i)); lua_rawseti(L, -2, x*veclen+i);} break;
        case 4: for (int i=0; i<veclen; i++) {lua_pushinteger(L, *((INT32  *)(vr->Data)+ptr+i)); lua_rawseti(L, -2, x*veclen+i);} break;
      }
      ptr += step*veclen;
    }
    return 1;
  }
  static int luasetrow(lua_State *L)
  { int pars=lua_gettop(L), veclen, count, step, bytes, n;
    unsigned int ptr;
    VR *vr;
    n=getptr(L, &ptr, &veclen, &bytes, &count, &step, &vr, 1);
    if (veclen==0 || n>pars) return 0;
    for(int x=0; x<count; x++)
    { switch(bytes)
      { case 1: for (int i=0; i<veclen; i++) {lua_rawgeti(L, pars, x*veclen+i); *((BYTE   *)(vr->Data)+ptr+i) = lua_tointeger(L,-1); lua_pop(L, 1); } break;
	case 2: for (int i=0; i<veclen; i++) {lua_rawgeti(L, pars, x*veclen+i); *((UINT16 *)(vr->Data)+ptr+i) = lua_tointeger(L,-1); lua_pop(L, 1); } break;
	case 4: for (int i=0; i<veclen; i++) {lua_rawgeti(L, pars, x*veclen+i); *((INT32  *)(vr->Data)+ptr+i) = lua_tointeger(L,-1); lua_pop(L, 1); } break;
      }
      ptr += step*veclen;
    }
    return 0;
  }
  static int luasetcolumn(lua_State *L)
  { int pars=lua_gettop(L), veclen, count, step, bytes, n;
    unsigned int ptr;
    VR *vr;
    n=getptr(L, &ptr, &veclen, &bytes, &count, &step, &vr, 2);
    if (veclen==0 || n>pars) return 0;
    for(int x=0; x<count; x++)
    { switch(bytes)
      { case 1: for (int i=0; i<veclen; i++) {lua_rawgeti(L, pars, x*veclen+i); *((BYTE   *)(vr->Data)+ptr+i) = lua_tointeger(L,-1); lua_pop(L, 1); } break;
	case 2: for (int i=0; i<veclen; i++) {lua_rawgeti(L, pars, x*veclen+i); *((UINT16 *)(vr->Data)+ptr+i) = lua_tointeger(L,-1); lua_pop(L, 1); } break;
	case 4: for (int i=0; i<veclen; i++) {lua_rawgeti(L, pars, x*veclen+i); *((INT32  *)(vr->Data)+ptr+i) = lua_tointeger(L,-1); lua_pop(L, 1); } break;
      }
      ptr += step*veclen;
    }
    return 0;
  }
  static int luagetimage(lua_State *L)
  { int veclen, count, step, bytes;
    unsigned int ptr;
    VR *vr;
    getptr(L, &ptr, &veclen, &bytes, &count, &step, &vr, 3);
    if (veclen==0) return 0;
    lua_pushlstring(L, (char *)(vr->Data)+ptr, count*bytes);
    return 1;
  }
  static int luasetimage(lua_State *L)
  { int pars=lua_gettop(L), veclen, count, step, bytes, n;
    size_t len; unsigned int ptr;
    VR *vr; const char *data;
    n=getptr(L, &ptr, &veclen, &bytes, &count, &step, &vr, 3);
    if (veclen==0 || n>pars) return 0;
    data = lua_tolstring (L, n, &len);
    if (data && vr->Length!=count*bytes) 
      vr->ReAlloc(count*bytes);
    if (data && len==count*bytes)
      memcpy((BYTE *)(vr->Data)+ptr, (BYTE *)data, len);
    else if (data && bytes==2 && len==2*count*bytes)
    { float scale=0; 
      if(pars>n) scale=lua_tonumber(L, n+1);
      else       scale = 1;
      for(int i=0; i<count; i++)
        ((short *)(vr->Data))[i] = scale * ((float *)data)[i];
    }
    else
    { luaL_error(L, "Failed to set image, data and image dimensions incompatible");
    }
    return 0;
  }
    
  // getvr(Object, group, element) returns table for normal VR, addo for sequence
  static int luagetvr(lua_State *L)
  { VR *vr;
    if (lua_isuserdata(L,1)) 
    { DICOMDataObject *O = NULL;
      lua_getmetatable(L, 1);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);
      int g = lua_tointeger(L,2);
      int e = lua_tointeger(L,3);
      int b = lua_toboolean(L,4);
      vr = O->GetVR(g, e);
      if (vr)
      { if (vr->SQObjectArray)
          luaCreateObject(L, NULL, (Array < DICOMDataObject * > *)vr->SQObjectArray, FALSE);
        else if (b==0)
        { UINT16 TypeCode = VRType.RunTimeClass(g, e, NULL);
          lua_newtable(L);
          if (TypeCode=='UL')
	    for(int i=0; i<vr->Length/4; i++)
            { lua_pushinteger(L, ((UINT32 *)(vr->Data))[i]); lua_rawseti(L, -2, i+1);
            }
          else if (TypeCode=='US')
	    for(int i=0; i<vr->Length/2; i++)
            { lua_pushinteger(L, ((UINT16 *)(vr->Data))[i]); lua_rawseti(L, -2, i+1);
            }
          else if (TypeCode=='SL')
	    for(int i=0; i<vr->Length/4; i++)
            { lua_pushinteger(L, ((INT32 *)(vr->Data))[i]); lua_rawseti(L, -2, i+1);
            }
          else if (TypeCode=='SS')
	    for(int i=0; i<vr->Length/2; i++)
            { lua_pushinteger(L, ((INT16 *)(vr->Data))[i]); lua_rawseti(L, -2, i+1);
            }
          else if (TypeCode=='FD')
	    for(int i=0; i<vr->Length/8; i++)
            { lua_pushnumber(L, ((double *)(vr->Data))[i]); lua_rawseti(L, -2, i+1);
            }
          else if (TypeCode=='OF')
	    for(int i=0; i<vr->Length/4; i++)
            { lua_pushnumber(L, ((float *)(vr->Data))[i]); lua_rawseti(L, -2, i+1);
            }
          else if (TypeCode=='FL')
	    for(int i=0; i<vr->Length/4; i++)
            { lua_pushnumber(L, ((float *)(vr->Data))[i]); lua_rawseti(L, -2, i+1);
            }
          else if (TypeCode=='OW')
	    for(int i=0; i<vr->Length/2; i++)
            { lua_pushinteger(L, ((INT16 *)(vr->Data))[i]); lua_rawseti(L, -2, i+1);
            }
	  else
	    for(int i=0; i<vr->Length; i++)
            { lua_pushinteger(L, *((BYTE *)(vr->Data)+i)); lua_rawseti(L, -2, i+1);
            }
        }
        else
        { lua_pushlstring(L, (char *)(vr->Data), vr->Length);
        }
        return 1;
      }
    }
    return 0;
  }
  
#define JSMN_STRICT
#include "jsmn.h"

  int jsonaddtoobject(DICOMDataObject *O, const char *js, jsmntok_t *t, size_t count);

  int jsonname(char *name, const char *js, jsmntok_t *t, size_t count)
  { if (t->end-t->start>127) return 0;
    if (t->type!=JSMN_STRING) return 0;
    strncpy(name, js+t->start, t->end-t->start);
    name[t->end-t->start]=0;
    return 1;
  }
  
  int jsonsetvr(VR *vr, const char *js, jsmntok_t *t, size_t count)
  { if (t->type==JSMN_PRIMITIVE || t->type==JSMN_STRING)
    { int llen = t->end-t->start;
      int len=0; 
      for (int i=0; i<llen; i++) { if (js[t->start+i]=='\\') i++; len++; }
      if (VRType.RunTimeClass(vr->Group, vr->Element, NULL)=='US')
      { char string[64];
        if (len) 
        { len = 2;
	  *(unsigned short *)string = atoi(string);
        }
        vr->ReAlloc(len);
        if (len) memcpy(vr->Data, string, len);
      }
      else if (VRType.RunTimeClass(vr->Group, vr->Element, NULL)=='UL')
      { char string[64];
        if (len) 
        { len = 4;
	  *(unsigned int *)string = atoi(string);
        }
        vr->ReAlloc(len);
        if (len) memcpy(vr->Data, string, len);
      }
      else
      { vr->ReAlloc(2*((len+1)/2));
        len=0; for (int i=0; i<llen; i++) { if (js[t->start+i]=='\\') i++; ((char *)(vr->Data))[len++]=js[t->start+i]; }
        if (len&1)((char *)(vr->Data))[len++]=' ';
      }
      return 1;
    }
    if (t->type==JSMN_ARRAY)
    { Array < DICOMDataObject * > *SQE  = new Array <DICOMDataObject *>;
      int j = 0;
      for (int i = 0; i < t->size; i++) {
        DICOMDataObject *dd = new DICOMDataObject; 
        j += jsonaddtoobject(dd, js, t + 1 + j, count - j);
	SQE->Add(dd);      
      }
      vr->SQObjectArray = SQE;
      return j+1;
    }
    return 0;
  }

  int jsonaddtoobject(DICOMDataObject *O, const char *js, jsmntok_t *t, size_t count)
  { char name[128];
    int j = 0;
    
    for (int i = 0; i < t->size; i++) 
    { if (t[j+1].type==JSMN_PRIMITIVE || t[j+1].type==JSMN_STRING || t[j+1].type==JSMN_ARRAY) 
      { int g=-1, e=-1;

        j += jsonname(name, js, t+j+1, count-j);
	if (isxdig(name[0]) && strlen(name)==8) 
	{ g = htoin(name,   4);
  	  e = htoin(name+4, 4);
	}
	else
	{ RTCElement Entry;
          Entry.Description = name;
          if (VRType.GetGroupElement(&Entry))
          { g = Entry.Group;
            e = Entry.Element;
	  }
	}
	if (g>=0 && e>=0)
        { VR *vr = new VR(g, e, 0, (void *) NULL, FALSE);
          O->DeleteVR(vr);
	  delete vr;
	  vr = new VR(g, e, 0, (void *) NULL, FALSE);
          j+=jsonsetvr(vr, js, t+j+1, count-j);
          O->Push(vr);
        }
	else
	  j++; // assume unknown item has one value -> cannot parse unknown sequences for now
      }
      else
	return 0;
    }
    return j + 1;
  }

  // setvr(Object, group, element, data); if data is a table sets a simple VR, if it is an ADDO sets a sequence
  // Also accepts a json array string as data to write into a sequence
  static int luasetvr(lua_State *L)
  { VR *vr;
    if (lua_isuserdata(L,1)) 
    { DICOMDataObject *O = NULL;
      Array < DICOMDataObject * > *A = NULL;
      lua_getmetatable(L, 1);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);
      int g = lua_tointeger(L,2);
      int e = lua_tointeger(L,3);
      UINT16 TypeCode = VRType.RunTimeClass(g, e, NULL);
      if (lua_isuserdata(L, 4) && O)
      { lua_getmetatable(L, 4);
          lua_getfield(L, -1, "ADDO");  A = (Array < DICOMDataObject * > *) lua_topointer(L, -1); lua_pop(L, 1);
        lua_pop(L, 1);
	if (A)
        { vr = new VR(g, e, 0, (void *) NULL, FALSE);
          O->DeleteVR(vr);
	  delete vr;
	  vr = new VR(g, e, 0, (void *) NULL, FALSE);
          Array < DICOMDataObject * > *SQE  = new Array <DICOMDataObject *>;
	  for (int j=0; j<A->GetSize(); j++)
	  { DICOMDataObject *dd = MakeCopy(A->Get(j)); 
	    SQE->Add(dd);
	  }
	  vr->SQObjectArray = SQE;
          O->Push(vr);
	}
      }
      else if (lua_istable(L, 4) && O)
      { int llen;
        vr = O->GetVR(g, e);
        if (!vr) 
        { vr = new VR(g, e, 0, (void *) NULL, FALSE);
          O->Push(vr);
        }
#ifdef LUA_5_2
        llen = lua_rawlen(L, 4);
#else
        llen = lua_objlen(L, 4);
#endif
	if (TypeCode == 'UL')
        { vr->ReAlloc(llen*4);
          for(int i=0; i<llen; i++)
          { lua_rawgeti(L, 4, i+1); ((UINT32 *)(vr->Data))[i] = lua_tointeger(L,-1); lua_pop(L, 1);
	  }
	}
	else if (TypeCode == 'US')
        { vr->ReAlloc(llen*2);
          for(int i=0; i<llen; i++)
          { lua_rawgeti(L, 4, i+1); ((UINT16 *)(vr->Data))[i] = lua_tointeger(L,-1); lua_pop(L, 1);
	  }
	} 
	else if (TypeCode == 'SL')
        { vr->ReAlloc(llen*4);
          for(int i=0; i<llen; i++)
          { lua_rawgeti(L, 4, i+1); ((INT32 *)(vr->Data))[i] = lua_tointeger(L,-1); lua_pop(L, 1);
	  }
	} 
	else if (TypeCode == 'SS')
        { vr->ReAlloc(llen*2);
          for(int i=0; i<llen; i++)
          { lua_rawgeti(L, 4, i+1); ((INT16 *)(vr->Data))[i] = lua_tointeger(L,-1); lua_pop(L, 1);
	  }
	} 
	else if (TypeCode == 'FD')
        { vr->ReAlloc(llen*8);
          for(int i=0; i<llen; i++)
          { lua_rawgeti(L, 4, i+1); ((double *)(vr->Data))[i] = lua_tonumber(L,-1); lua_pop(L, 1);
	  }
	} 
	else if (TypeCode == 'FL')
        { vr->ReAlloc(llen*4);
          for(int i=0; i<llen; i++)
          { lua_rawgeti(L, 4, i+1); ((float *)(vr->Data))[i] = lua_tonumber(L,-1); lua_pop(L, 1);
	  }
	} 
	else if (TypeCode == 'OF')
        { vr->ReAlloc(llen*4);
          for(int i=0; i<llen; i++)
          { lua_rawgeti(L, 4, i+1); ((float *)(vr->Data))[i] = lua_tonumber(L,-1); lua_pop(L, 1);
	  }
	} 
	else if (TypeCode == 'OW')
        { vr->ReAlloc(llen*2);
          for(int i=0; i<llen; i++)
          { lua_rawgeti(L, 4, i+1); ((INT16 *)(vr->Data))[i] = lua_tointeger(L,-1); lua_pop(L, 1);
	  }
	} 
	else
        { vr->ReAlloc(llen);
          for(int i=0; i<llen; i++)
          { lua_rawgeti(L, 4, i+1); ((BYTE *)(vr->Data))[i] = lua_tointeger(L,-1); lua_pop(L, 1);
	  }
	} 
      }
      else if (lua_isstring(L, 4) && O)
      { size_t llen;
	const char *data;
        data = lua_tolstring (L, 4, &llen);            
	vr = O->GetVR(g, e);
        if (!vr) 
        { vr = new VR(g, e, 0, (void *) NULL, FALSE);
          O->Push(vr);
        }

        if (TypeCode!='SQ')
        { if (data && llen) 
          { vr->ReAlloc(2*((llen+1)/2));
            memcpy((BYTE *)(vr->Data), (BYTE *)data, llen);
          }
          if (llen&1)
          { if (TypeCode != 'UI') memset((BYTE *)(vr->Data)+llen, ' ', 1);
            else    	          memset((BYTE *)(vr->Data)+llen, 0, 1);
          }
        }
        else
        { jsmn_parser p;
          jsmntok_t *tok;
          jsmn_init(&p);
          size_t tokcount = llen/3; // safe bet 'x':0,
          tok = (jsmntok_t *)malloc(tokcount*sizeof(jsmntok_t));
          int r = jsmn_parse(&p, data, llen, tok, tokcount);
          if (tok->type == JSMN_ARRAY)
            jsonsetvr(vr, data, tok, r);
          free((void *)tok);
        }
      }
    }
    return 0;
  }

  // readdicom(filename) into Data, or readdicom(x, filename)
  static int luareaddicom(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    DICOMDataObject *O, *P;
    if (lua_isstring(L,1)) 
    { char name[512];
      strcpy(name, (char *)lua_tostring(L,1));
      O = LoadForGUI(name);
      if (!O) return 0;
      if (sd->DDO && sd->DDO!=dummyddo) delete (sd->DDO);
      sd->DDO = O;
      lua_getglobal   (L, "Data");  
      lua_getmetatable(L, -1);
        lua_pushlightuserdata(L, sd->DDO); lua_setfield(L, -2, "DDO");
      lua_pop(L, 2);
    }
    else if (lua_isuserdata(L,1)) 
    { lua_getmetatable(L, 1);
        lua_getfield(L, -1, "DDO");  P = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
	char name[512];
        strcpy(name, (char *)lua_tostring(L,2));
        if (sd->DDO == P)
        { lua_pop(L, 1);
          return 0;
        }
        O = LoadForGUI(name);
        if (!O) 
        { lua_pop(L, 1);
          return 0;
        }
        if (P) delete P;
        lua_pushlightuserdata(L, O); lua_setfield(L, -2, "DDO");
      lua_pop(L, 1);
    }
    lua_pushinteger(L, 1);
    return 1;
  }
  // writedicom(filename), writedicom(Data, filename); Data:writedicom(filename)
  static int luawritedicom(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    if (lua_isstring(L,1) && sd->DDO) 
    { DICOMDataObject *pDDO = MakeCopy(sd->DDO);
      SaveDICOMDataObject((char *)lua_tostring(L,1), pDDO);
      delete pDDO;
    }
    else if (lua_isuserdata(L,1)) 
    { DICOMDataObject *O = NULL;
      lua_getmetatable(L, 1);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);

      if (O)
      { DICOMDataObject *pDDO = MakeCopy(O);
        SaveDICOMDataObject((char *)lua_tostring(L,2), pDDO);
        delete pDDO;
      }
    }
    return 0;
  }

  // copydicom(source), Data:Copy(), Data:Copy(json) -- latter adds/overwrites json object elements to Data
  static int luacopydicom(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    if (lua_isuserdata(L,1)) 
    { DICOMDataObject *O = NULL;
      lua_getmetatable(L, 1);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);

      if (O)
      { DICOMDataObject *pDDO = MakeCopy(O);
        if (lua_isstring(L, 2))
        { size_t llen=0;
          const char *data = lua_tolstring (L, 2, &llen);
          jsmn_parser p;
          jsmntok_t *tok;
          jsmn_init(&p);
          size_t tokcount = llen/3; // safe bet 'x':0,
          tok = (jsmntok_t *)malloc(tokcount*sizeof(jsmntok_t));
          int r = jsmn_parse(&p, data, llen, tok, tokcount);
          if (tok->type == JSMN_OBJECT)
            jsonaddtoobject(pDDO, data, tok, r);
          free((void *)tok);
        }
        luaCreateObject(L, pDDO, NULL, TRUE); 
        return 1;
      }
    }
    return 0;
  }

  // Object:DeleteFromSequence(name, item)
  // Array:Delete(item)
  static int luadeletefromsequence(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    
    if (lua_isuserdata(L,1)) 
    { DICOMDataObject *O = NULL;
      Array < DICOMDataObject * > *A = NULL;
      lua_getmetatable(L, 1);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
        lua_getfield(L, -1, "ADDO");  A = (Array < DICOMDataObject * > *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);

      if (O)
      { Array < DICOMDataObject * > *A2;
        DICOMDataObject *O2;
        char buf[2000]; buf[0]=0;
        SearchDICOMObject(O, lua_tostring(L,2), buf, &A2, &O2);
	if (A2)
	  A2->RemoveAt(lua_tointeger(L, 3));
        return 0;
      }
      else if (A)
      { A->RemoveAt(lua_tointeger(L, 2));
        return 0;
      }
    }
    return 0;
  }

  // Object:AddToSequence(name, item)
  // Array:Add(item)
  static int luaaddtosequence(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    
    if (lua_isuserdata(L,1) && lua_isuserdata(L,2)) 
    { DICOMDataObject *O = NULL;
      Array < DICOMDataObject * > *A = NULL;
      lua_getmetatable(L, 1);
        lua_getfield(L, -1, "ADDO");  A = (Array < DICOMDataObject * > *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);
      lua_getmetatable(L, 2);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);

      if (A && O)
      { DICOMDataObject *dd = MakeCopy(O); 
	A->Add(dd);
        return 0;
      }
    }
    return 0;
  }

  // compressdicom(source, string), Data:Compress(string)
  static int luacompressdicom(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    char name[512];
    strcpy(name, (char *)lua_tostring(L,2));
    if (lua_isuserdata(L,1)) 
    { DICOMDataObject *O = NULL;
      lua_getmetatable(L, 1);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);

      if (O)
      { DICOMDataObject *pDDO = MakeCopy(O);

        //char cmd[512];
	//sprintf(cmd, "compression %s", name);
        //CallImportConverterN(NULL, pDDO, -2, sd->pszModality, sd->pszStationName, sd->pszSop, sd->patid, sd->PDU, sd->Storage, cmd);

        if (!recompress(&pDDO, name, "", name[0]=='n' || name[0]=='N', sd->PDU))
        { delete pDDO; 
          return 0;
        }
      
	//BOOL StripGroup2 = memicmp(name, "as", 2)!=0 && memicmp(name, "is", 2)!=0;
	//recompress(&pDDO, name, "", StripGroup2, sd->PDU);
        luaCreateObject(L, pDDO, NULL, TRUE);
        return 1;
      }
    }
    return 0;
  }

  // Data:writeheader(filename) or writeheader(filename)
  static int luawriteheader(lua_State *L) 
  { struct scriptdata *sd = getsd(L);
    if (lua_isuserdata(L,1)) 
    { DICOMDataObject *O = NULL;
      lua_getmetatable(L, 1);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);

      if (O)
      { FILE *f = fopen(lua_tostring(L, 2), "wt");
        if (f) 
        { NonDestructiveDumpDICOMObject(O, f);
          fclose(f);
        }
      }
    }
    else if (lua_isstring(L,1)) 
    { FILE *f = fopen(lua_tostring(L, 1), "wt");
      if (f) 
      { if (sd->DDO) NonDestructiveDumpDICOMObject(sd->DDO, f);
        fclose(f);
      }
    }
    return 0;
  }
  
// list items in dicom object
  static int lualistitems(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    char names[16384];
    char types[8192];
    char groups[8192];
    char elements[8192];
    char s[256];
    names[0] = 0;
    types[0] = 0;
    groups[0] = 0;
    elements[0] = 0;

    if (lua_isuserdata(L,1)) 
    { DICOMDataObject *O = NULL;
      lua_getmetatable(L, 1);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);

      if (O)
      {	DICOMObject	DO2;
	VR		*vr;
	int		i;
	unsigned int 	Index;
		
	while((vr=O->Pop()))
	{ UINT16 c2 = VRType.RunTimeClass(vr->Group, vr->Element, s);
	  if (names[0]) 
          { strcat(names, "|");
            strcat(types, "|");
            strcat(groups, "|");
            strcat(elements, "|");
	  }
	  strcat(names, s);
	  sprintf(types + strlen(types), "%c%c", c2>>8, c2&0x00ff);
	  sprintf(groups + strlen(groups), "%d", vr->Group);
	  sprintf(elements + strlen(elements), "%d", vr->Element);
	  DO2.Push(vr);
	}
	O->Reset();
	while((vr=DO2.Pop()))
	{ O->Push(vr);
	}

        lua_pushstring(L, names);
        lua_pushstring(L, types);
        lua_pushstring(L, groups);
        lua_pushstring(L, elements);
      	return 4;
      }
    }
    return 0;
  }

  // serialize dicom object; optional include pixel data, truncates large elements
  #define MAXLEN 200000000 // total length, one element is max 1/2 of that

  static int luaserialize(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    BOOL json=false, includepixeldata=false, dicomweb=false;
    if (lua_isboolean(L,2))
      json = lua_toboolean(L, 2);
    if (lua_isboolean(L,3))
      includepixeldata = lua_toboolean(L, 3);
    if (lua_isboolean(L,4))
      dicomweb = lua_toboolean(L, 4);

    char eq='=';
    char br1='{';
    char br2='}';
    char br3[2];
    if (json)
    { eq=':';
      br1='[';
      br2=']';
    }
    br3[0]=0;
    if (dicomweb)
    { strcpy(br3, "}");
    }

    if (lua_isuserdata(L,1)) 
    { char *result=(char *)malloc(MAXLEN);
      result[0]=0;
      DICOMDataObject *O = NULL;
      Array < DICOMDataObject * > *A=NULL;
      unsigned int Index=0;
      int nUN=0;
      BOOL truncated = FALSE;

      lua_getmetatable(L, 1);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
        lua_getfield(L, -1, "ADDO");  A = (Array < DICOMDataObject * > *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);

      if (A && !json) Index+=sprintf(result+Index, "{");
      if ((A||dicomweb) && json)  Index+=sprintf(result+Index, "[");

      for (int j=0; j<(A!=NULL?A->GetSize():1); j++) 
      { if (A) O=(DICOMDataObject *)(A->Get(j));
        if (O==NULL) break;

        DICOMObject	DO2;
	VR		*vr;
	int		i;
		
	Index+=sprintf(result+Index, "{");

	int count=0;
	while((vr=O->Pop()))
	{ char s[128];
          UINT16 c2 = VRType.RunTimeClass(vr->Group, vr->Element, s);
	  if (c2==0) nUN++;
	  if (Index>=MAXLEN/2) truncated=TRUE;
	  
	  if (vr->Element!=0 && *s!=0 && c2!=0 && Index<MAXLEN/2)
	  { count++;
            char *name=s;
              
            char tmp[128];
            if (json && !dicomweb) 
	    { sprintf(tmp, "\"%s\"", s);
              name = tmp;
	    }
            if (json && dicomweb)
            { sprintf(tmp, "\"%04X%04X\": { \"vr\": \"%c%c\", \"Value\"", 
                vr->Group, vr->Element, c2>>8, c2&255);
              name = tmp;
            }
            if (c2=='UL' && (vr->Length==4 && !dicomweb))
            { Index+=sprintf(result+Index, "%s%c%d,", name, eq, (int)((UINT32 *)(vr->Data))[0]);
	    }
	    else if (c2=='UL' && (vr->Length>4 || dicomweb))
            { Index+=sprintf(result+Index, "%s%c%c", name, eq, br1);
              for (i=0; (i<vr->Length/4) && (Index<MAXLEN/2); i++)
                Index+=sprintf(result+Index, "%d,", (int)((UINT32 *)(vr->Data))[i]);
	      if (vr->Length) Index--;
	      if (Index>=MAXLEN/2 && !json) Index+=sprintf(result+Index, " --[[truncated]] ");
	      Index+=sprintf(result+Index, "%c%s,", br2, br3);
	    }
	    else if (c2=='UL')
	    { count--;
	    }
	    else if (c2=='US' && (vr->Length==2 && !dicomweb))
            { Index+=sprintf(result+Index, "%s%c%d,", name, eq, ((UINT16 *)(vr->Data))[0]);
	    }
	    else if (c2=='US' && (vr->Length>2 || dicomweb))
            { Index+=sprintf(result+Index, "%s%c%c", name, eq, br1);
              for (i=0; (i<vr->Length/2) && (Index<MAXLEN/2); i++)
                Index+=sprintf(result+Index, "%d,", ((UINT16 *)(vr->Data))[i]);
	      if (vr->Length) Index--;
	      if (Index>=MAXLEN/2 && !json) Index+=sprintf(result+Index, " --[[truncated]] ");
	      Index+=sprintf(result+Index, "%c%s,", br2, br3);
	    }
	    else if (c2=='US')
	    { count--;
	    }
	    else if (c2=='SL' && (vr->Length==4 && !dicomweb))
            { Index+=sprintf(result+Index, "%s%c%d,", name, eq, (int)((INT32 *)(vr->Data))[0]);
	    }
	    else if (c2=='SL' && (vr->Length>4 || dicomweb))
            { Index+=sprintf(result+Index, "%s%c%c", name, eq, br1);
              for (i=0; (i<vr->Length/4) && (Index<MAXLEN/2); i++)
                Index+=sprintf(result+Index, "%d,", (int)((INT32 *)(vr->Data))[i]);
	      if (vr->Length) Index--;
	      if (Index>=MAXLEN/2 && !json) Index+=sprintf(result+Index, " --[[truncated]] ");
	      Index+=sprintf(result+Index, "%c%s,", br2, br3);
	    }
	    else if (c2=='SL')
	    { count--;
	    }
	    else if (c2=='SS' && (vr->Length==2 && !dicomweb))
            { Index+=sprintf(result+Index, "%s%c%d,", name, eq, ((INT16 *)(vr->Data))[0]);
	    }
	    else if (c2=='SS' && (vr->Length>2 || dicomweb))
            { Index+=sprintf(result+Index, "%s%c%c", name, eq, br1);
              for (i=0; (i<vr->Length/2) && (Index<MAXLEN/2); i++)
                Index+=sprintf(result+Index, "%d,", ((INT16 *)(vr->Data))[i]);
	      if (vr->Length) Index--;
	      if (Index>=MAXLEN/2 && !json) Index+=sprintf(result+Index, " --[[truncated]] ");
	      Index+=sprintf(result+Index, "%c%s,", br2, br3);
	    }
	    else if (c2=='SS')
	    { count--;
	    }
	    else if (c2=='FD' && (vr->Length==8 && !dicomweb))
            { Index+=sprintf(result+Index, "%s%c%f,", name, eq, ((double *)(vr->Data))[0]);
	    }
	    else if (c2=='FD' && (vr->Length>8 || dicomweb))
            { Index+=sprintf(result+Index, "%s%c%c", name, eq, br1);
              for (i=0; (i<vr->Length/8) && (Index<MAXLEN/2); i++)
                Index+=sprintf(result+Index, "%f,", ((double *)(vr->Data))[i]);
	      if (vr->Length) Index--;
	      if (Index>=MAXLEN/2 && !json) Index+=sprintf(result+Index, " --[[truncated]] ");
	      Index+=sprintf(result+Index, "%c%s,", br2, br3);
	    }
	    else if (c2=='FD')
	    { count--;
	    }
	    else if (c2=='FL' && (vr->Length==4 && !dicomweb))
            { Index+=sprintf(result+Index, "%s%c%f,", name, eq, ((float *)(vr->Data))[0]);
	    }
	    else if (c2=='FL' && (vr->Length>4 || dicomweb))
            { Index+=sprintf(result+Index, "%s%c%c", name, eq, br1);
              for (i=0; (i<vr->Length/4) && (Index<MAXLEN/2); i++)
                Index+=sprintf(result+Index, "%f,", ((float *)(vr->Data))[i]);
	      if (vr->Length) Index--;
	      if (Index>=MAXLEN/2 && !json) Index+=sprintf(result+Index, " --[[truncated]] ");
	      Index+=sprintf(result+Index, "%c%s,", br2, br3);
	    }
	    else if (c2=='FL')
	    { count--;
	    }

  	    else if (c2 == 'OF' && vr->Length>0 && !includepixeldata && !json)
            { Index+=sprintf(result+Index, "%s%cnil --[[OF not serialized]],", name, eq);
	    }
	    else if (c2=='OF' && vr->Length>0 && !includepixeldata)
            { count--; // do nothing
	    }
	    else if (c2=='OF' && vr->Length>0 && includepixeldata)
            { Index+=sprintf(result+Index, "%s%c%c", name, eq, br1);
              for (i=0; (i<vr->Length/4) && (Index<MAXLEN/2); i++)
                Index+=sprintf(result+Index, "%f,", ((float *)(vr->Data))[i]);
	      Index--;
	      if (Index>=MAXLEN/2 && !json) Index+=sprintf(result+Index, " --[[truncated]] ");
	      Index+=sprintf(result+Index, "%c%s,", br2, br3);
	    }

  	    else if (c2 == 'OW' && vr->Length>0 && !includepixeldata && !json)
            { Index+=sprintf(result+Index, "%s%cnil --[[OW not serialized]],", name, eq);
	    }
  	    else if (c2 == 'OW' && vr->Length>0 && !includepixeldata)
            { count--; // do nothing
	    }
	    else if (c2=='OW' && vr->Length>0 && includepixeldata)
            { Index+=sprintf(result+Index, "%s%c%c", name, eq, br1);
              for (i=0; (i<vr->Length/2) && (Index<MAXLEN/2); i++)
                Index+=sprintf(result+Index, "%d,", ((short *)(vr->Data))[i]);
	      Index--;
	      if (Index>=MAXLEN/2 && !json) Index+=sprintf(result+Index, " --[[truncated]] ");
	      Index+=sprintf(result+Index, "%c%s,", br2, br3);
	    }

  	    else if (c2 == 'OB' && vr->Length>0 && includepixeldata && !json)
            { Index+=sprintf(result+Index, "%s%cnil --[[OB not serialized]],", name, eq);
	    }
  	    else if (c2 == 'OB' && vr->Length>0 && !includepixeldata)
            { count--; // do nothing
	    }
	    else if (c2=='OB' && vr->Length>0 && includepixeldata)
            { Index+=sprintf(result+Index, "%s%c%c", name, eq, br1);
              for (i=0; (i<vr->Length) && (Index<MAXLEN/2); i++)
                Index+=sprintf(result+Index, "%d,", ((BYTE *)(vr->Data))[i]);
	      Index--;
	      if (Index>=MAXLEN/2 && !json) Index+=sprintf(result+Index, " --[[truncated]] ");
	      Index+=sprintf(result+Index, "%c%s,", br2, br3);
	    }

	    else if (c2=='DS' && vr->Length>1 && dicomweb)
            { char *list = (char *)malloc(vr->Length+1);
              memcpy(list, vr->Data, vr->Length);
	      list[vr->Length]=0;
	      char *p=list-1;
              Index+=sprintf(result+Index, "%s%c%c", name, eq, br1);
	      int count1=0;
	      while(p)
	      {  float a;
                 a = atof(p+1);
		 Index+=sprintf(result+Index, "%f,", a);
	         p=strchr(p+1, '\\');
		 count1++;
	      }
	      if (count1) Index--;
	      Index+=sprintf(result+Index, "%c%s,", br2, br3);
	      free(list);
	    }
	    else if (c2=='IS' && vr->Length>1 && dicomweb)
            { char *list = (char *)malloc(vr->Length+1);
              memcpy(list, vr->Data, vr->Length);
	      list[vr->Length]=0;
	      char *p=list-1;
              Index+=sprintf(result+Index, "%s%c%c", name, eq, br1);
	      int count1=0;
	      while(p)
	      {  int a;
                 a = atoi(p+1);
		 Index+=sprintf(result+Index, "%d,", a);
	         p=strchr(p+1, '\\');
		 count1++;
	      }
	      if (count1) Index--;
	      Index+=sprintf(result+Index, "%c%s,", br2, br3);
	      free(list);
	    }
	    else if (c2=='AT' && vr->Length==4 && dicomweb)
            { Index+=sprintf(result+Index, "%s%c%c", name, eq, br1);
	      int t[2];
	      t[0]=((UINT16 *)(vr->Data))[0];
	      t[1]=((UINT16 *)(vr->Data))[1];
	      Index+=sprintf(result+Index, "\"%04X%04X\"", t[0], t[1]);
	      Index+=sprintf(result+Index, "%c%s,", br2, br3);
	    }

  	    else if (c2 == 'SQ')
            { if (vr->SQObjectArray)
	      { lua_getglobal(L, "serialize");
	        luaCreateObject(L, NULL, (Array < DICOMDataObject * > *)vr->SQObjectArray, FALSE);
		lua_pushboolean(L, json);
		lua_pushboolean(L, includepixeldata);
		lua_pushboolean(L, dicomweb);
                lua_call(L, 4, 1);
		if (lua_strlen(L, -1)>=MAXLEN/2)
                { Index+=sprintf(result+Index, "%s%cnil", name, eq);
	          if (!json) Index+=sprintf(result+Index, "--[[long SQ not serialized]]");
	          Index+=sprintf(result+Index, ",");
		}
	        else
                  Index+=sprintf(result+Index, "%s%c%s%s,", name, eq, lua_tostring(L, -1), br3);
	        lua_pop(L, 1);
	      }
	    }
	    else if (vr->Length>0)
            { unsigned int len = vr->Length;
    
              // remove trailing zero or space
	      if (c2 == 'UI' && ((unsigned char *)(vr->Data))[len-1]==0) len--;
              else if (c2 == 'LO' && ((unsigned char *)(vr->Data))[len-1]==' ') len--;
              else if (c2 == 'CS' && ((unsigned char *)(vr->Data))[len-1]==' ') len--;
              else if (c2 == 'SH' && ((unsigned char *)(vr->Data))[len-1]==' ') len--;
              else if (c2 == 'TM' && ((unsigned char *)(vr->Data))[len-1]==' ') len--;
              else if (c2 == 'DA' && ((unsigned char *)(vr->Data))[len-1]==' ') len--;
              else if (c2 == 'PN' && ((unsigned char *)(vr->Data))[len-1]==' ') len--;
              else if (c2 == 'LT' && ((unsigned char *)(vr->Data))[len-1]==' ') len--;
              else if (c2 == 'UT' && ((unsigned char *)(vr->Data))[len-1]==' ') len--;
              else if (c2 == 'ST' && ((unsigned char *)(vr->Data))[len-1]==' ') len--;

	      if (len>MAXLEN/2) 
	      { len = MAXLEN/2; // keep to safe limit
      	        Index+=sprintf(result+Index, " --[[truncated]] ");
	      }
              
              // todo: in dicomweb mode string should be split into elements at backslash
              char *t = (char *)malloc(len*6+1); // generate escape characters where needed /u0000 requires 6
              int k=0;
              for (int i=0; i<len; i++)
              { unsigned char c= ((unsigned char *)(vr->Data))[i];
                if (c=='"' || c=='\\' || (c=='/' && json)) 
		{ t[k++] = '\\';
	          t[k++] = c;
		}
		else if (c<32) // Lua escapes are e.g. \n and \nnn where nnn is Decimal!
		{ static char escapes[] = "0123456AbtnVfr456789012345678901";
	          if (escapes[c]>='a') // Lua and Json
		  { t[k++]='\\';
	            t[k++]=escapes[c];
		  }
	          else if (escapes[c]>='A') // Lua only
		  { t[k++]='\\';
	            t[k++]=escapes[c]-32;
		  }
		  else if (json)
		  { t[k++]='\\';
	            t[k++]='u';
		    sprintf(t+k, "%04x", c);
		    k+=4;
		  }
		  else
		  { t[k++]='\\';
	            t[k++]=((c/100)%10)+'0';
	            t[k++]=((c/ 10)%10)+'0';
	            t[k++]=( c     %10)+'0';
		  }
		}
                else		
	          t[k++] = c;
	      }
	      t[k++] = 0;
              if (dicomweb)
                Index+=sprintf(result+Index, "%s%c%c\"%s\"%c%s,", name, eq, br1, t, br2, br3);
              else
                Index+=sprintf(result+Index, "%s%c\"%s\"%s,", name, eq, t, br3);
	      free(t);
	    }
	    else if (vr->Length==0)
            { if (dicomweb)
                Index+=sprintf(result+Index, "%s%c%c\"\"%c%s,", name, eq, br1, br2, br3);
              else
                Index+=sprintf(result+Index, "%s%c\"\"%s,", name, eq, br3);
	    }
	  }
	  DO2.Push(vr);
	}
	if (count) Index--;
	Index+=sprintf(result+Index, "}");
	O->Reset();
	while((vr=DO2.Pop()))
	{ O->Push(vr);
	}
	if (A) Index+=sprintf(result+Index, ",");
      }
      if (A && A->GetSize()>0) Index--;
      if (A || dicomweb) Index+=sprintf(result+Index, "%c", br2);
      if (truncated && !json) 
	Index+=sprintf(result+Index, " --[[truncated]] ");
      if (nUN && !json) 
	Index+=sprintf(result+Index, " --[[skipped %d UN elements]] ", nUN);
      lua_pushlstring(L, result, Index);
      free(result);
      return 1;
    }
    return 0;
  }

  // newdicomobject(), DicomObject:new(), DicomObject:new(json) -- latter converts json object to DICOM
  static int luanewdicomobject(lua_State *L)
  { DICOMDataObject *O = new DICOMDataObject;
    luaCreateObject(L, O, NULL, TRUE); 
    size_t llen=0;
    const char *data=NULL;
    if (lua_isstring(L, 1)) data = lua_tolstring (L, 1, &llen);
    if (lua_isstring(L, 2)) data = lua_tolstring (L, 2, &llen);
    if (data)
    { jsmn_parser p;
      jsmntok_t *tok;
      jsmn_init(&p);
      size_t tokcount = llen/3; // safe bet 'x':0,
      tok = (jsmntok_t *)malloc(tokcount*sizeof(jsmntok_t));
      int r = jsmn_parse(&p, data, llen, tok, tokcount);
      if (tok->type == JSMN_OBJECT)
        jsonaddtoobject(O, data, tok, r);
      free((void *)tok);
    }

    return 1;
  }
  
  // newdicomarray()
  static int luanewdicomarray(lua_State *L)
  { Array < DICOMDataObject * > *A = new Array < DICOMDataObject * >;
    luaCreateObject(L, NULL, A, TRUE); 
    return 1;
  }

  // deletedicomobject()
  static int luadeletedicomobject(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    DICOMDataObject *O = NULL;
    int owned=0;
    Array < DICOMDataObject * > *A = NULL;
    if (lua_isuserdata(L,1)) 
    { lua_getmetatable(L, 1);
        lua_getfield(L, -1, "DDO");   O = (DICOMDataObject *)             lua_topointer(L, -1); lua_pop(L, 1);
        lua_getfield(L, -1, "ADDO");  A = (Array < DICOMDataObject * > *) lua_topointer(L, -1); lua_pop(L, 1);
        lua_getfield(L, -1, "owned"); owned =                             lua_tointeger(L, -1); lua_pop(L, 1);
        lua_pushlightuserdata(L, NULL); lua_setfield(L, -2, "DDO");
        lua_pushlightuserdata(L, NULL); lua_setfield(L, -2, "ADDO");
      lua_pop(L, 1);
    }
    if (owned)
    { if (O && O!=(DICOMDataObject *)(sd->DCO) && O!=sd->DDO) 
        delete O;
      if (A) 
      { for (int i=0; i<A->GetSize(); i++) delete (DICOMDataObject *)(A->Get(i));
        delete A;
      }
    }
    return 0;
  }

  static int luaConvertBinaryData(lua_State *L)
  { char *name;
    int len, i, j, N, typ;
    size_t dum;
    char *p, *q;
  
    name = (char *)lua_tostring(L, 1);  // e.g. 1000*f8
    q = strchr(name, '*');
    if (q) N = atoi(name), q++; else N = 1, q = name;
    typ = q[0];
    len = atoi(q+1);
    if (lua_isstring(L, 2))
    { p = (char *)lua_tolstring(L, 2, &dum);
      lua_newtable(L);
      if (typ=='f')
        for (i=0; i<N; i++)
        { if (len==8) lua_pushnumber(L, *(double *)(p+i*8));
          if (len==4) lua_pushnumber(L, *(float *) (p+i*4));
          lua_rawseti(L, -2, i+1);
        }
      if (typ=='u')
        for (i=0; i<N; i++)
        { if (len==4) lua_pushinteger(L, *(unsigned int *)   (p+i*4));
          if (len==3) lua_pushinteger(L, *(unsigned short *) (p+i*3) + 256*256 * *(unsigned char *) (p+i*3+2));
          if (len==2) lua_pushinteger(L, *(unsigned short *) (p+i*2));
          if (len==1) lua_pushinteger(L, *(unsigned char *)  (p+i*1));
          lua_rawseti(L, -2, i+1);
        }
      if (typ=='s')
        for (i=0, j=0; i<N; i++)
        { lua_pushstring(L, p+j);
	  j += strlen(p+j+1);
          lua_rawseti(L, -2, i+1);
        }
    }
    else if(lua_istable(L, 2))
    { p = (char *) malloc(N*len+1); // +1 slack needed for last u3 stored as uint
      if (typ=='f')
        for (i=0; i<N; i++)
	{ lua_rawgeti(L, -1, i+1);
          if (len==8) *(double *)(p+i*8) = lua_tonumber(L, -1);
          if (len==4) *(float *) (p+i*4) = lua_tonumber(L, -1);
          lua_pop(L, 1);
      }
      if (typ=='u')
        for (i=0; i<N; i++)
	{ lua_rawgeti(L, -1, i+1);
          if (len==4) *(unsigned int *) (p+i*4) = lua_tointeger(L, -1);
          if (len==3) *(unsigned int *) (p+i*3) = lua_tointeger(L, -1);
          if (len==2) *(unsigned short*)(p+i*2) = lua_tointeger(L, -1);
          if (len==1) *(unsigned char *)(p+i*1) = lua_tointeger(L, -1);
          lua_pop(L, 1);
        }
      if (typ=='s')
        for (i=0; i<N; i++)
	{ lua_rawgeti(L, -1, i+1);
	  memset(p+i*len, 0, len);
	  strcpy(p+i*len, lua_tostring(L, -1));
        }
      lua_pushlstring(L, p, N*len);
    }
    return 1;
  }

// heap information for leak debugging
  static int luaheapinfo(lua_State *L)
  { lua_pushstring(L, heapinfo());
    return 1;
  }

// system does a backgroundexec
  static int luasystem(lua_State *L)
  { if (lua_isstring(L,1))
    { int r;
#ifdef WIN32
      char command[512], arg[10], *p;
      strcpy(command, lua_tostring(L,1));
      if (command[0]=='\'' || command[0]=='"')
      { arg[0] = command[0];
        arg[1] = ' ';
	arg[2] = 0;
	p = strstr(command, arg);
	if (p) 
	{ *p=0;
          r = (int)BackgroundExec(command+1, p+2);
	}
	else
          r = (int)BackgroundExec(command+1, "");
      }
      else
      { p = strchr(command, ' ');
	if (p) 
	{ *p=0;
          r = (int)BackgroundExec(command, p+1);
	}
	else
          r = (int)BackgroundExec(command, "");
      }
#else
      r = system((char *)lua_tostring(L,1));
#endif
      lua_pushinteger(L, r);
      return 1;
    }
    return 0;
  }

  // sleep in milliseconds
  static int luasleep(lua_State *L)
  { if (lua_isnumber(L,1))
      Sleep(lua_tointeger(L,1));
    return 0;
  }
  
  static int luaaddimage(lua_State *L)
  { if (lua_isuserdata(L,1)) 
    { DICOMDataObject *O = NULL;
      lua_getmetatable(L, 1);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);

      if (O)
      { Database DB;
      	char rFilename[1024];
	ExtendedPDU_Service PDU;
        PDU.AttachRTC(&VRType);

        if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
        { OperatorConsole.printf("***Error Connecting to SQL\n");
          return ( FALSE );
        }

        DICOMDataObject *P = MakeCopy(O);
	int rc = SaveToDisk(&DB, NULL, P, rFilename, TRUE, &PDU, 0, TRUE);
	if (!rc)
	{ OperatorConsole.printf("***[lua addimage] Error entering object into server%s\n", DataHost);
	}
	else
        { if (rc==2) OperatorConsole.printf("Lua script rewritten file: %s\n", rFilename);
          else       OperatorConsole.printf("Lua script written file: %s\n", rFilename);
	}
      }
    }
    return 0;
  }

  static int luamkdir(lua_State *L)
  { if (lua_isstring(L,1)) 
    { char cline[512], s[512];
      strcpy(cline, lua_tostring(L,1));
      for (int sIndex = 0; sIndex<=strlen(cline); sIndex++)
	if (cline[sIndex]==PATHSEPCHAR)
	{ strcpy(s, cline);
	  s[sIndex]='\0';
	  mkdir(s);
	}
        mkdir(cline);
    }
    return 0;
  }

  static int luatempfile(lua_State *L)
  { char tempf[512];
    NewTempFile(tempf, lua_tostring(L,1));
    lua_pushstring(L, tempf);
    return 1;
  }
  
static void HTML(const char *string, ...);
static int CGI(char *out, const char *name, const char *def);

  static int luaHTML(lua_State *L)
  { // char buf[1000];
    int n=lua_gettop(L);
    switch(n)
    { case 1: HTML(lua_tostring(L, 1)); break;
      case 2: HTML(lua_tostring(L, 1), lua_tostring(L, 2)); break;
      case 3: HTML(lua_tostring(L, 1), lua_tostring(L, 2), lua_tostring(L, 3)); break;
      case 4: HTML(lua_tostring(L, 1), lua_tostring(L, 2), lua_tostring(L, 3), lua_tostring(L, 4)); break;
      case 5: HTML(lua_tostring(L, 1), lua_tostring(L, 2), lua_tostring(L, 3), lua_tostring(L, 4), lua_tostring(L, 5)); break;
    }
    return 0;
  }

static char *post_buf=NULL;
static char *post_file=NULL;
static int post_len=0;
static char uploadedfile[256];

  static int luaCGI(lua_State *L)
  { char buf[1000];
    int n=lua_gettop(L);
    switch(n)
    { case 0: lua_pushlstring(L, post_file, post_len); return 1;
      case 1: CGI(buf, lua_tostring(L, 1), ""); break;
      case 2: CGI(buf, lua_tostring(L, 1), lua_tostring(L,2)); break;
    }
    lua_pushstring(L, buf);
    return 1;
  }
  static int luagpps(lua_State *L)
  { char buf[1000];
    int n=lua_gettop(L);
    switch(n)
    { case 1: MyGetPrivateProfileString ( "sscscp", lua_tostring(L,1), "", buf, 256, ConfigFile);
      case 2: MyGetPrivateProfileString ( lua_tostring(L,1), lua_tostring(L,2),"", buf, 256, ConfigFile);
      case 3: MyGetPrivateProfileString ( lua_tostring(L,1), lua_tostring(L,2), lua_tostring(L,3), buf, 256, ConfigFile);
    }
    lua_pushstring(L, buf);
    return 1;
  }

//todo (now read/write directly from dicom.ini): 
//IgnoreOutOfMemoryErrors, ImportExportDragAndDrop, TruncateFieldNames, ForwardAssociationRelease, 
//RetryForwardFailed, UIDPrefix, ForwardAssociationLevel, ForwardAssociationRefreshDelay
//ForwardAssociationCloseDelay, ForwardAssociationRelease, MaximumExportRetries,
//MaximumDelayedFetchForwardRetries, ziptime, NightlyCleanThreshhold, NightlyCleanThreshhold
//OverlapVirtualGet, SendUpperCaseAE, TCPIPTimeOut, PadAEWithZeros
//acrnema.map, dicom.sql, dgatesop.lst, dgate.dic, Database, Patient, Study[i], Series[i], Image[i]

  static int luaGlobalIndex(lua_State *L)
  { int n;
    int *idata=NULL;
    char *sdata=NULL;
    char szRootSC[64], Parameter[512];
    for (n=0;;n++)
    { if (!iglobals[n].data) break;
      if (stricmp(lua_tostring(L,2), iglobals[n].name)==0) idata = iglobals[n].data;
    }
    for (n=0;;n++)
    { if (!sglobals[n].data) break;
      if (stricmp(lua_tostring(L,2), sglobals[n].name)==0) sdata = sglobals[n].data;
    }
    if (MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile))
      MyGetPrivateProfileString(szRootSC, lua_tostring(L,2), "", Parameter, 512, ConfigFile);

    if (idata)      { lua_pushinteger(L, *idata);     return 1; }
    if (sdata)      { lua_pushstring (L,  sdata);     return 1; }
    if (*Parameter) { lua_pushstring (L,  Parameter); return 1; }
    return 0;
  }
  static int luaGlobalNewIndex(lua_State *L)
  { int n, slen;
    int *idata=NULL;
    char *sdata=NULL;
    char szRootSC[64], Parameter[512], script[1024];
    for (n=0;;n++)
    { if (!iglobals[n].data) break;
      if (stricmp(lua_tostring(L,2), iglobals[n].name)==0) idata = iglobals[n].data;
    }
    for (n=0;;n++)
    { if (!sglobals[n].data) break;
      if (stricmp(lua_tostring(L,2), sglobals[n].name)==0) sdata = sglobals[n].data, slen = sglobals[n].len;
    }
    if (MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile))
      MyGetPrivateProfileString(szRootSC, lua_tostring(L,2), "", Parameter, 512, ConfigFile);
    if (*Parameter)
    { sprintf(script, "put_param:%s,%s", lua_tostring(L,2), lua_tostring(L,3));
      SendServerCommand("", script, 0, NULL);
      SendServerCommand("", "read_ini:", 0, NULL);
    }
    if (idata) *idata = lua_tointeger(L,3);
    if (sdata) strncpy(sdata, lua_tostring(L,3), slen-1), sdata[slen-1]=0;
    return 0;
  }

  static int luaAssociationIndex(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    char *cdata=NULL;
    int *idata=NULL;
    BYTE buf[64]; buf[0]=0;
    if (strcmp(lua_tostring(L,2), "Calling")==0)     { cdata = (char *)buf; sd->PDU->GetLocalAddress (buf); }
    else if (strcmp(lua_tostring(L,2), "Called")==0) { cdata = (char *)buf; sd->PDU->GetRemoteAddress(buf); }
    else if (strcmp(lua_tostring(L,2), "ConnectedIP")==0) 
    { unsigned int ip = (unsigned int) sd->ConnectedIP; 
      sprintf((char *)buf, "%d.%d.%d.%d", ip&255, (ip>>8)&255, (ip>>16)&255, (ip>>24)&255);
      cdata = (char *)buf;
    }

    if (cdata) while (strlen(cdata)>0 && cdata[strlen(cdata)-1]==' ') cdata[strlen(cdata)-1] = 0;

    if (strcmp(lua_tostring(L,2), "Thread")==0) idata = &(sd->PDU->ThreadNum);
		        
    if (cdata) { lua_pushstring (L,  cdata);  return 1; }
    if (idata) { lua_pushinteger(L, *idata);  return 1; }
    return 0;
  }

  static int luaSeqClosure(lua_State *L)
  { const char *s = lua_tostring(L, lua_upvalueindex(1));
    if (strcmp(s, "Read" )==0)    return luareaddicom(L);
    if (strcmp(s, "Write")==0)    return luawritedicom(L);
    if (strcmp(s, "Dump" )==0)    return luawriteheader(L);
    if (strcmp(s, "SetVR")==0)    return luasetvr(L);
    if (strcmp(s, "GetVR")==0)    return luagetvr(L);
    if (strcmp(s, "GetPixel")==0) return luagetpixel(L);
    if (strcmp(s, "SetPixel")==0) return luasetpixel(L);
    if (strcmp(s, "GetRow")==0)   return luagetrow(L);
    if (strcmp(s, "SetRow")==0)   return luasetrow(L);
    if (strcmp(s, "GetColumn")==0)return luagetcolumn(L);
    if (strcmp(s, "SetColumn")==0)return luasetcolumn(L);
    if (strcmp(s, "GetImage")==0) return luagetimage(L);
    if (strcmp(s, "SetImage")==0) return luasetimage(L);
    if (strcmp(s, "Script")==0)   return luascript(L);
    if (strcmp(s, "AddImage")==0) return luaaddimage(L);
    if (strcmp(s, "Copy")==0)     return luacopydicom(L);
    if (strcmp(s, "Compress")==0) return luacompressdicom(L);
    if (strcmp(s, "ListItems")==0) return lualistitems(L);
    if (strcmp(s, "Serialize")==0) return luaserialize(L);
    if (strcmp(s, "DeleteFromSequence")==0) return luadeletefromsequence(L);
    if (strcmp(s, "AddToSequence")==0) return luaaddtosequence(L);
    if (strcmp(s, "new")==0)      return luanewdicomobject(L);
    if (strcmp(s, "newarray")==0) return luanewdicomarray(L);
    if (strcmp(s, "free")==0)     return luadeletedicomobject(L);
    return 0;
  }
  static int luaSeqnewindex(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    char script[1024];

    Array < DICOMDataObject * > *A = NULL;
    DICOMDataObject *O = NULL;
    lua_getmetatable(L, 1);
      lua_getfield(L, -1, "ADDO"); A = (Array < DICOMDataObject * > *)lua_topointer(L, -1); lua_pop(L, 1);
      lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *)            lua_topointer(L, -1); lua_pop(L, 1);
    lua_pop(L, 1);

    // Data.Sequence, e.g., need to write into Data.Sequence.Item
    if (A)
    { if (A->GetSize()>0) 
        O=A->Get(0);
      else 
      { O = new DICOMDataObject; 
        A->Add(O);
      }
    }

    // write into Data.Storage
    if (O==sd->DDO && strcmp(lua_tostring(L,2), "Storage")==0)
    { sprintf(script, "storage %s", lua_tostring(L,3));
      CallImportConverterN(NULL, O, -2, sd->pszModality, sd->pszStationName, sd->pszSop, sd->patid, sd->PDU, sd->Storage, script);
    }
    // write into Data.Item, Data.Sequence.Item, or Data.Sequence[N].Item
    else if (O) 
    { // write table: creates empty! sequence
      if (lua_istable(L, 3))
        sprintf(script, "set %s to \"\"", lua_tostring(L,2));
      // write nil: deletes item
      else if (lua_isnil(L, 3))
        sprintf(script, "set %s to nil", lua_tostring(L,2));
       // write string: creates item
      else
      { // sprintf(script, "set %s to \"%s\"", lua_tostring(L,2), lua_tostring(L,3));
        char temp[1024];
        int i,j;
        strcpy(temp, lua_tostring(L,3));
	sprintf(script, "set %s to \"", lua_tostring(L,2));
	for (i=0, j=strlen(script); i<strlen(temp); i++)
	{ if (temp[i]=='%')
	  { script[j++] = '%';
	    script[j++] = '%';
	  }
	  else
	    script[j++] = temp[i];
	}
	script[j++] = '"';
	script[j++] = 0;
      }
	    
      CallImportConverterN(NULL, O, -2, sd->pszModality, sd->pszStationName, sd->pszSop, sd->patid, sd->PDU, sd->Storage, script);
    }
    return 0;
  }
  static int luaSeqindex(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    Array < DICOMDataObject * > *A, *A2;
    DICOMDataObject *O, *O2;
    char buf[2000]; buf[0]=0;

    lua_getmetatable(L, 1);
      lua_getfield(L, -1, "ADDO"); A = (Array < DICOMDataObject * > *)lua_topointer(L, -1); lua_pop(L, 1);
      lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *)            lua_topointer(L, -1); lua_pop(L, 1);
    lua_pop(L, 1);

    // Address sequence
    if (A) 
    { // push table for selected element (Data.Sequence[N], allow new elements in sequence)
      if (strstr("Delete", lua_tostring(L,2))) 
      { lua_pushstring(L, "DeleteFromSequence");
	lua_pushcclosure(L, luaSeqClosure, 1);
	return 1;
      }
      if (strstr("Add", lua_tostring(L,2))) 
      { lua_pushstring(L, "AddToSequence");
	lua_pushcclosure(L, luaSeqClosure, 1);
	return 1;
      }
      if (strstr("Serialize", lua_tostring(L,2))) 
      { lua_pushvalue(L, 2);
	lua_pushcclosure(L, luaSeqClosure, 1);
	return 1;
      }     
      if (isdigit(*lua_tostring(L,2))) 
      { int N = lua_tonumber(L, 2);
        if (N >A->GetSize()) 
        { O2=NULL;
          A2=NULL;
        }
        else 
        { if (N==A->GetSize()) 
	  { DICOMDataObject *D = new DICOMDataObject; A->Add(D);
	  }
          O2=A->Get(N);
          A2=NULL;
        }
      }
      // Address default element 0 (Data.Sequence.Item)
      else if (A->GetSize()>0)
	SearchDICOMObject(A->Get(0), lua_tostring(L,2), buf, &A2, &O2);
    }
    // Address Data.Storage
    // Address listed methods by name, e.g., Data:Write('c:\\x.dcm')
    // or get selected item (Data.Item, Data.Sequence[N].Item)
    else if (O) 
    { if (O==sd->DDO && strcmp(lua_tostring(L,2), "Storage")==0) 
      { lua_pushstring(L, sd->Storage);
        return 1;
      }
      else if (strstr("Write|Read|Dump|GetVR|SetVR|GetPixel|SetPixel|GetRow|SetRow|GetColumn|SetColumn|GetImage|SetImage|Script|new|newarray|free|AddImage|Copy|Compress|ListItems|Serialize|DeleteFromSequence", lua_tostring(L,2))) 
      { lua_pushvalue(L, 2);
	lua_pushcclosure(L, luaSeqClosure, 1);
	return 1;
      }
      else 
	SearchDICOMObject(O, lua_tostring(L,2), buf, &A2, &O2);
    }
    else
    { A2=NULL; O2=NULL;
      if (strstr("new|newarray", lua_tostring(L,2))) 
      { lua_pushvalue(L, 2);
	lua_pushcclosure(L, luaSeqClosure, 1);
	return 1;
      }
    }

    // The item is a sequence or sequence element: return a new table (non-gc'd)
    if (A2 || O2)
    { luaCreateObject(L, O2, A2, FALSE);
      return 1;
    }

    // return a read string: empty string coded as \00\01; non-existing element as \00\00
    if (buf[0]) { lua_pushstring(L, buf); return 1; }
    if (buf[1]) { lua_pushstring(L, buf); return 1; }
    return 0;
  }
  static int luaSeqgc(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    DICOMDataObject *O = NULL;
    Array < DICOMDataObject * > *A = NULL;
    int owned;
    lua_getmetatable(L, 1);
      lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_getfield(L, -1, "ADDO");  A = (Array < DICOMDataObject * > *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_getfield(L, -1, "owned"); owned =                             lua_tointeger(L, -1); lua_pop(L, 1);
    lua_pop(L, 1);

    if (owned)
    { if (O && O!=(DICOMDataObject *)(sd->DCO) && O!=sd->DDO) delete O;
      if (A) 
      { for (int i=0; i<A->GetSize(); i++) delete (DICOMDataObject *)(A->Get(i));
        delete A;
      }
    }
    return 0;
  }

  static int luaSeqlen(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    DICOMDataObject *O = NULL;
    Array < DICOMDataObject * > *A = NULL;
    lua_getmetatable(L, 1);
      lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_getfield(L, -1, "ADDO");  A = (Array < DICOMDataObject * > *) lua_topointer(L, -1); lua_pop(L, 1);
    lua_pop(L, 1);

    if (O) lua_pushinteger(L, 1);
    else if (A) lua_pushinteger(L, A->GetSize());
    return 1;
  }

  static int luaSeqToString(lua_State *L)
  { struct scriptdata *sd = getsd(L);
    char text[80];
    DICOMDataObject *O = NULL;
    Array < DICOMDataObject * > *A = NULL;
    lua_getmetatable(L, 1);
      lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_getfield(L, -1, "ADDO");  A = (Array < DICOMDataObject * > *) lua_topointer(L, -1); lua_pop(L, 1);
    lua_pop(L, 1);

    if (O) lua_pushstring(L, "Dicom Data Object");
    else if (A) 
    { sprintf(text, "Dicom sequence with %d entries", A->GetSize());
      lua_pushstring(L, text);
    }
    return 1;
  }

  static void luaCreateObject(lua_State *L, DICOMDataObject *pDDO, Array < DICOMDataObject *> *A, BOOL owned)
  { lua_newuserdata(L, 0); 
    lua_createtable(L, 0, 0);
    lua_pushcfunction(L, luaSeqindex);    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, luaSeqnewindex); lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, luaSeqgc);       lua_setfield(L, -2, "__gc");
    lua_pushcfunction(L, luaSeqlen);      lua_setfield(L, -2, "__len");
    lua_pushcfunction(L, luaSeqToString); lua_setfield(L, -2, "__tostring");
    lua_pushlightuserdata(L, A);          lua_setfield(L, -2, "ADDO");
    lua_pushlightuserdata(L, pDDO);       lua_setfield(L, -2, "DDO");
    lua_pushinteger      (L, (int)owned); lua_setfield(L, -2, "owned");
    lua_setmetatable(L, -2);
  }
}
  
ExtendedPDU_Service globalPDU; // for global script context

/*
* lpack.c
* a Lua library for packing and unpacking binary data
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 29 Jun 2007 19:27:20
* This code is hereby placed in the public domain.
* with contributions from Ignacio Castaño <castanyo@yahoo.es> and
* Roberto Ierusalimschy <roberto@inf.puc-rio.br>.
*/

#define	OP_ZSTRING	'z'		/* zero-terminated string */
#define	OP_BSTRING	'p'		/* string preceded by length byte */
#define	OP_WSTRING	'P'		/* string preceded by length word */
#define	OP_SSTRING	'a'		/* string preceded by length size_t */
#define	OP_STRING	'A'		/* string */
#define	OP_FLOAT	'f'		/* float */
#define	OP_DOUBLE	'd'		/* double */
#define	OP_NUMBER	'n'		/* Lua number */
#define	OP_CHAR		'c'		/* char */
#define	OP_BYTE		'b'		/* byte = unsigned char */
#define	OP_SHORT	'h'		/* short */
#define	OP_USHORT	'H'		/* unsigned short */
#define	OP_INT		'i'		/* int */
#define	OP_UINT		'I'		/* unsigned int */
#define	OP_LONG		'l'		/* long */
#define	OP_ULONG	'L'		/* unsigned long */
#define	OP_LITTLEENDIAN	'<'		/* little endian */
#define	OP_BIGENDIAN	'>'		/* big endian */
#define	OP_NATIVE	'='		/* native endian */

#include <string.h>

#ifndef LUA51EXTERN
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#endif

static void badcode(lua_State *L, int c)
{
 char s[]="bad code `?'";
 s[sizeof(s)-3]=c;
 luaL_argerror(L,1,s);
}

static int doendian(int c)
{
 int x=1;
 int e=*(char*)&x;
 if (c==OP_LITTLEENDIAN) return !e;
 if (c==OP_BIGENDIAN) return e;
 if (c==OP_NATIVE) return 0;
 return 0;
}

static void doswap(int swap,void *p, size_t n)
{
 if (swap)
 {char *a=(char *)p;
  int i,j;
  for (i=0, j=n-1, n=n/2; n--; i++, j--)
  {
   char t=a[i]; a[i]=a[j]; a[j]=t;
  }
 }
}

#define UNPACKNUMBER(OP,T)		\
   case OP:				\
   {					\
    T a;				\
    int m=sizeof(a);			\
    if (i+m>len) goto done;		\
    memcpy(&a,s+i,m);			\
    i+=m;				\
    doswap(swap,&a,m);			\
    lua_pushnumber(L,(lua_Number)a);	\
    ++n;				\
    break;				\
   }

#define UNPACKSTRING(OP,T)		\
   case OP:				\
   {					\
    T l;				\
    int m=sizeof(l);			\
    if (i+m>len) goto done;		\
    memcpy(&l,s+i,m);			\
    doswap(swap,&l,m);			\
    if (i+m+l>len) goto done;		\
    i+=m;				\
    lua_pushlstring(L,s+i,l);		\
    i+=l;				\
    ++n;				\
    break;				\
   }

static int l_unpack(lua_State *L) 		/** unpack(s,f,[init]) */
{
 size_t len;
 const char *s=luaL_checklstring(L,1,&len);
 const char *f=luaL_checkstring(L,2);
 int i=luaL_optnumber(L,3,1)-1;
 int n=0;
 int swap=0;
 lua_pushnil(L);
 while (*f)
 {
  int c=*f++;
  int N=1;
  if (isdigit(*f)) 
  {
   N=0;
   while (isdigit(*f)) N=10*N+(*f++)-'0';
   if (N==0 && c==OP_STRING) { lua_pushliteral(L,""); ++n; }
  }
  while (N--) switch (c)
  {
   case OP_LITTLEENDIAN:
   case OP_BIGENDIAN:
   case OP_NATIVE:
   {
    swap=doendian(c);
    N=0;
    break;
   }
   case OP_STRING:
   {
    ++N;
    if (i+N>len) goto done;
    lua_pushlstring(L,s+i,N);
    i+=N;
    ++n;
    N=0;
    break;
   }
   case OP_ZSTRING:
   {
    size_t l;
    if (i>=len) goto done;
    l=strlen(s+i);
    lua_pushlstring(L,s+i,l);
    i+=l+1;
    ++n;
    break;
   }
   UNPACKSTRING(OP_BSTRING, unsigned char)
   UNPACKSTRING(OP_WSTRING, unsigned short)
   UNPACKSTRING(OP_SSTRING, size_t)
   UNPACKNUMBER(OP_NUMBER, lua_Number)
   UNPACKNUMBER(OP_DOUBLE, double)
   UNPACKNUMBER(OP_FLOAT, float)
   UNPACKNUMBER(OP_CHAR, char)
   UNPACKNUMBER(OP_BYTE, unsigned char)
   UNPACKNUMBER(OP_SHORT, short)
   UNPACKNUMBER(OP_USHORT, unsigned short)
   UNPACKNUMBER(OP_INT, int)
   UNPACKNUMBER(OP_UINT, unsigned int)
   UNPACKNUMBER(OP_LONG, long)
   UNPACKNUMBER(OP_ULONG, unsigned long)
   case ' ': case ',':
    break;
   default:
    badcode(L,c);
    break;
  }
 }
done:
 lua_pushnumber(L,i+1);
 lua_replace(L,-n-2);
 return n+1;
}

#define PACKNUMBER(OP,T)			\
   case OP:					\
   {						\
    T a=(T)luaL_checknumber(L,i++);		\
    doswap(swap,(void *)&a,sizeof(a));			\
    luaL_addlstring(&b,(const char *)&a,sizeof(a));	\
    break;					\
   }

#define PACKSTRING(OP,T)			\
   case OP:					\
   {						\
    size_t l;					\
    const char *a=luaL_checklstring(L,i++,&l);	\
    T ll=(T)l;					\
    doswap(swap,(void *)&ll,sizeof(ll));		\
    luaL_addlstring(&b,(const char *)&ll,sizeof(ll));	\
    luaL_addlstring(&b,a,l);			\
    break;					\
   }

static int l_pack(lua_State *L) 		/** pack(f,...) */
{
 int i=2;
 const char *f=luaL_checkstring(L,1);
 int swap=0;
 luaL_Buffer b;
 luaL_buffinit(L,&b);
 while (*f)
 {
  int c=*f++;
  int N=1;
  if (isdigit(*f)) 
  {
   N=0;
   while (isdigit(*f)) N=10*N+(*f++)-'0';
  }
  while (N--) switch (c)
  {
   case OP_LITTLEENDIAN:
   case OP_BIGENDIAN:
   case OP_NATIVE:
   {
    swap=doendian(c);
    N=0;
    break;
   }
   case OP_STRING:
   case OP_ZSTRING:
   {
    size_t l;
    const char *a=luaL_checklstring(L,i++,&l);
    luaL_addlstring(&b,a,l+(c==OP_ZSTRING));
    break;
   }
   PACKSTRING(OP_BSTRING, unsigned char)
   PACKSTRING(OP_WSTRING, unsigned short)
   PACKSTRING(OP_SSTRING, size_t)
   PACKNUMBER(OP_NUMBER, lua_Number)
   PACKNUMBER(OP_DOUBLE, double)
   PACKNUMBER(OP_FLOAT, float)
   PACKNUMBER(OP_CHAR, char)
   PACKNUMBER(OP_BYTE, unsigned char)
   PACKNUMBER(OP_SHORT, short)
   PACKNUMBER(OP_USHORT, unsigned short)
   PACKNUMBER(OP_INT, int)
   PACKNUMBER(OP_UINT, unsigned int)
   PACKNUMBER(OP_LONG, long)
   PACKNUMBER(OP_ULONG, unsigned long)
   case ' ': case ',':
    break;
   default:
    badcode(L,c);
    break;
  }
 }
 luaL_pushresult(&b);
 return 1;
}

static int l_reshape(lua_State *L)
{ size_t len;
  const char *in = lua_tolstring(L, 1, &len);
  int d1 = lua_tointeger(L, 2);
  int d2 = lua_tointeger(L, 3);
  if (len % d1*d2) return 0;

  const char *out = (const char *)malloc(len);
  char *p, *q;
  if (out==NULL) return 0;

  int steps = len/(d1*d2);
  q = (char *)out;
  for (int j=0; j<d2; j++)
  { p = (char *)in + j*d1;
    for (int i=0; i<steps; i++)
    { memcpy(q, p, d1);
      q += d1;
      p += d1*d2;
    }
  }

  lua_pushlstring(L, out, len);
  return 1;
}

static const luaL_reg R[] =
{
	{"pack",	l_pack},
	{"unpack",	l_unpack},
	{"reshape",	l_reshape},
	{NULL,	NULL}
};

int luaopen_pack(lua_State *L)
{
#ifdef USE_GLOBALS
 lua_register(L,"bpack",l_pack);
 lua_register(L,"bunpack",l_unpack);
#else
 luaL_openlib(L, LUA_STRLIBNAME, R, 0);
#endif
 return 0;
}

// endof pack library

//#ifndef LUA51EXTERN
//extern "C" int luaopen_socket_core(lua_State *L);
//#endif

const char *do_lua(lua_State **L, char *cmd, struct scriptdata *sd)
{ if (!*L) 
  {
#ifdef LUA_5_2
    *L = luaL_newstate();
#else
    *L = lua_open();
#endif
    luaL_openlibs (*L);

    lua_register      (*L, "print",         luaprint);
    lua_register      (*L, "script",        luascript);
    lua_register      (*L, "destroy",       luadestroy);
    lua_register      (*L, "reject",        luareject);
    lua_register      (*L, "retry",         luaretry);
    lua_register      (*L, "get_amap",      luaget_amap);
    lua_register      (*L, "get_sqldef",    luaget_sqldef);
    lua_register      (*L, "dbquery",       luadbquery);
    lua_register      (*L, "sql",           luasql);
    lua_register      (*L, "dicomquery",    luadicomquery);
    lua_register      (*L, "dicomquery2",   luadicomquery2);
    lua_register      (*L, "dicommove",     luadicommove);
    lua_register      (*L, "dicomdelete",   luadicomdelete);
    lua_register      (*L, "newdicomdelete",   luanewdicomdelete);
    lua_register      (*L, "newdicommodify",   luanewdicommodify);
    lua_register      (*L, "newdicomcopy",   luanewdicomcopy);
    lua_register      (*L, "debuglog",      luadebuglog);
    lua_register      (*L, "servercommand", luaservercommand);
    lua_register      (*L, "setpixel",      luasetpixel);
    lua_register      (*L, "getpixel",      luagetpixel);
    lua_register      (*L, "getrow",        luagetrow);
    lua_register      (*L, "setrow",        luasetrow);
    lua_register      (*L, "getcolumn",     luagetcolumn);
    lua_register      (*L, "setcolumn",     luasetcolumn);
    lua_register      (*L, "getimage",      luagetimage);
    lua_register      (*L, "setimage",      luasetimage);
    lua_register      (*L, "readdicom",     luareaddicom);
    lua_register      (*L, "copydicom",     luacopydicom);
    lua_register      (*L, "compressdicom", luacompressdicom);
    lua_register      (*L, "writedicom",    luawritedicom);
    lua_register      (*L, "writeheader",   luawriteheader);
    lua_register      (*L, "newdicomobject",luanewdicomobject);
    lua_register      (*L, "newdicomarray", luanewdicomarray);
    lua_register      (*L, "deletedicomobject",luadeletedicomobject);
    lua_register      (*L, "getvr",         luagetvr);
    lua_register      (*L, "setvr",         luasetvr);
    lua_register      (*L, "HTML",          luaHTML);
    lua_register      (*L, "CGI",           luaCGI);
    lua_register      (*L, "gpps",          luagpps);
    lua_register      (*L, "heapinfo",      luaheapinfo);
    lua_register      (*L, "dictionary",    luadictionary);
    lua_register      (*L, "changeuid",     luachangeuid);
    lua_register      (*L, "changeuidback", luachangeuidback);
    lua_register      (*L, "genuid",        luagenuid);
    lua_register      (*L, "crc",           luacrc);
    lua_register      (*L, "md5",           luamd5);
    lua_register      (*L, "system",        luasystem);
    lua_register      (*L, "sleep",         luasleep);
    lua_register      (*L, "addimage",      luaaddimage);
    lua_register      (*L, "mkdir",         luamkdir);
    lua_register      (*L, "tempfile",      luatempfile);
    lua_register      (*L, "ConvertBinaryData", luaConvertBinaryData);
    lua_register      (*L, "listitems",     lualistitems);
    lua_register      (*L, "serialize",     luaserialize);
    lua_register      (*L, "dicomprint",    luadicomprint);
    lua_register      (*L, "dicomget",      luadicomget);
    lua_register      (*L, "dicomread",     luadicomread);
    lua_register      (*L, "dicomstore",    luadicomstore);
    lua_register      (*L, "dicomecho",     luadicomecho);
    lua_register      (*L, "tickcount",     luatickcount);
    lua_register      (*L, "listoldestpatients", lualistoldestpatients);
    lua_createtable   (*L, 0, 0); 
    lua_createtable   (*L, 0, 0);
    lua_pushcfunction (*L, luaGlobalIndex);    lua_setfield (*L, -2, "__index");
    lua_pushcfunction (*L, luaGlobalNewIndex); lua_setfield (*L, -2, "__newindex");
    lua_setmetatable  (*L, -2);
    lua_setglobal     (*L, "Global");

    lua_createtable(*L, 0, 0); 
    lua_createtable(*L, 0, 0);
    lua_pushcfunction(*L, luaAssociationIndex); lua_setfield(*L, -2, "__index");
    lua_setmetatable(*L, -2);
    lua_setglobal(*L, "Association");  
    
    lua_getfield(*L, LUA_GLOBALSINDEX, "package");
    lua_getfield(*L, -1, "preload");
//#ifndef LUA51EXTERN
//    lua_pushcfunction(*L, luaopen_socket_core);
//    lua_setfield(*L, -2, "socket.core");
//#endif
    lua_pushcfunction(*L, luaopen_pack);
    lua_setfield(*L, -2, "pack");
  }

  if (!sd->DDO) 
  { sd->DDO = dummyddo = new(DICOMDataObject);
  }
  else
    dummyddo = NULL;


  luaCreateObject(*L, (DICOMDataObject *)sd->DCO, NULL, FALSE); lua_setglobal(*L, "Command");  
  luaCreateObject(*L,                    sd->DDO, NULL, FALSE); lua_setglobal(*L, "Data");
  luaCreateObject(*L,                    NULL,    NULL, FALSE); lua_setglobal(*L, "DicomObject");

  lua_pushlightuserdata(*L, sd); 
  lua_setglobal(*L, "scriptdata");

  if (cmd[0]) 
  { if (luaL_loadstring(*L, cmd)) 
    { OperatorConsole.printf("*** lua syntax error %s in '%s'\n", lua_tostring(*L, -1), cmd);
      if (sd->DDO == dummyddo) 
      { sd->DDO = NULL;
        delete dummyddo;
	dummyddo = NULL;
      }
      lua_pop(*L, 1);
      return NULL;
    } 
    else 
    { 
#ifdef LUA_5_2
      if (lua_pcallk(*L, 0, 1, 0, 0, NULL)) 
#else        
      if (lua_pcall(*L, 0, 1, 0))
#endif
      { OperatorConsole.printf("*** lua run error %s in '%s'\n", lua_tostring(*L, -1), cmd);
        if (sd->DDO == dummyddo) 
        { sd->DDO = NULL;
          delete dummyddo;
  	  dummyddo = NULL;
	}
        lua_pop(*L, 1);
        return NULL;
      }
      else
      { if (sd->DDO == dummyddo) 
        { sd->DDO = NULL;
          delete dummyddo;
  	  dummyddo = NULL;
	}
        if (lua_isstring(*L, -1))
          return lua_tostring(*L, -1);
        else
	  lua_pop(*L, 1);
      }
    }
  }
  return NULL;  
}

void lua_setvar(ExtendedPDU_Service *pdu, char *name, char *value)
{ struct scriptdata sd = {pdu, NULL, NULL, 0, NULL, NULL, NULL, NULL, NULL, 0, 0};
  do_lua(&(pdu->L), "", &sd);
  if (value)
    lua_pushstring(pdu->L, value);
  else
    lua_pushstring(pdu->L, "NULL");
  lua_setglobal (pdu->L, name);
}

void lua_getstring(ExtendedPDU_Service *pdu, DICOMCommandObject *dco, DICOMDataObject *ddo, char *cmd, char *result)
{ char script[1000];
  struct scriptdata sd1 = {pdu, dco, ddo, -1, NULL, NULL, NULL, NULL, NULL, 0, 0};
  strcpy(script, "return ");
  strcat(script, cmd);
  strcpy(result, do_lua(&(pdu->L), script, &sd1));
}

// support stuff for ImportConverters

int DcmConvertPixelData(DICOMDataObject*pDDO, bool bConvertToMono, bool	bCrop,
  			int iStartX, int iEndX, int iStartY, int iEndY,
  			float fPixelSizeX, float fPixelSizeY, float fPixelSizeZ);
			
static BOOL import_c_init[MAXExportConverters][MAXExportConverters];
static CRITICAL_SECTION import_critical[MAXExportConverters][MAXExportConverters];
static ExtendedPDU_Service import_forward_PDU[MAXExportConverters * MAXExportConverters]; // for direct forwards, not script context
static time_t import_forward_PDU_time[MAXExportConverters][MAXExportConverters];
static BOOL import_forward_active[MAXExportConverters][MAXExportConverters];
static CRITICAL_SECTION count_critical;
static CRITICAL_SECTION dolua_critical;
extern int MaybeScrub(DICOMDataObject* pDDO, DICOMCommandObject* pDCO);

static BOOL WINAPI import_forward_PDU_close_thread(char *folder)
{ while (TRUE)
  { char szTemp[128];
    char szRootSC[64];

    MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile);
    MyGetPrivateProfileString(szRootSC, "ForwardAssociationCloseDelay", "5",   szTemp, 128, ConfigFile);
    int closedelay   = atoi(szTemp);
    time_t t = time(NULL);

    for (int i=0; i<MAXExportConverters; i++)
      for (int j=0; j<MAXExportConverters; j++)
        if (t-import_forward_PDU_time[i][j] > closedelay && !import_forward_active[i][j] && import_c_init[i][j])
	  if (import_forward_PDU[i*MAXExportConverters+j].Link.Connected) 
	    import_forward_PDU[i*MAXExportConverters+j].Close();
	    
    Sleep(1000);
  }
  return TRUE;
}

void Startimport_forward_PDU_close_thread(void)
{ static BOOL started = false;
  if (started) return;
  started = TRUE;

  #ifdef WIN32
  unsigned long ThreadID;
  CreateThread(NULL, 2097152, (LPTHREAD_START_ROUTINE) import_forward_PDU_close_thread, NULL, 0, &ThreadID);
#else
  pthread_t ThreadID;
  pthread_create(&ThreadID, NULL, (void*(*)(void*))import_forward_PDU_close_thread, (void *)NULL);
  pthread_detach(ThreadID);
#endif
}

// operates synchronously on image BEFORE it is stored in the database
// For general scripting use:
// CallImportConverterN (pDDO,                 -1,    NULL,              NULL,                 NULL,         NULL,        NULL,                     NULL,          script);
// use -2 to suppress status printing, -3 to also suppress error printing

int CallImportConverterN(DICOMCommandObject *DCO, DICOMDataObject *DDO, int N, char *pszModality, char *pszStationName, char *pszSop, char *patid, ExtendedPDU_Service *PDU, char *Storage, char *Script)
{ char		szRootSC[64];
  char		szEntry[64];
  char		szTemp[66];
  char		szExecName[512], szNext[512];
  char		szExecModality[66];
  char		szExecStationName[66];
  char		ImportCalledAE[18], ImportCallingAE[18], Called[18], Calling[18];
  int           rc, part, skipping;
  char		ininame[30];
  static char 	ForwardUIDS[66 * MAXExportConverters * MAXExportConverters];
  char		pszFileName[512];

  char calling[20];
  char called[20];

  memset(calling, 0, 20);
  memset(called, 0, 20);

  if (PDU)
  { strcpy(calling, (char *)(((AAssociateAC *)PDU)->CallingApTitle));
    if (calling[0]>32)
      while (calling[strlen(calling)-1]==' ') calling[strlen(calling)-1] = 0;

    strcpy(called, (char *)(((AAssociateAC *)PDU)->CalledApTitle));
    if (called[0]>32)
      while (called[strlen(called)-1]==' ') called[strlen(called)-1] = 0;
  }

  int threadnum = 0;
  if (PDU) threadnum = PDU->ThreadNum;
  pszFileName[0]=0;

  if (DDO==NULL)
  { if (N>=0 && N<MAXExportConverters) 
      for (int i=0; i<MAXExportConverters; i++) import_forward_PDU[N*MAXExportConverters+i].Close();
    return FALSE;
  }

  if      (N==1000) strcpy(ininame, "Query");
  else if (N==1100) strcpy(ininame, "WorkListQuery");
  else if (N==1200) strcpy(ininame, "Retrieve");
  else if (N==1300) strcpy(ininame, "RetrieveResult");
  else if (N==1500) strcpy(ininame, "QueryResult");
  else if (N==1600) strcpy(ininame, "ModalityWorkListQueryResult");
  else if (N==1700) strcpy(ininame, "MergeSeries");
  else if (N==1800) strcpy(ininame, "MergeStudies");
  else if (N==1900) strcpy(ininame, "Archive");
  else if (N==2000) strcpy(ininame, "MoveDevice");
  else if (N==2100) strcpy(ininame, "RejectedImage");
  else if (N==2200) strcpy(ininame, "RejectedImageWorkList");
  else if (N==2300) strcpy(ininame, "VirtualServerQuery");
  else if (N==2400) strcpy(ininame, "VirtualServerQueryResult");
  else if (N==2500) strcpy(ininame, "Regen");
  else if (N==2600) strcpy(ininame, "RegenFail");
  else if ((N/100)==14) strcpy(ininame, "Compression");	// 1400-1499
  else              strcpy(ininame, "Import");

  if (N>=0)
  { N = N%100;
    sprintf(szEntry, "%sConverter%d", ininame, N);

    if (!MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile)) return FALSE;

    MyGetPrivateProfileString("lua", szEntry, "", szExecName, 512, ConfigFile);
    if (szExecName[0])
    { struct scriptdata sd1 = {PDU, DCO, DDO, N, pszModality, pszStationName, pszSop, patid, Storage, 0, 0};
      sd1.rc = 1;
      do_lua(&(PDU->L), szExecName, &sd1);
      //if (!MyGetPrivateProfileString(szRootSC, szEntry, "", szExecName, 512, ConfigFile)) 
      return sd1.rc;
    }
  
    /* filter using modality, stationname, calling and called */

    if (pszModality)
    { sprintf(szEntry, "%sModality%d", ininame, N);
      if (!MyGetPrivateProfileString(szRootSC, szEntry, "*", szExecModality, 64, ConfigFile)) return FALSE;
      if (!match(pszModality, szExecModality)) return FALSE;
    }

    if (pszStationName)
    { sprintf(szEntry, "%sStationName%d", ininame, N);
      if (!MyGetPrivateProfileString(szRootSC, szEntry, "*", szExecStationName, 64, ConfigFile)) return FALSE;
      if (!match(pszStationName, szExecStationName)) return FALSE;
    }

    if (calling)
    { sprintf(szEntry, "%sCallingAE%d", ininame, N);
      if (!MyGetPrivateProfileString(szRootSC, szEntry, "*", ImportCallingAE, 18, ConfigFile)) return FALSE;
      if (!match(calling, ImportCallingAE)) return FALSE;
    }

    if (called)
    { sprintf(szEntry, "%sCalledAE%d", ininame, N);
      if (!MyGetPrivateProfileString(szRootSC, szEntry, "*", ImportCalledAE, 18, ConfigFile)) return FALSE;
      if (!match(called, ImportCalledAE)) return FALSE;
    }

    /* get the import converter name */

    if (called && calling)
    { sprintf(szEntry, "%sConverter%d", ininame, N);
      if (!MyGetPrivateProfileString(szRootSC, szEntry, "", // changed default to ""
            szExecName, 512, ConfigFile)) return FALSE;
    }
    else
    { strcpy(szExecName, Script);
    }
  }
  else
    strcpy(szExecName, Script);

  /* only import converters using %x syntax are allowed */

  rc       = 1;	// status for if statement, destroy and stop
  part     = 0;
  skipping = 0;

  while(1)
  { char line[1000], vr1[256];
    int i=0, l, L, len1;
    unsigned int g, e;
    char *p1;//, *q;
    VR *pVR, *newVR;
    DICOMDataObject DDO2;
    Array < DICOMDataObject * > ADDO;
    char level1;

    szNext[0] = 0;

    if (skipping || rc==4)			// {} block being skipped
    { if      (szExecName[0]=='{') skipping++, rc=0;
      else if (szExecName[0]=='}') skipping--, rc=0;
    }
        
    // find ; not in string: splits import commands
    int instring=0;
    L = strlen(szExecName);
    for (int k=0; k<L; k++) 
    { if (szExecName[k]=='"') instring = !instring;
      if (szExecName[k]==';' && instring==0)
      { szExecName[k] = 0;			// split command
        while (szExecName[k+1]==' ') k++;	// skip spaces after ;
        strcpy(szNext, szExecName+k+1);		// remaining for next
        break;
      }
      if (memicmp(szExecName+k, "lua:", 4)==0 && instring==0)
      { strcpy(szNext, "");		// remaining for next
	break;
      }
    }

    /* substitute %x variables and remove possible { and } at begin of statement */

    line[0] = 0;
    instring = 0;
    L = strlen(szExecName);
    while (i<L)
    { if      (i==0 && szExecName[i] == '{') {i++; while(szExecName[i]==' ')i++;}
      else if (i==0 && szExecName[i] == '}') {i++; while(szExecName[i]==' ')i++;}
      else if (szExecName[i] == '"')
      { if (instring && szExecName[i+1]=='"')
        { strcat(line, """"); 
          i+=2;
        }
        else
        { l = strlen(line);
          line[l]   = szExecName[i++];
          line[l+1] = 0;
        }
        instring = !instring;
      }
      else if (szExecName[i] == '%')
      { char *ps = line + strlen(line);

        switch(tolower(szExecName[i+1]))
        { case 'f': if (pszFileName[0]==0)
	            { Database DB; char dum[512], mag[64];
		      if (DB.Open ( DataSource, UserName, Password, DataHost ))
		      { GenerateFileName(DDO, mag, dum, pszFileName, TRUE, 0, called, calling, &DB);
		        if (memcmp(pszFileName, "lua:", 4)==0) lua_getstring(PDU, NULL, DDO, pszFileName+4, pszFileName);
	                GetPhysicalDevice(mag, dum);
	                strcat(dum, pszFileName);
			strcpy(pszFileName, dum);
		      }
	            }
	            strcat(line, pszFileName);		// %f=filename
                    break;
          case 'b': if (pszFileName[0]==0)
	            { Database DB; char dum[512], mag[64];
		      if (DB.Open ( DataSource, UserName, Password, DataHost ))
		      { GenerateFileName(DDO, mag, dum, pszFileName, TRUE, 0, called, calling, &DB);
		        if (memcmp(pszFileName, "lua:", 4)==0) lua_getstring(PDU, NULL, DDO, pszFileName+4, pszFileName);
	                GetPhysicalDevice(mag, dum);
	                strcat(dum, pszFileName);
			strcpy(pszFileName, dum);
		      }
	            }
	            p1 = strrchr(pszFileName, PATHSEPCHAR);
                    if (p1)				// %b=file base name
                    { strcat(line, p1+1);
                      p1 = strrchr(line, '.');
                      if (p1) *p1=0;
                    }
                    break;
          case 'p': if (pszFileName[0]==0)
	            { Database DB; char dum[512], mag[64];
		      if (DB.Open ( DataSource, UserName, Password, DataHost ))
		      { GenerateFileName(DDO, mag, dum, pszFileName, TRUE, 0, called, calling, &DB);
		        if (memcmp(pszFileName, "lua:", 4)==0) lua_getstring(PDU, NULL, DDO, pszFileName+4, pszFileName);
	                GetPhysicalDevice(mag, dum);
	                strcat(dum, pszFileName);
			strcpy(pszFileName, dum);
		      }
	            }
	            strcat(line, pszFileName);		// %p=file path
                    p1 = strrchr(line, PATHSEPCHAR);
                    if (p1) *p1=0;
                    break;

	  case 'm': if (pszModality)
                      strcat(line, pszModality);   // %m=modality
                    else
		    { char result[256];
                      SearchDICOMObject(DDO, "0008,0060", result);
                      strcat(line, result);
                    }
                    break;
          case 's': if (pszStationName)
                      strcat(line, pszStationName);// %s=stationname
                    else
		    { char result[256];
                      SearchDICOMObject(DDO, "0008,1010", result);
                      strcat(line, result);
                    }
                    break;
          case 'o': if (pszSop)
                      strcat(line, pszSop);	  // %o=object instance
                    else
		    { char result[256];
                      SearchDICOMObject(DDO, "0008,0018", result);
                      strcat(line, result);
                    }
                    break;
          case 'i': //if (patid)
                    //  strcat(line, patid);	  // %i=patient ID
                    //else
		    { char result[256];
                      SearchDICOMObject(DDO, "0010,0020", result);
                      strcat(line, result);
                    }
                    break;
          case 'u': Calling[0]=0;
		    if (calling) strcpy(Calling, calling);
		    if (Calling[0]>32)
        	      while (strlen(Calling)>0 && Calling[strlen(Calling)-1]==' ') Calling[strlen(Calling)-1] = 0;
		    strcat(line, Calling);	// %u=SCU
                    break;
          case 'c': Called[0]=0;
		    if (called) strcpy(Called, called);
		    if (Called[0]>32)
        	      while (strlen(Called)>0 && Called[strlen(Called)-1]==' ') Called[strlen(Called)-1] = 0;
		    strcat(line, Called);	// %c=calledAE = scp
                    break;
          case 'd': time_t TimeOfDay;		// %d=date and time
		    char TimeString[128], buf[64];
		    UNUSED_ARGUMENT(buf);//Stop gcc4.2 warning bcb
		    TimeOfDay = time(NULL);
		    strcpy(TimeString, ctime_r(&TimeOfDay, buf));
		    TimeString[strlen(TimeString)-1] = '\0';
                    strcat(line, TimeString);
                    break;
          case 'n': strcat(line, "\n");		// %n=newline
                    break;
          case 't': strcat(line, "\t");		// %t=tab
                    break;
          case '%': strcat(line, "%");		// %%=%
                    break;
          case '^': strcat(line, "^");		// %^=^
                    break;
          case '~': strcat(line, "~");		// %~=~
                    break;
          case '[': strcat(line, "[");		// %[=[
                    break;
          case 'x': 				// %x, %y, %z are general purpose variables
          case 'y': 
          case 'z': 
                    pVR = PDU->VariableVRs[tolower(szExecName[i+1])-'x'];
                    if (!pVR) pVR = new VR;
                    PDU->VariableVRs[tolower(szExecName[i+1])-'x'] = pVR;
	            strncpy(vr1, (char*)pVR->Data, pVR->Length);
	            vr1[pVR->Length] = 0;
	            len1 = pVR->Length - 1;
                    while(len1>0)
                    { if (vr1[len1] == ' ')
	                vr1[len1] = 0;
                      else
                        break;
                      len1--;
                    }
                    strcat(line, vr1);
                    break;
          case 'v': 				// %Vxxxx,yyyy=any vr (group and element must have 4 digits)
		    char result[256];		// %V*xxxx,yyyy=any vr in any sequence
		    i += SearchDICOMObject(DDO, szExecName+i+2, result);
                    strcat(line, result);
                    break;

	  case 'a': {				// %A as %V but then writes CRC32
		    char result2[256];
		    i += SearchDICOMObject(DDO, szExecName+i+2, result2);
		    unsigned int crc = ComputeCRC(result2, strlen(result2));
		    sprintf(result2, "%u", crc);
                    strcat(line, result2);
                    break;
		    }

	  case 'e': {				// %Exxxx,yyyy= changed UID for any UID
		    char result2[256], result3[256];
		    i += SearchDICOMObject(DDO, szExecName+i+2, result2);
		    ChangeUID(result2, "percente", result3, NULL, NULL);
                    strcat(line, result3);
                    break;
		    }

	  case 'r': {				// %Rxxxx,yyyy= old UID for any changed UID
		    char result2[256], result3[256];
		    i += SearchDICOMObject(DDO, szExecName+i+2, result2);
		    ChangeUIDBack(result2, result3, NULL, NULL, NULL);
                    strcat(line, result3);
                    break;
		    }

	  case 'g': {				// %g generate UID
		    char result2[256];
		    GenUID(result2);
                    strcat(line, result2);
                    break;
		    }

	  case 'q': level1 = szExecName[i+2];	// %QXxxxx,yyyy=from db (group and element must have 4 digits)
                    g = htoin(szExecName+i+3, 4);
	            e = htoin(szExecName+i+8, 4);

		    switch(level1)
                    { case 'p':
                      case 'P': 		// %QPxxxx,yyyy=query from patient db on patid
		        SetStringVR(&pVR, 0x0008, 0x0052, "PATIENT"); 
	                DDO2.Push(pVR); 
		        pVR = DDO->GetVR(0x0010, 0x0020);
			if (pVR && pVR->Length)
			{ newVR = new VR(pVR->Group, pVR->Element, pVR->Length, (BOOL) TRUE);
			  memcpy(newVR->Data, pVR->Data, pVR->Length);
	                  DDO2.Push(newVR);
                          SetStringVR(&pVR, g, e, ""); 
	                  DDO2.Push(pVR);
                          QueryOnPatient (&DDO2, &ADDO);
                        }
			break;
		      case 's':
                      case 'S': 		// %QSxxxx,yyyy=from study db on patid and study uid
		        SetStringVR(&pVR, 0x0008, 0x0052, "STUDY"); 
	                DDO2.Push(pVR); 
		        pVR = DDO->GetVR(0x0010, 0x0020);
			if (pVR && pVR->Length)
			{ newVR = new VR(pVR->Group, pVR->Element, pVR->Length, (BOOL) TRUE);
  			  memcpy(newVR->Data, pVR->Data, pVR->Length);
	                  DDO2.Push(newVR);
		          pVR = DDO->GetVR(0x0020, 0x000d);
			  if (pVR && pVR->Length)
			  { newVR = new VR(pVR->Group, pVR->Element, pVR->Length, (BOOL) TRUE);
			    memcpy(newVR->Data, pVR->Data, pVR->Length);
	                    DDO2.Push(newVR);
                            SetStringVR(&pVR, g, e, ""); 
	                    DDO2.Push(pVR);
                            QueryOnStudy (&DDO2, &ADDO);
                          }
                        }
			break;
		      case 'e':
                      case 'E': 		// %QExxxx,yyyy=from series db on patid, study and series uid
		        SetStringVR(&pVR, 0x0008, 0x0052, "SERIES"); 
	                DDO2.Push(pVR); 
		        pVR = DDO->GetVR(0x0010, 0x0020);
			if (pVR && pVR->Length)
			{ newVR = new VR(pVR->Group, pVR->Element, pVR->Length, (BOOL) TRUE);
			  memcpy(newVR->Data, pVR->Data, pVR->Length);
	                  DDO2.Push(newVR);
		          pVR = DDO->GetVR(0x0020, 0x000d);
			  if (pVR && pVR->Length)
			  { newVR = new VR(pVR->Group, pVR->Element, pVR->Length, (BOOL) TRUE);
			    memcpy(newVR->Data, pVR->Data, pVR->Length);
	                    DDO2.Push(newVR);
		            pVR = DDO->GetVR(0x0020, 0x000e);
			    if (pVR && pVR->Length)
                            { newVR = new VR(pVR->Group, pVR->Element, pVR->Length, (BOOL) TRUE);
			      memcpy(newVR->Data, pVR->Data, pVR->Length);
	                      DDO2.Push(newVR);
                              SetStringVR(&pVR, g, e, ""); 
	                      DDO2.Push(pVR);
                              QueryOnSeries (&DDO2, &ADDO);
                            }
                          }
                        }
			break;
                      case 'w':
                      case 'W': 		// %QWxxxx,yyyy=from worklist db on accession number
		        SetStringVR(&pVR, 0x0008, 0x0052, "MODALITYWORKLIST"); 
	                DDO2.Push(pVR); 
		        pVR = DDO->GetVR(0x0008, 0x0050);
                        if (pVR && pVR->Length)
			{ newVR = new VR(pVR->Group, pVR->Element, pVR->Length, (BOOL) TRUE);
  			  memcpy(newVR->Data, pVR->Data, pVR->Length);
	                  DDO2.Push(newVR);
                          SetStringVR(&pVR, g, e, ""); 
	                  DDO2.Push(pVR);
                          QueryOnModalityWorkList (&DDO2, &ADDO);
                        }
			break;

                      default:
                      		 		// %QXxxxx,yyyy=from aliasfileqX.txt (tab sep) on xxxx,yyyy
			char result2[512];
			FILE *f;
			char cmd[1024];
			char *q = line + strlen(line);

			i += SearchDICOMObject(DDO, szExecName+i+3, result2) - 9;  // fix for +10 later
			strcpy(q, result2);	// default return original data

			sprintf(cmd, "aliasfileq%c.txt", level1);
			f = fopen(cmd, "rt");
			if (f)
			{ while(fgets(cmd, sizeof(cmd), f) != NULL)
        		  { if (cmd[strlen(cmd)-1]=='\n') cmd[strlen(cmd)-1]=0;
        		    p1 = strchr(cmd, '\t');
        		    if (p1) 
			    { *p1=0;
        		      if (stricmp(result2, cmd)==0)
			      { strcpy(q, p1+1);
                                //i += strlen(p1+1) - strlen(result2);	// adjust lenght
                                break;
			      }
                            }
          		  }
			  fclose(f);
			}
			break;
                    }
                    
                    if (ADDO.GetSize()>0) pVR = ADDO.Get(0)->GetVR(g, e);
		    else  		  pVR = NULL;

                    if (pVR) 
	            { strncpy(vr1, (char*)pVR->Data, pVR->Length);
	              vr1[pVR->Length] = 0;
	              len1 = pVR->Length - 1;
                      while(len1>0)
                      { if (vr1[len1] == ' ')
	                  vr1[len1] = 0;
                        else
                          break;
                        len1--;
                      }
                      strcat(line, vr1);
	            }

		    for (unsigned int j=0; j<ADDO.GetSize(); j++)
		    { while((pVR=ADDO.Get(j)->Pop()))
		        delete pVR;
		      delete ADDO.Get(j);
                    }

                    i+=10;
                    break;
        }
        i += 2;

	// get substring of variable [start,end] or [,fromend]
        if (szExecName[i]=='[')
        { int a=0, b=-1;
	  if (szExecName[i+1]==',')
	    sscanf(szExecName+i, "[,%d]", &b), a=-1;
	  else
	    sscanf(szExecName+i, "[%d,%d]", &a, &b);
          for (;i<L;i++) if (szExecName[i-1]==']') break;
          if (a>(signed int)strlen(ps)) a=strlen(ps);
          if (a<0) { a = strlen(ps)-b; b = strlen(ps); }
	  if (a<0) a = 0;
          if (b>(signed int)strlen(ps)) b=strlen(ps);
          if (b>=a) {memmove(ps, ps+a, b-a+1); ps[b-a+1]=0;}
        }

	// convert to uppercase
        if (szExecName[i]=='^')
        { i++;
	  strupr(ps);
        }
	// convert to lowercase
        else if (szExecName[i]=='~')
        { i++;
	  strlwr(ps);
        }
      }
      else
      { l = strlen(line);
        line[l]   = szExecName[i++];
        line[l+1] = 0;
      }
    }

    if (skipping)	// {} block being skipped
    { if (N >= -1) SystemDebug.printf("%sconverter%d.%d skipped\n", ininame, N, part);
    }

    else if (rc==4)	// ifxxxx statement causes a skip
    { rc = 0;
      if (N >= -1) SystemDebug.printf("%sconverter%d.%d not executed because of previous statement\n", ininame, N, part);
    }

    /* IMPORT converter: direct forward of received object */

    else if (memicmp(line, "forward to", 10)==0 ||
        memicmp(line, "forward compressed", 18)==0
       )
    { char host[512], port[64], compress[64], dummy[64], Level[64], Temp[66];
      char *p, *q, *c;
      RunTimeClassStorage	RTCStorage;
      UID			AppUID ("1.2.840.10008.3.1.1.1");
      UID			iUID, uid;
      VR			*vr;
      int			offset = 11, level, channel;
      DICOMDataObject		*pDDO;
      BOOL		        StripGroup2;
      BYTE			org[512], dest[512];
      char			script[512];
      ExtendedPDU_Service	*PDU;

      if (N<0 || N>=MAXExportConverters) N = MAXExportConverters-1;  // typically from lua
      PDU = &import_forward_PDU[N*MAXExportConverters];
      char *ForwardLastUID = ForwardUIDS + 66 * MAXExportConverters * N;
      pDDO = MakeCopy(DDO);

      // preprocess the forwarder string
      if (memicmp(line, "forward compressed as ", 22)==0) offset = 28;
  					       // xx to	where xx = 'is', 'un', 'n0'..'n4', or 'jl', 'jk', 'j0'..'j6'
					       // xxNN same as above but NN is lossy quality

      /// offer transfer syntax (compression) from forward string
      compress[0]=0;
      if (offset==28)
      { compress[0] = line[22];
        compress[1] = line[23];
        compress[2] = 0;
        if (line[24] != ' ') 
        { compress[2] = line[24];
          compress[3] = line[25];
          compress[4] = 0;
          offset = 30;
	}
      }

      // get the script clause (to process), must be last
      script[0] = 0;
      p = strstr(line + offset, " script ");
      if (p)
      { strcpy((char *)script, p+8);
        p[8] = 0;
      }

      // get the org clause (originator)
      strcpy((char *)org, (char *)MYACRNEMA);
      p = strstr(line + offset, " org ");
      if (p)
      { strcpy((char *)org, p+5);
        q = strchr((char *)org, ' '); if (q) *q=0;
      }

      // get the dest clause (called AE)
      dest[0] = 0;
      p = strstr(line + offset, " dest ");
      if (p)
      { strcpy((char *)dest, p+6);
        q = strchr((char *)dest, ' '); if (q) *q=0;
      }

      // get the channel clause (to keep multiple associations open)
      channel = part;
      p = strstr(line + offset, " channel ");
      if (p)
      { if (p[9]=='*') channel = threadnum % MAXExportConverters;
        else channel = atoi(p+9);
        if (channel<0 || channel>=MAXExportConverters) channel = 0;
      }

      // get the "to AE" or "to host:port" clause
      q = strchr(line+offset, ' '); if (q) *q=0;
      c=compress; 
      if (compress[0]) c=dummy;
      if(!GetACRNema(line+offset, (char *)host, (char *)port, (char *)c))
      { strcpy(host, line+offset);
        p = strchr(host, ':');
        if (p) 
        { *p=0;
          strcpy(port, p+1);
        }
        else 
          strcpy(port, "5678");
	if (offset!=28) strcpy(compress, "UN");
      }

      if (N >= -1) OperatorConsole.printf("%sConverter%d.%d: forwarded object to %s\n", ininame, N, channel, line+offset);

      // get UID at selected ForwardAssociationLevel into szTemp
      vr = NULL;
      level = 0;
      MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile);
      MyGetPrivateProfileString(szRootSC, "ForwardAssociationLevel", "STUDY", Level, 64, ConfigFile);
      if      (strncmp(Level, "PATIENT", 7)==0) level=1;
      else if (strncmp(Level, "STUDY",   5)==0) level=2;
      else if (strncmp(Level, "SERIES",  6)==0) level=3;
      else if (strncmp(Level, "IMAGE",   5)==0) level=4;
      else if (strncmp(Level, "SOPCLASS",8)==0) level=5, PDU[channel].ClearAbstractSyntaxs();
      else if (strncmp(Level, "GLOBAL",  6)==0) level=6;
      szTemp[0] = 0;
      if (level==1) vr = pDDO->GetVR(0x0010, 0x0020);
      if (level==2) vr = pDDO->GetVR(0x0020, 0x000d);
      if (level==3) vr = pDDO->GetVR(0x0020, 0x000e);
      if (level==4) vr = pDDO->GetVR(0x0008, 0x0018);
      if (level==5) vr = pDDO->GetVR(0x0008, 0x0016);
      if (vr) memcpy(szTemp, vr->Data, vr->Length);
      if (vr) szTemp[vr->Length]=0;

      // get sopclass (to check whether it is accepted at the current connection)
      vr = pDDO -> GetVR(0x0008, 0x0016);
      if (!vr)
      { if (N >= -2) OperatorConsole.printf("*** %sConverter%d.%d: Forward failed because SopClass is missing\n", ininame, N, channel);
      }

      if (import_c_init[N][channel]==FALSE)
      { InitializeCriticalSection(import_critical[N]+channel);
        import_forward_PDU_time[N][channel] = time(NULL);
        import_forward_active[N][channel] = FALSE;
        import_c_init[N][channel]=TRUE;
      }
      import_forward_active[N][channel] = TRUE;
      EnterCriticalSection(import_critical[N]+channel);
      import_forward_PDU_time[N][channel] = time(NULL);

      // does the existing link accept images of this type - if not then hang up to force a reconnect
      // also hang up when UID at ForwardAssociationLevel changes to start a new association
      if (PDU[channel].Link.Connected)
      { SetUID ( iUID, vr );
        if (!PDU[channel].IsAbstractSyntaxAccepted(iUID) || strcmp(ForwardLastUID+channel*66, szTemp)!=0 )
        { //if (N >= -1) OperatorConsole.printf("!!! ExportConverter%d.%d: attempt to reconnect %s \n", N, channel, szTemp);
	  OperatorConsole.printf("%sConverter%d.%d: Forward association closed by %s\n", 
	    ininame, N, channel, strcmp(ForwardLastUID+channel*66, szTemp)!=0?Level:"SOPCLASS");
          MyGetPrivateProfileString(szRootSC, "ForwardAssociationRelease", "1", Temp, 64, ConfigFile);
          if (atoi(Temp)) PDU[channel].Close();
	  else            PDU[channel].Link.Close();
        }
      }

      // for new check of UID at ForwardAssociationLevel
      strcpy(ForwardLastUID+channel*66, szTemp);

      // (re)connect
      if (!PDU[channel].Link.Connected)
      { PDU[channel].AttachRTC(&VRType);
        PDU[channel].SetRequestedCompressionType(compress);

        PDU[channel].SetApplicationContext ( AppUID );
        PDU[channel].SetLocalAddress ( org );
	if (dest[0])
          PDU[channel].SetRemoteAddress ( (unsigned char *)(dest) );
	else
          PDU[channel].SetRemoteAddress ( (unsigned char *)(line+offset) );
        PDU[channel].SetTimeOut(TCPIPTimeOut);

        vr = pDDO -> GetVR(0x0008, 0x0016);
        SetUID ( iUID, vr );
        PDU[channel].AddAbstractSyntax ( iUID );		// adds type of this image to presentation contexts
  
        uid.Set("1.2.840.10008.1.1");		// This one should always accept: verification
        PDU[channel].AddAbstractSyntax(uid);		// assures connect will not return FALSE because image is not accepted
  
        if (!PDU[channel].Connect((unsigned char *)&host, (unsigned char *)&port))
        { if (N >= -2) OperatorConsole.printf("*** %sConverter%d.%d: Forward failed to connect to host %s\n", ininame, N, channel, line+offset);
        }
	else
	{ vr = pDDO -> GetVR(0x0008, 0x0016);
	  SetUID ( iUID, vr );
	  if (!PDU[channel].IsAbstractSyntaxAccepted(iUID))
	  { if (N >= -2) OperatorConsole.printf("*** %sConverter%d.%d: DICOM server %s does not accept type of forwarded image\n", ininame, N, channel, line+offset);
	    PDU[channel].Close();
	  }
	}
      }

      if (PDU[channel].Link.Connected)
      { // process data
        if (script[0])
          CallImportConverterN(DCO, pDDO, -1, NULL, NULL, NULL, NULL, PDU, NULL, script);
        import_forward_PDU_time[N][channel] = time(NULL);
  
        // recompress data to be forwarded here according to accepted compression mode; strip group 2 unless "as" or "is"
	p = PDU[channel].GetAcceptedCompressionType(iUID);
	StripGroup2 = memicmp(p, "as", 2)!=0 && memicmp(p, "is", 2)!=0;
	recompress(&pDDO, p, "", StripGroup2, PDU);
        import_forward_PDU_time[N][channel] = time(NULL);

	RTCStorage.SetUID(iUID);

	if (!RTCStorage.Write(PDU+channel, pDDO))
	{ if (N >= -2) OperatorConsole.printf("*** %sConverter%d.%d: Forward failed to send DICOM image to %s\n", ininame, N, channel, line+offset);
  	  PDU[channel].Close();
	}
	else
	{ ImagesForwarded++;

	  if (level==4) 
	  { MyGetPrivateProfileString(szRootSC, "ForwardAssociationRelease", "1", Temp, 64, ConfigFile);
	    if (atoi(Temp)) PDU[channel].Close();
	    else	    PDU[channel].Link.Close();
	  }
	}
      }
      import_forward_PDU_time[N][channel] = time(NULL);
      import_forward_active[N][channel] = FALSE;
      LeaveCriticalSection(import_critical[N]+channel);
      Startimport_forward_PDU_close_thread();

      delete pDDO;	// write was destructive
      pDDO = NULL;
    }

    /* converter: write "string" to file - uses text file, use %n for newline */

    else if (memicmp(line, "write \"", 7)==0)
    { char string[256];
      char *file;
      FILE *f;

      strcpy(string, line+7);
      p1 = strstr(string, "\" to ");
      if (!p1) p1 = strstr(string, "\" TO ");
      if (p1)
      { *p1=0;

        if (N >= -1) OperatorConsole.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);

        file = p1+5;
        f = fopen(file, "wt");
        if (f)
        { fputs(string, f);
          fclose(f);
        }
        else
          if (N >= -2) OperatorConsole.printf("*** %sconverter%d.%d: Failed to write to file %s\n", ininame, N, part, file);
      }
    }

    /* converter: append "string" to file - uses text file, use %n for newline */

    else if (memicmp(line, "append \"", 8)==0)
    { char string[256];
      char *file;
      FILE *f;

      strcpy(string, line+8);
      p1 = strstr(string, "\" to ");
      if (!p1) p1 = strstr(string, "\" TO ");
      if (p1)
      { *p1=0;

        if (N >= -1) OperatorConsole.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);

        file = p1+5;
        f = fopen(file, "at");
        if (f)
        { fputs(string, f);
          fclose(f);
        }
        else
          if (N >= -2) OperatorConsole.printf("*** Importconverter%d.%d: Failed to append to file %s\n", N, part, file);
      }
    }

    // converter: set xxxx,yyyy to "items like %Vxxxx,yyyy or %QP0010,0030"
    // converter: set PatientID to "items like %VPatientID or %QP0010,0030"
    // converter: set xxxx,yyyy if "items like %Vxxxx,yyyy or %QP0010,0030"
    // converter: set xxxx,yyyy/xxxx,yyyy to "items like %Vxxxx,yyyy or %QP0010,0030"
    // converter: set xxxx,yyyy/xxxx,yyyy if "items like %Vxxxx,yyyy or %QP0010,0030"
    // converter: set xxxx,yyyy.1/xxxx,yyyy to "items like %Vxxxx,yyyy or %QP0010,0030"
    // converter: set xxxx,yyyy.1/xxxx,yyyy if "items like %Vxxxx,yyyy or %QP0010,0030"
    // converter: set xxxx,yyyy.*/xxxx,yyyy to "items like %Vxxxx,yyyy or %QP0010,0030"
    // converter: set xxxx,yyyy.*/xxxx,yyyy if "items like %Vxxxx,yyyy or %QP0010,0030"
    // converter: set InstitutionCodeSequence.0/InstitutionName to "test"
    // converter: set x to "items like %Vxxxx,yyyy or %QP0010,0030"
    // converter: set x if "items like %Vxxxx,yyyy or %QP0010,0030"
    // converter: set x format "%%08x" to "items like %Vxxxx,yyyy or %QP0010,0030"
    // converter: set x format "%%08x" if "items like %Vxxxx,yyyy or %QP0010,0030"

    else if (memicmp(line, "set ", 4)==0)	
    { char string[1024], format[1024];
      int offset=0;
      int g2=0, e2=0;
      unsigned int index=0;
      char ftype;

      if (strchr("xyzXYZ", line[4]) && line[5]==' ')
      { g=e=0;
        offset = 5;
      }
      else if (isxdig(line[4]) && line[8]==',') 
      { g = htoin(line+4, 4);
        e = htoin(line+9, 4);
	offset = 13;
      }
      else if (isalpha(line[4]))
      { char d[256]; 
	int j;
        RTCElement Entry;
	strcpy(d, line+4);
	j=0; while (isalnum(d[j])) j++; //20191019 isalpha-->isalnum
	d[j]=0;
	Entry.Description = d;
        if (VRType.GetGroupElement(&Entry))
	{ g = Entry.Group;
          e = Entry.Element;
	  offset = j+4;
	}
      }

      if (line[offset]=='.')
      { if (line[offset+1]=='*') 
        { index = 9999;
          offset += 2;
        }
	else
	{ offset++;
          index = atoi(line+offset);
          while (isdigit(line[offset])) offset++;
        }
      }

      if (line[offset]=='/' && isxdig(line[offset+1]) && line[offset+5]==',')
      { g2 = htoin(line+offset+1, 4);
        e2 = htoin(line+offset+6, 4);
        offset += 10;
      }
      else if (line[offset]=='/' && isalpha(line[offset+1]))
      { char d[256]; 
	int j;
        RTCElement Entry;
	strcpy(d, line+offset+1);
	j=0; while (isalnum(d[j])) j++; //20191019 isalpha-->isalnum
	d[j]=0;
	Entry.Description = d;
        if (VRType.GetGroupElement(&Entry))
	{ g2 = Entry.Group;
          e2 = Entry.Element;
	  offset += j+1;
	}
      }

      ftype = 's';
      if (memicmp(line+offset, " format \"", 9)==0)
      { offset += 9;
	strcpy(format, line+offset);
        p1 = strchr(format, '"');
	if (p1) 
	{ *p1=0;
	  offset += p1-format+1;
	  p1 = strchr(format, '%');
	  if (p1)
	  { while (*p1) 
	    { if (*p1=='s' || *p1=='f' || *p1=='x' || *p1=='d' || *p1=='g') 
	      { ftype = *p1; 
	        break; 
	      }
	      else 
		p1++;
	    }
	  }
	}
      }
      else
      {	*format = 0;
      }

      if (memicmp(line+offset, " to \"", 5)==0 ||
          memicmp(line+offset, " if \"", 5)==0)
      { strcpy(string, line+offset+5);
        p1 = strrchr(string, '"');
        if (p1)
        { *p1=0;
	  if (*format)
	  { char dum[1024];
	    strcpy(dum, string);
	    switch (ftype)
	    { case 'd': sprintf(string, format, atoi(dum)); break;
	      case 'x': sprintf(string, format, atoi(dum)); break;
	      case 'f': sprintf(string, format, atof(dum)); break;
	      case 'g': sprintf(string, format, atof(dum)); break;
              default:  sprintf(string, format, dum); break;
	    }
	  }

          int len = strlen(string);
          VR *vr, *vrs;
          Array < DICOMDataObject * > *ADDO1 = NULL;
	  
	  if (VRType.RunTimeClass(g, e, NULL)=='US')
	  { if (len) 
            { len = 2;
	      *(unsigned short *)string = atoi(string);
            }
	  }
	  else if (VRType.RunTimeClass(g, e, NULL)=='UL')
	  { if (len)
            { len = 4;
	      *(unsigned int *)string = atoi(string);
            }
	  }
          
  	  if (len&1)
          { len++;
	    if (VRType.RunTimeClass(g, e, NULL)!='UI' && VRType.RunTimeClass(g2, e2, NULL)!='UI') 
              strcat(string, " ");
          }
  
          if (memicmp(line+offset, " to \"", 4)==0 || len>0)	// "set xxxx,xxxx if" does not set when len==0
     	  { if (g==0 && e==0)					// setting variable
            { vr = PDU->VariableVRs[tolower(line[4])-'x'];
              if (!vr) vr = new VR;
              PDU->VariableVRs[tolower(line[4])-'x'] = vr;
            }
            else
            { vr = DDO->GetVR(g, e);

              if ((g2||e2) && vr && vr->SQObjectArray)		// set in existing sequence
              { ADDO1 = (Array<DICOMDataObject*>*) vr->SQObjectArray;
	        if (index>=ADDO1->GetSize())			// exceeding length of sequence ?
		{ DICOMDataObject *D = new DICOMDataObject;	// new item
		  vr = new VR(g2, e2, 0, (void *)NULL, FALSE);	// new VR
		  D->Push(vr);
		  ADDO1->Add(D);
		  index = ADDO1->GetSize()-1;			// go set this one
		}
		else
                  vr = ADDO1->Get(index)->GetVR(g2, e2);	// vr maybe in existing item
              }
            }

    	    if(!vr)						// vr does not exist
  	    { if (g2||e2)
	      { vr = new VR(g2, e2, len, (BOOL)TRUE);		// create it in sequence
                if (ADDO1) 
	          ADDO1->Get(index)->Push(vr);			// in existing sequence
	        else						// in new sequence
		{ Array < DICOMDataObject * > *SQE = new Array < DICOMDataObject * >;
		  DICOMDataObject *D = new DICOMDataObject;
		  
		  vrs = new VR(g, e, 0, (void *)NULL, FALSE);
		  vrs->SQObjectArray = (void*) SQE;
		  D->Push(vr);
		  SQE->Add(D);
                  DDO->Push(vrs);
	        }
	      }
  	      else                  
	      { if (VRType.RunTimeClass(g, e, NULL)=='SQ' && !DDO->GetVR(g, e)) // create new sequence
                { Array < DICOMDataObject * > *SQE = new Array < DICOMDataObject * >;
		  //DICOMDataObject *D = new DICOMDataObject;
		  
		  vrs = new VR(g, e, 0, (void *)NULL, FALSE);
		  vrs->SQObjectArray = (void*) SQE;
		  //SQE->Add(D);
                  DDO->Push(vrs);
                  if (N >= -1) SystemDebug.printf("%sconverter%d.%d created sequence: %04x,%04x\n", ininame, N, part, g, e);
                  vr = NULL;
                }
                else
                { vr = new VR(g, e, len, (BOOL)TRUE);
                  DDO->Push(vr);
                }
              }
            }
            else
    	      vr->ReAlloc(len);
  
  	    if (vr) 
            { if (len) memcpy(vr->Data, string, len);
              if (N >= -1) SystemDebug.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
            }
          }
          else
            if (N >= -1) SystemDebug.printf("%sconverter%d.%d executes void: %s\n", ininame, N, part, line);
        }
      }
      else if (memicmp(line+offset, " to nil", 7)==0)
      { if ((g||e) && !(g2||e2)) 
        { VR *vr = DDO->GetVR(g, e);
          if (vr) 
          { DDO->DeleteVR(vr);
            if (N >= -1) SystemDebug.printf("%sconverter%d.%d deletes: %04x,%04x\n", ininame, N, part, g, e);
          }
        }
      }
    }

    /* converter: setifempty xxxx,yyyy to "items like %Vxxxx,yyyy or %QP0010,0030" */
    /* converter: setifempty xxxx,yyyy if "items like %Vxxxx,yyyy or %QP0010,0030" */
    /* converter: setifempty x to "items like %Vxxxx,yyyy or %QP0010,0030" */
    /* converter: setifempty y if "items like %Vxxxx,yyyy or %QP0010,0030" */

    else if (memicmp(line, "setifempty ", 11)==0)	
    { char string[1024];
      int offset=20;

      if (strchr("xyzXYZ", line[11]))
      { g=e=0;
        offset = 12;
      }
      else
      { g = htoin(line+11, 4);
        e = htoin(line+16, 4);
      }

      if (memicmp(line+offset, " to \"", 4)==0 ||
          memicmp(line+offset, " if \"", 4)==0)
      { strcpy(string, line+offset+5);
        p1 = strrchr(string, '"');
        if (p1)
        { VR *vr;
          *p1=0;
          int len = strlen(string);
          
  	  if (len&1)
          { len++;
            strcat(string, " ");
          }
  
          if (memicmp(line+offset, " if \"", 4)==0 || len>0)	// if does not set when len==0
          { if (g==0 && e==0)
            { vr = PDU->VariableVRs[tolower(line[11])-'x'];
              if (!vr) vr = new VR;
              PDU->VariableVRs[tolower(line[11])-'x'] = vr;
            }
            else
              vr = DDO->GetVR(g, e);

  	    if(!vr) 
  	    { vr = new VR(g, e, len, (BOOL)TRUE);
              memcpy(vr->Data, string, len);
  	      DDO->Push(vr);
              if (N >= -1) OperatorConsole.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
            }
            else if (vr->Length==0)
            { vr->ReAlloc(len);
              memcpy(vr->Data, string, len);
              if (N >= -1) OperatorConsole.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
            }
            else
              if (N >= -1) SystemDebug.printf("%sconverter%d.%d executes void: %s\n", ininame, N, part, line);
          }
          else
            if (N >= -1) SystemDebug.printf("%sconverter%d.%d executes void: %s\n", ininame, N, part, line);
        }
      }
    }

    /* converter: delete xxxx,yyyy */

    else if (memicmp(line, "delete ", 7)==0)
    { g = htoin(line+7, 4);
      e = htoin(line+12, 4);

      pVR = DDO->GetVR(g, e);
      if (pVR) 
      { DDO->DeleteVR(pVR);
        if (N >= -1) OperatorConsole.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
      }
      else
        if (N >= -1) SystemDebug.printf("%sconverter%d.%d executes void: %s\n", ininame, N, part, line);
    }

    /* converter: newuids except g,e|uid and newuids */

    else if (memicmp(line, "newuids except ", 15)==0)
    { char tmp[1024];
      strcpy(tmp, line+15);
      strcat(tmp, "|");
      NewUIDsInDICOMObject(DDO, tmp, NULL, NULL, NULL);
      if (N >= -1) OperatorConsole.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
    }

    else if (memicmp(line, "newuids stage ", 14)==0)
    { NewUIDsInDICOMObject(DDO, "", NULL, line+14, NULL);
      if (N >= -1) OperatorConsole.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
    }

    else if (memicmp(line, "newuids", 7)==0)
    { NewUIDsInDICOMObject(DDO, "", NULL, NULL, NULL);
      if (N >= -1) OperatorConsole.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
    }

    /* converter: olduids except g,e|uid and newuids */

    else if (memicmp(line, "olduids except ", 15)==0)
    { char tmp[1024];
      strcpy(tmp, line+15);
      strcat(tmp, "|");
      OldUIDsInDICOMObject(DDO, tmp, NULL, NULL);
      if (N >= -1) OperatorConsole.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
    }

    else if (memicmp(line, "olduids stage ", 14)==0)
    { OldUIDsInDICOMObject(DDO, "", line+14, NULL);
      if (N >= -1) OperatorConsole.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
    }

    else if (memicmp(line, "olduids", 7)==0)
    { OldUIDsInDICOMObject(DDO, "", NULL, NULL);
      if (N >= -1) OperatorConsole.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
    }

    /* converters: fixkodak (remove leading 0 in 8 digit PATID), unfixkodak (add leading 0 in 8 digit PATID) */

    else if (memicmp(line, "fixkodak", 8)==0)
    { KodakFixer(DDO, FALSE);
      if (N >= -1) OperatorConsole.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
    }

    else if (memicmp(line, "unfixkodak", 10)==0)
    { KodakFixer(DDO, TRUE);
      if (N >= -1) OperatorConsole.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
    }

    /* import converter: copy file to file; copy file to directory */
  
    else if (memicmp(line, "copy ", 5)==0)
    { char string[1024];
      char *file;
      struct stat statbuf;
      char *p;
          
      strcpy(string, line+5);
      p = strstr(string, " to ");
      if (!p) p = strstr(string, " TO ");
      if (p)
      { *p=0;
  
        if (N >= -1) OperatorConsole.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
        file = p+4;
  
  	/* if the destination a directory then append the source filename to it */
  
        stat(file, &statbuf);
        if (statbuf.st_mode & S_IFDIR)
        { p = strrchr(pszFileName, PATHSEPCHAR);
          if (p)
          { if (file[strlen(file)-1]==PATHSEPCHAR) 
              file[strlen(file)-1]=0;
            strcat(file, p);
          }
        }
  
        if (!DFileCopy2(string, file, 0))
          if (N >= -2) OperatorConsole.printf("*** Exportconverter%d.%d: Failed to copy %s to %s\n", N, part, string, file);
  	else
  	  ImagesCopied++;
      }
    }

    /* converter: save to filename (filename can be generated using all % tricks) */

    else if (memicmp(line, "save to ", 8)==0)
    { char *file = line+8;

      if (N >= -1) OperatorConsole.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);

      DICOMDataObject *pDDO = MakeCopy(DDO);
      SaveDICOMDataObject(file, pDDO);
      delete pDDO;
    }

    /* converter: save frame N to filename (filename can be generated using all % tricks) */

    else if (memicmp(line, "save frame ", 11)==0)
    { char *p2 = strstr(line, " to ");
      char *file = p2 + 4;
      int frame = atoi(line+11);

      if (p2)
      { if (N >= -1) OperatorConsole.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);

        DICOMDataObject *pDDO = MakeCopy(DDO);
        ExtractFrame(pDDO, frame);
        SaveDICOMDataObject(file, pDDO);
        delete pDDO;
      }
    }

    /* converter: save bmp [frame N] [size N] filename (filename can be generated using all % tricks) */

    else if (memicmp(line, "save bmp ", 9)==0 ||
             memicmp(line, "save gif ", 9)==0 ||
             memicmp(line, "save jpg ", 9)==0)
    { int level=0, window=0, frame=0, size=0;
      char *file=NULL, *p2;
      float gamma = 1;
      p2 = strstr(line, " to ");
      if (p2) file = p2 + 4;
      p2 = strstr(line, " as ");
      if (p2) file = p2 + 4;
      p2 = strstr(line, " level ");
      if (p2) level = atoi(p2+7);
      p2 = strstr(line, " window ");
      if (p2) window = atoi(p2+8);
      p2 = strstr(line, " frame ");
      if (p2) frame = atoi(p2+7);
      p2 = strstr(line, " size ");
      if (p2) size = atoi(p2+6);
      p2 = strstr(line, " gamma ");
      if (p2) gamma = atof(p2+7);
      if (size<10) size = 4096;

      if (file)
      { if (N >= -1) SystemDebug.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);

	     if (memicmp(line + 5, "bmp ", 4)==0) ToBMP(DDO, file, size, 0, level, window, frame, gamma);
	else if (memicmp(line + 5, "gif ", 4)==0) ToGif(DDO, file, size, 0, level, window, frame, gamma);
	else                                      ToJPG(DDO, file, size, 0, level, window, frame, 95, gamma);
      }
    }

    /* converter: process with (command line can be generated using all % tricks, tempfile appended) */

    else if (memicmp(line, "process with ", 13)==0)
    { char cline[512];
      char tempfile[512];
      DICOMDataObject *DO2;

      PDU_Service PDU;
      PDU.AttachRTC(&VRType);

      NewTempFile(tempfile, ".dcm");
      strcpy(cline, line+13);
      if (cline[strlen(cline)-1]!=' ') strcat(cline, " ");
      strcat(cline, tempfile);

      if (N >= -1) OperatorConsole.printf("%sconverter%d.%d executes: process with %s\n", ininame, N, part, cline);

      SaveDICOMDataObject(tempfile, DDO);
      DDO->Reset();

      system(cline);

      DO2 = PDU.LoadDICOMDataObject(tempfile);
      while (( pVR = DO2->Pop() ))
      { DDO->Push(pVR);
      }
      delete DO2;
    }

    /* converter: tomono */
    else if (memicmp(line, "tomono", 6)==0)
    { DcmConvertPixelData(DDO, TRUE, FALSE, 0, 0, 0, 0, 0.0, 0.0, 0.0);
      if (N >= -1) SystemDebug.printf("%sconverter%d.%d: %s\n", ininame, N, part, line);
    }

    /* converter: crop */
    else if (memicmp(line, "crop ", 5)==0)
    { int startx=0, endx=9999, starty=0, endy=9999;
      sscanf(line+5, "%d,%d,%d,%d", &startx, &starty, &endx, &endy);
      DcmConvertPixelData(DDO, FALSE, TRUE, startx, endx, starty, endy, 0.0, 0.0, 0.0);
      if (N >= -1) SystemDebug.printf("%sconverter%d.%d: %s\n", ininame, N, part, line);
    }

    /* converter: scrub */
    else if (memicmp(line, "scrub ", 6)==0)
    { DICOMCommandObject d;
      VR *vr; 
      SetStringVR(&vr, 0x9999, 0x0202, line+6); 
      d.Push(vr);
      MaybeScrub(DDO, &d);
      if (N >= -1) SystemDebug.printf("%sconverter%d.%d: %s\n", ininame, N, part, line);
    }

    /* converter: system (command line can be generated using all % tricks) */
    else if (memicmp(line, "system ", 7)==0)
    { char cline[512];
      strcpy(cline, line+7);
      // BackgroundExec(cline, "");
      system(cline);
      if (N >= -1) SystemDebug.printf("%sconverter%d.%d: %s\n", ininame, N, part, line);
    }

    /* converter: mkdir (command line can be generated using all % tricks) */
    else if (memicmp(line, "mkdir ", 6)==0)
    { char cline[512], s[512];
      strcpy(cline, line+6);
      for (int sIndex = 0; sIndex<=strlen(cline); sIndex++)
        if (cline[sIndex]==PATHSEPCHAR)
	{ strcpy(s, cline);
	  s[sIndex]='\0';
	  mkdir(s);
	}
      mkdir(cline); // 20120829
      if (N >= -1) SystemDebug.printf("%sconverter%d.%d: %s\n", ininame, N, part, line);
    }

    /* converter: rm (command line can be generated using all % tricks) */

    else if (memicmp(line, "rm ", 3)==0)
    { char cline[512];
      strcpy(cline, line+3);
      unlink(cline);
      if (N >= -1) SystemDebug.printf("%sconverter%d.%d: %s\n", ininame, N, part, line);
    }

    /* converter: destroy (will prevent the image from being stored in the database) */

    else if (memicmp(line, "destroy2", 8)==0 && calling && called)
    { rc = 2;	
    }

    else if (memicmp(line, "destroy", 7)==0 && calling && called)
    { if (N >= -1) OperatorConsole.printf("%sconverter%d.%d: destroyed received image\n", ininame, N, part);
      rc = 2;	
    }

    else if (memicmp(line, "reject", 6)==0 && calling && called)
    { if (N >= -1) OperatorConsole.printf("%sconverter%d.%d: rejected received image\n", ininame, N, part);
      rc = 6;	
    }

    else if (memicmp(line, "retry", 6)==0 && (N==2100 || N==2200))
    { if (N >= -1) OperatorConsole.printf("%sconverter%d.%d: retry storing rejected image\n", ininame, N, part);
      rc = 7;	
    }

    else if (memicmp(line, "defer", 5)==0)
    { if (N >= -1) OperatorConsole.printf("%sconverter%d.%d: defer for exportconverter\n", ininame, N, part);
      rc = 8;	
    }

    /* converter: storage (sets preferred device for storing image, e.g, STORAGE MAG1) */
    /* disabled for general scripting */

    else if (memicmp(line, "storage ", 8)==0 && called && calling)
    { if (Storage && N >= -1) OperatorConsole.printf("%sconverter%d.%d: sets preferred storage to %s\n", ininame, N, part, line+8);
      if (Storage) strcpy(Storage, line+8);
    }

    else if (memicmp(line, "testmode ", 9)==0)
    { if (N >= -1) OperatorConsole.printf("%sconverter%d.%d: sets testmode to %s\n", ininame, N, part, line+9);
      strcpy(TestMode, line+9);
    }

    /* converter: virtualservermask (only useful to insert into queries, moves) */

    else if (memicmp(line, "virtualservermask ", 18)==0 && called && calling)
    { if (N >= -1) OperatorConsole.printf("%sconverter%d.%d: sets virtual server mask to %d\n", ininame, N, part, atoi(line+18));
      DDO->ChangeVR(0x9999, 0x0802, (UINT16) atoi(line+18), 0);
    }

    /* converter: virtualserver (only useful to insert into queries, moves) */

    else if (memicmp(line, "virtualserver ", 14)==0 && called && calling)
    { if (N >= -1) OperatorConsole.printf("%sconverter%d.%d: enables virtual server %d\n", ininame, N, part, atoi(line+14));
      DDO->ChangeVR(0x9999, 0x0802, (UINT16)(DDO->GetUINT16(0x9999, 0x0802) | (1 << atoi(line+14))), 0);
    }

    /* converter: compression (sets preferred compression for storing image, e.g, COMPRESSION un) */

    else if (memicmp(line, "compression ", 12)==0)
    { if (N >= -1) OperatorConsole.printf("%sconverter%d.%d: compression to %s\n", ininame, N, part, line+12);
      
      DICOMDataObject *DO2 = MakeCopy(DDO);
      if (!recompress(&DO2, line+12, "", line[12]=='n' || line[12]=='N', PDU))
        if (N >= -2) OperatorConsole.printf("**** %sconverter%d.%d: compression to %s failed\n", ininame, N, part, line+12);
      
      DDO->Reset();
      while (( pVR = DO2->Pop() ))
      { DDO->Push(pVR);
      }
      delete DO2;
    }

    /* converter: prefetch */

    else if (memicmp(line, "prefetch", 8)==0)
    { char pat[256];
      SearchDICOMObject(DDO, "0010,0020", pat);
      if (prefetch_queue("prefetch", pat, "", "", "", "", "", "", "", "", "", 0, ""))
        if (N >= -1) OperatorConsole.printf("%sconverter%d.%d: queued prefetch %s\n", ininame, N, part, pat);
    }

    /* converter: preretrieve */

    else if (memicmp(line, "preretrieve ", 12)==0)
    { char pat[256];
      SearchDICOMObject(DDO, "0010,0020", pat);
      if (prefetch_queue("preretrieve", pat, line+12, "", "", "", "", "", "", "", "", 0, ""))
        if (N >= -1) OperatorConsole.printf("%sconverter%d.%d: queued preretrieve of patient %s from %s\n", ininame, N, part, pat, line+12);
    }

    else if (memicmp(line, "forward patient ",  16)==0  ||
             memicmp(line, "forward study ",    14)==0  ||
             memicmp(line, "forward series ",   15)==0  ||
             memicmp(line, "forward image ",    14)==0  ||
             memicmp(line, "get patient ",      12)==0  ||
             memicmp(line, "get study ",        10)==0  ||
             memicmp(line, "get series ",       11)==0  ||
             memicmp(line, "get image ",        10)==0  ||
             memicmp(line, "delete patient ",   15)==0  ||
             memicmp(line, "delete study ",     13)==0  ||
             memicmp(line, "delete series ",    14)==0  ||
             memicmp(line, "delete image ",     13)==0  ||
             memicmp(line, "delete patient",    15)==0  ||
             memicmp(line, "delete study",      12)==0  ||
             memicmp(line, "delete series",     13)==0  ||
             memicmp(line, "delete image",      12)==0  ||
             memicmp(line, "process patient ",  16)==0  ||
             memicmp(line, "process study ",    14)==0  ||
             memicmp(line, "process series ",   15)==0  ||
             memicmp(line, "process image ",    14)==0  ||
             memicmp(line, "merge study ",      12)==0  ||
             memicmp(line, "submit patient ",   15)==0  ||
             memicmp(line, "submit study ",     13)==0  ||
             memicmp(line, "submit series ",    14)==0  ||
             memicmp(line, "submit image ",     13)==0  ||
             memicmp(line, "submit2 patient ",  16)==0  ||
             memicmp(line, "submit2 study ",    14)==0  ||
             memicmp(line, "submit2 series ",   15)==0  ||
             memicmp(line, "submit2 image ",    14)==0)
    { char *p = strchr(line, ' ')+1; const char *compress="", *dest="", *date="", *modality="", *sop="", *imagetype="", *seriesdesc="", *script="";
      char studyuid[65], seriesuid[65], vr[200], dat[200];
      VR *pVR1;
      int len, delay=0;
      
      studyuid[0]=0;
      seriesuid[0]=0;

      const char *level=p;
      p = strchr(p, ' ');
      while(p)
      { *p=0;
        p++;
  
        if (memicmp(p, "compressed as ", 14)==0) 
        { compress = p+14; 
          p = strchr(p+14, ' '); 
        }
        else if (memicmp(p, "to ", 3)==0) 
        { dest = p+3; 
          p = strchr(p+3, ' '); 
        }
        else if (memicmp(p, "after ", 6)==0) 
        { delay = atoi(p+6); 
          p = strchr(p+6, ' '); 
        }
        else if (memicmp(p, "by ", 3)==0) 
        { dest = p+3; 
          p = NULL; // strchr(p+3, ' '); 
        }
        else if (memicmp(p, "from ", 5)==0) 
        { dest = p+5; 
          p = strchr(p+5, ' '); 
        }
        else if (memicmp(p, "date ", 5)==0) 	// all series date yyyymmdd or yyyymmdd-yyyymmdd
        { date = p+5; 
          p = strchr(p+5, ' '); 
          level = "selection";			// causes that no study or series UID are passed to query
        }
        else if (memicmp(p, "now ", 4)==0) 	// now -365+365: within one year from now (+/- compulsory)
	{ struct tm tmbuf1, tmbuf2;		// also works on all series within date range
	  char *q;
          int from=0, to=0;
	  from=atoi(p+4);
          q = strchr(p+5, '+'); if (!q) q = strchr(p+5, '-'); if (q) to=atoi(q);
		
	  time_t t1 = time(NULL)+24*3600*from;
	  time_t t2 = time(NULL)+24*3600*to;
          localtime_r(&t1, &tmbuf1);
          localtime_r(&t2, &tmbuf2);

	  sprintf(dat, "%04d%02d%02d-%04d%02d%02d", tmbuf1.tm_year+1900, tmbuf1.tm_mon, tmbuf1.tm_mday, tmbuf2.tm_year+1900, tmbuf2.tm_mon, tmbuf2.tm_mday);
	  date = dat;

          p = strchr(p+4, ' '); 
          level = "selection";			// causes that no study or series UID passed to query
        }
        else if (memicmp(p, "age ", 4)==0) 	// age -365+365: within one year from series date (+/- compulsory)
	{ struct tm tmbuf1, tmbuf2;		// also works on all series withon date range
	  char *q;
          int from=0, to=0;
	  from=atoi(p+4);
          q = strchr(p+5, '+'); if (!q) q = strchr(p+5, '-'); if (q) to=atoi(q);

          SearchDICOMObject(DDO, "0008,0020", dat);
          memset(&tmbuf1, 0, sizeof(tmbuf1));
	  sscanf(dat, "%04d%02d%02d", &tmbuf1.tm_year, &tmbuf1.tm_mon, &tmbuf1.tm_mday);
          tmbuf1.tm_year -= 1900;

	  time_t t = mktime(&tmbuf1);
	  time_t t1 = t+24*3600*from;
	  time_t t2 = t+24*3600*to;
          localtime_r(&t1, &tmbuf1);
          localtime_r(&t2, &tmbuf2);

	  sprintf(dat, "%04d%02d%02d-%04d%02d%02d", tmbuf1.tm_year+1900, tmbuf1.tm_mon, tmbuf1.tm_mday, tmbuf2.tm_year+1900, tmbuf2.tm_mon, tmbuf2.tm_mday);
	  date = dat;

          p = strchr(p+4, ' '); 
          level = "selection";			// causes that no study or series UID passed to query
        }
        else if (memicmp(p, "modality ", 9)==0) 
        { modality = p+9; 
          p = strchr(p+9, ' '); 
          level = "selection";			// causes that no study or series UID is passed to query
        }
        else if (memicmp(p, "sop ", 4)==0) 
        { sop = p+4; 
          p = strchr(p+4, ' '); 
          level = "selection";			// causes that no study or series UID is passed to query except the given SOP
        }
        else if (memicmp(p, "imagetype ", 10)==0) 
        { imagetype = p+10; 
          p = strchr(p+10, ' '); 
          level = "study";			// causes that no series UID is passed to query
        }
        else if (memicmp(p, "split ", 6)==0) 
        { imagetype = p+6; 
          p = strchr(p+6, ' '); 
        }
        else if (memicmp(p, "target ", 7)==0) 	// for DcmSubmit only
        { imagetype = p+7; 
          p = strchr(p+7, ' '); 
        }
        else if (memicmp(p, "seriesdesc ", 11)==0) 
        { seriesdesc = p+11; 
          p = strchr(p+11, ' '); 
          level = "study";			// causes that no series UID is passed to query
        }
        else if (memicmp(p, "password ", 9)==0) // for DcmSubmit only
        { seriesdesc = p+9; 
          p = strchr(p+9, ' '); 
        }
        else if (memicmp(p, "command [", 9)==0) // for submit2 only
        { seriesdesc = p+9; 
          p = strchr(p+9, ']'); 
        }
        else if (memicmp(p, "command ", 8)==0) // for submit2 only
        { seriesdesc = p+8; 
          p = strchr(p+8, ' '); 
        }
        else if (memicmp(p, "script \"", 8)==0) 
        { if (p[strlen(p)-1]=='"') p[strlen(p)-1]=0;
	  script = p+8; 			// must be last parameter
	  p = NULL;
          break;
        }
        else if (memicmp(p, "script ", 7)==0)	// script without ""
        { script = p+7; 			// must be last parameter
	  p = NULL;
          break;
        }
        else if (memicmp(p, "series ", 7)==0) 
        { char *a=p+7;
          p = strchr(p+7, ' ');
	  *p=0;
	  strcpy(seriesuid, a);
          level = "selection";			// causes that no study UID is passed and give series
        }
        else if (memicmp(p, "study ", 6)==0) 
        { char *a=p+6;
          p = strchr(p+6, ' ');
          *p=0;
	  strcpy(studyuid, a);
          level = "selection";			// causes that no series UID is passed and give study
        }
      }

      pVR1 = DDO->GetVR(0x0020, 0x000d);
      if (pVR1 && (memicmp(level, "stu", 3)==0 || memicmp(level, "ser", 3)==0) || memicmp(level, "ima", 3)==0)
      { strncpy(vr, (char*)pVR1->Data, pVR1->Length);
        vr[pVR1->Length] = 0;
        len = pVR1->Length - 1;
        while(len>0)
        { if (vr[len] == ' ')
            vr[len] = 0;
          else
            break;
          len--;
        }
        strcpy(studyuid, vr);
      }

      pVR1 = DDO->GetVR(0x0020, 0x000e);
      if (pVR1 && (memicmp(level, "ser", 3)==0 || memicmp(level, "ima", 3)==0))
      { strncpy(vr, (char*)pVR1->Data, pVR1->Length);
        vr[pVR1->Length] = 0;
        len = pVR1->Length - 1;
        while(len>0)
        { if (vr[len] == ' ')
            vr[len] = 0;
          else
            break;
          len--;
        }
        strcpy(seriesuid, vr);
      }

      pVR1 = DDO->GetVR(0x0008, 0x0018);
      if (pVR1 && memicmp(level, "ima", 3)==0)
      { strncpy(vr, (char*)pVR1->Data, pVR1->Length);
        vr[pVR1->Length] = 0;
        len = pVR1->Length - 1;
        while(len>0)
        { if (vr[len] == ' ')
            vr[len] = 0;
          else
            break;
          len--;
        }
        sop = vr;
      }

      line[11]=0;
      if (sop) level = "single object";
      char pat[256];
      SearchDICOMObject(DDO, "0010,0020", pat);
      if (prefetch_queue(line, pat, dest, studyuid, seriesuid, compress, modality, date, sop, imagetype, seriesdesc, delay, script))
        if (N >= -1) OperatorConsole.printf("%sconverter%d.%d: queued %s - (%s %s %s of %s) to %s\n", ininame, N, part, line, level, modality, date, pat, dest);
    }

    /* converter: stop and silentstop (will prevent further converters from running for this image) */

    else if (memicmp(line, "stop", 4)==0)
    { if (N >= -1) SystemDebug.printf("%sconverter%d.%d: stop\n", ininame, N, part);
      rc = 3;	
    }

    else if (memicmp(line, "silentstop", 10)==0)
    { rc = 3;	
    }

    else if (memicmp(line, "return", 6)==0)
    { if (N >= -1) SystemDebug.printf("%sconverter%d.%d: return\n", ininame, N, part);
      rc = 5;	
    }

    else if (memicmp(line, "call ", 5)==0)
    { FILE *f;
      int ret;
      char cmd[1024];

      cmd[0]=0;
      MyGetPrivateProfileString ( "scripts", line+5, cmd, cmd, 1024, ConfigFile);
      if (*cmd)
        ret = CallImportConverterN(DCO, DDO, -1, NULL, NULL, NULL, NULL, PDU, Storage, cmd);
      else
      { f = fopen(line+5, "rt");
        if (f)
        { if (N >= -1) SystemDebug.printf("%sconverter%d.%d: %s\n", ininame, N, part, line);
          while(fgets(cmd, sizeof(cmd), f) != NULL)
          { if (cmd[strlen(cmd)-1]=='\n') cmd[strlen(cmd)-1]=0;
	    if (cmd[0]!='#' && cmd[0]!=';')
	    { // ret = CallImportConverterN(DDO, -1, NULL, NULL, NULL, NULL, NULL, NULL, Storage, cmd, vars);
	      ret = CallImportConverterN(DCO, DDO, -1, NULL, NULL, NULL, NULL, PDU, Storage, cmd);
              if (ret==5 || ret==2 || ret==6) break; // return or destroy or reject
	    }
          }
	  fclose(f);
        }
        else
          if (N >= -2) OperatorConsole.printf("*** %sconverter%d.%d script not found: %s\n", ininame, N, part, line+5);
      }
      rc = ret;
    }

    /* converter: script, evaluate string as script */

    else if (memicmp(line, "script \"", 8)==0)
    { char cmd[1024];
      strcpy(cmd, line+8);
      cmd[strlen(cmd)-1]=0;
      rc = CallImportConverterN(DCO, DDO, -1, NULL, NULL, NULL, NULL, PDU, Storage, cmd);
    }

    /* converter: lua, evaluate string as lua program */

    else if (memicmp(line, "lua \"", 5)==0 || memicmp(line, "lua:", 4)==0)
    { char cmd[1024];
      // note; threadnum and dco not implemented
      struct scriptdata sd = {PDU, DCO, DDO, N, pszModality, pszStationName, pszSop, patid, Storage, 0, 0};
      if (memicmp(line, "lua:", 4)==0) 
	strcpy(cmd, line+4);
      else                             
      { strcpy(cmd, line+5);
        cmd[strlen(cmd)-1]=0;
      }
      do_lua(&(PDU->L), cmd, &sd);
      rc = sd.rc;
    }

    /* converter: if "string" (only execute next converter if string not empty) */

    else if (memicmp(line, "ifnotempty \"", 12)==0)	
    { char string[1024];
      char *p;

      strcpy(string, line+12);
      p = strrchr(string, '"');
      if (p)
      { *p=0;

        if (N >= -1) SystemDebug.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);

        int len = strlen(string);
        if (len==0) rc = 4;
      }
    }

    /* converter: ifempty "string" (only execute next converter if string empty) */

    else if (memicmp(line, "ifempty \"", 9)==0)	
    { char string[1024];
      char *p;

      strcpy(string, line+9);
      p = strrchr(string, '"');
      if (p)
      { *p=0;

        if (N >= -1) SystemDebug.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);

        int len = strlen(string);
        if (len!=0) rc = 4;
      }
    }

    /* converter: ifequal "string1","string2" (only execute next converter if string1 equals string2) */
  
    else if (memicmp(line, "ifequal \"", 9)==0)	
    { char string[1024];
      char *p, *string2;
  
      strcpy(string, line+9);
      p = strrchr(string, '"');
      if (p)
      { *p=0;

        p = strstr(string, "\",\"");
        if (!p) p = strstr(string, "\", \"");
        if (p) 
        { string2 = strchr(p+1, '"')+1;
          *p=0;
          if (N >= -1) SystemDebug.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
          int cmp = strcmp(string, string2);
          if (cmp!=0) rc = 4;
        }
      }
    }

    /* converter: ifnotequal "string1","string2" (only execute next converter if string1 not equals string2) */
  
    else if (memicmp(line, "ifnotequal \"", 12)==0)	
    { char string[1024];
      char *p, *string2;
  
      strcpy(string, line+12);
      p = strrchr(string, '"');
      if (p)
      { *p=0;

        p = strstr(string, "\",\"");
        if (!p) p = strstr(string, "\", \"");
        if (p) 
        { string2 = strchr(p+1, '"')+1;
          *p=0;
          if (N >= -1) SystemDebug.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
          int cmp = strcmp(string, string2);
          if (cmp==0) rc = 4;
        }
      }
    }

    /* converter: ifmatch "string1","string2" (only execute next converter if string1 matches string2 - case insensitive) */
  
    else if (memicmp(line, "ifmatch \"", 9)==0)	
    { char string[1024];
      char *p, *string2;
  
      strcpy(string, line+9);
      p = strrchr(string, '"');
      if (p)
      { *p=0;

        p = strstr(string, "\",\"");
        if (!p) p = strstr(string, "\", \"");
        if (p) 
        { string2 = strchr(p+1, '"')+1;
          *p=0;
          if (N >= -1) SystemDebug.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
          if (!match(string, string2)) rc = 4;
        }
      }
    }

    /* converter: ifnotmatch "string1","string2" (only execute next converter if string1 not matches string2 - case insensitive) */
  
    else if (memicmp(line, "ifnotmatch \"", 12)==0)	
    { char string[1024];
      char *p, *string2;
  
      strcpy(string, line+12);
      p = strrchr(string, '"');
      if (p)
      { *p=0;

        p = strstr(string, "\",\"");
        if (!p) p = strstr(string, "\", \"");
        if (p) 
        { string2 = strchr(p+1, '"')+1;
          *p=0;
          if (N >= -1) SystemDebug.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
          if (match(string, string2)) rc = 4;
        }
      }
    }

    /* converter: ifnumequal "string1","string2" (only execute next converter if string1 num equals string2 - integer) */
  
    else if (memicmp(line, "ifnumequal \"", 12)==0)	
    { char string[1024];
      char *p, *string2;
  
      strcpy(string, line+12);
      p = strrchr(string, '"');
      if (p)
      { *p=0;

        p = strstr(string, "\",\"");
        if (!p) p = strstr(string, "\", \"");
        if (p) 
        { string2 = strchr(p+1, '"')+1;
          *p=0;
          if (N >= -1) SystemDebug.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
          if (atoi(string)!=atoi(string2)) rc = 4;
        }
      }
    }

    /* converter: ifnumnotequal "string1","string2" (only execute next converter if string1 num not equals string2 - integer) */
  
    else if (memicmp(line, "ifnumnotequal \"", 15)==0)	
    { char string[1024];
      char *p, *string2;
  
      strcpy(string, line+15);
      p = strrchr(string, '"');
      if (p)
      { *p=0;

        p = strstr(string, "\",\"");
        if (!p) p = strstr(string, "\", \"");
        if (p) 
        { string2 = strchr(p+1, '"')+1;
          *p=0;
          if (N >= -1) SystemDebug.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
          if (atoi(string)==atoi(string2)) rc = 4;
        }
      }
    }

    /* converter: ifnumgreater "string1","string2" (only execute next converter if string1 num greater than string2 num - integer) */
  
    else if (memicmp(line, "ifnumgreater \"", 14)==0)	
    { char string[1024];
      char *p, *string2;
  
      strcpy(string, line+14);
      p = strrchr(string, '"');
      if (p)
      { *p=0;

        p = strstr(string, "\",\"");
        if (!p) p = strstr(string, "\", \"");
        if (p) 
        { string2 = strchr(p+1, '"')+1;
          *p=0;
          if (N >= -1) SystemDebug.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
          if (atoi(string)<=atoi(string2)) rc = 4;
        }
      }
    }

    /* converter: ifnumless "string1","string2" (only execute next converter if string1 num less than string2 num - integer) */
  
    else if (memicmp(line, "ifnumless \"", 11)==0)	
    { char string[1024];
      char *p, *string2;
  
      strcpy(string, line+11);
      p = strrchr(string, '"');
      if (p)
      { *p=0;

        p = strstr(string, "\",\"");
        if (!p) p = strstr(string, "\", \"");
        if (p) 
        { string2 = strchr(p+1, '"')+1;
          *p=0;
          if (N >= -1) SystemDebug.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
          if (atoi(string)>=atoi(string2)) rc = 4;
        }
      }
    }

    /* converter: between "string1","string2" (execute next converter on time in hours: between "9", "17"; defer) */
  
    else if (memicmp(line, "between \"", 9)==0)	
    { char string[1024];
      char *p, *string2;
      int h1, h2, h;
      struct tm tmbuf;
      time_t t = time(NULL);

      localtime_r(&t, &tmbuf);
      h = tmbuf.tm_hour;

      strcpy(string, line+9);
      h1 = atoi(string);
      p = strrchr(string, '"');
      if (p)
      { *p=0;

        p = strstr(string, "\",\"");
        if (!p) p = strstr(string, "\", \"");
        if (p) 
        { string2 = strchr(p+1, '"')+1;
          *p=0;
          h2 = atoi(string2);
          if (N >= -1) SystemDebug.printf("%sconverter%d.%d executes: %s\n", ininame, N, part, line);
          if (h2>=h1 && (h <h1 || h>=h2)) rc = 4;	// between "9", "17": skip if h<9 or h>=17
          if (h2< h1 && (h <h1 && h>=h2)) rc = 4;	// between "17", "9": skip if h<=17 and h>=9
        }
      }
    }

    /* converter: nop (no operation) */

    else if (memicmp(line, "nop", 3)==0)
    { if (N >= -1) OperatorConsole.printf("%sconverter%d.%d: %s\n", ininame, N, part, line);
    }

    /* unrecognized command: assume call line or file */

    else if (*line)
    { FILE *f;
      int ret;
      char cmd[1024];
      char fn[256];

      cmd[0]=0;
      MyGetPrivateProfileString ( "scripts", line, cmd, cmd, 1024, ConfigFile);
      if (*cmd)
        ret = CallImportConverterN(DCO, DDO, -1, NULL, NULL, NULL, NULL, PDU, Storage, cmd);
      else
      { char *b = strchr(line,'(');
        if (b) *b=0;
	f = fopen(line, "rt");
	if (!f) 
	{ strcpy(fn, BaseDir);
          strcat(fn, line);
          f = fopen(fn, "rt");
	}
        if (f)
        { if (strstr(line, ".lua"))	// direct call of lua file
          { int i, n;
            fclose(f);

            char script[512]; 
            strcpy(script, "dofile(Global.basedir..'");
            for (i=0, n=24; i<strlen(line); i++)
            { if (line[i]=='\\')
              { script[n++] = '\\';
                script[n++] = '\\';
	      }
	      else 
	      { script[n++] = line[i];
	      }
	    }
            script[n++] = 0;
            strcat(script, "')");
	    
	    // accept file.lua(%i:%p) or e.g., file.lua("%i:%p"), substituted string goes unparsed to command_line
	    
	    if (b) 
	    { b++;
	      if (b[0]=='"')
	      { b++;
	        if (b[strlen(b)-2]=='"') 
		  b[strlen(b)-2] = 0;
              }
	      else if (b[strlen(b)-1]==')') 
	        b[strlen(b)-1] = 0;

	      lua_setvar(PDU, "command_line",    b);      
              lua_setvar(PDU, "version",         DGATE_VERSION);
	    }
            
	    // note: threadnum not implemented
            struct scriptdata sd = {PDU, DCO, DDO, N, pszModality, pszStationName, pszSop, patid, Storage, 0, 0};
    	    if (N >= -1) SystemDebug.printf("Importconverter%d.%d: %s\n", N, part, script);
    	    sd.rc = 1;
            do_lua(&(PDU->L), script, &sd);
            ret = sd.rc;
          }
	  else
	  { if (N >= -1) SystemDebug.printf("%sconverter%d.%d: %s\n", ininame, N, part, line);
            while(fgets(cmd, sizeof(cmd), f) != NULL)
            { if (cmd[strlen(cmd)-1]=='\n') cmd[strlen(cmd)-1]=0;
	      if (cmd[0]!='#' && cmd[0]!=';' && cmd[0]!=0)
	      { ret = CallImportConverterN(DCO, DDO, -1, NULL, NULL, NULL, NULL, PDU, Storage, cmd);
                if (ret==5 || ret==2 || ret==6) break; // return or destroy or reject
	      }
            }
	    fclose(f);
	  }
        }
        else
          if (N >= -2) OperatorConsole.printf("*** %sconverter%d.%d error: %s\n", ininame, N, part, line);
      }
      rc = ret;
    }

    // process next command
    if (szNext[0]==0) break;
    strcpy(szExecName, szNext);
    part++;

    if (rc==3 || rc==2 || rc==5 || rc==6) break;
    //  stop     destroy  return   reject
  }

  return rc;
}

int CallImportConverters(DICOMCommandObject *DCO, DICOMDataObject *DDO, char *pszModality, char *pszStationName, char *pszSop, char *patid, ExtendedPDU_Service *PDU, char *Storage)
{ char		szRootSC[64];
//  char		szEntry[32];
  char		szTemp[32];
  int		i, iNbConverters;
  int		rc;

  if (!MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig,
		szRootSC, 64, ConfigFile)) return FALSE;
  if (!MyGetPrivateProfileString(szRootSC, "ImportConverters", "99",
		szTemp, 128, ConfigFile)) return FALSE;
  iNbConverters = atoi(szTemp);

  /* Loop over all converters */
  rc = 0;
  for(i=0; i<iNbConverters; i++)
  { rc = CallImportConverterN(DCO, DDO, i, pszModality, pszStationName, pszSop, patid, PDU, Storage, NULL);

    if (rc==2) break;
    //  destroy

    if (rc==6) break;
    //  reject
  }

  // the status of the last one is returned: FALSE means destroy command (status 2) was executed
  //return !(rc==2);

  return rc;
}

//////////////////////////////////////////////////////////////////////////////////
/* This code is a simple thread safe queue with its own processing thread       */
//////////////////////////////////////////////////////////////////////////////////

struct conquest_queue
{ int    top;
  int    bottom;
  int    num;
  int    entrysize;
  char   *data;
#ifdef WIN32
  HANDLE threadhandle;
#else
  pthread_t threadhandle;
#endif
  CRITICAL_SECTION critical;
  int    delay;
  BOOL   (*process)   (char *, ExtendedPDU_Service	*PDU, char *);
  BOOL   (*mayprocess)(char *, ExtendedPDU_Service	*PDU, char *);
  int    fails;			// counts failures
  time_t lastfailtime;		// set on failure, cleared on success
  int    maxfails;		// if set and exceeded stop retrying
  char	 failfile[512];		// file to store failed entries in

  ExtendedPDU_Service *PDU;	// to allow keeping association open
  time_t opentime;		// set on success
  int    pdu_refreshdelay;	// clear accepted syntaxes after .. s (use long time)
  int    pdu_closedelay;	// close unused pdu after .. s (use short time)
  int    pdu_release;		// if set, release pdu else just drop link
  char   *ForwardLastUID;	// remember last uid of link (hang up?)
} q1;

static BOOL WINAPI processthread(struct conquest_queue *q)
{ char *data = (char *)malloc(q->entrysize);

  while (1)
  { while (1)
    { EnterCriticalSection(&q->critical);

      if (q->top!=q->bottom) 
      { memcpy(data, q->data + q->bottom * q->entrysize, q->entrysize);

        // it is possible to delay processing of queue through this function

        if (q->mayprocess)
        { if (!q->mayprocess(data, q->PDU, q->ForwardLastUID)) 
          { LeaveCriticalSection(&q->critical);
            Sleep(q->delay);
            continue;
          }
        }

        q->bottom = (q->bottom+1)%q->num;
        LeaveCriticalSection(&q->critical);

	// only try to process if last failure was more than FailHoldOff seconds ago

        if (time(NULL)-q->lastfailtime > FailHoldOff)
        { if (q->process(data, q->PDU, q->ForwardLastUID)) 
  	  { q->lastfailtime = time(NULL);		// failed
            q->fails++;

	    // store failures
            if (q->failfile[0])
            { FILE *f = fopen(q->failfile, "ab");
	      if (f)
              { fwrite(data, q->entrysize, 1, f);
                fclose(f);
                OperatorConsole.printf("Queue: holding processing of file %s\n", data);
              }
            }
          }
          else
          { q->lastfailtime= 0;				// remember that last process was a success
            q->opentime    = time(NULL);		// PDU must be open by now
	    q->fails       = 0;
          }
        }
        else
        { // store failures (actually these were not even tried)
          if (q->failfile[0])
          { FILE *f = fopen(q->failfile, "ab");
            if (f)
            { fwrite(data, q->entrysize, 1, f);
              fclose(f);
              OperatorConsole.printf("Queue: holding processing of file %s\n", data);
            }
          }
        }

        break;		// this break avoids delays when the queue is not empty
      }

      LeaveCriticalSection(&q->critical);

      Sleep(q->delay);

      // Retry logic: RetryDelay s after last failure retry to process the data
      // This condition would also be at startup: therefore wait until DB completely indexed

      if (time(NULL)-q->lastfailtime>RetryDelay && q->failfile[0] && NumIndexing==0)
      { int rec=0, done=0, count=0;
        FILE *f = fopen(q->failfile, "r+b");
        
        if (f)
        { while(1)
          { fseek(f, rec * q->entrysize, SEEK_SET);

            if (fread(data, 1, q->entrysize, f)!=(unsigned int)q->entrysize) 	// if eof -> done
            { done=1;
              break;
            }

            if (data[0])					// filled entry ?
            { OperatorConsole.printf("Queue: retrying processing of file %s\n", data);

              if (q->process(data, q->PDU, q->ForwardLastUID))
              { q->lastfailtime = time(NULL);			// failed again
		q->fails++;

		if (q->maxfails && (q->fails>q->maxfails))
		{ OperatorConsole.printf("*** Queue: %d retries exceeds set maximum of %d - giving up\n", q->fails, q->maxfails);
                  q->lastfailtime = 0;
		  q->fails        = 0;
                  done = 1;
		}
                break;						// retry later
              }
              else
	      { q->lastfailtime = 0;
                q->opentime     = time(NULL);			// PDU must be open by now
		q->fails        = 0;

                data[0]=0;					// mark data as processed
                fseek(f, rec * q->entrysize, SEEK_SET);
                fwrite(data, 1, 1, f);

                count++;					// every 10 files make time to process..
                if(count>10) break;				// ..queue entries that are freshly created
              }
            }
            rec++;						// next entry in file
          }

          fclose(f);
          if (done) unlink(q->failfile);			// delete file when not needed anymore
        }
      }

      for(int i=0; i<MAXExportConverters; i++)
      { if (q->PDU && (time(NULL)-q->opentime > q->pdu_closedelay)) // close unused association after 5 seconds
        { if (q->PDU[i].Link.Connected)
          //q->PDU[i].Close(); just hangup without AReleaseRQ: otherwise server sometimes fails to reconnect

          if (q->pdu_release) q->PDU[i].Close();
	  else                q->PDU[i].Link.Close();
        }

        if (q->PDU && (time(NULL)-q->opentime > q->pdu_refreshdelay)) // clean collected sopclasses after one hour of inactivity
        { if (!q->PDU[i].Link.Connected)
            q->PDU[i].ClearAbstractSyntaxs();
          q->opentime = time(NULL);
        }
      }
    }
  }

  free(data);
  return TRUE;
}

struct conquest_queue *new_queue(int num, int size, int delay, BOOL (*process)(char *, ExtendedPDU_Service *PDU, char *), ExtendedPDU_Service *PDU, int maxfails)
{ struct conquest_queue *result;
#ifdef WIN32
   unsigned long ThreadID;
#endif

  result               = new conquest_queue;
  result->top          = 0;
  result->bottom       = 0;
  result->num          = num;
  result->entrysize    = size;
  result->delay        = delay;
  result->process      = process;
  result->mayprocess   = NULL;
  result->data         = (char *)malloc(num * size);
  result->fails        = 0;
  result->maxfails     = maxfails;
  result->lastfailtime = 0;
  result->failfile[0]  = 0;
  result->PDU	       = PDU;
  result->opentime     = 0;
  result->pdu_refreshdelay = 3600;
  result->pdu_closedelay   = 5;
  result->pdu_release      = 0;
  result->ForwardLastUID   = NULL;

  InitializeCriticalSection(&result->critical);

  /* Note: since the queue is thread safe it is possible to start more than one thread to service it */

  char Temp[64], szRootSC[64];
  strcpy(Temp, "1");
  if (MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile))
    MyGetPrivateProfileString(szRootSC, "DelayedForwarderThreads", "1", Temp, 64, ConfigFile);
  OperatorConsole.printf("Starting %s DelayedForwarderThreads\n", Temp);
#ifdef WIN32
  for (int i=0; i<atoi(Temp); i++)
    result->threadhandle = 
      CreateThread(NULL, 2097152, (LPTHREAD_START_ROUTINE) processthread, result, 0, &ThreadID);
#else
  for (int i=0; i<atoi(Temp); i++) 
  { pthread_create(&result->threadhandle, NULL, (void*(*)(void*))processthread, (void *)result);
    pthread_detach(result->threadhandle);
  }
#endif

  return result;
}

void into_queue(struct conquest_queue *q, char *in)
{ while (1)
  { EnterCriticalSection(&q->critical);

    if ((q->top+1)%q->num != q->bottom)
    { memcpy(q->data + q->top * q->entrysize, in, q->entrysize);
      q->top = (q->top+1)%q->num;
      LeaveCriticalSection(&q->critical);
      return;
    }

    LeaveCriticalSection(&q->critical);

    Sleep(q->delay);
  }
}

// enters entry into queue if first numunique characters are not there.
// note: will discard entry when no space in queue
// intended for prefetching stuff: use a large queue

BOOL into_queue_unique(struct conquest_queue *q, char *in, int numunique)
{ int i;

  EnterCriticalSection(&q->critical);

  // no space!
  if ((q->top+1)%q->num == q->bottom) 
  { LeaveCriticalSection(&q->critical);
    return FALSE;
  }

  // search for identical items
  for (i=q->bottom; i!=(q->top+1)%q->num; i = (i+1)%q->num)
  { if (memcmp(q->data + i * q->entrysize, in, numunique)==0)
    { memcpy(q->data + i * q->entrysize, in, q->entrysize);
      LeaveCriticalSection(&q->critical);
      return FALSE;
    }
  }
  
  // enter new item
  memcpy(q->data + q->top * q->entrysize, in, q->entrysize);
  q->top = (q->top+1)%q->num;
  LeaveCriticalSection(&q->critical);
  return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// This code starts a thread for each defined export converter
//////////////////////////////////////////////////////////////////////////////////

static struct conquest_queue **eqN = NULL;
static ExtendedPDU_Service ForwardPDU[MAXExportConverters][MAXExportConverters];	// max 20*20 with remaining association

BOOL exportprocessN(char *data, ExtendedPDU_Service *PDU, char *t)
{ return CallExportConverterN(data, data[1024+18+18+66], data+1024, data+1024+18, data+1024+18+18, data+1024+18+18+66+4, PDU, t, data+1024+18+18+66+4+66, data+1024+18+18+66+4+66+18, data+1024+18+18+66+4+66+18+18);
}

void reset_queue_fails(int N)
{ eqN[N]->fails = 0;
}

void export_queueN(struct conquest_queue **q, char *pszFileName, int N, char *pszModality, char *pszStationName, char *pszSop, char *patid, char *calling, char *called, char *script)
{ char data[1536];

  if (!*q) 
  { char szRootSC[64], szTemp[32];

    if (N<0)
      *q = new_queue(QueueSize, 1536, 100, exportprocessN, ScriptForwardPDU[0], 0);
    else
       *q = new_queue(QueueSize, 1536, 100, exportprocessN, N>=MAXExportConverters ? NULL : ForwardPDU[N], 0);
    sprintf((*q)->failfile, "ExportFailures%s_%d", Port, N);
    (*q)->ForwardLastUID = (char *) malloc(MAXExportConverters * 66);
    memset((*q)->ForwardLastUID, 0, MAXExportConverters * 66);

    MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile);

    MyGetPrivateProfileString(szRootSC, "ForwardAssociationRefreshDelay", "3600", szTemp, 128, ConfigFile);
    (*q)->pdu_refreshdelay = atoi(szTemp);

    MyGetPrivateProfileString(szRootSC, "ForwardAssociationCloseDelay", "5", szTemp, 128, ConfigFile);
    (*q)->pdu_closedelay   = atoi(szTemp);

    MyGetPrivateProfileString(szRootSC, "ForwardAssociationRelease", "1",	szTemp, 128, ConfigFile);
    (*q)->pdu_release   = atoi(szTemp);

    MyGetPrivateProfileString(szRootSC, "MaximumExportRetries", "0", szTemp, 128, ConfigFile);
    (*q)->maxfails   = atoi(szTemp);
  }

  if (pszFileName==NULL) return;

  memset(data, 0, 1536);

  strcpy(data,           pszFileName);
  strcpy(data+1024,       pszModality);
  strcpy(data+1024+18,    pszStationName);
  strcpy(data+1024+18+18, pszSop);
         data[1024+18+18+66] = N;
  strcpy(data+1024+18+18+66+4, patid);

  if(calling) strcpy(data+1024+18+18+66+4+66, calling);
  if(called)  strcpy(data+1024+18+18+66+4+66+18, called);
  if(script)  strcpy(data+1024+18+18+66+4+66+18+18, script);

  into_queue(*q, data);
}

// This routine queues all export converter requests. Use instead of CallExportConverters

void QueueExportConverters(char *pszFileName, char *pszModality, char *pszStationName, char *pszSop, char *patid, char *calling, char *called)
{ char		szRootSC[64];
//  char		szEntry[32];
  char		szTemp[32];
  int		i, iNbConverters;

  if (!MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig,
		szRootSC, 64, ConfigFile)) return;
  if (!MyGetPrivateProfileString(szRootSC, "ExportConverters", "0",
		szTemp, 128, ConfigFile)) return;
  iNbConverters = atoi(szTemp);
  if (!iNbConverters) return;
  if (iNbConverters>MAXExportConverters) iNbConverters=MAXExportConverters;

  if (!eqN) 
  { eqN = (struct conquest_queue **)malloc(iNbConverters * sizeof(struct conquest_queue *));
    memset(eqN, 0, iNbConverters * sizeof(struct conquest_queue *));
  };

  /* Loop over all converters */
  for(i=0; i<iNbConverters; i++)
  { export_queueN(&eqN[i], pszFileName, i, pszModality, pszStationName, pszSop, patid, calling, called);
  }
}

//////////////////////////////////////////////////////////////////////////////////
// This code is a queue for filecopy requests; it is used to speed mirror copying
//////////////////////////////////////////////////////////////////////////////////

// return TRUE if it worthwile to retry (copy failed)

BOOL copyprocess(char *data, ExtendedPDU_Service *PDU, char *dum)
{ int devlen = *(unsigned char*)(data+2048);
  char s[1024];

  for (int sIndex = devlen; sIndex<strlen(data+1024); sIndex++)
    if (data[1024+sIndex]==PATHSEPCHAR)
    { strcpy(s, data+1024);
      s[sIndex]='\0';
      mkdir(s);
    }

  if (DFileCopy2(data, data+1024, 0))
  { OperatorConsole.printf("Mirror copy: %s\n", data+1024);
    return FALSE;
  }
  else
  { OperatorConsole.printf("***Writing mirror copy failed: %s\n", data+1024);
    return TRUE;
  }
  UNUSED_ARGUMENT(PDU);
  UNUSED_ARGUMENT(dum);
}

void mirrorcopy_queue(char *pszFileName, char *pszDestination, int devlen)
{ char data[2050];
  static struct conquest_queue *q = NULL;

  if (!q) 
  { q = new_queue(QueueSize, 2050, 100, copyprocess, NULL, 0);
    sprintf(q->failfile, "CopyFailures%s", Port);
  }
  if (pszFileName==NULL) return;

  strcpy(data,           pszFileName);
  strcpy(data+1024,      pszDestination);
  data[2048] = devlen;

  into_queue(q, data);
}

//////////////////////////////////////////////////////////////////////////////////
// This code is a queue for prefetch and preforward requests
//////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// read all files of a patient (to populate cache) use windows call for windows version
BOOL TestFile(char *FullFilename, char *status);

static int DFileRead(char *source, unsigned int MaxRead)
{
	BOOL		err_status = TRUE; 	/* Error status                 */
	char		*copybuffer;        	/* The copy buffer              */
	unsigned 	bufsize;            	/* Size of copy buffer          */
	unsigned long	bytes, totbytes;      	/* Actual transferred bytes     */
#ifdef WIN32
        HANDLE          handle = CreateFile(source, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if( handle == NULL) return FALSE;
#else
	/* Open input file for read-only. parser error if operation fails.  */
	int inhandle = sopen( source, O_RDONLY | O_BINARY, SH_DENYNO, S_IREAD);
	if( inhandle == -1) return FALSE;
#endif

	/* get a large buffer */
	bufsize = 0x4000;
	copybuffer = (char *)malloc(bufsize);
	if (copybuffer == NULL)
   		{
#ifdef WIN32
	        CloseHandle(handle);
#else
		close(inhandle);
#endif

		return -1;
		}

	totbytes=0;

	while( /*!eof(inhandle) && */err_status)
		{
#ifdef WIN32
                ReadFile(handle, copybuffer, bufsize, &bytes, NULL);
#else
		bytes= (unsigned)read( inhandle, copybuffer, bufsize);
#endif

//#ifndef WIN32
		if (bytes == 0)
       			break;
//#endif

		if (bytes == (unsigned) -1)
			{ 
			err_status= FALSE;
       			break;
			}

		totbytes += (bytes+512)>>10;
		if (MaxRead && totbytes>(MaxRead>>10)) break;
		}

	free(copybuffer);
#ifdef WIN32
        CloseHandle(handle);
#else
	close(inhandle);
#endif
	return totbytes;
}

char LastPrefetch[65] = "";

BOOL
PrefetchPatientData(char *PatientID, unsigned int MaxRead, int Thread)
	{
	Database aDB;
	char Tables[512], DeviceName[512], ObjectFile[1024], FileNameFrom[1024];
	char QueryString[512], PatientIDValue[512], PhysicalFrom[1024];
	SQLLEN SQLResultLength;
	BOOL Status=TRUE;
	char error[256];
	int count=0;

	if (strcmp(PatientID, LastPrefetch)==0) return TRUE;

	OperatorConsole.printf("Prefetching patient %s\n", PatientID);

	sprintf(Tables, "%s", ImageTableName);

	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Unable to open database %s as user %s on %s\n",
					DataSource, UserName, DataHost);
		return FALSE;
		}
	
	// MakeSafeString(PatientID, PatientIDValue);
	strcpy(PatientIDValue, PatientID);
	DICOM2SQLValue(PatientIDValue);	// allow exact match only
	sprintf(QueryString, 	"DICOMImages.ImagePat = %s", PatientIDValue);

	if (!aDB.Query(Tables, "DICOMImages.DeviceName, DICOMImages.ObjectFile", QueryString, NULL))
		{
		OperatorConsole.printf("***Unable to query for image records for prefetch\n");
		aDB.Close();
		return FALSE;
		}

	if (Thread) Progress.printf("Process=%d, Type='prefetchpatientdata', Active=1\n", Thread);
	aDB.BindField (1, SQL_C_CHAR, DeviceName, 255, &SQLResultLength);
	aDB.BindField (2, SQL_C_CHAR, ObjectFile, 255, &SQLResultLength);

	while (aDB.NextRecord())
		{ 	
		if (!FindPhysicalDevice(DeviceName, PhysicalFrom, ObjectFile)) continue;
		strcpy(FileNameFrom, PhysicalFrom);
		strcat(FileNameFrom, ObjectFile);

		SystemDebug.printf("Prefetching file: %s\n", FileNameFrom);

		if (MaxRead<0)
			{
			if (!TestFile(FileNameFrom, error))
				{
				OperatorConsole.printf("***Prefetch read failed for file: %s (%s)\n", FileNameFrom, error);
				Status = FALSE;
				}
			}
		else
			{
			if (DFileRead(FileNameFrom, MaxRead)<0)
				{
				OperatorConsole.printf("***Prefetch read failed for file: %s\n", FileNameFrom);
				Status = FALSE;
				}
			}
		if (Thread) Progress.printf("Process=%d, Total=%d, Current=%d\n", Thread, 100, count++);
		}

	aDB.Close();	
	strcpy(LastPrefetch, PatientID);
	if (Thread) Progress.printf("Process=%d, Active=0\n", Thread);

	return Status;
	}

// move data from this to other server: returns TRUE is meaningful to retry
// returns 0 for success; 1 for error; 2 for retryable error
// if imagetype contains a / it is considered a split move, e.g. 0/2 and 1/2 send odd and even images
static int DcmMove(const char *patid, char* pszSourceAE, char* pszDestinationAE, const char *studyuid, const char *seriesuid, const char *compress, const char *modality, const char *date, 
		    const char *sop, const char *imagetype, const char *seriesdesc, int id, char *script, int Thread)
{	PDU_Service		PDU;
	DICOMCommandObject	DCO;
	DICOMCommandObject	DCOR;
	DICOMDataObject		DDO;
	DICOMDataObject		DDOR;
	UID			uid;
	VR			*vr;
	LE_UINT16		command, datasettype, messageid, priority;
	BYTE			SOP[64];
	BYTE			AppTitle[64], host[64], port[64], compr[64];
	int			rc = 0;
	char			buffer[64];
	char			szRootSC[64];

	if (strcmp(pszSourceAE, "(local)")==0)
  	  pszSourceAE = (char *)MYACRNEMA;

	PDU.AttachRTC(&VRType);

	PDU.ClearAbstractSyntaxs();
	PDU.SetLocalAddress((unsigned char*)MYACRNEMA);
	PDU.SetRemoteAddress((unsigned char*)pszSourceAE);

	uid.Set("1.2.840.10008.3.1.1.1");	// Application context (DICOM App)
	PDU.SetApplicationContext(uid);
	uid.Set("1.2.840.10008.5.1.4.1.2.1.2");	// patientrootmove
	PDU.AddAbstractSyntax(uid);
	uid.Set("1.2.840.10008.5.1.4.1.2.2.2");	// studyrootmove
	PDU.AddAbstractSyntax(uid);

	if (strcmp(pszSourceAE, (char *)MYACRNEMA)==0)
		{
		if(!PDU.Connect((BYTE *)"127.0.0.1", Port)) 
			{
			OperatorConsole.printf("***forward xxxx to: local server not working\n");
			return 1;	// self not working: no retry
			}
		messageid = id;
		}
	else
		{
		if(!GetACRNema(pszSourceAE, (char *)host, (char *)port, (char *)compr)) 
			{
			OperatorConsole.printf("***preretrieve: AE not found: %s\n", pszSourceAE);
			return 1; 	// wrong address no retry
			}
		if(!PDU.Connect(host, port)) 
			{
			OperatorConsole.printf("***preretrieve: could not connect to AE: %s\n", pszSourceAE);
			return 2;	// remote not working: retry
			}
		messageid = id;	// special to indicate move to itself - no import/exportconverters
		}

	if (*studyuid || *seriesuid || *sop || *modality || *date || *imagetype || *seriesdesc) 
          sprintf((char*) SOP, "1.2.840.10008.5.1.4.1.2.2.2");  // study-root move
	else 
          sprintf((char*) SOP, "1.2.840.10008.5.1.4.1.2.1.2"); // patient-root move
	
	if (Thread) Progress.printf("Process=%d, Type='dicommove', Active=1\n", Thread);

	vr = new VR (0x0000, 0x0002, strlen((char*)SOP), (void*) SOP, FALSE);
	DCO.Push(vr);
	command = 0x0021;
	vr = new VR (0x0000, 0x0100, 0x0002, &command, FALSE);
	DCO.Push(vr);
	priority = 0;
	vr = new VR (0x0000, 0x0700, 0x0002, &priority, FALSE);
	DCO.Push(vr);
	datasettype = 0x0102;	
	vr = new VR (0x0000, 0x0800, 0x0002, &datasettype, FALSE);
	DCO.Push(vr);

	vr = new VR (0x0000, 0x0110, 0x0002, &messageid, FALSE);
	DCO.Push(vr);

	memset((char*)AppTitle, ' ', 20);
	memcpy((void*)AppTitle, pszDestinationAE, strlen(pszDestinationAE));
	vr = new VR(0x0000, 0x0600, 16, (void*)&AppTitle[0], FALSE);
	DCO.Push(vr);

	if      (*sop)       SetStringVR(&vr, 0x0008, 0x0052, "IMAGE"); 
	else if (*imagetype) SetStringVR(&vr, 0x0008, 0x0052, "IMAGE"); 
	else if (*modality)  SetStringVR(&vr, 0x0008, 0x0052, "SERIES"); 
	else if (*date)      SetStringVR(&vr, 0x0008, 0x0052, "SERIES"); 
	else if (*seriesuid) SetStringVR(&vr, 0x0008, 0x0052, "SERIES"); 
	else if (*seriesdesc)SetStringVR(&vr, 0x0008, 0x0052, "SERIES"); 
	else if (*studyuid)  SetStringVR(&vr, 0x0008, 0x0052, "STUDY"); 
	else                 SetStringVR(&vr, 0x0008, 0x0052, "PATIENT"); 
	DDO.Push(vr);

	if (*patid)
		{
		SetStringVR(&vr, 0x0010, 0x0020, patid); 
		DDO.Push(vr);
		}
	if (*studyuid && id!=7777)
		{
		SetStringVR(&vr, 0x0020, 0x000d, studyuid); 
		DDO.Push(vr);
		}
	if (*studyuid && id==7777) // lazy move by accession number
		{
		SetStringVR(&vr, 0x0008, 0x0050, studyuid); 
		DDO.Push(vr);
		}
	if (*seriesuid)
		{
		SetStringVR(&vr, 0x0020, 0x000e, seriesuid); 
		DDO.Push(vr);
		}
	if (*sop)
		{
		SetStringVR(&vr, 0x0008, 0x0018, sop); 
		DDO.Push(vr);
		}
	if (*modality)
		{
		SetStringVR(&vr, 0x0008, 0x0060, modality); 
		DDO.Push(vr);
		}
	if (*date)
		{
		SetStringVR(&vr, 0x0008, 0x0020, date); 
		DDO.Push(vr);
		}
	if (*imagetype)
		{
		if (strchr(imagetype, '/'))
			{
			vr = new VR(0x9999, 0x0b00, strlen(imagetype), (void*)imagetype, FALSE );
			DCO.Push(vr);
			}
		else
			{
			vr = new VR(0x0008, 0x0008, strlen(imagetype), (void*)imagetype, FALSE );
			DDO.Push(vr);
			}
		}
	if (*seriesdesc)
		{
                vr = new VR(0x0008, 0x103e, strlen(seriesdesc), (void*)seriesdesc, FALSE );
		DDO.Push(vr);
		}
	if (*compress)
		{
		SetStringVR(&vr, 0x9999, 0x0700, compress); 
		DCO.Push(vr);
		}
	if (*script)
		{
		SetStringVR(&vr, 0x9999, 0x0900, script); 
		DCO.Push(vr);
		}

	uid.Set(SOP);
	PDU.Write(&DCO, uid);
	PDU.Write(&DDO, uid);
	
	while(TRUE)
		{
		if(!PDU.Read(&DCOR))
			{
			OperatorConsole.printf("***preretrieve/forward xxx to: association lost\n");
			if (Thread) Progress.printf("Process=%d, Active=0\n", Thread);
			return 2;	// associate lost: may retry
			}

		if(DCOR.GetUINT16(0x0000, 0x0100)!=0x8021)
			{
			OperatorConsole.printf("***preretrieve/forward xxx to: invalid C-move response\n");
			rc = 1;
			goto EXIT; 	// not a C-MOVE-RSP ? error no retry
			}
		
		if(DCOR.GetUINT16(0x0000, 0x0800)!=0x0101)
			{	
			PDU.Read(&DDOR);// ignore the data set
			DDOR.Reset();
			}

		if(DCOR.GetUINT16(0x0000, 0x0900)==0x0000)
			{
			rc = 0;
			goto EXIT;	// completed OK, no retry
			}
		else if (DCOR.GetUINT16(0x0000, 0x0900)==0xff00)
			{		// continuing no problem
			int iNbSent = DCOR.GetUINT16(0x0000, 0x1021);
			int iNbToGo = DCOR.GetUINT16(0x0000, 0x1020);
			DCOR.Reset();
			if (Thread) Progress.printf("Process=%d, Total=%d, Current=%d\n", Thread, iNbSent+iNbToGo, iNbSent);
			continue;
			}
		else
			{
			OperatorConsole.printf("***preretrieve/forward xxx to: remote DICOM error\n");
			NonDestructiveDumpDICOMObject(&DCOR);
			rc = 1;	// remote DICOM error: NO retry FOR NOW
			if (MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile))
				{
				MyGetPrivateProfileString(szRootSC, "RetryForwardRemoteDICOMError", "0", buffer, 64, ConfigFile);
				if (atoi(buffer)) rc = 2;
				}
			goto EXIT;
			}
		DCOR.Reset();
		}
EXIT:
	PDU.Close();
  	if (Thread) Progress.printf("Process=%d, Active=0\n", Thread);
	return rc;
}

// move data from this to other server controlled by DICOMDataObject, returns error string

char *DcmMove2(char* pszSourceAE, const char* pszDestinationAE, BOOL patroot, int id, DICOMDataObject *DDO, DICOMDataObject *DCOextra, lua_State *L)
{	ExtendedPDU_Service	PDU;
	DICOMCommandObject	DCO;
	DICOMCommandObject	DCOR;
	DICOMDataObject		DDOR;
	UID			uid;
	VR			*vr;
	LE_UINT16		command, datasettype, messageid, priority;
	BYTE			SOP[64], AppTitle[64], host[64], port[64], compr[64];
	BOOL			rc = FALSE;
	const char		*callback = NULL;
	static char		retvalue[80]="";

	if (strcmp(pszSourceAE, "(local)")==0)
  	  pszSourceAE = (char *)MYACRNEMA;

	PDU.AttachRTC(&VRType);

	PDU.ClearAbstractSyntaxs();
	PDU.SetLocalAddress((unsigned char*)MYACRNEMA);
	PDU.SetRemoteAddress((unsigned char*)pszSourceAE);

	uid.Set("1.2.840.10008.3.1.1.1");	// Application context (DICOM App)
	PDU.SetApplicationContext(uid);
	uid.Set("1.2.840.10008.5.1.4.1.2.1.2");	// patientrootmove
	PDU.AddAbstractSyntax(uid);
	uid.Set("1.2.840.10008.5.1.4.1.2.2.2");	// studyrootmove
	PDU.AddAbstractSyntax(uid);

	callback = "none";
	if (lua_isnumber(L, 5)) callback = lua_tostring(L, 5);
	if (lua_isstring(L, 5)) callback = lua_tostring(L, 5);
	if (lua_isfunction(L, 5)) callback = "function";

	if (atoi(callback)) 
	   Progress.printf("Process=%d, Type='dicommove', Active=1\n", atoi(callback));

	if (strcmp(pszSourceAE, (char *)MYACRNEMA)==0)
		{
		if(!PDU.Connect((BYTE *)"127.0.0.1", Port)) 
			{
			OperatorConsole.printf("***dicommove: local server not working\n");
			sprintf(StatusString, "***dicommove: local server not working");
			strcpy(retvalue, StatusString);
			return retvalue;
			}
		messageid = id;
		}
	else
		{
		if(!GetACRNema(pszSourceAE, (char *)host, (char *)port, (char *)compr)) 
			{
			OperatorConsole.printf("***dicommove: AE not found: %s\n", pszSourceAE);
			sprintf(StatusString, "***dicommove: AE not found: %s", pszSourceAE);
			if (atoi(callback)) Progress.printf("Process=%d, Active=0\n", atoi(callback));
			strcpy(retvalue, StatusString);
			return retvalue;
			}
		if(!PDU.Connect(host, port)) 
			{
			OperatorConsole.printf("***dicommove: could not connect to AE: %s\n", pszSourceAE);
			sprintf(StatusString, "***dicommove: could not connect to AE: %s", pszSourceAE);
			if (atoi(callback)) Progress.printf("Process=%d, Active=0\n", atoi(callback));
			strcpy(retvalue, StatusString);
			return retvalue;
			}
		messageid = id;	// special to indicate move to itself - no import/exportconverters
		}

	if (patroot) 
          sprintf((char*) SOP, "1.2.840.10008.5.1.4.1.2.1.2"); // patient-root move
	else 
          sprintf((char*) SOP, "1.2.840.10008.5.1.4.1.2.2.2");  // study-root move
	
	vr = new VR (0x0000, 0x0002, strlen((char*)SOP), (void*) SOP, FALSE);
	DCO.Push(vr);
	command = 0x0021;
	vr = new VR (0x0000, 0x0100, 0x0002, &command, FALSE);
	DCO.Push(vr);
	priority = 0;
	vr = new VR (0x0000, 0x0700, 0x0002, &priority, FALSE);
	DCO.Push(vr);
	datasettype = 0x0102;	
	vr = new VR (0x0000, 0x0800, 0x0002, &datasettype, FALSE);
	DCO.Push(vr);

	vr = new VR (0x0000, 0x0110, 0x0002, &messageid, FALSE);
	DCO.Push(vr);

	memset((char*)AppTitle, ' ', 20);
	memcpy((void*)AppTitle, pszDestinationAE, strlen(pszDestinationAE));
	vr = new VR(0x0000, 0x0600, 16, (void*)&AppTitle[0], FALSE);
	DCO.Push(vr);
	
	if (DCOextra)
	{ while((vr=DCOextra->Pop()))
	  { DCO.DeleteVR(vr);
	    DCO.Push(vr);
            //delete vr;
	  }
	}

	uid.Set(SOP);
	PDU.Write(&DCO, uid);
	PDU.Write(DDO, uid);
	
	while(TRUE)
		{
		if(!PDU.Read(&DCOR))
			{
			OperatorConsole.printf("***dicommove: association lost\n");
			sprintf(StatusString, "***dicommove: association lost");
			if (atoi(callback)) Progress.printf("Process=%d, Active=0\n", atoi(callback));
			strcpy(retvalue, StatusString);
			return retvalue;
			}

		if(DCOR.GetUINT16(0x0000, 0x0100)!=0x8021)
			{
			OperatorConsole.printf("***dicommove: invalid C-move response\n");
			sprintf(StatusString, "***dicommove: invalid C-move response");
			if (atoi(callback)) Progress.printf("Process=%d, Active=0\n", atoi(callback));
			strcpy(retvalue, StatusString);
			PDU.Close();
			return retvalue;
			}
		
		if(DCOR.GetUINT16(0x0000, 0x0800)!=0x0101)
			{	
			PDU.Read(&DDOR);// ignore the data set
			DDOR.Reset();
			}

		if(DCOR.GetUINT16(0x0000, 0x0900)==0x0000)
			{
			rc = FALSE;
			*StatusString=0;
			goto EXIT;	// completed OK, no retry
			}
		else if (DCOR.GetUINT16(0x0000, 0x0900)==0xff00)
			{		// continuing no problem
			int iNbSent = DCOR.GetUINT16(0x0000, 0x1021);
			int iNbToGo = DCOR.GetUINT16(0x0000, 0x1020);
			sprintf(StatusString, "dicommove: %d of %d images transferred", iNbSent, iNbSent+iNbToGo);

			if (atoi(callback))
				Progress.printf("Process=%d, Total=%d, Current=%d\n", atoi(callback), iNbSent+iNbToGo, iNbSent);
			else if (strcmp(callback, "function")==0)
				{
				}
			else if (strcmp(callback, "none")==0)
				{
				}
			else 
				luaL_dostring(L, callback);

			DCOR.Reset();
			continue;
			}
		else
			{
			OperatorConsole.printf("***dicommove: remote DICOM error\n");
			sprintf(StatusString, "***dicommove: remote DICOM error");
			if (atoi(callback)) Progress.printf("Process=%d, Active=0\n", atoi(callback));
			NonDestructiveDumpDICOMObject(&DCOR);
			strcpy(retvalue, StatusString);
			PDU.Close();
			return retvalue;
			}
		DCOR.Reset();
		}
EXIT:
  	if (atoi(callback)) Progress.printf("Process=%d, Type='dicommove', Active=0\n", atoi(callback));
	StatusString[0]=0;
	strcpy(retvalue, StatusString);
	PDU.Close();
	return retvalue;
}

// delete patient data from this server: always returns FALSE

static BOOL DcmDelete(char *patid, char *studyuid, char *seriesuid, char *modality, char *date, char *sop, char *imagetype)
{	DICOMDataObject		DDO;
	VR			*vr;

	if (*patid)
		{
		SetStringVR(&vr, 0x0010, 0x0020, patid); 
		DDO.Push(vr);
		}
	if (*studyuid)
		{
//                vr = new VR(0x0020, 0x000d, (strlen(studyuid)+1)&0xfe, (void*)studyuid, FALSE );
		SetStringVR(&vr, 0x0020, 0x000d, studyuid); 
		DDO.Push(vr);
		}
	if (*seriesuid)
		{
//                vr = new VR(0x0020, 0x000e, (strlen(seriesuid)+1)&0xfe, (void*)seriesuid, FALSE );
		SetStringVR(&vr, 0x0020, 0x000e, seriesuid); 
		DDO.Push(vr);
		}
	if (*sop)
		{
//                vr = new VR(0x0008, 0x0018, (strlen(sop)+1)&0xfe, (void*)sop, FALSE );
		SetStringVR(&vr, 0x0008, 0x0018, sop); 
		DDO.Push(vr);
		}
	if (*modality)
		{
		SetStringVR(&vr, 0x0008, 0x0060, modality); 
		DDO.Push(vr);
		}
	if (*date)
		{
		SetStringVR(&vr, 0x0008, 0x0020, date); 
		DDO.Push(vr);
		}
	if (*imagetype)
		{
                vr = new VR(0x0008, 0x0008, strlen(imagetype), (void*)imagetype, FALSE );
		DDO.Push(vr);
		}

	RemoveFromPACS(&DDO, FALSE);
	return FALSE;
}

int	DcmMergeStudy(const char *server, char *pat, char *study, char *modality, char *seriesdesc, char *script, ExtendedPDU_Service *PDU);
int	DcmSubmitData(char *pat, char *study, char *series, char *sop, char *script, 
		      char *modesubmit, char *hostsubmit, int portsubmit, char *ident, int Thread);

// runs in the thread
BOOL prefetchprocess(char *data, ExtendedPDU_Service *PDU, char *dum)
{ if (memicmp(data+65, "prefetch",     8)==0) PrefetchPatientData(data, 0, 0);
  if (memicmp(data+65, "forward ",     8)==0) return DcmMove  (data, (char *)MYACRNEMA, data+82, data+100, data+165, data+230, data+256, data+276, data+300, data+366, data+500, 5, data+570, 0)==2;
  if (memicmp(data+65, "preretrieve", 11)==0) return DcmMove  (data, data+82, (char *)MYACRNEMA, data+100, data+165, data+230, data+256, data+276, data+300, data+366, data+500, 0xfbad, data+570, 0)==2;
  if (memicmp(data+65, "get ",         4)==0) return DcmMove  (data, data+82, (char *)MYACRNEMA, data+100, data+165, data+230, data+256, data+276, data+300, data+366, data+500, 0xfbad, data+570, 0)==2;
  if (memicmp(data+65, "delete ",      7)==0) DcmDelete(data, data+100, data+165, data+256, data+276, data+300, data+366);
  if (memicmp(data+65, "merge ",       6)==0) DcmMergeStudy ("local", data, data+100, data+256, data+500, data+570, PDU);
  if (memicmp(data+65, "submit ",      7)==0) DcmSubmitData (data, data+100, data+165, data+300, data+570, "sftp", data+366, 22, data+500, 0);
  if (memicmp(data+65, "submit2 ",     8)==0) DcmSubmitData (data, data+100, data+165, data+300, data+570, "other", data+366, 22, data+500, 0);
  if (memicmp(data+65, "process ",     8)==0) 
  { char *p = strchr(data+230, ' ');
    if (!p) 
    { p = strchr(data+230, '(');
      *p = ' ';
      char *q = strrchr(data+230, ')');
      if (q) *q=0;
    }
    if (p && memicmp(p-4, ".lua", 4)==0)
    { struct scriptdata sd1 = {PDU, NULL, NULL, -1, NULL, NULL, NULL, NULL, NULL, 0, 0};
      char script[512]; int i, n;
      strcpy(script, "dofile('"); // allow absolute path only!
      *p=0;
      for (i=0, n=8; i<strlen(data+230); i++)
      { if (data[i+230]=='\\')
        { script[n++] = '\\';
          script[n++] = '\\';
	}
	else 
	{ script[n++] = data[i+230];
	}
      }
      script[n++] = 0;
      strcat(script, "')");
      //OperatorConsole.On();

      lua_setvar(PDU, "command_line",    p+1);      
      lua_setvar(PDU, "version",         DGATE_VERSION);
      do_lua(&(PDU->L), script, &sd1);
    }
    else
#ifdef WIN32
      WinExec(data+230, SW_MINIMIZE);     
#else
      system(data+230);
#endif
  }

  return FALSE;
  UNUSED_ARGUMENT(dum);
}

// blocks the thread so as not to prefetch too often
BOOL mayprefetchprocess(char *data, ExtendedPDU_Service *PDU, char *dum)
{ if ((*(unsigned int*)(data+996)) + (*(unsigned int*)(data+992)) > (unsigned int)time(NULL)) return FALSE; // hold until e.g. 10 minutes past
  return TRUE;
  UNUSED_ARGUMENT(PDU);
  UNUSED_ARGUMENT(dum);
}

ExtendedPDU_Service prefetchPDU; // for prefetch script context

// enter prefetch or preforward request into queue
BOOL prefetch_queue(const char *operation, char *patientid, const char *server, const char *studyuid, 
		    const char *seriesuid, const char *compress, const char *modality, const char *date, 
		    const char *sop, const char *imagetype, const char *seriesdesc, int delay, const char *script)
{ char data[1000];
  static struct conquest_queue *q = NULL;
  
  if (delay==0) delay = ForwardCollectDelay;

  if (!q) 
  { char szRootSC[64], szTemp[32];

  prefetchPDU.SetLocalAddress ( (BYTE *)"prefetch" );
  prefetchPDU.SetRemoteAddress ( (BYTE *)"prefetch" );
  prefetchPDU.ThreadNum = 0;

    q = new_queue(QueueSize, 1000, 100, prefetchprocess, &prefetchPDU, 0);
    q->mayprocess = mayprefetchprocess;
    sprintf(q->failfile, "DelayedFetchForwardFailures%s", Port);

    MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile);
    MyGetPrivateProfileString(szRootSC, "MaximumDelayedFetchForwardRetries", "0", szTemp, 128, ConfigFile);
    q->maxfails   = atoi(szTemp);
  }

  memset(data, 0,  1000);
  strncpy(data,     patientid, 64);
  strncpy(data+65,  operation, 16);
  *(unsigned int*)(data+996) = time(NULL);
  *(unsigned int*)(data+992) = delay;

  if (memicmp(data+65, "process ",     8)==0) 
  { strncpy(data+100, studyuid,  64);
    strncpy(data+165, seriesuid, 64);
    strncpy(data+230, server,   360);
    return into_queue_unique(q, data, 490);
  }
  else
  { strncpy(data+82,  server,    17);
    strncpy(data+100, studyuid,  64);
    strncpy(data+165, seriesuid, 64);
    strncpy(data+230, compress,  25);
    strncpy(data+256, modality,  19);
    strncpy(data+276, date,      23);
    strncpy(data+300, sop,       64);
    strncpy(data+366, imagetype, 128);
    strncpy(data+500, seriesdesc,64);
    if (script) strncpy(data+570, script,    400);
    return into_queue_unique(q, data, 992);
  }
}

//////////////////////////////////////////////////////////////////////////////////
// This queue is intended to prefetch (in disk cache) patient data 
// likely to be requested soon in the time that the server is inactive
// Enter P+patientID or S+studyUID to be read using into_queue or enter 
// empty item to abort current prefetch
//////////////////////////////////////////////////////////////////////////////////

static BOOL WINAPI prefetcherthread(struct conquest_queue *q)
{ char data[66], curitem[65];
  Database aDB;
  char DeviceName[512], ObjectFile[1024], FileNameFrom[1024];
  char QueryString[512], QueryValue[512], PhysicalFrom[1024];
  SQLLEN SQLResultLength;
//  BOOL Status=TRUE;
  BOOL qact = FALSE;
  int  count, kb;
  time_t TimeOfDay;

#ifdef __GNUC__ //Warnings
  count = 0;
  kb = 0;
  TimeOfDay = 0;
#endif
  curitem[0]=0;

  while (1)
  { while (1)
    { EnterCriticalSection(&q->critical);

      if (q->top!=q->bottom) 
      { memcpy(data, q->data + q->bottom * q->entrysize, q->entrysize);
        q->bottom = (q->bottom+1)%q->num;
        LeaveCriticalSection(&q->critical);

	if (data[0]==0 && qact)		// abort current prefetch
	{ int skipped = 0;
          while (aDB.NextRecord()) skipped++;
          OperatorConsole.printf("Aborted prefetch after reading %d images (%d MB) in %d s, %d images skipped\n", count, (kb+512)>>10, (int)time(NULL) - TimeOfDay, skipped);
          qact = FALSE;
	  curitem[0]=0;
          aDB.Close();
	  break;
	};

	if (data[0]==0) break;		// ignore abort when not active
        if (strcmp(curitem, data+1)==0) break;	// already prefetched

        if (!qact)
	  if(!aDB.Open(DataSource, UserName, Password, DataHost))
	  { OperatorConsole.printf("***Unable to open database for prefetch\n");
	    break;
          };

	//MakeSafeString(data+1, QueryValue);
	strcpy(QueryValue, data+1);
	DICOM2SQLValue(QueryValue);	// allow exact match only

        TimeOfDay = time(NULL);
        kb = 0;
        count = 0;

        if (data[0]=='P')
  	{ OperatorConsole.printf("Prefetching patient %s\n", data+1);
          sprintf(QueryString, 	"DICOMImages.ImagePat = %s", QueryValue);
	  if (!aDB.Query("DICOMImages", "DICOMImages.DeviceName, DICOMImages.ObjectFile", QueryString, NULL))
	  { OperatorConsole.printf("***Unable to query for image records for prefetch\n");
	    break;
          };
        }
        else
        { OperatorConsole.printf("Prefetching study %s\n", data+1);
          sprintf(QueryString, 	"DICOMSeries.StudyInsta = %s and DICOMSeries.SeriesInst = DICOMImages.SeriesInst", QueryValue);
	  if (!aDB.Query("DICOMSeries, DICOMImages", "DICOMImages.DeviceName, DICOMImages.ObjectFile", QueryString, NULL))
	  { OperatorConsole.printf("***Unable to query on study for image records for prefetch\n");
	    break;
          };
        };

	aDB.BindField (1, SQL_C_CHAR, DeviceName, 255, &SQLResultLength);
	aDB.BindField (2, SQL_C_CHAR, ObjectFile, 255, &SQLResultLength);
	qact = TRUE;
	strcpy(curitem, data+1);

	// do not break: delay a bit
      }
      else if (qact)
      { LeaveCriticalSection(&q->critical);

        if (aDB.NextRecord())
	{ if (!FindPhysicalDevice(DeviceName, PhysicalFrom, ObjectFile)) break;
	  strcpy(FileNameFrom, PhysicalFrom);
	  strcat(FileNameFrom, ObjectFile);

	  SystemDebug.printf("Prefetching file: %s\n", FileNameFrom);
  	  kb += DFileRead(FileNameFrom, 0);
          count++;
          break;
	}
	else
        { qact = FALSE;
          aDB.Close();
          OperatorConsole.printf("Prefetch done: read %d images (%d MB) in %d s\n", count, (kb+512)>>10, (int)time(NULL) - TimeOfDay);
        }
      }
      else
        LeaveCriticalSection(&q->critical);

      Sleep(q->delay);
    }
  }

  DeleteCriticalSection(&q->critical);

  return TRUE;
}

struct conquest_queue *prefetcherqueue;
ExtendedPDU_Service prefetcherPDU; // for prefetcher script context

struct conquest_queue *new_prefetcherqueue(void)
{ struct conquest_queue *result;
#ifdef WIN32
  unsigned long ThreadID;
#endif
  int num=1000, size = 66;

  prefetcherPDU.SetLocalAddress ( (BYTE *)"prefetcher" );
  prefetcherPDU.SetRemoteAddress ( (BYTE *)"prefetcher" );
  prefetcherPDU.ThreadNum = 0;

  result               = new conquest_queue;
  result->top          = 0;
  result->bottom       = 0;
  result->num          = num;
  result->entrysize    = size;
  result->delay        = 10;
  result->process      = NULL;
  result->mayprocess   = NULL;
  result->data         = (char *)malloc(num * size);
  result->fails        = 0;
  result->lastfailtime = 0;
  result->failfile[0]  = 0;
  result->PDU	       = &prefetcherPDU;
  result->opentime     = 0;
  result->pdu_refreshdelay = 3600;
  result->pdu_closedelay   = 5;
  result->pdu_release      = 0;
  result->ForwardLastUID   = NULL;

  InitializeCriticalSection(&result->critical);

#ifdef WIN32
  result->threadhandle = 
    CreateThread(NULL, 2097152, (LPTHREAD_START_ROUTINE) prefetcherthread, result, 0, &ThreadID);
#else
  pthread_create(&result->threadhandle, NULL, (void*(*)(void*))prefetcherthread, (void *)result);
  pthread_detach(result->threadhandle);
#endif

  return result;
}

// Control prefetcher from queries or moves

static int prefetchermode=0;

BOOL prefetcher(DICOMDataObject *DDO, BOOL move)
{ VR *pVR = NULL;
  char id[66];
  int len;

  if (!prefetchermode) return FALSE;

  if (!prefetcherqueue) 
    prefetcherqueue = new_prefetcherqueue();

  if (!DDO) return FALSE;

  memset(id, 0, 66);

  if (move)
  { pVR = DDO->GetVR(0x0008, 0x0018);
    if (pVR && pVR->Length) return FALSE;	// do not abort on single sop move

    into_queue(prefetcherqueue, id);	   	// abort on all other moves
    return FALSE;
  }

  pVR = DDO->GetVR(0x0010, 0x0020);		// get patientid
  if (!pVR)
    id[0] = 0;
  else
  { id[0] = 'P';
    strncpy(id+1, (char*)pVR->Data, pVR->Length);
    id[pVR->Length+1] = 0;
    len = pVR->Length - 1;
    while(len>0)
    { if (id[len+1] == ' ')
        id[len+1] = 0;
      else
        break;
      len--;
    }
  }

  // no patient id or wildcard provided: try study
  if (id[0]==0 || id[1]==0 || strchr(id, '*')!=NULL)
  { pVR = DDO->GetVR(0x0020, 0x000d);
    if (!(pVR && pVR->Length)) return FALSE;	// no study: do not affect prefetch

    id[0]='S';					// prefetch on study (for e.g. kpacs)
    strncpy(id+1, (char*)pVR->Data, pVR->Length);
    id[pVR->Length+1] = 0;
  }

  // any other query will start prefetch of patient or study
  into_queue(prefetcherqueue, id);
  return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// This code is for worklist processing
//////////////////////////////////////////////////////////////////////////////////

// will replace all items in object with worklist fields (if filled) for AccessionNumber
// returns TRUE if replacement was succesfull: AccessionNumber was found or no entries to replace
// Note: since PatientId and StudyInstanceUID may be changed by this operation, this operation
// cannot be done after the original images were entered into the server. I will need to consider
// generating new SOP and Series UID's in this case

BOOL ApplyWorklist(DICOMDataObject *DDOPtr, Database *DB)
	{
	DBENTRY	*DBE;
	int Index, I;
	char Fields[1024];
	char QueryString[256];
	VR *vr;
	char *Data[100]; 
	SQLLEN SQLResultLength[100];

	if (!WorkListDB[0].Element)				// no worklist DB
		return FALSE;

	DBE = WorkListDB;
	Fields[0]=0;

	vr = DDOPtr->GetVR(DBE[0].Group, DBE[0].Element);	// query string for AccessionNumber
	if (vr)
		{
		strcpy(QueryString, DBE[0].SQLColumn);
//		strcat(QueryString, DBE[0].SQLColumn);
		strcat(QueryString, " = '");
		QueryString[strlen(QueryString) + vr->Length]=0;
		memcpy(QueryString+strlen(QueryString), vr->Data, vr->Length);
		if (QueryString[strlen(QueryString)-1]==' ') QueryString[strlen(QueryString)-1]=0;
		strcat(QueryString, "'");

//		strcat(QueryString, " and PatientID = '");	// patient ID cannnot be used: may be the same in image as in worklist !
//		vr = DDOPtr->GetVR(0x0010, 0x0020);
//		memcpy(QueryString+strlen(QueryString), vr->Data, vr->Length);
//		if (QueryString[strlen(QueryString)-1]==' ') QueryString[strlen(QueryString)-1]=0;
//		strcat(QueryString, "'");
		}
	else
		return FALSE;

	if (vr->Length==0)
		return FALSE;					// accessionnumber not filled in

	Index = 1; 						// do not read AccessionNumber from db
	I = 0;
	while ( TRUE )
		{
		if(!DBE[Index].Element) break;			// end of fields
		vr = DDOPtr->GetVR(DBE[Index].Group, DBE[Index].Element);
		if (vr && DBE[Index].DICOMType!=DT_STARTSEQUENCE && DBE[Index].DICOMType!=DT_ENDSEQUENCE)
			{					// field is real and present in image
			strcat(Fields, DBE[Index].SQLColumn);
			strcat(Fields, ",");
			++I;
			}
		++Index;
		}
	Fields[strlen(Fields)-1]=0;				// remove trailing ,
        
	if (I==0) return TRUE;					// nothing to do

								// no db access
	if (!DB->Query(WorkListTableName, Fields, QueryString, NULL))
		return FALSE;

	Index = 1; 						// do not read AccessionNumber from db
	I = 0;
	while ( TRUE )
		{
		if(!DBE[Index].Element) break;			// end of fields
		vr = DDOPtr->GetVR(DBE[Index].Group, DBE[Index].Element);
		if (vr && DBE[Index].DICOMType!=DT_STARTSEQUENCE && DBE[Index].DICOMType!=DT_ENDSEQUENCE)
			{					// field is real and present in image
			Data[I] = (char *)malloc(256);		// data area for db read
			DB->BindField (I+1, SQL_C_CHAR, Data[I], 255, SQLResultLength+I);
			Data[I][0]=0;
			++I;
			}
		++Index;
		}

	if (!DB->NextRecord())					// no record found ?
		return FALSE;

	Index = 1; 						// do not read AccessionNumber from db
	I = 0;
	while ( TRUE )
		{
		if(!DBE[Index].Element) break;
		vr = DDOPtr->GetVR(DBE[Index].Group, DBE[Index].Element);
		if (vr && DBE[Index].DICOMType!=DT_STARTSEQUENCE && DBE[Index].DICOMType!=DT_ENDSEQUENCE)
			{					// field is real and present in image

#if 0								// mvh 20051208
			SQLResultLength[I] = strlen(Data[I]);
#endif

			if (SQLResultLength[I])			// only replace if db field contains data
				{
				if (SQLResultLength[I] & 1)	// make length even
					{
					if (DBE[Index].DICOMType==DT_UI)
						Data[I][SQLResultLength[I]++] = 0;	// uid
					else
						Data[I][SQLResultLength[I]++] = ' ';	// text
					}
				Data[I][SQLResultLength[I]] = 0;
					
				vr->ReAlloc(SQLResultLength[I]);
				memcpy(vr->Data, (void*)Data[I], SQLResultLength[I]);
				}

			free(Data[I]);
			++I;
			}
		++Index;
		}

	return TRUE;						// success
	}


// Save a dicom composite data object to disk and update database; returns filename
// Warning: Filename must point to an array of 1024 characters that is filled

BOOL    NewDeleteSopFromDB(char *pat, char *study, char *series, char *sop, Database &aDB);

int
SaveToDisk(Database	*DB, DICOMCommandObject *DCO, DICOMDataObject	*DDOPtr, char 	*Filename, BOOL NoKill, ExtendedPDU_Service *PDU, int Syntax, BOOL nopreget)
	{
//	FILE		*fp;
	DICOMDataObject	DDO;
	//PDU_Service	PDU;
	Debug		AutoRoute;
	char		s[1024];
	char		rRoot[1024], rFilename[1024], MirrorFilename[1024];
	char		Device[256], MirrorDevice[256], Device2[256];
//	char		SOPInstanceUID[256];
	int		len, UseChapter10, devlen, has_dcm_extension=0;
	unsigned int	sIndex;
	VR*		pVR;
	char		szModality[66];
	char		szStationName[66];
	char		szSop[66], szSeries[66], szStudy[66];
	char		patid[66];
	BOOL		bForcedImplicit = FALSE;
	char		*p;
	char		Storage[64];
	BOOL		rewrite=FALSE;

	char calling[20];
	char called[20];

	memset(calling, 0, 20);
	strcpy(calling, (char *)(((AAssociateAC *)PDU)->CallingApTitle));
        if (calling[0]>32)
	  while (calling[strlen(calling)-1]==' ') calling[strlen(calling)-1] = 0;

	memset(called, 0, 20);
	strcpy(called, (char *)(((AAssociateAC *)PDU)->CalledApTitle));
        if (called[0]>32)
  	  while (called[strlen(called)-1]==' ') called[strlen(called)-1] = 0;

	//PDU.AttachRTC(&VRType);

	if (WorkListMode==1) 			// optionally update information from worklist db
		ApplyWorklist(DDOPtr, DB);
	else if (WorkListMode==2) 		// compulsory update information from worklist db
		{
		if (!ApplyWorklist(DDOPtr, DB)) 
			{
			OperatorConsole.printf("***[WorkListMode 2] Worklist entry (AccessionNumber) not found; object not saved\n");
			//return FALSE;

			int rc = CallImportConverterN(DCO, DDOPtr, 2200, NULL, NULL, NULL, NULL, PDU, NULL, NULL);
			if (rc==7)	// retry command
				{
				if (!ApplyWorklist(DDOPtr, DB)) 
					{
					OperatorConsole.printf("***[WorkListMode 2] Worklist entry (AccessionNumber) not found on retry; object not saved\n");
					return FALSE;
					}
				}
			else
				{
				delete DDOPtr;
				return FALSE;
				}
			}
		}

	FixImage(DDOPtr);	// Make sure the required DICOM UID's are there + fix patient ID if enabled

	/* PDU.SaveDICOMDataObject destroys the contents of DDOPtr. We need to know
	   the modality in order to decide whether ExportConverters or ImportConverters 
	   should be called. Also, importconverters may modify dicom tags. 
	   Get the original data here...
	*/
	SearchDICOMObject(DDOPtr, "0008,0060", szModality);
	SearchDICOMObject(DDOPtr, "0008,1010", szStationName);
	SearchDICOMObject(DDOPtr, "0008,0018", szSop);
	SearchDICOMObject(DDOPtr, "0020,000e", szSeries);
	SearchDICOMObject(DDOPtr, "0020,000d", szStudy);
	SearchDICOMObject(DDOPtr, "0010,0020", patid);

	Storage[0]=0;
	if (!nopreget)
		{ 
		int rc = CallImportConverters(DCO, DDOPtr, szModality, szStationName, szSop, patid, PDU, Storage);
		if (rc==2)
			{
			// the import converter destroyed the image: silently cancel store
			Filename[0]=0;
			delete DDOPtr;
			return TRUE;
			}
		else if (rc==6)
			{
			// the import converter reject the image: cancel store and complain
			Filename[0]=0;
			delete DDOPtr;
			return FALSE;
			}

		// These might have changed ... reload
		SearchDICOMObject(DDOPtr, "0008,0060", szModality);
		SearchDICOMObject(DDOPtr, "0008,1010", szStationName);
		SearchDICOMObject(DDOPtr, "0008,0018", szSop);
		SearchDICOMObject(DDOPtr, "0020,000e", szSeries);
		SearchDICOMObject(DDOPtr, "0020,000d", szStudy);
		SearchDICOMObject(DDOPtr, "0010,0020", patid);
		}

	// make a filename for the new object; note: rRoot (patient directory part of rFilename) is not used any longer
	*rRoot=1;
	if (!GenerateFileName(DDOPtr, Device, rRoot, rFilename, NoKill, Syntax, (char *)called, (char *)calling, DB))
        	{  
		//OperatorConsole.printf("***Failed to create filename for DICOM object (is it a slice ?)\n");
#ifdef FAILSAFE_STORAGE
		strcpy(rFilename, "failsafe_storage\\");
		GenUID(rFilename + strlen(rFilename));
		strcat(rFilename, ".v2");
		OperatorConsole.printf("***There was a DB problem, failsafe storage created following filename: %s\n", rFilename);
#else
           	return FALSE; //!!!!!!!!! added mvh+ljz 19980409
#endif
        	}
	if (memcmp(rFilename, "lua:", 4)==0) lua_getstring(PDU, NULL, DDOPtr, rFilename+4, rFilename);

	// newly written image (not rewritten image): use selection preferred storage if enough space
	if (*rRoot)
		{
		if (Storage[0])
			{
			int PreferredDevice = atoi(Storage+3);
			if(CheckFreeStoreOnMAGDevice(PreferredDevice) > 30)
				sprintf(Device, "MAG%d", PreferredDevice);
			}
		}
	else
	  	rewrite=TRUE;

	// If MAG is full, may write to MIRROR, but database will say MAG
	if (memicmp(Device, "MIRROR", 6)==0)
		sprintf(Device2, "MAG%d", atoi(Device+6));
	else
		strcpy(Device2, Device);	// fix 20040606 (was sprintf(Device2, "MAG%d", atoi(Device+3));)

	// do not rewrite image not on MAG but it is not an error (warning is generated earlier)
	if (memicmp(Device2, "MAG", 3)!=0)
		{
		Filename[0]=0;
		delete DDOPtr;	// 20040402
		return TRUE;
		}

	// When written replace virtual MAG device, i.e. MAG0.1 (for VirtualServerFor1) with real one: MAG0
	p = strchr(Device2, '.');
	if (p && p[1]>='0' && p[1]<='9') 
	{ *p = 0;
	  rewrite=FALSE;
	}

	// try to enter object into database (object is not saved when this fails)
	if(!SaveToDataBase(*DB, DDOPtr, rFilename, Device2, *rRoot))
		{
		OperatorConsole.printf("***Error saving to SQL: %s\n", rFilename);
#ifndef FAILSAFE_STORAGE
		int rc = CallImportConverterN(DCO, DDOPtr, 2100, NULL, NULL, NULL, NULL, PDU, NULL, NULL);
		if (rc==7)	// retry command
			{
			if(!SaveToDataBase(*DB, DDOPtr, rFilename, Device2, *rRoot))
				{
				OperatorConsole.printf("***Error saving to SQL on retry: %s\n", rFilename);
				}
			}
		else
			{
			delete DDOPtr;	// 20090616
			return FALSE;
			}
#endif
		}

	// Make the patient directory (and any required subdirectories), that might not be there
	GetPhysicalDevice(Device, Filename);
	devlen = strlen(Filename);
	strcat(Filename, rFilename);

	for (sIndex = devlen; sIndex<=strlen(Filename); sIndex++)
		if (Filename[sIndex]==PATHSEPCHAR)
			{
			strcpy(s, Filename);
			s[sIndex]='\0';
			mkdir(s);
			}

	// Find a mirror device if it exists (will not do this if MIRROR is being written because MAG if full)
	if (memicmp(Device, "MAG", 3)==0)
		{
		sprintf(MirrorDevice, "MIRROR%d", atoi(Device+3));
		if (!GetPhysicalDevice(MirrorDevice, MirrorFilename))
			MirrorDevice[0]=0;
		}
	else
		MirrorDevice[0]=0;

	// Make the mirror patient directory (and any required subdirectories), that might not be there
	if (MirrorDevice[0])
		{
		devlen = strlen(MirrorFilename);
		strcat(MirrorFilename, rFilename);

#if DONTQUEUEMIRRORCOPY
		for (sIndex = devlen; sIndex<strlen(MirrorFilename); sIndex++)
			if (MirrorFilename[sIndex]==PATHSEPCHAR)
				{
				strcpy(s, MirrorFilename);
				s[sIndex]='\0';
				mkdir(s);
				}
#endif
		}

	// check the filename to determine format; this avoids format change on rewrite
	len = strlen(rFilename);
	if (len>4)
		has_dcm_extension = (stricmp(rFilename+len-3, ".v2")!=0);

	if (*TestMode) strcat(rFilename, TestMode);

	/* Type of save depends on file name extension is .dcm (UseChapter10) 
	   and on the transfer syntax (not ImplicitLittleEndian, not
	   ExplicitLittleEndian and not ExplicitBigEndian)
	*/
	pVR = DDOPtr->GetVR(0x0002, 0x0010);	// TransferSyntaxUID
	if (pVR && pVR->Data)
		{
		strncpy(s, (char*)pVR->Data, pVR->Length);
		s[pVR->Length] = 0;
		if ((strcmp(s, "1.2.840.10008.1.2")   != 0) &&
		    (strcmp(s, "1.2.840.10008.1.2.1") != 0) &&
		    (strcmp(s, "1.2.840.10008.1.2.2") != 0))
			{
			if (!has_dcm_extension)
				{
				OperatorConsole.printf("SaveToDisk: cannot rewrite jpeg/rle image in v2 format: %s\n", Filename);
				Filename[0]=0;
				delete DDOPtr;
				return TRUE;
				}
			UseChapter10 = has_dcm_extension || NoDicomCheck;
			}
		else
			{
			UseChapter10 = has_dcm_extension || NoDicomCheck;
			//bForcedImplicit = TRUE; // 20040406
			}
		}
	else
		UseChapter10 = has_dcm_extension || NoDicomCheck;

	// is it nki compressed and attempt to store as .dcm --> decompress ?
	if (DDOPtr->GetVR(0x7fdf, 0x0010)!=NULL && DDOPtr->GetVR(0x7fe0, 0x0010)==NULL && has_dcm_extension)
		{
		OperatorConsole.printf("SaveToDisk: *** warning: cannot write NKI compressed in dcm format, decompressing: %s\n", Filename);
		DecompressNKI(DDOPtr);
		}
		
	int t = time(NULL);
	if (!UseChapter10)
		PDU->SaveDICOMDataObject(Filename, ACRNEMA_VR_DUMP, DDOPtr);
	else if (bForcedImplicit)
	 	PDU->SaveDICOMDataObject(Filename, DICOM_CHAPTER_10_IMPLICIT, DDOPtr);
	else
	// OR (tested OK) chapter 10 format (does not compress) code in filepdu.cxx (dicom.lib)
	 	PDU->SaveDICOMDataObject(Filename, DICOM_CHAPTER_10_EXPLICIT, DDOPtr);
	SaveTime += time(NULL)-t;

	// 20080817: check file was actually written, if not report error and delete database entry
	if (DFileSize(Filename)==0)
		{
		OperatorConsole.printf("***Error writing file: %s\n", Filename);
		NewDeleteSopFromDB(patid, szStudy, szSeries, szSop, *DB);
		return FALSE;
		}

	if (MirrorDevice[0])
		{
#if DONTQUEUEMIRRORCOPY
                if (DFileCopy2(Filename, MirrorFilename, 0))
			OperatorConsole.printf("Mirror copy: %s\n", MirrorFilename);
		else
			OperatorConsole.printf("***Writing mirror copy failed: %s\n", MirrorFilename);
#else
		mirrorcopy_queue(Filename, MirrorFilename, devlen);
#endif
		}

	// notify the autorouter that a dicom object has landed
	if ( Edition != E_PERSONAL )
		{
		if(szSop[0]!='\0')
			AutoRoute.printf("%s", szSop);
		}

	// notify additional executables that a dicom object has landed; note: szModality, szStationName, szSop, and patid 
	// filters work on the original dicom tags; not on those thay were changed by import converters; while %V etc are changed

	if (!nopreget)
#if DONTQUEUEEXPORTCONVERTERS
		CallExportConverters(Filename, szModality, szStationName, szSop, patid, (char *)calling, (char *)called, NULL, NULL);
#else
        	QueueExportConverters(Filename, szModality, szStationName, szSop, patid, (char *)calling, (char *)called);
#endif

	delete DDOPtr;

	return ( rewrite?2:TRUE );
	}

static BOOL WINAPI monitorthread(char *folder)
{ Database DB;
  ExtendedPDU_Service PDU; // for script context of monitoring thread
  PDU.SetLocalAddress ( (BYTE *)"monitor" );
  PDU.SetRemoteAddress ( (BYTE *)"monitor" );
  if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
	{
	OperatorConsole.printf("***Error Connecting to SQL\n");
	return ( FALSE );
	}

  while (TRUE)
  { LoadAndDeleteDir(folder, NULL, &PDU, 0, NULL, &DB);
    Sleep(1000);
  }

  return TRUE;
}

static char monitorfolder[1024], monitorfolder2[1024];

void StartMonitorThread(char *global_folder)
{ 
#ifdef WIN32
  unsigned long ThreadID;
  CreateThread(NULL, 2097152, (LPTHREAD_START_ROUTINE) monitorthread, global_folder, 0, &ThreadID);
#else
  pthread_t ThreadID;
  pthread_create(&ThreadID, NULL, (void*(*)(void*))monitorthread, (void *)global_folder);
  pthread_detach(ThreadID);
#endif
}

static BOOL ZipTriggered=FALSE;
static BOOL CleanTriggered=FALSE;
static BOOL GoZipThread=FALSE;
static BOOL ScriptTriggered=FALSE;

BOOL	PanicKillOff(unsigned int MAGThreshHold);

static BOOL WINAPI zipthread(void)
{ OperatorConsole.printf("Started zip and cleanup thread\n");

  while (GoZipThread)
  { Sleep(1000);
    
    char timehhmmss[16], szRootSC[64], logfolder[512], ziptime[64], NightlyCleanThreshhold[64];
    struct tm tmbuf;
    time_t t = time(NULL);
    localtime_r(&t, &tmbuf);
    sprintf(timehhmmss, "%02d:%02d:%02d", tmbuf.tm_hour, tmbuf.tm_min, tmbuf.tm_sec);

    *ziptime=0;
    *NightlyCleanThreshhold=0;

    if (MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile))
    { MyGetPrivateProfileString(szRootSC, "ziptime", "", ziptime, 64, ConfigFile);
      MyGetPrivateProfileString(szRootSC, "NightlyCleanThreshhold", "0", NightlyCleanThreshhold, 64, ConfigFile);
    }
    
    if (memicmp(timehhmmss, ziptime, strlen(ziptime))==0 && strlen(ziptime))
    { if (!ZipTriggered)
      { // does the log folder exists
	strcpy(logfolder, BaseDir); strcat(logfolder, "logs");
	if (!dgate_IsDirectory(logfolder))continue;

	// create zip file name for today or yesterday
	if (atoi(ziptime)<8) t = time(NULL) - 24*3600;
	else                 t = time(NULL);
        localtime_r(&t, &tmbuf);
	sprintf(logfolder + strlen(logfolder), "%clogs_%04d%02d%02d.zip", PATHSEPCHAR, tmbuf.tm_year+1900, tmbuf.tm_mon+1, tmbuf.tm_mday);

	// do not overwrite current zip
	if (!DFileExists(logfolder))
	{ char b[512];

          // something to zip
	  strcpy(b, BaseDir);
	  strcat(b, "serverstatus.log");
          if (!DFileExists(b)) continue;

          // zip the relevant files
          ZipTriggered = TRUE;
#ifdef WIN32
	  sprintf(b, "-y -tzip a \"%s\" \"%s*.log\" \"%s*.ini\" \"%s*.sql\" \"%s*.map\" \"%s*.lst\"", logfolder, BaseDir, BaseDir, BaseDir, BaseDir, BaseDir);
  	  BackgroundExec("7za.exe", b);
#else
	  sprintf(b, "7za -y -tzip a \"%s\" \"%s*.log\" \"%s*.ini\" \"%s*.sql\" \"%s*.map\" \"%s*.lst\"", logfolder, BaseDir, BaseDir, BaseDir, BaseDir, BaseDir);
	  system(b);
#endif
          // delete the usual log file stuff that was just zipped
	  strcpy(b, BaseDir); strcat(b, "serverstatus.log");        unlink(b);
	  strcpy(b, BaseDir); strcat(b, "PacsTrouble.log");         unlink(b);
	  strcpy(b, BaseDir); strcat(b, "PacsUser.log");            unlink(b);
	  strcpy(b, BaseDir); strcat(b, "maintenance.log");         unlink(b);
	  strcpy(b, BaseDir); strcat(b, "installation.log");        unlink(b);
	  strcpy(b, BaseDir); strcat(b, "conquestdicomserver.log"); unlink(b);

          OperatorConsole.printf("Dgate zipped todays log files as: %s\n", logfolder);
	}
      }
    }
    else
      ZipTriggered = FALSE;

    if (memicmp(timehhmmss, "01:00", strlen("01:00"))==0 && atoi(NightlyCleanThreshhold))
    { if (!CleanTriggered)
      { CleanTriggered = TRUE;
	if (LargestFreeMAG()<(unsigned int)atoi(NightlyCleanThreshhold))
	{ OperatorConsole.printf("Dgate is performing disk cleanup\n");
          PanicKillOff((unsigned int)atoi(NightlyCleanThreshhold));
	}
      }
    }
    else
      CleanTriggered = FALSE;
      
    if (memicmp(timehhmmss, "03:00", strlen("03:00"))==0)
    { if (!ScriptTriggered)
      { ScriptTriggered = TRUE;
        char cmd[1024];
	MyGetPrivateProfileString("lua", "nightly", "", cmd, 1024, ConfigFile);
	EnterCriticalSection(&dolua_critical);
        globalPDU.SetLocalAddress ( (BYTE *)"global" );
        globalPDU.SetRemoteAddress ( (BYTE *)"nightly" );
	globalPDU.ThreadNum = 0;
        struct scriptdata sd = {&globalPDU, NULL, NULL, 0, NULL, NULL, NULL, NULL, NULL, 0, 0};
	do_lua(&(globalPDU.L), cmd, &sd);
	LeaveCriticalSection(&dolua_critical);
	// if (sd.DDO) delete sd.DDO;
      }
    }
    else
      ScriptTriggered = FALSE;

    if (!ScriptTriggered)
    { ScriptTriggered = TRUE;
      char cmd[1024];
      MyGetPrivateProfileString("lua", "background", "", cmd, 1024, ConfigFile);
      EnterCriticalSection(&dolua_critical);
      globalPDU.SetLocalAddress ( (BYTE *)"global" );
      globalPDU.SetRemoteAddress ( (BYTE *)"background" );
      globalPDU.ThreadNum = 0;
      struct scriptdata sd = {&globalPDU, NULL, NULL, 0, NULL, NULL, NULL, NULL, NULL, 0, 0};
      do_lua(&(globalPDU.L), cmd, &sd);
      LeaveCriticalSection(&dolua_critical);
      // if (sd.DDO) delete sd.DDO;
      ScriptTriggered = FALSE;
    }
  }
  OperatorConsole.printf("Stopped zip and cleanup thread\n");
  return TRUE;
}

void StartZipThread(void)
{ if (GoZipThread) return;

  GoZipThread = TRUE;
#ifdef WIN32
  unsigned long ThreadID;
  CreateThread(NULL, 2097152, (LPTHREAD_START_ROUTINE) zipthread, NULL, 0, &ThreadID);
#else
  pthread_t ThreadID;
  pthread_create(&ThreadID, NULL, (void*(*)(void*))zipthread, (void *)NULL);
  pthread_detach(ThreadID);
#endif
}

void StopZipThread(void)
{ GoZipThread = FALSE;
  Sleep(2000);
}

// some help for the simple user
BOOL
PrintOptions ()
	{
	fprintf(stderr, "\n");
	fprintf(stderr, "DGATE: UCDMC/NKI DICOM server thread and PACS utility application %s\n", DGATE_VERSION);
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "(1) DGATE <-!#|-v|-u#>        Report as in dicom.ini|stdout|UDP(#=port)\n");
	fprintf(stderr, "          [-^|-l|-Lfile|-$]   GUI/Normal/Debug log to file, binary output\n");
	fprintf(stderr, "          [-p#|-hAE|-qIP|-b]  Set port|AE|Target IP|Single thread debug mode\n");
	fprintf(stderr, "          [-wDIR]             Set the working directory for dgate(ini,dic,...)\n");
	fprintf(stderr, "          [-i|-r|-arDEV[,dir]]Init|Init/regenerate DB|Regen dirs single device\n");
	fprintf(stderr, "          [-d|-m|-k]          List (-d) devices (-m) AE map (-k) DICOM.SQL\n");
	fprintf(stderr, "          [-t|-o]             Test console|Test database\n");
	fprintf(stderr, "          [-sOpt|-esap d u p] Create ODBC source (WIN32), database with SApw\n");
	fprintf(stderr, "          [-nd|-nc#,FILE]     NKI de-/compress# FILE\n");
	fprintf(stderr, "          [-nu IN OUT]        Generic decompress NKI file\n");
	fprintf(stderr, "          [-jd|-jc#,FILE]     JPEG de-/compress# FILE\n");
	fprintf(stderr, "          [-js|-jjFILE]       DICOM to json summary;complete with pixel data\n");
	fprintf(stderr, "          [-j*##|-j-##,FILE]  Recompress FILE to ##\n");
	fprintf(stderr, "          [-as#,N|-amFROM,TO] Select#KB to archive of MAGN|move device data\n");
	fprintf(stderr, "          [-au|-aeFROM,TO]    Undo select for archiving|rename device\n");
	fprintf(stderr, "          [-av|-atDEVICE]     Verify mirror disk|Test read files for DEVICE\n");
	fprintf(stderr, "          [-abJUKEBOX1.2,N]   Make cacheset to burn JUKEBOX1,CD2 from MAGN\n");
	fprintf(stderr, "          [-acJUKEBOX1.2]     Verify JUKEBOX1,CD2 against cacheset\n");
	fprintf(stderr, "          [-adJUKEBOX1.2]     Verify and delete cacheset for JUKEBOX1, CD2\n");
	fprintf(stderr, "          [-f<p|t|s|i>ID]     Delete DB for Patient, sTudy, Series, Image\n");
	fprintf(stderr, "          [-f<e|d|z>file]     Enter/Delete DB of file, Zap server file\n");
	fprintf(stderr, "          [-faFILE<,ID>]      Add file to server<optionally change PATID>\n");
	fprintf(stderr, "          [-zID]              Delete (zap) patient\n");
	fprintf(stderr, "          [-frDEVICE,DIR]     Regen single directory DIR on DEVICE\n");
	fprintf(stderr, "          [-f<c|k>PATID,file] Change/Kopy PATID of file (irreversible/once)\n");
	fprintf(stderr, "          [-f?file|-fu|-c#]   get UID of file|Make new UID|UID helper(0..99)\n");
	fprintf(stderr, "          [-ff#]              Delete old patients until #MB free\n");
	fprintf(stderr, "          [-gSERVER,DATE]     grab images from SERVER of date not on here\n");
	fprintf(stderr, "          [-yQUERY_STRING]    run as cgi client avoiding environment\n");
	fprintf(stderr, "                              Otherwise: run as threaded server, port=1111\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "(2) DGATE FileMapping         Run server child; shared memory has socket#\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "(3) DGATE <-pPORT> <-qIP> --command:arguments\n");
	fprintf(stderr, "                              Send command to (this or other) running server\n");
	fprintf(stderr, "                              (works directly - use with care)\n");
	fprintf(stderr, "Delete options:\n");
	fprintf(stderr, "    --deleteimagefile:file                  Delete given image file from server\n" );
	fprintf(stderr, "    --deletepatient:patid                   Delete given patient from server\n" );
	fprintf(stderr, "    --deletestudy:patid:studyuid            Delete given study from server\n" );
	fprintf(stderr, "    --deletestudies:date(range)             Delete studies from server on date\n" );
	fprintf(stderr, "    --deleteseries:patid:seriesuid          Delete given series from server\n" );
	fprintf(stderr, "    --deleteimage:patid:sop                 Delete given image from server\n" );
	fprintf(stderr, "    --deleteimagefromdb:file                Delete given file from db only\n" );
	fprintf(stderr, "    --deletesopfromdb:pat,study,series,sop  Delete specified image from db only\n" );
	fprintf(stderr, "\n");
	fprintf(stderr, "DICOM move options:\n");
	fprintf(stderr, "    --movepatient:source,dest,patid         Move patient, source e.g. (local)\n" );
	fprintf(stderr, "    --movestudy:source,dest,patid:studyuid  Move study, patid: optional\n" );
	fprintf(stderr, "    --moveaccession:source,dest,patid:acc   Move by Accession#, patid: optional\n" );
	fprintf(stderr, "    --movestudies:source,dest,date(range)   Move studies on date\n" );
	fprintf(stderr, "    --moveseries:src,dst,patid:seruid,stuid Move series patid: optional\n" );
	fprintf(stderr, "    --move:src,dst,p,st,ser,sop             Move patient..image\n" );
	fprintf(stderr, "\n");
	fprintf(stderr, "Modification of dicom objects:\n");
	fprintf(stderr, "    --modifypatid:patid,file  Change patid of given file\n" );
	fprintf(stderr, "    --anonymize:patid,file    Anonymize given file\n" );
	fprintf(stderr, "    --modifystudy:p,st,script Change patient or study\n" );
	fprintf(stderr, "    --modifyseries:p,se,scrip Change series\n" );
	fprintf(stderr, "    --modifier:p,st,se,so,ch,s Change(ch=1)/copy pat/st/ser/sop\n" );
	fprintf(stderr, "    --modifyimage:file,script Change single image (filename passed)\n" );
	fprintf(stderr, "    --mergestudy:uid,uid,..   Start merging studies with given studyuids\n" );
	fprintf(stderr, "    --mergestudyfile:file     Use to process all files to merge\n" );
	fprintf(stderr, "    --mergeseries:uid,uid,..  Start merging series with given seriesuids\n" );
	fprintf(stderr, "    --mergeseriesfile:file    Use to process all files to merge\n" );
	fprintf(stderr, "    --attachanytopatient:any,sample     Modify uids to attach any object to\n" );
	fprintf(stderr, "    --attachanytostudy:any,sample        patient|study|series in sample file\n" );
	fprintf(stderr, "    --attachanytoseries:any,sample       Do not attach same at different levels\n" );
	fprintf(stderr, "    --attachrtplantortstruct:plan,struc Attach rtplan to rtstruct\n" );
	fprintf(stderr, "\n");
	fprintf(stderr, "Maintenance options:\n");
	fprintf(stderr, "    --initializetables:       Clear and create database\n" );
	fprintf(stderr, "    --initializetables:1      Clear and create database without indices\n" );
	fprintf(stderr, "    --initializetables:2      Clear and create worklist database\n" );
	fprintf(stderr, "    --regen:                  Re-generate entire database\n" );
	fprintf(stderr, "    --regendevice:device      Re-generate database for single device\n" );
	fprintf(stderr, "    --regendir:device,dir     Re-generate database for single directory\n" );
	fprintf(stderr, "    --regenfile:file          Re-enter given file in database\n" );
	fprintf(stderr, "    --makespace:#             Delete old patients to make #MB space\n" );
	fprintf(stderr, "    --quit:                   Stop the server\n" );
	fprintf(stderr, "    --safequit:               Stop the server when not active\n" );
	fprintf(stderr, "\n");
	fprintf(stderr, "Logging options:\n");
	fprintf(stderr, "    --debuglog_on:file/port   Start debug logging\n" );
	fprintf(stderr, "    --log_on:file/port/pipe   Start normal logging\n" );
	fprintf(stderr, "    --debuglevel:#            Set debug logging level\n" );
	fprintf(stderr, "    --display_status:file     Display server status\n" );
    	fprintf(stderr, "    --status_string:file      Display status string of submit operation\n" );
	fprintf(stderr, "    --checklargestmalloc:     Estimates DICOM object size limit\n" );
	fprintf(stderr, "    --get_freestore:dev,fmt   Report free #Mb on device\n" );
	fprintf(stderr, "    --testmode:#              Append # to dicom filenames\n" );
	fprintf(stderr, "    --echo:AE,file            Echo server; show response\n" );
	fprintf(stderr, "\n");
	fprintf(stderr, "Configuration options:\n");
	fprintf(stderr, "    --get_param:name,fmt      Read any parameter from DICOM.INI\n" );
	fprintf(stderr, "    --get_ini_param:name,fmt  Read any parameter from DICOM.INI\n" );
	fprintf(stderr, "    --get_ini_num:index,fmt   List any entry from DICOM.INI\n" );
	fprintf(stderr, "    --get_ini:fmt             List all entries from DICOM.INI\n" );
	fprintf(stderr, "    --put_param:name,value    Write any parameter to DICOM.INI\n" );
	fprintf(stderr, "    --delete_param:name       Delete any parameter from DICOM.INI\n" );
	fprintf(stderr, "    --read_ini:               Re-read all parameters from DICOM.INI\n" );
	fprintf(stderr, "    --get_amap:index,fmt      List any entry from ACRNEMA.MAP\n" );
	fprintf(stderr, "    --get_amaps:fmt           List all entries from ACRNEMA.MAP\n" );
	fprintf(stderr, "    --put_amap:i,AE,ip,p#,cmp Write entry in memory for ACRNEMA.MAP\n" );
	fprintf(stderr, "    --delete_amap:index       Delete entry in memory for ACRNEMA.MAP\n" );
	fprintf(stderr, "    --write_amap:             Write ACRNEMA.MAP from memory to disk\n" );
	fprintf(stderr, "    --read_amap:              Re-read ACRNEMA.MAP from disk to memory\n" );
	fprintf(stderr, "    --get_sop:index,fmt       List any accepted service class UID\n" );
	fprintf(stderr, "    --put_sop:index,UID,name  Write/add accepted service class UID\n" );
	fprintf(stderr, "    --delete_sop:index        Delete accepted service class UID\n" );
	fprintf(stderr, "    --get_transfer:index,fmt  List any accepted transfer syntax\n" );
	fprintf(stderr, "    --put_transfer:in,UID,nam Write/add accepted transfer syntax\n" );
	fprintf(stderr, "    --delete_transfer:index   Delete accepted transfer syntax\n" );
	fprintf(stderr, "    --get_application:idx,fmt List any accepted application UID\n" );
	fprintf(stderr, "    --put_application:i,U,n   Write/add accepted application UID\n" );
	fprintf(stderr, "    --delete_application:inde Delete accepted application UID\n" );
	fprintf(stderr, "    --get_localae:index,fmt   List any accepted local AE title\n" );
	fprintf(stderr, "    --put_localae:in,AE,name  Write/add accepted local AE title\n" );
	fprintf(stderr, "    --delete_localae:index    Delete accepted local AE title\n" );
	fprintf(stderr, "    --get_remoteae:index,fmt  List any accepted remote AE title\n" );
	fprintf(stderr, "    --put_remoteae:in,AE,name Write/add accepted remote AE title\n" );
	fprintf(stderr, "    --delete_remoteae:index   Delete accepted remote AE title\n" );
	fprintf(stderr, "    --get_dic:index,fmt       List any dicom dictionary item\n" );
	fprintf(stderr, "    --mk_binary_dic:filename  Save dictionary in faster binary form\n" );
	fprintf(stderr, "    --get_sqldef:level,in,fmt List any database field definition\n" );
	fprintf(stderr, "\n");
	fprintf(stderr, "Communication options:\n");
	fprintf(stderr, "    --addimagefile:file,patid      Copy file into server, optionally new patid\n" );
	fprintf(stderr, "    --addlocalfile:file,patid      Copy local file into server, opt. new patid\n" );
	fprintf(stderr, "    --attachfile:file,script       Copy local file into server, process with script\n" );
	fprintf(stderr, "    --loadanddeletedir:dir,patid   Load folder and delete its contents\n" );
	fprintf(stderr, "    --loadhl7:file                 Load HL7 data into worklist\n" );
	fprintf(stderr, "    --dump_header:filein,fileout   Create header dump of file\n" );
        fprintf(stderr, "    --forward:file,mode,server     Send file with compr. mode to server\n");
	fprintf(stderr, "    --grabimagesfromserver:AE,date Update this server from other\n" );
	fprintf(stderr, "    --prefetch:patientid           Prefetch all images for improved speed\n" );
	fprintf(stderr, "    --browsepatient:searchstring   Select patient in windows GUI\n" );
	fprintf(stderr, "    --submit:p,s,s,s,target,pw,scr Immediate sftp submit of data\n" );
	fprintf(stderr, "    --submit2:p,s,s,s,target,c,scr Immediate submit with command line c\n" );
	fprintf(stderr, "    --export:p,st,ser,sop,file,scr Immediate process and zip/7z data\n" );
	fprintf(stderr, "    --scheduletransfer:options     Background sftp transfer as above\n" );
	fprintf(stderr, "\n");
	fprintf(stderr, "Test options:\n");
	fprintf(stderr, "    --genuid:                      Generate an UID\n" );
	fprintf(stderr, "    --changeuid:UID                Give new UID as generated now or before\n" );
	fprintf(stderr, "    --changeuidback:UID            Give old UID from one generated above\n" );
	fprintf(stderr, "    --checksum:string              Give checksum of string\n" );
	fprintf(stderr, "    --testcompress:file            Enter file in server with many compressions\n" );
	fprintf(stderr, "    --clonedb:AE                   Clone db from server for testing\n" );
	fprintf(stderr, "\n");
	fprintf(stderr, "Conversion options:\n");
        fprintf(stderr, "    --convert_to_gif:file,size,out,l/w/f Downsize and convert to mono GIF\n");
        fprintf(stderr, "    --convert_to_bmp:file,size,out,l/w/f Downsize and convert to color BMP\n");
        fprintf(stderr, "    --convert_to_jpg:file,size,out,l/w/f Downsize and convert to color JPG\n");
        fprintf(stderr, "    --convert_to_dicom:file,size,comp,f  Downsize/compress/frame DICOM to stdout\n");
        fprintf(stderr, "    --convert_to_json:file,size,comp,f,p Down/comp/frame to json p=pixeldata\n");
        fprintf(stderr, "    --extract_frames:file,out,first,last Select frames of DICOM file\n");
	fprintf(stderr, "    --count_frames:file                  report # frames in DICOM file\n");
        fprintf(stderr, "    --uncompress:file,out                Uncompress DICOM\n");
        fprintf(stderr, "    --compress:file,mode,out             Compress DICOM to mode e.g. J2\n");
        fprintf(stderr, "    --wadorequest:parameters             Internal WADO server (old)\n");
        fprintf(stderr, "    --wadoparse:query_string             Internal WADO server\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Database options:\n");
	fprintf(stderr, "    --query:table|fields|where|fmt|file Arbitrary query output to file\n" );
	fprintf(stderr, "    --query2:tab|fld|whe|fmt|max|file   Same but limit output rows to max\n" );
	fprintf(stderr, "    --patientfinder:srv|str|fmt|file    List patients on server\n" );
	fprintf(stderr, "    --studyfinder:srv|str|fmt|file      List studies on server\n" );
	fprintf(stderr, "    --seriesfinder:srv|str|fmt|file     List series on server\n" );
	fprintf(stderr, "    --imagefinder:srv|str|fmt|file      List images on server\n" );
	fprintf(stderr, "    --serieslister:srv|pat|stu|fmt|file List series in a study\n" );
	fprintf(stderr, "    --imagelister:srv|pat|ser|fmt|file  List (local) files in a series\n" );
	fprintf(stderr, "    --extract:PatientID = 'id'          Extract all tables to Xtable.dbf\n" );
	fprintf(stderr, "    --extract:                          Extract patient dbase table to XA..\n" );
	fprintf(stderr, "    --todbf:folder|table|query|sort|max Convert query result all fields to dbf\n");
	fprintf(stderr, "    --addrecord:table|flds|values       Append record, values must be in ''\n" );
	fprintf(stderr, "    --deleterecord:table,where          Delete record from table\n" );
	fprintf(stderr, "For DbaseIII without ODBC:\n");
	fprintf(stderr, "    --packdbf:                          Pack database, recreate memory index\n" );
	fprintf(stderr, "    --indexdbf:                         Re-create memory index\n" );
	fprintf(stderr, "\n");
	fprintf(stderr, "Archival options:\n");
	fprintf(stderr, "    --renamedevice:from,to              Rename device in database\n" );
	fprintf(stderr, "    --verifymirrordisk:device           Verify mirror disk for selected device\n" );
	fprintf(stderr, "    --testimages:device                 Test read all images on device\n" );
	fprintf(stderr, "    --movedatatodevice:to,from          Move patients from one device to another\n" );
	fprintf(stderr, "    --moveseriestodevice:to,from        Move series from one device to another\n" );
	fprintf(stderr, "    --selectlruforarchival:kb,device    Step 1 for archival: to device.Archival\n" );
	fprintf(stderr, "    --selectseriestomove:device,age,kb  Step 1 for archival: to device.Archival\n" );
	fprintf(stderr, "    --preparebunchforburning:to,from    Step 2 for archival: moves to cache\n" );
	fprintf(stderr, "    --deletebunchafterburning:deviceto  Step 3 for archival: deletes from cache\n" );
	fprintf(stderr, "    --comparebunchafterburning:deviceto Part step 3 - compare jukebox to cache\n" );
	fprintf(stderr, "    --restoremagflags:                  Undo archival sofar\n" );
	fprintf(stderr, "\n");
	fprintf(stderr, "Scripting options:\n");
	fprintf(stderr, "    --lua:chunk                         Run lua chunk in server, wait to finish\n" );
	fprintf(stderr, "    --globallua:chunk                   Run lua chunk in main thread in server\n" );
	fprintf(stderr, "    --luastart:chunk                    Run lua chunk in server, retn immediate\n" );
	fprintf(stderr, "    --dolua:chunk                       Run lua chunk in this dgate instance\n" );
	fprintf(stderr, "    --dolua:filename                    Run lua file in this dgate instance\n" );
	return ( TRUE );
	}

// external references
BOOL 	SelectLRUForArchival(char *Device, int KB, ExtendedPDU_Service *PDU);
BOOL	SelectSeriesForArchival(char *Device, int age, int kb);
BOOL	PrepareBunchForBurning(char *DeviceFrom, char *DeviceTo);
BOOL	MoveDataToDevice(char *DeviceFrom, char *DeviceTo);
BOOL	MoveSeriesToDevice(char *DeviceFrom, char *DeviceTo);
BOOL	RestoreMAGFlags();
BOOL	CompareBunchAfterBurning(char *DeviceTo);
BOOL	DeleteBunchAfterBurning(char *DeviceTo);
BOOL	VerifyMirrorDisk(char *DeviceFrom);
BOOL	RenameDevice(char *DeviceFrom, char *DeviceTo);
BOOL	TestImages(char *DeviceFrom);

// forward references
BOOL 	GrabImagesFromServer(BYTE *calledae, const char *studydate, char *destination, int Thread);

#if defined(__BORLANDC__) || defined(__WATCOMC__)
typedef BOOL (__stdcall *_SQLConfigDataSource)(HWND       hwndParent,
                                               WORD       fRequest,
                                               LPCSTR     lpszDriver,
                                               LPCSTR     lpszAttributes);
_SQLConfigDataSource __SQLConfigDataSource;
#define SQLConfigDataSource __SQLConfigDataSource
#define  ODBC_ADD_DSN     1               // Add data source
#endif

static char ServerCommandAddress[64] = "127.0.0.1";
BOOL DecompressNKI(char *file_in, char *file_out);

// Main routine for parsing the command line; return FALSE when not running
// as server or a socket # when running as server thread
int
ParseArgs (int	argc, char	*argv[], ExtendedPDU_Service *PDU)
	{
	UINT		Valid;
	int		valid_argc, mode1=0;
	int		Socketfd = 0;
	BOOL		bUIDPostfixSet = FALSE;
	BOOL		Logging = FALSE;

	Valid = 2;

	ConfigMicroPACS();

	UserLog.On(UserLogFile);
	UserLog.AddTimeStamps(TRUE);
	TroubleLog.On(TroubleLogFile);
	TroubleLog.AddTimeStamps(TRUE);

	valid_argc = 1;

	while ( valid_argc < argc )
		{
#ifndef UNIX	// Win32...
		if ((argv[valid_argc][0]=='-')||(argv[valid_argc][0]=='/'))
#else		// UNIX..
		if(argv[valid_argc][0]=='-')
#endif
			{
			switch ( argv[valid_argc][1] )
				{
				//case	'h':	// wait e.g., to allow attaching debugger
				//case	'H':
#ifdef WIN32
				//	getch();
#endif
				//	break;	//Done already.

				case	'w':	// set workdir for ini, map, dic
				case	'W':
					break;	//Done already.

				case	'v':	// verbose mode
				case	'V':
					SystemDebug.On();
					OperatorConsole.On();
					Logging=TRUE;
					break;

				case	'b':	// debug mode
				case	'B':
					SystemDebug.On();
					OperatorConsole.On();
					DebugLevel = 4;
					NoThread=TRUE;
					Logging=TRUE;
					break;

				case	'c':	// set UID counter
				case	'C':
					UIDPostfix = atoi(argv[valid_argc]+2);
					bUIDPostfixSet = TRUE;
					break;

				case	'u':
				case	'U':	// be verbose to specified pipe/udp
					if(argv[valid_argc][2] == PATHSEPCHAR)
						{
						OperatorConsole.OnMsgPipe(argv[valid_argc]+2);
						SystemDebug.OnMsgPipe(argv[valid_argc]+2);
						}
					else
						{
						OperatorConsole.OnUDP(OCPipeName, argv[valid_argc]+2);
						SystemDebug.OnUDP(OCPipeName, argv[valid_argc]+2);
						}
					Logging=TRUE;
					break;


				case	'!':	// be verbose to pipe/udp specified in DICOM.INI (no debug)
					if(OCPipeName[0] == PATHSEPCHAR)
						{
						OperatorConsole.OnMsgPipe(OCPipeName);
						}
					else
						{
                                                if (argv[valid_argc][2]>='0' && argv[valid_argc][2]<='9')
						{
							OperatorConsole.OnUDP(OCPipeName, argv[valid_argc]+2);
#if 0
							SystemDebug.OnUDP(OCPipeName, argv[valid_argc]+2);
#endif
						}
						else
							OperatorConsole.OnUDP(OCPipeName, "1111");
						}
					Logging=TRUE;
					break;

				case	'^':	// be verbose to passed file with timestamps (no debug)
					OperatorConsole.On(argv[valid_argc]+2);
					OperatorConsole.AddTimeStamps(1);
					StartZipThread();
					Logging=TRUE;
					break;

				case	'$':	// change stdout to binary
#ifdef WIN32
					setmode(fileno(stdout), O_BINARY);
#endif
					break;

				case	'#':	// be verbose to passed file with timestamps (with debug)
					OperatorConsole.On(argv[valid_argc]+2);
					OperatorConsole.AddTimeStamps(1);
					SystemDebug.On(argv[valid_argc]+2);
					SystemDebug.AddTimeStamps(1);
					StartZipThread();
					Logging=TRUE;
					break;

				case	'L':	// debug log verbose to passed file
					SystemDebug.On(argv[valid_argc]+2);
				case	'l':	// normal log verbose to passed file
					OperatorConsole.On(argv[valid_argc]+2);
					Logging=TRUE;
					break;

				case	'x':
					if (argv[valid_argc][2]=='!')
					  Progress.OnTCP(OCPipeName, argv[valid_argc]+3);
				        else
					  Progress.OnUDP(OCPipeName, argv[valid_argc]+2);
					break;

				case	'p':	// override dicom.ini port#
				case	'P':
					strcpy((char*)Port, argv[valid_argc]+2);
					++Valid;
					break;

				case	'q':	// override dicom.ini ServerCommandAddress (for dgate -- options)
				case	'Q':
					strcpy((char*)ServerCommandAddress, argv[valid_argc]+2);
					++Valid;
					break;

				case	'h':	// override dicom.ini MYACRNEMA
				case	'H':
					strcpy((char*)MYACRNEMA, argv[valid_argc]+2);
					++Valid;
					break;

				case	'r':	// init and regenerate database
				case	'R':
					RunServer = FALSE;
					NeedPack = 2;
					SystemDebug.printf("Regen Database\n");

					if(!LoadKFactorFile((char*)KFACTORFILE))
						{
						OperatorConsole.printf("***Could not load kfactor file: %s\n", KFACTORFILE);
						exit(1);
						}

					OperatorConsole.printf("Step 1: Re-intialize SQL Tables\n");

					mode1 = atoi(argv[valid_argc]+2);
					InitializeTables (mode1);

					OperatorConsole.printf("Step 2: Load / Add DICOM Object files\n");

					Regen();

					OperatorConsole.printf("Regeneration Complete\n");

					return ( FALSE );

				case	'd':	// List magnetic device status
				case	'D':
					RunServer = FALSE;
					NeedPack = FALSE;

					SystemDebug.Off();
					PrintFreeSpace();
					return ( FALSE );

				case	'm':	// list map of outgoing AE's
				case	'M':
					RunServer = FALSE;
					NeedPack = FALSE;

					if(!InitACRNemaAddressArray())
						{
						OperatorConsole.printf("***Error loading acr-nema map file:%s\n",ACRNEMAMAP);
						exit(1);
						}

					PrintAMap();
					return ( FALSE );


				case	'k':	// list K factor file (DICOM.SQL)
				case	'K':
					RunServer = FALSE;
					NeedPack = FALSE;

					if(!LoadKFactorFile((char*)KFACTORFILE))
						{
						OperatorConsole.printf("***Could not load kfactor file: %s\n", KFACTORFILE);
						exit (1);
						}

					PrintKFactorFile();
					return ( FALSE );


				case	't':	// test (TCP/IP) console connnection
				case	'T':
					{ 
					char lt[] = "This is a very long text output for testing -- ";
					RunServer = FALSE;
					NeedPack = FALSE;
					OperatorConsole.printf("This output is generated by the dicom server application\n");
					OperatorConsole.printf("If you can read this, the console communication is OK\n");
                          		SystemDebug.printf("This is systemdebug output; can you read this ?\n");
                          		SystemDebug.printf("%s%s%s%s%s%s\n", lt, lt, lt, lt, lt, lt);
					OperatorConsole.printf(" ---------- Succesful end of test -----------\n");
					return ( FALSE );
					}

				case	'o':	// test ODBC database connnection
				case	'O':
					{
					Database aDB;
					int	i;
					RunServer = FALSE;
					NeedPack = FALSE;
					for (i=1; i<=10; i++)
						{
						OperatorConsole.printf("Attempting to open database; test #%d of 10\n", i);
						if(!aDB.Open(DataSource, UserName, Password, DataHost))
							{
							OperatorConsole.printf("***Unable to open database %s as user %s on %s\n",
							DataSource, UserName, DataHost);
							exit (1);
							}
	
						OperatorConsole.printf("Creating test table\n");
						aDB.CreateTable ( "xtesttable", "Name varchar(254), AccessTime  int" );

						OperatorConsole.printf("Adding a record\n");
						aDB.AddRecord("xtesttable", "Name", "'Testvalue'");

						OperatorConsole.printf("Dropping test table\n");
						aDB.DeleteTable ( "xtesttable" );

						OperatorConsole.printf("Closing database\n");
						aDB.Close();
						}

					OperatorConsole.printf(" ---------- Succesful end of test -----------\n");
					return ( FALSE );
					}

				case	'a':	// Archival code
				case	'A':	// Archival code
					{
					RunServer = FALSE;
					NeedPack = 5;
					OperatorConsole.printf(" ---------- Start archival operation  -----------\n");

					if(!LoadKFactorFile((char*)KFACTORFILE))
						{
						OperatorConsole.printf("***Could not load kfactor file: %s\n", KFACTORFILE);
						exit(1);
						}

					// Select a number of KB to archive from device MAGn (default MAG0)
					if (argv[valid_argc][2] == 's' || argv[valid_argc][2] == 'S')
						{
						char MAG[256];
						char *p;
						p = strchr(argv[valid_argc]+3,',');
						if (p) 
							sprintf(MAG, "MAG%d", atoi(p+1));
						else 
							strcpy(MAG, "MAG?");
						if (!SelectLRUForArchival(MAG, atoi(argv[valid_argc]+3), PDU))
							exit(1);
						}

					// Create cache set for burning for JUKEBOXn.n from MAGn (default MAG0)
					else if (argv[valid_argc][2] == 'b' || argv[valid_argc][2] == 'B')
						{
						char MAG[256], Device[256];
						char *p;
						strcpy(Device, argv[valid_argc]+3);
						p = strchr(Device,',');
						if (p) 
							{
							sprintf(MAG, "MAG%d", atoi(p+1));
							*p = 0;
							}
						else 
							strcpy(MAG, "MAG?");
						if (!PrepareBunchForBurning(MAG, Device))
							exit(1);
						}

					// Move (all) data from device1 to device2
					else if (argv[valid_argc][2] == 'm' || argv[valid_argc][2] == 'M')
						{
						char Device1[256];
						char *p;
						strcpy(Device1, argv[valid_argc]+3);
						p = strchr(Device1,',');
						if (p) 
							{
							*p = 0;
							if (!MoveDataToDevice(Device1, p+1)) exit(1);
							}
						else
							exit(1);
						}

					// Undo any archiving that was not completed
					else if (argv[valid_argc][2] == 'u' || argv[valid_argc][2] == 'U')
						{
						if (!RestoreMAGFlags())
							exit(1);
						}

					// Verify CD in jukebox against its cache set
					else if (argv[valid_argc][2] == 'c' || argv[valid_argc][2] == 'C')
						{
						if (!CompareBunchAfterBurning(argv[valid_argc]+3))
							exit(1);
						}

					// Verify MAG device against its mirror device
					else if (argv[valid_argc][2] == 'v' || argv[valid_argc][2] == 'V')
						{
						if (!VerifyMirrorDisk(argv[valid_argc]+3))
							exit(1);
						}

					// Test read slices on device
					else if (argv[valid_argc][2] == 't' || argv[valid_argc][2] == 'T')
						{
						if (!TestImages(argv[valid_argc]+3))
							exit(1);
						}

					// Delete cache set for burned CD
					else if (argv[valid_argc][2] == 'd' || argv[valid_argc][2] == 'D')
						{
						if (!DeleteBunchAfterBurning(argv[valid_argc]+3))
							exit(1);
						}

					// Regen a single device (for database maintenance)
					else if (argv[valid_argc][2] == 'r' || argv[valid_argc][2] == 'R')
						{
						if(!LoadKFactorFile((char*)KFACTORFILE))
							{
							OperatorConsole.printf("***Could not load kfactor file: %s\n", KFACTORFILE);
							exit(1);
							}
						char *p = strchr(argv[valid_argc]+3,',');
						if (p)
							{
							*p=0;
							OperatorConsole.printf("Regen folders on single device: %s\n", argv[valid_argc]+3);
							if (!Regen(argv[valid_argc]+3, FALSE, p+1)) exit(1);
							OperatorConsole.printf("Regeneration folders Complete\n");
							}
						else
							{
							OperatorConsole.printf("Regen single device: %s\n", argv[valid_argc]+3);
							if (!Regen(argv[valid_argc]+3, FALSE)) exit(1);
							OperatorConsole.printf("Regeneration Complete\n");
							}
						}
						
					// rEname device name
					else if (argv[valid_argc][2] == 'e' || argv[valid_argc][2] == 'E')
						{
						char DeviceFrom[256];
						char *p;
						strcpy(DeviceFrom, argv[valid_argc]+3);
						p = strchr(DeviceFrom,',');
						if (p)
							{ 
							*p = 0;
							if (!RenameDevice(DeviceFrom, p+1)) exit(1);
							}
						else
							{
							OperatorConsole.printf("option requires two device names\n");
							exit(1);
							}
						}

					else
						{
						OperatorConsole.printf("Unknown archiving option\n");
						exit(1);
						}

					OperatorConsole.printf(" ---------- End of archival operation -----------\n");
					return ( FALSE );
					}


#ifdef WIN32
				case	's':
				case	'S':	// Create ODBC data source
					{
					char Options[1512], Driver[512];
					int i, len;
#if defined(__BORLANDC__) || defined(__WATCOMC__)
					HINSTANCE hODBCInst = LoadLibrary("odbccp32.dll");
					__SQLConfigDataSource= (_SQLConfigDataSource)GetProcAddress(hODBCInst, "SQLConfigDataSource");
#endif
					strcpy(Driver, "Microsoft dBase Driver (*.dbf)");

					strcpy(Options, 
						"DSN=ConQuestPacs;"
                   				"Description=ConQuest DICOM server data source;"
						"Fil=dBase III;"
						"DriverID=21;"
						"Deleted=1;"
                   				"DefaultDir=C:\\quirt");

					for (i=2; i<strlen(argv[valid_argc]); i++)
						{
						if (argv[valid_argc][i] == ';')
							{
							memcpy(Driver, argv[valid_argc]+2, i-2);
							Driver[i-2]=0;
							strcpy(Options, argv[valid_argc]+i+1);
							Options[strlen(Options)+1] = 0;
							break;
							}
						}

					OperatorConsole.printf("Creating data source\n");
					OperatorConsole.printf("Driver = %s\n", Driver);
					OperatorConsole.printf("Options = %s\n", Options);

					len = strlen(Options);
					for (i=0; i<len; i++)
						if (Options[i] == ';') Options[i] = 0;

					RunServer = FALSE;
					NeedPack = FALSE;
					if ( !SQLConfigDataSource(NULL, ODBC_ADD_SYS_DSN, Driver, Options) )
					// if ( !SQLConfigDataSource(NULL, ODBC_ADD_DSN, Driver, Options) )
						{ char		szMsg[256] = "(No details)";
						  char		szTmp[280];
						  DWORD		dwErrorCode;
						  OperatorConsole.printf("***Datasource configuration FAILED\n");
#if !defined(__BORLANDC__) && !defined(__WATCOMC__) && 0
						  SQLInstallerError(1, &dwErrorCode, szMsg, sizeof(szMsg) - 1, NULL);
						  sprintf(szTmp, "***%s (ec=%d)\n", szMsg, dwErrorCode);
						  OperatorConsole.printf(szTmp);
#endif
						  exit(1);
						}
					else
						OperatorConsole.printf("Datasource configuration succesful\n");
					OperatorConsole.printf(" ----------------------------------\n");
					return ( FALSE );
					}
#endif


				case	'e':	// create database (-eSAPW DB USER PW)
				case	'E':
					{
					Database DB;
					DebugLevel=4;
					if (DB.db_type==DT_POSTGRES)
						{
						if (DB.Open ( "postgres", "postgres", argv[valid_argc]+2, DataHost))
							if (DB.CreateDatabase(argv[valid_argc+1], argv[valid_argc+2], argv[valid_argc+3]))
								OperatorConsole.printf("succesfully created database and login\n");
						}
					else if (strcmp(argv[valid_argc+2], "root")==0 || strcmp(argv[valid_argc+2], "sa")==0)
						{
						if (DB.Open ( DataSource, argv[valid_argc+2], argv[valid_argc]+2, DataHost))
							if (DB.CreateDatabase(argv[valid_argc+1], argv[valid_argc+2], argv[valid_argc]+3))
								OperatorConsole.printf("succesfully created database\n");
						}
					else
						{
						if (DB.Open ( DataSource, "sa", argv[valid_argc]+2, DataHost))
							if (DB.CreateDatabase(argv[valid_argc+1], argv[valid_argc+2], argv[valid_argc+3]))
								OperatorConsole.printf("succesfully created database and login\n");
						}
					OperatorConsole.printf(" ----------------------------------\n");
					exit(0);
					}

				case	'i':	// initialize database
				case	'I':
					RunServer = FALSE;
					NeedPack = FALSE;

					OperatorConsole.printf("Initializing Tables\n");

					if(!LoadKFactorFile((char*)KFACTORFILE))
						{
						OperatorConsole.printf("***Could not load kfactor file: %s\n", KFACTORFILE);
						exit(1);
						}

					mode1 = atoi(argv[valid_argc]+2);
					InitializeTables (mode1);

					return ( FALSE );

				case	'z':	// delete (zap) patient
				case	'Z':
					RunServer = FALSE;
					NeedPack = 5;

					if(!LoadKFactorFile((char*)KFACTORFILE))
						{
						OperatorConsole.printf("***Could not load kfactor file: %s\n", KFACTORFILE);
						exit (1);
						}

					if (!DeletePatient(argv[valid_argc]+2, FALSE, 1))
						exit(1);

					return ( FALSE );

				case	'g':	// grab data from server
				case	'G':
					{
					char *p;

					RunServer = FALSE;
					NeedPack = 5;

					if(!InitACRNemaAddressArray())
						{
						OperatorConsole.printf("***Error loading acr-nema map file:%s\n",ACRNEMAMAP);
						exit(1);
						}

					if(!LoadKFactorFile((char*)KFACTORFILE))
						{
						OperatorConsole.printf("***Could not load kfactor file: %s\n", KFACTORFILE);
						exit (1);
						}

					p = strchr(argv[valid_argc]+2,',');
					if (p)
						{
						*p=0;
						if (!GrabImagesFromServer((unsigned char *)argv[valid_argc]+2, p+1, (char *)MYACRNEMA, 1))
							exit(1);
						}
					else
						{
						if (!GrabImagesFromServer((unsigned char *)argv[valid_argc]+2, "*", (char *)MYACRNEMA, 1))
							exit(1);
						}

					return ( FALSE );
					}

				case	'f':	// Database fix options
				case	'F':
					RunServer = FALSE;
					NeedPack = 5;

					if(!LoadKFactorFile((char*)KFACTORFILE))
						{
						OperatorConsole.printf("***Could not load kfactor file: %s\n", KFACTORFILE);
						exit (1);
						}

					
					if (argv[valid_argc][2] == 'p' || argv[valid_argc][2] == 'P')
						{
						if (!DeletePatient(argv[valid_argc]+3, TRUE, 1))
							exit(1);
						}

					if (argv[valid_argc][2] == 'f' || argv[valid_argc][2] == 'F')
						{
						if (LargestFreeMAG()>(unsigned int)atoi(argv[valid_argc]+3)) exit(0);

						if (!PanicKillOff((unsigned int)atoi(argv[valid_argc]+3)))
							exit(1);
						}

					else if (argv[valid_argc][2] == 't' || argv[valid_argc][2] == 'T')
						{
						if (!DeleteStudy(argv[valid_argc]+3, TRUE, 1))
							exit(1);
						}

					else if (argv[valid_argc][2] == 's' || argv[valid_argc][2] == 'S')
						{
						if (!DeleteSeries(argv[valid_argc]+3, TRUE, 1))
							exit(1);
						}

					else if (argv[valid_argc][2] == 'i' || argv[valid_argc][2] == 'I')
						{
						if (!DeleteImage(argv[valid_argc]+3, TRUE, 1))
							exit(1);
						}

					else if (argv[valid_argc][2] == 'r' || argv[valid_argc][2] == 'R')
						{
						char par[256];
						char *p;
						strcpy(par, argv[valid_argc]+3);
						p = strchr(par,',');
						if (p)
							{
							*p=0;
							if (!Regen(par, FALSE, p+1))
								exit(1);
							}
						else
							if (!Regen("MAG0", FALSE, par))
								exit(1);
						}

					else if (argv[valid_argc][2] == 'e' || argv[valid_argc][2] == 'E')
						{
						if (!RegenFile(argv[valid_argc]+3))
							exit(1);
						}

					else if (argv[valid_argc][2] == 'd' || argv[valid_argc][2] == 'D')
						{
						if (!DeleteImageFile(argv[valid_argc]+3, TRUE))
							exit(1);
						}

					else if (argv[valid_argc][2] == 'z' || argv[valid_argc][2] == 'Z')
						{
						if (!DeleteImageFile(argv[valid_argc]+3, FALSE))
							exit(1);
						}

					else if (argv[valid_argc][2] == 'a' || argv[valid_argc][2] == 'A')
						{
						char par[1024];
						char *p;

						strcpy(par, argv[valid_argc]+3);
						p = strchr(par,',');
						if (bUIDPostfixSet)
							gl_iFileCounter = UIDPostfix;
						else
							gl_iFileCounter = ComputeCRC(par, strlen(par));
						if (p)
							{
							*p=0;
							if (!AddImageFile(par, p+1, PDU))
								exit(1);
							}
						else
							{
							if (!AddImageFile(par, NULL, PDU))
								exit(1);
							}
						}

					else if (argv[valid_argc][2] == '?')
						{
						char UID[256];
						if (!GetImageFileUID(argv[valid_argc]+3, UID))
							exit(1);
						printf("%s\n", UID);
						}

					else if (argv[valid_argc][2] == 'u' || argv[valid_argc][2] == 'U')
						{
						char UID[256];
						if (!GenUID(UID))
							exit(1);
						printf("%s\n", UID);
						}

					else if (argv[valid_argc][2] == 'c' || argv[valid_argc][2] == 'C')
						{
						char par[256];
						char *p;
						strcpy(par, argv[valid_argc]+3);
						p = strchr(par,',');
						if (p)
							{
							*p=0;
							if (!ModifyPATIDofImageFile(p+1, par, TRUE, NULL, PDU))
								exit(1);
							}
						}
					else if (argv[valid_argc][2] == 'k' || argv[valid_argc][2] == 'K')
						{
						char par[256];
						char *p;
						strcpy(par, argv[valid_argc]+3);
						p = strchr(par,',');
						if (p)
							{
							*p=0;
							if (!ModifyPATIDofImageFile(p+1, par, FALSE, NULL, PDU))
								exit(1);
							}
						}
					return ( FALSE );



				case	'n':	// NKI compression options
				case	'N':
					RunServer = FALSE;
					NeedPack = FALSE;

					if (argv[valid_argc][2] == 'd' || argv[valid_argc][2] == 'D')
						{
						//int dum;
						//if (!DecompressImageFile(argv[valid_argc]+3, &dum))
						char dum[3]="un";
						if (!recompressFile(argv[valid_argc]+3, dum, PDU))
							exit(1);
						}

					if (argv[valid_argc][2] == 'u' || argv[valid_argc][2] == 'U')
						{
						DecompressNKI(argv[valid_argc+1], argv[valid_argc+2]);
						}

					if (argv[valid_argc][2] == 'c' || argv[valid_argc][2] == 'C')
						{
						//int dum;
						//if (!CompressNKIImageFile(argv[valid_argc]+4, argv[valid_argc][3]-'0', &dum))
						if (!recompressFile(argv[valid_argc]+4, argv[valid_argc]+2, PDU))
							exit(1);
						}

					return ( FALSE );

				case	'j':	// JPEG compression options
				case	'J':
					RunServer = FALSE;
					NeedPack = FALSE;

					if (argv[valid_argc][2] == 'd' || argv[valid_argc][2] == 'D')
						{
						//int dum;
						//if (!DecompressImageFile(argv[valid_argc]+3, &dum))
						char dum[3]="un";
						if (!recompressFile(argv[valid_argc]+3, dum, PDU))
							exit(1);
						}

					if (argv[valid_argc][2] == 'c' || argv[valid_argc][2] == 'C')
						{
						//int dum;
						//if (!CompressJPEGImageFile(argv[valid_argc]+4, argv[valid_argc][3], &dum))
						if (!recompressFile(argv[valid_argc]+4, argv[valid_argc]+2, PDU))
							exit(1);
						}

					if (argv[valid_argc][2] == '*')		// test recompressFile
						{
						char mode[4];
						mode[0] = argv[valid_argc][3];
						mode[1] = argv[valid_argc][4];
						mode[2] = 0;
						if (!recompressFile(argv[valid_argc]+5, mode, PDU))
							exit(1);
						}

					if (argv[valid_argc][2] == '-')		// test recompress
						{
						PDU_Service	PDU2;
//						VR *pVR;

						PDU2.AttachRTC(&VRType);

						DICOMDataObject *pDDO;
						pDDO = PDU2.LoadDICOMDataObject(argv[valid_argc]+5);
						char mode[4];
						mode[0] = argv[valid_argc][3];
						mode[1] = argv[valid_argc][4];
						mode[2] = 0;
						if (!recompress(&pDDO, mode, "", mode[0]=='n' || mode[0]=='N', PDU))
							exit(1);

						PDU2.SaveDICOMDataObject(argv[valid_argc]+5, ACRNEMA_VR_DUMP, pDDO);
						delete pDDO;
						exit(1);
						}

					if (argv[valid_argc][2] == 's')		// -js to json (summary)
						{
					        OperatorConsole.On();
						struct scriptdata sd = {&globalPDU, NULL, NULL, -1, NULL, NULL, NULL, NULL, NULL, 0, 0};
						char script[1000]="local d=DicomObject:new();d:Read([[";
						strcat(script, argv[valid_argc]+3);
						strcat(script, "]]);io.write(d:Serialize(true,false))");
						do_lua(&(globalPDU.L), script, &sd);
						exit(1);
						}

					if (argv[valid_argc][2] == 'j')		// -jj to json (complete)
						{
					        OperatorConsole.On();
						struct scriptdata sd = {&globalPDU, NULL, NULL, -1, NULL, NULL, NULL, NULL, NULL, 0, 0};
						char script[1000]="local d=DicomObject:new();d:Read([[";
						strcat(script, argv[valid_argc]+3);
						strcat(script, "]]);io.write(d:Serialize(true,true))");
						do_lua(&(globalPDU.L), script, &sd);
						exit(1);
						}

					return ( FALSE );


				case	'-':	// Server command
					if (memcmp(argv[valid_argc]+2, "addlocalfile:", 13)==0)
						{
						char *p1 = strchr(argv[valid_argc]+2, ',');
						if (p1) *p1=0;
						char com[512];
						if (p1) sprintf(com, "addimagefile:%s,%s", argv[valid_argc]+15, p1+1);
						else    sprintf(com, "addimagefile:%s", argv[valid_argc]+15);
						printf("%s", com);
						printf("%s", argv[valid_argc]+15);
						int rc = SendServerCommand("Server command sent using DGATE -- option", com, 0, argv[valid_argc]+15, FALSE, TRUE);
						exit(rc);
						}
					if (memcmp(argv[valid_argc]+2, "dolua:", 6)==0)
						{
						struct	stat st;
						if (!Logging)
					          OperatorConsole.On();
						//SystemDebug.On();
						LoadKFactorFile((char*)KFACTORFILE);
					  	InitACRNemaAddressArray();
						
						struct scriptdata sd = {&globalPDU, NULL, NULL, -1, NULL, NULL, NULL, NULL, NULL, 0, 0};
						globalPDU.SetLocalAddress ( (BYTE *)"global" );
						globalPDU.SetRemoteAddress ( (BYTE *)"dolua" );
						lua_setvar(&globalPDU, "version", DGATE_VERSION); // creates globalPDU.L if needed
	  					lua_createtable   (globalPDU.L, 0, 0);
						for (int i=0; i<argc; i++)
							{
							lua_pushinteger(globalPDU.L, i+1);
							lua_pushstring (globalPDU.L, argv[i]); 
							lua_settable (globalPDU.L, -3);
							}
				                lua_setglobal (globalPDU.L, "arg");

						char file[1024], cmd[1024]; 
						strcpy(file, argv[valid_argc]+8);
						if(stat(file, &st) != 0)
							{
							memmove(file+4, file, strlen(file)+1);
							strcpy(file, "lua");
							file[3] = PATHSEPCHAR;
							}
						if(stat(file, &st) == 0)
							sprintf(cmd, "dofile([[%s]])", file);
						else
							strcpy(cmd, argv[valid_argc]+8);

						do_lua(&(globalPDU.L), cmd, &sd);
						if (sd.DDO) delete sd.DDO;
						exit(0);
						}

					{
					int rc = SendServerCommand("Server command sent using DGATE -- option", argv[valid_argc]+2, 0, NULL, FALSE);
					exit(rc);
					}

				default:   	// provide some help
					RunServer = FALSE;
					NeedPack = FALSE;
					PrintOptions();
					return (FALSE);
				}
			}
		else
			{
//			FILE	*logFile;

// This is a simple way to get a socket#; but why make it simple if it can be done complex ?
//			Socketfd = (void*)atoi(argv[valid_argc]);


// Open some channel (UDP/pipe) for statistics when running as server child (from .ini file)
			if(OCPipeName[0] == PATHSEPCHAR)
				{
				OperatorConsole.OnMsgPipe(OCPipeName);
				}
			else
				{
				OperatorConsole.OnUDP(OCPipeName, "1111");
				}

// WIN32: get the socket# through shared memory
#ifndef UNIX
			HANDLE		hMap;
			IPCBlock	*IPCBlockPtrInstance;
			hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS,
				FALSE,
				argv[valid_argc]);
			if(!hMap)
				{
				OperatorConsole.printf("***Child: Unable to OpenFileMapping : %d (%s)\n",
					GetLastError(), argv[valid_argc]);
				exit(1);
				}

			IPCBlockPtrInstance = (IPCBlock*)MapViewOfFile(hMap,
				FILE_MAP_ALL_ACCESS, 0,0, sizeof(IPCBlock));
			if(!IPCBlockPtrInstance)
				{
				OperatorConsole.printf("***Child: Unable to MapViewOfFile : %d\n",
					GetLastError());
				}
			Socketfd = IPCBlockPtrInstance->Socketfd;
			UnmapViewOfFile(IPCBlockPtrInstance);
			CloseHandle(hMap);

// UNIX: get the socket# from a small file or pipe
#else
#if 0
			IPCBlock aIPCBlock;
			ifstream aStream(argv[valid_argc], ios::in | ios::nocreate);
			if(aStream.good())
				{
				aStream.read((unsigned char *)&aIPCBlock, sizeof(aIPCBlock));
				}
			if(!aStream.good() || aStream.gcount() != sizeof(aIPCBlock) ||
				aIPCBlock.Magic != IPCBlockMagic)
				{
				// magic # assures safety --
				// we don't try unlink("/"), for example
				OperatorConsole.printf("***Child: Unable to read file %s\n",
					argv[valid_argc]);
				}
			else
				{
					unlink(argv[valid_argc]);
				}
			Socketfd = aIPCBlock.Socketfd;
#endif
#endif
			}

		++valid_argc;
		}

        /* check access for logging and saving images */

	if (1)
		{
	        FILE *f;
	        char Filename[1024];
		int i;

	        f = fopen(TroubleLogFile, "at");
        	if (f) fclose(f);
	        if (!f) OperatorConsole.printf("*** Not enough rights to write to logfiles\n");

		memset(Filename, 0, 1024);
		GetPhysicalDevice("MAG0", Filename);
		strcat(Filename, "printer_files");
		mkdir(Filename);
		i = strlen(Filename);
		Filename[i]   = PATHSEPCHAR;
		Filename[i+1] = 0;
		strcat(Filename, "accesstest.log");
	        f = fopen(Filename, "at");
        	if (f) fclose(f);
	        if (!f) OperatorConsole.printf("*** Not enough rights to write in MAG0\n");

		memset(Filename, 0, 1024);
		GetPhysicalDevice("MAG0", Filename);
		strcat(Filename, "incoming");
		if (dgate_IsDirectory(Filename))
			{
			strcat(Filename, "\\");
			Filename[strlen(Filename)-1] = PATHSEPCHAR;
			OperatorConsole.printf("Monitoring for files in: %s\n", Filename);
			strcpy(monitorfolder, Filename);
			StartMonitorThread(monitorfolder);
			}
		}

// prepare to run as a child server thread
	if(!LoadKFactorFile((char*)KFACTORFILE))
		{
		OperatorConsole.printf("***Error loading kfactor file:%s\n", KFACTORFILE);
		return ( FALSE );
		}

	if(!InitACRNemaAddressArray())
		{
		OperatorConsole.printf("***Error loading acr-nema map file:%s\n",ACRNEMAMAP);
		return ( FALSE );
		}

	if(!atoi((char*)Port))
		return ( Socketfd );
	if(Valid > 1)				// always TRUE !!!!!
		return ( Socketfd );

	return ( Socketfd );
	}

/*
BOOL
SetString(VR	*vr, char	*s, int	Max)
	{
	memset((void*)s, 0, Max);
	if(vr)
		if(vr->Data)
			{
			memcpy((void*)s, vr->Data, vr->Length%Max);
			return ( TRUE );
			}
	return ( FALSE );
	}
*/

class	DriverApp
	{
	public:
		virtual	BOOL	ServerChild ( int, unsigned int ) = 0;
	public:
		BOOL		ServerChildThread ( int, unsigned int);
		BOOL		Server ( BYTE * );
		int		ChildSocketfd;
		volatile int	Lock;
		unsigned int    ConnectedIP;
#ifndef __GNUC__
		DriverApp () { Lock = 0; };
#else //The GCC way
		DriverApp ():ChildSocketfd(0), Lock(0) {};
		virtual ~DriverApp() {};
#endif
	};

// Query / Retrieve Engine

class	MyPatientRootQuery	:
	public	PatientRootQuery
	{
	public:
		BOOL	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	QueryResultScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	SearchOn (DICOMDataObject *, Array < DICOMDataObject *> *);
		BOOL	CallBack (DICOMCommandObject *, DICOMDataObject *);
		int	RecordsFound;
#ifdef __GNUC__
		MyPatientRootQuery():RecordsFound(0) {};
#endif
	};

class	MyStudyRootQuery	:
	public	StudyRootQuery
	{
	public:
		BOOL	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	QueryResultScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	SearchOn (DICOMDataObject *, Array < DICOMDataObject *> *);
		BOOL	CallBack (DICOMCommandObject *, DICOMDataObject *);
		int	RecordsFound;
#ifdef __GNUC__
		MyStudyRootQuery():RecordsFound(0) {};
#endif
	};

class	MyPatientStudyOnlyQuery	:
	public	PatientStudyOnlyQuery
	{
	public:
		BOOL	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	QueryResultScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	SearchOn (DICOMDataObject *, Array < DICOMDataObject *> *);
		BOOL	CallBack (DICOMCommandObject *, DICOMDataObject *);
		int	RecordsFound;
#ifdef __GNUC__
		MyPatientStudyOnlyQuery():RecordsFound(0) {};
#endif
	};

	
class	MyModalityWorkListQuery	:
	public	ModalityWorkListQuery
	{
	public:
		BOOL	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	QueryResultScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	SearchOn (DICOMDataObject *, Array < DICOMDataObject *> *);
		BOOL	CallBack (DICOMCommandObject *, DICOMDataObject *);
		int	RecordsFound;
#ifdef __GNUC__
		MyModalityWorkListQuery():RecordsFound(0) {};
#endif
	};


class	MyPatientRootRetrieve	:
	public	PatientRootRetrieve
	{
		RunTimeClassStorage	RTCStorage;

	public:
		BOOL	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	SearchOn (DICOMDataObject *, Array < DICOMDataObject *> *);
		BOOL	CallBack (DICOMCommandObject *, DICOMDataObject *);
		BOOL	RetrieveOn ( DICOMDataObject *, DICOMDataObject **,
							StandardStorage **);
		BOOL	QualifyOn ( BYTE *, BYTE *, BYTE *, BYTE * );
#ifdef __GNUC__
		MyPatientRootRetrieve():RTCStorage() {};
#endif
	};
	
class	MyPatientRootRetrieveNKI	:
	public	PatientRootRetrieveNKI
	{
		RunTimeClassStorage	RTCStorage;

	public:
		BOOL	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	QueryResultScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	SearchOn (DICOMDataObject *, Array < DICOMDataObject *> *);
		BOOL	CallBack (DICOMCommandObject *, DICOMDataObject *);
		BOOL	RetrieveOn ( DICOMDataObject *, DICOMDataObject **,
							StandardStorage **,
							DICOMCommandObject	   *,
						        Array < DICOMDataObject *> *,
							void 		*);
		BOOL	QualifyOn ( BYTE *, BYTE *, BYTE *, BYTE *, BYTE * );
#ifdef __GNUC__
		MyPatientRootRetrieveNKI():RTCStorage() {};
#endif
	};

class	MyPatientRootRetrieveGeneric	:
	public	PatientRootRetrieveGeneric
	{
		RunTimeClassStorage	RTCStorage;

	public:
		BOOL	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	SearchOn (DICOMDataObject *, Array < DICOMDataObject *> *);
		BOOL	CallBack (DICOMCommandObject *, DICOMDataObject *);
		BOOL	RetrieveOn ( DICOMDataObject *, DICOMDataObject **,
							StandardStorage **,
							DICOMCommandObject	   *,
						        Array < DICOMDataObject *> *,
							void 		*);
		BOOL	QualifyOn ( BYTE *, BYTE *, BYTE *, BYTE *, BYTE * );
#ifdef __GNUC__
		MyPatientRootRetrieveGeneric():RTCStorage() {};
#endif
	};

class	MyPatientRootGetGeneric	:
	public	PatientRootGetGeneric
	{
		RunTimeClassStorage	RTCStorage;

	public:
		BOOL	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	SearchOn (DICOMDataObject *, Array < DICOMDataObject *> *);
		BOOL	CallBack (DICOMCommandObject *, DICOMDataObject *);
		BOOL	RetrieveOn ( DICOMDataObject *, DICOMDataObject **,
							StandardStorage **,
							DICOMCommandObject	   *,
						        Array < DICOMDataObject *> *,
							void 		*);
		BOOL	QualifyOn ( BYTE *, BYTE *, BYTE *, BYTE *, BYTE * );
#ifdef __GNUC__
		MyPatientRootGetGeneric():RTCStorage() {};
#endif
	};

class	MyStudyRootRetrieve	:
	public	StudyRootRetrieve
	{
		RunTimeClassStorage	RTCStorage;

	public:
		BOOL	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	SearchOn (DICOMDataObject *, Array < DICOMDataObject *> *);
		BOOL	CallBack (DICOMCommandObject *, DICOMDataObject *);
		BOOL	RetrieveOn ( DICOMDataObject *, DICOMDataObject **,
							StandardStorage **);
		BOOL	QualifyOn ( BYTE *, BYTE *, BYTE *, BYTE * );
#ifdef __GNUC__
		MyStudyRootRetrieve():RTCStorage() {};
#endif
	};

class	MyStudyRootRetrieveNKI	:
	public	StudyRootRetrieveNKI
	{
		RunTimeClassStorage	RTCStorage;

	public:
		BOOL	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	SearchOn (DICOMDataObject *, Array < DICOMDataObject *> *);
		BOOL	CallBack (DICOMCommandObject *, DICOMDataObject *);
		BOOL	RetrieveOn ( DICOMDataObject *, DICOMDataObject **,
							StandardStorage **,
							DICOMCommandObject	   *,
						        Array < DICOMDataObject *> *,
							void 		*);
		BOOL	QualifyOn ( BYTE *, BYTE *, BYTE *, BYTE *, BYTE * );
#ifdef __GNUC__
		MyStudyRootRetrieveNKI():RTCStorage() {};
#endif
	};

class	MyStudyRootRetrieveGeneric	:
	public	StudyRootRetrieveGeneric
	{
		RunTimeClassStorage	RTCStorage;

	public:
		BOOL	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	SearchOn (DICOMDataObject *, Array < DICOMDataObject *> *);
		BOOL	CallBack (DICOMCommandObject *, DICOMDataObject *);
		BOOL	RetrieveOn ( DICOMDataObject *, DICOMDataObject **,
							StandardStorage **,
							DICOMCommandObject	   *,
						        Array < DICOMDataObject *> *,
							void 		*);
		BOOL	QualifyOn ( BYTE *, BYTE *, BYTE *, BYTE *, BYTE * );
#ifdef __GNUC__
		MyStudyRootRetrieveGeneric():RTCStorage() {};
#endif
	};

class	MyStudyRootGetGeneric	:
	public	StudyRootGetGeneric
	{
		RunTimeClassStorage	RTCStorage;

	public:
		BOOL	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	SearchOn (DICOMDataObject *, Array < DICOMDataObject *> *);
		BOOL	CallBack (DICOMCommandObject *, DICOMDataObject *);
		BOOL	RetrieveOn ( DICOMDataObject *, DICOMDataObject **,
							StandardStorage **,
							DICOMCommandObject	   *,
						        Array < DICOMDataObject *> *,
							void 		*);
		BOOL	QualifyOn ( BYTE *, BYTE *, BYTE *, BYTE *, BYTE * );
#ifdef __GNUC__
		MyStudyRootGetGeneric():RTCStorage() {};
#endif
	};

class	MyPatientStudyOnlyRetrieve	:
	public	PatientStudyOnlyRetrieve
	{
		RunTimeClassStorage	RTCStorage;

	public:
		BOOL	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	SearchOn (DICOMDataObject *, Array < DICOMDataObject *> *);
		BOOL	CallBack (DICOMCommandObject *, DICOMDataObject *);
		BOOL	RetrieveOn ( DICOMDataObject *, DICOMDataObject **,
							StandardStorage **);
		BOOL	QualifyOn ( BYTE *, BYTE *, BYTE *, BYTE * );
#ifdef __GNUC__
		MyPatientStudyOnlyRetrieve():RTCStorage() {};
#endif
	};
	
class	MyPatientStudyOnlyRetrieveNKI	:
	public	PatientStudyOnlyRetrieveNKI
	{
		RunTimeClassStorage	RTCStorage;

	public:
		BOOL	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	SearchOn (DICOMDataObject *, Array < DICOMDataObject *> *);
		BOOL	CallBack (DICOMCommandObject *, DICOMDataObject *);
		BOOL	RetrieveOn ( DICOMDataObject *, DICOMDataObject **,
							StandardStorage **,
							DICOMCommandObject	   *,
						        Array < DICOMDataObject *> *,
							void 		*);
		BOOL	QualifyOn ( BYTE *, BYTE *, BYTE *, BYTE *, BYTE * );
#ifdef __GNUC__
		MyPatientStudyOnlyRetrieveNKI():RTCStorage() {};
#endif
	};

class	MyPatientStudyOnlyRetrieveGeneric	:
	public	PatientStudyOnlyRetrieveGeneric
	{
		RunTimeClassStorage	RTCStorage;

	public:
		BOOL	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	SearchOn (DICOMDataObject *, Array < DICOMDataObject *> *);
		BOOL	CallBack (DICOMCommandObject *, DICOMDataObject *);
		BOOL	RetrieveOn ( DICOMDataObject *, DICOMDataObject **,
							StandardStorage **,
							DICOMCommandObject	   *,
						        Array < DICOMDataObject *> *,
							void 		*);
		BOOL	QualifyOn ( BYTE *, BYTE *, BYTE *, BYTE *, BYTE * );
#ifdef __GNUC__
		MyPatientStudyOnlyRetrieveGeneric():RTCStorage() {};
#endif
	};

class	MyPatientStudyOnlyGetGeneric	:
	public	PatientStudyOnlyGetGeneric
	{
		RunTimeClassStorage	RTCStorage;

	public:
		BOOL	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO);
		BOOL	SearchOn (DICOMDataObject *, Array < DICOMDataObject *> *);
		BOOL	CallBack (DICOMCommandObject *, DICOMDataObject *);
		BOOL	RetrieveOn ( DICOMDataObject *, DICOMDataObject **,
							StandardStorage **,
							DICOMCommandObject	   *,
						        Array < DICOMDataObject *> *,
							void 		*);
		BOOL	QualifyOn ( BYTE *, BYTE *, BYTE *, BYTE *, BYTE * );
#ifdef __GNUC__
		MyPatientStudyOnlyGetGeneric():RTCStorage() {};
#endif
	};

// This class handles storage; used to allow error messages to be sent back to the caller

class	MyUnknownStorage	:	public UnknownStorage
	{
	public:
		Database *DB;		// must be set by caller (needs one per thread)
		BOOL	 nopreget;	// must be set by caller (derived from messageid)

				// called for each incoming DDO
		UINT16 CheckObject(DICOMCommandObject *DCO, DICOMDataObject *DDO, PDU_Service *PDU) 
			{
			char Filename[1024];
			char Called[20], Calling[20];
			char *Compression;
			int rc;

			if (!DB) return 0;	// no db set: processing done by caller (failsafe_storage)

#ifdef	DEBUG_MODE
		//OperatorConsole.printf("C-Store (RTC class)\n");
#endif
			ImagesReceived++;

			// If the called AE looks like SERVER~j2, then the last 2 characters override IncomingCompression
			memset(Called, 0, 20);
			strcpy(Called, (char *)(((AAssociateAC *)PDU)->CalledApTitle));
		        if (Called[0]>32)
			  while (Called[strlen(Called)-1]==' ') Called[strlen(Called)-1] = 0;

			memset(Calling, 0, 20);
			strcpy(Calling, (char *)(((AAssociateAC *)PDU)->CallingApTitle));
		        if (Calling[0]>32)
			  while (Calling[strlen(Calling)-1]==' ') Calling[strlen(Calling)-1] = 0;

                        Compression = strchr(Called, '~');
			if (!Compression) Compression = IncomingCompression; else Compression++;
					
			// Recompress (e.g., leave as is or decompress) incoming files
			recompress(&DDO, Compression, "", Compression[0]=='n' || Compression[0]=='N', (ExtendedPDU_Service *)PDU);

			// Virtual compression: save in database, not on disk
			if (Compression[0]=='v' || Compression[0]=='V')
				{
				char		rRoot[1024], rFilename[1024];
				char		Device[256];

				FixImage(DDO);	// Make sure the required DICOM UID's are there + fix patient ID if enabled
		
				*rRoot=1;
				if (!GenerateFileName(DDO, Device, rRoot, rFilename, TRUE, 0, Called, Calling, DB))
					{
					OperatorConsole.printf("**** Virtual storage filename problem - %s\n", rFilename);
					}
				if (memcmp(rFilename, "lua:", 4)==0) lua_getstring((ExtendedPDU_Service *)PDU, NULL, DDO, rFilename+4, rFilename);
				if(!SaveToDataBase(*DB, DDO, rFilename, Device, *rRoot))
					{
					OperatorConsole.printf("**** Virtual storage db problem - %s\n", rFilename);
					}

				OperatorConsole.printf("Virtual file: %s\n", rFilename);
				delete DDO;

				ImagesSaved++;
				return  0;
				}

			// NOTE: NOT THREAD SAFE - IF ONE THREAD HANDLES READS AND WRITES THIS OPERATION CAN FAIL DUE TO DB SHARING:

			rc = SaveToDisk(DB, DCO, DDO, Filename, FALSE, (ExtendedPDU_Service *)PDU, 0, nopreget);
			//delete DDO;
			//rc = TRUE;
			//strcpy(Filename, "");

			if(!rc)
				{
				//delete DDO;
				return StorageFailedErrorCode;	// Processing failed
				}

			else
				{
				if(strlen(Filename))
					{
					if (rc==2) OperatorConsole.printf("Rewritten file: %s\n", Filename);
					else       OperatorConsole.printf("Written file: %s\n", Filename);
					}
				ImagesSaved++;
				return 0;
				}
			}
#ifdef __GNUC__
		MyUnknownStorage():DB(),nopreget(FALSE) {};
	private:// This will prevent it from being copied (it has a pointer)
		MyUnknownStorage(const MyUnknownStorage&);
		const	MyUnknownStorage & operator = (const MyUnknownStorage&);
#endif
	};

class	StorageApp	:
	public	DriverApp
	{
		// MyUnknownStorage		SOPUnknownStorage;
		//Verification				SOPVerification;
		//MyPatientRootQuery			SOPPatientRootQuery;
		//MyPatientRootRetrieve			SOPPatientRootRetrieve;
		//MyPatientRootRetrieveNKI		SOPPatientRootRetrieveNKI;
		//MyPatientRootRetrieveGeneric		SOPPatientRootRetrieveGeneric;

		//MyStudyRootQuery			SOPStudyRootQuery;
		//MyStudyRootRetrieve			SOPStudyRootRetrieve;
		//MyStudyRootRetrieveNKI			SOPStudyRootRetrieveNKI;
		//MyStudyRootRetrieveGeneric		SOPStudyRootRetrieveGeneric;

		//MyPatientStudyOnlyQuery			SOPPatientStudyOnlyQuery;
		//MyPatientStudyOnlyRetrieve		SOPPatientStudyOnlyRetrieve;
		//MyPatientStudyOnlyRetrieveNKI		SOPPatientStudyOnlyRetrieveNKI;
		//MyPatientStudyOnlyRetrieveGeneric	SOPPatientStudyOnlyRetrieveGeneric;

		//MyModalityWorkListQuery			SOPModalityWorkListQuery;
	public:
		BOOL	ServerChild ( int, unsigned int );
		void FailSafeStorage(CheckedPDU_Service *PDU);
		BOOL PrinterSupport( ExtendedPDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *PrintData[], int Thread, char *lua);
		BOOL StorageCommitmentSupport( CheckedPDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject **CommitData);
	};

// from dbsql.cpp
BOOL
UpdateOrAddToTable(
	Database		&DB,
	DBENTRY			*DCMGateDB,
	char			*TableName,
	DICOMDataObject		*DDOPtr,
	const char		*ObjectFile,
	const char		*DeviceName,
	char			*Patid,
	char			*Modality,
	BOOL			*Added,
	BOOL			JustAdd,
	BOOL			CheckDuplicates);

void KodakFixer(DICOMDataObject	*DDOPtr, BOOL tokodak)
{ 	VR *vr = DDOPtr->GetVR(0x0010, 0x0020);
  	char patid[11];

 	if (!vr) return;
	if (!vr->Data) return;
	if (!vr->Length) return;
	if (vr->Length>8) return;

	memset(patid, 0, 10);
	memcpy(patid, (char *)(vr->Data), vr->Length);
	patid[vr->Length]=0;
	if (patid[vr->Length-1]==' ') patid[vr->Length-1] = 0;

	if (!tokodak && strlen(patid)==8 && patid[0] == '0' && atoi(patid)>1000000 )
		{
		vr->ReAlloc(8);
		patid[8] = ' ';
		memcpy(vr->Data, patid + 1, 8);

		SystemDebug.printf("Fixed patient ID from kodak, now %s\n", patid+1);
		}

	if (tokodak && strlen(patid)==7 && patid[0] != '0' && atoi(patid)>1000000 )
		{
		vr->ReAlloc(8);
		((char *)(vr->Data))[0]='0';
		memcpy(((char *)(vr->Data))+1, patid, 7);

		//sprintf((char *)(vr->Data), "0%s", patid);

		SystemDebug.printf("Fixed patient ID to kodak, now 0%s\n", patid);
		}
	}


// compute some sort of CRC as hash key for query
UINT32 crc_ddo(DICOMObject *DO)
{ DICOMObject	DO2;
  VR		*vr;
  unsigned int 	crc=0;
		
  while((vr=DO->Pop()))
  { crc += ComputeCRC((char *)vr->Data, vr->Length);
    crc += 4523 * vr->Group;
    crc += 1023 * vr->Element;
    DO2.Push(vr);
  }
  DO->Reset();
  while((vr=DO2.Pop()))
  { DO->Push(vr);
  }
  return crc;
}

// compare two dicom objects
BOOL compare_ddo(DICOMObject *DO1, DICOMObject *DO2)
{ DICOMObject	DO10, DO20;
  VR		*vr1, *vr2;
  BOOL		result=TRUE;
		
  while((vr1=DO1->Pop()))
  { vr2=DO2->Pop();
    if (vr2)
    { if (vr1->Length != vr2->Length || vr1->Group != vr2->Group || vr1->Element != vr2->Element) result = FALSE;
      else
      { if (vr1->Length)
          if (memcmp(vr1->Data, vr2->Data, vr1->Length)!=0) result = FALSE;
      }
      DO20.Push(vr2);
    }
    DO10.Push(vr1);
  }
  while((vr2=DO2->Pop()))
  { DO20.Push(vr2);
    result = FALSE;
  }

  DO1->Reset();
  while((vr1=DO10.Pop()))
  { DO1->Push(vr1);
  }
  DO2->Reset();
  while((vr1=DO20.Pop()))
  { DO2->Push(vr1);
  }
  return result;
}

// strip meta information group 2 from a dicom object
static void dgateStrip2(DICOMDataObject *pDDO)
{ DICOMObject	DO2;
  VR		*vr;

  while((vr=pDDO->Pop()))
  { if (vr->Group==2) delete vr;
    else DO2.Push(vr);
  }

  pDDO->Reset();
  while((vr=DO2.Pop())) pDDO->Push(vr);
}

// cached virtual query: use in place of 3 calls to VirtualQuery(DDOPtr, Level, i, ADDO)
// use e.g., VirtualServerFor0 = AE,CACHESERIES or AE,CACHESTUDIES
int VirtualQueryCached(DICOMDataObject *DDO, const char *Level, int N, Array < DICOMDataObject  *> *pADDO)
{ BOOL cache = FALSE, cacheseries = TRUE, nonvirtual=FALSE;
  int rc;

  // if ImageType in query change cache name
  char ImageType[128];
  ImageType[0]=0;
  VR *vr = DDO->GetVR(0x0008, 0x0008);
  if (vr && vr->Length)
  { strncpy(ImageType, (char*)vr->Data, vr->Length);
    ImageType[vr->Length] = 0;
    if (ImageType[vr->Length-1]==' ') ImageType[vr->Length-1]=0;
  }

  if (*VirtualServerFor[N]==0) return 0;
  if (strstr(VirtualServerFor[N], ",CACHE"))        cache       = TRUE;
  if (strstr(VirtualServerFor[N], ",CACHESTUDIES")) cacheseries = FALSE;
  if (strstr(VirtualServerFor[N], ",NONVIRTUAL"))   nonvirtual  = TRUE;

  if (!cache)
  { int save = DDO->GetUINT16(0x9999, 0x0802);
    if (nonvirtual) DDO->ChangeVR(0x9999, 0x0802, (UINT16) 0x8000, 0);
    rc = VirtualQuery(DDO, Level, N, pADDO);
    if (nonvirtual) DDO->ChangeVR(0x9999, 0x0802, (UINT16) save, 0);
    return rc;
  }

  VR *vpat = DDO->GetVR(0x0010, 0x0020);
  VR *vstu = DDO->GetVR(0x0020, 0x000d);
  VR *vser = DDO->GetVR(0x0020, 0x000e);
  VR *vsop = DDO->GetVR(0x0008, 0x0018);

  int lpat=0, lstu=0, lser=0, lsop=0;
  if (vpat) lpat = vpat->Length;
  if (vstu) lstu = vstu->Length;
  if (vser) lser = vser->Length;
  if (vsop) lsop = vsop->Length;

  // do not cache wildcard patient ID searches
  if (lpat) if (memchr(vpat->Data, '*', vpat->Length)) lpat=0;

  // only cache IMAGE queries at {series,} study and patient level (pat or study {or series} fully specified)
  if (memcmp(Level, "IMAGE", 5)==0 && lsop==0 && (lpat!=0 || lstu!=0 || (lser!=0 && cacheseries)))
  { // VR *vr;
    Array < DICOMDataObject  *> Series;
    char today[70];
    struct tm tmbuf1;
    
    time_t now = time(NULL);
    localtime_r(&now, &tmbuf1);
    sprintf(today, "%04d%02d%02d", tmbuf1.tm_year+1900, tmbuf1.tm_mon, tmbuf1.tm_mday);

    // Edit query down to series/study level to ask for #images per series/study
    DICOMDataObject *DDO2 = MakeCopy(DDO);
    dgateStrip2(DDO2);
                                DDO2->ChangeVR(0x0008, 0x0020+(int)cacheseries, "", 'DA', TRUE); // series/study date
    if (cacheseries)            DDO2->ChangeVR(0x0008, 0x0052, "SERIES", 'CS', TRUE);            // level
    else                        DDO2->ChangeVR(0x0008, 0x0052, "STUDY" , 'CS', TRUE);            // level
    if (lstu==0)                DDO2->ChangeVR(0x0020, 0x000d, "", 'UI', FALSE);                 // studyuid
    if (lser==0 && cacheseries) DDO2->ChangeVR(0x0020, 0x000e, "", 'UI', FALSE);                 // seriesuid
                                DDO2->ChangeVR(0x0020, 0x1208+(int)cacheseries, "", 'IS', TRUE); // numberofseries/studyrelatedinstances
    if (nonvirtual)             DDO2->ChangeVR(0x9999, 0x0802, (UINT16) 0x8000, 0);

    if (cacheseries) VirtualQuery(DDO2, "SERIES", N, &Series);
    else             VirtualQuery(DDO2, "STUDY", N, &Series);
    delete DDO2;

    // perform sub-query per series/study and combine results
    for (unsigned int i=0; i<Series.GetSize(); i++)
    { char studuid[70], seruid[70], num[70], pat[70], date[70];

      SearchDICOMObject(Series[i], "0010,0020", pat);  
      if (*pat==0)  strcpy(pat,  "unknown");
      SearchDICOMObject(Series[i], "0020,000d", studuid);
      SearchDICOMObject(Series[i], "0020,000e", seruid);

      if (cacheseries) SearchDICOMObject(Series[i], "0020,1209", num);
      else             SearchDICOMObject(Series[i], "0020,1208", num);
      if (cacheseries) SearchDICOMObject(Series[i], "0008,0021", date);
      else             SearchDICOMObject(Series[i], "0008,0020", date);
      if (*date==0) strcpy(date, "00000000");

      DDO2 = MakeCopy(DDO);
      dgateStrip2(DDO2);
                       DDO2->ChangeVR(0x0020, 0x000d, studuid, 'UI', FALSE);
      if (cacheseries) DDO2->ChangeVR(0x0020, 0x000e, seruid,  'UI', FALSE);
      if (nonvirtual)  DDO2->ChangeVR(0x9999, 0x0802, (UINT16) 0x8000, 0);

      // still do small queries and those related to today
      if (atoi(num)<1 || strcmp(today, date)==0) 
      { if (atoi(num)<1) SystemDebug.printf("VirtualQueryCached: %s does not provide computed fields - cannot cache\n,", VirtualServerFor[N]);
      	VirtualQuery(DDO2, Level, N, pADDO);
      }
      else
      { char Filename[1024], s[1024], *p;
	VR *vr1;
	BOOL hit;
	int devlen;

	// key for hash based searching of query (collisions will force re-query)
	UINT32 crc = atoi(num) * (crc_ddo(DDO2) + 41 + 6*(int)cacheseries) + 
	             ComputeCRC(VirtualServerFor[N], strlen(VirtualServerFor[N]));
	// new query type needs other cache entry
	if (ImageType[0]) crc += 273; 

	// filename of stored query
	memset(Filename, 0, 1024);
	GetPhysicalDevice("MAG0", Filename);
	devlen = strlen(Filename);
	strcat(Filename, "printer_files");
	p = Filename + strlen(Filename);
	sprintf(p, "%cquerycache%c%4.4s%c%4.4s%c%08x.query.dcm", PATHSEPCHAR, PATHSEPCHAR, date, PATHSEPCHAR, date+4, PATHSEPCHAR, crc);

	// was the same query stored previously ?
	hit = FALSE;
	if (DFileExists(Filename))
	{ DICOMDataObject *DO2 = LoadForGUI(Filename);
	  if (DO2)
	  { dgateStrip2(DO2);
	    if (compare_ddo(DDO2, DO2))
	      hit = TRUE;
	  }
	}

	// if so, load the result from disk
	if (hit)
	{ //sprintf(p, "\\querycache\\%4.4s\\%4.4s\\%08x.result.dcm", date, date+4, crc);
	  sprintf(p, "%cquerycache%c%4.4s%c%4.4s%c%08x.result.dcm", PATHSEPCHAR, PATHSEPCHAR, date, PATHSEPCHAR, date+4, PATHSEPCHAR, crc);
	  DICOMDataObject *DO = LoadForGUI(Filename);
	  if (DO)
	  { vr1 = DO->GetVR(0x9999, 0x1000);
	    Array < DICOMDataObject * > *ADDOread =
	      (Array<DICOMDataObject*>*) vr1->SQObjectArray;
	    if (ADDOread)
	    { for (unsigned int j=0; j<ADDOread->GetSize(); j++)
	        pADDO->Add(ADDOread->Get(j));
              SystemDebug.printf("VirtualQueryCached: read %d cache records for %s:%s/%s from %s\n", 
	        ADDOread->GetSize(), pat, studuid, seruid, Filename);
	    }
	    else
	      hit = FALSE;
	  }
	  else
	    hit = FALSE;
	}

	// if not, perform the query and store the query and its result on disk
	if (!hit)
	{ int N1 = pADDO->GetSize();
	  VirtualQuery(DDO2, Level, N, pADDO);
	  int M1 = pADDO->GetSize();
	  //sprintf(p, "\\querycache\\%4.4s\\%4.4s\\%08x.query.dcm", date, date+4, crc);
	  sprintf(p, "%cquerycache%c%4.4s%c%4.4s%c%08x.query.dcm", PATHSEPCHAR, PATHSEPCHAR, date, PATHSEPCHAR, date+4, PATHSEPCHAR, crc);
	  for (unsigned int sIndex = devlen; sIndex<=strlen(Filename); sIndex++)
	    if (Filename[sIndex]==PATHSEPCHAR)
	    { strcpy(s, Filename);
	      s[sIndex]='\0';
	      mkdir(s);
	    }
          SaveDICOMDataObject(Filename, DDO2);	// destroys DDO2!

	  // transfer ADDO N1..M1 into sequence to be stored
	  vr1 = new VR(0x9999, 0x1000, 0, (void *)NULL, FALSE);
          Array < DICOMDataObject * > *SQE = new Array < DICOMDataObject * >;
	  for (int j=N1; j<M1; j++)
	  { DICOMDataObject *dd = MakeCopy(pADDO->Get(j)); 
	    SQE->Add(dd);
	  }
          vr1->SQObjectArray = (void*) SQE;

	  DICOMDataObject *D = new DICOMDataObject;
          D->Push(vr1);
	  sprintf(p, "%cquerycache%c%4.4s%c%4.4s%c%08x.result.dcm", PATHSEPCHAR, PATHSEPCHAR, date, PATHSEPCHAR, date+4, PATHSEPCHAR, crc);
	  SaveDICOMDataObject(Filename, D);
          SystemDebug.printf("VirtualQueryCached: stored %d cache records for %s:%s/%s in %s\n", 
	    M1-N1, pat, studuid, seruid, Filename);
	}
      }
      delete DDO2;
      delete Series[i];
    }
    return TRUE;
  }
  else
  { int save = DDO->GetUINT16(0x9999, 0x0802);
    if (nonvirtual) DDO->ChangeVR(0x9999, 0x0802, (UINT16) 0x8000, 0);
    rc = VirtualQuery(DDO, Level, N, pADDO);
    if (nonvirtual) DDO->ChangeVR(0x9999, 0x0802, (UINT16) save, 0);
    return rc;
  }
}

int VirtualQuery(DICOMDataObject *DDO, const char *Level, int N, Array < DICOMDataObject  *> *pADDO, char *ae)
	{
	char AE[48];
	strcpy(AE, VirtualServerFor[N]);
	if (ae) strcpy(AE, ae);
	if (*AE==0) return FALSE;

        char *param = strchr(AE, ',');
        if (param) *param++ = 0;

	unsigned char 	ip[64], port[64], compress[64], SOP[66];
	VR		*vr;
	UID		uid;
	DICOMCommandObject	DCO, DCOR;
	DICOMDataObject		DDOR;
	LE_UINT16	command, datasettype, messageid, priority;
	DICOMDataObject	*DDOPtr, *DDOPtr2;
	DBENTRY		*DBE;
	int		Index, level, count=0;
		
	level=0;
	if      (strncmp(Level, "PATIENT", 7)==0) level=1;
	else if (strncmp(Level, "STUDY",   5)==0) level=2;
	else if (strncmp(Level, "SERIES",  6)==0) level=3;
	else if (strncmp(Level, "IMAGE",   5)==0) level=4;
	else if (strncmp(Level, "WORKLIST",8)==0) level=5;

	ExtendedPDU_Service PDU;
	PDU.AttachRTC(&VRType);

	if(!GetACRNema(AE, (char *)ip, (char *)port, (char *)compress))
		return 0;

	// 20070103
	if (param)
		if (strstr(param, "FIXKODAK") || strstr(param, "fixkodak"))  // disallow wildcard date queries
			{
			BOOL flag;
			vr = DDO->GetVR(0x0010, 0x0030);	// birthdate
			flag =         (vr && vr->Length>0 && *(char *)vr->Data == '*');
			vr = DDO->GetVR(0x0008, 0x0020);	// study date
			flag = flag || (vr && vr->Length>0 && *(char *)vr->Data == '*');
			vr = DDO->GetVR(0x0008, 0x0021);	// series date
			flag = flag || (vr && vr->Length>0 && *(char *)vr->Data == '*');
			vr = DDO->GetVR(0x0008, 0x0022);	// acq date
			flag = flag || (vr && vr->Length>0 && *(char *)vr->Data == '*');
			vr = DDO->GetVR(0x0008, 0x0023);	// img date
			flag = flag || (vr && vr->Length>0 && *(char *)vr->Data == '*');
			
			if (!FixKodak)
				{
				OperatorConsole.printf("*** Virtual query - FIXKODAK is set for server %s but FixKodak is not set in dicom.ini\n", AE);
				}

			if (flag)
				{
				OperatorConsole.printf("Virtual query - blocked wildcard date query to %s\n", AE);
				return 0;
				}
			}
	

	// Start setting up connection for C-FIND
	PDU.ClearAbstractSyntaxs();
	PDU.SetLocalAddress(MYACRNEMA);
	PDU.SetRemoteAddress((unsigned char *)AE);

	uid.Set("1.2.840.10008.3.1.1.1");	// Application context (DICOM App)
	PDU.SetApplicationContext(uid);

	if      (level==1) uid.Set("1.2.840.10008.5.1.4.1.2.1.1"); // PatientRootQuery
	else if (level==5) uid.Set("1.2.840.10008.5.1.4.31");      // WorkListQuery
	else               uid.Set("1.2.840.10008.5.1.4.1.2.2.1"); // StudyRootQuery
	PDU.AddAbstractSyntax(uid);

	PDU.SetTimeOut(TCPIPTimeOut);

	// Make the association for the FIND on port/ip
	if(!PDU.Connect(ip, port))
		{
		OperatorConsole.printf("*** Virtual query - failed to connect for C-FIND to %s\n", AE);
		return ( 0 );
		}


	// Start a Patient/StudyRootQuery

	/* if (level==1)      strcpy((char*) SOP, "1.2.840.10008.5.1.4.1.2.1.1"); // PatientRootQuery
	else if (level==5) strcpy((char*) SOP, "1.2.840.10008.5.1.4.31");      // WorklistQuery
	else               strcpy((char*) SOP, "1.2.840.10008.5.1.4.1.2.2.1"); // StudyRootQuery
	vr = new VR (0x0000, 0x0002, strlen((char*)SOP), (void*) SOP, FALSE);
	DCO.Push(vr);
	command = 0x0020;
	vr = new VR (0x0000, 0x0100, 0x0002, &command, FALSE);
	DCO.Push(vr);
	priority = 0;	// MEDIUM
	vr = new VR (0x0000, 0x0700, 0x0002, &priority, FALSE);
	DCO.Push(vr);
	datasettype = 0x0102;	
	vr = new VR (0x0000, 0x0800, 0x0002, &datasettype, FALSE);
	DCO.Push(vr);
	messageid = 0x0003;
	vr = new VR (0x0000, 0x0110, 0x0002, &messageid, FALSE);
	DCO.Push(vr);
	*/

	DDOPtr  = MakeCopy(DDO);
	DDOPtr2 = MakeCopy(DDO);
	
	// Use passed data object and Level for query

	DDOPtr->ChangeVR(0x0008, 0x0052, Level, 'CS', TRUE);

	vr = DDOPtr->GetVR(0x0002, 0x0010); // delete transfer syntax
	if (vr) DDOPtr->DeleteVR(vr);

	// Here fixkodak fiximage should operate adding 0 to e.g., 9901234 patient ID
	if (param)
		if (strstr(param, "FIXKODAK") || strstr(param, "fixkodak"))
			KodakFixer(DDOPtr, TRUE);
	CallImportConverterN(NULL, DDOPtr, 2300, NULL, NULL, NULL, NULL, &PDU, NULL, NULL);

	MyPatientRootQuery mq;
	MyStudyRootQuery sq;
	MyModalityWorkListQuery wq;
	unsigned int s=pADDO->GetSize();
	
	// ImageType in query needed for IMAGETYPEFIX below
	char ImageType[128];
	ImageType[0]=0;
	vr = DDOPtr->GetVR(0x0008, 0x0008);
	if (vr && vr->Length)
		{
		strncpy(ImageType, (char*)vr->Data, vr->Length);
		ImageType[vr->Length] = 0;
		if (ImageType[vr->Length-1]==' ') ImageType[vr->Length-1]=0;
		}
	
	// EchoNumbers in query needed for IMAGETYPEFIX below
	char EchoNumbers[128];
	EchoNumbers[0]=0;
	vr = DDOPtr->GetVR(0x0018, 0x0068);
	if (vr && vr->Length)
		{
		strncpy(EchoNumbers, (char*)vr->Data, vr->Length);
		EchoNumbers[vr->Length] = 0;
		if (EchoNumbers[vr->Length-1]==' ') EchoNumbers[vr->Length-1]=0;
		}

		// forward query
	if      (level==1) mq.Write(&PDU, DDOPtr, pADDO);
	else if (level==5) wq.Write(&PDU, DDOPtr, pADDO);
	else               sq.Write(&PDU, DDOPtr, pADDO);
	
	PDU.Close();

	// anything recieved ?
	if (pADDO->GetSize() != s)
		{
		DBENTRY *dum;
		int el[1000], gr[1000], count1, j, count2;
		unsigned int i;
		BOOL us[1000];
		DICOMDataObject DO2;

		if (DebugLevel>=3)
			{
			SystemDebug.printf("Virtual query: first response from %s (total %d records)\n", AE, pADDO->GetSize() - s);
			NonDestructiveDumpDICOMObject(pADDO->Get(s));

			//for(int i=s; i<pADDO->GetSize(); i++)
			//  NonDestructiveDumpDICOMObject(pADDO->Get(s));
			}

		// build list of valid response vr's: those in query AND in our database at correct level
		// Our database model is used because the virtual server mimics ours with data from elsewhere

		count1=0;
		while ((vr = DDOPtr2->Pop()))
			{
			if (level==0)
				{
				if (VerifyIsInDBE(vr, WorkListDB, dum))
					{
					us[count1  ] = FALSE;
					el[count1  ] = vr->Element;
					gr[count1++] = vr->Group;
					delete vr;
					continue;
					}
				}
			if (level>=1)
				{
				if (VerifyIsInDBE(vr, PatientDB, dum) || (vr->Group==0x0020 && vr->Element==0x1200) || (vr->Group==0x0020 && vr->Element==0x1202) || (vr->Group==0x0020 && vr->Element==0x1204))
					{
					us[count1  ] = FALSE;
					el[count1  ] = vr->Element;
					gr[count1++] = vr->Group;
					delete vr;
					continue;
					}
				}
			if (level>=2)
				{
				if (VerifyIsInDBE(vr, StudyDB, dum) || (vr->Group==0x0020 && vr->Element==0x1208) || (vr->Group==0x0020 && vr->Element==0x1206))
					{
					us[count1  ] = FALSE;
					el[count1  ] = vr->Element;
					gr[count1++] = vr->Group;
					delete vr;
					continue;
					}
				}
			if (level>=3)
				{
				if (VerifyIsInDBE(vr, SeriesDB, dum) || (vr->Group==0x0020 && vr->Element==0x1209))
					{
					us[count1  ] = FALSE;
					el[count1  ] = vr->Element;
					gr[count1++] = vr->Group;
					delete vr;
					continue;
					}
				}
			if (level>=4)
				{
				if (VerifyIsInDBE(vr, ImageDB, dum))
					{
					us[count1  ] = FALSE;
					el[count1  ] = vr->Element;
					gr[count1++] = vr->Group;
					delete vr;
					continue;
					}
				}
			if (level>=5)
				{
				if (VerifyIsInDBE(vr, WorkListDB, dum))
					{
					us[count1  ] = FALSE;
					el[count1  ] = vr->Element;
					gr[count1++] = vr->Group;
					delete vr;
					continue;
					}
				}
			delete vr;
			}

		count2=0;	// counts valid fields during first record

		SystemDebug.printf("Virtualquery: processing response from server: %s\n", AE);
		for (i=s; i<pADDO->GetSize(); i++)
			{
			if (i==s+1 && count2==count1)		// after one record: all fields were valid 
				break;

			if (i==s+1 && count2!=count1) 
				SystemDebug.printf("Virtualquery: cleaning response from server: %s %d %d\n", AE, count2, count1);

			while((vr=pADDO->Get(i)->Pop()))
				{
				for (j=0; j<count1; j++)
					{
					if (0x0052==vr->Element && 0x0008==vr->Group)
						{
						delete vr;
						break;	// auto generated - no need to copy
						}

					if (0x0000==vr->Element)
						{
						delete vr;
						break;	// auto generated - no need to copy
						}

					if (el[j]==vr->Element && gr[j]==vr->Group)
						{		// copy valid fields
						DO2.Push(vr);
						us[j] = TRUE;
						count2++;
						break;
						}

					if (j==count1-1)
						{
						count2=-10000;	// superfluous fields: cleanup always required
						delete vr;
						}
					}
				}

			for (j=0; j<count1; j++)			// insert empties for missing fields
				{
				if (!us[j])
					{
					vr = new VR(gr[j], el[j], 0, FALSE);
					DO2.Push(vr);
					}
				else
					us[j]=FALSE;
				}

			pADDO->Get(i)->Reset();
			while((vr=DO2.Pop()))
				pADDO->Get(i)->Push(vr);

			DO2.Reset();
			}

		// Here fixkodak operates stripping 0 to e.g., 09901234 patient ID
		if (param)
			if (strstr(param, "FIXKODAK") || strstr(param, "fixkodak"))
				for (i=s; i<pADDO->GetSize(); i++)
					KodakFixer(pADDO->Get(i), FALSE);
		for (i=s; i<pADDO->GetSize(); i++)
			CallImportConverterN(NULL, pADDO->Get(i), 2400, NULL, NULL, NULL, NULL, &PDU, NULL, NULL);

		// Here IMAGETYPEFIX filters results that do not match the queried ImageType
		if (ImageType[0] && param && strstr(param, "IMAGETYPEFIX"))
			{
			int count=0;
			for (i=pADDO->GetSize()-1; i!=((unsigned int)-1); i--)
				{
				BOOL pass=TRUE;
				vr = pADDO->Get(i)->GetVR(0x0008, 0x0008);
				if (vr && vr->Length)
					{ 
					char it[256];
					char *p=it+1;
					strcpy(it, "\\");
					strncpy(p, (char*) vr->Data, vr->Length);
					p[vr->Length]=0;
					if (p[vr->Length-1]==' ') p[vr->Length-1]=0;
					strcat(it, "\\");
					char *q = strstr(it, ImageType);
					if (!q)
						pass = FALSE;
					else if (q[-1]!='\\' || q[strlen(ImageType)]!='\\')
						pass = FALSE;
					}
				vr = pADDO->Get(i)->GetVR(0x0018, 0x0068);
				if (vr && vr->Length)
					{ 
					char en[256];
					char *p=en+1;
					strcpy(en, "\\");
					strncpy(p, (char*) vr->Data, vr->Length);
					p[vr->Length]=0;
					if (p[vr->Length-1]==' ') p[vr->Length-1]=0;
					strcat(en, "\\");
					char *q = strstr(en, EchoNumbers);
					if (!q)
						pass = FALSE;
					else if (q[-1]!='\\' || q[strlen(EchoNumbers)]!='\\')
						pass = FALSE;
					}
				if (!pass)
					{
					while((vr = pADDO->Get(i)->Pop()))
						delete vr;
					delete pADDO->Get(i);
					pADDO->RemoveAt (i);
					count++;
					}
				}
			if (count)
				OperatorConsole.printf("IMAGETYPEFIX removed %d incorrect query results\n", count);
			}
		}
	else
		while ((vr = DDOPtr2->Pop()))
			delete vr;

	delete DDOPtr2;
	delete DDOPtr;

	return pADDO->GetSize() - s;
	}

int VirtualQueryToDB(DICOMDataObject *DDO, int N, char *ae=NULL)
	{
	char AE[48];
	strcpy(AE, VirtualServerFor[N]);
	if (ae) strcpy(AE, ae);
	if (*AE==0) return FALSE;

        char *param = strchr(AE, ',');
        if (param) *param++ = 0;

	char		rRoot[1024], rFilename[1024], ID[66];
	VR		*vr;
	BOOL		quit;
	Database 	DB;
	char		Device[256];
	DICOMDataObject	*DDOPtr;
	DBENTRY		*DBE;
	int		Index, count=0;

	ExtendedPDU_Service PDU;
        PDU.AttachRTC(&VRType);
		
	// 20070103
	if (param)
		if (strstr(param, "FIXKODAK") || strstr(param, "fixkodak"))  // disallow wildcard date queries
			{
			BOOL flag;
			vr = DDO->GetVR(0x0010, 0x0030);	// birthdate
			flag =         (vr && vr->Length>0 && *(char *)vr->Data == '*');
			vr = DDO->GetVR(0x0008, 0x0020);	// study date
			flag = flag || (vr && vr->Length>0 && *(char *)vr->Data == '*');
			vr = DDO->GetVR(0x0008, 0x0021);	// series date
			flag = flag || (vr && vr->Length>0 && *(char *)vr->Data == '*');
			vr = DDO->GetVR(0x0008, 0x0022);	// acq date
			flag = flag || (vr && vr->Length>0 && *(char *)vr->Data == '*');
			vr = DDO->GetVR(0x0008, 0x0023);	// img date
			flag = flag || (vr && vr->Length>0 && *(char *)vr->Data == '*');
			
			if (!FixKodak)
				{
				OperatorConsole.printf("*** Virtual query to db - FIXKODAK is set for server %s but FixKodak is not set in dicom.ini\n", AE);
				}

			if (flag)
				{
				OperatorConsole.printf("Virtual query to db - blocked wildcard date query to %s\n", AE);
				return 0;
				}
			}
	
	DDOPtr  = MakeCopy(DDO);
	
	// Use passed data object and Level for query

	DDOPtr->ChangeVR(0x0008, 0x0052, "IMAGE", 'CS', TRUE);

	vr = DDOPtr->GetVR(0x0002, 0x0010); // delete transfer syntax
	if (vr) DDOPtr->DeleteVR(vr);

	// Here fixkodak fiximage should operate adding 0 to e.g., 9901234 patient ID
	if (param)
		if (strstr(param, "FIXKODAK") || strstr(param, "fixkodak"))
			KodakFixer(DDOPtr, TRUE);
	CallImportConverterN(NULL, DDOPtr, 2300, NULL, NULL, NULL, NULL, &PDU, NULL, NULL);

	Index = 0; DBE = StudyDB;
	while ( TRUE )	// study, series and image query
		{
		if(!DBE[Index].Element) break;
		if (DDOPtr->GetVR(DBE[Index].Group, DBE[Index].Element)) {++Index; continue; }
		SetStringVR(&vr, DBE[Index].Group, DBE[Index].Element, "");
		DDOPtr->Push(vr);
		++Index;
		}
	Index = 0; DBE = SeriesDB;
	while ( TRUE )	// series and image query
		{
		if(!DBE[Index].Element) break;
		if (DDOPtr->GetVR(DBE[Index].Group, DBE[Index].Element)) {++Index; continue; }
		SetStringVR(&vr, DBE[Index].Group, DBE[Index].Element, "");
		DDOPtr->Push(vr);
		++Index;
		}

	if (!DB.Open ( DataSource, UserName, Password, DataHost ))
		{
		OperatorConsole.printf("**** Virtualquery: failed to connect to database: %s\n", AE);
		return 0;
		}

	char ImageType[128];
	ImageType[0]=0;

	Index = 0; DBE = ImageDB;
	vr = DDOPtr->GetVR(0x0008, 0x0008);
	if (vr && vr->Length)
		{
		strncpy(ImageType, (char*)vr->Data, vr->Length);
		ImageType[vr->Length] = 0;
		if (ImageType[vr->Length-1]==' ') ImageType[vr->Length-1]=0;

		// mvh 20120914: a move with an imagetype of e.g. original\phasemap
		// will match original OR phase; this is typically not intended
		// for now extract third item and assume it is intended 
		// to be differentiating, note that against conquest this 
		// only works if DT_MSTR is defined in dicom.sql for ImageType
		// Enable this option by adding virtualserver parameter IMAGETYPEFIX3
		
		if (param && strstr(param, "IMAGETYPEFIX3"))
			{
			char tmp[128];
			strcpy(tmp, ImageType);
			char *p = strchr(tmp, '\\');		// p = \primary
			if (p) p = strchr(p+1, '\\');		// p = \phasemap
			if (p)
				{
				char *q = strchr(p+1, '\\');	// end of 3rd section
				if (q) *q=0;
				SetStringVR(&vr, 0x0008, 0x0008, p+1);
				strcpy(ImageType, p+1);
				}
			}
		}

	while ( TRUE )
		{
		if(!DBE[Index].Element) break;
	
		if (DDOPtr->GetVR(DBE[Index].Group, DBE[Index].Element)) {++Index; continue; }
		SetStringVR(&vr, DBE[Index].Group, DBE[Index].Element, "");
		DDOPtr->Push(vr);
		++Index;
		}

	// Here fixkodak fiximage should operate adding 0 to e.g., 9901234 patient ID
	if (param)
		if (strstr(param, "FIXKODAK") || strstr(param, "fixkodak"))
			KodakFixer(DDOPtr, TRUE);
	CallImportConverterN(NULL, DDOPtr, 2300, NULL, NULL, NULL, NULL, &PDU, NULL, NULL);
	
	// Do the Query
	
	Array < DICOMDataObject  *> BDDO;
	DICOMDataObject		*DDOR2;
	VirtualQueryCached(DDOPtr, "IMAGE", N, &BDDO);
        
	for (int i=0; i<BDDO.GetSize(); i++)
		{
		DDOR2 = BDDO.Get(i);
	
		// full db add
		FixImage(DDOR2);	// Make sure the required DICOM UID's are there + fix patient ID if enabled
		
		*rRoot=1;
		*rFilename=0;
		if (!GenerateFileName(DDOR2, Device, rRoot, rFilename, TRUE, 0, (char *)MYACRNEMA, (char *)AE, &DB))
			OperatorConsole.printf("**** Virtual query (%s) filename problem - %s\n", AE, rFilename);
	        if (memcmp(rFilename, "lua:", 4)==0) lua_getstring(&PDU, NULL, DDOR2, rFilename+4, rFilename);
		if (*rRoot) 		// keeps set if image is not on server
			{
			if (strchr(Device, '.')==NULL)
				sprintf(Device+strlen(Device), ".%d", N);	// set virtual device # (keep lowest)
			if(!SaveToDataBase(DB, DDOR2, rFilename, Device, TRUE))
				OperatorConsole.printf("**** Virtual query db problem - %s\n", rFilename);
			else
				{
				SystemDebug.printf("Virtual query (%s) added - %s\n", AE, rFilename);
				count++;
				}
			}
		while((vr = DDOR2->Pop()))
			delete vr;
		}

	while ((vr = DDOPtr->Pop()))
		delete vr;
	delete DDOPtr;
	return count;
	}
	
// sort functions used by RemoveQueryDuplicates

static int SortPatient(const void* pElem1, const void* pElem2)
	{
	VR *vr1, *vr2;
	DICOMDataObject *ddo1= *(DICOMDataObject **) pElem1;
	DICOMDataObject *ddo2= *(DICOMDataObject **) pElem2;

	vr1 = ddo1->GetVR(0x0010, 0x0020);
	vr2 = ddo2->GetVR(0x0010, 0x0020);

	if (vr1==NULL || vr2==NULL)
		return 0;
	if (vr1->Length < vr2->Length)
		return -1;
	if (vr1->Length > vr2->Length)
		return  1;
	if (vr1->Length==0)
	        return 0;
	return memicmp((char *)vr1->Data, (char *)vr2->Data, vr1->Length);
	}

static int SortStudy(const void* pElem1, const void* pElem2)
	{
	VR *vr1, *vr2;
	DICOMDataObject *ddo1= *(DICOMDataObject **) pElem1;
	DICOMDataObject *ddo2= *(DICOMDataObject **) pElem2;

	vr1 = ddo1->GetVR(0x0020, 0x000d);
	vr2 = ddo2->GetVR(0x0020, 0x000d);

	if (vr1==NULL || vr2==NULL)
		return 0;
	if (vr1->Length < vr2->Length)
		return -1;
	if (vr1->Length > vr2->Length)
		return  1;
	if (vr1->Length==0)
	        return 0;
	return memicmp((char *)vr1->Data, (char *)vr2->Data, vr1->Length);
	}

static int SortSeries(const void* pElem1, const void* pElem2)
	{
	VR *vr1, *vr2;
	DICOMDataObject *ddo1= *(DICOMDataObject **) pElem1;
	DICOMDataObject *ddo2= *(DICOMDataObject **) pElem2;

	vr1 = ddo1->GetVR(0x0020, 0x000e);
	vr2 = ddo2->GetVR(0x0020, 0x000e);

	if (vr1==NULL || vr2==NULL)
		return 0;
	if (vr1->Length < vr2->Length)
		return -1;
	if (vr1->Length > vr2->Length)
		return  1;
	if (vr1->Length==0)
	        return 0;
	return memicmp((char *)vr1->Data, (char *)vr2->Data, vr1->Length);
	}

static int SortImages(const void* pElem1, const void* pElem2)
	{
	VR *vr1, *vr2;
	DICOMDataObject *ddo1= *(DICOMDataObject **) pElem1;
	DICOMDataObject *ddo2= *(DICOMDataObject **) pElem2;

	vr1 = ddo1->GetVR(0x0008, 0x0018);
	vr2 = ddo2->GetVR(0x0008, 0x0018);

	if (vr1==NULL || vr2==NULL)
		return 0;
	if (vr1->Length < vr2->Length)
		return -1;
	if (vr1->Length > vr2->Length)
		return  1;
	if (vr1->Length==0)
	        return 0;
	return memicmp((char *)vr1->Data, (char *)vr2->Data, vr1->Length);
	}

static int SortAccession(const void* pElem1, const void* pElem2)
	{
	VR *vr1, *vr2;
	DICOMDataObject *ddo1= *(DICOMDataObject **) pElem1;
	DICOMDataObject *ddo2= *(DICOMDataObject **) pElem2;

	vr1 = ddo1->GetVR(0x0008, 0x0050);
	vr2 = ddo2->GetVR(0x0008, 0x0050);

	if (vr1==NULL || vr2==NULL)
		return 0;
	if (vr1->Length < vr2->Length)
		return -1;
	if (vr1->Length > vr2->Length)
		return  1;
	if (vr1->Length==0)
	        return 0;
	return memicmp((char *)vr1->Data, (char *)vr2->Data, vr1->Length);
	}

static int SortImageNumber(const void* pElem1, const void* pElem2)
	{
	VR *vr1, *vr2;
	DICOMDataObject *ddo1= *(DICOMDataObject **) pElem1;
	DICOMDataObject *ddo2= *(DICOMDataObject **) pElem2;
	char t1[68], t2[68];

	vr1 = ddo1->GetVR(0x0020, 0x0013);
	vr2 = ddo2->GetVR(0x0020, 0x0013);

	if (vr1==NULL || vr2==NULL)
		return 0;

	if (vr1->Length) strncpy(t1, (char *)vr1->Data, vr1->Length); t1[vr1->Length]=0;
	if (vr2->Length) strncpy(t2, (char *)vr2->Data, vr2->Length); t2[vr2->Length]=0;

	if (atoi(t1) < atoi(t2))
		return -1;
	else if (atoi(t1) > atoi(t2))
		return  1;
	else
		return  0;
	}

// Remove duplicate entries from a query response - on selected Level

void RemoveQueryDuplicates(const char *Level, Array < DICOMDataObject * > *ADDO)
	{
	DICOMDataObject **addo = (DICOMDataObject **)malloc(ADDO->GetSize() * sizeof(DICOMDataObject *));
	int g, e;
	unsigned int i, j;
	VR *vr1, *vr2;

        int Index=0;
        //DataLink<DICOMDataObject *> *DataPointer = ADDO->first;
        while( Index < ADDO->GetSize())
        { addo[Index] = ADDO->Get(Index);
          //DataPointer = DataPointer->next;
          ++Index;
        }

	// sort on primary key
	     if (strncmp(Level, "PATIENT", 7)==0) qsort(addo, ADDO->GetSize(), sizeof(DICOMDataObject *), SortPatient),  g=0x0010, e=0x0020;
	else if (strncmp(Level, "STUDY",   5)==0) qsort(addo, ADDO->GetSize(), sizeof(DICOMDataObject *), SortStudy),    g=0x0020, e=0x000d;
	else if (strncmp(Level, "SERIES",  6)==0) qsort(addo, ADDO->GetSize(), sizeof(DICOMDataObject *), SortSeries),   g=0x0020, e=0x000e;
	else if (strncmp(Level, "IMAGE",   5)==0) qsort(addo, ADDO->GetSize(), sizeof(DICOMDataObject *), SortImages),   g=0x0008, e=0x0018;
	else if (strncmp(Level, "WORKLIST",8)==0) qsort(addo, ADDO->GetSize(), sizeof(DICOMDataObject *), SortAccession),g=0x0008, e=0x0050;
        else 
		{
		free(addo);
		return;
		}

        j = ADDO->GetSize();
	while(ADDO->GetSize()) ADDO->RemoveAt(0);

	// remove duplicates of primary key
	for (i=0; i<j; i++) 
		{
		if (i>0)
			{
			vr1 = addo[i-1]->GetVR(g, e);
			vr2 = addo[i  ]->GetVR(g, e);
			if (vr1 && vr2 && vr1->Length==vr2->Length)
				if (memicmp((char *)vr1->Data, (char *)vr2->Data, vr1->Length)==0)
					{
					// This is a double: here it should be checked if there are any
					// computed fields 0020,12xx, if so these should be ADDED
					ADDO->Get(ADDO->GetSize()-1) = addo[i];
					delete addo[i-1];
					continue;
					}
			}
		ADDO->Add(addo[i]);
		}
	SystemDebug.printf("Virtualquery: removed %d duplicates out of %d entries\n", j-ADDO->GetSize(), j);
	free(addo);
	}

// Patient, study or series finder (server functional); search string starts with = means exact

BOOL	PatientStudyFinder(char *server, char *str, char *fmt, FILE *f, const char *level)
	{	
	DICOMDataObject DDO;
	VR *vr;
	char tmp[256], tmp2[256];
	char outp[7][256];
//	BOOL local=stricmp(server, (char *)MYACRNEMA)==0;
	Array < DICOMDataObject * > ADDO;
	unsigned int i, j;
	tmp2[0]=0; tmp[0]=0;

	// search ID
	SetStringVR(&vr, 0x0008, 0x0052, level); 
	DDO.Push(vr); 
	SetStringVR(&vr, 0x0010, 0x0010, ""); 
	DDO.Push(vr);

	if (str[0] == '=')
		{
		if (level[0]=='I') sprintf(tmp2, "%s", str+1);
		else sprintf(tmp, "%s", str+1);
		}
	else
		sprintf(tmp, "*%s*", str);
	SetStringVR(&vr, 0x0010, 0x0020, tmp); 
	DDO.Push(vr);

	if (level[0]=='S' || level[0]=='I')
		{
		SetStringVR(&vr, 0x0020, 0x000d, ""); 
		DDO.Push(vr);
		if (level[1]=='E')
			{
			SetStringVR(&vr, 0x0008, 0x0020, ""); 
			DDO.Push(vr);
			SetStringVR(&vr, 0x0008, 0x0031, ""); 
			DDO.Push(vr);
			SetStringVR(&vr, 0x0020, 0x000e, ""); 
			DDO.Push(vr);
			SetStringVR(&vr, 0x0008, 0x0060, ""); 
			DDO.Push(vr);
			}
		else if (level[1]=='T')
			{
			SetStringVR(&vr, 0x0008, 0x0020, ""); 
			DDO.Push(vr);
			SetStringVR(&vr, 0x0008, 0x0061, ""); 
			DDO.Push(vr);
			}
		else if (level[1]=='M')
			{
			SetStringVR(&vr, 0x0020, 0x000e, tmp2); 
			DDO.Push(vr);
			SetStringVR(&vr, 0x0008, 0x0018, ""); 
			DDO.Push(vr);
			SetStringVR(&vr, 0x0020, 0x0013, ""); 
			DDO.Push(vr);
			SetStringVR(&vr, 0x0020, 0x1041, ""); 
			DDO.Push(vr);
			}
		}

	if (strcmp(server, "local")==0)
		{
		if (level[0]=='P') QueryOnPatient (&DDO, &ADDO);
		if (level[0]=='S' && level[1]=='T') QueryOnStudy   (&DDO, &ADDO);
		if (level[0]=='S' && level[1]=='E') QueryOnSeries  (&DDO, &ADDO);
		if (level[0]=='I' && level[1]=='M') QueryOnImage   (&DDO, &ADDO);
		}
	else
		VirtualQuery(&DDO, level, 0, &ADDO, server);

	DDO.Reset();

	// search name
	if (level[0]!='I')
		{
		SetStringVR(&vr, 0x0008, 0x0052, level); 
		DDO.Push(vr); 
		if (str[0] == '=')
			sprintf(tmp, "%s", str+1);
		else
			sprintf(tmp, "*%s*", str);
		SetStringVR(&vr, 0x0010, 0x0010, tmp); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0010, 0x0020, ""); 
		DDO.Push(vr);
		if (level[0]=='S')
			{
			SetStringVR(&vr, 0x0008, 0x0020, ""); 
			DDO.Push(vr);
			SetStringVR(&vr, 0x0020, 0x000d, ""); 
			DDO.Push(vr);
			if (level[1]=='E')
				{
				SetStringVR(&vr, 0x0008, 0x0031, ""); 
				DDO.Push(vr);
				SetStringVR(&vr, 0x0020, 0x000e, ""); 
				DDO.Push(vr);
				SetStringVR(&vr, 0x0008, 0x0060, ""); 
				DDO.Push(vr);
				}
			else
				{
				SetStringVR(&vr, 0x0008, 0x0061, ""); 
				DDO.Push(vr);
				}
			}
		if (strcmp(server, "local")==0)
			{
			if (level[0]=='P') QueryOnPatient (&DDO, &ADDO);
			if (level[0]=='S' && level[1]=='T') QueryOnStudy   (&DDO, &ADDO);
			if (level[0]=='S' && level[1]=='E') QueryOnSeries  (&DDO, &ADDO);
			}
		else
			VirtualQuery(&DDO, level, 0, &ADDO, server);

		DDO.Reset();
		}

	// search study date (=passed as is), only yyyy, yyyymm, yyyymmdd or data range queries
	if (level[0]=='S' && str[0]!='=')
		{
		SetStringVR(&vr, 0x0008, 0x0052, level); 
		DDO.Push(vr); 
		if (str[0] == '=')
			sprintf(tmp, "%s", str+1);
		else
			{
			if (strlen(str)==4 && atoi(str)>1900 && atoi(str)<2200)
				sprintf(tmp, "%s0101-%s1231", str, str);
			else if (strlen(str)==6 && atoi(str)>190000 && atoi(str)<220000)
				sprintf(tmp, "%s01-%s31", str, str);
			else if (strlen(str)==8 && atoi(str)>19000000 && atoi(str)<22000000)
				sprintf(tmp, "%s", str);
			else if (strlen(str)==9 && atoi(str)>19000000 && atoi(str)<22000000 && str[8]=='-')
				sprintf(tmp, "%s", str);
			else if (strlen(str)==9 && atoi(str+1)>19000000 && atoi(str+1)<22000000 && str[0]=='-')
				sprintf(tmp, "%s", str);
			else if (strlen(str)==17 && atoi(str)>19000000 && atoi(str)<22000000 && str[8]=='-' && atoi(str+9)>19000000 && atoi(str+9)<22000000)
				sprintf(tmp, "%s", str);
			else
				sprintf(tmp, "invalid");
			}
                

		SetStringVR(&vr, 0x0008, 0x0020, tmp); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0010, 0x0010, ""); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0010, 0x0020, ""); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0020, 0x000d, ""); 
		DDO.Push(vr);
		if (level[1]=='E')
			{
			SetStringVR(&vr, 0x0008, 0x0031, ""); 
			DDO.Push(vr);
			SetStringVR(&vr, 0x0020, 0x000e, ""); 
			DDO.Push(vr);
			SetStringVR(&vr, 0x0008, 0x0060, ""); 
			DDO.Push(vr);
			}
		else
			{
			SetStringVR(&vr, 0x0008, 0x0061, ""); 
			DDO.Push(vr);
			}


		if (strcmp(tmp, "invalid")!=0)
			{
			if (strcmp(server, "local")==0)
				{
				if (level[1]=='T') QueryOnStudy   (&DDO, &ADDO);
				if (level[1]=='E') QueryOnSeries  (&DDO, &ADDO);
				}
			else
				VirtualQuery(&DDO, level, 0, &ADDO, server);
			}

		DDO.Reset();
		}

	// search study UID (as is) for series level (exact only)
	if (level[0]=='S' && level[1]=='E' && str[0] == '=' && strlen(str)>8)
		{
		SetStringVR(&vr, 0x0008, 0x0052, level); 
		DDO.Push(vr); 
		SetStringVR(&vr, 0x0008, 0x0020, ""); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0010, 0x0010, ""); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0010, 0x0020, ""); 
		DDO.Push(vr);
		if (str[0] == '=')
			sprintf(tmp, "%s", str+1);
		else
			sprintf(tmp, "%s", str);
		SetStringVR(&vr, 0x0020, 0x000d, tmp); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0008, 0x0031, ""); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0020, 0x000e, ""); 
		DDO.Push(vr);
		SetStringVR(&vr, 0x0008, 0x0060, ""); 
		DDO.Push(vr);


		if (strcmp(server, "local")==0)
			{
			QueryOnSeries  (&DDO, &ADDO);
			}
		else
			VirtualQuery(&DDO, level, 0, &ADDO, server);

		DDO.Reset();
		}

	RemoveQueryDuplicates(level, &ADDO);

	for (i=0; i<ADDO.GetSize(); i++)
		{
		j = 0;
		while((vr=ADDO.Get(i)->Pop()))
			{
			if (0x0052==vr->Element && 0x0008==vr->Group)
				{
				delete vr;
				continue;
				}

			if (0x0000==vr->Element)
				{
				delete vr;
				continue;
				}

			if (j<7)
				{
				memcpy(outp[j], vr->Data, vr->Length);
				if (vr->Length && outp[j][vr->Length-1]==' ') outp[j++][vr->Length-1]=0;
				else                                          outp[j++][vr->Length]=0;
				}
			delete vr;
			}

		if (level[0]=='P')
  			fprintf(f, fmt, outp[0], outp[1], 
					outp[0], outp[1], 
					outp[0], outp[1], 
					outp[0], outp[1], 
					outp[0], outp[1], 
					outp[0], outp[1], 
					outp[0], outp[1], 
					outp[0], outp[1]);
		else if (level[1]=='T')
  			fprintf(f, fmt, outp[0], outp[1], outp[2], outp[3], outp[4], 
					outp[0], outp[1], outp[2], outp[3], outp[4], 
					outp[0], outp[1], outp[2], outp[3], outp[4], 
					outp[0], outp[1], outp[2], outp[3], outp[4], 
					outp[0], outp[1], outp[2], outp[3], outp[4], 
					outp[0], outp[1], outp[2], outp[3], outp[4], 
					outp[0], outp[1], outp[2], outp[3], outp[4]);
		else if (level[1]=='E' || level[1]=='M')
  			fprintf(f, fmt, outp[0], outp[1], outp[2], outp[3], outp[4], outp[5], outp[6],
					outp[0], outp[1], outp[2], outp[3], outp[4], outp[5], outp[6],
					outp[0], outp[1], outp[2], outp[3], outp[4], outp[5], outp[6],
					outp[0], outp[1], outp[2], outp[3], outp[4], outp[5], outp[6],
					outp[0], outp[1], outp[2], outp[3], outp[4], outp[5], outp[6],
					outp[0], outp[1], outp[2], outp[3], outp[4], outp[5], outp[6],
					outp[0], outp[1], outp[2], outp[3], outp[4], outp[5], outp[6]);
		delete ADDO.Get(i);
		}

	return TRUE;
	}

// Image lister: get filenames, url's or pat:uid identifier of all objects in one series
BOOL	ImageFileLister(const char *server, char *pat, char *study, char *series, char *sop, char *fmt, FILE *f)
	{	
	DICOMDataObject DDO;
	VR *vr;
	char Device[256], Filename[512], FullFilename[512], Sop[66];
//	BOOL local=stricmp(server, (char *)MYACRNEMA)==0;
	Array < DICOMDataObject * > ADDO;
	unsigned int i, j;
	DICOMDataObject **addo;

	// search ID
	SetStringVR(&vr, 0x0008, 0x0052, "IMAGE"); 
	DDO.Push(vr); 
	if (pat) 
		{
		SetStringVR(&vr, 0x0010, 0x0020, pat); 
		DDO.Push(vr);
		}
	if (study) 
		{
		SetStringVR(&vr, 0x0020, 0x000d, study); 
		DDO.Push(vr);
		}
	if (series) 
		{
		SetStringVR(&vr, 0x0020, 0x000e, series); 
		DDO.Push(vr);
		}
	if (sop) 
		{
		SetStringVR(&vr, 0x0008, 0x0018, sop); 
		DDO.Push(vr);
		}
	SetStringVR(&vr, 0x9999, 0x0800, ""); 	// filename
	DDO.Push(vr);
	SetStringVR(&vr, 0x9999, 0x0801, ""); 	// device
	DDO.Push(vr);
	SetStringVR(&vr, 0x0020, 0x0013, ""); 	// slice number
	DDO.Push(vr);
	SetStringVR(&vr, 0x0008, 0x0018, ""); 	// uid
	DDO.Push(vr);
	SetStringVR(&vr, 0x0028, 0x0008, ""); 	// #frames
	DDO.Push(vr);

	if (strcmp(server, "local")==0)
		{
		QueryOnImage (&DDO, &ADDO);
		}
	else
		VirtualQuery(&DDO, "IMAGE", 0, &ADDO, (char *)server);

	// sort on image number
	addo = (DICOMDataObject **)malloc(ADDO.GetSize() * sizeof(DICOMDataObject *));
	for (i=0; i<ADDO.GetSize(); i++) addo[i] = ADDO.Get(i);
	qsort(addo, ADDO.GetSize(), sizeof(DICOMDataObject *), SortImageNumber);
        j = ADDO.GetSize(); while(ADDO.GetSize()) ADDO.RemoveAt(0);
	for (i=0; i<j; i++) ADDO.Add(addo[i]);
	free(addo);

        if (fmt[0]=='?')
		{
		fprintf(f, fmt+1, ADDO.GetSize());

		for (i=0; i<ADDO.GetSize(); i++)
		{	while((vr=ADDO.Get(i)->Pop()))
				delete vr;
			delete ADDO.Get(i);
		}
		return true;
		}

	// locate files
	for (i=0; i<ADDO.GetSize(); i++)
		{
		char is[20], is1[20], frames[256];
		int nframes;
		sprintf(is, "%d", i);
		sprintf(is1, "%d", i+1);
		
		Filename[0] = 0; Device[0] = 0;

		vr = ADDO.Get(i)->GetVR(0x9999, 0x0800);
		if (vr) 
			{ 
			memcpy(Filename, vr->Data, vr->Length);
  		  	if (vr->Length && Filename[vr->Length-1]==' ') Filename[vr->Length-1] = 0;
			else                                           Filename[vr->Length] = 0;
			}
		vr = ADDO.Get(i)->GetVR(0x9999, 0x0801);
			if (vr) 
			{
			memcpy(Device, vr->Data, vr->Length);
  			if (vr->Length && Device[vr->Length-1]==' ') Device[vr->Length-1] = 0;
			else Device[vr->Length] = 0;
			}
		vr = ADDO.Get(i)->GetVR(0x0008, 0x0018);
			if (vr) 
			{
			memcpy(Sop, vr->Data, vr->Length);
  			if (vr->Length && Sop[vr->Length-1]==' ') Sop[vr->Length-1] = 0;
			else Sop[vr->Length] = 0;
			}

		SearchDICOMObject(ADDO.Get(i), "0028,0008", frames);
		nframes = atoi(frames);

		if (FindPhysicalDevice(Device, FullFilename, Filename))
			{
			if (i==ADDO.GetSize()-1 && fmt[strlen(fmt)-1]=='*') fmt[strlen(fmt)-1]=0;
			strcat(FullFilename, Filename);
			if      (fmt[0]=='#') 
				fprintf(f, fmt+1, Device, Filename, is, is1, Device, Filename, is, is1, Device, Filename, is, is1 );
			else if (fmt[0]=='@') 
				{
				for (j=0; j<strlen(Filename); j++) if (Filename[j]=='\\') Filename[j]='/';
				fprintf(f, fmt+1, Device, Filename, is, is1, Device, Filename, is, is1, Device, Filename, is, is1 );
				}
			else if (fmt[0]==':') 
				{
                                char out[100]; int outlen=0;
				for (j=0; j<strlen(pat); j++)
					{
					if (pat[j]==' ') { out[outlen++] = '%'; out[outlen++] = '2'; out[outlen++] = '0'; }
					else 		   out[outlen++] = pat[j];
					}
				out[outlen++]=0;
				fprintf(f, fmt+1, out, Sop, is, is1, out, Sop, is, is1, out, Sop, is, is1);
				}
			else if (fmt[0]=='$') 
				fprintf(f, fmt+1, nframes);
			else
				fprintf(f, fmt, FullFilename, is, is1, FullFilename, is, is1, FullFilename, is, is1);
			}

		while((vr=ADDO.Get(i)->Pop()))
			delete vr;
		delete ADDO.Get(i);
		}

	return TRUE;
	}

// Series lister: get all series of one (or all) study
BOOL	SeriesUIDLister(char *server, char *pat, char *study, char *fmt, FILE *f)
	{	
	DICOMDataObject DDO;
	VR *vr;
	char UID[256];
//	BOOL local=stricmp(server, (char *)MYACRNEMA)==0;
	Array < DICOMDataObject * > ADDO;
	unsigned int i;

	// search ID
	SetStringVR(&vr, 0x0008, 0x0052, "SERIES"); 
	DDO.Push(vr); 
	if (pat) 
		{
		SetStringVR(&vr, 0x0010, 0x0020, pat); 
		DDO.Push(vr);
		}
	if (study) 
		{
		SetStringVR(&vr, 0x0020, 0x000d, study); 
		DDO.Push(vr);
		}
	SetStringVR(&vr, 0x0020, 0x000e, ""); 
	DDO.Push(vr);

	if (strcmp(server, "local")==0)
		{
		QueryOnSeries (&DDO, &ADDO);
		}
	else
		VirtualQuery(&DDO, "SERIES", 0, &ADDO, server);

        if (fmt[0]=='?')
		{
		fprintf(f, fmt+1, ADDO.GetSize());

		for (i=0; i<ADDO.GetSize(); i++)
		{	while((vr=ADDO.Get(i)->Pop()))
				delete vr;
			delete ADDO.Get(i);
		}

		return TRUE;
		}

	for (i=0; i<ADDO.GetSize(); i++)
		{
		UID[0] = 0;

		vr = ADDO.Get(i)->GetVR(0x0020, 0x000e);
		if (vr) 
			{ 
			memcpy(UID, vr->Data, vr->Length);
  		  	if (vr->Length && UID[vr->Length-1]==' ') UID[vr->Length-1] = 0;
			else UID[vr->Length] = 0;

			fprintf(f, fmt, UID, i, UID, i, UID, i);
			}

		while((vr=ADDO.Get(i)->Pop()))
			delete vr;
		delete ADDO.Get(i);
		}

	return TRUE;
	}

BOOL DeleteUIDChanges(char *Reason);

// Series merge: merge all series of one study of a given modality
int	DcmMergeStudy(const char *server, char *pat, char *study, char *modality, char *seriesdesc, char *script, ExtendedPDU_Service *PDU)
	{	
	DICOMDataObject DDO;
	VR *vr;
	char UID[256];
//	BOOL local=stricmp(server, (char *)MYACRNEMA)==0;
	Array < DICOMDataObject * > ADDO;
	unsigned int i;
	char *uids[1000];
	char tempfile[512], mergeid[32];
	FILE *f;

	NewTempFile(tempfile, ".txt");

#ifdef WIN32
	sprintf(mergeid, "mergst.%u.%d.%u", (unsigned int)time(NULL), GetTickCount()%1000, (UIDPostfix++)%100);
#else
	sprintf(mergeid, "mergst.%u.%u", (unsigned int)time(NULL), (UIDPostfix++)%10000);
#endif


	// search ID
	SetStringVR(&vr, 0x0008, 0x0052, "SERIES"); 
	DDO.Push(vr); 
	if (*pat) 
		{
		SetStringVR(&vr, 0x0010, 0x0020, pat); 
		DDO.Push(vr);
		}
	if (*study) 
		{
		SetStringVR(&vr, 0x0020, 0x000d, study); 
		DDO.Push(vr);
		}
	if (*modality)
		{
		SetStringVR(&vr, 0x0008, 0x0060, modality); 
		DDO.Push(vr);
		}
	if (*seriesdesc)
		{
                SetStringVR(&vr, 0x0008, 0x103e, seriesdesc);
		DDO.Push(vr);
		}
	SetStringVR(&vr, 0x0020, 0x000e, ""); 
	DDO.Push(vr);

	if (strcmp(server, "local")==0)
		{
		QueryOnSeries (&DDO, &ADDO);
		}
	else
		VirtualQuery(&DDO, "SERIES", 0, &ADDO, (char *)server);
       
	for (i=0; i<ADDO.GetSize(); i++)
		{
		UID[0] = 0;

		vr = ADDO.Get(i)->GetVR(0x0020, 0x000e);
		if (vr) 
			{ 
			memcpy(UID, vr->Data, vr->Length);
  		  	if (vr->Length && UID[vr->Length-1]==' ') UID[vr->Length-1] = 0;
			else UID[vr->Length] = 0;

			uids[i] = strdup(UID);
			}

		while((vr=ADDO.Get(i)->Pop()))
			delete vr;
		delete ADDO.Get(i);
		}

	char temp[512];
	MergeUIDs(uids, ADDO.GetSize(), mergeid, temp);

	for (i=0; i<ADDO.GetSize(); i++)
		{
                f = fopen(tempfile, "wt");
		char formt[]="%s\n";
		ImageFileLister(server, pat, NULL, uids[i], NULL, formt, f);
		fclose(f);

		f = fopen(tempfile, "rt");
                while(fgets(temp, sizeof(temp), f) != NULL)
		{ if (temp[strlen(temp)-1]=='\n') temp[strlen(temp)-1]=0;
		  MergeUIDofImageFile(temp, TRUE, "SeriesUID", script, mergeid, PDU);
		}
		fclose(f);
		unlink(tempfile);
	
		free(uids[i]);
	}
	
	DeleteUIDChanges(mergeid);
	
	return TRUE;
	}

// Submit: zip and sftp mode: use 7za to zip and (sftp mode) pscp to transfer data to central server
// optionally data may be processed by passed script

int	DcmSubmitData(char *pat, char *study, char *series, char *sop, char *script, 
		      char *modesubmit, char *hostsubmit, int portsubmit, char *ident, int Thread)
	{	
        char tempfile[512], tempfile2[512], archfile[512], temp[512], ziptype[512];
	FILE *f, *g;
	int rc = FALSE;
	char *p;
	ExtendedPDU_Service PDU; // script context of submit process
        PDU.SetLocalAddress ( (BYTE *)"submit" );
        PDU.SetRemoteAddress ( (BYTE *)"submit" );
	int count=0, current=0;

	UNUSED_ARGUMENT(portsubmit);

	if (Thread) Progress.printf("Process=%d, Type='export', Active=1\n", Thread);

	NewTempFile(tempfile,  ".txt");
	NewTempFile(tempfile2, ".txt");

        f = fopen(tempfile, "wt");
	char formt[]="%s\n";
	ImageFileLister("local", pat, study, series, sop, formt, f);
	fclose(f);
	
	sprintf(StatusString, "Starting submission");

	if (script)
	{ f = fopen(tempfile,  "rt");
          while(fgets(temp, sizeof(temp), f) != NULL) count++;
	  fclose(f);
	  count = count;

	  f = fopen(tempfile,  "rt");
	  g = fopen(tempfile2, "wt");
          while(fgets(temp, sizeof(temp), f) != NULL)
	  { if (temp[strlen(temp)-1]=='\n') temp[strlen(temp)-1]=0;

	    sprintf(StatusString, "Processing image %d of %d", current++, count);
  	    if (Thread) Progress.printf("Process=%d, Total=%d, Current=%d\n", Thread, count+10, current);

	    DICOMDataObject *pDDO;
            pDDO = LoadForGUI(temp);
	    if (pDDO)
	    { rc = CallImportConverterN(NULL, pDDO, -1, NULL, NULL, NULL, NULL, &PDU, NULL, script);
	      if (rc!=2 && rc!=6)
	      { // NewTempFile(temp, ".dcm");
		SearchDICOMObject(pDDO, "0008,0018", temp);
		strcat(temp, ".dcm");
      
                SaveDICOMDataObject(temp, pDDO);
	        fprintf(g, "%s\n", temp);
	      }
	      delete pDDO;
	    }
	    else
	    {
	      sprintf(StatusString, "*** Failed to read DICOM image to process before submit\n");
	      OperatorConsole.printf("*** Failed to read DICOM image to process before submit\n");
	    }
	  }
	  fclose(f);
	  fclose(g);
	  unlink(tempfile);
	  strcpy(tempfile, tempfile2);
	}

        strcpy(ziptype, "zip");
        if (stricmp(modesubmit, "zip")==0)
	{ strcpy(archfile, hostsubmit);
	  p = strrchr(archfile, '.');
	  if (p) strcpy(ziptype, p+1);
	  if (stricmp(ziptype, "7z"    )==0) strcpy(ziptype, "7z");
	  if (stricmp(ziptype, "zip"   )==0) strcpy(ziptype, "zip");
	  if (stricmp(ziptype, "tar"   )==0) strcpy(ziptype, "tar");
	  unlink(archfile);
	}
	else
	  NewTempFile(archfile, ".zip");

	sprintf(StatusString, "Zipping processed images");
	if (Thread) Progress.printf("Process=%d, Total=%d, Current=%d\n", Thread, count+10, current+1);

#ifdef WIN32
	sprintf(temp, "-y a \"%s\" -t%s @%s", archfile, ziptype, tempfile);
  	BackgroundExec("7za.exe", temp);
  	// Note: does not check result of 7za
#else
	sprintf(temp, "7za -y a \"%s\" -t%s @%s", archfile, ziptype, tempfile);
        if (system(temp)!=0)
	{ OperatorConsole.printf("*** Failed zipping archive to submit/export\n");
	}
#endif

	if (Thread) Progress.printf("Process=%d, Total=%d, Current=%d\n", Thread, count+10, current+5);
	if (script)
	{ sprintf(StatusString, "Deleting processed images");
	  f = fopen(tempfile, "rt");
          while(fgets(temp, sizeof(temp), f) != NULL)
	  { if (temp[strlen(temp)-1]=='\n') temp[strlen(temp)-1]=0;
	    unlink(temp);
	  }
	  fclose(f);
	}
	unlink(tempfile);

	if (stricmp(modesubmit, "sftp")==0)
	{ sprintf(StatusString, "Starting upload");
	  sprintf(temp, "pscp -pw %s -batch %s %s", ident, archfile, hostsubmit);
          if (system(temp)!=0)
	  { OperatorConsole.printf("*** Failed uploading archive to submit\n");
	    sprintf(StatusString, "*** Failed uploading archive to submit");
	    rc = TRUE;  // may be retried (scheduletransfer)
	  }
	  else
	  { *StatusString=0;
	    unlink(archfile);
	  }
	}

	if (stricmp(modesubmit, "other")==0)
	{ sprintf(StatusString, "Starting upload");
	  sprintf(temp, ident, archfile, hostsubmit);
          if (system(temp)!=0)
	  { OperatorConsole.printf("*** Failed uploading archive to submit\n");
	    sprintf(StatusString, "*** Failed uploading archive to submit");
	    rc = TRUE;  // may be retried (scheduletransfer)
	  }
	  else
	  { *StatusString=0;
	    unlink(archfile);
	  }
	}

	if (Thread) Progress.printf("Process=%d, Total=%d, Current=%d\n", Thread, count+10, current+9);
	if (Thread) Progress.printf("Process=%d, Type='export', Active=0\n", Thread);
	return rc;
	}

BOOL	MyPatientRootQuery	::	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	UNUSED_ARGUMENT(DCO);
	return CallImportConverterN(DCO, DDO, 1000, NULL, NULL, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL)!=2;
	}

// this callback is called very often, therefore save much time by this:
// if ImportConverter 1500 does not exists check once a second it appears
static time_t t_1500=0;
BOOL	MyPatientRootQuery	::	QueryResultScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	int rc=0;
	UNUSED_ARGUMENT(DCO);
	if (!t_1500)
		{
		rc = CallImportConverterN(DCO, DDO, 1500, NULL, NULL, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL);
		if (rc==0) t_1500 = time(NULL);
		}
	else
		{
		if (t_1500 != time(NULL)) t_1500=0;
		}
	return rc!=2;
	}

// Implementation: Query
BOOL	MyPatientRootQuery	::	SearchOn (
	DICOMDataObject	*DDOPtr,
	Array < DICOMDataObject * >	*ADDO)
	{
	char	Level [ 10 ];
	int	sources=0, count1, count2;
	BOOL	st;
	UINT16	mask;

	SetString(DDOPtr->GetVR(0x0008, 0x0052), Level, 10);
	strcpy(QueryRetrieveLevel, Level);
	OperatorConsole.printf("(PatientRootQuery) search level: %s\n", Level);
	if (DebugLevel>=2) NonDestructiveDumpDICOMObject(DDOPtr);

	mask = DDOPtr->GetUINT16(0x9999, 0x0802);
	if (!mask) mask = 0xffff;

	for (int i=0; i<10; i++)
		if (mask & (1<<i))		 
			sources += VirtualQueryCached(DDOPtr, Level, i, ADDO)!=0;
	count1 = ADDO->GetSize();

        prefetcher(DDOPtr, FALSE);

	if(!strncmp(Level, "PATIENT", 7))
		st = ( QueryOnPatient (DDOPtr, ADDO));
	else if(!strncmp(Level, "STUDY", 5))
		st = ( QueryOnStudy (DDOPtr, ADDO));
	else if(!strncmp(Level, "SERIES", 6))
		st = ( QueryOnSeries (DDOPtr, ADDO));
	else if(!strncmp(Level, "IMAGE", 5))
		st = ( QueryOnImage (DDOPtr, ADDO));
	else
		st = FALSE;

	count2 = ADDO->GetSize()-count1;
	sources += count2!=0;

	if (sources>1)	// more than one source; data can be duplicated
		RemoveQueryDuplicates(Level, ADDO);

	RecordsFound = ADDO->GetSize();

	return st;
	}

BOOL	MyStudyRootQuery	::	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	UNUSED_ARGUMENT(DCO);
	return CallImportConverterN(DCO, DDO, 1000, NULL, NULL, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL)!=2;
	}

BOOL	MyStudyRootQuery	::	QueryResultScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	int rc=0;
	UNUSED_ARGUMENT(DCO);
	if (!t_1500)
		{
		rc = CallImportConverterN(DCO, DDO, 1500, NULL, NULL, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL);
		if (rc==0) t_1500 = time(NULL);
		}
	else
		{
		if (t_1500 != time(NULL)) t_1500=0;
		}
	return rc!=2;
	}

BOOL	MyStudyRootQuery	::	SearchOn (
	DICOMDataObject	*DDOPtr,
	Array < DICOMDataObject * >	*ADDO)
	{
	char	Level [ 10 ];
	int	sources=0, count1, count2;
	BOOL	st;
	UINT16	mask;

	SetString(DDOPtr->GetVR(0x0008, 0x0052), Level, 10);
	strcpy(QueryRetrieveLevel, Level);
	OperatorConsole.printf("(StudyRootQuery) search level: %s\n", Level);
        if (DebugLevel>=2) NonDestructiveDumpDICOMObject(DDOPtr);

	mask = DDOPtr->GetUINT16(0x9999, 0x0802);
	if (!mask) mask = 0xffff;

	for (int i=0; i<10; i++)
		if (mask & (1<<i))		 
			sources += VirtualQueryCached(DDOPtr, Level, i, ADDO)!=0;
	count1 = ADDO->GetSize();

        prefetcher(DDOPtr, FALSE);

	if(!strncmp(Level, "STUDY", 5))
		st = ( QueryOnStudy (DDOPtr, ADDO));
	else if(!strncmp(Level, "SERIES", 6))
		st = ( QueryOnSeries (DDOPtr, ADDO));
	else if(!strncmp(Level, "IMAGE", 5))
		st = ( QueryOnImage (DDOPtr, ADDO));
	else
		st = FALSE;

	count2 = ADDO->GetSize()-count1;
	sources += count2!=0;
	if (sources>1)
		RemoveQueryDuplicates(Level, ADDO);

	if (DebugLevel>=3 && ADDO->GetSize()!=0)
		{
		SystemDebug.printf("First record of cleaned response:\n");
		NonDestructiveDumpDICOMObject(ADDO->Get(0));
		}

	RecordsFound = ADDO->GetSize();
	return st;
	}

BOOL	MyPatientStudyOnlyQuery	::	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	UNUSED_ARGUMENT(DCO);
	return CallImportConverterN(DCO, DDO, 1000, NULL, NULL, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL)!=2;
	}

BOOL	MyPatientStudyOnlyQuery	::	QueryResultScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	int rc=0;
	UNUSED_ARGUMENT(DCO);
	if (!t_1500)
		{
		rc = CallImportConverterN(DCO, DDO, 1500, NULL, NULL, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL);
		if (rc==0) t_1500 = time(NULL);
		}
	else
		{
		if (t_1500 != time(NULL)) t_1500=0;
		}
	return rc!=2;
	}

BOOL	MyPatientStudyOnlyQuery	::	SearchOn (
	DICOMDataObject	*DDOPtr,
	Array < DICOMDataObject * >	*ADDO)
	{
	char	Level [ 10 ];
	int	sources=0, count1, count2;
	BOOL	st;
	UINT16	mask;

	SetString(DDOPtr->GetVR(0x0008, 0x0052), Level, 10);
	strcpy(QueryRetrieveLevel, Level);
	OperatorConsole.printf("(PatientStudyOnlyQuery) search level: %s\n", Level);
        if (DebugLevel>=2) NonDestructiveDumpDICOMObject(DDOPtr);

	mask = DDOPtr->GetUINT16(0x9999, 0x0802);
	if (!mask) mask = 0xffff;

	for (int i=0; i<10; i++)
		if (mask & (1<<i))
			sources += VirtualQueryCached(DDOPtr, Level, i, ADDO)!=0;
	count1 = ADDO->GetSize();

        prefetcher(DDOPtr, FALSE);

	if(!strncmp(Level, "PATIENT", 7))
		st = ( QueryOnPatient (DDOPtr, ADDO));
	else if(!strncmp(Level, "STUDY", 5))
		st = ( QueryOnStudy (DDOPtr, ADDO));
	else
		st = FALSE;

	count2 = ADDO->GetSize()-count1;
	sources += count2!=0;
	if (sources>1)
		RemoveQueryDuplicates(Level, ADDO);

	if (DebugLevel>=3 && ADDO->GetSize()!=0)
		{
		SystemDebug.printf("First record of cleaned response:\n");
		NonDestructiveDumpDICOMObject(ADDO->Get(0));
		}

	RecordsFound = ADDO->GetSize();
	return st;
	}

BOOL	MyModalityWorkListQuery	::	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	UNUSED_ARGUMENT(DCO);
	return CallImportConverterN(DCO, DDO, 1100, NULL, NULL, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL)!=2;
	}

BOOL	MyModalityWorkListQuery	::	QueryResultScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	UNUSED_ARGUMENT(DCO);
	return CallImportConverterN(DCO, DDO, 1600, NULL, NULL, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL)!=2;
	}

BOOL	MyModalityWorkListQuery	::	SearchOn (
	DICOMDataObject	*DDOPtr,
	Array < DICOMDataObject * >	*ADDO)
	{
	char	Level [ 10 ];
	int	sources=0, count1, count2;
	BOOL	st;
	UINT16	mask;

	SetString(DDOPtr->GetVR(0x0008, 0x0052), Level, 10);
	strcpy(QueryRetrieveLevel, Level);

	mask = DDOPtr->GetUINT16(0x9999, 0x0802);
	if (!mask) mask = 0xffff;

	OperatorConsole.printf("(ModalityWorkListQuery) search level: %s\n", Level);
        if (DebugLevel>=2) NonDestructiveDumpDICOMObject(DDOPtr);

//	for (int i=0; i<10; i++)
//		if (mask & (1<<i))		 
//			sources += VirtualQuery(DDOPtr, Level, i, ADDO)!=0;
	count1 = ADDO->GetSize();

//        prefetcher(DDOPtr, FALSE);

	st = ( QueryOnModalityWorkList (DDOPtr, ADDO));

	count2 = ADDO->GetSize()-count1;
	sources += count2!=0;
	if (sources>1)
		RemoveQueryDuplicates(Level, ADDO);

	if (DebugLevel>=3 && ADDO->GetSize()!=0)
		{
		SystemDebug.printf("First record of cleaned response:\n");
		NonDestructiveDumpDICOMObject(ADDO->Get(0));
		}

	RecordsFound = ADDO->GetSize();
	return st;
	}

BOOL	MyPatientRootQuery	::	CallBack (
	DICOMCommandObject	*DCOPtr,
	DICOMDataObject		*DDOPtr)
	{
	UNUSED_ARGUMENT(DCOPtr);
	UNUSED_ARGUMENT(DDOPtr);
	return ( TRUE );
	}

BOOL	MyStudyRootQuery	::	CallBack (
	DICOMCommandObject	*DCOPtr,
	DICOMDataObject		*DDOPtr)
	{
	UNUSED_ARGUMENT(DCOPtr);
	UNUSED_ARGUMENT(DDOPtr);
	return ( TRUE );
	}

BOOL	MyPatientStudyOnlyQuery	::	CallBack (
	DICOMCommandObject	*DCOPtr,
	DICOMDataObject		*DDOPtr)
	{
	UNUSED_ARGUMENT(DCOPtr);
	UNUSED_ARGUMENT(DDOPtr);
	return ( TRUE );
	}

BOOL	MyModalityWorkListQuery	::	CallBack (
	DICOMCommandObject	*DCOPtr,
	DICOMDataObject		*DDOPtr)
	{
	UNUSED_ARGUMENT(DCOPtr);
	UNUSED_ARGUMENT(DDOPtr);
	return ( TRUE );
	}


BOOL	MyPatientRootRetrieve	::	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	char dest[20];
	memset(dest, 0, 20);
	VR *vr = DCO->GetVR(0x0000, 0x0600);
	if (vr) memcpy(dest, (char *)(vr->Data), vr->Length);
        if (dest[0]>32)
          while (strlen(dest)>0 && dest[strlen(dest)-1]==' ') dest[strlen(dest)-1] = 0;

	return CallImportConverterN(DCO, DDO, 1200, NULL, dest, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL)!=2;
	}

BOOL	MyPatientRootRetrieve	::	SearchOn (
	DICOMDataObject	*DDOPtr,
	Array < DICOMDataObject * >	*ADDO)
	{
	VR		*vr;
	UINT16	mask;

        prefetcher(DDOPtr, TRUE);

	SystemDebug.printf("MyPatientRootRetrieve :: SearchOn\n");
        if (DebugLevel>=2) NonDestructiveDumpDICOMObject(DDOPtr);

	mask = DDOPtr->GetUINT16(0x9999, 0x0802);
	if (!mask) mask = 0xffff;

	for (int i=0; i<10; i++)
		if (mask & (1<<i))		 
			VirtualQueryToDB(DDOPtr, i);

	if(!DDOPtr->GetVR(0x0002, 0x0010))
		{
		vr = new VR ( 0x0002, 0x0010, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0018))
		{
		vr = new VR ( 0x0008, 0x0018, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0016))
		{
		vr = new VR ( 0x0008, 0x0016, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000d))
		{
		vr = new VR ( 0x0020, 0x000d, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000e))
		{
		vr = new VR ( 0x0020, 0x000e, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0800))
		{
		vr = new VR ( 0x9999, 0x0800, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0801))
		{
		vr = new VR ( 0x9999, 0x0801, 0, FALSE );
		DDOPtr->Push(vr);
		}
	QueryOnImage(DDOPtr, ADDO);
	if(!ADDO->GetSize())
		return(FALSE);
        OperatorConsole.printf("Number of images to send: %d\n", ADDO->GetSize());
	return(TRUE);
	}

BOOL	MyPatientRootRetrieveNKI	::	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	char dest[20];
	memset(dest, 0, 20);
	VR *vr = DCO->GetVR(0x0000, 0x0600);
	if (vr) memcpy(dest, (char *)(vr->Data), vr->Length);
	if (dest[0]>32)
          while (strlen(dest)>0 && dest[strlen(dest)-1]==' ') dest[strlen(dest)-1] = 0;

	return CallImportConverterN(DCO, DDO, 1200, NULL, dest, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL)!=2;
	}

BOOL	MyPatientRootRetrieveNKI	::	SearchOn (
	DICOMDataObject	*DDOPtr,
	Array < DICOMDataObject * >	*ADDO)
	{
	VR		*vr;
	UINT16		mask;

        prefetcher(DDOPtr, TRUE);

	SystemDebug.printf("MyPatientRootRetrieveNKI :: SearchOn\n");
        if (DebugLevel>=2) NonDestructiveDumpDICOMObject(DDOPtr);

	mask = DDOPtr->GetUINT16(0x9999, 0x0802);
	if (!mask) mask = 0xffff;

	for (int i=0; i<10; i++)
		if (mask & (1<<i))		 
			VirtualQueryToDB(DDOPtr, i);

	if(!DDOPtr->GetVR(0x0002, 0x0010))
		{
		vr = new VR ( 0x0002, 0x0010, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0018))
		{
		vr = new VR ( 0x0008, 0x0018, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0016))
		{
		vr = new VR ( 0x0008, 0x0016, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000d))
		{
		vr = new VR ( 0x0020, 0x000d, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000e))
		{
		vr = new VR ( 0x0020, 0x000e, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0800))
		{
		vr = new VR ( 0x9999, 0x0800, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0801))
		{
		vr = new VR ( 0x9999, 0x0801, 0, FALSE );
		DDOPtr->Push(vr);
		}
	QueryOnImage(DDOPtr, ADDO);
	if(!ADDO->GetSize())
		return(FALSE);
        OperatorConsole.printf("Number of images to send: %d\n", ADDO->GetSize());
	return(TRUE);
	}

BOOL	MyPatientRootRetrieveGeneric	::	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	char dest[20];
	memset(dest, 0, 20);
	VR *vr = DCO->GetVR(0x0000, 0x0600);
	if (vr) memcpy(dest, (char *)(vr->Data), vr->Length);
	if (dest[0]>32)
          while (strlen(dest)>0 && dest[strlen(dest)-1]==' ') dest[strlen(dest)-1] = 0;

	return CallImportConverterN(DCO, DDO, 1200, NULL, dest, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL)!=2;
	}

BOOL	MyPatientRootGetGeneric	::	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	char dest[20];
	memset(dest, 0, 20);
	VR *vr = DCO->GetVR(0x0000, 0x0600);
	if (vr) memcpy(dest, (char *)(vr->Data), vr->Length);
	if (dest[0]>32)
          while (strlen(dest)>0 && dest[strlen(dest)-1]==' ') dest[strlen(dest)-1] = 0;

	return CallImportConverterN(DCO, DDO, 1200, NULL, dest, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL)!=2;
	}
	
BOOL	MyPatientRootRetrieveGeneric	::	SearchOn (
	DICOMDataObject	*DDOPtr,
	Array < DICOMDataObject * >	*ADDO)
	{
	VR		*vr;
	UINT16		mask;

        prefetcher(DDOPtr, TRUE);

	SystemDebug.printf("MyPatientRootRetrieveGeneric :: SearchOn\n");
        if (DebugLevel>=2) NonDestructiveDumpDICOMObject(DDOPtr);

	mask = DDOPtr->GetUINT16(0x9999, 0x0802);
	if (!mask) mask = 0xffff;

	for (int i=0; i<10; i++)
		if (mask & (1<<i))		 
			VirtualQueryToDB(DDOPtr, i);

	if(!DDOPtr->GetVR(0x0002, 0x0010))
		{
		vr = new VR ( 0x0002, 0x0010, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0018))
		{
		vr = new VR ( 0x0008, 0x0018, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0016))
		{
		vr = new VR ( 0x0008, 0x0016, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000d))
		{
		vr = new VR ( 0x0020, 0x000d, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000e))
		{
		vr = new VR ( 0x0020, 0x000e, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0800))
		{
		vr = new VR ( 0x9999, 0x0800, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0801))
		{
		vr = new VR ( 0x9999, 0x0801, 0, FALSE );
		DDOPtr->Push(vr);
		}
	QueryOnImage(DDOPtr, ADDO);
	if(!ADDO->GetSize())
		return(FALSE);
        OperatorConsole.printf("Number of images to send: %d\n", ADDO->GetSize());
	return(TRUE);
	}

BOOL	MyPatientRootGetGeneric	::	SearchOn (
	DICOMDataObject	*DDOPtr,
	Array < DICOMDataObject * >	*ADDO)
	{
	VR		*vr;
	UINT16		mask;

        prefetcher(DDOPtr, TRUE);

	SystemDebug.printf("MyPatientRootGetGeneric :: SearchOn\n");
        if (DebugLevel>=2) NonDestructiveDumpDICOMObject(DDOPtr);

	mask = DDOPtr->GetUINT16(0x9999, 0x0802);
	if (!mask) mask = 0xffff;

	for (int i=0; i<10; i++)
		if (mask & (1<<i))		 
			VirtualQueryToDB(DDOPtr, i);

	if(!DDOPtr->GetVR(0x0002, 0x0010))
		{
		vr = new VR ( 0x0002, 0x0010, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0018))
		{
		vr = new VR ( 0x0008, 0x0018, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0016))
		{
		vr = new VR ( 0x0008, 0x0016, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000d))
		{
		vr = new VR ( 0x0020, 0x000d, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000e))
		{
		vr = new VR ( 0x0020, 0x000e, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0800))
		{
		vr = new VR ( 0x9999, 0x0800, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0801))
		{
		vr = new VR ( 0x9999, 0x0801, 0, FALSE );
		DDOPtr->Push(vr);
		}
	QueryOnImage(DDOPtr, ADDO);
	if(!ADDO->GetSize())
		return(FALSE);
        OperatorConsole.printf("Number of images to send: %d\n", ADDO->GetSize());
	return(TRUE);
	}
	
BOOL	MyStudyRootRetrieve	::	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	char dest[20];
	memset(dest, 0, 20);
	VR *vr = DCO->GetVR(0x0000, 0x0600);
	if (vr) memcpy(dest, (char *)(vr->Data), vr->Length);
	if (dest[0]>32)
          while (strlen(dest)>0 && dest[strlen(dest)-1]==' ') dest[strlen(dest)-1] = 0;

	return CallImportConverterN(DCO, DDO, 1200, NULL, dest, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL)!=2;
	}

BOOL	MyStudyRootRetrieve	::	SearchOn (
	DICOMDataObject	*DDOPtr,
	Array < DICOMDataObject * >	*ADDO)
	{
	VR	*vr;
	UINT16	mask;

        prefetcher(DDOPtr, TRUE);

	SystemDebug.printf("MyStudyRootRetrieve :: SearchOn\n");
        if (DebugLevel>=2) NonDestructiveDumpDICOMObject(DDOPtr);

	mask = DDOPtr->GetUINT16(0x9999, 0x0802);
	if (!mask) mask = 0xffff;

	for (int i=0; i<10; i++)
		if (mask & (1<<i))		 
			VirtualQueryToDB(DDOPtr, i);

	if(!DDOPtr->GetVR(0x0002, 0x0010))
		{
		vr = new VR ( 0x0002, 0x0010, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0018))
		{
		vr = new VR ( 0x0008, 0x0018, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0016))
		{
		vr = new VR ( 0x0008, 0x0016, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000d))
		{
		vr = new VR ( 0x0020, 0x000d, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000e))
		{
		vr = new VR ( 0x0020, 0x000e, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0800))
		{
		vr = new VR ( 0x9999, 0x0800, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0801))
		{
		vr = new VR ( 0x9999, 0x0801, 0, FALSE );
		DDOPtr->Push(vr);
		}
	QueryOnImage(DDOPtr, ADDO);
	if(!ADDO->GetSize())
		return(FALSE);
	OperatorConsole.printf("Number of Images to send: %d\n", ADDO->GetSize());
	return(TRUE);
	}

BOOL	MyStudyRootRetrieveNKI	::	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	char dest[20];
	memset(dest, 0, 20);
	VR *vr = DCO->GetVR(0x0000, 0x0600);
	if (vr) memcpy(dest, (char *)(vr->Data), vr->Length);
	if (dest[0]>32)
          while (strlen(dest)>0 && dest[strlen(dest)-1]==' ') dest[strlen(dest)-1] = 0;

	return CallImportConverterN(DCO, DDO, 1200, NULL, dest, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL)!=2;
	}

BOOL	MyStudyRootRetrieveNKI	::	SearchOn (
	DICOMDataObject	*DDOPtr,
	Array < DICOMDataObject * >	*ADDO)
	{
	VR	*vr;
	UINT16	mask;

        prefetcher(DDOPtr, TRUE);

	SystemDebug.printf("MyStudyRootRetrieveNKI :: SearchOn\n");
        if (DebugLevel>=2) NonDestructiveDumpDICOMObject(DDOPtr);

	mask = DDOPtr->GetUINT16(0x9999, 0x0802);
	if (!mask) mask = 0xffff;

	for (int i=0; i<10; i++)
		if (mask & (1<<i))		 
			VirtualQueryToDB(DDOPtr, i);

	if(!DDOPtr->GetVR(0x0002, 0x0010))
		{
		vr = new VR ( 0x0002, 0x0010, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0018))
		{
		vr = new VR ( 0x0008, 0x0018, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0016))
		{
		vr = new VR ( 0x0008, 0x0016, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000d))
		{
		vr = new VR ( 0x0020, 0x000d, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000e))
		{
		vr = new VR ( 0x0020, 0x000e, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0800))
		{
		vr = new VR ( 0x9999, 0x0800, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0801))
		{
		vr = new VR ( 0x9999, 0x0801, 0, FALSE );
		DDOPtr->Push(vr);
		}
	QueryOnImage(DDOPtr, ADDO);
	if(!ADDO->GetSize())
		return(FALSE);
	OperatorConsole.printf("Number of Images to send: %d\n", ADDO->GetSize());
	return(TRUE);
	}

BOOL	MyStudyRootRetrieveGeneric	::	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	char dest[20];
	memset(dest, 0, 20);
	VR *vr = DCO->GetVR(0x0000, 0x0600);
	if (vr) memcpy(dest, (char *)(vr->Data), vr->Length);
	if (dest[0]>32)
          while (strlen(dest)>0 && dest[strlen(dest)-1]==' ') dest[strlen(dest)-1] = 0;

	return CallImportConverterN(DCO, DDO, 1200, NULL, dest, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL)!=2;
	}

BOOL	MyStudyRootGetGeneric	::	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	char dest[20];
	memset(dest, 0, 20);
	VR *vr = DCO->GetVR(0x0000, 0x0600);
	if (vr) memcpy(dest, (char *)(vr->Data), vr->Length);
	if (dest[0]>32)
          while (strlen(dest)>0 && dest[strlen(dest)-1]==' ') dest[strlen(dest)-1] = 0;

	return CallImportConverterN(DCO, DDO, 1200, NULL, dest, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL)!=2;
	}

BOOL	MyStudyRootRetrieveGeneric	::	SearchOn (
	DICOMDataObject	*DDOPtr,
	Array < DICOMDataObject * >	*ADDO)
	{
	VR	*vr;
	UINT16	mask;

        prefetcher(DDOPtr, TRUE);

	SystemDebug.printf("MyStudyRootRetrieveGeneric :: SearchOn\n");
        if (DebugLevel>=2) NonDestructiveDumpDICOMObject(DDOPtr);

	mask = DDOPtr->GetUINT16(0x9999, 0x0802);
	if (!mask) mask = 0xffff;

	for (int i=0; i<10; i++)
		if (mask & (1<<i))		 
			VirtualQueryToDB(DDOPtr, i);

	if(!DDOPtr->GetVR(0x0002, 0x0010))
		{
		vr = new VR ( 0x0002, 0x0010, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0018))
		{
		vr = new VR ( 0x0008, 0x0018, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0016))
		{
		vr = new VR ( 0x0008, 0x0016, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000d))
		{
		vr = new VR ( 0x0020, 0x000d, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000e))
		{
		vr = new VR ( 0x0020, 0x000e, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0800))
		{
		vr = new VR ( 0x9999, 0x0800, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0801))
		{
		vr = new VR ( 0x9999, 0x0801, 0, FALSE );
		DDOPtr->Push(vr);
		}
	QueryOnImage(DDOPtr, ADDO);
	if(!ADDO->GetSize())
		return(FALSE);
	OperatorConsole.printf("Number of Images to send: %d\n", ADDO->GetSize());
	return(TRUE);
	}

BOOL	MyStudyRootGetGeneric	::	SearchOn (
	DICOMDataObject	*DDOPtr,
	Array < DICOMDataObject * >	*ADDO)
	{
	VR	*vr;
	UINT16	mask;

        prefetcher(DDOPtr, TRUE);

	SystemDebug.printf("MyStudyRootGetGeneric :: SearchOn\n");
        if (DebugLevel>=2) NonDestructiveDumpDICOMObject(DDOPtr);

	mask = DDOPtr->GetUINT16(0x9999, 0x0802);
	if (!mask) mask = 0xffff;

	for (int i=0; i<10; i++)
		if (mask & (1<<i))		 
			VirtualQueryToDB(DDOPtr, i);

	if(!DDOPtr->GetVR(0x0002, 0x0010))
		{
		vr = new VR ( 0x0002, 0x0010, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0018))
		{
		vr = new VR ( 0x0008, 0x0018, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0016))
		{
		vr = new VR ( 0x0008, 0x0016, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000d))
		{
		vr = new VR ( 0x0020, 0x000d, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000e))
		{
		vr = new VR ( 0x0020, 0x000e, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0800))
		{
		vr = new VR ( 0x9999, 0x0800, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0801))
		{
		vr = new VR ( 0x9999, 0x0801, 0, FALSE );
		DDOPtr->Push(vr);
		}
	QueryOnImage(DDOPtr, ADDO);
	if(!ADDO->GetSize())
		return(FALSE);
	OperatorConsole.printf("Number of Images to send: %d\n", ADDO->GetSize());
	return(TRUE);
	}

BOOL	MyPatientStudyOnlyRetrieve	::	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	char dest[20];
	memset(dest, 0, 20);
	VR *vr = DCO->GetVR(0x0000, 0x0600);
	if (vr) memcpy(dest, (char *)(vr->Data), vr->Length);
	if (dest[0]>32)
          while (strlen(dest)>0 && dest[strlen(dest)-1]==' ') dest[strlen(dest)-1] = 0;

	return CallImportConverterN(DCO, DDO, 1200, NULL, dest, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL)!=2;
	}

BOOL	MyPatientStudyOnlyRetrieve	::	SearchOn (
	DICOMDataObject	*DDOPtr,
	Array < DICOMDataObject * >	*ADDO)
	{
	VR	*vr;
	UINT16	mask;

        prefetcher(DDOPtr, TRUE);

	SystemDebug.printf("MyPatientStudyOnlyRetrieve :: SearchOn\n");
        if (DebugLevel>=2) NonDestructiveDumpDICOMObject(DDOPtr);

	mask = DDOPtr->GetUINT16(0x9999, 0x0802);
	if (!mask) mask = 0xffff;

	for (int i=0; i<10; i++)
		if (mask & (1<<i))		 
			VirtualQueryToDB(DDOPtr, i);

	if(!DDOPtr->GetVR(0x0002, 0x0010))
		{
		vr = new VR ( 0x0002, 0x0010, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0018))
		{
		vr = new VR ( 0x0008, 0x0018, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0016))
		{
		vr = new VR ( 0x0008, 0x0016, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000d))
		{
		vr = new VR ( 0x0020, 0x000d, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000e))
		{
		vr = new VR ( 0x0020, 0x000e, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0800))
		{
		vr = new VR ( 0x9999, 0x0800, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0801))
		{
		vr = new VR ( 0x9999, 0x0801, 0, FALSE );
		DDOPtr->Push(vr);
		}
	QueryOnImage(DDOPtr, ADDO);
	if(!ADDO->GetSize())
		return(FALSE);
	OperatorConsole.printf("Number of Images to send: %d\n", ADDO->GetSize());
	return(TRUE);
	}

BOOL	MyPatientStudyOnlyRetrieveNKI	::	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	char dest[20];
	memset(dest, 0, 20);
	VR *vr = DCO->GetVR(0x0000, 0x0600);
	if (vr) memcpy(dest, (char *)(vr->Data), vr->Length);
	if (dest[0]>32)
          while (strlen(dest)>0 && dest[strlen(dest)-1]==' ') dest[strlen(dest)-1] = 0;

	return CallImportConverterN(DCO, DDO, 1200, NULL, dest, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL)!=2;
	}

BOOL	MyPatientStudyOnlyRetrieveNKI	::	SearchOn (
	DICOMDataObject	*DDOPtr,
	Array < DICOMDataObject * >	*ADDO)
	{
	VR	*vr;
	UINT16	mask;

        prefetcher(DDOPtr, TRUE);

	SystemDebug.printf("MyPatientStudyOnlyRetrieveNKI :: SearchOn\n");
        if (DebugLevel>=2) NonDestructiveDumpDICOMObject(DDOPtr);

	mask = DDOPtr->GetUINT16(0x9999, 0x0802);
	if (!mask) mask = 0xffff;

	for (int i=0; i<10; i++)
		if (mask & (1<<i))		 
			VirtualQueryToDB(DDOPtr, i);

	if(!DDOPtr->GetVR(0x0002, 0x0010))
		{
		vr = new VR ( 0x0002, 0x0010, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0018))
		{
		vr = new VR ( 0x0008, 0x0018, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0016))
		{
		vr = new VR ( 0x0008, 0x0016, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000d))
		{
		vr = new VR ( 0x0020, 0x000d, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000e))
		{
		vr = new VR ( 0x0020, 0x000e, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0800))
		{
		vr = new VR ( 0x9999, 0x0800, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0801))
		{
		vr = new VR ( 0x9999, 0x0801, 0, FALSE );
		DDOPtr->Push(vr);
		}
	QueryOnImage(DDOPtr, ADDO);
	if(!ADDO->GetSize())
		return(FALSE);
	OperatorConsole.printf("Number of Images to send: %d\n", ADDO->GetSize());
	return(TRUE);
	}

BOOL	MyPatientStudyOnlyRetrieveGeneric	::	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	char dest[20];
	memset(dest, 0, 20);
	VR *vr = DCO->GetVR(0x0000, 0x0600);
	if (vr) memcpy(dest, (char *)(vr->Data), vr->Length);
	if (dest[0]>32)
          while (strlen(dest)>0 && dest[strlen(dest)-1]==' ') dest[strlen(dest)-1] = 0;

	return CallImportConverterN(DCO, DDO, 1200, NULL, dest, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL)!=2;
	}

BOOL	MyPatientStudyOnlyGetGeneric	::	QueryMoveScript (PDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO)
	{
	char dest[20];
	memset(dest, 0, 20);
	VR *vr = DCO->GetVR(0x0000, 0x0600);
	if (vr) memcpy(dest, (char *)(vr->Data), vr->Length);
	if (dest[0]>32)
          while (strlen(dest)>0 && dest[strlen(dest)-1]==' ') dest[strlen(dest)-1] = 0;

	return CallImportConverterN(DCO, DDO, 1200, NULL, dest, NULL, NULL, (ExtendedPDU_Service *)PDU, NULL, NULL)!=2;
	}

BOOL	MyPatientStudyOnlyRetrieveGeneric	::	SearchOn (
	DICOMDataObject	*DDOPtr,
	Array < DICOMDataObject * >	*ADDO)
	{
	VR	*vr;
	UINT16	mask;

        prefetcher(DDOPtr, TRUE);

	SystemDebug.printf("MyPatientStudyOnlyRetrieveGeneric :: SearchOn\n");
        if (DebugLevel>=2) NonDestructiveDumpDICOMObject(DDOPtr);

	mask = DDOPtr->GetUINT16(0x9999, 0x0802);
	if (!mask) mask = 0xffff;

	for (int i=0; i<10; i++)
		if (mask & (1<<i))		 
			VirtualQueryToDB(DDOPtr, i);

	if(!DDOPtr->GetVR(0x0002, 0x0010))
		{
		vr = new VR ( 0x0002, 0x0010, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0018))
		{
		vr = new VR ( 0x0008, 0x0018, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0016))
		{
		vr = new VR ( 0x0008, 0x0016, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000d))
		{
		vr = new VR ( 0x0020, 0x000d, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000e))
		{
		vr = new VR ( 0x0020, 0x000e, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0800))
		{
		vr = new VR ( 0x9999, 0x0800, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0801))
		{
		vr = new VR ( 0x9999, 0x0801, 0, FALSE );
		DDOPtr->Push(vr);
		}
	QueryOnImage(DDOPtr, ADDO);
	if(!ADDO->GetSize())
		return(FALSE);
	OperatorConsole.printf("Number of Images to send: %d\n", ADDO->GetSize());
	return(TRUE);
	}

BOOL	MyPatientStudyOnlyGetGeneric	::	SearchOn (
	DICOMDataObject	*DDOPtr,
	Array < DICOMDataObject * >	*ADDO)
	{
	VR	*vr;
	UINT16	mask;

        prefetcher(DDOPtr, TRUE);

	SystemDebug.printf("MyPatientStudyOnlyGetGeneric :: SearchOn\n");
        if (DebugLevel>=2) NonDestructiveDumpDICOMObject(DDOPtr);

	mask = DDOPtr->GetUINT16(0x9999, 0x0802);
	if (!mask) mask = 0xffff;

	for (int i=0; i<10; i++)
		if (mask & (1<<i))		 
			VirtualQueryToDB(DDOPtr, i);

	if(!DDOPtr->GetVR(0x0002, 0x0010))
		{
		vr = new VR ( 0x0002, 0x0010, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0018))
		{
		vr = new VR ( 0x0008, 0x0018, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0008, 0x0016))
		{
		vr = new VR ( 0x0008, 0x0016, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000d))
		{
		vr = new VR ( 0x0020, 0x000d, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x0020, 0x000e))
		{
		vr = new VR ( 0x0020, 0x000e, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0800))
		{
		vr = new VR ( 0x9999, 0x0800, 0, FALSE );
		DDOPtr->Push(vr);
		}
	if(!DDOPtr->GetVR(0x9999, 0x0801))
		{
		vr = new VR ( 0x9999, 0x0801, 0, FALSE );
		DDOPtr->Push(vr);
		}
	QueryOnImage(DDOPtr, ADDO);
	if(!ADDO->GetSize())
		return(FALSE);
	OperatorConsole.printf("Number of Images to send: %d\n", ADDO->GetSize());
	return(TRUE);
	}

BOOL	MyPatientRootRetrieve	::	CallBack (
	DICOMCommandObject	*DCOPtr,
	DICOMDataObject		*DDOPtr)
	{
	UNUSED_ARGUMENT(DCOPtr);
	UNUSED_ARGUMENT(DDOPtr);
	return ( TRUE );
	}

BOOL	MyPatientRootRetrieveNKI	::	CallBack (
	DICOMCommandObject	*DCOPtr,
	DICOMDataObject		*DDOPtr)
	{
	UNUSED_ARGUMENT(DCOPtr);
	UNUSED_ARGUMENT(DDOPtr);
	return ( TRUE );
	}

BOOL	MyPatientRootRetrieveGeneric	::	CallBack (
	DICOMCommandObject	*DCOPtr,
	DICOMDataObject		*DDOPtr)
	{
	UNUSED_ARGUMENT(DCOPtr);
	UNUSED_ARGUMENT(DDOPtr);
	return ( TRUE );
	}

BOOL	MyPatientRootGetGeneric	::	CallBack (
	DICOMCommandObject	*DCOPtr,
	DICOMDataObject		*DDOPtr)
	{
	UNUSED_ARGUMENT(DCOPtr);
	UNUSED_ARGUMENT(DDOPtr);
	return ( TRUE );
	}

BOOL	MyStudyRootRetrieve	::	CallBack (
	DICOMCommandObject	*DCOPtr,
	DICOMDataObject		*DDOPtr)
	{
	UNUSED_ARGUMENT(DCOPtr);
	UNUSED_ARGUMENT(DDOPtr);
	return ( TRUE );
	}

BOOL	MyStudyRootRetrieveNKI	::	CallBack (
	DICOMCommandObject	*DCOPtr,
	DICOMDataObject		*DDOPtr)
	{
	UNUSED_ARGUMENT(DCOPtr);
	UNUSED_ARGUMENT(DDOPtr);
	return ( TRUE );
	}

BOOL	MyStudyRootRetrieveGeneric	::	CallBack (
	DICOMCommandObject	*DCOPtr,
	DICOMDataObject		*DDOPtr)
	{
	UNUSED_ARGUMENT(DCOPtr);
	UNUSED_ARGUMENT(DDOPtr);
	return ( TRUE );
	}

BOOL	MyStudyRootGetGeneric	::	CallBack (
	DICOMCommandObject	*DCOPtr,
	DICOMDataObject		*DDOPtr)
	{
	UNUSED_ARGUMENT(DCOPtr);
	UNUSED_ARGUMENT(DDOPtr);
	return ( TRUE );
	}
	
BOOL	MyPatientStudyOnlyRetrieve	::	CallBack (
	DICOMCommandObject	*DCOPtr,
	DICOMDataObject		*DDOPtr)
	{
	UNUSED_ARGUMENT(DCOPtr);
	UNUSED_ARGUMENT(DDOPtr);
	return ( TRUE );
	}

BOOL	MyPatientStudyOnlyRetrieveNKI	::	CallBack (
	DICOMCommandObject	*DCOPtr,
	DICOMDataObject		*DDOPtr)
	{
	UNUSED_ARGUMENT(DCOPtr);
	UNUSED_ARGUMENT(DDOPtr);
	return ( TRUE );
	}

BOOL	MyPatientStudyOnlyRetrieveGeneric	::	CallBack (
	DICOMCommandObject	*DCOPtr,
	DICOMDataObject		*DDOPtr)
	{
	UNUSED_ARGUMENT(DCOPtr);
	UNUSED_ARGUMENT(DDOPtr);
	return ( TRUE );
	}

BOOL	MyPatientStudyOnlyGetGeneric	::	CallBack (
	DICOMCommandObject	*DCOPtr,
	DICOMDataObject		*DDOPtr)
	{
	UNUSED_ARGUMENT(DCOPtr);
	UNUSED_ARGUMENT(DDOPtr);
	return ( TRUE );
	}
	
/* Read ahead implementation */

#define STATUS_EMPTY 0
#define STATUS_FAILED 1
#define STATUS_READ 2
#define STATUS_RETRIEVED 3
#define STATUS_BUSY 4

struct ReadAheadThreadData
	{
	char				*SOPs;
	char				*Studies;
	char				*Series;
	char				*ObjectFiles;
	char				*Devices;
	int				*volatile Status;
	char				*pats;
	int				NumToRead;
	DICOMDataObject			**DDOOutPtr;
	Database			*DB;
	int				iVrSizeLimit;
#ifdef WIN32
	HANDLE 				Handle;
#else
  	pthread_t 			Handle;
#endif
	DICOMCommandObject	   	*pDCO;
	char				*Filenames;
	volatile BOOL			Busy;
	volatile BOOL			ForceEnd;
	volatile BOOL			TimeOut;
	char 				*FullFilenames;
	int 				filesmissing;
	int 				numreadahead;
	int 				overlap;
	volatile BOOL 		        reverse;
	};

#ifdef WIN32
#define NULLHANDLE NULL
#else
#define NULLHANDLE 0
#endif

// to process any recieved data during virtual server operation
// call once in a while during virtual reception of data

static int ProcessRATData(struct ReadAheadThreadData *ratd, int maxfiles)
	{
	char		FullFilename[1024];
	ExtendedPDU_Service	PDU;
	int		len, UseChapter10=0, i, k;
	DICOMDataObject	*DDOOut;

        // count files in memory and limit to 2 * OverlapVirtualGet
	k = 0;
        for (i=0; i<ratd->NumToRead; i++)
	  if (ratd->Status[i]==STATUS_READ) ++k;
	if (k>=maxfiles) return 0;

	PDU.AttachRTC(&VRType);

	// find images ready to read from start of list
	for (i=0; i<ratd->NumToRead; i++)
		{
		if (ratd->Status[i]!=STATUS_EMPTY) continue;	// already processed

		ratd->Status[i]=STATUS_BUSY;
		if (*(ratd->FullFilenames+i*1024)==0)		// retry; may have been collected by virtual server
			{
			GetPhysicalDevice(ratd->Devices + i * 32, FullFilename);
			strcat(FullFilename, ratd->ObjectFiles + i * 256);
			if (!DFileExists(FullFilename)) 
			    { ratd->Status[i]=STATUS_EMPTY;	// stop at hole
			      break;
			    }
			}
		else
			strcpy(FullFilename, ratd->FullFilenames + i*1024);

		len = strlen(FullFilename);
		if (len>4)
			UseChapter10 = (stricmp(FullFilename+len-4, ".v2")!=0) || NoDicomCheck;

		if (ratd->ForceEnd)
			{
			ratd->Status[i]=STATUS_EMPTY;
			return k;
			}

		if (*TestMode) strcat(FullFilename, TestMode);

		int t = (int)time(NULL);
		if (!UseChapter10)
		  	DDOOut = LoadImplicitLittleEndianFile(FullFilename, ratd->iVrSizeLimit);
		else	
			DDOOut = PDU.LoadDICOMDataObject(FullFilename);
		LoadTime += (int)time(NULL)-t;

		if (!DDOOut) 
			{
			ratd->Status[i]=STATUS_EMPTY;
			break;
			}

		if (ratd->ForceEnd)
			{
			delete DDOOut;
			ratd->Status[i]=STATUS_EMPTY;
			return k;
			}

		t = (int)time(NULL);
		ProcessDDO(&DDOOut, ratd->pDCO, &PDU);
		ProcessTime += (int)time(NULL)-t;

		strcpy(ratd->Filenames + i*1024, FullFilename);
		ratd->DDOOutPtr[i] = DDOOut;
		ratd->Status[i]    = STATUS_READ;
		SystemDebug.printf("ProcessRATData: overlap read > %04d\n", i);

		if (++k >= maxfiles) break;
		}

	// find images ready to read from end of list (sometimes PACS deliver in reversed order)
	for (i=ratd->NumToRead-1; i>=0; i--)
		{
		if (ratd->Status[i]!=STATUS_EMPTY) continue;

		ratd->Status[i]=STATUS_BUSY;
		if (*(ratd->FullFilenames+i*1024)==0)
			{
			GetPhysicalDevice(ratd->Devices + i * 32, FullFilename);
			strcat(FullFilename, ratd->ObjectFiles + i * 256);
			if (!DFileExists(FullFilename)) 
			    { ratd->Status[i]=STATUS_EMPTY;
			      break;
			    }
			}
		else
			strcpy(FullFilename, ratd->FullFilenames + i*1024);

		len = strlen(FullFilename);
		if (len>4)
			UseChapter10 = (stricmp(FullFilename+len-4, ".v2")!=0) || NoDicomCheck;

		if (ratd->ForceEnd)
			{
			ratd->Status[i]=STATUS_EMPTY;
			return k;
			}

		if (*TestMode) strcat(FullFilename, TestMode);

		int t = (int)time(NULL);
		if (!UseChapter10)
		  	DDOOut = LoadImplicitLittleEndianFile(FullFilename, ratd->iVrSizeLimit);
		else	
			DDOOut = PDU.LoadDICOMDataObject(FullFilename);
		LoadTime += (int)time(NULL)-t;

		if (!DDOOut) 
			{
			ratd->Status[i]=STATUS_EMPTY;
			break;
			}

		if (ratd->ForceEnd)
			{
			delete DDOOut;
			ratd->Status[i]=STATUS_EMPTY;
			return k;
			}

		t = (int)time(NULL);
		ProcessDDO(&DDOOut, ratd->pDCO, &PDU);
		ProcessTime += (int)time(NULL)-t;

		strcpy(ratd->Filenames + i*1024, FullFilename);
		ratd->DDOOutPtr[i] = DDOOut;
		ratd->Status[i]    = STATUS_READ;
		SystemDebug.printf("ProcessRATData: overlap read > %04d\n", i);

		if (++k >= maxfiles) break;
		}

	return k;
	}

// same as VirtualServer but splits virtual query in one request per series
BOOL VirtualServer2(struct ReadAheadThreadData *ratd, int N)
	{
	char AE[48];
	strcpy(AE, VirtualServerFor[N]);
	BOOL PerSeries = VirtualServerPerSeries[N];
	if (*AE==0) return FALSE;

        char *param = strchr(AE, ',');
        if (param) *param++ = 0;

  	unsigned char 		ip[64], port[64], compress[64];
	VR			*vr;
	char			SOP[66];
	DICOMCommandObject	DCO, DCOR2;
	DICOMDataObject		DDO, DDOR2;
	LE_UINT16		command, datasettype, messageid=3, priority;
	UID			uid;
	int			quit, i, j, ims=0;
	unsigned int k;
	ExtendedPDU_Service	PDU2;
	char			*sops, *p;
	int			*seriesnum;
	char			pats[66], series[66], studies[66], szTemp[64], 	szRootSC[64];

	MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile);
  	MyGetPrivateProfileString(szRootSC, "OverlapVirtualGet", "0", szTemp, 64, ConfigFile);
	ratd->overlap = atoi(szTemp);

	OperatorConsole.printf("Virtual server - collecting %d missing objects from %s\n", ratd->filesmissing, AE);

	PDU2.AttachRTC(&VRType);
	if(!GetACRNema(AE, (char *)ip, (char *)port, (char *)compress))
		return FALSE;

	PDU2.ClearAbstractSyntaxs();
	PDU2.SetLocalAddress(MYACRNEMA);
	PDU2.SetRemoteAddress((unsigned char *)AE);

	uid.Set("1.2.840.10008.3.1.1.1");	// Application context (DICOM App)
	PDU2.SetApplicationContext(uid);

	uid.Set("1.2.840.10008.5.1.4.1.2.2.2"); // StudyRootRetrieve (C-Move SOP)
	PDU2.AddAbstractSyntax(uid);

        PDU2.SetTimeOut(TCPIPTimeOut);

	// Make association for MOVE on port/ip
	if(!PDU2.Connect(ip, port))
		{
		PDU2.Close();
		OperatorConsole.printf("*** Virtual server - failed to connect %s (%s %s) for C-MOVE\n", AE, ip, port);
		return(FALSE);
		}

	// collect move requests together if possible (same series)
	sops = (char *)malloc(66*ratd->NumToRead); sops[0]=0;
	seriesnum = (int *)malloc(sizeof(int)*(ratd->NumToRead+1)); memset(seriesnum, 0, sizeof(int)*(ratd->NumToRead+1));

	pats[0]=series[0]=studies[0] = 0;
	j = 1;

	if (ratd->NumToRead)
		{
		strcpy(pats   , ratd->pats   +0*66);
		strcpy(studies, ratd->Studies+0*66);
		strcpy(series , ratd->Series +0*66);
		}
	
	for (i=0; i<ratd->NumToRead; i++)
		{
		if (*(ratd->FullFilenames+i*1024)) continue;	// present (keeps seriesnum to 0)

		p = strchr(ratd->Devices+i*32, '.');
		if (p && atoi(p+1)!=N) continue;		// not on this server (keeps seriesnum to 0)

		if (strcmp(pats,    ratd->pats   +i*66) ||	// next series to collect ?
                    strcmp(studies, ratd->Studies+i*66) ||
                    strcmp(series,  ratd->Series +i*66))
			{
			strcpy(pats   , ratd->pats   +i*66);
			strcpy(studies, ratd->Studies+i*66);
			strcpy(series , ratd->Series +i*66);
			j++;
			}

		seriesnum[i] = j;				// this loop sets it to e.g., 111101220222333330
								// or  00000000000000000000000010000000000000000
		}

	// perform one or more c-move requests (sops = all sops for this move; j = series#, k = number of images collected)
	sops[0]=0; j=1; k=0;
	for (i=0; i<=ratd->NumToRead; i++)
		{
		// image does not need to be to be retrieved (do not continue for the last cycle; always needs to retrieve)
		if (i<ratd->NumToRead && seriesnum[i]==0) 
			continue;

		// image i is from a new series, send series belonging to image i-1
		else if (   (j!=seriesnum[i] && k>0) 
		         || (k>=64)			// limit to 64 images per move
			)
			{
			DCO.Reset();
			DDO.Reset();

			// move missing file to this server
			strcpy((char*) SOP, "1.2.840.10008.5.1.4.1.2.2.2"); // StudyRootMove (C-Move SOP)
			vr = new VR (0x0000, 0x0002, strlen((char*)SOP), (void*) SOP, FALSE);
			DCO.Push(vr);
			command = 0x0021;
			vr = new VR (0x0000, 0x0100, 0x0002, &command, FALSE);
			DCO.Push(vr);
			priority = 0;	// MEDIUM
			vr = new VR (0x0000, 0x0700, 0x0002, &priority, FALSE);
			DCO.Push(vr);
			datasettype = 0x0102;	
			vr = new VR (0x0000, 0x0800, 0x0002, &datasettype, FALSE);
			DCO.Push(vr);	
			messageid++;
			vr = new VR (0x0000, 0x0110, 0x0002, &messageid, FALSE);
			DCO.Push(vr);
			vr = new VR(0x0000, 0x0600, 16, (void*)MYACRNEMA, FALSE);
			DCO.Push(vr);

			// Data object. Query level is image or series; ask for patient ID and SopInstanceUID 
			// use IMAGE level only if PerSeries=0 or #images < PerSeries (e.g., allow max 1000 sops to be sent as IMAGE move)
			if (PerSeries && k>PerSeries) SetStringVR(&vr, 0x0008, 0x0052, "SERIES"); 
			else                          SetStringVR(&vr, 0x0008, 0x0052, "IMAGE"); 
			DDO.Push(vr); 

			// send images collected in sops (or one series) with patient etc from previous image
			// send sops only if PerSeries=0 or #images < PerSeries (e.g., allow max 1000 sops to be sent)
			// 20131103: made it <= instead of > such that check few lines above is exact inverse
			if (!PerSeries || k<=PerSeries)	
				{
				// for safety: it could be that we need to collect 0 images
				if (strlen(sops)>0) sops[strlen(sops)-1]=0;
				SetStringVR(&vr, 0x0008, 0x0018, sops);
				DDO.Push(vr); 
				}
			SetStringVR(&vr, 0x0010, 0x0020, ratd->pats+(i-1)*66); 
			DDO.Push(vr); 
			SetStringVR(&vr, 0x0020, 0x000d, ratd->Studies+(i-1)*66);
			DDO.Push(vr); 
			SetStringVR(&vr, 0x0020, 0x000e, ratd->Series+(i-1)*66);
			DDO.Push(vr); 

			// Here fixkodak fiximage should operate adding 0 to e.g., 9901234 patient ID
			if (param)
				if (strstr(param, "FIXKODAK") || strstr(param, "fixkodak"))
					KodakFixer(&DDO, TRUE);
			CallImportConverterN(NULL, &DDO, 2300, NULL, NULL, NULL, NULL, &PDU2, NULL, NULL);

			// Write command object and data object
			PDU2.Write(&DCO, uid);
			
			if (DebugLevel>3)
				{
				SystemDebug.printf("Move request sent to virtual server %s (VirtualServerPerSeries = %d):\n", AE, PerSeries);
				NonDestructiveDumpDICOMObject(&DDO);
				}

			PDU2.Write(&DDO, uid);

			// Get response and count collected images
			quit = 0;
			while(!quit)
				{
				if(!PDU2.Read(&DCOR2))
					{
					OperatorConsole.printf("*** Virtual server - C-MOVE association lost to %s\n", AE);
					quit=2; // error (flag not yet used)
					}
				if(DCOR2.GetUINT16(0x0000, 0x0100)!=0x8021)
					{
					OperatorConsole.printf("*** Virtual server - C-MOVE got a wrong response from %s\n", AE);
					quit=3; // error (flag not yet used)
					}
				// ignore the data set
				if(DCOR2.GetUINT16(0x0000, 0x0800)!=0x0101)
					{
					PDU2.Read(&DDOR2);
					DDOR2.Reset();
					}
				if(DCOR2.GetUINT16(0x0000, 0x0900)==0x0000)	// success ?
					{
					OperatorConsole.printf("Virtual server - Grabbed %d images for %s from %s\n", ims, pats, AE);
					quit=1;
					}
				else if (DCOR2.GetUINT16(0x0000, 0x0900)==0xff00)	// continuing no problem
					{
					ims++;
					if (ratd->ForceEnd)
						{
						/* Send a CANCEL message... */
						DCO.Reset();
						command = 0x0fff;		// C_CANCEL_MOVE_RQ
						vr = new VR (0x0000, 0x0100, 0x0002, &command, FALSE);
						DCO.Push(vr);
						messageid = 5;			// MsgID of original MOVE_RQ
						vr = new VR (0x0000, 0x0120, 0x0002, &messageid, FALSE);
						DCO.Push(vr);
						datasettype = 0x0101;	// No DataSet is present
						vr = new VR (0x0000, 0x0800, 0x0002, &datasettype, FALSE);
						DCO.Push(vr);
						PDU2.Write(&DCO, uid);
						/* ... and drop the connection. */
						goto EXIT;
						}
					if (ratd->overlap)
						if ((ims%ratd->overlap)==0) 
							ProcessRATData(ratd, ratd->overlap); // parallel process some data
					}
				else
					{
					int RemoteError = DCOR2.GetUINT16(0x0000, 0x0900);
					OperatorConsole.printf("*** Virtual server - remote C-MOVE DICOM Error code %x (%d) from %s\n", RemoteError, RemoteError, AE);
					NonDestructiveDumpDICOMObject(&DCOR2);
					quit=4;	// error (flag not yet used)
					}
				DCOR2.Reset();
				}

			// start a new move for a new series
			sops[0]=0; k=0;
			// rare virtualserver crash 2; a if was needed but now not because I made seriesnum one longer
			j = seriesnum[i];
			}

		// append image sop to series
		if (i<ratd->NumToRead) 
			{
			strcat(sops, ratd->SOPs+i*66);
			strcat(sops, "\\");
			k++;
			}
		}

EXIT:
	free(sops);
	free(seriesnum);
	PDU2.Close();

	ratd->filesmissing -= ims;	// how many are left ?
	return (ratd->filesmissing==0);	// all missing images retrieved? -> OK
	}

static BOOL WINAPI ReadAheadThread(struct ReadAheadThreadData *ratd)
	{
	char		FullFilename[1024];
	char		Filename[1024];
	char		Device[256];
	ExtendedPDU_Service	PDU;
	int		len, UseChapter10=0, i, j, k, ir;
	DICOMDataObject	*DDOOut;

	ratd->filesmissing=0;

	PDU.AttachRTC(&VRType);

	// Check if all files are there and generate their full filenames

	ratd->FullFilenames = (char *)malloc(ratd->NumToRead * 1024);

	for (i=0; i<ratd->NumToRead; i++)
		{
		strcpy(Filename, ratd->ObjectFiles + i * 256);
		strcpy(Device,   ratd->Devices     + i * 32);

		if (!FindPhysicalDevice(Device, ratd->FullFilenames+i*1024, Filename))
			{
			*(ratd->FullFilenames+i*1024)=0;
			ratd->filesmissing++;
			}
		else
			strcat(ratd->FullFilenames+i*1024, Filename);
		}

	// virtual server: grab images in db but not on disk from other server; send them; and optionally delete them

	if (ratd->filesmissing>0 && !ratd->ForceEnd)
		{
		for (int i1=0; i1<10; i1++)
			if (VirtualServer2(ratd, i1)) 
			{ break;
		        }
		}

        // double check that files are indeed missing; may have been miscounted by VirtualServer2 (if too few c-move callbacks)
	if (ratd->filesmissing>0)
		{
		ratd->filesmissing = 0;
		for (int i1=0; i1<ratd->NumToRead; i1++)
			if (*(ratd->FullFilenames+i1*1024)==0)
				{ 
				strcpy(Filename, ratd->ObjectFiles + i1 * 256);
		      		strcpy(Device,   ratd->Devices     + i1 * 32);
			  	if (!FindPhysicalDevice(Device, FullFilename, Filename)) ratd->filesmissing++;
				}
		}

	if (ratd->filesmissing>0)
		{
		OperatorConsole.printf("*** Failed to retrieve some files:\n");
		for (int i1=0; i1<ratd->NumToRead; i1++)
			if (*(ratd->FullFilenames+i1*1024)==0)
				{ 
				strcpy(Filename, ratd->ObjectFiles + i1 * 256);
		      		strcpy(Device,   ratd->Devices     + i1 * 32);

			  	if (!FindPhysicalDevice(Device, ratd->FullFilenames+i1*1024, Filename))
			  		{ 
					OperatorConsole.printf("*** missing file: %s %s\n", Device, Filename);
                			if (strlen(Device)>3 && Device[4]=='.') 
                			  DeleteImage(ratd->SOPs+i1*66, TRUE, 0); // e.g. MAG0.0
			  		}
				else
				  	strcat(ratd->FullFilenames+i1*1024, Filename); // was missing added 20131030
				}
		}

	k=0;
	for (i=0; i<ratd->NumToRead; i++)
		{
		if (ratd->reverse) ir = ratd->NumToRead-1-i;
		else ir = i;
		if (ratd->Status[ir]!=STATUS_EMPTY) continue;
		ratd->Status[ir]=STATUS_BUSY;

		if (*(ratd->FullFilenames+ir*1024)==0)	// retry; may have been collected by virtual server
			{
			strcpy(Filename, ratd->ObjectFiles + ir * 256);
			strcpy(Device,   ratd->Devices     + ir * 32);

			if (!FindPhysicalDevice(Device, FullFilename, Filename))
				{
				OperatorConsole.printf("***Could still not find file: %s\n", Filename);
				ratd->Status[ir]=STATUS_FAILED;
				continue;
				}
/*				{
				OperatorConsole.printf("***Could not find file:%s\n", Filename);
				free(ratd->FullFilenames);

				// mvh 20060401: fix hang condition when a read ahead failed
				ratd->Busy = FALSE;
				ratd->TimeOut = TRUE;

				return FALSE;
				}
*/
			strcat(FullFilename, Filename);
			}
		else
			strcpy(FullFilename, ratd->FullFilenames + ir*1024);

		len = strlen(FullFilename);
		if (len>4)
			UseChapter10 = (stricmp(FullFilename+len-4, ".v2")!=0) || NoDicomCheck;

		if (ratd->ForceEnd) 
		{ ratd->Status[ir]=STATUS_FAILED;
		  goto TimeOut;
		}

		if (*TestMode) strcat(FullFilename, TestMode);

		int t = (int)time(NULL);
		if (!UseChapter10)
		  	DDOOut = LoadImplicitLittleEndianFile(FullFilename, ratd->iVrSizeLimit);
		else	
			DDOOut = PDU.LoadDICOMDataObject(FullFilename);
		LoadTime += (int)time(NULL)-t;

		if (!DDOOut) 
			// mvh 20060401: fix hang condition when a read ahead failed
			{
			OperatorConsole.printf("***Could not read file:%s\n", Filename);
			ratd->Status[ir]=STATUS_FAILED;
			continue;
/*			ratd->Busy = FALSE;
			ratd->TimeOut = TRUE;
			free(ratd->FullFilenames);
			return FALSE;
*/
			}

		if (!CacheVirtualData)
			{
			if (ratd->Status[ir]==STATUS_RETRIEVED)		// if virtual server retrieved data
				unlink(FullFilename);			// delete data if not caching
			}

		if (ratd->ForceEnd)
			{
			delete DDOOut;
			ratd->Status[ir]=STATUS_EMPTY;
			goto TimeOut;
			}

		t = (int)time(NULL);
		ProcessDDO(&DDOOut, ratd->pDCO, &PDU);
		ProcessTime += (int)time(NULL)-t;

		strcpy(ratd->Filenames + ir*1024, FullFilename);
		ratd->DDOOutPtr[ir] = DDOOut;
		ratd->Status[ir] = STATUS_READ;
		SystemDebug.printf("ReadAheadThread: readahead > %04d\n", ir);

		k = 0;
		for (j=0; j<ratd->NumToRead; j++)
		   if (ratd->Status[j]==STATUS_READ) 
		     k++;

		int m=0;
		while (k>ratd->numreadahead+ratd->overlap+2)
			{
			if (ratd->ForceEnd) goto TimeOut;
			//printf(".");
			Sleep(10);
			k = 0;
			for (j=0; j<ratd->NumToRead; j++)
			  if (ratd->Status[j]==STATUS_READ) 
			    k++;
			if (++m>100) 
			  { SystemDebug.printf("ReadAheadThread: warning - resolving deadlock due to erratic incoming order\n");
			    break;
			  }
			}

		// Make the thread wait when it is further than numreadahead slices ahead - self timeout is disabled
		/* j = 0;
		while (ir>=ratd->numreadahead && ratd->DDOOutPtr[i-ratd->numreadahead] && ratd->SOPs[(ir-ratd->numreadahead)*66]!=0) 
			{
			if (ratd->ForceEnd) goto TimeOut;
			Sleep(1);
			//printf(".");
			//if (j++ > 5000) goto TimeOut;
			} */
		}

	ratd->Busy = FALSE;
	free(ratd->FullFilenames);
	return TRUE;

	// Happens when the client hangs up the TCP/IP connection (signalled by ForceEnd)
TimeOut:
	ratd->TimeOut = TRUE;
	ratd->Busy    = FALSE;
	free(ratd->FullFilenames);
	return TRUE;
	}

BOOL
RetrieveOn (
	DICOMDataObject	*DDOMask,
	DICOMDataObject	**DDOOutPtr,
	StandardStorage	**SStorage,
	RunTimeClassStorage		&RTCStorage,
	DICOMCommandObject	   *pDCO  = NULL,
        Array < DICOMDataObject *> *pADDO = NULL,
	void			   *ExtraBytes = NULL)
	{
	VR		*vr1;
	char		FullFilename[1024];
	char		Filename[1024];
	char		Device[256];
	ExtendedPDU_Service	PDU;
	int		len, UseChapter10=0, i, j;
	VR*		pVR;
	unsigned int	iVrSizeLimit;
#ifdef WIN32
	unsigned long	ThreadID;
#endif
	struct 	ReadAheadThreadData *ratd;
	DICOMDataObject *tt;

	PDU.AttachRTC(&VRType);

	ratd = (struct 	ReadAheadThreadData *)ExtraBytes;

	// assume read will fail unless proven otherwise
	*DDOOutPtr = NULL;

	// This line will disable the read ahead thread
	if (!EnableReadAheadThread || NoThread) pADDO = NULL;

	// mvh 20060401: fix hang condition when a read ahead failed
	if (ratd->TimeOut) return FALSE;

	// start read ahead thread when first slice of more than (one:normal, zero:virtual server) is requested
	if (pADDO && pADDO->Get(0)==DDOMask && pADDO->GetSize() > 0)
		{
		VR		*vr;

		iVrSizeLimit = 0xffffffff;
		if (pDCO)
			{
			pVR = pDCO->GetVR(0x9999, 0x0100);
		    	if (pVR)
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
	    			iVrSizeLimit = (unsigned int)(*((unsigned int*)pVR->Data));
#else //Big Endian like Apple power pc
	    			iVrSizeLimit = SwitchEndian((unsigned int)(*((unsigned int*)pVR->Data)));
#endif
		  	}
		
		// wait for thread to end from possible previous move on same association
		while (ratd->Busy) 
			{
			Sleep(10);
			//printf(".");
			}

		// free memory of thread if not already done so
		if (ratd->Devices)
			{
			if (ratd->pDCO) delete ratd->pDCO; ratd->pDCO = NULL;
			for (i=0; i<ratd->NumToRead; i++) 
				if (ratd->DDOOutPtr[i])
					delete ratd->DDOOutPtr[i];
			free(ratd->DDOOutPtr);  ratd->DDOOutPtr   = NULL;
			free(ratd->Filenames);  ratd->Filenames   = NULL;
			free(ratd->ObjectFiles);ratd->ObjectFiles = NULL;
			free(ratd->Devices);    ratd->Devices     = NULL;
			free(ratd->Status);     ratd->Status      = NULL;
			free(ratd->SOPs);       ratd->SOPs        = NULL;
			free(ratd->Series);     ratd->Series      = NULL;
			free(ratd->Studies);    ratd->Studies     = NULL;
			free(ratd->pats);       ratd->pats        = NULL;
			CloseHandle(ratd->Handle); ratd->Handle   = NULLHANDLE;
			}

		// initialize thread data
		ratd->Busy    = TRUE;
		ratd->TimeOut = FALSE;
		if (pDCO)
		  ratd->pDCO  = (DICOMCommandObject *)MakeCopy((DICOMDataObject *)pDCO);

		ratd->numreadahead = EnableReadAheadThread<2 ? 5 : EnableReadAheadThread;
		ratd->overlap = 0;
		ratd->NumToRead = pADDO->GetSize();
		ratd->SOPs   = (char *) malloc(ratd->NumToRead * 66);
		ratd->Series = (char *) malloc(ratd->NumToRead * 66);
		ratd->Studies= (char *) malloc(ratd->NumToRead * 66);
		ratd->pats   = (char *) malloc(ratd->NumToRead * 66);
		ratd->ObjectFiles = (char *) malloc(ratd->NumToRead * 256);
		ratd->Devices     = (char *) malloc(ratd->NumToRead * 32);
		ratd->Status      = (int  *) malloc(ratd->NumToRead * sizeof(int));
		memset(ratd->SOPs,    0, ratd->NumToRead * 66);
		memset(ratd->Series,  0, ratd->NumToRead * 66);
		memset(ratd->Studies, 0, ratd->NumToRead * 66);
		memset(ratd->pats,    0, ratd->NumToRead * 66);
		memset(ratd->ObjectFiles, 0, ratd->NumToRead * 256);
		memset(ratd->Devices,     0, ratd->NumToRead * 32);
		memset((char *)ratd->Status, 0, ratd->NumToRead * sizeof(int));

		for (i=0; i<ratd->NumToRead; i++) 
			{
			tt = pADDO->Get(i);
			vr = tt->GetVR(0x9999, 0x0800);
			if (vr) 
				{ 
				memcpy(ratd->ObjectFiles+i*256, vr->Data, vr->Length);
  			  	if (vr->Length && (ratd->ObjectFiles+i*256)[vr->Length-1]==' ') (ratd->ObjectFiles+i*256)[vr->Length-1] = 0;
				}

			vr = tt->GetVR(0x9999, 0x0801);
			if (vr) 
				{
				memcpy(ratd->Devices+i*32, vr->Data, vr->Length);
  			  	if (vr->Length && (ratd->Devices+i*32)[vr->Length-1]==' ') (ratd->Devices+i*32)[vr->Length-1] = 0;
				}

			vr = tt->GetVR(0x0008, 0x0018);
			if (vr) memcpy(ratd->SOPs+i*66, vr->Data, vr->Length);

			vr = tt->GetVR(0x0020, 0x000d);
			if (vr) memcpy(ratd->Studies+i*66, vr->Data, vr->Length);

			vr = tt->GetVR(0x0020, 0x000e);
			if (vr) memcpy(ratd->Series+i*66, vr->Data, vr->Length);

			vr = tt->GetVR(0x0010, 0x0020);
			if (vr) 
                        	{ 
				memcpy(ratd->pats+i*66, vr->Data, vr->Length);
  			  	if (vr->Length && (ratd->pats+i*66)[vr->Length-1]==' ') (ratd->pats+i*66)[vr->Length-1] = 0;
				}
			}

		ratd->iVrSizeLimit  = iVrSizeLimit;

		ratd->DDOOutPtr  = (DICOMDataObject **) malloc(ratd->NumToRead * sizeof(DICOMDataObject *));
		memset(ratd->DDOOutPtr, 0, ratd->NumToRead * sizeof(DICOMDataObject *));

		ratd->Filenames  = (char *) malloc(ratd->NumToRead * 1024);
		memset(ratd->Filenames, 0, ratd->NumToRead * 1024);
		ratd->reverse=FALSE;

		// start the thread
#ifdef WIN32
		ratd->Handle     = CreateThread(NULL, 2097152, 
                                    (LPTHREAD_START_ROUTINE) ReadAheadThread,
			            ratd, 0, &ThreadID);
#else
		pthread_create(&ratd->Handle, NULL, (void*(*)(void*))ReadAheadThread, (void *)ratd);
		pthread_detach(ratd->Handle);
#endif
		}


	// Get an item from the read ahead thread if it is running
	if (pADDO && ratd->Handle!=NULLHANDLE && !ratd->TimeOut)
		{
		j = 0;
		while (!ratd->TimeOut)
			{
			for (i=0; i<ratd->NumToRead; i++)
				{
				if (!ratd->reverse && (ratd->Status[i]==STATUS_READ || ratd->Status[i]==STATUS_FAILED) && pADDO->Get(i)==DDOMask) 
					{
					*DDOOutPtr = ratd->DDOOutPtr[i];
					strcpy(Filename, ratd->Filenames + i*1024);
					ratd->Status[i] = STATUS_RETRIEVED;
					SystemDebug.printf("RetrieveOn: givenout < %04d\n", i);

					// if it is the last item wait for the thread to end and free the memory
					if (pADDO->Get(ratd->NumToRead-1)==DDOMask) // && i==ratd->NumToRead-1 && ratd->Status[i] == STATUS_RETRIEVED)
						{
						while (1)
							{
							ratd->ForceEnd = TRUE;
#ifdef WIN32
							if (WaitForSingleObject(ratd->Handle, 10) != WAIT_TIMEOUT) break;
							if (j++ > 200*100) break;
#else
							while(ratd->Busy) Sleep(10);
							if (j++ > 200*100) break;
#endif
							}
						if (ratd->pDCO) delete ratd->pDCO; ratd->pDCO = NULL;
						free(ratd->DDOOutPtr); ratd->DDOOutPtr = NULL;
						free(ratd->Filenames); ratd->Filenames = NULL;
						free(ratd->SOPs);      ratd->SOPs      = NULL;
						free(ratd->Series);     ratd->Series      = NULL;
						free(ratd->Studies);    ratd->Studies     = NULL;
						free(ratd->pats);       ratd->pats         = NULL;
						free(ratd->ObjectFiles); ratd->ObjectFiles = NULL;
						free(ratd->Devices);     ratd->Devices     = NULL;
						free(ratd->Status);      ratd->Status      = NULL;
                                                CloseHandle(ratd->Handle); ratd->Handle = NULLHANDLE;
						}

					goto further;
					}
				if ( (ratd->Status[ratd->NumToRead-1-i]==STATUS_READ || ratd->Status[ratd->NumToRead-1-i]==STATUS_FAILED) && pADDO->Get(i)==DDOMask) 
					{
					ratd->reverse = TRUE;
					*DDOOutPtr = ratd->DDOOutPtr[ratd->NumToRead-1-i];
					strcpy(Filename, ratd->Filenames + (ratd->NumToRead-1-i)*1024);
					ratd->Status[ratd->NumToRead-1-i] = STATUS_RETRIEVED;
					SystemDebug.printf("RetrieveOn: givenout < %04d\n", ratd->NumToRead-1-i);

					// if it is the last item wait for the thread to end and free the memory
					if (pADDO->Get(ratd->NumToRead-1)==DDOMask) // && i==ratd->NumToRead-1 && ratd->Status[i] == STATUS_RETRIEVED)
						{
						while (1)
							{
							ratd->ForceEnd = TRUE;
#ifdef WIN32
							if (WaitForSingleObject(ratd->Handle, 10) != WAIT_TIMEOUT) break;
							if (j++ > 200*100) break;
#else
							while(ratd->Busy) Sleep(10);
							if (j++ > 200*100) break;
#endif
							}
						if (ratd->pDCO) delete ratd->pDCO; ratd->pDCO = NULL;
						free(ratd->DDOOutPtr); ratd->DDOOutPtr = NULL;
						free(ratd->Filenames); ratd->Filenames = NULL;
						free(ratd->SOPs);      ratd->SOPs      = NULL;
						free(ratd->Series);     ratd->Series      = NULL;
						free(ratd->Studies);    ratd->Studies     = NULL;
						free(ratd->pats);       ratd->pats         = NULL;
						free(ratd->ObjectFiles); ratd->ObjectFiles = NULL;
						free(ratd->Devices);     ratd->Devices     = NULL;
						free(ratd->Status);      ratd->Status      = NULL;
                                                CloseHandle(ratd->Handle); ratd->Handle = NULLHANDLE;
						}

					goto further;
					}
				}
			Sleep(1);		// 20021215: fixes bug reported by Aaron Cann - 100% load
			} // end of endless while

			// mvh 20060401: fix hang condition when a read ahead failed
			if (ratd->TimeOut)
				{
				if (ratd->Devices)
					{
					if (ratd->pDCO) delete ratd->pDCO; ratd->pDCO = NULL;
					for (i=0; i<ratd->NumToRead; i++) 
						if (ratd->DDOOutPtr[i])
							delete ratd->DDOOutPtr[i];
					free(ratd->DDOOutPtr);  ratd->DDOOutPtr   = NULL;
					free(ratd->Filenames);  ratd->Filenames   = NULL;
					free(ratd->ObjectFiles);ratd->ObjectFiles = NULL;
					free(ratd->Devices);    ratd->Devices     = NULL;
					free(ratd->Status);     ratd->Status      = NULL;
					free(ratd->SOPs);       ratd->SOPs        = NULL;
					free(ratd->Series);     ratd->Series      = NULL;
					free(ratd->Studies);    ratd->Studies     = NULL;
					free(ratd->pats);       ratd->pats        = NULL;
					CloseHandle(ratd->Handle); ratd->Handle   = NULLHANDLE;
					}
				return FALSE;
				}


further:
		if(*DDOOutPtr)
			{
			OperatorConsole.printf("Sending file : %s\n", Filename);	// note subtle difference in message!
			RTCStorage.SetUID(DDOMask->GetVR(0x0008, 0x0016));
			(*SStorage) = &RTCStorage;
			ImagesSent++;

			SystemDebug.printf("Image Loaded from Read Ahead Thread, returning TRUE\n");
			return ( TRUE );
			}		
		// apparently flow could before fall through here in case 
		// of an error condition; added return FALSE 20120908
		return ( FALSE );
		} // end if ratd running


	// use normal read code used when only one slice is requested

	char pat[66];				/* get the patient ID */
        pat[0]=0;
	vr1 = DDOMask->GetVR(0x0010, 0x0020);
	if (vr1) 
        	{ 
		memcpy(pat, vr1->Data, vr1->Length);
                pat[vr1->Length]=0;
  		if (vr1->Length && pat[vr1->Length-1]==' ') pat[vr1->Length-1] = 0;
		}

	char series[66];			/* get the series UID */
        series[0]=0;
	vr1 = DDOMask->GetVR(0x0020, 0x000e);
	if (vr1) 
        	{ 
		memcpy(series, vr1->Data, vr1->Length);
                series[vr1->Length]=0;
  		if (vr1->Length && series[vr1->Length-1]==' ') series[vr1->Length-1] = 0;
		}

	char study[66];			/* get the study UID */
        study[0]=0;
	vr1 = DDOMask->GetVR(0x0020, 0x000d);
	if (vr1) 
        	{ 
		memcpy(study, vr1->Data, vr1->Length);
                study[vr1->Length]=0;
  		if (vr1->Length && study[vr1->Length-1]==' ') study[vr1->Length-1] = 0;
		}

	vr1 = DDOMask->GetVR(0x0008, 0x0018);	/* The SOPInstanceUID */
	if(!vr1)
		return ( FALSE );	// it aint going to happen


	// New code uses cached database, but this connection might have hung up
	// New code: cache a database for as long as the RTCStorage exists

	// if (!RTCStorage.DB.Connected)
	//	RTCStorage.DB.Open ( DataSource, UserName, Password, DataHost );
	// if(!GetFileName(vr1, Filename, Device, /* RTCStorage.DB, */ TRUE))
	//	return ( FALSE );	// not in database ?

	// Therefore use slower old code here: re-opens a database for each slice read
	if(!GetFileName(vr1, Filename, Device, TRUE, pat, study, series))
		return ( FALSE );	// not in database ?

	if (!FindPhysicalDevice(Device, FullFilename, Filename))
		{
		OperatorConsole.printf("***Could not find file:%s\n", Filename);
		return FALSE;
		}
	strcat(FullFilename, Filename);

	// check the filename to determine format

	len = strlen(FullFilename);
	if (len>4)
		UseChapter10 = (stricmp(FullFilename+len-4, ".v2")!=0) || NoDicomCheck;

	if (*TestMode) strcat(FullFilename, TestMode);

	int t = (int)time(NULL);
	if (!UseChapter10)
	{
	  /* Following load-routine works only for 'implicit' (i.e. no type-codes)
	     LittleEndian files.
	     It is able to skip large VRs; only NKI DicomNodes know how to add extra
	     (private) VRs to the DCO, in order to accomplish this.
	  */
	  iVrSizeLimit = 0xffffffff;
	  if (pDCO)
	  { /* Check whether there is a iVrSizeLimitLength */
	    pVR = pDCO->GetVR(0x9999, 0x0100);
	    if (pVR)
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
	    iVrSizeLimit = (unsigned int)(*((unsigned int*)pVR->Data));
#else //Big Endian like Apple power pc
	    iVrSizeLimit = SwitchEndian((unsigned int)(*((unsigned int*)pVR->Data)));
#endif
	  }
	  (*DDOOutPtr) = LoadImplicitLittleEndianFile(FullFilename, iVrSizeLimit);
	  /* Decompression and (in the future) other processing takes place in
	     ProcessDDO (c.f. NKIqrsop.cxx)
	  */
	}
	else	
	  // OR: code in filepdu.cxx (dicom.lib); does not decompress but supports all formats
  	{ (*DDOOutPtr) = PDU.LoadDICOMDataObject(FullFilename);
	  // OR: Code in loadddo.c, this directory; includes decompression, does not support sequences
  	  //	(*DDOOutPtr) = LoadDICOMDataObject(FullFilename);

        }
	LoadTime += time(NULL)-t;

	t= (int)time(NULL);
	ProcessDDO(DDOOutPtr, pDCO, &PDU);
	ProcessTime += time(NULL)-t;

	if(*DDOOutPtr)
		{
		OperatorConsole.printf("Sending file: %s\n", FullFilename);
		ImagesSent++;
		}
	else
		{
		OperatorConsole.printf("***Could not read file: %s\n", FullFilename);
		return ( FALSE );
		}

	RTCStorage.SetUID(DDOMask->GetVR(0x0008, 0x00016));
	(*SStorage) = &RTCStorage;

	SystemDebug.printf("Image Loaded, returning TRUE\n");
	return ( TRUE );
	}

BOOL	MyPatientRootRetrieve	::	RetrieveOn (
	DICOMDataObject	*DDOMask,
	DICOMDataObject	**DDOOutPtr,
	StandardStorage	**SStorage)
	{
	SystemDebug.printf("MyPatientRootRetrieve :: RetrieveOn\n");
	return(::RetrieveOn(DDOMask, DDOOutPtr,SStorage,RTCStorage));
	}

BOOL	MyPatientRootRetrieveNKI	::	RetrieveOn (
	DICOMDataObject	*DDOMask,
	DICOMDataObject	**DDOOutPtr,
	StandardStorage	**SStorage,
	DICOMCommandObject	   *pDCO,
        Array < DICOMDataObject *> *pADDO,
	void		*ExtraBytes)
	{
	SystemDebug.printf("MyPatientRootRetrieveNKI :: RetrieveOn\n");
//	return(::RetrieveOn(DDOMask, DDOOutPtr,SStorage,RTCStorage, GetDCO(), GetADDO(), ExtraBytes));
	return(::RetrieveOn(DDOMask, DDOOutPtr,SStorage,RTCStorage, pDCO, pADDO, ExtraBytes));
	}

BOOL	MyPatientRootRetrieveGeneric	::	RetrieveOn (
	DICOMDataObject	*DDOMask,
	DICOMDataObject	**DDOOutPtr,
	StandardStorage	**SStorage,
	DICOMCommandObject	   *pDCO,
        Array < DICOMDataObject *> *pADDO,
	void		*ExtraBytes)
	{
	SystemDebug.printf("MyPatientRootRetrieveGeneric :: RetrieveOn\n");
//	return(::RetrieveOn(DDOMask, DDOOutPtr,SStorage,RTCStorage, NULL, GetADDO(), ExtraBytes));
	return(::RetrieveOn(DDOMask, DDOOutPtr,SStorage,RTCStorage, pDCO, pADDO, ExtraBytes)); // 20100227: pass DCO: used in %s for RetrieveResultConverter
	}

BOOL	MyPatientRootGetGeneric	::	RetrieveOn (
	DICOMDataObject	*DDOMask,
	DICOMDataObject	**DDOOutPtr,
	StandardStorage	**SStorage,
	DICOMCommandObject	   *pDCO,
        Array < DICOMDataObject *> *pADDO,
	void		*ExtraBytes)
	{
	SystemDebug.printf("MyPatientRootGetGeneric :: RetrieveOn\n");
	return(::RetrieveOn(DDOMask, DDOOutPtr,SStorage,RTCStorage, pDCO, pADDO, ExtraBytes)); // 20100227: pass DCO: used in %s for RetrieveResultConverter
	}

BOOL	MyStudyRootRetrieve	::	RetrieveOn (
	DICOMDataObject	*DDOMask,
	DICOMDataObject	**DDOOutPtr,
	StandardStorage	**SStorage)
	{
	SystemDebug.printf("MyStudyRootRetrieve :: RetrieveOn\n");
	return(::RetrieveOn(DDOMask, DDOOutPtr,SStorage,RTCStorage));
	}

BOOL	MyStudyRootRetrieveNKI	::	RetrieveOn (
	DICOMDataObject	*DDOMask,
	DICOMDataObject	**DDOOutPtr,
	StandardStorage	**SStorage,
	DICOMCommandObject	   *pDCO,
        Array < DICOMDataObject *> *pADDO,
	void		*ExtraBytes)
	{
	SystemDebug.printf("MyStudyRootRetrieveNKI :: RetrieveOn\n");
//	return(::RetrieveOn(DDOMask, DDOOutPtr,SStorage,RTCStorage, GetDCO(), GetADDO(), ExtraBytes));
	return(::RetrieveOn(DDOMask, DDOOutPtr,SStorage,RTCStorage, pDCO, pADDO, ExtraBytes));
	}

BOOL	MyStudyRootRetrieveGeneric	::	RetrieveOn (
	DICOMDataObject	*DDOMask,
	DICOMDataObject	**DDOOutPtr,
	StandardStorage	**SStorage,
	DICOMCommandObject	   *pDCO,
        Array < DICOMDataObject *> *pADDO,
	void		*ExtraBytes)
	{
	SystemDebug.printf("MyStudyRootRetrieveGeneric :: RetrieveOn\n");
	return(::RetrieveOn(DDOMask, DDOOutPtr,SStorage,RTCStorage, pDCO, pADDO, ExtraBytes)); // 20100227: pass DCO: used in %s for RetrieveResultConverter
	}

BOOL	MyStudyRootGetGeneric	::	RetrieveOn (
	DICOMDataObject	*DDOMask,
	DICOMDataObject	**DDOOutPtr,
	StandardStorage	**SStorage,
	DICOMCommandObject	   *pDCO,
        Array < DICOMDataObject *> *pADDO,
	void		*ExtraBytes)
	{
	SystemDebug.printf("MyStudyRootGetGeneric :: RetrieveOn\n");
	return(::RetrieveOn(DDOMask, DDOOutPtr,SStorage,RTCStorage, pDCO, pADDO, ExtraBytes)); // 20100227: pass DCO: used in %s for RetrieveResultConverter
	}

BOOL	MyPatientStudyOnlyRetrieve	::	RetrieveOn (
	DICOMDataObject	*DDOMask,
	DICOMDataObject	**DDOOutPtr,
	StandardStorage	**SStorage)
	{
	SystemDebug.printf("MyPatientStudyOnlyRetrieve :: RetrieveOn\n");
	return(::RetrieveOn(DDOMask, DDOOutPtr,SStorage,RTCStorage));
	}

BOOL	MyPatientStudyOnlyRetrieveNKI	::	RetrieveOn (
	DICOMDataObject	*DDOMask,
	DICOMDataObject	**DDOOutPtr,
	StandardStorage	**SStorage,
	DICOMCommandObject	   *pDCO,
        Array < DICOMDataObject *> *pADDO,
	void		*ExtraBytes)
	{
	SystemDebug.printf("MyPatientStudyOnlyRetrieveNKI :: RetrieveOn\n");
	return(::RetrieveOn(DDOMask, DDOOutPtr,SStorage,RTCStorage, pDCO, pADDO, ExtraBytes));
	}

BOOL	MyPatientStudyOnlyRetrieveGeneric	::	RetrieveOn (
	DICOMDataObject	*DDOMask,
	DICOMDataObject	**DDOOutPtr,
	StandardStorage	**SStorage,
	DICOMCommandObject	   *pDCO,
        Array < DICOMDataObject *> *pADDO,
	void		*ExtraBytes)
	{
	SystemDebug.printf("MyPatientStudyOnlyRetrieveGeneric :: RetrieveOn\n");
	return(::RetrieveOn(DDOMask, DDOOutPtr,SStorage,RTCStorage, pDCO, pADDO, ExtraBytes)); // 20100227: pass DCO: used in %s for RetrieveResultConverter
	}

BOOL	MyPatientStudyOnlyGetGeneric	::	RetrieveOn (
	DICOMDataObject	*DDOMask,
	DICOMDataObject	**DDOOutPtr,
	StandardStorage	**SStorage,
	DICOMCommandObject	   *pDCO,
        Array < DICOMDataObject *> *pADDO,
	void		*ExtraBytes)
	{
	SystemDebug.printf("MyPatientStudyOnlyGetGeneric :: RetrieveOn\n");
	return(::RetrieveOn(DDOMask, DDOOutPtr,SStorage,RTCStorage, pDCO, pADDO, ExtraBytes)); // 20100227: pass DCO: used in %s for RetrieveResultConverter
	}

BOOL
QualifyOn(
	BYTE	*ACRNema,
	BYTE	*MyACRNema,
	BYTE	*RemoteIP,
	BYTE	*RemotePort,
	BYTE	*Compress)
	{
	strcpy((char*)MyACRNema, (char*)MYACRNEMA);
	if(!GetACRNema((char*)ACRNema, (char*)RemoteIP, (char*)RemotePort, (char*)Compress))
		{
		return FALSE;	// use "*	*	104	un" in acrnema.map to emulate this

		// This is a non-standard "feature".  Perhaps the ACR-Nema
		// is reversable via DNS.  You should return FALSE here
		// if you don't want this.
		/* strcpy((char*)RemoteIP, (char*) ACRNema);
		strcpy((char*)RemotePort, "104");
		SystemDebug.printf("(QualifyOn) (anonymous) IP:%s, PORT:%s\n", RemoteIP,RemotePort);
		return ( TRUE );
		*/
		}

	/* GetACRNema is able to discern upper and lower case, so that when two different
           stations have the same AE, we can still map them to different IP addresses...
	   However, when associating to a 'lower case AE station', it may very well be that
	   that station wants an upper case 'Called AE title'.
	*/

	char szRootSC[64], Temp[64];
  	if (MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile))
		if (MyGetPrivateProfileString(szRootSC, "SendUpperCaseAE", "0", Temp, 64, ConfigFile))
			if (atoi(Temp)) strupr((char*)ACRNema);

	SystemDebug.printf("(QualifyOn) (mapped) IP:%s, PORT:%s\n", RemoteIP,RemotePort);
	return ( TRUE );
	}

BOOL	MyPatientRootRetrieve	::	QualifyOn (
	BYTE	*ACRNema,
	BYTE	*MyACRNema,
	BYTE	*RemoteIP,
	BYTE	*RemotePort)
	{
	BYTE dummy[64];
	return(::QualifyOn(ACRNema, MyACRNema, RemoteIP, RemotePort, dummy));
	}

BOOL	MyPatientRootRetrieveNKI	::	QualifyOn (
	BYTE	*ACRNema,
	BYTE	*MyACRNema,
	BYTE	*RemoteIP,
	BYTE	*RemotePort,
	BYTE	*Compress)
	{
	return(::QualifyOn(ACRNema, MyACRNema, RemoteIP, RemotePort, Compress));
	}

BOOL	MyPatientRootRetrieveGeneric	::	QualifyOn (
	BYTE	*ACRNema,
	BYTE	*MyACRNema,
	BYTE	*RemoteIP,
	BYTE	*RemotePort,
	BYTE	*Compress)
	{
	return(::QualifyOn(ACRNema, MyACRNema, RemoteIP, RemotePort, Compress));
	}

BOOL	MyPatientRootGetGeneric	::	QualifyOn (
	BYTE	*ACRNema,
	BYTE	*MyACRNema,
	BYTE	*RemoteIP,
	BYTE	*RemotePort,
	BYTE	*Compress)
	{
	return(::QualifyOn(ACRNema, MyACRNema, RemoteIP, RemotePort, Compress));
	}

BOOL	MyStudyRootRetrieve	::	QualifyOn (
	BYTE	*ACRNema,
	BYTE	*MyACRNema,
	BYTE	*RemoteIP,
	BYTE	*RemotePort)
	{
	BYTE dummy[64];
	return(::QualifyOn(ACRNema, MyACRNema, RemoteIP, RemotePort, dummy));
	}

BOOL	MyStudyRootRetrieveNKI	::	QualifyOn (
	BYTE	*ACRNema,
	BYTE	*MyACRNema,
	BYTE	*RemoteIP,
	BYTE	*RemotePort,
	BYTE	*Compress)
	{
	return(::QualifyOn(ACRNema, MyACRNema, RemoteIP, RemotePort, Compress));
	}

BOOL	MyStudyRootRetrieveGeneric	::	QualifyOn (
	BYTE	*ACRNema,
	BYTE	*MyACRNema,
	BYTE	*RemoteIP,
	BYTE	*RemotePort,
	BYTE	*Compress)
	{
	return(::QualifyOn(ACRNema, MyACRNema, RemoteIP, RemotePort, Compress));
	}

BOOL	MyStudyRootGetGeneric	::	QualifyOn (
	BYTE	*ACRNema,
	BYTE	*MyACRNema,
	BYTE	*RemoteIP,
	BYTE	*RemotePort,
	BYTE	*Compress)
	{
	return(::QualifyOn(ACRNema, MyACRNema, RemoteIP, RemotePort, Compress));
	}

BOOL	MyPatientStudyOnlyRetrieve	::	QualifyOn (
	BYTE	*ACRNema,
	BYTE	*MyACRNema,
	BYTE	*RemoteIP,
	BYTE	*RemotePort)
	{
	BYTE dummy[64];
	return(::QualifyOn(ACRNema, MyACRNema, RemoteIP, RemotePort, dummy));
	}

BOOL	MyPatientStudyOnlyRetrieveNKI	::	QualifyOn (
	BYTE	*ACRNema,
	BYTE	*MyACRNema,
	BYTE	*RemoteIP,
	BYTE	*RemotePort,
	BYTE	*Compress)
	{
	return(::QualifyOn(ACRNema, MyACRNema, RemoteIP, RemotePort, Compress));
	}

BOOL	MyPatientStudyOnlyRetrieveGeneric	::	QualifyOn (
	BYTE	*ACRNema,
	BYTE	*MyACRNema,
	BYTE	*RemoteIP,
	BYTE	*RemotePort,
	BYTE	*Compress)
	{
	return(::QualifyOn(ACRNema, MyACRNema, RemoteIP, RemotePort, Compress));
	}

BOOL	MyPatientStudyOnlyGetGeneric	::	QualifyOn (
	BYTE	*ACRNema,
	BYTE	*MyACRNema,
	BYTE	*RemoteIP,
	BYTE	*RemotePort,
	BYTE	*Compress)
	{
	return(::QualifyOn(ACRNema, MyACRNema, RemoteIP, RemotePort, Compress));
	}

BOOL	RunTimeClassStorage :: GetUID ( UID	&uid )
	{
	uid = MyUID;
	return ( TRUE );
	}

BOOL	RunTimeClassStorage :: SetUID ( UID	&uid )
	{
	MyUID = uid;
	return ( TRUE );
	}

BOOL	RunTimeClassStorage :: SetUID ( VR	*vr )
	{
	char	s [ 66 ];

	memset((void*)s, 0, 66);
	if(vr)
		{
		memcpy((void*)s, vr->Data, vr->Length>64 ? 64 : vr->Length);
		MyUID.Set(s);
		return(TRUE);
		}
	return ( FALSE );
	}

BOOL	RunTimeClassStorage :: SetUID ( DICOMDataObject *DDOPtr)
	{
	return(SetUID (DDOPtr->GetVR(0x0008, 0x0016)));
	}

#ifndef UNIX
BOOL	WINAPI DriverHelper ( DriverApp	* );
#else
ThreadRoutineType	DriverHelper ( ThreadRoutineArgType	);
#endif

BOOL	DriverApp	::	Server ( BYTE	*port )
	{
	Socket		MSocket;
#ifndef UNIX
	DWORD		ThreadID;
	HANDLE		ThreadHandle;
#else
	pthread_t 	ThreadID;

	// NOTE: this Database is not used BUT, if you don't create/open the
	// Database in the main thread, you won't be able to in child threads...

	Database aDB;
	if(!aDB.Open(DataSource, UserName, Password, DataHost))
		{
		OperatorConsole.printf("***Unable to open database %s as user %s on %s\n",
			DataSource, UserName, DataHost);
		return FALSE;
		}
#endif
	MSocket.SetTimeOut(TCPIPTimeOut);

	if (!MSocket.Listen ((char*) port ))
		{
		OperatorConsole.printf("***Failed to Listen () - bind error\n");
		return ( FALSE );	// failed on the bind
		}

	while (MSocket.Accept() )
		{

		while ( Lock )
			Sleep(50);
		Lock = 1;

		ConnectedIP = MSocket.sa.sin_addr.s_addr;
		ChildSocketfd= MSocket.Socketfd;

		if (NoThread)
			DriverHelper(this);
		else
			{
#ifndef UNIX
			if(!(ThreadHandle = CreateThread(NULL, 2097152, (LPTHREAD_START_ROUTINE) DriverHelper,
					this, 0, &ThreadID)))
#else	// UNIX
			pthread_create(&ThreadID, NULL, DriverHelper, (void *)this);
			pthread_detach(ThreadID);
			if (!ThreadID)
#endif
				{
				OperatorConsole.printf("***Failed to create thread for child\n");
				closesocket ( MSocket.Socketfd );
				}
			else 
				CloseHandle(ThreadHandle);
			}

		MSocket.Socketfd = 0;
		MSocket.Connected = FALSE;

#ifdef DO_LEAK_DETECTION
		if (kbhit())
		{	/* Only in debug-mode */
			Sleep(10000);	// Hope that other threads are finished by now
			/* Cleanup */
			CloseACRNemaAddressArray();
			free(PatientDB);
			free(StudyDB);
			free(SeriesDB);
			free(ImageDB);
			FreeDeviceTables();
			break;
		}
#endif
		}
#ifdef	DEBUG_MODE
		OperatorConsole.printf("***Error in Accept() function call\n");
		SystemDebug.printf("***Attemping to re-bind socket\n");
#endif
	return ( FALSE );
	}

BOOL	DriverApp	::	ServerChildThread ( int	Socketfd, unsigned int ConnectedIP )
	{
	//ExitThread ( (int) ServerChild ( Socketfd ) );
	ServerChild( Socketfd, ConnectedIP );
	return ( FALSE );
	}

// Technically this routine is not needed.  However, Microsoft Visual C++
// has some odd quarks to it.  This work around maintains MT safe.

#ifndef UNIX
BOOL	WINAPI DriverHelper (DriverApp	*App)
	{
#else
ThreadRoutineType DriverHelper(void *theApp)
	{
	DriverApp *App = (DriverApp *)theApp;
#endif
	int	Socketfd;
	unsigned int	ConnectedIP;

	// pick up data from passed-in object

	Socketfd = App->ChildSocketfd;
	ConnectedIP = App->ConnectedIP;

	// release lock on data

	App->Lock = 0;

	App->ServerChildThread ( Socketfd, ConnectedIP );
	// we never technically get here
	// LJ: in M$VC-debug, we actually do get here, and a crash follows (at least when
	// ODBC is used)
//	ExitThread(0x00);
	return ( THREAD_RETURN_FALSE );
	}

// Log some user information

void LogUser(const char* pszOperation, AAssociateRQ* pARQ, DICOMCommandObject* pDCO)
	{
	UserLog.printf("\"%s\",\"%16.16s\"\n", pszOperation, pARQ->CallingApTitle);
	UNUSED_ARGUMENT(pDCO);
//	NonDestructiveDumpDICOMObject(pDCO); object does not exist anymore
	}

// Helps when looking at crash dump

void WipeStack(void)
	{ 
	char	Dummy[512];
  	memset(Dummy, '?', sizeof(Dummy));
	}

#ifdef __BORLANDC__
extern "C" int         _RTLENTRY _EXPFUNC _heapchk    (void);
#endif

// Storage that does not require a database

void StorageApp	:: FailSafeStorage(CheckedPDU_Service *PDU)
{ DICOMDataObject *DDO;
  DICOMCommandObject DCO;
  char Filename[1024];
  char Physical[1024];
  int i;

  // defined here to make thread safe: multiple instances of SOPUnknownStorage.DB required
  MyUnknownStorage SOPUnknownStorage;

  DDO = new DICOMDataObject;

  while(TRUE)
  { DCO.Reset();
    if (!PDU->Read(&DCO))
	break;

    SOPUnknownStorage.DB = NULL;	// no processing
    SOPUnknownStorage.nopreget = FALSE;
    if(!SOPUnknownStorage.Read(PDU, &DCO, DDO)) break;

    GetPhysicalDevice("MAG0", Physical);
    sprintf(Filename, "%sfailsafe_storage", Physical);
    mkdir(Filename);
    i = strlen(Filename);
    Filename[i]   = PATHSEPCHAR;
    Filename[i+1] = 0;
    GenUID(Filename + strlen(Filename));
    strcat(Filename, ".dcm");
    PDU->SaveDICOMDataObject(Filename, DICOM_CHAPTER_10_IMPLICIT, DDO);

    OperatorConsole.printf("***Failsafe storage saved incoming file as: %s\n", Filename);
  }

  delete DDO;
}

void TestCompress(char *filename, const char *modes, ExtendedPDU_Service *PDU)
{ DICOMDataObject 	*pDDO;
  int			syntax;//, len;
  unsigned int		i;
  //PDU_Service		PDU;
//  VR			*vrSOPInstanceUID;
  Database		DB;
  char			rFilename[1024];
  char 			mode[3];
  char			SOP[66];
  VR			*vr;

  //PDU.AttachRTC(&VRType);

  if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
	{
	OperatorConsole.printf("***Error Connecting to SQL\n");
	return;
	}

  pDDO = LoadForGUI(filename);
  if(!pDDO)
	{
	OperatorConsole.printf("***[TestCompress] %s -FAILED: Error on Load\n", filename);
	return;
	}

  for(syntax=3; syntax<=4; syntax++)
  for(i=0; i<strlen(modes); i+=2)
  { memcpy(mode, modes+i, 2);
    mode[2]=0;
    Sleep(1);

    pDDO = LoadForGUI(filename);
    strcpy(rFilename, "org");

    GenUID(SOP); vr = new VR(0x0008, 0x0018, (strlen(SOP)+1)&0xfe, (void*)SOP, (BOOL) FALSE );
    pDDO->ReplaceVR(vr); delete vr;
    vr = new VR(0x0020, 0x0013, strlen(rFilename), (void*)rFilename, (BOOL) FALSE );
    pDDO->ReplaceVR(vr); delete vr;
    //SaveToDisk(DB, pDDO, rFilename, TRUE, (unsigned char *)"testcompress", (unsigned char *)"testcompress", syntax);
    SaveToDisk(&DB, NULL, pDDO, rFilename, TRUE, PDU, syntax);
    pDDO = PDU->LoadDICOMDataObject(rFilename);
    OperatorConsole.printf("Added file: %s (filename syntax %d)\n", rFilename, syntax);

    // recompress dropped file
    recompress(&pDDO, mode, "", mode[0]=='n' || mode[0]=='N', PDU); strcpy(rFilename, mode);
    GenUID(SOP); vr = new VR(0x0008, 0x0018, (strlen(SOP)+1)&0xfe, (void*)SOP, (BOOL) FALSE );
    pDDO->ReplaceVR(vr); delete vr;
    vr = new VR(0x0020, 0x0013, strlen(rFilename), (void*)rFilename, (BOOL) FALSE );
    pDDO->ReplaceVR(vr); delete vr;
    //SaveToDisk(DB, pDDO, rFilename, TRUE, (unsigned char *)"testcompress", (unsigned char *)"testcompress", syntax);
    SaveToDisk(&DB, NULL, pDDO, rFilename, TRUE, PDU, syntax);
    pDDO = PDU->LoadDICOMDataObject(rFilename);
    OperatorConsole.printf("Added file: %s (compression %s, syntax %d)\n", rFilename, mode, syntax);

    // uncompress dropped file
    recompress(&pDDO, "un", "", FALSE, PDU);   strcpy(rFilename, mode); strcat(rFilename, "_un");
    GenUID(SOP); vr = new VR(0x0008, 0x0018, (strlen(SOP)+1)&0xfe, (void*)SOP, (BOOL) FALSE );
    pDDO->ReplaceVR(vr); delete vr;
    vr = new VR(0x0020, 0x0013, strlen(rFilename), (void*)rFilename, (BOOL) FALSE );
    pDDO->ReplaceVR(vr);  delete vr;
    //SaveToDisk(DB, pDDO, rFilename, TRUE, (unsigned char *)"testcompress", (unsigned char *)"testcompress", syntax);
    SaveToDisk(&DB, NULL, pDDO, rFilename, TRUE, PDU, syntax);
    pDDO = PDU->LoadDICOMDataObject(rFilename);
    OperatorConsole.printf("Added file: %s (compression %s_un, syntax %d)\n", rFilename, mode, syntax);

    // recompress dropped file again
    recompress(&pDDO, mode, "", mode[0]=='n' || mode[0]=='N', PDU); strcpy(rFilename, mode); strcat(rFilename, "_un_"); strcat(rFilename, mode);
    GenUID(SOP); vr = new VR(0x0008, 0x0018, (strlen(SOP)+1)&0xfe, (void*)SOP, (BOOL) FALSE );
    pDDO->ReplaceVR(vr); delete vr;
    vr = new VR(0x0020, 0x0013, strlen(rFilename), (void*)rFilename, (BOOL) FALSE );
    pDDO->ReplaceVR(vr); delete vr;
    //SaveToDisk(DB, pDDO, rFilename, TRUE, (unsigned char *)"testcompress", (unsigned char *)"testcompress", syntax);
    SaveToDisk(&DB, NULL, pDDO, rFilename, TRUE, PDU, syntax);
    pDDO = PDU->LoadDICOMDataObject(rFilename);
    OperatorConsole.printf("Added file: %s (compression %s_un_%s, syntax %d)\n", rFilename, mode, mode, syntax);
  }

  delete pDDO;
}

void TestForward(char *filename, const char *mode, char *server, ExtendedPDU_Service *PDU)
{ char host[64], port[64], compress[64];
  char *p;
  RunTimeClassStorage	RTCStorage;
  ExtendedPDU_Service	PDU2;
  DICOMDataObject*	pDDO;
  UID			AppUID ("1.2.840.10008.3.1.1.1");
  UID			iUID;
  VR			*vr;
  char			SOP[66], temp[20];
  BOOL			StripGroup2;

  PDU2.AttachRTC(&VRType);

  if(!GetACRNema(server, (char *)host, (char *)port, (char *)compress))
  { strcpy(host, server);
    p = strchr(host, ':');
    if (p) 
    { *p=0;
      strcpy(port, p+1);
    }
    else 
      strcpy(port, "5678");
  }

  PDU2.SetRequestedCompressionType("");	// default

  pDDO = LoadForGUI(filename);
  if (!pDDO)
  { OperatorConsole.printf("*** Test forward: failed to load DICOM object %s\n", filename);
    return;
  }

  // offer transfer syntax from forward string
  if (mode)
    PDU2.SetRequestedCompressionType(mode);
  else
  // offer transfer syntax from acrnema.map
    PDU2.SetRequestedCompressionType(compress);

  PDU2.SetApplicationContext ( AppUID );
  PDU2.SetLocalAddress ( MYACRNEMA );
  PDU2.SetRemoteAddress ( (unsigned char *)server );

  vr = pDDO -> GetVR(0x0008, 0x0016);
  if (!vr)
  { OperatorConsole.printf("*** Test forward: Forward failed because SopClass is missing in %s\n", filename);
    delete pDDO;
    return;
  }
  SetUID ( iUID, vr );
  PDU2.AddAbstractSyntax ( iUID );
  PDU2.SetTimeOut(TCPIPTimeOut);

  if (!PDU2.Connect((unsigned char *)&host, (unsigned char *)&port))
  { OperatorConsole.printf("*** Test forward: Forward failed to connect to host %s\n", server);
    delete pDDO;
    return;
  }

  if (!PDU2.IsAbstractSyntaxAccepted(iUID))
  { OperatorConsole.printf("*** Test forward: DICOM server %s does not accept type of forwarded images\n", server);
    PDU2.Close();
    delete pDDO;
    return;
  }

  OperatorConsole.printf("Test forward: requested %s, accepted %s, sending to %s = %s:%s\n", mode, PDU2.GetAcceptedCompressionType(iUID), server, host, port);

  GenUID(SOP); vr = new VR(0x0008, 0x0018, (strlen(SOP)+1)&0xfe, (void*)SOP, (BOOL) FALSE );
  pDDO->ReplaceVR(vr); delete vr;
  sprintf(temp, "%s %s", mode, PDU2.GetAcceptedCompressionType(iUID)); 
  vr = new VR(0x0020, 0x0013, strlen(temp), (void*)temp, (BOOL) FALSE );
  pDDO->ReplaceVR(vr);  delete vr;

  // recompress data to be forwarded here according to accepted compression mode
  p = PDU2.GetAcceptedCompressionType(iUID);
  StripGroup2 = memicmp(p, "as", 2)!=0 && memicmp(p, "is", 2)!=0;
  recompress(&pDDO, p, "", StripGroup2, PDU);

  RTCStorage.SetUID(iUID);

  if (!RTCStorage.Write(&PDU2, pDDO))
  { OperatorConsole.printf("*** Test forward: Forward failed to send DICOM image to %s\n", server);
    PDU2.Close();
    delete pDDO;
    return;
  }

  delete pDDO;
  PDU2.Close();
}

void TestSyntax(char *filename, int syntax, ExtendedPDU_Service *PDU)
{ DICOMDataObject*	pDDO;
  VR			*vr;
  char			SOP[66], temp[20];
  Database		DB;
  char			rFilename[1024];

  if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
	{
	OperatorConsole.printf("***Error Connecting to SQL\n");
	return;
	}

  pDDO = LoadForGUI(filename);
  if (!pDDO)
  { OperatorConsole.printf("*** Test syntax: failed to load DICOM object %s\n", filename);
    return;
  }

  GenUID(SOP); vr = new VR(0x0008, 0x0018, (strlen(SOP)+1)&0xfe, (void*)SOP, (BOOL) FALSE );
  pDDO->ReplaceVR(vr);  delete vr;
  sprintf(temp, "syntax %d", syntax); 
  vr = new VR(0x0020, 0x0013, strlen(temp), (void*)temp, (BOOL) FALSE );
  pDDO->ReplaceVR(vr);  delete vr;

  //SaveToDisk(DB, pDDO, rFilename, TRUE, (unsigned char *)"testsyntax", (unsigned char *)"testsyntax", syntax);
  SaveToDisk(&DB, NULL, pDDO, rFilename, TRUE, PDU, syntax);
}

static BOOL WINAPI testsavethread(char *filename)
{ ExtendedPDU_Service PDU;  // for script context (test only)
  PDU.SetLocalAddress ( (BYTE *)"testsavethread" );
  PDU.SetRemoteAddress ( (BYTE *)"testsavethread" );
  TestSyntax(filename, 0, &PDU);
  return TRUE;
}

void TestThreadedSave(char *filename)
{ 
#ifdef WIN32
  unsigned long ThreadID;
  CreateThread(NULL, 2097152, (LPTHREAD_START_ROUTINE) testsavethread, filename, 0, &ThreadID);
#else
  pthread_t ThreadID;
  pthread_create(&ThreadID, NULL, (void*(*)(void*))testsavethread, (void *)filename);
  pthread_detach(ThreadID);
#endif
}

///////////////////////////// elementary printer server support //////////////////////////

// basic film session: is created, deleted, set and action

class CFSRQ : public NCreateRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.1"); return TRUE; }
	};

class CFSRSP : public NCreateRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.1"); return TRUE; }
	};

class DFSRQ : public NDeleteRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.1"); return TRUE; }
	};

class DFSRSP : public NDeleteRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.1"); return TRUE; }
	};

class SFSRQ : public NSetRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.1"); return TRUE; }
	};

class SFSRSP : public NSetRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.1"); return TRUE; }
	};

class AFSRQ : public NActionRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.1"); return TRUE; }
	};

class AFSRSP : public NActionRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.1"); return TRUE; }
	};

// basic film box: is created, set, action(printed) and deleted

class CFBRQ : public NCreateRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.2"); return TRUE; }
	};

class CFBRSP : public NCreateRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.2"); return TRUE; }
	};

class DFBRQ : public NDeleteRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.2"); return TRUE; }
	};

class DFBRSP : public NDeleteRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.2"); return TRUE; }
	};

class AFBRQ : public NActionRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.2"); return TRUE; }
	};

class AFBRSP : public NActionRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.2"); return TRUE; }
	};

class SFBRQ : public NSetRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.2"); return TRUE; }
	};

class SFBRSP : public NSetRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.2"); return TRUE; }
	};

// Basic GrayScale ImageBox should only be set

class SBGIBRQ : public NSetRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.4"); return TRUE; }
	};

class SBGIBRRSP : public NSetRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.4"); return TRUE; }
	};

// Basic Color ImageBox should only be set

class SBCIBRQ : public NSetRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.4.1"); return TRUE; }
	};

class SBCIBRRSP : public NSetRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.4.1"); return TRUE; }
	};

// Printer

class GPRQ : public NGetRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.16"); return TRUE; }
	};

class GPRSP : public NGetRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.16"); return TRUE; }
	};

class EPRQ : public NEventReportRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.16"); return TRUE; }
	};

class EPRSP : public NEventReportRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.16"); return TRUE; }
	};

/* OPTIONAL sop classes

// Basic Annotation Box should only be set

class SBABRQ : public NSetRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.15"); return TRUE; }
	} SetBasicAnnotationBoxRequest;

class SBABRRSP : public NSetRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.15"); return TRUE; }
	} SetBasicAnnotationBoxResponse;

// Print job 

class GPJRQ : public NGetRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.14"); return TRUE; }
	} GetPrintJobRequest;

class GPJRSP : public NGetRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.14"); return TRUE; }
	} GetPrintJobResponse;

class EPJRQ : public NEventReportRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.14"); return TRUE; }
	} EventReportPrintJobRequest;

class EPJRSP : public NEventReportRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.14"); return TRUE; }
	} EventReportPrintJobResponse;

// Presentation LUT: is created and deleted

class CPLRQ : public NCreateRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.23"); return TRUE; }
	} CreatePresentationLUTRequest;

class CPLRSP : public NCreateRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.23"); return TRUE; }
	} CreatePresentationLUTResponse;

class DPLRQ : public NDeleteRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.23"); return TRUE; }
	} DeletePresentationLUTRequest;

class DPLRSP : public NDeleteRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.23"); return TRUE; }
	} DeletePresentationLUTResponse;

// Printer configuration: get only

class GPCRQ : public NGetRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.16.376"); return TRUE; }
	} GetPrinterConfigurationRequest;

class GPCRSP : public NGetRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.5.1.1.16.376"); return TRUE; }
	} GetPrinterConfigurationResponse;

*/

static int pcount=0;

void lua_printserver(ExtendedPDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *DDO, char *lua, char *cmd)
	{
	if (lua[0])
		{
		lua_setvar(PDU, "version",      DGATE_VERSION);
		lua_setvar(PDU, "command_line", cmd);
		struct scriptdata sd = {PDU, DCO, DDO, -1, NULL, NULL, NULL, NULL, NULL, 0, 0};
		do_lua(&(PDU->L), lua, &sd);
		}
	}
	

BOOL StorageApp ::	PrinterSupport( ExtendedPDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject *PrintData[], int Thread, char *lua)
	{
	DICOMDataObject		*DDO;
	char			uid[65], Filename[1024];
	UID			Uid;
	VR			*vr, *vrs;

	UID			aBasicGrayscalePrintManagementMetaSOPClass("1.2.840.10008.5.1.1.9");
	UID			aBasicColorPrintManagementMetaSOPClass("1.2.840.10008.5.1.1.18");

	UID			aBasicFilmSessionSOPClassUID("1.2.840.10008.5.1.1.1");
	UID			aBasicFilmBoxSOPClassUID("1.2.840.10008.5.1.1.2");
	UID			aBasicGrayscaleImageBoxSOPClassUID("1.2.840.10008.5.1.1.4");
	UID			aBasicColorImageBoxSOPClassUID("1.2.840.10008.5.1.1.4.1");
	UID			aBasicPrinterSOPClassUID("1.2.840.10008.5.1.1.16");
	UID			aAppuid("1.2.840.10008.3.1.1.1");
	UID			aPrinter("1.2.840.10008.5.1.1.17");
	int			i;
	char			szPrintSOP[65];
	bool			rc;


CFSRQ	CreateBasicFilmSessionRequest;
CFSRSP	CreateBasicFilmSessionResponse;
DFSRQ	DeleteBasicFilmSessionRequest;
DFSRSP	DeleteBasicFilmSessionResponse;
SFSRQ	SetBasicFilmSessionRequest;
SFSRSP	SetBasicFilmSessionResponse;
AFSRQ	ActionBasicFilmSessionRequest;
AFSRSP	ActionBasicFilmSessionResponse;
CFBRQ	CreateBasicFilmBoxRequest;
CFBRSP	CreateBasicFilmBoxResponse;
DFBRQ	DeleteBasicFilmBoxRequest;
DFBRSP	DeleteBasicFilmBoxResponse;
AFBRQ	ActionBasicFilmBoxRequest;
AFBRSP	ActionBasicFilmBoxResponse;
SFBRQ	SetBasicFilmBoxRequest;
SFBRSP	SetBasicFilmBoxResponse;
SBGIBRQ	SetBasicGrayScaleImageBoxRequest;
SBGIBRRSP SetBasicGrayScaleImageBoxResponse;
SBCIBRQ	SetBasicColorImageBoxRequest;
SBCIBRRSP SetBasicColorImageBoxResponse;
GPRQ 	GetPrinterRequest;
GPRSP	GetPrinterResponse;
EPRQ	EventReportPrinterRequest;
EPRSP	EventReportPrinterResponse;

	DDO = new DICOMDataObject;
	
	vr = DCO->GetVR(0x0000, 0x0002);
	if (!vr)
		vr = DCO->GetVR(0x0000, 0x0003);

	if (vr && vr->Length >= strlen("1.2.840.10008.5.1.1.1 "))
		{
		strncpy(szPrintSOP, (char*)vr->Data, vr->Length);
		szPrintSOP[vr->Length] = 0;

		if (strcmp(szPrintSOP, "1.2.840.10008.5.1.1.16") == 0)
			{
			if (GetPrinterRequest.Read(DCO, PDU, DDO))
				{
				if (lua[0]) 
				  lua_printserver(PDU, DCO, DDO, lua, "gettingprinter"); 
				else 
				{ OperatorConsole.printf("getting Printer\n");
				  if (Thread) Progress.printf("Process=%d, Type='gettingprinter'\n", Thread);
				  if (DebugLevel>=3) NonDestructiveDumpDICOMObject(DDO);
				}

				// open up PDU for class UIDs used by printer objects
				PDU->AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
				aBasicFilmSessionSOPClassUID);
				PDU->AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
				aBasicFilmBoxSOPClassUID);
				PDU->AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
				aBasicGrayscaleImageBoxSOPClassUID);
				PDU->AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
				aBasicPrinterSOPClassUID);
				
				PDU->AddAbstractSyntaxAlias(aBasicColorPrintManagementMetaSOPClass,
				aBasicFilmSessionSOPClassUID);
				PDU->AddAbstractSyntaxAlias(aBasicColorPrintManagementMetaSOPClass,
				aBasicFilmBoxSOPClassUID);
				PDU->AddAbstractSyntaxAlias(aBasicColorPrintManagementMetaSOPClass,
				aBasicColorImageBoxSOPClassUID);
				PDU->AddAbstractSyntaxAlias(aBasicColorPrintManagementMetaSOPClass,
				aBasicPrinterSOPClassUID);

				PrintData[0] = new DICOMDataObject;
				VR *aVR;
				aVR = new VR(0x2110, 0x0010, strlen("NORMAL"), (void *)"NORMAL", FALSE);
				PrintData[0]->Push(aVR);
				aVR = new VR(0x2110, 0x0020, strlen("NORMAL"), (void *)"NORMAL", FALSE);
				PrintData[0]->Push(aVR);
				aVR = new VR(0x2110, 0x0030, strlen("Conquest dicom printer"), (void *)"Conquest dicom printer", FALSE);
				PrintData[0]->Push(aVR);

				delete DDO;
				if (DebugLevel>=3) NonDestructiveDumpDICOMObject(PrintData[0]);
				rc = GetPrinterResponse.Write(PDU, DCO, &aPrinter, 0, PrintData[0]);
				free(PrintData[0]);
				return rc;
				}

			if (EventReportPrinterRequest.Read(DCO, PDU, DDO))
				{
				if (lua[0])
				  lua_printserver(PDU, DCO, DDO, lua, "neventprinter"); 
				else
				{ OperatorConsole.printf("N-event Printer\n");
				  if (Thread) Progress.printf("Process=%d, Type='neventprinter'\n", Thread);
				  if (DebugLevel>=3) NonDestructiveDumpDICOMObject(DDO);
				}
				delete DDO;
				return EventReportPrinterResponse.Write(PDU, DCO, &aPrinter, 0, PrintData[0]);
				}
			}

		else if (strcmp(szPrintSOP, "1.2.840.10008.5.1.1.1") == 0)
			{
			if (CreateBasicFilmSessionRequest.Read(DCO, PDU, DDO))
				{
				GenUID(uid); 
				Uid.Set(uid);
				VR *aVR = new VR(0x0000, 0x1000, strlen(uid), (void *)uid, FALSE);
				DDO->Push(aVR);

				if (lua[0])
				  lua_printserver(PDU, DCO, DDO, lua, "creatingbasicfilmsession"); 
				else
				{ OperatorConsole.printf("Creating Basic Film Session\n");
				  if (Thread) Progress.printf("Process=%d, Type='creatingbasicfilmsession'\n", Thread);
				  if (DebugLevel>=3) NonDestructiveDumpDICOMObject(DDO);
				}
				PrintData[1] = DDO;

				// open up PDU for class UIDs used by printer objects
				PDU->AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
				aBasicFilmSessionSOPClassUID);
				PDU->AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
				aBasicFilmBoxSOPClassUID);
				PDU->AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
				aBasicGrayscaleImageBoxSOPClassUID);
				PDU->AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
				aBasicPrinterSOPClassUID);
				
				PDU->AddAbstractSyntaxAlias(aBasicColorPrintManagementMetaSOPClass,
				aBasicFilmSessionSOPClassUID);
				PDU->AddAbstractSyntaxAlias(aBasicColorPrintManagementMetaSOPClass,
				aBasicFilmBoxSOPClassUID);
				PDU->AddAbstractSyntaxAlias(aBasicColorPrintManagementMetaSOPClass,
				aBasicColorImageBoxSOPClassUID);
				PDU->AddAbstractSyntaxAlias(aBasicColorPrintManagementMetaSOPClass,
				aBasicPrinterSOPClassUID);

				CreateBasicFilmSession++;
				BOOL r = CreateBasicFilmSessionResponse.Write(PDU, DCO, &Uid, 0, DDO);
				delete DDO;
				return r;
				}
			if (DeleteBasicFilmSessionRequest.Read(DCO, PDU))
				{
				if (lua[0])
				  lua_printserver(PDU, DCO, DDO, lua, "deletebasicfilmsession"); 
				else
				{ OperatorConsole.printf("Deleting Basic Film Session\n");
				  if (Thread) Progress.printf("Process=%d, Type='deletebasicfilmsession'\n", Thread);
				}
				//delete PrintData[1];
				//PrintData[1] = NULL;
				DeleteBasicFilmSession++;
				delete DDO;
				return DeleteBasicFilmSessionResponse.Write(PDU, DCO, &Uid, 0);
				}

			if (ActionBasicFilmSessionRequest.Read(DCO, PDU, DDO))
				{
				if (lua[0])
				  lua_printserver(PDU, DCO, DDO, lua, "printbasicfilmsession"); 
				else
				{ OperatorConsole.printf("Printing Basic Film Session\n");
				  if (Thread) Progress.printf("Process=%d, Type='printbasicfilmsession'\n", Thread);
				  if (DebugLevel>=3) NonDestructiveDumpDICOMObject(DDO);
				}
				ActionBasicFilmSession++;
				delete DDO;
				return ActionBasicFilmSessionResponse.Write(PDU, DCO, NULL, 0, 0, NULL);
				}

			if (SetBasicFilmSessionRequest.Read(DCO, PDU, DDO))
				{
				PrintData[1] = DDO;
				if (lua[0])
				  lua_printserver(PDU, DCO, DDO, lua, "setbasicfilmsession"); 
				else
				{ OperatorConsole.printf("Set Basic Film Session\n");
				  if (Thread) Progress.printf("Process=%d, Type='setbasicfilmsession'\n", Thread);
				  if (DebugLevel>=3) NonDestructiveDumpDICOMObject(DDO);
				}
				SetBasicFilmSession++;
				delete DDO;
				return SetBasicFilmSessionResponse.Write(PDU, DCO, NULL, 0, NULL);
				}
			}


		else if (strcmp(szPrintSOP, "1.2.840.10008.5.1.1.2") == 0)
			{
			if (CreateBasicFilmBoxRequest.Read(DCO, PDU, DDO))
				{
				int n=1, rows=1, cols=1;
				char text[256];

				PrintData[2] = DDO;
				GenUID(uid); 
				Uid.Set(uid);
				VR *aVR = new VR(0x0000, 0x1000, strlen(uid), (void *)uid, FALSE);
				DDO->Push(aVR);

				VR *avr = DDO->GetVR(0x2010, 0x0010);
				if (avr)
					{
					char *p1, *p2;
					memset(text, 0, 256);
					memcpy(text, avr->Data, avr->Length);
					p1 = strchr(text, '\\');
					p2 = strchr(text, ',');
					if (p1 && p2)
						{
						rows = atoi(p1+1);
						cols = atoi(p2+1);
						n =  rows * cols;
						}
					}

				text[0]=0;
				avr = DDO->GetVR(0x2010, 0x0040);
				if (avr)
					{
					memset(text, 0, 256);
					memcpy(text, avr->Data, avr->Length);
					}

				Array < DICOMDataObject * > *SQE = new Array < DICOMDataObject * >;
				vrs = new VR(0x2010, 0x0510, 0, (void *)NULL, FALSE);
				vrs->SQObjectArray = (void*) SQE;

				for (i=0; i<n; i++)
					{
					char *uid2 = (char *)malloc(65);
					strcpy(uid2, uid);
					if (uid2[50]=='.') uid2[50]=0;	// make space for pcount, row/col info
                                        else               uid2[51]=0;
					sprintf(uid2 + strlen(uid2), ".%d.%d.%d.%d", pcount%1000, rows, cols, i);
					DICOMDataObject *D = new DICOMDataObject;
					VR *aVR;
					aVR = new VR(0x0008, 0x1150, strlen("1.2.840.10008.5.1.1.4 "), (void *)"1.2.840.10008.5.1.1.4", FALSE);
					D->Push(aVR);
					aVR = new VR(0x0008, 0x1155, (strlen(uid2)+1)&0xfffe, (void *)uid2, TRUE); //leak
					D->Push(aVR);
					SQE->Add(D);
					}

				pcount++;
				DDO->Push(vrs);

				if (lua[0])
				  lua_printserver(PDU, DCO, DDO, lua, "createbasicfilmbox"); 
				else
				{ OperatorConsole.printf("Creating Basic Film Box with %d Image boxes - %s - Film# %d\n", n, text, pcount);
				  if (Thread) Progress.printf("Process=%d, Type='createbasicfilmbox'\n, Nbox=%d, Orientation='%s', Film=%d", Thread, n, text, pcount);
				  if (DebugLevel>=3) NonDestructiveDumpDICOMObject(DDO);
				}

				CreateBasicFilmBox++;
				if (DebugLevel>=3) NonDestructiveDumpDICOMObject(DDO);
				BOOL r = CreateBasicFilmBoxResponse.Write(PDU, DCO, &Uid, 0, DDO);
				delete DDO;
				return r;
				}

			if (ActionBasicFilmBoxRequest.Read(DCO, PDU, DDO))
				{
				if (lua[0])
				  lua_printserver(PDU, DCO, DDO, lua, "printbasicfilmbox"); 
				else
				{ OperatorConsole.printf("Printing Basic Film Box\n");
				  if (Thread) Progress.printf("Process=%d, Type='printbasicfilmbox'\n", Thread);
				  if (DebugLevel>=3) NonDestructiveDumpDICOMObject(DDO);
				}
				ActionBasicFilmBox++;
				delete DDO;
				return ActionBasicFilmBoxResponse.Write(PDU, DCO, NULL, 0, 0, NULL);
				}

			if (SetBasicFilmBoxRequest.Read(DCO, PDU, DDO))
				{
				PrintData[2] = DDO;
				if (lua[0])
				  lua_printserver(PDU, DCO, DDO, lua, "setbasicfilmbox"); 
				else
				{ OperatorConsole.printf("Set Basic Film Box\n");
				  if (Thread) Progress.printf("Process=%d, Type='setbasicfilmbox'\n", Thread);
				  if (DebugLevel>=3) NonDestructiveDumpDICOMObject(DDO);
				}
				SetBasicFilmBox++;
				delete DDO;
				return SetBasicFilmBoxResponse.Write(PDU, DCO, NULL, 0, NULL);
				}

			if (DeleteBasicFilmBoxRequest.Read(DCO, PDU))
				{
				if (lua[0])
				  lua_printserver(PDU, DCO, DDO, lua, "deletebasicfilmbox"); 
				else
				{ OperatorConsole.printf("Deleting Basic Film Box\n");
				  if (Thread) Progress.printf("Process=%d, Type='deletebasicfilmbox'\n", Thread);
				}
				//delete PrintData[2];
				//PrintData[2] = NULL;
				DeleteBasicFilmBox++;
				delete DDO;
				return DeleteBasicFilmBoxResponse.Write(PDU, DCO, &Uid, 0);
				}
			}

		else if (strcmp(szPrintSOP, "1.2.840.10008.5.1.1.4") == 0)
			{
			if (SetBasicGrayScaleImageBoxRequest.Read(DCO, PDU, DDO))
				{
				if (DebugLevel>=3) NonDestructiveDumpDICOMObject(DDO);
				vr = DCO->GetVR(0x0000, 0x1001);
				if (vr)
					{
					memset(Filename, 0, 1024);
					GetPhysicalDevice("MAG0", Filename);
					strcat(Filename, "printer_files");
					mkdir(Filename);
					i = strlen(Filename);
					Filename[i]   = PATHSEPCHAR;
					Filename[i+1] = 0;
					memcpy(Filename + strlen(Filename), vr->Data, vr->Length);
					strcat(Filename, ".dcm");
					}

				//OperatorConsole.printf("Setting Basic GrayScale ImageBox\n");

				vr = DDO->GetVR(0x2020, 0x0110);
				if(vr && vr->SQObjectArray)
					{
					Array < DICOMDataObject	* > *aSQArray;
					aSQArray = (Array < DICOMDataObject * > *) vr->SQObjectArray;
					if ( aSQArray->GetSize() )
						{
							if (lua[0])
							  lua_printserver(PDU, DCO, DDO, lua, Filename); 
							else
							{ PDU->SaveDICOMDataObject(Filename, DICOM_CHAPTER_10_EXPLICIT, aSQArray->Get(0));
							  OperatorConsole.printf("Print file: %s\n", Filename);
							  if (Thread) Progress.printf("Process=%d, Type='printfile', File='%s'\n", Thread, Filename);
							}
						}
					}

				SetBasicGrayScaleImageBox++;
				delete DDO;
				return SetBasicGrayScaleImageBoxResponse.Write(PDU, DCO, NULL, 0, NULL);
				}
			}

		else if (strcmp(szPrintSOP, "1.2.840.10008.5.1.1.4.1") == 0)
			{
			if (SetBasicColorImageBoxRequest.Read(DCO, PDU, DDO))
				{
				if (DebugLevel>=3) NonDestructiveDumpDICOMObject(DDO);
				vr = DCO->GetVR(0x0000, 0x1001);
				if (vr)
					{
					memset(Filename, 0, 1024);
					GetPhysicalDevice("MAG0", Filename);
					strcat(Filename, "printer_files");
					mkdir(Filename);
					i = strlen(Filename);
					Filename[i]   = PATHSEPCHAR;
					Filename[i+1] = 0;
					memcpy(Filename + strlen(Filename), vr->Data, vr->Length);
					strcat(Filename, ".dcm");
					}

				//OperatorConsole.printf("Setting Basic Color ImageBox\n");

				vr = DDO->GetVR(0x2020, 0x0111);
				if(vr && vr->SQObjectArray)
					{
					Array < DICOMDataObject	* > *aSQArray;
					aSQArray = (Array < DICOMDataObject * > *) vr->SQObjectArray;
					if ( aSQArray->GetSize() )
						{
							if (lua[0])
							  lua_printserver(PDU, DCO, DDO, lua, Filename); 
							else
							{ PDU->SaveDICOMDataObject(Filename, DICOM_CHAPTER_10_EXPLICIT, aSQArray->Get(0));
							  OperatorConsole.printf("Print file: %s\n", Filename);
							  if (Thread) Progress.printf("Process=%d, Type='printfile', File='%s'\n", Thread, Filename);
							}
						}
					}

				SetBasicColorImageBox++;
				delete DDO;
				return SetBasicColorImageBoxResponse.Write(PDU, DCO, NULL, 0, NULL);
				}
			}
		}
	delete DDO;
	return FALSE;
	}

///////////////////////////// elementary storage commitment support as SCP //////////////////////////
//////////// storage commitment as SCU would be needed for a safe Move/Delete operation /////////////

/* first is a sample transaction 

SEND ASSOCIATE-RQ (
PRESENTATION-CONTEXT-ITEMS
("Storage Commitment Push Model SOP Class", "Implicit VR Little Endian")
)

RECEIVE ASSOCIATE-AC (
PRESENTATION-CONTEXT-ITEMS
("Storage Commitment Push Model SOP Class", 0, "Implicit VR Little Endian")
)


SEND N-ACTION-RQ "Commitment Push" (
(0x00000003, "Storage Commitment Push Model SOP Class")
(0x00001001, "1.2.840.10008.1.20.1.1") # Well-known Instance UID
(0x00001008, 1) # Request Storage Commitment

(0x00081195, UI, "NEW:TransactionUID") # Transaction UID
(0x00081199, SQ,
>(0x00081150, UI, "1.2.840.10008.5.1.4.1.1.7") # Referenced SOP Class UID
>(0x00081155, UI, "NEW:ImageInstanceUID1") # Referenced SOP Instance UID
,
>(0x00081150, UI, "1.2.840.10008.5.1.4.1.1.7") # Referenced SOP Class UID
>(0x00081155, UI, "NEW:ImageInstanceUID2") # Referenced SOP Instance UID
,
>(0x00081150, UI, "1.2.840.10008.5.1.4.1.1.7") # Referenced SOP Class UID
>(0x00081155, UI, "NEW:ImageInstanceUID3") # Referenced SOP Instance UID
,
>(0x00081150, UI, "1.2.840.10008.5.1.4.1.1.7") # Referenced SOP Class UID
>(0x00081155, UI, "NEW:ImageInstanceUID4") # Referenced SOP Instance UID
) # Referenced SOP Sequence
(0x00081111, SQ,
>(0x00081150, UI, "1.2.840.10008.3.1.2.3.3") # Referenced SOP Class UID
>(0x00081155, UI, "NEW:MppsUID") # Referenced SOP Instance UID
) # Referenced Study Component Sequence
)

RECEIVE N-ACTION-RSP (
(0x00000002, "Storage Commitment Push Model SOP Class")
(0x00001000, "1.2.840.10008.1.20.1.1") # Well-known Instance UID
(0x00000900, 0x0000)
)


SEND RELEASE-RQ
RECEIVE RELEASE-RP


RECEIVE ASSOCIATE-RQ (
PRESENTATION-CONTEXT-ITEMS
("Storage Commitment Push Model SOP Class", "Implicit VR Little Endian")
)

SEND ASSOCIATE-AC (
PRESENTATION-CONTEXT-ITEMS
("Storage Commitment Push Model SOP Class", 0, "Implicit VR Little Endian")
)


RECEIVE N-EVENT-REPORT-RQ "Commitment Push" (
(0x00000002, "Storage Commitment Push Model SOP Class")
(0x00001000, "1.2.840.10008.1.20.1.1") # Well-known Instance UID
(0x00001002, 1) # Storage Commitment Request Successful

(0x00081195, UI, "TransactionUID") # Transaction UID
(0x00080054, AE, "ARCHIVE") # Retrieve AE Title
(0x00081199, SQ,
>(0x00081150, UI, "1.2.840.10008.5.1.4.1.1.7") # Referenced SOP Class UID
>(0x00081155, UI, "ImageInstanceUID1") # Referenced SOP Instance UID
,
>(0x00081150, UI, "1.2.840.10008.5.1.4.1.1.7") # Referenced SOP Class UID
>(0x00081155, UI, "ImageInstanceUID2") # Referenced SOP Instance UID
,
>(0x00081150, UI, "1.2.840.10008.5.1.4.1.1.7") # Referenced SOP Class UID
>(0x00081155, UI, "ImageInstanceUID3") # Referenced SOP Instance UID
,
>(0x00081150, UI, "1.2.840.10008.5.1.4.1.1.7") # Referenced SOP Class UID
>(0x00081155, UI, "ImageInstanceUID4") # Referenced SOP Instance UID
) # Referenced SOP Sequence
)

SEND N-EVENT-REPORT-RSP (
(0x00000002, "Storage Commitment Push Model SOP Class")
(0x00001000, "1.2.840.10008.1.20.1.1") # Well-known Instance UID
(0x00000900, 0x0000)
)


RECEIVE RELEASE-RQ
SEND RELEASE-RP
*/

class ASCPRQ : public NActionRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.1.20.1"); return TRUE; }
	};

class ASCPRSP : public NActionRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.1.20.1"); return TRUE; }
	};

class ESCPRQ : public NEventReportRQ
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.1.20.1"); return TRUE; }
	};

class ESCPRSP : public NEventReportRSP
	{
	public:	BOOL	GetUID ( UID &theUID ) { theUID.Set("1.2.840.10008.1.20.1"); return TRUE; }
	};

BOOL StorageApp ::	StorageCommitmentSupport( CheckedPDU_Service *PDU, DICOMCommandObject *DCO, DICOMDataObject **CommitData)
	{
	VR *vr;
	DICOMCommandObject DCOR;
	char Sop[65], Calling[20];
	BYTE ip[64], port[64], compress[64];
	int nfail=0;
	BOOL status;

	UID	StorageCommitmentPushModelClassUID("1.2.840.10008.1.20.1");
	UID	StorageCommitmentPushModelInstanceUID("1.2.840.10008.1.20.1.1");

	ASCPRQ	ActionStorageCommitmentPushRequest;
	ASCPRSP	ActionStorageCommitmentPushResponse;

	ESCPRQ	EventStorageCommitmentPushRequest;
	ESCPRSP	EventStorageCommitmentPushResponse;

	// association was closed after action: send response on new association

	if (*CommitData)
		{
		PDU_Service PDU2;
		UID uid;

		memset(Calling, 0, 20);
		strcpy(Calling, (char *)(((AAssociateAC *)PDU)->CallingApTitle));
	        if (Calling[0]>32)
		  while (strlen(Calling) && Calling[strlen(Calling)-1]==' ') Calling[strlen(Calling)-1] = 0;

		PDU2.AttachRTC(&VRType);
		if(!GetACRNema(Calling, (char *)ip, (char *)port, (char *)compress))
			{
			OperatorConsole.printf("*** Storage commitment - host not found: %s\n", Calling);
			return FALSE;
			}

		PDU2.ClearAbstractSyntaxs();
		PDU2.SetLocalAddress(MYACRNEMA);
		PDU2.SetRemoteAddress((unsigned char *)Calling);

		uid.Set("1.2.840.10008.3.1.1.1");	// Application context (DICOM App)
		PDU2.SetApplicationContext(uid);

		uid.Set("1.2.840.10008.1.20.1");
		PDU2.AddAbstractSyntax(uid);

		PDU2.SetTimeOut(TCPIPTimeOut);

		// Make new association for EVENT on port/ip
		if(!PDU2.Connect(ip, port))
			{
			OperatorConsole.printf("*** Storage commitment - failed to reconnect to %s (%s %s) for N-EVENT\n", Calling, ip, port);
			return FALSE;
			}

		status = EventStorageCommitmentPushRequest.Write(&PDU2, *CommitData, &StorageCommitmentPushModelInstanceUID, nfail==0 ? 1 : 2);
			if (status)
			{
			status = PDU2.Read(&DCOR);
			if (status)
				{
				EventStorageCommitmentPushResponse.Read(&DCOR, &PDU2, NULL);
				PDU2.Close();
				return FALSE;
				}
			}

		PDU2.Close();
		return FALSE;
		}

	// is this a storage commitment action request ?

	vr = DCO->GetVR(0x0000, 0x0003);
	if (!vr) return FALSE;
	strncpy(Sop, (char*)vr->Data, vr->Length);
	Sop[vr->Length] = 0;
	if (strcmp(Sop, "1.2.840.10008.1.20.1")!=0) return FALSE;

	vr = DCO->GetVR(0x0000, 0x1001);
	if (!vr) return FALSE;
	strncpy(Sop, (char*)vr->Data, vr->Length);
	Sop[vr->Length] = 0;
	if (strcmp(Sop, "1.2.840.10008.1.20.1.1")!=0) return FALSE;

	vr = DCO->GetVR(0x0000, 0x1008);
	if (!vr) return FALSE;
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
	if ((int)(*((unsigned short*)vr->Data))!=1) return FALSE;
#else //Big Endian like Apple power pc
	if ((int)SwitchEndian(*((unsigned short*)vr->Data))!=1) return FALSE;
#endif

	// read and acknowledge the action

	*CommitData = new DICOMDataObject;
	if (ActionStorageCommitmentPushRequest.Read(DCO, PDU, *CommitData))
		{
		OperatorConsole.printf("Storage commitment request recieved\n");
 		if (!ActionStorageCommitmentPushResponse.Write(PDU, DCO, &StorageCommitmentPushModelInstanceUID, 1, 0, NULL)) 
			return FALSE;
	
		// delete retired study component sequence if there

		vr = (*CommitData)->GetVR(0x0008, 0x1111);
      		if (vr) (*CommitData)->DeleteVR(vr);

		// add AE title

		SetStringVR(&vr, 0x0008, 0x0054, (char*)MYACRNEMA); 
		(*CommitData)->Push(vr);

		// todo:
		// check presence of each SOPInstance 0008,1155 in sequence 0008,1199 
		// move failures from sequence 0008,1199 to sequence 0008,1198
		// count failures in nfail

		// write an event (on same association) with results

		Sleep(200);

		status = EventStorageCommitmentPushRequest.Write(PDU, *CommitData, &StorageCommitmentPushModelInstanceUID, nfail==0 ? 1 : 2);

		if (status)
			{
			status = PDU->Read(&DCOR);
			if (status)
				{
				EventStorageCommitmentPushResponse.Read(&DCOR, PDU, NULL);
				delete *CommitData;
				*CommitData = NULL;
				return TRUE;
				}
			}

		// in this case, the link was closed: create a new link and try to send response again
		return FALSE;
		}

	return FALSE;
	}

void CloneDB(char *AE)
{ VR *vr1;
  VR *vr;
  DICOMDataObject	DDO;
  Array < DICOMDataObject * > ADDO;
  char patid[66];
  int len;
  
  SetStringVR(&vr1, 0x0010, 0x0020, ""); 
  DDO.Push(vr1);
  SetStringVR(&vr1, 0x0008, 0x0052, "PATIENT"); 
  DDO.Push(vr1);
  VirtualQuery(&DDO, "PATIENT", 0, &ADDO, AE);

  for (unsigned int i=0; i<ADDO.GetSize(); i++)
  { while((vr=ADDO.Get(i)->Pop()))
    { if (0x0020==vr->Element && 0x0010==vr->Group)
      { strncpy(patid, (char*)vr->Data, vr->Length);
	patid[vr->Length] = 0;
	len = vr->Length - 1;
        while(len>0)
        { if (patid[len] == ' ')
	    patid[len] = 0;
          else
            break;
          len--;
        }

        OperatorConsole.printf("cloning db for patient id=%s\n", patid);

        DDO.Reset();
        SetStringVR(&vr1, 0x0010, 0x0020, patid); 
        DDO.Push(vr1);
        SetStringVR(&vr1, 0x0008, 0x0052, "IMAGE"); 
        DDO.Push(vr1);
        VirtualQueryToDB(&DDO, 0, AE);
      }

      delete vr;
    }
    delete ADDO.Get(i);
  }

  OperatorConsole.printf("clone db done\n");
}

static void NewTempFile(char *name, const char *ext)
	{
	int i;
	char name2[70], szTemp[256], szRootSC[256];

	MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile);
  	MyGetPrivateProfileString(szRootSC, "TempDir", "", szTemp, 64, ConfigFile);

        if (szTemp[0]==0)
		{
		name[0]=0;
		GetPhysicalDevice("MAG0", name);
		strcat(name, "printer_files");
		}
	else
		strcpy(name, szTemp);

	mkdir(name);

	i = strlen(name);
	name[i]   = PATHSEPCHAR;
	name[i+1] = 0;

	GenUID(name2);
	strcat(name2, ext);

	strcat(name, name2);
	}

BOOL
MakeQueryString ( DBENTRY	*DBE, char	*s)
	{
	UINT	Index;
//	char	TempString [ 128 ];
		
	s[0] = '\0';

	Index = 0;
	while ( TRUE )
		{
		if(!DBE [ Index ].Element)
			return ( TRUE );

		if (DBE[Index].DICOMType!=DT_STARTSEQUENCE && DBE[Index].DICOMType!=DT_ENDSEQUENCE)
			{
			if(Index)
				strcat(s, ", ");

			strcat ( s, DBE [ Index ].SQLColumn);
			}
		
		++Index;
		}

	return ( TRUE );
	}

BOOL MakeSafeStringValues (VR *vr, char *string);
static BOOL DcmEcho(const char *AE);

static char* CommaInFilenameWorkAround(char* SilentText)
{ char*		pComma;
  char*		pCopy;

  pComma = strchr(SilentText, ',');
  while (pComma)
  { /* */
    pCopy = strdup(pComma);
    strlwr(pCopy);
    if ((strstr(pCopy, ".dcm") != NULL) || (strstr(pCopy, ".v2") != NULL) ||
	(strstr(pCopy, ".hl7") != NULL) || (strstr(pCopy, ".gz") != NULL))
    { /* The comma is apparently part of a filename; search for the next comma */
      free(pCopy);
      pComma = strchr(pComma + 1, ',');
    }
    else
    { free(pCopy);
      break;
    }
  }
  return pComma;
}

static void SimplifyDicom(DICOMDataObject *pDDO)
{ 	DICOMObject	DO2;
	VR		*vr;

	while((vr=pDDO->Pop()))
		{
		if (vr->SQObjectArray) delete vr;
		else DO2.Push(vr);
		}

	pDDO->Reset();
	while((vr=DO2.Pop())) pDDO->Push(vr);
}

static int cgi_parse(char *in, char *out, const char *name, const char *def);

void ServerTask(char *SilentText, ExtendedPDU_Service &PDU, DICOMCommandObject &DCO,
                char *Response, unsigned int ConnectedIP, char *tempfile, int Thread)
	{
	VR *vr;
	char blank[]="";
	char	TimeString[128];
	
	char *p = strchr(SilentText, ','), *q=NULL, *r1=NULL, *s1=NULL, *t1=NULL;
	GuiRequest++;

	if (memcmp(SilentText, "lua:", 4)==0)
		{ 
		struct scriptdata sd = {&PDU, &DCO, NULL, -1, NULL, NULL, NULL, NULL, NULL, 0, ConnectedIP};
		p = (char *)do_lua(&(PDU.L), SilentText+4, &sd);
		if (p) strcpy(Response, p);
		p = (char *)do_lua(&(PDU.L), "return returnfile", &sd);
		if (p) strcpy(tempfile, p);
		if (sd.DDO) delete sd.DDO;
		}

	if (memcmp(SilentText, "luastart:", 9)==0)
		{
		struct scriptdata sd = {&PDU, &DCO, NULL, -1, NULL, NULL, NULL, NULL, NULL, 0, ConnectedIP};
		PDU.Link.Close();
		p = (char *)do_lua(&(PDU.L), SilentText+9, &sd);
		if (p) strcpy(Response, p);
		if (sd.DDO) delete sd.DDO;
		}

	if (memcmp(SilentText, "globallua:", 10)==0)		////// Note: not used by dgate --dolua: at all, for use in servercommand only
		{					// Runs in global context so keeps data over instances
		EnterCriticalSection(&dolua_critical);
		struct scriptdata sd = {&globalPDU, &DCO, NULL, -1, NULL, NULL, NULL, NULL, NULL, 0, ConnectedIP};
		p = (char *)do_lua(&(globalPDU.L), SilentText+10, &sd);
		if (p) strcpy(Response, p);
		LeaveCriticalSection(&dolua_critical);
		}

	else if (memcmp(SilentText, "extract:", 8)==0)
		{
		char t[512], u[512];
		Database DB;
		char fld[48][256];
		SQLLEN SQLResultLength;
		int table;
		char *q = SilentText+8;
		const char *sort=NULL;
		
		if (DB.Open ( DataSource, UserName, Password, DataHost ) )
			{
			if (DB.db_type == DT_DBASEIII && q && *q)
				{
				sprintf(t, "%sX%s.DBF", DataSource, PatientTableName);
				DB.ExtractRecords("DICOMPatients",q, t);
				sprintf(t, "%sX%s.DBF", DataSource, StudyTableName);
				DB.ExtractRecords("DICOMStudies", q, t);
				sprintf(t, "%sX%s.DBF", DataSource, SeriesTableName); 
				DB.ExtractRecords("DICOMSeries",  q, t);
				sprintf(t, "%sX%s.DBF", DataSource, ImageTableName);  
				DB.ExtractRecords("DICOMImages",  q, t);
				//sprintf(t, "%sX%s.DBF", DataSource, WorkListTableName);
				//DB.ExtractRecords(WorkListTableName, q, t);
				}
			else if (q)
				{
				char s[8192];
				char TempString[512];
				char Physical[512];
				Database DB2(DT_DBASEIIINOINDEX);
				GetPhysicalDevice("MAG0", Physical);
				sprintf(Physical+strlen(Physical), "dbase%c", PATHSEPCHAR);
				DB2.Open ( Physical, "", "", "");
				DBENTRY *dbe;
				char *tabname;
				int i, k;

				dbe = NULL;
				tabname = NULL;
				for (int j=0; j<5; j++)
					{
					switch(j)
						{
						case 0: dbe = PatientDB; tabname = PatientTableName; break;
						case 1: dbe = PatientDB; tabname = PatientTableName; break;
						case 2: dbe = StudyDB;   tabname = StudyTableName;   break;
						case 3: dbe = SeriesDB;  tabname = SeriesTableName;  break;
						case 4: dbe = ImageDB;   tabname = ImageTableName;   break;
						case 5: dbe = WorkListDB;tabname = WorkListTableName;sort = NULL; break;
						}

					MakeTableString(dbe, s, 1);
					MakeQueryString(dbe, t);
					strcat(s, ", AccessTime int, qTimeStamp int, qFlags int, qSpare varchar(64)" );
					strcat(t, ", AccessTime, qTimeStamp, qFlags, qSpare" );
					if (j==4)
						{
						strcat(s, ", ObjectFile varchar(255), DeviceName varchar(32)");
						strcat(t, ", ObjectFile, DeviceName");
						}

					for (i=0, k=0; i<999 && dbe[i].Group!=0; i++) k++; // count fields
					if (j==4) k+=6; else k+=4;

					if (j==0) sprintf(u, "XA%s",  tabname);	// all patients
					else      sprintf(u, "X%s", tabname);
					DB2.CreateTable (u, s);

					if (j==3)
					{ p = strstr(q, "PatientID");
					  if (p) memcpy(p, "SeriesPat", 9);
					}
					else if (j==4)
					{ p = strstr(q, "SeriesPat");
					  if (p) memcpy(p, "ImagePat ", 9);
					  p = strstr(q, "PatientID");
					  if (p) memcpy(p, "ImagePat ", 9);
					}
					  
					if (j!=0) 
					  DB.Query(tabname, t, q, sort);
					else
					  DB.Query(tabname, t, NULL, NULL);	// all patients

					for (i=0; i<k; i++)
					  DB.BindField(i+1, SQL_C_CHAR, fld[i], 255, &SQLResultLength);
					memset(fld, 0, sizeof(fld));

					while (DB.NextRecord())
						{
						s[0]=0;
						for (int i1=0; i1<k; i1++)
							{
							VR vr2;
							vr2.Data   = fld[i1];
							vr2.Length = strlen(fld[i1]);
							if (i1) strcat ( s, ", ");
							
							MakeSafeStringValues ( &vr2, TempString );
							vr2.Data = NULL;
							vr2.Length = 0;
							strcat ( s, TempString );
							}
						DB2.AddRecord(u, t, s);
						}

					if (*q==0) break;
					}
				}
			ExtractFromGui++;
			strcpy(Response, "0");
			}
			else
				strcpy(Response, "1");
		}

	else if (memcmp(SilentText, "todbf:", 6)==0)
		{
		char t[512], u[512];
		Database DB;
		char fld[48][256];
		SQLLEN SQLResultLength;
		char *q = SilentText+6;
		const char *folder=NULL;
		const char *tabname=NULL;
		const char *query=NULL;
		const char *sort=NULL;
		int imax=0;
		
		folder = q;
		q = strchr(q, '|');
		if (q) 
			{
			*q++ = 0;
			tabname = q;
			q = strchr(q, '|');
			if (q) 
				{
				*q++ = 0;
				query = q;
				q = strchr(q, '|');
				if (q) 
					{
					*q++ = 0;
					sort = q;
					q = strchr(q, '|');
					if (q) 
						{
						*q++ = 0;
						imax = atoi(q);
						}
					}
				}
			}
						
		if (tabname && DB.Open ( DataSource, UserName, Password, DataHost ) )
			{
			char s[8192];
			char TempString[512];
			char Physical[512];
			Database DB2(DT_DBASEIIINOINDEX);
			DB2.Open ( folder, "", "", "");
			DBENTRY *dbe;
			int i, k, count=0;
			
			dbe = NULL;
			if (stricmp(tabname, PatientTableName)==0) dbe = PatientDB;
			if (stricmp(tabname, StudyTableName)==0) dbe = StudyDB;
			if (stricmp(tabname, SeriesTableName)==0) dbe = SeriesDB;
			if (stricmp(tabname, ImageTableName)==0) dbe = ImageDB;
			if (stricmp(tabname, WorkListTableName)==0) dbe = WorkListDB;

			MakeTableString(dbe, s, 1);
			MakeQueryString(dbe, t);
			for (i=0, k=0; i<999 && dbe[i].Group!=0; i++) k++; // count fields

			strcat(s, ", AccessTime int, qTimeStamp int, qFlags int, qSpare varchar(64)" );
			strcat(t, ", AccessTime, qTimeStamp, qFlags, qSpare" );
			k += 4;

			if (dbe == ImageDB)
				{
				strcat(s, ", ObjectFile varchar(255), DeviceName varchar(32)");
				strcat(t, ", ObjectFile, DeviceName");
				k += 2;
				}

			DB2.CreateTable (tabname, s);

			DB.Query(tabname, t, query, sort);
			for (i=0; i<k; i++)
				DB.BindField(i+1, SQL_C_CHAR, fld[i], 255, &SQLResultLength);

			memset(fld, 0, sizeof(fld));
			while (DB.NextRecord())
				{
				s[0]=0;
				for (int i1=0; i1<k; i1++)
					{
					VR vr2;
					vr2.Data   = fld[i1];
					vr2.Length = strlen(fld[i1]);
					if (i1) strcat ( s, ", ");
							
					MakeSafeStringValues ( &vr2, TempString );
					vr2.Data = NULL;
					vr2.Length = 0;
					strcat ( s, TempString );
					}
				DB2.AddRecord(tabname, t, s);
				if (imax && count++ > imax) break;
				}

			ExtractFromGui++;
			strcpy(Response, "0");
			}
		else
			strcpy(Response, "1");
		}

	else if (memcmp(SilentText, "query:", 6)==0 || memcmp(SilentText, "query2:", 7)==0 )
		{
		int n=1;
		unsigned int i, L, mx,  flds=1;
		char *items[6];
		SQLLEN SQLResultLength;
		memset(items, 0, sizeof(items));

		p = strchr(SilentText, ':')+1;
		items[0]=p;
		L = strlen(p);
		for (i=0; i<L; i++)
			{
			if (p[i]=='|')
				{
				p[i]=0;
				items[n++] = p+i+1;
				if(n==6) break;
				}
			}

		if (memcmp(SilentText, "query2:", 7)==0) p=items[5], mx=items[4] ? atoi(items[4]): 0; 
		else p=items[4], mx=0;

		if (items[1])
			for (i=0; i<strlen(items[1]); i++) 
				if (items[1][i]==',') flds++;

		Database DB;
		strcpy(Response, "1");
		if (DB.Open ( DataSource, UserName, Password, DataHost ) )
			{
			char fld[48][256];
			FILE *f;
			char format[8192];
			int pstars, first=1;

			if (items[3]) strcpy(format, items[3]);
			else for (i=0; i<flds; i++) strcat (format, "%s ");

			pstars = format[strlen(format)-1] == '*';
			if (!pstars) strcat(format, "\n"); else format[strlen(format)-1]=0;
			memset(fld, 0, sizeof(fld));
			
			if (items[2])
				if (*items[2]==0) items[2]=NULL;

			if (mx)
				{
				DB.Query(items[0], items[1], items[2], NULL);
				for (i=0; i<flds; i++) 
					DB.BindField (i+1, SQL_C_CHAR, fld[i], 255, &SQLResultLength);
				i = 0;
				while (DB.NextRecord()) i++;
				if (i>mx) mx=(i+mx-1)/mx; else mx=0;
				}

			DB.Query(items[0], items[1], items[2], NULL);
			for (i=0; i<flds; i++) 
				DB.BindField (i+1, SQL_C_CHAR, fld[i], 255, &SQLResultLength);
			if (p==NULL || *p==0)
				{
				NewTempFile(tempfile, ".txt");
				f = fopen(tempfile, "wt");
				}
			else if (strcmp(p, "cgi")==0)
				{
				NewTempFile(tempfile, ".txt");
				f = fopen(tempfile, "wt");
				fprintf(f, "Content-type: text/html\n\n");
				}
			else if (strcmp(p, "binary")==0)
				{
				NewTempFile(tempfile, ".txt");
				f = fopen(tempfile, "wb");
				}
			else
				f = fopen(p, "wt");

			n=0;
			for (i=0; i<48; i++) fld[i][0]=0;
			while (DB.NextRecord())
				{
				if (mx) if ((n++) % mx) continue;
				for (i=flds; i<48; i++)	strcpy(fld[i], fld[i%flds]); // duplicate fields

				if (!first && pstars) fprintf(f, "*");
				first=0;

				fprintf(f, format, fld[0], fld[1], fld[2], fld[3], fld[4], fld[5], fld[6], fld[7],
						   fld[8], fld[9], fld[10],fld[11],fld[12],fld[13],fld[14],fld[15],
						   fld[16],fld[17],fld[18],fld[19],fld[20],fld[21],fld[22],fld[23],
						   fld[24],fld[25],fld[26],fld[27],fld[28],fld[29],fld[30],fld[31],
						   fld[32],fld[33],fld[34],fld[35],fld[36],fld[37],fld[38],fld[39],
						   fld[40],fld[41],fld[42],fld[43],fld[44],fld[45],fld[46],fld[47]);
				for (i=0; i<48; i++) fld[i][0]=0;
				strcpy(Response, "0");
				}

			fclose(f);
			DB.Close();
			QueryFromGui++;
			}
		}

	else if (memcmp(SilentText, "patientfinder:", 14)==0 ||
		 memcmp(SilentText, "studyfinder:", 12)==0 ||
		 memcmp(SilentText, "seriesfinder:", 13)==0 ||
		 memcmp(SilentText, "imagefinder:", 12)==0 )
		{
		char ps2[] = "%s %s";
		char ps5[] = "%s %s %s %s %s";
		char ps7[] = "%s %s %s %s %s %s %s";
		int i, n=1, L;//, flds=1, mx;
		char *items[6];
		FILE *f;
		char format[1024];

		memset(items, 0, sizeof(items));

		p = strchr(SilentText, ':')+1;
		items[0]=p;
		L = strlen(p);
		for (i=0; i<L; i++)
			{
			if (p[i]=='|')
				{
				p[i]=0;
				items[n++] = p+i+1;
				if(n==6) break;
				}
			}
		p = items[3];

		if (items[2]==0 || *items[2]==0)
			{
			if (memcmp(SilentText, "pat", 3)==0) items[2]=ps2;
			if (memcmp(SilentText, "stu", 3)==0) items[2]=ps5;
			if (memcmp(SilentText, "ser", 3)==0) items[2]=ps7;
			if (memcmp(SilentText, "ima", 3)==0) items[2]=ps7;
			}
		strcpy(format, items[2]);
		strcat(format, "\n");

		if (p==NULL || *p==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wt");
			}
		else if (strcmp(p, "cgi")==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wt");
			fprintf(f, "Content-type: text/html\n\n");
			}
		else if (strcmp(p, "binary")==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wb");
			}
		else
			f = fopen(p, "wt");

		if (items[1]==NULL) items[1] = blank;
		if (memcmp(SilentText, "pat", 3)==0) PatientStudyFinder(items[0], items[1], format, f, "PATIENT");
		if (memcmp(SilentText, "stu", 3)==0) PatientStudyFinder(items[0], items[1], format, f, "STUDY");
		if (memcmp(SilentText, "ser", 3)==0) PatientStudyFinder(items[0], items[1], format, f, "SERIES");
		if (memcmp(SilentText, "ima", 3)==0) PatientStudyFinder(items[0], items[1], format, f, "IMAGE");
		fclose(f);
		}

	else if (memcmp(SilentText, "imagelister:", 12)==0 ||
		 memcmp(SilentText, "serieslister:", 13)==0 )
		{
		int i, n=1, L;//, flds=1, mx;
		char *items[6];
		FILE *f;
		char format[256];

		memset(items, 0, sizeof(items));

		p = strchr(SilentText, ':')+1;
		items[0]=p;
		L = strlen(p);
		for (i=0; i<L; i++)
			{
			if (p[i]=='|')
				{
				p[i]=0;
				items[n++] = p+i+1;
				if(n==6) break;
				}
			}
		p = items[4];

		if (items[3]==0 || *items[3]==0)
			strcpy(format, "%s*");
		else
			strcpy(format, items[3]);
		if (format[0]!='$' && format[strlen(format)-1]!='*') strcat(format, "\n");

		if (p==NULL || *p==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wt");
			}
		else if (strcmp(p, "cgi")==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wt");
			fprintf(f, "Content-type: text/html\n\n");
			}
		else if (strcmp(p, "binary")==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wb");
			}
		else
			f = fopen(p, "wt");

		if (SilentText[1]=='m') ImageFileLister(items[0], items[1], NULL, items[2], NULL, format, f);
		else 			SeriesUIDLister(items[0], items[1], items[2], format, f);
		fclose(f);
		}

	else if (memcmp(SilentText, "addrecord:", 10)==0)
		{
		int i, n=1, L;
		char *items[3];
		memset(items, 0, sizeof(items));

		p = SilentText+10;
		items[0]=p;
		L = strlen(p);
		for (i=0; i<L; i++)
			{
			if (p[i]=='|')
				{
				p[i]=0;
				items[n++] = p+i+1;
				if(n==3) break;
				}
			}

		Database DB;
		if (DB.Open ( DataSource, UserName, Password, DataHost ) )
			{
			DB.AddRecord(items[0], items[1], items[2]);
			DB.Close();
			QueryFromGui++;
			}
		}

	else if (memcmp(SilentText, "deleterecord:", 13)==0)
		{
		Database DB;
		if (DB.Open ( DataSource, UserName, Password, DataHost ) )
			{
			if (p) *p++=0;				// points after 1st comma
			DB.DeleteRecord(SilentText+13, p);
			DB.Close();
			QueryFromGui++;
			}
		}

	else if (memcmp(SilentText, "deleteimagefromdb:", 18)==0)
		{
		DeleteImageFile(SilentText+18, TRUE);
		}

	else if (memcmp(SilentText, "deletesopfromdb:", 16)==0)
		{
		Database DB;
		if (DB.Open ( DataSource, UserName, Password, DataHost ) )
			{ if (p) 
			  { *p++=0;				// points after 1st comma
			    q = strchr(p, ',');
			    if (q) 
			    { *q++=0;				// points after 2nd comma
			       r1 = strchr(q, ',');
			       if (r1) 
			       { *r1++=0;			// points after 3rd comma
				 NewDeleteSopFromDB(SilentText+16, p, q, r1, DB);
			       }
			     }
			  }
			}
		}

	else if (memcmp(SilentText, "deleteimagefile:", 16)==0)
		{
		DeleteImageFile(SilentText+16, FALSE);
		DeleteImageFromGui++;
		}

	else if (memcmp(SilentText, "deletepatient:", 14)==0)
		{
		DeletePatient(SilentText+14, FALSE, Thread);
		DeletePatientFromGui++;
		}

	else if (memcmp(SilentText, "deletestudy:", 12)==0)
		{
		DeleteStudy(SilentText+12, FALSE, Thread);
		DeleteStudyFromGui++;
		}

	else if (memcmp(SilentText, "deletestudies:", 14)==0)
		{
		DeleteStudies(SilentText+14, FALSE, Thread);
		DeleteStudiesFromGui++;
		}

	else if (memcmp(SilentText, "deleteseries:", 13)==0)
		{
		DeleteSeries(SilentText+13, FALSE, Thread);
		DeleteSeriesFromGui++;
		}

	else if (memcmp(SilentText, "deleteimage:", 12)==0)
		{
		DeleteImage(SilentText+12, FALSE, Thread);
		DeleteImageFromGui++;
		}

	else if (memcmp(SilentText, "movepatient:", 12)==0)
		{
		if (p) 
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');
		  if (q) 
		  { *q++=0;				// points after 2nd comma
		    if (DcmMove(q, SilentText+12, p, "", "", "", "", "", "", "", "", 6, "", Thread))
		      sprintf(Response, "1 Move failed from %s to %s", SilentText+12, p);
		    else
		    { MovePatientFromGui++;
		      strcpy(Response, "0");
		    }
		  }
		}
		}

	else if (memcmp(SilentText, "movestudy:", 10)==0)
		{
		if (p) 
		  { *p++=0;				// points after 1st comma
		    q = strchr(p, ',');
		    if (q) 
		    { *q++=0;				// points after 2nd comma
		      s1 = strchr(q, ':');			// patid optional
		      if (s1)
		      { *s1=0;
			r1 = q;
			q = s1+1;
		       }
		      else
			r1 = blank;
		      
		      if (DcmMove(r1, SilentText+10, p, q, "", "", "", "", "", "", "", 7, "", Thread))
			sprintf(Response, "1 Move failed from %s to %s", SilentText+10, p);
		      else
		      { MoveStudyFromGui++;
			strcpy(Response, "0");
		      }
		    }
		  }
		}

	else if (memcmp(SilentText, "moveaccession:", 14)==0)
		{
		if (p) 
		  { *p++=0;				// points after 1st comma
		    q = strchr(p, ',');
		    if (q) 
		    { *q++=0;				// points after 2nd comma
		      s1 = strchr(q, ':');			// patid optional
		      if (s1)
		      { *s1=0;
			r1 = q;
			q = s1+1;
		       }
		      else
			r1 = blank;
		      
		      // 7777 is a magic number
		      if (DcmMove(r1, SilentText+14, p, q, "", "", "", "", "", "", "", 7777, "", Thread))
			sprintf(Response, "1 Move failed from %s to %s", SilentText+14, p);
		      else
		      { MoveStudyFromGui++;
			strcpy(Response, "0");
		      }
		    }
		  }
		}

	else if (memcmp(SilentText, "movestudies:", 12)==0)
		{
		if (p) 
		  { *p++=0;				// points after 1st comma
		    q = strchr(p, ',');
		    if (q) 
		    { *q++=0;				// points after 2nd comma
		      if (DcmMove("", SilentText+12, p, "", "", "", "", q, "", "", "", 8, "", Thread))
			sprintf(Response, "1 Move failed from %s to %s", SilentText+12, p);
		      else
		      { MoveStudiesFromGui++;
			strcpy(Response, "0");
		      }
		    }
		  }
		}

	else if (memcmp(SilentText, "moveseries:", 11)==0)
		{
		t1 = blank;
		if (p) 
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');
		  if (q) 
		  { *q++=0;				// points after 2nd comma
		    t1 = strchr(q, ',');
		    if (t1) *t1++=0;			// points after 3rd comma
		    else t1 = blank;

		    s1 = strchr(q, ':');			// patid optional
		    if (s1)
		    {   *s1=0;
		      r1 = q;
		      q = s1+1;
		    }
		    else
		      r1 = blank;

		    if (DcmMove(r1, SilentText+11, p, t1, q, "", "", "", "", "", "", 9, "", Thread))
		      sprintf(Response, "1 Move failed from %s to %s", SilentText+11, p);
		    else
		    { MoveSeriesFromGui++;
		      strcpy(Response, "0");
		    }
		  }
		}
		}

	else if (memcmp(SilentText, "packdbf:", 8)==0)
		{
		Database DB;
		NeedPack = 3;	// pack and threaded index creation
		DB.Open ( DataSource, UserName, Password, DataHost);
		}

	else if (memcmp(SilentText, "clonedb:", 8)==0)
		{
		CloneDB(SilentText+8);
		}

	else if (memcmp(SilentText, "indexdbf:", 9)==0)
		{
		Database DB;
		NeedPack = 4;	// threaded index creation - used after full regen from GUI
		DB.Open ( DataSource, UserName, Password, DataHost);
		}

	else if (memcmp(SilentText, "browsepatient:", 14)==0)
		{
		OperatorConsole.printf("Browse patient: %s\n", SilentText+14);
		}

	else if (memcmp(SilentText, "regenfile:", 10)==0)
		RegenFile(SilentText+10);

	else if (memcmp(SilentText, "addimagefile:", 13)==0)
		{
		p = CommaInFilenameWorkAround(SilentText);
		int rc = 0;
		//p = strchr(SilentText+13, '|');
		if (p) *p++ = 0;
		if ((vr = DCO.GetVR(0x9999,0x0402)))
			{
                        int oddlength = DCO.GetUINT16(0x9999,0x0404)?1:0;
			char *p1 = strrchr(SilentText+13, '.');
			if (p1) NewTempFile(tempfile, p1);
			else    NewTempFile(tempfile, "");
			FILE *f = fopen(tempfile, "wb");
			fwrite(vr->Data, vr->Length-oddlength, 1, f);
			fclose(f);
			if (p) rc=!AddImageFile(tempfile, p, &PDU);
			else   rc=!AddImageFile(tempfile, NULL, &PDU);
			unlink(tempfile);
			tempfile[0]=0;
			}
		else
			rc=!AddImageFile(SilentText+13, p, &PDU);
		sprintf(Response, "%d", rc);
		AddedFileFromGui++;
		}

	else if (memcmp(SilentText, "loadanddeletedir:", 17)==0)
		{
		Database DB;
		if (!DB.Open ( DataSource, UserName, Password, DataHost ) )
			{
			OperatorConsole.printf("***Error Connecting to SQL\n");
			return;
			}
		p = CommaInFilenameWorkAround(SilentText);
		if (p) *p++ = 0;
		LoadAndDeleteDir(SilentText+17, p, &PDU, Thread, NULL, &DB);
		}

	else if (memcmp(SilentText, "modifypatid:", 12)==0 && p)
		{
		if (p) *p++=0;				// points after 1st comma
		if (p) ModifyPATIDofImageFile(p, SilentText+12, TRUE, NULL, &PDU);
		}

	else if (memcmp(SilentText, "modifyimage:", 12)==0 && p)
		{
		if (p) 
		{ *p++=0;				// points after 1st comma
		  char test[1024];

		  strcpy(test, p); strlwr(test);
		  if ((strstr(test, "set 0010,0020") || strstr(test, "set 0020,000d") || strstr(test, "set 0020,000e")) && strstr(test, "newuids")==0)
			  {
			  OperatorConsole.printf("[modifyimage]: can't change patientID, study or series UID without using newuids\n");
			  }
		  else
			  {
			  // used ' instead of "; detect this (set xxx to 'yyy') or (set xxx if 'yyy') and replace
			  if (strstr(test, " to '") || strstr(test, " if '"))
				for (unsigned int i=0; i<strlen(p); i++) if (p[i]=='\'') p[i] = '"';

			  ModifyImageFile(SilentText+12, p, &PDU, TRUE);
			  }
		}
		}

	else if (memcmp(SilentText, "modifystudy:", 12)==0)
		{
		if (p) 					// study
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');			// script
		  if (q) 
		  { *q++=0;				// points after 2nd comma
		  }
		}

		ModifyData(SilentText+12, p, NULL, NULL, q, &PDU, TRUE, Thread);
		}

	else if (memcmp(SilentText, "modifyseries:", 13)==0)
		{
		if (p) 					// study
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');			// script
		  if (q) 
		  { *q++=0;				// points after 2nd comma
		  }
		}

		ModifyData(SilentText+13, NULL, p, NULL, q, &PDU, TRUE, Thread);
		}

	else if (memcmp(SilentText, "modifier:", 9)==0)
		{
		char *r=NULL, *r2=NULL, *r3=NULL;
		if (p) 					// study
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');			// series
		  if (q) 
		  { *q++=0;				// points after 2nd comma
		    r = strchr(q, ',');			// sop
		    if (r) 
		    { *r++=0;				// points after 2nd comma
		       r2 = strchr(r, ',');		// deletefile
		       if (r2) 
		       { *r2++=0;
		          r3 = strchr(r2, ',');		// script
		          if (r3) 
			   *r3++=0;
		       }
		     }
		  }
		}

		ModifyData(SilentText+9, p, q, r, r3, &PDU, atoi(r2), Thread);
		}

	else if (memcmp(SilentText, "anonymize:", 10)==0 && p)
		{
		*p++=0;				// points after 1st comma

		lua_setvar(&PDU, "command_line",  SilentText+10);      
		lua_setvar(&PDU, "version",       DGATE_VERSION);

		FILE *f = fopen("anonymize_script.cq", "rt");
		if (f) 
		{ char anonscript2[] = "call anonymize_script.cq;";
		  fclose(f);
		  ModifyPATIDofImageFile(p, SilentText+10, TRUE, anonscript2, &PDU);
		}
		else if (f = fopen("anonymize_script.lua", "rt"))
		{ char anonscript2[] = "anonymize_script.lua";
		  fclose(f);
		  ModifyPATIDofImageFile(p, SilentText+10, TRUE, anonscript2, &PDU);
		}
		else
		{ char anonscript[] = 
		  "set 0010,0010 to \"anonymous\";"
		  "set 0010,0030 to \"19010101\";"
		  "set 0008,0090 to \"\";"
		  "set 0020,1020 to \"\";" 
		  "set 0051,1010 to \"\";"
		  "set 0010,1040 to \"\";"
		  "set 0009,1200 to \"\";";
		  ModifyPATIDofImageFile(p, SilentText+10, TRUE, anonscript, &PDU);
		}
		}

	else if (memcmp(SilentText, "mergeseriesfile:", 16)==0)
		MergeUIDofImageFile(SilentText+16, TRUE, "SeriesUID", "", NULL, &PDU);

	else if (memcmp(SilentText, "mergestudyfile:", 15)==0)
		MergeUIDofImageFile(SilentText+15, TRUE, "StudyUID", "", NULL, &PDU);

	else if (memcmp(SilentText, "mergeseries:", 12)==0)
		{
		char temp[128];
		int i, n=1, L;
		char *uids[1000];
		p = SilentText+12;
		uids[0]=p;
		L = strlen(p);
		for (i=0; i<L; i++)
			{
			if (p[i]==',')
				{
				p[i]=0;
				uids[n++] = p+i+1;
				if(n==1000) break;
				}
			}
		if (uids[0][0]) MergeUIDs(uids, n, "SeriesUID", temp);
		}

	else if (memcmp(SilentText, "mergestudy:", 11)==0)
		{
		char temp[128];
		int i, n=1, L;
		char *uids[1000];
		p = SilentText+11;
		uids[0]=p;
		L = strlen(p);
		for (i=0; i<L; i++)
			{
			if (p[i]==',')
				{
				p[i]=0;
				uids[n++] = p+i+1;
				if(n==1000) break;
				}
			}

		if (uids[0][0]) MergeUIDs(uids, n, "StudyUID", temp);
		}

	else if (memcmp(SilentText, "initializetables:", 17)==0)
		InitializeTables (atoi(SilentText+17));

	else if (memcmp(SilentText, "regen:", 6)==0)
		Regen();

	else if (memcmp(SilentText, "regendevice:", 12)==0)
		Regen(SilentText+12, FALSE);

	else if (memcmp(SilentText, "regendir:", 9)==0)
		{
		if (p) *p++=0;				// points after 1st comma
		Regen(SilentText+9, FALSE, p);
		}

	else if (memcmp(SilentText, "makespace:", 10)==0)
		{
		if (LargestFreeMAG()<(unsigned int)atoi(SilentText+10))
			PanicKillOff((unsigned int)atoi(SilentText+10));
		}

	else if (memcmp(SilentText, "selectlruforarchival:", 21)==0 && p)
		{
		if (p) *p++=0;				// points after 1st comma
		if (p) SelectLRUForArchival(p, atoi(SilentText+21), &PDU);
		}

	else if (memcmp(SilentText, "selectseriestomove:", 19)==0 && p)
		{
		if (p) 
		  { *p++=0;				// points after 1st comma
		    q = strchr(p, ',');
		    if (q) *q++=0;			// points after 2nd comma
		  }
		if (p && q) SelectSeriesForArchival(SilentText+19, atoi(p), atoi(q));
		}

	else if (memcmp(SilentText, "preparebunchforburning:", 23)==0 && p)
		{
		if (p) *p++=0;				// points after 1st comma
		if (p) PrepareBunchForBurning(p, SilentText+23);
		}

	else if (memcmp(SilentText, "movedatatodevice:", 17)==0 && p)
		{
		if (p) *p++=0;				// points after 1st comma
		if (p) MoveDataToDevice(p, SilentText+17);
		}

	else if (memcmp(SilentText, "moveseriestodevice:", 19)==0 && p)
		{
		if (p) *p++=0;				// points after 1st comma
		if (p) MoveSeriesToDevice(p, SilentText+19);
		}

	else if (memcmp(SilentText, "restoremagflags:", 16)==0)
		RestoreMAGFlags();

	else if (memcmp(SilentText, "comparebunchafterburning:", 25)==0)
		CompareBunchAfterBurning(SilentText+25);

	else if (memcmp(SilentText, "verifymirrordisk:", 17)==0)
		VerifyMirrorDisk(SilentText+17);

	else if (memcmp(SilentText, "testimages:", 11)==0)
		TestImages(SilentText+11);

	else if (memcmp(SilentText, "deletebunchafterburning:", 24)==0)
		DeleteBunchAfterBurning(SilentText+24);
			
	else if (memcmp(SilentText, "renamedevice:", 13)==0 && p)
		{
		if (p) *p++=0;				// points after 1st comma
		if (p) RenameDevice(SilentText+13, p);
		}

	else if (memcmp(SilentText, "testcompress:", 13)==0)
#ifdef HAVE_J2K
		TestCompress(SilentText+13, "unasn1n2n3n4j1j2j3j4j5j6jkjlk1k2k4k8", &PDU);
#else
		TestCompress(SilentText+13, "unasn1n2n3n4j1j2j3j4j5j6k1k2k4k8", &PDU);
#endif

	else if (memcmp(SilentText, "debuglevel:", 11)==0)
		DebugVRs = DebugLevel = atoi(SilentText+11);

	else if (memcmp(SilentText, "testmode:", 9)==0)
		strcpy(TestMode, SilentText+9);

	else if (memcmp(SilentText, "debuglog_on:", 12)==0)
		{
		if (SilentText[12]>='0' && SilentText[12]<='9')
			{
			SystemDebug.Off();
			OperatorConsole.Off();
			OperatorConsole.OnUDP(OCPipeName, SilentText+12);
			SystemDebug.OnUDP(OCPipeName, SilentText+12);
			OperatorConsole.AddTimeStamps(0);
			SystemDebug.AddTimeStamps(0);
			StopZipThread();
			}
		else if (SilentText[12] == PATHSEPCHAR)
			{
			SystemDebug.Off();
			OperatorConsole.Off();
			OperatorConsole.OnMsgPipe(SilentText+12);
			SystemDebug.OnMsgPipe(SilentText+12);
			OperatorConsole.AddTimeStamps(0);
			SystemDebug.AddTimeStamps(0);
			StopZipThread();
			}
		else
			{
			SystemDebug.Off();
			OperatorConsole.Off();
			OperatorConsole.On(SilentText+12);
			SystemDebug.On(SilentText+12);
			OperatorConsole.AddTimeStamps(1);
			SystemDebug.AddTimeStamps(1);
			StartZipThread();
			}
		}

	else if (memcmp(SilentText, "log_on:", 7)==0)
		{
		if (SilentText[7]>='0' && SilentText[7]<='9')
			{
			SystemDebug.Off();
			OperatorConsole.Off();
			OperatorConsole.OnUDP(OCPipeName, SilentText+7);
			OperatorConsole.AddTimeStamps(0);
			StopZipThread();
			}
		else if (SilentText[7] == PATHSEPCHAR)
			{
			SystemDebug.Off();
			OperatorConsole.Off();
			OperatorConsole.OnMsgPipe(SilentText+7);
			OperatorConsole.AddTimeStamps(0);
			StopZipThread();
			}
		else
			{
			SystemDebug.Off();
			OperatorConsole.Off();
			OperatorConsole.On(SilentText+7);
			OperatorConsole.AddTimeStamps(1);
			StartZipThread();
			}
		}

	else if (memcmp(SilentText, "read_amap:", 10)==0)
		{
		CloseACRNemaAddressArray();
		if(!InitACRNemaAddressArray())
			{
			OperatorConsole.printf("***Error loading acr-nema map file:%s\n",ACRNEMAMAP);
			}
		}

	else if (memcmp(SilentText, "read_ini:", 9)==0)
		{
		ConfigDgate();
		ConfigMicroPACS();
		}

	else if (memcmp(SilentText, "get_ini:", 8)==0)
		{
		char ps[]="%s";
		FILE *f, *g;
		char line[512];
		p = SilentText+8;
		if (*p==0) p = ps;
		f = fopen(ConfigFile,     "rt");
		NewTempFile(tempfile, ".txt");
		g = fopen(tempfile, "wt");
		while(fgets(line, sizeof(line), f) != NULL)
			fprintf(g, p, line);
		fclose(f);
		fclose(g);
		}

	else if (memcmp(SilentText, "get_param:", 10)==0)
		{
		char ps[] = "%s";
		char szRootSC[64], Parameter[512];
		if (p) *p++=0;				// points after 1st comma
		if (p==NULL) p = ps;
		if (MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile))
			{
			if (MyGetPrivateProfileString(szRootSC, SilentText+10, "", Parameter, 512, ConfigFile))
				sprintf(Response, p, Parameter);
			}

		}

	else if (memcmp(SilentText, "get_ini_param:", 14)==0)
		{
		char ps[] = "%s";
		char szRootSC[64], Parameter[512];
		if (p) *p++=0;				// points after 1st comma
		if (p==NULL) p = ps;
		if (MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile))
			{
			if (MyGetPrivateProfileString(szRootSC, SilentText+14, "", Parameter, 512, ConfigFile))
				sprintf(Response, p, Parameter);
			}

		}

	else if (memcmp(SilentText, "get_ini_num:", 12)==0)
		{
		char ps[] = "%s";
//					char szRootSC[64], Parameter[512];
		int r, i;
		FILE *f;
		char line[512];
		if (p) *p++=0;				// points after 1st comma
		if (p==NULL) p = ps;
		i = 0;
		r = atoi(SilentText+12);
		f = fopen(ConfigFile,     "rt");
		while(fgets(line, sizeof(line), f) != NULL && i <= r)
			{
			if (i == r)
				sprintf(Response, p, line);
			i++;
			}
		fclose(f);
		}

	else if (memcmp(SilentText, "put_param:", 10)==0)
		{
		FILE *f, *g;
		char line[512];
		BOOL written=FALSE;
		time_t TimeOfDay3;
		char TimeString1[128], buf[64];
		char newConfigFile[512];
		TimeOfDay3 = time(NULL);
		UNUSED_ARGUMENT(buf);//Stop gcc4.2 warning bcb
		strcpy(TimeString1, ctime_r(&TimeOfDay3, buf));
		TimeString1[strlen(TimeString1)-1] = '\0';

		if (p) *p++=0;				// points after 1st comma
		strcpy(newConfigFile, ConfigFile);
		strcat(newConfigFile, ".new");

		f = fopen(ConfigFile,     "rt");
		g = fopen(newConfigFile, "wt");

		sprintf(line, "# Written by put_param on %s\n", TimeString1);
		fputs(line, g);

		while(fgets(line, sizeof(line), f) != NULL)
			{
			if (memicmp(line, "# Written by put_param on ", strlen("# Written by put_param on "))==0)
				continue;
			if (memicmp(line, SilentText+10, strlen(SilentText+10))==0 && isspace(line[strlen(SilentText+10)]))
				{
				sprintf(line, "%-24s = %s\n", SilentText+10, p);
				written=TRUE;
				}
			fputs(line, g);
			}
		if (!written)
			{
			fputs("\n", g);
			sprintf(line, "# Parameter added by put_param on %s\n", TimeString1);
			fputs(line, g);
			sprintf(line, "%-24s = %s\n", SilentText+10, p);
			fputs(line, g);
			}

		fclose(f);
		fclose(g);
		unlink(ConfigFile);
		rename(newConfigFile, ConfigFile);
		FlushPrivateProfileStringCache();
		}

	else if (memcmp(SilentText, "delete_param:", 13)==0)
		{
		FILE *f, *g;
		char line[512];
		time_t TimeOfDay3;
		char TimeString1[128], buf[64];
		char newConfigFile[512];
		TimeOfDay3 = time(NULL);
		UNUSED_ARGUMENT(buf);//Stop gcc4.2 warning bcb
		strcpy(TimeString1, ctime_r(&TimeOfDay3, buf));
		TimeString1[strlen(TimeString1)-1] = '\0';
		strcpy(newConfigFile, ConfigFile);
		strcat(newConfigFile, ".new");

		f = fopen(ConfigFile,     "rt");
		g = fopen(newConfigFile, "wt");

		sprintf(line, "# Written by put_param on %s\n", TimeString1);
		fputs(line, g);

		while(fgets(line, sizeof(line), f) != NULL)
			{
			if (memicmp(line, "# Written by put_param on ", strlen("# Written by put_param on "))==0)
				continue;
			if (memicmp(line, SilentText+13, strlen(SilentText+13))==0 && isspace(line[strlen(SilentText+13)]))
				continue;
			fputs(line, g);
			}

		fclose(f);
		fclose(g);
		unlink(ConfigFile);
		rename(newConfigFile, ConfigFile);
		FlushPrivateProfileStringCache();
		}

	else if (memcmp(SilentText, "get_freestore:", 14)==0)
		{
		char pd[] = "%d";
		int r = -1;
		if (p) *p++=0;				// points after 1st comma
		if (p==NULL) p = pd;
		if (memicmp("MAG", SilentText+14, 3)==0) 
			r = CheckFreeStoreOnMAGDevice(atoi(SilentText+17));
		if (memicmp("CACHE", SilentText+14, 5)==0) 
			r = CheckFreeStoreOnCACHEDevice(atoi(SilentText+19));
		if (memicmp("MIRROR", SilentText+14, 6)==0) 
			r = CheckFreeStoreOnMIRRORDevice(atoi(SilentText+20));
		sprintf(Response, p, r);
		}

	else if (memcmp(SilentText, "get_amap:", 9)==0)
		{
		char pq[] = "%-17s %-30s %-10s %-16s";
		if (p) *p++=0;				// points after 1st comma
		unsigned int r = atoi(SilentText+9);
		if (p==NULL) p = pq;
		if (r < ACRNemaAddressArray.GetSize() )
			{
			ACRNemaAddress	*AAPtr = ACRNemaAddressArray.Get(r);
			sprintf(Response, p, AAPtr->Name, AAPtr->IP, AAPtr->Port, AAPtr->Compress,
					     AAPtr->Name, AAPtr->IP, AAPtr->Port, AAPtr->Compress,
					     AAPtr->Name, AAPtr->IP, AAPtr->Port, AAPtr->Compress);
			}
		}

	else if (memcmp(SilentText, "get_amaps:", 10)==0)
		{
		char pq[] = "%-17s %-30s %-10s %-16s\n";
		FILE *g;
		p = SilentText+10;
		if (*p==0) p = pq;
		NewTempFile(tempfile, ".txt");
		g = fopen(tempfile, "wt");
		for (unsigned int r=0; r< ACRNemaAddressArray.GetSize(); r++ )
			{
			ACRNemaAddress	*AAPtr = ACRNemaAddressArray.Get(r);
			if (strchr(AAPtr->Name, '*')==NULL)
			fprintf(g, p, AAPtr->Name, AAPtr->IP, AAPtr->Port, AAPtr->Compress,
				      AAPtr->Name, AAPtr->IP, AAPtr->Port, AAPtr->Compress,
				      AAPtr->Name, AAPtr->IP, AAPtr->Port, AAPtr->Compress);
			}
		fclose(g);
		}

	else if (memcmp(SilentText, "write_amap:", 11)==0)
		{
		char pq[]="%-17s %-30s %-10s %-16s\n";
		unsigned int r = 0;
		if (p) *p++=0;				// points after 1st comma
		p = pq;
		time_t TimeOfDay3;
		char TimeString1[128], buf[64];
		TimeOfDay3 = time(NULL);
		UNUSED_ARGUMENT(buf);//Stop gcc4.2 warning bcb
		strcpy(TimeString1, ctime_r(&TimeOfDay3, buf));
		TimeString1[strlen(TimeString1)-1] = '\0';

		FILE *f = fopen((const char *)ACRNEMAMAP, "wt");
		fprintf(f, "/* **********************************************************\n");
		fprintf(f, " *                                                          *\n");
		fprintf(f, " * DICOM AE (Application entity) -> IP address / Port map   *\n");
		fprintf(f, " * (This is file ACRNEMA.MAP)                               *\n");
		fprintf(f, " * Written by write_amap on %-32s*\n", TimeString);
		fprintf(f, " *                                                          *\n");
		fprintf(f, " * All DICOM systems that want to retrieve images from the  *\n");
		fprintf(f, " * Conquest DICOM server must be listed here with correct   *\n");
		fprintf(f, " * AE name, (IP adress or hostname) and port number.        *\n");
		fprintf(f, " * The first entry is the Conquest system as example.       *\n");
		fprintf(f, " *                                                          *\n");
		fprintf(f, " *                                                          *\n");
		fprintf(f, " * The syntax for each entry is :                           *\n");
		fprintf(f, " *   AE   <IP adress|Host name>   port number   compression *\n");
		fprintf(f, " *                                                          *\n");
		fprintf(f, " * For compression see manual. Values are un=uncompressed;  *\n");
		fprintf(f, " * ul=littleendianexplicit,ub=bigendianexplicit,ue=both     *\n");
		fprintf(f, " * j1,j2=lossless jpeg;j3..j6=lossy jpeg;n1..n4=nki private *\n");
#ifdef HAVE_J2K
		fprintf(f, " * jk = jpeg2000 lossless, jl = jpeg2000 lossy              *\n");
#endif
		fprintf(f, " *                                                          *\n");
		fprintf(f, " ********************************************************** */\n");
		fprintf(f, "\n");
		while (r < ACRNemaAddressArray.GetSize() )
			{
			ACRNemaAddress	*AAPtr = ACRNemaAddressArray.Get(r);
			fprintf(f, p, AAPtr->Name, AAPtr->IP, AAPtr->Port, AAPtr->Compress);
			r++;
			}
		fclose(f);
		}

	else if (memcmp(SilentText, "put_amap:", 9)==0)
		{
		int i, n=1, L;
		char *items[5];
		ACRNemaAddress	*AAPtr;

		memset(items, 0, sizeof(items));

		p = SilentText+9;
		items[0]=p;
		L = strlen(p);
		for (i=0; i<L; i++)
			{
			if (p[i]==',')
				{
				p[i]=0;
				items[n++] = p+i+1;
				if(n==5) break;
				}
			}

		unsigned int r = atoi(items[0]);
		if (r < ACRNemaAddressArray.GetSize() )
			AAPtr = ACRNemaAddressArray.Get(r);
		else
			{
			AAPtr = new ACRNemaAddress;
			strcpy(AAPtr->Name, "");
			strcpy(AAPtr->IP, "");
			strcpy(AAPtr->Port, "");
			strcpy(AAPtr->Compress, "un");
			ACRNemaAddressArray.Add(AAPtr);
			r = ACRNemaAddressArray.GetSize()-1;
			}
			
		if (items[1]) strcpy(AAPtr->Name,     items[1]);
		if (items[2]) strcpy(AAPtr->IP,       items[2]);
		if (items[3]) strcpy(AAPtr->Port,     items[3]);
		if (items[4]) strcpy(AAPtr->Compress, items[4]);				
		}

	else if (memcmp(SilentText, "delete_amap:", 12)==0)
		{
		ACRNemaAddress	*AAPtr;

		unsigned int r = atoi(SilentText+12);
		if (r < ACRNemaAddressArray.GetSize() )
			{
			AAPtr = ACRNemaAddressArray.Get(r);
			delete AAPtr;
			ACRNemaAddressArray.RemoveAt(r);
			}
		}

	else if (memcmp(SilentText, "get_sqldef:", 11)==0)
		{
		char pq[] = "0x%4.4x, 0x%4.4x %20s %4d %10s %12s";
		int r=0;
		if (p) 
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');
		  if (q) *q++=0;			// points after 2nd comma
		}

		if (p) r = atoi(p);
		if (q==NULL) q = pq;
		DBENTRY	*DBE = NULL;
		if (stricmp(SilentText+11, "patient" )==0) DBE = PatientDB;
		if (stricmp(SilentText+11, "study"   )==0) DBE = StudyDB;
		if (stricmp(SilentText+11, "series"  )==0) DBE = SeriesDB;
		if (stricmp(SilentText+11, "image"   )==0) DBE = ImageDB;
		if (stricmp(SilentText+11, "worklist")==0) DBE = WorkListDB;
		if (DBE)
			if (DBE[r].Group)
				sprintf(Response, q, DBE[r].Group, DBE[r].Element, DBE[r].SQLColumn,
				DBE[r].SQLLength, SQLTypeSymName(DBE[r].SQLType), DICOMTypeSymName(DBE[r].DICOMType));
		}

	else if (memcmp(SilentText, "get_sop:", 8)==0)
		{
		char ps2[]="%s %s";
		if (p) *p++=0;				// points after 1st comma
		unsigned int r = atoi(SilentText+8);
		if (p==NULL) p = ps2;
		if (r<PDU.SOPUIDListCount)
			sprintf(Response, p, PDU.SOPUIDList[r], PDU.SOPUIDListNames[r]);
		}

	else if (memcmp(SilentText, "put_sop:", 8)==0 ||
		 memcmp(SilentText, "put_transfer:", 13)==0 ||
		 memcmp(SilentText, "put_localae:", 12)==0 ||
		 memcmp(SilentText, "put_remoteae:", 13)==0 ||
		 memcmp(SilentText, "put_application:", 16)==0
		)
		{
		unsigned int r = atoi(strchr(SilentText, ':')+1);
		char **List, **ListNames;
		unsigned int *Count;
#ifdef __GNUC__ //Warnings
		List = NULL;
		ListNames = NULL;
		Count = NULL;
#endif
		if (p) 
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');
		  if (q) *q++=0;			// points after 2nd comma
		}

		if (memcmp(SilentText, "put_sop:", 8)==0)
			{
			Count = &PDU.SOPUIDListCount;
			List  = PDU.SOPUIDList;
			ListNames = PDU.SOPUIDListNames;
			}
		else if (memcmp(SilentText, "put_transfer:", 13)==0)
			{
			Count = &PDU.TransferUIDListCount;
			List  = PDU.TransferUIDList;
			ListNames = PDU.TransferUIDListNames;
			}
		else if (memcmp(SilentText, "put_application:", 16)==0)
			{
			Count = &PDU.ApplicationUIDListCount;
			List  = PDU.ApplicationUIDList;
			ListNames = PDU.ApplicationUIDListNames;
			}
		else if (memcmp(SilentText, "put_localae:", 12)==0)
			{
			Count = &PDU.LocalAEListCount;
			List  = PDU.LocalAEList;
			ListNames = PDU.LocalAEListNames;
			}
		else if (memcmp(SilentText, "put_remoteae:", 12)==0)
			{
			Count = &PDU.RemoteAEListCount;
			List  = PDU.RemoteAEList;
			ListNames = PDU.RemoteAEListNames;
			}

		if (r<*Count)
			{
			delete List[r];
			delete ListNames[r];
			}
		else
			r = *Count++;
		
		List[r] = new char[strlen(p)+1];
		strcpy(List[r], p);
		ListNames[r] = new char[strlen(q)+1];
		strcpy(ListNames[r], q);

		time_t TimeOfDay3;
		char TimeString1[128], buf[64];
		UNUSED_ARGUMENT(buf);//Stop gcc4.2 warning bcb
		TimeOfDay3 = time(NULL);
		strcpy(TimeString1, ctime_r(&TimeOfDay3, buf));
		TimeString1[strlen(TimeString1)-1] = '\0';

		FILE *f = fopen(SOPClassFile, "wt");
		fprintf(f, "#\n");
		fprintf(f, "# DICOM Application / sop / transfer UID list.\n");
		fprintf(f, "#\n");
		fprintf(f, "# This list is used by the server (CheckedPDU_Service class).\n");
		fprintf(f, "# All incoming associations are verified against this file.\n");
		fprintf(f, "# File written by %s on: %s\n", SilentText, TimeString1);
		fprintf(f, "#\n");

		for (r=0; r<PDU.RemoteAEListCount; r++)
			fprintf(f, "%-42s %-44s RemoteAe\n", PDU.RemoteAEListNames[r], PDU.RemoteAEList[r]);
		for (r=0; r<PDU.LocalAEListCount; r++)
			fprintf(f, "%-42s %-44s LocalAe\n", PDU.LocalAEListNames[r], PDU.LocalAEList[r]);
		for (r=0; r<PDU.ApplicationUIDListCount; r++)
			fprintf(f, "%-42s %-44s application\n", PDU.ApplicationUIDListNames[r], PDU.ApplicationUIDList[r]);
		for (r=0; r<PDU.SOPUIDListCount; r++)
			fprintf(f, "%-42s %-44s sop\n", PDU.SOPUIDListNames[r], PDU.SOPUIDList[r]);
		for (r=0; r<PDU.TransferUIDListCount; r++)
			fprintf(f, "%-42s %-44s transfer\n", PDU.TransferUIDListNames[r], PDU.TransferUIDList[r]);
		fclose(f);
		}

	else if (memcmp(SilentText, "delete_sop:", 11)==0 ||
		 memcmp(SilentText, "delete_transfer:", 16)==0 ||
		 memcmp(SilentText, "delete_localae:", 15)==0 ||
		 memcmp(SilentText, "delete_remoteae:", 16)==0 ||
		 memcmp(SilentText, "delete_application:", 19)==0
		)
		{
		unsigned int r = atoi(strchr(SilentText, ':')+1), i;
		char **List, **ListNames;
		unsigned int *Count;
#ifdef __GNUC__ //Warnings
		List = NULL;
		ListNames = NULL;
		Count = NULL;
#endif
			
		if (memcmp(SilentText, "delete_sop:", 11)==0)
			{
			Count = &PDU.SOPUIDListCount;
			List  = PDU.SOPUIDList;
			ListNames = PDU.SOPUIDListNames;
			}
		else if (memcmp(SilentText, "delete_transfer:", 16)==0)
			{
			Count = &PDU.TransferUIDListCount;
			List  = PDU.TransferUIDList;
			ListNames = PDU.TransferUIDListNames;
			}
		else if (memcmp(SilentText, "delete_application:", 19)==0)
			{
			Count = &PDU.ApplicationUIDListCount;
			List  = PDU.ApplicationUIDList;
			ListNames = PDU.ApplicationUIDListNames;
			}
		else if (memcmp(SilentText, "delete_localae:", 15)==0)
			{
			Count = &PDU.LocalAEListCount;
			List  = PDU.LocalAEList;
			ListNames = PDU.LocalAEListNames;
			}
		else if (memcmp(SilentText, "delete_remoteae:", 16)==0)
			{
			Count = &PDU.RemoteAEListCount;
			List  = PDU.RemoteAEList;
			ListNames = PDU.RemoteAEListNames;
			}

		if (r<*Count)
			{
			delete List[r];
			delete ListNames[r];
			for (i=r+1; i<*Count; i++)
				{
				List[i+1] = List[i];
				ListNames[i+1] = ListNames[i];
				}
			(*Count)--;
			}

		time_t TimeOfDay3;
		char TimeString1[128], buf[64];
		TimeOfDay3 = time(NULL);
		UNUSED_ARGUMENT(buf);//Stop gcc4.2 warning bcb
		strcpy(TimeString1, ctime_r(&TimeOfDay3, buf));
		TimeString1[strlen(TimeString1)-1] = '\0';

		FILE *f = fopen(SOPClassFile, "wt");
		fprintf(f, "#\n");
		fprintf(f, "# DICOM Application / sop / transfer UID list.\n");
		fprintf(f, "#\n");
		fprintf(f, "# This list is used by the server (CheckedPDU_Service class).\n");
		fprintf(f, "# All incoming associations are verified against this file.\n");
		fprintf(f, "# File written by %s on: %s\n", SilentText, TimeString1);
		fprintf(f, "#\n");

		for (r=0; r<PDU.RemoteAEListCount; r++)
			fprintf(f, "%-42s %-44s RemoteAe\n", PDU.RemoteAEListNames[r], PDU.RemoteAEList[r]);
		for (r=0; r<PDU.LocalAEListCount; r++)
			fprintf(f, "%-42s %-44s LocalAe\n", PDU.LocalAEListNames[r], PDU.LocalAEList[r]);
		for (r=0; r<PDU.ApplicationUIDListCount; r++)
			fprintf(f, "%-42s %-44s application\n", PDU.ApplicationUIDListNames[r], PDU.ApplicationUIDList[r]);
		for (r=0; r<PDU.SOPUIDListCount; r++)
			fprintf(f, "%-42s %-44s sop\n", PDU.SOPUIDListNames[r], PDU.SOPUIDList[r]);
		for (r=0; r<PDU.TransferUIDListCount; r++)
			fprintf(f, "%-42s %-44s transfer\n", PDU.TransferUIDListNames[r], PDU.TransferUIDList[r]);
		fclose(f);
		}

	else if (memcmp(SilentText, "get_transfer:", 13)==0)
		{
		char ps2[]="%s %s";
		if (p) *p++=0;				// points after 1st comma
		unsigned int r = atoi(SilentText+13);
		if (p==NULL) p = ps2;
		if (r<PDU.TransferUIDListCount)
			sprintf(Response, p, PDU.TransferUIDList[r], PDU.TransferUIDListNames[r]);
		}

	else if (memcmp(SilentText, "get_application:", 16)==0)
		{
		char ps2[]="%s %s";
		if (p) *p++=0;				// points after 1st comma
		unsigned int r = atoi(SilentText+16);
		if (p==NULL) p = ps2;
		if (r<PDU.ApplicationUIDListCount)
			sprintf(Response, p, PDU.ApplicationUIDList[r], PDU.ApplicationUIDListNames[r]);
		}

	else if (memcmp(SilentText, "get_localae:", 12)==0)
		{
		char ps2[]="%s %s";
		if (p) *p++=0;				// points after 1st comma
		unsigned int r = atoi(SilentText+12);
		if (p==NULL) p = ps2;
		if (r<PDU.LocalAEListCount)
			sprintf(Response, p, PDU.LocalAEList[r], PDU.LocalAEListNames[r]);
		}

	else if (memcmp(SilentText, "get_remoteae:", 13)==0)
		{
		char ps2[]="%s %s";
		if (p) *p++=0;				// points after 1st comma
		unsigned int r = atoi(SilentText+13);
		if (p==NULL) p = ps2;
		if (r<PDU.RemoteAEListCount)
			sprintf(Response, p, PDU.RemoteAEList[r], PDU.RemoteAEListNames[r]);
		}

	else if (memcmp(SilentText, "get_dic:", 8)==0)
		{
		char ps3[]="%04x %04x %c%c %s";
		if (p) *p++=0;				// points after 1st comma
		unsigned int r = atoi(SilentText+8);
		if (p==NULL) p = ps3;
		if (r<VRType.TypeCodes->GetSize())
			{
			RTCElement *e;
			e = &(VRType.TypeCodes->Get(r));
			sprintf(Response, p, VRType.TypeCodes->Get(r).Group, VRType.TypeCodes->Get(r).Element, VRType.TypeCodes->Get(r).TypeCode>>8, VRType.TypeCodes->Get(r).TypeCode&255, VRType.TypeCodes->Get(r).Description);
			}
		}

	else if (memcmp(SilentText, "mk_binary_dic:", 14)==0)
		MkBinaryRtc(DicomDict, SilentText+14, TRUE);

	else if (memcmp(SilentText, "dump_header:", 12)==0)
		{ 
		FILE *f;
		if (p) *p++=0;				// points after 1st comma
		if (p==NULL || *p==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wt");
			}
		else if (strcmp(p, "cgi")==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wt");
			fprintf(f, "Content-type: text/html\n\n<pre>");
			}
		else if (strcmp(p, "pre")==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wt");
			fprintf(f, "<pre>");
			}
		else if (strcmp(p, "binary")==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wb");
			}
		else
			f = fopen(p, "wt");
		DICOMDataObject *pDDO = LoadForGUI(SilentText+12);
		if (pDDO) NonDestructiveDumpDICOMObject(pDDO, f);
		if (f) fclose(f);
		if (pDDO) delete pDDO;
		DumpHeaderFromGui++;
		}

	else if (memcmp(SilentText, "display_status:", 15)==0)
		{ 
		FILE *f;
		p = SilentText+15;
		if (p==NULL || *p==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wt");
			}
		else if (strcmp(p, "cgi")==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wt");
			fprintf(f, "Content-type: text/html\n\n<pre>");
			}
		else if (strcmp(p, "pre")==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wt");
			fprintf(f, "<pre>");
			}
		else if (strcmp(p, "binary")==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wb");
			}
		else
			f = fopen(p, "wt");
		StatusDisplay(f);
		if (f) fclose(f);
		}

	else if (memcmp(SilentText, "status_string:", 14)==0)
		{ 
		FILE *f;
		p = SilentText+14;
		if (p==NULL || *p==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wt");
			}
		else if (strcmp(p, "cgi")==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wt");
			fprintf(f, "Content-type: text/plain\n\n<pre>");
			}
		else if (strcmp(p, "pre")==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wt");
			fprintf(f, "<pre>");
			}
		else if (strcmp(p, "binary")==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wb");
			}
		else
			f = fopen(p, "wt");
		fprintf(f, "%s", StatusString);
		if (strcmp(p, "cgi")==0) // 20120829
			{ 
			fputc(' ', f);
			strcpy(SilentText, "text/plain");
			}
		if (f) fclose(f);
		}

	else if (memcmp(SilentText, "echo:", 5)==0)
		{ 
		FILE *f;
		if (p) *p++=0;	// points after 1st comma (file)
		if (p==NULL || *p==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wt");
			}
		else if (strcmp(p, "cgi")==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wt");
			fprintf(f, "Content-type: text/html\n\n<pre>");
			}
		else if (strcmp(p, "pre")==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wt");
			fprintf(f, "<pre>");
			}
		else if (strcmp(p, "binary")==0)
			{
			NewTempFile(tempfile, ".txt");
			f = fopen(tempfile, "wb");
			}
		else
			f = fopen(p, "wt");
		if (DcmEcho(SilentText+5))
			fprintf(f, "%s is UP", SilentText+5);
		else
			fprintf(f, "%s is DOWN", SilentText+5);
		if (f) fclose(f);
		}

	else if (memcmp(SilentText, "forward:", 8)==0)
		{
		if (p) 
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');
		  if (q) 
		  { *q++=0; 				// points after 2nd comma
		    TestForward(SilentText+8, p, q, &PDU);
		    ForwardFromGui++;
		  }
		}
		}

	else if (memcmp(SilentText, "convert_to_gif:", 15)==0 || memcmp(SilentText, "convert_to_bmp:", 15)==0 || memcmp(SilentText, "convert_to_jpg:", 15)==0)
		{
		int level, window;
		unsigned int frame;
		DICOMDataObject *pDDO;
		if (p) 
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');
		  if (q) 
		  { *q++=0;				// points after 2nd comma
		    r1 = strchr(q, ',');
		    if (r1) *r1++=0;			// points after 3rd comma
		  }
		}

		pDDO = LoadForGUI(SilentText+15);
		if (pDDO) 
			{
			char p256[]="256";
			//char p32[]="32";
			char p00[]="0/0";
			char p0[]="0";
			if (p==NULL)    p = p256;
			//if (atoi(p)==0) p = p32;
			if (r1==NULL)    r1 = p00;
			level  = atoi(r1);
			r1 = strchr(r1, '/');
			if (r1==NULL)    r1 = p0; else r1++;
			window = atoi(r1);
			r1 = strchr(r1, '/');
			if (r1==NULL)    r1 = p0; else r1++;
			frame = (unsigned int)atoi(r1);

			if (memcmp(SilentText, "convert_to_gif:", 15)==0)
				{
				if (q==NULL || *q==0)
					{
					NewTempFile(tempfile, ".gif");
					ToGif(pDDO, tempfile, atoi(p), 0, level, window, frame, 1);
					}
				else if (strcmp(q, "binary")==0)
					{
					NewTempFile(tempfile, ".gif");
					ToGif(pDDO, tempfile, atoi(p), 0, level, window, frame, 1);
					}
				else if (strcmp(q, "cgi")==0)
					{
					NewTempFile(tempfile, ".gif");
					FILE *f = fopen(tempfile, "wb");
					fprintf(f, "Content-type: image/gif\n\n");
					fclose(f);
					ToGif(pDDO, tempfile, atoi(p), 1, level, window, frame, 1);
					}
				else
					ToGif(pDDO, q, atoi(p), 0, level, window, frame, 1);
				ImagesToGifFromGui++;
				}
			if (memcmp(SilentText, "convert_to_bmp:", 15)==0)
				{
				if (q==NULL || *q==0)
					{
					NewTempFile(tempfile, ".bmp");
					ToBMP(pDDO, tempfile, atoi(p), 0, level, window, frame, 1);
					}
				else if (strcmp(q, "binary")==0)
					{
					NewTempFile(tempfile, ".bmp");
					ToGif(pDDO, tempfile, atoi(p), 0, level, window, frame, 1);
					}
				else if (strcmp(q, "cgi")==0)
					{
					NewTempFile(tempfile, ".bmp");
					FILE *f = fopen(tempfile, "wb");
					fprintf(f, "Content-type: image/bmp\n\n");
					fclose(f);
					ToBMP(pDDO, tempfile, atoi(p), 1, level, window, frame, 1);
					}
				else
					ToBMP(pDDO, q, atoi(p), 0, level, window, frame, 1);
				ImagesToBmpFromGui++;
				}
			if (memcmp(SilentText, "convert_to_jpg:", 15)==0)
				{
				if (q==NULL || *q==0)
					{
					NewTempFile(tempfile, ".jpg");
					ToJPG(pDDO, tempfile, atoi(p), 0, level, window, frame, 95, 1);
					}
				else if (strcmp(q, "binary")==0)
					{
					NewTempFile(tempfile, ".jpg");
					ToGif(pDDO, tempfile, atoi(p), 0, level, window, frame, 1);
					}
				else if (strcmp(q, "cgi")==0)
					{
					NewTempFile(tempfile, ".jpg");
					FILE *f = fopen(tempfile, "wb");
					fprintf(f, "Content-type: image/jpeg\n\n");
					fclose(f);
					ToJPG(pDDO, tempfile, atoi(p), 1, level, window, frame, 95, 1);
					}
				else
					ToJPG(pDDO, q, atoi(p), 0, level, window, frame, 95, 1);
				ImagesToJpgFromGui++;
				}
			delete pDDO;
			}
		}

	else if (memcmp(SilentText, "convert_to_dicom:", 17)==0)
		{
		DICOMDataObject *pDDO;
		if (p) 
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');
		  if (q)  
		  { *q++=0;				// points after 2nd comma
		    r1 = strchr(q, ',');
		    if (r1) *r1++=0;			// points after 3nd comma
		  }
		}
		pDDO = LoadForGUI(SilentText+17);

		if (pDDO) 
			{
			char un[]="un";
			char p0[]="0";
			if (p==NULL) p = p0;
			if (q==NULL) q = un;
			NewTempFile(tempfile, ".dcm");
			if (r1) ExtractFrame(pDDO, (unsigned int)atoi(r1));
			MaybeDownsize(pDDO, NULL, atoi(p));
			recompress(&pDDO, q, "", q[0]=='n' || q[0]=='N', &PDU);
			SaveDICOMDataObject(tempfile, pDDO);
			ImagesToDicomFromGui++;
			delete pDDO;
			}
		}

	else if (memcmp(SilentText, "convert_to_json:", 16)==0)
		{
		char *r2=NULL;
		DICOMDataObject *pDDO;
		if (p) 
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');
		  if (q)  
		  { *q++=0;				// points after 2nd comma
		    r1 = strchr(q, ',');
		    if (r1) 
		    { *r1++=0;				// points after 3nd comma
	               r2 = strchr(r1, ',');
	               if (r2) *r2++=0;
		    }
		  }
		}
		pDDO = LoadForGUI(SilentText+16);

		if (pDDO) 
			{
			char un[]="un";
			char p0[]="0";
			if (p==NULL) p = p0;
			if (q==NULL) q = un;
			NewTempFile(tempfile, ".json");
			if (r1 && r2) ExtractFrame(pDDO, (unsigned int)atoi(r1));
			if (r2) MaybeDownsize(pDDO, NULL, atoi(p));
			if (r2) recompress(&pDDO, q, "", q[0]=='n' || q[0]=='N', &PDU);
			char *script="lua:local r=Data:Serialize(true,%s);local f=io.open([[%s]], 'wt');f:write(r);f:close()";
			char script2[1024];
			sprintf(script2, script, r2&&atoi(r2)?"true":"false", tempfile);
		        CallImportConverterN(NULL, pDDO, -1, NULL, NULL, NULL, NULL, &PDU, NULL, script2);
			delete pDDO;
			}
		}

	else if (memcmp(SilentText, "uncompress:", 11)==0)
		{
		DICOMDataObject *pDDO;
		if (p) 
		  *p++=0;				// points after 1st comma
		pDDO = LoadForGUI(SilentText+11);

		if (pDDO) 
			{
			char un[]="un";
			recompress(&pDDO, un, "", FALSE, &PDU);
			SaveDICOMDataObject(p, pDDO);
			ImagesToDicomFromGui++;
			delete pDDO;
			}
		}

	else if (memcmp(SilentText, "compress:", 9)==0)
		{
		DICOMDataObject *pDDO;
		if (p) 
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');
		  if (q) 
		    *q++=0;				// points after 2nd comma
		}
		pDDO = LoadForGUI(SilentText+9);

		if (pDDO) 
			{
			if (p) recompress(&pDDO, p, "", p[0]=='n' || p[0]=='N', &PDU);
			if (q) SaveDICOMDataObject(q, pDDO);
			ImagesToDicomFromGui++;
			delete pDDO;
			}
		}

	else if (memcmp(SilentText, "extract_frames:", 15)==0)
		{
		DICOMDataObject *pDDO;
		if (p) 
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');
		  if (q) 
		  { *q++=0;				// points after 2nd comma
		    r1 = strchr(q, ',');
		    if (r1) *r1++=0;			// points after 3nd comma
		  }
		}
		pDDO = LoadForGUI(SilentText+17);
		if (pDDO) 
			{
			if (q==NULL) q = blank;
			if (r1==NULL) r1 = q;
			ExtractFrames(pDDO, (unsigned int)atoi(q), (unsigned int)atoi(r1), 1);
			SaveDICOMDataObject(p, pDDO);
			delete pDDO;
			}
		}

	else if (memcmp(SilentText, "wadorequest:", 12)==0 || memcmp(SilentText, "wadoparse:", 10)==0)
		{
		DICOMDataObject *pDDO;
		char *begin = SilentText+12;
		if (memcmp(SilentText, "wadoparse:", 10)==0)
			{	
			begin = SilentText+10;
	
			char studyUID[256];
			cgi_parse(begin, studyUID,   "studyUID",    "");
			
			char seriesUID[256];
			cgi_parse(begin, seriesUID,   "seriesUID",    "");
			
			char objectUID[256];
			cgi_parse(begin, objectUID,   "objectUID",    "");
			
			char contentType[256];
			cgi_parse(begin, contentType,   "contentType",    "");
			
			char rows[256];
			cgi_parse(begin, rows,   "rows",    "");
			
			char columns[256];
			cgi_parse(begin, columns,   "columns",    "");
			
			char region[256];
			cgi_parse(begin, region,   "region",    "");
			for (unsigned int i=0; i<strlen(region); i++) if (region[i]==',') region[i] = '/';
			
			char windowCenter[256];
			cgi_parse(begin, windowCenter,   "windowCenter",    "");
			
			char windowWidth[256];
			cgi_parse(begin, windowWidth,   "windowWidth",    "");
			
			char frameNumber[256];
			cgi_parse(begin, frameNumber,   "frameNumber",    "");
			
			char transferSyntax[256];
			cgi_parse(begin, transferSyntax,   "transferSyntax",    "");
			
			char anonymize[256];
			cgi_parse(begin, anonymize,   "anonymize",    "");
		
			char imageQuality[256];
			cgi_parse(begin, imageQuality,   "imageQuality",    "");
			
	
			char presentationUID[256];
			cgi_parse(begin, presentationUID,   "presentationUID",    ""); // ignored for now
			
			char bridge[256];
			cgi_parse(bridge, bridge,   "bridge",    "");
			
			char obj[256];
			sprintf(obj, "%s\\%s\\%s", studyUID, seriesUID, objectUID);
			
			char lwfq[256];
			sprintf(lwfq, "%d/%d/%d/%d", atoi(windowCenter), atoi(windowWidth), atoi(frameNumber), atoi(imageQuality));
			
			char size[256];
			sprintf(size, "%d/%d", atoi(rows), atoi(columns));
			
			char command[512];
			sprintf(command, "%s,%s,%s,%s,%s,%s,%s,%s,%s", obj, lwfq, size, region, contentType, transferSyntax, anonymize, "", bridge);
			p = strchr(command, ',');
			begin = command;
			}

		char *r2=NULL, *r3=NULL, *r4=NULL, *r5=NULL, *r6=NULL, *r7=NULL;
		if (p) 
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');
		  if (q) 
		  { *q++=0;				// points after 2nd comma
		    r1 = strchr(q, ',');
		    if (r1) 
		    { *r1++=0;			          // points after 3rd comma
		      r2 = strchr(r1, ',');
		      if (r2) 
		      { *r2++=0;			// points after 4th comma
			r3 = strchr(r2, ',');
			if (r3) 
			{ *r3++=0;			// points after 5th comma
			  r4 = strchr(r3, ',');
			  if (r4) 
			  { *r4++=0;
			    r5 = strchr(r4, ',');
			    if (r5) 
			    { *r5++=0;
			      r6 = strchr(r5, ',');
			      if (r6)
			      { *r6++=0;
 			        r7 = strchr(r6, ',');
			        if (r7)
			        { *r7++=0;
			        }
			      }
			    }
			  }
			}
		      }
		    }
		  }
		}

		// sprintf(command, "wadorequest:%s,%s,%s,%s,%s,%s,%s,%s,%s", 
		// obj, lwfq, size, region, contentType, transferSyntax, anonymize, annotation, bridge);

		int level=0, window=0, frame=0, quality=0;
		if (p) sscanf(p, "%d/%d/%d/%d", &level, &window, &frame, &quality);

		int rows=0, cols=0;
		if (q) sscanf(q, "%d/%d", &rows, &cols);
		int size = rows>cols?rows:cols;
		if (size==0) size=1600;

		// experimental bitmap strip, will likely be removed
		if (r2 && strcmp(r2, "image/bmp")==0 && frame<0)
			{
			char *stud, *ser, *sop;
			ser = strchr(begin, '\\');
			if (ser) sop = strchr(ser+1, '\\');
			if (ser && sop)
				{
				*sop++= 0;
				*ser++= 0;
				stud  = begin;

				if(r7)
				  strcpy(tempfile, r7);
			        else
				  NewTempFile(tempfile, ".bmp");
				FILE *f = fopen(tempfile, "wb");
				fprintf(f, "Content-type: image/bmp\nAccess-Control-Allow-Origin: *\n\n");
				fclose(f);
				ImageStripforSeries(stud, ser, tempfile, size, 1, level, window, 0, -frame);
				}
			if (r7) tempfile[0]=0;
			return;
			}

		// experimental jpg strip, will likely be removed
		if (r2 && strcmp(r2, "image/jpeg")==0 && frame<0)
			{
			char *stud, *ser, *sop;
			ser = strchr(begin, '\\');
			if (ser) sop = strchr(ser+1, '\\');
			if (ser && sop)
				{
				*sop++= 0;
				*ser++= 0;
				stud  = begin;

				if(r7)
				  strcpy(tempfile, r7);
			        else
				  NewTempFile(tempfile, ".jpg");
				FILE *f = fopen(tempfile, "wb");
				fprintf(f, "Content-type: image/bmp\nAccess-Control-Allow-Origin: *\n\n");
				fclose(f);
				ImageStripforSeries(stud, ser, tempfile, size, 1, level, window, 1, -frame);
				}
			if (r7) tempfile[0]=0;
			return;
			}

		if (r6==NULL || *r6==0)
			pDDO = LoadForGUI(begin);
		else
			pDDO = LoadForBridge(begin, r6);
		if (pDDO) 
			{
			int nf = GetNumberOfFrames(pDDO);
			if (r1 && *r1)
				{
				int startx, endx, starty, endy;
				float sx, ex, sy, ey;
				int changed;
				sscanf(r1, "%f/%f/%f/%f", &sx, &ex, &sy, &ey);
				startx = (int)(sx * pDDO->GetUINT16(0x0028, 0x0011)+0.5);
				endx   = (int)(ex * pDDO->GetUINT16(0x0028, 0x0011)+0.5);
				starty = (int)(sy * pDDO->GetUINT16(0x0028, 0x0010)+0.5);
				endy   = (int)(ey * pDDO->GetUINT16(0x0028, 0x0010)+0.5);
				DecompressImage(&pDDO, &changed);
				DcmConvertPixelData(pDDO, FALSE, TRUE, startx, endx, starty, endy, 0.0, 0.0, 0.0);
				}
			if (strcmp(r4, "yes")==0)
				CallImportConverterN(NULL, pDDO, -1, NULL, NULL, NULL, NULL, &PDU, NULL, "call anonymize_script.cq");
			else if (strlen(r4)>0)
				{ 
				char comm[512];
				sprintf(comm, "lua/anonymize_script.lua(%s)", r4);
				CallImportConverterN(NULL, pDDO, -1, NULL, NULL, NULL, NULL, &PDU, NULL, comm);
				}
			// AnnotateDICOMObject(pDDO, r5);
			if (r2 && strcmp(r2, "image/gif")==0 && nf<2)
				{
				if(r7)
				  strcpy(tempfile, r7);
			        else
				  NewTempFile(tempfile, ".gif");
				FILE *f = fopen(tempfile, "wb");
				fprintf(f, "Content-type: image/gif\nAccess-Control-Allow-Origin: *\n\n");
				fclose(f);
				ToGif(pDDO, tempfile, size, 1, level, window, frame, 1);
				}
			if (r2 && strcmp(r2, "image/bmp")==0 && nf<2)
				{
				if(r7)
				  strcpy(tempfile, r7);
			        else
  				  NewTempFile(tempfile, ".bmp");
				FILE *f = fopen(tempfile, "wb");
				fprintf(f, "Content-type: image/bmp\nAccess-Control-Allow-Origin: *\n\n");
				fclose(f);
				ToBMP(pDDO, tempfile, size, 1, level, window, frame, 1);
				}
			if (r2 && strcmp(r2, "image/gif")==0 && nf>=2)
				{
				if(r7)
				  strcpy(tempfile, r7);
			        else
				  NewTempFile(tempfile, ".gif");
				FILE *f = fopen(tempfile, "wb");
				fprintf(f, "Content-type: image/gif\nAccess-Control-Allow-Origin: *\n\n");
				fclose(f);
				ToGif(pDDO, tempfile, size, 1, level, window, 10000+5, 1);
				}
			if (r2 && strcmp(r2, "image/jpeg")==0 || strcmp(r2, "image/png")==0 || strcmp(r2, "image/jp2")==0)
				{
				if(r7)
				  strcpy(tempfile, r7);
			        else
				  NewTempFile(tempfile, ".jpg");
				FILE *f = fopen(tempfile, "wb");
				// ODD HEADER LENGHT AVOIDED HERE
				fprintf(f, "Content-type: image/jpeg\nAccess-Control-Allow-Origin:*\n\n");
				fclose(f);
				ToJPG(pDDO, tempfile, size, 1, level, window, frame, 95, 1);
				}
			if (r2 && strcmp(r2, "application/dicom")==0)
				{
				if(r7)
				  strcpy(tempfile, r7);
			        else
				  NewTempFile(tempfile, ".dcm");
				     if (strcmp(r3, "1.2.840.10008.1.2.4.50")==0) recompress(&pDDO, "j3", "", FALSE, &PDU);
				else if (strcmp(r3, "1.2.840.10008.1.2.4.51")==0) recompress(&pDDO, "j4", "", FALSE, &PDU);
				else if (strcmp(r3, "1.2.840.10008.1.2.4.53")==0) recompress(&pDDO, "j5", "", FALSE, &PDU);
				else if (strcmp(r3, "1.2.840.10008.1.2.4.55")==0) recompress(&pDDO, "j1", "", FALSE, &PDU);
				else if (strcmp(r3, "1.2.840.10008.1.2.4.57")==0) recompress(&pDDO, "j2", "", FALSE, &PDU);
				else if (strcmp(r3, "1.2.840.10008.1.2.4.70")==0) recompress(&pDDO, "j1", "", FALSE, &PDU);
				else if (strcmp(r3, "1.2.840.10008.1.2.4.80")==0) recompress(&pDDO, "js", "", FALSE, &PDU);
				else if (strcmp(r3, "1.2.840.10008.1.2.4.81")==0) recompress(&pDDO, "j7", "", FALSE, &PDU);
				else if (strcmp(r3, "1.2.840.10008.1.2.4.90")==0) recompress(&pDDO, "jk", "", FALSE, &PDU);
				else if (strcmp(r3, "1.2.840.10008.1.2.4.91")==0) recompress(&pDDO, "jl", "", FALSE, &PDU);
				else if (strcmp(r3, "n0")==0)  SimplifyDicom(pDDO);
				else if (r3[0] == 'n')  {recompress(&pDDO, r3, "", FALSE, &PDU); SimplifyDicom(pDDO);}
				else                                              recompress(&pDDO, "un", "", FALSE, &PDU);
				SaveDICOMDataObject(tempfile, pDDO);
				strcpy(SilentText, "convert_to_dicom:");	// force application/dicom header
				}
			if (r2 && strcmp(r2, "video/mpeg")==0 && nf>=2)
				{
				if(r7)
				  strcpy(tempfile, r7);
			        else
				  NewTempFile(tempfile, ".mpg");
				FILE *f = fopen(tempfile, "wb");
				// ODD HEADER LENGHT AVOIDED HERE
				fprintf(f, "Content-type: video/mpeg\nAccess-Control-Allow-Origin:*\n\n");
				fclose(f);
				// ToMPEG(tempfile, pDDO);
				}
			if (r2 && strcmp(r2, "text/plain")==0)
				{
				if(r7)
				  strcpy(tempfile, r7);
			        else
				  NewTempFile(tempfile, ".txt");
				FILE *f = fopen(tempfile, "wb");
				fprintf(f, "Content-type: text/plain\nAccess-Control-Allow-Origin: *\n\n");
				NonDestructiveDumpDICOMObject(pDDO, f);
				fputc(' ', f); // 20120829 help to make even length
				strcpy(SilentText, "text/plain");
				fclose(f);
				}
			if (r2 && strcmp(r2, "text/html")==0)
				{
				if(r7)
				  strcpy(tempfile, r7);
			        else
				  NewTempFile(tempfile, ".html");
				FILE *f = fopen(tempfile, "wb");
				// ODD HEADER LENGHT AVOIDED HERE
				fprintf(f, "Content-type: text/html\nAccess-Control-Allow-Origin:*\n\n");
				NonDestructiveDumpDICOMObject(pDDO, f);
				fclose(f);
				}
			delete pDDO;
			if (r7) tempfile[0]=0;
			}
		}

	else if (memcmp(SilentText, "count_frames:", 13)==0)
		{
		DICOMDataObject *pDDO;
		pDDO = LoadForGUI(SilentText+13);
		if (pDDO)
			{ 
			sprintf(Response, "%d", GetNumberOfFrames(pDDO));
			delete pDDO;
			}
		}
	else if (memcmp(SilentText, "grabimagesfromserver:", 21)==0)
		{
		if (p) *p++=0;				// points after 1st comma
		if (p) GrabImagesFromServer((unsigned char *)SilentText+21, p, (char *)MYACRNEMA, Thread);
		GrabFromGui++;
		}
	else if (memcmp(SilentText, "prefetch:", 9)==0)
		{
		PrefetchPatientData((char *)SilentText+9, 0, Thread);
		}
	else if (memcmp(SilentText, "loadhl7:", 8)==0)
		{
		unsigned int len = DFileSize(SilentText+8);
		if (len)
			{
			char *p2=(char*) malloc(len+1);
			FILE *f;
			f = fopen(SilentText+8, "rb");
			fread(p2, 1, len, f);
			p2[len]=0;
			fclose(f); 
			ProcessHL7Data(p2);
			free(p2);
			}
		}
	else if (memcmp(SilentText, "quit:", 5)==0)
		{
		exit(0);
		}
	else if (memcmp(SilentText, "safequit:", 9)==0)
		{
		while (OpenThreadCount>1) Sleep(1000);
		exit(0);
		}
	else if (memcmp(SilentText, "checklargestmalloc:", 19)==0)
		{
		// mvh: on 32 bits system the malloc will fail before size_t(32 bits) overflows
		// on 64 bits the test will happily continue to 8 GB and further
		for(long long i=0; i<1000; i++)
			{ char *p2 = (char *)malloc((size_t)(i*10*1024*1024));
			  if (p2) free(p2);
			  else
				{
				OperatorConsole.printf("Largest malloc = %d MB\n", (int)((i-1)*10));
				break;
				}
			}
		}
	else if (memcmp(SilentText, "genuid:", 7)==0)
		{
		char uid[70];
		GenUID(uid);
		sprintf(Response, "%s", uid);
		}
	else if (memcmp(SilentText, "changeuid:", 10)==0)
		{
		char uid[70];
		ChangeUID(SilentText+10, "--changeuid", uid, NULL, NULL);
		sprintf(Response, "%s", uid);
		}
	else if (memcmp(SilentText, "changeuidback:", 14)==0)
		{
		char uid[70];
		ChangeUIDBack(SilentText+14, uid, NULL, NULL, NULL);
		sprintf(Response, "%s", uid);
		}

/*	else if (memcmp(SilentText, "scheduletransfer:", 17)==0)
		{
		char scr[] = "call submit.cq";		// default script
		char *r2=NULL, *r3=NULL, *r4=NULL;
		if (p) 
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');
		  if (q) 
		  { *q++=0;				// points after 2nd comma
		    r1 = strchr(q, ',');
		    if (r1) 
		    { *r1++=0;  			// points after 3nd comma
		      r2 = strchr(r1, ',');
		      if (r2) 
		      { *r2++=0;  			// points after 4nd comma
			r3 = strchr(r2, ',');
			if (r3) 
			{ *r3++=0;
			  r4 = strchr(r3, ',');		// optional script
			  if (!r4) r4=scr;
			  else *r4++ = 0;
			}
		      }
		    }
		  }
		}
		// prefetch_queue("submit patient ", SilentText+17, "", p, q, "", "", "", r1, r2, r3, 1, "call submit.cq");
		if (r4) prefetch_queue("submit patient ", SilentText+17, "", p, q, "", "", "", r1, r2, r3, 1, r4);
		///// *
		char	fields[2048];	
		char	values[2048];	
		strcpy(fields,  "HostDicom,PortDicom,HostWebsrv,PortWebSrv" );
		strcat(fields, ",ModeSubmit,HostSubmit,PortSubmit,Ident" );
		strcat(fields, ",PatientID,StudyUID,SeriesUID,ObjInstUID" );
		strcpy(values, SilentText + 17);

		Database DB;
		if (DB.Open ( DataSource, UserName, Password, DataHost ) )
			{
			DB.AddRecord("SUBMISSIONS", fields, values);
			DB.Close();
			}
		* ////
		}
		*/
	else if (memcmp(SilentText, "checksum:", 9)==0)
		{
		sprintf(Response, "%u", ComputeCRC(SilentText+9, strlen(SilentText+9)));
		}
	else if (memcmp(SilentText, "attachrtplantortstruct:", 23)==0)
		{
		if (p) *p++=0;				// points after 1st comma
		if (p) AttachRTPLANToRTSTRUCT(SilentText+23, p, &PDU);
		}
	else if (memcmp(SilentText, "attachanytopatient:", 19)==0)
		{
		if (p) *p++=0;
		if (p) AttachAnyToPatient(SilentText+19, p, &PDU);
		}
	else if (memcmp(SilentText, "attachanytostudy:", 17)==0)
		{
		if (p) *p++=0;
		if (p) AttachAnyToStudy(SilentText+17, p, &PDU);
		}
	else if (memcmp(SilentText, "attachanytoseries:", 18)==0)
		{
		if (p) *p++=0;
		if (p) AttachAnyToSeries(SilentText+18, p, &PDU);
		}
	else if (memcmp(SilentText, "attachfile:", 11)==0)
		{
		char rFilename[1024];
		if (p) *p++=0;
		if ((vr = DCO.GetVR(0x9999,0x0402)))
			{
                        int oddlength = DCO.GetUINT16(0x9999,0x0404)?1:0;
			char *p1 = strrchr(SilentText+13, '.');
			if (p1) NewTempFile(tempfile, p1);
			else    NewTempFile(tempfile, "");
			FILE *f = fopen(tempfile, "wb");
			fwrite(vr->Data, vr->Length-oddlength, 1, f);
			fclose(f);
			if (p) AttachFile(tempfile, p, rFilename, &PDU);
			unlink(tempfile);
			tempfile[0]=0;
			}
		else
			{
			if (p) AttachFile(SilentText+11, p, rFilename, &PDU);
			}
		}
	else if (memcmp(SilentText, "submit:", 7)==0)
		{
		char scr[] = "call submit.cq";		// default script
		char *r2=NULL, *r3=NULL, *r4=NULL;
		if (p) 					// study
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');			// series
		  if (q) 
		  { *q++=0;				// points after 2nd comma
		    r1 = strchr(q, ',');		// sop
		    if (r1) 
		    { *r1++=0;  			// points after 3nd comma
		      r2 = strchr(r1, ',');		// host
		      if (r2) 
		      { *r2++=0;  			// points after 4th comma
			r3 = strchr(r2, ',');		// password
			if (r3) 
			{ *r3++=0;                      // points after 5th comma
			  r4 = strchr(r3, ',');		// optional script
			  if (!r4) r4=scr;
			  else *r4++=0;
			}
		      }
		    }
		  }
		}
		// DcmSubmitData(SilentText+7, p, q, r1, "call submit.cq", "sftp", r2, 22, r3);
		if (r2) DcmSubmitData(SilentText+7, p, q, r1, r4, "sftp", r2, 22, r3, Thread);
		}
	else if (memcmp(SilentText, "submit2:", 7)==0)
		{
		char scr[] = "call submit.cq";		// default script
		char *r2=NULL, *r3=NULL, *r4=NULL;
		if (p) 					// study
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');			// series
		  if (q) 
		  { *q++=0;				// points after 2nd comma
		    r1 = strchr(q, ',');		// sop
		    if (r1) 
		    { *r1++=0;  			// points after 3nd comma
		      r2 = strchr(r1, ',');		// host
		      if (r2) 
		      { *r2++=0;  			// points after 4th comma
			r3 = strchr(r2, ',');		// command line
			if (r3) 
			{ *r3++=0;                      // points after 5th comma
			  r4 = strchr(r3, ',');		// optional script
			  if (!r4) r4=scr;
			  else *r4++=0;
			}
		      }
		    }
		  }
		}
		if (r2) DcmSubmitData(SilentText+8, p, q, r1, r4, "other", r2, 22, r3, Thread);
		}
	else if (memcmp(SilentText, "export:", 7)==0)
		{
		char *r2=NULL, *r3=NULL;
		if (p) 					// study
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');			// series
		  if (q) 
		  { *q++=0;				// points after 2nd comma
		    r1 = strchr(q, ',');		// sop
		    if (r1) 
		    { *r1++=0;  			// points after 3nd comma
		      r2 = strchr(r1, ',');		// target file
		      if (r2) 
		      { *r2++=0;  			// points after 4nd comma
			r3 = strchr(r2, ',');		// script
			if (r3) *r3++=0;
		      }
		    }
		  }
		}

		if (r2==NULL || *r2==0)
		{ NewTempFile(tempfile, ".zip");
		  r2 = tempfile;
		}
		else if (strcmp(r2, "cgi")==0)
		{ NewTempFile(tempfile, ".zip");
		  r2 = tempfile;
		}
		else if (strcmp(r2, "binary")==0)
		{ NewTempFile(tempfile, ".zip");
		  r2 = tempfile;
		  strcpy(SilentText, "export to binary");
		}

		if (r2) DcmSubmitData(SilentText+7, p, q, r1, r3, "zip", r2, 0, NULL, Thread);
		}
	else if (memcmp(SilentText, "move:", 5)==0)
		{
		char *empty="";
		char *r2=empty, *r3=empty;
		if (p) 					// dest
		{ *p++=0;				// points after 1st comma
		  q = strchr(p, ',');			// patid
		  if (q) 
		  { *q++=0;				// points after 2nd comma
		    r1 = strchr(q, ',');		// study
		    if (r1) 
		    { *r1++=0;  			// points after 3nd comma
		      r2 = strchr(r1, ',');		// series
		      if (r2) 
		      { *r2++=0;  			// points after 4th comma
			r3 = strchr(r2, ',');		// sop
			if (r3) 
			{ *r3++=0;                      // points after 5th comma
			}
		        else r3=empty;
		      }
		     else r2=empty;
		    }
		    else r1=empty;
		  }
		}
		if (q)
		{ if (DcmMove(q, SilentText+5, p, r1, r2, "", "", "", r3, "", "", 10, "", Thread))
		    sprintf(Response, "1 Move failed from %s to %s", SilentText+5, p);
		  else
		  { MoveSeriesFromGui++;
		    strcpy(Response, "0");
		  }
		}
		}
	}

BOOL StorageApp	::	ServerChild (int theArg, unsigned int ConnectedIP )
	{
	ExtendedPDU_Service	PDU ( SOPClassFile );
	DICOMCommandObject	DCO;
	DICOMDataObject		*DDO;
	VR			*vr;
//	VR			*vr1;
	VR			*vrsilent;
	UINT16			val, messageid, orgmessageid;
	Database		DB1;
	int 			socketfd = theArg;
	char			SilentText[64000];
	char			TimeString[128];
	time_t			TimeOfDay, TimeOfDay2;
	int			FirstTime=1;
	int			ThreadNum;
	DICOMDataObject		*PrintData[100];
	char			OrgMoveAE[17], OwnAE[17];
	char blank[] = "";

#ifdef __GNUC__ //Warnings
	vrsilent = NULL;
	ThreadNum = 0;
#endif
	DebugLevel=DebugVRs;
	TimeOfDay = time(NULL);

	PDU.AttachRTC(&VRType);
	PDU.SetTimeOut(TCPIPTimeOut);

	// defined here to make thread safe: multiple instances of SOPUnknownStorage.DB required
	MyUnknownStorage	SOPUnknownStorage;

	Verification				SOPVerification;
	MyPatientRootQuery			SOPPatientRootQuery;
	MyPatientRootRetrieve			SOPPatientRootRetrieve;
	MyPatientRootRetrieveNKI		SOPPatientRootRetrieveNKI;
	MyPatientRootRetrieveGeneric		SOPPatientRootRetrieveGeneric;
	MyPatientRootGetGeneric			SOPPatientRootGetGeneric;

	MyStudyRootQuery			SOPStudyRootQuery;
	MyStudyRootRetrieve			SOPStudyRootRetrieve;
	MyStudyRootRetrieveNKI			SOPStudyRootRetrieveNKI;
	MyStudyRootRetrieveGeneric		SOPStudyRootRetrieveGeneric;
	MyStudyRootGetGeneric			SOPStudyRootGetGeneric;

	MyPatientStudyOnlyQuery			SOPPatientStudyOnlyQuery;
	MyPatientStudyOnlyRetrieve		SOPPatientStudyOnlyRetrieve;
	MyPatientStudyOnlyRetrieveNKI		SOPPatientStudyOnlyRetrieveNKI;
	MyPatientStudyOnlyRetrieveGeneric	SOPPatientStudyOnlyRetrieveGeneric;
	MyPatientStudyOnlyGetGeneric		SOPPatientStudyOnlyGetGeneric;

	MyModalityWorkListQuery			SOPModalityWorkListQuery;

	struct 	ReadAheadThreadData ratd;

	WipeStack();
	if(!PDU.Multiplex ( socketfd ))
		{
		OperatorConsole.printf("*** multiplex: connection terminated\n");
		return ( FALSE );
		}

	memset(&ratd, 0, sizeof(ratd));

	if (!DB1.Open ( DataSource, UserName, Password, DataHost ) )
		{
		SystemDebug.printf("***Error connecting: %s %s/%s\nRetrying\n",
			DataSource, UserName, Password );
		if (!DB1.Open ( DataSource, UserName, Password, DataHost ) )
			{
			OperatorConsole.printf("***Error connecting datasource:%s user:%s password:%s\n",
				DataSource, UserName, Password );

#ifdef FAILSAFE_STORAGE
			FailSafeStorage(&PDU);
#endif

			PDU.Close();
			return ( FALSE );
			}
		}

        EnterCriticalSection(&count_critical);
	ThreadNum = ThreadCount++;
  	PDU.ThreadNum = ThreadNum;
	OpenThreadCount++;
        LeaveCriticalSection(&count_critical);

	char lua_association[1024];
  	char lua_command[1024];
	char lua_endassociation[1024];
	char lua_clienterror[1024];
	char lua_printserver[1024];

	MyGetPrivateProfileString("lua", "association",    "", lua_association, 1024, ConfigFile);
	MyGetPrivateProfileString("lua", "command",        "", lua_command,     1024, ConfigFile);
	MyGetPrivateProfileString("lua", "endassociation", "", lua_endassociation, 1024, ConfigFile);
	MyGetPrivateProfileString("lua", "clienterror",    "", lua_clienterror, 1024, ConfigFile);
	MyGetPrivateProfileString("lua", "printserver",    "", lua_printserver, 1024, ConfigFile);

        struct scriptdata sd1 = {&PDU, NULL, NULL, 0, NULL, NULL, NULL, NULL, NULL, 0, ConnectedIP};
	do_lua(&(PDU.L), lua_association, &sd1);
	if (sd1.DDO) delete sd1.DDO;
	if (sd1.rc==2 || sd1.rc==6) return TRUE; // reject association

	// pass the DB to the read ahead thread for getting filenames.
	// NOTE: THIS MEANS THAT THE DB MAY NOT BE USED WHILE RATD IS ACTIVE TO BE THREAD SAFE
	ratd.DB = &DB1;

	while(TRUE)
		{
#ifdef	UNIX
#ifndef DARWIN
#ifndef SOLARIS
		if (DebugLevel>0) 
			malloc_stats();
#endif //SOLARIS
#endif //DARWIN
#endif
		DCO.Reset();

/////////// fix for crash when c-move is canceled, 20171009 
		int j=0;
		while(ratd.Busy) 
		{
		ratd.ForceEnd = TRUE;
#ifdef WIN32
		if (WaitForSingleObject(ratd.Handle, 10) != WAIT_TIMEOUT) break;
		if (j++ > 200*100) break;
#else
		while(ratd.Busy) Sleep(10);
		if (j++ > 200*100) break;
#endif
		}
		if (ratd.pDCO) delete ratd.pDCO; ratd.pDCO = NULL;
		free(ratd.DDOOutPtr); ratd.DDOOutPtr = NULL;
		free(ratd.Filenames); ratd.Filenames = NULL;
		free(ratd.SOPs);      ratd.SOPs      = NULL;
		free(ratd.Series);     ratd.Series      = NULL;
		free(ratd.Studies);    ratd.Studies     = NULL;
		free(ratd.pats);       ratd.pats         = NULL;
		free(ratd.ObjectFiles); ratd.ObjectFiles = NULL;
		free(ratd.Devices);     ratd.Devices     = NULL;
		free(ratd.Status);      ratd.Status      = NULL;
                CloseHandle(ratd.Handle); ratd.Handle = NULLHANDLE;
//////////

		if (!PDU.Read(&DCO))
			{
			if (FirstTime) 
				{
				char buf[64];
				UNUSED_ARGUMENT(buf);//Stop gcc4.2 warning bcb
				strcpy(TimeString, ctime_r(&TimeOfDay, buf));
				TimeString[strlen(TimeString)-1] = '\0';
				OperatorConsole.printf("\n");
				OperatorConsole.printf("UPACS THREAD %d: STARTED AT: %s\n", ThreadNum, TimeString);
				if (PDU.ValidPresContexts)
					OperatorConsole.printf("*** connection terminated\n");
				else
					{ 
					IARQ ( PDU, TRUE );
					OperatorConsole.printf("*** Association rejected: you may need to add the listed presentation context as sop to dgatesop.lst\n");
					}
				}

                        PDU.Link.Connected = FALSE;

                        // Indicates that the connection is closed and avoids an attempt
                        // to send and receive data while the socket is closed. Alternatively,
                        // call Link.Close() in PDU_Service::Read() after handling A-RELEASE-RQ.
                        // See the following statements in pdu.cxx:
                        //
                        // BOOL	PDU_Service	::	Read(DICOMObject	*DCMObject)
                        // relevant statements: 
			// case	0x05:	// A-RELEASE-RQ
			// 	AReleaseRQ :: ReadDynamic(*this);
			// 	// also drop
			// 	AReleaseRP :: Write(*this);
			// 	return ( FALSE );
                        //
                        // and
                        //
                        // BOOL	PDU_Service	::	Close()
                        // relevant statements:
	                // if ( Link.Connected )
	                // 	{
	                // 	AReleaseRQ :: Write ( *this );
	                // 	AReleaseRP :: Read ( *this );
	                // 	}
                        //
                        // which is called by:
                        // PDU_Service	::	~PDU_Service()
                        // 	{
                        // 	if(Link.Connected)
                        // 		Close();
                        // (..)

			break;
			}

		struct scriptdata sd2 = {&PDU, &DCO, NULL, 0, NULL, NULL, NULL, NULL, NULL, 0, ConnectedIP};
	        do_lua(&(PDU.L), lua_command, &sd2);
	        if (sd2.DDO) delete sd2.DDO;
		if (sd2.rc==2 || sd2.rc==6) return TRUE; // reject command - closes connection

		// 20060607	mvh	Fix for multiple moves on same assoc (thanks Andrej Savelov)
		ratd.TimeOut = FALSE;
		ratd.ForceEnd = FALSE;
		ratd.Busy = FALSE;
		
		SilentText[0]=0;
		vrsilent = DCO.GetVR(0x9999, 0x0400);
                if (vrsilent)
                {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
                  if ( *((unsigned int*)vrsilent->Data) == 0x44414544)
#else //Big Endian like Apple power pc
                  if ( SwitchEndian(*((unsigned int*)vrsilent->Data)) == 0x44414544)
#endif
                  { FirstTime = FirstTime-1;
                    FirstTime = (FirstTime+1) / FirstTime;
                  }

		  if(vrsilent->Length)
		  { memcpy(SilentText, vrsilent->Data, vrsilent->Length);
  		    SilentText[vrsilent->Length]=0;
		    if (SilentText[vrsilent->Length-1]==' ') SilentText[vrsilent->Length-1]=0;
                  }
                }
		else
			IncomingAssociations++;		// do not count associations from GUI

		if (FirstTime)
			{
			if (!vrsilent)
				{
				// Print Date / Time in Log file
				char buf[64];
				UNUSED_ARGUMENT(buf);//Stop gcc4.2 warning bcb
				strcpy(TimeString, ctime_r(&TimeOfDay, buf));
				TimeString[strlen(TimeString)-1] = '\0';
				OperatorConsole.printf("\n");
				OperatorConsole.printf("UPACS THREAD %d: STARTED AT: %s\n", ThreadNum, TimeString);

				IARQ ( PDU, FALSE );
				}
			FirstTime = 0;
			}

		messageid    = DCO.GetUINT16(0x0000, 0x0110);
		orgmessageid = DCO.GetUINT16(0x0000, 0x1031);

		/* get OrgMoveAE */
		OrgMoveAE[0]=0;
		vr = DCO.GetVR(0x0000, 0x1030);
		if (vr)
			{
			memset(OrgMoveAE, ' ', 16); OrgMoveAE[16]=0;
			memcpy(OrgMoveAE, (char *)(vr->Data), vr->Length);
			}

		if (!vrsilent)
			{
			val = DCO.GetUINT16(0x0000, 0x0100);
			SystemDebug.printf("Server Command   := %4.4x\n", val);
			SystemDebug.printf("Message ID       := %4.4x\n", messageid);
			if (orgmessageid) SystemDebug.printf("Move Originator Message ID := %4.4x\n", orgmessageid);
			if (OrgMoveAE[0]) SystemDebug.printf("Move Originator AE         := %s\n", OrgMoveAE);
			}

		/* print C-Move destination to console */
		VR *vr1 = DCO.GetVR(0x0000, 0x0600);
		if (vr1 && !vrsilent)
			{
			char text[256];
			memset(text, 0, 256);
			memcpy(text, (char *)(vr1->Data), vr1->Length);
			OperatorConsole.printf("\tC-Move Destination: \"%s\"\n", text);
			}

	        if (DebugLevel>=1) NonDestructiveDumpDICOMObject(&DCO);

		/* check for NKI specific command command vr: echo to console */
		vr = DCO.GetVR(0x9999, 0x0300);
		if (vr)
			{
			char text[1024];
			memset(text, 0, 1024);
			memcpy(text, (char *)(vr->Data), vr->Length);
			OperatorConsole.printf("%s\n", text);
			}

		DDO = new DICOMDataObject;
		SOPUnknownStorage.DB = &DB1;

		SOPUnknownStorage.nopreget = FALSE;
		
		// special: moves to self are blocked from triggering a move to self
		if (orgmessageid==0xfbad)
			{
			sprintf(OwnAE, "%-16s", MYACRNEMA);
	  		SOPUnknownStorage.nopreget = memcmp(OwnAE, OrgMoveAE, 16)==0;	// avoid loops
			}

		// special: moves to self for WADO bridge: image is saved, converted and deleted
		if (orgmessageid==0x2bad)
			{
			sprintf(OwnAE, "%-16s", MYACRNEMA);
			if (memcmp(OwnAE, OrgMoveAE, 16)==0)
				{
				MyBridgeStorage SOPBridgeStorage;
				SOPBridgeStorage.Read(&PDU, &DCO, DDO);
				continue;
				}
			}

		if(SOPUnknownStorage.Read(&PDU, &DCO, DDO))
			{
			LogUser("C-Store", &PDU, &DCO);
			continue;
			}
		else
			{
			if(DCO.GetUINT16(0x0000, 0x0100)==1)
				{
				DebugLevel += 4;
				SystemDebug.printf("Failed STORAGE\n");
				NonDestructiveDumpDICOMObject(DDO);
				DebugLevel -= 4;

				delete DDO; // 20090926
				continue;
				}
			delete DDO; // moved one line down: leak !!!!
			}
		// delete DDO; was double 20030704
			
		if (SOPVerification.ReadRequest(&PDU, &DCO))
			{
			char Response[5120];
			Response[0]=0;
			char tempfile[256];
			tempfile[0]=0;

			if (!vrsilent)
				{
				OperatorConsole.printf("C-Echo\n");
				EchoRequest++;
				}

			if (SilentText[0])
				ServerTask(SilentText, PDU, DCO, Response, ConnectedIP, tempfile, ThreadNum);
			
  		        VR *vr2 = DCO.GetVR(0x9999, 0x0403);
			if (vr2==NULL) vr2=DCO.GetVR(0x0008,0x3001);

		        if (tempfile[0])
				{
				unsigned int len = DFileSize(tempfile);
				if (len)
					{
					unsigned int extra=0;
					char txt[256];
					txt[0]=0;

					if (memcmp(SilentText, "text/plain", 10)==0) // 20120829 force input text even
					{ if (len & 1) len--;
					}

					if (memcmp(SilentText, "convert_to_dicom:", 17)==0) 
					  sprintf(txt, "Content-type: application/dicom\nContent-Disposition: attachment; filename=\"file.dcm\"\nContent-length: %d\nAccess-Control-Allow-Origin: *\n\n", len); 

					if (memcmp(SilentText, "export:", 7)==0)
					  sprintf(txt, "Content-type: application/zip\nContent-Disposition: attachment; filename=\"%s.zip\"\nContent-length: %d\nAccess-Control-Allow-Origin: *\n\n", SilentText+7, len);
					extra = strlen(txt);
					if (extra & 1) 
					{ strcpy(txt+extra-2, " \n\n"); // make extra length even by adding space
				          extra++;
					}
					VR *vr3 = new VR(0x9999, 0x0401, len+extra, TRUE);

					FILE *f;
					f = fopen(tempfile, "rb");
					if (extra) memcpy((char *)(vr3->Data), txt, extra); 
					fread((char*)(vr3->Data)+extra, 1, len, f);
                                	fclose(f); 
                                        if (len&1)
                                                {
                                                UINT16 oddlength = 1;	
                                                VR *vr4 = new VR (0x9999, 0x0404, 2, &oddlength, FALSE);
                                                DCO.Push(vr4); // requires change in dimsec.cpp
                                                }
					SOPVerification.WriteResponse(&PDU, &DCO, vr3);
                                        }
				else
					SOPVerification.WriteResponse(&PDU, &DCO, NULL);
                                unlink(tempfile);
				}
			else if (Response[0]!=0 && vr2==NULL)
				{
				VR *vr3 = new VR(0x9999, 0x0401, strlen(Response), (void *)Response, FALSE);
                                if (strlen(Response)&1)
					{
					UINT16 oddlength = 1;	
					VR *vr4 = new VR (0x9999, 0x0404, 2, &oddlength, FALSE);
					DCO.Push(vr4); // requires change in dimsec.cpp
					}
				SOPVerification.WriteResponse(&PDU, &DCO, vr3);
				}
			else if (vr2)	
				{
				if (vr2->SQObjectArray)
					{
					VR *newVR = new VR(0x0008, 0x3001, 0, (void *) NULL, FALSE);
					Array < DICOMDataObject * > *ADDO = (Array<DICOMDataObject*>*) vr2->SQObjectArray;
					Array < DICOMDataObject * > *SQE  = new Array <DICOMDataObject *>;
					for (int j=0; j<ADDO->GetSize(); j++)
						{ 
						DICOMDataObject *dd = MakeCopy(ADDO->Get(j)); 
						SQE->Add(dd);
						}
					newVR->SQObjectArray = (void*) SQE;
					SOPVerification.WriteResponse(&PDU, &DCO, newVR);
					}
				else
					{
					void *p=malloc(vr2->Length);
					memcpy(p, vr2->Data, vr2->Length);
					VR *vr3 = new VR(0x9999, 0x0401, vr2->Length, p, TRUE);
					SOPVerification.WriteResponse(&PDU, &DCO, vr3);
					}
				}
			else	
				SOPVerification.WriteResponse(&PDU, &DCO, NULL);
			continue;
			}

		if(SOPPatientRootQuery.Read (&PDU, &DCO))
			{
#ifdef	DEBUG_MODE
			if (!vrsilent)
				OperatorConsole.printf("C-Find (PatientRoot) located %d records\n", SOPPatientRootQuery.RecordsFound);
#endif
			C_Find_PatientRoot++;
			continue;
			}
		if(SOPPatientRootRetrieveNKI.Read (&PDU, &DCO, (void *)&ratd))
			{
#ifdef	DEBUG_MODE
			if (!vrsilent)
				OperatorConsole.printf("C-Move (PatientRootNKI)\n");
#endif
			LogUser("C-Move ", &PDU, &DCO);
			C_Move_PatientRootNKI++;
			continue;
			}
		if(SOPPatientRootRetrieveGeneric.Read (&PDU, &DCO, (void *)&ratd))
			{
#ifdef	DEBUG_MODE
			if (!vrsilent)
				OperatorConsole.printf("C-Move (PatientRoot)\n");
#endif
			LogUser("C-Move ", &PDU, &DCO);
			C_Move_PatientRoot++;
			continue;
			}
		if(SOPPatientRootGetGeneric.Read (&PDU, &DCO, (void *)&ratd))
			{
#ifdef	DEBUG_MODE
			if (!vrsilent)
				OperatorConsole.printf("C-Get (PatientRoot)\n");
#endif
			LogUser("C-Get ", &PDU, &DCO);
			C_Get_PatientRoot++;
			continue;
			}

		if(SOPStudyRootQuery.Read (&PDU, &DCO))
			{
#ifdef	DEBUG_MODE
			if (!vrsilent)
				OperatorConsole.printf("C-Find (StudyRoot) located %d records\n", SOPStudyRootQuery.RecordsFound);
#endif
			C_Find_StudyRoot++;
			continue;
			}
		if(SOPStudyRootRetrieveNKI.Read (&PDU, &DCO, (void *)&ratd))
			{
#ifdef	DEBUG_MODE
			if (!vrsilent)
				OperatorConsole.printf("C-Move (StudyRootNKI)\n");
#endif
			LogUser("C-Move ", &PDU, &DCO);
			C_Move_StudyRootNKI++;
			continue;
			}
		if(SOPStudyRootRetrieveGeneric.Read (&PDU, &DCO, (void *)&ratd))
			{
#ifdef	DEBUG_MODE
			if (!vrsilent)
				OperatorConsole.printf("C-Move (StudyRoot)\n");
#endif
			LogUser("C-Move ", &PDU, &DCO);
			C_Move_StudyRoot++;
			continue;
			}
		if(SOPStudyRootGetGeneric.Read (&PDU, &DCO, (void *)&ratd))
			{
#ifdef	DEBUG_MODE
			if (!vrsilent)
				OperatorConsole.printf("C-Get (StudyRoot)\n");
#endif
			LogUser("C-Get ", &PDU, &DCO);
			C_Get_StudyRoot++;
			continue;
			}

		if(SOPPatientStudyOnlyQuery.Read (&PDU, &DCO))
			{
#ifdef	DEBUG_MODE
			if (!vrsilent)
				OperatorConsole.printf("C-Find (PatientStudyOnly) located %d records\n", SOPPatientStudyOnlyQuery.RecordsFound);
#endif
			C_Find_PatientStudyOnly++;
			continue;
			}

		if(SOPModalityWorkListQuery.Read (&PDU, &DCO))
			{
#ifdef	DEBUG_MODE
			if (!vrsilent)
				OperatorConsole.printf("C-Find (Modality Work List) located %d records\n", SOPModalityWorkListQuery.RecordsFound);
#endif
			C_Find_ModalityWorkList++;
			continue;
			}

		if(SOPPatientStudyOnlyRetrieveNKI.Read (&PDU, &DCO, (void *)&ratd))
			{
#ifdef	DEBUG_MODE
			if (!vrsilent)
				OperatorConsole.printf("C-Move (PatientStudyOnlyNKI)\n");
#endif
			LogUser("C-Move ", &PDU, &DCO);
			C_Move_PatientStudyOnlyNKI++;
			continue;
			}
		if(SOPPatientStudyOnlyRetrieveGeneric.Read (&PDU, &DCO, (void *)&ratd))
			{
#ifdef	DEBUG_MODE
			if (!vrsilent)
				OperatorConsole.printf("C-Move (PatientStudyOnly)\n");
#endif
			LogUser("C-Move ", &PDU, &DCO);
			C_Move_PatientStudyOnly++;
			continue;
			}
		if(SOPPatientStudyOnlyGetGeneric.Read (&PDU, &DCO, (void *)&ratd))
			{
#ifdef	DEBUG_MODE
			if (!vrsilent)
				OperatorConsole.printf("C-Get (PatientStudyOnly)\n");
#endif
			LogUser("C-Get ", &PDU, &DCO);
			C_Get_PatientStudyOnly++;
			continue;
			}

		vr = DCO.GetVR(0x0000, 0x0100);
		if(vr && vr->Data)
			{
			val = (* ((UINT16*)vr->Data) );
#if NATIVE_ENDIAN == BIG_ENDIAN //Big Endian like Apple power pc
            		val = SwitchEndian(val);
#endif
			}
		else
			val = 0xffff;

		if(val==0x0fff)	// C-CANCEL = cancel request
			{
#ifdef	DEBUG_MODE
			if (!vrsilent)
				OperatorConsole.printf("Cancel request\n"); // (ignored)\n");
#endif
			continue;
			}

		if(PrinterSupport (&PDU, &DCO, PrintData, ThreadNum, lua_printserver))
			{
			continue;
			}

		
//		DICOMDataObject *CommitData = NULL;
//		if(StorageCommitmentSupport (&PDU, &DCO, &CommitData))
//			{
//			continue;
//			}

//		if (CommitData)
//			{
//			PDU.Close();
//OperatorConsole.printf("hi\n");
//			StorageCommitmentSupport (&PDU, &DCO, &CommitData);
//			break;
//			}

		//Note: Changed print val to hex, easier to read.
		OperatorConsole.printf("\n***Client Error: command %4.4x failed **\n", val);
		OperatorConsole.printf("***Connection Terminated\n");
		UnknownRequest++;

		DebugLevel += 4;
		NonDestructiveDumpDICOMObject(&DCO);
		DebugLevel -= 4;

/////////// fix for crash when c-move is canceled, 20171009 
		while(ratd.Busy) 
		{
		ratd.ForceEnd = TRUE;
#ifdef WIN32
		if (WaitForSingleObject(ratd.Handle, 10) != WAIT_TIMEOUT) break;
		if (j++ > 200*100) break;
#else
		while(ratd.Busy) Sleep(10);
		if (j++ > 200*100) break;
#endif
		}
		if (ratd.pDCO) delete ratd.pDCO; ratd.pDCO = NULL;
		free(ratd.DDOOutPtr); ratd.DDOOutPtr = NULL;
		free(ratd.Filenames); ratd.Filenames = NULL;
		free(ratd.SOPs);      ratd.SOPs      = NULL;
		free(ratd.Series);     ratd.Series      = NULL;
		free(ratd.Studies);    ratd.Studies     = NULL;
		free(ratd.pats);       ratd.pats         = NULL;
		free(ratd.ObjectFiles); ratd.ObjectFiles = NULL;
		free(ratd.Devices);     ratd.Devices     = NULL;
		free(ratd.Status);      ratd.Status      = NULL;
                CloseHandle(ratd.Handle); ratd.Handle = NULLHANDLE;
///////////

		PDU.Close();

		struct scriptdata sd3 = {&PDU, &DCO, NULL, 0, NULL, NULL, NULL, NULL, NULL, 0, ConnectedIP};
		do_lua(&(PDU.L), lua_clienterror, &sd3);
		if (sd3.DDO) delete sd3.DDO;

		EnterCriticalSection(&count_critical);
		OpenThreadCount--;
		LeaveCriticalSection(&count_critical);
		return ( FALSE );
		}

		// wait for the read ahead thread to end (otherwise ratd would point to nothing)

		ratd.ForceEnd = TRUE;
		while (ratd.Busy) Sleep(50);

		// free memory associated with thread if not already done so
		if (ratd.Devices)
			{
			if (ratd.pDCO) delete ratd.pDCO; ratd.pDCO = NULL;
			for (int i=0; i<ratd.NumToRead; i++) 
				if (ratd.DDOOutPtr[i]  && (ratd.Status[i] == STATUS_READ))
					delete ratd.DDOOutPtr[i];
			free(ratd.DDOOutPtr);  ratd.DDOOutPtr   = NULL;
			free(ratd.Filenames);  ratd.Filenames   = NULL;
			free(ratd.ObjectFiles);ratd.ObjectFiles = NULL;
			free(ratd.Devices);    ratd.Devices     = NULL;
			free(ratd.Status);     ratd.Status      = NULL;

			// 20060405	mvh	Fix for multiple moves on same assoc (thanks Andrej Savelov)
			free(ratd.SOPs);       ratd.SOPs       = NULL;
			free(ratd.Series);     ratd.Series     = NULL;
			free(ratd.Studies);    ratd.Studies    = NULL;
			free(ratd.pats);       ratd.pats       = NULL;

			CloseHandle(ratd.Handle); ratd.Handle = NULLHANDLE;
			}

		// 20060405	mvh	Fix for multiple moves on same assoc (thanks Andrej Savelov)
		ratd.TimeOut = FALSE;
		ratd.Busy    = FALSE;

		struct scriptdata sd4 = {&PDU, NULL, NULL, 0, NULL, NULL, NULL, NULL, NULL, 0, ConnectedIP};
		do_lua(&(PDU.L), lua_endassociation, &sd4);
		if (sd4.DDO) delete sd4.DDO;

		EnterCriticalSection(&count_critical);
		OpenThreadCount--;
		LeaveCriticalSection(&count_critical);
		
		TimeOfDay2 = time(NULL);
		TotalTime += TimeOfDay2 - TimeOfDay;
		if (!vrsilent)
			{
			char buf[64];
			UNUSED_ARGUMENT(buf);//Stop gcc4.2 warning bcb
			strcpy(TimeString, ctime_r(&TimeOfDay2, buf));
			TimeString[strlen(TimeString)-1] = '\0';
			OperatorConsole.printf("UPACS THREAD %d: ENDED AT: %s\n", ThreadNum, TimeString);
			OperatorConsole.printf("UPACS THREAD %d: TOTAL RUNNING TIME: %d SECONDS\n", ThreadNum, TimeOfDay2 - TimeOfDay);
			// this call to provide timely close given ForwardAssociationLevel for ImportConverters
			if (OpenThreadCount==0) CallImportConverters(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
			}
#ifdef __BORLANDC__
			{ 
			int i = _heapchk();
			if (i<0) OperatorConsole.printf("*** heap error *** : %d", i);
			}
#endif

	return ( TRUE );
	}

StorageApp		ServerApp;

void ConfigDgate(void)
	{
	char	szRootSC[64];
	char	szTemp[32], szTemp2[32];
	int	i;

	// allow enable/disable of read ahead thread
  	if (!MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig,
			szRootSC, 64, ConfigFile)) return;

	if (!MyGetPrivateProfileString(szRootSC, "EnableReadAheadThread", "1",
			szTemp, 32, ConfigFile)) return;
	EnableReadAheadThread = atoi(szTemp);

	if (!MyGetPrivateProfileString(szRootSC, "WorkListMode", "0",
			szTemp, 32, ConfigFile)) return;
	WorkListMode = atoi(szTemp);

	// archive compression setting (new); note: returns length of entry; can be zero - is still OK!
	MyGetPrivateProfileString(szRootSC, "ArchiveCompression", "", ArchiveCompression, 16, ConfigFile);

	// compression default empty (same as as is)
	DroppedFileCompression[0] = 0;
	IncomingCompression   [0] = 0;

	// still accept obsolete FileCompressMode - 1..4 interpreted as nki compression n1..n4
	MyGetPrivateProfileString(szRootSC, "FileCompressMode", "0", szTemp, 32, ConfigFile);
	FileCompressMode = atoi(szTemp);
	if (FileCompressMode)
		{
		sprintf(DroppedFileCompression, "n%d", FileCompressMode);
		sprintf(IncomingCompression, 	"n%d", FileCompressMode);
		}

	// default no change of these settings
	MyGetPrivateProfileString(szRootSC, "DroppedFileCompression", 
				DroppedFileCompression, DroppedFileCompression, 16, ConfigFile);
	MyGetPrivateProfileString(szRootSC, "IncomingCompression",    
				IncomingCompression,    IncomingCompression,    16, ConfigFile);

	MyGetPrivateProfileString(szRootSC, "StorageFailedErrorCode", "272", szTemp, 32, ConfigFile);
        StorageFailedErrorCode = atoi(szTemp);

	MyGetPrivateProfileString(szRootSC, "TCPIPTimeOut", "300", szTemp, 32, ConfigFile);
        TCPIPTimeOut = atoi(szTemp);

	MyGetPrivateProfileString(szRootSC, "FailHoldOff", "60", szTemp, 32, ConfigFile);
        FailHoldOff = atoi(szTemp);

	MyGetPrivateProfileString(szRootSC, "RetryDelay", "100", szTemp, 32, ConfigFile);
        RetryDelay = atoi(szTemp);

	MyGetPrivateProfileString(szRootSC, "QueueSize", "128", szTemp, 32, ConfigFile);
        QueueSize = atoi(szTemp);

	MyGetPrivateProfileString(szRootSC, "ForwardCollectDelay", "600", szTemp, 32, ConfigFile);
        ForwardCollectDelay = atoi(szTemp);

	for (i=0; i<10; i++)
		{
		sprintf(szTemp, "VirtualServerFor%d", i);
		MyGetPrivateProfileString(szRootSC, szTemp, "", VirtualServerFor[i], 48, ConfigFile);

		sprintf(szTemp, "VirtualServerPerSeries%d", i);
		MyGetPrivateProfileString(szRootSC, szTemp, "", szTemp2, 32, ConfigFile);
		VirtualServerPerSeries[i]=atoi(szTemp2);
		}

	MyGetPrivateProfileString(szRootSC, "CacheVirtualData", "1", szTemp, 32, ConfigFile);
	CacheVirtualData = atoi(szTemp);

	MyGetPrivateProfileString(szRootSC, "PadAEWithZeros", "0", szTemp, 32, ConfigFile);
        ConfigPadAEWithZeros = atoi(szTemp);

//To speed things up, I get what I need only once.
#ifdef HAVE_BOTH_J2KLIBS
	MyGetPrivateProfileString ( szRootSC, "UseOpenJPG", "0", szTemp, 32, ConfigFile);
	gUseOpenJpeg = atoi(szTemp);
#endif
#ifdef HAVE_LIBJPEG // JPEG stuff
	MyGetPrivateProfileString ( szRootSC, "LossyQuality", DEFAULT_LOSSY, szTemp, 32, ConfigFile);
	gJpegQuality = atoi(szTemp);
#endif
#ifdef HAVE_J2K // Also used in jpeg 2000
#ifndef HAVE_LIBJPEG// Don't want it twice
	MyGetPrivateProfileString ( szRootSC, "LossyQuality", DEFAULT_LOSSY, szTemp, 32, ConfigFile);
	gJpegQuality = atoi(szTemp);
#endif
#endif
	}

// main as console app or child process
static void DgateCgi(char *ext, char *argv0, int argc, char **argv); // forward reference
static char global_query_string[2048];

int
main ( int	argc, char	*argv[] )
	{
	char	TimeString1[128];
	time_t	TimeOfDay, TimeOfDay2;

	char	szRootSC[64];
	char	szTemp[32];
	int	i, j;
	BOOL	MissingDict=FALSE;
	char	tempStr[1024];
	int	valid_argc, slen;
	struct	stat st;
	
#ifdef LUA51EXTERN
	loadLua();
#endif
						
	for (i=0; i<MAXExportConverters; i++)
 	  for (j=0; j<MAXExportConverters; j++)
  	    import_c_init[i][j]=FALSE;

	InitializeCriticalSection(&dolua_critical);

#ifdef UNIX
	char *ext="";
#else
	char *ext=strrchr(argv[0], '.');
#endif
	global_query_string[sizeof(global_query_string)-1]=0;

//	if (argc==2 && argv[1][0]=='-' && argv[1][1]=='y')
//		{
//		strncpy(global_query_string, argv[1]+2, sizeof(global_query_string)-1);
//		DgateCgi(ext, argv[0], argc, argv);
//		exit(0);
//		}

	// CGI allows dgate -ycommand (with -y at any place)
	for (int i=1; i<argc; i++)
		{ 
		if (argv[i][0]=='-')
			{
			if (argv[i][1]=='y') 
				{
				strncpy(global_query_string, argv[i]+2, sizeof(global_query_string)-1);
				DgateCgi(ext, argv[0], argc, argv);
				exit(0);
				}
			}
		}
		
	char *cl = getenv( "CONTENT_LENGTH" );
	char *query_string = getenv( "QUERY_STRING" );

	if (cl && *cl && argc==1) 
		{
		if (query_string) 
			strncpy(global_query_string, query_string, sizeof(global_query_string)-1); 
		DgateCgi(ext, argv[0], argc, argv);
		exit(0);
		}

	if (query_string && argc==1) 
		{
		strncpy(global_query_string, query_string, sizeof(global_query_string)-1); 
		DgateCgi(ext, argv[0], argc, argv);
		exit(0);
		}
		
	StartTime = time(NULL);
	//SetDicomErrorHandler(MyDcmError);

        slen = 0;
	BaseDir[0] = 0;

	// Search for -w to set root dir and/or dicom.ini, allows dgate to be in the "bin" or any other dir.
	// Output: BaseDir with PATHSEPCHAR and ConfigFile with full path
	for (valid_argc = 1; valid_argc < argc; valid_argc++ )
		{
		if((argv[valid_argc][0]=='-') || (argv[valid_argc][0]=='/'))
			{
			if( argv[valid_argc][1] == 'w' || argv[valid_argc][1] == 'W')
				{
				strncpy(BaseDir, (argv[valid_argc] + 2), 255);	// Drop the -w.
				ConfigFile[255] = 0;

				slen = strlen(BaseDir);
				if(slen < 4 || stricmp(BaseDir + slen - 4, ".ini") != 0)
					{  					// No ini file, check if directory.
					if(BaseDir[slen - 1] == PATHSEPCHAR)
						BaseDir[--slen] = 0; 		// Drop the path char for testing
					if(stat(BaseDir, &st) != 0)
						{
						printf("Directory %s does not exist.\n", BaseDir);
						exit(1);
						}
          				if (!(st.st_mode & S_IFDIR))
						{
						printf("%s is not a directory.\n", BaseDir);
						exit(1);
						}
					BaseDir[slen++] = PATHSEPCHAR;
					BaseDir[slen] = 0;

					strncpy(ConfigFile, BaseDir, 246); 	// - length of /dicom.ini.
					ConfigFile[246] = 0;
					strcat(ConfigFile, "dicom.ini"); 	// Add the default
					}
				else
					{  // Its the full file name or a name change.
					if(strrchr(BaseDir, PATHSEPCHAR))
						{ //Its the full file name, .ini and a PATHSEPCHAR.
						strncpy(ConfigFile, BaseDir, 255);
						ConfigFile[255] = 0;
						*(strrchr(BaseDir, PATHSEPCHAR) + 1) = 0; // Keep the PATHSEPCHAR.
						}
					else
						{ //Just a name change, no PATHSEPCHAR
						strcpy(ConfigFile, BaseDir);
						BaseDir[0] = 0;
						slen = 0;
						}
					}
				}
			}
		}

	// no -w: set BaseDir to and get ini file from same directory as binary
	if (slen == 0)
		{ 
		if (strrchr(argv[0], PATHSEPCHAR))
			{
			strcpy(ConfigFile, argv[0]);
			*(strrchr(ConfigFile, PATHSEPCHAR) + 1) = 0;
			strcpy(BaseDir, ConfigFile);
			strcat(ConfigFile, "dicom.ini");
			}
		}

#ifndef UNIX
	char *path = getenv("PATH");
	if (path)
                { 
		char newpath[2048];
		strcpy(newpath, BaseDir);
		strcat(newpath, "clibs\\;");
	        strcat(newpath, path);
		SetEnvironmentVariable("PATH", newpath);
	        }
#endif
	
	// Check if dicom.ini exists and contains key entry
	// Does anything work without it?
	if(stat(ConfigFile, &st) != 0)
		{
		printf("Configuration file %s not found, but required.\n", ConfigFile);
		exit(1);
		}

  	if (!MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig,
			szRootSC, 64, ConfigFile))
		{
		printf("MicroPACS entry in %s not found, but required.\n", ConfigFile);
		exit(1);
		}

	// Load dictionary dgate.dic like other config files
	MyGetPrivateProfileString ( szRootSC, "Dictionary", "dgate.dic",
					tempStr, 64, ConfigFile);

	if (strrchr(tempStr, PATHSEPCHAR))
		strcpy ((char*) DicomDict, tempStr);
	else
		{
		strncpy ((char*) DicomDict, BaseDir, 256 - strlen(tempStr));
		KFACTORFILE[256 - strlen(tempStr)] = 0;
		strcat((char*) DicomDict, tempStr);
		}

	if (!VRType.AttachRTC(DicomDict))
		{
		MissingDict = TRUE;
		printf("Dictionary file %s is missing, some server functions will not work correctly\n", DicomDict);
		}

	MyGetPrivateProfileString ( szRootSC, "NoDicomCheck", "0",
					tempStr, 64, ConfigFile);
	NoDicomCheck = atoi(tempStr);
	if (!NoDicomCheck)
		SetDicomErrorHandler(MyDcmError);
	
	ConfigDgate();

	// autostart lua file with same name as executable
	strcpy(tempStr, argv[0]);
	char *p = strrchr(tempStr, PATHSEPCHAR);
	if (!p) p = strrchr(tempStr, '/');
	if (!p) p = tempStr; else p++;
	char *q = strchr(p, '.');
	if (!q) q = p+strlen(p);
	strcpy(q, ".lua");
	if(stat(p, &st) != 0)
		{
		memmove(p+4, p, strlen(p)+1);
		strcpy(p, "lua");
		p[3] = PATHSEPCHAR;
		}
	if(stat(p, &st) == 0)
		{
  	  	char cmd[1024];
		sprintf(cmd, "dofile([[%s]])", p);
	        OperatorConsole.On();
		LoadKFactorFile((char*)KFACTORFILE);
		InitACRNemaAddressArray();
		EnterCriticalSection(&dolua_critical);
  	        globalPDU.SetLocalAddress ( (BYTE *)"standalone" );
  	        globalPDU.SetRemoteAddress ( (BYTE *)"standalone" );
  		globalPDU.ThreadNum = 0;
  	        struct scriptdata sd = {&globalPDU, NULL, NULL, 0, NULL, NULL, NULL, NULL, NULL, 0, 0};		
		lua_setvar(&globalPDU, "version", DGATE_VERSION);
		lua_createtable   (globalPDU.L, 0, 0);
		for (int i=0; i<argc; i++)
			{
			lua_pushinteger(globalPDU.L, i+1);
			lua_pushstring (globalPDU.L, argv[i]); 
			lua_settable (globalPDU.L, -3);
			}
                lua_setglobal (globalPDU.L, "arg");
		do_lua(&(globalPDU.L), cmd, &sd);
		LeaveCriticalSection(&dolua_critical);
  		// if (sd.DDO) delete sd.DDO;
  		if (sd.rc==2 || sd.rc==6) exit(1); // reject on startup
		}

  	char cmd[1024];
	MyGetPrivateProfileString("lua", "startup", "", cmd, 1024, ConfigFile);
	EnterCriticalSection(&dolua_critical);
        globalPDU.SetLocalAddress ( (BYTE *)"global" );
        globalPDU.SetRemoteAddress ( (BYTE *)"global" );
	globalPDU.ThreadNum = 0;
        struct scriptdata sd = {&globalPDU, NULL, NULL, 0, NULL, NULL, NULL, NULL, NULL, 0, 0};
	if (cmd[0]) do_lua(&(globalPDU.L), cmd, &sd);
	LeaveCriticalSection(&dolua_critical);
	// if (sd.DDO) delete sd.DDO;
	if (sd.rc==2 || sd.rc==6) exit(1); // reject on startup

#ifdef UNIX
	// set that pesky broken pipe signal to be ignored
	// I/O that would have generated a SIGPIPE will return EPIPE instead
	// this applies to named pipe writes and read/write of sockets

	signal(SIGPIPE, SIG_IGN);
#endif

	int	Socketfd;

#if defined(DO_LEAK_DETECTION) || defined(DO_VIOLATION_DETECTION)
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

#ifdef DO_LEAK_DETECTION
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
#endif

#ifdef DO_VIOLATION_DETECTION
	tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;
#endif

	_CrtSetDbgFlag(tmpDbgFlag);
	/* If a leak is detected, run again after specifying the serial num of the
	  leak using _CrtSetBreakAlloc(<id>); */
//	_CrtSetBreakAlloc(37);
#endif

	if ( ! (Socketfd = ParseArgs(argc, argv, &globalPDU) ))
		{
		if (RunServer)
			{
			struct stat statbuf;
			char TimeString[100], buf[64];
			int iNbConverters;

			UNUSED_ARGUMENT(buf);//Stop gcc4.2 warning bcb
			memset((void *)&statbuf, 0, sizeof(statbuf));
			
			InitializeCriticalSection(&count_critical);

			// Start queue for export converters (may reprocess previously failed requests)
			stat(argv[0], &statbuf);
			strcpy(TimeString, ctime_r(&statbuf.st_mtime, buf));
			TimeString[strlen(TimeString)-1] = '\0';
			if (!(statbuf.st_mode & S_IFREG)) strcpy(TimeString, "?");
			if (NoThread)
				OperatorConsole.printf("DGATE (%s, build %s, bits %d) runs in non-threaded debug mode\n", DGATE_VERSION, TimeString, 8*sizeof(void *));
			else
				OperatorConsole.printf("DGATE (%s, build %s, bits %d) is running as threaded server\n", DGATE_VERSION, TimeString, 8*sizeof(void *));
				
			{
			Database testDB;
			if (testDB.Open ( DataSource, UserName, Password, DataHost ) )
				{ 
				switch(testDB.db_type)
					{ 
					case DT_ODBC:             OperatorConsole.printf("Database type: ODBC connection\n"); break;
				  	case DT_POSTGRES:         OperatorConsole.printf("Database type: native PostGres connection\n"); break;
				  	case DT_MYSQL:            OperatorConsole.printf("Database type: native MySQL connection\n"); break;
				  	case DT_SQLITE:           OperatorConsole.printf("Database type: built-in SQLite driver\n"); break;
				  	case DT_DBASEIII:         OperatorConsole.printf("Database type: built-in Dbase driver\n"); break;
				  	case DT_DBASEIIINOINDEX:  OperatorConsole.printf("Database type: built-in Dbase driver (indices disabled)\n"); break;
				  	case DT_NULL:  		  OperatorConsole.printf("Database type: NULL driver (black hole)\n"); break;
					}
				};
			}

			if (MissingDict)
				OperatorConsole.printf("*** File dgate.dic is missing, some server functions will not work correctly\n");

			MyGetPrivateProfileString(szRootSC, "ExportConverters", "0", szTemp, 32, ConfigFile);
			iNbConverters = atoi(szTemp);
			if (iNbConverters>MAXExportConverters) iNbConverters=MAXExportConverters;

			if (iNbConverters && !NoThread)
			{ eqN = (struct conquest_queue **)malloc(iNbConverters * sizeof(struct conquest_queue *));
			  memset(eqN, 0, iNbConverters * sizeof(struct conquest_queue *));
			  for(i=0; i<iNbConverters; i++)
			    export_queueN(&eqN[i], NULL, i, NULL, NULL, NULL, NULL, NULL, NULL);
			  OperatorConsole.printf("Started %d export queue thread(s)\n", iNbConverters);
			}

			// Start queue for mirror copy (may reprocess previously failed requests)
			MyGetPrivateProfileString(szRootSC, "MIRRORDevice0", "-1", szTemp, 32, ConfigFile);
			if (atoi(szTemp)!=-1 && !NoThread) 
			{ mirrorcopy_queue(NULL, NULL, 0);
			  OperatorConsole.printf("Started mirror copy queue thread\n");
			}

			// Start prefetcher queue 
			MyGetPrivateProfileString(szRootSC, "Prefetcher", "0", szTemp, 32, ConfigFile);
			if (atoi(szTemp) && !NoThread) 
			{ prefetchermode = atoi(szTemp);
			  prefetcher(NULL, FALSE);
			  OperatorConsole.printf("Started prefetcher queue thread\n");
			}

			MyGetPrivateProfileString ( szRootSC, "WatchFolder", "",
					tempStr, 1024, ConfigFile);
			if (tempStr[0]) 
			{ strcpy(monitorfolder2, tempStr);
			  OperatorConsole.printf("Also monitoring for files in: %s\n", tempStr);
			  StartMonitorThread(monitorfolder2);
			}

			if (NoThread)
				while ( TRUE )
					ServerApp.Server(Port);
			else
				ServerApp.Server(Port);
			}
		return(0);
		}

	// run a child from a driver application
	OperatorConsole.printf("Child: Socketfd = %d\n", Socketfd);

	// Print Date / Time in Log file
	char buf[64];
	UNUSED_ARGUMENT(buf);//Stop gcc4.2 warning bcb
	TimeOfDay = time(NULL);
	strcpy(TimeString1, ctime_r(&TimeOfDay, buf));
	TimeString1[strlen(TimeString1)-1] = '\0';
	OperatorConsole.printf("UPACS: %s: STARTED AT: %s\n", argv[1],  TimeString1);

	ServerApp.ServerChild(Socketfd, 0);

	TimeOfDay2 = time(NULL);
	strcpy(TimeString1, ctime_r(&TimeOfDay2, buf));
	TimeString1[strlen(TimeString1)-1] = '\0';
	OperatorConsole.printf("UPACS: %s: ENDED AT: %s\n", argv[1],  TimeString1);
	OperatorConsole.printf("UPACS: %s: TOTAL RUNNING TIME: %d SECONDS\n", argv[1], TimeOfDay2 - TimeOfDay );

	return ( 0 );
	}

///////////////////////////////////////////////////////////////////////////////
// grabbing code
///////////////////////////////////////////////////////////////////////////////

void SetStringVR(VR **vr, int g, int e, const char *String)
{ unsigned int	len;
  char*		pCopy;

  /* NOTE:
	Uneven length strings must be padded with a space.
	Uneven length UIDs must be padded with \0.
  */
  len = strlen(String);
  pCopy = strdup(String);

  if ((g==0x0008 && e==0x0018) || (g==0x0020 && e==0x000d) || (g==0x0020 && e==0x000e))
  { if (len & 1) len++;
  }
  else
  { if (len & 1)
      pCopy[len++] = ' ';
  }

  *vr = new VR(g, e, len, (void*)pCopy, (BOOL) TRUE); 
}


BOOL GrabImagesFromServer(BYTE *calledae, const char *studydate, char *destination, int Thread)
	{
	BOOL			quit, quit2;
	PDU_Service		PDU, PDU2;
	DICOMCommandObject	DCO;
	DICOMCommandObject	DCOR, DCOR2;
	DICOMDataObject		DDO;
	DICOMDataObject		DDOR, DDOR2;
	UID			uid, uid2;
	VR			*vr1;
	LE_UINT16		command, datasettype, messageid, priority;
	BYTE			SOP[66];
  	unsigned char 		ip[64], port[64], compress[64];
	int			count=0;

	PDU.AttachRTC(&VRType);
	PDU2.AttachRTC(&VRType);

	if(!GetACRNema((char *)calledae, (char *)ip, (char *)port, (char *)compress))
		return FALSE;
		
	// Start setting up connection for C-FIND
	PDU.ClearAbstractSyntaxs();
	PDU.SetLocalAddress(MYACRNEMA);
	PDU.SetRemoteAddress(calledae);

	uid.Set("1.2.840.10008.3.1.1.1");	// Application context (DICOM App)
	PDU.SetApplicationContext(uid);

	uid.Set("1.2.840.10008.5.1.4.1.2.2.1"); // StudyRootQuery (C-Find SOP)
	PDU.AddAbstractSyntax(uid);

	// idem for the C-MOVE
	PDU2.ClearAbstractSyntaxs();
	PDU2.SetLocalAddress(MYACRNEMA);
	PDU2.SetRemoteAddress(calledae);

	uid2.Set("1.2.840.10008.3.1.1.1");	// Application context (DICOM App)
	PDU2.SetApplicationContext(uid2);

	uid2.Set("1.2.840.10008.5.1.4.1.2.2.2"); // StudyRootRetrieve (C-Move SOP)
	PDU2.AddAbstractSyntax(uid2);

        PDU.SetTimeOut(TCPIPTimeOut);
        PDU2.SetTimeOut(TCPIPTimeOut);

	// Make the association for the FIND on port/ip
	if(!PDU.Connect(ip, port))
		{
		OperatorConsole.printf("*** Grab - failed to connect for C-FIND\n");
		return ( FALSE );
		}

	// Make the association for the MOVE on port/ip
	if(!PDU2.Connect(ip, port))
		{
		PDU.Close();
		OperatorConsole.printf("*** Grab - failed to connect for C-MOVE\n");
		return(FALSE);
		}

	// Start a StudyRootQuery

	if (Thread) Progress.printf("Process=%d, Type='grabimagesfromserver', Active=1\n", Thread);

	strcpy((char*) SOP, "1.2.840.10008.5.1.4.1.2.2.1"); // StudyRootQuery (C-Find SOP)
	vr1 = new VR (0x0000, 0x0002, strlen((char*)SOP), (void*) SOP, FALSE);
	DCO.Push(vr1);
	command = 0x0020;
	vr1 = new VR (0x0000, 0x0100, 0x0002, &command, FALSE);
	DCO.Push(vr1);
	priority = 0;	// MEDIUM
	vr1 = new VR (0x0000, 0x0700, 0x0002, &priority, FALSE);
	DCO.Push(vr1);
	datasettype = 0x0102;	
	vr1 = new VR (0x0000, 0x0800, 0x0002, &datasettype, FALSE);
	DCO.Push(vr1);
	messageid = 0x0003;
	vr1 = new VR (0x0000, 0x0110, 0x0002, &messageid, FALSE);
	DCO.Push(vr1);

	// Start of data object. Query level is image; ask for image UIDs and patid

	SetStringVR(&vr1, 0x0008, 0x0052, "IMAGE"); 
	DDO.Push(vr1); 
	SetStringVR(&vr1, 0x0008, 0x0020, studydate); 
	DDO.Push(vr1); 
	SetStringVR(&vr1, 0x0008, 0x0018, "");
	DDO.Push(vr1); 
	SetStringVR(&vr1, 0x0010, 0x0020, "");
	DDO.Push(vr1); 

	// Write query command object and data object

	PDU.Write(&DCO, uid);
	PDU.Write(&DDO, uid);

	// Get response

	quit = 0;
	while(!quit)
		{
		if(!PDU.Read(&DCOR))
			{
			PDU2.Close();
			OperatorConsole.printf("*** Grab - association lost in C-FIND\n");
			if (Thread) Progress.printf("Process=%d, Active=0\n", Thread);
			return ( FALSE );	// association lost
			}
		// is this a C-Find-RSP ?
		if(DCOR.GetUINT16(0x0000, 0x0100)!=0x8020)
			{
			PDU.Close();
			PDU2.Close();
			OperatorConsole.printf("*** Grab - wrong response for C-FIND\n");
			if (Thread) Progress.printf("Process=%d, Active=0\n", Thread);
			return(FALSE);
			}
		// is there a data set?
		if(DCOR.GetUINT16(0x0000, 0x0800)==0x0101)
			{
			PDU.Close();
			PDU2.Close();
			OperatorConsole.printf("Grab - finished\n");
			if (Thread) Progress.printf("Process=%d, Active=0\n", Thread);
			return( TRUE );
			}
		// no success
		if(DCOR.GetUINT16(0x0000, 0x0900)==0x0000)
			{
			PDU.Close();
			PDU2.Close();
			OperatorConsole.printf("*** Grab - C-FIND failed\n");
			if (Thread) Progress.printf("Process=%d, Active=0\n", Thread);
			return ( FALSE );
			}
		// read response data
		if(!PDU.Read(&DDOR))
			{
			PDU.Close();
			PDU2.Close();
			OperatorConsole.printf("*** Grab - C-FIND failed\n");
			if (Thread) Progress.printf("Process=%d, Active=0\n", Thread);
			return(FALSE);
			}

		while((vr1 = DCOR.Pop()))
			delete vr1;

		while((vr1 = DDOR.Pop()))
			{
			if(vr1->Group == 0x0000)	// error response
				{
				PDU.Close();
				PDU2.Close();
				OperatorConsole.printf("*** Grab - C-FIND error\n");
				if (Thread) Progress.printf("Process=%d, Active=0\n", Thread);
				return ( TRUE );
				}

			// recieving a SOPInstanceUID
			if (vr1->Group == 0x0008 && vr1->Element == 0x0018)
				{
				char s[255], none[] = "";
				memset(s, 0, 255);
				memcpy(s, vr1->Data, vr1->Length);

				// is it on the local server (note: test may be speeded by passing patid) ?		
				if (!TestFilter(none, s, 10))
					{
					VR	*vr;

					// move it to the destination if it is not
					strcpy((char*) SOP, "1.2.840.10008.5.1.4.1.2.2.2"); // StudyRootMove (C-Move SOP)
					vr = new VR (0x0000, 0x0002, strlen((char*)SOP), (void*) SOP, FALSE);
					DCO.Push(vr);
					command = 0x0021;
					vr = new VR (0x0000, 0x0100, 0x0002, &command, FALSE);
					DCO.Push(vr);
					priority = 0;	// MEDIUM
					vr = new VR (0x0000, 0x0700, 0x0002, &priority, FALSE);
					DCO.Push(vr);
					datasettype = 0x0102;	
					vr = new VR (0x0000, 0x0800, 0x0002, &datasettype, FALSE);
					DCO.Push(vr);	
					messageid++;
					vr = new VR (0x0000, 0x0110, 0x0002, &messageid, FALSE);
					DCO.Push(vr);
					vr = new VR(0x0000, 0x0600, 16, (void*)destination, FALSE);
					DCO.Push(vr);

					// Data object. Query level is image; ask for date and SopInstanceUID 
					SetStringVR(&vr, 0x0008, 0x0052, "IMAGE"); 
					DDO.Push(vr); 
					SetStringVR(&vr, 0x0008, 0x0020, studydate); 
					DDO.Push(vr); 
					SetStringVR(&vr, 0x0008, 0x0018, s);
					DDO.Push(vr); 

					// Write command object and data object
					PDU2.Write(&DCO, uid2);
					PDU2.Write(&DDO, uid2);

					// Get response
					quit2 = 0;
					while(!quit2)
						{
						if(!PDU2.Read(&DCOR2))
							{
							PDU.Close();
							OperatorConsole.printf("*** Grab - C-MOVE association lost\n");
							if (Thread) Progress.printf("Process=%d, Active=0\n", Thread);
							return ( FALSE );	// associate lost
							}
						if(DCOR2.GetUINT16(0x0000, 0x0100)!=0x8021)
							{
							PDU.Close();
							PDU2.Close();
							OperatorConsole.printf("*** Grab - C-MOVE got a wrong response\n");
							if (Thread) Progress.printf("Process=%d, Active=0\n", Thread);
							return(FALSE);
							}
						// ignore the data set
						if(DCOR2.GetUINT16(0x0000, 0x0800)!=0x0101)
							{
							PDU2.Read(&DDOR2);
							DDOR2.Reset();
							}
						if(DCOR2.GetUINT16(0x0000, 0x0900)==0x0000)	// success ?
							{
							OperatorConsole.printf("Grabbed %s\n", s);
							quit2=1;
							}
						else if (DCOR2.GetUINT16(0x0000, 0x0900)==0xff00)	// continuing no problem
							{
							}
						else
							{
							UINT16	RemoteError = DCOR2.GetUINT16(0x0000, 0x0900);	// mvh 20051208
							OperatorConsole.printf("*** Grab - remote C-MOVE DICOM Error code %x (%d)\n", RemoteError, RemoteError);
							NonDestructiveDumpDICOMObject(&DCOR2);
							PDU.Close();
							PDU2.Close();
							if (Thread) Progress.printf("Process=%d, Active=0\n", Thread);
							return ( FALSE );
							}
						DCOR2.Reset();
						}

					}
				}

			delete vr1;
			}
		if (Thread) Progress.printf("Process=%d, Total=%d, Current=%d\n", Thread, 100, count++);
		}

	PDU.Close();
	PDU2.Close();
	if (Thread) Progress.printf("Process=%d, Active=0\n", Thread);
	return ( TRUE );
	}

/////////////////////////////////////////////////////////////////////////////////////////////
// Used to send commands (e.g, from linux gui to be developed) to running server without reindexing
/////////////////////////////////////////////////////////////////////////////////////////////

int processhtml(char *out, char *in, int len)
	{	
	int i, outlen=0, instring=0;
	for (i=0; i<len; i++)
		{
		if (i>3 && in[i-3]=='<' && in[i-2]=='A' && instring==0) instring=1;
		if (in[i]=='>' && instring==1) instring=0;

		if (in[i]==' ' && instring) { out[outlen++] = '%'; out[outlen++] = '2'; out[outlen++] = '0'; }
		else 		              out[outlen++] = in[i];
		}
	return outlen;
	}

// returns 1 on failure, 0 on success, -1 if value is returned on con, buf or L
static int SendServerCommand(const char *NKIcommand1, const char *NKIcommand2, int con, char *buf, BOOL html, BOOL upload, lua_State *L)
	{
	PDU_Service		PDU;
	DICOMCommandObject	DCO;
	DICOMCommandObject	DCOR;
	UID			uid;
	VR			*vr;
	LE_UINT16		command, datasettype, messageid;//, tuint16;
	BYTE			SOP[64];
	int			rc=0;

	PDU.ClearAbstractSyntaxs();
	PDU.SetLocalAddress(MYACRNEMA);
	PDU.SetRemoteAddress(MYACRNEMA);
	uid.Set("1.2.840.10008.3.1.1.1");	// Dicom APP
	PDU.SetApplicationContext(uid);
	uid.Set("1.2.840.10008.1.1");		// Verification
	PDU.AddAbstractSyntax(uid);

	if(!PDU.Connect((unsigned char *)ServerCommandAddress, Port))
		return ( 1 );
	
	strcpy((char*) SOP, "1.2.840.10008.1.1"); // Verification
	vr = new VR (0x0000, 0x0002, strlen((char*)SOP), (void*) SOP, FALSE);
	DCO.Push(vr);

	if (strlen(NKIcommand1))
	{ vr = new VR (0x9999, 0x0300, strlen((char*)NKIcommand1), (void*)NKIcommand1, FALSE);
	  DCO.Push(vr);
	}

	if (strlen(NKIcommand2))
	{ vr = new VR (0x9999, 0x0400, strlen((char*)NKIcommand2), (void*)NKIcommand2, FALSE);
	  DCO.Push(vr);
	}

	command = 0x0030;
	vr = new VR (0x0000, 0x0100, 0x0002, &command, FALSE);
	DCO.Push(vr);

	datasettype = 0x0101;	
	vr = new VR (0x0000, 0x0800, 0x0002, &datasettype, FALSE);
	DCO.Push(vr);

	messageid = 0x0001;
	vr = new VR (0x0000, 0x0110, 0x0002, &messageid, FALSE);
	DCO.Push(vr);

	if (upload && buf)
	{ unsigned int len = DFileSize(buf);
	  vr = new VR(0x9999, 0x0402, len, TRUE);
	  FILE *f;
	  f = fopen(buf, "rb");
	  fread((char*)(vr->Data), 1, len, f);
          fclose(f); 
          DCO.Push(vr);
          if (len&1)
          { UINT16 oddlength = 1;	
            VR *vr4 = new VR (0x9999, 0x0404, 2, &oddlength, FALSE);
            DCO.Push(vr4); // requires change in dimsec.cpp
          }
	}
        
        if (buf && !L) *buf=0;
	PDU.Write(&DCO, uid);

	if(!PDU.Read(&DCOR))
		return ( 1 );	// associate lost
        
        int oddlength = DCOR.GetUINT16(0x9999,0x0404)?1:0;

	while((vr = DCOR.Pop()))
		{
		if (vr->Group == 0x9999 && vr->Element == 0x0401)
			{
			if (vr->Length < 100)
				rc = atoi((char *)vr->Data);

			if (L)
				{
				int len=vr->Length;
                                if (oddlength) len--;
                                if (buf==NULL)
				{ if (len>1 && ((char *)(vr->Data))[len-1]==' ') len--;
				  else if (len>1 && ((char *)(vr->Data))[len-1]==0) len--;
                                }
                                lua_pushlstring(L, (char *)(vr->Data), len);
				rc = -1;
				}
			else if (buf)
				{
				int len;
				if (html) len = processhtml(buf, (char *)vr->Data, vr->Length);
                                else      memcpy(buf, vr->Data, len = vr->Length);
				buf[len]=0;
				if (len>1 && buf[len-1]==' ') len--;
				rc = -1;
				}
			else
				{
				if (con==0)
					{
					con = fileno(stdout);
#ifdef WIN32
					setmode(con, O_BINARY);
#endif
					}

				if (html)
					{ 
					char *buf1 = (char *)malloc(vr->Length*3);  // worst case; every char replaced by %CC
					int len = processhtml(buf1, (char *)vr->Data, vr->Length);
					if (len>1 && buf1[len-1]==0 && (len&1)==0) len--;
					write(con, buf1, len);
					free(buf1);
					rc = -1;
					}
				else
					{ int len = vr->Length;
					  // assume header is EVEN for application/dicom
					//if (len>33)
					  //if (memcmp(vr->Data, "Content-type: application/dicom\n", 32)==0)
					    //len--;
                                        if (oddlength) len--;
					write(con, vr->Data, len);
					rc = -1;
					}
				}
			}
		delete vr;
		}

	PDU.Close();
	return ( rc );
	}

static BOOL DcmEcho(const char *AE)
	{
	PDU_Service		PDU;
	DICOMCommandObject	DCO;
	DICOMCommandObject	DCOR;
	UID			uid;
	VR			*vr;
	LE_UINT16		command, datasettype, messageid;//, tuint16;
	BYTE			SOP[64], host[64], port[64], compr[64];

	PDU.ClearAbstractSyntaxs();
	PDU.SetLocalAddress(MYACRNEMA);
	PDU.SetRemoteAddress((BYTE *)AE);
	uid.Set("1.2.840.10008.3.1.1.1");	// Dicom APP
	PDU.SetApplicationContext(uid);
	uid.Set("1.2.840.10008.1.1");		// Verification
	PDU.AddAbstractSyntax(uid);

	if(!GetACRNema((char *)AE, (char *)host, (char *)port, (char *)compr)) 
		return FALSE;
	
	if(!PDU.Connect(host, port))
		return ( FALSE );
	
	strcpy((char*) SOP, "1.2.840.10008.1.1"); // Verification
	vr = new VR (0x0000, 0x0002, strlen((char*)SOP), (void*) SOP, FALSE);
	DCO.Push(vr);

	command = 0x0030;
	vr = new VR (0x0000, 0x0100, 0x0002, &command, FALSE);
	DCO.Push(vr);

	datasettype = 0x0101;	
	vr = new VR (0x0000, 0x0800, 0x0002, &datasettype, FALSE);
	DCO.Push(vr);

	messageid = 0x0002;
	vr = new VR (0x0000, 0x0110, 0x0002, &messageid, FALSE);
	DCO.Push(vr);

	PDU.Write(&DCO, uid);

	if(!PDU.Read(&DCOR))
		return ( FALSE );	// associate lost

	while((vr = DCOR.Pop()))
		{
		delete vr;
		}

	PDU.Close();
	return ( TRUE );
	}

/////////////////////////////////////////////////////////////////////////////////////////////
// Elementary web interface
/////////////////////////////////////////////////////////////////////////////////////////////

static int isxdig(char ch)
{ if ((ch>='0' && ch<='9') ||
      (ch>='A' && ch<='F') ||
      (ch>='a' && ch<='f'))
    return 1;
  else
    return 0;
}

static int xdigit(char ch)
{  if (ch>='0' && ch<='9')
    return ch - '0';
  else
    return (ch & 15) + 9;
}

static int htoin(const char *value, int len)
{ int i, result;

  result = 0;
  i      = 0;
  while (i < len && value[i] == ' ') i++;

  while (i < len && isxdig(value[i]))
  { result = result * 16 + xdigit(value[i]);
    i++;
  }

  return result;
}

static void HTML(const char *string, ...)
{ char n[2]="\n";
  char s[2500];
  va_list vargs;

  va_start(vargs, string);
  vsprintf(s, string, vargs);
  va_end(vargs);

  write(console, s, strlen(s));
  write(console, n, 1);
}

static int CGI(char *out, const char *name, const char *def)
{ char *p = global_query_string; // getenv( "QUERY_STRING" );
  char *q = getenv( "CONTENT_LENGTH" );
  char *r = getenv( "REQUEST_METHOD" );
  char tmp[512];
  int  i, j;
  char *fstart=NULL;
  int flen=0;
  
//FILE *g = fopen("c:\\temp\\postbuf.txt", "wt");
//fprintf(g, "%s\n%s\n%s\n", p,q,r);
//fclose(g);

  if (out!=def) *out = 0;

  // if (r!=NULL && memcmp(r, "POST", 3)==0)
  { if (q!=NULL && *q!=0 && atoi(q)!=0 && post_buf==NULL) 
    { post_len = atoi(q);
      post_buf = (char *)malloc(post_len+1);
      post_buf[post_len]=0;
#ifdef WIN32
      setmode(fileno(stdin), O_BINARY);
#endif
      fread(post_buf, post_len, 1, stdin);
      p = post_buf;

//FILE *g = fopen("c:\\temp\\postbuf.bin", "wb");
//fwrite(post_buf, post_len, 1, g);
//fclose(g);
    }
    else
      if (post_buf) p = post_buf;

    if (p==NULL) return 0;

    if (post_len>0 && p[0]=='-')      // multipart data, locate the file (one assumed)
    { q = strstr(p, "filename=");
            
      if (q)
      { *q=0;	      // cut contents short to allow fast read of other entries

        char *ext = strchr(q+1, 0x0d);   // parse extension from filename
        q = ext+1;
        if (ext)
        { *ext--=0;
          if (ext[0]=='"')
          { *ext--=0;
          }
          while (*ext!='.' && *ext!=0) ext--;
        }

        q = strchr(q+1, 0x0a);  // file starts after three newlines
        if (q)
        { q = strchr(q+1, 0x0a);
	  *q = 0;
	  fstart = q+1;
	                      // file ends after two newlines (one at end)
	  flen = post_len - (fstart-post_buf);
	  flen-=4;
	  while (fstart[flen]!=0x0d && flen>0) flen--;
	  
	  post_file = fstart;
	  post_len = flen;

	  NewTempFile(uploadedfile, ext);
	  FILE *g = fopen(uploadedfile, "wb");
          if (g) 
	  { fwrite(fstart, flen, 1, g);
            fclose(g);
	  }
        }
      }

      if (strcmp(name, "_filename_")==0 && uploadedfile[0])
      { strcpy(out, uploadedfile);
        return 0;
      }

      strcpy(tmp, "name=\"");
      strcat(tmp, name);
      strcat(tmp, "\x22\x0d\x0a\x0d\x0a");

      p = strstr(p, tmp);	      // parse data
      if (p)
      { p += strlen(tmp);
        q = strchr(p, 0x0d);
        if (q) *q=0;
        strcpy(out, p);
        if (q) *q=0x0d; // 20191227
        return 0;
      }

      if (out!=def) strcpy(out, def);
      return 0;
    }
    else if (*uploadedfile==0 && post_len>0 && p[0]=='<')      // xml
    { NewTempFile(uploadedfile, ".xml");
      post_file = post_buf;
      FILE *g = fopen(uploadedfile, "wb");
      if (g)
      { fwrite(p, post_len, 1, g);
        fclose(g);
      }
      p = global_query_string; //getenv( "QUERY_STRING" );
    }
    else if (*uploadedfile==0 && post_len>0)      		// any other type
    { NewTempFile(uploadedfile, ".dat");
      post_file = post_buf;
      FILE *g = fopen(uploadedfile, "wb");
      if (g)
      { fwrite(p, post_len, 1, g);
        fclose(g);
      }
      p = global_query_string; //getenv( "QUERY_STRING" );
    }
    else
      p = global_query_string; //getenv( "QUERY_STRING" );
  }


  strcpy(tmp, "&");	// & only hits on second item
  strcat(tmp, name);
  strcat(tmp, "=");
  
  // check first item
  if (p)
  { if (strlen(p)>=strlen(tmp+1) && memcmp(p, tmp+1, strlen(tmp+1))==0) q=p-1;
    else q = strstr(p, tmp);
  }
  else
    q=NULL;

  if (q==NULL)
  { if (out!=def) strcpy(out, def);
    return 0;
  }

  q = q + strlen(tmp);

  i = 0; j = 0;
  while (q[i] != 0  && q[i] != '&')
  { if (q[i] == '%')
    { tmp[j++] = (char)(htoin(q+i+1, 2));
      i = i+3;
    }
    else if (q[i] == '+')
    { tmp[j++] = ' ';
      i++;
    }
    else
      tmp[j++] = q[i++];
  }
  tmp[j++] = 0;

  strcpy(out, tmp);

  return 0;
}

static int cgi_parse(char *in, char *out, const char *name, const char *def)
{ char *p = in, *q;
  char tmp[512];
  int  i, j;

  if (out!=def) *out = 0;
  strcpy(tmp, "&");	// & only hits on second item
  strcat(tmp, name);
  strcat(tmp, "=");
  
  // check first item
  if (p)
  { if (strlen(p)>=strlen(tmp+1) && memcmp(p, tmp+1, strlen(tmp+1))==0) q=p-1;
    else q = strstr(p, tmp);
  }
  else
    q=NULL;

  if (q==NULL)
  { if (out!=def) strcpy(out, def);
    return 0;
  }

  q = q + strlen(tmp);

  i = 0; j = 0;
  while (q[i] != 0  && q[i] != '&')
  { if (q[i] == '%')
    { tmp[j++] = (char)(htoin(q+i+1, 2));
      i = i+3;
    }
    else if (q[i] == '+')
    { tmp[j++] = ' ';
      i++;
    }
    else
      tmp[j++] = q[i++];
  }
  tmp[j++] = 0;

  strcpy(out, tmp);

  return 0;
}

static BOOL Tabulate(const char *c1, const char *par, const char *c4, BOOL edit=FALSE)
{ const char *p=strchr(par, ':');
  char buf[512];

  SendServerCommand("", par, 0, buf);
  if (buf[0])
  { if (edit) HTML("<TR><TD>%s<TD>%s<TD><INPUT SIZE=64 NAME=\"%s\" VALUE=\"%s\"><TD>%s<TD></TR>", c1, p+1, p+1, buf, c4, p+1);
    else      HTML("<TR><TD>%s<TD>%s<TD>%s<TD>%s<TD></TR>", c1, p+1, buf, c4, p+1);
    return TRUE;
  }
  else
    return FALSE;
}

static void replace(char *string, const char *key, const char *value)
{ char temp[1000];
  char *p = strstr(string, key), *q = string;

  if (p==NULL) return;
  if (value==NULL) return;

  temp[0] = 0;
  while(p)
  { *p = 0;
    strcat(temp, string);
    *p = key[0];
    strcat(temp, value);
    string = p + strlen(key);
    p = strstr(string, key);
  };

  strcat(temp, string);
  strcpy(q, temp);
}

static BOOL DgateWADO(int arg, char **argv);

static void DgateCgi(char *ext, char *argv0, int argc, char **argv)
{ char mode[512], command[8192], size[32], dsize[32], iconsize[32], slice[512], slice2[512], query[512], buf[512], 
       patientidmatch[512], patientnamematch[512], studydatematch[512], startdatematch[512], 
       db[256], series[512], study[512], compress[64], WebScriptAddress[256], WebMAG0Address[256], 
       WebServerFor[64], WebCodeBase[512], lw[64], source[64], dest[64], series2[512], study2[512],
       DefaultPage[256], AnyPage[256], key[512], AnyPageExceptions[512];
  char ex[128], extra[256], graphic[32], viewer[128], studyviewer[128], script[1024];
  unsigned int  i, j;
  DBENTRY *DBE;
  char  RootSC[64];
  char  Temp[128];
  char  *p1;
  
  if (strrchr(argv0, PATHSEPCHAR))
  { strcpy(ConfigFile, argv0);
    *(strrchr(ConfigFile, PATHSEPCHAR) + 1) = 0;
    strcpy(BaseDir, ConfigFile);
    strcat(ConfigFile, "dicom.ini");
    chdir(BaseDir);
  }
  
  *uploadedfile=0;
  if (DgateWADO(argc, argv)) return;

  BOOL ReadOnly=FALSE;
  BOOL WebPush=TRUE;
	
  console = fileno(stdout);
#ifdef WIN32
  strcpy(ex, ".exe");
  if (ext) strcpy(ex, ext);
  setmode(console, O_BINARY);
#else
  ex[0]=0;
  if (ext) strcpy(ex, ext);
#endif

  ConfigMicroPACS();
  LoadKFactorFile((char*)KFACTORFILE);
  
  MyGetPrivateProfileString ( RootConfig, "MicroPACS", RootConfig, RootSC, 64, ConfigFile);
  MyGetPrivateProfileString ( RootSC, "Dictionary", "", Temp, 64, ConfigFile);
  if (Temp[0]) 
  { VRType.AttachRTC(Temp);
    globalPDU.AttachRTC(&VRType);
    InitACRNemaAddressArray();
  }

  MyGetPrivateProfileString ( RootSC, "WEBReadOnly", "1", Temp, 64, ConfigFile);
  ReadOnly = atoi(Temp);
  MyGetPrivateProfileString ( RootSC, "WEBPush", "1", Temp, 64, ConfigFile);
  WebPush = atoi(Temp);

  sprintf(WebScriptAddress, "http://%s/cgi-bin/dgate%s", getenv("SERVER_NAME"), ex);
  MyGetPrivateProfileString ( RootSC, "WEBScriptAddress", WebScriptAddress, WebScriptAddress, 256, ConfigFile);

  sprintf(WebMAG0Address, "http://%s/mag0", getenv("SERVER_NAME"));//, ex); bcb, only 1 %s
  MyGetPrivateProfileString ( RootSC, "WEBMAG0Address", WebMAG0Address, WebMAG0Address, 256, ConfigFile);

  sprintf(WebServerFor, "127.0.0.1");
  MyGetPrivateProfileString ( RootSC,        "WebServerFor", WebServerFor, WebServerFor, 256, ConfigFile);
  MyGetPrivateProfileString ( "webdefaults", "address",      WebServerFor, WebServerFor, 256, ConfigFile);
  strcpy(ServerCommandAddress, WebServerFor);

  strcpy(WebCodeBase, WebScriptAddress);		// note: WebCodeBase should include trailing slash or backslash
  p1 = strrchr(WebCodeBase, '/'); 
  if (p1) p1[0]=0;
  p1 = strrchr(WebCodeBase, '/'); 
  if (p1) p1[1]=0;
  MyGetPrivateProfileString ( RootSC, "WebCodeBase", WebCodeBase, WebCodeBase, 256, ConfigFile);

  // If no mode is specified, go to this page
  // MyGetPrivateProfileString ( RootSC, "DefaultPage", "", DefaultPage, 256, ConfigFile);
  *DefaultPage=0;
  MyGetPrivateProfileString ( "DefaultPage", "source", "", buf, 512, ConfigFile);
  MyGetPrivateProfileString ( "DefaultPage", "line0", buf, buf, 512, ConfigFile);
  if (buf[0]) strcpy(DefaultPage, "DefaultPage");

  // If this page is specified, all requests land here (effectively disabling the built-in web pages)
  // MyGetPrivateProfileString ( RootSC, "AnyPage", "", AnyPage, 256, ConfigFile);
  *AnyPage=0;
  MyGetPrivateProfileString ( "AnyPage", "source", "", buf, 512, ConfigFile);
  MyGetPrivateProfileString ( "AnyPage", "line0", buf, buf, 512, ConfigFile);
  if (buf[0]) strcpy(AnyPage, buf);

  strcpy(AnyPageExceptions, ",");
  MyGetPrivateProfileString ( "AnyPage", "exceptions", "", AnyPageExceptions+1, 511, ConfigFile);
  strcat(AnyPageExceptions, ",");

  MyGetPrivateProfileString ( "webdefaults", "port", (char *)Port, (char*)Port,       256, ConfigFile);

  //CGI((char *)Port,         "port",    (char *)Port);	// allow serving any server
  //CGI(ServerCommandAddress, "address", ServerCommandAddress);

  CGI(mode,    "mode",     "");		// web page
  char mode2[256];
  strcpy(mode2, ",");
  strcat(mode2, mode);
  strcat(mode2, ",");
  if (strstr(AnyPageExceptions, mode2)) AnyPage[0]=0; // 20151026
  if (AnyPage[0]) 
  { strcpy(mode, "AnyPage");
    strcpy(DefaultPage, "AnyPage");
  }

  CGI(query,   "query",    "");		// query for most db selectors
  CGI(slice2,  "slice",    "");		// patid:sop for slice
  CGI(series2, "series",   "");		// patid:seriesuid for seriesviewer/move/delete
  CGI(study2,  "study",    "");		// patid:studyuid for move/delete
  CGI(db,      "db",       "");		// database to edit or list
  CGI(lw,      "lw",       "0/0");	// level/window
  CGI(source,  "source",   "(local)");  // source for move
  CGI(dest,    "dest",     "");		// destination for move
  CGI(key,     "key",      "");		// key for mark
  CGI(script,  "script",   "");		// script for attachfile

  j = 0;
  for(i=0; i<strlen(slice2); i++) if (slice2[i]==' ') { slice[j++]='%'; slice[j++]='2'; slice[j++]='0'; } else slice[j++]=slice2[i];
  slice[j++]=0;

  j = 0;
  for(i=0; i<strlen(series2); i++) if (series2[i]==' ') { series[j++]='%'; series[j++]='2'; series[j++]='0'; } else series[j++]=series2[i];
  series[j++]=0;

  j = 0;
  for(i=0; i<strlen(study2); i++) if (study2[i]==' ') { study[j++]='%'; study[j++]='2'; study[j++]='0'; } else study[j++]=study2[i];
  study[j++]=0;

  MyGetPrivateProfileString ( "webdefaults", "size",     "512", size,     256, ConfigFile);
  MyGetPrivateProfileString ( "webdefaults", "dsize",    "0",   dsize,    256, ConfigFile);
  MyGetPrivateProfileString ( "webdefaults", "compress", "n4",  compress, 256, ConfigFile);
  MyGetPrivateProfileString ( "webdefaults", "iconsize", "48",  iconsize, 256, ConfigFile);
  MyGetPrivateProfileString ( "webdefaults", "graphic",  "bmp", graphic,   32, ConfigFile);
  MyGetPrivateProfileString ( "webdefaults", "viewer",   "seriesviewer", viewer, 128, ConfigFile);
  MyGetPrivateProfileString ( "webdefaults", "studyviewer",   "", studyviewer, 128, ConfigFile);

  CGI(size,    "size",     size);	// size of viewer in pixels or %
  CGI(dsize,   "dsize",    dsize);	// max size of transmitted dicom images in pixels, 0=original
  CGI(compress,"compress", compress);	// compression of transmitted dicom images to (our) web viewer
  CGI(iconsize,"iconsize", iconsize);	// size of icons in image table
  CGI(graphic, "graphic",  graphic);	// style of transmitting thumbnails and slices (gif, bmp, or jpg)
  CGI(viewer,  "viewer",   viewer);	// mode of used viewer

  CGI(patientidmatch,   "patientidmatch",   "");	// search strings
  CGI(patientnamematch, "patientnamematch", "");
  CGI(studydatematch,   "studydatematch",   "");
  CGI(startdatematch,   "startdatematch",   "");
  
  // allow overrule of port, ip and AE in dicom.ini from command line (e.g. in php or node.js)
  for (i=1; i<argc; i++)
	{ 
	if (argv[i][0]=='-')
		{
		if (argv[i][1]=='h') strcpy((char *)MYACRNEMA, argv[i]+2);
		else if (argv[i][1]=='p') strcpy((char *)Port, argv[i]+2);
		else if (argv[i][1]=='q') strcpy(ServerCommandAddress, argv[i]+2);
		}
	}
  
  sprintf(extra, "port=%s&address=%s", Port, ServerCommandAddress);

  if (patientidmatch[0]!=0)
  { if (query[0]) strcat(query, " and ");
    strcat(query, "PatientID like '%");
    strcat(query, patientidmatch);
    strcat(query, "%'");
  };

  if (patientnamematch[0]!=0)
  { if (query[0]) strcat(query, " and ");
    strcat(query, "PatientNam like '%");
    strcat(query, patientnamematch);
    strcat(query, "%'");
  };

  if (studydatematch[0]!=0)
  { if (query[0]) strcat(query, " and ");
    strcat(query, "StudyDate like '%");
    strcat(query, studydatematch);
    strcat(query, "%'");
  };

  if (startdatematch[0]!=0)
  { if (query[0]) strcat(query, " and ");
    strcat(query, "StartDate like '%");
    strcat(query, startdatematch);
    strcat(query, "%'");
  };

  if      (stricmp(db, "dicomworklist")==0) DBE=WorkListDB;
  else if (stricmp(db, "dicompatients")==0) DBE=PatientDB;
  else if (stricmp(db, "dicomstudies" )==0) DBE=StudyDB;
  else if (stricmp(db, "dicomseries"  )==0) DBE=SeriesDB;
  else if (stricmp(db, "dicomimages"  )==0) DBE=ImageDB;
  else                                      DBE=WorkListDB;

  /************************** top page **************************/

  if ((strcmp(mode, "")==0 && !DefaultPage[0]) || strcmp(mode, "top")==0)
  { HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H1>Welcome to the Conquest DICOM server - version %s</H1>", DGATE_VERSION);
    HTML("<IMG SRC='%sconquest.jpg' ALT='Developed in the Conquest project'>", WebCodeBase);
    HTML("<HR>");
    HTML("<PRE>");
    SendServerCommand("", "display_status:", console);
    HTML("</PRE>");

    HTML("<HR>");

    HTML("<table>");
    HTML("<tr>");
    //HTML("<FORM ACTION=\"dgate%s\" METHOD=POST>", ex);
    HTML("<FORM ACTION=\"dgate%s\">", ex);
    HTML("<INPUT NAME=mode    TYPE=HIDDEN VALUE=querypatients>");
    HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", (char *)Port);
    HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", ServerCommandAddress);
    HTML("<INPUT NAME=key     TYPE=HIDDEN VALUE=%s>", key);
    HTML("<td>List local patients");
    HTML("<td>Patient ID: <INPUT NAME=patientidmatch TYPE=Text VALUE=>");
    HTML("<td>Name: <INPUT NAME=patientnamematch TYPE=Text VALUE=>");
    HTML("<td>");
    HTML("<td><INPUT TYPE=SUBMIT VALUE=Go>");
    HTML("</FORM>");
    HTML("</tr>");

    HTML("<tr>");
    HTML("<FORM ACTION=\"dgate%s\">", ex);
    HTML("<INPUT NAME=mode    TYPE=HIDDEN VALUE=querystudies>");
    HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", (char *)Port);
    HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", ServerCommandAddress);
    HTML("<INPUT NAME=key     TYPE=HIDDEN VALUE=%s>", key);
    HTML("<td>List local studies");
    HTML("<td>Patient ID: <INPUT NAME=patientidmatch TYPE=Text VALUE=>");
    HTML("<td>Name: <INPUT NAME=patientnamematch TYPE=Text VALUE=>");
    HTML("<td>Date: <INPUT NAME=studydatematch TYPE=Text VALUE=>");
    HTML("<td><INPUT TYPE=SUBMIT VALUE=Go>");
    HTML("</FORM>");
    HTML("</tr>");

    HTML("<tr>");
    HTML("<FORM ACTION=\"dgate%s\">", ex);
    HTML("<INPUT NAME=mode    TYPE=HIDDEN VALUE=queryworklist>");
    HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", (char *)Port);
    HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", ServerCommandAddress);
    HTML("<td>List local worklist");
    HTML("<td>Patient ID: <INPUT NAME=patientidmatch TYPE=Text VALUE=>");
    HTML("<td>Name: <INPUT NAME=patientnamematch TYPE=Text VALUE=>");
    HTML("<td>Date: <INPUT NAME=startdatematch TYPE=Text VALUE=>");
    HTML("<td><INPUT TYPE=SUBMIT VALUE=Go>");
    HTML("</FORM>");
    HTML("</tr>");

    HTML("<tr>");
    HTML("<FORM ACTION=\"dgate%s\">", ex);
    HTML("<td>Find ");
    HTML("<select name=mode>");
    HTML("  <option value='patientfinder'>Patient</option>");
    HTML("  <option value='studyfinder'>Study</option>");
    HTML("  <option value='seriesfinder'>Series</option>");
    HTML("</select>");
    HTML("<td>on server");
    HTML("<select name=dest>");
    SendServerCommand("", "get_amaps:<option value='%s'>%0.0s%0.0s%0.0s%s</option>", console);
    HTML("</select>");
    HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", (char *)Port);
    HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", ServerCommandAddress);
    HTML("<INPUT NAME=source  TYPE=HIDDEN VALUE=%s>", source);
    HTML("<INPUT NAME=dest    TYPE=HIDDEN VALUE=%s>", dest);
    HTML("<INPUT NAME=key     TYPE=HIDDEN VALUE=%s>", key);
    HTML("<td>Search: <INPUT NAME=query TYPE=Text VALUE=>");
    HTML("<td><td><INPUT TYPE=SUBMIT VALUE=Go>");
    HTML("</FORM>");
    HTML("</tr>");
    HTML("</table>");

    HTML("<FORM ACTION=\"dgate%s\" METHOD=POST ENCTYPE=\"multipart/form-data\">", ex);
    HTML("<INPUT NAME=mode    TYPE=HIDDEN VALUE=addlocalfile>");
    HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", (char *)Port);
    HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", ServerCommandAddress);
    HTML("Upload file to enter into server (dcm/v2/HL7/zip/7z/gz/tar): <INPUT NAME=filetoupload SIZE=40 TYPE=file VALUE=>");
    HTML("<INPUT TYPE=SUBMIT VALUE=Go>");
    HTML("</FORM>");

    HTML("<HR>");
    HTML("<BR><A HREF=\"dgate%s?%s&mode=showconfig\">Show server configuration</A>", ex, extra);
    HTML("<BR><A HREF=\"dgate%s?%s&mode=showsops\">Show server accepted SOPs</A>", ex, extra);
    HTML("<BR><A HREF=\"dgate%s?%s&mode=showdb\">Show database layout</A>", ex, extra);
    HTML("<BR><A HREF=\"dgate%s?%s&mode=showdictionary\">Show DICOM dictionary (Long!)</A>", ex, extra);
    HTML("</BODY>");
    exit(0);
  };

  /************************** configuration **************************/

  if (strcmp(mode, "showconfig")==0)
  { ReadOnly = TRUE;

    HTML("Content-type: text/html\nCache-Control: no-cache\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H1>Welcome to the Conquest DICOM server - version %s</H1>", DGATE_VERSION);
    HTML("<FORM ACTION=\"dgate%s\">", ex);
    if (!ReadOnly) HTML("<INPUT NAME=mode TYPE=HIDDEN VALUE=updateconfig>");
    HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", (char *)Port);
    HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", ServerCommandAddress);
    HTML("<HR>");
    HTML("<table RULES=ALL BORDER=1>");

    HTML("<Caption>List of configuration parameters</caption>");
    HTML("<TR><TD>File<TD>Parameter<TD>Value<TD>Description</TR>");
    Tabulate("dicom.ini", "get_param:MyACRNema", "DICOM name of server", !ReadOnly);
    Tabulate("dicom.ini", "get_param:TCPPort",   "TCP/IP port where server listens", !ReadOnly);
    Tabulate("dicom.ini", "get_param:FileNameSyntax", "Determines name for new files", !ReadOnly);
    Tabulate("dicom.ini", "get_param:FixPhilips", "Strip leading zeros from 10 digit patient ID", !ReadOnly);
    Tabulate("dicom.ini", "get_param:FixKodak", "Strip leading zeros from 8 digit patient ID", !ReadOnly);
    Tabulate("dicom.ini", "get_param:IncomingCompression", "Compression mode for incoming files", !ReadOnly);
    Tabulate("dicom.ini", "get_param:ArchiveCompression", "Compression mode for acrhived files", !ReadOnly);
    Tabulate("dicom.ini", "get_param:MAGDevices", "Number of storage devices", !ReadOnly);
    for (i=0; i<100; i++)
    { sprintf(command, "get_param:MAGDevice%d", i);
      if (!Tabulate("dicom.ini", command, "Directory where data is stored", !ReadOnly)) break;
    }   
    for (i=0; i<10; i++)
    { sprintf(command, "get_param:VirtualServerFor%d", i);
      if (!Tabulate("dicom.ini", command, "Act as pass through for this server", !ReadOnly)) break;
    }
    for (i=0; i<100; i++)
    { sprintf(command, "get_param:ExportConverter%d", i);
      if (!Tabulate("dicom.ini", command, "Script applied to just saved data", !ReadOnly)) break;
    }   
    for (i=0; i<100; i++)
    { sprintf(command, "get_param:ImportConverter%d", i);
      if (!Tabulate("dicom.ini", command, "Script applied to incoming images", !ReadOnly)) break;
    }   
    Tabulate("dicom.ini", "get_param:QueryConverter0", "Script applied to queries", !ReadOnly);
    Tabulate("dicom.ini", "get_param:WorkListQueryConverter0", "Script applied to worklist queries", !ReadOnly);
    Tabulate("dicom.ini", "get_param:RetrieveConverter0", "Script applied to transmitted images", !ReadOnly);
    Tabulate("dicom.ini", "get_param:RetrieveResultConverter0", "Script applied to query results", !ReadOnly);
    Tabulate("dicom.ini", "get_param:ModalityWorkListQueryResultConverter0", "Script applied to worklist query results", !ReadOnly);
    HTML("<TR></TR>");
    for (i=0; i<1000; i++)
    { sprintf(command, "get_amap:%d", i);
      if (!Tabulate((const char *)ACRNEMAMAP, command, "Known DICOM provider")) break;
    }

    HTML("</table>");
    if (!ReadOnly) HTML("<INPUT TYPE=SUBMIT VALUE=Update>");
    HTML("</FORM>");
    HTML("</BODY>");
    exit(0);
  };

  if (strcmp(mode, "showsops")==0)
  { HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H1>Welcome to the Conquest DICOM server - version %s</H1>", DGATE_VERSION);
    HTML("<HR>");
    HTML("<table RULES=ALL BORDER=1>");

    HTML("<Caption>List of uids</caption>");
    HTML("<TR><TD>File<TD>Parameter<TD>Value<TD>Description</TR>");
    for (i=0; i<1000; i++)
    { sprintf(command, "get_sop:%d", i);
      if (!Tabulate("dgatesop.lst", command, "Accepted SOP class uid")) break;
    }
    for (i=0; i<1000; i++)
    { sprintf(command, "get_transfer:%d", i);
      if (!Tabulate("dgatesop.lst", command, "Accepted transfer syntax ")) break;
    }
    for (i=0; i<1000; i++)
    { sprintf(command, "get_application:%d", i);
      if (!Tabulate("dgatesop.lst", command, "Accepted application uid")) break;
    }
    for (i=0; i<1000; i++)
    { sprintf(command, "get_localae:%d", i);
      if (!Tabulate("dgatesop.lst", command, "Accepted called AE title")) break;
    }
    for (i=0; i<1000; i++)
    { sprintf(command, "get_remoteae:%d", i);
      if (!Tabulate("dgatesop.lst", command, "Accepted calling AE title")) break;
    }
    HTML("</table>");
    HTML("</BODY>");
    exit(0);
  };

  if (strcmp(mode, "showdb")==0)
  { HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H1>Welcome to the Conquest DICOM server - version %s</H1>", DGATE_VERSION);
    HTML("<HR>");
    HTML("<table RULES=ALL BORDER=1>");

    HTML("<Caption>List of configuration parameters</caption>");
    HTML("<TR><TD>File<TD>Parameter<TD>Value<TD>Description</TR>");
    for (i=0; i<1000; i++)
    { sprintf(command, "get_sqldef:PATIENT,%d", i);
      if (!Tabulate("dicom.sql", command, "Patient database field definition")) break;
    }
    for (i=0; i<1000; i++)
    { sprintf(command, "get_sqldef:STUDY,%d", i);
      if (!Tabulate("dicom.sql", command, "Study database field definition")) break;
    }
    for (i=0; i<1000; i++)
    { sprintf(command, "get_sqldef:SERIES,%d", i);
      if (!Tabulate("dicom.sql", command, "Series database field definition")) break;
    }
    for (i=0; i<1000; i++)
    { sprintf(command, "get_sqldef:IMAGE,%d", i);
      if (!Tabulate("dicom.sql", command, "Image database field definition")) break;
    }
    for (i=0; i<1000; i++)
    { sprintf(command, "get_sqldef:WORKLIST,%d", i);
      if (!Tabulate("dicom.sql", command, "Worklist database field definition")) break;
    }
    HTML("</table>");
    HTML("</BODY>");
    exit(0);
  };

  if (strcmp(mode, "showdictionary")==0)
  { HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H1>Welcome to the Conquest DICOM server - version %s</H1>", DGATE_VERSION);
    HTML("<HR>");
    HTML("<table RULES=ALL BORDER=1>");

    HTML("<Caption>List of configuration parameters</caption>");
    HTML("<TR><TD>File<TD>Parameter<TD>Value<TD>Description</TR>");
    for (i=0; i<9000; i++)
    { sprintf(command, "get_dic:%d", i);
      if (!Tabulate("dgate.dic", command, "Dictionary item")) break;
    }
    HTML("</table>");
    HTML("</BODY>");
    exit(0);
  };

/*  if (strcmp(mode, "updateconfig")==0)
  { HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H1>Welcome to the Conquest DICOM server - version %s</H1>", DGATE_VERSION);
    HTML("<HR>");
    if (!ReadOnly) HTML("Updating the configuration through the WEB interface is not yet supported");
    else           HTML("Updating the configuration through the WEB interface is not allowed");
    HTML("</BODY>");
    exit(0);
  };
*/

  /************************** local browsers **************************/

  if (strcmp(mode, "querypatients")==0)
  { HTML("Content-type: text/html\nCache-Control: no-cache\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>",DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    HTML("<table RULES=ALL BORDER=1>");
    if (query[0]) HTML("<Caption>List of selected patients on local server</caption>");
    else          HTML("<Caption>List of all patients on local server</caption>");
    HTML("<TR><TD>Patient ID<TD>Name<TD>Sex<TD>Birth date</TR>");

    strcpy(command,                 "query:DICOMPatients|patientid,patientnam,patientsex,patientbir|");
    strcpy(command+strlen(command),  query);
    sprintf(command+strlen(command), "|<TR><TD><A HREF=dgate%s?%s&mode=querystudies&key=%s&query=patientid+=+'%%s'>%%0.0s %%0.0s %%0.0s", ex, extra, key);
    strcpy(command+strlen(command), "%s</A><TD>%s<TD>%s<TD>%s</TR>");
    SendServerCommand("", command, console);

    HTML("</table>");
    HTML("</BODY>");
    exit(0);
  };

  if (strcmp(mode, "querystudies")==0)
  { HTML("Content-type: text/html\nCache-Control: no-cache\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    HTML("<table RULES=ALL BORDER=1>");
    if (query[0]) HTML("<Caption>List of selected studies on local server</caption>");
    else          HTML("<Caption>List of all studies on local server</caption>");
    HTML("<TR><TD>Patient ID<TD>Name<TD>Study date<TD>Study description<TD>Study modality</TR>");

    strcpy(command,                 "query:DICOMStudies|patientid,studyinsta,patientnam,studydate,studydescr,studymodal|");
    strcpy(command+strlen(command),  query);
    sprintf(command+strlen(command), "|<TR><TD><A HREF=dgate%s?%s&mode=queryseries&key=%s&query=DICOMStudies.patientid+=+'%%s'+and+DICOMSeries.studyinsta+=+'%%s'>%%0.0s %%0.0s %%0.0s %%0.0s", ex, extra, key);
    strcpy(command+strlen(command), "%s</A><TD>%0.0s %s<TD>%s<TD>%s<TD>%s");
    if (studyviewer[0])
      sprintf(command+strlen(command), "<TD><A HREF=dgate%s?%s&mode=%s&study=%%s:%%s&size=%s>view%%0.0s %%0.0s %%0.0s %%0.0s</A>", ex, extra, studyviewer, size);
    if (WebPush)
      sprintf(command+strlen(command), "<TD><A HREF=dgate%s?%s&mode=studymover&study=%%s:%%s&source=%s&key=%s>push%%0.0s %%0.0s %%0.0s %%0.0s</A>", ex, extra, source, key);
    MyGetPrivateProfileString ( "markstudy", "source", "", buf, 1000, ConfigFile);
    MyGetPrivateProfileString ( "markstudy", "line0", buf, buf, 1000, ConfigFile);
    if (buf[0] && key[0])
    { MyGetPrivateProfileString ( "markstudy", "caption", "Mark", buf, 1000, ConfigFile);
      sprintf(command+strlen(command), "<TD><A HREF=dgate%s?%s&mode=markstudy&study=%%s:%%s&source=%s&key=%s>%s%%0.0s %%0.0s %%0.0s %%0.0s %%0.0s</A>", ex, extra, source, key, buf);
    }
    if (!ReadOnly) 
      sprintf(command+strlen(command), "<TD><A HREF=dgate%s?%s&mode=studydeleter&study=%%s:%%s>delete%%0.0s %%0.0s %%0.0s %%0.0s</A>", ex, extra);
    strcpy(command+strlen(command), "</TR>");
    SendServerCommand("", command, console);

    HTML("</table>");

    MyGetPrivateProfileString ( "shoppingcart", "source", "", buf, 1000, ConfigFile);
    MyGetPrivateProfileString ( "shoppingcart", "line0", buf, buf, 1000, ConfigFile);
    if (buf[0] && key[0])
    { MyGetPrivateProfileString ( "shoppingcart", "caption", "Shopping cart", buf, 1000, ConfigFile);
      sprintf(command, "<A HREF=dgate%s?%s&mode=shoppingcart&key=%s>%s</A>",  ex, extra, key, buf);
      HTML("%s", command);
    }

    HTML("</BODY>");
    exit(0);
  };

  if (strcmp(mode, "queryseries")==0)
  { HTML("Content-type: text/html\nCache-Control: no-cache\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    HTML("<table RULES=ALL BORDER=1>");
    HTML("<Caption>List of series on local server</caption>");
    HTML("<TR><TD>Patient ID<TD>Name<TD>Series date<TD>Series time<TD>Series description<TD>Modality</TR>");

    strcpy(command,                 "query:DICOMSeries,DICOMStudies|DICOMStudies.patientid,DICOMSeries.seriesinst,DICOMStudies.patientnam,DICOMSeries.seriesdate,DICOMSeries.seriestime,DICOMSeries.seriesdesc,DICOMSeries.modality|");
    strcpy(command+strlen(command),  query);
    strcpy(command+strlen(command), " and DICOMStudies.studyinsta=DICOMSeries.studyinsta");
    sprintf(command+strlen(command), "|<TR><TD><A HREF=dgate%s?%s&mode=queryallimages&key=%s&query=DICOMStudies.patientid+=+'%%s'+and+DICOMSeries.seriesinst+=+'%%s'>%%0.0s %%0.0s %%0.0s %%0.0s %%0.0s %%s %%0.0s</A>", ex, extra, key);
    strcpy(command+strlen(command), "<TD>%s<TD>%s<TD>%s<TD>%s<TD>%s");
    sprintf(command+strlen(command), "<TD><A HREF=dgate%s?%s&mode=queryimages&query=DICOMStudies.patientid+=+'%%s'+and+DICOMSeries.seriesinst+=+'%%s'&size=%s>thumbs%%0.0s %%0.0s %%0.0s %%0.0s %%0.0s</A>", ex, extra, size);
    if (viewer[0])
      sprintf(command+strlen(command), "<TD><A HREF=dgate%s?%s&mode=%s&series=%%s:%%s&size=%s>view%%0.0s %%0.0s %%0.0s %%0.0s %%0.0s</A>", ex, extra, viewer, size);
    if (WebPush)
      sprintf(command+strlen(command), "<TD><A HREF=dgate%s?%s&mode=seriesmover&series=%%s:%%s&source=%s&key=%s>push%%0.0s %%0.0s %%0.0s %%0.0s %%0.0s</A>", ex, extra, source, key);
    MyGetPrivateProfileString ( "markseries", "source", "", buf, 1000, ConfigFile);
    MyGetPrivateProfileString ( "markseries", "line0", buf, buf, 1000, ConfigFile);
    if (buf[0] && key[0])
    { MyGetPrivateProfileString ( "markseries", "caption", "Mark", buf, 1000, ConfigFile);
      sprintf(command+strlen(command), "<TD><A HREF=dgate%s?%s&mode=markseries&series=%%s:%%s&source=%s&key=%s>%s%%0.0s %%0.0s %%0.0s %%0.0s %%0.0s</A>", ex, extra, source, key, buf);
    }
    if (!ReadOnly) 
      sprintf(command+strlen(command), "<TD><A HREF=dgate%s?%s&mode=seriesdeleter&series=%%s:%%s>delete%%0.0s %%0.0s %%0.0s %%0.0s %%0.0s</A>", ex, extra);
    strcpy(command+strlen(command), "</TR>");
    SendServerCommand("", command, console);

    HTML("</table>");

    MyGetPrivateProfileString ( "shoppingcart", "source", "", buf, 1000, ConfigFile);
    MyGetPrivateProfileString ( "shoppingcart", "line0", buf, buf, 1000, ConfigFile);
    if (buf[0] && key[0])
    { MyGetPrivateProfileString ( "shoppingcart", "caption", "Shopping cart", buf, 1000, ConfigFile);
      sprintf(command, "<A HREF=dgate%s?%s&mode=shoppingcart&key=%s>%s</A>",  ex, extra, key, buf);
      HTML("%s", command);
    }

    HTML("</BODY>");
    exit(0);
  };

  if (strcmp(mode, "queryimages")==0)
  { HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    HTML("<table RULES=ALL BORDER=1>");
    HTML("<Caption>List of images with thumbnails on local server (maximal 50)</caption>");
    HTML("<TR><TD>Patient ID<TD>Name<TD>Date<TD>Image number<TD>Slice location<TD>Icon</TR>");

    strcpy(command,                 "query2:DICOMImages,DICOMSeries,DICOMStudies|DICOMStudies.patientid,DICOMImages.sopinstanc,DICOMStudies.patientnam,DICOMSeries.seriesdate,DICOMImages.imagenumbe,DICOMImages.slicelocat|");
    strcpy(command+strlen(command),  query);
    strcpy(command+strlen(command), " and DICOMStudies.studyinsta=DICOMSeries.studyinsta and DICOMSeries.seriesinst=DICOMImages.seriesinst");
    sprintf(command+strlen(command), "|<TR><TD><A HREF=dgate%s?%s&mode=sliceviewer&slice=%%s:%%s&size=%s&graphic=%s>%%0.0s %%0.0s %%0.0s %%0.0s", ex, extra, size, graphic);
    strcpy(command+strlen(command), "%s</A><TD>%0.0s %s<TD>%s<TD>%s<TD>%s");
    sprintf(command+strlen(command), "<TD><A HREF=dgate%s?%s&mode=headerdump&slice=%%s:%%s %%0.0s %%0.0s %%0.0s %%0.0s>", ex, extra);
    sprintf(command+strlen(command), "<IMG SRC=dgate%s?%s&mode=slice&slice=%%s:%%s&size=%s&graphic=%s></A></TR>", ex, extra, iconsize, graphic);
    strcpy(command+strlen(command), "|50");
    SendServerCommand("", command, console);

    HTML("</table>");
    HTML("</BODY>");
    exit(0);
  };

  if (strcmp(mode, "queryallimages")==0)
  { HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    HTML("<table RULES=ALL BORDER=1>");
    HTML("<Caption>List of images on local server (all)</caption>");
    HTML("<TR><TD>Patient ID<TD>Name<TD>Date<TD>Image number<TD>Slice location<TD>Header</TR>");

    strcpy(command,                 "query:DICOMImages,DICOMSeries,DICOMStudies|DICOMStudies.patientid,DICOMImages.sopinstanc,DICOMStudies.patientnam,DICOMSeries.seriesdate,DICOMImages.imagenumbe,DICOMImages.slicelocat|");
    strcpy(command+strlen(command),  query);
    strcpy(command+strlen(command), " and DICOMStudies.studyinsta=DICOMSeries.studyinsta and DICOMSeries.seriesinst=DICOMImages.seriesinst");
    sprintf(command+strlen(command), "|<TR><TD><A HREF=dgate%s?%s&mode=sliceviewer&slice=%%s:%%s&size=%s&graphic=%s>%%0.0s %%0.0s %%0.0s %%0.0s", ex, extra, size, graphic);
    strcpy(command+strlen(command), "%s</A><TD>%0.0s %s<TD>%s<TD>%s<TD>%s");
    sprintf(command+strlen(command), "<TD><A HREF=dgate%s?%s&mode=headerdump&slice=%%s:%%s %%0.0s %%0.0s %%0.0s %%0.0s>", ex, extra);
    strcpy(command+strlen(command), "*</A></TR>");
    SendServerCommand("", command, console);

    HTML("</table>");
    HTML("</BODY>");
    exit(0);
  };

  /************************** remote query **************************/

  if (strcmp(mode, "patientfinder")==0)
  { HTML("Content-type: text/html\nCache-Control: no-cache\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    HTML("<table RULES=ALL BORDER=1>");
    if (query[0]) HTML("<Caption>List of selected patients on %s</caption>", dest);
    else          HTML("<Caption>List of all patients on %s</caption>", dest);
    HTML("<TR><TD>ID<TD>Name</TR>");

    sprintf(command,                 "patientfinder:%s|%s", dest, query);
    sprintf(command+strlen(command), "|<TR><TD><A HREF=dgate%s?%s&mode=studyfinder&dest=%s&key=%s&query==%%0.0s%%s>", ex, extra, dest, key);
    strcpy(command+strlen(command),  "%0.0s%s</A><TD>%s");
    SendServerCommand("", command, console);

    HTML("</table>");
    HTML("</BODY>");
    exit(0);
  };

  if (strcmp(mode, "studyfinder")==0)
  { HTML("Content-type: text/html\nCache-Control: no-cache\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    HTML("<table RULES=ALL BORDER=1>");
    if (query[0]) HTML("<Caption>List of selected studies on %s</caption>", dest);
    else          HTML("<Caption>List of all studies on %s</caption>", dest);
    HTML("<TR><TD>Date<TD>Modality<TD>Name<TD>Id<TD>UID</TR>");

    sprintf(command,                 "studyfinder:%s|%s", dest, query);
    sprintf(command+strlen(command), "|<TR><TD><A HREF=dgate%s?%s&mode=seriesfinder&dest=%s&key=%s&query==%%0.0s%%0.0s%%0.0s%%0.0s%%s>", ex, extra, dest, key);
    strcpy(command+strlen(command),  "%s</A><TD>%s<TD>%s<TD>%s<TD>%s");
    if (WebPush)
      sprintf(command+strlen(command), "<TD><A HREF=dgate%s?%s&mode=studymover&study=%%0.0s%%0.0s%%0.0s%%s:%%s&source=%s&key=%s>push</A>", ex, extra, dest, key);
    MyGetPrivateProfileString ( "markstudy", "source", "", buf, 1000, ConfigFile);
    MyGetPrivateProfileString ( "markstudy", "line0", buf, buf, 1000, ConfigFile);
    if (buf[0] && key[0])
    { MyGetPrivateProfileString ( "markstudy", "caption", "Mark", buf, 1000, ConfigFile);
      sprintf(command+strlen(command), "<TD><A HREF=dgate%s?%s&mode=markstudy&study=%%0.0s%%0.0s%%0.0s%%s:%%s&source=%s&key=%s>%s</A>", ex, extra, dest, key, buf);
    }
    strcpy(command+strlen(command), "</TR>");
    SendServerCommand("", command, console);

    HTML("</table>");

    MyGetPrivateProfileString ( "shoppingcart", "source", "", buf, 1000, ConfigFile);
    MyGetPrivateProfileString ( "shoppingcart", "line0", buf, buf, 1000, ConfigFile);
    if (buf[0] && key[0])
    { MyGetPrivateProfileString ( "shoppingcart", "caption", "Shopping cart", buf, 1000, ConfigFile);
      sprintf(command, "<A HREF=dgate%s?%s&mode=shoppingcart&key=%s>%s</A>",  ex, extra, key, buf);
      HTML("%s", command);
    }

    HTML("</BODY>");
    exit(0);
  };

  if (strcmp(mode, "seriesfinder")==0)
  { HTML("Content-type: text/html\nCache-Control: no-cache\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    HTML("<table RULES=ALL BORDER=1>");
    if (query[0]) HTML("<Caption>List of selected series on %s</caption>", dest);
    else          HTML("<Caption>List of all series on %s</caption>", dest);
    HTML("<TR><TD>Date<TD>Time<TD>Modality<TD>Name<TD>ID<TD>UID</TR>");

    sprintf(command,                 "seriesfinder:%s|%s", dest, query);
    sprintf(command+strlen(command),  "|<TR><TD><A HREF=dgate%s?%s&mode=imagefinder&dest=%s&key=%s&query==%%0.0s%%0.0s%%0.0s%%0.0s%%0.0s%%0.0s%%s>%%s</A><TD>%%s<TD>%%s<TD>%%s<TD>%%s%%0.0s<TD>%%s</TR>", ex, extra, dest, key);
    MyGetPrivateProfileString ( "markseries", "source", "", buf, 1000, ConfigFile);
    MyGetPrivateProfileString ( "markseries", "line0", buf, buf, 1000, ConfigFile);
    if (WebPush)
      sprintf(command+strlen(command), "<TD><A HREF=dgate%s?%s&mode=seriesmover&series=%%0.0s%%0.0s%%0.0s%%0.0s%%s:%%0.0s%%s&study=%%0.0s%%0.0s%%0.0s%%0.0s%%s:%%s%%0.0s&source=%s&key=%s>push</A>", ex, extra, dest, key);
    if (buf[0] && key[0])
    { MyGetPrivateProfileString ( "markseries", "caption", "Mark", buf, 1000, ConfigFile);
      sprintf(command+strlen(command), "<TD><A HREF=dgate%s?%s&mode=markseries&series=%%0.0s%%0.0s%%0.0s%%0.0s%%s:%%0.0s%%s&study=%%0.0s%%0.0s%%0.0s%%0.0s%%s:%%s%%0.0s&source=%s&key=%s>%s</A>", ex, extra, dest, key, buf);
    }
    SendServerCommand("", command, console);

    HTML("</table>");

    MyGetPrivateProfileString ( "shoppingcart", "source", "", buf, 1000, ConfigFile);
    MyGetPrivateProfileString ( "shoppingcart", "line0", buf, buf, 1000, ConfigFile);
    if (buf[0] && key[0])
    { MyGetPrivateProfileString ( "shoppingcart", "caption", "Shopping cart", buf, 1000, ConfigFile);
      sprintf(command, "<A HREF=dgate%s?%s&mode=shoppingcart&key=%s>%s</A>",  ex, extra, key, buf);
      HTML("%s", command);
    }

    HTML("</BODY>");
    exit(0);
  };

  if (strcmp(mode, "imagefinder")==0)
  { char WADOserver[256];
    sprintf(WADOserver, "dgate%s?%s", ex, extra);
    MyGetPrivateProfileString ( "wadoservers", dest, WADOserver, WADOserver, 256, ConfigFile);

    HTML("Content-type: text/html\nCache-Control: no-cache\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    HTML("<table RULES=ALL BORDER=1>");
    if (query[0]) HTML("<Caption>List of selected images on %s</caption>", dest);
    else          HTML("<Caption>List of all images on %s</caption>", dest);
    HTML("<TR><TD>Sop<TD>ID<TD>Slice<TD>Location</TR>");

    sprintf(command,                 "imagefinder:%s|%s", dest, query);
    strcpy(command+strlen(command),  "|<TR><TD>%s%0.0s<TD>%s%0.0s%0.0s<TD>%s<TD>%s</TR>");
    sprintf(command+strlen(command), "<TD><A HREF=%s&requestType=WADO&objectUID=%%s%%0.0s%%0.0s&studyUID=%%s&seriesUID=%%s&contentType=image/gif>view (WADO)</A>", WADOserver);
    SendServerCommand("", command, console);

    HTML("</table>");
    HTML("</BODY>");
    exit(0);
  };

  /* unused */
/*
  if (strcmp(mode, "dicomfind")==0)
  { HTML("Content-type: text/html\nCache-Control: no-cache\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    HTML("<FORM ACTION=\"dgate%s\">", ex);

    HTML("Find ");
    HTML("<select name=mode>");
    HTML("  <option value='patientfinder'>Patient</option>");
    HTML("  <option value='studyfinder'>Study</option>");
    HTML("  <option value='seriesfinder'>Series</option>");
    HTML("</select>");
    HTML("on server");

    HTML("<select name=dest>");
    SendServerCommand("", "get_amaps:<option value='%s'>%0.0s%0.0s%0.0s%s</option>", console);
    HTML("</select>");

    HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", (char *)Port);
    HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", ServerCommandAddress);
    HTML("<INPUT NAME=source  TYPE=HIDDEN VALUE=%s>", source);
    HTML("<INPUT NAME=dest    TYPE=HIDDEN VALUE=%s>", dest);
    HTML("Search: <INPUT NAME=query TYPE=Text VALUE=>");
    HTML("<INPUT TYPE=SUBMIT VALUE=Go>");
    HTML("</FORM>");
    HTML("</BODY>");
    exit(0);
  };
  */

  /************************** movers **************************/

  if (strcmp(mode, "studymover")==0)
  { HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    HTML("<table RULES=ALL BORDER=1>");
    HTML("<Caption>Send study: %s</caption>", study2);
    HTML("<TR><TD>Source<TD>Destination</TR>");

    for (i=0; i<1000; i++)
    { char com[80], buf1[80];
      sprintf(com, "get_amap:%d,%%s", i);
      SendServerCommand("", com, 0, buf1);
      if (buf1[0]==0) break;
      if (strchr(buf1, '*')==NULL && strcmp(source, buf1)!=0)
      { sprintf(command, "<TR><TD>%s<TD><A HREF=dgate%s?%s&mode=movestudy&study=%s&source=%s&dest=%s>%s</A></TR>", source, ex, extra, study, source, buf1, buf1);
        HTML("%s", command);
      }
    }

    if (strcmp(source, "(local)")==0)
    { sprintf(command, "<TR><TD>(local)<TD><A HREF=dgate%s?%s&mode=zipstudy&study=%s&dum=.zip>Zip file</A></TR>", ex, extra, study2);
      HTML("%s", command);
    }

    MyGetPrivateProfileString ( "markstudy", "source", "", buf, 1000, ConfigFile);
    MyGetPrivateProfileString ( "markstudy", "line0", buf, buf, 1000, ConfigFile);
    if (buf[0] && key[0])
    { MyGetPrivateProfileString ( "markstudy", "caption", "mark", buf, 1000, ConfigFile);
      sprintf(command, "<TR><TD>%s<TD><A HREF=dgate%s?%s&mode=markstudy&study=%s&source=%s&key=%s>%s</A></TR>", source, ex, extra, study2, source, key, buf);
      HTML("%s", command);
    }

    HTML("</table>");

    MyGetPrivateProfileString ( "shoppingcart", "source", "", buf, 1000, ConfigFile);
    MyGetPrivateProfileString ( "shoppingcart", "line0", buf, buf, 1000, ConfigFile);
    if (buf[0] && key[0])
    { MyGetPrivateProfileString ( "shoppingcart", "caption", "Shopping cart", buf, 1000, ConfigFile);
      sprintf(command, "<A HREF=dgate%s?%s&mode=shoppingcart&key=%s>%s</A>",  ex, extra, key, buf);
      HTML("%s", command);
    }

    HTML("</BODY>");
    exit(0);
  };

  if (strcmp(mode, "seriesmover")==0)
  { HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    HTML("<table RULES=ALL BORDER=1>");
    HTML("<Caption>Send series: %s</caption>", series2);
    HTML("<TR><TD>Source<TD>Destination</TR>");

    for (i=0; i<1000; i++)
    { char com[80], buf1[80];
      sprintf(com, "get_amap:%d,%%s", i);
      SendServerCommand("", com, 0, buf1);
      if (buf1[0]==0) break;
      if (strchr(buf1, '*')==NULL && strcmp(source, buf1)!=0)
      { sprintf(command, "<TR><TD>%s<TD><A HREF=dgate%s?%s&mode=moveseries&series=%s&study=%s&source=%s&dest=%s>%s</A></TR>", source, ex, extra, series, study, source, buf1, buf1);
        HTML("%s", command);
      }
    }

    if (strcmp(source, "(local)")==0)
    { sprintf(command, "<TR><TD>(local)<TD><A HREF=dgate%s?%s&mode=zipseries&series=%s&dum=.zip>Zip file</A></TR>", ex, extra, series);
      HTML("%s", command);
    }

    MyGetPrivateProfileString ( "markseries", "source", "", buf, 1000, ConfigFile);
    MyGetPrivateProfileString ( "markseries", "line0", buf, buf, 1000, ConfigFile);
    if (buf[0] && key[0])
    { MyGetPrivateProfileString ( "markseries", "caption", "mark", buf, 1000, ConfigFile);
      sprintf(command, "<TR><TD>%s<TD><A HREF=dgate%s?%s&mode=markseries&series=%s&source=%s&key=%s>%s</A></TR>", source, ex, extra, series, source, key, buf);
      HTML("%s", command);
    }

    HTML("</table>");

    MyGetPrivateProfileString ( "shoppingcart", "source", "", buf, 1000, ConfigFile);
    MyGetPrivateProfileString ( "shoppingcart", "line0", buf, buf, 1000, ConfigFile);
    if (buf[0] && key[0])
    { MyGetPrivateProfileString ( "shoppingcart", "caption", "Shopping cart", buf, 1000, ConfigFile);
      sprintf(command, "<A HREF=dgate%s?%s&mode=shoppingcart&key=%s>%s</A>",  ex, extra, key, buf);
      HTML("%s", command);
    }

    HTML("</BODY>");
    exit(0);
  };

  if (strcmp(mode, "movestudy")==0)
  { HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);
    sprintf(command, "movestudy:%s,%s,%s", source, dest, study2);
    SendServerCommand("", command, 0, buf);
    HTML("%s", command);
    HTML("<BR>");
    HTML("Done %s", study2);
    HTML("</BODY>");
    exit(0);
  }

  if (strcmp(mode, "moveseries")==0)
  { HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);
    sprintf(command, "moveseries:%s,%s,%s", source, dest, series2);
    if (study2[0])
    { p1 = strrchr(study2, ':'); if(!p1) p1 = study2; else p1++;
      sprintf(command+strlen(command), ",%s", p1);
    }
    SendServerCommand("", command, 0, buf);
    HTML("%s", command);
    HTML("<BR>");
    HTML("Done %s", series2);
    HTML("</BODY>");
    exit(0);
  }

  if (strcmp(mode, "addlocalfile")==0)
  { HTML("Content-type: text/html\nCache-Control: no-cache\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);
    HTML("</BODY>");
    HTML("Uploaded and processed file:");
    HTML(uploadedfile);
    char com[512], file[512];
    sprintf(com, "addimagefile:%s", uploadedfile);
    strcpy(file, uploadedfile);
    SendServerCommand("", com, 0, uploadedfile, FALSE, TRUE);
    unlink(file);
    exit(0);
  };

  if (strcmp(mode, "attachfile")==0)
  { HTML("Content-type: text/html\nCache-Control: no-cache\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);
    HTML("</BODY>");
    HTML("Uploaded and scripted processed file:");
    HTML(uploadedfile);
    char com[1512], file[512];
    sprintf(com, "attachfile:%s,%s", uploadedfile, script);
    strcpy(file, uploadedfile);
    SendServerCommand("", com, 0, uploadedfile, FALSE, TRUE);
    unlink(file);
    exit(0);
  };

  /************************** delete **************************/

  if (!ReadOnly) 
  { if (strcmp(mode, "studydeleter")==0)
    { HTML("Content-type: text/html\n");
      HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
      HTML("<BODY BGCOLOR='CFDFCF'>");
      HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);
      HTML("<Caption>Delete study: %s</caption>", study);
  
      HTML("<FORM ACTION=\"dgate%s\">", ex);
      HTML("<INPUT NAME=mode    TYPE=HIDDEN VALUE=deletestudy>");
      HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", (char *)Port);
      HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", ServerCommandAddress);
      HTML("<INPUT NAME=study   TYPE=HIDDEN VALUE='%s'>", study);
      HTML("<INPUT TYPE=SUBMIT VALUE='Delete the study'>");
      HTML("</FORM>");
      HTML("</BODY>");
      exit(0);
    };
  
    if (strcmp(mode, "seriesdeleter")==0)
    { HTML("Content-type: text/html\n");
      HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
      HTML("<BODY BGCOLOR='CFDFCF'>");
      HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);
      HTML("<Caption>Delete series: %s</caption>", series);
  
      HTML("<FORM ACTION=\"dgate%s\">", ex);
      HTML("<INPUT NAME=mode    TYPE=HIDDEN VALUE=deleteseries>");
      HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", (char *)Port);
      HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", ServerCommandAddress);
      HTML("<INPUT NAME=series  TYPE=HIDDEN VALUE='%s'>", series);
      HTML("<INPUT TYPE=SUBMIT VALUE='Delete the series'>");
      HTML("</FORM>");
      HTML("</BODY>");
      exit(0);
    };
  
    if (strcmp(mode, "deletestudy")==0)
    { HTML("Content-type: text/html\n");
      HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
      HTML("<BODY BGCOLOR='CFDFCF'>");
      HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);
      sprintf(command, "deletestudy:%s", study2);
      SendServerCommand("", command, 0, buf);
      HTML(command);
      HTML("<BR>");
      HTML("Done");
      HTML("</BODY>");
      exit(0);
    }
  
    if (strcmp(mode, "deleteseries")==0)
    { HTML("Content-type: text/html\n");
      HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
      HTML("<BODY BGCOLOR='CFDFCF'>");
      HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);
      sprintf(command, "deleteseries:%s", series2);
      SendServerCommand("", command, 0, buf);
      HTML(command);
      HTML("<BR>");
      HTML("Done");
      HTML("</BODY>");
      exit(0);
    }
  }

  /************************** worklist browser and editor **************************/

  if (strcmp(mode, "queryworklist")==0)
  { HTML("Content-type: text/html\nCache-Control: no-cache\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    HTML("<table RULES=ALL BORDER=1>");
    if (query[0]) HTML("<Caption>List of selected worklist entries</caption>");
    else          HTML("<Caption>List of all worklist entries (not all fields displayed)</caption>");
    HTML("<TR><TD>AccessionN<TD>Patient ID<TD>Name<TD>Birth date<TD>Sex<TD>Physician<TD>Description<TD>Modality<TD>Date<TD>Time</TR>");

    strcpy(command,                  "query:DICOMWorkList|AccessionN,PatientID,PatientNam,PatientBir,PatientSex,ReqPhysici,ReqProcDes,Modality,StartDate,StartTime|");
    strcpy(command+strlen(command),  query);
    sprintf(command+strlen(command), "|<TR><TD><A HREF=dgate%s?%s&mode=editrecord&db=DICOMWorkList&query=AccessionN+=+'%%s'>%%0.0s %%0.0s %%0.0s %%0.0s %%0.0s %%0.0s %%0.0s %%0.0s %%0.0s", ex, extra);
    sprintf(command+strlen(command), "%%s</A><TD>%%s<TD>%%s<TD>%%s<TD>%%s<TD>%%s<TD>%%s<TD>%%s<TD>%%s<TD>%%s<TD><A HREF=dgate%s?%s&mode=deleterecord&db=DICOMWorkList&query=AccessionN+=+'%%s'>Delete</A></TR>", ex, extra);
    SendServerCommand("", command, console);

    HTML("</table>");

    sprintf(command, "<A HREF=dgate%s?%s&mode=addrecord&db=DICOMWorkList>Add worklist entry</A>", ex, extra);
    HTML(command);
    HTML("</BODY>");
    exit(0);
  };

  /************************** general purpose database editing **************************/

  if (strcmp(mode, "editrecord")==0)
  { HTML("Content-type: text/html\nCache-Control: no-cache\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    if (!DBE)
    { HTML("<H2>File DICOM.SQL not found - please check configuration</H2>");
      exit(0);
    }

    HTML("<FORM ACTION=\"dgate%s\">", ex);
    if (!ReadOnly) HTML("<INPUT TYPE=HIDDEN NAME=mode  VALUE=saverecord>");
    HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", (char *)Port);
    HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", ServerCommandAddress);
    HTML("<INPUT NAME=db      TYPE=HIDDEN VALUE=%s>", db);
    HTML("<INPUT NAME=query   TYPE=HIDDEN VALUE=%s>", query);

    HTML("<table RULES=ALL BORDER=2>");
    HTML("<Caption>Edit %s entry</caption>", db);
    HTML("<TR><TD>Field<TD>Value</TR>");

    strcpy(command, "query2:");
    strcat(command, db);
    strcat(command, "|");
    i = 0;
    while ( TRUE )
    { if(!DBE[i].Element) break;			// end of fields
      if (DBE[i].DICOMType!=DT_STARTSEQUENCE && DBE[i].DICOMType!=DT_ENDSEQUENCE)
      {	strcat(command, DBE[i].SQLColumn);
	strcat(command, ",");
      }
      ++i;
    }
    command[strlen(command)-1]=0;			// remove trailing ,

    strcat(command, "|");
    strcat(command, query);
    strcat(command, "|");
    
    i = 0;
    while ( TRUE )
    { if(!DBE[i].Element) break;			// end of fields
      if (DBE[i].DICOMType!=DT_STARTSEQUENCE && DBE[i].DICOMType!=DT_ENDSEQUENCE)
      {	strcat(command, "<TR><TD>");
      	strcat(command, DBE[i].SQLColumn);
	strcat(command, "<TD><INPUT NAME=");
      	strcat(command, DBE[i].SQLColumn);
      	strcat(command, " TYPE=Text VALUE='%s'></TR>");
      }
      ++i;
    };
    strcat(command, "|1");	// max 1 record !!!!!
    
    SendServerCommand("", command, console);

    HTML("</table>");
    if (!ReadOnly) HTML("<INPUT TYPE=SUBMIT VALUE=Save>");
    HTML("</FORM>");
    HTML("</BODY>");
    exit(0);
  };

  if (strcmp(mode, "addrecord")==0)
  { HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    if (!DBE)
    { HTML("<H2>File DICOM.SQL not found - please check configuration</H2>");
      exit(0);
    }

    HTML("<FORM ACTION=\"dgate%s\">", ex);
    if (!ReadOnly) HTML("<INPUT TYPE=HIDDEN NAME=mode VALUE=saverecord>");
    HTML("<INPUT NAME=db      TYPE=HIDDEN VALUE=%s>", db);
    HTML("<INPUT NAME=port    TYPE=HIDDEN VALUE=%s>", (char *)Port);
    HTML("<INPUT NAME=address TYPE=HIDDEN VALUE=%s>", ServerCommandAddress);

    HTML("<table RULES=ALL BORDER=2>");
    sprintf(command, "<Caption>Add %s entry</caption>", db);
    HTML(command);

    HTML("<TR><TD>Field<TD>Value</TR>");

    i = 0;
    while ( TRUE )
    { if(!DBE[i].Element) break;			// end of fields
      if (DBE[i].DICOMType!=DT_STARTSEQUENCE && DBE[i].DICOMType!=DT_ENDSEQUENCE)
      {	strcpy(command, "<TR><TD>");
      	strcat(command, DBE[i].SQLColumn);
	strcat(command, "<TD><INPUT NAME=");
      	strcat(command, DBE[i].SQLColumn);
      	strcat(command, " TYPE=Text VALUE=''></TR>");
      	HTML(command);
      }
      ++i;
    }

    HTML("</table>");
    if (!ReadOnly) HTML("<INPUT TYPE=SUBMIT VALUE=Save>");
    HTML("</FORM>");
    HTML("</BODY>");
    exit(0);
  };

  if (strcmp(mode, "saverecord")==0)
  { HTML("Content-type: text/html\nCache-Control: no-cache\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    if (!DBE)
    { HTML("<H2>File DICOM.SQL not found - please check configuration</H2>");
      exit(0);
    }

    if (ReadOnly || DBE!=WorkListDB)
    { HTML("<H2>This table is readonly</H2>");
      exit(0);
    }
    
    if (strlen(query))
    { sprintf(command, "deleterecord:%s,%s", db, query); 
      SendServerCommand("", command, console);
      sprintf(command, "Updated/added %s entry for %s", db, query);
      HTML(command);
    }
    else
    { sprintf(command, "Added new record in %s", db);
      HTML(command);
    };
     
    sprintf(command, "addrecord:%s|", db);
    i = 0;
    while ( TRUE )
    { if(!DBE[i].Element) break;			// end of fields
      if (DBE[i].DICOMType!=DT_STARTSEQUENCE && DBE[i].DICOMType!=DT_ENDSEQUENCE)
      {	strcat(command, DBE[i].SQLColumn);
	strcat(command, ",");
      }
      ++i;
    }
    command[strlen(command)-1]=0;			// remove trailing ,
    strcat(command, "|");

    i = 0;
    while ( TRUE )
    { if(!DBE[i].Element) break;			// end of fields
      if (DBE[i].DICOMType!=DT_STARTSEQUENCE && DBE[i].DICOMType!=DT_ENDSEQUENCE)
      {	strcat(command, "'"); 
        CGI(buf, DBE[i].SQLColumn, ""); 
        strcat(command, buf); 
        strcat(command, "', "); 
      }
      ++i;
    }
    command[strlen(command)-2]=0;			// remove trailing , and space

    SendServerCommand("", command, console);
    HTML("</BODY>");
    exit(0);
  };

  if (strcmp(mode, "deleterecord")==0)
  { HTML("Content-type: text/html\nCache-Control: no-cache\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    if (ReadOnly || DBE!=WorkListDB)
    { HTML("<H2>This table is readonly</H2>");
      exit(0);
    }

    sprintf(command, "deleterecord:%s,%s", db, query); 
    SendServerCommand("", command, console);
    HTML("Deleted record entry for ");
    HTML(db);
    HTML("Where ");
    HTML(query);
    HTML("</BODY>");
    exit(0);
  };

  /************************** viewers **************************/

  /* page with one slice */
  if (strcmp(mode, "sliceviewer")==0)
  { HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    HTML("<A HREF=dgate%s?%s&mode=headerdump&slice=%s>", ex, extra, slice);
    if (size[0])
      HTML("<IMG SRC=dgate%s?%s&mode=slice&slice=%s&size=%s&graphic=%s&lw=%s HEIGHT=%s>", ex, extra, slice, dsize, graphic, lw, size);
    else
      HTML("<IMG SRC=dgate%s?%s&mode=slice&slice=%s&size=%s&graphic=%s&lw=%s>", ex, extra, slice, dsize, graphic, lw);
    HTML("</A>");
    HTML("</BODY>");
    exit(0);
  };

  if (strcmp(mode, "headerdump")==0)
  { HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);
    HTML("<BR>Header dump of DICOM object:<BR>");
    HTML("<PRE>");

    sprintf(command, "dump_header:%s", slice2);
    SendServerCommand("", command, console, NULL, FALSE);

    HTML("</PRE>");
    HTML("</BODY>");
    exit(0);
  };

  /* just generates the bitmap */
  if (strcmp(mode, "slice")==0)
  { sprintf(command, "convert_to_%s:%s,%s,cgi,%s", graphic, slice2, size, lw); // 1.4.16i: use size instead of dsize
    SendServerCommand("", command, console, NULL, FALSE);
    exit(0);
  };

  /* transmits the image contents in dicom format */
  if (strcmp(mode, "dicom")==0)
  { sprintf(command, "convert_to_dicom:%s,%s,%s", slice2, dsize, compress);
    SendServerCommand("", command, console, NULL, FALSE);
    exit(0);
  };

  /* transmits the series in zipped dicom format */
  if (strcmp(mode, "zipseries")==0)
  { char *p = strrchr(series2, ':');
    if (p) 
    { *p++ = ',';
      memmove(p, p-1, strlen(p-1)+1);
      *p++ = ',';
    }

    sprintf(command, "export:%s,,cgi,call zip.cq", series2);
    SendServerCommand("", command, console, NULL, FALSE);
    exit(0);
  };

  /* transmits the study in zipped dicom format */
  if (strcmp(mode, "zipstudy")==0)
  { char *p = strrchr(study2, ':');
    if (p) *p = ',';

    sprintf(command, "export:%s,,,cgi,call zip.cq", study2);
    SendServerCommand("", command, console, NULL, FALSE);
    exit(0);
  };

  /* transmits the image list with urls in text format */
  if (strcmp(mode, "imagelisturls")==0)
  { char *p;

    p = strrchr(series2, ':');
    if (p) *p = '|';

    sprintf(command, "imagelister:local|%s|:%s?%s&mode=dicom&slice=%%s:%%s&dsize=%s&compress=%s&dum=.dcm|cgi", series2, WebScriptAddress, extra, dsize, compress);
    SendServerCommand("", command, console, NULL, FALSE);
    exit(0);
  };

  /* transmits the image list with http references in text format */
  if (strcmp(mode, "imagelisthttp")==0)
  { char *p;

    p = strrchr(series2, ':');
    if (p) *p = '|';

    sprintf(command, "imagelister:local|%s|@%s/%%0.0s%%s*|cgi", series2, WebMAG0Address);
    SendServerCommand("", command, console, NULL, FALSE);
    exit(0);
  };

  /* transmits the image list with filenames in text format */
  if (strcmp(mode, "imagelistfiles")==0)
  { char *p;

    p = strrchr(series2, ':');
    if (p) *p = '|';

    sprintf(command, "imagelister:local|%s|%%s|cgi", series2);// %% warning, bcb -> ok but wrong change ;->>> mvh
//	sprintf(command, "imagelister:local|%s|%%s|cgi", series2, WebMAG0Address);
    SendServerCommand("", command, console, NULL, FALSE);
    exit(0);
  };

  /* k-pacs viewer in an OCX; internet explorer only */
/*  if (strcmp(mode, "seriesviewer")==0)
  { char *p;
  
    if (size[0]==0) strcpy(size, "80%25%25");

    HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);

    HTML("<OBJECT");
    HTML("          classid=\"clsid:0AA480F2-78EB-4A31-B4C8-F981C5004BBB\"");
    HTML("          codebase=\"%sActiveFormProj1.ocx\"", WebCodeBase);
    HTML("          width=%s", size);
    HTML("          height=%s", size);
    HTML("          align=center");
    HTML("          hspace=0");
    HTML("          vspace=0");
    HTML(">");
    HTML("<PARAM name=DCMFilelist value=");

    p = strrchr(series2, ':');
    if (p) *p = '|';
    sprintf(command, "imagelister:local|%s|:%s?%s&mode=dicom&slice=%%s:%%s&dsize=%s&compress=%s*", series2, WebScriptAddress, extra, dsize, compress);
    SendServerCommand("", command, console);

    HTML(">");
    HTML("</OBJECT>");
    HTML("</BODY>");
    exit(0);
  };
  */

  /* k-pacs viewer in an OCX; internet explorer only; data access through http served image files */
  /*if (strcmp(mode, "seriesviewer2")==0)
  { char wwwserveraddress[512] = "http://";
    strcat(wwwserveraddress, getenv("SERVER_NAME"));

    if (size[0]==0) strcpy(size, "80%25%25");

    HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);
    HTML("<OBJECT");
    HTML("          classid=\"clsid:0AA480F2-78EB-4A31-B4C8-F981C5004BBB\"");
    HTML("          codebase=\"%sActiveFormProj1.ocx\"", WebCodeBase);
    HTML("          width=%s", size);
    HTML("          height=%s", size);
    HTML("          align=center");
    HTML("          hspace=0");
    HTML("          vspace=0");
    HTML(">");
    HTML("<PARAM name=DCMFilelist value=");

    char *p = strrchr(series2, ':');
    if (p) *p = '|';

    if (memcmp(wwwserveraddress, "file", 4)==0)
      sprintf(command, "imagelister:local|%s", series2);
    else
      sprintf(command, "imagelister:local|%s|@%s/%%0.0s%%s*", series2, WebMAG0Address);

    SendServerCommand("", command, console);

    HTML(">");
    HTML("</OBJECT>");
    HTML("</BODY>");
    exit(0);
  };
  */

  /* no viewer */
  if (strcmp(mode, "noviewer")==0)
  { //char *p;

    HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s</H2>", DGATE_VERSION);
    HTML("<HR>");
    HTML("<H2>No viewer is installed</H2>");
    HTML("</OBJECT>");
    HTML("</BODY>");
    exit(0);
  };

  /* The Japanese java-based viewer; requires some modifications to work */
  /*if (strcmp(mode, "aiviewer")==0)
  { char *p;

    HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s with AiViewer</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s with AiViewer</H2>", DGATE_VERSION);

    HTML("<APPLET");
    HTML("  CODEBASE = '%sdicomviewer100'", WebCodeBase);
    HTML("  CODE = 'dicomviewer.Viewer.class'");
    HTML("  NAME = 'Viewer.java'");
    HTML("  WIDTH = '100%%'");
    HTML("  HEIGHT = '90%%'");
    HTML("  HSPACE = 0");
    HTML("  VSPACE = 0");
    HTML("  ALIGN = middle >");
    HTML("<PARAM NAME = tmpSize VALUE = 1>");
    HTML("<PARAM NAME = NUM VALUE = 0>");
    HTML("<PARAM NAME = currentNo VALUE = 0>");
    HTML("<PARAM NAME = dicURL VALUE = '%sdicomviewer100/Dicom.dic'>", WebCodeBase);
    HTML("<PARAM NAME = imgURL0 VALUE = ");

    p = strrchr(series2, ':');
    if (p) *p = '|';
    sprintf(command, "imagelister:local|%s|:%s?%s&mode=dicom&slice=%%s:%%s&dsize=%s&compress=%s*", series2, WebScriptAddress, extra, dsize, "un");
    SendServerCommand("", command, console);

    HTML(">");

    HTML("</APPLET>");
    HTML("</BODY>");
    exit(0);
  };
  */

  /* very simple jave-script based viewer with server side processing */
  if (strcmp(mode, "serversideviewer")==0)
  { char *p;

    p = strrchr(series2, ':');
    if (p) *p = '|';

    if (size[0]==0) strcpy(size, "80%25%25");

    HTML("Content-type: text/html\n");
    HTML("<HEAD><TITLE>Conquest DICOM server - version %s server side viewer</TITLE></HEAD>", DGATE_VERSION);
    HTML("<BODY BGCOLOR='CFDFCF'>");
    HTML("<H2>Conquest DICOM server - version %s server side viewer</H2>", DGATE_VERSION);

    HTML("<SCRIPT language=JavaScript>");
    char s[] = "var frames, nframes ='";
    write(console, s, strlen(s));
    sprintf(command, "imagelister:local|%s|$%%04d", series2);
    SendServerCommand("", command, console);
    HTML("';");
    HTML("function load()");
    HTML("{ document.images[0].src = 'dgate%s?%s&mode=slice'+", ex, extra);
    HTML("                           '&graphic=' + document.forms[0].graphic.value +");
    HTML("                           '&slice='   + document.forms[0].slice.value +");
    HTML("                           '&dsize=%s' +", dsize);
    HTML("                           '&lw='      + document.forms[0].level.value + '/' + document.forms[0].window.value + '/' + document.forms[0].frame.value;");
    HTML("  frames = parseInt(nframes.slice(document.forms[0].slice.selectedIndex*4, document.forms[0].slice.selectedIndex*4+4), 10);");
    HTML("  if (frames < 2) document.forms[0].frame.style.visibility = 'hidden'; else document.forms[0].frame.style.visibility = 'visible';");
    HTML("  if (frames < 2) document.forms[0].nframe.style.visibility = 'hidden'; else document.forms[0].nframe.style.visibility = 'visible';");
    HTML("  if (frames < 2) document.forms[0].pframe.style.visibility = 'hidden'; else document.forms[0].pframe.style.visibility = 'visible';");
    HTML("}");
    HTML("function nextslice()");
    HTML("{ if (document.forms[0].slice.selectedIndex == document.forms[0].slice.length-1) document.forms[0].slice.selectedIndex = 0; else document.forms[0].slice.selectedIndex = document.forms[0].slice.selectedIndex + 1; load()");
    HTML("}");
    HTML("function nextframe()");
    HTML("{ if (parseInt(document.forms[0].frame.value, 10) == frames-1) document.forms[0].frame.value = 0; else document.forms[0].frame.value = parseInt(document.forms[0].frame.value, 10)+1; load()");
    HTML("}");
    HTML("function prevframe()");
    HTML("{ document.forms[0].frame.value = parseInt(document.forms[0].frame.value, 10)-1; if (document.forms[0].frame.value<0) document.forms[0].frame.value = 0; load()");
    HTML("}");
    HTML("</SCRIPT>");

    HTML("<IMG SRC=loading.jpg BORDER=0 HEIGHT=%s>", size);

    HTML("<FORM>");
    HTML("Slice: ");
    HTML("  <select name=slice onchange=load()>");

    sprintf(command, "imagelister:local|%s|:<option value=%%s:%%s>%%s</option>", series2);
    SendServerCommand("", command, console);

    HTML("  </select>");
    HTML("  <INPUT TYPE=BUTTON VALUE='>' onclick=nextslice() >");

    HTML("Level:");
    HTML("  <INPUT name=level size=4 onchange=load() >");
    HTML("Window:");
    HTML("  <INPUT name=window size=4 onchange=load() >");
    HTML("Frame:");
    HTML("  <INPUT name=pframe TYPE=BUTTON VALUE='<' onclick=prevframe() >");
    HTML("  <INPUT name=frame size=3 onchange=load() value=0>");
    HTML("  <INPUT name=nframe TYPE=BUTTON VALUE='>' onclick=nextframe() >");
    HTML("Color:");
    HTML("  <select name=graphic onchange=load()>");
    if (strcmp(graphic, "bmp")==0)
    { HTML("    <option value=bmp>yes</option>");
      HTML("    <option value=gif>no</option>");
      HTML("    <option value=jpg>jpg</option>");
    }
    else if (strcmp(graphic, "gif")==0)
    { HTML("    <option value=gif>no</option>");
      HTML("    <option value=bmp>yes</option>");
      HTML("    <option value=jpg>jpg</option>");
    }
    else if (strcmp(graphic, "jpg")==0)
    { HTML("    <option value=jpg>jpg</option>");
      HTML("    <option value=bmp>yes</option>");
      HTML("    <option value=gif>no</option>");
    }
    HTML("  </select>");
    HTML("  <INPUT TYPE=BUTTON VALUE=load onclick=load() >");
    HTML("</FORM>");

    HTML("<SCRIPT language=JavaScript>");
    HTML("  document.onload=load()");
    HTML("</SCRIPT>");

    HTML("</BODY>");
    exit(0);
  };

  /************************** general purpose web scripting **************************/

/*
This is a general purpose web script processor; it can be used to create any web page, not just viewers.

This is a sample from dicom.ini:

[flexviewer]
line0 = <HEAD><TITLE>Conquest DICOM server - version %version% and %windowname%</TITLE></HEAD>
line1 = <BODY BGCOLOR='CFDFCF'>
line2 = <H2>Conquest DICOM server - version %version% and AiViewer v1.00</H2>
line3 = <APPLET CODEBASE=%webcodebase%dicomviewer100 CODE='dicomviewer.Viewer.class'
line4 = NAME='Viewer.java' WIDTH='100%' HEIGHT='90%' HSPACE=0 VSPACE=0 ALIGN=middle >
line5 = <PARAM NAME = tmpSize VALUE = 10>
line6 = <PARAM NAME = NUM VALUE = 0>
line7 = <PARAM NAME = currentNo VALUE = 0>
line8 = <PARAM NAME = dicURL VALUE = '%webcodebase%dicomviewer100/Dicom.dic'>
line9 = <PARAM NAME = imgURL0 VALUE = 
line10 = --imagelister:local|%patid%|%seruid%|:%webscriptadress%?%extra%&mode=dicom&slice=%s:%s&compress=un*
line11 = >
line12 = </APPLET>
line13 = </BODY>
# this is the default; the variable can be passed in the url
windowname = AiViewer V1.00

[flexviewer2]
source = flexviewer2.cq
windowname = AiViewer V1.00

;[AnyPage]
;source = anypage.cq

;[DefaultPage]
;source = ecrf\*.lua
*/

  /* check mode of web page against dicom.ini sections */
  MyGetPrivateProfileString ( mode, "source", "", buf, 1000, ConfigFile);
  MyGetPrivateProfileString ( mode, "line0", buf, buf, 1000, ConfigFile);

  if (buf[0]==0)
  { strcpy(mode, DefaultPage);
    MyGetPrivateProfileString ( mode, "source", "", buf, 1000, ConfigFile);
    MyGetPrivateProfileString ( mode, "line0", buf, buf, 1000, ConfigFile);
  }

  /* if source contains a * substitute the mode */
  char *p = strchr(buf, '*');
  if (p)
  { char tmp[256];
    *p=0;
    CGI(tmp, "mode", "");
    strcat(tmp, p+1);
    strcat(buf, tmp);
  }

  if (buf[0])
  { char string[1000], temp[1000], server[1000], patid[66], patid2[200], seruid[66], studyuid[66], sopuid[66], chunk[8192];
    FILE *f=NULL;

    /* create several variables useful for scripting */

    sprintf(server, "%s?%s", WebScriptAddress, extra);

    patid[0] = seruid[0] = studyuid[0] = sopuid[0] = 0;

    if (study2[0])
    { strcpy(temp, study2);
      p = strrchr(temp, ':');
      if (p) 
      { *p = 0;
        strcpy(patid2, temp);
        *p = '|';
        strcpy(studyuid, p+1);
      }
    
      strcpy(temp, study);
      p = strrchr(temp, ':');
      if (p) 
      { *p = 0;
        strcpy(patid, temp);
      }
    }

    if (series2[0])
    { strcpy(temp, series2);
      p = strrchr(temp, ':');
      if (p) 
      { *p = 0;
        strcpy(patid2, temp);
        *p = '|';
        strcpy(seruid, p+1);
      }
    
      strcpy(temp, series);
      p = strrchr(temp, ':');
      if (p) 
      { *p = 0;
        strcpy(patid, temp);
      }
    }

    if (slice2[0])
    { strcpy(temp, slice2);
      p = strrchr(temp, ':');
      if (p) 
      { *p = 0;
        strcpy(patid2, temp);
        *p = '|';
        strcpy(sopuid, p+1);
      }
    
      strcpy(temp, slice);
      p = strrchr(temp, ':');
      if (p) 
      { *p = 0;
        strcpy(patid, temp);
      }
    }

    /* contents may come from file (lua extension runs lua script) or from dicom.ini */
    MyGetPrivateProfileString ( mode, "source", buf, temp, 1000, ConfigFile);
    strcpy(temp, buf);
    if (strcmp(temp+strlen(temp)-4, ".lua")==0)
    { MyGetPrivateProfileString ( mode, "header", "", buf, 1000, ConfigFile);
      while ((p = strstr(buf, "\\"))) p[0]='\n';
      if (buf[0]) HTML(buf);
    
      struct scriptdata sd1 = {&globalPDU, NULL, NULL, -1, NULL, NULL, NULL, NULL, NULL, 0, 0};
      char script[512]; int i, n;
      strcpy(script, "dofile('");
      for (i=0, n=8; i<strlen(temp); i++)
      { if (temp[i]=='\\')
        { script[n++] = '\\';
          script[n++] = '\\';
	}
	else 
	{ script[n++] = temp[i];
	}
      }
      script[n++] = 0;
      strcat(script, "')");
      OperatorConsole.On();

      lua_setvar(&globalPDU, "query_string",    global_query_string); //getenv( "QUERY_STRING"));      
      lua_setvar(&globalPDU, "server_name",     getenv( "SERVER_NAME" ));
      lua_setvar(&globalPDU, "script_name",     getenv( "SCRIPT_NAME" ));
      lua_setvar(&globalPDU, "path_translated", getenv( "PATH_TRANSLATED" ));
      lua_setvar(&globalPDU, "port",            (char *)Port);
      lua_setvar(&globalPDU, "address",         ServerCommandAddress);
      lua_setvar(&globalPDU, "webcodebase",     WebCodeBase);
      lua_setvar(&globalPDU, "webscriptadress", WebScriptAddress);
      lua_setvar(&globalPDU, "webscriptaddress",WebScriptAddress); // was misspelled
      lua_setvar(&globalPDU, "extra",           extra);
      lua_setvar(&globalPDU, "version",         DGATE_VERSION);
      lua_setvar(&globalPDU, "mode",            mode);
      lua_setvar(&globalPDU, "uploadedfile",    uploadedfile);
          
      lua_setvar(&globalPDU, "series",          series);
      lua_setvar(&globalPDU, "series2",         series2);
      lua_setvar(&globalPDU, "slice",           slice);
      lua_setvar(&globalPDU, "slice2",          slice2);
      lua_setvar(&globalPDU, "study",           study);
      lua_setvar(&globalPDU, "study2",          study2);
      lua_setvar(&globalPDU, "patid",           patid);
      lua_setvar(&globalPDU, "patid2",          patid2);
      lua_setvar(&globalPDU, "seruid",          seruid);
      lua_setvar(&globalPDU, "studyuid",        studyuid);
      lua_setvar(&globalPDU, "sopuid",          sopuid);
	  
      lua_setvar(&globalPDU, "size",            size);
      lua_setvar(&globalPDU, "dsize",           dsize);
      lua_setvar(&globalPDU, "compress",        compress);
      lua_setvar(&globalPDU, "iconsize",        iconsize);
      lua_setvar(&globalPDU, "graphic",         graphic);
      lua_setvar(&globalPDU, "viewer",          viewer);
      lua_setvar(&globalPDU, "lw",              lw);
      lua_setvar(&globalPDU, "query",           query);
      lua_setvar(&globalPDU, "db",              db);
      lua_setvar(&globalPDU, "source",          source);
      lua_setvar(&globalPDU, "dest",            dest);
      lua_setvar(&globalPDU, "patientidmatch",  patientidmatch);
      lua_setvar(&globalPDU, "patientnamematch",patientnamematch);
      lua_setvar(&globalPDU, "studydatematch",  studydatematch);
      lua_setvar(&globalPDU, "startdatematch",  startdatematch);

      do_lua(&(globalPDU.L), script, &sd1);

      if (*uploadedfile) unlink(uploadedfile);
      exit(0);
    }

    if (temp[0]) f = fopen(temp, "rt");

    MyGetPrivateProfileString ( mode, "header", "Content-type: text/html\n", temp, 1000, ConfigFile);
    while ((p = strstr(temp, "\\"))) p[0]='\n';
    HTML(temp);
    
    int inlua=0;

    for (i=0; i<10000; i++)
    { if (f)
      { if (fgets(string, sizeof(string), f) == NULL)
          break;
        if (!inlua && string[strlen(string)-1]=='\n') string[strlen(string)-1]=0;
      }
      else
      { sprintf(temp, "line%d", i);
        MyGetPrivateProfileString ( mode, temp, "", string, 1000, ConfigFile);
        if (string[0]==0) break;
      }
      
      if (inlua)
      { char *p = strstr(string, "?>");
        if (p)
	{ *p=0;
	  strcat(chunk, string);
	  inlua=0;
          struct scriptdata sd1 = {&globalPDU, NULL, NULL, -1, NULL, NULL, NULL, NULL, NULL, 0, 0};

          lua_setvar(&globalPDU, "query_string",    global_query_string); //getenv( "QUERY_STRING"));      
          lua_setvar(&globalPDU, "server_name",     getenv( "SERVER_NAME" ));
          lua_setvar(&globalPDU, "script_name",     getenv( "SCRIPT_NAME" ));
          lua_setvar(&globalPDU, "path_translated", getenv( "PATH_TRANSLATED" ));
          lua_setvar(&globalPDU, "port",            (char *)Port);
          lua_setvar(&globalPDU, "address",         ServerCommandAddress);
          lua_setvar(&globalPDU, "webcodebase",     WebCodeBase);
          lua_setvar(&globalPDU, "webscriptadress", WebScriptAddress);
          lua_setvar(&globalPDU, "extra",           extra);
          lua_setvar(&globalPDU, "version",         DGATE_VERSION);
          lua_setvar(&globalPDU, "mode",            mode);
	  lua_setvar(&globalPDU, "uploadedfile",    uploadedfile);
          
          lua_setvar(&globalPDU, "series",          series);
          lua_setvar(&globalPDU, "series2",         series2);
          lua_setvar(&globalPDU, "slice",           slice);
          lua_setvar(&globalPDU, "slice2",          slice2);
          lua_setvar(&globalPDU, "study",           study);
          lua_setvar(&globalPDU, "study2",          study2);
          lua_setvar(&globalPDU, "patid",           patid);
          lua_setvar(&globalPDU, "patid2",          patid2);
          lua_setvar(&globalPDU, "seruid",          seruid);
          lua_setvar(&globalPDU, "studyuid",        studyuid);
          lua_setvar(&globalPDU, "sopuid",          sopuid);
	  
	  lua_setvar(&globalPDU, "size",            size);
          lua_setvar(&globalPDU, "dsize",           dsize);
          lua_setvar(&globalPDU, "compress",        compress);
          lua_setvar(&globalPDU, "iconsize",        iconsize);
          lua_setvar(&globalPDU, "graphic",         graphic);
          lua_setvar(&globalPDU, "viewer",          viewer);
          lua_setvar(&globalPDU, "lw",              lw);
          lua_setvar(&globalPDU, "query",           query);
          lua_setvar(&globalPDU, "db",              db);
          lua_setvar(&globalPDU, "source",          source);
          lua_setvar(&globalPDU, "dest",            dest);
          lua_setvar(&globalPDU, "patientidmatch",  patientidmatch);
          lua_setvar(&globalPDU, "patientnamematch",patientnamematch);
          lua_setvar(&globalPDU, "studydatematch",  studydatematch);
          lua_setvar(&globalPDU, "startdatematch",  startdatematch);

          do_lua(&(globalPDU.L), chunk, &sd1);
	}
	else
	  strcat(chunk, string);
      }

      /* fill in predefined scripting variables */

      replace(string, "%query_string%",    global_query_string); //getenv( "QUERY_STRING" ));
      replace(string, "%server_name%",     getenv( "SERVER_NAME" ));
      replace(string, "%script_name%",     getenv( "SCRIPT_NAME" ));
      replace(string, "%path_translated%", getenv( "PATH_TRANSLATED" ));
      replace(string, "%uploadedfile%",    uploadedfile);

      replace(string, "%port%",            (char *)Port);
      replace(string, "%address%",         ServerCommandAddress);
      replace(string, "%webcodebase%",     WebCodeBase);
      replace(string, "%webscriptadress%", WebScriptAddress);
      replace(string, "%extra%",           extra);
      replace(string, "%server%",          server);
      replace(string, "%version%",         DGATE_VERSION);
      replace(string, "%mode%",            mode);

      replace(string, "%series%",          series2); // unprocessed
      replace(string, "%series2%",         series);  // replaced spaces by %20
      replace(string, "%slice%",           slice2);  // unprocessed
      replace(string, "%slice2%",          slice);   // replaced spaces by %20
      replace(string, "%study%",           study2);  // unprocessed
      replace(string, "%study2%",          study);   // replaced spaces by %20
      replace(string, "%patid%",           patid2);  // unprocessed
      replace(string, "%patid2%",          patid);   // replaced spaces by %20
      replace(string, "%seruid%",          seruid);
      replace(string, "%studyuid%",        studyuid);
      replace(string, "%sopuid%",          sopuid);

      replace(string, "%size%",            size);
      replace(string, "%dsize%",           dsize);
      replace(string, "%compress%",        compress);
      replace(string, "%iconsize%",        iconsize);
      replace(string, "%graphic%",         graphic);
      replace(string, "%viewer%",          viewer);
      replace(string, "%lw%",              lw);

      replace(string, "%query%",           query);
      replace(string, "%db%",              db);
      replace(string, "%source%",          source);
      replace(string, "%dest%",            dest);
      replace(string, "%patientidmatch%",  patientidmatch);
      replace(string, "%patientnamematch%",patientnamematch);
      replace(string, "%studydatematch%",  studydatematch);
      replace(string, "%startdatematch%",  startdatematch);

      /* this code will substitute any other %var% with a cgi variable 
         with a default given in section for this server mode in dicom.ini 
	 
	 or substitute ... with the string result of a lua expression
	 <%= .... %>
      */
      
      char *p = strstr(string, "<%=");
      if (p)
      { char *p2 = strstr(string, "%>");
        char script[1000];
        *p=0;
        *p2=0;
        struct scriptdata sd1 = {&globalPDU, NULL, NULL, -1, NULL, NULL, NULL, NULL, NULL, 0, 0};
	strcpy(script, "return ");
	strcat(script, p+3);
        HTML("%s", string);
        HTML("%s", do_lua(&(globalPDU.L), script, &sd1));
	HTML("%s", p2+2);
	string[0] = '#';
      }
      else
      { char *p2 = strchr(string, '%');
        if (p2)
        { char *q = strchr(p2+1, '%');
          if (q && q!=p2+1)
          { char var[512], val[512], var2[512];
            *q=0;
            strcpy(var, p2+1);
            *q='%';;
            strcpy(var2, "%");
            strcat(var2, var);
            strcat(var2, "%");

            MyGetPrivateProfileString ( mode, var, var2, val, 512, ConfigFile);
            CGI(val, var, val);
            replace(string, var2, val);
          }
        }
      }

      /* runs: #comment, --servercommand, <?lua .... ?> as lua, or straight HTML output */
      if      (!inlua && string[0]=='#')                   strcpy(string, "");
      else if (!inlua && string[0]=='-' && string[1]=='-') SendServerCommand("", string+2, console);
      else if (!inlua && (p=strstr(string, "<?lua")))      {*p=0; HTML("%s", string); inlua=1; strcpy(chunk, p+5); strcat(chunk, "\n");}
      else if (!inlua)                                     HTML("%s", string);
    };

    if (f) fclose(f);
    if (*uploadedfile) unlink(uploadedfile);
    exit(0);
  };

  exit(0);
}

/* Dicom web viewer works as follows:

client		-> 	webserver			url		http://127.0.0.1/scripts/dgate.exe?mode=seriesviewer&series=...
webserver 	-> 	dicomserver 			query		imagelister:local|.....
client  	<- 	webserver <- dicomserver	query results	(to build list of urls of dicom slices)
client  	<- 	webserver			activex control http://127.0.0.1/ActiveFormProj1.ocx

Then for each slice that is required:

control 	-> 	webserver			url of slice	http://127.0.0.1/scripts/dgate.exe?mode=dicom&slice=......
webserver	-> 	dicomserver			dicom request   convert_to_dicom:....
control 	<- 	webserver <- dicomserver	dicom data

*/

static BOOL DgateWADO(int argc, char **argv)
{ char requestType[256];
  CGI(requestType,   "requestType",    "");		// is this a WADO request?
  if (strcmp(requestType, "WADO")!=0) return FALSE;

  console = fileno(stdout);
#ifdef WIN32
  setmode(console, O_BINARY);
#endif

  ConfigMicroPACS();
  LoadKFactorFile((char*)KFACTORFILE);

  char RootSC[256];
  MyGetPrivateProfileString ( RootConfig, "MicroPACS", RootConfig, RootSC, 64, ConfigFile);
  char WebServerFor[256];
  sprintf(WebServerFor, "127.0.0.1");
  MyGetPrivateProfileString ( RootSC,        "WebServerFor", WebServerFor, WebServerFor, 256, ConfigFile);
  MyGetPrivateProfileString ( "webdefaults", "address",      WebServerFor, WebServerFor, 256, ConfigFile);
  strcpy(ServerCommandAddress, WebServerFor);

  // allow overrule of port, ip and AE in dicom.ini from command line (e.g. in php or node.js)
  for (int i=1; i<argc; i++)
	{ 
	if (argv[i][0]=='-')
		{
		if (argv[i][1]=='h') strcpy((char *)MYACRNEMA, argv[i]+2);
		else if (argv[i][1]=='p') strcpy((char *)Port, argv[i]+2);
		else if (argv[i][1]=='q') strcpy(ServerCommandAddress, argv[i]+2);
		}
	}

  char command[1024];
  sprintf(command, "wadoparse:%s", global_query_string);
  SendServerCommand("", command, console, NULL, FALSE);
  exit(0);
};

//////////////////////////////////////////////////////////////////
// Elementary HL7 interface
//////////////////////////////////////////////////////////////////

// These contain HL7 DateTime code: provide .DATE and .TIME split, note: should start/end with |

static char HL7DateTimeTypes[]=
"|OBR.7|OBR.8|OBR.22|PID.7|PID.29|PV1.44|PV1.45|TXA.4|TXA.6|TXA.7|TXA.8|OBR.6|OBR.36|MSH.6|OBX.12|OBX.14|ORC.9|ORC.15|";

// read next item from data passed in p; data->item, type->name, tmp->internal
// note that data, type, and tmp are also used to maintain the parser state

void parseHL7(char **p, char *data, char *type, char *tmp, char *HL7FieldSep, char *HL7SubFieldSep, char *HL7RepeatSep)
{ int field;
  char *q, *r;
  char t[32];
  unsigned int i, dots=0;

  sprintf(t, "|%s|", type);								// to seek types
  if (strlen(type)>2)									// count .N
    for (i=0; i<strlen(type)-1; i++) dots += (type[i]=='.' && type[i+1]>='0' && type[i+1]<='9');

  if (**p==0)
  { strcpy(type, "EOM");								// end of message
    data[0]=0;
  }
  else if (strstr(HL7DateTimeTypes, t))							// translate datetime type
  { tmp[0] = 0;
    if (strlen(data)>8) strcpy(tmp, data+8);						// get time for later use
    data[8]=0;
    strcat(type, ".DATE");								// XXX.N.DATE
  }
  else if (strstr(type, ".DATE"))							// date was returned, now get time
  { strcpy(data, tmp);									// time
    strcpy(type+strlen(type)-4, "TIME");						// XXX.N.TIME
  }
  else if (strchr(data, *HL7SubFieldSep))						// translate field type, first entry
  { q = strchr(data, *HL7SubFieldSep);
    if (q) *q=0;
    if (q) strcpy(tmp, q+1);
    else   tmp[0]=0;

    strcat(type, ".0");									// XXX.N.0
  }
  else if (strchr(data, *HL7RepeatSep))						// translate repeat type, first entry
  { q = strchr(data, *HL7RepeatSep);
    if (q) *q=0;
    if (q) strcpy(tmp, q+1);
    else   tmp[0]=0;

    strcat(type, ".0");									// XXX.N.0
  }
  else if (dots>1)									// process subfields/repeats
  { if (tmp[0]==0)
    { q = strrchr(type, '.');
      if (q) *q=0;
      strcat(type, ".END");								// internal token
      data[0]=0;
    }
    else
    { strcpy(data, tmp);
              q = strchr(data, *HL7SubFieldSep);
      if (!q) q = strchr(data, *HL7RepeatSep);
      if (q) *q=0;
      if (q) strcpy(tmp, q+1);
      else   tmp[0]=0;

      q = strrchr(type, '.');
      sprintf(q+1, "%d", atoi(q+1)+1);							// XXX.N.M
    }
  }
  else if (**p==0x0d)
  { strcpy(type, "EOS"); strcpy(data, ""); (*p)+=1;					// end of segment
    if (**p==0x0a) (*p)+=1;
    if (strncmp(*p, "MSH", 3)==0) strcpy(type, "EOM");					// peek ahead for end of message
  }
  else if (**p==0x0a)
  { strcpy(type, "EOS"); strcpy(data, ""); (*p)+=1;					// end of segment
    if (**p==0x0d) (*p)+=1;
    if (strncmp(*p, "MSH", 3)==0) strcpy(type, "EOM");					// peek ahead for end of message
  }
  else if (strcmp(type, "EOS")==0 || strcmp(type, "EOM")==0 || strcmp(type, "")==0)	// new segment
  { field = 0;
    if (strncmp(*p, "MSH", 3)==0)
    { *HL7FieldSep    = (*p)[3];
      *HL7SubFieldSep = (*p)[4];
      *HL7RepeatSep   = (*p)[5];
      strncpy(type, *p, 3); strcat(type, ".0");
      strncpy(data, *p, 3); 
      (*p)+=4; 
    }
    else if ((*p)[0]>='A' && (*p)[0] <='Z' && 						// message header 
        (*p)[1]>='A' && (*p)[1] <='Z' &&
        (((*p)[2]>='A' && (*p)[2] <='Z') || ((*p)[2]>='0' && (*p)[2] <='9')))
    { strncpy(type, *p, 3); strcat(type, ".0");
      strncpy(data, *p, 3); 
      (*p)+=4; 
    }
    else
    { strcpy(type, "UNK.0");
      data[0]=0;
    }
  }
  else
  { field = atoi(type+4);								// genererate new segment
    sprintf(type+4, "%d", field+1);

    q = strchr(*p, *HL7FieldSep);							// upto segment separator
    if (q)
    { *q=0;
      strncpy(data, *p, 255);
      data[255]=0;
      *q = *HL7FieldSep;
      *p = q+1;
    }
    else
    { q = strchr(*p, 0x0d);								// or 0x0d
      r = strchr(*p, 0x0a);								// or 0x0a
      if (q || r)
      { if (q) *q = 0;
        if (r) *r = 0;
        strncpy(data, *p, 255);
        data[255]=0;
        if (r>q) q=r;
        *q = 0x0d;
        *p = q;										// process 0x0d again
      }
      else
      { strcpy(data, "");
        strcpy(type, "ERR");								// internal token
      }
    }
  }

  SystemDebug.printf("HL7 item: %s, contents: %s\n", type, data);  
}

// load HL7 data into modality worklist using the translation table from dicom.sql

void ProcessHL7Data(char *data)
{ char *p=data;
  char fields[1024], values[4096], type[32], item[256], uid[66], tmp[256];//, command[8192];
  int i;
  
  char HL7FieldSep    = '|';
  char HL7SubFieldSep = '^';
  char HL7RepeatSep   = '~';

  Database DB;
  if (!DB.Open ( DataSource, UserName, Password, DataHost ) ) 
  { OperatorConsole.printf("*** HL7 import: cannot open database");
    return;
  }

  memset(fields, 0, sizeof(fields));
  memset(values, 0, sizeof(values));
  memset(type, 0, sizeof(type));	// used for context of parser
  memset(tmp, 0, sizeof(type));

  while (TRUE)
  { parseHL7(&p, item, type, tmp, &HL7FieldSep, &HL7SubFieldSep, &HL7RepeatSep);
    if (strcmp(type, "ERR")==0) break;			// error

    // search type in database; if found prepare strings to write
    i = 0;
    while ( TRUE )
    { if(!WorkListDB[i].Element) break;
      if (WorkListDB[i].DICOMType!=DT_STARTSEQUENCE && WorkListDB[i].DICOMType!=DT_ENDSEQUENCE)
      {	if (strcmp(type, WorkListDB[i].HL7Tag)==0)
        { strcat(fields, WorkListDB[i].SQLColumn);
          strcat(fields, ",");

          strcat(values, "'");
          item[WorkListDB[i].SQLLength]=0;		// truncate data to make fit
	  for (int j=0; j<strlen(item); j++)		// remove invalid characters
	  { if (item[j]=='\'') item[j]='_';
          }
          strcat(values, item);
          strcat(values, "',");
        }
      }
      ++i;
    }

    // on end of message save the data into the database

    if (strcmp(type, "EOM")==0)
    { // search for special items (start with *) that are not read from the HL7 parser
      i = 0;
      while ( TRUE )
      { if(!WorkListDB[i].Element) break;

        if (WorkListDB[i].DICOMType!=DT_STARTSEQUENCE && WorkListDB[i].DICOMType!=DT_ENDSEQUENCE)
        { if (WorkListDB[i].HL7Tag[0]=='*')
          { strcat(fields, WorkListDB[i].SQLColumn);
            strcat(fields, ",");

            strcat(values, "'");		

            if (strcmp("*AN", WorkListDB[i].HL7Tag)==0)		// generate new accession number
            { GenUID(uid);
              strcat(values, uid + strlen(uid)-16);
            }
            else if (strcmp("*UI", WorkListDB[i].HL7Tag)==0)	// generate new uid
            { GenUID(uid);
              strcat(values, uid);
            }

            strcat(values, "',");
          }
        }
        ++i;
      }

      fields[strlen(fields)-1]=0;	// remove trailing ,
      values[strlen(values)-1]=0;	// remove trailing ,

      DB.AddRecord(WorkListTableName, fields, values);
      SystemDebug.printf("Entering modality worklist fields: %s\n", fields);  
      QueryFromGui++;

      fields[0]=0;
      values[0]=0;

      HL7FieldSep    = '|';
      HL7SubFieldSep = '^';
      HL7RepeatSep   = '~';
    }

    if (strcmp(type, "EOM")==0)		// end of message
      if (*p==0) break;			// end of file
  }

  DB.Close();
}


#ifdef WIN32

#include <malloc.h>

char *heapinfo( void )
{  UINT32 stot=0, sn=0, mtot=0, mn=0, ltot=0, ln=0;
   static char s[256];

HeapLock(GetProcessHeap());
   
  _HEAPINFO hinfo;
   int heapstatus;
   hinfo._pentry = NULL;
   while( ( heapstatus = _heapwalk( &hinfo ) ) == _HEAPOK )
   { if (hinfo._useflag == _USEDENTRY)
     { if (hinfo._size < 256) 
       { sn++, stot += hinfo._size;
//if (hinfo._size == 100)
//{ FILE *f;
//  f = fopen("c:\\debug.bin", "ab");
//  fwrite(hinfo._pentry, hinfo._size, 1, f);
//  fclose(f);
//}
       }
       else if (hinfo._size < 4096) mn++, mtot += hinfo._size;
       else ln++, ltot += hinfo._size;
     }
   }

HeapUnlock(GetProcessHeap());

  sprintf(s, "%d small (%d); %d medium (%d) %d large (%d) ", sn, stot, mn, mtot, ln, ltot);

   switch( heapstatus )
   {
   case _HEAPEMPTY:
      strcat(s, "OK - empty heap\n" );
      break;
   case _HEAPEND:
      strcat(s, "OK - end of heap\n" );
      break;
   case _HEAPBADPTR:
      strcat(s, "*** ERROR - bad heap ptr\n" );
      break;
   case _HEAPBADBEGIN:
      strcat(s, "*** ERROR - bad heap start\n" );
      break;
   case _HEAPBADNODE:
      strcat(s, "*** ERROR - bad heap node\n" );
      break;
   }

   return s;
}

#else

char *heapinfo( void )
{ static char s[] = "not available";
  return s;
}

#endif

 static int luadicomquery(lua_State *L)
  { const char *ae    = lua_tostring(L,1);
    const char *Level = lua_tostring(L,2);
    if (lua_isuserdata(L, 3)) 
    { DICOMDataObject *O = NULL;
      lua_getmetatable(L, 3);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);
      Array < DICOMDataObject * > *A = new Array < DICOMDataObject * >;
      luaCreateObject(L, NULL, A, TRUE); 
      if (O) 
      { DICOMDataObject *P = MakeCopy(O);

        unsigned char 	ip[64], port[64], compress[64], SOP[66];
	VR		*vr;
	UID		uid;
	// DICOMCommandObject	DCO;
	LE_UINT16	command, datasettype, messageid, priority;
	DICOMDataObject	*DDOPtr;
	int		level;
		
	level=0;
	if      (strncmp(Level, "PATIENTP", 8)==0) level=6;
	else if (strncmp(Level, "STUDYP",   6)==0) level=7;
	else if (strncmp(Level, "PATIENT",  7)==0) level=1;
	else if (strncmp(Level, "STUDY",    5)==0) level=2;
	else if (strncmp(Level, "SERIES",   6)==0) level=3;
	else if (strncmp(Level, "IMAGE",    5)==0) level=4;
	else if (strncmp(Level, "WORKLIST", 8)==0) level=5;

	ExtendedPDU_Service PDU;
	PDU.AttachRTC(&VRType);

	if(!GetACRNema((char *)ae, (char *)ip, (char *)port, (char *)compress))
		return 0;

	PDU.ClearAbstractSyntaxs();
	PDU.SetLocalAddress(MYACRNEMA);
	PDU.SetRemoteAddress((unsigned char *)ae);

	uid.Set("1.2.840.10008.3.1.1.1");
	PDU.SetApplicationContext(uid);

	if      (level==1) uid.Set("1.2.840.10008.5.1.4.1.2.1.1"); // PatientRootQuery
	else if (level==5) uid.Set("1.2.840.10008.5.1.4.31");      // WorkListQuery
	else if (level>=6) uid.Set("1.2.840.10008.5.1.4.1.2.3.1");      // PatientStudyOnlyQuery
	else               uid.Set("1.2.840.10008.5.1.4.1.2.2.1"); // StudyRootQuery
	PDU.AddAbstractSyntax(uid);

// alternative to above 4 lines to debug Aria connection issues
//      uid.Set("1.2.840.10008.5.1.4.1.2.1.1");	// PatientRootQuery
//	PDU.AddAbstractSyntax(uid);
//	uid.Set("1.2.840.10008.5.1.4.1.2.2.1"); // StudyRootQuery (C-Find SOP)
//	PDU.AddAbstractSyntax(uid);
//	uid.Set("1.2.840.10008.5.1.4.1.2.3.1");	// PatientStudyOnlyQuery
//	PDU.AddAbstractSyntax(uid);
//	uid.Set("1.2.840.10008.5.1.4.31");
//	PDU.AddAbstractSyntax(uid);

	PDU.SetTimeOut(TCPIPTimeOut);

	// Make the association for the FIND on port/ip
	if(!PDU.Connect(ip, port))
		return ( 0 );

	// Start a Patient/StudyRootQuery
	// Use passed data object and Level for query

	if (strcmp(Level, "PATIENTP")==0)
	  P->ChangeVR(0x0008, 0x0052, "PATIENT", 'CS', TRUE);
	else if (strcmp(Level, "STUDYP")==0)
	  P->ChangeVR(0x0008, 0x0052, "STUDY", 'CS', TRUE);
	else if (level != 5)
	  P->ChangeVR(0x0008, 0x0052, Level, 'CS', TRUE);

	vr = P->GetVR(0x0002, 0x0010); // delete transfer syntax
	if (vr) P->DeleteVR(vr);

	MyPatientRootQuery mq;
	MyStudyRootQuery sq;
	MyModalityWorkListQuery wq;
	MyPatientStudyOnlyQuery psq;

	if      (level==1) mq.Write(&PDU, P, A);
	else if (level==5) wq.Write(&PDU, P, A);
	else if (level>=6) psq.Write(&PDU, P, A);
	else               sq.Write(&PDU, P, A);
	
	PDU.Close();
        delete P;
	return 1;
      }
      return 1;
    }
    return 0;
  }
 
// lua DICOM printing support
#include "printsop.hpp"
 
class	PolaroidPDU_Service	:
	public	PDU_Service
	{
	public:
		BOOL	FixPolaroidConnection()
			{
			UINT	Index;
			UID		uidbad("1.2.840.10008.1.1");
			UID		uidgood("1.2.840.10008.1.2");
			
			Index = 0;
			while ( Index < AcceptedPresentationContexts.GetSize() )
				{
					AcceptedPresentationContexts.Get(Index).TrnSyntax.TransferSyntaxName
						= uidgood;
				printf("FPC: %s\n",
					AcceptedPresentationContexts.Get(Index).TrnSyntax.TransferSyntaxName.GetBuffer(1));
				++Index;
				}
			return ( TRUE );
			}					
	};
	
		
class MyGrayscalePrintManagement : public BasicGrayscalePrintManagementMeta
{

		// define stubs for missing SCP functions we don't need

	private:

		virtual BOOL    ActionPrint ( AbstractFilmSession *, DICOMDataObject * )
		{
			return FALSE;
		}

		virtual BOOL    ActionPrint ( AbstractFilmBox *, DICOMDataObject * )
		{
			return FALSE;
		}

		virtual void    HandlePrinterEvent ( DICOMDataObject *theDDO,
			UID		&theUID,
			UINT16 theEventTypeID,
			void *theArg)
		{
			RTC *aRTC = (RTC *)theArg;
			return;
		}

		virtual void    HandlePrintJobEvent ( DICOMDataObject *theDDO,
			UID		&theUID,
			UINT16 theEventTypeID,
			void *theArg)
		{
			return;
		}
		virtual BOOL	HandleError (
				const	ErrorDescription	&theError ) const
			{
			OperatorConsole.printf("HandleError: (%s:%d) %s::%s Msg = %s\n", 
			theError.GetFile(), theError.GetLine(), theError.GetClass(), 
			theError.GetMethod(), theError.GetMessage());

			return ( TRUE );
			}
};
#define COLLATED FALSE

static char* GetExtraOption(char* pszTextBlock, char* pszKey)
{
  static char*	pszValue = NULL;
  char*		pcDelimiter;
  char		szKey[100];

  if (pszValue)
  { free(pszValue);
    pszValue = NULL;
  }

  if ((!pszTextBlock) || (!pszKey) || (pszKey[0] == 0))
    return NULL;

  /* Search the key */
  strcpy(szKey, pszKey);
  strcat(szKey, "=");
  pszValue = strstr(pszTextBlock, szKey);
  if (!pszValue)
    return NULL;
  pszValue = strdup(pszValue + strlen(szKey));

  /* Terminate the string */
  pcDelimiter = strchr(pszValue, '\r');
  if (pcDelimiter)
    *pcDelimiter = 0;
  pcDelimiter = strchr(pszValue, '\n');
  if (pcDelimiter)
    *pcDelimiter = 0;
  pcDelimiter = strchr(pszValue, '^');
  if (pcDelimiter)
    *pcDelimiter = 0;
  pcDelimiter = strchr(pszValue, ';');
  if (pcDelimiter)
    *pcDelimiter = 0;

  return pszValue;
}

BOOL
StripDDOForPrinting (
	DICOMDataObject	*DDO )
	{
	DICOMDataObject	TDDO;
	VR				*vr;
	
	while ( vr = DDO->Pop() )
		{	
		if ( vr->Group == 0x0028 && vr->Element == 0x0002 )	{ TDDO.Push(vr);continue; }
		if ( vr->Group == 0x0028 && vr->Element == 0x0004 )	{ TDDO.Push(vr);continue; }
		if ( vr->Group == 0x0028 && vr->Element == 0x0010 )	{ TDDO.Push(vr);continue; }
		if ( vr->Group == 0x0028 && vr->Element == 0x0011 )	{ TDDO.Push(vr);continue; }
		if ( vr->Group == 0x0028 && vr->Element == 0x0034 )	{ TDDO.Push(vr);continue; }
		if ( vr->Group == 0x0028 && vr->Element == 0x0100 )	{ TDDO.Push(vr);continue; }
		if ( vr->Group == 0x0028 && vr->Element == 0x0101 )	{ TDDO.Push(vr);continue; }
		if ( vr->Group == 0x0028 && vr->Element == 0x0102 )	{ TDDO.Push(vr);continue; }
		if ( vr->Group == 0x0028 && vr->Element == 0x0103 )	{ TDDO.Push(vr);continue; }
		if ( vr->Group == 0x7fe0 && vr->Element == 0x0010 )	{ TDDO.Push(vr);continue; }
		
		delete vr;
		}
	while ( vr = TDDO.Pop() )
		DDO->Push(vr);

	return ( TRUE );
	}
	
static int PrintGrayScaleImages (
	BYTE	*MyAE,
	BYTE	*RemoteAE,
	BYTE	*RemoteHost,
	BYTE	*RemotePort,
	Array < DICOMDataObject *>	*ADDO,
	char	*FilmFormat,
	char	*AnnotationText,
	char	*FilmOrientation, 
	lua_State *L)
	{
	UID	aBasicFilmSessionSOPClassUID("1.2.840.10008.5.1.1.1");
	UID	aBasicFilmBoxSOPClassUID("1.2.840.10008.5.1.1.2");
	UID	aBasicGrayscaleImageBoxSOPClassUID("1.2.840.10008.5.1.1.4");
	UID	aPrintJobSOPClassUID("1.2.840.10008.5.1.1.14");
	UID	aBasicGrayscalePrintManagementMetaSOPClass("1.2.840.10008.5.1.1.9");
	UID	aBasicPrinterSOPClassUID("1.2.840.10008.5.1.1.16");
	UID	aBasicAnnotationBoxSOPClassUID("1.2.840.10008.5.1.1.15");
	UID	aAppuid("1.2.840.10008.3.1.1.1");
	PolaroidPDU_Service	PDU;
	int	ImagesPerRow, ImagesPerColumn;
	char*	pTmp;
	const char *callback = NULL;

	/* Get ImagesPerRow and ImagesPerColumn from FilmFormat like:
	   "STANDARD\\2,2"
	*/
	ImagesPerRow = ImagesPerColumn = 1;
	pTmp = strchr(FilmFormat, '\\');
	if (pTmp)
	{ ImagesPerRow = atoi(pTmp + 1);
	  pTmp = strchr(pTmp, ',');
	  if (pTmp)
	    ImagesPerColumn = atoi(pTmp + 1);
	}

	// create a run-time classer for the PDU_Service to use
	// in making explicit VR's out of implicit ones
	// tell RTC to keep descriptions and the dictionary name

	PDU.AttachRTC(&VRType);


	MyGrayscalePrintManagement aSession;  

	// add the abstract syntaxes we need to use in the association

	PDU.SetApplicationContext(aAppuid);
	PDU.AddAbstractSyntax(aBasicGrayscalePrintManagementMetaSOPClass);
	//                ^^^^ this the one accepted by the MIR print_server app !

	PDU.AddAbstractSyntax(aPrintJobSOPClassUID);
	PDU.AddAbstractSyntax(aBasicAnnotationBoxSOPClassUID);

	// set application titles (MIR_PRINT_SCU for test with CTN)

	PDU.SetLocalAddress((BYTE *)MyAE);
	PDU.SetRemoteAddress((BYTE *)RemoteAE);

	callback = "none";
	if (lua_isnumber(L, 5)) callback = lua_tostring(L, 5);
	if (lua_isstring(L, 5)) callback = lua_tostring(L, 5);
	if (lua_isfunction(L, 5)) callback = "function";

	if (atoi(callback)) 
	   Progress.printf("Process=%d, Type='dicomprint', Active=1\n", atoi(callback));

	if(!PDU.Connect((BYTE *)RemoteHost, (BYTE *)RemotePort))
		{
		if (atoi(callback)) 
			Progress.printf("Process=%d Error='DCM_E_PRINT_CONNECT' Type='dicomprint', Active=0\n", atoi(callback));
		return 1;
		}
	PDU.FixPolaroidConnection ();

	// add the abstract syntaxes making up the
	// BasicGrayscalePrintManagementMetaSOPClass
	// as aliases so that PDU can find a presentation context to use for them

	PDU.AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
		aBasicFilmSessionSOPClassUID);
	PDU.AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
		aBasicFilmBoxSOPClassUID);
	PDU.AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
		aBasicGrayscaleImageBoxSOPClassUID);
	PDU.AddAbstractSyntaxAlias(aBasicGrayscalePrintManagementMetaSOPClass,
		aBasicPrinterSOPClassUID);
	PDU.FixPolaroidConnection ();
 
	// set an argument for event handler callbacks to use

	aSession.SetPrinterEventUserArg((void *)&VRType);
	aSession.SetPrintJobEventUserArg((void *)&VRType);

	// attempt to get printer status
	DICOMDataObject aStatusDDO;

	// all supposedly optional parameters

	VR *aVR;

	char *aString = "1";
	aVR = new VR(0x2000, 0x0010, strlen(aString), (void *)aString, FALSE);
	aSession.Push(aVR);

	aString = "MED ";
	aVR = new VR(0x2000, 0x0020, strlen(aString), (void *)aString, FALSE);
	aSession.Push(aVR);

	aString = "BLUE FILM ";
	aVR = new VR(0x2000, 0x0030, strlen(aString), (void *)aString, FALSE);
	aSession.Push(aVR);

	aString = "MAGAZINE";
	aVR = new VR(0x2000, 0x0040, strlen(aString), (void *)aString, FALSE);
	aSession.Push(aVR);

	aString = "QUIRT CqPrint ";
	aVR = new VR(0x2000, 0x0050, strlen(aString), (void *)aString, FALSE);
	aSession.Push(aVR);

	// optional MemoryAllocation (not set)

	// Film box parameters:
	// Image display format STANDARD\2,1
	// FilmOrientation = PORTRAIT
	// FilmSizeID 8INX10IN
	// MagnificationType = REPLICATE
	// BorderDensity = BLACK
	// EmptyImageDensity = BLACK
	// print management object handles reference sequences for us...

	BOOL aCollationFlag = COLLATED;
	UINT aFilmBoxIndex=0;

	UINT	ImageIndex = 0;
	UINT	ImagesOnASheet = ImagesPerRow * ImagesPerColumn;
		
	while ( ImageIndex < ADDO->GetSize() )
		{
		if (atoi(callback))
			Progress.printf("Process=%d, Total=%d, Current=%d\n", atoi(callback), ADDO->GetSize(), ImageIndex);
		else if (strcmp(callback, "function")==0)
			{
			}
		else if (strcmp(callback, "none")==0)
			{
			}
		else 
			luaL_dostring(L, callback);

		// for each film
		// create a new film box
		AbstractFilmBox *aFilmBox = new AbstractFilmBox(aSession);


		// set film box'es attributes
		// when doing VR's with multiple values, don't forget
		// to escape the \ or you won't have one in your string...
		char	TempString[256];
		strcpy(TempString, FilmFormat);
		if (strlen(TempString) & 0x01)
			strcat(TempString, " ");
			
//		aString = "STANDARD\\1,1";
		aVR = new VR(0x2010, 0x0010, strlen(TempString), (BOOL)TRUE);
		memcpy((void*)aVR->Data, (void*)TempString, strlen(TempString));
		aFilmBox->Push(aVR);

		strcpy(TempString, FilmOrientation);
		if (strlen(TempString) & 0x01)
			strcat(TempString, " ");

//		aString = "PORTRAIT";
		aVR = new VR(0x2010, 0x0040, strlen(TempString), (void *)TempString, FALSE);
		aFilmBox->Push(aVR);

		aString = "14INX17IN ";
		aVR = new VR(0x2010, 0x0050, strlen(aString), (void *)aString, FALSE);
		aFilmBox->Push(aVR);

//		aString = "NONE";
//		aString = "REPLICATE";
		aString = "BILINEAR";
//		aString = "CUBIC";
		aVR = new VR(0x2010, 0x0060, strlen(aString), (void *)aString, FALSE);
		aFilmBox->Push(aVR);

/*		aString = "MAYBE";
		aVR = new VR(0x2010, 0x0140, strlen(aString), (void*)aString, FALSE);
		aFilmBox->Push(aVR);
*/	

		aString = "BLACK ";
		aVR = new VR(0x2010, 0x0100, strlen(aString), (void *)aString, FALSE);
		aFilmBox->Push(aVR);

		aString = "BLACK ";
		aVR = new VR(0x2010, 0x0110, strlen(aString), (void *)aString, FALSE);
		aFilmBox->Push(aVR);

/*		aString = "COMBINED";
		aVR = new VR(0x2010, 0x0030, strlen(aString), (void *)aString, FALSE);
		aFilmBox->Push(aVR);
*/

		// the combined annotation style means that
		// annotation box at position 0 is a film label,
		// and annotation boxes at positions corresponding to
		// image positions are image labels -- this is printer
		// dependent and specified in the conformance statement...

		// add a film label

		// for each image on a film create and add a new image box

		UINT aImageBoxIndex=0;
/*
		AbstractAnnotationBox *aAnnotationBox =
			new AbstractAnnotationBox(aSession);
		
		LE_UINT16 *aNumber = new LE_UINT16((UINT16)(0));
		aVR = new VR(0x2030, 0x0010, sizeof(*aNumber), (void *)aNumber, TRUE);
		aAnnotationBox->Push(aVR);
		*/
	
/*		aString = "Sample Output";
		aVR = new VR(DC3TagFromName(TextString), 
        	strlen(AnnotationText)+1, (BOOL) TRUE );
		strcpy((char*)aVR->Data, AnnotationText);
		aAnnotationBox->Push(aVR);

		aFilmBox->Add(aAnnotationBox);
		*/

		/*
		AbstractImageBox *aImageBox = new AbstractImageBox(aSession);

		LE_UINT16 *aNumber = new LE_UINT16((UINT16)(aImageBoxIndex + 1));
		aVR = new VR(DC3TagFromName(ImageBoxPosition), 
       		sizeof(*aNumber), (void *)aNumber, TRUE);
   		aImageBox->Push(aVR);

		// create the PreformattedGrayscaleImageSequence attribute
			
		aVR = new VR(0x2020, 0x0010, 0, FALSE);
   		aImageBox->Push(aVR);
		*/

		UINT	ImageOnFilmIndex = 0;
		while ( (ImageIndex < ADDO->GetSize()) && 
				(ImageOnFilmIndex < ImagesOnASheet ))
			{
			if (atoi(callback))
				Progress.printf("Process=%d, Total=%d, Current=%d\n", atoi(callback), ADDO->GetSize(), ImageIndex);
			else if (strcmp(callback, "function")==0)
				{
				}
			else if (strcmp(callback, "none")==0)
				{
				}
			else 
				luaL_dostring(L, callback);
			DICOMDataObject	*DDO = ADDO->Get(ImageIndex);
			ADDO->Get(ImageIndex) = NULL;
			StripDDOForPrinting (DDO);

// from dicomprn
			AbstractImageBox *aImageBox = new AbstractImageBox(aSession);
			LE_UINT16	*aNumber = new LE_UINT16((UINT16)(ImageOnFilmIndex + 1));
			aVR = new VR(0x2020, 0x0010, sizeof(*aNumber), (void *)aNumber, TRUE);
   			aImageBox->Push(aVR);

			// create the PreformattedGrayscaleImageSequence attribute
			aVR = new VR(0x2020, 0x0110, 0, FALSE);
	   		aImageBox->Push(aVR);
// end from dicomprn

   			aImageBox->Push(DDO);
			++ImageIndex;
			++ImageOnFilmIndex;

			aFilmBox->Add(aImageBox);

			}
			
//		aFilmBox->Add(aImageBox);

		/*
		aAnnotationBox = new AbstractAnnotationBox(aSession);
		
		aNumber = new LE_UINT16((UINT16)(aImageBoxIndex + 1));
		aVR = new VR(0x2030, 0x0010, sizeof(*aNumber), (void *)aNumber, TRUE);
   		aAnnotationBox->Push(aVR);

		aString = "Copyright 1996, UCDMC Radiology/MSU Radiology";
		aVR = new VR(0x2030, 0x0020, strlen(aString), (void *)aString, FALSE);
   		aAnnotationBox->Push(aVR);

		aFilmBox->Add(aAnnotationBox);
		*/

		// add the film box to the current session

		aSession.Add(aFilmBox);

		// print film now and delete if NO collation...

		if(!aCollationFlag)
			{
			UID aPrintJobUID("");

			if(!aSession.PrintFilm(&PDU, &aPrintJobUID))
				{
				if (atoi(callback)) 
					Progress.printf("Process=%d, Error='DCM_E_PRINT_FILM', Type='dicomprint', Active=0\n", atoi(callback));
				return 2; // DCM_E_PRINT_FILM;
				}


			// do a Get on the print job...

// mvh causes problem when working with MIR print_server
#if 0
			DICOMDataObject aPrintJobDDO;

			aString = "";
			aVR = new VR(0x2100, 0x0020, strlen(aString), (void *)aString, FALSE);
			aPrintJobDDO.Push(aVR);
			aString = "";
			aVR = new VR(0x2100, 0x0030, strlen(aString), (void *)aString, FALSE);
			aPrintJobDDO.Push(aVR);
			aString = "";
			aVR = new VR(0x2000, 0x0020, strlen(aString), (void *)aString, FALSE);
			aPrintJobDDO.Push(aVR);
			aString = "";
			aVR = new VR(0x2100, 0x0040, strlen(aString), (void *)aString, FALSE);
			aPrintJobDDO.Push(aVR);
			aString = "";
			aVR = new VR(0x2100, 0x0050, strlen(aString), (void *)aString, FALSE);
			aPrintJobDDO.Push(aVR);
			aString = "";
			aVR = new VR(0x2110, 0030, strlen(aString), (void *)aString, FALSE);
			aPrintJobDDO.Push(aVR);
			aString = "";
			aVR = new VR(0x2100, 0x0070, strlen(aString), (void *)aString, FALSE);
			aPrintJobDDO.Push(aVR);

			while( TRUE )
				{
				cerr << "Request print job status:" << endl;
				if ( aSession.GetPrintJob( &PDU, aPrintJobUID, &aPrintJobDDO ) )
					{
					cerr << "Got print job status!" << endl;
					DumpDDO(&aPrintJobDDO, 0, &aRTC);
					}
				else
					{
					cerr << "Can't get print job status!" << endl;
					break;
					}
				cerr << endl;

				aVR = aPrintJobDDO.GetVR(0x2100, 0x0020);

				if( !aVR || !aVR->Data ||
					(strncmp((const char *)aVR->Data,
						"PENDING", strlen("PENDING")) &&
					strncmp((const char *)aVR->Data, "PRINTING",
						strlen("PRINTING"))))
					{
					break;
					}
#ifdef	WINDOWS
				Sleep(5000);
#else
				sleep(5);
#endif
				aPrintJobDDO.Reset();
				}
#endif


// mvh: deletes the MIR file from disk
#if 0
			if(!aSession.DeleteFilm(&PDU))
				{
				cerr << "DeleteFilm(" <<  aFilmBoxIndex << ") failed." << endl;
				return ( FALSE );
				}
			cerr << "Film " << aFilmBoxIndex << " Deleted!" << endl;
#endif
			}
		++aFilmBoxIndex;
		}

	// print the entire session if collation was requested

	if(aCollationFlag)
		{
		UID aPrintJobUID("");

		if(!aSession.PrintSession(&PDU))
			{
			return 2; // DCM_E_PRINT_SESSION;
			}

		// do a Get on the print job...
		DICOMDataObject aPrintJobDDO;
		aString = "";
		aVR = new VR(0x2100, 0x0020, strlen(aString), (void *)aString, FALSE);
		aPrintJobDDO.Push(aVR);
		aString = "";
		aVR = new VR(0x2100, 0x0030, strlen(aString), (void *)aString, FALSE);
		aPrintJobDDO.Push(aVR);
		aString = "";
		aVR = new VR(0x2000, 0x0020, strlen(aString), (void *)aString, FALSE);
		aPrintJobDDO.Push(aVR);
		aString = "";
		aVR = new VR(0x2100, 0x0040, strlen(aString), (void *)aString, FALSE);
		aPrintJobDDO.Push(aVR);
		aString = "";
		aVR = new VR(0x2100, 0x0050, strlen(aString), (void *)aString, FALSE);
		aPrintJobDDO.Push(aVR);
		aString = "";
		aVR = new VR(0x2110, 0x0030, strlen(aString), (void *)aString, FALSE);
		aPrintJobDDO.Push(aVR);
		aString = "";
		aVR = new VR(0x2100, 0x0070, strlen(aString), (void *)aString, FALSE);
		aPrintJobDDO.Push(aVR);

		if ( aSession.GetPrintJob( &PDU, aPrintJobUID, &aPrintJobDDO ) )
			{
//			DumpDDO(&aPrintJobDDO, 0, &VRType);
			}
		else
			{
//			cerr << "Can't get print job status!" << endl;
			}
		}

	// delete the film session

// mvh: deletes the MIR file from disk
#if 0
	if(!aSession.Delete(&PDU))
		{
		cerr << "aSession.Delete() failed." << endl;
		return ( FALSE );
		}
	cerr << "Film session deleted!" << endl;
#endif

	// terminate the association
	PDU.Close();

	if (atoi(callback)) 
		Progress.printf("Process=%d Type='dicomprint', Active=0\n", atoi(callback));
	return DCM_E_OK;
	}
	
static int WINAPI DcmPrintADDO(Array<DICOMDataObject*>*pADDO,
	char* pszRemoteAE, char* pszAnnotation, char* pszExtra, lua_State *L)
{ int		rc;
  char		szImageDisplayFormat[30];
  char		szFilmOrientation[30] = "PORTRAIT";
  char*		pszExtraOption;
  unsigned char ip[64], port[64], compress[64];

  if(!GetACRNema((char *)pszRemoteAE, (char *)ip, (char *)port, (char *)compress))
	return 0;
  
  /* Create default ImageDisplayFormat */
  switch (pADDO->GetSize())
  { case 1:
      strcpy(szImageDisplayFormat, "STANDARD\\1,1");
      break;
    case 2:
      strcpy(szImageDisplayFormat, "STANDARD\\1,2");
      break;
    default:
      strcpy(szImageDisplayFormat, "STANDARD\\2,2");
      break;
  }

  /* Parse all extra options: maybe overrule some defaults */
  if (pszExtra)
  { pszExtraOption = GetExtraOption(pszExtra, "ImageDisplayFormat");
    if (pszExtraOption)
      strcpy(szImageDisplayFormat, pszExtraOption);

    pszExtraOption = GetExtraOption(pszExtra, "FilmOrientation");
    if (pszExtraOption)
      strcpy(szFilmOrientation, pszExtraOption);

    /* Free the static in GetExtraOption */
    GetExtraOption(NULL, NULL);
  }

  /* Do the print job */
  rc = PrintGrayScaleImages(MYACRNEMA,
	(unsigned char*)pszRemoteAE, ip, port,
	pADDO,
	szImageDisplayFormat,
	pszAnnotation,
	szFilmOrientation, L);
  return rc;
}

  static int luadicomprint(lua_State *L)
  { if (lua_isuserdata(L,1)) 
    { DICOMDataObject *O = NULL;
      Array < DICOMDataObject * > *A=NULL;
      int rc=0;

      lua_getmetatable(L, 1);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
        lua_getfield(L, -1, "ADDO");  A = (Array < DICOMDataObject * > *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);

      if (A) 
	rc=DcmPrintADDO(A, (char *)lua_tostring(L, 2), (char *)lua_tostring(L, 3), (char *)lua_tostring(L, 4), L);
      if (O) 
      { Array<DICOMDataObject*>	ADDO;
        DICOMDataObject *O2 = MakeCopy(O);
        ADDO.Add(O2);
	rc=DcmPrintADDO(&ADDO, (char *)lua_tostring(L, 2), (char *)lua_tostring(L, 3), (char *)lua_tostring(L, 4), L);
      }
      if (rc!=DCM_E_OK)
      { lua_pushstring(L, "Failed to print DICOM image(s)");
        return 1;
      }
      return 0;
    }	      
    return 0;
  }

  static int luadicomget(lua_State *L)
  { const char *ae    = lua_tostring(L,1);
    const char *Level = lua_tostring(L,2);
    if (lua_isuserdata(L, 3)) 
    { DICOMDataObject *O = NULL;
      lua_getmetatable(L, 3);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);
      if (O) 
      { unsigned char 	ip[64], port[64], compress[64], SOP[66];
	VR		*vr;
	UID		uid;
	LE_UINT16	command, datasettype, messageid, priority;
	DICOMDataObject	*DDOPtr;
	int		level;
		
	level=0;
	if      (strncmp(Level, "PATIENTP", 8)==0) level=6;
	else if (strncmp(Level, "STUDYP",   6)==0) level=7;
	else if (strncmp(Level, "PATIENT",  7)==0) level=1;
	else if (strncmp(Level, "STUDY",    5)==0) level=2;
	else if (strncmp(Level, "SERIES",   6)==0) level=3;
	else if (strncmp(Level, "IMAGE",    5)==0) level=4;

	ExtendedPDU_Service PDU;
	PDU.AttachRTC(&VRType);

	if(!GetACRNema((char *)ae, (char *)ip, (char *)port, (char *)compress))
        { // it does not exist: get host:port
          strcpy((char *)ip, (char *)ae);
          char *p = strchr((char *)ip, ':');
          if (p) 
          { *p=0;
            strcpy((char *)port, p+1);
          }
          else 
            strcpy((char *)port, "5678");
          strcpy((char *)compress, "UN");
        }

	// query to get list of SopClass UIDs

	PDU.ClearAbstractSyntaxs();
	PDU.SetLocalAddress(MYACRNEMA);
	PDU.SetRemoteAddress((unsigned char *)ae);

	uid.Set("1.2.840.10008.3.1.1.1");
	PDU.SetApplicationContext(uid);

	if      (level==1) uid.Set("1.2.840.10008.5.1.4.1.2.1.1"); // PatientRootQuery
	else if (level>=6) uid.Set("1.2.840.10008.5.1.4.1.2.3.1");      // PatientStudyOnlyQuery
	else               uid.Set("1.2.840.10008.5.1.4.1.2.2.1"); // StudyRootQuery
	PDU.AddAbstractSyntax(uid);

	PDU.SetTimeOut(TCPIPTimeOut);

	// Make the association for the FIND on port/ip
	if(!PDU.Connect(ip, port))
	  return ( 0 );

	// Use passed data object and Level for query

	DICOMDataObject *P = MakeCopy(O);

        if (strcmp(Level, "PATIENTP")==0)
	  P->ChangeVR(0x0008, 0x0052, "PATIENT", 'CS', TRUE);
	else if (strcmp(Level, "STUDYP")==0)
	  P->ChangeVR(0x0008, 0x0052, "STUDY", 'CS', TRUE);
	else if (level != 5)
	  P->ChangeVR(0x0008, 0x0052, Level, 'CS', TRUE);

	// get sopclass; needed for get
	P->ChangeVR(0x0008, 0x0016, "", 'UI', TRUE);

	vr = P->GetVR(0x0002, 0x0010); // delete transfer syntax
	if (vr) P->DeleteVR(vr);

	MyPatientRootQuery mq;
	MyStudyRootQuery sq;
	MyPatientStudyOnlyQuery psq;

	DICOMDataObject *P2 = MakeCopy(P);

        Array < DICOMDataObject * > *Q = new Array < DICOMDataObject * >;
	if      (level==1) mq.Write(&PDU, P, Q);
	else if (level>=6) psq.Write(&PDU, P, Q);
	else               sq.Write(&PDU, P, Q);
        
	PDU.Close();

	// now execute the get

	PDU.ClearAbstractSyntaxs();
	  
	if      (level==1) uid.Set("1.2.840.10008.5.1.4.1.2.1.3"); // PatientRootGet
	else if (level>=6) uid.Set("1.2.840.10008.5.1.4.1.2.3.3");      // PatientStudyOnlyGet
	else               uid.Set("1.2.840.10008.5.1.4.1.2.2.3"); // StudyRootGet
	PDU.AddAbstractSyntax(uid);

        for (int i=0; i<Q->GetSize(); i++)
	{ UID iUID;
	  SetUID ( iUID, Q->Get(i)->GetVR(0x0008, 0x0016) );
          PDU.AddAbstractSyntax ( iUID );		// adds type of this image to presentation contexts
	}

	// Make the association for the GET on port/ip
	if(!PDU.Connect(ip, port))
        { delete P;
          delete P2;
  	  for (int i=0; i<Q->GetSize(); i++) delete (DICOMDataObject *)(Q->Get(i));
	  delete Q;
	  return ( 0 );
	}

	MyPatientRootGetGeneric mg;
	MyStudyRootGetGeneric sg;
	MyPatientStudyOnlyGetGeneric psg;

        Array < DICOMDataObject * > *A = new Array < DICOMDataObject * >;
        luaCreateObject(L, NULL, A, TRUE); 

	if      (level==1) mg.WriteGet(&PDU, P2, A);
	else if (level>=6) psg.WriteGet(&PDU, P2, A);
	else               sg.WriteGet(&PDU, P2, A);

	PDU.Close();

        delete P;
        delete P2;
  	for (int i=0; i<Q->GetSize(); i++) delete (DICOMDataObject *)(Q->Get(i));
	delete Q;
	return 1;
      }
      return 0;
    }
    return 0;
  }
 
  static int luadicomstore(lua_State *L)
  { const char *ae    = lua_tostring(L,1);
    BOOL flag=FALSE;
    if (lua_isuserdata(L, 2)) 
    { DICOMDataObject *O = NULL;
      Array < DICOMDataObject * > *A = NULL;  
      lua_getmetatable(L, 2);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
        lua_getfield(L, -1, "ADDO");  A = (Array < DICOMDataObject * > *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);
      if (A==NULL) 
      { A = new Array < DICOMDataObject * >;
        A->Add(O);
	flag=TRUE;
      }
      if (A) 
      { unsigned char 	ip[64], port[64], compress[64], SOP[66];
	VR		*vr;
	UID		uid, appuid;
	char 		*p;
        RunTimeClassStorage	RTCStorage;
	int		nfail=0;
		
	ExtendedPDU_Service PDU;
	PDU.AttachRTC(&VRType);

	if(!GetACRNema((char *)ae, (char *)ip, (char *)port, (char *)compress))
        { strcpy((char *)ip, (char *)ae);
          char *p = strchr((char *)ip, ':');
          if (p) 
          { *p=0;
            strcpy((char *)port, p+1);
          }
          else 
            strcpy((char *)port, "5678");
          strcpy((char *)compress, "UN");
        }

	PDU.SetRequestedCompressionType("");	// default
        PDU.SetRequestedCompressionType((char *)compress);	// default

	appuid.Set("1.2.840.10008.3.1.1.1");
        PDU.SetApplicationContext ( appuid );
        PDU.SetLocalAddress ( MYACRNEMA );
        PDU.SetRemoteAddress ( (unsigned char *)ae );
        PDU.SetTimeOut(TCPIPTimeOut);

        for (int i=0; i<A->GetSize(); i++)
        { vr = A -> Get(i) -> GetVR(0x0008, 0x0016);
          if (vr)
          { SetUID ( uid, vr );
            PDU.AddAbstractSyntax ( uid );
  	  }
	}

	uid.Set("1.2.840.10008.1.1");		// This one should always accept: verification
        PDU.AddAbstractSyntax(uid);		// assures connect will not 

	if (!PDU.Connect((unsigned char *)ip, (unsigned char *)port))
        { OperatorConsole.printf("*** dicomstore: failed to connect to host %s\n", ae);
          lua_pushstring(L, "Failed to connect to host");
          return 1;
        }

        Array < DICOMCommandObject * > *Rsp = new Array < DICOMCommandObject * >;
	
        for (int i=0; i<A->GetSize(); i++)
        { DICOMCommandObject *DCO = new DICOMCommandObject;
          Rsp->Add(DCO);

          vr = A -> Get(i) -> GetVR(0x0008, 0x0016);
          if (!vr) continue;
          SetUID ( uid, vr );
          if (!PDU.IsAbstractSyntaxAccepted(uid)) 
	  { OperatorConsole.printf("*** dicomstore: object not accepted\n");
            nfail++;
	    continue;
	  }

          // recompress data to be forwarded here according to accepted compression mode
          p = PDU.GetAcceptedCompressionType(uid);
          BOOL StripGroup2 = memicmp(p, "as", 2)!=0 && memicmp(p, "is", 2)!=0;
	  DICOMDataObject *DDO = MakeCopy(A->Get(i));
          recompress(&DDO, p, "", StripGroup2, &PDU);

          RTCStorage.SetUID(uid);

          if (!RTCStorage.Write(&PDU, DDO, DCO))
          { OperatorConsole.printf("*** dicomstore: failed to send DICOM image to %s\n", ae);
            nfail++;
	  }
	  delete DDO;
	}
	
	if (flag) delete A;
        //if (nfail)
        //{ lua_pushstring(L, "Failed to send DICOM image(s) to host");
        //  return 1;
        //}
        luaCreateObject(L, NULL, (Array < DICOMDataObject * > *)Rsp, TRUE); 
	return 1;
      }
    
      return 0;
    }
    return 0;
  }
 
  static int luadicomecho(lua_State *L)
  { const char *ae  = lua_tostring(L,1);
    DICOMCommandObject *DCOR = new DICOMCommandObject;
    DICOMDataObject *O = NULL;
    if (lua_isuserdata(L, 2)) 
    { lua_getmetatable(L, 2);
        lua_getfield(L, -1, "DDO");  O = (DICOMDataObject *) lua_topointer(L, -1); lua_pop(L, 1);
      lua_pop(L, 1);
    }
    
    PDU_Service		PDU;
    DICOMCommandObject	DCO;
    // DICOMCommandObject	DCOR;
    UID			uid;
    VR			*vr;
    LE_UINT16		command, datasettype, messageid;//, tuint16;
    BYTE		SOP[64], host[64], port[64], compr[64];
    
    PDU.ClearAbstractSyntaxs();
    PDU.SetLocalAddress(MYACRNEMA);
    PDU.SetRemoteAddress((BYTE *)ae);
    uid.Set("1.2.840.10008.3.1.1.1");	// Dicom APP
    PDU.SetApplicationContext(uid);
    uid.Set("1.2.840.10008.1.1");	// Verification
    PDU.AddAbstractSyntax(uid);
    
    if(!GetACRNema((char *)ae, (char *)host, (char *)port, (char *)compr)) 
    { // it does not exist: get host:port
      strcpy((char *)host, (char *)ae);
      char *p = strchr((char *)host, ':');
      if (p) 
      { *p=0;
        strcpy((char *)port, p+1);
      }
      else 
        strcpy((char *)port, "5678");
      strcpy((char *)compr, "UN");
    }
    
    if(!PDU.Connect(host, port))
    	return ( FALSE );
    
    strcpy((char*) SOP, "1.2.840.10008.1.1"); // Verification
    vr = new VR (0x0000, 0x0002, strlen((char*)SOP), (void*) SOP, FALSE);
    DCO.Push(vr);
    
    command = 0x0030;
    vr = new VR (0x0000, 0x0100, 0x0002, &command, FALSE);
    DCO.Push(vr);
    
    datasettype = 0x0101;	
    vr = new VR (0x0000, 0x0800, 0x0002, &datasettype, FALSE);
    DCO.Push(vr);
    
    messageid = 0x0002;
    vr = new VR (0x0000, 0x0110, 0x0002, &messageid, FALSE);
    DCO.Push(vr);
    
    if (O) 
	{	
        DICOMDataObject *DO = MakeCopy(O);
	while((vr=DO->Pop()))
	        {
		DCO.Push(vr);
		}
	delete DO;
	}

    PDU.Write(&DCO, uid);
    
    if(!PDU.Read(DCOR))
    	return ( 0 );	// associate lost
    
    luaCreateObject(L, (DICOMDataObject*)DCOR, NULL, TRUE); 
    
    PDU.Close();
    return ( 1 );
  }

  static int luatickcount(lua_State *L)
  { 
#ifdef WIN32
    lua_pushinteger(L, GetTickCount());
#else
    lua_pushinteger(L, time(NULL)*1000);
#endif
    return 1;
  }
