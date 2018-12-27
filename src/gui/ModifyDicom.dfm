object FormModifyDICOM: TFormModifyDICOM
  Left = 0
  Top = 0
  Caption = 'Modify DICOM images'
  ClientHeight = 315
  ClientWidth = 571
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnActivate = FormActivate
  DesignSize = (
    571
    315)
  PixelsPerInch = 96
  TextHeight = 13
  object Label3: TLabel
    Left = 8
    Top = 4
    Width = 210
    Height = 13
    Caption = 'Lua script to be run for each selected image'
  end
  object MemoScript: TMemo
    Left = 8
    Top = 21
    Width = 555
    Height = 185
    Anchors = [akLeft, akTop, akRight, akBottom]
    Lines.Strings = (
      '-- Enter Lua code to modify DICOM images here'
      '-- E.g., Data.SeriesDescription = '#39'First series'#39)
    TabOrder = 0
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 212
    Width = 161
    Height = 97
    Anchors = [akLeft, akBottom]
    TabOrder = 1
    object Label1: TLabel
      Left = 16
      Top = 8
      Width = 130
      Height = 13
      Caption = 'How to preprocess images:'
    end
    object RadioButtonNewUids: TRadioButton
      Left = 16
      Top = 27
      Width = 113
      Height = 17
      Hint = 
        'If set, modfied data will be safe to copy once next to original ' +
        'as all identifiers are changed'
      Caption = 'Generate new UIDs'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
    end
    object RadioButtonAnonymize: TRadioButton
      Left = 16
      Top = 50
      Width = 113
      Height = 17
      Hint = 'Runs standard anonymizer'
      Caption = 'Anonymize'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
    end
    object RadioButtonKeepTheSame: TRadioButton
      Left = 16
      Top = 73
      Width = 113
      Height = 17
      Hint = 'Modifications on original DICOM object - be careful'
      Caption = 'Keep the same'
      Checked = True
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
      TabStop = True
    end
  end
  object GroupBox2: TGroupBox
    Left = 175
    Top = 212
    Width = 145
    Height = 97
    Anchors = [akLeft, akBottom]
    TabOrder = 2
    object Label2: TLabel
      Left = 16
      Top = 8
      Width = 121
      Height = 13
      Caption = 'How to store the images:'
    end
    object RadioButtonCopy: TRadioButton
      Left = 16
      Top = 27
      Width = 113
      Height = 17
      Hint = 'Do not use unless generate new UIDs is set'
      Caption = 'Copy'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
    end
    object RadioButtonReplace: TRadioButton
      Left = 16
      Top = 50
      Width = 113
      Height = 17
      Caption = 'Replace'
      Checked = True
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
      TabStop = True
    end
  end
  object CancelButton: TButton
    Left = 407
    Top = 281
    Width = 75
    Height = 25
    Hint = 'Cancel - do not process anything'
    Anchors = [akLeft, akBottom]
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
  end
  object OKButton: TButton
    Left = 488
    Top = 281
    Width = 75
    Height = 25
    Hint = 
      'OK - start modification of all selected images; Alt-click keep t' +
      'his window open'
    Anchors = [akLeft, akBottom]
    Caption = 'Start modify'
    ModalResult = 1
    ParentShowHint = False
    ShowHint = True
    TabOrder = 4
    OnClick = OKButtonClick
    OnMouseDown = OKButtonMouseDown
  end
  object Button1: TButton
    Left = 407
    Top = 212
    Width = 75
    Height = 25
    Hint = 'Loads script from file'
    Anchors = [akLeft, akBottom]
    Cancel = True
    Caption = 'Load'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 5
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 488
    Top = 212
    Width = 75
    Height = 25
    Hint = 'Saves script to file'
    Anchors = [akLeft, akBottom]
    Caption = 'Save'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 6
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 326
    Top = 212
    Width = 75
    Height = 25
    Hint = 'Runs script to test on current image without modifying anything'
    Anchors = [akLeft, akBottom]
    Cancel = True
    Caption = 'Test syntax'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 7
    OnClick = Button3Click
  end
end
