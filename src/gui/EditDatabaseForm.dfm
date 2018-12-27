object Form4: TForm4
  Left = 42
  Top = 9
  Caption = 
    'Edit DICOM database by hand (use with extreme care; re-initializ' +
    'e the database to undo)'
  ClientHeight = 652
  ClientWidth = 983
  Color = clBtnFace
  Constraints.MinHeight = 600
  Constraints.MinWidth = 600
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  OnHide = FormHide
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  DesignSize = (
    983
    652)
  PixelsPerInch = 96
  TextHeight = 13
  object Label6: TLabel
    Left = 8
    Top = 132
    Width = 33
    Height = 13
    Caption = 'Patient'
  end
  object Label7: TLabel
    Left = 8
    Top = 284
    Width = 27
    Height = 13
    Caption = 'Study'
  end
  object Label8: TLabel
    Left = 8
    Top = 444
    Width = 29
    Height = 13
    Caption = 'Series'
  end
  object Label9: TLabel
    Left = 8
    Top = 636
    Width = 29
    Height = 13
    Anchors = [akLeft, akBottom]
    Caption = 'Image'
  end
  object DBGrid2: TDBGrid
    Left = 8
    Top = 8
    Width = 975
    Height = 120
    Hint = 'List of patients in the database'
    Anchors = [akLeft, akTop, akRight]
    DataSource = Form1.DataSource1
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    TitleFont.Charset = DEFAULT_CHARSET
    TitleFont.Color = clWindowText
    TitleFont.Height = -11
    TitleFont.Name = 'MS Sans Serif'
    TitleFont.Style = []
    Columns = <
      item
        Expanded = False
        FieldName = 'PatientID'
        Width = 128
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'PatientNam'
        Title.Caption = 'Patient Name'
        Width = 565
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'PatientBir'
        Title.Caption = 'Birth Date'
        Width = 80
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'PatientSex'
        Title.Caption = 'Patient Sex'
        Width = 100
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
    Width = 184
    Height = 22
    Hint = 'Navigate through patient list'
    DataSource = Form1.DataSource1
    VisibleButtons = [nbFirst, nbPrior, nbNext, nbLast]
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
  end
  object DBNavigator2: TDBNavigator
    Left = 48
    Top = 280
    Width = 184
    Height = 21
    Hint = 'Navigate through study list'
    DataSource = Form1.DataSource2
    VisibleButtons = [nbFirst, nbPrior, nbNext, nbLast]
    ParentShowHint = False
    ShowHint = True
    TabOrder = 2
  end
  object DBLookupComboBox2: TDBLookupComboBox
    Left = 252
    Top = 280
    Width = 20
    Height = 21
    DropDownRows = 15
    DropDownWidth = 200
    KeyField = 'STUDYINSTA'
    ListField = 'STUDYDATE'
    TabOrder = 3
  end
  object DBGrid1: TDBGrid
    Left = 8
    Top = 304
    Width = 975
    Height = 137
    Hint = 'List of series for selected patient and study in the database'
    Anchors = [akLeft, akTop, akRight]
    DataSource = Form1.DataSource3
    ParentShowHint = False
    ShowHint = True
    TabOrder = 4
    TitleFont.Charset = DEFAULT_CHARSET
    TitleFont.Color = clWindowText
    TitleFont.Height = -11
    TitleFont.Name = 'MS Sans Serif'
    TitleFont.Style = []
    OnCellClick = DBGrid1CellClick
    Columns = <
      item
        Expanded = False
        FieldName = 'SeriesNumb'
        Title.Caption = 'Number'
        Width = 42
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'SeriesDate'
        Title.Caption = 'Date'
        Width = 52
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'SeriesTime'
        Title.Caption = 'Time'
        Width = 48
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'SeriesDesc'
        Title.Caption = 'Description'
        Width = 68
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'Modality'
        Width = 48
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'PatientPos'
        Width = 56
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'ContrastBo'
        Title.Caption = 'Contrast Bolus'
        Width = 72
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'Manufactur'
        Title.Caption = 'Manufacturer'
        Width = 68
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'StationNam'
        Title.Caption = 'Station'
        Width = 60
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'Institutio'
        Title.Caption = 'Institution'
        Width = 60
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'ModelName'
        Title.Caption = 'Model'
        Width = 60
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'BodyPartEx'
        Title.Caption = 'Body Part'
        Width = 60
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'ProtocolNa'
        Title.Caption = 'Protocol'
        Width = 60
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'FrameOfRef'
        Width = 100
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'SeriesInst'
        Width = 100
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'StudyInsta'
        Width = 100
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
    Top = 440
    Width = 184
    Height = 20
    Hint = 'Navigate through series list'
    DataSource = Form1.DataSource3
    VisibleButtons = [nbFirst, nbPrior, nbNext, nbLast]
    ParentShowHint = False
    ShowHint = True
    TabOrder = 5
  end
  object DBLookupComboBox3: TDBLookupComboBox
    Left = 252
    Top = 440
    Width = 20
    Height = 21
    DropDownRows = 15
    DropDownWidth = 200
    KeyField = 'SERIESINST'
    ListField = 'SERIESNUMB'
    TabOrder = 6
  end
  object DBGrid3: TDBGrid
    Left = 9
    Top = 464
    Width = 975
    Height = 168
    Hint = 
      'List of images for selected patient, study and series in the dat' +
      'abase'
    Anchors = [akLeft, akTop, akRight, akBottom]
    DataSource = Form1.DataSource4
    ParentShowHint = False
    ShowHint = True
    TabOrder = 7
    TitleFont.Charset = DEFAULT_CHARSET
    TitleFont.Color = clWindowText
    TitleFont.Height = -11
    TitleFont.Name = 'MS Sans Serif'
    TitleFont.Style = []
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
        Title.Caption = 'Date'
        Width = 52
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'ImageTime'
        Title.Caption = 'Time'
        Width = 48
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'EchoNumber'
        Title.Caption = 'Echo'
        Width = 48
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'NumberOfFr'
        Title.Caption = 'Frames'
        Width = 48
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'AcqDate'
        Width = 60
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'AcqTime'
        Width = 52
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'ReceivingC'
        Title.Caption = 'Coil'
        Width = 60
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'AcqNumber'
        Width = 60
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'SliceLocat'
        Title.Caption = 'Slice Location'
        Width = 72
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'Samples'
        Width = 48
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'PhotoMetri'
        Title.Caption = 'PhotoMetric Interpretation'
        Width = 90
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'Rows'
        Width = 38
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'Colums'
        Width = 38
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'BitsStored'
        Title.Caption = 'Bits'
        Width = 28
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'ImageType'
        Width = 80
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'ImageID'
        Title.Caption = 'Image ID'
        Width = 80
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'SOPInstanc'
        Width = 100
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'SOPClassUI'
        Width = 100
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'SeriesInst'
        Width = 100
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
        Width = 100
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'DeviceName'
        Width = 100
        Visible = True
      end>
  end
  object DBNavigator4: TDBNavigator
    Left = 48
    Top = 632
    Width = 184
    Height = 20
    Hint = 'Navigate through image list'
    DataSource = Form1.DataSource4
    VisibleButtons = [nbFirst, nbPrior, nbNext, nbLast]
    Anchors = [akLeft, akBottom]
    ParentShowHint = False
    ShowHint = True
    TabOrder = 8
  end
  object DBLookupComboBox4: TDBLookupComboBox
    Left = 252
    Top = 632
    Width = 20
    Height = 21
    Anchors = [akLeft, akBottom]
    DropDownRows = 30
    DropDownWidth = 200
    KeyField = 'SOPINSTANC'
    ListField = 'IMAGENUMBE'
    TabOrder = 9
  end
  object DBGrid4: TDBGrid
    Left = 8
    Top = 152
    Width = 975
    Height = 129
    Hint = 'List of studies for selected patient in the database'
    Anchors = [akLeft, akTop, akRight]
    DataSource = Form1.DataSource2
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 10
    TitleFont.Charset = DEFAULT_CHARSET
    TitleFont.Color = clWindowText
    TitleFont.Height = -11
    TitleFont.Name = 'MS Sans Serif'
    TitleFont.Style = []
    OnCellClick = DBGrid4CellClick
    Columns = <
      item
        Expanded = False
        FieldName = 'StudyDate'
        Title.Caption = 'Date'
        Width = 52
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'StudyTime'
        Title.Caption = 'Time'
        Width = 48
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'StudyID'
        Width = 42
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'StudyDescr'
        Title.Caption = 'Description'
        Width = 68
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'AccessionN'
        Title.Caption = 'Accession Number'
        Width = 54
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'ReferPhysi'
        Title.Caption = 'Referring physician'
        Width = 96
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'PatientsAg'
        Title.Caption = 'Age'
        Width = 42
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'PatientsWe'
        Title.Caption = 'Weight'
        Width = 42
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'StudyModal'
        Title.Caption = 'Study Modality'
        Width = 80
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'StudyInsta'
        Width = 134
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'PatientID'
        Width = 96
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'AccessTime'
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'PatientNam'
        Title.Caption = 'Patient Name'
        Width = 100
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'PatientBir'
        Title.Caption = 'Birth Date'
        Width = 54
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'PatientSex'
        Title.Caption = 'Sex'
        Width = 20
        Visible = True
      end>
  end
end
