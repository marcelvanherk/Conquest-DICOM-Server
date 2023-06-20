{************************************************************************}
{*                                                                      *}
{*      file       : ServerDriver.PAS                                   *}
{*                                                                      *}
{*      type       : MAINFORM                                           *}
{*                                                                      *}
{*      location   : \QUIRT\APP\CONQUEST\DICOMSERVER                    *}
{*                                                                      *}
{*      purpose    : Dicom server for ConQuest based on Davis Dicom code*}
{*                                                                      *}
{*      author     : Marcel van Herk (NKI / AVL)                        *}
{*                                                                      *}
{*      date       : 19980405                                           *}
{*                                                                      *}
{*      portability: 32 bit delphi only (V3.0 up)                       *}
{*                                                                      *}
{*      notes      : None                                               *}
{*                                                                      *}
{************************************************************************}
{* Updates:
When            Who     What
19980405        mvh     Created
19980407        mvh     Added dgatesop.lst file creation; max AE to 16 chars
19980407        mvh     Sorted image list on SOPINSTANC to get correct order
19980412        mvh     Added DuplicateHandle code; but still deadlocks
19980413        mvh     Threaded mode avoids TCP/IP component (seems to fix deadlock)
19980414        mvh     Restart after switching server mode does not always work;
                        Fix application to threaded mode for now.
                        Added compression configuration.
19980415        mvh     Some fixes for the first installation code
19980505        mvh     Made length of directory field in MAGDEVIC 250 chars
19980608        jmm     Replaced TQuirtViewer with TImage => no need to load ntpars.dll
19980614        mvh     Merged code with use FPiette tcp components
19980616        mvh     All OK with new fpiette code; actually I had to fix my own bug
19980617        mvh     Save as bitmap and dump header in popup menu
19980618      mvh+jmm   Query and Move options for CQDICOM.DLL
19980619        mvh     Added header lister; default MagDeviceThreshHold to 10 MB
19980620        mvh     MagDeviceThreshHold is now configurable
19980621        mvh     Renamed table to MagDevice; use full table names
                        Nicely coded DBF/Access choice; but fixed to DBF
                        Delete patient and refresh database menu options
19980622        mvh     Double click query memo to make new query
                        Show on bitmap: patient is being deleted/file not found
19980623        mvh     Save bookmarks of tables; move to next record after delete
19980624        mvh     DICOM.SQL: StudyNumber (StudyID in DICOM) is now 64 long
                        (Microsoft SQL server does not allow truncation)
                        Update bitmap after loading for fast display
                        Text strings in UDP input to 1024 (for long SQL errors)
                        Default password and user name to 'conquest'
                        PAGETIMEOUT to 600 for better database performance
                        Made DataSource name more or less configurable
                        Made all port numbers more or less configurable
                        Installing a second server on one PC while one server is running works
19980702	mvh	Added KBUSED field in MAGDEVICE table
19980703	mvh	Some changes in static texts and hints
19980704	mvh	Found a handle leak in RunDgate; not fixed yet
19980708	mvh	Make anonymous; show progress for move
19980710	mvh	Added experimental archive option; fix stopcopying option

19980711	mvh	Fix double click query on empty form; use tray icon
19980713	mvh	Add Name of server in tooltip of tray icon; fix DiskFree
19980717      mvh+ljz   Fix crash if directory does not exist
19980721        mvh     Clicking tray icon restores window
19981219        mvh     Improved error handling display;
19981220        mvh     List header of undisplayable object in static
                        Read and save FileNameSyntax; default=3
19981221        mvh     Supress display of large files (>1 MB)
19981222        mvh     Use FieldByName, added combobox selectors
                        Disable study navigator if only 1 available
                        Moved modality from the study to the series level
19981229        mvh     Changed order of TableN.close to avoid side effects
19990104        mvh     Fixed MAGDEVICE creation under SQL server (KBused optional)
19990109        mvh     Made ImageNumber 10 bytes wide (for GE AdvantageWin)
19990110        mvh     Combobox1 is now patient filter and or selector
                        Added access to cache and jukebox in browser but
                        with limited support: up to 10 devices (one character)
                        Added archiving page; fixed most handle leaks
19990112        mvh     Finished first archiving page; -hide command line option
                        Added clear database and regen single device buttons
                        Clear combobox for each page select
                        NOTES: Archive page is configured by editing JUKEBOX.INI
                          Restore original configuration or restart to load it
                          Archiving allows up to 10 Jukeboxes, but only the
                          selected one is used for writing. Only the drive
                          with MAG0 is checked for disk space for automatic
                          archiving. Only device CACHE0 is passed correctly
                          to the burn program. This means: stick to 1 MAG
                          device and 1 CACHE device for now.
19990113        mvh     Speeded archiving by selecting MAG0 if there is only 1
                        Renamed entry to BurnThresholdMB, made BurnProgramList
                        Note: to do - keep log file of archiving operations.
                                    - add RT objects to default sop file.
19990114        mvh     Added date and time formats for burnprograms
19990115        mvh     label format burnprograms; no write empty jukebox.ini
                        Sync magdevice0 with DataDir; %% and %p formats
19990315        mvh     Added EchoNumber in image table - used to separate PD,T2
                        Completed dgatesop.lst with Philips, GE, RT
                        Added NumberOfFrames in image table - to find multiframe objects
19990413        mvh     truncate QUERY request depending of level
19990510	ljz	Allow AEs starting with a digit (Siemens MRI)
19990527        mvh     Revision 3 of database layout (birthday, sex, bolus)
19990902        mvh     Added load/store of mirror devices and export converters
19990905        mvh     Switched word wrap off for memo's
                        Added DcmMoveFilesHere on right click of copy; but seems to crash
                        Problem: if we pass DcmMoveFilesHere servers AE/PORT saving
                        slices from own server will fail. Now pass 'test;1234'
20000127        mvh     Hide archiving tabsheet in new install
                        Add NKI sops to install;
20000128        mvh     Added extra GE sop; compressmode 2; disabled movefileshere
20000316        mvh     version 1.3.5: Integrates dgate as service operation
                        New install does not start dgate right away
20000317        mvh     Truncate patient list if too long
20000318        mvh     '$ RESTART' as console message kills and restarts server
                        '& DGATE xx' as console message runs dgate with args xx
20000323        mvh     Added query sop choices through level dropdown list
20000326        mvh     Service supported for more than one server:
                        servicename=AE; port passed as /process dgate.exe -!port
                        Removed need to enter password for SQL access
                        Save and restore user and password in dicom.ini
                        Added 'ping' option for remote check of archiving:
                        Deletes ConquestDICOMServer.Ping and makes
                        ConquestDICOMServer.response with archive status
                        Allow up to four servers at the same time, also as services
                        Note: only the first server will start correctly on boot!
                        (on boot, the /process parameter is not passed and the
                        default IP message port will be used -!1111)
                        Presence of file USESQLSERVER allows install on SQL server
                        Presence of file USEMSACCESS allows install using access db
20000328        mvh     Added tests of local server at key moments
                        added -noconfirmquit option; show VERSION in caption
20000402        mvh     Install service with startup at right socket
                        Read service socket from registry on startup
20000403        mvh     Improved socket handling; fixed space in path when not
                        service; space in path blocks service install.
                        Now recognizes installed but not running service
                        Number of servers on one PC is now unlimited.
20000417        mvh     Fixed MSACCESS ODBC creation
20000501	ljz	ExportModalityx should be UPPER-case
20000528        mvh     Added "force display of large image", "remove image from
                        database", "run external viewer" and demoviewer option
                        Removed '.' as workdir from runprogram etc (allow bat files ?)
                        Added democopy option: copy incoming to dir + CallingAE.v2
20000529	mvh	Merged two changes
20000529        mvh     Release to 1.3.6
20000616        mvh     Attemp to create NewInstallForm database selector
20000625        mvh     Added NewInstallForm database selector; default to MS ACCESS
                        Added Send ... To menu items in popupmenu
                        Removed '*' items from Dicom system selectors
                        Added Send Selected Images for more flexibility
                        Added animated display menu options
20000626        mvh     Added Send Selected Images selector window
20000701        mvh     Some adaptations of Send Selected Images; MaxFileNameLength
                        TrimRight strings in query - trailing spaces depend on db
20000707        mvh     Added Edit Database by Hand menu option
20000808        mvh     Added ,0 code in timed archiving
20000915        mvh     Added ListenSocket.Addr (0,0,0,0) in multi-app mode
20001107        mvh     Added CheckBoxViewIncoming for demo display of incoming images
20001108        mvh     Display originating server on incoming image;
                        Disable menus when the ViewIncoming option is selected
20001129        mvh     Added fix so that DICOM.INI may contain UNC path
                        Added menu choice to change ID of selected series
                        Version to 1.3.7
20010312	ljz	Trimmed remote IP so that hostnames can be resolved in CqDicom.dll
20010318        mvh     Trim servername and ip;
                        alt-rightclick the service buttons installs service 4 times
                        e.g., at ports 5678, 5679, 5680 and 5681 (all with same data)
                        This may be used for efficiency and to limit the effect of crashes
			Version to 1.3.8
20010327        mvh     Always pass curdir to runprogramxxx and startprogram -
                        required because directoryview changes current directory!
20010328        mvh     Uppercase account for service; colors for archive status
                        Added verify mirror disk button; burn only once per time
                        Show date and time for archival steps; added KeepAlive
                        Added MirrorVerifyTime (verifies whole MAG0)
                        Added conquestdicomserver.log with major messages
20010405        mvh     Added comma-separated list option for patid in query page
20010406        mvh     Ctrl-dblclick adds patid to comma-separated list
20010406        mvh     All status memo's are now limited in length and save to separate log files
                        Added (and disabled) simple zip code for log files per day
20010409        mvh     Do not log query page
20010409        mvh     Added Tape backup page; replace few GetCurrentDir by CurDir
20010410        mvh     Added timeouts (10s and 5 hours) to tape backup
20010412        mvh     Separate tape backup button - some fixes
20010416        mvh     Improved anonymize and change patient ID using new dgate -f options
                        Added drag and drop interface to add files to server
20010418        mvh     Changed description of forced backup set; refuse dropped directories
                        Version to 1.3.9
20010418        mvh     Update maintenance memo during drag and drop
20010418        mvh     Fix: timer4 was enabled if no KeepAlive specified in dicom.ini
                        Hide tape backup page in new installation
                        Added default install button in firstinstall
20010419        mvh     Added anonymize and change patient ID for study
20010502        mvh     Added DeleteThisStudy/Series
20010830        mvh     revision 4 of database definition (extended)
                        Added ExportStationName in dicom.ini
                        Updated dgatesop.lst to conform to DICOM 1999 final text
                        Version to 1.3.10
20010903        mvh     Suggest .\username as account
20010904        ljz     Fix: Query was freed twice when a DicomMove failed
20011108        mvh     Added automatic zipping of log files
                        Make sure that tape backup and archive pages are not shown
                        Added save/restore of ExportFilter settings
20011109        mvh     Small fixes:
                        do not write empty configuration strings (empty<>default)
                        keep some settings with original case (see sorg)
                        use StrToIntDef to avoid exceptions on invalid settings
                        Added ZipTime as configurable
20011127        mvh     Added SeriesPat and ImagePat fields to DICOM.SQL
                        Surpress fail message box when using SQL server
                        Version to 1.3.11
20020402        mvh     Adapted controls.pas: exception handling in finddragtarget and findvclwindow
20020409        mvh     DB rev 6: added frame of reference UID to series
20020416        mvh     Store/save UIDPrefix for in dicom.ini
                        When creating dicom.ini, create unique UIDPrefix:
                        '1.2.826.0.1.3680043.2.135.Date.Time
                        Added configurable TestReadTime: regular check of MAG0
20020417        ljz     Fix: QueryResult has not necessarily 64 char
                mvh     Renamed some data structures
20020422        mvh     Do not save empty UIDPrefix; fix change patid for study
20020424        mvh     Added simple weekly checks page; mail ALL failures
20020426        mvh     Fix in mail sender list; use first e-mail as 'to' address
20020428        mvh     All recipients show in to: header
                        Only serious error messages have *** in string
                        Added MailCollectTime and MailWaitTime
                        Avoid starting check more than once
20020510        ljz     Added CheckDicomSubmits (send mail when users forgot to
                        send images to the PACS)
20020513        mvh     Checks: empty time runs all day; added log header/footer
                        Updated CONTROLS.PAS: more exception handling
                        Fixed reading of weeklychecks; display CD num in header
                        Fix warnings and patient ID checks in CheckDicomSubmits
20020515        mvh     Added ImageType to database (rev 7)
                        Added find missing patients button;
                        allow more than one weekly check time (; separated, not adjacent)
20020515      mvh+ljz   Fix missing pats when query empty
                        Added grab options to weekly checks
20020517        mvh     Changed StudyModality to 64 chars (e.g., fits :
                        CT\MR\PT\XA\ST\MG\MS\RTIMAGE\RTDOSE\RTSTRUCT\RTPLAN\RTRECORD)
20020518        mvh     Fix in weeklychecks tag: too many with tag 0
20020519        mvh     Fix in rev 7 db definition: AcqDate is now SQL_C_DATE
			Added FixPhilips in DICOM.INI
20020520        mvh     Use DGATE.EXE for grabbing
20020522        mvh     Fix reading of entries with an '=' in e.g., dicom.ini
20020524        mvh     Fix acrnema.map problem with new install
20020529        mvh     Fixed duplicate mailing of messages, added WriteMemo
                        Added archive.log; extra parameter for regen for DBASEII
                        Removed MessageBoxes
20020609        mvh     Read and write FileCompressMode, default remains 2
20020613        mvh     Added alt-drop code: enter with new patient ID
20020819        mvh     Basic printer support; view incoming shows printer data
20020819        mvh     Process messages as files are dropped
                        Print incoming imageboxes on default printer
20020821        mvh     Printer accepts landscape and portrait any pictures per page
                        Progressbar shows printer queue activity
                        Note: printer queue is single user (but may be fixed, using film#)!
20020823        mvh     Added print options in browser - print to local dicom server
20020825        mvh     Headerlister is modeless - added CTRL-A, CTRL-C, CTRL-F, CTRL-S, F1, F3 end ESC
                        Printer has progress hook - required to avoid socket data loss
                        Selection of print page format; edited info text
                        Added all 1.3.11 fields to edit database by hand
20021015      kg+mvh    Start deleting patient started delete ''=ALL when no patient in list!!!!!
                        Protect all pop-up functions for no selection
20021016      mvh+kg    Better support for copying large #patients, even if one or more missing
                        Added denormalized database option (for DBF without ODBC)
                        Removed obsolete magdevice button
20021017       mvh      Added "" around dropped filenames; use filter instead
                        of master-detail (requires index) for built-in dbf support
                        Version to 1.4.0; added dropping of directories
                        Added UID counter passed to dgate to avoid UID problems
20021018       mvh      Fixed browser fail and missing dbase dir after non-odbc install
                        Limit dbase list in browser (alternative to image) to 100 lines
20021020       mvh      Added Kill and Restart the Server weekly check (packs built-in dbf)
                        Fix reading of database name if not default
20021021       mvh      Small fix in maintenance memo for very long strings
20021028       mvh      Also denormalized series database; default de-normalize study (rev 8)
                        Show denormalized study entries in database editor
20021029       mvh      Fixed query keys transmittal (sent sop when not needed)
20021213       mvh      Added EnableReadAheadThread as configurable parameter
                        Disabled GECTStorage amd GEMRStorage in dgatesop.lst
20030113       mvh      Version to 1.4.1; Fixed keepalive option (was never enabled)
                        Added PatientQuerySortOrder and such
20030128       mvh      Fix update of headerlister when no image shown
                        Make DBF table links case insensitive
20030319       ljz      Fix: ImageDB entry 'ReceivingCoil' is 16 bytes (not 12)
20030324       mvh      Start on 1.4.2: db rev 10: made ReceivingCoil 16 wide
20030402       ljz      Merged above two changes
20030409       ljz      Handle DicomPrint error; several path fixes in tape-handling
20030613       mvh      Catch WM_QUERYENDSESSION: shutdown disables close dialog
20030628       mvh      Fix that more than one dropped directory will be loaded
20030706       mvh      Prepare for release 1.4.2; new dicom.ini; kill server
                        Compression column in acrnema.map
20030707       mvh      Added manufacturer to denormalized db (rev 11)
			Write registry key HKEY_LOCAL_MACHINE\SOFTWARE\NKI AVL\
                        Conquest\Dicomserver\SERVERNAME with values:
                        directory=path without trailing backslash and
                        laststarted=yyyymmdd. For use by Dicom-works only.
                        Note: servername changes are updated on GUI startup only.
20030708       mvh      Made JPEG compression configurable (affects dgatesop.lst)
20030709       mvh      Write version into registry
20030710       mvh      Install service as system account (no user required)
                        Fix attempt to mail when not wanted
                        ------------------ release 1.4.2 ----------------------
20030717       mvh      Fix problems when registry access fails
20030809       mvh      Will automatically use ADO or MDBF when BDE not installed
                        Set version to 1.4.3
20030810       mvh      Log used DB engine; Added partial ID matching for MDBF
                        Default enable uncompressed button before install
                        Force user to push 'Save config button'
                        Added some text to install and maintenance pages
                        Do not display images when on jukebox (use force display)
20030819       mvh      Made max drop filename length longer (256->1024)
20030905       mvh      Date of zip file is one day before if zip before 8 in the morning
                        Hide external viewer menu directly after install
                        Optional configuration of SendUpperCaseAE added
                        Removed obsolete flush button
                        Do not allow install in directory with spaces in its name
                        Removed faulty extra text from install page
20030907       mvh      Fix ODBC generation for directory with spaces
                        Just warn against directory with spaces
20030909       mvh      Test write access to log files prior to startup
20030920       mvh      Mail messages that contain 'fail' instead of 'failed'
                        Added tape full warning (that will be mailed)
                        Added enables for timed archival (to allow retry/timer disable)
                        Changed order of burn CD message / tape backup
                        Added NextSlotToUse (slot n) next to NextCDToBurn (JUKEBOX0.n)
                        %in burn parameters n = NextSlotToUse, %c = NextCDToBurn
                        Manual archive buttons control checkboxes as well
20030921       mvh      CTRL-Double click "automatic archival label" starts archival
                        CTRL-Double click "weekly checks time" starts check
                        Added NightlyCleanThreshhold parameter (clean # MB at 01:00)
20040401       mvh      Added ImageID to database; version to 1.4.4
                        Show ImageID in database editor and browser
20040402       mvh      Allow anonymize etc on MAG0..MAG9;
			changed datasource name (now system DSN)
20040425       mvh      Added DoubleBackSlashToDB to dicom.ini
20040426       mvh      Version to 1.4.4a
20040528       mvh      Use built-in servertask commands for maintenance and browser
20040530       mvh      Browser for built-in DBASEIII driver uses extract server command
                        Database editor to read only for built-in DBASEIII driver
                        These changes allow efficient use of in-memory index for dbaseIII
                        OK for 10^7 images when patientID, studyUID or seriesUID passed in queries
                        Added PackDBF and IndexDBF to dicom.ini
			Reworded some in popup menu; wait for patient deletion to finish
                        Fix refresh for sql server
20040601	mvh	Added LongQueryDBF flag
20040605	mvh	Fix extract when no patients; kill server stops loop of server tasks;
                        start server on activate (after windows shows)
                        Pack the Database as Weekly Check Action
20040606	mvh	Fix in formactivate; display @ startup
20040610        mvh     use servertask to clear index and rebuild it in built-in dbase mode
20040614        mvh     small fix in keep server alive; added nightlymove option
20040615        mvh     small fix in nightlymove option; also reindex after regen device
20040713        mvh     Added FixKodak; version to 1.4.6
20040722        mvh     Installation defaults for FixPhilips and FixKodak are now 0
20040804        mvh     Added *.lst to zipped log files; added bug report option
                        Version to 1.4.7
20041029        mvh     Added merge and split series options;
			Process messages during long GUI actions if possible (no DB)
20041030        mvh     Modified GUI of merge and split, process messages
20041101	mvh	Check for MAG storage for split/merge
			Delete from DB does not use image (for orphaned DB)
                        USEDBASEIIIWITHOUTODBC replaced by Pos('\', Datasource)>0 while running
20041102	mvh	Added nj compression mode
20041108	mvh	Added warning when alt-dropping file(s) with $.. as Patid (debug)
20041128        mvh     Fixed wording of delete messages: works on all mag devices
			Fixed problem in DeleteThisSeries, Anonymize, and change
                        Patient ID: last series could not be modified/deleted
20041129	mvh	Made font size of server status window selectable; Made debug
			log switchable; service will continue to log to file even if
                        user interface is closed (but with different date/time layout)
                        Now can save and use acrnema.map without server restart
                        Fix startup problem when disk 'c' not available
20041201      ljz+mvh	Backup fail 1..5 files temp logged as ALMOST OK -> continues
20050106        mvh     Fixed install as four services, fixed nightlymove amount
                        Is now nightlymovethreshhold - free space: i.e.,
                        attempts to make nightlymovethreshhold MB free space
                        Fixed nightlymove: forgot to pass ',0' for MAG0
                        Also, try move even if select failed to avoid stall
                        after partial failed select
20050109        mvh     Made backup of and warn when dicom.sql is overwritten
                        Added TCPIPTimeOut, FailHoldOff, RetryDelay, QueueSize
                        Version to 1.4.8
20050114        mvh     On startup ask to delete ExportFailures*, CopyFailures*
20050422	mvh	Added BurnTime2 (2 dvds per day) and BurnWeekend (all weekend through)
20050429	mvh	db rev 14: added worklist support
20050829	mvh	recurse directories when dropping files to ease load dicom CD
20050831	mvh	Added find modality worklist to dgatesop.lst
20050901	mvh	Added clear worklist button, version to 1.4.9, WorkListMode
20050905	mvh	Modality worklist query, auto append worklist to dicom.sql
20050907	mvh	Avoid dicom.sql message in new install, v* and s* in acrnema.map
20050912	mvh	Small changes to dicom.ini, dicom.sql, dgatesop.lst; added SQLHost
			Small change to mdbf: extra refresh avoids browser failures
20051024        mvh     Version to 1.4.9a: recompiled with patched controls.pas
20051230        mvh     Version to 1.4.10
                        Moved clear worklist button and add pack database button
                        Added warning when worklist is added to dicom.sql: clear worklist required
                        Added settings: DebugLevel, CacheVirtualData, LargeFileSizeKB, VirtualServerFor#
                        enable study controls prior to return if same (none) image shown
20060102        mvh     Update of hints
20060103        mvh     1.4.10 Release
20060201	mvh	Added K-Pacs/EZDICOM based viewer
20060312        mvh     Keep case of VirtualServerFor; Message buffers from 1024 to 8192
                        Open k-pacs viewer on correct series; version to 1.4.11
20060314        mvh     Added experimental native Mysql access; installs when
                        libmysql.dll is found - uses mysql version 4.1.14
20060315        mvh     Keep case of FileNameSyntax
20060328        mvh     Updated hints and file version for release 1.4.11
20060618        mvh     Set default user for mySQL to root (thanks Chris Muller)
                        Seems to then also work with mySQL 5.0.22, version to 1.4.12
20060707        mvh     Bug reported by Paolo Marchesi: save config loses DBF
                        Now only set to default when previous does not exist
                        Use same zipmaster as in previous versions (zipmaster\org)
20061101        mvh     Added SeriesDescription to query page
20061129        mvh     Added control for debuglevel, and allow queries on UID
                        Double click queries labels to change mode
20061130        mvh     Added new dgate parameters; consolidated debuglevel control and param
                        Made application forms resizable and set size constraints
                        Small font control works on all status pages
                        Double click image to force display it
20061220        mvh     Pass Tempfile to kpacs viewer form
20070123        mvh     Added processfilter to allow browsing of patients with ' in patientid
20070126        mvh     Importconverters stuff and ForwardAssociationRelease in dicom.ini
                        Version to 1.4.12b
20070201        mvh     Version to 1.4.12c
20070204        mvh     Fix in generation of UIDPrefix: was not automatically generated on startup
20070206        mvh     ODBC buttons will also create database for SQL server
20070207        mvh     ODBC button creates database for native mysql
20070211        mvh     Native MYSQL did not create indices during install
                        Fixed selection of DBF (with odbc)
20070221        mvh     Started to SQLite (not yet fully functional)
20070305        mvh     Version to 1.4.13alpha
20070316        mvh     Use | in patid instead of , in drag and drop to allow dropping files with
			a , in them while changing patientif (not backwards compatible)
20070415        mvh     Back to , in patid in drag and drop: ljz fixed it in dgate.exe
                        Added Prefetcher flag in dicom.ini (default 0)
20070520        mvh     Fixed missing completed message in regen single device
20070705        mvh     Added BrowseThroughDBF: unified browser used for SqLite
                        Works for other datasources but not configured as such
                        Small fixes in completed message in several regen functions
                        Fixed view/edit database second time round (enable)
20070706        mvh     Auto create mysql registry settings for native mysql
                        Disallow anonymize and change to empty name
                        Message on end of odbc or db creation
                        Version to 1.4.13beta
20070708        mvh     Patched forms.pas for Chad Moser's crash problem.
                        Added BrowseThroughDBF setting in dicom.ini
                        Default 1 for SqLite, 0 for others
                        Use extract: also for dbaseIII: to avoid multi-user problems
                        when browsing and writing at same time
20070709        mvh     Small fix in dbf+bde browsing
                        Auto register webserver\ActiveFormProj1.ocx if found
                        Do not use extract: for dbaseIII: browser does not work
                        while indexing
20071030        mvh     As to delete DelayedFetchForwardFailures* on startup
20071103        mvh     Added LRUSort, AllowTruncate, ForwardCollectDelay,
                        MaximumExportRetries, MaximumDelayedFetchForwardRetries
                        Version to 1.4.13
20071104        mvh     Added DecompressNon16BitsJpeg, UseBuiltInDecompressor
                        Made IncomingCompression editable (double click label)
                        Made FileNameSyntax editable (double click label) and
                        allow choice of V2 and DCM through radiobuttons
20071114	mvh	Save root password while installing for mysql with password
20071115        mvh     Disabled multi-process mode in RunInThreadedModeClick to allow
                        multiple saves of ini file during install (for above password)
20071115	mvh	Save password and username for SQL server install as well
20071116	mvh	Longer default password for sql server 2005
20080124	mvh	Added mailer settings in dicom.ini
                        Disable LittleEndianExplicit on install
                        Added configuration for mode UJ
                        Database revision 16: Moved Stationname and InstitutionalDepartmentName to series table
                        Added extra warning when revision 16 not detected
                        moved Stationname and InstitutionalDepartmentName in view/edit database
                        Version to 1.4.13a
20080126        mvh     Automatic add backslash to read MAGDEVICEs etc and give a warning message
20080322        mvh     Fix first install bug in test dicom.sql; fix send image crash
                        in UID mode; display selected UIDs in query/move page
                        added PadAEWithZeros and IgnoreOutOfMemoryErrors
20080818        mvh     Database revision 17: extended size of EchoNumber and ReqProcDescription to 64
                        EchoNumber, StudyModality and ImageType are DT_MSTR: multiple values
                        use Institution (8,80) instead of InstitutionDepartmentname (8, 1040)
                        Added WorkListReturnsISO_IR_100
20080819        mvh     version to 1.4.14
20080831	mvh	Default ForwardAssociationRelease to 1
20080902	mvh	Added EnableComputedFields, default 0
20081104	mvh	Fixed install as four NT services: trim IP port before convert to int
                        Fixed save of database name to dicom.ini for mysql install
                        version to 1.4.14a
20081121	mvh	Added Postgres support; attempt fix MySQL support
                        Automatically choose the 64 bit dgate if possible
20081123	mvh	Retry server test for 4 sec after db install
20090203        mvh     Trim data read from dicom.ini to remove trailing spaces
20090205        mvh     Added MailPort option
20090411        mvh     version to 1.4.15alpha;
                        Added PrintSquareLandscape, max converters to 19,
                        VirtualPerSeries0..9, RetrieveResultConverter0,
                        QueryConverter0, WorkListQueryConverter0, RetrieveConverter0
                        Let OCX registration fail without warning (for vista)
20090928        mvh     Optionally use 7za.exe instead of zipmaster (when ZipDLL.DLL deleted)
                        Added RetryForwardFailed
20090928        mvh     Default sql password to conquest1415; 
20090929        mvh     Added ImportExportDragAndDrop flag
20090929        mvh     version to 1.4.15
20100202        mvh     version to 1.4.16alpha; check other GUI running; jpeg2000 stuff; LossyQuality
20100206        mvh     use convert_to_bmp in browser to allow JPEG2000
20100207        mvh     export servertask; added spinedit for frames; UseKpacsDecompression flag
20100207        mvh     version to 1.4.16alpha2; made it a const; removed warnings
                        write zips to folder logs if exists; create logs folder on install
20100227        mvh     version to 1.4.16alpha3; added DICOM print to selected AE
20100827        mvh     version to 1.4.16beta; Fixed display of jpeg2000 config in GUI
                        LargeFileSizeKB to 4096; note: jpeg2000 requires HAVE_J2K in cqdicom.dll
                        Fixed viewincoming; fixed enable NJ mode choice logic; fixed service
                        detection to warn for existing but not running service
20100828        mvh     Automatically start installed but not running service when GUI comes up
                        Added QueryResultConverter and ModalityWorkListQueryResultConverter
20100905        mvh     Added UseBuiltInJPEG and IgnoreMagdevicethreshold flags
20101003        mvh     Added browsepatient command; show record counts in browser hints
                        Fixed frame number passed to BMP generator
                        Improved query page text selection when brackets in text
20101003        mvh     Version to 1.4.16beta4
20101110        mvh     Typo in reading UseBuiltInJPEG (save will always write 1)
20101111        mvh     Version to 1.4.16beta6; added tests to service control
                        Issue: uninstall often fails (and leaves dgate.exe running)
20101116        mvh     Fixed uninstall in dgateserv, changed messages
                        More checks in service code
20101122        mvh     Add option to log to file only with gui running
                        Added export to zip file options
20101123        mvh     Default EnableComputedFields to 1, added OverlapVirtualGet, 
			ArchiveConverter0, update acrnema.m ap header with ui, ul, ue
20101212        mvh     Right lock new checkbox; version to 1.4.16
20101212        mvh     Disabled BigEndianExplicit for new; default file type to DCM
20101212        mvh     Removed nop export script
20101213        mvh     Enable delete even if no file found;
                        stop zipping and cleanup (dgate will do it) if only log to file enabled
20101213        mvh     Fixed update of grid hints for totals: note count = unfiltered
20101220        mvh     Block spinedit for 1 frame image
20101227        mvh     Reset CheckBoxOnlyLogToFile when restarting server etc
20110105        mvh	version to 1.4.16rc1; default BrowseThroughDBF=1 for SQLSERVER
20110123        mvh	version to 1.4.16rc3; other support for other sections in dicom.ini
20110326        mvh	version to 1.4.16rc6; Also show incoming 'Rewritten' images
20110329	mvh	Version to 1.4.16; added MAGDeviceFullThreshHold
20110331	mvh	Added NoDICOMCheck; release date
20120818	mvh	Padd text queries with spaces not nulls; fix setting JL compression
                        Increases convert_to_bmp resolution for print jobs and bmp creation
                        Display some info in the query window on startup; delayed GUI test
                        a bit; Version to 1.4.17
20120826	mvh	Accept server messages without \n; backup Failure files when deleting
20120912	mvh	Fixed tab order of all server tabsheets
20121016	mvh	Document J3NN .. JLNN in acrnema.map; version to 1.4.17alpha & release date
20121212	mvh	1.4.17beta
20130517	mvh	Fixed anonymizer; 1.4.17
20130517	mvh	DoubleBackSlashToDB = 1 could happen when installing MS SQL server
20130522	mvh	Added WorkListTableName to dicom.ini
20130523	mvh	Some more anonymizer work
20130813	mvh	Choose .dcm extension radiobutton for lua file name syntax
			Added AllowEmptyPatientID to dicom.ini
			Added missing converters to dicom.ini code
20130813	mvh	Version to 1.4.17b, release date
20131013	mvh	Version to 1.4.17c, release date
20131123	mvh	Fix uid based image query, broken due to 20120818 fix
20131219	mvh     Fix mailer when weekly checks config is absent
20140304	mvh     Added W* wildcard AE
20140309	mvh     Updated dgatesop.lst to revision 5
20140618	mvh     Updated dgatesop.lst to revision 6
20140618	mvh	Version to 1.4.17e, release date, LargeFileSizeKB default to 100000
20141102	mvh	Only set DcmSetProgressHook on DcmMove, reset after use - needed for latest cqdicom
20150718	mvh	Adapted for Delphi XE8, removed DBE and slow directory tree selector
20150718	mvh	Create custom controls runtime, removed zipmaster, direct sqlite and mysql access
20150719	mvh	1.4.19alpha
20150730	mvh	reorganized menu for clarity; hide "force display of large image" if not needed;
			added advanced modify images dialog
20160306	mvh	Added image commands to modifier and delete; hide refresh command and animation menu
			Added JPEGLS options (bug: quality not yet saved; default j7 quality is lossless)
			BrowsethroughDBF now default on; simplify dicom.ini by not writing defaults
			Navigating series removes image; click series to display
20160307        mvh     Added testmode const: 0 is normal 1 is show servertask commands with optional execute
			Version to 1.4.19beta and builddate; new Anonymizer at 4 levels using modifier: command
			new ChangePatid at 4 levels usign modifier: command
			new ExportAnonymousZIP at 4 levels; new DICOM move at four levels
20160308        mvh     Greatly improved testmode; used it to fix move: call; UseKpacsDecompression default off
                        Enable/disable menu items according to image visibility in browser
                        encompass servercommands by luastart:servercommand[[..]] to run in background
                        Enable JpegLS in dgatesop.lst and document in acrnema.map header
                        Removed MAG only warnings; fix modify patient ID script syntax
20160309        mvh     clear progress data when read fails; wait until database file is ready to avoid 'image not found'
                        Added refresh logic (after progressbar disappears)
20160310        mvh     Show **working** and #images; go back to this/next record at refresh
20160312        mvh     Show DICOM file in explorer; ImportExportDragAndDrop default to 1
                        Updated hints; click image now shows imagea and set focus to image list for wheel
                        retry fixes failed to find image on first opening browser; beware for deadlock
                        Filter patients on ID using extract:1 command; test on 15000 pat list
                        Removed BrowseThroughDBF (always on)
                        Keyboard shortcuts F2..F9; pgup/pgdown/num+/num-; Show study/series sequence number
20160313        mvh     Removed commented code; show keyboard shortcuts; F2=KPACS viewer / F11=show database
20160314        mvh     Added some code to reduce flickering of image; Label25 shows slice / total
                        Show n/total for series and studies; disable modify - test syntax when no image shown
                        Left mouse slices through multiframe object; hint on spinedit
20160315        mvh     Code review, reverted sockets to Recieve; jpeg2000 setting read fix
20160316        mvh     1.4.19beta; allow optional scripted bitmap conversion; uncompress on export; experimental sort
20160317        mvh     Use new todbf: command instead of extract for more control
                        Added sorting on/off by clicking column headers; limit patlist to 10000
                        Disable ODBC button for SQLITE and DBASEIII; warning hint for ODBC on win64
20160318        mvh     Show sorting in table hint; fix zip decompress scripts
20160319        mvh     Move curdir to tempdir (make sure no dbfs there)
                        Delete temp files when done with them
                        Install method; copy files from install32/64 when dgate/64 not found
20160319        mvh     1.4.19beta release
20160717        mvh     Change patient ID option now leaves patient name the same
                        Made border of install page wider
20160827        mvh     Allow larger film sizes in printer; list defined in gui: SendSelectedForm.ComboBox1
			Save as bitmap also does JPG and GIF (GIF is animated for multiframe)
			Protect browser when dgate crashed: test dbf files really appear
			Mouse up/down scrolls through slices; left right through frames
			Fix sorting for Postgres; coded running server test on install but does not work
			Supress most browser messages in server; removed some compiler warnings
			Fix display of jukebox number in browser
			1.4.19beta3 release
20161120        mvh     Find missing patients also now works from study root level
                        Disable progress bar when debug log is enabled
                        Added code to update UIDMODS table in verify database installation button
			Version to 1.4.19
20170208	mvh	Removed ADO components; prompt ID/newid as filename when zipping
                        Block test database does frame table update until NewInstallDone
                        Release 1.4.19
20170419	mvh	Fix refresh levels 2 and 3 (reversed table refreshes), added merge study, fixed merge series, 	version 1.4.19a
20170430	mvh	Release 1.4.19a
20170405        mvh     Default install fix for sqlite
20170729        mvh     Massimo: added missing " at end of dicom.sql
20170922        mvh     Added headerbmp, footerbmp, backgroundbmp options for printer
20171001        mvh     Made them work; version to 1.4.19b
20180204        mvh     Added checkbox to start/stop/show Ladle webserver; version to 1.4.19c
20180315        mvh     Added C-GET to dgatesop.lst generator
20180317        mvh     Only show webserver button if ladle.lua exists
20181110        mvh     Updated version numbers of dgatesop.lst and dicom.sql
20181111        mvh     Set build date to today
20181115	mvh	Added RetryForwardRemoteDICOMError
20181125	mvh	Added $ QUITGUI $, $ RELOADACRNEMA $, $ RELOADSETTINGS $ commands
20181211	mvh	Copy dgate.exe to cgi-bin and newweb folders to reduce zip size
20181214	mvh	Update dicom.ini and acrnema.map in cgi-bin and cgi-bin/newweb
                        Put AE and port of server self in acrnema.map when saving config
                        Version to 1.4.19d
20181216	mvh     Use lua code for query and move to reduce dependency on CQDICOM.DLL
20181217	mvh     Also do this for find missing patients; fix modality worklist query
                        remove studyroot query items (now used default)
                        Added error handling in lua queries
20181218	mvh     Added simple error handling to lua moves
                        Added lua code for dicom printing
                        Set print bitmaps defaults to print_header.bmp print_footer.bmp and print_background.bmp
20181218	mvh     Added KPACS define; if not set use webviewer on Ladle instead
20181219	mvh     Fixed lua code of find missing patients
20181220	mvh     Testing TCP for progress socket
20181221	mvh     Print through progress socket, avoiding missing images
20181227	mvh     Use KPACS define; use -x! for TCP progress socket;
                        fix process number display in progress socket
20181231	mvh     Version to 1.5.0-alpha
20190103	mvh     Typo and allow Progress while debug log on
20190103	mvh     Newweb no longer needs acrnema.map; removed cqdicom related code; unused variables
20190114	mvh     Remove writing of registry on startup
20190302	mvh     Fixed a small bug in that
20191011	mvh     Fixed anchor of internal web server button; Added LadlePort config
			Also nightly zip DicomAnonymized_Log\* files (wip)
20191019	mvh     Cast for compile issue in xe8, version to 1.5.0alpha-t4
20191025	mvh     Fix progress bar (ignore empty messages), disable ProgressActive counter; increase size of visual log
20191215	mvh     Fix ladle port default; version to 1.5.0beta
20200110        mvh     Db revision 19; use QRows and QColumns as field names
20200203        mvh     Copy lua5.1.dll to web server; list time of lua5.1.dll in bugreport; version 1.5.beta2
20200204        mvh     Small fix in that for newweb
20200311        mvh     Version to 1.5.0beta4
20200314        mvh     Version to 1.5.0; jpeg support renamed and default ON during install
20200704        mvh     Version to 1.5.0a; Fix case of database table names
20201002        mvh     Version to 1.5.0b; Fix DiskFreeGB()*1024
                        Fix ALTER TABLE UIDMODS syntax; comparison in find local missing was reversed
20201101        mvh     Added optimisation of UIDMODS index under verify database button
20210203        mvh     Reduce updates of serverstatus using stringList and writeMemoSl
20210204        mvh     Make sure stringlist exist; consistently base all files on curdir
20210206        mvh     Verify database makes joint DICOMTables indices, fix UIDmods check for sql server
                        Small change in logic of updateTime; also call .Update on Memo
                        Fix setting of debuglevel; onClick comes before change; also print value in log
20210208        mvh     Keep lastUpdate and lastWrite time to make it work
20210210        mvh     Blocked out debug update source display, fixed race condition in timer
20210510        mvh     Remove sequence query results prior to display 
20210620        mvh     Added regen folder button, moved done message of regen device to correct page
20210621        mvh     Alt-regen folder load text file with one folder name per line; push again to stop
20210630        mvh     No missing \ alert on linux, GetTempDir adjusted for linux (wip)
20211003        mvh     Added GuiToServerFilename to enable mix OS running (wip)
20211010        mvh     Merged Luiz extra controls for move to MAGx (but simplified)
20220828        mvh     Fix run webviewer from browser; write port into 3 config.php
                        file at install; copy dgate.exe to newweb app; update build date
                        change case of Logs folder to logs
20220829        mvh     Allow threshold and treshhold in config files, write with correct spelling
20220912        mvh     Set ladle webroot to Global.BaseDir..[[webserver/htdocs/]]
20220912        mvh     Added popupmenu to choose app when right-clicking built-in webserver
20220919        mvh     Right-click bugreport buttons shows server folder
20221031        mvh     Enable jpeg on startup to sync with checkbox
20230607        mvh     Update version number
20230620        mvh     Set UTF8FromDB and UTF8ToDB for postgres

Todo for odbc: dgate64 -v "-sSQL Server;DSN=conquest;Description=bla;Server=.\SQLEXPRESS;Database=conquest;Trusted_Connection=Yes"
Update -e command

Todo: littlendianexplicit to cqdicom
      Browser blinking 4x during page change to browser - wont fix for now

ENDOFUPDATEHISTORY
*}

// {$DEFINE KPACS}

{************************************************************************}
{*                         MODULE DOCUMENTATION                         *}
{************************************************************************}

{************************************************************************}
{*                          START CODE AND USES                         *}
{************************************************************************}

unit Serverdriver;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, Mask, ComCtrls, OleCtrls, FileCtrl, ExtCtrls, ActiveX,
  Db, Grids, DBGrids, DBCtrls, Menus, Launch, WSocket, Winapi.WinSock,
  Buttons, Registry, shellapi, SmtpProt, MDBFTable, ADODB,
  Spin, System.UITypes;

{************************************************************************}
{*                              CONSTANTS                               *}
{************************************************************************}

const VERSION = '1.5.0d';
const BUILDDATE = '20230631';
const testmode = 0;

{************************************************************************}
{*                               TYPES                                  *}
{************************************************************************}

procedure SplitAE(AE: AnsiString; var AEName, IPaddress, Port: AnsiString);

procedure ServerTask(text, args: string);

// function in CQDICOM.DLL
function DcmEcho(LocalAE, RemoteAE, RemoteIP, RemotePort, NKIcommand1,
                   NKIcommand2: ansistring):integer; stdcall;
                   external 'CqDicom.dll' name 'DcmEcho'

{************************************}

type
  TForm1 = class(TForm)
    PageControl1: TPageControl;
    TabSheet1: TTabSheet;
    TabSheet2: TTabSheet;
    TabSheet3: TTabSheet;
    TabSheet4: TTabSheet;
    TabSheet5: TTabSheet;
    TabSheet6: TTabSheet;
    Panel1: TPanel;
    ReInitDatabaseButton: TButton;
    Panel2: TPanel;
    Panel3: TPanel;
    Panel4: TPanel;
    Panel5: TPanel;
    Panel6: TPanel;
    DataSource1: TDataSource;
    DataSource2: TDataSource;
    DataSource3: TDataSource;
    DataSource4: TDataSource;
    DBGrid1: TDBGrid;
    DBGrid2: TDBGrid;
    Label4: TLabel;
    Label1: TLabel;
    ServerName: TMaskEdit;
    Label2: TLabel;
    TCPIPport: TMaskEdit;
    Label3: TLabel;
    Label5: TLabel;
    SaveConfigButton: TButton;
    RestoreconfigButton: TButton;
    VerifyTCPIPButton: TButton;
    VerifyDatabaseButton: TButton;
    ServerStatusMemo: TMemo;
    MaintenanceMemo: TMemo;
    InstallationMemo: TMemo;
    MakeODBCButton: TButton;
    UpdateODBCButton: TButton;
    ShowDeviceButton: TButton;
    ListDICOMprovidersButton: TButton;
    ListDatabaseLayioutButton: TButton;
    DICOMMap: TMemo;
    Button1: TButton;
    RestoreListButton: TButton;
    DBNavigator1: TDBNavigator;
    Label6: TLabel;
    DBNavigator2: TDBNavigator;
    Label7: TLabel;
    Label8: TLabel;
    DBNavigator3: TDBNavigator;
    Label9: TLabel;
    DBNavigator4: TDBNavigator;
    DBGrid3: TDBGrid;
    Label10: TLabel;
    InitDatabaseButton: TButton;
    About1: TButton;
    Close1: TButton;
    About2: TButton;
    Close2: TButton;
    Button2: TButton;
    Button3: TButton;
    Button4: TButton;
    Button5: TButton;
    Button6: TButton;
    Button7: TButton;
    RunInThreadedMode: TButton;
    KillAndRestartTheServer: TButton;
    Image1: TImage;
    PopupMenu1: TPopupMenu;
    SaveasBitmap1: TMenuItem;
    SaveDialog1: TSaveDialog;
    TabSheet7: TTabSheet;
    Label11: TLabel;
    Label12: TLabel;
    Label13: TLabel;
    Label14: TLabel;
    EditQID: TEdit;
    Panel7: TPanel;
    Memo1: TMemo;
    EditQName: TEdit;
    EditQdate: TEdit;
    EditQNum: TEdit;
    DicomSystem: TComboBox;
    Label15: TLabel;
    QueryButton: TButton;
    Label16: TLabel;
    TargetSystem: TComboBox;
    MoveButton: TButton;
    Label17: TLabel;
    QueryLevel: TComboBox;
    Label18: TLabel;
    EditQIm: TEdit;
    Label19: TLabel;
    MagThreshold: TEdit;
    DumpHeader1: TMenuItem;
    N2: TMenuItem;
    ButtonClearDatabase: TButton;
    HideButton: TButton;
    Button8: TButton;
    Button9: TButton;
    Button10: TButton;
    Button11: TButton;
    LabelLister: TStaticText;
    DBLookupComboBox3: TDBLookupComboBox;
    DBLookupComboBox4: TDBLookupComboBox;
    DBLookupComboBox2: TDBLookupComboBox;
    DBText1: TDBText;
    ComboBox1: TComboBox;
    Timer1: TTimer;
    TabSheet8: TTabSheet;
    ArchiveMemo: TMemo;
    Panel8: TPanel;
    ActiveJukeboxLabel: TLabel;
    JukeBoxPathLabel: TLabel;
    NumberCDsLabel: TLabel;
    NextCDToBurnLabel: TLabel;
    CDCapacityMBLabel: TLabel;
    BurnThresholdLabel: TLabel;
    BurnTimeLabel: TLabel;
    BurnProgramLabel: TLabel;
    Label27: TLabel;
    Timer2: TTimer;
    StaticTextTime: TStaticText;
    RegenSelectedButton: TButton;
    Panel9: TPanel;
    SelectForArchivingButton: TButton;
    UndoArchivingButton: TButton;
    CreateFileSetForBurningButton: TButton;
    BurnCDButton: TButton;
    VerifyCDButton: TButton;
    VerifyAndDeleteButton: TButton;
    Label48: TLabel;
    SpeedButton1: TSpeedButton;
    Button12: TButton;
    Button13: TButton;
    Button14: TButton;
    InstallAsService: TButton;
    UninstallServerAsService: TButton;
    Removeimagefromdatabase1: TMenuItem;
    Runexternalviewer1: TMenuItem;
    N3: TMenuItem;
    EditDatabasebyhand1: TMenuItem;
    CheckBoxViewIncoming: TCheckBox;
    VerifyMirrorDisk: TButton;
    Timer4: TTimer;
    CheckBoxKeepAlive: TCheckBox;
    MirrorVerifyTimeLabel: TLabel;
    TapeBackupPage: TTabSheet;
    Memo2: TMemo;
    StaticTextTapeTime: TStaticText;
    BlankTape: TButton;
    OverwriteTape: TButton;
    Panel10: TPanel;
    TapeDeviceLabel: TLabel;
    TapeCapacityMBLabel: TLabel;
    Label29: TLabel;
    Timer5: TTimer;
    BackupTimeLabel: TLabel;
    UsedTapeMBLabel: TLabel;
    BackupToTapeButton: TButton;
    WeeklyChecksPage: TTabSheet;
    WeeklyMemo: TMemo;
    CheckBox1: TCheckBox;
    CheckBox2: TCheckBox;
    CheckBox3: TCheckBox;
    CheckBox4: TCheckBox;
    CheckBox6: TCheckBox;
    CheckBox5: TCheckBox;
    CheckBox7: TCheckBox;
    Label20: TLabel;
    Label21: TLabel;
    Label22: TLabel;
    Label23: TLabel;
    Label24: TLabel;
    Label28: TLabel;
    Label30: TLabel;
    ComboBox2: TComboBox;
    Edit1: TEdit;
    Label32: TLabel;
    Edit2: TEdit;
    Label31: TLabel;
    Label33: TLabel;
    CheckBox8: TCheckBox;
    CheckBox9: TCheckBox;
    CheckBox10: TCheckBox;
    CheckBox11: TCheckBox;
    CheckBox12: TCheckBox;
    CheckBox13: TCheckBox;
    CheckBox14: TCheckBox;
    ComboBox3: TComboBox;
    Edit3: TEdit;
    Edit4: TEdit;
    CheckBox15: TCheckBox;
    CheckBox16: TCheckBox;
    CheckBox17: TCheckBox;
    CheckBox18: TCheckBox;
    CheckBox19: TCheckBox;
    CheckBox20: TCheckBox;
    CheckBox21: TCheckBox;
    ComboBox4: TComboBox;
    Edit5: TEdit;
    Edit6: TEdit;
    CheckBox22: TCheckBox;
    CheckBox23: TCheckBox;
    CheckBox24: TCheckBox;
    CheckBox25: TCheckBox;
    CheckBox26: TCheckBox;
    CheckBox27: TCheckBox;
    CheckBox28: TCheckBox;
    ComboBox5: TComboBox;
    Edit7: TEdit;
    Edit8: TEdit;
    CheckBox29: TCheckBox;
    CheckBox30: TCheckBox;
    CheckBox31: TCheckBox;
    CheckBox32: TCheckBox;
    CheckBox33: TCheckBox;
    CheckBox34: TCheckBox;
    CheckBox35: TCheckBox;
    ComboBox6: TComboBox;
    Edit9: TEdit;
    Edit10: TEdit;
    CheckBox36: TCheckBox;
    CheckBox37: TCheckBox;
    CheckBox38: TCheckBox;
    CheckBox39: TCheckBox;
    CheckBox40: TCheckBox;
    CheckBox41: TCheckBox;
    CheckBox42: TCheckBox;
    ComboBox7: TComboBox;
    Edit11: TEdit;
    Edit12: TEdit;
    CheckBox43: TCheckBox;
    CheckBox44: TCheckBox;
    CheckBox45: TCheckBox;
    CheckBox46: TCheckBox;
    CheckBox47: TCheckBox;
    CheckBox48: TCheckBox;
    CheckBox49: TCheckBox;
    ComboBox8: TComboBox;
    Edit13: TEdit;
    Edit14: TEdit;
    CheckBox50: TCheckBox;
    CheckBox51: TCheckBox;
    CheckBox52: TCheckBox;
    CheckBox53: TCheckBox;
    CheckBox54: TCheckBox;
    CheckBox55: TCheckBox;
    CheckBox56: TCheckBox;
    ComboBox9: TComboBox;
    Edit15: TEdit;
    Edit16: TEdit;
    CheckBox57: TCheckBox;
    CheckBox58: TCheckBox;
    CheckBox59: TCheckBox;
    CheckBox60: TCheckBox;
    CheckBox61: TCheckBox;
    CheckBox62: TCheckBox;
    CheckBox63: TCheckBox;
    ComboBox10: TComboBox;
    Edit17: TEdit;
    Edit18: TEdit;
    CheckBox64: TCheckBox;
    CheckBox65: TCheckBox;
    CheckBox66: TCheckBox;
    CheckBox67: TCheckBox;
    CheckBox68: TCheckBox;
    CheckBox69: TCheckBox;
    CheckBox70: TCheckBox;
    ComboBox11: TComboBox;
    Edit19: TEdit;
    Edit20: TEdit;
    StaticTextWeeklyCheck: TStaticText;
    WeeklyCheckTimer: TTimer;
    ButtonFindMissingPatients: TButton;
    PrinterTimer: TTimer;
    ProgressBar1: TProgressBar;
    N5: TMenuItem;
    PrintImage1: TMenuItem;
    PrintSelectedImages1: TMenuItem;
    Panel11: TPanel;
    CompressNKIButton: TRadioButton;
    CompressLosslessJPEGButton: TRadioButton;
    CompressLossyJPEGButton: TRadioButton;
    UncompressedButton: TRadioButton;
    LabelCompression: TLabel;
    EnableJPEGCheckBox: TCheckBox;
    CheckBoxSelectForArchiving: TCheckBox;
    CheckBoxVerifyMirrorDisk: TCheckBox;
    CheckBoxCreateFileSet: TCheckBox;
    CheckBoxTapeBackup: TCheckBox;
    CheckBoxBurnCD: TCheckBox;
    CheckBoxVerifyCDAndDeleteFileSet: TCheckBox;
    NextSlotToUseLabel: TLabel;
    Label35: TLabel;
    NightlyTreshold: TEdit;
    EditNightlyMoveTreshold: TEdit;
    Label36: TLabel;
    Label37: TLabel;
    ComboBoxMoveTarget: TComboBox;
    ButtonBugReport: TButton;
    MergeSelectedSeries1: TMenuItem;
    SplitSeries1: TMenuItem;
    ModeNJRadioButton: TRadioButton;
    CheckBoxDebugLog: TCheckBox;
    CheckBoxSmallFonts: TCheckBox;
    ButtonClearWorkList: TButton;
    ButtonPackDatabase: TButton;
    RunKPacsviewer1: TMenuItem;
    Label38: TLabel;
    EditQSDesc: TEdit;
    UpDownDebugLevel: TUpDown;
    Panel12: TPanel;
    LabelImageNaming: TLabel;
    DCMRadioButton: TRadioButton;
    V2RadioButton: TRadioButton;
    ModeUJRadioButton: TRadioButton;
    CompressLosslessJPEG2000Button: TRadioButton;
    CompressLossyJPEG2000Button: TRadioButton;
    SpinEdit1: TSpinEdit;
    DICOMPrintSelectedImagesTo: TMenuItem;
    CheckBoxOnlyLogToFile: TCheckBox;
    Export1: TMenuItem;
    Patient1: TMenuItem;
    Study1: TMenuItem;
    Series1: TMenuItem;
    Image2: TMenuItem;
    Send1: TMenuItem;
    SendPatientTo: TMenuItem;
    SendStudyTo: TMenuItem;
    SendSeriesTo: TMenuItem;
    SendImagesTo: TMenuItem;
    Delete: TMenuItem;
    Deletethispatient1: TMenuItem;
    DeletethisStudy: TMenuItem;
    DeletethisSeries: TMenuItem;
    ModifyMenu: TMenuItem;
    ModifyPatient: TMenuItem;
    ModifyStudy: TMenuItem;
    ModifySeries: TMenuItem;
    Print1: TMenuItem;
    Anonymize1: TMenuItem;
    ChangePatientID1: TMenuItem;
    N1: TMenuItem;
    N6: TMenuItem;
    N7: TMenuItem;
    ProgressBar2: TProgressBar;
    CompressLosslessJPEGLSButton: TRadioButton;
    CompressLossyJPEGLSButton: TRadioButton;
    ModifySOP: TMenuItem;
    DeleteImage: TMenuItem;
    AnonymizePatient: TMenuItem;
    AnonymizeStudy: TMenuItem;
    AnonymizeSeries: TMenuItem;
    AnonymizeImage: TMenuItem;
    ChangePatidPatient: TMenuItem;
    ChangePatidStudy: TMenuItem;
    ChangePatidSeries: TMenuItem;
    ChangePatidImage: TMenuItem;
    ExportAnomymizedtoZIPfile1: TMenuItem;
    ExportAnonymousZIPPatient: TMenuItem;
    ExportAnonymousZIPStudy: TMenuItem;
    ExportAnonymousZIPSeries: TMenuItem;
    ExportAnonymousZIPImage: TMenuItem;
    SendImageTo: TMenuItem;
    ShowDICOMfilesinexplorer1: TMenuItem;
    LabelStudySeq: TLabel;
    LabelSeriesSequence: TLabel;
    Label25: TLabel;
    MenuMergeStudies: TMenuItem;
    ProgressBar3: TProgressBar;
    CheckBoxWebServer: TCheckBox;
    ButtonRegenFolder: TButton;
    OpenDialog1: TOpenDialog;
    btnDoItNow: TButton;
    NightlyStrTimeToMoveLabel: TLabel;
    NightlyStrTimeToMoveText: TMaskEdit;
    PopupMenu2: TPopupMenu;
    procedure FormCreate(Sender: TObject);
    procedure RestoreconfigButtonClick(Sender: TObject);
    procedure SaveConfigButtonClick(Sender: TObject);
    procedure PageControl1Change(Sender: TObject);
    procedure ReInitDatabaseButtonClick(Sender: TObject);
    procedure VerifyTCPIPButtonClick(Sender: TObject);
    procedure VerifyDatabaseButtonClick(Sender: TObject);
    procedure DataSource4DataChange(Sender: TObject; Field: TField);
    procedure MakeODBCButtonClick(Sender: TObject);
    procedure ShowDeviceButtonClick(Sender: TObject);
    procedure ListDICOMprovidersButtonClick(Sender: TObject);
    procedure ListDatabaseLayioutButtonClick(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure RestoreListButtonClick(Sender: TObject);
    procedure InitDatabaseButtonClick(Sender: TObject);
    procedure Close1Click(Sender: TObject);
    procedure About1Click(Sender: TObject);
    procedure FormCloseQuery(Sender: TObject; var CanClose: Boolean);
    procedure RunInThreadedModeClick(Sender: TObject);
    procedure KillAndRestartTheServerClick(Sender: TObject);
    procedure ListenSocketSessionAvailable(Sender: TObject; Error: Word);
    procedure InstallationSocketDataAvailable(Sender: TObject;
      Error: Word);
    procedure MaintenanceSocketDataAvailable(Sender: TObject; Error: Word);
    procedure ServerStatusSocketDataAvailable(Sender: TObject;
      Error: Word);
    procedure ProgressSocketDataAvailable(Sender: TObject;
      Error: Word);
    procedure ProgressSocketSessionAvailable(Sender: TObject;
      Error: Word);
    procedure SaveasBitmap1Click(Sender: TObject);
    procedure Dumpheader1Click(Sender: TObject);
    procedure QueryButtonClick(Sender: TObject);
    procedure MoveButtonClick(Sender: TObject);
    procedure _Deletethispatient1Click(Sender: TObject);
    procedure Memo1DblClick(Sender: TObject);
    procedure ServerNameChange(Sender: TObject);
    procedure ButtonClearDatabaseClick(Sender: TObject);
    procedure ButtonClearWorklistClick(Sender: TObject);
    procedure TrayIcon1Click(Sender: TObject);
    procedure TrayIcon1RightClick(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure HideButtonClick(Sender: TObject);
    procedure ComboBox1KeyDown(Sender: TObject; var Key: Word;
      Shift: TShiftState);
    procedure ComboBox1Exit(Sender: TObject);
    procedure ComboBox1Change(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure RegenSelectedButtonClick(Sender: TObject);
    procedure SelectForArchivingButtonClick(Sender: TObject);
    procedure UndoArchivingButtonClick(Sender: TObject);
    procedure CreateFileSetForBurningButtonClick(Sender: TObject);
    procedure ArchiveSocketDataAvailable(Sender: TObject; Error: Word);
    procedure VerifyCDButtonClick(Sender: TObject);
    procedure VerifyAndDeleteButtonClick(Sender: TObject);
    procedure BurnCDButtonClick(Sender: TObject);
    procedure Timer2Timer(Sender: TObject);
    procedure InstallAsServiceClick(Sender: TObject);
    procedure UninstallServerAsServiceClick(Sender: TObject);
    function TestLocalServer(limited, nomessage: boolean): boolean;
    procedure NewPort(Socket: TWSocket; startport: string; endport: string);
    procedure Removeimagefromdatabase1Click(Sender: TObject);
    procedure Runexternalviewer1Click(Sender: TObject);
    procedure SendImageToClick(Sender: TObject);
    procedure OnceUp1Click(Sender: TObject);
    procedure OnceDown1Click(Sender: TObject);
    procedure EditDatabasebyhand1Click(Sender: TObject);
    procedure CheckBoxViewIncomingClick(Sender: TObject);
    procedure InstallAsServiceMouseDown(Sender: TObject;
      Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure UninstallServerAsServiceMouseDown(Sender: TObject;
      Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure KillAndRestartTheServerMouseDown(Sender: TObject;
      Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure VerifyMirrorDiskClick(Sender: TObject);
    procedure CheckBoxKeepAliveClick(Sender: TObject);
    procedure Timer4Timer(Sender: TObject);
    procedure FlushButtonClick(Sender: TObject);
    procedure BlankTapeClick(Sender: TObject);
    procedure OverwriteTapeClick(Sender: TObject);
    procedure Timer5Timer(Sender: TObject);
    procedure BackupToTapeButtonClick(Sender: TObject);
    procedure _DeleteThisSeriesClick(Sender: TObject);
    procedure CheckBox1Click(Sender: TObject);
    procedure Edit1Change(Sender: TObject);
    procedure ComboBox2Change(Sender: TObject);
    procedure Edit2Change(Sender: TObject);
    procedure WeeklyCheckTimerTimer(Sender: TObject);
    procedure WeeklySocketDataAvailable(Sender: TObject; Error: Word);
    procedure ButtonFindMissingPatientsClick(Sender: TObject);
    procedure PrinterTimerTimer(Sender: TObject);
    procedure PrintImage1Click(Sender: TObject);
    procedure PrintSelectedImages1Click(Sender: TObject);
    procedure DataSource1DataChange(Sender: TObject; Field: TField);
    procedure DataSource2DataChange(Sender: TObject; Field: TField);
    procedure DataSource3DataChange(Sender: TObject; Field: TField);
    procedure EnableJPEGCheckBoxClick(Sender: TObject);
    procedure Label27DblClick(Sender: TObject);
    procedure Edit1DblClick(Sender: TObject);
    procedure FormActivate(Sender: TObject);
    procedure ButtonBugReportClick(Sender: TObject);
    procedure MergeSelectedSeries1Click(Sender: TObject);
    procedure SplitSeries1Click(Sender: TObject);
    procedure CheckBoxDebugLogClick(Sender: TObject);
    procedure CheckBoxSmallFontsClick(Sender: TObject);
    procedure ButtonPackDatabaseClick(Sender: TObject);
    procedure RunKPacsviewer1Click(Sender: TObject);
    procedure UpDownDebugLevelClick(Sender: TObject; Button: TUDBtnType);
    procedure Label14DblClick(Sender: TObject);
    procedure LabelImageNamingDblClick(Sender: TObject);
    procedure V2RadioButtonClick(Sender: TObject);
    procedure DCMRadioButtonClick(Sender: TObject);
    procedure LabelCompressionDblClick(Sender: TObject);
    procedure SpinEdit1Change(Sender: TObject);
    procedure CheckBoxOnlyLogToFileClick(Sender: TObject);
    procedure Patient1Click(Sender: TObject);
    procedure Study1Click(Sender: TObject);
    procedure Series1Click(Sender: TObject);
    procedure Image2Click(Sender: TObject);
    procedure Label10Click(Sender: TObject);
    procedure ModifyPatientClick(Sender: TObject);
    procedure DBGrid2DblClick(Sender: TObject);
    procedure DBText1DblClick(Sender: TObject);
    procedure DBGrid1DblClick(Sender: TObject);
    procedure DBLookupComboBox2Click(Sender: TObject);
    procedure DeleteImageClick(Sender: TObject);
    procedure AnonymizePatientClick(Sender: TObject);
    procedure ChangePatidPatientClick(Sender: TObject);
    procedure ExportAnonymousZIPPatientClick(Sender: TObject);
    procedure SendPatientToClick(Sender: TObject);
    procedure Send1Click(Sender: TObject);
    procedure Export1Click(Sender: TObject);
    procedure ExportAnomymizedtoZIPfile1Click(Sender: TObject);
    procedure Print1Click(Sender: TObject);
    procedure ModifyMenuClick(Sender: TObject);
    procedure Anonymize1Click(Sender: TObject);
    procedure DeleteClick(Sender: TObject);
    procedure ChangePatientID1Click(Sender: TObject);
    procedure ShowPopupMenu1(Sender: TObject);
    procedure ShowDICOMfilesinexplorer1Click(Sender: TObject);
    procedure LabelListerClick(Sender: TObject);
    procedure Image1Click(Sender: TObject);
    procedure DBGrid1CellClick(Column: TColumn);
    procedure FormKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
    procedure Image1MouseEnter(Sender: TObject);
    procedure Image1MouseMove(Sender: TObject; Shift: TShiftState; X,
      Y: Integer);
    procedure DBGrid2TitleClick(Column: TColumn);
    procedure DBGrid1TitleClick(Column: TColumn);
    procedure DBGrid3TitleClick(Column: TColumn);
    procedure MenuMergeStudiesClick(Sender: TObject);
    procedure CheckBoxWebServerClick(Sender: TObject);
    procedure CheckBoxWebServerMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure ButtonRegenFolderClick(Sender: TObject);
    procedure ButtonRegenFolderMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure btnDoItNowClick(Sender: TObject);
    procedure ShowPopupMenu2(Sender: TObject; X, Y: integer);
    procedure AppClick(Sender: TObject);
    procedure ButtonBugReportMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
  private
    procedure WMDropFiles(var Message: TWMDropFiles); message WM_DROPFILES;
    procedure WMQueryEndSession(var Message: TWMQueryEndSession); message WM_QUERYENDSESSION;
    procedure CheckDicomSubmits;
    { Private declarations }
  public
    Table1, Table2, Table3, Table4 : TDataset;
    { Public declarations }
  end;

var
  Form1: TForm1;
  CurDir: string;
  PrinterQueue: TThreadList;
  BrowseThroughDBF: string;
  othersections: TStringList;

    InstallationSocket: TWSocket;
    MaintenanceSocket: TWSocket;
    ServerStatusSocket: TWSocket;
    ProgressSocket: TWSocket;
    ArchiveSocket: TWSocket;
    ListenSocket: TWSocket;
    WeeklySocket: TWSocket;
    TestSocket: TWSocket;
    MDBFTable1: TMDBFTable;
    MDBFTable2: TMDBFTable;
    MDBFTable3: TMDBFTable;
    MDBFTable4: TMDBFTable;
    Mailer: TSyncSmtpCli;
    TrayIcon1: TTrayIcon;

var
  ServerStatusStringList: TStringList;

dgateExe: string        = 'dgate.exe';

procedure FillAELists;

{************************************************************************}
{*                 EXPORTED AND INTERNAL GLOBAL VARIABLES               *}
{************************************************************************}

implementation

uses ServerAbout, HeaderLister, AboutNew, SendSelected, EditDatabaseForm, printers, syncobjs,
{$IFDEF KPACS}
  Uviewer,
{$ENDIF KPACS}
  ModifyDicom, IniFiles;

var DataDir, DataSource, SQLHost, SqlUser, SqlPassWord, MySql, FSqLite, FPostGres,
    FileNameSyntax, MaxFileNameLength, FixPhilips, FixKodak,
    DoubleBackSlashToDB, UseEscapeStringConstants, AllowEmptyPatientID, UTF8ToDB, UTF8FromDB: string;
var ExternalViewer, DemoViewer, DemoCopy: string;
var NewInstall, NewInstallDone: boolean;
var PreRunning: boolean;
var ThreadedProcess, ActiveProcess: integer;

var MagDeviceList: array[0..9] of string;
var MagDevices: integer;
var CacheDeviceList: array[0..9] of string;
var CacheDevices: integer;
var JukeboxDeviceList: array[0..9] of string;
var JukeboxDevices: integer;
var MirrorDeviceList: array[0..9] of string;
var MirrorDevices: integer;
var IgnoreMAGDeviceThreshold: string = '0';
var MAGDeviceFullThreshold: string = '30';

var VirtualServerForList: array[0..9] of string;
var VirtualPerSeries: array[0..9] of integer;
var OverlapVirtualGet: string = '0';

var ActiveJukebox, NumberCDs, CDCapacityMB, BurnThreshold, NextCDToBurn, NextSlotToUse, BurnWeekend: integer;
var BurnTime, BurnTime2, BurnProgram, ArchiveStatus, MirrorVerifyTime, TestReadTime: string;
var BurnProgramList: array[0..9] of string;

var ExportConverters: integer;
var ExportModalityList: array[0..19] of string;
var ExportConverterList: array[0..19] of string;
var ExportStationNameList: array[0..19] of string;
var ExportFilterList: array[0..19] of string;
var ExportCalledAEList: array[0..19] of string;
var ExportCallingAEList: array[0..19] of string;
var ImportConverters: integer = 999;
var ImportModalityList: array[0..19] of string;
var ImportConverterList: array[0..19] of string;
var ImportStationNameList: array[0..19] of string;
var ImportCalledAEList: array[0..19] of string;
var ImportCallingAEList: array[0..19] of string;

var QueryConverter: array[0..0] of string;
var WorkListQueryConverter: array[0..0] of string;
var RetrieveConverter: array[0..0] of string;
var RetrieveResultConverter: array[0..0] of string;
var QueryResultConverter: array[0..0] of string;
var ModalityWorkListQueryResultConverter: array[0..0] of string;
var MergeSeriesConverter: array[0..0] of string;
var MergeStudiesConverter: array[0..0] of string;
var ArchiveConverter: array[0..0] of string;
var MoveDeviceConverter: array[0..0] of string;
var RejectedImageConverter: array[0..0] of string;
var RejectedImageWorkListConverter: array[0..0] of string;
var VirtualServerQueryConverter: array[0..0] of string;
var VirtualServerQueryResultConverter: array[0..0] of string;

var NoConfirmQuit: boolean;
var KeepAlive: integer;
var UIDCOUNT: integer=0;
var LargeFileSizeKB: integer=100000;
var PrintSquareLandscape: integer=0;
var UseKpacsDecompression: integer=0;

var ForwardAssociationLevel: string;
var ForwardAssociationRefreshDelay, ForwardAssociationCloseDelay, ForwardAssociationRelease: integer;
var Prefetcher : string = '0';

var FileCompressMode: integer;                  // obsolete parameter
var DroppedFileCompression: string = 'un';      // uncompressed
var IncomingCompression:    string = 'un';
var ArchiveCompression:     string = 'as';      // as-is
var JPEGSupport:            boolean= true;	// from dgatesop.lst

// Luiz added at Sept 19 2021
var NightlyStrTimeToMove: string ='02:00';
var DoNightlyToMoveNow  : boolean=false;
// end add

var TapebackupStatus : string;
var TapeDevice : integer    = 0;
var TapeCapacityMB: integer = 2000;
var BackupTime: string      = 'disabled';
var UsedTapeMB: integer     = 0;
var glHoldFlush: boolean    = false;

var TapeDriveLetter, TapeControl: string;

var ZipTime : string        = '05:';
var UIDPrefix: string       = '';

var PatientQuerySortOrder: string = '';
var StudyQuerySortOrder:   string = '';
var SeriesQuerySortOrder:  string = '';
var ImageQuerySortOrder:   string = '';
var EnableComputedFields:  string = '1';

var EnableReadAheadThread: integer = 1;
var SendUpperCaseAE:       integer = 0;
var StorageFailedErrorCode:integer = 272;

var PackDBF:  string = '0';
var IndexDBF: string = '1';
var LongQueryDBF: string = '1000';

var TCPIPTimeOut: string = '300';
var FailHoldOff:  string = '60';
var RetryDelay:   string = '100';
var RetryForwardFailed: string = '0';
var ImportExportDragAndDrop: string = '1';
var QueueSize:    string = '128';
var WorkListMode: string = '0';
var WorkListReturnsISO_IR_100: string = '1';
var CacheVirtualData: string = '1';

var LRUSort: string = '';
var AllowTruncate: string = '';
var ForwardCollectDelay: string = '600';
var MaximumExportRetries: string = '0';
var MaximumDelayedFetchForwardRetries: string = '0';
var DecompressNon16BitsJpeg: string = '1';
var UseBuiltInJPEG: string = '1';
var LossyQuality: string = '95';
//var UseOpenJPEG: string = '0';
var IgnoreOutOfMemoryErrors: string = '0';
var NoDICOMCheck: string = '0';
var PadAEWithZeros: string = '0';
var RetryForwardRemoteDICOMError: string = '0';
var LadlePort: string = '8086';

var CheckDays: array[1..10] of integer;
    CheckTimes: array[1..10] of string;
    CheckActions: array[1..10] of string;
    CheckCDs: array[1..10] of integer;
    CheckActive: array[1..10] of boolean;
    CheckTriggered: array[1..10] of boolean;
    CheckReading: boolean;

    MailCollectTime: integer = 1;
    MailWaitTime: integer = 10;
    MailActive: boolean;

var SubmitCheckTime: string = '21:00';
    PacsAE: string          = '';
    SourceAEs: array[1..10] of string;

var headerbmpname: string = 'print_header.bmp';
    footerbmpname: string = 'print_footer.bmp';
    backgroundbmpname: string = 'print_background.bmp';

{************************************************************************}
{*                             MODULE FUNCTIONS                         *}
{************************************************************************}

{$R *.DFM}

// wait s seconds until file is ready for opening
function WaitForFile(filename: string; s: double): boolean;
var h : Integer;
    StartTime, WaitTime: Double;
begin
    StartTime := Time;
    repeat
      h := FileOpen(filename, fmShareExclusive);
      if h <= 0 then // MH
      begin
        Sleep(200);
      end;
      WaitTime := (Time-StartTime)*24*3600;
    until ((h>0) or (WaitTime>s));
    if h>0 then
      FileClose(h);
    Result := h>0;
end;


// callback memory allocator for CQDICOM.DLL; avoids problems
// with alloc with one memory manager and free with another.
function MyAlloc(size: integer): Pointer; stdcall;
begin
  GetMem(result, size);
end;

var DirSep : string = '\';

function GetTempDir: String;
var
  tempFolder: array[0..MAX_PATH] of Char;
begin
  GetTempPath(MAX_PATH, @tempFolder);
  result := StrPas(tempFolder);
end;


{************************************************************************}
// This routine starts the Dgate application with passed arguments
// returns the handle of the process so that it can be terminated later.

function RunDgate(args: string; keephandle: boolean): integer;
  var sinfo: tstartupInfo;
  var pinfo : TProcessInformation;
  s, p: string;
begin
  ZeroMemory(@sinfo, sizeof(TStartupInfo));
  ZeroMemory(@pinfo, sizeof(TProcessInformation));
  sinfo.cb      := sizeof(TStartupInfo);
  p := //ExtractFileDir(ParamStr(0))
    CurDir + '\' + dgateExe;
  s := '"' + p + '"' + ' ' + args;
  ChDir(curdir);
  if not CreateProcess(Pchar(p), Pchar(s), nil, nil, true,
                 DETACHED_PROCESS, nil, nil, sinfo, pinfo) then
  begin
    //ShowMessage('Failed to start process ' + p);
    result := 0;
  end
  else
    result := pinfo.hProcess;

  CloseHandle(pinfo.hThread);

  if not keephandle then
    CloseHandle(pinfo.hProcess);
end;

var MailStartTime, LastMailTime: double;

procedure MailMessage(s: string);
begin
  if mailer.HdrSubject='' then
  begin
    mailer.HdrSubject := s;
    MailStartTime := Now;
    mailer.MailMessage.Add('The following errors have occurred in DICOM server ' + trim(Form1.ServerName.text) + ' :' );
  end;

  mailer.MailMessage.Add(DateTimeToStr(Now) + ' ' + s);
end;

// write text to memo, file and mail problems
procedure WriteMemo(Memo: TMemo; s:string; maxlines: integer; minlines: integer; name: string; adddate : boolean = true);
var f: TextFile;
begin
  // display text in memo
  Memo.Lines.Add(s);
  memo.Update;

  // write text to file
  if Length(name)<>0 then
  begin
    AssignFile(f, //ExtractFileDir(ParamStr(0))
      CurDir + '\' + name + '.log');
    if FileExists(//ExtractFileDir(ParamStr(0))
      Curdir + '\' + name + '.log') then
      Append(f)
    else
      Rewrite(f);

    if adddate then
      writeln(f, DateToStr(Date) + ' ' + TimeToStr(Time) + ' ' + s)
    else
      writeln(f, s);

    CloseFile(f);
  end;

  // mail all server fail messages using weekly check mail config
  if ((Pos('fail', LowerCase(s))<>0) or (Pos('***', s)<>0)) and (Length(mailer.FromName)<>0) then
    MailMessage('DICOM problem: ' + s);

  // keep memo contents small
  if (Memo.Lines.count > maxlines) and not glHoldFlush then
  begin
    Memo.Lines.BeginUpdate;
    while Memo.Lines.count > minlines do
    begin
      Memo.Lines.Delete(0);
    end;
    Memo.SelStart := 1000 * minlines;
    Memo.Lines.EndUpdate;
  end;
end;

var lastUpdate: double = 0.0;
var lastWrite: double = 0.0;

// write text to memo, file and mail problems
procedure WriteMemoSl(Memo: TMemo; s:string; maxlines: integer; minlines: integer; name: string; adddate : boolean = true; sl: TStringList = nil);
var f: TextFile;
begin
  // display text in memo
  sl.Add(s);

  // write text to file
  if Length(name)<>0 then
  begin
    AssignFile(f, // ExtractFileDir(ParamStr(0))
      CurDir + '\' + name + '.log');
    if FileExists(//ExtractFileDir(ParamStr(0))
      CurDir + '\' + name + '.log') then
      Append(f)
    else
      Rewrite(f);

    if adddate then
      writeln(f, DateToStr(Date) + ' ' + TimeToStr(Time) + ' ' + s)
    else
      writeln(f, s);

    CloseFile(f);
  end;

  // mail all server fail messages using weekly check mail config
  if ((Pos('fail', LowerCase(s))<>0) or (Pos('***', s)<>0)) and (Length(mailer.FromName)<>0) then
    MailMessage('DICOM problem: ' + s);

  lastWrite := Time;

  // keep memo contents small
  if (sl.count > maxlines) then
  begin
    while sl.count > minlines do
    begin
      sl.Delete(0);
    end;
    //Application.MainForm.Caption := 'Scrollupdate :' + FloatToStr(lastUpdate);
    Memo.Text := Sl.Text;
    Memo.Perform(EM_LineScroll, 0, Memo.Lines.Count); {move to bottom}
    lastUpdate := Time;
  end
  else if lastWrite-lastUpdate > 0.1/(24*3600) then
  begin
    //Application.MainForm.Caption := 'Socketupdate :' + FloatToStr(lastUpdate);
    Memo.Text := Sl.Text;
    Memo.Perform(EM_LineScroll, 0, Memo.Lines.Count); {move to bottom}
    lastUpdate := Time;
  end;
end;

procedure ServerTask(text, args: string);
var i: integer;
    s: array[1..20] of string;
    f: textfile;
begin
  if testmode=0 then
    DcmEcho(trim(Form1.ServerName.text), trim(Form1.ServerName.text), '127.0.0.1', trim(Form1.TCPIPport.text), text, args);

  if testmode=1 then
  begin
    if (pos('extract:', args)>0) or (pos('todbf:', args)>0) or (pos('convert_to_bmp:', args)>0) then
      DcmEcho(trim(Form1.ServerName.text), trim(Form1.ServerName.text), '127.0.0.1', trim(Form1.TCPIPport.text), text, args)
    else
    begin
      Form1.LabelLister.Visible := false;
      form1.Image1.Canvas.Brush.Color := clGreen;
      form1.Image1.Canvas.FillRect(Rect(0,0,4000,100));
      form1.Image1.Canvas.Font.Height := 18;
      form1.Image1.Canvas.TextOut(0, 0, 'Testmode is active, showing patid, studyuid, seriesuid, sopuid');
      if form1.Table1.active then form1.Image1.Canvas.TextOut(0, 20, form1.Table1.FieldByName('PATIENTID' ).AsString);
      if form1.Table2.active then form1.Image1.Canvas.TextOut(0, 40, form1.Table2.FieldByName('STUDYINSTA' ).AsString);
      if form1.Table3.active then form1.Image1.Canvas.TextOut(0, 60, form1.Table3.FieldByName('SERIESINST' ).AsString);
      if form1.Table4.active then form1.Image1.Canvas.TextOut(0, 80, form1.Table4.FieldByName('SOPINSTANC' ).AsString);

      WriteMemoSl(form1.ServerStatusMemo, 'TestMode: ' + text, 1100, 1000, 'serverstatus', true, ServerStatusStringList);
      WriteMemoSl(form1.ServerStatusMemo, 'TestMode: ' + args, 1100, 1000, 'serverstatus', true, ServerStatusStringList);

      AssignFile(f, //ExtractFileDir(ParamStr(0))
        CurDir + '\testmode.txt');
      if FileExists(//(ParamStr(0))
        CurDir  + '\testmode.txt') then
        Append(f)
      else
        Rewrite(f);
      writeln(f, 'rem Testmode: ' + text);
      writeln(f, args);
      CloseFile(f);

      s[1] := copy(args, 1, 45);
      for i := 1 to Length(args) div 45 do
        s[i+1] := copy(args, i*45+1, 45);
      args := '';
      for i := 1 to 20 do
      begin
        if s[i]='' then break;
        args := args + #13 + s[i];
      end;
      if MessageDlg('Testmode, do: '+text+'?'+#13+args, mtConfirmation, [mbYes, mbNo], 0) = mrNo then exit;
        DcmEcho(trim(Form1.ServerName.text), trim(Form1.ServerName.text), '127.0.0.1', trim(Form1.TCPIPport.text), text, args)
    end;
  end
end;

{************************************************************************}
// Compute free disk space in MB

function DiskFreeGB(Drive: char): integer;
var SectorsPerCluster,	                      // sectors per cluster
    BytesPerSector,	                      // bytes per sector
    NumberOfFreeClusters,                     // number of free clusters
    TotalNumberOfClusters: DWORD; 	      // total number of clusters
    temp: double;
    Path: Array[0..10] of char;
begin
  Path[0] := char(Drive);
  Path[1] := Char(':');
  Path[2] := Char('\');
  Path[3] := Char(0);

  if GetDiskFreeSpace(Path, SectorsPerCluster, BytesPerSector,
                                NumberOfFreeClusters, TotalNumberOfClusters) then
  begin
    temp := (SectorsPerCluster/1024) * (BytesPerSector/1024) *
            (NumberOfFreeClusters/1024.0);
    result := trunc(temp);
  end
  else
    result := -1;
end;

function TForm1.TestLocalServer(limited, nomessage: boolean): boolean;
var resp: integer;
    text: string;
begin
  result := true;

  resp := 1;
  try
    resp := DcmEcho(trim(ServerName.text), trim(ServerName.text), '127.0.0.1', trim(TCPIPport.text), 'User interface test: local server is running!', 'silent');
  except
  end;
  case resp of
    0:  text := 'ok';
    1:  text := 'exception';
    7:  text := 'no connection' + #13 + 'Probable cause: NT service failed to start (check account and password)';
    12: text := 'association rejected' + #13 + 'Probable cause: database is not accessible';
    14: text := 'wrong response';
    20: text := 'remote error';
  end;

  if limited then
  begin
    if (resp<>0) and (resp<12) then
    begin
      if not nomessage then ShowMessage('The server is not running properly: ' + text);
      result := false;
    end;
  end
  else
  begin
    if resp<>0 then
    begin
      if not nomessage then ShowMessage('The server is not running properly: ' + text);
      result := false;
    end;
  end;
end;

// open a free udp port following startport but before endport

procedure TForm1.NewPort(Socket: TWSocket; startport: string; endport: string);
var i: integer;
begin
  for i:=StrToInt(startport)+1 to StrToInt(endport) do
  begin
    try
      // Socket.Proto:='udp';
      Socket.Addr:='127.0.0.1';
      Socket.Port:=IntToStr(i);
      Socket.Listen;
      exit;
    except
    end;
  end;

  ShowMessage('ConquestDicomServer: NewPort could not locate free port for ' + Socket.Name);
end;

// write text to file
procedure WriteLog(s: string; name: string = 'conquestdicomserver'; adddate: boolean = true);
var f: TextFile;
begin
  AssignFile(f, //ExtractFileDir(ParamStr(0))
    CurDir + '\' + name + '.log');
  if FileExists(// ExtractFileDir(ParamStr(0))
    CurDir + '\' + name + '.log') then
    Append(f)
  else
    Rewrite(f);

  if adddate then
    writeln(f, DateToStr(Date) + ' ' + TimeToStr(Time) + ' ' + s)
  else
    writeln(f, s);

  CloseFile(f);
end;

{************************************************************************}
// Form creation: read or create required information files; open tcp/ip sockets

procedure CreateDGATESOP_LST(JPEGSupport: boolean);
var f: textfile;
    s: string;
begin
  if JPEGSupport then s:='' else s:='#';

  // create the dgatesop.lst file
  AssignFile(f, CurDir + '\dgatesop.lst');
  Rewrite(f);

  writeln(f, '#');
  writeln(f, '# DICOM Application / sop / transfer UID list.');
  writeln(f, '#');
  writeln(f, '# This list is used by the CheckedPDU_Service ( "filename" ) service');
  writeln(f, '# class.  All incoming associations will be verified against this');
  writeln(f, '# file.');
  writeln(f, '#');
  writeln(f, '# Revision 2: disabled GEMRStorage and GECTStorage');
  writeln(f, '# Revision 3: extended with new sops and with JPEG transfer syntaxes');
  writeln(f, '# Revision 4: added Modality Worklist query');
  writeln(f, '# Revision 5: Erdenay added UIDS for CTImageStorageEnhanced, RawDataStorage, ');
  writeln(f, '#             SpatialRegistrationStorage, SpatialFiducialsStorage, DeformableSpatialRegistrationStorage, ');
  writeln(f, '#             SegmentationStorage, SurfaceSegmentationStorage, RTIonPlanStorage, RTIonBeamsTreatmentRecordStorage    ');
  writeln(f, '# Revision 6: X-RayRadiationDoseSR, BreastTomosynthesisImageStorage and others');
  writeln(f, '# Revision 7: JpegLS and BigEndianExplicit enabled');
  writeln(f, '# Revision 8: Added C-GET');
  writeln(f, '#');
  writeln(f, '#None				none				RemoteAE');
  writeln(f, '#None				none				LocalAE');
  writeln(f, '#DICOM				1.2.840.10008.3.1.1.1		application');
  writeln(f, 'Verification			1.2.840.10008.1.1		sop');


  writeln(f, 'RETIRED_StoredPrintStorage                           1.2.840.10008.5.1.1.27		sop');
  writeln(f, 'RETIRED_HardcopyGrayscaleImageStorage                1.2.840.10008.5.1.1.29		sop');
  writeln(f, 'RETIRED_HardcopyColorImageStorage                    1.2.840.10008.5.1.1.30		sop');
  writeln(f, 'ComputedRadiographyImageStorage                      1.2.840.10008.5.1.4.1.1.1		sop');
  writeln(f, 'DigitalXRayImageStorageForPresentation               1.2.840.10008.5.1.4.1.1.1.1		sop');
  writeln(f, 'DigitalXRayImageStorageForProcessing                 1.2.840.10008.5.1.4.1.1.1.1.1		sop');
  writeln(f, 'DigitalMammographyXRayImageStorageForPresentation    1.2.840.10008.5.1.4.1.1.1.2		sop');
  writeln(f, 'DigitalMammographyXRayImageStorageForProcessing      1.2.840.10008.5.1.4.1.1.1.2.1		sop');
  writeln(f, 'DigitalIntraOralXRayImageStorageForPresentation      1.2.840.10008.5.1.4.1.1.1.3		sop');
  writeln(f, 'DigitalIntraOralXRayImageStorageForProcessing        1.2.840.10008.5.1.4.1.1.1.3.1		sop');
  writeln(f, 'CTImageStorage                                       1.2.840.10008.5.1.4.1.1.2		sop');
  writeln(f, 'EnhancedCTImageStorage                               1.2.840.10008.5.1.4.1.1.2.1		sop');
  writeln(f, 'RETIRED_UltrasoundMultiframeImageStorage             1.2.840.10008.5.1.4.1.1.3		sop');
  writeln(f, 'UltrasoundMultiframeImageStorage                     1.2.840.10008.5.1.4.1.1.3.1		sop');
  writeln(f, 'MRImageStorage                                       1.2.840.10008.5.1.4.1.1.4		sop');
  writeln(f, 'EnhancedMRImageStorage                               1.2.840.10008.5.1.4.1.1.4.1		sop');
  writeln(f, 'MRSpectroscopyStorage                                1.2.840.10008.5.1.4.1.1.4.2		sop');
  writeln(f, 'EnhancedMRColorImageStorage                          1.2.840.10008.5.1.4.1.1.4.3		sop');
  writeln(f, 'RETIRED_NuclearMedicineImageStorage                  1.2.840.10008.5.1.4.1.1.5		sop');
  writeln(f, 'RETIRED_UltrasoundImageStorage                       1.2.840.10008.5.1.4.1.1.6		sop');
  writeln(f, 'UltrasoundImageStorage                               1.2.840.10008.5.1.4.1.1.6.1		sop');
  writeln(f, 'EnhancedUSVolumeStorage                              1.2.840.10008.5.1.4.1.1.6.2		sop');
  writeln(f, 'SecondaryCaptureImageStorage                         1.2.840.10008.5.1.4.1.1.7		sop');
  writeln(f, 'MultiframeSingleBitSecondaryCaptureImageStorage      1.2.840.10008.5.1.4.1.1.7.1		sop');
  writeln(f, 'MultiframeGrayscaleByteSecondaryCaptureImageStorage  1.2.840.10008.5.1.4.1.1.7.2		sop');
  writeln(f, 'MultiframeGrayscaleWordSecondaryCaptureImageStorage  1.2.840.10008.5.1.4.1.1.7.3		sop');
  writeln(f, 'MultiframeTrueColorSecondaryCaptureImageStorage      1.2.840.10008.5.1.4.1.1.7.4		sop');
  writeln(f, 'RETIRED_StandaloneOverlayStorage                     1.2.840.10008.5.1.4.1.1.8		sop');
  writeln(f, 'RETIRED_StandaloneCurveStorage                       1.2.840.10008.5.1.4.1.1.9		sop');
  writeln(f, 'DRAFT_WaveformStorage                                1.2.840.10008.5.1.4.1.1.9.1		sop');
  writeln(f, 'TwelveLeadECGWaveformStorage                         1.2.840.10008.5.1.4.1.1.9.1.1		sop');
  writeln(f, 'GeneralECGWaveformStorage                            1.2.840.10008.5.1.4.1.1.9.1.2		sop');
  writeln(f, 'AmbulatoryECGWaveformStorage                         1.2.840.10008.5.1.4.1.1.9.1.3		sop');
  writeln(f, 'HemodynamicWaveformStorage                           1.2.840.10008.5.1.4.1.1.9.2.1		sop');
  writeln(f, 'CardiacElectrophysiologyWaveformStorage              1.2.840.10008.5.1.4.1.1.9.3.1		sop');
  writeln(f, 'BasicVoiceAudioWaveformStorage                       1.2.840.10008.5.1.4.1.1.9.4.1		sop');
  writeln(f, 'GeneralAudioWaveformStorage                          1.2.840.10008.5.1.4.1.1.9.4.2		sop');
  writeln(f, 'ArterialPulseWaveformStorage                         1.2.840.10008.5.1.4.1.1.9.5.1		sop');
  writeln(f, 'RespiratoryWaveformStorage                           1.2.840.10008.5.1.4.1.1.9.6.1		sop');
  writeln(f, 'RETIRED_StandaloneModalityLUTStorage                 1.2.840.10008.5.1.4.1.1.10		sop');
  writeln(f, 'RETIRED_StandaloneVOILUTStorage                      1.2.840.10008.5.1.4.1.1.11		sop');
  writeln(f, 'GrayscaleSoftcopyPresentationStateStorage            1.2.840.10008.5.1.4.1.1.11.1		sop');
  writeln(f, 'ColorSoftcopyPresentationStateStorage                1.2.840.10008.5.1.4.1.1.11.2		sop');
  writeln(f, 'PseudoColorSoftcopyPresentationStateStorage          1.2.840.10008.5.1.4.1.1.11.3		sop');
  writeln(f, 'BlendingSoftcopyPresentationStateStorage             1.2.840.10008.5.1.4.1.1.11.4		sop');
  writeln(f, 'XAXRFGrayscaleSoftcopyPresentationStateStorage       1.2.840.10008.5.1.4.1.1.11.5		sop');
  writeln(f, 'RETIRED_XASinglePlaneStorage                         1.2.840.10008.5.1.4.1.1.12	sop');
  writeln(f, 'XRayAngiographicImageStorage                         1.2.840.10008.5.1.4.1.1.12.1		sop');
  writeln(f, 'EnhancedXAImageStorage                               1.2.840.10008.5.1.4.1.1.12.1.1		sop');
  writeln(f, 'XRayRadiofluoroscopicImageStorage                    1.2.840.10008.5.1.4.1.1.12.2		sop');
  writeln(f, 'EnhancedXRFImageStorage                              1.2.840.10008.5.1.4.1.1.12.2.1		sop');
  writeln(f, 'RETIRED_XRayAngiographicBiPlaneImageStorage          1.2.840.10008.5.1.4.1.1.12.3		sop');
  writeln(f, 'XRay3DAngiographicImageStorage                       1.2.840.10008.5.1.4.1.1.13.1.1		sop');
  writeln(f, 'XRay3DCraniofacialImageStorage                       1.2.840.10008.5.1.4.1.1.13.1.2		sop');
  writeln(f, 'BreastTomosynthesisImageStorage                      1.2.840.10008.5.1.4.1.1.13.1.3		sop');
  writeln(f, 'NuclearMedicineImageStorage                          1.2.840.10008.5.1.4.1.1.20		sop');
  writeln(f, 'RawDataStorage                                       1.2.840.10008.5.1.4.1.1.66		sop');
  writeln(f, 'SpatialRegistrationStorage                           1.2.840.10008.5.1.4.1.1.66.1		sop');
  writeln(f, 'SpatialFiducialsStorage                              1.2.840.10008.5.1.4.1.1.66.2		sop');
  writeln(f, 'DeformableSpatialRegistrationStorage                 1.2.840.10008.5.1.4.1.1.66.3		sop');
  writeln(f, 'SegmentationStorage                                  1.2.840.10008.5.1.4.1.1.66.4		sop');
  writeln(f, 'SurfaceSegmentationStorage                           1.2.840.10008.5.1.4.1.1.66.5		sop');
  writeln(f, 'RealWorldValueMappingStorage                         1.2.840.10008.5.1.4.1.1.67		sop');
  writeln(f, 'RETIRED_VLImageStorage                               1.2.840.10008.5.1.4.1.1.77.1		sop');
  writeln(f, 'VLEndoscopicImageStorage                             1.2.840.10008.5.1.4.1.1.77.1.1		sop');
  writeln(f, 'VideoEndoscopicImageStorage                          1.2.840.10008.5.1.4.1.1.77.1.1.1		sop');
  writeln(f, 'VLMicroscopicImageStorage                            1.2.840.10008.5.1.4.1.1.77.1.2		sop');
  writeln(f, 'VideoMicroscopicImageStorage                         1.2.840.10008.5.1.4.1.1.77.1.2.1		sop');
  writeln(f, 'VLSlideCoordinatesMicroscopicImageStorage            1.2.840.10008.5.1.4.1.1.77.1.3		sop');
  writeln(f, 'VLPhotographicImageStorage                           1.2.840.10008.5.1.4.1.1.77.1.4		sop');
  writeln(f, 'VideoPhotographicImageStorage                        1.2.840.10008.5.1.4.1.1.77.1.4.1		sop');
  writeln(f, 'OphthalmicPhotography8BitImageStorage                1.2.840.10008.5.1.4.1.1.77.1.5.1		sop');
  writeln(f, 'OphthalmicPhotography16BitImageStorage               1.2.840.10008.5.1.4.1.1.77.1.5.2		sop');
  writeln(f, 'StereometricRelationshipStorage                      1.2.840.10008.5.1.4.1.1.77.1.5.3		sop');
  writeln(f, 'OphthalmicTomographyImageStorage                     1.2.840.10008.5.1.4.1.1.77.1.5.4		sop');
  writeln(f, 'VLWholeSlideMicroscopyImageStorage                   1.2.840.10008.5.1.4.1.1.77.1.6		sop');
  writeln(f, 'RETIRED_VLMultiFrameImageStorage                     1.2.840.10008.5.1.4.1.1.77.2		sop');
  writeln(f, 'LensometryMeasurementsStorage                        1.2.840.10008.5.1.4.1.1.78.1		sop');
  writeln(f, 'AutorefractionMeasurementsStorage                    1.2.840.10008.5.1.4.1.1.78.2		sop');
  writeln(f, 'KeratometryMeasurementsStorage                       1.2.840.10008.5.1.4.1.1.78.3		sop');
  writeln(f, 'SubjectiveRefractionMeasurementsStorage              1.2.840.10008.5.1.4.1.1.78.4		sop');
  writeln(f, 'VisualAcuityMeasurementsStorage                      1.2.840.10008.5.1.4.1.1.78.5		sop');
  writeln(f, 'SpectaclePrescriptionReportStorage                   1.2.840.10008.5.1.4.1.1.78.6		sop');
  writeln(f, 'OphthalmicAxialMeasurementsStorage                   1.2.840.10008.5.1.4.1.1.78.7		sop');
  writeln(f, 'IntraocularLensCalculationsStorage                   1.2.840.10008.5.1.4.1.1.78.8		sop');
  writeln(f, 'MacularGridThicknessAndVolumeReportStorage           1.2.840.10008.5.1.4.1.1.79.1		sop');
  writeln(f, 'OphthalmicVisualFieldStaticPerimetryMeasurementsSt.  1.2.840.10008.5.1.4.1.1.80.1		sop');
  writeln(f, 'DRAFT_SRTextStorage                                  1.2.840.10008.5.1.4.1.1.88.1		sop');
  writeln(f, 'DRAFT_SRAudioStorage                                 1.2.840.10008.5.1.4.1.1.88.2		sop');
  writeln(f, 'DRAFT_SRDetailStorage                                1.2.840.10008.5.1.4.1.1.88.3		sop');
  writeln(f, 'DRAFT_SRComprehensiveStorage                         1.2.840.10008.5.1.4.1.1.88.4		sop');
  writeln(f, 'BasicTextSRStorage                                   1.2.840.10008.5.1.4.1.1.88.11		sop');
  writeln(f, 'EnhancedSRStorage                                    1.2.840.10008.5.1.4.1.1.88.22		sop');
  writeln(f, 'ComprehensiveSRStorage                               1.2.840.10008.5.1.4.1.1.88.33		sop');
  writeln(f, 'ProcedureLogStorage                                  1.2.840.10008.5.1.4.1.1.88.40		sop');
  writeln(f, 'MammographyCADSRStorage                              1.2.840.10008.5.1.4.1.1.88.50		sop');
  writeln(f, 'KeyObjectSelectionDocumentStorage                    1.2.840.10008.5.1.4.1.1.88.59		sop');
  writeln(f, 'ChestCADSRStorage                                    1.2.840.10008.5.1.4.1.1.88.65		sop');
  writeln(f, 'XRayRadiationDoseSRStorage                           1.2.840.10008.5.1.4.1.1.88.67		sop');
  writeln(f, 'ColonCADSRStorage                                    1.2.840.10008.5.1.4.1.1.88.69		sop');
  writeln(f, 'ImplantationPlanSRDocumentStorage                    1.2.840.10008.5.1.4.1.1.88.70		sop');
  writeln(f, 'EncapsulatedPDFStorage                               1.2.840.10008.5.1.4.1.1.104.1		sop');
  writeln(f, 'EncapsulatedCDAStorage                               1.2.840.10008.5.1.4.1.1.104.2		sop');
  writeln(f, 'PositronEmissionTomographyImageStorage               1.2.840.10008.5.1.4.1.1.128		sop');
  writeln(f, 'RETIRED_StandalonePETCurveStorage                    1.2.840.10008.5.1.4.1.1.129		sop');
  writeln(f, 'EnhancedPETImageStorage                              1.2.840.10008.5.1.4.1.1.130		sop');
  writeln(f, 'BasicStructuredDisplayStorage                        1.2.840.10008.5.1.4.1.1.131		sop');
  writeln(f, 'RTImageStorage                                       1.2.840.10008.5.1.4.1.1.481.1		sop');
  writeln(f, 'RTDoseStorage                                        1.2.840.10008.5.1.4.1.1.481.2		sop');
  writeln(f, 'RTStructureSetStorage                                1.2.840.10008.5.1.4.1.1.481.3		sop');
  writeln(f, 'RTBeamsTreatmentRecordStorage                        1.2.840.10008.5.1.4.1.1.481.4		sop');
  writeln(f, 'RTPlanStorage                                        1.2.840.10008.5.1.4.1.1.481.5		sop');
  writeln(f, 'RTBrachyTreatmentRecordStorage                       1.2.840.10008.5.1.4.1.1.481.6		sop');
  writeln(f, 'RTTreatmentSummaryRecordStorage                      1.2.840.10008.5.1.4.1.1.481.7		sop');
  writeln(f, 'RTIonPlanStorage                                     1.2.840.10008.5.1.4.1.1.481.8		sop');
  writeln(f, 'RTIonBeamsTreatmentRecordStorage                     1.2.840.10008.5.1.4.1.1.481.9		sop');
  writeln(f, 'DRAFT_RTBeamsDeliveryInstructionStorage              1.2.840.10008.5.1.4.34.1		sop');
  writeln(f, 'GenericImplantTemplateStorage                        1.2.840.10008.5.1.4.43.1		sop');
  writeln(f, 'ImplantAssemblyTemplateStorage                       1.2.840.10008.5.1.4.44.1		sop');
  writeln(f, 'ImplantTemplateGroupStorage                          1.2.840.10008.5.1.4.45.1		sop');
  		
  writeln(f, '#GEMRStorage                                         1.2.840.113619.4.2		sop');
  writeln(f, '#GECTStorage                                         1.2.840.113619.4.3		sop');
  writeln(f, 'GE3DModelObjectStorage                               1.2.840.113619.4.26		sop');
  writeln(f, 'GERTPlanStorage                                      1.2.840.113619.5.249		sop');
  writeln(f, 'GERTPlanStorage2                                     1.2.840.113619.4.5.249		sop');
  writeln(f, 'GESaturnTDSObjectStorage                             1.2.840.113619.5.253		sop');
  writeln(f, 'Philips3DVolumeStorage                               1.2.46.670589.5.0.1		sop');
  writeln(f, 'Philips3DObjectStorage                               1.2.46.670589.5.0.2		sop');
  writeln(f, 'PhilipsSurfaceStorage                                1.2.46.670589.5.0.3		sop');
  writeln(f, 'PhilipsCompositeObjectStorage                        1.2.46.670589.5.0.4		sop');
  writeln(f, 'PhilipsMRCardioProfileStorage                        1.2.46.670589.5.0.7		sop');
  writeln(f, 'PhilipsMRCardioImageStorage                          1.2.46.670589.5.0.8		sop');

  writeln(f, 'PatientRootQuery                                     1.2.840.10008.5.1.4.1.2.1.1	sop');
  writeln(f, 'PatientRootRetrieve                                  1.2.840.10008.5.1.4.1.2.1.2	sop');
  writeln(f, 'PatientRootGet                                       1.2.840.10008.5.1.4.1.2.1.3	sop');
  writeln(f, 'StudyRootQuery                                       1.2.840.10008.5.1.4.1.2.2.1	sop');
  writeln(f, 'StudyRootRetrieve                                    1.2.840.10008.5.1.4.1.2.2.2	sop');
  writeln(f, 'StudyRootGet                                         1.2.840.10008.5.1.4.1.2.2.3	sop');
  writeln(f, 'PatientStudyOnlyQuery                                1.2.840.10008.5.1.4.1.2.3.1	sop');
  writeln(f, 'PatientStudyOnlyRetrieve                             1.2.840.10008.5.1.4.1.2.3.2	sop');
  writeln(f, 'PatientStudyOnlyGet                                  1.2.840.10008.5.1.4.1.2.3.3	sop');
  writeln(f, 'FindModalityWorkList                                 1.2.840.10008.5.1.4.31		sop');
  writeln(f, 'PatientRootRetrieveNKI                               1.2.826.0.1.3680043.2.135.1066.5.1.4.1.2.1.2	sop');
  writeln(f, 'StudyRootRetrieveNKI                                 1.2.826.0.1.3680043.2.135.1066.5.1.4.1.2.2.2	sop');
  writeln(f, 'PatientStudyOnlyRetrieveNKI                          1.2.826.0.1.3680043.2.135.1066.5.1.4.1.2.3.2	sop');
  writeln(f, 'BasicGrayscalePrintManagementMeta                    1.2.840.10008.5.1.1.9			sop');
  writeln(f, 'BasicColorPrintManagementMeta                        1.2.840.10008.5.1.1.18			sop');
  writeln(f, 'BasicFilmSession                                     1.2.840.10008.5.1.1.1			sop');
  writeln(f, 'BasicFilmBox                                         1.2.840.10008.5.1.1.2			sop');
  writeln(f, 'BasicGrayscaleImageBox                               1.2.840.10008.5.1.1.4			sop');
  writeln(f, 'BasicColorImageBox                                   1.2.840.10008.5.1.1.4.1			sop');
  writeln(f, 'BasicPrinter                                         1.2.840.10008.5.1.1.16			sop');
  writeln(f,  'LittleEndianImplicit                                 1.2.840.10008.1.2	transfer');
  writeln(f,s+'LittleEndianExplicit                                 1.2.840.10008.1.2.1	transfer');
  writeln(f,s+'BigEndianExplicit                                    1.2.840.10008.1.2.2	transfer');
  writeln(f,s+'JPEGBaseLine1                                        1.2.840.10008.1.2.4.50	transfer	LittleEndianExplicit');
  writeln(f,s+'JPEGExtended2and4                                    1.2.840.10008.1.2.4.51	transfer	LittleEndianExplicit');
  writeln(f,  '#JPEGExtended3and5                                   1.2.840.10008.1.2.4.52	transfer	LittleEndianExplicit');
  writeln(f,s+'JPEGSpectralNH6and8                                  1.2.840.10008.1.2.4.53	transfer	LittleEndianExplicit');
  writeln(f,  '#JPEGSpectralNH7and9                                 1.2.840.10008.1.2.4.54	transfer	LittleEndianExplicit');
  writeln(f,s+'JPEGFulllNH10and12                                   1.2.840.10008.1.2.4.55	transfer	LittleEndianExplicit');
  writeln(f,  '#JPEGFulllNH11and13                                  1.2.840.10008.1.2.4.56	transfer	LittleEndianExplicit');
  writeln(f,s+'JPEGLosslessNH14                                     1.2.840.10008.1.2.4.57	transfer	LittleEndianExplicit');
  writeln(f,  '#JPEGLosslessNH15                                    1.2.840.10008.1.2.4.58	transfer	LittleEndianExplicit');
  writeln(f,  '#JPEGExtended16and18                                 1.2.840.10008.1.2.4.59	transfer	LittleEndianExplicit');
  writeln(f,  '#JPEGExtended17and19                                 1.2.840.10008.1.2.4.60	transfer	LittleEndianExplicit');
  writeln(f,  '#JPEGSpectral20and22                                 1.2.840.10008.1.2.4.61	transfer	LittleEndianExplicit');
  writeln(f,  '#JPEGSpectral21and23                                 1.2.840.10008.1.2.4.62	transfer	LittleEndianExplicit');
  writeln(f,  '#JPEGFull24and26                                     1.2.840.10008.1.2.4.63	transfer	LittleEndianExplicit');
  writeln(f,  '#JPEGFull25and27                                     1.2.840.10008.1.2.4.64	transfer	LittleEndianExplicit');
  writeln(f,  '#JPEGLossless28                                      1.2.840.10008.1.2.4.65	transfer	LittleEndianExplicit');
  writeln(f,  '#JPEGLossless29                                      1.2.840.10008.1.2.4.66	transfer	LittleEndianExplicit');
  writeln(f,s+'JPEGLossless                                         1.2.840.10008.1.2.4.70	transfer	LittleEndianExplicit');
  writeln(f,s+'JPEGLS_Lossless                                      1.2.840.10008.1.2.4.80	transfer	LittleEndianExplicit');
  writeln(f,s+'JPEGLS_Lossy                                         1.2.840.10008.1.2.4.81	transfer	LittleEndianExplicit');
  writeln(f,  '#RLELossless                                         1.2.840.10008.1.2.5	transfer	LittleEndianExplicit');
  writeln(f,  '#LittleEndianExplicitDeflated                        1.2.840.10008.1.2.1.99	transfer	LittleEndianExplicit');
  writeln(f,s+'JPEG2000LosslessOnly                                 1.2.840.10008.1.2.4.90	transfer	LittleEndianExplicit');
  writeln(f,s+'JPEG2000                                             1.2.840.10008.1.2.4.91	transfer	LittleEndianExplicit');

  CloseFile(f);
end;

procedure CreateDICOM_SQL;
var  f: textfile;
     UseDBFWithoutODBC, warnworklist : boolean;
     s: string;
begin
  UseDBFWithoutODBC := false;
  if FileExists(curdir + '\USEDBASEIIIWITHOUTODBC') then UseDBFWithoutODBC := true;

  // create the sql configuration file
  warnworklist := true;
  if not FileExists(curdir + '\dicom.sql') then
  begin
    warnworklist := false;
    AssignFile(f, CurDir + '\dicom.sql');
    Rewrite(f);
    writeln(f, '/*');
    writeln(f, '#       DICOM Database layout');
    if UseDBFWithoutODBC then
      writeln(f, '#     Example version for built-in DbaseIII driver (denormalized)')
    else
      writeln(f, '#     Example version for all SQL servers (mostly normalized)');
    writeln(f, '#');
    writeln(f, '#	(File DICOM.SQL)');
    writeln(f, '#	** DO NOT EDIT THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING **');
    writeln(f, '#');
    writeln(f, '#	Version with modality moved to the series level and EchoNumber in image table');
    writeln(f, '#	Revision 3: Patient birthday and sex, bolus agent, correct field lengths');
    writeln(f, '#	Revision 4: Studymodality, Station and Department in study');
    writeln(f, '#	            Manufacturer, Model, BodyPart and Protocol in series');
    writeln(f, '#	            Acqdate/time, coil, acqnumber, slicelocation and pixel info in images');
    writeln(f, '#       Notes for revision 4:');
    writeln(f, '#         InstitutionalDepartmentName in study (should officially be in series, but eFilm expects it in study)');
    writeln(f, '#         StationName is in study              (should officially be in series, but more useful in study)');
    writeln(f, '#	Revision 5: Added patientID in series and images for more efficient querying');
    writeln(f, '#	Revision 6: Added frame of reference UID in series table');
    writeln(f, '#	Revision 7: Added ImageType in image table, StudyModality to 64 chars, AcqDate to SQL_C_DATE');
    writeln(f, '#	Revision 8: Denormalized study table (add patient ID, name, birthdate) to show consistency problems');

    if UseDBFWithoutODBC then
    writeln(f, '#	Revision 9: Denormalized series and image table for full query support with built-in DBF driver');

    writeln(f, '#	Revision 10: Fixed width of ReceivingCoil: to 16 chars');

    if UseDBFWithoutODBC then
    begin
    writeln(f, '#	Revision 11: Added Manufacturer to denormalized image table for built-in DBF driver');
    writeln(f, '#	Revision 12: Removed duplicate STUDYINST field in series for built-in DBF driver');
    writeln(f, '#	Revision 15: Added patient position to denormalized image table for built-in DBF driver');
    end;
    writeln(f, '#	Revision 13: Added ImageID to image database');
    writeln(f, '#	Revision 14: Added WorkList database with HL7 tags');
    writeln(f, '#	Revision 16: Moved Stationname and InstitutionalDepartmentName to series table');
    writeln(f, '#	Revision 17: EchoNumber, ReqProcDescription to 64 characters; StudyModality, EchoNumber, ImageType to DT_MSTR; use Institution instead of InstitutionalDepartmentName');
    writeln(f, '#	Revision 18: DT_STR can now be replaced by DT_ISTR to force case insensitive searches');
    writeln(f, '#	Revision 19: Use QRows and QColumns as field names');

    writeln(f, '#');
    writeln(f, '#');
    writeln(f, '#	5 databases need to be defined:');
    writeln(f, '#');
    writeln(f, '#		*Patient*');
    writeln(f, '#			*Study*');
    writeln(f, '#				*Series*');
    writeln(f, '#					*Image*');
    writeln(f, '#			*WorkList*');
    writeln(f, '#');
    writeln(f, '#');
    writeln(f, '# The last defined element of Study is a link back to Patient');
    writeln(f, '# The last defined element of Series is a link back to Study');
    writeln(f, '# The last defined element of Image is a link back to Series');
    writeln(f, '#');
    writeln(f, '#');
    writeln(f, '# Format for DICOM databases :');
    writeln(f, '#	{ Group, Element, Column Name, Column Length, SQL-Type, DICOM-Type }');
    writeln(f, '# Format for Worklist database :');
    writeln(f, '#	{ Group, Element, Column Name, Column Length, SQL-Type, DICOM-Type, HL7 tag}');
    writeln(f, '#	HL7 tags include SEQ.N, SEQ.N.M, SEQ.N.DATE, SEQ.N.TIME, *AN, *UI');
    writeln(f, '*/');
    writeln(f, '');

    writeln(f, '*Patient*');
    writeln(f, '{');
    writeln(f, '	{ 0x0010, 0x0020, "PatientID", 64, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0010, 0x0010, "PatientName", 64, SQL_C_CHAR, DT_STR },');
    writeln(f, '        { 0x0010, 0x0030, "PatientBirthDate", 8, SQL_C_DATE, DT_DATE },');
    writeln(f, '	{ 0x0010, 0x0040, "PatientSex", 16, SQL_C_CHAR, DT_STR }');
    writeln(f, '}');
    writeln(f, '');

    writeln(f, '*Study*');
    writeln(f, '{');
    writeln(f, '	{ 0x0020, 0x000d, "StudyInstanceUID", 64, SQL_C_CHAR, DT_UI },');
    writeln(f, '	{ 0x0008, 0x0020, "StudyDate", 8, SQL_C_DATE, DT_DATE },');
    writeln(f, '	{ 0x0008, 0x0030, "StudyTime", 16, SQL_C_CHAR, DT_TIME },');
    writeln(f, '	{ 0x0020, 0x0010, "StudyID", 16, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0008, 0x1030, "StudyDescription", 64, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0008, 0x0050, "AccessionNumber", 16, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0008, 0x0090, "ReferPhysician", 64, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0010, 0x1010, "PatientsAge", 16, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0010, 0x1030, "PatientsWeight", 16, SQL_C_CHAR, DT_STR },');
// revision 4, 17
    writeln(f, '	{ 0x0008, 0x0061, "StudyModality", 64, SQL_C_CHAR, DT_MSTR },');
// end revision 4, 17

// revision 8
    writeln(f, '');
    writeln(f, '	{ 0x0010, 0x0010, "PatientName", 64, SQL_C_CHAR, DT_STR },');
    writeln(f, '        { 0x0010, 0x0030, "PatientBirthDate", 8, SQL_C_DATE, DT_DATE },');
    writeln(f, '	{ 0x0010, 0x0040, "PatientSex", 16, SQL_C_CHAR, DT_STR }');
    writeln(f, '');
// end revision 8

    writeln(f, '	{ 0x0010, 0x0020, "PatientID", 64, SQL_C_CHAR, DT_STR }');
    writeln(f, '}');
    writeln(f, '');

    writeln(f, '*Series*');
    writeln(f, '{');
    writeln(f, '	{ 0x0020, 0x000e, "SeriesInstanceUID", 64, SQL_C_CHAR, DT_UI },');
    writeln(f, '	{ 0x0020, 0x0011, "SeriesNumber", 12, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0008, 0x0021, "SeriesDate", 8, SQL_C_DATE, DT_DATE },');
    writeln(f, '	{ 0x0008, 0x0031, "SeriesTime", 16, SQL_C_CHAR, DT_TIME },');
    writeln(f, '	{ 0x0008, 0x103e, "SeriesDescription", 64, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0008, 0x0060, "Modality", 16, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0018, 0x5100, "PatientPosition", 16, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0018, 0x0010, "ContrastBolusAgent", 64, SQL_C_CHAR, DT_STR },');

// revision 4
    writeln(f, '	{ 0x0008, 0x0070, "Manufacturer", 64, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0008, 0x1090, "ModelName", 64, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0018, 0x0015, "BodyPartExamined", 64, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0018, 0x1030, "ProtocolName", 64, SQL_C_CHAR, DT_STR },');
// end revision 4

// revision 16
    writeln(f, '	{ 0x0008, 0x1010, "StationName", 16, SQL_C_CHAR, DT_STR },');
// revision 17
    writeln(f, '	{ 0x0008, 0x0080, "Institution", 64, SQL_C_CHAR, DT_STR },');
// end revision 17
// end revision 16

// revision 6
    writeln(f, '	{ 0x0020, 0x0052, "FrameOfReferenceUID", 64, SQL_C_CHAR, DT_UI },');
// end revision 6

// revision 5
    writeln(f, '	{ 0x0010, 0x0020, "SeriesPat", 64, SQL_C_CHAR, DT_STR },');
// end revision 5

// for denormalized version (built-in dbf support)
    if UseDBFWithoutODBC then
    begin
    writeln(f, '');
    writeln(f, '	{ 0x0010, 0x0010, "PatientName", 64, SQL_C_CHAR, DT_STR },');
    writeln(f, '        { 0x0010, 0x0030, "PatientBirthDate", 8, SQL_C_DATE, DT_DATE },');
    writeln(f, '	{ 0x0010, 0x0040, "PatientSex", 16, SQL_C_CHAR, DT_STR }');
    writeln(f, '');
// revision 12: removed duplicate field
//    writeln(f, '	{ 0x0020, 0x000d, "StudyInstanceUID", 64, SQL_C_CHAR, DT_UI },');
    writeln(f, '	{ 0x0008, 0x0020, "StudyDate", 8, SQL_C_DATE, DT_DATE },');
    writeln(f, '	{ 0x0008, 0x0030, "StudyTime", 16, SQL_C_CHAR, DT_TIME },');
    writeln(f, '	{ 0x0020, 0x0010, "StudyID", 16, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0008, 0x1030, "StudyDescription", 64, SQL_C_CHAR, DT_STR },');
    writeln(f, '');
    end;
// end denormalized version

    writeln(f, '	{ 0x0020, 0x000d, "StudyInstanceUID", 64, SQL_C_CHAR, DT_UI }');
    writeln(f, '}');
    writeln(f, '');

    writeln(f, '*Image*');
    writeln(f, '{');
    writeln(f, '	{ 0x0008, 0x0018, "SOPInstanceUID", 64, SQL_C_CHAR, DT_UI },');
    writeln(f, '	{ 0x0008, 0x0016, "SOPClassUID", 64, SQL_C_CHAR, DT_UI },');
    writeln(f, '	{ 0x0020, 0x0013, "ImageNumber", 12, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0008, 0x0023, "ImageDate", 8, SQL_C_DATE, DT_DATE },');
    writeln(f, '	{ 0x0008, 0x0033, "ImageTime", 16, SQL_C_CHAR, DT_TIME },');
// revision 17: from 12 to 64 and DT_MSTR
    writeln(f, '	{ 0x0018, 0x0086, "EchoNumber", 64, SQL_C_CHAR, DT_MSTR },');
// end revision 17: from 12 to 64 and DT_MSTR
  
    writeln(f, '	{ 0x0028, 0x0008, "NumberOfFrames", 12, SQL_C_CHAR, DT_STR },');
// revision 4
// 	revision 7: made this a SQL_C_DATE
    writeln(f, '	{ 0x0008, 0x0022, "AcqDate", 8, SQL_C_DATE, DT_DATE },');
// 	end revision 7
    writeln(f, '	{ 0x0008, 0x0032, "AcqTime", 16, SQL_C_CHAR, DT_TIME },');

// revision 10: made field width 16 instead of 12
    writeln(f, '	{ 0x0018, 0x1250, "ReceivingCoil", 16, SQL_C_CHAR, DT_STR },');
// end revision 10

    writeln(f, '	{ 0x0020, 0x0012, "AcqNumber", 12, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0020, 0x1041, "SliceLocation", 16, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0028, 0x0002, "SamplesPerPixel", 5, SQL_C_CHAR, DT_UINT16 },');
    writeln(f, '	{ 0x0028, 0x0004, "PhotoMetricInterpretation", 16, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0028, 0x0010, "QRows", 5, SQL_C_CHAR, DT_UINT16 },');
    writeln(f, '	{ 0x0028, 0x0011, "QColumns", 5, SQL_C_CHAR, DT_UINT16 },');
    writeln(f, '	{ 0x0028, 0x0101, "BitsStored", 5, SQL_C_CHAR, DT_UINT16 },');
// end revision 4
// revision 7, 17
    writeln(f, '	{ 0x0008, 0x0008, "ImageType", 128, SQL_C_CHAR, DT_MSTR },');
// end revision 7, 17
// revision 13
    writeln(f, '	{ 0x0054, 0x0400, "ImageID", 16, SQL_C_CHAR, DT_STR },');
// end revision 13
// revision 5
    writeln(f, '	{ 0x0010, 0x0020, "ImagePat", 64, SQL_C_CHAR, DT_STR },');
// end revision 5

// for denormalized version (built-in dbf support)
    if UseDBFWithoutODBC then
    begin
    writeln(f, '');
    writeln(f, '	{ 0x0010, 0x0010, "PatientName", 64, SQL_C_CHAR, DT_STR },');
    writeln(f, '        { 0x0010, 0x0030, "PatientBirthDate", 8, SQL_C_DATE, DT_DATE },');
    writeln(f, '	{ 0x0010, 0x0040, "PatientSex", 16, SQL_C_CHAR, DT_STR }');
    writeln(f, '');
    writeln(f, '	{ 0x0020, 0x000d, "StudyInstanceUID", 64, SQL_C_CHAR, DT_UI },');
    writeln(f, '	{ 0x0008, 0x0020, "StudyDate", 8, SQL_C_DATE, DT_DATE },');
    writeln(f, '	{ 0x0008, 0x0030, "StudyTime", 16, SQL_C_CHAR, DT_TIME },');
    writeln(f, '	{ 0x0020, 0x0010, "StudyID", 16, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0008, 0x1030, "StudyDescription", 64, SQL_C_CHAR, DT_STR },');
    writeln(f, '');
    writeln(f, '	{ 0x0020, 0x0011, "SeriesNumber", 12, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0008, 0x0021, "SeriesDate", 8, SQL_C_DATE, DT_DATE },');
    writeln(f, '	{ 0x0008, 0x0031, "SeriesTime", 16, SQL_C_CHAR, DT_TIME },');
    writeln(f, '	{ 0x0008, 0x103e, "SeriesDescription", 64, SQL_C_CHAR, DT_STR },');
    writeln(f, '	{ 0x0008, 0x0060, "Modality", 16, SQL_C_CHAR, DT_STR },');
// revision 15
    writeln(f, '	{ 0x0018, 0x5100, "PatientPosition", 16, SQL_C_CHAR, DT_STR },');
// end revision 15
    writeln(f, '	{ 0x0020, 0x0052, "FrameOfReferenceUID", 64, SQL_C_CHAR, DT_UI },');
// revision 11
    writeln(f, '	{ 0x0008, 0x0070, "Manufacturer", 64, SQL_C_CHAR, DT_STR },');
// end revision 11
    writeln(f, '');
    end;
// end denormalized version

    writeln(f, '	{ 0x0020, 0x000e, "SeriesInstanceUID", 64, SQL_C_CHAR, DT_UI }');
    writeln(f, '}');

    CloseFile(f);
  end;

  AssignFile(f, CurDir + '\dicom.sql');
  Reset(f);
  while not EOF(f) do
  begin
    Readln(f, s);
    if pos('*WorkList*', s)=1 then break;
  end;
  CloseFile(f);

  if pos('*WorkList*', s)<>1 then
  begin
    AssignFile(f, CurDir + '\dicom.sql');
    Append(f);

// revision 14
    writeln(f, '');
    writeln(f, '*WorkList*');
    writeln(f, '{');
    writeln(f, ' 	{ 0x0008, 0x0050, "AccessionNumber",    16, SQL_C_CHAR, DT_STR,  "OBR.3" },');
    writeln(f, ' 	{ 0x0010, 0x0020, "PatientID",          64, SQL_C_CHAR, DT_STR,  "PID.4" },');
    writeln(f, ' 	{ 0x0010, 0x0010, "PatientName",        64, SQL_C_CHAR, DT_STR,  "PID.5" },');
    writeln(f, '        { 0x0010, 0x0030, "PatientBirthDate",    8, SQL_C_DATE, DT_DATE, "PID.7" },');
    writeln(f, ' 	{ 0x0010, 0x0040, "PatientSex",         16, SQL_C_CHAR, DT_STR,  "PID.8" },');
    writeln(f, '');
    writeln(f, ' 	{ 0x0010, 0x2000, "MedicalAlerts",      64, SQL_C_CHAR, DT_STR,  "---" },');
    writeln(f, ' 	{ 0x0010, 0x2110, "ContrastAllergies",  64, SQL_C_CHAR, DT_STR,  "---" },');
    writeln(f, ' 	{ 0x0020, 0x000d, "StudyInstanceUID",   64, SQL_C_CHAR, DT_UI,   "---" },');
    writeln(f, ' 	{ 0x0032, 0x1032, "ReqPhysician",       64, SQL_C_CHAR, DT_STR,  "OBR.16" },');
// revision 17: from 16 to 64
    writeln(f, ' 	{ 0x0032, 0x1060, "ReqProcDescription", 64, SQL_C_CHAR, DT_STR,  "OBR.4.1" },');
// end revision 17: from 16 to 64
    writeln(f, '');
    writeln(f, ' 	{ 0x0040, 0x0100, "--------",   0, SQL_C_CHAR, DT_STARTSEQUENCE, "---" },');
    writeln(f, ' 	{ 0x0008, 0x0060, "Modality",           16, SQL_C_CHAR, DT_STR,  "OBR.21" },');
    writeln(f, ' 	{ 0x0032, 0x1070, "ReqContrastAgent",   64, SQL_C_CHAR, DT_STR,  "---" },');
    writeln(f, ' 	{ 0x0040, 0x0001, "ScheduledAE",        16, SQL_C_CHAR, DT_STR,  "---" },');
    writeln(f, ' 	{ 0x0040, 0x0002, "StartDate",           8, SQL_C_DATE, DT_DATE, "OBR.7.DATE" },');
    writeln(f, ' 	{ 0x0040, 0x0003, "StartTime",          16, SQL_C_CHAR, DT_TIME, "OBR.7.TIME" },');
    writeln(f, ' 	{ 0x0040, 0x0006, "PerfPhysician",      64, SQL_C_CHAR, DT_STR,  "---" },');
    writeln(f, ' 	{ 0x0040, 0x0007, "SchedPSDescription", 64, SQL_C_CHAR, DT_STR,  "---" },');
    writeln(f, ' 	{ 0x0040, 0x0009, "SchedPSID",          16, SQL_C_CHAR, DT_STR,  "OBR.4" },');
    writeln(f, ' 	{ 0x0040, 0x0010, "SchedStationName",   16, SQL_C_CHAR, DT_STR,  "OBR.24" },');
    writeln(f, ' 	{ 0x0040, 0x0011, "SchedPSLocation",    16, SQL_C_CHAR, DT_STR,  "---" },');
    writeln(f, ' 	{ 0x0040, 0x0012, "PreMedication",      64, SQL_C_CHAR, DT_STR,  "---" },');
    writeln(f, ' 	{ 0x0040, 0x0400, "SchedPSComments",    64, SQL_C_CHAR, DT_STR,  "---" },');
    writeln(f, ' 	{ 0x0040, 0x0100, "---------",    0, SQL_C_CHAR, DT_ENDSEQUENCE, "---" },');
    writeln(f, '');
    writeln(f, '        { 0x0040, 0x1001, "ReqProcID",          16, SQL_C_CHAR, DT_STR,  "OBR.4.0" },');
    writeln(f, ' 	{ 0x0040, 0x1003, "ReqProcPriority",    16, SQL_C_CHAR, DT_STR,  "OBR.27" }');
    writeln(f, '}');
// end revision 14

    CloseFile(f);
    if warnworklist then
      ShowMessage('Added worklist entry to dicom.sql; worklist database must now be cleared!');
  end;
end;

procedure TForm1.FormCreate(Sender: TObject);
  var p, c, i, numserv: integer;
      f: TextFile;
      text, s: string;
      Registry: TRegistry;
      ModalResult : TModalResult;
      list: TStringList;
      sr: TSearchRec;

      OCXHandle: THandle;
      RegFunc: TDllRegisterServer;
      InstallDir: string;
begin
  // Set default configuration parameters
  try
    GetDir(0, CurDir);
  except
  end;

  InstallationSocket := TWSocket.Create(form1);
    InstallationSocket.LineMode := False;
    InstallationSocket.LineEnd := #13#10;
    InstallationSocket.LineEcho := False;
    InstallationSocket.LineEdit := False;
    InstallationSocket.Proto := 'udp';
    InstallationSocket.LocalAddr := '0.0.0.0';
    InstallationSocket.LocalPort := '0';
    InstallationSocket.MultiThreaded := False;
    InstallationSocket.OnDataAvailable := InstallationSocketDataAvailable;
    InstallationSocket.FlushTimeout := 60;
    InstallationSocket.SendFlags := wsSendNormal;
    InstallationSocket.LingerOnOff := wsLingerOn;
    InstallationSocket.LingerTimeout := 0;
    InstallationSocket.SocksLevel := '5';
    InstallationSocket.SocksAuthentication := socksNoAuthentication;

  MaintenanceSocket:= TWSocket.Create(form1);
    MaintenanceSocket.LineMode := False;
    MaintenanceSocket.LineEnd := #13#10;
    MaintenanceSocket.LineEcho := False;
    MaintenanceSocket.LineEdit := False;
    MaintenanceSocket.Proto := 'udp';
    MaintenanceSocket.LocalAddr := '0.0.0.0';
    MaintenanceSocket.LocalPort := '0';
    MaintenanceSocket.MultiThreaded := False;
    MaintenanceSocket.OnDataAvailable := MaintenanceSocketDataAvailable;
    MaintenanceSocket.FlushTimeout := 60;
    MaintenanceSocket.SendFlags := wsSendNormal;
    MaintenanceSocket.LingerOnOff := wsLingerOn;
    MaintenanceSocket.LingerTimeout := 0;
    MaintenanceSocket.SocksLevel := '5';
    MaintenanceSocket.SocksAuthentication := socksNoAuthentication;

  ServerStatusSocket := TWSocket.Create(form1);
    ServerStatusSocket.LineMode := False;
    ServerStatusSocket.LineEnd := #13#10;
    ServerStatusSocket.LineEcho := False;
    ServerStatusSocket.LineEdit := False;
    ServerStatusSocket.Proto := 'udp';
    ServerStatusSocket.LocalAddr := '0.0.0.0';
    ServerStatusSocket.LocalPort := '0';
    ServerStatusSocket.MultiThreaded := False;
    ServerStatusSocket.OnDataAvailable := ServerStatusSocketDataAvailable;
    ServerStatusSocket.FlushTimeout := 60;
    ServerStatusSocket.SendFlags := wsSendNormal;
    ServerStatusSocket.LingerOnOff := wsLingerOn;
    ServerStatusSocket.LingerTimeout := 0;
    ServerStatusSocket.SocksLevel := '5';
    ServerStatusSocket.SocksAuthentication := socksNoAuthentication;

  ProgressSocket := TWSocket.Create(form1);
    ProgressSocket.LineMode := False;
    ProgressSocket.LineEnd := #13#10;
    ProgressSocket.LineEcho := False;
    ProgressSocket.LineEdit := False;
    ProgressSocket.Proto := 'tcp';
    ProgressSocket.LocalAddr := '0.0.0.0';
    ProgressSocket.LocalPort := '0';
    ProgressSocket.MultiThreaded := False;
    ProgressSocket.OnDataAvailable := ProgressSocketDataAvailable;
    ProgressSocket.OnSessionAvailable := ProgressSocketSessionAvailable;
    ProgressSocket.FlushTimeout := 160;
    ProgressSocket.SendFlags := wsSendNormal;
    ProgressSocket.LingerOnOff := wsLingerOn;
    ProgressSocket.LingerTimeout := 0;
    ProgressSocket.SocksLevel := '5';
    ProgressSocket.SocksAuthentication := socksNoAuthentication;

  ArchiveSocket := TWSocket.Create(form1);
    ArchiveSocket.LineMode := False;
    ArchiveSocket.LineEnd := #13#10;
    ArchiveSocket.LineEcho := False;
    ArchiveSocket.LineEdit := False;
    ArchiveSocket.Proto := 'udp';
    ArchiveSocket.LocalAddr := '0.0.0.0';
    ArchiveSocket.LocalPort := '0';
    ArchiveSocket.MultiThreaded := False;
    ArchiveSocket.OnDataAvailable := ArchiveSocketDataAvailable;
    ArchiveSocket.FlushTimeout := 60;
    ArchiveSocket.SendFlags := wsSendNormal;
    ArchiveSocket.LingerOnOff := wsLingerOn;
    ArchiveSocket.LingerTimeout := 0;
    ArchiveSocket.SocksLevel := '5';
    ArchiveSocket.SocksAuthentication := socksNoAuthentication;

  ListenSocket := TWSocket.Create(form1);
    ListenSocket.LineMode := False;
    ListenSocket.LineEnd := #13#10;
    ListenSocket.LineEcho := False;
    ListenSocket.LineEdit := False;
    ListenSocket.Proto := 'tcp';
    ListenSocket.LocalAddr := '0.0.0.0';
    ListenSocket.LocalPort := '0';
    ListenSocket.MultiThreaded := False;
    ListenSocket.OnSessionAvailable := ListenSocketSessionAvailable;
    ListenSocket.FlushTimeout := 60;
    ListenSocket.SendFlags := wsSendNormal;
    ListenSocket.LingerOnOff := wsLingerOn;
    ListenSocket.LingerTimeout := 0;
    ListenSocket.SocksLevel := '5';
    ListenSocket.SocksAuthentication := socksNoAuthentication;

  WeeklySocket:= TWSocket.Create(form1);
    WeeklySocket.LineMode := False;
    WeeklySocket.LineEnd := #13#10;
    WeeklySocket.LineEcho := False;
    WeeklySocket.LineEdit := False;
    WeeklySocket.Proto := 'tcp';
    WeeklySocket.LocalAddr := '0.0.0.0';
    WeeklySocket.LocalPort := '0';
    WeeklySocket.MultiThreaded := False;
    WeeklySocket.OnDataAvailable := WeeklySocketDataAvailable;
    WeeklySocket.FlushTimeout := 60;
    WeeklySocket.SendFlags := wsSendNormal;
    WeeklySocket.LingerOnOff := wsLingerOn;
    WeeklySocket.LingerTimeout := 0;
    WeeklySocket.SocksLevel := '5';
    WeeklySocket.SocksAuthentication := socksNoAuthentication;

  TestSocket:= TWSocket.Create(form1);
    TestSocket.LineMode := False;
    TestSocket.LineEnd := #13#10;
    TestSocket.LineEcho := False;
    TestSocket.LineEdit := False;
    TestSocket.Proto := 'tcp';
    TestSocket.LocalAddr := '0.0.0.0';
    TestSocket.LocalPort := '0';
    TestSocket.MultiThreaded := False;
    TestSocket.FlushTimeout := 60;
    TestSocket.SendFlags := wsSendNormal;
    TestSocket.LingerOnOff := wsLingerOn;
    TestSocket.LingerTimeout := 0;
    TestSocket.SocksLevel := '5';
    TestSocket.SocksAuthentication := socksNoAuthentication;

  Mailer:= TSyncSmtpCli.Create(form1);
    Mailer.Tag := 9999;
    Mailer.Port := 'smtp';
    Mailer.CharSet := 'iso-8859-1';
    Mailer.ContentType := smtpPlainText;
    Mailer.Timeout := 15;
    Mailer.MultiThreaded := False;

  MDBFTable1:= TMDBFTable.Create(form1);
  MDBFTable2:= TMDBFTable.Create(form1);
  MDBFTable3:= TMDBFTable.Create(form1);
  MDBFTable4:= TMDBFTable.Create(form1);

  TrayIcon1 := TTrayIcon.Create(form1);
  TrayIcon1.Visible := True;
  TrayIcon1.Hint := 'ConQuest DICOM Server';
  TrayIcon1.OnClick := TrayIcon1Click;

  CheckBoxWebServer.Visible := FileExists('lua\ladle.lua');

  othersections := TStringList.Create;

  Timer1.Enabled := false;
  Timer2.Enabled := false;
  Timer5.Enabled := false;

  TabSheet8.TabVisible := false;
  TapeBackupPage.TabVisible := false;
  WeeklyChecksPage.TabVisible := false;

  ArchiveStatus := 'Inactive';
  DragAcceptFiles((self as TWinControl).Handle, True);
  ModalResult := mrCancel;

  Timer4.enabled := false;

  if not assigned(ServerStatusStringList) then
    ServerStatusStringList := TStringList.Create;

  if ParamCount>0 then
  begin
    for i:=1 to ParamCount do
    begin
      text := LowerCase(ParamStr(i));
      if (text = '/hide') or (text = '-hide') then
        Hide;

      if (text = '/noconfirmquit') or (text = '-noconfirmquit') then
        NoConfirmQuit := true;

      if (text = '/?') or (text = '-?') or (text = '?') then
      begin
        // operations would fail before initialization
        ShowMessage('Conquest DICOM server'+#13+#13+
                    'Usage:'+#13+
                    ' -hide               = start hidden as tray icon only' + #13 +
                    ' -noconfirmquit = allow close of server without confirmation');
      end;
    end;
  end;

  try
    WriteLog('starting conquestdicomserver');
  except
    ShowMessage('The Conquest DICOM server has not enough rights to write log files - quiting');
    Application.Terminate;
    exit;
  end;

  NewInstall := False;
  NewInstallDone := True;

  if not FileExists(Curdir+'\dgate.exe') and not FileExists(Curdir+'\dgate64.exe') then
  begin
    DgateExe := 'install64\dgate64.exe';
    if RunDgate('-?', false)=0 then
    begin
      InstallDir := curdir + '\install32\';
      DgateExe := 'dgate.exe';
    end
    else
    begin
      InstallDir := curdir + '\install64\';
      DgateExe := 'dgate64.exe';
    end;

    if FindFirst(InstallDir+'*.*',faAnyFile,sr) = 0 then
    repeat
      if (sr.Attr and faDirectory ) < $00000008 then
        CopyFile(PChar(InstallDir+sr.Name),PChar(Curdir+'\'+sr.Name), false);
    until findNext(sr) <> 0
  end
  else
  begin
    // attempt to run the 64 bits exe
    DgateExe := 'dgate64.exe';
    if RunDgate('-?', false)=0 then
      DgateExe := 'dgate.exe';
  end;

  if not SysUtils.DirectoryExists(GetTempDir + 'conquest_browser') then
    Mkdir(GetTempDir + 'conquest_browser');

  MagThreshold.text   := '0';
  SqlHost             := 'localhost';
  MySql               := '0';
  FSqLite             := '0';
  FPostGres           := '0';
  BrowseThroughDBF    := '1';  // 1.4.19beta
  SqlUser             := 'conquest';
  SqlPassWord         := 'conquest';
  FileNameSyntax      := '3';
  AllowEmptyPatientID := '0';
  MaxFileNameLength   := '255';
  FixPhilips          := '0';
  FixKodak            := '0';
  DoubleBackSlashToDB := '0';
  UseEscapeStringConstants := '0';
  UTF8ToDB            := '0';
  UTF8FromDB          := '0';
  MagDevices          := 1;
  BurnTime            := 'INVALID';
  BurnTime2           := 'INVALID';
  BurnWeekend         := 0;
  MirrorVerifyTime    := 'INVALID';
  TestReadTime        := 'INVALID';
  ForwardAssociationLevel        := 'IMAGE';
  ForwardAssociationCloseDelay   := 5;
  ForwardAssociationRefreshDelay := 3600;
  ForwardAssociationRelease      := 1;
  Prefetcher          := '0';

  if FileExists(Curdir + '\webserver\ActiveFormProj1.ocx') then
  begin
    OCXHandle := LoadLibrary (Pchar(Curdir + '\webserver\ActiveFormProj1.ocx'));
    if OCXHandle<>0 then
    begin
      RegFunc := GetProcAddress (OCXHandle, 'DllRegisterServer');
      RegFunc; // <> 0 then ShowMessage('Failed to register ActiveX control');
      FreeLibrary (OCXHandle);
    end;
  end;

  if not FileExists(CurDir + '\dicom.ini') then
  begin
    NewInstall := True;
    NewInstallDone := False;
    if pos(' ', ParamStr(0))>0 then
    begin
      ShowMessage('WARNING - THIS SERVER SHOULD NOT BE INSTALLED IN A DIRECTORY WITH SPACES IN ITS NAME');
    end;
  end;

  // Locate temporary ports for the status displays

  NewPort(InstallationSocket, '1150',                  '1999');
  NewPort(MaintenanceSocket , InstallationSocket.Port, '1999');
  NewPort(ArchiveSocket     , MaintenanceSocket.Port,  '1999');
  NewPort(WeeklySocket      , ArchiveSocket.Port,      '1999');
  NewPort(ProgressSocket    , WeeklySocket.Port,      '1999');

  // use ServerStatusPort to detect how many servers are running at this time

  NewPort(ServerStatusSocket, '1110',                  '1999');
  NumServ := StrToInt(ServerStatusSocket.Port)-1110;

  // set the default name, port and datasource for this server (if no dicom.ini)

  ServerName.text   := 'CONQUESTSRV' + IntToStr(NumServ);
  TCPIPport.text    := IntToStr(5677 + NumServ);

  if NumServ = 1 then
    DataSource      := 'conquestpacs_s'
  else
    DataSource      := 'conquestpacs_s' + IntToStr(NumServ);

  // open the file into one of the pages for editing
  if FileExists(CurDir + '\acrnema.map') then
    DICOMMap.Lines.LoadFromFile(CurDir + '\acrnema.map');
  FillAELists;

  // make sure a dgatesop.lst exists. Default support JPEG
  if not FileExists(curdir + '\dgatesop.lst') then
    CreateDGATESOP_LST(true);

  // Check dgatesop.lst to see if JPEG support is enabled
  JPEGSupport := false;
  AssignFile(f, Curdir + '\dgatesop.lst');
  Reset(f);
  while not Eof(f) do
  begin
    Readln(f, text);
    if Pos('JPEGLossless', text)=1 then
    begin
      JPEGSupport := true;
      break;
    end;
  end;
  CloseFile(f);

  EnableJPEGCheckBox.Enabled := true; // FileExists(ExtractFileDir(ParamStr(0)) + '\dcmdjpeg.exe');
  EnableJPEGCheckBox.Checked := JPEGSupport;
  EnableJPEGCheckBoxClick(self);
  UncompressedButton.Checked := true; // default
  DCMRadioButton.Checked     := true; // default

  // read dicom.ini and/or jukebox.ini and/or tapebackup.ini if they exist
  RestoreconfigButtonClick(Self);

  // create the default dicom providers list
  if not FileExists(CurDir + '\acrnema.map') then
  begin
    AssignFile(f, CurDir + '\acrnema.map');
    Rewrite(f);
    writeln(f, '/* **********************************************************');
    writeln(f, ' *                                                          *');
    writeln(f, ' * DICOM AE (Application entity) -> IP address / Port map   *');
    writeln(f, ' * (This is file ACRNEMA.MAP)                               *');
    writeln(f, ' *                                                          *');
    writeln(f, ' * All DICOM systems that want to retrieve images from the  *');
    writeln(f, ' * Conquest DICOM server must be listed here with correct   *');
    writeln(f, ' * AE name, (IP adress or hostname) and port number. The    *');
    writeln(f, ' * first entry is the Conquest server itself. The last ones *');
    writeln(f, ' * with * show wildcard mechanism. Add new entries above.   *');
    writeln(f, ' *                                                          *');
    writeln(f, ' * The syntax for each entry is :                           *');
    writeln(f, ' *   AE   <IP adress|Host name>   port number   compression *');
    writeln(f, ' *                                                          *');
    writeln(f, ' * For compression see manual. Values are un=uncompressed;  *');
    writeln(f, ' * ul=littleendianexplicit,ub=bigendianexplicit,ue=both     *');
    writeln(f, ' * j2=lossless jpeg;j3..j6=lossy jpeg;n1..n4=nki private    *');
    writeln(f, ' * js   =lossless jpegLS;  j7=lossy jpegLS                  *');
    writeln(f, ' * jk   =lossless jpeg2000;jl=lossy jpeg2000                *');
    writeln(f, ' * J3NN..j6NN, JLNN or J7NN overrides quality factor to NN% *');
    writeln(f, ' *                                                          *');
    writeln(f, ' ********************************************************** */');
    writeln(f, '');
    writeln(f, Trim(ServerName.text)+'		127.0.0.1	' + trim(TCPIPport.text) + '		un');
    writeln(f, '');
    writeln(f, 'V*	        	*               1234            un');
    writeln(f, 'W*	        	*               666             un');
    writeln(f, 'S*		        *               5678            un');
    CloseFile(f);

    DICOMMap.Lines.LoadFromFile(CurDir + '\acrnema.map');
    FillAELists;
  end;

  // create the DICOM.SQL file if required
  if not NewInstall then CreateDICOM_SQL;

  Registry := TRegistry.Create;

  {Registry.RootKey := HKEY_LOCAL_MACHINE;

  // list dicom servers from registry
  list := TStringList.Create;

  if Registry.OpenKey('SOFTWARE\NKI AVL\Conquest\Dicomserver', true) then
  begin
    Registry.GetKeyNames(list);
    Registry.CloseKey;
  end;

  // delete all dicomserver enries pointing to this directory
  for i:=0 to list.Count-1 do
  begin
    if Registry.OpenKey('SOFTWARE\NKI AVL\Conquest\Dicomserver\' + list[i], false) then
    begin
      if LowerCase(Registry.ReadString('directory')) = LowerCase(curdir) then
      begin
        Registry.CloseKey;
        Registry.DeleteKey('SOFTWARE\NKI AVL\Conquest\Dicomserver\' + list[i]);
      end
      else
        Registry.CloseKey;
    end;
  end;
  list.free;

  // write registry entry for this server so that dicomworks can find us
  if Registry.OpenKey('SOFTWARE\NKI AVL\Conquest\Dicomserver\' + Trim(ServerName.text), true) then
  begin
  try
    Registry.WriteString('directory', curdir);
    Registry.WriteString('laststarted', FormatDateTime('yyyymmdd', Date));
    Registry.WriteString('version', VERSION);
  except
  end;
  end;
  Registry.CloseKey;
  }

  // read the registry to know serverstatussocket of installed service (if any)
  text := '';
  Registry.RootKey := HKEY_LOCAL_MACHINE;
  if Registry.OpenKeyReadOnly('SYSTEM\CurrentControlSet\services\' + Trim(ServerName.text)) then
    text := Registry.ReadString('ImagePath');
  Registry.CloseKey;
  if text<>'' then
    WriteMemoSl(ServerStatusMemo, s, 1100, 1000, 'serverstatus', true, ServerStatusStringList);
  Registry.Free;

  i := pos('!', text);
  if i>0 then
  begin
    ServerStatusSocket.Close;
    ServerStatusSocket.Proto := 'udp';
    ServerStatusSocket.Addr:='127.0.0.1';
    ServerStatusSocket.Port:= copy(text, i+1, 4);

    try
      ServerStatusSocket.Listen;
    except
      ShowMessage('Address is in use: please close other servers on this PC and then restart this one.' +
            #13 + 'Otherwise the server status will be displayed elsewhere!');
    end;
  end
  else
    i := pos('^', text);

  // open the TCP/IP socket for listening to DICOM requests (not used in threaded mode)

  val(trim(TCPIPport.text), p, c);
  ListenSocket.Proto:= 'tcp';
  ListenSocket.Addr := '0.0.0.0';
  ListenSocket.Port := IntToStr(p);
  try
    ListenSocket.Listen;
  except
    // the port is occupied: is there a server running - ping it?
    AssignFile(f, //ExtractFileDir(ParamStr(0))
      CurDir +'\ConquestDICOMServer.Ping');
    Rewrite(f);
    Writeln(f, trim(TCPIPport.text));
    CloseFile(f);

    Sleep(2000);

    // the file ping was taken: another server is up
    if not FileExists(// ExtractFileDir(ParamStr(0))
      CurDir +'\ConquestDICOMServer.Ping') then
    begin
      if FileExists(// ExtractFileDir(ParamStr(0))
        CurDir +'\ConquestDICOMServer.Response') then
        DeleteFile(// ExtractFileDir(ParamStr(0))
        CurDir +'\ConquestDICOMServer.Response');
      ShowMessage('A server GUI seems already to be running for port: '+ trim(TCPIPport.text) + #13 + 'Only one may run at a time; please close it first.');
      Application.Terminate;
      exit;
    end;

    PreRunning := true;
    ServerNameChange(self);
  end;

  if (i>0) and not PreRunning then
  begin
    ListenSocket.Close;
    Sleep(500);
    RunProgramBlocking(curdir + '\dgateserv.exe', curdir,
                      ['/service', trim(ServerName.text),
                       '/start'
                      ]);
    ShowMessage('This server is installed as service but the service is not running' + #13 +
                'Attempting to start the '+trim(ServerName.text)+' service now....');
    Sleep(500);

    if not TestLocalServer(false, true) then
    begin
      ShowMessage('Failed to start service - you may need to run this program as administrator');
    end
    else
    begin
      PreRunning := true;
      ServerNameChange(self);
    end;
  end;

  InstallationMemo.Lines.Text :=
  'The installation options on this page can be run at any time without losing data.' + #13 +
  'However, active DICOM connections (image copy operations) may be terminated.' + #13 +
  #13 +
  'Follow the following steps for (re-)installing this program:' + #13 +
  #13 +
  'In the ''Configuration'' page, set the server name; port number and data directory.' + #13 +
  'The defaults will do fine in most cases. Push the ''Save configuration'' button.' + #13 +
  #13 +
  'In the ''Installation'' page (this one), perform all installation steps (buttons top' + #13 +
  'right) one by one from top to bottom.' + #13 +
  #13 +
  'Verify TCP/IP installation: tests communication between the DICOM server and installation' + #13 +
  'programs. This will fail if TCP/IP is not installed on this PC.' + #13 +
  #13 +
  'Make database: defines the connection between a database on disk and the DICOM' + #13 +
  'server. This fails if the database driver is not installed. This option is not required when using the ' + #13 +
  'SQLite or "DBASEIII without ODBC" driver or when you configured a database by hand.' + #13 +
  #13 +
  'Verify database installation: Tests database connectivity.' + #13 +
  #13 +
  '(Re-)initialize database: Deletes existing database tables (if they exist) and generates' + #13 +
  'new tables from any images already stored on disk (if any).' + #13 +
  'THIS MAKE TAKE A VERY LONG TIME FOR LARGE ARCHIVES.' + #13 +
  #13 +
  'After these steps, configure your other dicom systems to know this one and put them in '+ #13 +
  'in the list on the page ''Known DICOM providers''. '+ #13 +
  #13 +
  '--------------------------------------------------' + #13 +
  'Output of the installation programs will go here:' + #13;

  MaintenanceMemo.Lines.Add('All functions of this page are optional.');


  Memo1.Lines.Text :=
  'This page allows querying this and other dicom servers and moving data.' + #13 +
  '' + #13 +
  'In this memo window, after a query you can double click query results to populate the edit boxes' + #13 +
  'to make a subquery. The patient ID edit box accepts a comma separated list. Use ctrl-double click' + #13 +
  'on a patient ID in the memo to add a patient ID. Double click empty space to clear the relevant edit boxes.' + #13 +
  '' + #13 +
  'Double click the "Series number" label next the edit boxes to switch back and forth to querying on UID.' + #13 +
  '' + #13 +
  'The "Find Local Missing Patients" button compares the local server with another one and selects all'+#13+
  'patients with missing data.' + #13 +
  '';

  // ask to delete export failure files
  if FindFirst(curdir + '\ExportFailures*', faAnyFile, sr) = 0 then
  begin
    if Pos('.bak', sr.Name)<=0 then
      if MessageDlg('Starting server - keep on retrying export failures in '+sr.Name+' ?', mtConfirmation, [mbYes, mbNo], 0) = mrNo then
    begin
      if FileExists(curdir + '\' + sr.Name + '.bak') then
        DeleteFile(curdir + '\' + sr.Name + '.bak');
      RenameFile(curdir + '\' + sr.Name, curdir + '\' + sr.Name + '.bak');
    end;
    while FindNext(sr) = 0 do
      if Pos('.bak', sr.Name)<=0 then
        if MessageDlg('Keep on retrying export failures in '+sr.Name+' ?', mtConfirmation, [mbYes, mbNo], 0) = mrNo then
      begin
        if FileExists(curdir + '\' + sr.Name + '.bak') then
          DeleteFile(curdir + '\' + sr.Name + '.bak');
        RenameFile(curdir + '\' + sr.Name, curdir + '\' + sr.Name + '.bak');
      end;
  end;
  FindClose(sr);

  // ask to delete mirror copy failure files
  if FindFirst(curdir + '\CopyFailures*', faAnyFile, sr) = 0 then
  begin
    if Pos('.bak', sr.Name)<=0 then
      if MessageDlg('Starting server - Keep on retrying mirror copy failures in '+sr.Name+' ?', mtConfirmation, [mbYes, mbNo], 0) = mrNo then
    begin
      if FileExists(curdir + '\' + sr.Name + '.bak') then
        DeleteFile(curdir + '\' + sr.Name + '.bak');
      RenameFile(curdir + '\' + sr.Name, curdir + '\' + sr.Name + '.bak');
    end;
    while FindNext(sr) = 0 do
      if Pos('.bak', sr.Name)<=0 then
        if MessageDlg('Keep on retrying mirror copy failures in '+sr.Name+' ?', mtConfirmation, [mbYes, mbNo], 0) = mrNo then
      begin
        if FileExists(curdir + '\' + sr.Name + '.bak') then
          DeleteFile(curdir + '\' + sr.Name + '.bak');
        RenameFile(curdir + '\' + sr.Name, curdir + '\' + sr.Name + '.bak');
      end;
  end;
  FindClose(sr);

  // ask to delete mirror copy failure files
  if FindFirst(curdir + '\DelayedFetchForwardFailures*', faAnyFile, sr) = 0 then
  begin
    if Pos('.bak', sr.Name)<=0 then
      if MessageDlg('Starting server - Keep on retrying delayed fetch/forward failures in '+sr.Name+' ?', mtConfirmation, [mbYes, mbNo], 0) = mrNo then
    begin
      if FileExists(curdir + '\' + sr.Name + '.bak') then
        DeleteFile(curdir + '\' + sr.Name + '.bak');
      RenameFile(curdir + '\' + sr.Name, curdir + '\' + sr.Name + '.bak');
    end;
    while FindNext(sr) = 0 do
      if Pos('.bak', sr.Name)<=0 then
        if MessageDlg('Keep on retrying delayed fetch/forward failures in '+sr.Name+' ?', mtConfirmation, [mbYes, mbNo], 0) = mrNo then
      begin
        if FileExists(curdir + '\' + sr.Name + '.bak') then
          DeleteFile(curdir + '\' + sr.Name + '.bak');
        RenameFile(curdir + '\' + sr.Name, curdir + '\' + sr.Name + '.bak');
      end;
  end;
  FindClose(sr);

  if NewInstall then
  begin
    PageControl1.ActivePage := TabSheet1;
    TabSheet8.TabVisible := false;
    TapeBackupPage.TabVisible := false;
    ArchiveStatus := '&New Installation';

    NewInstallBox:=TNewInstallBox.Create(self);
    NewInstallBox.Version.Caption := 'vers. ' + VERSION;
    ModalResult := NewInstallBox.ShowModal;
    NewInstallBox.Free;
    if Modalresult = mrCancel then Application.Terminate;

    if FileExists(curdir + '\dicom.sql') then
    begin
      AssignFile(f, CurDir + '\dicom.sql');
      Reset(f);
      while not EOF(f) do
      begin
        Readln(f, s);
        if pos('Revision 16', s)>0 then break;
      end;
      CloseFile(f);
    end
    else
      s := '';

    if pos('Revision 16', s)<=0 then
    begin
      if FileExists(curdir + '\dicom.sql') then
      begin
        ShowMessage('Installation is about to overwrite dicom.sql - DATABASE REGEN IS MANDATORY BECAUSE THE DATABASE FORMAT WILL BE CHANGED');
        CopyFile(Pchar(curdir + '\dicom.sql'), Pchar(curdir + '\dicom.sql' + '.bak'), false);
        DeleteFile(curdir + '\dicom.sql');
      end;
    end
    else
    begin
      if FileExists(curdir + '\dicom.sql') then
      begin
        ShowMessage('Installation is about to overwrite dicom.sql - database regen is mandatory when db layout has changed');
        CopyFile(Pchar(curdir + '\dicom.sql'), Pchar(curdir + '\dicom.sql' + '.bak'), false);
        DeleteFile(curdir + '\dicom.sql');
      end;
    end;

    CreateDICOM_SQL;    // always write file (format depends on dbase)

{$I-}
    DataDir := Curdir + '\Data';
    MagDeviceList[0] := DataDir + '\';
    if not SysUtils.DirectoryExists(DataDir) then
      Mkdir(DataDir);
    if not SysUtils.DirectoryExists(Curdir + '\logs') then
      Mkdir(Curdir + '\logs');
    Label10.Caption := DataDir;
{$I+}
  end
  else
  begin
    PageControl1.ActivePage := TabSheet6;

    // Switch to threaded mode and stay there (button is disabled)
    if not PreRunning then
    begin
      RunInThreadedModeClick(self);
    end;
  end;

  PrinterQueue:= TThreadList.Create;

  if FileExists(curdir + '\USEDBASEIIIWITHOUTODBC') then
  begin
    MakeODBCButton.Enabled     := false;
    UpdateODBCButton.Enabled   := false;
    if not FileExists(CurDir + '\dicom.ini') then
      Datasource := DataDir + '\dbase\';
  end
  else
    ButtonPackDatabase.Enabled := false;

  if FileExists(curdir + '\USEMYSQL') then
  begin
    MakeODBCButton.Caption   := 'Make mysql database';
    UpdateODBCButton.Caption := 'Make mysql database';
    if not FileExists(CurDir + '\dicom.ini') then
    begin
      Datasource          := 'conquest';
      SqlUser             := 'root';
      SqlPassWord         := '';
      MySql               := '1';
      DoubleBackSlashToDB := '1';
      UseEscapeStringConstants := '0';
    end;
  end;

  if FileExists(curdir + '\USESQLITE') then
  begin
    MakeODBCButton.Enabled     := false;
    UpdateODBCButton.Enabled   := false;
    if not FileExists(CurDir + '\dicom.ini') then
    begin
      Datasource          := DataDir + '\dbase\conquest.db3';
      SqlUser             := '';
      SqlPassWord         := '';
      FSqLite             := '1';
      BrowseThroughDBF    := '1';
      DoubleBackSlashToDB := '0';
      UseEscapeStringConstants := '0';
    end;
  end;

  if FileExists(curdir + '\USEPOSTGRES') then
  begin
    MakeODBCButton.Caption   := 'Make postgres database';
    UpdateODBCButton.Caption := 'Make postgres database';
    if not FileExists(CurDir + '\dicom.ini') then
    begin
      Datasource          := 'conquest';
      SqlUser             := 'conquest';
      SqlPassWord         := 'conquest';
      FPostGres           := '1';
      BrowseThroughDBF    := '1';
      DoubleBackSlashToDB := '1';
      UseEscapeStringConstants := '1';
      UTF8ToDB            := '1';
      UTF8FromDB          := '1';
    end;
  end;

  if FileExists(curdir + '\USESQLSERVER') then
  begin
    MakeODBCButton.Caption   := 'Make ODBC and database';
    UpdateODBCButton.Caption := 'Make ODBC and database';
    if DgateExe = 'dgate64.exe' then
      UpdateODBCButton.Hint := 'May not work well under 64 bits windows';
    DoubleBackSlashToDB := '0';
    UseEscapeStringConstants := '0';
    if not FileExists(CurDir + '\dicom.ini') then
      BrowseThroughDBF    := '1';
  end;

  // default install button
  if Modalresult = mrYes then
  begin
    SaveConfigButtonClick(self);          sleep(1000);
    if (not FileExists(curdir + '\USESQLITE')) and (not FileExists(curdir + '\USEDBASEIIIWITHOUTODBC')) then
    begin
      MakeODBCButtonClick(self);          sleep(1000);
    end;
    InitDatabaseButtonClick(self);        sleep(1000);
    PageControl1.ActivePage := TabSheet5;
    PageControl1Change(self);
  end;
end;

var ShowOptions: boolean = true;

// enable KeyPreview in form
procedure TForm1.FormKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
  ShowOptions := true;
  if PageControl1.ActivePage=TabSheet5 then
  begin
    if Key=VK_F3 then begin if Table1.Active then Table1.Next; Key:=0; end;
    if Key=VK_F4 then begin if Table1.Active then Table1.Prior; Key:=0; end;
    if Key=VK_F5 then begin if Table2.Active then Table2.Next; Key:=0; end;
    if Key=VK_F6 then begin if Table2.Active then Table2.Prior; Key:=0; end;
    if Key=VK_F7 then begin if Table3.Active then Table3.Next; Key:=0; end;
    if Key=VK_F8 then begin if Table3.Active then Table3.Prior; Key:=0; end;
    if Key=VK_F9 then begin if Table4.Active then Dumpheader1Click(Sender); Key:=0; end;
    if Key=VK_F2 then begin if Table4.Active then RunKPacsviewer1Click(Sender); Key:=0; end;
    if Key=VK_F11 then begin if Table4.Active then EditDatabasebyhand1Click(Sender); Key:=0; end;
    if Key=VK_F12 then begin if Table4.Active then EditDatabasebyhand1Click(Sender); Key:=0; end;

    if Key=VK_ADD then
    begin
      ShowOptions := false;
      if not Table4.Active then
        DBGrid1DblClick(Sender)
      else
        Table4.Next;
      DBGrid3.SetFocus;
      Key:=0;
    end;
    if Key=vkSubtract then
    begin
      ShowOptions := false;
      if not Table4.Active then
        DBGrid1DblClick(Sender)
      else
        Table4.Prior;
      DBGrid3.SetFocus;
      Key:=0;
    end;
    if Key=VK_PRIOR then
    begin
      ShowOptions := false;
      Table3.Prior;
      DBGrid1DblClick(Sender);
      DBGrid3.SetFocus;
      Key:=0;
    end;
    if Key=VK_NEXT then
    begin
      ShowOptions := false;
      Table3.Next;
      DBGrid1DblClick(Sender);
      DBGrid3.SetFocus;
      Key:=0;
    end;
  end;
end;

var first:boolean=true;

procedure TForm1.FormActivate(Sender: TObject);
begin
{$IFNDEF KPACS}
  RunKPacsviewer1.Caption := 'Run web viewer (F2)';
{$ENDIF KPACS}

  if not first then exit;
  first := false;
  if NewInstall then exit;

  ServerStatusMemo.Lines.Add('Starting server (may take a while - especially for large DbaseIII based archives)....');
  Form1.Update;
  ServerStatusMemo.Lines.Clear;

  Sleep(2000);
  Timer1.Enabled := false;
  Timer2.Enabled := false;
  Timer5.Enabled := false;

  if PreRunning then
  begin
    ServerTask('', 'log_on:'+ServerStatusSocket.Port);
    Sleep(500);
  end;

  if FileExists(curdir + '\USESQLSERVER') then
  begin
    if not TestLocalServer(false, true) then
      WriteMemoSl(ServerStatusMemo, 'The server is not running properly - maybe the SQL server is not (yet) up', 1100, 1000, 'serverstatus', true, ServerStatusStringList);
  end
  else
    TestLocalServer(false, false);

  if FileExists(curdir + '\USEDBASEIIIWITHOUTODBC') or FileExists(curdir + '\USESQLITE') then
  begin
    MakeODBCButton.Enabled := false;
    UpdateODBCButton.Enabled := false;
  end;

  Timer2.Enabled := true;
  Timer5.Enabled := true;
end;

// seperate settings string like 'xxxx' from ' entry =   xxxx   '

function GetData(s: string): string;
var i, pastis: integer;
begin
  pastis := 0;
  result := '';
  for i:=1 to length(s) do
  begin
    if      (pastis=0) and (s[i] =  '=') then pastis := 1
    else if (pastis=1) and (s[i+1]<>' ') then pastis := 2
    else if (pastis=2) then result := result + s[i];
  end;

  result := trim(result);
end;

function GetKey(s: string; key: string): string;
var i, j: integer;
var t: string;
begin
  i := Pos('] ', s);
  if i>0 then s:=copy(s, i+2, 9999);
  i := Pos(#10, s);
  if i>0 then s:=copy(s, 1, i-1);
  result := '';
  t := ', '+s;
  i := Pos(key+'=', t);
  if (i<1) then exit;
  j := Pos(',', t, i+1);
  if j=0 then j:=Pos(#10, t, i+1);
  if j=0 then j:=Length(t)+1;
  result := trim(copy(t, i+Length(key)+1, j-(i+Length(key)+1)));
end;

procedure TForm1.RestoreconfigButtonClick(Sender: TObject);
var f: TextFile;
    s, sorg: string;
    i, j, k, n: integer;
    t: TTimeStamp;
    RemoteAE, RemoteIP, RemotePort, target: AnsiString;
    inmainsection: boolean;
begin
  // auto-generated prefix for UIDs
  if UIDPrefix='' then
  begin
    t := DateTimeToTimeStamp(Now);
    UIDPrefix := '1.2.826.0.1.3680043.2.135.' + IntToStr(t.Date) + '.' + IntToStr(t.Time);
  end;

  // Read the dicom.ini file with server configuration
  if not FileExists(//ExtractFileDir(ParamStr(0))
    CurDir + '\dicom.ini') then exit;

  // some defaults
  Runexternalviewer1.visible := false;
  Timer4.Enabled := false;
  ZipTime := '05:';

  AssignFile(f, //ExtractFileDir(ParamStr(0))
    CurDir + '\dicom.ini');
  Reset(f);

  FileCompressMode := -1;       // not-set
  inmainsection := true;
  othersections.Clear;
  
  while not Eof(f) do
  begin
    Readln(f, sorg);
    s := LowerCase(sorg);

    if copy(s, 0, length('[sscscp]')) = '[sscscp]' then
      inmainsection := true
    else if copy(s, 0, length('[')) = '[' then
      inmainsection := false;
      
    if not inmainsection then
    begin
      othersections.add(sorg);
      continue;
    end;
      
    if Copy(s, 0, length('myacrnema')) = 'myacrnema' then
    begin
      ServerName.text := UpperCase(GetData(s));
    end;

    if Copy(s, 0, length('filecompressmode')) = 'filecompressmode' then
      FileCompressMode := StrToIntDef(GetData(s), 0);

    if Copy(s, 0, length('droppedfilecompression')) = 'droppedfilecompression' then
      DroppedFileCompression := GetData(s);

    if Copy(s, 0, length('incomingcompression')) = 'incomingcompression' then
      IncomingCompression := GetData(s);

    if Copy(s, 0, length('archivecompression')) = 'archivecompression' then
      ArchiveCompression := GetData(s);
      
    if Copy(s, 0, length('tcpport')) = 'tcpport' then
      TCPIPport.text := GetData(s);

    if Copy(s, 0, length('tcpport')) = 'tcpport' then
      TCPIPport.text := GetData(s);

    if Copy(s, 0, length('sqlhost')) = 'sqlhost' then
      SQLHost := GetData(sorg);

    if Copy(s, 0, length('mysql')) = 'mysql' then
      mysql := GetData(sorg);

    if Copy(s, 0, length('sqlite')) = 'sqlite' then
      FSqLite := GetData(sorg);

    if Copy(s, 0, length('postgres')) = 'postgres' then
      FPostgres := GetData(sorg);

    if Copy(s, 0, length('sqlserver')) = 'sqlserver' then
      DataSource := GetData(sorg);

    if Copy(s, 0, length('username')) = 'username' then
      SqlUser := GetData(sorg);

    if Copy(s, 0, length('password')) = 'password' then
      SqlPassWord := GetData(sorg);

    if Copy(s, 0, length('filenamesyntax')) = 'filenamesyntax' then
      FileNameSyntax := GetData(sorg);

    if Copy(s, 0, length('allowemptypatientid')) = 'allowemptypatientid' then
      AllowEmptyPatientID := GetData(sorg);

    if Copy(s, 0, length('maxfilenamelength')) = 'maxfilenamelength' then
      MaxFileNameLength := GetData(s);

    if Copy(s, 0, length('fixphilips')) = 'fixphilips' then
      FixPhilips := GetData(s);

    if Copy(s, 0, length('fixkodak')) = 'fixkodak' then
      FixKodak := GetData(s);

    if Copy(s, 0, length('doublebackslashtodb')) = 'doublebackslashtodb' then
      DoubleBackSlashToDB := GetData(s);

    if Copy(s, 0, length('useescapestringconstants')) = 'useescapestringconstants' then
      UseEscapeStringConstants := GetData(s);

    if Copy(s, 0, length('utf8fromdb')) = 'utf8fromdb' then
      UTF8FromDB := GetData(s);

    if Copy(s, 0, length('utf8todb')) = 'utf8todb' then
      UTF8ToDB := GetData(s);

    if Copy(s, 0, length('magdevice0')) = 'magdevice0' then
    begin
      DataDir := copy(GetData(s), 1, Length(GetData(s))-1); // remove final '\'
      MagDeviceList[0] := DataDir + '\';
      if DataDir[1] <> '\' then
      begin
        Label10.Caption := MagDeviceList[0];
        Label5.Caption :=
          Format('Free disk space: %d gigabyte', [DiskFreeGB(Label10.Caption[1])]);

      end
      else
      begin
        Label10.Caption := MagDeviceList[0];
        Label5.Caption := 'Free disk space: unknown';
      end
    end;

    if Copy(s, 0, length('magdevicethreshhold')) = 'magdevicethreshhold' then
      MagThreshold.text := GetData(s);
    if Copy(s, 0, length('magdevicethreshold')) = 'magdevicethreshold' then
      MagThreshold.text := GetData(s);

    if Copy(s, 0, length('magdevicefullthreshhold')) = 'magdevicefullthreshhold' then
      MAGDeviceFullThreshold := GetData(s);
    if Copy(s, 0, length('magdevicefullthreshold')) = 'magdevicefullthreshold' then
      MAGDeviceFullThreshold := GetData(s);

    if Copy(s, 0, length('ignoremagdevicethreshhold')) = 'ignoremagdevicethreshhold' then
      IgnoreMagDeviceThreshold := GetData(s);
    if Copy(s, 0, length('ignoremagdevicethreshold')) = 'ignoremagdevicethreshold' then
      IgnoreMagDeviceThreshold := GetData(s);

    if Copy(s, 0, length('nightlycleanthreshhold')) = 'nightlycleanthreshhold' then
      NightlyTreshold.text := GetData(s);
    if Copy(s, 0, length('nightlycleanthreshold')) = 'nightlycleanthreshold' then
      NightlyTreshold.text := GetData(s);

    if Copy(s, 0, length('nightlymovethreshhold')) = 'nightlymovethreshhold' then
      EditNightlyMoveTreshold.text := GetData(s);
    if Copy(s, 0, length('nightlymovethreshold')) = 'nightlymovethreshold' then
      EditNightlyMoveTreshold.text := GetData(s);

    if Copy(s, 0, length('nightlymovetarget')) = 'nightlymovetarget' then
      target := GetData(s);
    // Luiz added at Sept 19 2021

    if Copy(s, 0, length('nightlystrtimetomove')) = 'nightlystrtimetomove' then begin
        NightlyStrTimeToMove := GetData(s);
        if length(NightlyStrTimeToMove) > 2 then
          if NightlyStrTimeToMove[2]=':' then NightlyStrTimeToMove := '0' + NightlyStrTimeToMove;
        NightlyStrTimeToMoveText.text := NightlyStrTimeToMove;
    end;

    if Copy(s, 0, length('magdevices')) = 'magdevices' then
      MagDevices := StrToIntDef(GetData(s), 1);

    if Copy(s, 0, length('cachedevices')) = 'cachedevices' then
      CacheDevices := StrToIntDef(GetData(s), 0);

    if Copy(s, 0, length('mirrordevices')) = 'mirrordevices' then
      MirrorDevices := StrToIntDef(GetData(s), 0);

    if Copy(s, 0, length('jukeboxdevices')) = 'jukeboxdevices' then
      JukeboxDevices := StrToIntDef(GetData(s), 0);

    if Copy(s, 0, length('externalviewer')) = 'externalviewer' then
    begin
      Runexternalviewer1.visible := true;
      ExternalViewer := GetData(s);
    end;

    if Copy(s, 0, length('ladleport')) = 'ladleport' then
      LadlePort := GetData(s);

    if Copy(s, 0, length('demoviewer')) = 'demoviewer' then
      DemoViewer := GetData(s);

    if Copy(s, 0, length('democopy')) = 'democopy' then
      DemoCopy := GetData(s);

    if Copy(s, 0, length('keepalive')) = 'keepalive' then
    begin
      KeepAlive := StrToIntDef(GetData(s), 0);
      Timer4.Interval := KeepAlive * 1000;
      Timer4.Enabled := KeepAlive <> 0;
      CheckBoxKeepAlive.Checked := KeepAlive <> 0;
    end;

    if Copy(s, 0, length('ziptime')) = 'ziptime' then
    begin
      ZipTime := GetData(s);
      if length(ZipTime) > 2 then
        if ZipTime[2]=':' then ZipTime := '0' + ZipTime;
    end;

    if Copy(s, 0, length('uidprefix')) = 'uidprefix' then
      UIDPrefix := GetData(s);

    if Copy(s, 0, length('enablereadaheadthread')) = 'enablereadaheadthread' then
      EnableReadAheadThread := StrToIntDef(GetData(s), 1);

    if Copy(s, 0, length('StorageFailedErrorCode')) = 'storagefailederrorcode' then
      StorageFailedErrorCode := StrToIntDef(GetData(s), 1);

    if Copy(s, 0, length('SendUpperCaseAE')) = 'senduppercaseae' then
      SendUpperCaseAE := StrToIntDef(GetData(s), 1);

    if Copy(s, 0, length('PatientQuerySortOrder')) = 'patientquerysortorder' then
      PatientQuerySortOrder := GetData(s);

    if Copy(s, 0, length('StudyQuerySortOrder')) = 'studyquerysortorder' then
      StudyQuerySortOrder := GetData(s);

    if Copy(s, 0, length('SeriesQuerySortOrder')) = 'seriesquerysortorder' then
      SeriesQuerySortOrder := GetData(s);

    if Copy(s, 0, length('ImageQuerySortOrder')) = 'imagequerysortorder' then
      ImageQuerySortOrder := GetData(s);

    if Copy(s, 0, length('EnableComputedFields')) = 'enablecomputedfields' then
      EnableComputedFields := GetData(s);

    if Copy(s, 0, length('IndexDBF')) = 'indexdbf' then
      IndexDBF := GetData(s);

    if Copy(s, 0, length('PackDBF')) = 'packdbf' then
      PackDBF := GetData(s);

    if Copy(s, 0, length('LongQueryDBF')) = 'longquerydbf' then
      LongQueryDBF := GetData(s);

    if Copy(s, 0, length('TCPIPTimeOut')) = 'tcpiptimeout' then
      TCPIPTimeOut := GetData(s);

    if Copy(s, 0, length('FailHoldOff')) = 'failholdoff' then
      FailHoldOff := GetData(s);

    if Copy(s, 0, length('RetryDelay')) = 'retrydelay' then
      RetryDelay := GetData(s);

    if Copy(s, 0, length('ImportExportDragAndDrop')) = 'importexportdraganddrop' then
      ImportExportDragAndDrop := GetData(s);

    if Copy(s, 0, length('RetryForwardFailed')) = 'retryforwardfailed' then
      RetryForwardFailed := GetData(s);

    if Copy(s, 0, length('QueueSize')) = 'queuesize' then
      QueueSize := GetData(s);

    if Copy(s, 0, length('WorkListMode')) = 'worklistmode' then
      WorkListMode := GetData(s);

    if Copy(s, 0, length('WorkListReturnsISO_IR_100')) = 'worklistreturnsiso_ir_100' then
      WorkListReturnsISO_IR_100 := GetData(s);

    if Copy(s, 0, length('exportconverters')) = 'exportconverters' then
      ExportConverters := StrToIntDef(GetData(s), 0);

    if Copy(s, 0, length('importconverters')) = 'importconverters' then
      ImportConverters := StrToIntDef(GetData(s), 999);

    if Copy(s, 0, length('ForwardAssociationLevel')) = 'forwardassociationlevel' then
      ForwardAssociationLevel := GetData(s);

    if Copy(s, 0, length('ForwardAssociationCloseDelay')) = 'forwardassociationclosedelay' then
      ForwardAssociationCloseDelay := StrToIntDef(GetData(s), 5);

    if Copy(s, 0, length('ForwardAssociationRefreshDelay')) = 'forwardassociationrefreshdelay' then
      ForwardAssociationRefreshDelay := StrToIntDef(GetData(s), 3600);

    if Copy(s, 0, length('ForwardAssociationRelease')) = 'forwardassociationrelease' then
      ForwardAssociationRelease := StrToIntDef(GetData(s), 1);

    if Copy(s, 0, length('Prefetcher')) = 'prefetcher' then
      Prefetcher := GetData(s);

    if Copy(s, 0, length('debuglevel')) = 'debuglevel' then
    begin
      UpDownDebugLevel.Position := StrToIntDef(GetData(s), 0);
    end;

    if Copy(s, 0, length('cachevirtualdata')) = 'cachevirtualdata' then
      CacheVirtualData := GetData(s);

    if Copy(s, 0, length('largefilesizekb')) = 'largefilesizekb' then
      LargeFileSizeKB := StrToIntDef(GetData(s), 0);

    if Copy(s, 0, length('printsquarelandscape')) = 'printsquarelandscape' then
      PrintSquareLandscape := StrToIntDef(GetData(s), 0);

    if Copy(s, 0, length('usekpacsdecompression')) = 'usekpacsdecompression' then
      UseKpacsDecompression := StrToIntDef(GetData(s), 0);

    if Copy(s, 0, length('lrusort')) = 'lrusort' then
      LRUSort := GetData(s);

    if Copy(s, 0, length('allowtruncate')) = 'allowtruncate' then
      AllowTruncate := GetData(s);

    if Copy(s, 0, length('decompressnon16bitsjpeg')) = 'decompressnon16bitsjpeg' then
      DecompressNon16BitsJpeg := IntToStr(StrToIntDef(GetData(s), 1));

    if Copy(s, 0, length('usebuiltindecompressor')) = 'usebuiltindecompressor' then
      UseBuiltInJPEG := IntToStr(StrToIntDef(GetData(s), 1));

    if Copy(s, 0, length('usebuiltinjpeg')) = 'usebuiltinjpeg' then
      UseBuiltInJPEG := IntToStr(StrToIntDef(GetData(s), StrToInt(UseBuiltInJPEG)));

    if Copy(s, 0, length('lossyquality')) = 'lossyquality' then
      LossyQuality := IntToStr(StrToIntDef(GetData(s), 95));

//    if Copy(s, 0, length('useopenjpeg')) = 'useopenjpeg' then
//      UseOpenJPEG := IntToStr(StrToIntDef(GetData(s), 0));

    if Copy(s, 0, length('ignoreoutofmemoryerrors')) = 'ignoreoutofmemoryerrors' then
      IgnoreOutOfMemoryErrors := IntToStr(StrToIntDef(GetData(s), 0));

    if Copy(s, 0, length('nodicomcheck')) = 'nodicomcheck' then
      NoDICOMCheck := IntToStr(StrToIntDef(GetData(s), 0));

      if Copy(s, 0, length('padaewithzeros')) = 'padaewithzeros' then
      PadAEWithZeros := IntToStr(StrToIntDef(GetData(s), 0));

    if Copy(s, 0, length('forwardcollectdelay')) = 'forwardcollectdelay' then
      ForwardCollectDelay := IntToStr(StrToIntDef(GetData(s), 600));

    if Copy(s, 0, length('maximumexportretries')) = 'maximumexportretries' then
      MaximumExportRetries := IntToStr(StrToIntDef(GetData(s), 0));

    if Copy(s, 0, length('maximumdelayedfetchforwardretries')) = 'maximumdelayedfetchforwardretries' then
      MaximumDelayedFetchForwardRetries := IntToStr(StrToIntDef(GetData(s), 0));

    if Copy(s, 0, length('overlapvirtualget')) = 'overlapvirtualget' then
      OverlapVirtualGet := IntToStr(StrToIntDef(GetData(s), 0));

    if Copy(s, 0, length('retryforwardremotedicomerror')) = 'retryforwardremotedicomerror' then
      RetryForwardRemoteDICOMError := IntToStr(StrToIntDef(GetData(s), 0));
      
    for i:=0 to 9 do
    begin
      if Copy(s, 0, length('magdevice0')) = 'magdevice' + IntToStr(i) then
      begin
        MagDeviceList[i] := GetData(s);
        if MagDeviceList[i]<>'' then
          if (MagDeviceList[i][Length(MagDeviceList[i])]<>'\') and (pos('/', MagDeviceList[i])=0) then
          begin
            MagDeviceList[i] := MagDeviceList[i] + '\';
            ShowMessage('Added missing \ to MagDevice'+IntToStr(i)+' - please fix dicom.ini');
          end;
      end;
      if Copy(s, 0, length('cachedevice0')) = 'cachedevice' + IntToStr(i) then
      begin
        CacheDeviceList[i] := GetData(s);
        if CacheDeviceList[i]<>'' then
          if (CacheDeviceList[i][Length(CacheDeviceList[i])]<>'\') and (pos('/', CacheDeviceList[i])=0) then
          begin
            CacheDeviceList[i] := CacheDeviceList[i] + '\';
            ShowMessage('Added missing \ to CacheDevice'+IntToStr(i)+' - please fix dicom.ini');
          end;
      end;
      if Copy(s, 0, length('jukeboxdevice0')) = 'jukeboxdevice' + IntToStr(i) then
      begin
        JukeBoxDeviceList[i] := GetData(s);
        if JukeBoxDeviceList[i]<>'' then
          if (JukeBoxDeviceList[i][Length(JukeBoxDeviceList[i])]<>'\') and (pos('/', JukeBoxDeviceList[i])=0) then
          begin
            JukeBoxDeviceList[i] := JukeBoxDeviceList[i] + '\';
            ShowMessage('Added missing \ to JukeBoxDevice'+IntToStr(i)+' - please fix dicom.ini');
          end;
      end;
      if Copy(s, 0, length('mirrordevice0')) = 'mirrordevice' + IntToStr(i) then
      begin
        MirrorDeviceList[i] := GetData(s);
        if MirrorDeviceList[i]<>'' then
          if (MirrorDeviceList[i][Length(MirrorDeviceList[i])]<>'\') and (pos('/', MirrorDeviceList[i])=0) then
          begin
            MirrorDeviceList[i] := MirrorDeviceList[i] + '\';
            ShowMessage('Added missing \ to MirrorDevice'+IntToStr(i)+' - please fix dicom.ini');
          end;
      end;

      if Copy(s, 0, length('exportmodality'+ IntToStr(i))) = 'exportmodality' + IntToStr(i) then
        ExportModalityList[i] := UpperCase(GetData(s));
      if Copy(s, 0, length('exportstationname'+ IntToStr(i))) = 'exportstationname' + IntToStr(i) then
        ExportStationNameList[i] := GetData(sorg);
      if Copy(s, 0, length('exportfilter'+ IntToStr(i))) = 'exportfilter' + IntToStr(i) then
        ExportFilterList[i] := GetData(sorg);
      if Copy(s, 0, length('exportconverter'+ IntToStr(i))) = 'exportconverter' + IntToStr(i) then
        ExportConverterList[i] := GetData(sorg);
      if Copy(s, 0, length('exportcalledae'+ IntToStr(i))) = 'exportcalledae' + IntToStr(i) then
        ExportCalledAEList[i] := GetData(sorg);
      if Copy(s, 0, length('exportcallingae'+ IntToStr(i))) = 'exportcallingae' + IntToStr(i) then
        ExportCallingAEList[i] := GetData(sorg);

      if Copy(s, 0, length('importmodality'+ IntToStr(i))) = 'importmodality' + IntToStr(i) then
        ImportModalityList[i] := UpperCase(GetData(s));
      if Copy(s, 0, length('importstationname'+ IntToStr(i))) = 'importstationname' + IntToStr(i) then
        ImportStationNameList[i] := GetData(sorg);
      if Copy(s, 0, length('importconverter'+ IntToStr(i))) = 'importconverter' + IntToStr(i) then
        ImportConverterList[i] := GetData(sorg);
      if Copy(s, 0, length('importcalledae'+ IntToStr(i))) = 'importcalledae' + IntToStr(i) then
        ImportCalledAEList[i] := GetData(sorg);
      if Copy(s, 0, length('importcallingae'+ IntToStr(i))) = 'importcallingae' + IntToStr(i) then
        ImportCallingAEList[i] := GetData(sorg);

      if Copy(s, 0, length('virtualserverfor'+ IntToStr(i))) = 'virtualserverfor' + IntToStr(i) then
        VirtualServerForList[i] := GetData(sorg);

      if Copy(s, 0, length('virtualperseries'+ IntToStr(i))) = 'virtualperseries' + IntToStr(i) then
        VirtualPerSeries[i] := StrToIntDef(GetData(sorg), 0);

      if Copy(s, 0, length('RetrieveResultConverter'+ IntToStr(i))) = 'retrieveresultconverter' + IntToStr(i) then
        RetrieveResultConverter[i] := GetData(sorg);
      if Copy(s, 0, length('QueryConverter'+ IntToStr(i))) = 'queryconverter' + IntToStr(i) then
        QueryConverter[i] := GetData(sorg);
      if Copy(s, 0, length('WorkListQueryConverter'+ IntToStr(i))) = 'worklistqueryconverter' + IntToStr(i) then
        WorkListQueryConverter[i] := GetData(sorg);
      if Copy(s, 0, length('RetrieveConverter'+ IntToStr(i))) = 'retrieveconverter' + IntToStr(i) then
        RetrieveConverter[i] := GetData(sorg);
      if Copy(s, 0, length('QueryResultConverter'+ IntToStr(i))) = 'queryresultconverter' + IntToStr(i) then
        QueryResultConverter[i] := GetData(sorg);
      if Copy(s, 0, length('ModalityWorkListQueryResultConverter'+ IntToStr(i))) = 'modalityworkListqueryresultconverter' + IntToStr(i) then
        ModalityWorkListQueryResultConverter[i] := GetData(sorg);
      if Copy(s, 0, length('MergeSeriesConverter'+ IntToStr(i))) = 'mergeseriesconverter' + IntToStr(i) then
        MergeSeriesConverter[i] := GetData(sorg);
      if Copy(s, 0, length('MergeStudiesConverter'+ IntToStr(i))) = 'mergestudiesconverter' + IntToStr(i) then
        MergeStudiesConverter[i] := GetData(sorg);
      if Copy(s, 0, length('ArchiveConverter'+ IntToStr(i))) = 'archiveconverter' + IntToStr(i) then
        ArchiveConverter[i] := GetData(sorg);
      if Copy(s, 0, length('MoveDeviceConverter'+ IntToStr(i))) = 'movedeviceconverter' + IntToStr(i) then
        MoveDeviceConverter[i] := GetData(sorg);
      if Copy(s, 0, length('RejectedImageConverter'+ IntToStr(i))) = 'rejectedimageconverter' + IntToStr(i) then
        RejectedImageConverter[i] := GetData(sorg);
      if Copy(s, 0, length('RejectedImageWorkListConverter'+ IntToStr(i))) = 'rejectedimageworklistconverter' + IntToStr(i) then
        RejectedImageWorkListConverter[i] := GetData(sorg);
      if Copy(s, 0, length('VirtualServerQueryConverter'+ IntToStr(i))) = 'virtualserverqueryconverter' + IntToStr(i) then
        VirtualServerQueryConverter[i] := GetData(sorg);
      if Copy(s, 0, length('VirtualServerQueryResultConverter'+ IntToStr(i))) = 'virtualserverqueryresultconverter' + IntToStr(i) then
        VirtualServerQueryResultConverter[i] := GetData(sorg);
    end;

    if Copy(s, 0, length('mailhost')) = 'mailhost' then
      mailer.Host       := GetData(sorg);

    if Copy(s, 0, length('mailport')) = 'mailport' then
      mailer.Port       := GetData(sorg);

    if Copy(s, 0, length('mailsignon')) = 'mailsignon' then
      mailer.Signon     := GetData(sorg);

    if Copy(s, 0, length('mailfromname')) = 'mailfromname' then
      mailer.FromName   := GetData(sorg);

    if Copy(s, 0, length('mailrcptname1')) = 'mailrcptname1' then
      mailer.HdrTo      := GetData(sorg);

    for i:=2 to 9 do
      if Copy(s, 0, length('mailrcptname1')) = 'mailrcptname'+IntToStr(i) then
        mailer.HdrTo    := mailer.HdrTo + ',' + GetData(sorg);

    for i:=1 to 9 do
      if Copy(s, 0, length('mailrcptname1')) = 'mailrcptname'+IntToStr(i) then
        mailer.RcptName.Add(GetData(sorg));

    if Copy(s, 0, length('mailcollecttime')) = 'mailcollecttime' then
      MailCollectTime   := StrToIntDef(GetData(sorg), 1);

    if Copy(s, 0, length('mailwaittime')) = 'mailwaittime' then
      MailWaitTime   := StrToIntDef(GetData(sorg), 10);

    if Copy(s, 0, length('headerbmp')) = 'headerbmp' then
      headerbmpname := GetData(sorg);

    if Copy(s, 0, length('footerbmp')) = 'footerbmp' then
      footerbmpname := GetData(sorg);

    if Copy(s, 0, length('backgroundbmp')) = 'backgroundbmp' then
      backgroundbmpname := GetData(sorg);
  end;

  if FileCompressMode=0 then
  begin
    DroppedFileCompression := 'un';     // uncompressed
    IncomingCompression    := 'un';
  end;

  if FileCompressMode>0 then
  begin
    DroppedFileCompression := 'n' + IntToStr(FileCompressMode); // nki 1..4
    IncomingCompression    := 'n' + IntToStr(FileCompressMode);
  end;

  UncompressedButton.Checked             := IncomingCompression    = 'un';
  CompressNKIButton.Checked              := IncomingCompression[1] = 'n';
  CompressLosslessJPEGButton.Checked     := (IncomingCompression[1] in ['j', 'J']) and (IncomingCompression[2] in ['1', '2']);
  CompressLossyJPEGButton.Checked        := (IncomingCompression[1] in ['j', 'J']) and (IncomingCompression[2] in ['3'..'9']);
  CompressLosslessJPEG2000Button.Checked := (IncomingCompression[1] in ['j', 'J']) and (IncomingCompression[2] in ['k', 'K']);
  CompressLossyJPEG2000Button.Checked    := (IncomingCompression[1] in ['j', 'J']) and (IncomingCompression[2] in ['l', 'L']);
  CompressLosslessJPEGLSButton.Checked   := (IncomingCompression[1] in ['j', 'J']) and (IncomingCompression[2] in ['s', 'S']);
  CompressLossyJPEGLSButton.Checked      := (IncomingCompression[1] in ['j', 'J']) and (IncomingCompression[2] in ['7']);
  ModeNJRadioButton.Checked              := (IncomingCompression = 'nj') or (IncomingCompression = 'NJ');
  ModeUJRadioButton.Checked              := (IncomingCompression = 'uj') or (IncomingCompression = 'UJ');
  LabelCompression.Hint                  := 'IncomingCompression (double click to edit) = ' + IncomingCompression;

  i := StrtoIntDef(FileNameSyntax, 99);
  V2RadioButton.Checked              := (i in [0, 1, 2, 3, 5, 6, 7]) or (pos('.v2', LowerCase(FileNameSyntax))>0);
  DCMRadioButton.Checked             := (i in [99, 4, 8, 9, 10, 11, 12]) or (pos('.dcm', LowerCase(FileNameSyntax))>0);
  CompressNKIButton.Enabled          := V2RadioButton.Checked;
  ModeNJRadioButton.Enabled          := V2RadioButton.Checked and EnableJPEGCheckBox.Checked;
  LabelImageNaming.Hint              := 'FileNameSyntax (double click to edit) = ' + FileNameSyntax;

  Closefile(f);

  VerifyMirrorDisk.Enabled := MirrorDevices > 0;
  CheckBoxVerifyMirrorDisk.Enabled := MirrorDevices > 0;

  ComboBoxMoveTarget.Enabled := MagDevices>1;
  EditNightlyMoveTreshold.Enabled := MagDevices>1;
  NightlyStrTimeToMoveLabel.Enabled := MagDevices>1;
  NightlyStrTimeToMoveText.Enabled := MagDevices>1;
  Label36.Enabled := MagDevices>1;
  Label37.Enabled := MagDevices>1;
  ComboBoxMoveTarget.Items.Clear;
  ComboBoxMoveTarget.Items.Add('None');
  for i:=1 to MagDevices-1 do
    ComboBoxMoveTarget.Items.Add('MAG'+IntToStr(i));

  ComboBoxMoveTarget.ItemIndex := ComboBoxMoveTarget.Items.IndexOf(target);

  // Read the auto archival configuration in jukebox.ini

  TabSheet8.TabVisible := false;
  ArchiveStatus := 'Archiving page disabled';

  if FileExists(//ExtractFileDir(ParamStr(0))
    CurDir + '\jukebox.ini') then
  begin
    TabSheet8.TabVisible := true;
    ArchiveStatus := 'Inactive';

    AssignFile(f, // ExtractFileDir(ParamStr(0))
      CurDir + '\jukebox.ini');
    Reset(f);

    while not Eof(f) do
    begin
      Readln(f, s);
      s := LowerCase(s);

      if Copy(s, 0, length('activejukebox')) = 'activejukebox' then
      begin
        ActiveJukebox := StrToIntDef(GetData(s), 0);
        ActiveJukeboxLabel.Caption := 'ActiveJukebox = ' + IntToStr(ActiveJukebox);
      end;

      if Copy(s, 0, length('numbercds')) = 'numbercds' then
      begin
        NumberCDs := StrToIntDef(GetData(s), 0);
        NumberCDsLabel.Caption := 'NumberCDs = ' + IntToStr(NumberCDs);
      end;

      if Copy(s, 0, length('nextcdtoburn')) = 'nextcdtoburn' then
      begin
        NextCDToBurn := StrToIntDef(GetData(s), 99999);
        NextCDToBurnLabel.Caption := 'NextCDToBurn = ' + IntToStr(NextCDToBurn);
        NextSlotToUse := NextCDToBurn;
        NextSlotToUseLabel.Caption := 'NextSlotToUse = ' + IntToStr(NextSlotToUse);
      end;

      if Copy(s, 0, length('nextslottouse')) = 'nextslottouse' then
      begin
        NextSlotToUse := StrToIntDef(GetData(s), NextCDToBurn);
        NextSlotToUseLabel.Caption := 'NextSlotToUse = ' + IntToStr(NextSlotToUse);
      end;

      if Copy(s, 0, length('cdcapacitymb')) = 'cdcapacitymb' then
      begin
        CDCapacityMB := StrToIntDef(GetData(s), 0);
        CDCapacityMBLabel.Caption := 'CDCapacityMB = ' + IntToStr(CDCapacityMB);
      end;

      if Copy(s, 0, length('burnthresholdmb')) = 'burnthresholdmb' then
      begin
        BurnThreshold := StrToIntDef(GetData(s), 0);
        BurnThresholdLabel.Caption := 'BurnThresholdMB = ' + IntToStr(BurnThreshold);
      end;

      if Copy(s, 0, length('burnweekend')) = 'burnweekend' then
      begin
        BurnWeekend := StrToIntDef(GetData(s), 0);
      end;

      if Copy(s, 0, length('burntime2')) = 'burntime2' then
      begin
        BurnTime2 := GetData(s);
        if length(BurnTime2) > 2 then
          if BurnTime2[2]=':' then Burntime2 := '0' + BurnTime2;
      end
      else if Copy(s, 0, length('burntime')) = 'burntime' then
      begin
        BurnTime := GetData(s);
        if length(BurnTime) > 2 then
          if BurnTime[2]=':' then Burntime := '0' + BurnTime;

        BurnTimeLabel.Caption := 'BurnTime = ' + BurnTime;
      end;

      if Copy(s, 0, length('tapecontrol')) = 'tapecontrol' then
      begin
        TapeControl := GetData(s);
      end;

      if Copy(s, 0, length('tapedriveletter')) = 'tapedriveletter' then
      begin
        TapeDriveLetter := GetData(s);
      end;

      if Copy(s, 0, length('mirrorverifytime')) = 'mirrorverifytime' then
      begin
        MirrorVerifyTime := GetData(s);
        if length(MirrorVerifyTime) > 2 then
          if MirrorVerifyTime[2]=':' then MirrorVerifytime := '0' + MirrorVerifyTime;

        MirrorVerifyTimeLabel.Caption := 'MirrorVerifyTime = ' + MirrorVerifyTime;
      end;

      if Copy(s, 0, length('testreadtime')) = 'testreadtime' then
      begin
        TestReadTime := GetData(s);
        if length(TestReadTime) > 2 then
          if TestReadTime[2]=':' then TestReadTime := '0' + TestReadTime;

        // TestReadTimeLabel.Caption := 'TestReadTime = ' + TestReadTime;
      end;

      if Copy(s, 0, length('burnprogram ')) = 'burnprogram ' then
      begin
        BurnProgram := GetData(s);
        BurnProgramLabel.Caption := 'BurnProgram = ' + BurnProgram;
      end;

      for i:=0 to 9 do
      begin
        if Copy(s, 0, length('burnprogram0')) = 'burnprogram' + IntToStr(i) then
          BurnProgramList[i] := GetData(s);
      end;

      JukeBoxPathLabel.Caption := 'Path = ' + JukeBoxDeviceList[ActiveJukebox];
    end;

    Closefile(f);

    BackupToTapeButton.Enabled := (length(TapeControl) > 0);
    CheckBoxTapeBackup.Enabled := (length(TapeControl) > 0);
  end;

  // Read the tape backup configuration in tapebackup.ini

  TapeBackupPage.TabVisible := false;
  TapebackupStatus := 'Tape backup page disabled';
  if FileExists(//ExtractFileDir(ParamStr(0))
    CurDir + '\tapebackup.ini') then
  begin
    TapeBackupPage.TabVisible := true;
    TapebackupStatus := 'Inactive';

    AssignFile(f, //ExtractFileDir(ParamStr(0))
      CurDir + '\tapebackup.ini');
    Reset(f);

    while not Eof(f) do
    begin
      Readln(f, s);
      s := LowerCase(s);

      if Copy(s, 0, length('tapedevice')) = 'tapedevice' then
      begin
        TapeDevice := StrToIntDef(GetData(s), 0);
        TapeDeviceLabel.Caption := 'TapeDevice = ' + IntToStr(TapeDevice);
      end;

      if Copy(s, 0, length('tapecapacitymb')) = 'tapecapacitymb' then
      begin
        TapeCapacityMB := StrToIntDef(GetData(s), 0);
        TapeCapacityMBLabel.Caption := 'TapeCapacityMB = ' + IntToStr(TapeCapacityMB);
      end;

      if Copy(s, 0, length('usedtapemb')) = 'usedtapemb' then
      begin
        UsedTapeMB := StrToIntDef(GetData(s), 0);
        UsedTapeMBLabel.Caption := 'UsedTapeMB = ' + IntToStr(UsedTapeMB);
      end;

      if Copy(s, 0, length('backuptime')) = 'backuptime' then
      begin
        BackupTime := GetData(s);
        if length(BackupTime) > 2 then
          if BackupTime[2]=':' then BackupTime := '0' + BackupTime;

        BackupTimeLabel.Caption := 'BackupTime = ' + BackupTime;
      end;
    end;

    Closefile(f);
  end;

  // Read the weekly checks configuration in weeklychecks.ini

  for i:=1 to 10 do CheckActions[i] := 'None';
  for i:=1 to 10 do CheckTimes[i]   := '23:00';
//  mailer.RcptName.Clear; 1.4.17d
  CheckReading := true;

  WeeklyChecksPage.TabVisible := false;
  if FileExists(//ExtractFileDir(ParamStr(0))
    CurDir + '\weeklychecks.ini') then
  begin
    WeeklyChecksPage.TabVisible := true;

    AssignFile(f, //ExtractFileDir(ParamStr(0))
      CurDir + '\weeklychecks.ini');
    Reset(f);

    while not Eof(f) do
    begin
      Readln(f, s);
      sorg := s;
      s := LowerCase(s);

      for i:=1 to 10 do
      begin
        if Copy(s, 0, length('checkdays' + IntToStr(i) + ' ')) = 'checkdays' + IntToStr(i) + ' ' then
          CheckDays[i] := StrToIntDef(GetData(s), 0);

        if Copy(s, 0, length('checktimes' + IntToStr(i) + ' ')) = 'checktimes' + IntToStr(i) + ' ' then
        begin
          CheckTimes[i] := GetData(s);
          if Length(CheckTimes[i])>2 then
            if CheckTimes[i][2]=':' then CheckTimes[i] := '0' + CheckTimes[i];
        end;

        if Copy(s, 0, length('checkactions' + IntToStr(i) + ' ')) = 'checkactions' + IntToStr(i) + ' ' then
          CheckActions[i] := GetData(sorg);

        if Copy(s, 0, length('checkcds' + IntToStr(i) + ' ')) = 'checkcds' + IntToStr(i) + ' ' then
          CheckCDs[i] := StrToIntDef(GetData(s), 0);
      end;

      if Copy(s, 0, length('mailhost')) = 'mailhost' then
        mailer.Host       := GetData(sorg);

      if Copy(s, 0, length('mailport')) = 'mailport' then
        mailer.Port       := GetData(sorg);

      if Copy(s, 0, length('mailsignon')) = 'mailsignon' then
        mailer.Signon     := GetData(sorg);

      if Copy(s, 0, length('mailfromname')) = 'mailfromname' then
        mailer.FromName   := GetData(sorg);

      if Copy(s, 0, length('mailrcptname1')) = 'mailrcptname1' then
        mailer.HdrTo      := GetData(sorg);

      for i:=2 to 9 do
        if Copy(s, 0, length('mailrcptname1')) = 'mailrcptname'+IntToStr(i) then
          mailer.HdrTo    := mailer.HdrTo + ',' + GetData(sorg);

      for i:=1 to 9 do
        if Copy(s, 0, length('mailrcptname1')) = 'mailrcptname'+IntToStr(i) then
          mailer.RcptName.Add(GetData(sorg));

      if Copy(s, 0, length('mailcollecttime')) = 'mailcollecttime' then
        MailCollectTime   := StrToIntDef(GetData(sorg), 1);

      if Copy(s, 0, length('mailwaittime')) = 'mailwaittime' then
        MailWaitTime   := StrToIntDef(GetData(sorg), 10);
    end;

    Closefile(f);
  end;

  for i:=0 to WeeklyChecksPage.ControlCount-1 do
  begin
    n := StrToIntDef(WeeklyChecksPage.Controls[i].Hint, -1);
    if n<0 then continue;
    if WeeklyChecksPage.Controls[i].Tag=10 then (WeeklyChecksPage.Controls[i] as TCheckBox).Checked := (CheckDays[n] and 1)<>0;
    if WeeklyChecksPage.Controls[i].Tag=11 then (WeeklyChecksPage.Controls[i] as TCheckBox).Checked := (CheckDays[n] and 2)<>0;
    if WeeklyChecksPage.Controls[i].Tag=12 then (WeeklyChecksPage.Controls[i] as TCheckBox).Checked := (CheckDays[n] and 4)<>0;
    if WeeklyChecksPage.Controls[i].Tag=13 then (WeeklyChecksPage.Controls[i] as TCheckBox).Checked := (CheckDays[n] and 8)<>0;
    if WeeklyChecksPage.Controls[i].Tag=14 then (WeeklyChecksPage.Controls[i] as TCheckBox).Checked := (CheckDays[n] and 16)<>0;
    if WeeklyChecksPage.Controls[i].Tag=15 then (WeeklyChecksPage.Controls[i] as TCheckBox).Checked := (CheckDays[n] and 32)<>0;
    if WeeklyChecksPage.Controls[i].Tag=16 then (WeeklyChecksPage.Controls[i] as TCheckBox).Checked := (CheckDays[n] and 64)<>0;
    if WeeklyChecksPage.Controls[i].Tag=7 then (WeeklyChecksPage.Controls[i] as TEdit).Text := CheckTimes[n];
    if WeeklyChecksPage.Controls[i].Tag=8 then
    begin
      for k:=0 to DicomSystem.Items.Count-1 do
      begin
        SplitAE(DicomSystem.Items[k], RemoteAE, RemoteIP, RemotePort);
        (WeeklyChecksPage.Controls[i] as TComboBox).Items.Add('Grab ' + RemoteAE);
      end;

      for j:=0 to (WeeklyChecksPage.Controls[i] as TComboBox).Items.Count-1 do
        if (WeeklyChecksPage.Controls[i] as TComboBox).Items[j] = CheckActions[n] then
          (WeeklyChecksPage.Controls[i] as TComboBox).ItemIndex := j;
    end;
    if WeeklyChecksPage.Controls[i].Tag=9 then
    begin
      (WeeklyChecksPage.Controls[i] as TEdit).Text := IntToStr(CheckCDs[n]);

      if pos('Jukebox', CheckActions[n])>0 then
      begin
        TEdit(WeeklyChecksPage.Controls[i]).Color := clWhite;
        TEdit(WeeklyChecksPage.Controls[i]).Enabled := True;
      end
      else
      begin
        TEdit(WeeklyChecksPage.Controls[i]).Color := clltGray;
        TEdit(WeeklyChecksPage.Controls[i]).Enabled := False;
      end;
    end;
  end;

  CheckReading := false;

  // Read the submit checks configuration in submitchecks.ini

  if FileExists(//ExtractFileDir(ParamStr(0))
    CurDir + '\submitchecks.ini') then
  begin
    AssignFile(f, //ExtractFileDir(ParamStr(0))
      CurDir + '\submitchecks.ini');
    Reset(f);

    while not Eof(f) do
    begin
      Readln(f, s);
      sorg := s;
      s := UpperCase(s);

      // One SubmitCheckTime
      if Copy(s, 0, length('SubmitCheckTime')) = 'SUBMITCHECKTIME' then
      begin
        SubmitCheckTime := GetData(s);
        if Length(SubmitCheckTime)>2 then
          if SubmitCheckTime[2]=':' then SubmitCheckTime := '0' + SubmitCheckTime;
      end;

      // One PacsAE
      if Copy(s, 0, length('PacsAE')) = 'PACSAE' then
        PacsAE := GetData(s);

      // Up to 10 (starting with 1) SourceAE's
      for i:=1 to 10 do
      begin
        if Copy(s, 0, length('SourceAE' + IntToStr(i) + ' ')) = 'SOURCEAE' + IntToStr(i) + ' ' then
          SourceAEs[i] := GetData(s);
      end;
    end;
    Closefile(f);
  end;
end;

procedure TForm1.EnableJPEGCheckBoxClick(Sender: TObject);
begin
  CompressLosslessJPEGButton.Enabled     := EnableJPEGCheckBox.Checked and EnableJPEGCheckBox.Enabled;
  CompressLossyJPEGButton.Enabled        := EnableJPEGCheckBox.Checked and EnableJPEGCheckBox.Enabled;
  CompressLosslessJPEG2000Button.Enabled := EnableJPEGCheckBox.Checked and EnableJPEGCheckBox.Enabled;
  CompressLossyJPEG2000Button.Enabled    := EnableJPEGCheckBox.Checked and EnableJPEGCheckBox.Enabled;
  CompressLosslessJPEGLSButton.Enabled   := EnableJPEGCheckBox.Checked and EnableJPEGCheckBox.Enabled;
  CompressLossyJPEGLSButton.Enabled      := EnableJPEGCheckBox.Checked and EnableJPEGCheckBox.Enabled;
  ModeNJRadioButton.Enabled              := V2RadioButton.Checked and EnableJPEGCheckBox.Checked;
  ModeUJRadioButton.Enabled              := EnableJPEGCheckBox.Checked and EnableJPEGCheckBox.Enabled;
end;

procedure TForm1.Export1Click(Sender: TObject);
begin
  Image2.Enabled := Table4.Active;
end;

procedure TForm1.ExportAnomymizedtoZIPfile1Click(Sender: TObject);
begin
  ExportAnonymousZIPImage.Enabled := Table4.Active;
end;

function GuiToServerFilename(s: string): string;
begin
  result := s;
  if (pos('/', MagDeviceList[0])>=1) then
  begin
    s := StringReplace(Result, 'C:\', 'Z:\home\marcel\.wine\drive_c\', []);
    result := StringReplace(copy(s, 3, 999), '\', '/', [rfReplaceAll]);
    Application.MainForm.Caption := result;
  end;
end;

procedure TForm1.ExportAnonymousZIPPatientClick(Sender: TObject);
var script, NewID, typ: string;
begin
  if Sender=ExportAnonymousZIPPatient then
  begin
    script := Table1.Fields[0].AsString + ',,,,';
    typ := 'patient';
  end;
  if Sender=ExportAnonymousZIPStudy then
  begin
    script := Table1.Fields[0].AsString + ',' + Table2.Fields[0].AsString + ',,,';
    typ := 'study';
  end;
  if Sender=ExportAnonymousZIPSeries  then
  begin
    script := Table1.Fields[0].AsString + ',' + Table2.Fields[0].AsString + ',' + Table3.Fields[0].AsString + ',,';
    typ := 'series';
  end;
  if (Sender=ExportAnonymousZIPImage) and Table4.Active then
  begin
    script := Table1.Fields[0].AsString + ',' + Table2.Fields[0].AsString + ',' + Table3.Fields[0].AsString + ',' + Table4.Fields[0].AsString + ',';
    typ := 'image';
  end
  else if (Sender=ExportAnonymousZIPImage) then
  begin
    ShowMessage('Select image to anonymize and ZIP first');
    exit;
  end;

  if InputQuery('Anonymize and zip this ' + typ + '?', 'Enter New ID', NewID) then
  begin
    SaveDialog1.Title := 'Export '+typ+' as zipped file';
    SaveDialog1.DefaultExt := 'zip';
    SaveDialog1.Filter := 'Zip files (*.zip)|*.zip|7z files (*.7z)|*.7z';
    SaveDialog1.FilterIndex := 0;
    SaveDialog1.Filename := NewID + '.zip';

    if SaveDialog1.Execute then
    begin
      script := script + GuiToServerFilename(SaveDialog1.Filename) + ',';
      script := script + 'compression UN;lua/anonymize_script.lua('+NewID+')';
      ServerTask('anonymizing and zipping data from GUI', 'luastart:servercommand[[export:'+script+']]');
    end;
  end;
end;

procedure TForm1.LabelCompressionDblClick(Sender: TObject);
begin
  IncomingCompression := InputBox('Edit Compression', '', IncomingCompression);
  DroppedFileCompression := IncomingCompression;
  UncompressedButton.Checked             := IncomingCompression    = 'un';
  CompressNKIButton.Checked              := IncomingCompression[1] = 'n';
  CompressLosslessJPEGButton.Checked     := (IncomingCompression[1] in ['j', 'J']) and (IncomingCompression[2] < '3');
  CompressLossyJPEGButton.Checked        := (IncomingCompression[1] in ['j', 'J']) and (IncomingCompression[2] > '2');
  CompressLossyJPEGButton.Checked        := (IncomingCompression[1] in ['j', 'J']) and (IncomingCompression[2] > '2');
  CompressLosslessJPEG2000Button.Checked := (IncomingCompression[1] in ['j', 'J']) and (IncomingCompression[2] in ['k', 'K']);
  CompressLossyJPEG2000Button.Checked    := (IncomingCompression[1] in ['j', 'J']) and (IncomingCompression[2] in ['l', 'L']);
  CompressLosslessJPEGLSButton.Checked   := (IncomingCompression[1] in ['j', 'J']) and (IncomingCompression[2] in ['s', 'S']);
  CompressLossyJPEGLSButton.Checked      := (IncomingCompression[1] in ['j', 'J']) and (IncomingCompression[2] in ['7']);
  ModeNJRadioButton.Checked              := (IncomingCompression = 'nj') or (IncomingCompression = 'NJ');
  ModeUJRadioButton.Checked              := (IncomingCompression = 'uj') or (IncomingCompression = 'UJ');
  LabelCompression.Hint                  := 'IncomingCompression (double click to edit) = ' + IncomingCompression;
end;

procedure TForm1.LabelImageNamingDblClick(Sender: TObject);
var i: integer;
begin
  FileNameSyntax := InputBox('Edit FileNameSyntax', '', FileNameSyntax);
  i := StrtoIntDef(FileNameSyntax, 99);
  V2RadioButton.Checked              := (i in [0, 1, 2, 3, 5, 6, 7]) or (pos('.v2', LowerCase(FileNameSyntax))>0);
  DCMRadioButton.Checked             := (i in [99, 4, 8, 9, 10, 11, 12]) or (pos('.dcm', LowerCase(FileNameSyntax))>0);
  CompressNKIButton.Enabled          := V2RadioButton.Checked;
  ModeNJRadioButton.Enabled          := V2RadioButton.Checked and EnableJPEGCheckBox.Checked;
  LabelImageNaming.Hint              := 'FileNameSyntax (double click to edit) = ' + FileNameSyntax;
end;

procedure TForm1.LabelListerClick(Sender: TObject);
begin
  if not Table4.Active then DBGrid1DblClick(self);
  DBGrid3.SetFocus;
end;

procedure TForm1.V2RadioButtonClick(Sender: TObject);
begin
  CompressNKIButton.Enabled          := V2RadioButton.Checked;
  ModeNJRadioButton.Enabled          := V2RadioButton.Checked and EnableJPEGCheckBox.Checked;
end;

procedure TForm1.DCMRadioButtonClick(Sender: TObject);
begin
  CompressNKIButton.Enabled          := V2RadioButton.Checked;
  ModeNJRadioButton.Enabled          := V2RadioButton.Checked and EnableJPEGCheckBox.Checked;
end;

procedure TForm1.SaveConfigButtonClick(Sender: TObject);
var f: TextFile;
    p, c, i, j, resp: integer;
    UseDBFWithoutODBC, changepage: boolean;
    i_f: TInifile;
    amap, php: TStringList;
begin
  UseDBFWithoutODBC := false;
  if FileExists(curdir + '\USEDBASEIIIWITHOUTODBC') then UseDBFWithoutODBC := true;

  changepage := not FileExists(//ExtractFileDir(ParamStr(0))
    CurDir + '\dicom.ini');
  Screen.Cursor := crHourGlass;

  if MagDeviceList[0][1]<>'\' then
  begin
    DataDir := Label10.Caption;
    if DataDir[Length(DataDir)]='\' then
      DataDir := Copy(DataDir, 1, Length(DataDir)-1);
    MagDeviceList[0] := DataDir + '\';

{$I-}
    MkDir(DataDir + '\dbase');
    if IOResult <> 0 then
    begin
    end;

    MkDir(DataDir + '\printer_files');
    if IOResult <> 0 then
    begin
    end;

    MkDir(DataDir + '\incoming');
    if IOResult <> 0 then
    begin
    end;
{$I+}
  end;

// Reported by Paolo Marchesi 20060707: save config loses DBF
// Now only set to default when previous does not exist
  if FileExists(curdir + '\USEDBASEIIIWITHOUTODBC') then
    if not DirectoryExists(DataSource) then
      DataSource := DataDir + '\dbase\';

  AssignFile(f, //ExtractFileDir(ParamStr(0))
    CurDir + '\dicom.ini');
  Rewrite(f);

  writeln(f, '# This file contains configuration information for the DICOM server');
  writeln(f, '# Do not edit unless you know what you are doing');
  writeln(f, '');
  writeln(f, '[sscscp]');
  writeln(f, 'MicroPACS                = sscscp');
  // writeln(f, 'Edition                  = Personal');
  writeln(f, '');
  writeln(f, '# Network configuration: server name and TCP/IP port#');
  writeln(f, 'MyACRNema                = ' + trim(ServerName.text));
  writeln(f, 'TCPPort                  = ' + trim(TCPIPport.text));
  writeln(f, '');
  //writeln(f, '# Reference to other files: known dicom servers; database layout; sops');
  //writeln(f, 'ACRNemaMap               = acrnema.map');
  //writeln(f, 'kFactorFile              = dicom.sql');
  //writeln(f, 'SOPClassList             = dgatesop.lst');
  //writeln(f, '');
  if (mysql<>'0') or (fsqlite<>'0') or (FPostGres<>'0') then
    writeln(f, '# Host, database, username and password for database')
  else
    writeln(f, '# Host(ignored), name, username and password for ODBC data source');
  writeln(f, 'SQLHost                  = ' + SQLHost);
  writeln(f, 'SQLServer                = ' + DataSource);
  writeln(f, 'Username                 = ' + SqlUser);
  writeln(f, 'Password                 = ' + SqlPassWord);
  if mysql<>'0' then
    writeln(f, 'MySql                    = ' + MySql);
  if fsqlite<>'0' then
    writeln(f, 'SqLite                   = ' + FSqLite);
  if FPostGres<>'0' then
    writeln(f, 'Postgres                 = ' + FPostgres);
  //if BrowseThroughDBF<>'0' then
  //  writeln(f, 'BrowseThroughDBF         = ' + BrowseThroughDBF);
  writeln(f, 'DoubleBackSlashToDB      = ' + DoubleBackSlashToDB);
  writeln(f, 'UseEscapeStringConstants = ' + UseEscapeStringConstants);
  if UTF8ToDB<>'0' then
    writeln(f, 'UTF8ToDB                 = ' + UTF8ToDB);
  if UTF8FromDB<>'0' then
    writeln(f, 'UTF8FromDB               = ' + UTF8FromDB);
  writeln(f, '');
  writeln(f, '# Configure server');
  //writeln(f, 'TruncateFieldNames       = 10');
  //writeln(f, 'MaxFieldLength           = 254');
  //writeln(f, 'MaxFileNameLength        = ' + MaxFileNameLength);
  writeln(f, 'ImportExportDragAndDrop  = ' + ImportExportDragAndDrop);

  if FixPhilips<>'0'    then
    writeln(f, 'FixPhilips               = ' + FixPhilips);
  if FixKodak<>'0'    then
    writeln(f, 'FixKodak                 = ' + FixKodak);
  if KeepAlive<>0 then
    writeln(f, 'KeepAlive                = ' + IntToStr(KeepAlive));
  if LargeFileSizeKB<>100000 then
    writeln(f, 'LargeFileSizeKB          = ' + IntToStr(LargeFileSizeKB));
  if PrintSquareLandscape<>0 then
    writeln(f, 'PrintSquareLandscape     = ' + IntToStr(PrintSquareLandscape));
  //writeln(f, 'UseKpacsDecompression    = ' + IntToStr(UseKpacsDecompression));
  writeln(f, 'ZipTime                  = ' + ZipTime);
  if UIDPrefix<>''    then
    writeln(f, 'UIDPrefix                = ' + UIDPrefix);
  if EnableReadAheadThread<>1 then
    writeln(f, 'EnableReadAheadThread    = ' + IntToStr(EnableReadAheadThread));

  if StorageFailedErrorCode<>272 then
    writeln(f, 'StorageFailedErrorCode   = ' + IntToStr(StorageFailedErrorCode));

  if SendUpperCaseAE<>0 then
    writeln(f, 'SendUpperCaseAE          = ' + IntToStr(SendUpperCaseAE));
  if PatientQuerySortOrder<>'' then
    writeln(f, 'PatientQuerySortOrder    = ' + PatientQuerySortOrder);
  if StudyQuerySortOrder<>'' then
    writeln(f, 'StudyQuerySortOrder      = ' + StudyQuerySortOrder);
  if SeriesQuerySortOrder<>'' then
    writeln(f, 'SeriesQuerySortOrder     = ' + SeriesQuerySortOrder);
  if ImageQuerySortOrder<>'' then
    writeln(f, 'ImageQuerySortOrder      = ' + ImageQuerySortOrder);
  if ImageQuerySortOrder<>'1' then
    writeln(f, 'EnableComputedFields     = ' + EnableComputedFields);
  if UseDBFWithoutODBC then
  begin
    writeln(f, 'IndexDBF                 = ' + IndexDBF);
    writeln(f, 'PackDBF                  = ' + PackDBF);
    writeln(f, 'LongQueryDBF             = ' + LongQueryDBF);
  end;
  if TCPIPTimeOut<>'300' then
    writeln(f, 'TCPIPTimeOut             = ' + TCPIPTimeOut);
  if FailHoldOff<>'60' then
    writeln(f, 'FailHoldOff              = ' + FailHoldOff);
  if RetryDelay<>'100' then
    writeln(f, 'RetryDelay               = ' + RetryDelay);
  if RetryForwardFailed<>'0' then
    writeln(f, 'RetryForwardFailed       = ' + RetryForwardFailed);
  if QueueSize<>'128' then
    writeln(f, 'QueueSize                = ' + QueueSize);
  if WorkListMode<>'0' then
    writeln(f, 'WorkListMode             = ' + WorkListMode);
  if WorkListReturnsISO_IR_100<>'1' then
    writeln(f, 'WorkListReturnsISO_IR_100 = ' + WorkListReturnsISO_IR_100);
  if Prefetcher<>'0' then
    writeln(f, 'Prefetcher               = ' + Prefetcher);
  if LRUSort<>'' then
    writeln(f, 'LRUSort                  = ' + LRUSort);
  if AllowTruncate<>'' then
    writeln(f, 'AllowTruncate            = ' + AllowTruncate);
  //writeln(f, 'DecompressNon16BitsJpeg  = ' + DecompressNon16BitsJpeg);
  //writeln(f, 'UseBuiltInJPEG           = ' + UseBuiltInJPEG);
  if LossyQuality<>'95' then
    writeln(f, 'LossyQuality             = ' + LossyQuality);
//  writeln(f, 'UseOpenJPEG              = ' + UseOpenJPEG);
  if IgnoreOutOfMemoryErrors<>'0' then
    writeln(f, 'IgnoreOutOfMemoryErrors  = ' + IgnoreOutOfMemoryErrors);
  if NoDICOMCheck<>'0' then
    writeln(f, 'NoDICOMCheck             = ' + NoDICOMCheck);
  //writeln(f, 'PadAEWithZeros           = ' + PadAEWithZeros);
  if AllowEmptyPatientID<>'0' then
    writeln(f, 'AllowEmptyPatientID      = ' + AllowEmptyPatientID);
  if RetryForwardRemoteDICOMError<>'0' then 
    writeln(f, 'RetryForwardRemoteDICOMError = ' + RetryForwardRemoteDICOMError);
                                    
  writeln(f, '');
  if DCMRadioButton.Checked then
  begin
    i := StrtoIntDef(FileNameSyntax, -1);
    if (i in [0, 1, 2, 3, 5, 6, 7]) or (pos('.v2', LowerCase(FileNameSyntax))>0) then
    case i of
      -1: FileNameSyntax := copy(FileNameSyntax, 1, pos('.v2', FileNameSyntax)) + 'dcm';
      0 : FileNameSyntax := '%name[0,7]_%id[0,7]\%series_%image_%time.dcm';
      1 : FileNameSyntax := '%name[0,7]_%id[0,7]\%series_%image_%time%counter.dcm';
      2 : FileNameSyntax := '%name[0,7]_%id[0,7]\%seriesuid_%series_%image_%time%counter.dcm';
      3 : FileNameSyntax := '4';
      5 : FileNameSyntax := '%name\%seriesuid_%series_%image_%time%counter.dcm';
      6 : FileNameSyntax := '8';
      7 : FileNameSyntax := '9';
    end;
  end;

  if V2RadioButton.Checked then
  begin
    i := StrtoIntDef(FileNameSyntax, -1);
    if (i in [4, 8, 9, 10, 11, 12]) or (pos('.dcm', LowerCase(FileNameSyntax))>0) then
    case i of
      -1 : FileNameSyntax := copy(FileNameSyntax, 1, pos('.dcm', FileNameSyntax)) + 'v2';
      4  : FileNameSyntax := '3';
      8  : FileNameSyntax := '6';
      9  : FileNameSyntax := '7';
      10 : FileNameSyntax := 'Images\%sopuid.v2';
      11 : FileNameSyntax := '%name\%studyuid\%seriesuid\%sopuid.v2';
      12 : FileNameSyntax := '%name_%id\%modality_%studyid\%seriesid\%sopuid.v2';
    end;
  end;

  writeln(f, 'FileNameSyntax           = ' + FileNameSyntax);
  LabelImageNaming.Hint := 'FileNameSyntax (double click to edit) = ' + FileNameSyntax;

  DroppedFileCompression := LowerCase(DroppedFileCompression);
  IncomingCompression    := LowerCase(IncomingCompression);
  ArchiveCompression     := LowerCase(ArchiveCompression);

  // Luiz added at Sept 19 2021
  NightlyStrTimeToMove:=trim(NightlyStrTimeToMoveText.text);

  if UncompressedButton.Checked then
  begin
    if (IncomingCompression<>'un') and (IncomingCompression<>'UN') then
      IncomingCompression := 'un';

    if (DroppedFileCompression<>'un') then
      DroppedFileCompression := 'un';
  end;

  if CompressNKIButton.Checked then
  begin
    if (not (IncomingCompression[1] in ['n', 'N'])) or (IncomingCompression[2]='0') then
      IncomingCompression := 'n4';

    if (not (DroppedFileCompression[1] in ['n', 'N'])) or (DroppedFileCompression[2]='0') then
      DroppedFileCompression := 'n4';
  end;

  if CompressLossyJPEGButton.Checked then
  begin
    if (IncomingCompression[1]<>'j') or (IncomingCompression[2]<'3') then
      IncomingCompression := 'j6';

    if (DroppedFileCompression[1]<>'j') or (DroppedFileCompression[2]<'3') then
      DroppedFileCompression := 'j6';
  end;

  if CompressLosslessJPEGButton.Checked then
  begin
    if (IncomingCompression<>'j1') and (IncomingCompression<>'j2') and
       (IncomingCompression<>'J1') and (IncomingCompression<>'J2') then
      IncomingCompression := 'j2';

    if (DroppedFileCompression<>'j1') and (DroppedFileCompression<>'j2') and
       (DroppedFileCompression<>'J1') and (DroppedFileCompression<>'J2') then
      DroppedFileCompression := 'j2';
  end;

  if CompressLossyJPEG2000Button.Checked then
  begin
    if (IncomingCompression<>'jl') and (IncomingCompression<>'JL') then
      IncomingCompression := 'jl';

    if (DroppedFileCompression<>'jl') and (DroppedFileCompression<>'JL') then
      DroppedFileCompression := 'jl';
  end;

  if CompressLosslessJPEG2000Button.Checked then
  begin
    if (IncomingCompression<>'jk') and (IncomingCompression<>'JK') then
      IncomingCompression := 'jk';

    if (DroppedFileCompression<>'jk') and (DroppedFileCompression<>'JK') then
      DroppedFileCompression := 'jk';
  end;

  if CompressLossyJPEGLSButton.Checked then
  begin
    if (IncomingCompression<>'j7') and (IncomingCompression<>'J7') then
      IncomingCompression := 'j7';

    if (DroppedFileCompression<>'j7') and (DroppedFileCompression<>'J7') then
      DroppedFileCompression := 'j7';
  end;

  if CompressLosslessJPEGLSButton.Checked then
  begin
    if (IncomingCompression<>'js') and (IncomingCompression<>'JS') then
      IncomingCompression := 'js';

    if (DroppedFileCompression<>'js') and (DroppedFileCompression<>'JS') then
      DroppedFileCompression := 'js';
  end;

  if ModeNJRadioButton.Checked then
  begin
    if (IncomingCompression<>'nj') and (IncomingCompression<>'NJ') then
      IncomingCompression := 'nj';

    if (DroppedFileCompression<>'nj') and (IncomingCompression<>'NJ') then
      DroppedFileCompression := 'nj';
  end;

  if ModeUJRadioButton.Checked then
  begin
    if (IncomingCompression<>'uj') and (IncomingCompression<>'UJ') then
      IncomingCompression := 'uj';

    if (DroppedFileCompression<>'uj') and (IncomingCompression<>'UJ') then
      DroppedFileCompression := 'uj';
  end;

  LabelCompression.Hint  := 'IncomingCompression (double click to edit) = ' + IncomingCompression;

  writeln(f, '');
  writeln(f, '# Configuration of compression for incoming images and archival');
  writeln(f, 'DroppedFileCompression   = ' + DroppedFileCompression);
  writeln(f, 'IncomingCompression      = ' + IncomingCompression);
  writeln(f, 'ArchiveCompression       = ' + ArchiveCompression);

  {writeln(f, '');
  writeln(f, '# Names of the database tables');
  writeln(f, 'PatientTableName         = DICOMPatients');
  writeln(f, 'StudyTableName           = DICOMStudies');
  writeln(f, 'SeriesTableName          = DICOMSeries');
  writeln(f, 'ImageTableName           = DICOMImages');
  writeln(f, 'WorkListTableName        = DICOMWorkList');
  writeln(f, 'DMarkTableName           = DICOMAccessUpdates');
  writeln(f, 'RegisteredMOPDeviceTable = RegisteredMOPIDs');
  writeln(f, 'UIDToMOPIDTable          = UIDToMOPID');
  writeln(f, 'UIDToCDRIDTable          = UIDToCDRID');
  }
  writeln(f, '');
  writeln(f, '# For debug information');
  writeln(f, 'PACSName                 = ' + trim(ServerName.Text));
  writeln(f, 'OperatorConsole          = 127.0.0.1');
  writeln(f, 'DebugLevel               = ' + IntToStr(UpDownDebugLevel.Position));
  writeln(f, '');
  if mailer.Host<>'' then
  begin
    writeln(f, '# Configure email of error messages');
    writeln(f, 'MailHost                 = ' + mailer.Host);
    writeln(f, 'MailPort                 = ' + mailer.Port);
    writeln(f, 'MailSignon               = ' + mailer.Signon);
    writeln(f, 'MailFromName             = ' + mailer.FromName);
    if mailer.RcptName.Count<>0 then
      writeln(f, 'MailRcptName1            = ' + mailer.RcptName[0])
    else
      writeln(f, 'MailRcptName1            = ');
    for i:=2 to mailer.RcptName.Count do
      writeln(f, 'MailRcptName'+IntToStr(i)+'            = ' + mailer.RcptName[i-1]);
    writeln(f, 'MailCollectTime          = ' + IntToStr(MailCollectTime));
    writeln(f, 'MailWaitTime             = ' + IntToStr(MailWaitTime));
    writeln(f, '');
  end;
  writeln(f, '# Configuration of disk(s) to store images');
  if trim(MagThreshold.text)<>'0' then
    writeln(f, 'MAGDeviceThreshold      = ' + trim(MagThreshold.text));
  writeln(f, 'MAGDeviceFullThreshold  = ' + trim(MAGDeviceFullThreshold));
  if trim(IgnoreMAGDeviceThreshold)<>'0' then
    writeln(f, 'IgnoreMAGDeviceThreshold = ' + trim(IgnoreMAGDeviceThreshold));
  writeln(f, 'MAGDevices               = ' + IntToStr(MagDevices));
  writeln(f, 'MAGDevice0               = ' + DataDir + '\');
  for i:=1 to MagDevices-1 do
    writeln(f, 'MAGDevice'+IntToStr(i)+'               = ' + MagDeviceList[i]);
  if trim(NightlyTreshold.text)<>'0' then
    writeln(f, 'NightlyCleanThreshold   = ' + trim(NightlyTreshold.text));

  if ComboBoxMoveTarget.Enabled then
  begin
    writeln(f, 'NightlyMoveThreshold    = ' + trim(EditNightlyMoveTreshold.text));
    writeln(f, 'NightlyMoveTarget        = ' + trim(ComboBoxMoveTarget.text));
    // Luiz added at Sept 19 2021
    writeln(f, 'NightlyStrTimeToMove     = ' + trim(NightlyStrTimeToMoveText.text));
  end;

  if MirrorDevices <> 0 then
  begin
    writeln(f, '');
    writeln(f, '# Configuration of mirror disk(s)');
    writeln(f, 'MIRRORDevices            = ' + IntToStr(MirrorDevices));
    for i:=0 to MirrorDevices-1 do
      writeln(f, 'MIRRORDevice'+IntToStr(i)+'            = ' + MirrorDeviceList[i]);
  end;

  if CacheDevices <> 0 then
  begin
    writeln(f, '');
    writeln(f, '# Configuration of disk(s) to hold images prior to archival');
    writeln(f, 'CACHEDevices             = ' + IntToStr(CacheDevices));
    for i:=0 to CacheDevices-1 do
      writeln(f, 'CACHEDevice'+IntToStr(i)+'             = ' + CacheDeviceList[i]);
  end;

  if JukeBoxDevices <> 0 then
  begin
    writeln(f, '');
    writeln(f, '# Configuration of disk jukebox(es) for archival');
    writeln(f, 'JUKEBOXDevices           = ' + IntToStr(JukeBoxDevices));
    for i:=0 to JukeBoxDevices-1 do
      writeln(f, 'JUKEBOXDevice'+IntToStr(i)+'           = ' + JukeBoxDeviceList[i]);
  end;

  p := 0;
  for i:=0 to 9 do p := p + Length(VirtualServerForList[i]);
  if p>0 then
  begin
    writeln(f, '');
    writeln(f, '# Configuration of the virtual server');
    writeln(f, 'CacheVirtualData         = ' + CacheVirtualData);
    writeln(f, 'OverlapVirtualGet        = ' + OverlapVirtualGet);
    for i:=0 to 9 do
    begin
      if VirtualServerForList[i]<>''    then
        writeln(f, 'VirtualServerFor'+IntToStr(i)+'        = ' + VirtualServerForList[i]);
      if VirtualPerSeries[i]<>0    then
        writeln(f, 'VirtualPerSeries'+IntToStr(i)+'        = ' + IntToStr(VirtualPerSeries[i]));
    end;
  end;

  if (Length(ExternalViewer) + Length(DemoViewer) + Length(DemoCopy) )>0 then
  begin
    writeln(f, '');
    writeln(f, '# Configuration of external (browse page) and demo (on receive) viewer');
    if ExternalViewer<>'' then writeln(f, 'ExternalViewer           = ' + ExternalViewer);
    if DemoViewer    <>'' then writeln(f, 'DemoViewer               = ' + DemoViewer);
    if DemoCopy      <>'' then writeln(f, 'DemoCopy                 = ' + DemoCopy);
  end;

  if ExportConverters <> 0 then
  begin
    writeln(f, '');
    writeln(f, '# Configuration of forwarding and/or converter programs to export DICOM slices');
    writeln(f, 'ForwardAssociationLevel           = ' + UpperCase(ForwardAssociationLevel));
    writeln(f, 'ForwardAssociationCloseDelay      = ' + IntToStr(ForwardAssociationCloseDelay));
    writeln(f, 'ForwardAssociationRefreshDelay    = ' + IntToStr(ForwardAssociationRefreshDelay));
    writeln(f, 'ForwardAssociationRelease         = ' + IntToStr(ForwardAssociationRelease));
    writeln(f, '');

    writeln(f, 'ExportConverters                  = ' + IntToStr(ExportConverters));
    for i:=0 to ExportConverters-1 do
    begin
      if ExportModalityList[i]<>''    then
        writeln(f, 'ExportModality'+IntToStr(i)+'                   = ' + ExportModalityList[i]);
      if ExportStationNameList[i]<>'' then
        writeln(f, 'ExportStationName'+IntToStr(i)+'                = ' + ExportStationNameList[i]);
      if ExportCalledAEList[i]<>'' then
        writeln(f, 'ExportCalledAE'+IntToStr(i)+'                   = ' + ExportCalledAEList[i]);
      if ExportCallingAEList[i]<>'' then
        writeln(f, 'ExportCallingAE'+IntToStr(i)+'                  = ' + ExportCallingAEList[i]);
      if ExportFilterList[i]<>'' then
        writeln(f, 'ExportFilter'+IntToStr(i)+'                     = ' + ExportFilterList[i]);
      if ExportConverterList[i]<>'' then
        writeln(f, 'ExportConverter'+IntToStr(i)+'                  = ' + ExportConverterList[i]);
    end;
  end;

  p := 0;
  for i:=0 to 19 do p := p + Length(ImportConverterList[i]);
  if p>0 then
  begin
    writeln(f, '');
    writeln(f, '# Configuration of rules to modify, log or reject incoming DICOM slices');

    if (ImportConverters<>0) and (ImportConverters<>999) then
      writeln(f, 'ImportConverters                  = ' + IntToStr(ImportConverters));

    for i:=0 to ImportConverters-1 do
    begin
      if i>19 then break;

      if ImportConverterList[i]='' then continue;

      if ImportModalityList[i]<>''    then
        writeln(f, 'ImportModality'+IntToStr(i)+'                   = ' + ImportModalityList[i]);
      if ImportStationNameList[i]<>'' then
        writeln(f, 'ImportStationName'+IntToStr(i)+'                = ' + ImportStationNameList[i]);
      if ImportCalledAEList[i]<>'' then
        writeln(f, 'ImportCalledAE'+IntToStr(i)+'                   = ' + ImportCalledAEList[i]);
      if ImportCallingAEList[i]<>'' then
        writeln(f, 'ImportCallingAE'+IntToStr(i)+'                  = ' + ImportCallingAEList[i]);
      if ImportConverterList[i]<>'' then
        writeln(f, 'ImportConverter'+IntToStr(i)+'                  = ' + ImportConverterList[i]);
    end;
  end;

  if (ExportConverters>0) or (p>0) then
  begin
    writeln(f, '');
    writeln(f, 'ForwardCollectDelay               = ' + ForwardCollectDelay);
    writeln(f, 'MaximumExportRetries              = ' + MaximumExportRetries);
    writeln(f, 'MaximumDelayedFetchForwardRetries = ' + MaximumDelayedFetchForwardRetries);
  end;

  writeln(f);
  if RetrieveResultConverter[0]<>'' then writeln(f, 'RetrieveResultConverter0          = ' + RetrieveResultConverter[0]);
  if QueryConverter[0]<>''          then writeln(f, 'QueryConverter0                   = ' + QueryConverter[0]);
  if WorkListQueryConverter[0]<>''  then writeln(f, 'WorkListQueryConverter0           = ' + WorkListQueryConverter[0]);
  if RetrieveConverter[0]<>''       then writeln(f, 'RetrieveConverter0                = ' + RetrieveConverter[0]);
  if QueryResultConverter[0]<>''    then writeln(f, 'QueryResultConverter0             = ' + QueryResultConverter[0]);
  if ModalityWorkListQueryResultConverter[0]<>''
                                    then writeln(f, 'ModalityWorkListQueryResultConverter0  = ' + ModalityWorkListQueryResultConverter[0]);
  if MergeSeriesConverter[0]<>''    then writeln(f, 'MergeSeriesConverter0             = ' + MergeSeriesConverter[0]);
  if MergeStudiesConverter[0]<>''   then writeln(f, 'MergeStudiesConverter0            = ' + MergeStudiesConverter[0]);
  if ArchiveConverter[0]<>''        then writeln(f, 'ArchiveConverter0                 = ' + ArchiveConverter[0]);
  if MoveDeviceConverter[0]<>''     then writeln(f, 'MoveDeviceConverter0              = ' + MoveDeviceConverter[0]);
  if RejectedImageConverter[0]<>''  then writeln(f, 'RejectedImageConverter0           = ' + RejectedImageConverter[0]);
  if RejectedImageWorkListConverter[0]<>'' 
                                    then writeln(f, 'RejectedImageWorkListConverter0   = ' + RejectedImageWorkListConverter[0]);
  if VirtualServerQueryConverter[0]<>''
                                    then writeln(f, 'VirtualServerQueryConverter0      = ' + VirtualServerQueryConverter[0]);
  if VirtualServerQueryResultConverter[0]<>''
                                    then writeln(f, 'VirtualServerQueryResultConverter0= ' + VirtualServerQueryResultConverter[0]);
  if othersections.count>0 then
  begin
    writeln(f, '');
    for i:=0 to othersections.count-1 do
    writeln(f, othersections[i]);
  end;
  
  CloseFile(f);

  // Update ACRNEMA.map putting AE and PORT of server in first line
  amap := TStringList.Create;
  amap.LoadFromFile(curdir+'\acrnema.map');
  for i := 0 to amap.Count-1 do
  begin
    if (pos('*', amap[i])=0) and (length(amap[i])>10) then
    begin
      amap[i] := trim(ServerName.Text)+'		127.0.0.1	'+trim(TCPIPport.text)+'		un';
      break;
    end;
  end;
  amap.SaveToFile(curdir+'\acrnema.map');
  DICOMMap.Lines.LoadFromFile(CurDir + '\acrnema.map');
  FillAELists;

  // Update classic web interface with executables and ini file settings
  if fileExists(curdir + '\webserver\cgi-bin\dicom.ini') then
  begin
    if not FileExists(curdir + '\webserver\cgi-bin\dgate.exe') then
      CopyFile(PChar(curdir + '\install32\dgate.exe'), PChar(curdir + '\webserver\cgi-bin\dgate.exe'), false);
    if not FileExists(curdir + '\webserver\cgi-bin\lua5.1.dll') then
      CopyFile(PChar(curdir + '\install32\lua5.1.dll'), PChar(curdir + '\webserver\cgi-bin\lua5.1.dll'), false);
    i_f := TIniFile.Create(curdir + '\webserver\cgi-bin\dicom.ini');
    i_f.WriteString('sscscp', 'ACRNemaMap', ' acrnema.map');
    i_f.WriteString('sscscp', 'TCPPort', ' '+trim(TCPIPport.text));
    i_f.WriteString('sscscp', 'MyACRNema', ' '+trim(ServerName.Text));
    i_f.Free;
    CopyFile(PChar(curdir + '\acrnema.map'), PChar(curdir + '\webserver\cgi-bin\acrnema.map'), false);
  end;

  // Update newweb cgi interface with executables and ini file settings
  if fileExists(curdir + '\webserver\cgi-bin\newweb\dicom.ini') then
  begin
    if not FileExists(curdir + '\webserver\cgi-bin\newweb\dgate.exe') then
      CopyFile(PChar(curdir + '\install32\dgate.exe'), PChar(curdir + '\webserver\cgi-bin\newweb\dgate.exe'), false);
    if not FileExists(curdir + '\webserver\cgi-bin\newweb\lua5.1.dll') then
      CopyFile(PChar(curdir + '\install32\lua5.1.dll'), PChar(curdir + '\webserver\cgi-bin\newweb\lua5.1.dll'), false);
    i_f := TIniFile.Create(curdir + '\webserver\cgi-bin\newweb\dicom.ini');
//    i_f.WriteString('sscscp', 'ACRNemaMap', ' acrnema.map');
    i_f.WriteString('sscscp', 'TCPPort', ' '+trim(TCPIPport.text));
    i_f.Free;
//    CopyFile(PChar(curdir + '\acrnema.map'), PChar(curdir + '\webserver\cgi-bin\newweb\acrnema.map'), false);
  end;

  // Update newweb app with executables and ini file settings
  if fileExists(curdir + '\webserver\htdocs\app\newweb\dicom.ini') then
  begin
    if not FileExists(curdir + '\webserver\htdocs\app\newweb\dgate.exe') then
      CopyFile(PChar(curdir + '\install32\dgate.exe'), PChar(curdir + '\webserver\htdocs\app\newweb\dgate.exe'), false);
    if not FileExists(curdir + '\webserver\htdocs\app\newweb\lua5.1.dll') then
      CopyFile(PChar(curdir + '\install32\lua5.1.dll'), PChar(curdir + '\webserver\htdocs\app\newweb\lua5.1.dll'), false);
    i_f := TIniFile.Create(curdir + '\webserver\htdocs\app\newweb\dicom.ini');
//    i_f.WriteString('sscscp', 'ACRNemaMap', ' acrnema.map');
    i_f.WriteString('sscscp', 'TCPPort', ' '+trim(TCPIPport.text));
    i_f.Free;
//    CopyFile(PChar(curdir + '\acrnema.map'), PChar(curdir + '\webserver\cgi-bin\newweb\acrnema.map'), false);
  end;

  // Update config.php putting PORT of server in $exe line
  if FileExists(curdir+'\webserver\htdocs\api\dicom\config.php') then
  begin
    php := TStringList.Create;
    php.LoadFromFile(curdir+'\webserver\htdocs\api\dicom\config.php');
    for i := 0 to php.Count-1 do
    begin
      if (pos('$exe', php[i])>1) and (pos('-p', php[i])>1) then
      begin
        j := pos('-p', php[i]);
        php[i] := copy(php[i], 1, j+1)+trim(TCPIPport.text)+' -q127.0.0.1'';';
        break;
      end;
    end;
    php.SaveToFile(curdir+'\webserver\htdocs\api\dicom\config.php');
  end;

  // Update config.php putting PORT of server in $exe line
  if FileExists(curdir+'\webserver\htdocs\app\newweb\config.php') then
  begin
    php := TStringList.Create;
    php.LoadFromFile(curdir+'\webserver\htdocs\app\newweb\config.php');
    for i := 0 to php.Count-1 do
    begin
      if (pos('$exe', php[i])>1) and (pos('-p', php[i])>1) then
      begin
        j := pos('-p', php[i]);
        php[i] := copy(php[i], 1, j+1)+trim(TCPIPport.text)+' -q127.0.0.1'';';
        break;
      end;
    end;
    php.SaveToFile(curdir+'\webserver\htdocs\app\newweb\config.php');
  end;

  // Update config.php putting PORT of server in $exe line
  if FileExists(curdir+'\webserver\htdocs\config.php') then
  begin
    php := TStringList.Create;
    php.LoadFromFile(curdir+'\webserver\htdocs\config.php');
    for i := 0 to php.Count-1 do
    begin
      if (pos('$exe', php[i])>1) and (pos('-p', php[i])>1) then
      begin
        j := pos('-p', php[i]);
        php[i] := copy(php[i], 1, j+1)+trim(TCPIPport.text)+' -q127.0.0.1'';';
        break;
      end;
    end;
    php.SaveToFile(curdir+'\webserver\htdocs\config.php');
  end;

  if FileExists(//ExtractFileDir(ParamStr(0))
    CurDir + '\jukebox.ini') then
  begin
    AssignFile(f, //ExtractFileDir(ParamStr(0))
      CurDir + '\jukebox.ini');
    Rewrite(f);

    writeln(f, '# This file contains configuration information for auto archival for the DICOM server');
    writeln(f, '');

    writeln(f, 'ActiveJukebox    = ' + IntToStr(ActiveJukebox));
    writeln(f, 'NumberCDs        = ' + IntToStr(NumberCDs));
    writeln(f, 'NextCDToBurn     = ' + IntToStr(NextCDToBurn));
    writeln(f, 'NextSlotToUse    = ' + IntToStr(NextSlotToUse));
    writeln(f, 'CDCapacityMB     = ' + IntToStr(CDCapacityMB));
    writeln(f, 'BurnThresholdMB  = ' + IntToStr(BurnThreshold));
    writeln(f, 'BurnTime         = ' + BurnTime);
    writeln(f, 'BurnTime2        = ' + BurnTime2);
    writeln(f, 'BurnWeekend      = ' + IntToStr(BurnWeekend));
    writeln(f, 'MirrorVerifyTime = ' + MirrorVerifyTime);
    writeln(f, 'TestReadTime     = ' + TestReadTime);
    writeln(f, 'TapeControl      = ' + TapeControl);
    writeln(f, 'TapeDriveLetter  = ' + TapeDriveLetter);
    writeln(f, '');

    writeln(f, '# In BurnProgram use program name with extension; e.g., TEST.EXE parameters');
    writeln(f, '# In parameters: %s=src %d=dst %l=label %n=CDSlot, %c=CDNum, %j=JukeboxNum, %m=MAG0, %a=date, %t=time, %p=udpport');
    writeln(f, '# It is possible to run multiple programs as well (BurnProgram0..9)');
    writeln(f, '');
    writeln(f, 'BurnProgram     = ' + BurnProgram);
    for i:=0 to 9 do if Length(BurnProgramList[i])<>0 then
      writeln(f, 'BurnProgram' + IntToStr(i) + '    = ' + BurnProgramList[i]);

    CloseFile(f);
  end;

  if FileExists(//ExtractFileDir(ParamStr(0))
    CurDir + '\tapebackup.ini') then
  begin
    AssignFile(f, //ExtractFileDir(ParamStr(0))
      CurDir + '\tapebackup.ini');
    Rewrite(f);

    writeln(f, '# This file contains configuration information for tape backup for the DICOM server');
    writeln(f, '');

    writeln(f, 'TapeDevice       = ' + IntToStr(TapeDevice));
    writeln(f, 'TapeCapacityMB   = ' + IntToStr(TapeCapacityMB));
    writeln(f, 'UsedTapeMB       = ' + IntToStr(UsedTapeMB));
    writeln(f, 'BackupTime       = ' + BackupTime);

    CloseFile(f);
  end;

  if FileExists(//ExtractFileDir(ParamStr(0))
    CurDir + '\weeklychecks.ini') then
  begin
    AssignFile(f, //ExtractFileDir(ParamStr(0))
      CurDir + '\weeklychecks.ini');
    Rewrite(f);

    writeln(f, '# This file contains configuration information for weekly checks for the DICOM server');
    writeln(f, '');

    for i:=1 to 10 do
    begin
      writeln(f, 'CheckDays'+IntToStr(i)+'      = ' + IntToStr(CheckDays[i]));
      writeln(f, 'CheckTimes'+IntToStr(i)+'     = ' + CheckTimes[i]);
      writeln(f, 'CheckActions'+IntToStr(i)+'   = ' + CheckActions[i]);
      writeln(f, 'CheckCDs'+IntToStr(i)+'       = ' + IntToStr(CheckCDs[i]));
      writeln(f, '');
    end;

    writeln(f, 'MailHost         = ' + mailer.host);
    writeln(f, 'MailPort         = ' + mailer.port);
    writeln(f, 'MailSignon       = ' + mailer.signon);
    writeln(f, 'MailFromName     = ' + mailer.fromname);
    writeln(f, 'MailCollectTime  = ' + IntToStr(MailCollectTime));
    writeln(f, 'MailWaitTime     = ' + IntToStr(MailWaitTime));
    for i:=0 to mailer.RcptName.Count-1 do
      writeln(f, 'MailRcptName'+IntToStr(i+1)+'    = ' + mailer.rcptname[i]);

    if mailer.RcptName.Count=0 then
      writeln(f, 'MailRcptName1    = ');

    CloseFile(f);
  end;

  if FileExists(//ExtractFileDir(ParamStr(0))
    CurDir + '\submitchecks.ini') then
  begin
    AssignFile(f, //ExtractFileDir(ParamStr(0))
      CurDir + '\submitchecks.ini');
    Rewrite(f);

    writeln(f, '# This file contains configuration information for submit checks for the DICOM server');
    writeln(f, '');

    writeln(f, 'SubmitCheckTime'+' = ' + SubmitCheckTime);
    writeln(f, 'PacsAE'+'          = ' + PacsAE);
    for i:=1 to 10 do
      writeln(f, 'SourceAE'+IntToStr(i)+'       = ' + SourceAEs[i]);

    CloseFile(f);
  end;

  if JPEGSupport <> EnableJPEGCheckBox.Checked then
  begin
    if MessageDlg('Update DGATESOP.LST? (Replaces any manual configuration of accepted modalities and transfer syntaxes)', mtConfirmation,
      [mbYes, mbNo], 0) = mrYes then
    begin
      CreateDGATESOP_LST(EnableJPEGCheckBox.Checked);
      JPEGSupport := EnableJPEGCheckBox.Checked;
    end
    else
      EnableJPEGCheckBox.Checked := not EnableJPEGCheckBox.Checked;
  end;

  // If running in threaded mode then kill the process and
  // start another one so that the new dicom.ini takes effect
  if NewInstall then
  begin
    //test if someting is already running on this port
    //this test does not work - but does not hurt either; leave it in for now
    try
      TestSocket.Proto:='tcp';
      TestSocket.Addr:='127.0.0.1';
      TestSocket.LocalAddr:='127.0.0.1';
      TestSocket.LocalPort:=trim(TCPIPport.text);
      TestSocket.Port:=trim(TCPIPport.text);
      TestSocket.Listen;
      TestSocket.Close;
      RunInThreadedModeClick(self);
      Sleep(1000);
      if assigned(sender) then TestLocalServer(true, false);
      CheckBoxDebugLog.Checked := false;
      CheckBoxWebServer.Checked := false;
      CheckBoxOnlyLogToFile.Checked := false;
    except
      ShowMessage('A server is already running at the selected port - cannot continue');
    end;
  end
  else if (ThreadedProcess<>0) or PreRunning then
  begin
    KillAndRestartTheServerClick(self);
  end
  else
  // in multi-process mode just change the port number since each
  // association re-reads dicom.ini
  begin
    Sleep(500);
    ListenSocket.Close;
    val(trim(TCPIPport.text), p, c);
    Sleep(500);
    ListenSocket.Addr := '0.0.0.0';
    ListenSocket.Port := IntToStr(p);
    Sleep(500);
    ListenSocket.Listen;
  end;

  if changepage then
    PageControl1.ActivePage := TabSheet2;

  Screen.Cursor := crDefault;
end;

function IsServiceInstalled: boolean;
var Registry: TRegistry;
    text: string;
begin
  Registry := TRegistry.Create;
  text := '';
  result := false;
  Registry.RootKey := HKEY_LOCAL_MACHINE;
  if Registry.OpenKeyReadOnly('SYSTEM\CurrentControlSet\services\' + Trim(Form1.ServerName.text)) then
    text := Registry.ReadString('ImagePath');
  Registry.CloseKey;
  if text<>'' then result := true;
  Registry.Free;
end;

procedure TForm1.InstallAsServiceClick(Sender: TObject);
var account, password: string;
    buffer: array[0..255] of Ansichar;
    i: DWORD;
    p: pchar;
begin
  if pos(' ', GetCurrentDir) > 0 then
  begin
    ShowMessage('Sorry, the server cannot be installed as service if its path contains space characters');
    exit;
  end;

  if MessageDlg('(Re)install the DICOM server as NT service ?', mtConfirmation,
    [mbYes, mbNo], 0) = mrNo then Exit;

  i := 256;
  p := @buffer;
  GetUserName(p, i);
  account := '.\' + buffer;

//  if not InputQuery('Account for server', 'Account (Domain\Username or Computer\Username)', account) then exit;
//  if not InputQuery('Password for account', 'Password', password) then exit;
  account  := '';
  password := '';

  if ThreadedProcess<>0 then
  begin
    TerminateProcess(ThreadedProcess, 0);
    CloseHandle(ThreadedProcess);
    ThreadedProcess := 0;
  end;

  RunProgramBlocking(curdir + '\dgateserv.exe', curdir,
                     ['/service', trim(ServerName.text),
                      '/uninstall'
                     ]);

  Sleep(500);

  if IsServiceInstalled then
  begin
    ShowMessage('Failed to uninstall existing service - you may need to run this program as administrator');
    exit;
  end;

  RunProgramBlocking(curdir + '\dgateserv.exe', curdir,
                     ['/service', trim(ServerName.text),
                      '/exename',
                      '"' + CurDir + '\dgateserv.exe /process '+dgateExe+' -^serverstatus.log"',
                      '/install', UpperCase(account), password
                      ]);
  Sleep(500);

  RunProgramBlocking(curdir + '\dgateserv.exe', curdir,
                    ['/service', trim(ServerName.text),
                     '/start'
                    ]);

  if not IsServiceInstalled then
  begin
    ShowMessage('Failed to install service - you may need to run this program as administrator');
    KillAndRestartTheServerClick(nil);
    exit;
  end;

  PreRunning := true;
  ServerNameChange(self);

  Sleep(1000);

  ServerTask('', 'log_on:'+ServerStatusSocket.Port);
  Sleep(500);

  TestLocalServer(newinstall, false);
  CheckBoxDebugLog.Checked := false;
  CheckBoxWebServer.Checked := false;
  CheckBoxOnlyLogToFile.Checked := false;
end;

procedure TForm1.InstallAsServiceMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
var account, password: string;
    i : integer;
begin
  if not (ssAlt in Shift) then exit;
  if Button <> mbRight    then exit;

  if pos(' ', GetCurrentDir) > 0 then
  begin
    ShowMessage('Sorry, the server cannot be installed as service if its path contains space characters');
    exit;
  end;

  if MessageDlg('(Re)install the DICOM server as four NT services (with consecutive DICOM ports)?', mtConfirmation,
    [mbYes, mbNo], 0) = mrNo then Exit;

  // if not InputQuery('Account for server (DomainName\UserName)', 'Account (Domain or ComputerName\Username)', account) then exit;
  // if not InputQuery('Password for account', 'Password', password) then exit;
  account  := '';
  password := '';

  if ThreadedProcess<>0 then
  begin
    TerminateProcess(ThreadedProcess, 0);
    CloseHandle(ThreadedProcess);
    ThreadedProcess := 0;
  end;

  RunProgramBlocking(curdir + '\dgateserv.exe', curdir,
                    ['/service', trim(ServerName.text),
                     '/uninstall'
                    ]);
  for i:=2 to 4 do
    RunProgramBlocking(curdir + '\dgateserv.exe', curdir,
                      ['/service', trim(ServerName.text) + '_' + IntToStr(i),
                       '/uninstall'
                      ]);

  Sleep(500);

  if IsServiceInstalled then
  begin
    ShowMessage('Failed to uninstall existing service - you may need to run this program as administrator');
    exit;
  end;

  RunProgramBlocking(curdir + '\dgateserv.exe', curdir,
                     ['/service', trim(ServerName.text),
                      '/exename',
                      '"' + CurDir + '\dgateserv.exe /process '+dgateExe+' -^serverstatus.log"',
                      '/install', UpperCase(account), password
                      ]);

  for i:=2 to 4 do
    RunProgramBlocking(curdir + '\dgateserv.exe', curdir,
                       ['/service', trim(ServerName.text) + '_' + IntToStr(i),
                        '/exename',
                        '"' + CurDir + '\dgateserv.exe /process '+dgateExe+' -^serverstatus.log' +
                          ' -p' + IntToStr(StrToInt(Trim(TCPIPport.Text))+i-1)+ '"',
                        '/install', UpperCase(account), password
                        ]);

  Sleep(500);

  RunProgramBlocking(curdir + '\dgateserv.exe', curdir,
                    ['/service', trim(ServerName.text),
                     '/start'
                    ]);

  for i:=2 to 4 do
    RunProgramBlocking(curdir + '\dgateserv.exe', curdir,
                      ['/service', trim(ServerName.text) + '_' + IntToStr(i),
                       '/start'
                      ]);

  if not IsServiceInstalled then
  begin
    ShowMessage('Failed to install service - you may need to run this program as administrator');
    KillAndRestartTheServerClick(nil);
    exit;
  end;

  PreRunning := true;
  ServerNameChange(self);

  Sleep(1000);

  ServerTask('', 'log_on:'+ServerStatusSocket.Port);
  Sleep(500);

  TestLocalServer(newinstall, false);
  CheckBoxDebugLog.Checked := false;
  CheckBoxWebServer.Checked := false;
  CheckBoxOnlyLogToFile.Checked := false;
end;

procedure TForm1.UninstallServerAsServiceClick(Sender: TObject);
begin
  if not IsServiceInstalled then
  begin
    ShowMessage('Server is not installed as service');
    exit;
  end;

  if MessageDlg('Uninstall the DICOM server as NT service ?', mtConfirmation,
    [mbYes, mbNo], 0) = mrNo then Exit;

  if ThreadedProcess<>0 then
    TerminateProcess(ThreadedProcess, 0);

  RunProgramBlocking(curdir + '\dgateserv.exe', curdir,
                    ['/service', trim(ServerName.text),
                     '/uninstall'
                    ]);
  Sleep(500);

  if IsServiceInstalled then
  begin
    ShowMessage('Failed to uninstall service - you may need to run this program as administrator');
    exit;
  end;

  PreRunning := false;
  ServerNameChange(self);

  ThreadedProcess := RunDgate('-!'+ServerStatusSocket.Port+' -x!'+ProgressSocket.Port, true);
  Sleep(1000);
  TestLocalServer(false, false);
  CheckBoxDebugLog.Checked := false;
  CheckBoxWebServer.Checked := false;
  CheckBoxOnlyLogToFile.Checked := false;
end;

procedure TForm1.UninstallServerAsServiceMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
var i : integer;
begin
  if not (ssAlt in Shift) then exit;
  if Button <> mbRight    then exit;

  if not IsServiceInstalled then
  begin
    ShowMessage('Server is not installed as service');
    exit;
  end;

  if MessageDlg('Uninstall the DICOM server as multiple NT services ?', mtConfirmation,
    [mbYes, mbNo], 0) = mrNo then Exit;

  if ThreadedProcess<>0 then
    TerminateProcess(ThreadedProcess, 0);

  RunProgramBlocking(curdir + '\dgateserv.exe', curdir,
                    ['/service', trim(ServerName.text),
                     '/uninstall'
                    ]);
  for i:=2 to 4 do
    RunProgramBlocking(curdir + '\dgateserv.exe', curdir, 
                      ['/service', trim(ServerName.text) + '_' + IntToStr(i),
                       '/uninstall'
                      ]);

  Sleep(500);

  if IsServiceInstalled then
  begin
    ShowMessage('Failed to uninstall service - you may need to run this program as administrator');
    exit;
  end;

  PreRunning := false;
  ServerNameChange(self);

  ThreadedProcess := RunDgate('-!'+ServerStatusSocket.Port+' -x!'+ProgressSocket.Port, true);
  Sleep(1000);
  TestLocalServer(false, false);
  CheckBoxDebugLog.Checked := false;
  CheckBoxWebServer.Checked := false;
  CheckBoxOnlyLogToFile.Checked := false;
end;

{************************************************************************}
// Installation page

// Display output from installation programs

procedure TForm1.InstallationSocketDataAvailable(Sender: TObject;
  Error: Word);
  var buffer: array[0..8192] of ansichar;
  s : string;
  i: integer;
begin
  i := InstallationSocket.Receive(@buffer, 8191);
  if i<1 then exit;

  buffer[i] := #00;
  if buffer[i-1] in [#10, #13] then buffer[i-1] := #00;
  s := buffer;
  // s := copy(s, 0, length(s)-1);
  for i:=1 to length(s) do if s[i]<' ' then s[i]:=' ';

  WriteMemo(InstallationMemo, s, 200, 100, 'installation');
end;

// Test UPD connection and application starting

procedure TForm1.VerifyTCPIPButtonClick(Sender: TObject);
begin
  WriteMemo(InstallationMemo, '', 200, 100, 'installation');
  WriteMemo(InstallationMemo, '------------------- Start TCP/IP test --------------------', 200, 100, 'installation');
  RunDgate('-u' + InstallationSocket.Port + ' -t', false); // test TCP/IP
end;

// Create ODBC datasource

procedure TForm1.MakeODBCButtonClick(Sender: TObject);
var dir    : string;
    UseDBF, UseSQLServer, UseDBFWithoutODBC, UseMySQL, UsePostGres: boolean;
//  UseSqLite: boolean;
    server, dbase, login, password, sapw: string;
    Registry : TRegistry;
    i: integer;
begin
  dir := Label10.Caption;

  UseDBF       := false;
  UseMySql     := false;
//  UseSqLite    := false;
  UseSQLServer := false;
  UsePostGres  := false;
  UseDBFWithoutODBC := false;
  if          FileExists(curdir + '\USEMYSQL')               then UseMySQL          := true
//  else if     FileExists(curdir + '\USESQLITE')              then UseSqLite         := true
  else if     FileExists(curdir + '\USEPOSTGRES')            then UsePostgres       := true
  else if     FileExists(curdir + '\USESQLSERVER')           then UseSQLServer      := true
  else if     FileExists(curdir + '\USEDBASEIIIWITHOUTODBC') then UseDBFWithoutODBC := true
  else if not FileExists(curdir + '\USEMSACCESS')            then UseDBF            := true;

  if UseDBFWithoutODBC then
  begin
    ShowMessage('ODBC configuration is not required with this database');
    exit;
  end;

  if (sender<>form1) then
  begin
    if UseMySQL then
    begin
      if MessageDlg('Create conquest database on mysql if it does not exist ?', mtConfirmation,
        [mbYes, mbNo], 0) = mrNo then Exit;
    end
    else if UsePostgres then
    begin
      if MessageDlg('Create conquest database on postgres if it does not exist ?', mtConfirmation,
        [mbYes, mbNo], 0) = mrNo then Exit;
    end
    else if UseMySQL then
    begin
      //if MessageDlg('Create conquest database on sqlite if it does not exist ?', mtConfirmation,
      //  [mbYes, mbNo], 0) = mrNo then Exit;
      Exit;
    end
    else if not UseSQLserver then
    begin
      if MessageDlg('Update the ODBC data source (replaces any manual ODBC configuration of the '+DataSource+' datasource) ?', mtConfirmation,
        [mbYes, mbNo], 0) = mrNo then Exit;
    end
    else if (sender<>form1) and UseSQLserver then
    begin
      if MessageDlg('Update the ODBC data source (replaces manual ODBC config of the '+DataSource+' datasource) ?'+#10+
                  '(Will also attempt to create database and login if they do not exist)', mtConfirmation,
        [mbYes, mbNo], 0) = mrNo then Exit;
    end;
  end;

  WriteMemo(InstallationMemo, '', 200, 100, 'installation');
  WriteMemo(InstallationMemo, '------------------- Start (ODBC) data source update or creation --------------------', 200, 100, 'installation');

  if UseDBF then
  begin
    RunDgate('-u' + InstallationSocket.Port +
                 ' "-sMicrosoft dBase Driver (*.dbf);DSN='+DataSource+';'+
                 'Description=ConQuest DICOM server data source;'+
                 'Fil=dBase III;'+
                 'DriverID=21;'+
                 'Deleted=1;'+
                 'PAGETIMEOUT=600;'+
                 'DefaultDir=' + dir + '\dbase"', false);
  end
  else if UseMySQL then
  begin
    dbase    := 'conquest';
    password := '';
    if not InputQuery('Database Name',  'Database', dbase    ) then exit;
    if not InputQuery('Root password',  'Password', password ) then exit;

    DataSource  := dbase;
    SqlPassWord := password;
    SaveConfigButtonClick(nil);

    RunDgate('-u' + InstallationSocket.Port + ' -e'+password+' '+dbase+' root '+password, false);

    // change TCPIP parameters for mysql choke problem
    Registry := TRegistry.Create;
    Registry.RootKey := HKEY_LOCAL_MACHINE;

    if Registry.OpenKey('SYSTEM\CurrentControlSet\Services\Tcpip\Parameters', false) then
    begin
      if not(Registry.ValueExists('MaxUserPort') and Registry.ValueExists('TcpTimedWaitDelay')) then
        if MessageDlg('Change TCPIP parameters to fix mysql problems ?', mtConfirmation, [mbYes, mbNo], 0) = mrYes then
      begin
        Registry.WriteInteger('MaxUserPort', 65534);
        Registry.WriteInteger('TcpTimedWaitDelay', 30);
      end;
    end;
    Registry.CloseKey;
    Registry.Free;
  end
  else if UsePostGres then
  begin
    WriteMemo(InstallationMemo, 'Note: for Postgres server will also attempt to create database and login (no problem if exists)', 200, 100, 'installation');
    server   := '127.0.0.1';
    dbase    := 'conquest';
    login    := 'conquest';
    password := 'conquest';
    sapw     := '';
    if not InputQuery('SQL Server Name', 'Server',   server   ) then exit;
    if not InputQuery('Database Name',   'Database', dbase    ) then exit;
    if not InputQuery('Login',           'Login',    login    ) then exit;
    if not InputQuery('Password',        'Password', password ) then exit;
    if not InputQuery('postgres Password','SA pw',    sapw     ) then exit;

    SQLHost     := server;
    DataSource  := dbase;
    SqlUser     := login;
    SqlPassWord := password;
    SaveConfigButtonClick(nil);

    RunDgate('-u' + InstallationSocket.Port + ' -e'+sapw+' '+dbase+' '+login+' '+password, false);
  end
  else if UseSQLserver then
  begin
    WriteMemo(InstallationMemo, 'Note: for SQL server will also attempt to create database and login (no problem if exists)', 200, 100, 'installation');
    server   := '(local)';
    dbase    := 'conquest';
    login    := 'conquest';
    password := 'conquest1415';
    sapw     := '';
    if not InputQuery('SQL Server Name', 'Server',   server   ) then exit;
    if not InputQuery('Database Name',   'Database', dbase    ) then exit;
    if not InputQuery('Login',           'Login',    login    ) then exit;
    if not InputQuery('Password',        'Password', password ) then exit;
    if not InputQuery('SA Password',     'SA pw',    sapw     ) then exit;
    RunDgate('-u' + InstallationSocket.Port +
                 ' "-sSQL Server;DSN='+DataSource+';'+
                 'Description=ConQuest DICOM server data source;'+
                 'Server='+server+';'+
                 'Database='+'master', false);

    Sleep(1000);
    RunDgate('-u' + InstallationSocket.Port +
                 ' -e'+sapw+' '+dbase+' '+login+' '+password, false);

    Sleep(1000);
    RunDgate('-u' + InstallationSocket.Port +
                 ' "-sSQL Server;DSN='+DataSource+';'+
                 'Description=ConQuest DICOM server data source;'+
                 'Server='+server+';'+
                 'Database='+dbase, false);

    SqlPassWord := password;
    SqlUser     := login;
    SaveConfigButtonClick(self);
  end
  else
  begin
    if not FileExists(dir + '\dbase\'+DataSource+'.mdb') then
    begin
      RunDgate('-u' + InstallationSocket.Port +
                 ' "-sMicrosoft Access Driver (*.mdb);DSN='+DataSource+';'+
                 'Description=ConQuest DICOM server data source;'+
                 'Fil=MS Access;'+
                 'DriverID=25;'+
                 'PAGETIMEOUT=600;'+
                 'CREATE_DB='  + ExtractShortPathName(dir + '\dbase') + '\' + DataSource+'.mdb;'+
                 'DefaultDir=' + dir + '\dbase;'+
                 'DBQ=' + DataSource+'.mdb;'+
                 '"', false);
      Sleep(2000);
    end;

    RunDgate('-u' + InstallationSocket.Port +
                 ' "-sMicrosoft Access Driver (*.mdb);DSN='+DataSource+';'+
                 'Description=ConQuest DICOM server data source;'+
                 'Fil=MS Access;'+
                 'DriverID=25;'+
                 'PAGETIMEOUT=600;'+
                 'DefaultDir=' + dir + '\dbase;'+
                 'DBQ=' + DataSource+'.mdb;'+
                 '"', false);
  end;

  NewInstall := false;
  for i:=1 to 8 do
  begin
    if TestLocalServer(false, true) then break;
    WriteMemo(InstallationMemo, 'waiting ..', 200, 100, 'installation');
    Sleep(500);
  end;
  TestLocalServer(false, false);
  WriteMemo(InstallationMemo, '------------------- End data source update or creation --------------------', 200, 100, 'installation');
end;

// Test if ODBC works well

procedure TForm1.VerifyDatabaseButtonClick(Sender: TObject);
begin
  WriteMemo(InstallationMemo, '', 200, 100, 'installation');
  WriteMemo(InstallationMemo, '------------------- Start ODBC test --------------------', 200, 100, 'installation');
  RunDgate('-u' + InstallationSocket.Port + ' -o', false); // test ODBC

  // Update UIDMODS table from 1.4.17 format to 1.4.19 format if needed
  if NewInstallDone then
    ServerTask('', 'lua:local a=dbquery("UIDMODS", "Stage", "Stage=''1''");if a==nil then sql("ALTER TABLE UIDMODS ADD Stage varchar(32)"); sql("ALTER TABLE UIDMODS ADD COLUMN Annotation varchar(64)"); print("updated UIDMODS table"); end');

  // Optimise UIDMODS index (fails harmlessly if already exists)
  ServerTask('', 'lua:sql("CREATE INDEX mods_joint ON UIDMODS (OldUID, Stage)")');

  // Optimise joint dicom table indices (fails harmlessly if already exists)
  ServerTask('', 'lua:sql("CREATE INDEX idx_patientid             ON DICOMStudies(PatientID             );")');
  ServerTask('', 'lua:sql("CREATE INDEX idx_patientid_studyinsta  ON DICOMStudies(PatientID,  StudyInsta);")');
  ServerTask('', 'lua:sql("CREATE INDEX idx_studyinsta_seriesinst ON DICOMSeries (StudyInsta, SeriesInst);")');
  ServerTask('', 'lua:sql("CREATE INDEX idx_seriesinst_sopinstanc ON DICOMImages (SeriesInst, SOPInstanc);")');

  NewInstall := false;
  TestLocalServer(false, false);
end;

// Re-build full database from all devices (can take a long time on a TB database)

procedure TForm1.InitDatabaseButtonClick(Sender: TObject);
var
  UseDBF : boolean;
begin
  UseDBF := true;
  if FileExists(curdir + '\USESQLSERVER')           then UseDBF := false;
  if FileExists(curdir + '\USEDBASEIIIWITHOUTODBC') then UseDBF := false;
  if FileExists(curdir + '\USEMYSQL')               then UseDBF := false;
  if FileExists(curdir + '\USESQLITE')              then UseDBF := false;
  if FileExists(curdir + '\USEPOSTGRES')            then UseDBF := false;
  if FileExists(curdir + '\USEMSACCESS')            then UseDBF := false;

  if sender<>form1 then
    if MessageDlg('Initialize/re-generate the database from the DICOM images (may take a while) ?', mtConfirmation,
      [mbYes, mbNo], 0) = mrNo then Exit;

  WriteMemo(InstallationMemo, '', 200, 100, 'installation');
  WriteMemo(InstallationMemo, '------------------- Start database init and regeneration --------------------', 200, 100, 'installation');
  WriteLog('started database regeneration');

  if (Pos('\', DataSource)>0) and (FSqLite='0') then
    ServerTask('initialize db from GUI', 'initializetables:'); // clear index
  if UseDBF then
    RunProgramWaiting(dgateExe, curdir,['-u'+InstallationSocket.Port + ' -r1'])
  else
    RunProgramWaiting(dgateExe, curdir,['-u'+InstallationSocket.Port + ' -r']);

  if (Pos('\', DataSource)>0) and (FSqLite='0') then
  begin
    PageControl1.ActivePage := TabSheet6;
    ServerTask('re-indexing db from GUI', 'indexdbf:');
    PageControl1.ActivePage := TabSheet2;
  end;
  WriteMemo(InstallationMemo, '------------------- Finished database regeneration --------------------', 200, 100, 'installation');
  NewInstallDone := true;
end;

// Clear the worklist database (and its data)

procedure TForm1.ButtonClearWorkListClick(Sender: TObject);
begin
  if MessageDlg('Clear the contents of the DICOM Worklist database ?', mtConfirmation,
    [mbYes, mbNo], 0) = mrNo then Exit;

  WriteMemo(InstallationMemo, '', 200, 100, 'maintenance');
  WriteMemo(InstallationMemo, '------------------- Clearing worklist --------------------', 200, 100, 'maintenance');
  WriteLog('cleared worklist');

  if (Pos('\', DataSource)>0) and (FSqLite='0') then
    ServerTask('initialize worklist from GUI', 'initializetables:2')
  else
    RunDgate('-!'+MaintenanceSocket.Port+' -i2', false);  // clear worklist database
end;

{************************************************************************}
// Maintenance page

// Display output from maintenance applications

procedure TForm1.MaintenanceSocketDataAvailable(Sender: TObject;
  Error: Word);
  var buffer: array[0..8192] of Ansichar;
  s : string;
  i: integer;
begin
  i := MaintenanceSocket.Receive(@buffer, 8191);
  if i<1 then exit;

  buffer[i] := #00;
  if buffer[i-1] in [#10, #13] then buffer[i-1] := #00;
  s := buffer;
  // s := copy(s, 0, length(s)-1);
  for i:=1 to length(s) do if s[i]<' ' then s[i]:=' ';

  WriteMemo(MaintenanceMemo, s, 200, 100, 'maintenance');
end;

// Re-build the complete database

procedure TForm1.ReInitDatabaseButtonClick(Sender: TObject);
var
  UseDBF : boolean;
begin
  UseDBF := true;
  if     FileExists(curdir + '\USESQLSERVER')           then UseDBF := false;
  if     FileExists(curdir + '\USEDBASEIIIWITHOUTODBC') then UseDBF := false;
  if     FileExists(curdir + '\USEMYSQL')               then UseDBF := false;
  if     FileExists(curdir + '\USESQLITE')              then UseDBF := false;
  if     FileExists(curdir + '\USEPOSTGRES')            then UseDBF := false;
  if     FileExists(curdir + '\USEMSACCESS')            then UseDBF := false;

  if MessageDlg('Re-generate the database from the DICOM images (may take a while) ?', mtConfirmation,
    [mbYes, mbNo], 0) = mrNo then Exit;

  WriteMemo(MaintenanceMemo, '', 200, 100, 'maintenance');
  WriteMemo(MaintenanceMemo, '------------------- Start database regeneration --------------------', 200, 100, 'maintenance');

  WriteLog('started database regeneration');

  if (Pos('\', DataSource)>0) and (FSqLite='0') then
    ServerTask('initialize db from GUI', 'initializetables:'); // clear index
  if UseDBF then
    RunProgramWaiting(dgateExe, curdir,['-u'+MaintenanceSocket.Port + ' -r1'])
  else
    RunProgramWaiting(dgateExe, curdir,['-u'+MaintenanceSocket.Port + ' -r']);

  if (Pos('\', DataSource)>0) and (FSqLite='0') then
  begin
    PageControl1.ActivePage := TabSheet6;
    ServerTask('re-indexing db from GUI', 'indexdbf:');
    PageControl1.ActivePage := TabSheet3;
  end;
  WriteMemo(MaintenanceMemo, '------------------- Finished database regeneration --------------------', 200, 100, 'installation');
  NewInstallDone := true;
end;

// List device(s) and free space

procedure TForm1.ShowDeviceButtonClick(Sender: TObject);
begin
  WriteMemo(MaintenanceMemo, '', 200, 100, 'maintenance');
  WriteMemo(MaintenanceMemo, '------------------- Show free space on devices --------------------', 200, 100, 'maintenance');
  RunDgate('-u'+MaintenanceSocket.Port+' -d', false); // devices
end;

var LastFile : string;

procedure TForm1.ShowDICOMfilesinexplorer1Click(Sender: TObject);
begin
  if LastFile<>'' then
    ShellExecute(Application.Handle, nil,
      'explorer.exe', pchar('/n,'+Lastfile + ',/Select,'+Lastfile),
      nil, SW_NORMAL)
  else
    ShowMessage('Select image first');
end;

procedure TForm1.ShowPopupMenu1(Sender: TObject);
begin
  DumpHeader1.Enabled := Table4.Active;
  SaveasBitmap1.Enabled := Table4.Active;
  ShowDICOMfilesinexplorer1.Enabled := Table4.Active;
  RunKPacsviewer1.Enabled := Table4.Active;
  Runexternalviewer1.Enabled := Table4.Active;
  FormModifyDICOM.Button3.Enabled := Table4.Active;
end;

// List acr/nema map file as interpreted by dgate

procedure TForm1.ListDICOMprovidersButtonClick(Sender: TObject);
begin
  WriteMemo(MaintenanceMemo, '', 200, 100, 'maintenance');
  WriteMemo(MaintenanceMemo, '------------------- Show DICOM known providers --------------------', 200, 100, 'maintenance');
  RunDgate('-u'+MaintenanceSocket.Port+' -m', false); // map
end;

// List DICOM->SQL translation

procedure TForm1.ListDatabaseLayioutButtonClick(Sender: TObject);
begin
  WriteMemo(MaintenanceMemo, '', 200, 100, 'maintenance');
  WriteMemo(MaintenanceMemo, '------------------- Show Database layout --------------------', 200, 100, 'maintenance');
  RunDgate('-u'+MaintenanceSocket.Port+' -k', false); // kfactor file
end;

// Clear the database (used to allow partial regeneration)

procedure TForm1.ButtonClearDatabaseClick(Sender: TObject);
var
  UseDBF : boolean;
begin
  UseDBF := true;
  if FileExists(curdir + '\USESQLSERVER')           then UseDBF := false;
  if FileExists(curdir + '\USEDBASEIIIWITHOUTODBC') then UseDBF := false;
  if FileExists(curdir + '\USEMYSQL')               then UseDBF := false;
  if FileExists(curdir + '\USESQLITE')              then UseDBF := false;
  if FileExists(curdir + '\USEPOSTGRES')            then UseDBF := false;
  if FileExists(curdir + '\USEMSACCESS')            then UseDBF := false;

  if MessageDlg('Clear the contents of the DICOM image database ?', mtConfirmation,
    [mbYes, mbNo], 0) = mrNo then Exit;

  WriteMemo(MaintenanceMemo, '', 200, 100, 'maintenance');
  WriteMemo(MaintenanceMemo, '------------------- Clearing database --------------------', 200, 100, 'maintenance');
  WriteLog('cleared database');

  if (Pos('\', DataSource)>0) and (FSqLite='0') then
    ServerTask('initialize db from GUI', 'initializetables:')
  else if UseDBF then
    RunDgate('-!'+MaintenanceSocket.Port+' -i1', false)  // clear simple database
  else
    RunDgate('-!'+MaintenanceSocket.Port+' -i', false);  // clear database
  NewInstallDone := true;
end;

// Pack the dbase files

procedure TForm1.ButtonPackDatabaseClick(Sender: TObject);
begin
  if Pos('\', DataSource)=0 then exit;

  if MessageDlg('Pack the contents of the DICOM image database (may take a while) ?', mtConfirmation,
    [mbYes, mbNo], 0) = mrNo then Exit;

  WriteMemo(MaintenanceMemo, '', 200, 100, 'maintenance');
  WriteMemo(MaintenanceMemo, '------------------- Packing database --------------------', 200, 100, 'maintenance');

  ServerTask('pack db from GUI', 'packdbf:');

  WriteMemo(MaintenanceMemo, '------------------- Packing done --------------------', 200, 100, 'maintenance');

  WriteLog('packed database');
end;

// Regenerate the database from a single disk device like MAG0 or JUKEBOX0.24

procedure TForm1.RegenSelectedButtonClick(Sender: TObject);
var s: string;
begin
  s := 'MAG0';
  if InputQuery('Select device to regenerate (e.g., MAG0)', 'Device', s) then
  begin
    s := UpperCase(s);
    WriteMemo(MaintenanceMemo, '', 200, 100, 'maintenance');
    WriteMemo(MaintenanceMemo, '------------------- Regen device ' + s + ' --------------------', 200, 100, 'maintenance');
    WriteLog('Regen device ' + s);
    RunDgate('-u'+MaintenanceSocket.Port+' -ar' + s, false); // regen single device

    if (Pos('\', DataSource)>0) and (FSqLite='0') then
    begin
      PageControl1.ActivePage := TabSheet6;
      ServerTask('re-indexing db from GUI', 'indexdbf:');
      PageControl1.ActivePage := TabSheet3;
    end;

    WriteMemo(MaintenanceMemo, '------------------- Finished Regen device --------------------', 200, 100, 'maintenance');
  end;
end;

{************************************************************************}
// Known DICOM providers page

var RegenFolderRunning:Boolean=false;

procedure TForm1.ButtonRegenFolderClick(Sender: TObject);
var s, t, u: string;
begin
  if RegenFolderRunning then
  begin
    RegenFolderRunning := false;
    ShowMessage('Stopped regenerate folder');
    exit;
  end;
  s := 'MAG0';
  if InputQuery('Select device to regenerate (e.g., MAG0)', 'Device', s) then
  begin
    if InputQuery('Select folder(s) to regenerate (e.g., 0009703828 or 123,456)', 'Folder', t) then
    begin
      RegenFolderRunning := true;
      s := UpperCase(s);

      repeat
        u := t;

        if pos(',', t)>1 then
        begin
          u := copy(t, 1, pos(',', t)-1);
          t := copy(t, pos(',', t)+1, 9999);
        end
        else
          t := '';

        WriteMemo(MaintenanceMemo, '', 200, 100, 'maintenance');
        WriteMemo(MaintenanceMemo, '------------------- Regen folder ' + s + '/' + u + ' --------------------', 200, 100, 'maintenance');
        WriteLog('Regen device ' + s);
        RunDgate('-u'+MaintenanceSocket.Port+' -fr' + s + ',' + u, false); // regen single folder on device
        WriteMemo(MaintenanceMemo, '------------------- Finished Regen folder --------------------', 200, 100, 'maintenance');
        Application.ProcessMessages();
      until (length(t)=0) or (not RegenFolderRunning);
      RegenFolderRunning := false;
    end;
  end;
end;

procedure TForm1.ButtonRegenFolderMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
  var s, t: string;
      f : textfile;
begin
  if not (ssAlt in Shift) then exit;
  if RegenFolderRunning then
  begin
    RegenFolderRunning := false;
    ShowMessage('Stopped regenerate folder');
    exit;
  end;
  s := 'MAG0';
  if InputQuery('Select device to regenerate (e.g., MAG0)', 'Device', s) then
  begin
    OpenDialog1.Title       := 'Select indirect file with one line per folder';
    OpenDialog1.Filename    := '*.*';
    OpenDialog1.Filter      := '*.*';
    OpenDialog1.FilterIndex := 1;
    OpenDialog1.Options    := [];
    if OpenDialog1.Execute then
    begin
      RegenFolderRunning := true;
      AssignFile(f, OpenDialog1.FileName);
      Reset(f);
      while (not eof(f)) and RegenFolderRunning do
      begin
        Readln(f, t);
        if trim(t)<>'' then
        begin
          WriteMemo(MaintenanceMemo, '', 200, 100, 'maintenance');
          WriteMemo(MaintenanceMemo, '------------------- Regen folders ' + s + '/' + t + ' --------------------', 200, 100, 'maintenance');
          WriteLog('Regen device ' + s);
          RunDgate('-u'+MaintenanceSocket.Port+' -fr' + s + ',' + t, false); // regen single folder on device
          WriteMemo(MaintenanceMemo, '------------------- Finished Regen folder --------------------', 200, 100, 'maintenance');
          Application.ProcessMessages();
        end;
      end;
      CloseFile(f);
      RegenFolderRunning := false;
    end;
  end;
end;

procedure TForm1.btnDoItNowClick(Sender: TObject);
begin
   DoNightlyToMoveNow := true;
end;

// Save ACRNEMA.MAP
procedure TForm1.Button1Click(Sender: TObject);
begin
  if MessageDlg('Save and use new ACRNEMA.MAP ?', mtConfirmation,
    [mbYes, mbNo], 0) = mrYes then
  begin
    DICOMMap.Lines.SaveToFile(CurDir + '\acrnema.map');
    FillAELists;
    ServerTask('Re-reading acrnema.map from GUI', 'read_amap:');

    CopyFile(PChar(curdir + '\acrnema.map'), PChar(curdir + '\webserver\cgi-bin\acrnema.map'), false);
//    CopyFile(PChar(curdir + '\acrnema.map'), PChar(curdir + '\webserver\cgi-bin\newweb\acrnema.map'), false);
  end;
  //KillAndRestartTheServerClick(self);
end;

// Load ACRNEMA.MAP

procedure TForm1.RestoreListButtonClick(Sender: TObject);
begin
  DICOMMap.Lines.LoadFromFile(CurDir + '\acrnema.map');
  FillAELists;
end;

{************************************************************************}
// Browse database page
// open/close database when switching pages, disable filters and make dropdown list

var B1, B2, B3, B4: TBookmark;
var BookMarkSaved: boolean = False;
var ShowPict: boolean = True;
var BDEInstalled: integer=-1;

var RefreshLevel: integer=0;
    CurrentPatientID: string;
    NextPatientID: string;
    CurrentStudyUID: string;
    NextStudyUID: string;
    CurrentSeriesUID: string;
    NextSeriesUID: string;
    CurrentSOPUID: string;
    NextSOPUID: string;

procedure SetRefreshLevel(Level: integer; prog: Boolean = true);
begin
  if not Assigned(Form1.Table1) then exit;

  RefreshLevel := Level;
  if (Level=1) and Form1.Table1.Active then
  begin
    form1.DataSource1.Enabled := false;
    CurrentPatientID := form1.Table1.FieldByName('PatientID').AsString;
    form1.Table1.Next;
    NextPatientID := form1.Table1.FieldByName('PatientID').AsString;
    form1.Table1.Prior;
    form1.DataSource1.Enabled := true;
  end;

  if (Level=2) and Form1.Table2.Active then
  begin
    form1.DataSource2.Enabled := false;
    CurrentStudyUID := form1.Table2.FieldByName('STUDYINSTA').AsString;
    form1.Table2.Next;
    NextStudyUID := form1.Table2.FieldByName('STUDYINSTA').AsString;
    form1.Table2.Prior;
    form1.DataSource2.Enabled := true;
  end;

  if (Level=3) and Form1.Table3.Active  then
  begin
    form1.DataSource3.Enabled := false;
    CurrentSeriesUID := form1.Table3.FieldByName('SERIESINST').AsString;
    form1.Table3.Next;
    NextStudyUID := form1.Table3.FieldByName('SERIESINST').AsString;
    form1.Table3.Prior;
    form1.DataSource3.Enabled := true;
  end;

  if (Level=4) and Form1.Table4.Active  then
  begin
    form1.DataSource4.Enabled := false;
    CurrentSOPUID := form1.Table4.FieldByName('SOPINSTANC').AsString;
    form1.Table4.Next;
    NextSOPUID := form1.Table4.FieldByName('SOPINSTANC').AsString;
    form1.Table4.Prior;
    form1.DataSource4.Enabled := true;
  end;

  // make sure it shows up early
  if prog and not Form1.CheckBoxDebugLog.Checked then
  begin
    Form1.ProgressBar2.Visible := true;
    Form1.ProgressBar2.Max := 100;
    Form1.ProgressBar2.Position := 1;
  end;
end;

// Update displayed database grids
procedure RefreshDatabase;
begin
  if RefreshLevel=1 then
  begin
    Form1.Timer1Timer(Form1.Timer1);
    if not Form1.Table1.Locate('PatientID', CurrentPatientID, []) then
      Form1.Table1.Locate('PatientID', NextPatientID, []);
  end;

  if RefreshLevel=2 then
  begin
//    form1.DBText1DblClick(form1.DbText1);
    form1.DBGrid2DblClick(form1.DbGrid2);
    if not Form1.Table2.Locate('STUDYINSTA', CurrentStudyUID, []) then
      Form1.Table2.Locate('STUDYINSTA', NextStudyUID, []);
  end;

  if RefreshLevel=3 then
  begin
//    form1.DBGrid2DblClick(form1.DbGrid2);
    form1.DBText1DblClick(form1.DBText1);
    if not Form1.Table3.Locate('SERIESINST', CurrentSeriesUID, []) then
      Form1.Table3.Locate('SERIESINST', NextSeriesUID, []);
  end;

  if RefreshLevel=4 then
  begin
    form1.DBGrid1DblClick(form1.DbGrid1);
    if not Form1.Table4.Locate('SOPINSTANC', CurrentSOPUID, []) then
      Form1.Table4.Locate('SOPINSTANC', NextSOPUID, []);
  end;

  RefreshLevel := 0;
end;

function processfilter(s: string): string;
var i: integer;
begin
  result := '';
  for i:=1 to length(s) do
  begin
    if   s[i] = '''' then result := result + ''''''
    else result := result + s[i];
  end;
end;

var PatSort, StudySort, SeriesSort, ImageSort: string;

procedure TForm1.PageControl1Change(Sender: TObject);
begin
  DeleteFile('move.txt');

  if not FileExists(curdir + '\dicom.ini') then
  begin
    PageControl1.ActivePage := TabSheet1;
    ShowMessage('Configuration must be saved before installation can continue');
  end;

  if PageControl1.ActivePage = TabSheet5 then
  begin
    Form1.PopupMenu := Form1.PopupMenu1;
    PopupMenu1.OnPopup := ShowPopupMenu1;

    // do not redisplay previous picture
    ShowPict := not BookMarkSaved;

    if not SysUtils.DirectoryExists(MagDeviceList[0]+'dbase') then
      MkDir(MagDeviceList[0]+'dbase');

    if not MDBFTable1.Active then
    begin
      Table1 := MDBFTable1;
      Table2 := MDBFTable2;
      Table3 := MDBFTable3;
      Table4 := MDBFTable4;
      DataSource1.DataSet := Table1;
      DataSource2.DataSet := Table2;
      DataSource3.DataSet := Table3;
      DataSource4.DataSet := Table4;
    end;

    Timer1Timer(self);

    if Table1.Active then
    begin
      if Form1.ComboBox1.Text<>'' then
        Form1.DbGrid2.hint := 'Filtered = ' + IntToStr(Form1.Table1.RecordCount) + ' patients'
      else
        Form1.DbGrid2.hint := 'Total = ' + IntToStr(Form1.Table1.RecordCount) + ' patients';

      if PatSort<>'' then
        DbGrid2.hint := DbGrid2.hint + ' (sorted on '+PatSort+')';
    end;

    // make a list of all patients in the combobox

    DataSource1.Enabled := False;
    DataSource2.Enabled := False;
    DataSource3.Enabled := False;
    if not Table1.Active then Table1.Open;
    Table1.First;
    ComboBox1.Items.Clear;
    while not Table1.Eof do
    begin
      ComboBox1.Items.Add(Table1.Fields[0].AsString);
      Table1.Next;
      if ComboBox1.Items.Count > 10000 then
      begin
        ComboBox1.Items.Add('** list truncated **');
        break;
      end;
    end;
    Table1.First;
    DataSource1.Enabled := True;
    DataSource2.Enabled := True;
    DataSource3.Enabled := True;
    RefreshDatabase;
  end
  else
  // Save database position prior to closing
  begin
    Form1.PopupMenu := nil;
    if Form4.Visible then Form4.Hide;
    SetRefreshLevel(1, false);

    MDBFTable1.Close;
    if FileExists(MDBFTable1.FileName) then DeleteFile(MDBFTable1.FileName);
    MDBFTable2.Close;
    if FileExists(MDBFTable2.FileName) then DeleteFile(MDBFTable2.FileName);
    MDBFTable3.Close;
    if FileExists(MDBFTable3.FileName) then DeleteFile(MDBFTable3.FileName);
    MDBFTable4.Close;
    if FileExists(MDBFTable4.FileName) then DeleteFile(MDBFTable4.FileName);
  end;
end;

// link combobox to filter for patient, but with some delay

procedure TForm1.ComboBox1KeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
  if Key = 13 then
  begin
    Timer1Timer(self);
  end;
end;

procedure TForm1.ComboBox1Exit(Sender: TObject);
begin
  Timer1Timer(self);
end;

procedure TForm1.ComboBox1Change(Sender: TObject);
begin
  Timer1.Enabled := False;
  Timer1.Enabled := True;
end;

// Set filter on patient table

procedure TForm1.Timer1Timer(Sender: TObject);
var dbdir: string;
begin
  if not assigned(Table1) then exit;
  dbdir := GetTempDir + 'conquest_browser' + DirSep;

  if ComboBox1.Text<>'' then
  begin
    Table1.Close;

    if FileExists(curdir + '\USEDBASEIIIWITHOUTODBC') then
      ServerTask('',
      'todbf:'+GuiToServerFilename(dbdir)+'|DICOMPatients|PatientID LIKE '+'''%'+ComboBox1.Text+'%''|'+PatSort)
    else
      ServerTask('',
      'todbf:'+GuiToServerFilename(dbdir)+'|DICOMPatients|PatientID LIKE '+'''%'+ComboBox1.Text+'%'' OR PatientNam LIKE '+'''%'+ComboBox1.Text+'%''|'+PatSort);

    MDBFTable1.FileName := dbdir+'DICOMPatients.DBF';
    WaitForFile(MDBFTable1.FileName, 1);
    if FileExists(MDBFTable1.FileName) then
    begin
      Table1.Open;
      DbGrid2.hint := 'Filtered = ' + IntToStr(table1.RecordCount) + ' patients';
      if PatSort<>'' then
        DbGrid2.hint := DbGrid2.hint + ' (sorted on '+PatSort+')';
    end
  end
  else
  begin
    Table1.Close;

    ServerTask('',
      'todbf:'+GuiToServerFilename(dbdir)+'|DICOMPatients||'+PatSort+'|10000');
    MDBFTable1.FileName := dbdir+'DICOMPatients.DBF';
    WaitForFile(MDBFTable1.FileName, 1);
    if FileExists(MDBFTable1.FileName) then
    begin
      Table1.Open;
      Form1.DbGrid2.hint := 'Total = ' + IntToStr(Form1.Table1.RecordCount) + ' patients';
      if Form1.Table1.RecordCount>=10000 then
        Form1.DbGrid2.hint := 'First ' + IntToStr(Form1.Table1.RecordCount) + ' patients, use filter for others';

      if PatSort<>'' then
        DbGrid2.hint := DbGrid2.hint + ' (sorted on '+PatSort+')';
    end
  end;

  Timer1.Enabled := False;
end;

// These filters are used instead of master-detail links for built-in DBF support

procedure TForm1.DataSource1DataChange(Sender: TObject; Field: TField);
begin
  if form1.DataSource1.Enabled then
    DBGrid2DblClick(self);  // patient list
  ShowOptions := true;
end;

procedure TForm1.DataSource2DataChange(Sender: TObject; Field: TField);
begin
  if form1.DataSource2.Enabled then
    DBText1DblClick(self);
  LabelStudySeq.Caption := IntToStr(Table2.RecNo) + '/' + IntToStr(Table2.RecordCount);
  ShowOptions := true;
end;

procedure TForm1.DataSource3DataChange(Sender: TObject; Field: TField);
begin
  if not form1.DataSource3.Enabled then exit;

  Table4.Close;
  LabelLister.Visible := false;
  SpinEdit1.Visible  := false;
  LabelSeriesSequence.Caption := IntToStr(Table3.RecNo) + '/' + IntToStr(Table3.RecordCount);
  if ShowOptions then
  begin
    if Image1.Picture.Bitmap.Width <> 512 then
      Image1.Picture.Bitmap.SetSize(512, 512);
    Image1.Canvas.Brush.Color := clGray;
    Image1.Canvas.FillRect(Rect(0,0,512,512));
    Image1.Canvas.Font.Height := 20;
    Image1.Canvas.TextOut(120,  20, 'Click this or series to show images');
    Image1.Canvas.TextOut(120,  40, 'Numeric +/- selects and shows images');
    Image1.Canvas.TextOut(120,  60, 'Page Up/Down selects and shows series');
    Image1.Canvas.TextOut(120, 100, 'F3/F4 steps through patients');
    Image1.Canvas.TextOut(120, 120, 'F5/F6 steps through studies');
    Image1.Canvas.TextOut(120, 140, 'F7/F8 steps through series');
    Label25.Visible := false;
  end;
end;

// Display a slice from the database using cqdicom.dll to make it a bitmap

var
    SearchTag : string;
    LastFrame : integer;

procedure TForm1.SpinEdit1Change(Sender: TObject);
begin
  DataSource4DataChange(Self, nil);
end;

var retrying: boolean=false;

procedure TForm1.DataSource4DataChange(Sender: TObject; Field: TField);
var s, t, tmp, text: string;
    f: TextFile;
    g: file of byte;
    i, size, magnum, cdnum, jukeboxnum, cachenum: integer;
    onjukebox: boolean;
begin
  if (PageControl1.ActivePage<>TabSheet5) or (not ShowPict) then
    exit;

  // Translate device name to path; allows up to 10 mag, 10 cache, and
  // 10 jukebox devices and allows unlimited CDs
  onjukebox := false;
  tmp := Table4.FieldByName('DEVICENAME').AsString;

  // retry needed - weird
  if (tmp='') and not retrying then
  begin
    retrying := true; // avoid infinite loop
    Table4.Close;
    Sleep(100);
    Table4.Open;
    tmp := Table4.FieldByName('DEVICENAME').AsString;
    if tmp='' then tmp := 'unknown';
    retrying := false;
  end;

  if copy(tmp, 0, 3)='MAG' then
  begin
    magnum := StrToIntDef(copy(tmp,4,1), 0);   // limited to one character
    s := MagDeviceList[magnum] + Table4.FieldByName('OBJECTFILE').AsString;
  end
  else if copy(tmp, 0, 7)='JUKEBOX' then
  begin
    onjukebox  := true;
    jukeboxnum := StrToIntDef(copy(tmp,8,1), 0);    // limited to one character
    cdnum      := StrToIntDef(copy(tmp,10,99), 0);
    s := Format(JukeboxDeviceList[jukeboxnum], [cdnum]) + Table4.FieldByName('OBJECTFILE').AsString;
    // Fix for faulty %04d format; replace spaces by 0
    for i:=1 to Length(s) do if s[i]=' ' then s[i]:='0';
  end
  else if copy(tmp, 0, 5)='CACHE' then
  begin
    cachenum   := StrToIntDef(copy(tmp,6,1), 0);   // limited to one character
    jukeboxnum := StrToIntDef(copy(tmp,8,1), 0);   // limited to one character
    cdnum      := StrToIntDef(copy(tmp,10,99), 0);
    s := Format(CacheDeviceList[cachenum], [jukeboxnum, cdnum]) + Table4.FieldByName('OBJECTFILE').AsString;
    // Fix for faulty %04d format; replace spaces by 0
    for i:=1 to Length(s) do if s[i]=' ' then s[i]:='0';
  end;

  DBNavigator2.Enabled := (Table2.RecordCount>1);
  DBLookupComboBox2.Enabled := (Table2.RecordCount>1);

  if LastFile<>s then
  begin
    t := Table4.FieldByName('NUMBEROFFR').AsString;
    if (t<>'') and (t<>'1') and (t<>'1 ') then
    begin
      SpinEdit1.Value    := 1;
      SpinEdit1.MinValue := 1;
      SpinEdit1.MaxValue := StrToIntDef(t, 0);
      SpinEdit1.Visible  := true;
      LastFrame          := 0;
      SpinEdit1.ParentShowHint := false;
      SpinEdit1.ShowHint := true;
      SpinEdit1.Hint := 'Use left mouse to slice through the '+tmp+' frames';
    end
    else
      SpinEdit1.Visible  := false;
  end;

  Screen.Cursor := crHourGlass;

  if onjukebox then
  begin
    LabelLister.Visible := false;
    Form1.SpinEdit1.Visible  := false;
    Image1.Picture.Bitmap.SetSize(512, 512);
    Image1.Canvas.Brush.Color := clGray;
    Image1.Canvas.FillRect(Rect(0,0,512,512));
    Image1.Canvas.Font.Height := round(Image1.Picture.Height/24);
    Image1.Canvas.TextOut(round(Image1.Picture.Width /6),
                      round(Image1.Picture.Height/2),
                      'File on jukebox not displayed');
    Image1.Canvas.TextOut(round(Image1.Picture.Width /6),
                      round(2*Image1.Picture.Height/3),
                      tmp);
    LabelLister.Visible := false;
    Screen.Cursor := crDefault;
    Label25.Visible := false;
    ShowOptions := true;
    exit;
  end;

  if FileExists(s) then
  begin
    AssignFile(g, s);
    Reset(g);
    size := FileSize(g);
    CloseFile(g);

    // Do not display large files (takes too long)
    // Disabled because need click anyway to display
    if (LastFile<>s) and (size > 1024*LargeFileSizeKB) then
      if MessageDlg('Show large file ('+IntToStr(size div 1024 div 1024)+' MB)?', mtConfirmation, [mbYes, mbNo], 0) = mrYes then
        size:=0;

    if LastFile=s then size:=0;


    // Do not redisplay same file
    LastFile := s;
    LastFrame := SpinEdit1.value;

    if size > 1024*LargeFileSizeKB then
    begin
      LabelLister.Visible := false;
      Form1.SpinEdit1.Visible  := false;
      Image1.Picture.Bitmap.SetSize(512, 512);
      Image1.Canvas.Brush.Color := clGray;
      Image1.Canvas.FillRect(Rect(0,0,512,512));
      Image1.Canvas.Font.Height := round(Image1.Picture.Height/24);
      Image1.Canvas.TextOut(round(Image1.Picture.Width /6),
                        round(Image1.Picture.Height/2),
                        'Large file not displayed');
      Image1.Canvas.TextOut(round(Image1.Picture.Width /6),
                        round(2*Image1.Picture.Height/3),
                        FormatFloat('0.0', size/(1024*1024))+ ' MB');
      LabelLister.Visible := false;
      Screen.Cursor := crDefault;
      Label25.Visible := false;
      ShowOptions := true;
    end
    else
    begin
      // convert DICOM slice to bitmap file
      tmp := GetTempDir + 'dcmsrv$$.bmp';
      try
	if fileexists('browserdisplay.lua') then
          ServerTask('', 'lua:x=DicomObject:new();x:Read[['+GuiToServerFilename(s)+']];outfile=[['+GuiToServerFilename(tmp)+']];frame='+IntToStr(SpinEdit1.Value-1)+';dofile("browserdisplay.lua")')
	 else
          ServerTask('', 'convert_to_bmp:'+GuiToServerFilename(s)+',1600,'+GuiToServerFilename(tmp)+',//'+IntToStr(SpinEdit1.Value-1));
      except
      end;
      if FileExists(tmp) then
      begin
        Image1.Picture.LoadFromFile(tmp);
        Image1.Update;
        DeleteFile(tmp);
        LabelLister.Visible := false;
        Screen.Cursor := crDefault;
        Label25.Caption := ' image ' + IntToStr(table4.RecNo)+'/'+IntToStr(Table4.RecordCount)+ ' ';
        Label25.Visible := true;
      end
      else

      // If conversion failed display the header
      begin
        LabelLister.Visible := true;
        LabelLister.Color := clGray;
        Image1.Picture.Bitmap.SetSize(512, 512);
        Image1.Canvas.Brush.Color := clGray;
        Image1.Canvas.FillRect(Rect(0,0,512,512));
        LabelLister.Transparent := false;
        tmp := GetTempDir + 'dcmsrv$$.txt';
        ServerTask('', 'lua:x=DicomObject:new();x:Read[['+GuiToServerFilename(s)+']];x:Dump[['+GuiToServerFilename(tmp)+']]');

        text := '';

        AssignFile(f, tmp);
        Reset(f);
        Readln(f, t);
        Readln(f, t);

        i := 0;
        while (not Eof(f)) and (i<100) do
        begin
          Readln(f, t);
          text := text + copy(t, 1, 98) + #13;
          inc(i);
        end;
        CloseFile(f);
        DeleteFile(tmp);

        LabelLister.Caption := text;
        Screen.Cursor := crDefault;
      end;
    end;

    // Update headerlister with new slice
    if assigned(Form3) then
    if Form3.visible then
    begin
      text := '';
      tmp := GetTempDir + 'dcmsrv$$.txt';
      ServerTask('', 'lua:x=DicomObject:new();x:Read[['+GuiToServerFilename(s)+']];x:Dump[['+GuiToServerFilename(tmp)+']]');
      Form3.Caption := 'Header dump of DICOM object: ' + s;

      AssignFile(f, tmp);
      Reset(f);
      Readln(f, t);
      Readln(f, t);

      while not Eof(f) do
      begin
        Readln(f, t);
        text := text + copy(t, 1, 1000) + #13 + #10;
      end;
      CloseFile(f);

      // search previously selected VR and highlight it in new listing

      i := Form3.Memo1.SelStart;
      if i<>0 then
      begin
        while Form3.Memo1.Text[i+1] <> #10 do Dec(i);
        s := copy(Form3.Memo1.Text, i+2, 11);
        SearchTag := s;
      end
      else
        s := SearchTag;

      Form3.Memo1.Text := text;
      i := pos(s, Form3.Memo1.text);
      if i>0 then
      begin
        Form3.Memo1.SelStart := i-1;
        Form3.Memo1.SelLength := Length(s);
        Form3.Memo1.HideSelection := false;
      end
      else
      begin
        Form3.Memo1.SelStart := 0;
        Form3.Memo1.SelLength := 0;
      end;

      DeleteFile(tmp);
    end;
  end
  else
  begin
    tmp := 'Image file not found';

    LabelLister.Visible := false;
    Form1.SpinEdit1.Visible  := false;
    Image1.Picture.Bitmap.SetSize(512, 512);
    Image1.Canvas.Brush.Color := clGray;
    Image1.Canvas.FillRect(Rect(0,0,512,512));
    Image1.Canvas.Font.Height := round(Image1.Picture.Height/24);
    Image1.Canvas.TextOut(round(Image1.Picture.Width /4),
                          round(Image1.Picture.Height/2), tmp);
    Screen.Cursor := crDefault;
    Label25.Visible := false;
    ShowOptions := true;
  end;
end;

procedure TForm1.DBGrid1CellClick(Column: TColumn);
begin
  DBGrid1DblClick(DBGrid1);
end;

// for mouse scrolling
var oldx: integer=0;
var oldy: integer=0;

procedure TForm1.DBGrid1DblClick(Sender: TObject);
var p, q:string;
    dbdir: string;
begin
  dbdir := GetTempDir + 'conquest_browser' + DirSep;
  Form1.LabelLister.Visible := false;
  Label25.Caption := ' ** working ** ';
  Label25.Visible := true;
  Label25.Update;
  Table4.Close;
  p := Table1.FieldByName('PATIENTID').AsString;
  q := Table3.FieldByName('SERIESINST').AsString;
  ServerTask('',
  'todbf:'+GuiToServerFilename(dbdir)+'|DICOMImages|ImagePat = ''' + processfilter(p) + ''' and SeriesInst = ''' + processfilter(q) + '''|'+ImageSort);
  MDBFTable4.FileName := dbdir+'DICOMImages.DBF';
  WaitForFile(MDBFTable4.FileName, 1);
  if FileExists(MDBFTable4.FileName) then
  begin
    Table4.Open;
    DbGrid3.hint := 'List of images in series: total = ' + IntToStr(table4.RecordCount) + ' images';
    if ImageSort<>'' then
      DbGrid3.hint := DbGrid3.hint + ' (sorted on '+ImageSort+')';
    Label25.Caption := ' image ' + IntToStr(table4.RecNo)+'/'+IntToStr(Table4.RecordCount)+ ' ';
    Label25.Visible := true;
    oldy := 0;
  end;
end;

procedure TForm1.DBGrid1TitleClick(Column: TColumn);
begin
  if not Assigned(Column.Field) then exit;

  if (Pos(Column.Field.FieldName, SeriesSort) = 1)
    and (Pos(' DESC', SeriesSort)= 0) then
    SeriesSort := '' // Column.Field.FieldName + ' DESC'
  else
    SeriesSort := Column.Field.FieldName;// + ' ASC';

  if SeriesSort = 'SERIESNUMB' then
  begin
    if FileExists(curdir + '\USEPOSTGRES') then
      SeriesSort := SeriesSort + '::int'
    else
      SeriesSort := SeriesSort + ' * 1';
  end;

  DBText1DblClick(self);
end;

procedure TForm1.DBGrid2DblClick(Sender: TObject);
var p:string;
    dbdir: string;
begin
  dbdir := GetTempDir + 'conquest_browser' + DirSep;
  Table2.Close;
  Table3.Close;
  Table4.Close;
  p := Table1.FieldByName('PATIENTID').AsString;
  ServerTask('',
  'todbf:'+GuiToServerFilename(dbdir)+'|DICOMStudies|PatientID = ''' + processfilter(p) + '''|'+StudySort);
  MDBFTable2.FileName := dbdir+'DICOMStudies.DBF';
  WaitForFile(MDBFTable2.FileName, 1);
  if FileExists(MDBFTable2.FileName) then
  begin
    Table2.Open;
    DBNavigator2.Enabled := (Table2.RecordCount>1);
    DBLookupComboBox2.Enabled := (Table2.RecordCount>1);
    if Table2.RecordCount>0 then
      DBText1DblClick(self)
    else
    begin
      LabelLister.Visible := false;
      Form1.SpinEdit1.Visible  := false;
      Image1.Picture.Bitmap.SetSize(512, 512);
      Image1.Canvas.Brush.Color := clGray;
      Image1.Canvas.FillRect(Rect(0,0,512,512));
    end;

    DbText1.hint := 'Navigate through studies for patient: total = ' + IntToStr(table2.RecordCount) + ' studies';
    if StudySort<>'' then
      DbText1.hint := DbText1.hint + ' (sorted on '+StudySort+')';
  end;
end;

procedure TForm1.DBGrid2TitleClick(Column: TColumn);
begin
 if not Assigned(Column.Field) then exit;

 if (Pos(Column.Field.FieldName, PatSort) = 1)
    and (Pos(' DESC', PatSort)= 0) then
   PatSort := ''
 else
   PatSort := Column.Field.FieldName;
  Timer1Timer(self);
end;

procedure TForm1.DBGrid3TitleClick(Column: TColumn);
begin
  if not Assigned(Column.Field) then exit;

  if (Pos(Column.Field.FieldName, ImageSort) = 1)
    and (Pos(' DESC', ImageSort)= 0) then
    ImageSort := ''
  else
    ImageSort := Column.Field.FieldName;

  if ImageSort = 'IMAGENUMBE' then
  begin
    if FileExists(curdir + '\USEPOSTGRES') then
      ImageSort := ImageSort + '::int'
    else
      ImageSort := ImageSort + ' * 1';
  end;

  DBGrid1DblClick(self);
end;


procedure TForm1.DBLookupComboBox2Click(Sender: TObject);
begin
  DBText1DblClick(self);
end;

procedure TForm1.DBText1DblClick(Sender: TObject);
var p, q:string;
    dbdir: string;
begin
  dbdir := GetTempDir + 'conquest_browser' + DirSep;
  Table3.Close;
  Table4.Close;
  p := Table1.FieldByName('PATIENTID').AsString;
  q := Table2.FieldByName('STUDYINSTA').AsString;
  ServerTask('',
  'todbf:'+GuiToServerFilename(dbdir)+'|DICOMSeries|SeriesPat = ''' + processfilter(p) + '''  and StudyInsta = ''' + processfilter(q) + '''|'+SeriesSort);
  MDBFTable3.FileName := dbdir+'DICOMSeries.DBF';
  WaitForFile(MDBFTable3.FileName, 1);
  if FileExists(MDBFTable3.FileName) then
  begin
    Table3.Open;
    if false and (Table3.RecordCount=1) then
      DBGrid1DblClick(self)
    else
    begin
      LabelLister.Visible := false;
      Form1.SpinEdit1.Visible  := false;
    end;

    DbGrid1.hint := 'List of series in study: total = ' + IntToStr(table3.RecordCount) + ' series';
    if SeriesSort<>'' then
      DbGrid1.hint := DbGrid1.hint + ' (sorted on '+SeriesSort+')';
  end;
end;

// Save slice from the database as a bitmap using cqdicom.dll

procedure TForm1.SaveasBitmap1Click(Sender: TObject);
var s, t: string;
begin
  s := LastFile;

  SaveDialog1.Title := 'Save DICOM file as bitmap';
  SaveDialog1.DefaultExt := 'BMP';
  SaveDialog1.Filter := 'Bitmap files (*.bmp)|*.BMP|Jpeg files (*.jpg)|*.JPG|Gif files (*.gif)|*.GIF';
  SaveDialog1.FilterIndex := 0;
  SaveDialog1.Filename := '';

  if FileExists(s) and (PageControl1.ActivePage=TabSheet5) then
  begin
    if SaveDialog1.Execute then
    begin
      t := SaveDialog1.Filename;
      if StrPos(Pchar(t), '.')=nil then t := t + '.bmp';
      Screen.Cursor := crHourGlass;
      if (ExtractFileExt(t)='.bmp') or (ExtractFileExt(t)='.BMP') then
      begin
        if fileexists('browserdisplay.lua') then
          ServerTask('', 'lua:x=DicomObject:new();x:Read[['+GuiToServerFilename(s)+']];outfile=[['+GuiToServerFilename(t)+']];frame='+IntToStr(SpinEdit1.Value-1)+';dofile(Global.Basedir.."browserdisplay.lua")')
        else
          ServerTask('', 'convert_to_bmp:'+(s)+',9600,'+GuiToServerFilename(t)+',//'+IntToStr(SpinEdit1.Value-1));
      end
      else
      begin
        if ExtractFileExt(t)='.jpg' then ServerTask('', 'convert_to_jpg:'+GuiToServerFilename(s)+',9600,'+GuiToServerFilename(t)+',//'+IntToStr(SpinEdit1.Value-1));
        if ExtractFileExt(t)='.JPG' then ServerTask('', 'convert_to_jpg:'+GuiToServerFilename(s)+',9600,'+GuiToServerFilename(t)+',//'+IntToStr(SpinEdit1.Value-1));
        if ExtractFileExt(t)='.gif' then ServerTask('', 'convert_to_gif:'+GuiToServerFilename(s)+',9600,'+GuiToServerFilename(t)+',//10005'); // animated 5fps
        if ExtractFileExt(t)='.GIF' then ServerTask('', 'convert_to_gif:'+GuiToServerFilename(s)+',9600,'+GuiToServerFilename(t)+',//10005');
      end;
      Screen.Cursor := crDefault;
    end;
  end
end;

// View header from a slice in the database using cqdicom.dll

procedure TForm1.Dumpheader1Click(Sender: TObject);
var s, tmp: string;
    f: TextFile;
begin
  s := LastFile;

  if Form3.Visible then Form3.SetFocus;

  if FileExists(s) and (PageControl1.ActivePage=TabSheet5) and not Form3.Visible then
  begin
    tmp := GetTempDir + 'dcmsrv$$.txt';
    ServerTask('', 'lua:x=DicomObject:new();x:Read[['+GuiToServerFilename(s)+']];x:Dump[['+GuiToServerFilename(tmp)+']]');

    Form3.Memo1.Lines.Clear;
    Form3.Caption := 'Header dump of DICOM object: ' + s;

    AssignFile(f, tmp);
    Reset(f);
    Readln(f, s);
    Readln(f, s);

    while not Eof(f) do
    begin
      Readln(f, s);
      Form3.Memo1.Lines.Add(copy(s, 1, 1000));
    end;
    CloseFile(f);

    Form3.Memo1.SelStart := 0;
    Form3.Show;
    DeleteFile(tmp);
  end;
end;

// Run external viewer

procedure TForm1.Runexternalviewer1Click(Sender: TObject);
begin
  RunProgramWaiting(ExternalViewer, curdir, [LastFile]);
end;

procedure TForm1.RunKPacsviewer1Click(Sender: TObject);
var webaddress: string;
begin
{$IFDEF KPACS}
  FormKViewer.Show;
  FormKViewer.TempFile := GetTempDir + 'viewertemp.$$$';
  kpacsuncompress := UseKpacsDecompression=0;   // uncompress means use dgate to uncompress not dgate
  FormKViewer.OpenSeries(Table2.FieldByName('STUDYDATE').AsString,
                        Table3.FieldByName('SERIESTIME').AsString,
                        Table3.FieldByName('MODALITY').AsString,
                        Table1.FieldByName('PATIENTNAM').AsString,
                        Table1.FieldByName('PATIENTID').AsString,
                        Table2.FieldByName('STUDYINSTA').AsString,
                        Table3.FieldByName('SERIESINST').AsString);
{$ELSE}
  webaddress := 'http://127.0.0.1:'+LadlePort+'/app/newweb/dgate.exe?mode=wadoseriesviewer&series=' +
                 Table1.FieldByName('PATIENTID').AsString+':'+Table3.FieldByName('SERIESINST').AsString;
  ServerTask('', 'luastart:arg={};arg[1]=[['+LadlePort+']];arg[2]=Global.BaseDir..[[webserver/htdocs/]];dofile(Global.Basedir..[[lua/ladle.lua]])');
  ShellExecute(0, 'open', PWideChar(webaddress), nil, nil, SW_SHOWNORMAL);
{$ENDIF KPACS}
end;

// Delete all files of this patient

procedure TForm1._Deletethispatient1Click(Sender: TObject);
var PID, Name: string;
begin
  if not Table1.Active then exit;

  PID  := Table1.FieldByName('PATIENTID').AsString;
  Name := Table1.Fields[1].AsString;

  if Length(PID)=0 then exit;

  if MessageDlg('Delete all data for patient: ' + PID + ' - ' + Name + ' ?', mtConfirmation,
    [mbYes, mbNo], 0) = mrNo then Exit;

  ServerTask('Deleting patient from GUI: ' + PID, 'luastart:servercommand[[deletepatient:'+PID+']]');
  WriteLog('deleting patient : ' + PID);
  SetRefreshLevel(1);

  LabelLister.Visible := false;
  Form1.SpinEdit1.Visible  := false;
  Image1.Picture.Bitmap.SetSize(512, 512);
  Image1.Canvas.Brush.Color := clGray;
  Image1.Canvas.FillRect(Rect(0,0,512,512));
  Image1.Canvas.Font.Height := round(Image1.Picture.Height/15);
  Image1.Canvas.TextOut(round(Image1.Picture.Width /4),
                        round(Image1.Picture.Height/2),
                        'Patient is being deleted');
  Label25.Visible := false;
  ShowOptions := true;
end;

// Remove image from database (regen would enter it again)

procedure TForm1.Removeimagefromdatabase1Click(Sender: TObject);
var tmp: string;
begin
  if not Table4.Active then exit;

  if MessageDlg('Delete this image from database (not from disk) ?' +
                #13 + #13 + Table4.FieldByName('OBJECTFILE').AsString,
                mtConfirmation, [mbYes, mbNo], 0
               ) = mrNo then Exit;

  tmp := Table1.FieldByName('PATIENTID' ).AsString+','+
         Table2.FieldByName('STUDYINSTA').AsString+','+
         Table3.FieldByName('SERIESINST').AsString+','+
         Table4.FieldByName('SOPINSTANC').AsString;
  ServerTask('Deleting db for SOP from GUI', 'deletesopfromdb:'+GuiToServerFilename(tmp));
  SetRefreshLevel(4);
  RefreshDatabase;
end;

// remove all patient information from a series

procedure TForm1.Anonymize1Click(Sender: TObject);
begin
  AnonymizeImage.Enabled := Table4.Active;
end;

procedure TForm1.AnonymizePatientClick(Sender: TObject);
var script, NewID, typ: string;
begin
  if Sender=AnonymizePatient then
  begin
    script := Table1.Fields[0].AsString + ',,,,';
    typ := 'patient';
  end;
  if Sender=AnonymizeStudy   then
  begin
    script := Table1.Fields[0].AsString + ',' + Table2.Fields[0].AsString + ',,,';
    typ := 'study';
  end;
  if Sender=AnonymizeSeries  then
  begin
    script := Table1.Fields[0].AsString + ',' + Table2.Fields[0].AsString + ',' + Table3.Fields[0].AsString + ',,';
    typ := 'series';
  end;
  if (Sender=AnonymizeImage) and Table4.Active then
  begin
    script := Table1.Fields[0].AsString + ',' + Table2.Fields[0].AsString + ',' + Table3.Fields[0].AsString + ',' + Table4.Fields[0].AsString + ',';
    typ := 'image';
  end
  else if (Sender=AnonymizeImage) then
  begin
    ShowMessage('Select image to anonymize first');
    exit;
  end;

  if InputQuery('Anonymize this ' + typ + '?', 'Enter New ID', NewID) then
  begin
    script := script + '1,';
    script := script + 'lua/anonymize_script.lua('+NewID+')';
    ServerTask('anonymizing data from GUI', 'luastart:servercommand[[modifier:'+script+']]');
    SetRefreshLevel(1);
  end;
end;

procedure TForm1.ChangePatidPatientClick(Sender: TObject);
var script, NewID, typ: string;
begin
  if Sender=ChangePatidPatient then
  begin
    script := Table1.Fields[0].AsString + ',,,,';
    typ := 'patient';
  end;
  if Sender=ChangePatidStudy   then
  begin
    script := Table1.Fields[0].AsString + ',' + Table2.Fields[0].AsString + ',,,';
    typ := 'study';
  end;
  if Sender=ChangePatidSeries  then
  begin
    script := Table1.Fields[0].AsString + ',' + Table2.Fields[0].AsString + ',' + Table3.Fields[0].AsString + ',,';
    typ := 'series';
  end;
  if (Sender=ChangePatidImage) and Table4.Active then
  begin
    script := Table1.Fields[0].AsString + ',' + Table2.Fields[0].AsString + ',' + Table3.Fields[0].AsString + ',' + Table4.Fields[0].AsString + ',';
    typ := 'image';
  end
  else if (Sender=ChangePatidImage) then
  begin
    ShowMessage('Select image to change first');
    exit;
  end;

  if InputQuery('Change PatientID for this ' + typ + '?', 'Enter New ID', NewID) then
  begin
    script := script + '1,';
    script := script + 'lua "script(''newuids'');Data.PatientID='''+NewID+'''"';

    ServerTask('Change PatientID from GUI', 'luastart:servercommand[[modifier:'+script+']]');
    SetRefreshLevel(1);
  end;
end;

procedure TForm1.ChangePatientID1Click(Sender: TObject);
begin
  ChangePatidImage.Enabled := Table4.Active;
end;

procedure TForm1.MergeSelectedSeries1Click(Sender: TObject);
var Bookmark: TBookmark;
    FileNames, tmp, s: string;
    magnum, i, j: integer;
    List: TStringList;
    NotMag : boolean;
begin
  if not Table4.Active then exit;

  SendSelectedForm.Caption := 'Merge selected series';
  Bookmark := Table3.Bookmark;
  ShowPict := False;
  NotMag   := False;

  try
    Table3.First;
    SendSelectedForm.ListBox.Items.Clear;
    while not Table3.EOF do
    begin
      SendSelectedForm.ListBox.Items.Add(Format('%12s'+#09+'%12s'+#09+'%12s'+#09+'%12s'+#09+'%8s',
        [Table3.Fields[1].AsString,
         Table3.Fields[2].AsString,
         Table3.Fields[3].AsString,
         Table3.Fields[4].AsString,
         Table3.Fields[5].AsString
        ]));
      Table3.Next;
    end;
  finally
    ShowPict := True;
  end;

  SendSelectedForm.ComboBox1.Hide;
  SendSelectedForm.Label1.Hide;

  if SendSelectedForm.ShowModal = mrCancel then exit;

  Screen.Cursor := crHourGlass;

  try
    j        := 0;
    i        := 0;
    ShowPict := false;
    Table3.First;
    while not Table3.EOF do
    begin
      if SendSelectedForm.ListBox.Selected[i] then
      begin
        FileNames := FileNames + Table3.FieldByName('SERIESINST').AsString  + ',';
        inc(j);
      end;
      Table3.Next;
      inc(i);
    end;
  finally
    ShowPict := true;
  end;

  if j<2 then
  begin
    ShowMessage('Need at least 2 series to merge');
    Table3.Bookmark := Bookmark;
    Screen.Cursor := crDefault;
    exit;
  end;

  if j>16 then
  begin
    ShowMessage('Maximum merge is 16 series');
    Table3.Bookmark := Bookmark;
    Screen.Cursor := crDefault;
    exit;
  end;

  if Length(FileNames)>0 then FileNames := copy(FileNames, 1, Length(FileNames)-1);
  ServerTask('Merging series UID: ' + FileNames, 'luastart:servercommand[[mergeseries:' + FileNames+']]');
  SetRefreshLevel(3);

  List := TStringList.Create;

  try
    j        := 0;
    ShowPict := false;
    form1.DBText1DblClick(form1.DBText1);
    Table3.First;
    while not Table3.EOF do
    begin
      if SendSelectedForm.ListBox.Selected[j] then
      begin
        form1.DBGrid1DblClick(form1.DbGrid1);
      	Table4.First;
        while not Table4.EOF do
        begin
          tmp := Table4.FieldByName('DEVICENAME').AsString;
          if copy(tmp, 0, 3)='MAG' then
          begin
            magnum := StrToIntDef(copy(tmp,4,1), 0);   // limited to one character
            s := MagDeviceList[magnum] + Table4.FieldByName('OBJECTFILE').AsString;
            List.Add(s);
          end
          else
            NotMag := True;

          Table4.Next;
        end;
      end;

      Table3.Next;
      inc(j);
    end;

    Form1.ProgressBar2.Max := List.Count;

    if NotMag then
    begin
      ShowMessage('Not all images are on MAG storage - cannot merge series');
    end
    else for i:=0 to List.Count-1 do
    begin
      LabelLister.Visible := false;
      Form1.SpinEdit1.Visible  := false;
      Image1.Picture.Bitmap.SetSize(512, 512);
      Image1.Canvas.Brush.Color := clGray;
      Image1.Canvas.FillRect(Rect(0,0,512,512));
      Image1.Canvas.Font.Height := round(Image1.Picture.Height/15);
      Image1.Canvas.TextOut(round(Image1.Picture.Width /4),
                          round(Image1.Picture.Height/2),
                          'Modifying image ' + IntToStr(i+1) + ' of ' + IntToStr(List.Count));
      Image1.Update;
      Label25.Visible := false;
      ShowOptions := true;

      ServerTask('', 'mergeseriesfile:' + List[i]);
      Form1.ProgressBar2.Position := i;
      Application.ProcessMessages;
    end;

  finally
    Screen.Cursor := crDefault;
    ShowPict      := true;
    if Table3.Active then Table3.Bookmark := Bookmark;
    List.Free;
    Form1.ProgressBar2.Visible := false;
  end;
end;

procedure TForm1.SplitSeries1Click(Sender: TObject);
var Bookmark: TBookmark;
    tmp, s: string;
    magnum, i, j: integer;
    List: TStringList;
    NotMag : boolean;
begin
  if not Table4.Active then exit;
  SendSelectedForm.Caption := 'Split selected images from series';
  Bookmark := Table4.Bookmark;
  ShowPict := False;
  NotMag   := False;

  try
    Table4.First;
    SendSelectedForm.ListBox.Items.Clear;
    while not Table4.EOF do
    begin
      SendSelectedForm.ListBox.Items.Add(Format('%12s'+#09+'%12s'+#09+'%12s'+#09+'%8s',
        [Table4.Fields[2].AsString,
         Table4.Fields[3].AsString,
         Table4.Fields[4].AsString,
         Table4.Fields[5].AsString
        ]));
      Table4.Next;
    end;
  finally
    ShowPict := True;
  end;

  SendSelectedForm.ComboBox1.Hide;
  SendSelectedForm.Label1.Hide;

  if SendSelectedForm.ShowModal = mrCancel then exit;

  Screen.Cursor := crHourGlass;

  try
    i        := 0;
    ShowPict := false;
    Table4.First;
    while not Table4.EOF do
    begin
      if SendSelectedForm.ListBox.Selected[i] then
      begin
        ServerTask('Splitting series UID: ' + Table4.FieldByName('SERIESINST').AsString, 'mergeseries:' + Table4.FieldByName('SERIESINST').AsString);
        Break;
      end;
      Table4.Next;
      inc(i);
    end;
  finally
    ShowPict := true;
  end;

  List := TStringList.Create;

  try
    j        := 0;
    ShowPict := false;
    Table4.First;
    while not Table4.EOF do
    begin
      if SendSelectedForm.ListBox.Selected[j] then
      begin
        tmp := Table4.FieldByName('DEVICENAME').AsString;
        if copy(tmp, 0, 3)='MAG' then
        begin
          magnum := StrToIntDef(copy(tmp,4,1), 0);   // limited to one character
          s := MagDeviceList[magnum] + Table4.FieldByName('OBJECTFILE').AsString;
          List.Add(s);
        end
        else
          NotMag := True;
      end;
      Table4.Next;
      inc(j);
    end;

    if NotMag then
    begin
      ShowMessage('Not all images are on MAG storage - cannot split series');
    end
    else for i:=0 to List.Count-1 do
    begin
      LabelLister.Visible := false;
      Form1.SpinEdit1.Visible  := false;
      Image1.Picture.Bitmap.SetSize(512, 512);
      Image1.Canvas.Brush.Color := clGray;
      Image1.Canvas.FillRect(Rect(0,0,512,512));
      Image1.Canvas.Font.Height := round(Image1.Picture.Height/15);
      Image1.Canvas.TextOut(round(Image1.Picture.Width /4),
                          round(Image1.Picture.Height/2),
                          'Modifying image ' + IntToStr(i+1) + ' of ' + IntToStr(List.Count));
      Image1.Update;
      Label25.Visible := false;
      ShowOptions := true;

      ServerTask('', 'mergeseriesfile:' + List[i]);
      Application.ProcessMessages;
    end;

  finally
    Screen.Cursor := crDefault;
    ShowPict      := true;
    if Table4.Active then Table4.Bookmark := Bookmark;
    List.Free;
  end;
end;

procedure TForm1._DeleteThisSeriesClick(Sender: TObject);
var tmp: string;
    typ: string;
begin
  typ := 'series';
  if sender = DeleteThisStudy then typ := 'study';

  if (sender = DeleteThisStudy ) and (not Table2.Active) then exit;
  if (sender = DeleteThisSeries) and (not Table3.Active) then exit;

  if MessageDlg('Delete this ' + typ + ' ?', mtConfirmation,
    [mbYes, mbNo], 0) = mrNo then Exit;

  if sender = DeleteThisStudy then
    tmp := Table1.FieldByName('PATIENTID' ).AsString+':'+
           Table2.FieldByName('STUDYINSTA').AsString
  else
    tmp := Table1.FieldByName('PATIENTID' ).AsString+':'+
           Table3.FieldByName('SERIESINST').AsString;
  ServerTask('Deleting '+typ+' from GUI', 'luastart:servercommand[[delete'+typ+':'+tmp+']]');
  if sender = DeleteThisStudy then SetRefreshLevel(3)
  else                             SetRefreshLevel(2);
end;

procedure TForm1.OnceUp1Click(Sender: TObject);
begin
  if not Table4.Active then exit;
  while not Table4.EOF do Table4.Next;
end;

procedure TForm1.OnceDown1Click(Sender: TObject);
begin
  if not Table4.Active then exit;
  while not Table4.BOF do Table4.Prior;
end;

procedure TForm1.EditDatabasebyhand1Click(Sender: TObject);
begin
  Form4.show;
end;

procedure TForm1.CheckBoxViewIncomingClick(Sender: TObject);
begin
  if CheckBoxViewIncoming.Checked then
  begin
    Table4.Active := false;
    DBGrid3.Enabled := false;
    LabelLister.Visible := false;
    Form1.SpinEdit1.Visible  := false;
    Image1.Picture.Bitmap.SetSize(512, 512);
    Image1.Canvas.Brush.Color := clGray;
    Image1.Canvas.FillRect(Rect(0,0,512,512));
    Image1.Canvas.Font.Height := round(Image1.Picture.Height/12);
    Image1.Canvas.TextOut(round(Image1.Picture.Width /6),
                       round(Image1.Picture.Height/2),
                       'waiting for incoming images');
    LabelLister.Visible := false;
    Label25.Visible := false;
    ShowOptions := true;
  end
  else
  begin
    LastFile := '';
    Table4.Active := true;
    Table4.First;
    DBGrid3.Enabled := true;
  end;
end;

procedure TForm1.CheckBoxWebServerClick(Sender: TObject);
begin
  if (Sender as TCheckBox).Checked then
    ServerTask('', 'luastart:arg={};arg[1]=[['+LadlePort+']];arg[2]=Global.BaseDir..[[webserver/htdocs/]];dofile(Global.Basedir..[[lua/ladle.lua]])')
  else
    ServerTask('', 'luastart:dofile(Global.Basedir..[[lua/quitladle.lua]])');
end;

procedure TForm1.AppCLick(sender: TObject);
begin
  ShellExecute(0, 'open', PWideChar('http://127.0.0.1:'+LadlePort+'/app/'+(sender as TMenuItem).Caption+'/'), nil, nil, SW_SHOWNORMAL);
end;

procedure TForm1.ShowPopupMenu2(Sender: TObject; X, Y:integer);
var sr: TSearchRec;
    p:TPoint;
begin
  PopupMenu2.AutoHotkeys := maManual;
  PopupMenu2.Items.Clear;

  PopupMenu2.Items.Add(TMenuItem.Create(Self));
  PopupMenu2.Items[PopupMenu2.Items.Count-1].Caption := 'Open web app:';
  PopupMenu2.Items.Add(TMenuItem.Create(Self));
  PopupMenu2.Items[PopupMenu2.Items.Count-1].Caption := '-';

  if FindFirst(curdir + '\webserver\htdocs\app\*.*',faAnyFile, sr) = 0 then
  repeat
    if ((sr.Attr and faDirectory)<>0) and (sr.Name<>'.') and (sr.Name<>'..') then
    begin
      PopupMenu2.Items.Add(TMenuItem.Create(Self));
      PopupMenu2.Items[PopupMenu2.Items.Count-1].Caption := sr.Name;
      PopupMenu2.Items[PopupMenu2.Items.Count-1].OnClick := AppClick;
    end;
  until findNext(sr) <> 0;


  p := (Sender as TCheckBox).ClientToScreen(TPoint.Create(X, Y));
  PopupMenu2.Popup(p.X, p.Y);
end;

procedure TForm1.CheckBoxWebServerMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  if not (Sender as TCheckBox).Checked then exit;
  if Button <> mbRight then exit;
  //ShellExecute(0, 'open', PWideChar('http://127.0.0.1:'+LadlePort+'/app/newweb/dgate.exe?mode=start'), nil, nil, SW_SHOWNORMAL);
  ShowPopupMenu2(Sender, X, Y);
end;

{************************************************************************}
// Server status page

var Count: integer;

// Listen for DICOM requests (multi-process mode only)

procedure TForm1.ListenSocketSessionAvailable(Sender: TObject;
  Error: Word);
  var requestID: Integer;
  var pi    : ^integer;
  var ph:   PHandle;
  var Name: string;
  hMap, NewHandle: integer;
begin
  requestID := ListenSocket.Accept;

  Inc(Count);
  name := 'DGATE#' + IntToStr(Count);

  ph := @NewHandle;

  DuplicateHandle(GetCurrentProcess(), requestID, GetCurrentProcess(),
                   ph, DUPLICATE_SAME_ACCESS, TRUE, DUPLICATE_SAME_ACCESS);

  hMap := CreateFileMapping($ffffffff, nil, PAGE_READWRITE, 0, 4, Pchar(name));
  pi := MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0,0, 4);
  pi^ := NewHandle; // requestid;
  UnmapViewOfFile(pi);

  ActiveProcess := RunDgate(name, true);
end;

// Show DICOM server output

var LastCallingAE, LastCalledAE: string;

procedure TForm1.ServerStatusSocketDataAvailable(Sender: TObject;
  Error: Word);
  var buffer: array[0..8192] of Ansichar;
  s, t, tmp: string;
  i, j: integer;
  mem: Pchar;
  Src: TSockAddrIn;
begin
  j := sizeof(TSockAddrIn);
  i := ServerStatusSocket.ReceiveFrom(@buffer, 8191, Src, j);
  if i<1 then exit;

  buffer[i] := #00;
  if buffer[i-1] in [#10, #13] then buffer[i-1] := #00;
  s := buffer;
  // s := copy(s, 0, length(s)-1);
  for i:=1 to length(s) do if s[i]<' ' then s[i]:=' ';

  if not assigned(ServerStatusStringList) then
    ServerStatusStringList := TStringList.Create;
  WriteMemoSl(ServerStatusMemo, s, 1100, 1000, 'serverstatus', true, ServerStatusStringList);

  // duplicate serious server problems in conquestdicomserver.log
  if Pos('***', s) > 0 then WriteLog(s);

  i := Pos('$ RESTART', s);
  if i>0 then
  begin
    WriteLog('$ RESTART');
    KillAndRestartTheServerClick(nil);
  end;

  i := Pos('$ DGATE', s);
  if i>0 then
  begin
    WriteLog('$ DGATE  : ' + copy(s, i+7, 250));
    RunDgate('-u'+ServerStatusSocket.Port + copy(s, i+7, 250), false);
  end;

  i := Pos('$ RELOADACRNEMA $', s);
  if i>0 then
  begin
    RestoreListButtonClick(RestoreListButton);
  end;

  i := Pos('$ RELOADSETTINGS $', s);
  if i>0 then
  begin
    RestoreconfigButtonClick(RestoreconfigButton);
  end;

  i := Pos('$ QUITGUI $', s);
  if i>0 then
  begin
    WriteLog('$ QUITGUI $');

    // kill the threaded process if it is running
    if ThreadedProcess<>0 then
    begin
      TerminateProcess(ThreadedProcess, 0);
      CloseHandle(ThreadedProcess);
    end;

    FlushButtonClick(self);
    WriteLog('closing the server');

    Application.Terminate;
  end;

  if CheckBoxViewIncoming.Checked then
  begin
    tmp := GetTempDir + 'dcmsrv$$.bmp';

    i := Pos('Calling Application Title : "', s);
    if i>0 then
    begin
      i := pos('"', s);
      s := copy(s, i+1, 250);
      i := pos('"', s);
      s := copy(s, 1, i-1);
      LastCallingAE := Trim(s);
    end;

    i := Pos('ritten file: ', s); // Written / Rewritten
    if i>0 then
    begin
      t := copy(s, i+13, 250);

      try
        if fileexists('browserdisplay.lua') then
          ServerTask('', 'lua:x=DicomObject:new();x:Read[['+t+']];outfile=[['+tmp+']];frame=0;dofile("browserdisplay.lua")')
	else
          ServerTask('', 'convert_to_bmp:'+t+',1600,'+tmp);
        i := 0;
      except
        i := 999;
      end;

      if i=0 then
      begin
        Image1.Picture.LoadFromFile(tmp);
        Image1.Update;
        DeleteFile(tmp);
        Image1.Canvas.Font.Height := round(Image1.Picture.Height/12);
        Image1.Canvas.Font.Color  := clRed;
        Image1.Canvas.Brush.Style := bsClear;
        Image1.Canvas.TextOut(round(Image1.Picture.Width /6),
                       round(Image1.Picture.Height/20),
                       LastCallingAE);
        Image1.Canvas.Font.Color  := clBlack;
        Image1.Canvas.Brush.Style := bsSolid;
      end;
    end;

    i := Pos('Print file: ', s);
    if i>0 then
    begin
      t := copy(s, i+12, 250);
      tmp := Lastfile + '.BMP';

      try
        if fileexists('printerdisplay.lua') then
          ServerTask('', 'lua:x=DicomObject:new();x:Read[['+GuiToServerFilename(t)+']];outfile=[['+GuiToServerFilename(tmp)+']];dofile("printerdisplay.lua")')
        else
          ServerTask('', 'convert_to_bmp:'+GuiToServerFilename(t)+',9600,'+GuiToServerFilename(tmp));
        i := 0;
      except
        i := 999;
      end;

      if i=0 then
      begin
        Image1.Picture.LoadFromFile(tmp);
        Image1.Update;
        Application.ProcessMessages;
        DeleteFile(tmp);
        DeleteFile(t);
        Image1.Canvas.Font.Height := round(Image1.Picture.Height/16);
        Image1.Canvas.Font.Color  := clRed;
        Image1.Canvas.Brush.Style := bsClear;
        Image1.Canvas.TextOut(round(Image1.Picture.Width /6),
                       round(Image1.Picture.Height/40),
                       'Printed by ' + LastCallingAE);
        Image1.Canvas.Font.Color  := clBlack;
        Image1.Canvas.Brush.Style := bsSolid;
      end;
    end;
  end;

  // print now through progress socket
  if false then // not CheckBoxViewIncoming.Checked then
  begin
    i := Pos('Printing Basic Film Box', s);
    if i>0 then
    begin
      GetMem(mem, 256);
      StrPCopy(mem, 'Printing Basic Film Box');
      PrinterQueue.Add(mem);
    end;

    i := Pos('Printing Basic Film Session', s);
    if i>0 then
    begin
      GetMem(mem, 256);
      StrPCopy(mem, 'Printing Basic Film Session');
      PrinterQueue.Add(mem);
    end;

    i := Pos('Creating Basic Film Box', s);
    if i>0 then
    begin
      GetMem(mem, 256);
      j := Pos('Film#', s);
      if j>0 then j := StrToInt(copy(s, j + 6, 5));

      i := Pos('PORTRAIT', s);
      if i>0 then
        StrPCopy(mem, 'Creating Basic Portrait Film Box - Film# ' + IntToStr(j))
      else
        StrPCopy(mem, 'Creating Basic Landscape Film Box - Film# ' + IntToStr(j));
      PrinterQueue.Add(mem);
    end;

    i := Pos('Print file: ', s);
    if i>0 then
    begin
      t := copy(s, i+12, 250);
      GetMem(mem, 256);
      StrPCopy(mem, t);
      PrinterQueue.Add(mem);
    end;
  end;

  i := Pos('Browse patient: ', s);
  if i>0 then
  begin
    ShowPict := false;
    if PageControl1.ActivePage <> TabSheet5 then
    begin
      PageControl1.ActivePage := TabSheet5;
      PageControl1Change(self);
    end;
    ComboBox1.text := copy(s, i+16, 250);
    ShowPict := true;
    Timer1Timer(self);
  end;

  if (Length(DemoViewer) + Length(DemoCopy))>0 then
  begin
    i := Pos('Calling Application Title : "', s);
    if i>0 then
    begin
      i := pos('"', s);
      s := copy(s, i+1, 250);
      i := pos('"', s);
      s := copy(s, 1, i-1);
      LastCallingAE := Trim(s);
    end;

    i := Pos('Called Application Title : "', s);
    if i>0 then
    begin
      i := pos('"', s);
      s := copy(s, i+1, 250);
      i := pos('"', s);
      s := copy(s, 1, i-1);
      LastCalledAE := Trim(s);
    end;

    i := Pos('Written file: ', s);
    if i>0 then
    begin
      s := copy(s, i+14, 250);
      t := DemoCopy + LastCallingAE + '.v2';

      if Length(DemoViewer)>0 then
        StartProgram(DemoViewer, curdir, [s, LastCallingAE, LastCalledAE]);

      if Length(DemoCopy)>0 then
        CopyFile(Pchar(s), Pchar(t), false);
    end;
  end;
end;

procedure TForm1.ProgressSocketSessionAvailable(Sender: TObject;
  Error: Word);
var NewHSocket : TSocket;
begin
  NewHSocket := ProgressSocket.Accept;
  ProgressSocket.Dup(NewHSocket);
end;

var ProgressActive: integer = 0;

procedure TForm1.ProgressSocketDataAvailable(Sender: TObject;
  Error: Word);
  var buffer: array[0..80192] of ansichar;
  s, t, p, f : string;
  i, j: integer;
  pg : TProgressBar;
  mem: Pchar;
begin
  buffer[0] := #00; // for when recieve fails
  i := ProgressSocket.Receive(@buffer, 80191);

  buffer[i] := #10;
  buffer[i+1] := #00;
  s := buffer;

  pg := ProgressBar2;
  if PageControl1.ActivePage = TabSheet7 then
    pg := ProgressBar3;

  while pos(#10, s)>0 do
  begin
    if length(s)<1 then exit;

    p := GetKey(s, 'Process');

    t := GetKey(s, 'Type');
    t := copy(t, 2, length(t)-2);
    if t = 'printbasicfilmbox' then
    begin
      GetMem(mem, 256);
      StrPCopy(mem, 'Printing Basic Film Box');
      PrinterQueue.Add(mem);
    end;

    if t = 'printbasicfilmsession' then
    begin
      GetMem(mem, 256);
      StrPCopy(mem, 'Printing Basic Film Session');
      PrinterQueue.Add(mem);
    end;

    if t = 'creatingbasicfilmbox' then
    begin
      GetMem(mem, 256);
      f := GetKey(s, 'Film');
      j := StrToIntDef(f, 0);

      i := Pos('PORTRAIT', s);
      if i>0 then
        StrPCopy(mem, 'Creating Basic Portrait Film Box - Film# ' + IntToStr(j))
      else
        StrPCopy(mem, 'Creating Basic Landscape Film Box - Film# ' + IntToStr(j));
      PrinterQueue.Add(mem);
    end;

    if t = 'printfile' then
    begin
      f := GetKey(s, 'File');
      f := copy(f, 2, length(f)-2);
      GetMem(mem, 256);
      StrPCopy(mem, f);
      PrinterQueue.Add(mem);
    end;

    t := GetKey(s, 'Active');
    if t='1' then
    begin
      pg.Visible := true;
      ProgressActive:=1;
      //inc(ProgressActive);
    end
    else if t='0' then
    begin
      //dec(ProgressActive);
      ProgressActive:=0;
      pg.Visible := ProgressActive<>0;
      if PageControl1.ActivePage <> TabSheet7 then RefreshDatabase
      else Memo1.Lines.Add(p + '. Copying done.');
    end;

    t := GetKey(s, 'Total');
    if pg.Visible and (t<>'') then
    begin
      pg.Max := StrToIntDef(t, 100)-1;
      f := GetKey(s, 'Current');
      if pg.Position < StrToIntDef(f, 100) then
        pg.Position := StrToIntDef(f, 100);
    end;
    pg.Update;
    s := copy(s, pos(#10, s)+1, 99999);
    //Application.ProcessMessages();
  end;
end;

// Serves Printer Queue

var InTimer: boolean;

procedure TForm1.Print1Click(Sender: TObject);
begin
  PrintSelectedImages1.Enabled := Table4.Active;
  PrintImage1.Enabled := Table4.Active;
  DICOMPrintSelectedImagesTo.Enabled := Table4.Active;
end;

var headerbmp, footerbmp, backgroundbmp: TBitmap;

procedure printheaderfooter;
 var scale: single;
     rc: TRect;
     top, height: integer;
begin
  if (not assigned(headerbmp)) and (headerbmpname<>'') and fileexists(headerbmpname) then
  begin
    headerbmp := TBitmap.Create;
    headerbmp.LoadFromFile(headerbmpname);
  end;

  if (not assigned(footerbmp)) and (footerbmpname<>'') and fileexists(footerbmpname) then
  begin
    footerbmp := TBitmap.Create;
    footerbmp.LoadFromFile(footerbmpname);
  end;

  if (not assigned(backgroundbmp)) and (backgroundbmpname<>'') and fileexists(backgroundbmpname) then
  begin
    backgroundbmp := TBitmap.Create;
    backgroundbmp.LoadFromFile(backgroundbmpname);
  end;

  if not printer.Printing then
    exit;

  top := 0;
  Height := printer.PageHeight;

  if assigned(headerbmp) then
  begin
    scale := Printer.PageWidth / headerbmp.Width;

    rc.Top    := 0;
    rc.Bottom := round(scale * headerbmp.height);
    rc.Left   := 0;
    rc.Right  := Printer.PageWidth;
    top := rc.Bottom;
    height := height - top;

    Printer.Canvas.MoveTo(0,0);             // required otherwise StretchDraw
    Printer.Canvas.LineTo(0,0);             // ... sometimes fails
    Printer.Canvas.StretchDraw(rc, headerbmp);
  end;

  if assigned(footerbmp) then
  begin
    scale := Printer.PageWidth / footerbmp.Width;

    rc.Top    := round(Printer.PageHeight-scale*footerbmp.height);
    rc.Bottom := Printer.PageHeight;
    rc.Left   := 0;
    rc.Right  := Printer.PageWidth;
    height := height - round(scale*footerbmp.height);

    Printer.Canvas.MoveTo(0,0);             // required otherwise StretchDraw
    Printer.Canvas.LineTo(0,0);             // ... sometimes fails
    Printer.Canvas.StretchDraw(rc, footerbmp);
  end;

  if assigned(backgroundbmp) then
  begin
    rc.Top    := top;
    rc.Bottom := top + Height;
    rc.Left   := 0;
    rc.Right  := Printer.PageWidth;

    Printer.Canvas.MoveTo(0,0);             // required otherwise StretchDraw
    Printer.Canvas.LineTo(0,0);             // ... sometimes fails
    Printer.Canvas.StretchDraw(rc, backgroundbmp);
  end;
end;

procedure TForm1.PrinterTimerTimer(Sender: TObject);
var LastFile, tmp: string;
    l: Tlist;
    j, film, rows, cols, num, num2, h, w, dx, dy: integer;
    Bmp: TBitmap;
    rc: TRect;
    scale: single;
    var top, height: integer;
begin
  if InTimer then exit;
  InTimer := true;

  if assigned(ServerstatusMemo) and
    (Time-lastWrite > 0.1/(24*3600))
    then
  begin
    //Application.MainForm.Caption := 'Timerupdate: ' + FloatToStr(lastUpdate);
    ServerstatusMemo.Text := ServerStatusStringList.Text;
    ServerstatusMemo.Perform(EM_LineScroll, 0, ServerstatusMemo.Lines.Count); {move to bottom}
    lastWrite := 1e38;
  end;

  if not assigned(PrinterQueue) then
  begin
    InTimer := false;
    exit;
  end;

  rows := 0;
  cols := 0;
  num  := 0;
  num2 := 0;
  film := 0;

  l := PrinterQueue.LockList;
  if l.count<>0 then
  begin
    ProgressBar1.Hint := 'Print server queue contains ' + IntToStr(l.Count) + ' images';
    LastFile := Pchar(l.items[0]);
    Freemem(l.items[0]);
    l.Delete(0);
    ProgressBar1.Position := l.Count;
  end
  else
    ProgressBar1.Hint := 'Print server queue is empty';
  PrinterQueue.UnlockList;

  if LastFile<>'' then
  begin
    //For debug:
    //MaintenanceMemo.Lines.Add(LastFile);

    if LastFile = 'Printing Basic Film Box' then
    begin
      if Printer.Printing then Printer.EndDoc;
      InTimer := false;
      exit;
    end;

    if LastFile = 'Printing Basic Film Session' then
    begin
      if Printer.Printing then Printer.EndDoc;
      InTimer := false;
      exit;
    end;

    // get film number when creating film box (later for multi-user printing)
    j := Pos('Film#', LastFile);
    if j>0 then film := StrToInt(copy(LastFile, j + 6, 5));

    if Pos('Creating Basic Portrait Film Box', LastFile)>0 then
    begin
      if not Printer.Printing then
        Printer.Orientation := poPortrait
      else if Printer.Printing then
      begin
        Printer.NewPage;
        printheaderfooter;
      end;
      InTimer := false;
      exit;
    end;

    if Pos('Creating Basic Landscape Film Box', LastFile)>0 then
    begin
      if not Printer.Printing then
        Printer.Orientation := poLandscape
      else if Printer.Printing then
      begin
        Printer.NewPage;
        printheaderfooter;
      end;
      InTimer := false;
      exit;
    end;

    tmp := Lastfile + '.BMP';

    try
      if fileexists('printerdisplay.lua') then
        ServerTask('', 'lua:x=DicomObject:new();x:Read[['+GuiToServerFilename(LastFile)+']];outfile=[['+GuiToServerFilename(tmp)+']];dofile("printerdisplay.lua")')
      else
        ServerTask('', 'convert_to_bmp:'+GuiToServerFilename(LastFile)+',9600,'+GuiToServerFilename(tmp));
    except
    end;

    if FileExists(tmp) then
    begin
      for j:=1 to Length(LastFile) do
        if LastFile[j]='.' then num2 := j;
      for j:=1 to num2-1 do
        if LastFile[j]='.' then num  := j;
      for j:=1 to num-1 do
        if LastFile[j]='.' then rows := j;
      for j:=1 to rows-1 do
        if LastFile[j]='.' then cols := j;
      for j:=1 to cols-1 do
        if LastFile[j]='.' then film := j;

      film := StrToInt(copy(LastFile, film+1, cols-film-1)); // for multi-user printing
      cols := StrToInt(copy(LastFile, cols+1, rows-cols-1));
      rows := StrToInt(copy(LastFile, rows+1, num -rows-1));
      num  := StrToInt(copy(LastFile, num +1, num2-num -1));

      if cols=0 then cols:=1;

      Bmp := TBitmap.Create;
      try
        Bmp.LoadFromFile(tmp);

        if not Printer.Printing then
        begin
          Printer.Title := 'Conquest DICOM printer - film# ' + IntToStr(film);
          Printer.BeginDoc;
          printheaderfooter;
        end;

    if printer.Printing then
    begin
    top := 0;
    height := Printer.PageHeight;
    if assigned(headerbmp) then
    begin
      scale := round(Printer.PageWidth / headerbmp.width);
      top := round(scale * headerbmp.Height);
      height := height - top;
    end;

    if assigned(footerbmp) then
    begin
      scale := round(Printer.PageWidth / footerbmp.width);
      height := height - round(scale * footerbmp.Height);
    end;

        scale := Printer.PageWidth / (Bmp.Width * cols);
        if scale > height / (Bmp.Height * rows) then
          scale := height / (Bmp.Height * rows);

        h := round(Bmp.Height * scale);
        w := round(Bmp.Width  * scale);

        dx := (Printer.PageWidth  - w * cols) div 2;
        dy := top + (height - h * rows) div 2;

        rc.Top    := dy + (num div cols) * h;
        rc.Bottom := dy + (num div cols) * h + h;
        rc.Left   := dx + (num mod cols) * w;
        rc.Right  := dx + (num mod cols) * w + w;

        Printer.Canvas.MoveTo(0,0);             // required otherwise StretchDraw
        Printer.Canvas.LineTo(0,0);             // ... sometimes fails
        Printer.Canvas.StretchDraw(rc, Bmp);
    end;
      finally
        Bmp.Free;
        DeleteFile(tmp);
        DeleteFile(LastFile);
      end;
    end;
  end;
  InTimer := false;
end;

// switch between threaded mode and multi process mode

procedure TForm1.RunInThreadedModeClick(Sender: TObject);
//var p, c: integer;
begin
  if Sender<> Form1 then
    if MessageDlg('Switch server mode (terminates active connections) ?', mtConfirmation,
      [mbYes, mbNo], 0) = mrNo then Exit;

  Screen.Cursor := crHourGlass;

  // go to multi-process mode
  if false then // RunInThreadedMode.Caption = 'Run in multi-process mode' then
  begin
    if ThreadedProcess<>0 then
    begin
      TerminateProcess(ThreadedProcess, 0);
      CloseHandle(ThreadedProcess);
      ThreadedProcess := 0;
      Sleep(500);
    end;

//    val(trim(TCPIPport.text), p, c);
    Sleep(500);
    ListenSocket.Addr := '0.0.0.0';
//    ListenSocket.Port  := IntToStr(p);
    Sleep(500);
    ListenSocket.Listen;

    RunInThreadedMode.Caption := 'Run in threaded mode';
  end
  else
  // go to threaded mode
  begin
    if ActiveProcess<>0 then
    begin
      TerminateProcess(ActiveProcess, 0);
      CloseHandle(ActiveProcess);
      ActiveProcess := 0;
    end;

    if ThreadedProcess<>0 then
    begin
      TerminateProcess(ThreadedProcess, 0);
      CloseHandle(ThreadedProcess);
      ThreadedProcess := 0;
    end;

    Sleep(500);
    ListenSocket.Close;

    Sleep(500);
    ThreadedProcess := RunDgate('-!'+ServerStatusSocket.Port+' -x!'+ProgressSocket.Port, true);
    WriteLog('started dgate as threaded process');

    // failed ?
    if ThreadedProcess<>0 then
      RunInThreadedMode.Caption := 'Run in multi-process mode';
  end;

  Screen.Cursor := crDefault;
end;

var killed:Boolean;

procedure TForm1.KillAndRestartTheServerClick(Sender: TObject);
begin
  if Assigned(sender) then
    if MessageDlg('Re-start the server (terminates active processes) ?', mtConfirmation,
      [mbYes, mbNo], 0) = mrNo then Exit;

  Screen.Cursor := crHourGlass;
  killed := true;

  if ActiveProcess<>0 then
  begin
    TerminateProcess(ActiveProcess, 0);
    CloseHandle(ActiveProcess);
    ActiveProcess := 0;
  end;

  if ThreadedProcess<>0 then
  begin
    TerminateProcess(ThreadedProcess, 0);
    CloseHandle(ThreadedProcess);
    Sleep(500);
  end;

  if not PreRunning then
    ThreadedProcess := RunDgate('-!'+ServerStatusSocket.Port+' -x!'+ProgressSocket.Port, true);

  if PreRunning then
  begin
    RunProgramBlocking(curdir + '\dgateserv.exe', curdir,
                      ['/service', trim(ServerName.text),
                       '/stop'
                      ]);
    Sleep(500);

    if TestLocalServer(false, true) then
    begin
      ShowMessage('Failed to stop service - you may need to run this program as administrator');
      exit;
    end;

    RunProgramBlocking(curdir + '\dgateserv.exe', curdir,
                      ['/service', trim(ServerName.text),
                       '/start'
                      ]);
    Sleep(500);
  end;

  WriteLog('killed and restarted the server');

  Screen.Cursor := crDefault;

  Sleep(1000);

  ServerTask('', 'log_on:'+ServerStatusSocket.Port);
  Sleep(500);

  TestLocalServer(false, false);
  CheckBoxDebugLog.Checked := false;
  CheckBoxWebServer.Checked := false;
  CheckBoxOnlyLogToFile.Checked := false;
end;

procedure TForm1.KillAndRestartTheServerMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
var i : integer;
begin
  if Button <> mbRight    then exit;

  if ssAlt in Shift then
  begin
    if not PreRunning       then exit;

    if MessageDlg('Re-start the four services (terminates active processes) ?', mtConfirmation,
      [mbYes, mbNo], 0) = mrNo then Exit;

    Screen.Cursor := crHourGlass;
    killed := true;

    if PreRunning then
    begin
      RunProgramBlocking(curdir + '\dgateserv.exe', curdir, 
                        ['/service', trim(ServerName.text),
                         '/stop'
                        ]);
      for i:=2 to 4 do
        RunProgramBlocking(curdir + '\dgateserv.exe', curdir,
                          ['/service', trim(ServerName.text) + '_' + IntToStr(i),
                           '/stop'
                          ]);

      Sleep(500);

      if TestLocalServer(false, true) then
      begin
        ShowMessage('Failed to stop service - you may need to run this program as administrator');
        exit;
      end;

      RunProgramBlocking(curdir + '\dgateserv.exe', curdir,
                        ['/service', trim(ServerName.text),
                         '/start'
                        ]);
      for i:=2 to 4 do
        RunProgramBlocking(curdir + '\dgateserv.exe', curdir, 
                          ['/service', trim(ServerName.text) + '_' + IntToStr(i),
                           '/start'
                          ]);

      Sleep(500);
    end;

    Screen.Cursor := crDefault;

    WriteLog('killed and restarted four servers');

    Sleep(1000);
    TestLocalServer(false, false);
    CheckBoxDebugLog.Checked := false;
    CheckBoxWebServer.Checked := false;
    CheckBoxOnlyLogToFile.Checked := false;
end
  else
  begin
    if MessageDlg('Kill the server (terminates active connections) ?', mtConfirmation,
      [mbYes, mbNo], 0) = mrNo then Exit;

    Screen.Cursor := crHourGlass;
    killed := true;

    if ActiveProcess<>0 then
    begin
      TerminateProcess(ActiveProcess, 0);
      CloseHandle(ActiveProcess);
      ActiveProcess := 0;
    end;

    if ThreadedProcess<>0 then
    begin
      TerminateProcess(ThreadedProcess, 0);
      CloseHandle(ThreadedProcess);
      ThreadedProcess := 0;
    end;

    if PreRunning then
    begin
      RunProgramBlocking(curdir + '\dgateserv.exe', curdir,
                        ['/service', trim(ServerName.text),
                         '/stop'
                        ]);
      Sleep(500);
    end;

    WriteLog('killed the server');

    Screen.Cursor := crDefault;
  end
end;

procedure TForm1.ButtonBugReportClick(Sender: TObject);
var g: file of byte;
    size: integer;
    ZipFileName: string;
begin
  ZipFileName := curdir + '\bugreport_' + FormatDateTime('yyyymmdd', date) + '.zip';	// zip for this day

  WriteMemoSl(ServerStatusMemo, '-----------------------------------------------------', 1100, 1000, 'serverstatus', true, ServerStatusStringList);
  WriteMemoSl(ServerStatusMemo, 'Creating bug report zip file for server version '+VERSION, 200, 100, 'serverstatus', true, ServerStatusStringList);

  if FileExists(curdir + '\' + dgateExe) then
    WriteMemoSl(ServerStatusMemo, 'File date for dgate.exe:               '+ FormatDateTime('yyyymmdd hh:mm:ss', FileDateToDateTime(FileAge(curdir + '\' + dgateExe))), 1100, 1000, 'serverstatus', true, ServerStatusStringList);

  if FileExists(curdir + '\lua5.1.dll') then
    WriteMemoSl(ServerStatusMemo, 'File date for lua5.1.dll:              '+ FormatDateTime('yyyymmdd hh:mm:ss', FileDateToDateTime(FileAge(curdir + '\lua5.1.dll'))), 1100, 1000, 'serverstatus', true, ServerStatusStringList);

    if FileExists(curdir + '\dgateserv.exe') then
    WriteMemoSl(ServerStatusMemo, 'File date for dgateserv.exe:           '+ FormatDateTime('yyyymmdd hh:mm:ss', FileDateToDateTime(FileAge(curdir + '\dgateserv.exe'))), 1100, 1000, 'serverstatus', true, ServerStatusStringList);

  if FileExists(curdir + '\dgate.dic') then
    WriteMemoSl(ServerStatusMemo, 'File date for dgate.dic:               '+ FormatDateTime('yyyymmdd hh:mm:ss', FileDateToDateTime(FileAge(curdir + '\dgate.dic'))), 21100, 1000, 'serverstatus', true, ServerStatusStringList);

  if FileExists(curdir + '\ConquestDICOMServer.exe') then
    WriteMemoSl(ServerStatusMemo, 'File date for ConquestDICOMServer.exe: '+ FormatDateTime('yyyymmdd hh:mm:ss', FileDateToDateTime(FileAge(curdir + '\ConquestDICOMServer.exe'))), 1100, 1000, 'serverstatus', true, ServerStatusStringList);

  if FileExists(curdir + '\ConquestPacs.doc') then
    WriteMemoSl(ServerStatusMemo, 'File date for ConquestPacs.doc:        '+ FormatDateTime('yyyymmdd hh:mm:ss', FileDateToDateTime(FileAge(curdir + '\ConquestPacs.doc'))), 1100, 1000, 'serverstatus', true, ServerStatusStringList);

  if FileExists(curdir + '\cqdicom.dll') then
    WriteMemoSl(ServerStatusMemo, 'File date for cqdicom.dll:             '+ FormatDateTime('yyyymmdd hh:mm:ss', FileDateToDateTime(FileAge(curdir + '\cqdicom.dll'))), 1100, 1000, 'serverstatus', true, ServerStatusStringList);

  if FileExists(curdir + '\Dcmcjpeg.exe') then
    WriteMemoSl(ServerStatusMemo, 'File date for Dcmcjpeg.exe:            '+ FormatDateTime('yyyymmdd hh:mm:ss', FileDateToDateTime(FileAge(curdir + '\Dcmcjpeg.exe'))), 1100, 1000, 'serverstatus', true, ServerStatusStringList);

  if FileExists(curdir + '\Dcmdjpeg.exe') then
    WriteMemoSl(ServerStatusMemo, 'File date for Dcmdjpeg.exe:            '+ FormatDateTime('yyyymmdd hh:mm:ss', FileDateToDateTime(FileAge(curdir + '\Dcmdjpeg.exe'))), 1100, 1000, 'serverstatus', true, ServerStatusStringList);

  RunProgramBlocking(curdir + '\7za.exe', curdir, ['-tzip', '-mx=9', 'a', ZipFileName, '*.log *.ini *.sql *.map *.lst USE*']);

  AssignFile(g, ZipFileName);
  Reset(g);
  size := FileSize(g);
  CloseFile(g);
  if size > 1024000 then
  begin
    ShowMessage('Please shorten log files using a text editor and then create the bugreport file again');
    WriteMemoSl(ServerStatusMemo, 'No bug report file has been created.', 1100, 1000, 'serverstatus', true, ServerStatusStringList);
    DeleteFile(ZipFileName);
  end
  else
  begin
    WriteMemoSl(ServerStatusMemo, 'Created bug report file as: ' + ZipFileName, 1100, 1000, 'serverstatus', true, ServerStatusStringList);
    WriteMemoSl(ServerStatusMemo, 'Mail this file to: vanherkmarcel@gmail.com, with a short description of the problem', 1100, 1000, 'serverstatus', true, ServerStatusStringList);
  end;
  WriteMemoSl(ServerStatusMemo, '-----------------------------------------------------', 1100, 1000, 'serverstatus', true, ServerStatusStringList);
end;

procedure TForm1.ButtonBugReportMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  if Button <> mbRight    then exit;
  ShellExecute(Application.Handle, nil,
    'explorer.exe', pchar('/n,'+CurDir), nil, SW_NORMAL)
end;

procedure TForm1.CheckBoxDebugLogClick(Sender: TObject);
begin
  ServerTask('set debug level ' + IntToStr(UpDownDebugLevel.Position) + ' from GUI', 'debuglevel:'+IntToStr(UpDownDebugLevel.Position));
  CheckBoxOnlyLogToFileClick(self);
end;

procedure TForm1.CheckBoxOnlyLogToFileClick(Sender: TObject);
begin
  if CheckBoxOnlyLogToFile.Checked then
  begin
    if CheckBoxDebugLog.Checked then
      ServerTask('set debug log directly to file from GUI', 'debuglog_on:serverstatus.log')
    else
      ServerTask('set normal log directly to file from GUI', 'log_on:serverstatus.log');
  end
  else
  begin
    if CheckBoxDebugLog.Checked then
      ServerTask('set debug log from GUI', 'debuglog_on:'+ServerStatusSocket.Port)
    else
      ServerTask('set normal log from GUI', 'log_on:'+ServerStatusSocket.Port);
  end;
end;

procedure TForm1.UpDownDebugLevelClick(Sender: TObject;
  Button: TUDBtnType);
  var c, level: Integer;
begin
  if Button=btNext then c := 1
  else c := -1;
  level := UpDownDebugLevel.Position+c;

  UpDownDebugLevel.Hint := 'Currently selected debug level = ' + IntToStr(level);
  ServerTask('set debug level ' + IntToStr(level) + ' from GUI', 'debuglevel:'+IntToStr(level));
end;

procedure TForm1.CheckBoxSmallFontsClick(Sender: TObject);
begin
  if CheckBoxSmallFonts.Checked then
  begin
    ServerStatusMemo.Font.Name := 'Small Fonts';
    ServerStatusMemo.Font.Size := 6;
    InstallationMemo.Font.Name := 'Small Fonts';
    InstallationMemo.Font.Size := 6;
    MaintenanceMemo.Font.Name := 'Small Fonts';
    MaintenanceMemo.Font.Size := 6;
  end
  else
  begin
    ServerStatusMemo.Font.Name := 'MS Sans Serif';
    ServerStatusMemo.Font.Size := 8;
    InstallationMemo.Font.Name := 'MS Sans Serif';
    InstallationMemo.Font.Size := 8;
    MaintenanceMemo.Font.Name := 'MS Sans Serif';
    MaintenanceMemo.Font.Size := 8;
  end;
end;

procedure TForm1.CheckBoxKeepAliveClick(Sender: TObject);
begin
 Timer4.Enabled := CheckBoxKeepAlive.Checked;
 if CheckBoxKeepAlive.Checked then KeepAlive := 60 else KeepAlive := 0;
 Timer4.Interval := KeepAlive * 1000;
end;

procedure TForm1.Timer4Timer(Sender: TObject);
var resp: integer;
begin
  resp := 1;
  try
    resp := DcmEcho(trim(ServerName.text), trim(ServerName.text), '127.0.0.1', trim(TCPIPport.text), '', 'silent');
  except
  end;

  if resp<>0 then
  begin
    WriteMemoSl(ServerStatusMemo, ' *** Restarted dead server after error ' + IntToStr(resp), 1100, 1000, 'serverstatus', true, ServerStatusStringList);
    WriteLog('Restarted dead failed server after error ' + IntToStr(resp));
    KillAndRestartTheServerClick(nil);
  end;
end;

procedure TForm1.FlushButtonClick(Sender: TObject);
begin
//  FlushMemo(ArchiveMemo,      0, 0, 'archive', false);
//  FlushMemo(ServerStatusMemo, 0, 0, 'serverstatus');
//  FlushMemo(InstallationMemo, 0, 0, 'installation');
//  FlushMemo(MaintenanceMemo,  0, 0, 'maintenance');
//  FlushMemo(Memo2,            0, 0, 'tapebackup');
//  FlushMemo(WeeklyMemo,       0, 0, 'weeklychecks');
end;

{************************************************************************}
// General purpose buttons: exit and about....

procedure TForm1.Close1Click(Sender: TObject);
var text: string;
begin
  if PreRunning then
    text := 'Close this user interface to the ConQuest DICOM server ?'
  else
    text := 'Close the ConQuest DICOM server ?';

  if MessageDlg(text, mtConfirmation,
    [mbYes, mbNo], 0) = mrYes then
  begin
    // kill the threaded process if it is running
    if ThreadedProcess<>0 then
    begin
      TerminateProcess(ThreadedProcess, 0);
      CloseHandle(ThreadedProcess);
    end;

    FlushButtonClick(self);
    WriteLog('closing the server');

    if PreRunning then
    begin
      if CheckBoxDebugLog.Checked then
        ServerTask('set debug log from GUI', 'debuglog_on:serverstatus.log')
      else
        ServerTask('set normal log from GUI', 'log_on:serverstatus.log');
    end;

    Application.Terminate;
  end;
end;

procedure TForm1.About1Click(Sender: TObject);
begin
  Form2.ShowModal;
end;

procedure TForm1.WMQueryEndSession(var Message: TWMQueryEndSession);
begin
  NoConfirmQuit := true;
  Message.Result := 1;
end;

procedure TForm1.FormCloseQuery(Sender: TObject; var CanClose: Boolean);
var text: string;
begin
  if NoConfirmQuit then
  begin
    CanClose := True;
    if CheckBoxDebugLog.Checked then
      ServerTask('set debug log from GUI', 'debuglog_on:serverstatus.log')
    else
      ServerTask('set normal log from GUI', 'log_on:serverstatus.log');
    exit;
  end;

  CanClose := False;
//  Hide;

  if PreRunning then
    text := 'Close this user interface to the ConQuest DICOM server ?'
  else
    text := 'Close the ConQuest DICOM server ?';

  if MessageDlg(text, mtConfirmation,
    [mbYes, mbNo], 0) = mrYes then
  begin
    CanClose := True;

    InstallationSocket.Close;
    MaintenanceSocket .Close;
    ServerStatusSocket.Close;
    ListenSocket.Close;

    WriteLog('closing the server');
    FlushButtonClick(self);

    // kill the threaded process if it is running
    if ThreadedProcess<>0 then
    begin
      TerminateProcess(ThreadedProcess, 0);
      CloseHandle(ThreadedProcess);
    end;

    if PreRunning then
    begin
      if CheckBoxDebugLog.Checked then
        ServerTask('set debug log from GUI', 'debuglog_on:serverstatus.log')
      else
        ServerTask('set normal log from GUI', 'log_on:serverstatus.log');
    end;

    MDBFTable1.Close;
    if FileExists(MDBFTable1.FileName) then DeleteFile(MDBFTable1.FileName);
    MDBFTable2.Close;
    if FileExists(MDBFTable2.FileName) then DeleteFile(MDBFTable2.FileName);
    MDBFTable3.Close;
    if FileExists(MDBFTable3.FileName) then DeleteFile(MDBFTable3.FileName);
    MDBFTable4.Close;
    if FileExists(MDBFTable4.FileName) then DeleteFile(MDBFTable4.FileName);
  end;
end;

procedure TForm1.HideButtonClick(Sender: TObject);
begin
  Hide;
end;

procedure TForm1.ServerNameChange(Sender: TObject);
begin
  if not PreRunning then
  begin
    Form1.Caption := 'ConQuest DICOM server '+ VERSION +' - ' + Trim(Servername.Text);
    TrayIcon1.Hint := 'ConQuest DICOM server '+ VERSION +' - ' + Trim(Servername.Text);
    ServerName.Hint     := 'This entry may be 16 letters long';
    ServerName.ReadOnly := false;
    ServerName.Color    := clWhite;
  end
  else
  begin
    Form1.Caption := 'ConQuest DICOM server '+ VERSION +' user interface - ' + Trim(Servername.Text);
    TrayIcon1.Hint := 'ConQuest DICOM server '+ VERSION +' user interface - ' + Trim(Servername.Text);
    ServerName.Hint     := 'You cannot change the name while the server runs as an NT service';
    ServerName.ReadOnly := true;
    ServerName.Color    := clltGray;
  end;
end;

procedure TForm1.TrayIcon1Click(Sender: TObject);
begin
  Show;
  Application.Restore;
end;

procedure TForm1.TrayIcon1RightClick(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  Show;
  Application.Restore;
end;

{************************************************************************}
// Query/Move page

var StopCopying: integer;

// Make query string that is used in Lua code
function MakeQueryString(Level, PatientID, PatientName,
                        StudyDate, SeriesNo, ImageNo, StudyUID, SeriesUID, ImageUID, SeriesDesc: ansistring): string;
var i, nlevel: integer;
begin
  // remove anything after a space in the level (sop class description)
  i := Pos(' ', Level);
  if i>0 then Level := Copy(Level, 1, i-1);
  if Level = 'Modality' then Level := '';

  // truncate QUERY request depending of level
  nlevel := 0;
  if Level = ''         then nlevel := 4;
  if Level = 'PATIENT'  then nlevel := 3;
  if Level = 'STUDY'    then nlevel := 4;
  if Level = 'SERIES'   then nlevel := 7;
  if Level = 'IMAGE'    then nlevel := 9;
  if Level = 'IMAGE2'   then nlevel := 11;
  if Level = 'PATIENTP' then nlevel := 3;
  if Level = 'STUDYP'   then nlevel := 4;

  Result := '{';
  if Level = 'IMAGE2'  then
    Result := Result + 'QueryRetrieveLevel=[[IMAGE]],'
  else if Level <> '' then
    Result := Result + 'QueryRetrieveLevel=[[' + TrimRight(Level) + ']],';
  Result := Result + 'PatientID=[[' + TrimRight(PatientID) + ']],';
  Result := Result + 'PatientName=[[' + TrimRight(PatientName) + ']],';

  if Level = '' then
    Result := Result + 'AccessionNumber=[[' + TrimRight(StudyDate) + ']],'
  else if nlevel>=4 then
    Result := Result + 'StudyDate=[[' + TrimRight(StudyDate) + ']],';

  if nlevel>4 then
  begin
    Result := Result + 'SeriesNumber=[[' + TrimRight(SeriesNo) + ']],';
    Result := Result + 'SeriesDescription=[[' + TrimRight(SeriesDesc) + ']],';
    Result := Result + 'Modality=[[]],';
  end;

  if nlevel>7 then
  begin
    Result := Result + 'InstanceNumber=[[' + TrimRight(ImageNo) + ']],';
    Result := Result + 'SOPInstanceUID=[[' + TrimRight(ImageUID) + ']],';
  end;

  if nlevel>9 then
  begin
    Result := Result + 'StudyInstanceUID=[[' + TrimRight(StudyUID) + ']],';
    Result := Result + 'SeriesInstanceUID=[[' + TrimRight(SeriesUID) + ']],';
  end;


  if Form1.Label14.Caption<>'Series number:' then
  begin
    if Level = 'STUDY'  then
      Result := Result + 'StudyInstanceUID=[[' + TrimRight(StudyUID) + ']],';
    if Level = 'SERIES' then
    begin
      Result := Result + 'StudyInstanceUID=[[' + TrimRight(StudyUID) + ']],';
      Result := Result + 'SeriesInstanceUID=[[' + TrimRight(SeriesUID) + ']],';
    end;
    if Level = 'IMAGE' then
    begin
      Result := Result + 'StudyInstanceUID=[[' + TrimRight(StudyUID) + ']],';
      Result := Result + 'SeriesInstanceUID=[[' + TrimRight(SeriesUID) + ']],';
    end;
  end;
  Result := Result + '}';
end;

// split an AE string as used in the dicom system dropdown lists
procedure SplitAE(AE: AnsiString; var AEName, IPaddress, Port: AnsiString);
var a, b, c: integer;
begin
  AEName    := '';
  IPaddress := '';
  Port      := '';

  a := Pos(' ', AE);
  b := Pos(' ', Copy(AE, a+40, 199)) + a + 39;
  c := Pos(' ', Copy(AE, b+40, 199)) + b + 39;

  if a=0 then exit;
  if b=0 then exit;

  AEName    := copy(AE, 1,    a-1);
  IPaddress := copy(AE, a+40, b-a-1);
  Port      := copy(AE, b+40, c-b-1);
end;

// Make 'Send Image To' etc submenu
procedure CreateSendToMenu(var menu: TMenuItem; Index: integer; NewCode:boolean);
var
  I: Integer;
  mi: TMenuItem;
begin
  if not assigned(menu) then exit;
  while menu.Count > 0 do menu.Delete(0);

  for I := 0 to Form1.DicomSystem.Items.Count-1 do
  begin
    if NewCode then
      mi := NewItem(copy(Form1.DicomSystem.Items[I], 1, 16), 0, False, True, Form1.SendPatientToClick, 0, '')
    else
      mi := NewItem(copy(Form1.DicomSystem.Items[I], 1, 16), 0, False, True, Form1.SendImageToClick, 0, '');
    mi.Tag := I;
    mi.GroupIndex := Index;
    menu.Add(mi);
  end;
end;

// Put all known dicom servers in the dropdown lists
procedure FillAELists;
var i, j: integer;
    c: char;
    s, t: string;
    inspace: boolean;
begin
  Form1.DicomSystem.Items.Clear;
  Form1.TargetSystem.Items.Clear;

  for i:=0 to Form1.DicomMap.Lines.Count-1 do
  begin
    s := Form1.DicomMap.Lines[i];
    if Pos('*', s)>0 then continue;
    if Length(s)<>0 then
    begin
      c := s[1];
      if (c in ['a'..'z']) or (c in ['A'..'Z']) or (c in ['0'..'9'])then
      begin
        inspace := false;
        t       := '';

        for j:=1 to length(s) do
        begin
          if inspace then if not (s[j] in [char(9), ' ']) then
            inspace := false;

          if not inspace then
          begin
            if s[j] in [char(9), ' '] then
            begin
              inspace := true;
              t := t + format('%-40s', ['']);
            end
            else
              t := t + s[j];
          end;
        end;

        Form1.DicomSystem.Items.Add(t);
        Form1.TargetSystem.Items.Add(t);
      end;
    end;
  end;

  Form1.DicomSystem.ItemIndex := 0;
  Form1.TargetSystem.ItemIndex := 0;

  // make the list of query levels
  if Form1.QueryLevel.Items.Count=0 then
  begin
    Form1.QueryLevel.Items.Clear;
    Form1.QueryLevel.Items.Add('PATIENT');          // 0
    Form1.QueryLevel.Items.Add('STUDY');            // 1
    Form1.QueryLevel.Items.Add('SERIES');           // 2
    Form1.QueryLevel.Items.Add('IMAGE');            // 3

    //Form1.QueryLevel.Items.Add('STUDY - StudyRoot');   // 4
    //Form1.QueryLevel.Items.Add('SERIES - StudyRoot');  // 5
    //Form1.QueryLevel.Items.Add('IMAGE - StudyRoot');   // 6

    Form1.QueryLevel.Items.Add('PATIENT - PatientStudyOnly'); // 7
    Form1.QueryLevel.Items.Add('STUDY - PatientStudyOnly');   // 8

    Form1.QueryLevel.Items.Add('Modality WorkList');   // 9

    Form1.QueryLevel.ItemIndex := 0;
  end;

  StopCopying := 1;

  CreateSendToMenu(Form1.SendPatientTo, 1, true);
  CreateSendToMenu(Form1.SendStudyTo,   2, true);
  CreateSendToMenu(Form1.SendSeriesTo,  3, true);
  CreateSendToMenu(Form1.SendImageTo,   4, true);
  CreateSendToMenu(Form1.SendImagesTo,  5, false);
  CreateSendToMenu(Form1.DICOMPrintSelectedImagesTo, 6, false);
end;

function MyProgress(percentage: integer; status: PAnsiChar): integer; stdcall;
begin
  Form1.Memo1.Lines.Add(status);
  Application.ProcessMessages;
  result := StopCopying;
end;

var query_error: boolean;

procedure TForm1.Label10Click(Sender: TObject);
var
  OpenDialog: TFileOpenDialog;
  SelectedFolder: string;
begin
OpenDialog := TFileOpenDialog.Create(Application.MainForm);
try
  OpenDialog.Options := OpenDialog.Options + [fdoPickFolders];
  OpenDialog.DefaultFolder := Label10.Caption;
  if not OpenDialog.Execute then
    Abort;
  SelectedFolder := OpenDialog.FileName;
  Label10.Caption := SelectedFolder;
  Label5.Caption :=
    Format('Free disk space: %d gigabyte', [DiskFreeGB(Label10.Caption[1])]);
finally
  OpenDialog.Free;
end
end;

procedure TForm1.Label14DblClick(Sender: TObject);
begin
  if Label14.Caption='Series number:' then
  begin
    Label14.Caption := 'Study  UID:';
    Label38.Caption := 'Series UID:';
    Label18.Caption := 'Image  UID:';
  end
  else
  begin
    Label14.Caption := 'Series number:';
    Label38.Caption := 'S. description:';
    Label18.Caption := 'Image number:';
  end;

  EditQNum .text := '';
  EditQSDesc.text := '';
  EditQIm.text := '';
end;

// query a DICOM server, using Lua code
procedure TForm1.QueryButtonClick(Sender: TObject);
var RemoteAE, RemoteIP, RemotePort, s, t: ansistring;
    i, total: integer;
    first : boolean;
    q, code, level: string;
    strings: TStringList;
begin
  first := true;
  total := 0;
  query_error := false;
  strings := TStringList.Create;
  DeleteFile('move.txt');

  SplitAE(DicomSystem.Items[DicomSystem.ItemIndex], RemoteAE, RemoteIP, RemotePort);

  if ((QueryLevel.ItemIndex = 3) or (QueryLevel.ItemIndex = 6)) and
     (Length(EditQID.Text)=0)
  then
    if MessageDlg('Warning: do you really want to perform this query, which may take a very long time ?' +
                #13 + '(Note that during this operation the user interface will be inactive)', mtConfirmation,
    [mbYes, mbNo], 0) = mrNo then Exit;

  Memo1.Lines.Clear;

  s := EditQID.Text;
  repeat
    t := s;

    if pos(',', s)>1 then
    begin
      t := copy(s, 1, pos(',', s)-1);
      s := copy(s, pos(',', s)+1, 9999);
    end
    else
      s := '';

    level := QueryLevel.Items[QueryLevel.ItemIndex];
    i := Pos(' ', level);
    if i>0 then level := Copy(level, 1, i-1);
    if Pos('PatientStudyOnly', QueryLevel.Items[QueryLevel.ItemIndex]) > 0 then
      level := level + 'P';
    if level='Modality' then level := 'WORKLIST';


    if Label14.Caption='Series number:' then
      q := MakeQueryString(level,
        trim(t), EditQName.Text, EditQDate.Text, EditQnum.Text, EditQIM.Text,
        '', '', '', EditQSDesc.Text)
    else
      q := MakeQueryString(level,
        trim(t), EditQName.Text, EditQDate.Text, '', '',
        EditQnum.Text, EditQSDesc.Text, EditQIM.Text, '');

    code :=
    'local ae=[['+RemoteAE+']];' +
    'local level=[['+level+']];' +
    'local q='+q+';' +
    'local q2=DicomObject:new(); for k,v in pairs(q) do q2[k]=v end;' +
    'local r2=dicomquery(ae, level, q2);' +
    'local s=tempfile("txt") f=io.open(s, "wb");' +
    'if r2==nil then f:write("no connection with "..ae.."\n") returnfile=s f:close() return end; ' +
    'local r = loadstring("return "..r2:Serialize())();' +
    'r[1].QueryRetrieveLevel=nil; r[1].TransferSyntaxUID=nil; '+
    'local keys={} for k,v in pairs(r[1]) do if type(v)~="table" then keys[#keys+1]=k end end; '+
    'table.sort(keys, function(a, b) return string.sub(a, 1, 7)<string.sub(b, 1, 7) end); ' +
    'if first then for k,v in ipairs(keys) do f:write(v.."    ") end f:write("\n") end '+
    'if first then f:write("---------------------------------------------------------------------------------------------------------------------------------------------------------------\n") end '+
    'for k,v in ipairs(r) do '+
    '  for k2,v2 in ipairs(keys) do f:write("["..v[v2].."]    ") end f:write("\n") '+
    'end '+
    'returnfile=s f:close();' +
    '';

    if first then
      ServerTask('#query.txt','lua:local first=true; '+code)
    else
      ServerTask('#query.txt','lua:'+code);

    strings.LoadFromFile('query.txt');
    DeleteFile('query.txt');
    memo1.Lines.AddStrings(strings);
    if first then total := total + strings.Count-1
    else total := total + strings.Count;

    first := false;
  until length(s)=0;

  Memo1.Lines.Add(    '---------------------------------------------------------------------------------------------------------------------------------------------------------------');
  Memo1.Lines.Add(    'Total of ' + IntToStr(total-1) + ' item(s) found');
  strings.Free;
end;

procedure TForm1.ButtonFindMissingPatientsClick(Sender: TObject);
var RemoteAE, RemoteIP, RemotePort, q, code: AnsiString;
    strings : TStringList;
begin
  DeleteFile('move.txt');
  SplitAE(DicomSystem.Items[DicomSystem.ItemIndex], RemoteAE, RemoteIP, RemotePort);

  if (Length(EditQID.Text)=0)   and
     (Length(EditQName.Text)=0) and
     (Length(EditQDate.Text)=0) and
     (Length(EditQnum.Text)=0)  and
     (Length(EditQIM.Text)=0)   then
    if MessageDlg('Warning: do you really want to perform this query, which may take a very long time ?' +
                #13 + '(Note that during this operation the user interface will be inactive)', mtConfirmation,
    [mbYes, mbNo], 0) = mrNo then Exit;

  Memo1.Lines.Clear;
  Memo1.Lines.Add('------------ working  --------------');
  Memo1.Update;

  if Label14.Caption='Series number:' then
    q := MakeQueryString('IMAGE', EditQID.Text, EditQName.Text, EditQDate.Text, EditQnum.Text, EditQIM.Text, '', '', '', EditQSDesc.Text)
  else
    q := MakeQueryString('IMAGE', EditQID.Text, EditQName.Text, EditQDate.Text, '', '', EditQnum.Text, EditQSDesc.Text, EditQIM.Text, '');

  code :=
  'local localae=[['+trim(ServerName.Text)+']];' +
  'local remoteae=[['+RemoteAE+']];' +
  'local q='+q+';' +
  'q2=DicomObject:new(); for k,v in pairs(q) do q2[k]=v end;' +
  'local s=tempfile("txt") f=io.open(s, "wb");' +
  'local r1 = dicomquery(localae, "IMAGE", q2);' +
  'if r1==nil then f:write("\nno connection with "..localae.."\n") returnfile=s f:close() return end; ' +
  'local r2 = dicomquery(remoteae, "IMAGE", q2);' +
  'if r2==nil then f:write("\nno connection with "..remoteae.."\n") returnfile=s f:close() return end; ' +
  'local sops = {}; '+
  'for i=0, #r2-1 do sops[r2[i].SOPInstanceUID]=r2[i].PatientID end; '+
  'for i=0, #r1-1 do sops[r1[i].SOPInstanceUID]=nil end; '+
  'local pats = {}; for k,v in pairs(sops) do pats[v] = (pats[v] or 0) + 1 end; '+
  'for k,v in pairs(pats) do f:write(k .. ",") end; f:write("\n"); ' +
  'f:write("images in "..localae  .. " (local) = " .. #r1 .. "\n"); ' +
  'f:write("images in "..remoteae .. " (remote) = " .. #r2 .. "\n\n"); ' +
  'local total=0; for k,v in pairs(pats) do total=total+v; f:write(k .. " misses " .. v .. " images\n") end; ' +
  'f:write("\n"); ' +
  'f:write("total missing images = " .. total .. "\n"); ' +
  'f:write("Select "..localae.." as destination and press copy to collect them!\n"); ' +
  'returnfile=s f:close();' +
  '';
  ServerTask('#query.txt','lua:'+code);

  Memo1.Lines.Clear;
  strings := TStringList.Create;
  strings.LoadFromFile('query.txt');
  if Length(strings[0])<>0 then
    EditQID.Text := Copy(strings[0], 1, length(strings[0])-1);
  strings[0] := '---------------------------------------------------------------------------------------------------------------------------------------------------------------';
  DeleteFile('query.txt');
  memo1.Lines.AddStrings(strings);
  strings.Free;
  Memo1.Lines.Add('---------------------------------------------------------------------------------------------------------------------------------------------------------------');
end;

procedure TForm1.ModifyMenuClick(Sender: TObject);
begin
  ModifySOP.Enabled := Table4.Active;
end;

procedure TForm1.ModifyPatientClick(Sender: TObject);
var script, base: string;
begin
  if Sender=ModifyPatient then
  begin
    FormModifyDICOM.Caption := 'Change/copy all images in current Patient';
    script := Table1.Fields[0].AsString + ',,,,'
  end;
  if Sender=ModifyStudy   then
  begin
    FormModifyDICOM.Caption := 'Change/copy all images in current Study';
    script := Table1.Fields[0].AsString + ',' + Table2.Fields[0].AsString + ',,,'
  end;
  if Sender=ModifySeries  then
  begin
    FormModifyDICOM.Caption := 'Change/copy all images in current Series';
    script := Table1.Fields[0].AsString + ',' + Table2.Fields[0].AsString + ',' + Table3.Fields[0].AsString + ',,'
  end;
  if (Sender=ModifySOP) and Table4.Active then
  begin
    FormModifyDICOM.Caption := 'Change/copy current Image';
    script := Table1.Fields[0].AsString + ',' + Table2.Fields[0].AsString + ',' + Table3.Fields[0].AsString + ',' + Table4.Fields[0].AsString + ','
  end
  else if (Sender=ModifySOP) then
  begin
    ShowMessage('Select image to modify first');
    exit;
  end;

  base := script;

  repeat
    script := base;
    if FormModifyDICOM.ShowModal()=mrCancel then break;

    if FormModifyDICOM.RadioButtonReplace.Checked then
      script := script + '1,'
    else
      script := script + '0,';

    if FormModifyDICOM.RadioButtonNewUids.Checked then
      script := script + 'newuids;'
    else if FormModifyDICOM.RadioButtonAnonymize.Checked then
      script := script + 'lua/anonymize_script.lua;';

    ServerTask('modifying data from GUI', 'luastart:servercommand[[modifier:'+script+'modifier.lua'+']]');
    if Sender=ModifyPatient then SetRefreshLevel(1);
    if Sender=ModifyStudy then SetRefreshLevel(2);
    if Sender=ModifySeries then SetRefreshLevel(3);
    if Sender=ModifySOP then SetRefreshLevel(4);
  until not FormModifyDICOM.AltPressed;
end;

procedure TForm1.MoveButtonClick(Sender: TObject);
var RemoteAE, RemoteIP, RemotePort, TargetAE, s, t, level, q, code: AnsiString;
    i, p: integer;
begin
  DeleteFile('move.txt');
  if StopCopying = 0 then
  begin
    StopCopying := 1;
    exit;
  end;

  // do a query to show what would be moved
  QueryButtonClick(self);
   
  // get target AE (IP address and port not used)
  SplitAE(TargetSystem.Items[TargetSystem.ItemIndex], TargetAE, RemoteIP, RemotePort);

  if Length(EditQID.Text)=0 then
    ShowMessage('Warning: some DICOM systems will copy too much if the patient ID is not specified');

  if MessageDlg('Copy selected data (see list) to system: '+TargetAE+' (which may take a while) ?' +
                #13 + '(Note that during this operation the user interface will be inactive)', mtConfirmation,
    [mbYes, mbNo], 0) = mrNo then Exit;

  // get source AE (IP and port used)
  SplitAE(DicomSystem.Items[DicomSystem.ItemIndex],   RemoteAE, RemoteIP, RemotePort);

  ProgressBar3.Visible := true;
  ProgressBar3.Max := 100;
  ProgressBar3.Position := 1;
  p := 1;

  Memo1.Lines.Clear;
  Memo1.Lines.Add('------------ started copying ------------');
  Memo1.Update;

  s := EditQID.Text;
  repeat
    t := s;

    if pos(',', s)>1 then
    begin
      t := copy(s, 1, pos(',', s)-1);
      s := copy(s, pos(',', s)+1, 9999);
    end
    else
      s := '';

    Memo1.Lines.Add(IntToStr(p) + '. Copying for patient: ' + t);

    level := QueryLevel.Items[QueryLevel.ItemIndex];
    i := Pos(' ', level);
    if i>0 then level := Copy(level, 1, i-1);
    if Pos('PatientStudyOnly', QueryLevel.Items[QueryLevel.ItemIndex]) > 0 then
      level := level + 'P';

    if Label14.Caption='Series number:' then
      q := MakeQueryString(level,
        trim(t), EditQName.Text, EditQDate.Text, EditQnum.Text, EditQIM.Text,
        '', '', '', EditQSDesc.Text)
    else
      q := MakeQueryString(level,
        trim(t), EditQName.Text, EditQDate.Text, '', '',
        EditQnum.Text, EditQSDesc.Text, EditQIM.Text, '');

    code :=
    'local ae=[['+RemoteAE+']];' +
    'local target=[['+TargetAE+']];' +
    'local q='+q+';' +
    'local p='+IntToStr(p)+';' +
    'q2=DicomObject:new(); for k,v in pairs(q) do q2[k]=v end;' +
    'local r=dicommove(ae, target, q2, 0, p);' +
    'if r then local f=io.open("move.txt", "a"); f:write(r); f:close(); end;' +
    '';

    ServerTask('','luastart:'+code);
    p := p + 1;
  until length(s)=0;
end;

procedure TForm1.Send1Click(Sender: TObject);
begin
  SendImageTo.Enabled := Table4.Active;
  SendImagesTo.Enabled := Table4.Active;
end;

// only used in mode 5 and 6
procedure TForm1.SendImageToClick(Sender: TObject);
var RemoteIP, RemotePort, TargetAE, RemoteAE: AnsiString;
    Bookmark: TBookmark;
    PatID, StudyUID, SeriesUID, ImageUID, what, q, code: string;
    index, tag, i, count: integer;
begin
  if not Table4.Active then exit;
  if Length(Table4.FieldByName('OBJECTFILE').AsString)=0 then exit;

  Index := (Sender as TMenuItem).GroupIndex; // 1..4 = image..patient
  Tag   := (Sender as TMenuItem).Tag;	     // item for target

  // get target AE (IP address and port not used)
  SplitAE(TargetSystem.Items[Tag], TargetAE, RemoteIP, RemotePort);

  if index=6 then
  begin
    SendSelectedForm.PORT.Caption := RemotePort;
    SendSelectedForm.AE.Caption := TargetAE;
    SendSelectedForm.IP.Caption := RemoteIP;
    PrintSelectedImages1Click(nil);
    exit;
  end;

  case index of
    {1: what := 'image';
    2: what := 'series';
    3: what := 'study';
    4: what := 'patient';
    }
    5: what := 'images';
  end;

  if index=5 then
  begin
    SendSelectedForm.Caption := 'Send selected images to - '+ TargetAE;
    Bookmark := Table4.Bookmark;
    ShowPict := False;
    try
      Table4.First;
      SendSelectedForm.ListBox.Items.Clear;
      while not Table4.EOF do
      begin
        SendSelectedForm.ListBox.Items.Add(Format('%12s'+#09+'%12s'+#09+'%12s'+#09+'%8s',
          [Table4.Fields[2].AsString,
           Table4.Fields[3].AsString,
           Table4.Fields[4].AsString,
           Table4.Fields[5].AsString
          ]));
        Table4.Next;
      end;
      Table4.Bookmark := Bookmark;
    finally
      ShowPict := True;
    end;

    SendSelectedForm.ComboBox1.Hide;
    SendSelectedForm.Label1.Hide;
    if SendSelectedForm.ShowModal = mrCancel then exit;
  end
  else
  begin
    if MessageDlg('Copy selected ' + what + ' to system: '+TargetAE+' (which may take a while) ?' +
                  #13 + '(Note that during this operation the user interface will be inactive)', mtConfirmation,
      [mbYes, mbNo], 0) = mrNo then Exit;
  end;

  // get source AE (IP and port not used)
  RemoteIP   := '127.0.0.1';
  RemotePort := trim(TCPIPport.text);
  RemoteAE   := trim(ServerName.text);

                                 PatID     := Table1.Fields[0].AsString;
  if (Index<4) or (Index>4) then StudyUID  := Table2.Fields[0].AsString;
  if (Index<3) or (Index>4) then SeriesUID := Table3.Fields[0].AsString;
  if (Index<2) or (Index>4) then ImageUID  := Table4.Fields[0].AsString;

  {EditQID.Text   := PatID;
  EditQName.Text := Table1.Fields[1].AsString;
  EditQDate.Text := Table2.Fields[1].AsString;
//  EditQnum.Text  := Table3.Fields[1].AsString;
//  EditQIM.Text   := Table4.Fields[2].AsString;

  if Label14.Caption='Series number:' then
  begin
    EditQNum .text := Table3.Fields[1].AsString;
    EditQIm  .text := Table4.Fields[2].AsString;
    // EditQSDesc.text:= Memo1.SelText;
  end
  else
  begin
    EditQNum .text := StudyUID;
    EditQSDesc.text := SeriesUID;
    EditQIm.text:= ImageUID;
  end;
  }

  Screen.Cursor := crHourGlass;
  StopCopying := 0;
  count := 0;

  if Index=5 then
  try
    for i:=0 to SendSelectedForm.ListBox.Items.Count-1 do
      if SendSelectedForm.ListBox.Selected[i] then inc(count);
    Memo1.Lines.Add('------------ started copying ' + IntToStr(count) + ' slices ------------');

    i        := 0;
    count    := 0;
    ShowPict := false;
    Bookmark := Table4.Bookmark;
    Table4.First;
    while not Table4.EOF do
    begin
      ImageUID  := Table4.Fields[0].AsString;
      if SendSelectedForm.ListBox.Selected[i] then
      begin
        inc(count);
        q := MakeQueryString('IMAGE2', PatID, '', '', '', '', StudyUID, SeriesUID, ImageUID, '');

        code :=
        'local ae=[['+RemoteAE+']];' +
        'local target=[['+TargetAE+']];' +
        'local q='+q+';' +
        'q2=DicomObject:new(); for k,v in pairs(q) do q2[k]=v end;' +
        'local r = dicommove(ae, target, q2, 0, 1);' +
        'if r then local f=io.open("move.txt", "a"); f:write(r); f:close(); end;' +
        '';

        ServerTask('','lua:'+code);
      end;
      Table4.Next;
      inc(i);
    end;
  finally
    Table4.Bookmark := Bookmark;
    ShowPict := true;
    Memo1.Lines.Add('------------ finished copying ------------');
    if fileExists('move.txt') then
    begin
      memo1.Lines.LoadFromFile('move.txt');
      memo1.Update;
      DeleteFile('move.txt');
      ShowMessage(TargetAE + ': ' + memo1.Lines[0]);
    end
    else
      ShowMessage('Sent ' + IntToStr(count) + ' image(s) to ' + TargetAE);
  end;

  Screen.Cursor := crDefault;
  StopCopying := 1;
end;

procedure TForm1.SendPatientToClick(Sender: TObject);
var script, typ, TargetAE, RemoteIP, RemotePort: ansistring;
begin
  if (Sender as TMenuItem).GroupIndex=1 then
  begin
    script := Table1.Fields[0].AsString + ',,,';
    typ := 'patient';
  end;
  if (Sender as TMenuItem).GroupIndex=2 then
  begin
    script := Table1.Fields[0].AsString + ',' + Table2.Fields[0].AsString + ',,';
    typ := 'study';
  end;
  if (Sender as TMenuItem).GroupIndex=3 then
  begin
    script := Table1.Fields[0].AsString + ',' + Table2.Fields[0].AsString + ',' + Table3.Fields[0].AsString + ',';
    typ := 'series';
  end;
  if ((Sender as TMenuItem).GroupIndex=4) and Table4.Active then
  begin
    script := Table1.Fields[0].AsString + ',' + Table2.Fields[0].AsString + ',' + Table3.Fields[0].AsString + ',' + Table4.Fields[0].AsString;
    typ := 'image';
  end
  else if ((Sender as TMenuItem).GroupIndex=4) then
  begin
    ShowMessage('Select image to send first');
    exit;
  end;

  if MessageDlg('Copy selected ' + typ + ' to system: '+TargetAE+' (which may take a while) ?' +
                #13 + '(Note that during this operation the user interface will be inactive)', mtConfirmation,
    [mbYes, mbNo], 0) = mrNo then Exit;

  SplitAE(TargetSystem.Items[(Sender as TMenuItem).Tag], TargetAE, RemoteIP, RemotePort);
  ServerTask('c-move from GUI', 'luastart:servercommand[[move:'+trim(ServerName.text)+','+TargetAE+','+script+']]');
  if ServerName.text = TargetAE then
    SetRefreshLevel((Sender as TMenuItem).GroupIndex);
end;

procedure TForm1.Patient1Click(Sender: TObject);
begin
  SaveDialog1.Title := 'Export patient as zipped file';
  SaveDialog1.DefaultExt := 'zip';
  SaveDialog1.Filter := 'Zip files (*.zip)|*.zip|7z files (*.7z)|*.7z';
  SaveDialog1.FilterIndex := 0;
  SaveDialog1.Filename := Table1.FieldByName('PATIENTID').AsString+'.zip';

  if SaveDialog1.Execute then
     ServerTask('Export from GUI: ', 'luastart:servercommand[[export:'+
     Table1.FieldByName('PATIENTID').AsString+
     ',,,,'+GuiToServerFilename(SaveDialog1.Filename)+',compression UN]]');
end;

procedure TForm1.Study1Click(Sender: TObject);
begin
  SaveDialog1.Title := 'Export study as zipped file';
  SaveDialog1.DefaultExt := 'zip';
  SaveDialog1.Filter := 'Zip files (*.zip)|*.zip|7z files (*.7z)|*.7z';
  SaveDialog1.FilterIndex := 0;
  SaveDialog1.Filename := Table1.FieldByName('PATIENTID').AsString+'.zip';

  if SaveDialog1.Execute then
     ServerTask('Export from GUI: ', 'luastart:servercommand[[export:'+
     Table1.FieldByName('PATIENTID').AsString+','+
     Table2.FieldByName('STUDYINSTA').AsString+
     ',,,'+GuiToServerFilename(SaveDialog1.Filename)+',compression UN]]');
end;

procedure TForm1.Series1Click(Sender: TObject);
begin
  SaveDialog1.Title := 'Export series as zipped file';
  SaveDialog1.DefaultExt := 'zip';
  SaveDialog1.Filter := 'Zip files (*.zip)|*.zip|7z files (*.7z)|*.7z';
  SaveDialog1.FilterIndex := 0;
  SaveDialog1.Filename := Table1.FieldByName('PATIENTID').AsString+'.zip';

  if SaveDialog1.Execute then
     ServerTask('Export from GUI: ', 'luastart:servercommand[[export:'+
     Table1.FieldByName('PATIENTID').AsString+','+
     Table2.FieldByName('STUDYINSTA').AsString+','+
     Table3.FieldByName('SERIESINST').AsString+
     ',,'+GuiToServerFilename(SaveDialog1.Filename)+',compression UN]]');
end;

procedure TForm1.Image1Click(Sender: TObject);
begin
  if not Table4.Active then DBGrid1DblClick(self);
  Dbgrid3.SetFocus;
end;

procedure TForm1.Image1MouseEnter(Sender: TObject);
begin
  if Table4.Active then
    DBGrid3.SetFocus;
end;

procedure TForm1.Image1MouseMove(Sender: TObject; Shift: TShiftState; X,
  Y: Integer);
begin
  if Table4.Active then
  begin
    if SpinEdit1.Visible and (ssLeft in Shift) then
    begin
      if oldx>0 then
        SpinEdit1.Value := round(SpinEdit1.Value+SpinEdit1.MaxValue*(X-oldx) / 512);
      oldx := X;
    end
    else
      oldx := 0;

    if (oldy>0) and (ssLeft in Shift) then
    begin
      if (Y-oldy) > round(512 / Table4.RecordCount) then
      begin
        form1.Table4.Next;
        oldy := Y;
      end
      else if (Y-oldy) < round(-512 / Table4.RecordCount) then
      begin
        form1.Table4.Prior;
        oldy := Y;
      end
    end
    else
      oldy := Y;
  end;
end;

procedure TForm1.Image2Click(Sender: TObject);
begin
  SaveDialog1.Title := 'Export image as zipped file';
  SaveDialog1.DefaultExt := 'zip';
  SaveDialog1.Filter := 'Zip files (*.zip)|*.zip|7z files (*.7z)|*.7z';
  SaveDialog1.FilterIndex := 0;
  SaveDialog1.Filename := Table1.FieldByName('PATIENTID').AsString+'.zip';

  if SaveDialog1.Execute then
     ServerTask('Export from GUI: ', 'export:'+
     Table1.FieldByName('PATIENTID').AsString+','+
     Table2.FieldByName('STUDYINSTA').AsString+','+
     Table3.FieldByName('SERIESINST').AsString+','+
     Table4.FieldByName('SOPINSTANC').AsString+
     ','+GuiToServerFilename(SaveDialog1.Filename)+',compression UN]]');
end;

procedure TForm1.DeleteClick(Sender: TObject);
begin
  DeleteImage.Enabled := Table4.Active;
  Removeimagefromdatabase1.Enabled := Table4.Active;
end;

procedure TForm1.DeleteImageClick(Sender: TObject);
var tmp: AnsiString;
begin
  if not table4.Active then
  begin
    ShowMessage('First select image to delete');
    exit;
  end;
  if MessageDlg('Delete this image ?', mtConfirmation,
    [mbYes, mbNo], 0) = mrNo then Exit;

  tmp := Table1.FieldByName('PATIENTID' ).AsString+':'+
         Table4.FieldByName('SOPINSTANC').AsString;
  ServerTask('Deleting image from GUI', 'deleteimage:'+GuiToServerFilename(tmp));
end;

procedure TForm1.PrintImage1Click(Sender: TObject);
var  code: string;
begin
  if Length(Table4.FieldByName('OBJECTFILE').AsString)=0 then exit;

  Screen.Cursor := crHourGlass;

  code :=
  'local file=[['+LastFile+']];' +
  'local target=[['+trim(ServerName.Text)+']];' +
  'local image=DicomObject:new(); image:Read(file); image:Compress("un");' +
  'local r=dicomprint(image, target);' +
  'if r then local f=io.open("print.txt", "a"); f:write(r); f:close(); end;' +
  '';

  ServerTask('','lua:'+code);
  Screen.Cursor := crDefault;
  {if (rc <> 0) then
        ShowMessage('DICOM print error: ' + DcmErrorMsg(rc));
  }
end;

function MyPrintProgress(percentage: integer; status: PAnsichar): integer; stdcall;
begin
  Screen.Cursor := crHourGlass;
  Application.ProcessMessages;
  result := 0;
end;

procedure TForm1.PrintSelectedImages1Click(Sender: TObject);
var Bookmark: TBookmark;
    FileNames, tmp, s, target, code: string;
    magnum, jukeboxnum, cdnum, cachenum, i, j, r, c: integer;
begin
  if assigned(Sender) then
    SendSelectedForm.Caption := 'Print selected images on local DICOM printer'
  else
    SendSelectedForm.Caption := 'Print selected images on ' +  SendSelectedForm.AE.Caption;
  Bookmark := Table4.Bookmark;
  ShowPict := False;

  try
    Table4.First;
    SendSelectedForm.ListBox.Items.Clear;
    while not Table4.EOF do
    begin
      SendSelectedForm.ListBox.Items.Add(Format('%12s'+#09+'%12s'+#09+'%12s'+#09+'%8s',
        [Table4.Fields[2].AsString,
         Table4.Fields[3].AsString,
         Table4.Fields[4].AsString,
         Table4.Fields[5].AsString
        ]));
      Table4.Next;
    end;
    Table4.Bookmark := Bookmark;
  finally
    ShowPict := True;
  end;

  SendSelectedForm.ComboBox1.Show;
  SendSelectedForm.Label1.Show;
  if  SendSelectedForm.ComboBox1.ItemIndex<0 then
     SendSelectedForm.ComboBox1.ItemIndex := 10;

  if SendSelectedForm.ShowModal = mrCancel then exit;

  Screen.Cursor := crHourGlass;

  try
    j        := 0;
    ShowPict := false;
    Bookmark := Table4.Bookmark;
    Table4.First;
    while not Table4.EOF do
    begin
      if SendSelectedForm.ListBox.Selected[j] then
      begin
        // Translate device name to path; allows up to 10 mag, 10 cache, and
        // 10 jukebox devices and allows unlimited CDs

        tmp := Table4.FieldByName('DEVICENAME').AsString;
        if copy(tmp, 0, 3)='MAG' then
        begin
          magnum := StrToIntDef(copy(tmp,4,1), 0);   // limited to one character
          s := MagDeviceList[magnum] + Table4.FieldByName('OBJECTFILE').AsString;
        end
        else if copy(tmp, 0, 7)='JUKEBOX' then
        begin
          jukeboxnum := StrToIntDef(copy(tmp,8,1), 0);    // limited to one character
          cdnum      := StrToIntDef(copy(tmp,10,99), 0);
          s := Format(JukeboxDeviceList[jukeboxnum], [cdnum]) + Table4.FieldByName('OBJECTFILE').AsString;
          // Fix for faulty %04d format; replace spaces by 0
          for i:=1 to Length(s) do if s[i]=' ' then s[i]:='0';

          // Jukebox image may still reside on cache disk; check it out
          if not FileExists(s) then
          begin
            for cachenum:=0 to CacheDevices-1 do
            begin
              s := Format(CacheDeviceList[cachenum], [jukeboxnum, cdnum]) + Table4.FieldByName('OBJECTFILE').AsString;
              for i:=1 to Length(s) do if s[i]=' ' then s[i]:='0';
              if FileExists(s) then break;
            end;
          end;
        end
        else if copy(tmp, 0, 5)='CACHE' then
        begin
          cachenum   := StrToIntDef(copy(tmp,6,1), 0);   // limited to one character
          jukeboxnum := StrToIntDef(copy(tmp,8,1), 0);   // limited to one character
          cdnum      := StrToIntDef(copy(tmp,10,99), 0);
          s := Format(CacheDeviceList[cachenum], [jukeboxnum, cdnum]) + Table4.FieldByName('OBJECTFILE').AsString;
          // Fix for faulty %04d format; replace spaces by 0
          for i:=1 to Length(s) do if s[i]=' ' then s[i]:='0';
        end;

        FileNames := FileNames + s  + ';'
      end;
      Table4.Next;
      inc(j);
    end;
  finally
    s := SendSelectedForm.ComboBox1.Text;
    if s[2]='x' then 
    begin
      s[2] := ',';
      c := StrToInt(s[1]);
      r := StrToInt(s[3]);
    end
    else
    begin
      s[3] := ',';
      c := StrToInt(copy(s, 1, 2));
      r := StrToInt(copy(s, 4, 2));
    end;
    if (c+PrintSquareLandscape > r) then
      s := 'FilmOrientation=LANDSCAPE;ImageDisplayFormat=STANDARD\' + s
    else
      s := 'FilmOrientation=PORTRAIT;ImageDisplayFormat=STANDARD\' + s;

    if Length(Filenames)<>0 then
    begin
      Screen.Cursor := crHourGlass;

      Table4.Bookmark := Bookmark;
      ShowPict := true;

      if assigned(sender) then
        target := trim(ServerName.Text)
      else
        target := trim(SendSelectedForm.AE.Caption);

      code :=
      'local target=[['+target+']];' +
      'local filelist=[['+FileNames+']];' +
      'local s=[['+s+']];' +
      'local files = {}; ' +
      'filelist:gsub("([^;]*);", function(c) table.insert(files, c) end);'+
      'addo = DicomObject:newarray();'+
      'for k, v in ipairs(files) do '+
      '  local o = DicomObject:new() o:Read(v); o:Compress("un"); addo:Add(o); '+
      'end;'+
      'local r=dicomprint(addo, target, "", s, "99999");' +
      'if r then local f=io.open("print.txt", "a"); f:write(r); f:close(); end;' +
      'addo=nil; collectgarbage(); ' +
      '';
      ServerTask('','luastart:'+code);
      Screen.Cursor := crDefault;

      { FileNames := copy(FileNames, 1, Length(FileNames)-1);
      DcmSetProgressHook(MyPrintProgress);
      if assigned(sender) then
        DcmPrintFile(FileNames, Pchar(trim(ServerName.Text)),
                     Pchar(trim(ServerName.Text)), '127.0.0.1', Pchar(trim(TCPIPport.Text)),
                     '', s)
      else
        DcmPrintFile(FileNames, Pchar(trim(ServerName.Text)),
                     Pchar(trim(SendSelectedForm.AE.Caption)), Pchar(trim(SendSelectedForm.IP.Caption)), Pchar(trim(SendSelectedForm.PORT.Caption)),
                     '', s);
      DcmSetProgressHook(nil);
      }
    end
    else
      ShowMessage('You did not select any images');

    Screen.Cursor := crDefault;
  end;
end;

// double clicking an entry in the list copies that into the right query entry
procedure TForm1.Memo1DblClick(Sender: TObject);
var I, J, Skipchars, line, col, entry: integer;
    InText: boolean;
    EntryName, patid: string;
begin
  if Memo1.Lines.Count=0 then exit;

  // find line and column of entry that was double clicked
  Skipchars := 0;
  col       := 0;
  line      := 0;

  for I := 0 to Memo1.Lines.Count-1 do
  begin
    Skipchars := Skipchars + Length(Memo1.Lines[I]) + 2;
    if SkipChars > Memo1.SelStart then
    begin
      line := I;
      Skipchars := Skipchars - Length(Memo1.Lines[I]) - 2;
      col := Memo1.SelStart - SkipChars + 1;
      break;
    end;
  end;

  // select contents of item (between '[' and ']' and '(' and ')'
  while (col>1) and (Memo1.Lines[line][col] <> '[') {and
                    (Memo1.Lines[line][col] <> '(')} do
  begin
    dec(col);
    Memo1.SelStart := Memo1.SelStart-1;
  end;

  Memo1.SelStart := Memo1.SelStart+1;

  for I:=col to Length(Memo1.Lines[line]) do
    if (Memo1.Lines[line][I] = ']') {or (Memo1.Lines[line][I] = ')')} then
    begin
      Memo1.SelLength := I-col-1;
      break;
    end;

  // find number of the query entry that was highlighted
  entry := -1; J := 0;
  for I:=1 to col do
    if (J=0) and (Memo1.Lines[line][I] = '[') or (Memo1.Lines[line][I] = '(') then
    begin
      inc(entry);
      inc(J);
    end
    else if (J>0) and (Memo1.Lines[line][I] = ']') or (Memo1.Lines[line][I] = ')') then
    begin
      dec(J);
    end;

  // translate that number in a field name (from the first line of the memo)
  J := 1;
  for I:=0 to entry do
  begin
    InText := (Memo1.Lines[0][J] <> ' ');
    SkipChars := J;
    while InText do
    begin
      inc(J);
      Intext := (Memo1.Lines[0][J] <> ' ');
    end;
    EntryName := copy(Memo1.Lines[0], Skipchars, J-Skipchars);
    while not InText do
    begin
      inc(J);
      Intext := (Memo1.Lines[0][J] <> ' ');
    end;
  end;

  patid := EditQID  .text;

  // clear query entries that are nonsense at selected query level
  if Querylevel.ItemIndex in [0, 7]                then EditQID  .text := '';
  if Querylevel.ItemIndex in [0, 7]                then EditQName.text := '';
  if Querylevel.ItemIndex in [0, 1, 4, 7, 8]       then EditQDate.text := '';

  if Label14.Caption='Series number:' then
  begin
    if Querylevel.ItemIndex in [0, 1, 2, 4, 5, 7, 8] then EditQNum .text := '';
    if Querylevel.ItemIndex in [0, 1, 2, 4, 5, 7, 8] then EditQSDesc.text := '';
  end
  else
  begin
    if Querylevel.ItemIndex in [0, 1, 4, 7, 8]       then EditQNum .text := '';
    if Querylevel.ItemIndex in [0, 1, 2, 4, 5, 7, 8] then EditQSDesc.text := '';
  end;

  if true                                          then EditQIm  .text := '';

  // put the double clicked entry into the right edit box
  if      entryname = 'PatientID'    then EditQID  .text := Memo1.SelText
  else if entryname = 'PatientName'  then EditQName.text := Memo1.SelText
  else if entryname = 'StudyDate'    then EditQDate.text := Memo1.SelText;

  if Label14.Caption='Series number:' then
  begin
         if entryname = 'SeriesNumber' then EditQNum .text := Memo1.SelText
    else if entryname = 'ImageNumber'  then EditQIm  .text := Memo1.SelText
    else if entryname = 'SeriesDescription' then EditQSDesc.text:= Memo1.SelText;
  end
  else
  begin
         if entryname = 'StudyInstanceUID'  then EditQNum .text := Memo1.SelText
    else if entryname = 'SeriesInstanceUID' then EditQSDesc.text := Memo1.SelText
    else if entryname = 'SOPInstanceUID'    then EditQIm.text:= Memo1.SelText;
  end;

  if (entryname = 'PatientID') and ((GetAsyncKeyState(VK_CONTROL) and $8000)<>0) then
    if Length(patid) > 0 then
      EditQID  .text := patid + ',' + Memo1.SelText;
end;

procedure TForm1.MenuMergeStudiesClick(Sender: TObject);
var Bookmark: TBookmark;
    FileNames, tmp, s: string;
    magnum, i, j: integer;
    List: TStringList;
    NotMag : boolean;
begin
  if not Table4.Active then exit;

  SendSelectedForm.Caption := 'Merge selected studies';
  Bookmark := Table2.Bookmark;
  ShowPict := False;
  NotMag   := False;

  try
    Table2.First;
    SendSelectedForm.ListBox.Items.Clear;
    while not Table2.EOF do
    begin
      SendSelectedForm.ListBox.Items.Add(Format('%12s'+#09+'%12s'+#09+'%12s'+#09+'%12s'+#09+'%8s',
        [Table2.Fields[1].AsString,
         Table2.Fields[2].AsString,
         Table2.Fields[3].AsString,
         Table2.Fields[4].AsString,
         Table2.Fields[5].AsString
        ]));
      Table2.Next;
    end;
  finally
    ShowPict := True;
  end;

  SendSelectedForm.ComboBox1.Hide;
  SendSelectedForm.Label1.Hide;

  if SendSelectedForm.ShowModal = mrCancel then exit;

  Screen.Cursor := crHourGlass;

  try
    j        := 0;
    i        := 0;
    ShowPict := false;
    Table2.First;
    while not Table2.EOF do
    begin
      if SendSelectedForm.ListBox.Selected[i] then
      begin
        FileNames := FileNames + Table2.FieldByName('STUDYINSTA').AsString  + ',';
        inc(j);
      end;
      Table2.Next;
      inc(i);
    end;
  finally
    ShowPict := true;
  end;

  if j<2 then
  begin
    ShowMessage('Need at least 2 studies to merge');
    Table2.Bookmark := Bookmark;
    Screen.Cursor := crDefault;
    exit;
  end;

  if j>64 then
  begin
    ShowMessage('Maximum merge is 64 studies');
    Table2.Bookmark := Bookmark;
    Screen.Cursor := crDefault;
    exit;
  end;

  if Length(FileNames)>0 then FileNames := copy(FileNames, 1, Length(FileNames)-1);
  ServerTask('Merging studies UID: ' + FileNames, 'luastart:servercommand[[mergestudy:' + FileNames+']]');
  SetRefreshLevel(3);

  List := TStringList.Create;

  try
    j        := 0;
    ShowPict := false;
    Table2.First;
    while not Table2.EOF do
    begin
      if SendSelectedForm.ListBox.Selected[j] then
      begin
//        form1.DBGrid2DblClick(form1.DbGrid2);
        form1.DBText1DblClick(form1.DBText1);
        Table3.First;
        while not Table3.EOF do
        begin
          form1.DBGrid1DblClick(form1.DbGrid1);
          Table4.First;
          while not Table4.EOF do
          begin
            tmp := Table4.FieldByName('DEVICENAME').AsString;
            if copy(tmp, 0, 3)='MAG' then
            begin
              magnum := StrToIntDef(copy(tmp,4,1), 0);   // limited to one character
              s := MagDeviceList[magnum] + Table4.FieldByName('OBJECTFILE').AsString;
              List.Add(s);
            end
            else
              NotMag := True;

            Table4.Next;
          end;
          Table3.Next;
        end;
      end;

      Table2.Next;
      inc(j);
    end;

    Form1.ProgressBar2.Visible := true;
    Form1.ProgressBar2.Max := List.Count;
    Form1.ProgressBar2.Position := 1;

    if NotMag then
    begin
      ShowMessage('Not all images are on MAG storage - cannot merge studies');
    end
    else for i:=0 to List.Count-1 do
    begin
      LabelLister.Visible := false;
      Form1.SpinEdit1.Visible  := false;
      Image1.Picture.Bitmap.SetSize(512, 512);
      Image1.Canvas.Brush.Color := clGray;
      Image1.Canvas.FillRect(Rect(0,0,512,512));
      Image1.Canvas.Font.Height := round(Image1.Picture.Height/15);
      Image1.Canvas.TextOut(round(Image1.Picture.Width /4),
                          round(Image1.Picture.Height/2),
                          'Modifying image ' + IntToStr(i+1) + ' of ' + IntToStr(List.Count));
      Image1.Update;
      Label25.Visible := false;
      ShowOptions := true;

      ServerTask('', 'mergestudyfile:' + List[i]);
      Form1.ProgressBar2.Position := i;
      Application.ProcessMessages;
    end;

  finally
    Screen.Cursor := crDefault;
    ShowPict      := true;
    if Table3.Active then Table3.Bookmark := Bookmark;
    List.Free;
    Form1.ProgressBar2.Visible := false;
  end;
end;

{************************************************************************}
// Archive page

// This routine parses the input string and splits it in an executable
// called 'e' and parameters called 'p'. In the parameter string %s
// is replaced by the cache directory, %d by the jukebox directory and
// %n = CD slot, %c = CD number, %j = jukebox number, %m = MAG0 directory,
// %a = date, %t=time, %l = CD label, %p = UDP port for messages, %%=%
// e.g., 'xcopy.exe /s/e %s %d' copies the data from cache to jukebox
// The executable should have exit code 0 for success and <>0 for failure.

procedure SubstituteBurnParams(t: string; var e, p: string);
var SrcPath, DstPath, MagPath, CDLabel: string;
    i: integer;
    ine, inp: boolean;
begin
  // cache directory with number substituted without trailing backslash
  SrcPath := Format(CacheDeviceList[0], [ActiveJukebox, NextCDToBurn]);
  for i:=1 to Length(SrcPath) do if SrcPath[i]=' ' then SrcPath[i]:='0';
  SrcPath := Copy(SrcPath, 1, Length(SrcPath)-1);

  // jukebox directory with number substituted without trailing backslash
  DstPath := Format(JukeBoxDeviceList[ActiveJukebox], [NextCDToBurn]);
  for i:=1 to Length(DstPath) do if DstPath[i]=' ' then DstPath[i]:='0';
  DstPath := Copy(DstPath, 1, Length(DstPath)-1);

  // MAG0 directory without trailing backslash
  MagPath := MagDeviceList[0];
  MagPath := Copy(MagPath, 1, Length(MagPath)-1);

  // Get CD label from destination path
  for i:=Length(DstPath) downto 1 do
  if DstPath[i] = '\' then
  begin
    CDLabel := Copy(DstPath, i+1, 99);
    break;
  end;

  // State machine to split burn string in exectable and parameters

  ine := true;  // in executable
  inp := false; // in substitutable parameter, e.g., %s
  e := '';
  p := '';

  for i:=1 to Length(t) do
  begin
    // copy first text up to space to executable name
    if ine then
    begin
      if t[i] = ' ' then
        ine := False
      else
        e := e + t[i];
    end

    // copy rest to parameter text and substitute parameters
    else if not inp then
    begin
      if t[i] = '%' then
        inp := true
      else
        p := p + t[i];
    end
    else
    begin
      if t[i] = 's' then p := p + SrcPath;
      if t[i] = 'd' then p := p + DstPath;
      if t[i] = 'm' then p := p + MagPath;
      if t[i] = 'n' then p := p + IntToStr(NextSlotToUse);
      if t[i] = 'c' then p := p + IntToStr(NextCDToBurn);
      if t[i] = 'j' then p := p + IntToStr(ActiveJukebox);
      if t[i] = 'a' then p := p + FormatDateTime('yyyymmdd', Date);
      if t[i] = 't' then p := p + FormatDateTime('hhmmss', Time);
      if t[i] = 'l' then p := p + CDLabel;
      if t[i] = 'p' then p := p + ArchiveSocket.Port;
      if t[i] = '%' then p := '%';
      inp := false;
    end;
  end;
end;

procedure TForm1.ArchiveSocketDataAvailable(Sender: TObject; Error: Word);
  var buffer: array[0..8192] of Ansichar;
  s : string;
  i: integer;
begin
  i := ArchiveSocket.Receive(@buffer, 8191);
  if i<1 then exit;

  buffer[i] := #00;
  if buffer[i-1] in [#10, #13] then buffer[i-1] := #00;
  s := buffer;
  // s := copy(s, 0, length(s)-1);
  for i:=1 to length(s) do if s[i]<' ' then s[i]:=' ';

  WriteMemo(ArchiveMemo, DateToStr(Date) + ' ' + TimeToStr(Time) + ' ' + s, 200, 100, 'archive', false);
end;

// Select data to fill one CD

procedure TForm1.SelectForArchivingButtonClick(Sender: TObject);
var i: integer;
    s: string;
begin
  // select MAG0 to archive if only a single one present
  s := '';
  if MagDevices=1 then s := ',0';

  ArchiveStatus := '!Selecting patients';
  WriteMemo(ArchiveMemo, 'manually selecting patients', 100, 200, 'archive');
  TabSheet8.Enabled := false;

  i := RunProgramWaiting(dgateExe, curdir,
       ['-!'+ArchiveSocket.Port + ' -as' + IntToStr(CDCapacityMB*1000) + s]);
  if (i<>0) then
  begin
    ArchiveStatus := '>Failed selecting patients';
    WriteMemo(ArchiveMemo, 'failed manually selecting patients', 100, 200, 'archive');
  end
  else
  begin
    ArchiveStatus := '&Ready selecting patients';
    WriteMemo(ArchiveMemo, 'finished manually selecting patients', 100, 200, 'archive');
    CheckBoxSelectForArchiving.Checked := False;
  end;

  TabSheet8.Enabled := true;
end;

// Verify mirror disk

procedure TForm1.VerifyMirrorDiskClick(Sender: TObject);
var i: integer;
begin
  ArchiveStatus := '!Verifying mirror disk';
  WriteMemo(ArchiveMemo, 'manually verifying mirror disk', 100, 200, 'archive');
  TabSheet8.Enabled := false;

  i := RunProgramWaiting(dgateExe, curdir,
       ['-!'+ArchiveSocket.Port + ' -avMAG0.Archiving']);
  if (i<>0) then
  begin
    ArchiveStatus := '>Failed verify mirror disk';
    WriteMemo(ArchiveMemo, 'failed verifying mirror disk', 100, 200, 'archive');
  end
  else
  begin
    ArchiveStatus := '&Ready verify mirror disk';
    WriteMemo(ArchiveMemo, 'ready verifying mirror disk', 100, 200, 'archive');
    CheckBoxVerifyMirrorDisk.Checked := False;
  end;

  TabSheet8.Enabled := true;
end;

// Undo selecting data

procedure TForm1.UndoArchivingButtonClick(Sender: TObject);
var i: integer;
begin
  ArchiveStatus := '!Undoing select patients';
  WriteMemo(ArchiveMemo, 'manually undoing select patients', 100, 200, 'archive');
  TabSheet8.Enabled := false;

  i := RunProgramWaiting(dgateExe, curdir, ['-!'+ArchiveSocket.Port + ' -au']);
  if (i<>0) then
  begin
    ArchiveStatus := '>Failed undoing select patients';
    WriteMemo(ArchiveMemo, 'failed manually undoing select patients', 100, 200, 'archive');
  end
  else
  begin
    ArchiveStatus := '&Ready undoing select patients';
    WriteMemo(ArchiveMemo, 'finished manually undoing select patients', 100, 200, 'archive');
  end;

  TabSheet8.Enabled := true;
end;

// Make file set suitable for burning a CD

procedure TForm1.CreateFileSetForBurningButtonClick(Sender: TObject);
var i: integer;
    s: string;
begin
  // select MAG0 to archive if only a single one present
  s := '';
  if MagDevices=1 then s := ',0';

  ArchiveStatus := '!Creating file set for burning';
  WriteMemo(ArchiveMemo, 'manually creating file set for burning', 100, 200, 'archive');
  TabSheet8.Enabled := false;

  i := RunProgramWaiting(dgateExe, curdir,
       ['-!'+ArchiveSocket.Port + ' -abJUKEBOX' + IntToStr(ActiveJukebox)
                                      + '.' + IntToStr(NextCDToBurn) + s]);
  if (i<>0) then
  begin
    ArchiveStatus := '>Failed creating file set for burning';
    WriteMemo(ArchiveMemo, 'failed manually creating file set for burning', 100, 200, 'archive');
  end
  else
  begin
    ArchiveStatus := '&Ready creating file set for burning';
    WriteMemo(ArchiveMemo, 'finished manually creating file set for burning', 100, 200, 'archive');
    CheckBoxCreateFileSet.Checked := false;
  end;

  TabSheet8.Enabled := true;
end;

// Request a remote or this ConquestDicomserver to do a tape backup

function TapeBackupCD: boolean;
var t, SrcPath: string;
    f: textfile;
    i: integer;
begin
  result := false;

  ArchiveStatus := '!Tape backup for CD';
  WriteMemo(Form1.ArchiveMemo, 'Performing tape backup for CD', 100, 200, 'archive');

  SrcPath := Format(CacheDeviceList[0], [ActiveJukebox, NextCDToBurn]);
  for i:=1 to Length(SrcPath) do if SrcPath[i]=' ' then SrcPath[i]:='0';
  SrcPath := Copy(SrcPath, 1, Length(SrcPath)-1);

  if not SysUtils.DirectoryExists(SrcPath) then
  begin
    ArchiveStatus := '>could not find file set directory';
    WriteMemo(Form1.ArchiveMemo, 'tape backup failed: could not find file set directory ' + SrcPath, 100, 200, 'archive');
    exit;
  end;

  if Length(TapeDriveLetter) =1 then SrcPath[1] := TapeDriveLetter[1];

  try
    DeleteFile(TapeControl + '\tapebackup.response');
    AssignFile(f, TapeControl + '\tapebackup.control');
    Rewrite(f);
    WriteLn(f, SrcPath);
    CloseFile(f);
  except
    ArchiveStatus := '>could not control remote tape drive';
    WriteMemo(Form1.ArchiveMemo, 'tape backup failed: could not control remote tape drive', 100, 200, 'archive');
    exit;
  end;

  for i:=1 to 100 do
  begin
    Sleep(100);
    Application.ProcessMessages;
    try
      if FileExists(TapeControl + '\tapebackup.response') then break;
    except
    end;

    if i=100 then
    begin
      ArchiveStatus := '>The remote tape device did not respond to request';
      WriteMemo(Form1.ArchiveMemo, 'tape backup failed: The remote tape device did not respond to request', 100, 200, 'archive');
      exit;
    end;
  end;

  i := 0;

  while true do
  begin
    Application.ProcessMessages;
    Sleep(100);
    inc(i);

    if i > 5 * 3600 * 10 then
    begin
      ArchiveStatus := '>The tape backup did not finish within 5 hours';
      WriteMemo(Form1.ArchiveMemo, 'tape backup failed: the tape backup did not finish within 5 hours', 100, 200, 'archive');
      exit;
    end;

    try
      AssignFile(f, TapeControl + '\tapebackup.response');
      Reset(f);
      ReadLn(f, t);
      CloseFile(f);

      if Pos('BUSY', t) >0 then continue
      else if Pos('OK', t) >0 then
      begin
        result := true;
        WriteMemo(Form1.ArchiveMemo, 'Finished tape backup for CD', 100, 200, 'archive');
        ArchiveStatus := '&Finished tape backup for CD';
        break;
      end
      else
      begin
        WriteMemo(Form1.ArchiveMemo, 'Failed tape backup for CD', 100, 200, 'archive');
        ArchiveStatus := '>Failed tape backup for CD';
        break;
      end;
    except
    end;
  end;
end;

// Call upon DICOM server to make a tape backup

procedure TForm1.BackupToTapeButtonClick(Sender: TObject);
begin
  TabSheet8.Enabled := false;

  if Length(TapeControl) > 0 then
  begin
    if not TapeBackupCD then
    begin
      ArchiveStatus := '>Auto archive: Failed tape backup CD';
      WriteMemo(ArchiveMemo, copy(ArchiveStatus, 1, 200), 100, 200, 'archive');
      TabSheet8.Enabled := true;
      exit;
    end;
  end;

  TabSheet8.Enabled := true;
  CheckBoxTapeBackup.Checked := false;
end;

// Call external program(s) to burn a CD and make a tape backup

procedure TForm1.BurnCDButtonClick(Sender: TObject);
var i, j: integer;
    t, e, p: string;
begin
  TabSheet8.Enabled := false;

  ArchiveStatus := '@Burning CD';
  WriteMemo(ArchiveMemo, 'manually burning CD', 100, 200, 'archive');

  // Supports only a single cache device, since selected cache device is not returned

  for i:=-1 to 9 do
  begin
    if i=-1 then t := BurnProgram
    else         t := BurnProgramList[i];
    if Length(t)=0 then continue;

    SubstituteBurnParams(t, e, p);

    j := RunProgramWaiting(e, curdir, [p]);
    if (j<>0) then
    begin
      ArchiveStatus := '>Failed burning CD';
      TabSheet8.Enabled := true;
      WriteMemo(ArchiveMemo, 'failed manually burning CD', 100, 200, 'archive');
      exit;
    end;
  end;

  ArchiveStatus := '&Ready burning CD';
  WriteMemo(ArchiveMemo, 'finished manually burning CD', 100, 200, 'archive');
  TabSheet8.Enabled := true;
  CheckBoxBurnCD.Checked := false;
end;

// Verify CD utility function

procedure TForm1.VerifyCDButtonClick(Sender: TObject);
var i: integer;
begin
  ArchiveStatus := '!Verifying CD';
  WriteMemo(ArchiveMemo, 'manually verifying CD', 100, 200, 'archive');
  TabSheet8.Enabled := false;

  i := RunProgramWaiting(dgateExe, curdir,
       ['-!'+ArchiveSocket.Port + ' -acJUKEBOX' + IntToStr(ActiveJukebox) + '.' + IntToStr(NextCDToBurn)]);
  if (i<>0) then begin
    ArchiveStatus := '>Failed verifying CD';
    WriteMemo(ArchiveMemo, 'failed manually verifying CD', 100, 200, 'archive');
  end
  else
  begin
    ArchiveStatus := '&Ready verifying CD';
    WriteMemo(ArchiveMemo, 'finished manually verifying CD', 100, 200, 'archive');
  end;

  TabSheet8.Enabled := true;
end;

// Update NextCDToBurn and NextSlotToUse variables in jukebox.ini

procedure IncrementCDNumber;
var f: TextFile;
    i: integer;
begin
  AssignFile(f, //ExtractFileDir(ParamStr(0))
    CurDir + '\jukebox.ini');
  Rewrite(f);

  inc(NextCDToBurn);
  inc(NextSlotToUse);

  if NextSlotToUse >= NumberCDs then
  begin
    dec(NextCDToBurn);
    dec(NextSlotToUse);
    ArchiveStatus := '>Failed: The jukebox is full !';
    WriteMemo(Form1.ArchiveMemo, 'Failed: The jukebox is full !', 100, 200, 'archive');
    Application.MainForm.Caption := Application.MainForm.Caption + ' The jukebox is full !';
    Form1.TabSheet8.Enabled := False;
  end;

  Form1.NextCDToBurnLabel.Caption := 'NextCDToBurn = ' + IntToStr(NextCDToBurn);
  Form1.NextSlotToUseLabel.Caption := 'NextSlotToUse = ' + IntToStr(NextSlotToUse);

  writeln(f, '# This file contains configuration information for auto archival for the DICOM server');
  writeln(f, '');

  writeln(f, 'ActiveJukebox    = ' + IntToStr(ActiveJukebox));
  writeln(f, 'NumberCDs        = ' + IntToStr(NumberCDs));
  writeln(f, 'NextCDToBurn     = ' + IntToStr(NextCDToBurn));
  writeln(f, 'NextSlotToUse    = ' + IntToStr(NextSlotToUse));
  writeln(f, 'CDCapacityMB     = ' + IntToStr(CDCapacityMB));
  writeln(f, 'BurnThresholdMB  = ' + IntToStr(BurnThreshold));
  writeln(f, 'BurnTime         = ' + BurnTime);
  writeln(f, 'BurnTime2        = ' + BurnTime2);
  writeln(f, 'BurnWeekend      = ' + IntToStr(BurnWeekend));
  writeln(f, 'MirrorVerifyTime = ' + MirrorVerifyTime);
  writeln(f, 'TestReadTime     = ' + TestReadTime);
  writeln(f, 'TapeControl      = ' + TapeControl);
  writeln(f, 'TapeDriveLetter  = ' + TapeDriveLetter);
  writeln(f, '');

  writeln(f, '# In BurnProgram use program name with extension; e.g., TEST.EXE parameters');
  writeln(f, '# In parameters: %s=src %d=dst %l=label %n=CDSlot, %c=CDNum, %j=JukeboxNum, %m=MAG0, %a=date, %t=time, %p=udpport');
  writeln(f, '# It is possible to run multiple programs as well (BurnProgram0..9)');
  writeln(f, '');
  writeln(f, 'BurnProgram     = ' + BurnProgram);
  for i:=0 to 9 do if Length(BurnProgramList[i])<>0 then
    writeln(f, 'BurnProgram' + IntToStr(i) + '    = ' + BurnProgramList[i]);

  CloseFile(f);

  WriteMemo(Form1.ArchiveMemo, 'Next CD to use  : ' + IntToStr(NextCDToBurn), 100, 200, 'archive');
  WriteMemo(Form1.ArchiveMemo, 'Next slot to use: ' + IntToStr(NextSlotToUse), 100, 200, 'archive');
end;

// Verify CD, delete source data and increment CD number if OK

procedure TForm1.VerifyAndDeleteButtonClick(Sender: TObject);
var i: integer;
begin
  ArchiveStatus := '!Verify CD/cleanup';
  WriteMemo(ArchiveMemo, 'manually verify CD/cleanup', 100, 200, 'archive');
  TabSheet8.Enabled := false;

  i := RunProgramWaiting(dgateExe, curdir,
       ['-!'+ArchiveSocket.Port + ' -adJUKEBOX' + IntToStr(ActiveJukebox) + '.' + IntToStr(NextCDToBurn)]);
  if (i<>0) then
  begin
    ArchiveStatus := '>Failed verify CD/cleanup';
    WriteMemo(ArchiveMemo, 'failed manually verify CD/cleanup', 100, 200, 'archive');
  end
  else
  begin
    IncrementCDNumber;
    ArchiveStatus := '&Ready verify CD/cleanup';
    WriteMemo(ArchiveMemo, 'finished manually verify CD/cleanup', 100, 200, 'archive');

    // arm all options for next timed operation
    CheckBoxSelectForArchiving.Checked       := true;
    CheckBoxVerifyMirrorDisk.Checked         := true;
    CheckBoxCreateFileSet.Checked            := true;
    CheckBoxTapeBackup.Checked               := true;
    CheckBoxBurnCD.Checked                   := true;
    CheckBoxVerifyCDAndDeleteFileSet.Checked := true;
  end;

  TabSheet8.Enabled := true;
end;

// This timer starts automatic archiving at a given time if diskspace is low
// The timer runs 3 times per second

var TimerTriggered: boolean;
var MirrorTriggered: boolean;
var TestReadTriggered: boolean;
var ZipTriggered: boolean;
var CleanTriggered: boolean;
var MoveTriggered: boolean;
var ForceArchival: boolean;

procedure TForm1.Timer2Timer(Sender: TObject);
var i, j, mb: integer;
    t, e, p, st, status, logfolder, ZipFilename: string;
    H, M, S, MS: WORD;
    f: textfile;
    sr: TSearchRec;
    shouldzip : boolean;
begin
  Timer2.Tag := Timer2.Tag + 1;

  if PageControl1.ActivePage = TabSheet7 then
  begin
    if fileExists('move.txt') then
    begin
      memo1.Lines.LoadFromFile('move.txt');
      memo1.Update;
    end;
  end;

  // select MAG0 to archive if only a single one present
  st := '';
  if MagDevices=1 then st := ',0';

  DecodeTime(Time, H, M, S, MS);
  t := format('%02d:%02d:%02d', [H, M, S]);
  for i:=1 to 8 do if t[i]=' ' then t[i]:='0';

  // color the status label
  if Length(Archivestatus)>0 then
  begin
    if Archivestatus[1] in ['!', '&', '>', '@'] then
    begin
      status := Copy(Archivestatus, 2, 255);
      case Archivestatus[1] of
        '!' : if Odd(Timer2.Tag) then StaticTextTime.Color := clYellow else StaticTextTime.Color := clltGray;
        '@' : if Odd(Timer2.Tag) then StaticTextTime.Color := clPurple else StaticTextTime.Color := clltGray;
        '>' : StaticTextTime.Color := clRed;
        '&' : StaticTextTime.Color := clLime;
      end;
    end
    else
    begin
      status := Archivestatus;
      StaticTextTime.Color := clltGray;
    end;
  end;

  StaticTextTime.Caption := t + ' - ' + status;
  StaticTextTime.Hint := status;

  // allow external program to query status of archive operation
  try
    if FileExists(//ExtractFileDir(ParamStr(0))
      CurDir +'\ConquestDICOMServer.Ping') then
    begin
      DeleteFile(//ExtractFileDir(ParamStr(0))
        CurDir +'\ConquestDICOMServer.Ping');
      AssignFile(f, //ExtractFileDir(ParamStr(0))
        CurDir +'\ConquestDICOMServer.Response');
      Rewrite(f);
      Writeln(f, status);
      CloseFile(f);
    end;
  except
  end;

  // automatically cleanup disk early in the morning
  if (copy(t, 1, length('01:00'))='01:00') and (StrToInt(trim(NightlyTreshold.text))<>0) and not CheckBoxOnlyLogToFile.Checked then
  begin
    if not CleanTriggered then
    begin
      CleanTriggered := true;

      WriteMemo(MaintenanceMemo, 'Making space at night by deleting LRU patients', 100, 200, 'maintenance');

      i := RunProgramWaiting(dgateExe, curdir,
           ['-!'+MaintenanceSocket.Port + ' -ff' + trim(NightlyTreshold.text)]);
      if (i<>0) then
      begin
        WriteMemo(MaintenanceMemo, 'failed making space', 100, 200, 'maintenance');
        exit;
      end
      else
      begin
        WriteMemo(MaintenanceMemo, 'finished making space', 100, 200, 'maintenance');
      end;
    end
  end
  else
    CleanTriggered := false;

  // automatically move data early in the morning
  // original if (copy(t, 1, length('02:00'))='02:00')
  // Luiz added at Sept 19 2021
  if (copy(t, 1, length(NightlyStrTimeToMove))=NightlyStrTimeToMove)
      and (StrToIntDef(trim(EditNightlyMoveTreshold.text), 0)<>0)
      and (NightlyStrTimeToMove<>'')
      and ComboBoxMoveTarget.Enabled
      and (ComboBoxMoveTarget.ItemIndex>0)
      or (DoNightlyToMoveNow and (ComboBoxMoveTarget.ItemIndex>0))
      and (DiskFreeGB(Label10.Caption[1])*1024 < StrToIntDef(trim(EditNightlyMoveTreshold.text), 0)) then
  begin
    if not MoveTriggered then
    begin
      MoveTriggered := true;
      btnDoItNow.Enabled := false;

      mb := StrToIntDef(trim(EditNightlyMoveTreshold.text), 0) - DiskFreeGB(Label10.Caption[1]) * 1024;

      WriteMemo(MaintenanceMemo, 'Making space at night by moving LRU patients to: ' + ComboBoxMoveTarget.Text, 100, 200, 'maintenance');
      WriteMemo(MaintenanceMemo, 'Amount of MB to move: ' + IntToStr(mb), 100, 200, 'maintenance');

      i := RunProgramWaiting(dgateExe, curdir,
           ['-!'+MaintenanceSocket.Port + ' -as' + IntToStr(mb*1024) + ',0']);
      if (i<>0) then
        WriteMemo(MaintenanceMemo, 'failed selecting patients for nightly moving', 100, 200, 'maintenance');

      i := RunProgramWaiting(dgateExe, curdir,
           ['-!'+MaintenanceSocket.Port + ' -amMAG0.Archiving,' + ComboBoxMoveTarget.Text]);
        if (i<>0) then
      begin
        WriteMemo(MaintenanceMemo, 'failed nightly moving', 100, 200, 'maintenance');
        exit;
      end;

      WriteMemo(MaintenanceMemo, 'finished nightly moving', 100, 200, 'maintenance');
      btnDoItNow.Enabled := true;
    end
  end
  else
    MoveTriggered := false;

  DoNightlyToMoveNow:= false;

  // automatically zip the log files early in the morning
  // done by dgate if directly logging to file or service
  if (copy(t, 1, length(ZipTime))=ZipTime) and not CheckBoxOnlyLogToFile.Checked then
  begin
    if not ZipTriggered then
    begin
      ZipTriggered := true;
      logfolder := curdir;
      if SysUtils.DirectoryExists(curdir + '\logs') then logfolder := curdir + '\logs';

      if StrToIntDef(copy(ZipTime, 1, 2), 0)<8 then
        ZipFileName := logfolder + '\logs_' + FormatDateTime('yyyymmdd', date-1) + '.zip'	// zip for previous day
      else
        ZipFileName := logfolder + '\logs_' + FormatDateTime('yyyymmdd', date) + '.zip';	// zip for this day

      // do not overwrite zip file
      if not FileExists(ZipFileName) then
      begin

        // was there some server activity making it worth to zip the log files ?
        shouldzip := FindFirst(curdir + '\serverstatus.log', faAnyFile, sr) = 0;
        FindClose(sr);

        if (shouldzip) then
        begin
          try
            glHoldFlush := false;
            FlushButtonClick(self);
            glHoldFlush := true;

            RunProgramBlocking(curdir + '\7za.exe', curdir, ['-tzip', '-mx=9', 'a', ZipFileName, '*.log *.ini *.sql *.map *.lst']);

            RunProgramBlocking(curdir + '\7za.exe', curdir, ['-tzip', '-mx=9', 'a', '-r', '-sdel', '-y', ZipFileName, 'DicomAnonymized_Log\*.*']);

            // and delete them if possible
            if FindFirst(curdir + '\*.log', faAnyFile, sr) = 0 then
            begin
              DeleteFile(curdir + '\' + sr.Name);
              while FindNext(sr) = 0 do DeleteFile(curdir + '\' + sr.Name);
            end;
            FindClose(sr);

            ServerStatusMemo.Lines.Add('Zipped todays log files as: ' + ZipFileName);
          finally
             glHoldFlush := false;
          end;
        end;
      end;
    end;
  end
  else
    ZipTriggered := false;

  if not TabSheet8.Enabled then exit;

  // test if current time is equal to mirrorverifytime
  if (copy(t, 1, length(MirrorVerifyTime))=MirrorVerifyTime) then
  begin
    if not MirrorTriggered then
    begin
      MirrorTriggered := true;

      ArchiveStatus := '!Maintenance: Verifying mirror disk';
      WriteMemo(ArchiveMemo, 'verifying mirror disk for MAG0', 100, 200, 'archive');

      i := RunProgramWaiting(dgateExe, curdir,
           ['-!'+ArchiveSocket.Port + ' -avMAG0']);
      if (i<>0) then
      begin
        TabSheet8.Enabled := true;
        ArchiveStatus := '>Maintenance: Failed verify mirror disk';
        WriteMemo(ArchiveMemo, 'failed verifying mirror disk for MAG0', 100, 200, 'archive');
        exit;
      end
      else
      begin
        ArchiveStatus := '&Maintenance: finished verify mirror disk';
        WriteMemo(ArchiveMemo, 'finished verifying mirror disk for MAG0', 100, 200, 'archive');
      end;
    end;
  end
  else
    MirrorTriggered := false;

  // test if current time is equal to testreadtime
  if (copy(t, 1, length(TestReadTime))=TestReadTime) then
  begin
    if not TestReadTriggered then
    begin
      TestReadTriggered := true;

      ArchiveStatus := '!Maintenance: Test reading MAG0 disk';
      WriteMemo(ArchiveMemo, 'Test reading MAG0', 100, 200, 'archive');

      i := RunProgramWaiting(dgateExe, curdir,
           ['-!'+ArchiveSocket.Port + ' -atMAG0']);
      if (i<>0) then
      begin
        TabSheet8.Enabled := true;
        ArchiveStatus := '>Maintenance: Test read of MAG0 failed';
        WriteMemo(ArchiveMemo, 'failed test read MAG0', 100, 200, 'archive');
        exit;
      end
      else
      begin
        ArchiveStatus := '&Maintenance: finished test read of MAG0';
        WriteMemo(ArchiveMemo, 'finished test read MAG0', 100, 200, 'archive');
      end;
    end;
  end
  else
    TestReadTriggered := false;

  // test if current time is unequal to start time: exit or forced archival
  if not (copy(t, 1, length(BurnTime))=BurnTime) then
    if not (copy(t, 1, length(BurnTime2))=BurnTime2) then
      if (BurnWeekend=0) or (DayOfWeek(Now) in [2,3,4,5,6]) then
  begin
    TimerTriggered := false;
    if not ForceArchival then exit;
  end;

  // do not trigger more than once on selected time
  if TimerTriggered then exit;
  TimerTriggered := true;

  if MagDeviceList[0][1]='\' then
  begin
    ArchiveStatus := '>Cannot archive from UNC path';
    WriteMemo(ArchiveMemo, 'Cannot archive from UNC path: failed', 100, 200, 'archive');
    exit;
  end;

  if (DiskFreeGB(Label10.Caption[1])*1024 > BurnThreshold) and not ForceArchival then
  begin
    ArchiveStatus := '&Enough space on drive: ' +
                     IntToStr(DiskFreeGB(Label10.Caption[1])) + ' GB';
    WriteMemo(ArchiveMemo, copy(ArchiveStatus, 1, 200), 100, 200, 'archive');
    exit;
  end;

  ForceArchival  := false;
  TabSheet8.Enabled := false;

  if CheckBoxSelectForArchiving.Checked then
  begin
    ArchiveStatus := '!Auto archive: Selecting patients';
    WriteMemo(ArchiveMemo, copy(ArchiveStatus, 1, 200), 100, 200, 'archive');

    i := RunProgramWaiting(dgateExe, curdir,
         ['-!'+ArchiveSocket.Port + ' -as' + IntToStr(CDCapacityMB*1000) + st]);
    if (i<>0) then
    begin
      TabSheet8.Enabled := true;
      ArchiveStatus := '>Auto archive: Failed selecting patients';
      WriteMemo(ArchiveMemo, copy(ArchiveStatus, 1, 200), 100, 200, 'archive');
      exit;
    end;

    CheckBoxSelectForArchiving.Checked := False;
  end;

  if (MirrorDevices>0) and CheckBoxVerifyMirrorDisk.Checked then
  begin
    ArchiveStatus := '!Auto archive: Verifying mirror disk';
    WriteMemo(ArchiveMemo, copy(ArchiveStatus, 1, 200), 100, 200, 'archive');
    i := RunProgramWaiting(dgateExe, curdir,
         ['-!'+ArchiveSocket.Port + ' -avMAG0.Archiving']);
    if (i<>0) then
    begin
      TabSheet8.Enabled := true;
      ArchiveStatus := '>Auto archive: Failed verify mirror disk';
      WriteMemo(ArchiveMemo, copy(ArchiveStatus, 1, 200), 100, 200, 'archive');
      exit;
    end;

    CheckBoxVerifyMirrorDisk.Checked := False;
  end;

  if CheckBoxCreateFileSet.Checked then
  begin
    ArchiveStatus := '!Auto archive: Creating file set';
    WriteMemo(ArchiveMemo, copy(ArchiveStatus, 1, 200), 100, 200, 'archive');

    i := RunProgramWaiting(dgateExe, curdir,
         ['-!'+ArchiveSocket.Port + ' -abJUKEBOX' + IntToStr(ActiveJukebox) + '.'
                                               + IntToStr(NextCDToBurn) + st]);
    if (i<>0) then
    begin
      TabSheet8.Enabled := true;
      ArchiveStatus := '>Auto archive: Failed creating file set';
      WriteMemo(ArchiveMemo, copy(ArchiveStatus, 1, 200), 100, 200, 'archive');
      exit;
    end;

    CheckBoxCreateFileSet.Checked := false;
  end;

  if (Length(TapeControl)>0) and CheckBoxTapeBackup.Checked then
  begin
    if not TapeBackupCD then
    begin
      ArchiveStatus := '>Auto archive: Failed tape backup CD';
      WriteMemo(ArchiveMemo, copy(ArchiveStatus, 1, 200), 100, 200, 'archive');
      TabSheet8.Enabled := true;
      exit;
    end;

    CheckBoxTapeBackup.Checked := false;
  end;

  if CheckBoxBurnCD.Checked then
  begin
    ArchiveStatus := '@Auto archive: Burning CD';
    WriteMemo(ArchiveMemo, copy(ArchiveStatus, 1, 200), 100, 200, 'archive');

    for i:=-1 to 9 do
    begin
      if i=-1 then t := BurnProgram
      else         t := BurnProgramList[i];
      if Length(t)=0 then continue;

      SubstituteBurnParams(t, e, p);

      WriteMemo(ArchiveMemo, e, 100, 200, 'archive');
      j := RunProgramWaiting(e, curdir, [p]);
      if (j<>0) then
      begin
        ArchiveStatus := '>Auto archive: Failed burning CD';
        WriteMemo(ArchiveMemo, copy(ArchiveStatus, 1, 200), 100, 200, 'archive');
        TabSheet8.Enabled := true;
        exit;
      end;
    end;

    CheckBoxBurnCD.Checked := false;
  end;

  if CheckBoxVerifyCDAndDeleteFileSet.Checked then
  begin
    ArchiveStatus := '!Auto archive: Verify/cleanup';
    WriteMemo(ArchiveMemo, copy(ArchiveStatus, 1, 200), 100, 200, 'archive');

    i := RunProgramWaiting(dgateExe, curdir,
         ['-!'+ArchiveSocket.Port + ' -adJUKEBOX' + IntToStr(ActiveJukebox) + '.' + IntToStr(NextCDToBurn)]);
    if (i<>0) then
    begin
      ArchiveStatus := '>Auto archive: Failed verify/cleanup';
      WriteMemo(ArchiveMemo, copy(ArchiveStatus, 1, 200), 100, 200, 'archive');
      TabSheet8.Enabled := true;
      exit;
    end;

    IncrementCDNumber;

    ArchiveStatus := '&Auto archive: Finished CD ' + IntToStr(NextCDToBurn-1);
    WriteMemo(ArchiveMemo, copy(ArchiveStatus, 1, 200), 100, 200, 'archive');

    // arm all options for next timed operation
    CheckBoxSelectForArchiving.Checked       := true;
    CheckBoxVerifyMirrorDisk.Checked         := true;
    CheckBoxCreateFileSet.Checked            := true;
    CheckBoxTapeBackup.Checked               := true;
    CheckBoxBurnCD.Checked                   := true;
    CheckBoxVerifyCDAndDeleteFileSet.Checked := true;
  end;

  TabSheet8.Enabled := true;
end;

procedure TForm1.Label27DblClick(Sender: TObject);
begin
  if (GetAsyncKeyState(VK_CONTROL) and $8000)=0 then exit;
  ForceArchival := true;
end;

{************************************************************************}
// Tape backup page

var BackupActive,            // ntbackup runs
    BackupNewTape,           // when set performs full backup
    BackupFinished: Boolean; // ntbackup ready

var TapeNr: integer = 0;         // nr of tape drive
    Compression: string = 'off'; // compression 'off' or 'on'
    TapeName : string;

// wrapper around ntbackup.exe
// arguments:
// path        = directory to backup + subdirectories
// description = description to give backup set
// tapelogfile = path to logfile to write by ntbackup
// nok         = returns non-zero on error
// tape        = returns name of tape
// MB          = returns number of MB processed
// btype       = 'Normal' or 'Incremental'
// newtape     = if set, overwrites tape else append
// compression = 'off' or 'on'

procedure nt_backup(path, description, tapelogfile: string;
                    var nok: integer; var tape: string; var MB: integer;
                    btype: string; newtape: boolean; compression: string);
var com, text:  string;
    f:          textfile;
    i:          integer;
    FromF, ToF: file;
    NumRead, NumWritten: Integer;
    Buf: array[1..2048] of AnsiChar;
begin
  backupactive := true;
  MB := 0;

  // erase existing log file
  if FileExists(tapelogfile) then DeleteFile(tapelogfile);

  // for if things fail
  nok  := -1;
  tape := 'Unknown';

  // append or overwrite
  if not newtape then text := '/a'
  else                text := '';

  // full logfile or summary only
  {if not FullLog then} text := text + '/e';

  com := Format('backup %s %s /v /d %s /hc:%s /t %s /l"%s" /tape:%d',
               [path, text, description, compression, btype, tapelogfile, TapeNR]);

  // run ntbackup (blocking)
  RunProgramWaiting('ntbackup.exe', CurDir, [com]);

  // no log file: operation failed
  if not FileExists(tapelogfile) then
  begin
    nok := -99999;
    backupactive := false;
    exit;
  end;

  // fixup logfile; required for NT4 SP3 (file contains 00 bytes)
  AssignFile(FromF, tapelogfile);
  Reset(FromF, 1);

  AssignFile(ToF, tapelogfile+'.tmp');
  Rewrite(ToF, 1);

  repeat
    BlockRead(FromF, Buf, SizeOf(Buf), NumRead);

    for i:=1 to 2048 do
      if Buf[i]=char(0) then Buf[i]:=' ';

    BlockWrite(ToF, Buf, NumRead, NumWritten);
  until (NumRead = 0) or (NumWritten <> NumRead);

  CloseFile(FromF);
  CloseFile(ToF);

  // interpret logfile
  assignfile (f, tapelogfile+'.tmp');
  reset(f);
  while not eof(f) do
  begin
    readln(f, text);

    // verify must have been completed
    if strpos(Pchar(text), 'Verify completed') <> nil then
      nok := 0;

    // extract tape name from file (obtained with ""s)
    if strpos(Pchar(text), 'Tape Name: ') <> nil then
    begin
      tape := copy(text, 11, 99);
      while tape[1]=' ' do tape := copy(tape, 2, 99);
    end;

    // comparison found differences; get number of different files
    if strpos(Pchar(text), ' files were different') <> nil then
    begin
      i := strpos(Pchar(text), ' files were different') - Pchar(text);
      text := copy(text, 1, i);
      while text[1]=' ' do text := copy(text, 2, 99);
      nok := StrToIntDef(text, -1);
      break;
    end;

    // idem
    if strpos(Pchar(text), ' file was different') <> nil then
    begin
      i := strpos(Pchar(text), ' file was different') - Pchar(text);
      text := copy(text, 1, i);
      while text[1]=' ' do text := copy(text, 2, 99);
      nok := StrToIntDef(text, -1);
      break;
    end;

    // find number of MB stored
    if strpos(Pchar(text), 'Processed') = Pchar(text) then
    begin
      text := copy(text, 11, 250);
      com  := '';
      for i:=1 to length(text) do
      begin
        if      text[i] = ','         then continue
        else if text[i] in ['0'..'9'] then com := com + text[i]
        else                               break;
      end;

      try
        MB := round (StrToFloat(com) / (1024 * 1024));
      except
        MB := 0;
      end;
    end;
  end;

  CloseFile(f);
  DeleteFile(tapelogfile+'.tmp');

  backupactive := false;
end;

procedure UpdateTapeIni;
var f: textfile;
begin
  Form1.UsedTapeMBLabel.Caption := 'UsedTapeMB = ' + IntToStr(UsedTapeMB);

  if FileExists(//ExtractFileDir(ParamStr(0))
    CurDir + '\tapebackup.ini') then
  begin
    AssignFile(f, //ExtractFileDir(ParamStr(0))
      CurDir + '\tapebackup.ini');
    Rewrite(f);

    writeln(f, '# This file contains configuration information for tape backup for the DICOM server');
    writeln(f, '');

    writeln(f, 'TapeDevice       = ' + IntToStr(TapeDevice));
    writeln(f, 'TapeCapacityMB   = ' + IntToStr(TapeCapacityMB));
    writeln(f, 'UsedTapeMB       = ' + IntToStr(UsedTapeMB));
    writeln(f, 'BackupTime       = ' + BackupTime);

    CloseFile(f);
  end;
end;

// creates header file for a new tape (with date, time + tape name)
procedure create_log_header(tapename: string);
var f:textfile;
    t: string;
    H, M, S, MS: word;
    i: integer;
begin
  DecodeTime(Time, H, M, S, MS);
  t := format('%02d:%02d:%02d', [H, M, S]);
  for i:=1 to 8 do if t[i]=' ' then t[i]:='0';

  if not SysUtils.DirectoryExists(CurDir + '\TapeHeader') then
    mkdir(CurDir + '\TapeHeader');

  assignfile(f, CurDir + '\TapeHeader\Logheader.txt');
  rewrite(f);
  writeln(f, DateToStr(Date));
  writeln(f, t);
  writeln(f, tapename);
  closefile(f);
end;

// log a line to the logfile and the memo
procedure log_line(line: string);
var text, t: string;
    H, M, S, MS: word;
    i: integer;
begin
  DecodeTime(Time, H, M, S, MS);
  t := format('%02d:%02d:%02d', [H, M, S]);
  for i:=1 to 8 do if t[i]=' ' then t[i]:='0';

  text := format('%s %s: %s', [DateToStr(Date), t, line]);
  TapebackupStatus := text;

  WriteMemo(Form1.Memo2, text, 200, 100, 'tapebackup');
end;

procedure TForm1.BlankTapeClick(Sender: TObject);
var M, Y, D: word;
    logname, text: string;
    nok, i, MB: integer;
begin
  if not InputQuery('Name for new tape (OK will overwrite tape!)', '', TapeName) then
    exit;

  TapebackupStatus := 'Busy initializing blank tape';

  DecodeDate(Date, Y, M, D);
  logname := format('Tape%04d%02d%02d', [Y, M, D]);
  for i:=1 to 12 do if logname[i]=' ' then logname[i]:='0';

  create_log_header(MagDeviceList[0]);

  nt_backup(CurDir + '\TapeHeader',
            'Blank tape on: ' + logname,
            CurDir + '\' + logname + '.log',
            nok,
            TapeName,
            MB,
            'Normal',
            true,
            Compression);

  UsedTapeMB := 0;
  UpdateTapeIni;

  if nok <> 0 then
  begin
    text := format('FAIL: Blank tape, tape %s', [TapeName]);
    log_line(text);
    backupnewtape := false;
    backupfinished := false;
    ShowMessage('Initialize blank tape failed: please fix the problem');
  end
  else
  begin
    text := format('OK: Blank tape, tape %s', [TapeName]);
    log_line(text);
    backupnewtape := true;
    backupfinished := false;
  end;
end;

procedure TForm1.OverwriteTapeClick(Sender: TObject);
var M, Y, D: word;
    logname, text : string;
    nok, i, MB: integer;
begin
  DecodeDate(Date, Y, M, D);
  logname := format('Tape%04d%02d%02d', [Y, M, D]);
  for i:=1 to 12 do if logname[i]=' ' then logname[i]:='0';

  if MessageDlg('Overwrite currently loaded tape ?', mtConfirmation,
    [mbOk, mbCancel], 0) = mrCancel then
    exit;

  TapebackupStatus := 'Busy re-initializing used tape';

  create_log_header(MagDeviceList[0]);

  nt_backup(CurDir + '\TapeHeader',
            'Overwriting tape on: ' + logname,
            CurDir + '\' + logname + '.log',
            nok,
            TapeName,
            MB,
            'Normal',
            true,
            Compression);

  UsedTapeMB := 0;
  UpdateTapeIni;

  if nok <> 0 then
  begin
    text := format('FAIL: Overwrite tape, tape %s', [TapeName]);
    log_line(text);
    backupnewtape  := false;
    backupfinished := false;
    ShowMessage('Overwriting tape failed: please fix the problem');
  end
  else
  begin
    text := format('OK: Overwrite tape, tape %s', [TapeName]);
    log_line(text);
    backupnewtape  := true;
    backupfinished := false;
  end;
end;

procedure TForm1.Timer5Timer(Sender: TObject);
var H, M, S, MS, Y, D: word;
    t, logname, text, directory: string;
    nok, i, MB: integer;
    f: textfile;
    forcedbackup: boolean;
begin
  // display time
  DecodeTime(Time, H, M, S, MS);
  t := format('%02d:%02d:%02d', [H, M, S]);
  for i:=1 to 8 do if t[i]=' ' then t[i]:='0';
  StaticTextTapeTime.Caption := t + ' - ' + TapeBackupStatus;
  StaticTextTapeTime.Hint := TapeBackupStatus;

  if UsedTapeMB >= TapeCapacityMB then
    TapebackupStatus := 'Tape capacity exceeded';

  directory := MagDeviceList[0];

  try
    if FileExists(curdir + '\TapeBackup.Ping') then
    begin
      DeleteFile(curdir + '\TapeBackup.Ping');
      AssignFile(f, curdir + '\TapeBackup.Response');
      Rewrite(f);
      Writeln(f, TapebackupStatus);
      CloseFile(f);
    end;
  except;
  end;

  if BackupActive then exit;

  forcedbackup := false;

  try
    if FileExists(curdir + '\TapeBackup.Control') then
    begin
      AssignFile(f, curdir + '\TapeBackup.Control');
      Reset(f);
      Readln(f, directory);
      forcedbackup := true;
      CloseFile(f);
      DeleteFile(curdir + '\TapeBackup.Control');
      if UsedTapeMB < TapeCapacityMB then
        TapebackupStatus := 'BUSY: forced backup of ' + directory
      else
        TapebackupStatus := 'Tape capacity exceeded';

      AssignFile(f, curdir + '\TapeBackup.Response');
      Rewrite(f);
      Writeln(f, TapebackupStatus);
      CloseFile(f);
    end;
  except
  end;

  if UsedTapeMB >= TapeCapacityMB then exit;

  // test if current time is start time
  if ((copy(t, 1, length(BackupTime))=BackupTime) or ForcedBackup)
     and not BackupFinished then
  begin
    // create name for logfile (date in QUIRT notation)
    DecodeDate(Date, Y, M, D);
    logname := format('Tape%04d%02d%02d', [Y, M, D]);
    for i:=1 to 12 do if logname[i]=' ' then logname[i]:='0';

    // perform full backup on request
    if BackupNewTape or ForcedBackup then
    begin
      if ForcedBackup then
        text := directory
      else
        text := 'Full backup ' + logname;

      nt_backup(directory,
               text,
               CurDir + '\' + logname + '.log',
               nok,
               TapeName,
               MB,
               'Normal',
               false,
               Compression);

      UsedTapeMB := UsedTapeMB + MB;
      UpdateTapeIni;

      if (nok > 0) and (nok <= 5) then
      begin
        text := format('ALMOST OK FAIL: FB (%d files), tape %s', [nok, TapeName]);
        log_line(text);
        backupnewtape := false;
      end
      else if nok <> 0 then
      begin
        text := format('FAIL: FB (%d files), tape %s', [nok, TapeName]);
        log_line(text);
        backupnewtape := false;
      end
      else
      begin
        if ForcedBackup then
          text := format('OK: forced backup, tape %s, directory %s', [TapeName, directory])
        else
          text := format('OK: FB, tape %s', [TapeName]);

        log_line(text);
        backupnewtape := false;
      end;
    end
    else
    begin
      // default do incremental backup
      nt_backup(directory,
               'Incremental backup ' + logname,
               CurDir + '\' + logname + '.log',
               nok,
               TapeName,
               MB,
               'Incremental',
               false,
               Compression);

      UsedTapeMB := UsedTapeMB + MB;
      UpdateTapeIni;

      if nok <> 0 then
      begin
        text := format('FAIL: IB (%d files), tape %s', [nok, TapeName]);
        log_line(text);
        backupnewtape := false;
      end
      else
      begin
        text := format('OK: IB, tape %s', [TapeName]);
        log_line(text);
        backupnewtape := false;
      end;
    end;

    BackupFinished := True;

    try
      AssignFile(f, curdir + '\TapeBackup.Response');
      Rewrite(f);
      Writeln(f, TapebackupStatus);
      CloseFile(f);
    except
    end;
  end;

  // only allow new backup to start after starttime no longer valid
  if copy(t, 1, length(BackupTime))<>BackupTime then
    BackupFinished := False;

  if UsedTapeMB >= TapeCapacityMB then
  begin
    log_line('WARNING: TAPE IS FULL, BACKUP WILL FAIL NEXT TIME');
  end;
end;

// Files dropped on the user interface will be loaded into the server
procedure Tform1.WMDropFiles(var Message: TWMDropFiles);
  procedure DropDirectory(root, newpatid: string);
  var sr: TSearchRec;
  begin
    WriteMemoSl(ServerStatusMemo, '---- Processing directory: '+root, 1100, 1000, 'serverstatus', true, ServerStatusStringList);
    if FindFirst(root + '\*.*', faAnyFile, sr) = 0 then
    begin
      if (sr.Attr and faDirectory)<>0 then
      begin
        if (sr.Name<>'.') and (sr.Name<>'..') then
          DropDirectory(root + '\' + sr.Name, newpatid);
      end
      else
      begin
        ServerTask('', 'addimagefile:' + GuiToServerFilename(root + '\' + sr.Name) + newpatid);
        Application.ProcessMessages;
      end;
    end;
    while FindNext(sr)=0 do
    begin
      if killed then break;
      if (sr.Attr and faDirectory)<>0 then
      begin
        if (sr.Name<>'.') and (sr.Name<>'..') then
          DropDirectory(root + '\' + sr.Name, newpatid)
      end
      else
      begin
        ServerTask('', 'addimagefile:' + GuiToServerFilename(root + '\' + sr.Name) + newpatid);
        Application.ProcessMessages;
      end;
    end;
    FindClose(sr);
  end;
var
  i, num: integer;
  aFileName: array[0..1024] of char;
  ok: boolean;
  newpatid: string;
begin
  num := DragQueryFile(Message.Drop, $FFFFFFFF, aFileName, 0);
  if num < 1 then Exit;

  if (GetKeyState(VK_LMENU) and $80000000)<>0 then
  begin
    ok := InputQuery('Enter '+IntToStr(num)+' files and/or directories with new patient ID ?', 'New patient ID:', newpatid);
    newpatid := ',' + newpatid;
    if ok and (newpatid[2]='$') then
      ok := MessageDlg('Warning: this action starts a server debug task - continue ?', mtConfirmation, [mbYes, mbNo], 0)=mrYes;
  end
  else
  begin
    ok := MessageDlg('Enter ' + IntToStr(num) + ' files and/or directories into server ?', mtConfirmation, [mbYes, mbNo], 0)=mrYes;
    newpatid := '';
  end;

  killed := false;

  if ok then
  begin
    if PageControl1.ActivePage <> TabSheet6 then
    begin
      PageControl1.ActivePage := TabSheet6;
      PageControl1Change(self);
    end;

    WriteMemoSl(ServerStatusMemo, '------------ Adding image files to server -----------', 1100, 1000, 'serverstatus', true, ServerStatusStringList);

    for i:=0 to num-1 do
    begin
      if killed then break;
      DragQueryFile(Message.Drop, i, aFileName, 1024);

      if SysUtils.DirectoryExists(aFileName) then
        DropDirectory(AFileName, newpatid)
      else
      begin
        ServerTask('', 'addimagefile:' + GuiToServerFilename(aFileName) + newpatid);
        Application.ProcessMessages;
      end;
    end;
  end;

  WriteMemoSl(ServerStatusMemo, '-----------------------------------------------------', 1100, 1000, 'serverstatus', true, ServerStatusStringList);
  DragFinish(Message.Drop);
end;

{************************************************************************}
// Weekly checks page

procedure TForm1.CheckBox1Click(Sender: TObject);
var i, n: integer;
begin
  // change of day
  if (CheckReading) then exit;

  n := StrToInt((Sender as TCheckBox).Hint);
  CheckDays[n] := 0;
  for i:=0 to WeeklyChecksPage.ControlCount-1 do
    if WeeklyChecksPage.Controls[i].Hint = (Sender as TCheckBox).Hint then
    begin
      if (WeeklyChecksPage.Controls[i].Tag = 10) and ((WeeklyChecksPage.Controls[i] as TCheckBox).Checked) then CheckDays[n] := CheckDays[n] + 1;
      if (WeeklyChecksPage.Controls[i].Tag = 11) and ((WeeklyChecksPage.Controls[i] as TCheckBox).Checked) then CheckDays[n] := CheckDays[n] + 2;
      if (WeeklyChecksPage.Controls[i].Tag = 12) and ((WeeklyChecksPage.Controls[i] as TCheckBox).Checked) then CheckDays[n] := CheckDays[n] + 4;
      if (WeeklyChecksPage.Controls[i].Tag = 13) and ((WeeklyChecksPage.Controls[i] as TCheckBox).Checked) then CheckDays[n] := CheckDays[n] + 8;
      if (WeeklyChecksPage.Controls[i].Tag = 14) and ((WeeklyChecksPage.Controls[i] as TCheckBox).Checked) then CheckDays[n] := CheckDays[n] + 16;
      if (WeeklyChecksPage.Controls[i].Tag = 15) and ((WeeklyChecksPage.Controls[i] as TCheckBox).Checked) then CheckDays[n] := CheckDays[n] + 32;
      if (WeeklyChecksPage.Controls[i].Tag = 16) and ((WeeklyChecksPage.Controls[i] as TCheckBox).Checked) then CheckDays[n] := CheckDays[n] + 64;
    end;
end;

procedure TForm1.Edit1Change(Sender: TObject);
var n: integer;
begin
  // change of time edit
  if (CheckReading) then exit;

  n := StrToInt((Sender as TEdit).Hint);
  CheckTimes[n] := (Sender as TEdit).Text;
end;

procedure TForm1.ComboBox2Change(Sender: TObject);
var n, i: integer;
begin
  // change of selected action
  if (CheckReading) then exit;

  n := StrToInt((Sender as TComboBox).Hint);

  for i:=0 to WeeklyChecksPage.ControlCount-1 do
    if (WeeklyChecksPage.Controls[i].Tag = 9) and (WeeklyChecksPage.Controls[i].Hint = (Sender as TComboBox).Hint) then
    begin
      if (Sender as TComboBox).ItemIndex > 20 then
      begin
        TEdit(WeeklyChecksPage.Controls[i]).Color := clWhite;
        TEdit(WeeklyChecksPage.Controls[i]).Enabled := True;
      end
      else
      begin
        TEdit(WeeklyChecksPage.Controls[i]).Color := clltGray;
        TEdit(WeeklyChecksPage.Controls[i]).Enabled := False;
      end;
    end;

  CheckActions[n] := (Sender as TComboBox).Items[(Sender as TComboBox).ItemIndex];
end;

procedure TForm1.Edit2Change(Sender: TObject);
var n: integer;
begin
  // change of CD num
  if (CheckReading) then exit;

  n := StrToInt((Sender as TEdit).Hint);
  CheckCDs[n] := StrToIntDef((Sender as TEdit).Text, 0);
end;

var ForceWeeklyCheck: integer;

procedure TForm1.Edit1DblClick(Sender: TObject);
begin
  if (GetAsyncKeyState(VK_CONTROL) and $8000)=0 then exit;
  ForceWeeklyCheck := StrToInt((sender as TEdit).Hint);
end;

procedure TForm1.WeeklyCheckTimerTimer(Sender: TObject);
var n, i, j, day: integer;
    en: array[1..10] of boolean;
    H, M, S, MS: WORD;
    u, t, v: string;
begin
  (* Maybe check whether all produced dicom-objects have been sent to the pacs
     NOTE: When a submit failure has been detected, the same mailer will be
     used...
  *)
  CheckDicomSubmits;

  // timer for check page (runs once per 2 s)
  DecodeTime(Time, H, M, S, MS);
  t := format('%02d:%02d', [H, M]);
  for i:=1 to 5 do if t[i]=' ' then t[i]:='0';

  StaticTextWeeklyCheck.Caption := t;

  // collect mail for one minute; mail at most once every 10 minutes
  if (mailer.HdrSubject<>''                ) and
     (not MailActive                             ) and
     (Now-MailStartTime > MailCollectTime/(24*60)) and
     (Now-LastMailTime  > MailWaitTime/(24*60)   ) then
  begin
    MailActive := true;
    try
      mailer.HdrFrom    := mailer.FromName;
      mailer.OpenSync;
      mailer.MailSync;
      mailer.QuitSync;
    finally
      mailer.HdrSubject := '';
      mailer.MailMessage.Clear;
      LastMailTime := Now;
      MailActive := false;
    end;
  end;

  for i:=1 to 10 do en[i] := false;

  // check day of week
  day := DayOfWeek(Now);
  if day=7 then day := 0; // saturday

  for i:=0 to WeeklyChecksPage.ControlCount-1 do
    if WeeklyChecksPage.Controls[i].Tag = day + 10 then
      if (WeeklyChecksPage.Controls[i] as TCheckBox).Checked then
        en[StrToInt(WeeklyChecksPage.Controls[i].Hint)] := true;

  // check time(s) of day
  for i:=0 to WeeklyChecksPage.ControlCount-1 do
    if WeeklyChecksPage.Controls[i].Tag = 7 then
    begin
      u := (WeeklyChecksPage.Controls[i] as TEdit).Text;
      n := StrToInt(WeeklyChecksPage.Controls[i].Hint);

      if pos(';', u)>0 then
      begin
        v := u + ';';
        while pos(';', v)>0 do
        begin
          u := copy(v, 1, pos(';', v)-1);
          v := copy(v, pos(';', v)+1, 250);
          if (copy(t, 1, length(u))=u) then break;
        end;
      end;

      en[n] := en[n] and (copy(t, 1, length(u))=u);
      if length(u)=0 then CheckTriggered[n] := false; // runs more than once a day
    end;

  if ForceWeeklyCheck<>0 then en[ForceWeeklyCheck] := true;
  ForceWeeklyCheck := 0;

  for i:= 1 to 10 do
  begin
    // logic to avoid starting check more than once in same minute
    if not en[i] then CheckTriggered[i] := false;

    if en[i] and (not CheckActive[i]) and (not CheckTriggered[i]) then
    begin
      n := 0;

      CheckActive[i] := true;
      CheckTriggered[i] := true;

      WriteMemo(WeeklyMemo, '----------------------------------------------------------', 100, 200, 'weeklychecks');
      WriteMemo(WeeklyMemo, 'Starting ' + CheckActions[i] + ' (' + IntToStr(CheckCDs[i]) + ') at: ' + DateTimeToStr(now), 100, 200, 'weeklychecks');

      if Pos('Check Mirror', CheckActions[i])>0 then
      begin
        n := RunProgramWaiting(dgateExe, curdir,
               ['-!'+WeeklySocket.Port + ' -avMAG' + Copy(CheckActions[i], Length('Check Mirror '), 99)]);
      end;
      if Pos('Test MAG', CheckActions[i])>0 then
      begin
        n := RunProgramWaiting(dgateExe, curdir,
               ['-!'+WeeklySocket.Port + ' -atMAG' + Copy(CheckActions[i], Length('Test MAG '), 99)]);
      end;
      if Pos('Test Jukebox', CheckActions[i])>0 then
      begin
        n := RunProgramWaiting(dgateExe, curdir,
               ['-!'+WeeklySocket.Port + ' -atJUKEBOX' + Copy(CheckActions[i], Length('Test Jukebox '), 99) + '.' + IntToStr(CheckCDs[i])]);
        inc(CheckCDs[i]);

        for j:=0 to WeeklyChecksPage.ControlCount-1 do
          if WeeklyChecksPage.Controls[j].Tag = 9 then
            if WeeklyChecksPage.Controls[j].Hint = IntToStr(i) then
              (WeeklyChecksPage.Controls[j] as Tedit).Text := IntToStr(CheckCDs[i]);
      end;
      if Pos('Grab ', CheckActions[i])>0 then
      begin
        // GrabPatients(Copy(CheckActions[i], 6, 250), FormatDateTime('yyyymmdd', now));
        n := RunProgramWaiting(dgateExe, curdir,
               ['-!'+WeeklySocket.Port + ' -g' + Copy(CheckActions[i], 6, 250) + ',' + FormatDateTime('yyyymmdd', now)]);
      end;

      if Pos('Kill and Restart the Server', CheckActions[i])>0 then
        KillAndRestartTheServerClick(nil);

      if Pos('Pack the Database', CheckActions[i])>0 then
        ServerTask('Weekly maintenance packs the database', 'packdbf:');

      CheckActive[i] := false;

      if (n<>0) and (Length(mailer.FromName)<>0) then
      begin
        MailMessage('DICOM check failed: ' + CheckActions[i]);
      end;

      WriteMemo(WeeklyMemo, 'Ready with ' + CheckActions[i] + ' at: ' + DateTimeToStr(now), 100, 200, 'weeklychecks');
      WriteMemo(WeeklyMemo, '----------------------------------------------------------', 100, 200, 'weeklychecks');
    end;
  end;
end;

procedure TForm1.WeeklySocketDataAvailable(Sender: TObject; Error: Word);
  var buffer: array[0..8192] of char;
  s : string;
  i: integer;
begin
  i := WeeklySocket.Receive(@buffer, 8191);
  if i<1 then exit;

  buffer[i] := #00;
  if buffer[i-1] in [#10, #13] then buffer[i-1] := #00;
  s := buffer;
  // s := copy(s, 0, length(s)-1);
  for i:=1 to length(s) do if s[i]<' ' then s[i]:=' ';

  WriteMemo(WeeklyMemo, s, 200, 100, 'weeklychecks');
end;


{************************************************************************}

var     LastSubmitCheckDate: string = '';

procedure TForm1.CheckDicomSubmits;
var     t: string;
        Year, Month, Day, H, M, S, MS: WORD;
        i, j: integer;
        bCheckSubmitAEs: bool;
        iNbItems: integer;
        RemoteAE, RemoteIP, RemotePort: Ansistring;
        ResultLog, MissingPatients: string;
        q, code: AnsiString;
        strings : TStringList;
label   SEND_MAIL;
begin
  (* There is nothing to do when the PacsAE is empty *)
  if PacsAE = '' then exit;

  (* There is nothing to do when when all the SourceAEs are empty. *)
  bCheckSubmitAEs := false;
  for i:=1 to 10 do
    if SourceAEs[i] <> '' then
    begin
      bCheckSubmitAEs := true;
      break;
    end;
  if bCheckSubmitAEs = false then exit;

  (* Timer for check page (runs once per 2 s) *)
  DecodeTime(Time, H, M, S, MS);
  t := format('%02d:%02d', [H, M]);
  for i:=1 to 5 do
    if t[i]=' ' then
      t[i]:='0';
  if t <> SubmitCheckTime then exit;

  (* Get the date as string *)
  DecodeDate(Date, Year, Month, Day);
  t := format('%04d%02d%02d', [Year, Month, Day]);
  for i:=1 to 8 do
    if t[i]=' ' then
      t[i]:='0';
  if (t = LastSubmitCheckDate) then exit;
  LastSubmitCheckDate := t;

  (* Search the AE in the Combobox of the Query/Move tabpage *)
  bCheckSubmitAEs := false;
  iNbItems := DicomSystem.Items.Count;
  for i:=0 to iNbItems - 1 do
  begin
   SplitAE(DicomSystem.Items[i], RemoteAE, RemoteIP, RemotePort);
   if RemoteAE = PacsAE then
   begin
     bCheckSubmitAEs := true;
     break;
   end;
  end;
  if bCheckSubmitAEs = false then exit;

  ResultLog := '';

  (* Loop over all SourceAEs *)
  for i:=1 to 10 do
  begin
    if SourceAEs[i] <> '' then
    begin
      iNbItems := DicomSystem.Items.Count;
      (* Find this SourceAE in the Combobox of the Query/Move tabpage *)
      for j:=0 to iNbItems - 1 do
      begin
        SplitAE(DicomSystem.Items[j], RemoteAE, RemoteIP, RemotePort);
        if RemoteAE = SourceAEs[i] then
        begin
          q := MakeQueryString('IMAGE', '', '', t, '', '', '', '', '', '');

          code :=
          'local localae=[['+trim(ServerName.Text)+']];' +
          'local remoteae=[['+RemoteAE+']];' +
          'local q='+q+';' +
          'q2=DicomObject:new(); for k,v in pairs(q) do q2[k]=v end;' +
          'local s=tempfile("txt") f=io.open(s, "wb");' +
          'local r1 = dicomquery(localae, "IMAGE", q2);' +
          'if r1==nil then f:write("\nno connection with "..localae.."\n") returnfile=s f:close() return end; ' +
          'local r2 = dicomquery(remoteae, "IMAGE", q2);' +
          'if r2==nil then f:write("\nno connection with "..remoteae.."\n") returnfile=s f:close() return end; ' +
          'local sops = {}; '+
          'for i=0, #r1-1 do sops[r1[i].SOPInstanceUID]=r1[i].PatientID end; '+
          'for i=0, #r2-1 do sops[r2[i].SOPInstanceUID]=nil end; '+
          'local pats = {}; for k,v in pairs(sops) do pats[v] = (pats[v] or 0) + 1 end; '+
          'for k,v in pairs(pats) do f:write(k .. ",") end; f:write("\n"); ' +
          'f:write("images in "..localae .. " = " .. #r1 .. "\n"); ' +
          'f:write("images in "..remoteae .. " = " .. #r2 .. "\n"); ' +
          'local total=0; for k,v in pairs(pats) do total=total+v; f:write(k .. " misses " .. v .. " images\n") end; ' +
          'f:write("total missing image = " .. total .. "\n"); ' +
          'returnfile=s f:close();' +
          '';
          ServerTask('#query.txt','lua:'+code);

          MissingPatients := '';
          strings := TStringList.Create;
          strings.LoadFromFile('query.txt');
          if strings.Count=0 then
            MissingPatients := 'Error connecting'
          else if Length(strings[0])<>0 then
            MissingPatients := Copy(strings[0], 1, length(strings[0])-1);
          DeleteFile('query.txt');
          strings.Free;

          if MissingPatients <> '' then
          begin
            ResultLog := ResultLog + 'Unsubmitted patients on ' + SourceAEs[i] + ':';
            ResultLog := ResultLog + #13 + #10 + MissingPatients + #13 + #10;
          end;
        end;
      end;
    end;
  end;

  (* Send an e-mail if anything goes wrong *)
SEND_MAIL:
  if (ResultLog <> '') and (Length(mailer.FromName)<>0) then
  begin
    ResultLog := 'CheckDicomSubmits ' + t + #13 + #10 + #13 + #10 + ResultLog;
    if mailer.HdrSubject='' then
    begin
      mailer.HdrSubject := 'PACS SubmitCheck';
      MailStartTime := Now;
    end;
    mailer.MailMessage.Add(ResultLog);
  end;
end;

{************************************************************************}
{*                    INITIALIZATION AND FINALIZATION                   *}
{************************************************************************}

end.
