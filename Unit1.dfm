object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'CDR  decoder  filter'
  ClientHeight = 1033
  ClientWidth = 1238
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -19
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnCreate = FormCreate
  DesignSize = (
    1238
    1033)
  PixelsPerInch = 165
  TextHeight = 23
  object LMDLabel1: TLMDLabel
    Left = 127
    Top = 19
    Width = 141
    Height = 25
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    Bevel.Mode = bmCustom
    Options = []
    Caption = #1055#1072#1087#1082#1072' '#1080#1089#1090#1086#1095#1085#1080#1082
  end
  object LMDLabel2: TLMDLabel
    Left = 878
    Top = 25
    Width = 164
    Height = 25
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    Bevel.Mode = bmCustom
    Options = []
    Caption = #1055#1072#1087#1082#1072' '#1085#1072#1079#1085#1072#1095#1077#1085#1080#1077
  end
  object LMDLabel3: TLMDLabel
    Left = 14
    Top = 690
    Width = 140
    Height = 25
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    Bevel.Mode = bmCustom
    Options = []
    Caption = #1042#1099#1093#1086#1076#1085#1086#1081' '#1092#1072#1081#1083
  end
  object LMDLabel4: TLMDLabel
    Left = 460
    Top = 143
    Width = 109
    Height = 25
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    Bevel.Mode = bmCustom
    Options = []
    Caption = #1041#1091#1092#1077#1088', '#1073#1072#1081#1090
  end
  object Progress1: TLMDProgress
    Left = 14
    Top = 732
    Width = 1194
    Height = 36
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    Bevel.StyleOuter = bvLowered
    Bevel.Mode = bmCustom
    Style = pgXPBar
  end
  object LMDSpeedButton1: TLMDSpeedButton
    Left = 1178
    Top = 12
    Width = 41
    Height = 39
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    Caption = '*'
    AutoSize = False
    OnClick = LMDSpeedButton2Click
  end
  object LMDSpeedButton2: TLMDSpeedButton
    Left = 386
    Top = 14
    Width = 40
    Height = 39
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    Caption = '*'
    AutoSize = False
    OnClick = LMDSpeedButton1Click
  end
  object DecodeBtn: TLMDButton
    Left = 495
    Top = 14
    Width = 186
    Height = 101
    Hint = ''
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    Caption = #1056#1072#1089#1082#1086#1076#1080#1088#1086#1074#1072#1090#1100
    TabOrder = 0
    OnClick = DecodeBtnClick
  end
  object FileListBox1: TFileListBox
    Left = 16
    Top = 57
    Width = 410
    Height = 622
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    ItemHeight = 23
    MultiSelect = True
    TabOrder = 1
    OnClick = FileListBox1Click
  end
  object FileListBox2: TFileListBox
    Left = 778
    Top = 62
    Width = 441
    Height = 617
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 23
    TabOrder = 2
    OnDblClick = FileListBox2DblClick
  end
  object LMDDockButton1: TLMDDockButton
    Left = 1129
    Top = 12
    Width = 39
    Height = 39
    Hint = #1054#1090#1082#1088#1099#1090#1100' '#1087#1072#1087#1082#1091
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    Anchors = [akTop, akRight]
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
    OnClick = LMDDockButton1Click
    Glyph.Data = {
      E6000000424DE60000000000000076000000280000000D0000000E0000000100
      0400000000007000000000000000000000001000000010000000000000000000
      8000008000000080800080000000800080008080000080808000C0C0C0000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      3000333333333333300033333333333330003333333333333000333333333333
      3000333333333333300033003003003330003300300300333000333333333333
      3000333333333333300033333333333330003333333333333000333333333333
      30003333333333333000}
  end
  object LMDDockButton2: TLMDDockButton
    Left = 336
    Top = 14
    Width = 39
    Height = 39
    Hint = #1054#1090#1082#1088#1099#1090#1100' '#1087#1072#1087#1082#1091
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    ParentShowHint = False
    ShowHint = True
    TabOrder = 4
    OnClick = LMDDockButton2Click
    Glyph.Data = {
      E6000000424DE60000000000000076000000280000000D0000000E0000000100
      0400000000007000000000000000000000001000000010000000000000000000
      8000008000000080800080000000800080008080000080808000C0C0C0000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      3000333333333333300033333333333330003333333333333000333333333333
      3000333333333333300033003003003330003300300300333000333333333333
      3000333333333333300033333333333330003333333333333000333333333333
      30003333333333333000}
  end
  object LMDGroupBox1: TLMDGroupBox
    Left = 437
    Top = 180
    Width = 331
    Height = 262
    Hint = ''
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    Bevel.Mode = bmWindows
    Caption = #1060#1080#1083#1100#1090#1088' '#1087#1086' '#1085#1086#1084#1077#1088#1091
    CaptionFont.Charset = DEFAULT_CHARSET
    CaptionFont.Color = clWindowText
    CaptionFont.Height = -19
    CaptionFont.Name = 'Tahoma'
    CaptionFont.Style = []
    TabOrder = 5
    object AnySubBtn: TLMDRadioButton
      Left = 5
      Top = 42
      Width = 186
      Height = 31
      Hint = ''
      Margins.Left = 5
      Margins.Top = 5
      Margins.Right = 5
      Margins.Bottom = 5
      Caption = #1051#1102#1073#1086#1081' '#1072#1073#1086#1085#1077#1085#1090
      Alignment.Alignment = agTopLeft
      Alignment.Spacing = 4
      Checked = False
      TabOrder = 0
      OnClick = AnySubBtnClick
    end
    object ASubBtn: TLMDRadioButton
      Left = 5
      Top = 83
      Width = 144
      Height = 30
      Hint = ''
      Margins.Left = 5
      Margins.Top = 5
      Margins.Right = 5
      Margins.Bottom = 5
      Caption = #1040#1073#1086#1085#1077#1085#1090' '#1040
      Alignment.Alignment = agTopLeft
      Alignment.Spacing = 4
      Checked = False
      TabOrder = 1
      OnClick = ASubBtnClick
    end
    object BSubBtn: TLMDRadioButton
      Left = 159
      Top = 83
      Width = 140
      Height = 30
      Hint = ''
      Margins.Left = 5
      Margins.Top = 5
      Margins.Right = 5
      Margins.Bottom = 5
      Caption = #1040#1073#1086#1085#1077#1085#1090' '#1041
      Alignment.Alignment = agTopLeft
      Alignment.Spacing = 4
      Checked = False
      TabOrder = 2
      OnClick = BSubBtnClick
    end
    object NumberEdit: TLMDEdit
      Left = 5
      Top = 124
      Width = 321
      Height = 37
      Hint = ''
      Margins.Left = 5
      Margins.Top = 5
      Margins.Right = 5
      Margins.Bottom = 5
      Bevel.Mode = bmWindows
      Caret.BlinkRate = 530
      Enabled = False
      TabOrder = 3
      OnKeyUp = NumberEditKeyUp
      CustomButtons = <>
      PasswordChar = #0
      Text = ''
    end
    object FilterCheckBox: TLMDCheckBox
      Left = 191
      Top = 2
      Width = 80
      Height = 30
      Hint = ''
      Margins.Left = 5
      Margins.Top = 5
      Margins.Right = 5
      Margins.Bottom = 5
      Alignment.Alignment = agTopLeft
      Alignment.Spacing = 4
      TabOrder = 4
      OnMouseUp = FilterCheckBoxMouseUp
    end
    object TransparentCheckBox: TLMDCheckBox
      Left = 5
      Top = 212
      Width = 321
      Height = 30
      Hint = ''
      Margins.Left = 5
      Margins.Top = 5
      Margins.Right = 5
      Margins.Bottom = 5
      Caption = #1055#1086#1080#1089#1082': '#1086#1090'-'#1079#1072#1087#1080#1089#1080'-'#1082'-'#1079#1072#1087#1080#1089#1080
      Alignment.Alignment = agTopLeft
      Alignment.Spacing = 4
      TabOrder = 5
      OnChange = TransparentCheckBoxChange
    end
    object AnyPartCheckBox: TLMDCheckBox
      Left = 5
      Top = 172
      Width = 308
      Height = 30
      Hint = ''
      Margins.Left = 5
      Margins.Top = 5
      Margins.Right = 5
      Margins.Bottom = 5
      Caption = #1055#1086#1080#1089#1082': '#1085#1072#1095#1080#1085#1072#1077#1090#1089#1103' '#1085#1072' '#1094#1080#1092#1088#1099
      Alignment.Alignment = agTopLeft
      Alignment.Spacing = 4
      TabOrder = 6
      OnChange = AnyPartCheckBoxChange
    end
  end
  object LMDGroupBox2: TLMDGroupBox
    Left = 442
    Top = 453
    Width = 326
    Height = 131
    Hint = ''
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    Bevel.Mode = bmWindows
    Caption = #1042#1099#1093#1086#1076#1085#1086#1081' '#1092#1086#1088#1084#1072#1090
    CaptionFont.Charset = DEFAULT_CHARSET
    CaptionFont.Color = clWindowText
    CaptionFont.Height = -19
    CaptionFont.Name = 'Tahoma'
    CaptionFont.Style = []
    TabOrder = 6
    object CSV_Btn: TLMDRadioButton
      Left = 28
      Top = 42
      Width = 115
      Height = 31
      Hint = ''
      Margins.Left = 5
      Margins.Top = 5
      Margins.Right = 5
      Margins.Bottom = 5
      Caption = 'CSV'
      Alignment.Alignment = agTopLeft
      Alignment.Spacing = 4
      Checked = True
      TabOrder = 0
      OnClick = CSV_BtnClick
    end
    object TXT_Btn: TLMDRadioButton
      Left = 28
      Top = 83
      Width = 115
      Height = 30
      Hint = ''
      Margins.Left = 5
      Margins.Top = 5
      Margins.Right = 5
      Margins.Bottom = 5
      Caption = 'TXT'
      Alignment.Alignment = agTopLeft
      Alignment.Spacing = 4
      Checked = False
      TabOrder = 1
      OnClick = TXT_BtnClick
    end
    object SplitEdit: TLMDSpinEdit
      Left = 154
      Top = 83
      Width = 149
      Height = 33
      Hint = ''
      Margins.Left = 5
      Margins.Top = 5
      Margins.Right = 5
      Margins.Bottom = 5
      Bevel.Mode = bmWindows
      Caret.BlinkRate = 530
      Enabled = False
      TabOrder = 2
      AutoSelect = True
      CustomButtons = <>
      MaxValue = 1000000
      Value = 0
    end
    object SplitCheckBox: TLMDCheckBox
      Left = 154
      Top = 28
      Width = 143
      Height = 45
      Hint = ''
      Margins.Left = 5
      Margins.Top = 5
      Margins.Right = 5
      Margins.Bottom = 5
      Caption = #1056#1072#1079#1073#1080#1090#1100' '#1085#1072' '#1095#1080#1089#1083#1086' '#1089#1090#1088#1086#1082
      Alignment.Alignment = agTopLeft
      Alignment.Spacing = 4
      TabOrder = 3
      OnClick = SplitCheckBoxClick
    end
  end
  object EventListBox: TLMDListBox
    Left = 14
    Top = 778
    Width = 1205
    Height = 236
    Hint = ''
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    HorzScrollBar.ButtonSize = 8
    HorzScrollBar.Size = 15
    HorzScrollBar.Style = ssHotTrack
    HorzScrollBar.Tracking = True
    Anchors = [akLeft, akTop, akRight, akBottom]
    Color = clWindow
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Tahoma'
    Font.Style = []
    TabOrder = 7
    HeaderFont.Charset = DEFAULT_CHARSET
    HeaderFont.Color = clWindowText
    HeaderFont.Height = -19
    HeaderFont.Name = 'Tahoma'
    HeaderFont.Style = []
    HeaderHeight = 30
    HeaderSections = <
      item
        ImageIndex = -1
        Width = 50
      end>
    ImeName = ''
    ItemHeight = 16
  end
  object LMDGroupBox3: TLMDGroupBox
    Left = 437
    Top = 594
    Width = 331
    Height = 85
    Hint = ''
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    Bevel.Mode = bmWindows
    Caption = #1042#1099#1093#1086#1076#1085#1086#1081' '#1092#1072#1081#1083
    CaptionFont.Charset = DEFAULT_CHARSET
    CaptionFont.Color = clWindowText
    CaptionFont.Height = -19
    CaptionFont.Name = 'Tahoma'
    CaptionFont.Style = []
    TabOrder = 8
    object OneBtn: TLMDRadioButton
      Left = 28
      Top = 28
      Width = 103
      Height = 30
      Hint = ''
      Margins.Left = 5
      Margins.Top = 5
      Margins.Right = 5
      Margins.Bottom = 5
      Caption = #1054#1076#1080#1085
      Alignment.Alignment = agTopLeft
      Alignment.Spacing = 4
      Checked = True
      TabOrder = 0
      OnMouseUp = OneBtnMouseUp
    end
    object SomeBtn: TLMDRadioButton
      Left = 156
      Top = 27
      Width = 143
      Height = 30
      Hint = ''
      Margins.Left = 5
      Margins.Top = 5
      Margins.Right = 5
      Margins.Bottom = 5
      Caption = #1053#1077#1089#1082#1086#1083#1100#1082#1086
      Alignment.Alignment = agTopLeft
      Alignment.Spacing = 4
      Checked = False
      Enabled = False
      TabOrder = 1
      OnClick = SomeBtnClick
    end
  end
  object OutFileEdit: TLMDEdit
    Left = 170
    Top = 690
    Width = 1049
    Height = 37
    Hint = ''
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    Bevel.Mode = bmWindows
    Caret.BlinkRate = 530
    TabOrder = 9
    CustomButtons = <>
    PasswordChar = #0
    Text = ''
  end
  object BufEdit: TLMDEdit
    Left = 586
    Top = 133
    Width = 177
    Height = 37
    Hint = 
      #1056#1072#1079#1084#1077#1088' '#1073#1091#1092#1077#1088#1072' '#1076#1083#1103' '#1086#1087#1077#1088#1072#1094#1080#1081' '#1079#1072#1075#1088#1091#1079#1082#1080' '#1080#1079' '#1092#1072#1081#1083#1072', '#1087#1086#1080#1089#1082#1072' '#1080' '#1076#1077#1082#1086#1076#1080#1088#1086#1074 +
      #1072#1085#1080#1103
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    Bevel.Mode = bmWindows
    Caret.BlinkRate = 530
    ParentShowHint = False
    ShowHint = True
    TabOrder = 10
    CustomButtons = <>
    PasswordChar = #0
    Text = '1048576'
  end
  object OpenDialog1: TOpenDialog
    FileName = '(Get Folder)'
    Filter = 'Folders only|*.FOLDER'
    Left = 590
    Top = 165
  end
  object LMDAboutDlg1: TLMDAboutDlg
    Appname = 'Q.825 CDR  decoder  filter'
    Description = 
      'Q.825 CDR decoder and filter program, version 2.3.4 (build 41)'#13#10 +
      'Copyright (c) 2011 Aleksandr Shulaev, All Rights Reserved.'#13#10
    Left = 592
    Top = 217
  end
  object MainMenu1: TMainMenu
    Left = 597
    Top = 279
    object f1: TMenuItem
      Caption = #1060#1072#1081#1083
      object exit1: TMenuItem
        Caption = #1042#1099#1093#1086#1076
        OnClick = exit1Click
      end
    end
    object about1: TMenuItem
      Caption = #1055#1086#1084#1086#1097#1100
      object about2: TMenuItem
        Caption = #1054' '#1087#1088#1086#1075#1088#1072#1084#1084#1077'...'
        OnClick = about2Click
      end
    end
  end
end
