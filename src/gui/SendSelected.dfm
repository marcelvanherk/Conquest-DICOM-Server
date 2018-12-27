object SendSelectedForm: TSendSelectedForm
  Left = 86
  Top = 47
  Caption = 'Send selected images to -'
  ClientHeight = 450
  ClientWidth = 401
  Color = clBtnFace
  Constraints.MinHeight = 480
  Constraints.MinWidth = 200
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 299
    Top = 0
    Width = 102
    Height = 450
    Hint = 'Select images for transmision - then press OK'
    Align = alRight
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    DesignSize = (
      102
      450)
    object Label1: TLabel
      Left = 16
      Top = 312
      Width = 56
      Height = 13
      Caption = 'Film Format:'
    end
    object IP: TLabel
      Left = 16
      Top = 352
      Width = 10
      Height = 13
      Caption = 'IP'
      Visible = False
    end
    object PORT: TLabel
      Left = 48
      Top = 352
      Width = 30
      Height = 13
      Caption = 'PORT'
      Visible = False
    end
    object AE: TLabel
      Left = 32
      Top = 368
      Width = 14
      Height = 13
      Caption = 'AE'
      Visible = False
    end
    object CancelButton: TButton
      Left = 14
      Top = 383
      Width = 75
      Height = 25
      Hint = 'Cancel - do not transmit anything'
      Anchors = [akLeft, akBottom]
      Cancel = True
      Caption = 'Cancel'
      ModalResult = 2
      ParentShowHint = False
      ShowHint = True
      TabOrder = 9
    end
    object OKButton: TButton
      Left = 14
      Top = 415
      Width = 75
      Height = 25
      Hint = 'OK - start transmission'
      Anchors = [akLeft, akBottom]
      Caption = 'OK'
      ModalResult = 1
      ParentShowHint = False
      ShowHint = True
      TabOrder = 10
    end
    object SelectAllButton: TButton
      Left = 14
      Top = 9
      Width = 75
      Height = 25
      Hint = 'Select all images for transmission'
      Caption = 'Select all'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      OnClick = SelectAllButtonClick
    end
    object SelectDownButton: TButton
      Left = 14
      Top = 41
      Width = 75
      Height = 25
      Hint = 'Select all from cursor - towards the end of the list'
      Caption = 'Select down'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
      OnClick = SelectDownButtonClick
    end
    object SelectUpButton: TButton
      Left = 14
      Top = 73
      Width = 75
      Height = 25
      Hint = 'Select all from cursor - towards the beginning of the list'
      Caption = 'Select up'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
      OnClick = SelectUpButtonClick
    end
    object OtherDownButton: TButton
      Left = 14
      Top = 233
      Width = 75
      Height = 25
      Hint = 
        'Select every other image including cursor - towards the end of t' +
        'he list'
      Caption = 'Other down'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 6
      OnClick = OtherDownButtonClick
    end
    object OtherUpButton: TButton
      Left = 14
      Top = 265
      Width = 75
      Height = 25
      Hint = 
        'Select every other image including cursor - towards the beginnin' +
        'g of the list'
      Caption = 'Other up'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 7
      OnClick = OtherUpButtonClick
    end
    object DeselectAllButton: TButton
      Left = 14
      Top = 121
      Width = 75
      Height = 25
      Hint = 'Deselect all images'
      Caption = 'Deselect all'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 3
      OnClick = DeselectAllButtonClick
    end
    object DeselectDownButton: TButton
      Left = 14
      Top = 153
      Width = 75
      Height = 25
      Hint = 'Deselect all from cursor - towards the end of the list'
      Caption = 'Deselect down'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 4
      OnClick = DeselectDownButtonClick
    end
    object DeselectUpButton: TButton
      Left = 14
      Top = 185
      Width = 75
      Height = 25
      Hint = 'Deselect all from cursor - towards the beginning of the list'
      Caption = 'Deselect up'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 5
      OnClick = DeselectUpButtonClick
    end
    object ComboBox1: TComboBox
      Left = 14
      Top = 328
      Width = 75
      Height = 21
      TabOrder = 8
      Text = 'ComboBox1'
      Items.Strings = (
        '1x1'
        '1x2'
        '2x1'
        '2x2'
        '3x3'
        '3x4'
        '4x3'
        '4x4'
        '4x5'
        '5x4'
        '4x6'
        '6x4'
        '5x5'
        '5x6'
        '6x5'
        '5x7'
        '7x5'
        '8x10'
        '10x12'
        '10x14'
        '11x14'
        '11x17'
        '14x14')
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 299
    Height = 450
    Align = alClient
    Caption = 'Panel2'
    TabOrder = 1
    object ListBox: TListBox
      Left = 1
      Top = 1
      Width = 297
      Height = 448
      Align = alClient
      ItemHeight = 13
      MultiSelect = True
      TabOrder = 0
      TabWidth = 16
    end
  end
end
