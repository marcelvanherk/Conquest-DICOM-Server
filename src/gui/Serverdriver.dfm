object Form1: TForm1
  Left = 28
  Top = 84
  Caption = 'CONQUEST Dicom Server - Installation'
  ClientHeight = 554
  ClientWidth = 818
  Color = clBtnFace
  Constraints.MinHeight = 584
  Constraints.MinWidth = 834
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = True
  Position = poScreenCenter
  Scaled = False
  Visible = True
  OnActivate = FormActivate
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  PixelsPerInch = 120
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 818
    Height = 554
    ActivePage = TabSheet6
    Align = alClient
    ParentShowHint = False
    ShowHint = False
    TabOrder = 0
    OnChange = PageControl1Change
    object TabSheet1: TTabSheet
      Caption = 'Configuration'
      DesignSize = (
        810
        526)
      object Panel2: TPanel
        Left = 0
        Top = 0
        Width = 585
        Height = 526
        Align = alLeft
        BevelOuter = bvLowered
        TabOrder = 0
        object Label4: TLabel
          Left = 72
          Top = 32
          Width = 412
          Height = 13
          Caption = 
            'This screen contains essential parameters for operation of the C' +
            'onQuest DICOM server.'
        end
        object Label1: TLabel
          Left = 48
          Top = 68
          Width = 305
          Height = 13
          Caption = 
            'Local unique name of this DICOM server (application entity, AE) ' +
            ':'
        end
        object Label2: TLabel
          Left = 48
          Top = 115
          Width = 317
          Height = 13
          Caption = 'TCP/IP port to use (other DICOM systems must know this number) :'
        end
        object Label3: TLabel
          Left = 48
          Top = 162
          Width = 209
          Height = 13
          Caption = 'Local disk directory to store DICOM images :'
        end
        object Label5: TLabel
          Left = 344
          Top = 182
          Width = 181
          Height = 16
          Caption = 'Free disk space: ? Megabyte'
          Color = clBtnFace
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clRed
          Font.Height = -13
          Font.Name = 'MS Sans Serif'
          Font.Style = [fsItalic]
          ParentColor = False
          ParentFont = False
        end
        object Label10: TLabel
          Left = 344
          Top = 160
          Width = 145
          Height = 16
          Caption = 'C:\WINDOWS\system32'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          OnClick = Label10Click
        end
        object Label19: TLabel
          Left = 42
          Top = 365
          Width = 120
          Height = 13
          Caption = 'Cleanup disk below: (MB)'
        end
        object Label35: TLabel
          Left = 42
          Top = 415
          Width = 131
          Height = 13
          Caption = 'Cleanup nightly below: (MB)'
        end
        object Label36: TLabel
          Left = 111
          Top = 465
          Width = 29
          Height = 13
          Caption = 'Below'
          Enabled = False
        end
        object Label37: TLabel
          Left = 201
          Top = 465
          Width = 66
          Height = 13
          Caption = 'MB, move to: '
          Enabled = False
        end
        object NightlyStrTimeToMoveLabel: TLabel
          Left = 44
          Top = 465
          Width = 13
          Height = 13
          Caption = 'At:'
        end
        object ServerName: TMaskEdit
          Left = 408
          Top = 64
          Width = 129
          Height = 21
          Hint = 'This entry may be 16 letters long'
          EditMask = '>aaaaaaaaaaaaaaaa;1; '
          MaxLength = 16
          ParentShowHint = False
          ShowHint = True
          TabOrder = 0
          Text = '                '
          OnChange = ServerNameChange
        end
        object TCPIPport: TMaskEdit
          Left = 408
          Top = 111
          Width = 129
          Height = 21
          Hint = 'The default port 5678 should be OK'
          EditMask = '99999;1; '
          MaxLength = 5
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
          Text = '     '
        end
        object EnableJPEGCheckBox: TCheckBox
          Left = 48
          Top = 226
          Width = 217
          Height = 17
          Hint = 'Enable for modern DICOM systems'
          Caption = 'Enable Explicit and JPEG transfers'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 2
          OnClick = EnableJPEGCheckBoxClick
        end
        object Panel11: TPanel
          Left = 324
          Top = 223
          Width = 213
          Height = 122
          Hint = 'Configure DICOM file compression here'
          BevelOuter = bvLowered
          BorderStyle = bsSingle
          ParentShowHint = False
          ShowHint = True
          TabOrder = 3
          object LabelCompression: TLabel
            Left = 12
            Top = 2
            Width = 124
            Height = 13
            Caption = 'Images on disk are stored:'
            OnDblClick = LabelCompressionDblClick
          end
          object UncompressedButton: TRadioButton
            Left = 8
            Top = 16
            Width = 100
            Height = 21
            Hint = 'Decompress images before storage'
            Caption = 'Uncompressed'
            TabOrder = 0
          end
          object CompressNKIButton: TRadioButton
            Left = 8
            Top = 35
            Width = 100
            Height = 17
            Hint = 
              'Non-standard; has factor 2 to 2.5 compression ratio and is very ' +
              'fast'
            Caption = 'NKI compressed'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 1
          end
          object ModeNJRadioButton: TRadioButton
            Left = 114
            Top = 35
            Width = 96
            Height = 17
            Hint = 
              'Leaves JPEG images unchanged; otherwise uses maximal NKI compres' +
              'sion'
            Caption = 'JPEG or NKI'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 2
          end
          object CompressLosslessJPEGButton: TRadioButton
            Left = 8
            Top = 52
            Width = 100
            Height = 17
            Hint = 'Compress to JPEG using built-in compressor'
            Caption = 'Lossless JPEG'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 3
          end
          object CompressLossyJPEGButton: TRadioButton
            Left = 114
            Top = 52
            Width = 96
            Height = 17
            Hint = 'Some loss of image fidelity; uses built-in compressor'
            Caption = 'Lossy JPEG'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 4
          end
          object CompressLosslessJPEG2000Button: TRadioButton
            Left = 8
            Top = 86
            Width = 100
            Height = 17
            Hint = 'Using built-in compressor (OpenJpeg)'
            Caption = 'Lossless JP2000'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 5
          end
          object CompressLossyJPEG2000Button: TRadioButton
            Left = 114
            Top = 86
            Width = 96
            Height = 17
            Hint = 'Some loss of image fidelity; uses built-in compressor (OpenJpeg)'
            Caption = 'Lossy JP2000'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 7
          end
          object ModeUJRadioButton: TRadioButton
            Left = 8
            Top = 103
            Width = 185
            Height = 17
            Hint = 
              'Leaves JPEG images unchanged; otherwise stores uncompressed (opt' +
              'imal use of space)'
            Caption = 'Keep JPEG or Uncompressed'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 6
          end
          object CompressLosslessJPEGLSButton: TRadioButton
            Left = 8
            Top = 69
            Width = 100
            Height = 17
            Hint = 'Compress to Lossless JPEGLS (Using CharLS)'
            Caption = 'Lossless JPEGLS'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 8
          end
          object CompressLossyJPEGLSButton: TRadioButton
            Left = 114
            Top = 69
            Width = 96
            Height = 17
            Hint = 'Some loss of image fidelity; uses built-in compressor (CharLS)'
            Caption = 'Lossy JPEGLS'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 9
          end
        end
        object Panel12: TPanel
          Left = 42
          Top = 268
          Width = 213
          Height = 62
          Hint = 'Configure DICOM file extension here'
          BevelOuter = bvLowered
          BorderStyle = bsSingle
          ParentShowHint = False
          ShowHint = True
          TabOrder = 4
          object LabelImageNaming: TLabel
            Left = 12
            Top = 2
            Width = 127
            Height = 13
            Caption = 'Images on disk are named:'
            OnDblClick = LabelImageNamingDblClick
          end
          object V2RadioButton: TRadioButton
            Left = 8
            Top = 19
            Width = 185
            Height = 17
            Hint = 
              'Raw dump of DICOM tags, readable for most applications unless NK' +
              'I compressed, overruled to DCM when storing JPEG'
            Caption = 'V2 (allows NKI compression)'
            TabOrder = 0
            OnClick = V2RadioButtonClick
          end
          object DCMRadioButton: TRadioButton
            Left = 8
            Top = 41
            Width = 185
            Height = 17
            Hint = 
              'Standard chapter 10 DICOM format: precludes use of NKI compressi' +
              'on'
            Caption = 'DCM (standard format)'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 1
            OnClick = DCMRadioButtonClick
          end
        end
        object MagThreshold: TEdit
          Left = 176
          Top = 362
          Width = 81
          Height = 21
          Hint = 
            'If less than this disk space is left, least recently used patien' +
            'ts are deleted (0 is disable delete)'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 5
          Text = '0'
        end
        object NightlyTreshold: TEdit
          Left = 176
          Top = 412
          Width = 81
          Height = 21
          Hint = 
            'If less than this disk space is left at 01:00, least recently us' +
            'ed patients are deleted (0 is disable delete)'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 7
          Text = '0'
        end
        object EditNightlyMoveTreshold: TEdit
          Left = 143
          Top = 462
          Width = 52
          Height = 21
          Hint = 
            'Free disk space treshold of MAG0 for automatic transfer of data ' +
            'to other disk at 02:00'
          Enabled = False
          ParentShowHint = False
          ShowHint = True
          TabOrder = 9
          Text = '0'
        end
        object ComboBoxMoveTarget: TComboBox
          Left = 268
          Top = 462
          Width = 67
          Height = 21
          Hint = 
            'Disk selection for automatic transfer of data to other disk at 0' +
            '2:00'
          Style = csDropDownList
          Enabled = False
          ParentShowHint = False
          ShowHint = True
          TabOrder = 6
        end
        object CheckBoxKeepAlive: TCheckBox
          Left = 324
          Top = 361
          Width = 153
          Height = 17
          Hint = 'Test server once per minute and restart if it is dead'
          Caption = 'Keep Server Alive'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 8
          OnClick = CheckBoxKeepAliveClick
        end
        object NightlyStrTimeToMoveText: TMaskEdit
          Left = 63
          Top = 462
          Width = 42
          Height = 21
          EditMask = '!90:00;1;_'
          MaxLength = 5
          TabOrder = 10
          Text = '  :  '
        end
      end
      object SaveConfigButton: TButton
        Left = 624
        Top = 16
        Width = 153
        Height = 25
        Hint = 
          'Stores the information on this page. Unstored changes are not fu' +
          'nctional'
        Anchors = [akTop, akRight]
        Caption = 'Save configuration'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
        OnClick = SaveConfigButtonClick
      end
      object UpdateODBCButton: TButton
        Left = 624
        Top = 64
        Width = 153
        Height = 25
        Hint = 
          'Required when using an existing database: update ODBC to use sel' +
          'ected data directory or server (run server as administrator requ' +
          'ired)'
        Anchors = [akTop, akRight]
        Caption = 'Update ODBC data source'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 2
        OnClick = MakeODBCButtonClick
      end
      object RestoreconfigButton: TButton
        Left = 624
        Top = 112
        Width = 153
        Height = 25
        Hint = 'Discards all changes made to the situation which was last stored'
        Anchors = [akTop, akRight]
        Caption = 'Restore original configuration'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 3
        OnClick = RestoreconfigButtonClick
      end
      object InstallAsService: TButton
        Left = 624
        Top = 220
        Width = 153
        Height = 25
        Hint = 
          'Makes that server runs without logging in and without user inter' +
          'face (OPTIONAL, run server as administrator required) '
        Anchors = [akTop, akRight]
        Caption = 'Install server as NT service'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 4
        OnClick = InstallAsServiceClick
        OnMouseDown = InstallAsServiceMouseDown
      end
      object UninstallServerAsService: TButton
        Left = 624
        Top = 264
        Width = 153
        Height = 25
        Hint = 
          'Make the server a normal application (run server as admininstrat' +
          'or required)'
        Anchors = [akTop, akRight]
        Caption = 'Uninstall server as NT service'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 5
        OnClick = UninstallServerAsServiceClick
        OnMouseDown = UninstallServerAsServiceMouseDown
      end
      object HideButton: TButton
        Left = 624
        Top = 392
        Width = 153
        Height = 25
        Hint = 
          'Restore the window by clicking on the Conquest tray icon on the ' +
          'taskbar'
        Anchors = [akRight, akBottom]
        Caption = 'Hide the server (as tray icon)'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 6
        OnClick = HideButtonClick
      end
      object About1: TButton
        Left = 624
        Top = 440
        Width = 153
        Height = 25
        Hint = 'Short description and some copyright information.'
        Anchors = [akRight, akBottom]
        Caption = 'About this server'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 7
        OnClick = About1Click
      end
      object Close1: TButton
        Left = 624
        Top = 487
        Width = 153
        Height = 25
        Hint = 'Close the application.'
        Anchors = [akRight, akBottom]
        Caption = 'Close the server'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 8
        OnClick = Close1Click
      end
      object btnDoItNow: TButton
        Left = 342
        Top = 460
        Width = 75
        Height = 25
        Caption = 'Do It Now'
        TabOrder = 9
        OnClick = btnDoItNowClick
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Installation'
      DesignSize = (
        810
        526)
      object VerifyTCPIPButton: TButton
        Left = 624
        Top = 16
        Width = 153
        Height = 25
        Hint = 
          'Test communication between the DICOM application and the user in' +
          'terface'
        Anchors = [akTop, akRight]
        Caption = 'Verify TCP/IP installation'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
        OnClick = VerifyTCPIPButtonClick
      end
      object MakeODBCButton: TButton
        Left = 624
        Top = 64
        Width = 153
        Height = 25
        Hint = 
          'Updates the ODBC database link to point to the newly selected da' +
          'ta directory or server (run server as administrator required)'
        Anchors = [akTop, akRight]
        Caption = 'Make ODBC data source'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
        OnClick = MakeODBCButtonClick
      end
      object VerifyDatabaseButton: TButton
        Left = 624
        Top = 112
        Width = 153
        Height = 25
        Hint = 
          'Test database functions; and update UIDMODS table (add Stage col' +
          'umn) if needed'
        Anchors = [akTop, akRight]
        Caption = 'Verify database installation'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 2
        OnClick = VerifyDatabaseButtonClick
      end
      object InitDatabaseButton: TButton
        Left = 624
        Top = 160
        Width = 153
        Height = 25
        Hint = 
          'Delete current tables except worklist (if present) and rescan al' +
          'l images to regenerate all tables without loss of information (c' +
          'an take a long time)'
        Anchors = [akTop, akRight]
        Caption = '(Re)-initialize database'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 3
        OnClick = InitDatabaseButtonClick
      end
      object ButtonClearWorkList: TButton
        Left = 624
        Top = 208
        Width = 153
        Height = 25
        Hint = 
          'Makes the worklist database empty - contains optional data impor' +
          'ted from HL7 source - cannot be regenerated'
        Anchors = [akTop, akRight]
        Caption = 'Clear worklist database'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 4
        OnClick = ButtonClearWorklistClick
      end
      object Button8: TButton
        Left = 624
        Top = 392
        Width = 153
        Height = 25
        Hint = 
          'Restore the window by clicking on the Conquest tray icon on the ' +
          'taskbar'
        Anchors = [akRight, akBottom]
        Caption = 'Hide the server (as tray icon)'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 5
        OnClick = HideButtonClick
      end
      object About2: TButton
        Left = 624
        Top = 440
        Width = 153
        Height = 25
        Hint = 'Short description and some copyright information.'
        Anchors = [akRight, akBottom]
        Caption = 'About this server'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 6
        OnClick = About1Click
      end
      object Close2: TButton
        Left = 624
        Top = 487
        Width = 153
        Height = 25
        Hint = 'Close the application.'
        Anchors = [akRight, akBottom]
        Caption = 'Close the server'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 7
        OnClick = Close1Click
      end
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 585
        Height = 526
        Align = alLeft
        Anchors = [akLeft, akTop, akRight, akBottom]
        BevelOuter = bvLowered
        TabOrder = 8
        DesignSize = (
          585
          526)
        object InstallationMemo: TMemo
          Left = 8
          Top = 8
          Width = 569
          Height = 510
          Hint = 'Shows output from DICOM installation applications'
          Anchors = [akLeft, akTop, akRight, akBottom]
          ParentShowHint = False
          ScrollBars = ssBoth
          ShowHint = True
          TabOrder = 0
          WordWrap = False
        end
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Maintenance'
      DesignSize = (
        810
        526)
      object ReInitDatabaseButton: TButton
        Left = 624
        Top = 16
        Width = 153
        Height = 25
        Hint = 
          'Delete current tables (if present) and rescan all images to rege' +
          'nerate all tables without loss of information (can take a long t' +
          'ime)'
        Anchors = [akTop, akRight]
        Caption = '(Re)-initialize database'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
        OnClick = ReInitDatabaseButtonClick
      end
      object ShowDeviceButton: TButton
        Left = 624
        Top = 57
        Width = 153
        Height = 25
        Hint = 'Show free space on used disk device(s)'
        Anchors = [akTop, akRight]
        Caption = 'Show free space on device'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
        OnClick = ShowDeviceButtonClick
      end
      object ListDICOMprovidersButton: TButton
        Left = 624
        Top = 98
        Width = 153
        Height = 25
        Hint = 
          'List all known systems which can access the Conquest DICOM serve' +
          'r'
        Anchors = [akTop, akRight]
        Caption = 'List DICOM providers'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 2
        OnClick = ListDICOMprovidersButtonClick
      end
      object ListDatabaseLayioutButton: TButton
        Left = 624
        Top = 139
        Width = 153
        Height = 25
        Hint = 'Shows internal layout of the DICOM database'
        Anchors = [akTop, akRight]
        Caption = 'List Database Layout'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 3
        OnClick = ListDatabaseLayioutButtonClick
      end
      object ButtonPackDatabase: TButton
        Left = 624
        Top = 180
        Width = 153
        Height = 25
        Hint = 
          'Deletes empty space from database files (only for dBase without ' +
          'ODBC driver) - improves performance'
        Anchors = [akTop, akRight]
        Caption = 'Pack image database'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 4
        OnClick = ButtonPackDatabaseClick
      end
      object ButtonClearDatabase: TButton
        Left = 624
        Top = 221
        Width = 153
        Height = 25
        Hint = 'Make the database empty - afterwards use '#39'Regen single device'#39
        Anchors = [akTop, akRight]
        Caption = 'Clear image database'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 5
        OnClick = ButtonClearDatabaseClick
      end
      object RegenSelectedButton: TButton
        Left = 624
        Top = 262
        Width = 153
        Height = 25
        Hint = 'Refresh or regenerate the database for selected disk'
        Anchors = [akTop, akRight]
        Caption = 'Regen single device'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 6
        OnClick = RegenSelectedButtonClick
      end
      object Button9: TButton
        Left = 624
        Top = 392
        Width = 153
        Height = 25
        Hint = 
          'Restore the window by clicking on the Conquest tray icon on the ' +
          'taskbar'
        Anchors = [akRight, akBottom]
        Caption = 'Hide the server (as tray icon)'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 7
        OnClick = HideButtonClick
      end
      object Button2: TButton
        Left = 624
        Top = 440
        Width = 153
        Height = 25
        Hint = 'Short description and some copyright information.'
        Anchors = [akRight, akBottom]
        Caption = 'About this server'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 8
        OnClick = About1Click
      end
      object Button3: TButton
        Left = 624
        Top = 487
        Width = 153
        Height = 25
        Hint = 'Close the application.'
        Anchors = [akRight, akBottom]
        Caption = 'Close the server'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 9
        OnClick = Close1Click
      end
      object Panel3: TPanel
        Left = 0
        Top = 0
        Width = 585
        Height = 526
        Align = alLeft
        Anchors = [akLeft, akTop, akRight, akBottom]
        BevelOuter = bvLowered
        Caption = 'Panel3'
        TabOrder = 10
        DesignSize = (
          585
          526)
        object MaintenanceMemo: TMemo
          Left = 8
          Top = 8
          Width = 569
          Height = 510
          Hint = 'Shows output from DICOM maintenance applications'
          Anchors = [akLeft, akTop, akRight, akBottom]
          ParentShowHint = False
          ScrollBars = ssBoth
          ShowHint = True
          TabOrder = 0
          WordWrap = False
        end
      end
      object ButtonRegenFolder: TButton
        Left = 624
        Top = 304
        Width = 153
        Height = 25
        Hint = 
          'Refresh or regenerate the database for selected device and folde' +
          'r (alt-click to use indirect file)'
        Anchors = [akTop, akRight]
        Caption = 'Regen single folder'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 11
        OnClick = ButtonRegenFolderClick
        OnMouseDown = ButtonRegenFolderMouseDown
      end
    end
    object TabSheet4: TTabSheet
      Caption = 'Known DICOM providers'
      DesignSize = (
        810
        526)
      object Button1: TButton
        Left = 624
        Top = 16
        Width = 153
        Height = 25
        Hint = 
          'Stores the list on this page. Unstored changes are not functiona' +
          'l'
        Anchors = [akTop, akRight]
        Caption = 'Save this list'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
        OnClick = Button1Click
      end
      object RestoreListButton: TButton
        Left = 624
        Top = 64
        Width = 153
        Height = 25
        Hint = 'Discards all changes made to the last situation which was stored'
        Anchors = [akTop, akRight]
        Caption = 'Restore this list'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
        OnClick = RestoreListButtonClick
      end
      object Button10: TButton
        Left = 624
        Top = 392
        Width = 153
        Height = 25
        Hint = 
          'Restore the window by clicking on the Conquest tray icon on the ' +
          'taskbar'
        Anchors = [akRight, akBottom]
        Caption = 'Hide the server (as tray icon)'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 2
        OnClick = HideButtonClick
      end
      object Button4: TButton
        Left = 624
        Top = 440
        Width = 153
        Height = 25
        Hint = 'Short description and some copyright information.'
        Anchors = [akRight, akBottom]
        Caption = 'About this server'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 3
        OnClick = About1Click
      end
      object Button5: TButton
        Left = 624
        Top = 487
        Width = 153
        Height = 25
        Hint = 'Close the application.'
        Anchors = [akRight, akBottom]
        Caption = 'Close the server'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 4
        OnClick = Close1Click
      end
      object Panel4: TPanel
        Left = 0
        Top = 0
        Width = 585
        Height = 526
        Align = alLeft
        Anchors = [akLeft, akTop, akRight, akBottom]
        BevelOuter = bvLowered
        TabOrder = 5
        DesignSize = (
          585
          526)
        object DICOMMap: TMemo
          Left = 8
          Top = 8
          Width = 569
          Height = 510
          Hint = 
            'Editor for the list of DICOM systems that may retrieve or receiv' +
            'e images from this server'
          Anchors = [akLeft, akTop, akRight, akBottom]
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'Courier'
          Font.Style = []
          ParentFont = False
          ParentShowHint = False
          ScrollBars = ssVertical
          ShowHint = True
          TabOrder = 0
          WantTabs = True
        end
      end
    end
    object TabSheet5: TTabSheet
      Caption = 'Browse database'
      object Panel5: TPanel
        Left = 0
        Top = 0
        Width = 810
        Height = 526
        Align = alClient
        BevelOuter = bvLowered
        TabOrder = 0
        DesignSize = (
          810
          526)
        object Label6: TLabel
          Left = 8
          Top = 132
          Width = 33
          Height = 13
          Caption = 'Patient'
        end
        object Label7: TLabel
          Left = 8
          Top = 156
          Width = 27
          Height = 13
          Caption = 'Study'
        end
        object Label8: TLabel
          Left = 8
          Top = 316
          Width = 29
          Height = 13
          Caption = 'Series'
        end
        object Label9: TLabel
          Left = 8
          Top = 508
          Width = 29
          Height = 13
          Anchors = [akLeft, akBottom]
          Caption = 'Image'
        end
        object Image1: TImage
          Left = 296
          Top = 8
          Width = 513
          Height = 513
          Hint = 
            'Preview of slice: contrast automatically stretched - right click' +
            ' = menu; click open image; wheel scrolls after click'
          Anchors = [akLeft, akTop, akRight, akBottom]
          ParentShowHint = False
          PopupMenu = PopupMenu1
          ShowHint = True
          Stretch = True
          OnClick = Image1Click
          OnMouseEnter = Image1MouseEnter
          OnMouseMove = Image1MouseMove
        end
        object DBText1: TDBText
          Left = 200
          Top = 154
          Width = 64
          Height = 17
          Hint = 'Navigate through series list'
          DataField = 'STUDYDATE'
          DataSource = DataSource2
          ParentShowHint = False
          ShowHint = True
          OnDblClick = DBText1DblClick
        end
        object LabelStudySeq: TLabel
          Left = 284
          Top = 154
          Width = 6
          Height = 13
          Hint = 'Sequence number of active study'
          Alignment = taRightJustify
          Caption = '1'
          ParentShowHint = False
          ShowHint = True
        end
        object LabelSeriesSequence: TLabel
          Left = 284
          Top = 318
          Width = 6
          Height = 13
          Hint = 'Sequence number of active series'
          Alignment = taRightJustify
          Caption = '1'
          ParentShowHint = False
          ShowHint = True
        end
        object Label25: TLabel
          Left = 300
          Top = 13
          Width = 3
          Height = 13
          Transparent = False
          Visible = False
        end
        object LabelLister: TStaticText
          Left = 296
          Top = 8
          Width = 513
          Height = 513
          Hint = 'Header of object that cannot be displayed'
          Anchors = [akLeft, akTop, akRight, akBottom]
          AutoSize = False
          BorderStyle = sbsSunken
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
          ParentShowHint = False
          PopupMenu = PopupMenu1
          ShowHint = True
          TabOrder = 13
          Visible = False
          OnClick = LabelListerClick
        end
        object DBGrid2: TDBGrid
          Left = 9
          Top = 6
          Width = 281
          Height = 120
          Hint = 'List of patients in the database'
          DataSource = DataSource1
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          Options = [dgTitles, dgIndicator, dgColumnResize, dgColLines, dgRowLines, dgTabs, dgRowSelect, dgConfirmDelete, dgCancelOnExit, dgTitleClick]
          ParentFont = False
          ParentShowHint = False
          ReadOnly = True
          ShowHint = True
          TabOrder = 0
          TitleFont.Charset = DEFAULT_CHARSET
          TitleFont.Color = clWindowText
          TitleFont.Height = -11
          TitleFont.Name = 'MS Sans Serif'
          TitleFont.Style = []
          OnDblClick = DBGrid2DblClick
          OnTitleClick = DBGrid2TitleClick
          Columns = <
            item
              Expanded = False
              FieldName = 'PatientID'
              Width = 96
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'PatientNam'
              Width = 185
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'PatientBir'
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'PatientSex'
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'AccessTime'
              Visible = True
            end>
        end
        object DBNavigator1: TDBNavigator
          Left = 48
          Top = 128
          Width = 120
          Height = 22
          Hint = 'Navigate through patient list'
          DataSource = DataSource1
          VisibleButtons = [nbFirst, nbPrior, nbNext, nbLast]
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
        end
        object ComboBox1: TComboBox
          Left = 172
          Top = 128
          Width = 117
          Height = 21
          Hint = 'Select patient -or- Filter list Patient ID or name'
          DropDownCount = 30
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ParentShowHint = False
          ShowHint = True
          TabOrder = 2
          OnChange = ComboBox1Change
          OnExit = ComboBox1Exit
          OnKeyDown = ComboBox1KeyDown
        end
        object DBNavigator2: TDBNavigator
          Left = 48
          Top = 152
          Width = 120
          Height = 21
          Hint = 'Navigate through study list'
          DataSource = DataSource2
          VisibleButtons = [nbFirst, nbPrior, nbNext, nbLast]
          ParentShowHint = False
          ShowHint = True
          TabOrder = 3
        end
        object DBLookupComboBox2: TDBLookupComboBox
          Left = 172
          Top = 152
          Width = 20
          Height = 21
          DropDownRows = 15
          DropDownWidth = 200
          KeyField = 'STUDYINSTA'
          ListField = 'STUDYDATE'
          ListSource = DataSource2
          TabOrder = 4
          OnClick = DBLookupComboBox2Click
        end
        object DBGrid1: TDBGrid
          Left = 8
          Top = 176
          Width = 281
          Height = 137
          Hint = 'List of series for selected patient and study in the database'
          DataSource = DataSource3
          Options = [dgTitles, dgIndicator, dgColumnResize, dgColLines, dgRowLines, dgTabs, dgRowSelect, dgConfirmDelete, dgCancelOnExit, dgTitleClick]
          ParentShowHint = False
          ReadOnly = True
          ShowHint = True
          TabOrder = 5
          TitleFont.Charset = DEFAULT_CHARSET
          TitleFont.Color = clWindowText
          TitleFont.Height = -11
          TitleFont.Name = 'MS Sans Serif'
          TitleFont.Style = []
          OnCellClick = DBGrid1CellClick
          OnDblClick = DBGrid1DblClick
          OnTitleClick = DBGrid1TitleClick
          Columns = <
            item
              Expanded = False
              FieldName = 'SeriesNumb'
              Width = 42
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'SeriesDate'
              Title.Caption = 'DATE'
              Width = 52
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'SeriesTime'
              Title.Caption = 'TIME'
              Width = 48
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'Modality'
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'SeriesDesc'
              Title.Caption = 'Description'
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'PatientPos'
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'ContrastBo'
              Width = 100
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'SeriesInst'
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'StudyInsta'
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'AccessTime'
              Visible = True
            end>
        end
        object DBNavigator3: TDBNavigator
          Left = 48
          Top = 312
          Width = 120
          Height = 20
          Hint = 'Navigate through series list'
          DataSource = DataSource3
          VisibleButtons = [nbFirst, nbPrior, nbNext, nbLast]
          ParentShowHint = False
          ShowHint = True
          TabOrder = 6
        end
        object DBLookupComboBox3: TDBLookupComboBox
          Left = 172
          Top = 312
          Width = 20
          Height = 21
          DropDownRows = 15
          DropDownWidth = 200
          KeyField = 'SERIESINST'
          ListField = 'SERIESNUMB'
          ListSource = DataSource3
          TabOrder = 7
        end
        object DBGrid3: TDBGrid
          Left = 9
          Top = 336
          Width = 281
          Height = 169
          Hint = 
            'List of images for selected patient, study and series in the dat' +
            'abase'
          Anchors = [akLeft, akTop, akBottom]
          DataSource = DataSource4
          Options = [dgTitles, dgIndicator, dgColumnResize, dgColLines, dgRowLines, dgTabs, dgRowSelect, dgConfirmDelete, dgCancelOnExit, dgTitleClick]
          ParentShowHint = False
          ReadOnly = True
          ShowHint = True
          TabOrder = 8
          TitleFont.Charset = DEFAULT_CHARSET
          TitleFont.Color = clWindowText
          TitleFont.Height = -11
          TitleFont.Name = 'MS Sans Serif'
          TitleFont.Style = []
          OnTitleClick = DBGrid3TitleClick
          Columns = <
            item
              Expanded = False
              FieldName = 'ImageNumbe'
              Title.Caption = 'Im. Number'
              Width = 60
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'ImageDate'
              Title.Caption = 'DATE'
              Width = 52
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'ImageTime'
              Title.Caption = 'TIME'
              Width = 48
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'EchoNumber'
              Title.Caption = 'Echo'
              Width = 38
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'ImageID'
              Title.Caption = 'Image ID'
              Width = 60
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'NumberOfFr'
              Title.Caption = 'Frames'
              Width = 60
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'SOPInstanc'
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'SOPClassUI'
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'SeriesInst'
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'AccessTime'
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'ObjectFile'
              Visible = True
            end
            item
              Expanded = False
              FieldName = 'DeviceName'
              Visible = True
            end>
        end
        object DBNavigator4: TDBNavigator
          Left = 48
          Top = 504
          Width = 120
          Height = 20
          Hint = 'Navigate through image list'
          DataSource = DataSource4
          VisibleButtons = [nbFirst, nbPrior, nbNext, nbLast]
          Anchors = [akLeft, akBottom]
          ParentShowHint = False
          ShowHint = True
          TabOrder = 9
        end
        object DBLookupComboBox4: TDBLookupComboBox
          Left = 172
          Top = 504
          Width = 20
          Height = 21
          Anchors = [akLeft, akBottom]
          DropDownRows = 30
          DropDownWidth = 200
          KeyField = 'SOPINSTANC'
          ListField = 'IMAGENUMBE'
          ListSource = DataSource4
          TabOrder = 10
        end
        object CheckBoxViewIncoming: TCheckBox
          Left = 200
          Top = 507
          Width = 92
          Height = 17
          Hint = 
            'If set, the images stored by a remote DICOM system are directly ' +
            'shown on this page'
          Anchors = [akLeft, akBottom]
          Caption = 'View incoming'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 11
          OnClick = CheckBoxViewIncomingClick
        end
        object SpinEdit1: TSpinEdit
          Left = 299
          Top = 496
          Width = 49
          Height = 22
          Hint = 'Select frame of multiframe object'
          MaxValue = 0
          MinValue = 0
          ParentShowHint = False
          ShowHint = True
          TabOrder = 12
          Value = 0
          OnChange = SpinEdit1Change
        end
        object ProgressBar2: TProgressBar
          Left = 354
          Top = 496
          Width = 415
          Height = 17
          Hint = 'Progress of image operation'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 14
          Visible = False
        end
      end
    end
    object TabSheet6: TTabSheet
      Caption = 'Server status'
      DesignSize = (
        810
        526)
      object KillAndRestartTheServer: TButton
        Left = 624
        Top = 16
        Width = 153
        Height = 25
        Hint = 
          'Use this button to interrupt processing or to restart if the ser' +
          'ver process has crashed (need to run as administrator if server ' +
          'is installed as service)- right click only kills the server'
        Anchors = [akTop, akRight]
        Caption = 'Kill and restart the server'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
        OnClick = KillAndRestartTheServerClick
        OnMouseDown = KillAndRestartTheServerMouseDown
      end
      object RunInThreadedMode: TButton
        Left = 624
        Top = 112
        Width = 153
        Height = 25
        Hint = 'Currently disabled'
        Anchors = [akTop, akRight]
        Caption = 'Run in threaded mode'
        Enabled = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
        Visible = False
        OnClick = RunInThreadedModeClick
      end
      object ButtonBugReport: TButton
        Left = 624
        Top = 64
        Width = 153
        Height = 25
        Hint = 'Zip interesting files for bug report'
        Anchors = [akTop, akRight]
        Caption = 'Create bug report'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 2
        OnClick = ButtonBugReportClick
        OnMouseDown = ButtonBugReportMouseDown
      end
      object ProgressBar1: TProgressBar
        Left = 624
        Top = 164
        Width = 153
        Height = 16
        Hint = 'Displays print server progress'
        Anchors = [akTop, akRight]
        ParentShowHint = False
        Smooth = True
        ShowHint = True
        TabOrder = 3
      end
      object CheckBoxDebugLog: TCheckBox
        Left = 624
        Top = 208
        Width = 157
        Height = 17
        Hint = 
          'Show some more details on server operation in log window - amoun' +
          't can be varied.'
        Anchors = [akTop, akRight]
        Caption = 'Debug log (more information)'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 4
        OnClick = CheckBoxDebugLogClick
      end
      object UpDownDebugLevel: TUpDown
        Left = 790
        Top = 206
        Width = 15
        Height = 20
        Hint = 
          'Use this control to set the amount of debug information displaye' +
          'd'
        Anchors = [akTop, akRight]
        Max = 4
        ParentShowHint = False
        ShowHint = True
        TabOrder = 5
        OnClick = UpDownDebugLevelClick
      end
      object CheckBoxSmallFonts: TCheckBox
        Left = 624
        Top = 240
        Width = 169
        Height = 17
        Hint = 'Use only if you have good eyes!'
        Anchors = [akTop, akRight]
        Caption = 'Use small fonts in log windows'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 6
        OnClick = CheckBoxSmallFontsClick
      end
      object CheckBoxOnlyLogToFile: TCheckBox
        Left = 624
        Top = 272
        Width = 161
        Height = 17
        Hint = 'When checked logs are only in file '#39'serverstatus.log'#39
        Anchors = [akTop, akRight]
        Caption = 'Only log to file (faster)'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 7
        OnClick = CheckBoxOnlyLogToFileClick
      end
      object Button11: TButton
        Left = 624
        Top = 392
        Width = 153
        Height = 25
        Hint = 
          'Restore the window by clicking on the Conquest tray icon on the ' +
          'taskbar'
        Anchors = [akRight, akBottom]
        Caption = 'Hide the server (as tray icon)'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 8
        OnClick = HideButtonClick
      end
      object Button6: TButton
        Left = 624
        Top = 440
        Width = 153
        Height = 25
        Hint = 'Short description and some copyright information.'
        Anchors = [akRight, akBottom]
        Caption = 'About this server'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 9
        OnClick = About1Click
      end
      object Button7: TButton
        Left = 624
        Top = 487
        Width = 153
        Height = 25
        Hint = 'Close the application.'
        Anchors = [akRight, akBottom]
        Caption = 'Close the server'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 10
        OnClick = Close1Click
      end
      object Panel6: TPanel
        Left = 0
        Top = 0
        Width = 585
        Height = 526
        Align = alLeft
        Anchors = [akLeft, akTop, akRight, akBottom]
        BevelOuter = bvLowered
        TabOrder = 11
        DesignSize = (
          585
          526)
        object ServerStatusMemo: TMemo
          Left = 8
          Top = 8
          Width = 569
          Height = 510
          Hint = 
            'Shows output from DICOM server application (for troubleshooting ' +
            'only)'
          Anchors = [akLeft, akTop, akRight, akBottom]
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ParentShowHint = False
          ScrollBars = ssBoth
          ShowHint = True
          TabOrder = 0
          WordWrap = False
        end
      end
      object CheckBoxWebServer: TCheckBox
        Left = 624
        Top = 304
        Width = 153
        Height = 17
        Hint = 
          'Starts small built-in webserver on port 8086 (right click to sho' +
          'w it)'
        Anchors = [akTop, akRight]
        Caption = 'Enable built-in web server '
        ParentShowHint = False
        ShowHint = True
        TabOrder = 12
        OnClick = CheckBoxWebServerClick
        OnMouseDown = CheckBoxWebServerMouseDown
      end
    end
    object TabSheet7: TTabSheet
      Caption = 'Query / Move'
      DesignSize = (
        810
        526)
      object Label11: TLabel
        Left = 589
        Top = 43
        Width = 50
        Height = 13
        Anchors = [akTop, akRight]
        Caption = 'Patient ID:'
      end
      object Label12: TLabel
        Left = 589
        Top = 68
        Width = 67
        Height = 13
        Anchors = [akTop, akRight]
        Caption = 'Patient Name:'
      end
      object Label13: TLabel
        Left = 589
        Top = 93
        Width = 54
        Height = 13
        Anchors = [akTop, akRight]
        Caption = 'Study date:'
      end
      object Label14: TLabel
        Left = 589
        Top = 118
        Width = 70
        Height = 13
        Hint = 'Double click for other query mode'
        Anchors = [akTop, akRight]
        Caption = 'Series number:'
        ParentShowHint = False
        ShowHint = True
        OnDblClick = Label14DblClick
      end
      object Label15: TLabel
        Left = 589
        Top = 200
        Width = 73
        Height = 13
        Anchors = [akTop, akRight]
        Caption = 'DICOM system:'
      end
      object Label16: TLabel
        Left = 591
        Top = 306
        Width = 56
        Height = 13
        Anchors = [akTop, akRight]
        Caption = 'Destination:'
      end
      object Label17: TLabel
        Left = 589
        Top = 18
        Width = 56
        Height = 13
        Anchors = [akTop, akRight]
        Caption = 'Query level:'
      end
      object Label18: TLabel
        Left = 589
        Top = 168
        Width = 70
        Height = 13
        Hint = 'Double click for other query mode'
        Anchors = [akTop, akRight]
        Caption = 'Image number:'
        ParentShowHint = False
        ShowHint = True
        OnDblClick = Label14DblClick
      end
      object Label38: TLabel
        Left = 589
        Top = 143
        Width = 67
        Height = 13
        Hint = 'Double click for other query mode'
        Anchors = [akTop, akRight]
        Caption = 'S. description:'
        ParentShowHint = False
        ShowHint = True
        OnDblClick = Label14DblClick
      end
      object QueryLevel: TComboBox
        Left = 666
        Top = 14
        Width = 136
        Height = 21
        Hint = 'DICOM table to inspect'
        Style = csDropDownList
        Anchors = [akTop, akRight]
        DropDownCount = 10
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
      end
      object EditQID: TEdit
        Left = 666
        Top = 39
        Width = 136
        Height = 21
        Hint = 'Search item (* is wildcard), may be comma-separated list'
        Anchors = [akTop, akRight]
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
      end
      object EditQName: TEdit
        Left = 666
        Top = 64
        Width = 136
        Height = 21
        Hint = 'Search item (* is wildcard)'
        Anchors = [akTop, akRight]
        ParentShowHint = False
        ShowHint = True
        TabOrder = 2
      end
      object EditQdate: TEdit
        Left = 666
        Top = 89
        Width = 136
        Height = 21
        Hint = 'Search item (* is wildcard)'
        Anchors = [akTop, akRight]
        ParentShowHint = False
        ShowHint = True
        TabOrder = 3
      end
      object EditQNum: TEdit
        Left = 666
        Top = 114
        Width = 136
        Height = 21
        Hint = 'Search item (* is wildcard)'
        Anchors = [akTop, akRight]
        ParentShowHint = False
        ShowHint = True
        TabOrder = 4
      end
      object EditQSDesc: TEdit
        Left = 666
        Top = 139
        Width = 136
        Height = 21
        Hint = 'Search item (* is wildcard)'
        Anchors = [akTop, akRight]
        ParentShowHint = False
        ShowHint = True
        TabOrder = 5
      end
      object EditQIm: TEdit
        Left = 666
        Top = 164
        Width = 136
        Height = 21
        Hint = 'Search item (* is wildcard)'
        Anchors = [akTop, akRight]
        ParentShowHint = False
        ShowHint = True
        TabOrder = 6
      end
      object DicomSystem: TComboBox
        Left = 666
        Top = 196
        Width = 136
        Height = 21
        Hint = 'System to query / source for copy operation'
        Style = csDropDownList
        Anchors = [akTop, akRight]
        ParentShowHint = False
        ShowHint = True
        TabOrder = 7
      end
      object QueryButton: TButton
        Left = 624
        Top = 230
        Width = 153
        Height = 25
        Hint = 'Find data on a selected DICOM system'
        Anchors = [akTop, akRight]
        Caption = 'Query selected DICOM system'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 8
        OnClick = QueryButtonClick
      end
      object ButtonFindMissingPatients: TButton
        Left = 624
        Top = 265
        Width = 153
        Height = 25
        Hint = 
          'Prepares to copy all missing data on the selected DICOM server t' +
          'hat is not on THIS server to the DESTINATION server'
        Anchors = [akTop, akRight]
        Caption = 'Find Local Missing Patients'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 9
        OnClick = ButtonFindMissingPatientsClick
      end
      object TargetSystem: TComboBox
        Left = 668
        Top = 302
        Width = 136
        Height = 21
        Hint = 'Destination for copy operation'
        Style = csDropDownList
        Anchors = [akTop, akRight]
        ParentShowHint = False
        ShowHint = True
        TabOrder = 10
      end
      object MoveButton: TButton
        Left = 624
        Top = 338
        Width = 153
        Height = 25
        Hint = 'Transfer data from selected DICOM system to destination system'
        Anchors = [akTop, akRight]
        Caption = 'Copy to destination'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 11
        OnClick = MoveButtonClick
      end
      object Button12: TButton
        Left = 624
        Top = 392
        Width = 153
        Height = 25
        Hint = 
          'Restore the window by clicking on the Conquest tray icon on the ' +
          'taskbar'
        Anchors = [akRight, akBottom]
        Caption = 'Hide the server (as tray icon)'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 12
        OnClick = HideButtonClick
      end
      object Button13: TButton
        Left = 624
        Top = 440
        Width = 153
        Height = 25
        Hint = 'Short description and some copyright information.'
        Anchors = [akRight, akBottom]
        Caption = 'About this server'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 13
        OnClick = About1Click
      end
      object Button14: TButton
        Left = 624
        Top = 487
        Width = 153
        Height = 25
        Hint = 'Close the application.'
        Anchors = [akRight, akBottom]
        Caption = 'Close the server'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 14
        OnClick = Close1Click
      end
      object Panel7: TPanel
        Left = 0
        Top = 0
        Width = 585
        Height = 526
        Align = alLeft
        Anchors = [akLeft, akTop, akRight, akBottom]
        BevelOuter = bvLowered
        Caption = 'Panel7'
        TabOrder = 15
        DesignSize = (
          585
          526)
        object Memo1: TMemo
          Left = 8
          Top = 8
          Width = 569
          Height = 510
          Hint = 
            'Shows query result from DICOM server; double click entry to make' +
            ' new query'
          Anchors = [akLeft, akTop, akRight, akBottom]
          ParentShowHint = False
          ScrollBars = ssBoth
          ShowHint = True
          TabOrder = 0
          WordWrap = False
          OnDblClick = Memo1DblClick
        end
        object ProgressBar3: TProgressBar
          Left = 66
          Top = 472
          Width = 415
          Height = 17
          Hint = 'Progress of image operation'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
          Visible = False
        end
      end
    end
    object TabSheet8: TTabSheet
      Caption = 'Archiving'
      ImageIndex = 7
      DesignSize = (
        810
        526)
      object SpeedButton1: TSpeedButton
        Left = 400
        Top = 256
        Width = 23
        Height = 22
        AllowAllUp = True
      end
      object ArchiveMemo: TMemo
        Left = 8
        Top = 8
        Width = 569
        Height = 513
        Hint = 'Shows output from DICOM archiving applications'
        Anchors = [akLeft, akTop, akRight, akBottom]
        ParentShowHint = False
        ScrollBars = ssBoth
        ShowHint = True
        TabOrder = 0
        WordWrap = False
      end
      object Panel8: TPanel
        Left = 592
        Top = 328
        Width = 217
        Height = 169
        Hint = 'Configuration for automatic archiving - from JUKEBOX.INI'
        Anchors = [akTop, akRight]
        BevelOuter = bvLowered
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
        object ActiveJukeboxLabel: TLabel
          Left = 5
          Top = 25
          Width = 208
          Height = 13
          AutoSize = False
          Caption = 'ActiveJukebox = ?'
        end
        object JukeBoxPathLabel: TLabel
          Left = 5
          Top = 39
          Width = 208
          Height = 13
          AutoSize = False
          Caption = 'Path = ?'
        end
        object NumberCDsLabel: TLabel
          Left = 5
          Top = 53
          Width = 208
          Height = 13
          AutoSize = False
          Caption = 'NumberCDs ='
        end
        object NextCDToBurnLabel: TLabel
          Left = 5
          Top = 67
          Width = 208
          Height = 13
          AutoSize = False
          Caption = 'NextCDToBurn = ?'
        end
        object CDCapacityMBLabel: TLabel
          Left = 5
          Top = 96
          Width = 208
          Height = 13
          AutoSize = False
          Caption = 'CDCapacityMB = ?'
        end
        object BurnThresholdLabel: TLabel
          Left = 5
          Top = 110
          Width = 208
          Height = 13
          AutoSize = False
          Caption = 'BurnThreshold = ?'
        end
        object BurnTimeLabel: TLabel
          Left = 5
          Top = 124
          Width = 208
          Height = 13
          AutoSize = False
          Caption = 'BurnTime = ?'
        end
        object BurnProgramLabel: TLabel
          Left = 5
          Top = 138
          Width = 208
          Height = 13
          AutoSize = False
          Caption = 'BurnProgram ='
        end
        object Label27: TLabel
          Left = 8
          Top = 4
          Width = 201
          Height = 21
          Alignment = taCenter
          AutoSize = False
          Caption = 'Automatic archiving'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -16
          Font.Name = 'MS Sans Serif'
          Font.Style = [fsBold]
          ParentFont = False
          OnDblClick = Label27DblClick
        end
        object MirrorVerifyTimeLabel: TLabel
          Left = 5
          Top = 152
          Width = 208
          Height = 13
          AutoSize = False
          Caption = 'MirrorVerifyTime = ?'
        end
        object NextSlotToUseLabel: TLabel
          Left = 5
          Top = 81
          Width = 208
          Height = 13
          AutoSize = False
          Caption = 'NextSlotToUse = ?'
        end
      end
      object StaticTextTime: TStaticText
        Left = 592
        Top = 504
        Width = 217
        Height = 17
        Anchors = [akTop, akRight]
        AutoSize = False
        BorderStyle = sbsSunken
        Caption = 'StaticTextTime'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 2
      end
      object Panel9: TPanel
        Left = 592
        Top = 8
        Width = 217
        Height = 313
        Anchors = [akTop, akRight]
        BevelOuter = bvLowered
        TabOrder = 3
        object Label48: TLabel
          Left = 30
          Top = 6
          Width = 135
          Height = 20
          Caption = 'Manual archiving'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -16
          Font.Name = 'MS Sans Serif'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object SelectForArchivingButton: TButton
          Left = 20
          Top = 32
          Width = 153
          Height = 25
          Hint = 'Selects LRU patients to be moved to CDRom'
          Caption = 'Select for archiving'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 0
          OnClick = SelectForArchivingButtonClick
        end
        object UndoArchivingButton: TButton
          Left = 20
          Top = 103
          Width = 153
          Height = 25
          Hint = 'Restores situation after select for archiving'
          Caption = 'Undo select for archiving'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
          OnClick = UndoArchivingButtonClick
        end
        object CreateFileSetForBurningButton: TButton
          Left = 20
          Top = 138
          Width = 153
          Height = 25
          Hint = 'Moves selected patient to cache directory'
          Caption = 'Create file set for burning CD'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 2
          OnClick = CreateFileSetForBurningButtonClick
        end
        object BurnCDButton: TButton
          Left = 20
          Top = 209
          Width = 153
          Height = 25
          Hint = 'Calls program to burn CD from cache directory'
          Caption = 'Burn CD'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 3
          OnClick = BurnCDButtonClick
        end
        object VerifyCDButton: TButton
          Left = 20
          Top = 245
          Width = 153
          Height = 25
          Hint = 'Verify that the CD is correct'
          Caption = 'Verify CD'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 4
          OnClick = VerifyCDButtonClick
        end
        object VerifyAndDeleteButton: TButton
          Left = 20
          Top = 280
          Width = 153
          Height = 25
          Hint = 
            'Verify CD against file set and if ok, delete the file set and in' +
            'crement CD number'
          Caption = 'Verify CD and delete file set'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 5
          OnClick = VerifyAndDeleteButtonClick
        end
        object VerifyMirrorDisk: TButton
          Left = 20
          Top = 67
          Width = 153
          Height = 25
          Hint = 'Verify mirror disk of patients selected for archiving'
          Caption = 'Verify mirror disk'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 6
          OnClick = VerifyMirrorDiskClick
        end
        object BackupToTapeButton: TButton
          Left = 20
          Top = 174
          Width = 153
          Height = 25
          Hint = 'Backup of cache directory to tape'
          Caption = 'Tape backup of CD file set'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 7
          OnClick = BackupToTapeButtonClick
        end
        object CheckBoxSelectForArchiving: TCheckBox
          Left = 184
          Top = 36
          Width = 15
          Height = 17
          Hint = 'Enable operation for automatic archiving'
          Checked = True
          ParentShowHint = False
          ShowHint = True
          State = cbChecked
          TabOrder = 8
        end
        object CheckBoxVerifyMirrorDisk: TCheckBox
          Left = 184
          Top = 71
          Width = 15
          Height = 17
          Hint = 'Enable operation for automatic archiving'
          Checked = True
          ParentShowHint = False
          ShowHint = True
          State = cbChecked
          TabOrder = 9
        end
        object CheckBoxCreateFileSet: TCheckBox
          Left = 184
          Top = 142
          Width = 15
          Height = 17
          Hint = 'Enable operation for automatic archiving'
          Checked = True
          ParentShowHint = False
          ShowHint = True
          State = cbChecked
          TabOrder = 10
        end
        object CheckBoxTapeBackup: TCheckBox
          Left = 184
          Top = 178
          Width = 15
          Height = 17
          Hint = 'Enable operation for automatic archiving'
          Checked = True
          ParentShowHint = False
          ShowHint = True
          State = cbChecked
          TabOrder = 11
        end
        object CheckBoxBurnCD: TCheckBox
          Left = 184
          Top = 213
          Width = 15
          Height = 17
          Hint = 'Enable operation for automatic archiving'
          Checked = True
          ParentShowHint = False
          ShowHint = True
          State = cbChecked
          TabOrder = 12
        end
        object CheckBoxVerifyCDAndDeleteFileSet: TCheckBox
          Left = 184
          Top = 284
          Width = 15
          Height = 17
          Hint = 'Enable operation for automatic archiving'
          Checked = True
          ParentShowHint = False
          ShowHint = True
          State = cbChecked
          TabOrder = 13
        end
      end
    end
    object TapeBackupPage: TTabSheet
      Caption = 'Tape backup'
      ImageIndex = 8
      DesignSize = (
        810
        526)
      object Memo2: TMemo
        Left = 8
        Top = 8
        Width = 569
        Height = 513
        Hint = 'Shows output from DICOM archiving applications'
        Anchors = [akLeft, akTop, akRight, akBottom]
        ParentShowHint = False
        ScrollBars = ssBoth
        ShowHint = True
        TabOrder = 0
        WordWrap = False
      end
      object StaticTextTapeTime: TStaticText
        Left = 592
        Top = 504
        Width = 217
        Height = 17
        Hint = 'Shows current time'
        Anchors = [akRight, akBottom]
        AutoSize = False
        BorderStyle = sbsSunken
        Caption = 'StaticTextTime'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
      end
      object BlankTape: TButton
        Left = 624
        Top = 16
        Width = 153
        Height = 25
        Hint = 'Writes a header to a blank new tape'
        Anchors = [akTop, akRight]
        Caption = 'Initialize Blank Tape'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 2
        OnClick = BlankTapeClick
      end
      object OverwriteTape: TButton
        Left = 624
        Top = 64
        Width = 153
        Height = 25
        Hint = 'Re-use tape without changing its name'
        Anchors = [akTop, akRight]
        Caption = 'Overwrite tape'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 3
        OnClick = OverwriteTapeClick
      end
      object Panel10: TPanel
        Left = 592
        Top = 328
        Width = 217
        Height = 169
        Hint = 'Configuration for tape archiving - from TAPEBACKUP.INI'
        Anchors = [akRight, akBottom]
        BevelOuter = bvLowered
        ParentShowHint = False
        ShowHint = True
        TabOrder = 4
        object TapeDeviceLabel: TLabel
          Left = 5
          Top = 25
          Width = 208
          Height = 13
          AutoSize = False
          Caption = 'Tape device = ?'
        end
        object TapeCapacityMBLabel: TLabel
          Left = 5
          Top = 41
          Width = 208
          Height = 13
          AutoSize = False
          Caption = 'Tape capacity MB = ?'
        end
        object Label29: TLabel
          Left = 8
          Top = 4
          Width = 201
          Height = 21
          Alignment = taCenter
          AutoSize = False
          Caption = 'Tape backup'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -16
          Font.Name = 'MS Sans Serif'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object BackupTimeLabel: TLabel
          Left = 5
          Top = 73
          Width = 208
          Height = 13
          AutoSize = False
          Caption = 'Backup Time = ?'
        end
        object UsedTapeMBLabel: TLabel
          Left = 5
          Top = 57
          Width = 208
          Height = 13
          AutoSize = False
          Caption = 'Used Tape MB = ?'
        end
      end
    end
    object WeeklyChecksPage: TTabSheet
      Tag = 9999
      Caption = 'Weekly Checks'
      ImageIndex = 9
      DesignSize = (
        810
        526)
      object Label20: TLabel
        Tag = 9999
        Left = 481
        Top = 9
        Width = 13
        Height = 13
        Anchors = [akTop, akRight]
        Caption = 'Sa'
      end
      object Label21: TLabel
        Tag = 9999
        Left = 497
        Top = 9
        Width = 13
        Height = 13
        Anchors = [akTop, akRight]
        Caption = 'Su'
      end
      object Label22: TLabel
        Tag = 9999
        Left = 579
        Top = 9
        Width = 9
        Height = 13
        Anchors = [akTop, akRight]
        Caption = 'Fr'
      end
      object Label23: TLabel
        Tag = 9999
        Left = 529
        Top = 9
        Width = 13
        Height = 13
        Anchors = [akTop, akRight]
        Caption = 'Tu'
      end
      object Label24: TLabel
        Tag = 9999
        Left = 513
        Top = 9
        Width = 15
        Height = 13
        Anchors = [akTop, akRight]
        Caption = 'Mo'
      end
      object Label28: TLabel
        Tag = 9999
        Left = 561
        Top = 9
        Width = 13
        Height = 13
        Anchors = [akTop, akRight]
        Caption = 'Th'
      end
      object Label30: TLabel
        Tag = 9999
        Left = 545
        Top = 9
        Width = 17
        Height = 13
        Anchors = [akTop, akRight]
        Caption = 'We'
      end
      object Label32: TLabel
        Tag = 9999
        Left = 773
        Top = 9
        Width = 25
        Height = 13
        Anchors = [akTop, akRight]
        Caption = 'CD#:'
      end
      object Label31: TLabel
        Tag = 9999
        Left = 636
        Top = 9
        Width = 33
        Height = 13
        Anchors = [akTop, akRight]
        Caption = 'Action:'
      end
      object Label33: TLabel
        Tag = 9999
        Left = 594
        Top = 9
        Width = 37
        Height = 13
        Anchors = [akTop, akRight]
        Caption = 'Time(s):'
      end
      object WeeklyMemo: TMemo
        Tag = 9999
        Left = 8
        Top = 8
        Width = 465
        Height = 513
        Hint = 'Shows output from DICOM archiving applications'
        Anchors = [akLeft, akTop, akRight, akBottom]
        ParentShowHint = False
        ScrollBars = ssBoth
        ShowHint = True
        TabOrder = 0
        WordWrap = False
      end
      object CheckBox1: TCheckBox
        Tag = 10
        Left = 481
        Top = 28
        Width = 17
        Height = 17
        Hint = '1'
        Anchors = [akTop, akRight]
        TabOrder = 1
        OnClick = CheckBox1Click
      end
      object CheckBox2: TCheckBox
        Tag = 11
        Left = 497
        Top = 28
        Width = 17
        Height = 17
        Hint = '1'
        Anchors = [akTop, akRight]
        TabOrder = 2
        OnClick = CheckBox1Click
      end
      object CheckBox3: TCheckBox
        Tag = 12
        Left = 513
        Top = 28
        Width = 17
        Height = 17
        Hint = '1'
        Anchors = [akTop, akRight]
        TabOrder = 3
        OnClick = CheckBox1Click
      end
      object CheckBox4: TCheckBox
        Tag = 13
        Left = 529
        Top = 28
        Width = 17
        Height = 17
        Hint = '1'
        Anchors = [akTop, akRight]
        TabOrder = 4
        OnClick = CheckBox1Click
      end
      object CheckBox6: TCheckBox
        Tag = 14
        Left = 545
        Top = 28
        Width = 17
        Height = 17
        Hint = '1'
        Anchors = [akTop, akRight]
        TabOrder = 5
        OnClick = CheckBox1Click
      end
      object CheckBox5: TCheckBox
        Tag = 15
        Left = 561
        Top = 28
        Width = 17
        Height = 17
        Hint = '1'
        Anchors = [akTop, akRight]
        TabOrder = 6
        OnClick = CheckBox1Click
      end
      object CheckBox7: TCheckBox
        Tag = 16
        Left = 577
        Top = 28
        Width = 17
        Height = 17
        Hint = '1'
        Anchors = [akTop, akRight]
        TabOrder = 7
        OnClick = CheckBox1Click
      end
      object ComboBox2: TComboBox
        Tag = 8
        Left = 634
        Top = 23
        Width = 137
        Height = 21
        Hint = '1'
        Style = csDropDownList
        Anchors = [akTop, akRight]
        TabOrder = 8
        OnChange = ComboBox2Change
        Items.Strings = (
          'None'
          'Check Mirror0'
          'Check Mirror1'
          'Check Mirror2'
          'Check Mirror3'
          'Check Mirror4'
          'Check Mirror5'
          'Check Mirror6'
          'Check Mirror7'
          'Check Mirror8'
          'Check Mirror9'
          'Test MAG0'
          'Test MAG1'
          'Test MAG2'
          'Test MAG3'
          'Test MAG4'
          'Test MAG5'
          'Test MAG6'
          'Test MAG7'
          'Test MAG8'
          'Test MAG9'
          'Test Jukebox0'
          'Test Jukebox1'
          'Test Jukebox2'
          'Test Jukebox3'
          'Test Jukebox4'
          'Test Jukebox5'
          'Test Jukebox6'
          'Test Jukebox7'
          'Test Jukebox8'
          'Test Jukebox9'
          'Kill and Restart the Server'
          'Pack the Database')
      end
      object Edit1: TEdit
        Tag = 7
        Left = 593
        Top = 23
        Width = 39
        Height = 21
        Hint = '1'
        Anchors = [akTop, akRight]
        TabOrder = 9
        Text = '23:00'
        OnChange = Edit1Change
        OnDblClick = Edit1DblClick
      end
      object Edit2: TEdit
        Tag = 9
        Left = 772
        Top = 23
        Width = 31
        Height = 21
        Hint = '1'
        Anchors = [akTop, akRight]
        Color = clBtnFace
        Enabled = False
        TabOrder = 10
        OnChange = Edit2Change
      end
      object CheckBox8: TCheckBox
        Tag = 10
        Left = 481
        Top = 52
        Width = 17
        Height = 17
        Hint = '2'
        Anchors = [akTop, akRight]
        TabOrder = 11
        OnClick = CheckBox1Click
      end
      object CheckBox9: TCheckBox
        Tag = 11
        Left = 497
        Top = 52
        Width = 17
        Height = 17
        Hint = '2'
        Anchors = [akTop, akRight]
        TabOrder = 12
        OnClick = CheckBox1Click
      end
      object CheckBox10: TCheckBox
        Tag = 12
        Left = 513
        Top = 52
        Width = 17
        Height = 17
        Hint = '2'
        Anchors = [akTop, akRight]
        TabOrder = 13
        OnClick = CheckBox1Click
      end
      object CheckBox11: TCheckBox
        Tag = 13
        Left = 529
        Top = 52
        Width = 17
        Height = 17
        Hint = '2'
        Anchors = [akTop, akRight]
        TabOrder = 14
        OnClick = CheckBox1Click
      end
      object CheckBox12: TCheckBox
        Tag = 14
        Left = 545
        Top = 52
        Width = 17
        Height = 17
        Hint = '2'
        Anchors = [akTop, akRight]
        TabOrder = 15
        OnClick = CheckBox1Click
      end
      object CheckBox13: TCheckBox
        Tag = 15
        Left = 561
        Top = 52
        Width = 17
        Height = 17
        Hint = '2'
        Anchors = [akTop, akRight]
        TabOrder = 16
        OnClick = CheckBox1Click
      end
      object CheckBox14: TCheckBox
        Tag = 16
        Left = 577
        Top = 52
        Width = 17
        Height = 17
        Hint = '2'
        Anchors = [akTop, akRight]
        TabOrder = 17
        OnClick = CheckBox1Click
      end
      object ComboBox3: TComboBox
        Tag = 8
        Left = 634
        Top = 47
        Width = 137
        Height = 21
        Hint = '2'
        Style = csDropDownList
        Anchors = [akTop, akRight]
        TabOrder = 18
        OnChange = ComboBox2Change
        Items.Strings = (
          'None'
          'Check Mirror0'
          'Check Mirror1'
          'Check Mirror2'
          'Check Mirror3'
          'Check Mirror4'
          'Check Mirror5'
          'Check Mirror6'
          'Check Mirror7'
          'Check Mirror8'
          'Check Mirror9'
          'Test MAG0'
          'Test MAG1'
          'Test MAG2'
          'Test MAG3'
          'Test MAG4'
          'Test MAG5'
          'Test MAG6'
          'Test MAG7'
          'Test MAG8'
          'Test MAG9'
          'Test Jukebox0'
          'Test Jukebox1'
          'Test Jukebox2'
          'Test Jukebox3'
          'Test Jukebox4'
          'Test Jukebox5'
          'Test Jukebox6'
          'Test Jukebox7'
          'Test Jukebox8'
          'Test Jukebox9'
          'Kill and Restart the Server'
          'Pack the Database')
      end
      object Edit3: TEdit
        Tag = 7
        Left = 593
        Top = 47
        Width = 39
        Height = 21
        Hint = '2'
        Anchors = [akTop, akRight]
        TabOrder = 19
        Text = '23:00'
        OnChange = Edit1Change
        OnDblClick = Edit1DblClick
      end
      object Edit4: TEdit
        Tag = 9
        Left = 772
        Top = 47
        Width = 31
        Height = 21
        Hint = '2'
        Anchors = [akTop, akRight]
        Color = clBtnFace
        Enabled = False
        TabOrder = 20
        OnChange = Edit2Change
      end
      object CheckBox15: TCheckBox
        Tag = 10
        Left = 481
        Top = 76
        Width = 17
        Height = 17
        Hint = '3'
        Anchors = [akTop, akRight]
        TabOrder = 21
        OnClick = CheckBox1Click
      end
      object CheckBox16: TCheckBox
        Tag = 11
        Left = 497
        Top = 76
        Width = 17
        Height = 17
        Hint = '3'
        Anchors = [akTop, akRight]
        TabOrder = 22
        OnClick = CheckBox1Click
      end
      object CheckBox17: TCheckBox
        Tag = 12
        Left = 513
        Top = 76
        Width = 17
        Height = 17
        Hint = '3'
        Anchors = [akTop, akRight]
        TabOrder = 23
        OnClick = CheckBox1Click
      end
      object CheckBox18: TCheckBox
        Tag = 13
        Left = 529
        Top = 76
        Width = 17
        Height = 17
        Hint = '3'
        Anchors = [akTop, akRight]
        TabOrder = 24
        OnClick = CheckBox1Click
      end
      object CheckBox19: TCheckBox
        Tag = 14
        Left = 545
        Top = 76
        Width = 17
        Height = 17
        Hint = '3'
        Anchors = [akTop, akRight]
        TabOrder = 25
        OnClick = CheckBox1Click
      end
      object CheckBox20: TCheckBox
        Tag = 15
        Left = 561
        Top = 76
        Width = 17
        Height = 17
        Hint = '3'
        Anchors = [akTop, akRight]
        TabOrder = 26
        OnClick = CheckBox1Click
      end
      object CheckBox21: TCheckBox
        Tag = 16
        Left = 577
        Top = 76
        Width = 17
        Height = 17
        Hint = '3'
        Anchors = [akTop, akRight]
        TabOrder = 27
        OnClick = CheckBox1Click
      end
      object ComboBox4: TComboBox
        Tag = 8
        Left = 634
        Top = 71
        Width = 137
        Height = 21
        Hint = '3'
        Style = csDropDownList
        Anchors = [akTop, akRight]
        TabOrder = 28
        OnChange = ComboBox2Change
        Items.Strings = (
          'None'
          'Check Mirror0'
          'Check Mirror1'
          'Check Mirror2'
          'Check Mirror3'
          'Check Mirror4'
          'Check Mirror5'
          'Check Mirror6'
          'Check Mirror7'
          'Check Mirror8'
          'Check Mirror9'
          'Test MAG0'
          'Test MAG1'
          'Test MAG2'
          'Test MAG3'
          'Test MAG4'
          'Test MAG5'
          'Test MAG6'
          'Test MAG7'
          'Test MAG8'
          'Test MAG9'
          'Test Jukebox0'
          'Test Jukebox1'
          'Test Jukebox2'
          'Test Jukebox3'
          'Test Jukebox4'
          'Test Jukebox5'
          'Test Jukebox6'
          'Test Jukebox7'
          'Test Jukebox8'
          'Test Jukebox9'
          'Kill and Restart the Server'
          'Pack the Database')
      end
      object Edit5: TEdit
        Tag = 7
        Left = 593
        Top = 71
        Width = 39
        Height = 21
        Hint = '3'
        Anchors = [akTop, akRight]
        TabOrder = 29
        Text = '23:00'
        OnChange = Edit1Change
        OnDblClick = Edit1DblClick
      end
      object Edit6: TEdit
        Tag = 9
        Left = 772
        Top = 71
        Width = 31
        Height = 21
        Hint = '3'
        Anchors = [akTop, akRight]
        Color = clBtnFace
        Enabled = False
        TabOrder = 30
        OnChange = Edit2Change
      end
      object CheckBox22: TCheckBox
        Tag = 10
        Left = 481
        Top = 100
        Width = 17
        Height = 17
        Hint = '4'
        Anchors = [akTop, akRight]
        TabOrder = 31
        OnClick = CheckBox1Click
      end
      object CheckBox23: TCheckBox
        Tag = 11
        Left = 497
        Top = 100
        Width = 17
        Height = 17
        Hint = '4'
        Anchors = [akTop, akRight]
        TabOrder = 32
        OnClick = CheckBox1Click
      end
      object CheckBox24: TCheckBox
        Tag = 12
        Left = 513
        Top = 100
        Width = 17
        Height = 17
        Hint = '4'
        Anchors = [akTop, akRight]
        TabOrder = 33
        OnClick = CheckBox1Click
      end
      object CheckBox25: TCheckBox
        Tag = 13
        Left = 529
        Top = 100
        Width = 17
        Height = 17
        Hint = '4'
        Anchors = [akTop, akRight]
        TabOrder = 34
        OnClick = CheckBox1Click
      end
      object CheckBox26: TCheckBox
        Tag = 14
        Left = 545
        Top = 100
        Width = 17
        Height = 17
        Hint = '4'
        Anchors = [akTop, akRight]
        TabOrder = 35
        OnClick = CheckBox1Click
      end
      object CheckBox27: TCheckBox
        Tag = 15
        Left = 561
        Top = 100
        Width = 17
        Height = 17
        Hint = '4'
        Anchors = [akTop, akRight]
        TabOrder = 36
        OnClick = CheckBox1Click
      end
      object CheckBox28: TCheckBox
        Tag = 16
        Left = 577
        Top = 100
        Width = 17
        Height = 17
        Hint = '4'
        Anchors = [akTop, akRight]
        TabOrder = 37
        OnClick = CheckBox1Click
      end
      object ComboBox5: TComboBox
        Tag = 8
        Left = 634
        Top = 95
        Width = 137
        Height = 21
        Hint = '4'
        Style = csDropDownList
        Anchors = [akTop, akRight]
        TabOrder = 38
        OnChange = ComboBox2Change
        Items.Strings = (
          'None'
          'Check Mirror0'
          'Check Mirror1'
          'Check Mirror2'
          'Check Mirror3'
          'Check Mirror4'
          'Check Mirror5'
          'Check Mirror6'
          'Check Mirror7'
          'Check Mirror8'
          'Check Mirror9'
          'Test MAG0'
          'Test MAG1'
          'Test MAG2'
          'Test MAG3'
          'Test MAG4'
          'Test MAG5'
          'Test MAG6'
          'Test MAG7'
          'Test MAG8'
          'Test MAG9'
          'Test Jukebox0'
          'Test Jukebox1'
          'Test Jukebox2'
          'Test Jukebox3'
          'Test Jukebox4'
          'Test Jukebox5'
          'Test Jukebox6'
          'Test Jukebox7'
          'Test Jukebox8'
          'Test Jukebox9'
          'Kill and Restart the Server'
          'Pack the Database')
      end
      object Edit7: TEdit
        Tag = 7
        Left = 593
        Top = 95
        Width = 39
        Height = 21
        Hint = '4'
        Anchors = [akTop, akRight]
        TabOrder = 39
        Text = '23:00'
        OnChange = Edit1Change
        OnDblClick = Edit1DblClick
      end
      object Edit8: TEdit
        Tag = 9
        Left = 772
        Top = 95
        Width = 31
        Height = 21
        Hint = '4'
        Anchors = [akTop, akRight]
        Color = clBtnFace
        Enabled = False
        TabOrder = 40
        OnChange = Edit2Change
      end
      object CheckBox29: TCheckBox
        Tag = 10
        Left = 481
        Top = 124
        Width = 17
        Height = 17
        Hint = '5'
        Anchors = [akTop, akRight]
        TabOrder = 41
        OnClick = CheckBox1Click
      end
      object CheckBox30: TCheckBox
        Tag = 11
        Left = 497
        Top = 124
        Width = 17
        Height = 17
        Hint = '5'
        Anchors = [akTop, akRight]
        TabOrder = 42
        OnClick = CheckBox1Click
      end
      object CheckBox31: TCheckBox
        Tag = 12
        Left = 513
        Top = 124
        Width = 17
        Height = 17
        Hint = '5'
        Anchors = [akTop, akRight]
        TabOrder = 43
        OnClick = CheckBox1Click
      end
      object CheckBox32: TCheckBox
        Tag = 13
        Left = 529
        Top = 124
        Width = 17
        Height = 17
        Hint = '5'
        Anchors = [akTop, akRight]
        TabOrder = 44
        OnClick = CheckBox1Click
      end
      object CheckBox33: TCheckBox
        Tag = 14
        Left = 545
        Top = 124
        Width = 17
        Height = 17
        Hint = '5'
        Anchors = [akTop, akRight]
        TabOrder = 45
        OnClick = CheckBox1Click
      end
      object CheckBox34: TCheckBox
        Tag = 15
        Left = 561
        Top = 124
        Width = 17
        Height = 17
        Hint = '5'
        Anchors = [akTop, akRight]
        TabOrder = 46
        OnClick = CheckBox1Click
      end
      object CheckBox35: TCheckBox
        Tag = 16
        Left = 577
        Top = 124
        Width = 17
        Height = 17
        Hint = '5'
        Anchors = [akTop, akRight]
        TabOrder = 47
        OnClick = CheckBox1Click
      end
      object ComboBox6: TComboBox
        Tag = 8
        Left = 634
        Top = 119
        Width = 137
        Height = 21
        Hint = '5'
        Style = csDropDownList
        Anchors = [akTop, akRight]
        TabOrder = 48
        OnChange = ComboBox2Change
        Items.Strings = (
          'None'
          'Check Mirror0'
          'Check Mirror1'
          'Check Mirror2'
          'Check Mirror3'
          'Check Mirror4'
          'Check Mirror5'
          'Check Mirror6'
          'Check Mirror7'
          'Check Mirror8'
          'Check Mirror9'
          'Test MAG0'
          'Test MAG1'
          'Test MAG2'
          'Test MAG3'
          'Test MAG4'
          'Test MAG5'
          'Test MAG6'
          'Test MAG7'
          'Test MAG8'
          'Test MAG9'
          'Test Jukebox0'
          'Test Jukebox1'
          'Test Jukebox2'
          'Test Jukebox3'
          'Test Jukebox4'
          'Test Jukebox5'
          'Test Jukebox6'
          'Test Jukebox7'
          'Test Jukebox8'
          'Test Jukebox9'
          'Kill and Restart the Server'
          'Pack the Database')
      end
      object Edit9: TEdit
        Tag = 7
        Left = 593
        Top = 119
        Width = 39
        Height = 21
        Hint = '5'
        Anchors = [akTop, akRight]
        TabOrder = 49
        Text = '23:00'
        OnChange = Edit1Change
        OnDblClick = Edit1DblClick
      end
      object Edit10: TEdit
        Tag = 9
        Left = 772
        Top = 119
        Width = 31
        Height = 21
        Hint = '5'
        Anchors = [akTop, akRight]
        Color = clBtnFace
        Enabled = False
        TabOrder = 50
        OnChange = Edit2Change
      end
      object CheckBox36: TCheckBox
        Tag = 10
        Left = 481
        Top = 148
        Width = 17
        Height = 17
        Hint = '6'
        Anchors = [akTop, akRight]
        TabOrder = 51
        OnClick = CheckBox1Click
      end
      object CheckBox37: TCheckBox
        Tag = 11
        Left = 497
        Top = 148
        Width = 17
        Height = 17
        Hint = '6'
        Anchors = [akTop, akRight]
        TabOrder = 52
        OnClick = CheckBox1Click
      end
      object CheckBox38: TCheckBox
        Tag = 12
        Left = 513
        Top = 148
        Width = 17
        Height = 17
        Hint = '6'
        Anchors = [akTop, akRight]
        TabOrder = 53
        OnClick = CheckBox1Click
      end
      object CheckBox39: TCheckBox
        Tag = 13
        Left = 529
        Top = 148
        Width = 17
        Height = 17
        Hint = '6'
        Anchors = [akTop, akRight]
        TabOrder = 54
        OnClick = CheckBox1Click
      end
      object CheckBox40: TCheckBox
        Tag = 14
        Left = 545
        Top = 148
        Width = 17
        Height = 17
        Hint = '6'
        Anchors = [akTop, akRight]
        TabOrder = 55
        OnClick = CheckBox1Click
      end
      object CheckBox41: TCheckBox
        Tag = 15
        Left = 561
        Top = 148
        Width = 17
        Height = 17
        Hint = '6'
        Anchors = [akTop, akRight]
        TabOrder = 56
        OnClick = CheckBox1Click
      end
      object CheckBox42: TCheckBox
        Tag = 16
        Left = 577
        Top = 148
        Width = 17
        Height = 17
        Hint = '6'
        Anchors = [akTop, akRight]
        TabOrder = 57
        OnClick = CheckBox1Click
      end
      object ComboBox7: TComboBox
        Tag = 8
        Left = 634
        Top = 143
        Width = 137
        Height = 21
        Hint = '6'
        Style = csDropDownList
        Anchors = [akTop, akRight]
        TabOrder = 58
        OnChange = ComboBox2Change
        Items.Strings = (
          'None'
          'Check Mirror0'
          'Check Mirror1'
          'Check Mirror2'
          'Check Mirror3'
          'Check Mirror4'
          'Check Mirror5'
          'Check Mirror6'
          'Check Mirror7'
          'Check Mirror8'
          'Check Mirror9'
          'Test MAG0'
          'Test MAG1'
          'Test MAG2'
          'Test MAG3'
          'Test MAG4'
          'Test MAG5'
          'Test MAG6'
          'Test MAG7'
          'Test MAG8'
          'Test MAG9'
          'Test Jukebox0'
          'Test Jukebox1'
          'Test Jukebox2'
          'Test Jukebox3'
          'Test Jukebox4'
          'Test Jukebox5'
          'Test Jukebox6'
          'Test Jukebox7'
          'Test Jukebox8'
          'Test Jukebox9'
          'Kill and Restart the Server'
          'Pack the Database')
      end
      object Edit11: TEdit
        Tag = 7
        Left = 593
        Top = 143
        Width = 39
        Height = 21
        Hint = '6'
        Anchors = [akTop, akRight]
        TabOrder = 59
        Text = '23:00'
        OnChange = Edit1Change
        OnDblClick = Edit1DblClick
      end
      object Edit12: TEdit
        Tag = 9
        Left = 772
        Top = 143
        Width = 31
        Height = 21
        Hint = '6'
        Anchors = [akTop, akRight]
        Color = clBtnFace
        Enabled = False
        TabOrder = 60
        OnChange = Edit2Change
      end
      object CheckBox43: TCheckBox
        Tag = 10
        Left = 481
        Top = 172
        Width = 17
        Height = 17
        Hint = '7'
        Anchors = [akTop, akRight]
        TabOrder = 61
        OnClick = CheckBox1Click
      end
      object CheckBox44: TCheckBox
        Tag = 11
        Left = 497
        Top = 172
        Width = 17
        Height = 17
        Hint = '7'
        Anchors = [akTop, akRight]
        TabOrder = 62
        OnClick = CheckBox1Click
      end
      object CheckBox45: TCheckBox
        Tag = 12
        Left = 513
        Top = 172
        Width = 17
        Height = 17
        Hint = '7'
        Anchors = [akTop, akRight]
        TabOrder = 63
        OnClick = CheckBox1Click
      end
      object CheckBox46: TCheckBox
        Tag = 13
        Left = 529
        Top = 172
        Width = 17
        Height = 17
        Hint = '7'
        Anchors = [akTop, akRight]
        TabOrder = 64
        OnClick = CheckBox1Click
      end
      object CheckBox47: TCheckBox
        Tag = 14
        Left = 545
        Top = 172
        Width = 17
        Height = 17
        Hint = '7'
        Anchors = [akTop, akRight]
        TabOrder = 65
        OnClick = CheckBox1Click
      end
      object CheckBox48: TCheckBox
        Tag = 15
        Left = 561
        Top = 172
        Width = 17
        Height = 17
        Hint = '7'
        Anchors = [akTop, akRight]
        TabOrder = 66
        OnClick = CheckBox1Click
      end
      object CheckBox49: TCheckBox
        Tag = 16
        Left = 577
        Top = 172
        Width = 17
        Height = 17
        Hint = '7'
        Anchors = [akTop, akRight]
        TabOrder = 67
        OnClick = CheckBox1Click
      end
      object ComboBox8: TComboBox
        Tag = 8
        Left = 634
        Top = 167
        Width = 137
        Height = 21
        Hint = '7'
        Style = csDropDownList
        Anchors = [akTop, akRight]
        TabOrder = 68
        OnChange = ComboBox2Change
        Items.Strings = (
          'None'
          'Check Mirror0'
          'Check Mirror1'
          'Check Mirror2'
          'Check Mirror3'
          'Check Mirror4'
          'Check Mirror5'
          'Check Mirror6'
          'Check Mirror7'
          'Check Mirror8'
          'Check Mirror9'
          'Test MAG0'
          'Test MAG1'
          'Test MAG2'
          'Test MAG3'
          'Test MAG4'
          'Test MAG5'
          'Test MAG6'
          'Test MAG7'
          'Test MAG8'
          'Test MAG9'
          'Test Jukebox0'
          'Test Jukebox1'
          'Test Jukebox2'
          'Test Jukebox3'
          'Test Jukebox4'
          'Test Jukebox5'
          'Test Jukebox6'
          'Test Jukebox7'
          'Test Jukebox8'
          'Test Jukebox9'
          'Kill and Restart the Server'
          'Pack the Database')
      end
      object Edit13: TEdit
        Tag = 7
        Left = 593
        Top = 167
        Width = 39
        Height = 21
        Hint = '7'
        Anchors = [akTop, akRight]
        TabOrder = 69
        Text = '23:00'
        OnChange = Edit1Change
        OnDblClick = Edit1DblClick
      end
      object Edit14: TEdit
        Tag = 9
        Left = 772
        Top = 167
        Width = 31
        Height = 21
        Hint = '7'
        Anchors = [akTop, akRight]
        Color = clBtnFace
        Enabled = False
        TabOrder = 70
        OnChange = Edit2Change
      end
      object CheckBox50: TCheckBox
        Tag = 10
        Left = 481
        Top = 196
        Width = 17
        Height = 17
        Hint = '8'
        Anchors = [akTop, akRight]
        TabOrder = 71
        OnClick = CheckBox1Click
      end
      object CheckBox51: TCheckBox
        Tag = 11
        Left = 497
        Top = 196
        Width = 17
        Height = 17
        Hint = '8'
        Anchors = [akTop, akRight]
        TabOrder = 72
        OnClick = CheckBox1Click
      end
      object CheckBox52: TCheckBox
        Tag = 12
        Left = 513
        Top = 196
        Width = 17
        Height = 17
        Hint = '8'
        Anchors = [akTop, akRight]
        TabOrder = 73
        OnClick = CheckBox1Click
      end
      object CheckBox53: TCheckBox
        Tag = 13
        Left = 529
        Top = 196
        Width = 17
        Height = 17
        Hint = '8'
        Anchors = [akTop, akRight]
        TabOrder = 74
        OnClick = CheckBox1Click
      end
      object CheckBox54: TCheckBox
        Tag = 14
        Left = 545
        Top = 196
        Width = 17
        Height = 17
        Hint = '8'
        Anchors = [akTop, akRight]
        TabOrder = 75
        OnClick = CheckBox1Click
      end
      object CheckBox55: TCheckBox
        Tag = 15
        Left = 561
        Top = 196
        Width = 17
        Height = 17
        Hint = '8'
        Anchors = [akTop, akRight]
        TabOrder = 76
        OnClick = CheckBox1Click
      end
      object CheckBox56: TCheckBox
        Tag = 16
        Left = 577
        Top = 196
        Width = 17
        Height = 17
        Hint = '8'
        Anchors = [akTop, akRight]
        TabOrder = 77
        OnClick = CheckBox1Click
      end
      object ComboBox9: TComboBox
        Tag = 8
        Left = 634
        Top = 191
        Width = 137
        Height = 21
        Hint = '8'
        Style = csDropDownList
        Anchors = [akTop, akRight]
        TabOrder = 78
        OnChange = ComboBox2Change
        Items.Strings = (
          'None'
          'Check Mirror0'
          'Check Mirror1'
          'Check Mirror2'
          'Check Mirror3'
          'Check Mirror4'
          'Check Mirror5'
          'Check Mirror6'
          'Check Mirror7'
          'Check Mirror8'
          'Check Mirror9'
          'Test MAG0'
          'Test MAG1'
          'Test MAG2'
          'Test MAG3'
          'Test MAG4'
          'Test MAG5'
          'Test MAG6'
          'Test MAG7'
          'Test MAG8'
          'Test MAG9'
          'Test Jukebox0'
          'Test Jukebox1'
          'Test Jukebox2'
          'Test Jukebox3'
          'Test Jukebox4'
          'Test Jukebox5'
          'Test Jukebox6'
          'Test Jukebox7'
          'Test Jukebox8'
          'Test Jukebox9'
          'Kill and Restart the Server'
          'Pack the Database')
      end
      object Edit15: TEdit
        Tag = 7
        Left = 593
        Top = 191
        Width = 39
        Height = 21
        Hint = '8'
        Anchors = [akTop, akRight]
        TabOrder = 79
        Text = '23:00'
        OnChange = Edit1Change
        OnDblClick = Edit1DblClick
      end
      object Edit16: TEdit
        Tag = 9
        Left = 772
        Top = 191
        Width = 31
        Height = 21
        Hint = '8'
        Anchors = [akTop, akRight]
        Color = clBtnFace
        Enabled = False
        TabOrder = 80
        OnChange = Edit2Change
      end
      object CheckBox57: TCheckBox
        Tag = 10
        Left = 481
        Top = 220
        Width = 17
        Height = 17
        Hint = '9'
        Anchors = [akTop, akRight]
        TabOrder = 81
        OnClick = CheckBox1Click
      end
      object CheckBox58: TCheckBox
        Tag = 11
        Left = 497
        Top = 220
        Width = 17
        Height = 17
        Hint = '9'
        Anchors = [akTop, akRight]
        TabOrder = 82
        OnClick = CheckBox1Click
      end
      object CheckBox59: TCheckBox
        Tag = 12
        Left = 513
        Top = 220
        Width = 17
        Height = 17
        Hint = '9'
        Anchors = [akTop, akRight]
        TabOrder = 83
        OnClick = CheckBox1Click
      end
      object CheckBox60: TCheckBox
        Tag = 13
        Left = 529
        Top = 220
        Width = 17
        Height = 17
        Hint = '9'
        Anchors = [akTop, akRight]
        TabOrder = 84
        OnClick = CheckBox1Click
      end
      object CheckBox61: TCheckBox
        Tag = 14
        Left = 545
        Top = 220
        Width = 17
        Height = 17
        Hint = '9'
        Anchors = [akTop, akRight]
        TabOrder = 85
        OnClick = CheckBox1Click
      end
      object CheckBox62: TCheckBox
        Tag = 15
        Left = 561
        Top = 220
        Width = 17
        Height = 17
        Hint = '9'
        Anchors = [akTop, akRight]
        TabOrder = 86
        OnClick = CheckBox1Click
      end
      object CheckBox63: TCheckBox
        Tag = 16
        Left = 577
        Top = 220
        Width = 17
        Height = 17
        Hint = '9'
        Anchors = [akTop, akRight]
        TabOrder = 87
        OnClick = CheckBox1Click
      end
      object ComboBox10: TComboBox
        Tag = 8
        Left = 634
        Top = 215
        Width = 137
        Height = 21
        Hint = '9'
        Style = csDropDownList
        Anchors = [akTop, akRight]
        TabOrder = 88
        OnChange = ComboBox2Change
        Items.Strings = (
          'None'
          'Check Mirror0'
          'Check Mirror1'
          'Check Mirror2'
          'Check Mirror3'
          'Check Mirror4'
          'Check Mirror5'
          'Check Mirror6'
          'Check Mirror7'
          'Check Mirror8'
          'Check Mirror9'
          'Test MAG0'
          'Test MAG1'
          'Test MAG2'
          'Test MAG3'
          'Test MAG4'
          'Test MAG5'
          'Test MAG6'
          'Test MAG7'
          'Test MAG8'
          'Test MAG9'
          'Test Jukebox0'
          'Test Jukebox1'
          'Test Jukebox2'
          'Test Jukebox3'
          'Test Jukebox4'
          'Test Jukebox5'
          'Test Jukebox6'
          'Test Jukebox7'
          'Test Jukebox8'
          'Test Jukebox9'
          'Kill and Restart the Server'
          'Pack the Database')
      end
      object Edit17: TEdit
        Tag = 7
        Left = 593
        Top = 215
        Width = 39
        Height = 21
        Hint = '9'
        Anchors = [akTop, akRight]
        TabOrder = 89
        Text = '23:00'
        OnChange = Edit1Change
        OnDblClick = Edit1DblClick
      end
      object Edit18: TEdit
        Tag = 9
        Left = 772
        Top = 215
        Width = 31
        Height = 21
        Hint = '9'
        Anchors = [akTop, akRight]
        Color = clBtnFace
        Enabled = False
        TabOrder = 90
        OnChange = Edit2Change
      end
      object CheckBox64: TCheckBox
        Tag = 10
        Left = 481
        Top = 244
        Width = 17
        Height = 17
        Hint = '10'
        Anchors = [akTop, akRight]
        TabOrder = 91
        OnClick = CheckBox1Click
      end
      object CheckBox65: TCheckBox
        Tag = 11
        Left = 497
        Top = 244
        Width = 17
        Height = 17
        Hint = '10'
        Anchors = [akTop, akRight]
        TabOrder = 92
        OnClick = CheckBox1Click
      end
      object CheckBox66: TCheckBox
        Tag = 12
        Left = 513
        Top = 244
        Width = 17
        Height = 17
        Hint = '10'
        Anchors = [akTop, akRight]
        TabOrder = 93
        OnClick = CheckBox1Click
      end
      object CheckBox67: TCheckBox
        Tag = 13
        Left = 529
        Top = 244
        Width = 17
        Height = 17
        Hint = '10'
        Anchors = [akTop, akRight]
        TabOrder = 94
        OnClick = CheckBox1Click
      end
      object CheckBox68: TCheckBox
        Tag = 14
        Left = 545
        Top = 244
        Width = 17
        Height = 17
        Hint = '10'
        Anchors = [akTop, akRight]
        TabOrder = 95
        OnClick = CheckBox1Click
      end
      object CheckBox69: TCheckBox
        Tag = 15
        Left = 561
        Top = 244
        Width = 17
        Height = 17
        Hint = '10'
        Anchors = [akTop, akRight]
        TabOrder = 96
        OnClick = CheckBox1Click
      end
      object CheckBox70: TCheckBox
        Tag = 16
        Left = 577
        Top = 244
        Width = 17
        Height = 17
        Hint = '10'
        Anchors = [akTop, akRight]
        TabOrder = 97
        OnClick = CheckBox1Click
      end
      object ComboBox11: TComboBox
        Tag = 8
        Left = 634
        Top = 239
        Width = 137
        Height = 21
        Hint = '10'
        Style = csDropDownList
        Anchors = [akTop, akRight]
        TabOrder = 98
        OnChange = ComboBox2Change
        Items.Strings = (
          'None'
          'Check Mirror0'
          'Check Mirror1'
          'Check Mirror2'
          'Check Mirror3'
          'Check Mirror4'
          'Check Mirror5'
          'Check Mirror6'
          'Check Mirror7'
          'Check Mirror8'
          'Check Mirror9'
          'Test MAG0'
          'Test MAG1'
          'Test MAG2'
          'Test MAG3'
          'Test MAG4'
          'Test MAG5'
          'Test MAG6'
          'Test MAG7'
          'Test MAG8'
          'Test MAG9'
          'Test Jukebox0'
          'Test Jukebox1'
          'Test Jukebox2'
          'Test Jukebox3'
          'Test Jukebox4'
          'Test Jukebox5'
          'Test Jukebox6'
          'Test Jukebox7'
          'Test Jukebox8'
          'Test Jukebox9'
          'Kill and Restart the Server'
          'Pack the Database')
      end
      object Edit19: TEdit
        Tag = 7
        Left = 593
        Top = 239
        Width = 39
        Height = 21
        Hint = '10'
        Anchors = [akTop, akRight]
        TabOrder = 99
        Text = '23:00'
        OnChange = Edit1Change
        OnDblClick = Edit1DblClick
      end
      object Edit20: TEdit
        Tag = 9
        Left = 772
        Top = 239
        Width = 31
        Height = 21
        Hint = '10'
        Anchors = [akTop, akRight]
        Color = clBtnFace
        Enabled = False
        TabOrder = 100
        OnChange = Edit2Change
      end
      object StaticTextWeeklyCheck: TStaticText
        Tag = 9999
        Left = 495
        Top = 504
        Width = 320
        Height = 17
        Hint = 'Shows current time'
        Anchors = [akRight, akBottom]
        AutoSize = False
        BorderStyle = sbsSunken
        Caption = 'StaticTextWeeklyCheck'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 101
      end
    end
  end
  object DataSource1: TDataSource
    AutoEdit = False
    OnDataChange = DataSource1DataChange
    Left = 44
    Top = 136
  end
  object DataSource2: TDataSource
    AutoEdit = False
    OnDataChange = DataSource2DataChange
    Left = 76
    Top = 136
  end
  object DataSource3: TDataSource
    AutoEdit = False
    OnDataChange = DataSource3DataChange
    Left = 108
    Top = 136
  end
  object DataSource4: TDataSource
    AutoEdit = False
    OnDataChange = DataSource4DataChange
    Left = 140
    Top = 136
  end
  object PopupMenu1: TPopupMenu
    Left = 668
    Top = 384
    object DumpHeader1: TMenuItem
      Caption = 'Show Header (F9)'
      OnClick = Dumpheader1Click
    end
    object SaveasBitmap1: TMenuItem
      Caption = 'Save as Bitmap'
      OnClick = SaveasBitmap1Click
    end
    object ShowDICOMfilesinexplorer1: TMenuItem
      Caption = 'Show image in explorer'
      OnClick = ShowDICOMfilesinexplorer1Click
    end
    object RunKPacsviewer1: TMenuItem
      Caption = 'Run K-Pacs viewer (F2)'
      OnClick = RunKPacsviewer1Click
    end
    object Runexternalviewer1: TMenuItem
      Caption = 'Run external viewer'
      Visible = False
      OnClick = Runexternalviewer1Click
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object Send1: TMenuItem
      Caption = 'DICOM Send'
      OnClick = Send1Click
      object SendPatientTo: TMenuItem
        Caption = 'Patient To'
      end
      object SendStudyTo: TMenuItem
        Caption = 'Study To'
      end
      object SendSeriesTo: TMenuItem
        Caption = 'Series To'
      end
      object SendImageTo: TMenuItem
        Caption = 'Image To'
      end
      object N7: TMenuItem
        Caption = '-'
      end
      object SendImagesTo: TMenuItem
        Caption = 'Selected Selected Images To'
      end
    end
    object Export1: TMenuItem
      Caption = 'Export to ZIP file'
      OnClick = Export1Click
      object Patient1: TMenuItem
        Caption = 'Patient'
        OnClick = Patient1Click
      end
      object Study1: TMenuItem
        Caption = 'Study'
        OnClick = Study1Click
      end
      object Series1: TMenuItem
        Caption = 'Series'
        OnClick = Series1Click
      end
      object Image2: TMenuItem
        Caption = 'Image'
        OnClick = Image2Click
      end
    end
    object ExportAnomymizedtoZIPfile1: TMenuItem
      Caption = 'Export Anomymized to ZIP file'
      OnClick = ExportAnomymizedtoZIPfile1Click
      object ExportAnonymousZIPPatient: TMenuItem
        Caption = 'this Patient'
        OnClick = ExportAnonymousZIPPatientClick
      end
      object ExportAnonymousZIPStudy: TMenuItem
        Caption = 'this Study'
        OnClick = ExportAnonymousZIPPatientClick
      end
      object ExportAnonymousZIPSeries: TMenuItem
        Caption = 'this Series'
        OnClick = ExportAnonymousZIPPatientClick
      end
      object ExportAnonymousZIPImage: TMenuItem
        Caption = 'this Image'
        OnClick = ExportAnonymousZIPPatientClick
      end
    end
    object Print1: TMenuItem
      Caption = 'Print'
      OnClick = Print1Click
      object PrintSelectedImages1: TMenuItem
        Caption = 'Print Selected Images on local DICOM printer'
        OnClick = PrintSelectedImages1Click
      end
      object PrintImage1: TMenuItem
        Caption = 'Print Image on local DICOM printer'
        OnClick = PrintImage1Click
      end
      object DICOMPrintSelectedImagesTo: TMenuItem
        Caption = 'DICOM print selected images to ..'
      end
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object ChangePatientID1: TMenuItem
      Caption = 'Change Patient ID'
      OnClick = ChangePatientID1Click
      object ChangePatidPatient: TMenuItem
        Caption = 'of Patient'
        OnClick = ChangePatidPatientClick
      end
      object ChangePatidStudy: TMenuItem
        Caption = 'of Study'
        OnClick = ChangePatidPatientClick
      end
      object ChangePatidSeries: TMenuItem
        Caption = 'of Series'
        OnClick = ChangePatidPatientClick
      end
      object ChangePatidImage: TMenuItem
        Caption = 'of Image'
        OnClick = ChangePatidPatientClick
      end
    end
    object Anonymize1: TMenuItem
      Caption = 'Anonymize'
      OnClick = Anonymize1Click
      object AnonymizePatient: TMenuItem
        Caption = 'this Patient'
        OnClick = AnonymizePatientClick
      end
      object AnonymizeStudy: TMenuItem
        Caption = 'this Study'
        OnClick = AnonymizePatientClick
      end
      object AnonymizeSeries: TMenuItem
        Caption = 'this Series'
        OnClick = AnonymizePatientClick
      end
      object AnonymizeImage: TMenuItem
        Caption = 'this Image'
        OnClick = AnonymizePatientClick
      end
    end
    object ModifyMenu: TMenuItem
      Caption = 'Modify'
      OnClick = ModifyMenuClick
      object ModifyPatient: TMenuItem
        Caption = 'this Patient (advanced)'
        OnClick = ModifyPatientClick
      end
      object ModifyStudy: TMenuItem
        Caption = 'this Study (advanced)'
        OnClick = ModifyPatientClick
      end
      object ModifySeries: TMenuItem
        Caption = 'this Series (advanced)'
        OnClick = ModifyPatientClick
      end
      object ModifySOP: TMenuItem
        Caption = 'this Image (advanced)'
        OnClick = ModifyPatientClick
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object MergeSelectedSeries1: TMenuItem
        Caption = 'Merge Selected Series'
        OnClick = MergeSelectedSeries1Click
      end
      object MenuMergeStudies: TMenuItem
        Caption = 'Merge Selected Studies'
        OnClick = MenuMergeStudiesClick
      end
      object SplitSeries1: TMenuItem
        Caption = 'Split Series into two'
        OnClick = SplitSeries1Click
      end
    end
    object Delete: TMenuItem
      Caption = 'Delete'
      OnClick = DeleteClick
      object Deletethispatient1: TMenuItem
        Caption = 'Delete Patient'
        OnClick = _Deletethispatient1Click
      end
      object DeletethisStudy: TMenuItem
        Caption = 'Delete Study'
        OnClick = _DeleteThisSeriesClick
      end
      object DeletethisSeries: TMenuItem
        Caption = 'Delete Series'
        OnClick = _DeleteThisSeriesClick
      end
      object DeleteImage: TMenuItem
        Caption = 'Delete Image'
        OnClick = DeleteImageClick
      end
      object N6: TMenuItem
        Caption = '-'
      end
      object Removeimagefromdatabase1: TMenuItem
        Caption = 'Remove image from database'
        OnClick = Removeimagefromdatabase1Click
      end
    end
    object N5: TMenuItem
      Caption = '-'
    end
    object EditDatabasebyhand1: TMenuItem
      Caption = 'View full database (F11)'
      OnClick = EditDatabasebyhand1Click
    end
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = 'bmp'
    Filter = 'Bitmap files (*.bmp)|*.BMP'
    Options = [ofOverwritePrompt, ofHideReadOnly]
    Title = 'Save slice as bitmap'
    Left = 708
    Top = 384
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 12
    Top = 168
  end
  object Timer2: TTimer
    Interval = 300
    OnTimer = Timer2Timer
    Left = 108
    Top = 168
  end
  object Timer4: TTimer
    Enabled = False
    Interval = 5000
    OnTimer = Timer4Timer
    Left = 788
    Top = 384
  end
  object Timer5: TTimer
    Interval = 500
    OnTimer = Timer5Timer
    Left = 76
    Top = 168
  end
  object WeeklyCheckTimer: TTimer
    Tag = 9999
    Interval = 2000
    OnTimer = WeeklyCheckTimerTimer
    Left = 620
    Top = 384
  end
  object PrinterTimer: TTimer
    Interval = 10
    OnTimer = PrinterTimerTimer
    Left = 140
    Top = 168
  end
  object OpenDialog1: TOpenDialog
    Left = 740
    Top = 384
  end
  object PopupMenu2: TPopupMenu
    Left = 668
    Top = 344
  end
end
