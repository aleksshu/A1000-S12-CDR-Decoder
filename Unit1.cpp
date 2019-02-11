//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"

#include <iostream.h>
#include <sys\stat.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <streambuf>
#include <dir.h>
#include <dirent.h>
#include <cstdlib.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LMDButton"
#pragma link "LMDCustomButton"
#pragma link "LMDBaseControl"
#pragma link "LMDBaseGraphicControl"
#pragma link "LMDBaseLabel"
#pragma link "LMDCustomLabel"
#pragma link "LMDDockButton"
#pragma link "LMDLabel"
#pragma link "LMDBaseGraphicButton"
#pragma link "LMDCustomSpeedButton"
#pragma link "LMDSpeedButton"
#pragma link "LMDButtonControl"
#pragma link "LMDControl"
#pragma link "LMDCustomBevelPanel"
#pragma link "LMDCustomControl"
#pragma link "LMDCustomGroupBox"
#pragma link "LMDCustomPanel"
#pragma link "LMDCustomParentPanel"
#pragma link "LMDGroupBox"
#pragma link "LMDRadioButton"
#pragma link "LMDBaseEdit"
#pragma link "LMDCustomEdit"
#pragma link "LMDEdit"
#pragma link "LMDAboutDlg"
#pragma link "LMDBaseDialog"
#pragma link "LMDContainerComponent"
#pragma link "LMDCustomComponent"
#pragma link "LMDCustomScrollBox"
#pragma link "LMDListBox"
#pragma link "LMDBaseMeter"
#pragma link "LMDGraphicControl"
#pragma link "LMDObj"
#pragma link "LMDCustomProgress"
#pragma link "LMDProgress"
#pragma link "LMDCheckBox"
#pragma link "LMDCustomCheckBox"
#pragma link "LMDCustomExtSpinEdit"
#pragma link "LMDCustomMaskEdit"
#pragma link "LMDSpinEdit"
#pragma resource "*.dfm"
TForm1 *Form1;

bool bFind = false, bCsv = false, bFilter = false, bCDRHeader = false;
bool bCDRExchangeID = false, bCDR_SWversion = false, bCDR_FileName = false;
bool bAnyPartOfNumber = false; // ищется любая часть номера
bool bTransparent = false; // прозрачный поиск - ищется без начального структурирования записей
int iInterFieldSize = 0;  // число байт между первыми полями A3
int bufPos = 0;  // текущая позиция в буфере чтения из файла =0
int iFilePos = 0; // текущая позиция в файле =0
int findRecSize =0;   // размер найденной записи
int iSpaceNumber = 0;
int iFoundRecordsNumber = 1; //, iCDRHeaderLength;
int iSplitPosition, iOutSplitFileNbr = 1, iDeltaSplit;
long int iFileSize =0;
//int iFindRecordStartPre;

 int handle, handle2, bytes, iTBCDlen; //, endig, numparts;
 int ic_blength, ic_Nlength;
 //int iIndex;
 void *xNsubsc, *xNsubsc2;
 char cTimebuf[32];  // для показа времени
 long iTimeValue;    // для показа времени

 int recordsize = 0x100;  // размер find_rec_buf =256 байт для найденной записи

 bool bOdd;  // признак нечетного номера - другой способ поиска
 bool bNsub, bAsub;
 bool bMultyFiles = false, bFileNamesMatch = false;
 bool bSplit = false;
 char *arg, *cSubsc, *cNsubsc, *cOddEven;
 //char *cbufsize = "1048576";
 char *sTime = " ";
 char *cInputFileName ="11111111111111111111111111111111111111\
 11111111111111111111111111111111111111\
 11111111111111111111111111111111111111\
 11111111111111111111111111111111111111";

 char *cOutputFileName ="11111111111111111111111111111111111111\
 11111111111111111111111111111111111111\
 11111111111111111111111111111111111111\
 11111111111111111111111111111111111111";

 FILE *outfile;

 struct stat statbuf;
 int bufsize = 1048576;

 void *buf;  // буфер для файла размером bufsize
 void *find_rec_buf;  // буфер для найденной записи
 
 char *path = NULL;
 size_t size;
 DIR *dir;
 struct dirent *ent;
 TStringList *FileNameList = new TStringList;
  TStringList *OutFileNameList = new TStringList;

//------------- ф-ция нахождения длины строки ------------------
int find_length(char* input)
{
 int length;
 asm {
	 lea edi, input     // адрес указателя
     xor ecx, ecx
	 mov edi, dword ptr [edi]  // адрес строки
 nexch2:
	 mov al, byte ptr [edi]  // загружаем байт - тот что лежит по адресу строки
	 inc edi                 // увеличиваем адрес строки
	 inc ecx     // длина    - увеличиваем счетчик длины
	 test al, al     // проверяем не конец ли строки
	 jnz nexch2      // если не конец идем на начало загрузки следующего символа

	 dec ecx  // длина найдена  - уменьшаем т.к. вышли из цикла с увеличениемы

     lea edx, length
     mov dword ptr [edx], ecx // длина
 }
 return length;    // в eax 
}

//------- ф-ция преобразования строки из 10-тичных цифр в целое число ---------

int char2int(char* input)
{
 int output, ilength;
 ilength = find_length(input);
  asm {
   //lea edi, input
   mov edi, dword ptr [input]   // загружаем адрес указателя строки

  //lea esi, ilength
  mov ecx, dword ptr [ilength]   //  загружаем длину

  xor eax, eax    // обнуляем eax - счетчик десятичных разрядов
  inc eax     // al =1  увеличиваем eax на 1
  xor esi, esi  // esi =0 , [edi-ecx] = input
                // обнуляем  esi - где будет храниться целое число
  // dec edi     // [edi] = 0 terminate for string input
  //inc edi  // [edi] = last char in string input = "1048576"
  add edi, ecx   // прибавляем к адресу строки длину
  dec edi        // [edi] = last char in string input = "1048576"
                // адрес последнего символа строки

  mov dl, byte ptr [edi]   // dl = last char of input - последний символ строки
  and edx, 0x0f      // edx = last char of input without fist semibyte of ASCII
                     // последний символ строки без первого полубайта
  add esi, edx   // esi = последний символ строки без первого полубайта
  dec edi        //  [edi] = next char begining from end of input
                 // следующий символ строки
  dec ecx        // уменьшаем длину

begint:
  mov bl, byte ptr [edi]  // bl = next char begining from end of input
						  // загружаем следующий символ с конца
  and ebx, 0x0f  // ebx = next char begining from end of input without first semibyte
				 // обнуляем первый полубайт
  imul eax, 10  // eax = eax * 10    умножаем eax на 10
  imul ebx, eax  // ebx = ebx * eax - умножаем число, полученное из символа
			  // на счетчик десятичных разрядов
  add esi, ebx    // esi = esi + ebx * eax  суммируем результаты в одном месте - esi
  dec edi     // [edi] = next char begining from end of input
			   // уменьшаем адрес строки
  dec ecx    // ecx = number of char in input begining from start
			  // уменьшаем длину
  jnz begint  // number != 0 ?  - не равно нулю? если да, иди назад

  lea edx, output  // загружаем  адрес выходной переменной
  mov dword ptr [edx], esi  // помещаем туда результат
	}
  return output;  // возвращаем адрес выходной переменной
}

//------ ф-ция преобразования ASCII строки из цифр в TBCD строку ------
// например, ASCII код 32 36 35 38 30 33 = TBCD код 26 58 03
void char2tbcd(char* input, int &ilength, void *SubNBuf)
{
 ilength = find_length(input); 
 asm {
  mov eax, ilength
  mov ecx, dword ptr [eax]  // длина

  mov eax, input

  mov edi, SubNBuf

  xor ebx, ebx
  
begin1:
  mov bl, byte ptr [eax]  // [eax] - first char input
  shl bl, 4
  mov bh, byte ptr [eax+1]
  and bh, 0x0f
  or bl, bh
  mov byte ptr [edi], bl  

  inc eax
  inc eax
  inc edi
  dec ecx
  dec ecx
  jnle begin1   // если не меньше или равно 0, т.е. >  =0
 }
 if(ilength>0) {
  ilength = ilength+1;
  ilength = ilength-1;
 } 
}


//----- функция печатания пустых полей в CSV формате заданное кол-во раз ------

/*void PrintSpace_csv(long iNumLoop)
{ 
  for(iNumLoop;iNumLoop>0;iNumLoop--)
  printf(cCsv_Space);
}     */

//------ функция вставки значения поля в CSV строку в нужное место ------
void InsertToCsvString(char &cString, char *cHeaderStr, char *cField, char *cValue, short iOffSet)
{    // строка         заголовок         поле         значение       смещение
     // смещение - для дополнительного значения к полю

 int iLength = find_length(cField);  //  длина поля
 asm {
  mov edi, cHeaderStr   // в edi указатель на заголовок
  mov esi, cField       // в esi указатель на поле
  xor ecx, ecx     // счетчик длины cField
  xor eax, eax
  xor edx, edx   // счетчик разделителей
  mov al, 0x3B   // это ASCII код разделителя ';'

  // поиск поля в заголовке
CmpNextBytes:
  cmpsb         // сравним байт [esi] cField с байтом [edi] cHeaderStr
  jz BytesEqual

  // байты не равны - проверим дошли мы или нет до конца заголовка 
  mov esi, cField   // на начало Field
  xor ecx, ecx
  cmp byte ptr [edi], 0 // конец заголовка cHeaderStr ?
  jz FieldNotFind

  // еще не конец заголовка
  dec edi  // предыдущий байт cHeaderStr
  scasb   // сравнить предыдущий байт [edi] cHeaderStr с ';'
  jnz CmpNextBytes  // если не равен ';' , то сравним следующие байты

  // найден разделитель
  inc edx   // увеличим счетчик разделителей
  jmp CmpNextBytes   // будем сравнивать следующие байты

FieldNotFind:
  //  если конец заголовка и поле в нем не найдено - вывести сообщение об этом
  push cValue
  push cInsertErrorFrmt
  //call printf
  //add esp, 8
  push outfile
  call fprintf
  add esp, 12
  
  mov eax, 1   // return 1
  jmp End    //  конец подпрограммы

BytesEqual:
  inc ecx   //  увеличиваем счетчик длины cField на 1
  cmp ecx, iLength // счетчик длины достиг значения iLength ?
  jnz CmpNextBytes    // если не достигли конца поля, то
					  // сравним следующие байты

// поле в заголовке найдено, искомая величина - edx - число разделителей перед
  // вставкой значения поля cValue
  add dx, iOffSet // прибавляем смещение - для дополнительного значения в поле

  // поиск адреса в cString для вставки значения cValue после
  // найденного числа разделителей
  mov edi, cString
  test edx, edx // число разделителей = 0 ?
  jz InsertValueToString // если число разделителей = 0 ?
                        // адрес вставки edi - начало строки cString

  // нахождение адреса вставки
ScanNextStringByte:
  cmp byte ptr [edi], 0   // конец строки ?
  jz StringEnd

  scasb  // сравнить байт cString [edi] с ';'
  jnz ScanNextStringByte  // если не равны

  // если равны
  dec edx     //  уменьшим счетчик разделителей
  jnz ScanNextStringByte  // если счетчик не 0, ищем дальше

  // счетчик разделителей edx = 0 и в edi - адрес вставки или
  // счетчик разделителей edx не 0 и в edi - конец cString 

StringEnd:
  test edx, edx  // счетчик разделителей = 0 ?
  jz InsertValueToString

  // если счетчик разделителей не 0
  mov ecx, edx    // счетчик разделителей
  rep stosb   // копируем al = ';' -> [edi]  ecx раз
              // копируем оставшееся число разделителей до вставки
  mov byte ptr [edi], 0 // делаем конец строки 
     // в edi - адрес вставки в cString

   // копирование подстроки SUBcString(адрес вставки, end) во
// временную строку cStr_csv_tmp2

InsertValueToString:
  // найдем длину cString
  push cString
  call find_length  // после выполнения в eax - длина cString
  add esp, 4
  
  //  найдем число байт после разделителя - счечик для копирования
  mov esi, cString  // адрес начала cString
  add esi, eax  // адрес конца cString
  sub esi, edi   // адрес конца - адрес вставки = число байт после разделителя
  mov ecx, esi   // число байт после разделителя - счечик копирования
  push ecx   // сохраним число байт после разделителя
  test ecx, ecx
  jz InsertValue   // если число байт = 0, т.е. адрес вставки = адрес конца

  mov esi, edi  // адрес вставки
  push edi  // сохраним адрес вставки
  mov edi, cStr_csv_tmp2

  rep movsb   //  коприуем [esi] SUBcString(адрес вставки, end) ->
              // -> [edi] cStr_csv_tmp2   ecx раз
              
  pop edi  // восстановим адрес вставки в cString

InsertValue:
  mov esi, cValue
  // найдем длину cValue
  push cValue
  call find_length  // после функции в eax - длина cValue
  add esp, 4
  
  mov ecx, eax  // число байт для копирования
  rep movsb   // коприуем [esi] cValue -> [edi] адрес вставки ecx раз
  
  pop ecx  // восстановим число байт после разделителя
  test ecx, ecx
  jz PutZeroInsertEnd

  mov esi, cStr_csv_tmp2
  rep movsb   // коприуем [esi] cStr_csv_tmp2 -> [edi] cString ecx раз

PutZeroInsertEnd:
  mov byte ptr [edi], 0  // закончим строку cString нулем
  mov eax, cString // return cString  */
  //pop edi   // выравнивание стека
End:
 }
}


//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::LMDDockButton2Click(TObject *Sender)
{
 if(OpenDialog1->Execute()) {
	  FileListBox1->Directory = ExtractFileDir(OpenDialog1->FileName);
 }
}
//---------------------------------------------------------------------------


void __fastcall TForm1::LMDDockButton1Click(TObject *Sender)
{
 if(OpenDialog1->Execute()) {
	  FileListBox2->Directory = ExtractFileDir(OpenDialog1->FileName);
 }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LMDSpeedButton1Click(TObject *Sender)
{
 FileListBox1->Update();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LMDSpeedButton2Click(TObject *Sender)
{
 FileListBox2->Update();	
}
//---------------------------------------------------------------------------


void __fastcall TForm1::AnySubBtnClick(TObject *Sender)
{
 NumberEdit->Enabled = AnySubBtn->Checked;
 bFilter = AnySubBtn->Checked;
 FilterCheckBox->Checked = bFilter;
 if(bFilter) FilterCheckBox->Caption = "on"; 
 bNsub = AnySubBtn->Checked;
 if(!OutFileEdit->Text.IsEmpty()) {
  FileListBox1Click(Application);
 }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ASubBtnClick(TObject *Sender)
{
 NumberEdit->Enabled = ASubBtn->Checked;
 bFilter = ASubBtn->Checked;
 FilterCheckBox->Checked = bFilter;
 if(bFilter) FilterCheckBox->Caption = "on";
 bAsub = ASubBtn->Checked;
 bNsub = !ASubBtn->Checked;
 if(!OutFileEdit->Text.IsEmpty()) {
  FileListBox1Click(Application);
 }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BSubBtnClick(TObject *Sender)
{
 NumberEdit->Enabled = BSubBtn->Checked;
 bFilter = BSubBtn->Checked;
 FilterCheckBox->Checked = bFilter;
 if(bFilter) FilterCheckBox->Caption = "on";
 bAsub = !BSubBtn->Checked;
 bNsub = !BSubBtn->Checked;
 if(!OutFileEdit->Text.IsEmpty()) {
  FileListBox1Click(Application);
 }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FilterCheckBoxMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 bFilter = FilterCheckBox->Checked;
 if(FilterCheckBox->Checked) FilterCheckBox->Caption = "on";
 else FilterCheckBox->Caption = "off";
 ASubBtn->Checked = bFilter;
 BSubBtn->Checked = bFilter;
 AnySubBtn->Checked = bFilter;
 FileListBox1Click(Application);	
}
//---------------------------------------------------------------------------



void __fastcall TForm1::about2Click(TObject *Sender)
{
 LMDAboutDlg1->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::exit1Click(TObject *Sender)
{
 Close();	
}

//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
 bCsv = CSV_Btn->Checked;
 if(!FileExists(".\\out\\")) mkdir(".\\out\\"); // если нет папки out в текущей директории, то создадим ее
 FileListBox2->Directory = ".\\out\\";
 EventListBox->HeaderSections->Items[0]->Width = EventListBox->Width; 
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CSV_BtnClick(TObject *Sender)
{
 bCsv = CSV_Btn->Checked;
 if(!OutFileEdit->Text.IsEmpty()) {
  OutFileEdit->Text = ChangeFileExt(OutFileEdit->Text,".csv");
 }
 SplitCheckBox->Enabled = bCsv;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TXT_BtnClick(TObject *Sender)
{
 bCsv = !TXT_Btn->Checked;
 if(!OutFileEdit->Text.IsEmpty()) {
  OutFileEdit->Text = ChangeFileExt(OutFileEdit->Text,".txt");
 }
 SplitCheckBox->Enabled = bCsv;
 SplitCheckBox->Checked = bCsv;
 bSplit = bCsv;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::NumberEditKeyUp(TObject *Sender, WORD &Key,
	  TShiftState Shift)
{
  if(!OutFileEdit->Text.IsEmpty()) {
   FileListBox1Click(Application);
 }
}
//---------------------------------------------------------------------------


void __fastcall TForm1::FileListBox1Click(TObject *Sender)
{
 if(FileListBox1->Selected[FileListBox1->ItemIndex]) {
 if(FileListBox1->SelCount>1) {
  SomeBtn->Enabled = true;
  for(int n=0;n<FileListBox1->Count;n++) {
   if(FileListBox1->Selected[n]) {  // самый первый выделенный файл
	OutFileEdit->Text = FileListBox1->Items->Strings[n];
	break;
   }
  }
  OutFileEdit->Text = OutFileEdit->Text + "_m";
 }
 else {
  OutFileEdit->Text = FileListBox1->FileName;
  SomeBtn->Enabled = false;
  OneBtn->Checked = true;
 }
 OutFileEdit->Text = ExtractFileName(OutFileEdit->Text);
 if(SomeBtn->Checked) OutFileEdit->Text = "*";
 if(bFilter) {
  if(ASubBtn->Checked) OutFileEdit->Text = OutFileEdit->Text + "_A_";
  if(BSubBtn->Checked) OutFileEdit->Text = OutFileEdit->Text + "_B_";
  if(AnySubBtn->Checked) OutFileEdit->Text = OutFileEdit->Text + "_";
  OutFileEdit->Text = OutFileEdit->Text + NumberEdit->Text;
  if(bAnyPartOfNumber) OutFileEdit->Text = OutFileEdit->Text + "_p";
  if(bTransparent) OutFileEdit->Text = OutFileEdit->Text + "_t";
 }
 //OutFileEdit->Text = OutFileEdit->Text + ".o";
 if(CSV_Btn->Checked) //OutFileEdit->Text = ChangeFileExt(OutFileEdit->Text,".csv");
  OutFileEdit->Text = OutFileEdit->Text + ".csv";
 else //OutFileEdit->Text = ChangeFileExt(OutFileEdit->Text,".txt");
  OutFileEdit->Text = OutFileEdit->Text + ".txt";
 OutFileEdit->Text = ExtractFileName(OutFileEdit->Text);
}
 DecodeBtn->Enabled = true; 
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SomeBtnClick(TObject *Sender)
{
 OutFileEdit->Enabled = SomeBtn->Checked;
 if(!OutFileEdit->Text.IsEmpty()) {
   FileListBox1Click(Application);
 }
}
//---------------------------------------------------------------------------


void __fastcall TForm1::OneBtnMouseUp(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
   if(!OutFileEdit->Text.IsEmpty()) {
   FileListBox1Click(Application);
 }	
}

//---------------------------------------------------------------------------

void __fastcall TForm1::SplitCheckBoxClick(TObject *Sender)
{
 bSplit = SplitCheckBox->Checked;
 SplitEdit->Enabled = bSplit;
 if(bSplit) SplitEdit->Value = 65500.;
 else SplitEdit->Value = 0;
}



//---------------------------------------------------------------------------

void __fastcall TForm1::DecodeBtnClick(TObject *Sender)
{
 String sFileName;
 iOutSplitFileNbr = 1;
 iDeltaSplit = SplitEdit->Value;
 iSplitPosition = iDeltaSplit;
 int iEndBuf;
 
 if(FileListBox1->SelCount) {    // если выделен файл
 DecodeBtn->Enabled = false;
 //EventListBox->Clear();

 EventListBox->Items->Add("Входные параметры:");

//---------------- печатаем время начала операции ---------------
 /*time (&iTimeValue);
 strftime(cTimebuf, 32, "%a %d %b %Y %H:%M:%S", localtime(&iTimeValue));
 sprintf(cStr_csv_tmp, "Time begining is %s\n", cTimebuf);
 EventListBox->Items->Add((AnsiString)(cStr_csv_tmp)); */
 DateTimeToString(sFileName,"dd.mm.yy hh:nn:ss,zzz",Now());
 EventListBox->Items->Add(" Время начала операции "+sFileName);

//-------------- подготовка к открытию файла -ов ----------------------
 if (FileListBox1->SelCount>1) { // если выделено больше чем 1 файл
  bMultyFiles = true;
  EventListBox->Items->Add(" входные файлы:");
  for(int l=0;l<FileListBox1->Count;l++) {
   if(FileListBox1->Selected[l]) {
	sFileName = FileListBox1->Directory + "\\" +
	  FileListBox1->Items->Strings[l];
	FileNameList->Add(sFileName);
	EventListBox->Items->Add(sFileName);
   }
  }
 }   // конец если много входных файлов
 else  {	 // если один входной файл
  sFileName = FileListBox1->FileName;
  EventListBox->Items->Add(" входной файл:");
  EventListBox->Items->Add(ExtractFileName(sFileName));
  //FileNameList->Add(FileListBox1->Directory + "\\" +sFileName);
  FileNameList->Add(sFileName);
 } 

//--------------- подготовка к открытию выходного файла -ов -------------

EventListBox->Items->Add(" выходные файлы:");
sFileName = OutFileEdit->Text;
if(!sFileName.IsEmpty()) {
 if(OneBtn->Checked)  {
  OutFileNameList->Add(FileListBox2->Directory +"\\" + sFileName);
  EventListBox->Items->Add(sFileName);
 }
 else {
  for(int j=0;j<FileNameList->Count;j++) {
   sFileName = ExtractFileName(FileNameList->Strings[j]) +
     OutFileEdit->Text.SubString(2,OutFileEdit->Text.Length());
   OutFileNameList->Add(FileListBox2->Directory+"\\" + sFileName);
   EventListBox->Items->Add(sFileName);
  }
 }
 if(bSplit)  {
 EventListBox->Items->Add("Разделить на части по " + IntToStr(SplitEdit->Value)+
  " CSV строк");
 iSplitPosition = SplitEdit->Value;
 }
 else iSplitPosition = 0;
}   

//-------------- печатаем номер абонента, если он введен ---------------
 if(bFilter) {
  xNsubsc = malloc(16);
  if(ASubBtn->Checked) cSubsc = "А";
  if(BSubBtn->Checked) cSubsc = "Б";
  if(AnySubBtn->Checked) cSubsc = "любой";
  cNsubsc = AnsiString(NumberEdit->Text).c_str();
  char2tbcd(cNsubsc, ic_Nlength, xNsubsc);
  if(ic_Nlength%2 == 1) {
   cOddEven = "нечетное";
   bOdd = 1;
   xNsubsc2 = malloc(14);  // max 14
  }
  else {
   cOddEven = "четное";
   bOdd = 0;
  }
  //sprintf(cStr_csv_tmp, "Абонент %s = %s, число цифр %s = %d\n",
  // cSubsc, cNsubsc, cOddEven, ic_Nlength);  
  if(bAnyPartOfNumber) printf(" Search all records with %s number, containing digits %s, digits lehgth is %s = %d\n",
 		 cSubsc, cNsubsc, cOddEven, ic_Nlength);
  else printf(" Search all records with %s number, begining with digits %s, digits lehgth is %s = %d\n",
		 cSubsc, cNsubsc, cOddEven, ic_Nlength);
  EventListBox->Items->Add((AnsiString)(cStr_csv_tmp));
  if(bTransparent) sprintf(cStr_csv_tmp, " Search type is transparent\n");
  else printf(cStr_csv_tmp, " Search type is record-by-record\n");
  EventListBox->Items->Add((AnsiString)(cStr_csv_tmp));
 }

 //bufsize = char2int(cbufsize);   // преобразуем ASCII символы в число
 //printf(" buffer size = %d bytes\n", bufsize);
 if(!BufEdit->Text.IsEmpty()) {
  try{
  bufsize = StrToInt(BufEdit->Text);
  }
  catch(...) {BufEdit->Text = "неверное целое!";}
 }
 buf = malloc(bufsize);  // создание буфера для файла размером bufsize

 find_rec_buf = malloc(recordsize);  // создание буфера для 1-й записи в памяти
 
 /*sprintf(cStr_csv_tmp, " buffer size = %d bytes\n", bufsize);
 EventListBox->Items->Add((AnsiString)(cStr_csv_tmp));  */
 EventListBox->Items->Add(" размер буфера = "+BufEdit->Text+" bytes\n");
 
 if(bCsv) EventListBox->Items->Add(" выходной формат CSV\n");
 //sprintf(cStr_csv_tmp, " output format is csv\n");
 else EventListBox->Items->Add(" выходной формат TXT\n");
 //sprintf(cStr_csv_tmp, " output format is text\n");
 //EventListBox->Items->Add((AnsiString)(cStr_csv_tmp));


//-------------------- подготовка отыскиваемого номера N --------------------

if(bFilter) {

 iTBCDlen = ic_Nlength/2.;  // длина номера в TBCD коде xNsubsc = ic_Nsubsc/2
 if(bOdd)  iTBCDlen++; // если ic_Nlength нечетный, то учитываем полубайт

 asm {
	 cmp bOdd, 1
	 //jnz BeginCircleRecordRead
	 jnz FilesCircle  // если длина номера четная, то ничего не делаем
	 
	 // если длина xNsubsc нечетная, то нужно искать
     // сначала байты с начала + полубайт в конце номера  xx xx xF
	 // потом полубайт с начала + байты в конце номера    Fx xx xx
	 // поэтому подготовим искомые номера из введенного номера  
     mov esi, xNsubsc2
     mov edx, xNsubsc
     mov ebx, [iTBCDlen] // ebx = длина xNsubsc
     add ebx, edx   // ebx = адрес последнего +1 байта xNsubsc  
     dec ebx       // адрес последнего байта xNsubsc
BufferCopy:
     mov al, byte ptr [edx]  // загрузим следующий байт xNsubsc
     mov byte ptr [esi], al    // сохраним al в [esi]
     inc esi
     inc edx
     cmp edx, ebx
	 jng BufferCopy  // если адрес не последний, копируем дальше

//------------- приведем xNsubsc2 к виду xx xx xF -----------------
     or al, 0xF    // al - байт с последним полубайтом в xNsubsc2 вида x0
     dec esi       // адрес последнего байта xNsubsc2
     mov byte ptr [esi], al   // приведем к виду  xF  , где x - цифра
     mov esi, xNsubsc2  // на первый байт xNsubsc2
//------------- приведем xNsubsc к виду Fx xx xx -------------------
     dec edx    // адрес последнего байта xNsubsc
xNsubscShift:
     mov al, byte ptr [edx]
     cmp edx, xNsubsc
     jz xNsubscShiftLast  // адрес текущего байта = адрес 1-го байта xNsubsc ?
	 mov ah, byte ptr [edx-1]
     and al, 0xF0
     and ah, 0x0F
     or al, ah
     ror al, 4    // поменять местами полубайты в al
     mov byte ptr [edx], al
     dec edx
     cmp edx, xNsubsc
     jnl xNsubscShift  // адрес текущего байта > адрес 1-го байта xNsubsc ?
	 //jmp BeginCircleRecordRead
	 jmp FilesCircle
     
xNsubscShiftLast:
     shr al, 4
     or al, 0xF0
     mov byte ptr [edx], al
	 }
 }



//---------------------- цикл по кол-ву файлов -----------------------

FilesCircle:
 for(int k=0; k<FileNameList->Count; k++) {  // цикл по кол-ву файлов

//---------------------- открытие ВХОДНОГО файла -ов ----------------------
  cInputFileName = AnsiString(FileNameList->Strings[k]).c_str();
  iFilePos = 0;
  handle = open(cInputFileName, O_RDONLY | O_BINARY);
  if(handle == -1)  {
   /*perror("open input file failed, error");
   return 1;  */
   EventListBox->Items->Add("не открывается входной файл: "+FileNameList->Strings[k]);
   continue;
  }

//-------------- создание и открытие выходного файла -ов ------------------

  /*handle2 = open(cOutputFileName, O_CREAT |O_APPEND | O_BINARY);
  if (handle2 != -1)  {	// argv[i+1] - выходной файл
     //lseek(handle2,0L,SEEK_SET);   // на начало
     //bOutfile = true;   // выводить в файл
	 //printf(" output file - %s\n", argv[i+1]);
   EventListBox->Items->Add(" output file:\n"+OutFileNameList->Strings[k]);
  }
	else {
	 //perror("create output file failed, error");
	 //return 1;
	 EventListBox->Items->Add("create output file failed");
	 goto END_PushButton;
	 continue;
	}   */           

  if(k==0 || SomeBtn->Checked) {
   cOutputFileName = AnsiString(OutFileNameList->Strings[k]).c_str();
	// outfile = указатель на файловый поток
   outfile = fopen(cOutputFileName, "w+"); // откроем для записи с добавлением
   if(outfile==0)  {
	EventListBox->Items->Add("не открывается выходной файл: "+
	  OutFileNameList->Strings[k]);
	continue;
   }
  }
      //  размер входного файла 
 //if(handle!=-1) { // stores information in the stat structure about the file
  fstat(handle, &statbuf);
  iFileSize = statbuf.st_size;
 //}
  Progress1->MaxValue = iFileSize;

 
 //buf = malloc(bufsize);  // создание буфера для файла размером bufsize

 // 1-е чтение из файла в буфер, bytes = размер буфера или размер файла,
   // если размер файла < размера буфера
 if ((bytes = read(handle, buf, bufsize)) == -1) {
  /*perror("file read failed, error\n");
  exit(1);  */
  EventListBox->Items->Add("не читается входной файл: "+FileNameList->Strings[k]);
  continue;
 }
 iFilePos = iFilePos + bytes;   // текущая позиция файла
 Progress1->UserValue = iFilePos;
 /*void *find_rec_buf;  // буфер для найденной записи
 find_rec_buf = malloc(recordsize);  // создание буфера в памяти */

//------ печатаем заголовок результата поиска для текстового формата ----------
asm {
     cmp bCsv, 1
	 jz EndPrintBeginOutputHeader

	 push sOutputResult
	 //call printf      // печатаем заголовок
	 //add esp, 4

     // печатаем в файловый поток
	 //int fprintf(FILE *stream, const char *format[, argument, ...]);

	 push outfile
	 call fprintf
	 add esp, 8


EndPrintBeginOutputHeader:
}
    //fclose(outfile);
 
//------------------ печатаем заголовок файла CDR ---------------------
asm {
     mov edi, buf   // адрес первого байта buf
	 mov eax, dword ptr [edi]
     cmp eax, 0x80A08030  // проверим, правильный ли заголовок CDR
     jz CDRHeaderPrint

	 cmp bCsv, 1
	 jz CDRHeaderEnd
	 
	 push cHeader
	 push cCDRHeaderErrorFrmt
	 //call printf
	 //add esp, 8

	 push outfile
	 call fprintf
	 add esp, 12
	 jmp CDRHeaderEnd 

CDRHeaderPrint:
     cmp bFilter, 1
     jz ProductionDateTimeBeg

	 push cHeader
	 push cCDR_File
	 push cCDRHeaderFrmt
	 //call printf
	 //add esp, 12

	 push outfile
	 call fprintf
	 add esp, 16

	 /*push outfile
	 call fclose
	 add esp, 4 */
	 
ProductionDateTimeBeg:// продолжим
     add edi, 4
     mov al, byte ptr [edi]
     cmp al, 4
     jz ProductionDateTime
     
	 push cProductionDateTime
     push cCDRHeaderErrorFrmt
	 //call printf
	 //add esp, 8

	 push outfile
	 call fprintf
	 add esp, 12
	 jmp CDRHeaderEnd 

ProductionDateTime:
	 inc edi
     mov cl, byte ptr [edi]
     cmp cl, 7           // длина ProductionDateTime = 7 ?
     jz DateTimeEvalPrep

	 push cProductionDateTime
     push cFieldWrongLengthFrmt
	 //call printf
	 //add esp, 8

	 push outfile
	 call fprintf
	 add esp, 12
	 jmp CDRHeaderEnd

DateTimeEvalPrep:

     cmp bFilter, 1
     jz ExchangeInfoCDRBeg

     mov bCDRHeader, 1
     xor eax, eax
     mov al, bCsv  // сохраним bCsv
     push eax
     mov bCsv, 0    // сбросим bCsv
     mov ebx, edi   // указатель buf
     jmp DateTimeEval

CDRProductionDateTimePrint:   //  edx - время, eax - дата

	 push edx
     push eax
     push cProductionDateTime
     push cProductionDateTimeFormat
	 //call printf
	 //add esp, 16

	 push outfile
	 call fprintf
	 add esp, 20

     pop eax
     mov bCsv, al  // восстановим bCsv

ExchangeInfoCDRBeg:
     add edi, 8    // прибавим к ук-лю buf длину ProductionDateTime +1 =7+1
     mov ax, word ptr [edi]
     cmp ax, 0x8031
     jz ExchangeInfoCDR

	 push cExchangeInfo
     push cCDRHeaderErrorFrmt
	 /*call printf
	 add esp, 8   */

	 push outfile
	 call fprintf
	 add esp, 12
	 jmp CDRHeaderEnd

ExchangeInfoCDR:
     inc edi
     inc edi
     mov al, byte ptr [edi]
     cmp al, 0x80
     jz ExchangeID_CDR

	 push cExchangeID
     push cCDRHeaderErrorFrmt
	 //call printf
	 //add esp, 8
	 
	 push outfile
	 call fprintf
     add esp, 12
	 jmp CDRHeaderEnd

ExchangeID_CDR:
     cmp bFilter, 1
     jz SoftwareVersionCDRBeg

     mov bCDRExchangeID, 1
     mov ebx, edi   // ук-ль buf
     push edi  // сохраним ук-ль buf
     jmp ExchangeID

ExchangeID_CDR_Print:   // edi = sDigits - строка ExchangeID
	 push sDigits      // строка ExchangeID
     push cExchangeID
     push cExchangeInfo
     push cExchangeInfoCDR_Frmt
	 //call printf
	 //add esp, 16    

	 push outfile
	 call fprintf
	 add esp, 20

     pop edi  // восстановим ук-ль buf
     mov bCDRExchangeID, 0

SoftwareVersionCDRBeg:
     xor eax, eax
     inc edi
     mov al, byte ptr [edi] // длина ExchangeID
     add edi, eax
     inc edi
     mov al, byte ptr [edi]
     cmp al, 0x81
     jz SoftwareVersionCDR

	 push cSoftwareVersion
     push cCDRHeaderErrorFrmt
	 //call printf
	 //add esp, 8

	 push outfile
	 call fprintf
     add esp, 12
	 jmp CDRHeaderEnd

SoftwareVersionCDR:
     cmp bFilter, 1
     jz  FileNameCDRBeg

     mov bCDR_SWversion, 1
     mov ebx, edi   // ук-ль buf
     push edi  // сохраним ук-ль buf
     jmp ExchangeID

SoftwareVersionCDR_Print:
     mov bCDR_SWversion, 0
	 pop edi   // восстановим ук-ль buf
	 
	 push sDigits      // строка ExchangeID
     push cSoftwareVersion
	 push cSoftwareVersionFrmt
	 //call printf
	 //add esp, 12
	 
	 push outfile
	 call fprintf
	 add esp, 16

FileNameCDRBeg:
     xor eax, eax
     inc edi
     mov al, byte ptr [edi]  // длина SoftwareVersion
     add edi, eax
     inc edi
     inc edi
     inc edi
     mov al, byte ptr [edi]
     cmp al, 0x19
     jz FileNameCDR

	 push cFileName
     push cCDRHeaderErrorFrmt
	 //call printf
	 //add esp, 8

	 push outfile
	 call fprintf
     add esp, 12
	 jmp CDRHeaderEnd

FileNameCDR:
     cmp bFilter, 1
     jz  ReasonForOutputCDRBeg

     mov bCDR_FileName, 1
     mov ebx, edi   // ук-ль buf
     push edi  // сохраним ук-ль buf
     jmp ExchangeID

FileNameCDR_Print:
     mov bCDR_FileName, 0
	 pop edi   // восстановим ук-ль buf

	 push sDigits      // строка ExchangeID
	 push cFileName
	 push cSoftwareVersionFrmt
	 //call printf
	 //add esp, 12   

	 push outfile
	 call fprintf
	 add esp, 16

ReasonForOutputCDRBeg:
     inc edi
     mov al, byte ptr [edi]  // длина FileName
     add edi, eax
     inc edi
     mov al, byte ptr [edi]
     cmp al, 0x0A
     jz ReasonForOutputCDR

	 push cReasonForOutput
     push cCDRHeaderErrorFrmt
	 //call printf
	 //add esp, 8

	 push outfile
	 call fprintf
     add esp, 12
	 jmp CDRHeaderEnd

ReasonForOutputCDR:
     inc edi
     mov al, byte ptr [edi]
     cmp al, 1   // длина ReasonForOutput = 1 ?
     jz ReasonForOutputCDR_Eval

	 push cReasonForOutput
     push cFieldWrongLengthFrmt
	 //call printf
	 //add esp, 8

	 push outfile
	 call fprintf
     add esp, 12
	 jmp CDRHeaderEnd

ReasonForOutputCDR_Eval:
     inc edi
     cmp bFilter, 1
     jz BodyCDRBeg

     xor eax, eax
     mov al, byte ptr [edi]
     test al, al
     jz AbsoluteTimeEvent
     cmp al, 1
     jz MaxBlockSizeReached
     cmp al, 2
     jz MaxTimeIntervalElapsed
     cmp al, 3
     jz IternalSizeLimitReached
     cmp al, 4
     jz oSAction

	 push cReasonForOutput
	 push cCDRHeaderErrorFrmt
	 //call printf
	 //add esp, 8

	 push outfile
     call fprintf
     add esp, 12
	 jmp CDRHeaderEnd

oSAction:
     push coSAction
     jmp oSActionPrint

IternalSizeLimitReached:
     push cIternalSizeLimitReached
     jmp oSActionPrint

MaxTimeIntervalElapsed:
     push cMaxTimeIntervalElapsed
     jmp oSActionPrint

MaxBlockSizeReached:
     push cMaxBlockSizeReached
     jmp oSActionPrint

AbsoluteTimeEvent:
     push cAbsoluteTimeEvent

oSActionPrint:
     push eax
     push cReasonForOutput
     push cPrintField2string1HexFrmt
	 //call printf
	 //add esp, 16

	 push outfile
     call fprintf
     add esp, 20

BodyCDRBeg:
     inc edi
     inc edi
     inc edi
     mov ax, word ptr [edi]
     cmp ax, 0x80A1
     jz BodyCDR

     push cBody
     push cCDRHeaderErrorFrmt
	 //call printf
	 //add esp, 8

	 push outfile
     call fprintf
     add esp, 12
     jmp CDRHeaderEnd

BodyCDR:
     cmp bFilter, 1
     jz  CDRHeaderEnd
     
     push cBody
	 //call printf
	 //add esp, 4

	 push outfile
     call fprintf
     add esp, 8

	 inc edi
	 inc edi   // edi - в конце заголовка +1, т.е. на начале 1-й записи
     
CDRHeaderEnd:
	 mov bCDRHeader, 0  // сбросим признак расшифровки заголовка
	 /*mov eax, edi
	 sub eax, dword ptr [buf] // edi - длина CDR заголовка до Body
	 inc eax
	 inc eax
	 mov dword ptr [iCDRHeaderLength], eax  // длина CDR заголовка */

	 cmp bCsv, 0
	 jz  SearchOrFullParse
	 
	 PrintRecordHeader_csv:
	 push cRecordHeader_s2
	 push cRecordHeader_s1
	 push cRecordHearderFrmt
	 //call printf      // печатаем заголовок для CSV формата
	 //add esp, 12

	 push outfile
     call fprintf
     add esp, 16

SearchOrFullParse:
	 cmp bFilter, 1
	 //jnz BeginCircleRecordRead

	 /*mov bCDRHeader, 0  // сбросим признак расшифровки заголовка
	 mov edi, buf
	 add edi, iCDRHeaderLength // edi - на позиции конца CDR заголовка */

}

//SearchNumberBegin:



//------------- цикл по сравнению позиции файла с его разиером --------------
//CircleBegin:
BeginCircleRecordRead:
//------------- цикл последовательного чтения и смешанной записи ------------
 //for(int i=1;i<=numparts;i++) {  // цикл по кол-ву частей файла
  //for(int n=0;n<iTBCDlen;n++) {  // цикл по символам внутри строки поиска
  // for(int k=0;k<bufsize;k++) {  // цикл по символам внутри буфера
    //if( strchr( buf, xAsubsc[n]) ) printf("pos.buf %d", k);
    
	asm {
	 /*mov eax, iFilePos
	 cmp eax, iFileSize  // сравним позицию файла с его размером
	 jge CircleEnd    // если больше или равно - на конец

	 // если меньше, читаем дальше  */

	 /*cmp bCDRHeader, 1  // CDR заголовок есть ?
	 jnz BeginCircleRecordRead

	 mov bCDRHeader, 0  // сбросим признак расшифровки заголовка
	 add edi, iCDRHeaderLength // edi - на позиции конца CDR заголовка */

//BeginRecordRead:
     // прибавим 8 байт для поиска начала записи A3 назад или номера вперед
	 //add edi, 8
     cmp bFilter, 1  // проверим, введен ли номер для поиска
	 //jz FindBeg  // если введен номер для поиска, ищем записи с номером
	 jnz StartRecord // если нет фильтра - к раскодировке всех записей файла
	
	cmp bTransparent, 1
						// если не прозрачный поиск - к последовательной раскодировке записей файла
	jnz StartRecord   	// и затем поиск номера внутри нее
	 /*// если не введен номер - приступим к раскодировке всех записей файла
	 mov ecx, dword ptr [bytes] // размер загруженных в buf данных из файла
	 add ecx, buf  // = адрес последнего байта buf
	 cmp ecx, edi  //  сравнить адрес последнего и текущего байта buf
	 jl NexFileRead   // если ecx < edi , то конец buf
	 jmp StartRecord       */

	 //cmp i, 1 // только для 1-й загрузки из файла в буфер
	 //jnz FindRecordStart
	 // прибавим 8 байт для поиска начала записи A3 назад или номера вперед
	 //add edi, 8
	 //jmp FindRecordStart

	 
FindBeg:   // ищем записи с номером
	 
	 mov ecx, dword ptr [bytes] // размер загруженных в buf данных их файла =
	 add ecx, buf  // адрес последнего байта buf +1
	 dec ecx     // = адрес последнего байта buf
         mov iEndBuf, ecx  // созраним адрес последнего байта buf

FindBeg2:             
     mov edx, xNsubsc  // адрес первого байта xNsubsc
     mov ebx, [iTBCDlen] // ebx = длина xNsubsc
     add ebx, edx   // ebx = адрес последнего байта xNsubsc +1
     dec ebx       // адрес последнего байта xNsubsc

     mov esi, xNsubsc2

//---------------------- поиск номера N в buf -----------------------
     xor ah, ah   //  сброс флага нахождения
CmpNexByte:
     mov al, byte ptr [edx]  // загрузить следующий байт xNsubsc
     cmp bOdd, 1       //  если нечетное число цифр введенного номера
     jz FindOddNsubsc

//-------------- поиск ЧЕТНОГО числа цифр введенного номера ------------
     cmp byte ptr [edi], al  // сравним байты buf и xNsubsc
     jz NsubscEqual

NotEqual:            // если не равны и xNsubsc, и xNsubsc2
	 //test ah, ah     //  проверить, сброшен ли флаг нахождения
     //jz NexBufbyte   // если ДА, переходим к следующему байту buf

      // если НЕ сброшен флаг нахождения
     mov ah, 0  // сбросить флаг нахождения
	 mov edx, xNsubsc     // адрес первого байта xNsubsc
	 //mov esi, xNsubsc2  // адрес первого байта xNsubsc2
     jmp NexBufbyte
     
 //-------------- поиск введенного нечетного номера -------------------
FindOddNsubsc:     //  НЕчетное число цифр введенного номера
     cmp edx, xNsubsc  // если текущий байт xNsubsc - это первый байт xNsubsc
     jz Compare1stByteNsubsc

     // если не первый байт xNsubsc
     cmp byte ptr [edi], al  // сравним байты buf и xNsubsc
     jz NsubscEqual

     // если не равен не первый байт xNsubsc и buf
     mov edx, xNsubsc // проверим опять первый байт xNsubsc на текущем байте buf
     jmp Compare1stByteNsubsc

Compare_xNsubsc2:
     push esi   //  перейдем к сравнению xNsubsc2
     sub esi, [iTBCDlen] // вычтем из текущего адреса xNsubsc2 его длину
     inc esi             // прибавим 1
     cmp esi, xNsubsc2  // сравним с начальным адресом
     pop esi
     jz CompareLastByteNsubsc2 // если текущий байт xNsubsc2 - последний

     // если текущий байт xNsubsc2 не последний
     mov al, byte ptr [esi]  // загрузить следующий байт xNsubsc2
     cmp byte ptr [edi], al  // сравнить байты buf и xNsubsc2
     jz Nsubsc2Equal

	 mov esi, xNsubsc2  // адрес первого байта xNsubsc2
	 jmp NotEqual

Compare1stByteNsubsc:     // сравним первый байт xNsubsc
     mov al, byte ptr [edi]  // загрузить байт buf
     or al, 0xF0  // ставим маску
     cmp byte ptr [edx], al  // сравнить байты xNsubsc и buf по маске Fx
     jnz Compare_xNsubsc2   // если не равны перейдем к сравнению xNsubsc2

	 // если равны, проверим, равен ли байт из Nsubsc2 или из NSubsc ?
	 cmp ah, 2           // если равен 2 - признак "найден xNsubsc2", то
	 jz Compare_xNsubsc2 // переходим к сравнению следующего байта xNsubsc2

	  // если признак не равен 2, то сравним Nsubsc
NsubscEqual:     // если  равны xNsubsc и buf
     mov ah, 1  // признак "найден xNsubsc"
	 cmp ebx, edx  // сравнить адрес последнего и текущего байта xNsubsc
     je NumberFieldCheck // если ebx = ebx, найдены все байты xNsubsc в buf

      //  найдены НЕ все байты xNsubsc
     inc edx  //  следующий байт xNsubsc
     jmp NexBufbyte

CompareLastByteNsubsc2:
     mov al, byte ptr [edi]  // загрузить байт buf
     or al, 0x0F  // ставим маску
     cmp byte ptr [esi], al  // сравнить байты xNsubsc2 и buf по маске xF
	 //jnz NotEqual
	 jz Nsubsc2Equal

	 mov esi, xNsubsc2  // адрес первого байта xNsubsc2
	 jmp NotEqual
	 
     // найден xNsubsc2 - 2-й вариант поиска нечетного номера
Nsubsc2Equal:
     mov ah, 2  // признак "найден xNsubsc2"
     push ebx  
     mov ebx, [iTBCDlen] // ebx = длина xNsubsc2
     add ebx, xNsubsc2   // ebx = адрес последнего +1 байта xNsubsc2
     dec ebx       // адрес последнего байта xNsubsc2
     cmp esi, ebx  // сравним текущий и последний адреса xNsubsc2
     pop ebx
     jnl NumberFieldCheck // если esi >= ebx, найдены все байты xNsubsc2 в buf

     //  найдены НЕ все байты xNsubsc2
     inc esi  //  следующий байт xNsubsc2

NexBufbyte:
     inc edi  // следующий байт buf
     cmp ecx, edi  //  сравнить адрес последнего и текущего байта buf
     jl NexFileRead   // если ecx < edi , то конец buf

       // если не конец buf
     jmp CmpNexByte   // сравним следующий байт buf

      // проверка, что найдено значение поля, содержащего номер абонента
NumberFieldCheck:
     push edi    //  сохраним указатель buf
     push ecx  // сохраним  адрес последнего байта buf
	 xor ecx, ecx
	 xor eax, eax

SearchParticipantInfo:
     dec edi
     mov al, byte ptr [edi]
     cmp al, 0xA2   // поле participantInfo ?
     jz ThisIsParticipantInfo
     cmp al, 0xAE   // поле AdditionalParticipantInfo ?
	 jz ThisIsParticipantInfo
	 cmp al, 0xA1
	 jz ThisIsTimeStartStamp
	 cmp al, 0xB8
	 jz NexBufbytePre  //ThisIsCallDuration
	 cmp al, 0xA3   // поле AdditionalParticipantInfo ?
	 jnz CheckBeginBuf  // additionalRecordTypes-ME ?

	 // нашли A3
	 mov al, byte ptr [edi+1]
	 cmp eax, 4 // оценим длину поля A3 (вместо длины может быть =81)
     jg NexBufbytePre // если длина > 4, это поле additionalRecordTypes-ME,
                    // значит отыскиваемый номер не содержится в поле A2 или AE

     // если длина поля A3 <= 4, это поле bearerService - продолжаем поиск

CheckBeginBuf:
     cmp edi, dword ptr [buf]
	 jnl SearchParticipantInfo  // если текущий адрес buf не меньше начального,
                                                      //  т.е. >=
     // если меньше, то пришли к началу буфера, искать больше негде
	 push cSearchErr
	 //call printf    // печатаем ошибку
	 //add esp, 4

	 push outfile
     call fprintf
     add esp, 8
	 jmp NexBufbytePre

/*ThisIsCallDuration:
	 inc edi
	 mov cl, byte ptr [edi]
	 cmp ecx, 5  // длина CallDuration должна быть МЕНЬШЕ 5
	 jg NexBufbytePre

	 // если меньше проверим, точно ли это поле CallDuration
	 inc edi
	 mov al, byte ptr [edi]
	 and al, 0xFE
	 cmp al, 0x80
	 jnz NexBufbytePre // следующий байт должен = 8x (80, 81, 82, 83)

	 // если равен 8x
	 inc edi
	 mov al, byte ptr [edi]
	 sub cl, al
	 cmp ecx, 2
	 jnz NexBufbytePre
	 jmp NexBufbytePre  // */

ThisIsTimeStartStamp:
	 inc edi
	 mov cl, byte ptr [edi]
	 cmp ecx, 9  // длина TimeStartStamp всегда равна 9
	 jz NexBufbytePre
	 
	 dec edi
	 jmp SearchParticipantInfo

ThisIsParticipantInfo:  //  или AdditionalParticipantInfo
     inc edi
	 mov cl, byte ptr [edi]  // длина ParticipantInfo
     add edi, ecx    // адрес последнего байта ParticipantInfo
     cmp edi, dword ptr [esp+4]   // сравним с указателем buf
     jl NexBufbytePre    // если адрес последнего байта МЕНЬШЕ ук-ля buf,
                // то найденное значение НЕ входит в поле ParticipantInfo
     //  если адрес последнего байта БОЛЬШЕ ук-ля buf,
     //  то найденное значение входит в поле ParticipantInfo
	 cmp al, 0xA2
	 //jz FindRecordStartPre  // если поле ParticipantInfo - на поиск начала записи
                                 // /*
jnz CheckAdditionalParticipantInfo

	   // если поле 0xA2 ParticipantInfo, проверим, что найденное значение
	   // входит в субполе "adress digits"
	 xor eax, eax
	 sub edi, ecx  // переместимся на длину ParticipantInfo
	 
NexSubFieldParticipantInfo:
	 inc edi   // тип субполя ParticipantInfo
	 inc edi
	 mov al, byte ptr [edi] // длина субполя ParticipantInfo
	 add edi, eax   // последний байт субполя ParticipantInfo
	 cmp dword ptr [esp+4], edi // если ук-ль buf БОЛЬШЕ последнего байта
	 jg NexSubFieldParticipantInfo // субполя, идем к следующему субполю

	 // если ук-ль buf МЕНЬШЕ или равен ук-лю на последнеий байт,
	 // проверим, что он в пределах "adress digits"
	 sub edi, eax // назад на длину cубполя ParticipantInfo
	 inc edi  // идем на начало "adress digits" -
	 inc edi  // пропускаем 2 байта -
	 inc edi  // odd/even, Nat.of.addr., Numb.plan ind.
	 mov eax, dword ptr [esp+4]  // ук-ль buf
	 sub eax, [iTBCDlen] // вычтем из текущего ук-ля buf длину xNsubsc/xNsubsc2
	 inc eax             // прибавим 1
	 cmp eax, edi  // сравним с началом "adress digits"
	 jl NexBufbytePre //если МЕНЬШЕ, ищем номер дальше

     // если БОЛЬШЕ или равно
	 cmp bNsub, 1           // если любой абонент - и А, и Б,
	 //mov iFindRecordStartPre, 1
	 jz FindRecordStartPre  // то идем на поиск начала записи

	 // если не любой абонент
	 sub edi, 4  // идем назад на тип субполя ParticipantInfo - ParticipantId
	 mov al, byte ptr [edi]
	 and al, 0x0F
	 test al, al  // CallingPartyNumber ?
	 jz A_Subsc
	 cmp al, 1   // CalledPartyNumber ?
	 jz B_Subsc
	 cmp al, 2   // RedirectingNumber ?
	 jz A_Subsc
	 cmp al, 3   // RedirectionNumber ?
	 jz B_Subsc
	 cmp al, 4   // OriginalCalledNumber ?
	 jz B_Subsc
	 cmp al, 5   // CallingPartyNumberNotScreened ?
	 jz A_Subsc
	 jmp NexBufbytePre
	 
A_Subsc:
	 cmp bAsub, 1  // если абонент А
	 //mov iFindRecordStartPre, 2
	 jz FindRecordStartPre
	 jmp NexBufbytePre

B_Subsc:
	 cmp bAsub, 1  // если абонент А
     //mov iFindRecordStartPre, 3
	 jnz FindRecordStartPre
	 jmp NexBufbytePre
	 
	 /*jge FindRecordStartPre // если позиция в buf на начало найденного номера
				 // БОЛЬШЕ или равна позиции начала "adress digits",
				// то найденное значение входит в "adress digits"

	 //если нет, то ищем номер дальше
	 jmp NexBufbytePre  */

CheckAdditionalParticipantInfo:  // */
	 cmp bAsub, 1     // это поле содержит информацию только для аб-а Б
	 jz NexBufbytePre

     //  если поле AdditionalParticipantInfo, проверим, что найденное значение
     //  находится в субполе 81 - receivedDigits
     sub edi, ecx  // переместимся на длину AdditionalParticipantInfo
     inc edi  // поле 80 - physicalLineCode
     inc edi
     mov cl, byte ptr [edi]  // длина поля 80
     add edi, ecx  // адрес последнего байта поля 80
     cmp edi, dword ptr [esp+4]   // сравним с указателем buf
	 jge NexBufbytePre  // если адрес последнего байта БОЛЬШЕ или равен ук-ля buf,
                // то найденное значение ВХОДИТ в поле physicalLineCode
     //  если если адрес последнего байта МЕНЬШЕ ук-ля buf,
     //  то найденное значение НЕ входит в поле physicalLineCode,
     //  т.е. входит в поле 81 receivedDigits

     //test ah, ah
     //jz NexBufbytePre  // если поле 80 - physicalLineCode, искать дальше

     // если поле 81 - receivedDigits
	 cmp bOdd, 1  // если введенный номер нечетный
	 //mov iFindRecordStartPre, 4
	 jnz FindRecordStartPre  // если четный - поиск начала записи

     // если нечетный - проверим правильность найденного номера
     // edi - адрес последнего байта поля 80

     /*inc edi   // длина AdditionalParticipantInfo
     inc edi  //  поле  80 - physicalLineCode
     mov al, byte ptr [edi]
     inc edi
     xor ecx, ecx
     mov cl, byte ptr [edi] // длина physicalLineCode

IncEdi:
     inc edi
     loop IncEdi  */

     inc edi   // поле 81 receivedDigits
     inc edi
     mov cl, byte ptr [edi]   // длина receivedDigits
     inc edi
     mov al, byte ptr [edi]  // odd/even, nature of addr. ind.
     and al, 0x80
	 cmp al, 0x80   // проверим odd/even ind.
	 //mov iFindRecordStartPre, 5
	 jnz FindRecordStartPre   // если even ind. (al=0) и найден введенный
     // нечетный номер, выводим в результат - идем к поиску начала записи

     //  если номер нечетный (al=80), сравним номера еще раз
CompareOddNuber_ReceivedDigits:
     inc edi
     mov al, byte ptr [edi]
     mov esi, xNsubsc2
     cmp al, byte ptr [esi]
     jnz NexBufbytePre  // если не равны, возвращаемся к поиску дальше

     // если байты равны
     dec cl  // последний байт цифр ?
     jnz CompareOddNuber_ReceivedDigits

	 // если да
	 //mov iFindRecordStartPre, 6
	 jmp FindRecordStartPre

NexBufbytePre:
     pop ecx // восстановим адрес последнего байта buf
     pop edi  //  восстановим указатель buf
     jmp NexBufbyte

//------------------- поиск начала записи при поиске номера -------------------
FindRecordStartPre:
     pop ecx // восстановим адрес последнего байта buf
     pop edi // восстановим указатель buf
	 mov byte ptr [bFind], 1  // установим признак "найдено"

FindRecordStart: // начало поиска начала записи
	 // поиск назад байта 0xA3
     dec edi    // уменьшаем на 1 адрес buf
     cmp byte ptr [edi], 0xA3  // искать A3 назад
	 jnz FindRecordStart

     // если нашли A3
	 cmp byte ptr [edi+1], 3     // длина =3 (Bearer Service ?)
	 jz FindRecordStart

	 // если нашли A3 поле 
	 xor ecx, ecx   // счетчик байт до предыдующего A3
CheckRecordStart:
     dec edi   // уменьшаем на 1 адрес buf
	 inc cl    // счетчик байт до предыдующего A3
     cmp byte ptr [edi], 0xA3  // искать A3 назад
     jnz CheckRecordStart

     // если нашли A3 опять
     cmp cl, 8           // число байт между байтами A3 > 8
	 jg FindRecordStart  //  искать дальше назад
	 cmp cl, 1      // если поля A3 стоят вместе, то одно из них - длина
	 jz CheckRecordStart
	 
     // если число байт между байтами A3 < 8 байт - начало Записи
StartRecord:
     /*mov ecx, dword ptr [bytes] // размер загруженных в buf данных из файла
	 add ecx, buf  //  адрес последнего байта buf +1
	 dec ecx     // адрес последнего байта buf
	 cmp ecx, edi  //  сравнить адрес последнего и текущего байта buf
	 jl NexFileRead   // если ecx < edi , то конец buf  */

	 xor ecx, ecx  // edi - начало записи
	 xor edx, edx
	 xor eax, eax
	 mov al, byte ptr [edi]
	 cmp al, 0xA3     // проверим, что начало записи начинается на A3
	 jz FindNexForLength

	 push cRecordContent
	 push eax
	 jmp notField

FindNexForLength:  // ищем длину записи 1-го поля A3, к-рая находится
	 inc edi       // перед 2-ым полем A3, поэтому ищем 2-е поле A3
	 inc cl  // счетчик байтов
	 mov al, byte ptr [edi]   // место Access Violation
	 cmp al, 0xA3   // ищем 2-е поле типа A3 - additionalRecordTypes-ME 
	 jz Found2ndFieldA3

	 // если не нашли, проверим не конец ли буфера
	 mov eax, buf
	 add eax, bufsize  // адрес последнего байта в buf +1
	 cmp edi, eax  // сравним текущий и последний адреса buf
	 jl FindNexForLength

	 // если текущий адрес buf больше последнего, то нам нужно
	 // загрузить из файла с позиции начала данной записи
	 // ecx - число байт от начала записи
	 // edi = eax = конец buf +1 = iFilePos

	 mov esi, dword ptr [iFilePos] //
	 sub esi, ecx // уменьшим позицию файла на длину 1-й части записи
	 mov dword ptr [iFilePos], esi // 
	 jmp EndBuf_lseek
	 
	 /*test eax, eax // а не нули ли мы загружаем, т.е. не конец ли буфера
	 cmp ecx, 0xB0  // проверим сколько раз уже ищем
	 jl FindNexForLength 
	 
	 //  если счетчик больше 0xB0 - наибольшая предполагаемая длина записи

     push cRecordContent
	 push eax
	 jmp notField  */

Found2ndFieldA3:
	 // нашли A3
	 mov al, byte ptr [edi+1]
	 cmp al, 0xA3    // проверим, что следующее поле не A3,
     // т.е найденное A3 не длина первого поля A3, а начало второго поля A3
	 jnz A3NotLengthOf1stFieldA3

	 // если два подряд встречаются A3, то
	 inc edi     //  длина 1-го поля A3 = A3, мы переходим на байт вперед -
				 // на 2-й байт A3
	 inc cl      // счетчик байтов + 1
	 
A3NotLengthOf1stFieldA3:

	 // edi - начало 2-го поля A3
	 mov dl, byte ptr [edi-1]  // длина записи перед 2-ым полем A3
	 sub edi, ecx  // на начало записи

	 add dl, cl   // общий размер записи
	 mov dword ptr [iInterFieldSize], ecx // число байт между первыми полями A3

//----- начало копирования записи с найденым номером из buf в find_rec_buf ----
	 mov cl, dl  // cl - счетчик размера записи
	 mov ebx, find_rec_buf  // адрес 1-го байта find_rec_buf
	 mov esi, dword ptr [bytes] // размер загруженных в buf данных из файла ...99184
	 add esi, buf  // адрес последнего байта buf +1
	 dec esi      // адрес последнего байта buf

	 // проверка разрывается ли запись концом буфера
	 mov eax, edi  // edi - начало записи
	 add eax, edx     // eax - адрес конца текущей записи +1
	 dec eax       // адрес конца текущей записи
	 cmp eax, esi   // сравним адрес конца текущей записи с адресом конца buf
	 jle copynex   // если адрес конца записи <= адреса конца buf,
				   // то запись находится в пределах буфера

	 // если адрес конца записи больше адреса конца buf,
	 // то запись разрывается концом buf
	 sub eax, esi  // длина 2-й части записи    (edi = bufPos)
	 sub eax, edx   // длина 2-й части - общая длина = отриц. длина 1-й части
	 //neg eax    // длина 1-й части 
	 mov esi, dword ptr [iFilePos] // 1048576, 2097152, 3145728, 4194304 ... 28409333
	 cmp esi, dword ptr [iFileSize]  // проверим, на конце ли позиция файла ?
	 jnz ContinueCorrectFilePos

	 // если да, то последняя запись сохранена в файле не полностью
	 //push eax
	 push cLastCDRerror
	 push outfile
	 call fprintf
	 add esp, 8
	 inc iFoundRecordsNumber
	 jmp CircleEnd
	 
	 /*// для раскодирования неполной записи - пока не реализовано
	 pop eax
	 neg eax  // длина записанной 1-й части записи
	 mov ecx, eax
	 mov edx, eax
	 jmp copynex   */

ContinueCorrectFilePos:
	 add eax, esi // уменьшим позицию файла на длину 1-й части записи ... -4
	 mov dword ptr [iFilePos], eax // ... 28409329
	 jmp EndBuf_lseek
	 
copynex:
	 mov al, byte ptr [edi] // загрузить байт из buf
     mov byte ptr [ebx], al // копировать в find_rec_buf найденную запись из buf
	 inc edi   // следующий байт buf
	 /*cmp esi, edi  //  сравнить адрес последнего и текущего байта buf
	 jnl  ContinueCopy  // если esi >= edi , то НЕ конец buf  
	 // если esi < edi , то конец buf
	 sub dl, cl  // число байт от начала записи до конца буфера
	 mov eax, dword ptr [iFilePos]
	 //and edx, 0xFF
	 sub eax, edx   // уменьшим позицию файла на число байт отрезанной записи
	 mov dword ptr [iFilePos], eax
	 jmp EndBuf_lseek  */
	 /*// lseek(int handle, long Offset, int Origin)  Origin - SEEK_CUR, SEEK_SET
	 push 1   // int Origin = SEEK_SET
	 push eax  // long Offset
	 push handle
	 call lseek      // установка позиции файла
	 jmp NexFileRead   */
//ContinueCopy:
	 inc ebx   // следующий байт find_rec_buf
	 dec cl  // обратный счетчик размера записи
	 jnz copynex   // если не конец записи, копировать следующий байт buf

          //  вся запись скопирована
	 //and edx, 0xff  // общий размер записи - в dl
     mov findRecSize, edx
     sub ebx, edx  // вычесть из адреса последнего байта размер записи -
                   //  перейдем в начало find_rec_buf

	 mov bufPos, edi // сохранить адрес текущей позиции буфера buf в bufPos:
					 // edi - первый байт следующей записи после текущей

//---- парсинг, расшифровка найденной записи из find_rec_buf и вывод данных ---
     cmp bCsv, 1
     jz LoadBeginRecord     // если csv

     push iFoundRecordsNumber
	 push cRecordContent
     push cRecordContentFrmt
	 //call printf
	 //add esp, 12

	 push outfile
     call fprintf
     add esp, 16
     
LoadBeginRecord:      // загрузить 1-й байт find_rec_buf
	 xor eax, eax
     mov al, byte ptr [ebx]
	 /*mov ah, al
	 and ah, 0xE0   // 11100000 старшие 3 байта - тип поля
     cmp ah, 0xA0      // тип поля Ax 1010x или Bx 1011x ?
	 jnz fieldnotAB
	 // если тип поля Ax или Bx     */

	 cmp al, 0xA3     // RecordContent ?
	 jz RecordContent

	 cmp ebx, find_rec_buf  // если тип поля НЕ A3
	 jz WrongBeginFind_rec_buf     // если мы на начале find_rec_buf

	 // если мы НЕ на начале find_rec_buf
	 mov ebx, find_rec_buf       // в начало
	 jmp LoadBeginRecord

WrongBeginFind_rec_buf:
	 push cRecordContent
	 push eax
	 jmp notField

RecordContent:
	 add ebx, dword ptr [iInterFieldSize] // число байт между первыми полями A3
	 mov cl, byte ptr [ebx-1] // размер поля A3 RecordContent
	 mov al, byte ptr [ebx]
	 cmp al, 0xA3         // cAdditionalRecordTypes_ME ?
	 jz AdditionalRecordTypes_ME

	 push cAdditionalRecordTypes_ME
     push eax
	 jmp notField

AdditionalRecordTypes_ME:
	 //add ebx, dword ptr [iInterFieldSize]
	 // т.к. iInterFieldSize не постоянное, проверим по длине 
	 inc ebx
	 mov al, byte ptr [ebx]
	 sub al, cl  // сравним с длиной поля RecordContent
	 cmp al, -2
	 jz NextFieldIsRecordType

	 dec cl   // уменьшим длину поля RecordContent на 1
	 jmp AdditionalRecordTypes_ME
	 
NextFieldIsRecordType:
	 inc ebx
	 mov al, byte ptr [ebx]
	 /*mov ah, al    // излишняя проверка
	 and ah, 0xA0
	 cmp ah, 0xA0   // поле Ax ?
	 jz ParseRecordBeg   */

ParseRecordBeg:
	 cmp al, 0xA0   //  dB_Subscriber_RecordType ?
     jz dB_Subscriber_RecordType
     cmp al, 0xA1
     jz dB_Trunk_RecordType   // dB_Trunk_RecordType ?
	 cmp al, 0xA2              // dB_SS_RecordType ?
     jz dB_SS_RecordType

     inc ebx
     mov al, byte ptr [ebx]
     jmp ParseRecordBeg

dB_SS_RecordType:
     push cdB_SS_RecordType
     jmp PrintRecordType

dB_Trunk_RecordType:
     push cdB_Trunk_RecordType
     jmp PrintRecordType

dB_Subscriber_RecordType:
     push cdB_Subscriber_RecordType

PrintRecordType:
     cmp bCsv, 1
     jz dB_RecordTypePrint_csv

	 push cAdditionalRecordTypes_ME
     push cPrintField2stringFrmt
	 //call printf    // печатаем тип dB_RecordType  (меняется edx)
	 //add esp, 12

	 push outfile
     call fprintf
     add esp, 16
     jmp ParseRecordContent

dB_RecordTypePrint_csv:
     push cCsv_Frmt
	 push cStr_csv
	 call sprintf  // запись в cStr_csv поля dB_RecordType по CSV формату
     //(char *buffer, const char *format[, argument, ...])
     add esp, 12
     /*
     pop eax
     push 0    // iOffSet =0 - не дополнительное, а главное значение
     push eax  // cValues
     push cRecordType  // cField   ( cRecordType  cSeizureTime )
     push cRecordHeader_s1     // cHeaderStr
     push cStr_csv        // cString
     call InsertToCsvString   // cString, cHeaderStr, cField, cValue, iOffSet
     //mov cStr_csv, eax     // в eax - ук-ль на готовую строку
     jmp ParseRecordContent
     */
     
ParseRecordContent:
     //inc ebx
	 add ebx, dword ptr [iInterFieldSize] // число байт между первыми полями Ax

ParseRecordWithLoadByte:
     xor eax, eax
     mov al, byte ptr [ebx]  // загрузить байт find_rec_buf
     //dec dl

ParseRecord:
     cmp al, 0x80   //  поле начинается на 80 ?
     jz RecordType
     cmp al, 0x8F
     jz CallingPartyCategory
     cmp al, 0xAE
     jz AdditionalParticipantInfo
     cmp al, 1  // поле =1
     jz DecrBufPos

      // field not RecordType
     push cRecordType
     push eax
	 jmp notField   //

DecrBufPos:
     dec ebx
     jmp ParseRecordWithLoadByte

// ----------------------- поле RecordType --------------------------
RecordType:
     //add esp, 4
     inc ebx
     mov cl, byte ptr [ebx]  // загрузить длину recordType
     cmp cl, 1
     jz RecordTypeEval

     push eax
     push cRecordType
     jmp FieldWrongLength

RecordTypeEval:
     inc ebx
     cmp bCsv, 1
     jz startTimeStampBeg
     
     mov al, byte ptr [ebx] // загрузить байт recordType из find_rec_buf
     test al, al  
     jz CallRecordType
     cmp al, 1
     jz SupplServiceInputRecordRecordType

     push eax
     push cRecordType
     push cOther2ValueFieldFrmt
     jmp PrintParseError

SupplServiceInputRecordRecordType:
     push cSupplServiceInputRecord
     jmp printRecordType

CallRecordType:
     push cCall
     jmp printRecordType

printRecordType:
     push cRecordType
     push cPrintField2stringFrmt
	 //call printf    // печатаем RecordType
	 //add esp, 12

	 push outfile
     call fprintf
     add esp, 16
          
//------------------ поле startTimeStamp ---------------------
startTimeStampBeg:
	 inc ebx
	 xor eax, eax
     mov al, byte ptr [ebx] // загрузить байт startTimeStamp из find_rec_buf
     //dec dl
     cmp al, 0xA1
     jz StartTimeStamp

     push cStartTimeStamp
     push eax
     jmp notField

StartTimeStamp:
     //cmp bCsv, 1
     //jz LoadStartTimeStampLength
     
     //push cStartTimeStamp
	 //call printf    // печатаем startTimeStamp
     //add esp, 4

//LoadStartTimeStampLength:
     inc ebx
     mov cl, byte ptr [ebx] // загрузить длину startTimeStamp из find_rec_buf
     //dec dl
     cmp cl, 9
     jz startTimeStampEval

     push eax
     push cStartTimeStamp
     jmp FieldWrongLength

startTimeStampEval:     

     inc ebx
     xor eax, eax
     mov al, byte ptr [ebx] // загрузить тип startTimeStamp из find_rec_buf 
     cmp al, 0x80
     jz AnswerTime
     cmp al, 0x81
     jz SeizureTime
     cmp al, 0x82
     jz PartialTime
     cmp al, 0x83
     jz EventTime

     push eax
     push cStartTimeStamp
     push cOther2ValueFieldFrmt
     jmp PrintParseError

AnswerTime:
     //cmp bCsv, 1
     //jz AnswerTime_csv  // bCsv = true ?

     push cAnswerTime
     jmp DateTimeParse   

/*AnswerTime_csv:
     mov iSpaceNumber, 4
     jmp DateTimeParse  */

SeizureTime:
     //cmp bCsv, 1
     //jz SeizureTime_csv  // bCsv = true ?

     push cSeizureTime
     jmp DateTimeParse

/*SeizureTime_csv:
     mov iSpaceNumber, 2
     push 2
     call PrintSpace_csv
     add esp, 4
     jmp DateTimeParse  */

PartialTime:
     //cmp bCsv, 1
     //jz PartialTime_csv  // bCsv = true ?

     push cPartialTime
     jmp DateTimeParse

/*PartialTime_csv:
     mov iSpaceNumber, 0
     push 4
     call PrintSpace_csv
     add esp, 4
     jmp DateTimeParse   */

EventTime:
     push cEventTime
     jmp DateTimeParse

DateTimeParse:
     //cmp bCsv, 1
     //jz DateTimeLengthLoad   // bCsv = true ?
     
     //call printf    // печатаем type of startTimeStamp
     //add esp, 4

//DateTimeLengthLoad:
     inc ebx
     mov cl, byte ptr [ebx] // загрузить длину datetime из find_rec_buf
     //pop ecx    // восстановить есх  длина в startTimeStamp
     cmp cl, 7    // длина datetime =7 ?  
     jz DateTimeEval

     push eax
     push cStartTimeStamp
     jmp FieldWrongLength

DateTimeEval:
     inc ebx
     xor eax, eax
     mov al, byte ptr [ebx] // загрузить datetime из find_rec_buf
     mov ah, al
     //dec cl   // счетчик байт в datetime startTimeStamp
     and al, 0x0F
     or al, 0x30
     //lea edx, sDigits
     mov edx, dword ptr [sDigits]
     //mov edx, dword ptr [edx]
     and ah, 0xF0
     shr ah, 4
     or ah, 0x30
     //mov dx, ax
     //or eax, 0x2E0000
     //shl eax, 8    // 2E313100
     //mov dword ptr [edx+5], eax
     mov byte ptr [edx+7], al
     mov byte ptr [edx+6], ah
     mov ch, 0x2E
     mov byte ptr [edx+5], ch
     xor eax, eax
     mov byte ptr [edx+8], al

     inc ebx
     mov al, byte ptr [ebx] // загрузить datetime startTimeStamp из find_rec_buf
     mov ah, al
     //dec cl
     and al, 0x0F
     or al, 0x30

     and ah, 0xF0
     shr ah, 4
     or ah, 0x30
     mov byte ptr [edx+3], ah
     mov byte ptr [edx+4], al
     //mov al, 0x2E
     mov byte ptr [edx+2], ch
     
     inc ebx
     mov al, byte ptr [ebx] // загрузить datetime startTimeStamp из find_rec_buf
     mov ah, al
     //dec cl
     and al, 0x0F
     or al, 0x30

     and ah, 0xF0
     shr ah, 4
     or ah, 0x30
     mov byte ptr [edx], ah
     mov byte ptr [edx+1], al 
     push edx  // сохраним в стек дату

     lea edx, sTime
     //mov edx, dword ptr [sTime]
     inc ebx
     mov al, byte ptr [ebx] // загрузить часы startTimeStamp из find_rec_buf
     mov ah, al
     //dec cl
     and al, 0x0F
     or al, 0x30

     and ah, 0xF0
     shr ah, 4
     or ah, 0x30

     mov byte ptr [edx], ah
     mov byte ptr [edx+1], al
     mov al, 0x3A    // ASCII ':'
     mov byte ptr [edx+2], al

     inc ebx
     mov al, byte ptr [ebx] // загрузить минуты startTimeStamp из find_rec_buf
     mov ah, al
     //dec cl
     and al, 0x0F
     or al, 0x30

     and ah, 0xF0
     shr ah, 4
     or ah, 0x30

     mov byte ptr [edx+3], ah
     mov byte ptr [edx+4], al

     cmp bCsv, 1
     jz Colon

     mov al, 0x2E    // ASCII '.'
     jmp WriteDelim
     
Colon:
     mov al, 0x3A    // ASCII ':'

WriteDelim:
     mov byte ptr [edx+5], al

     inc ebx
     mov al, byte ptr [ebx] // загрузить секунды startTimeStamp из find_rec_buf
     mov ah, al
     //dec cl
     and al, 0x0F
     or al, 0x30

     and ah, 0xF0
     shr ah, 4
     or ah, 0x30

     mov byte ptr [edx+6], ah
     mov byte ptr [edx+7], al
     xor ecx, ecx

     cmp bCsv, 1
     jz TimeCSV

     mov al, 0x2C    // ASCII ','
     mov byte ptr [edx+8], al
     inc cl     //  позиция конца строки sTime для текстового вывода

     inc ebx
     mov al, byte ptr [ebx] // загрузить сотые доли секунд из find_rec_buf
     mov ah, al
     //dec cl
     and al, 0x0F
     or al, 0x30

     and ah, 0xF0
     shr ah, 4
     or ah, 0x30
     
     mov byte ptr [edx+9], ah
     mov byte ptr [edx+10], al
     jmp PutZeroEnd

TimeCSV:
     dec ecx
     dec ecx     //  позиция конца строки sTime для CSV вывода
     inc ebx   // пропуск байта сотых долей секунд

PutZeroEnd:
     xor eax, eax
     mov byte ptr [edx+10+ecx], al  // в конце строки sTime - ноль

     pop eax    // восстановим из стека дату

	 cmp bCDRHeader, 1   // если расшифровка ProductionDateTime CDR заголовка
     jz CDRProductionDateTimePrint  // идем назад

     pop esi    // из стека  type of StartTimeStamp

     cmp bCsv, 1
     jz PrintDateTime_csv    // bCsv = true ?


     push edx   // время
     push eax   // дата
     push esi    // type of StartTimeStamp
     push cStartTimeStamp
     push sStartTimeFormat
	 //call printf         // печатаем дату и время startTimeStamp
	 //add esp, 20

	 push outfile
     call fprintf
     add esp, 24
     jmp ParticipantInfoBeg

PrintDateTime_csv:
     push edx   // время
     push eax   // дата
     push cCsv2ValFrmt
     push cStr_csv_tmp  // временная строка CSV
	 call sprintf  // пишем в cStr_csv_tmp дату и время в csv формате
     add esp, 16
     //call printf   // печатаем дату и время в csv формате
     //push iSpaceNumber
     //call PrintSpace_csv  // печатаем пустые поля
     //add esp, 4
     
     push 0  // iOffSet
     push cStr_csv_tmp  // cValue
     push esi  // type of StartTimeStamp - cField
     push cRecordHeader_s1  // cHeaderStr
     push cStr_csv  //  cString
//(char &cString, char *cHeaderStr, char *cField, char *cValue, short iOffSet)
     call InsertToCsvString  // вставим в cStr_csv значение cValue в место поля

                            // cField согласно заголовка cRecordHeader_s1
     add esp, 20
     
     /*
     push 0    // iOffSet
     push eax  // cValues - дата
     push esi  // cField - type of StartTimeStamp: AnswerTime, cSeizureTime,..
     push cRecordHeader_s1     // cHeaderStr
     push cStr_csv        // cString
     call InsertToCsvString   // cString, cHeaderStr, cField, cValue, iOffSet

     push 1    // iOffSet =1 - дополнительное значение
     push edx  // cValues - время
     push esi  // cField - type of StartTimeStamp: AnswerTime, cSeizureTime,..
     push cRecordHeader_s1     // cHeaderStr
     push cStr_csv        // cString
     call InsertToCsvString   // cString, cHeaderStr, cField, cValue, iOffSet
     */

//-------------------- поле cParticipantInfo ----------------------

ParticipantInfoBeg:
	 inc ebx
	 xor eax, eax
     mov al, byte ptr [ebx]
     cmp al, 0xA2  //  поле cParticipantInfo ?
     jz ParticipantInfo

     push cParticipantInfo
     push eax
	 jmp notField

ParticipantInfo:
     cmp bCsv, 1
     jz ParticipantInfoLength

     push cParticipantInfo
	 //call printf         // печатаем заголовок поля ParticipantInfo
	 //add esp, 4

	 push outfile
     call fprintf
     add esp, 8
	 
ParticipantInfoLength:
     inc ebx
     //mov cl, byte ptr [ebx]  // длина  cParticipantInfo
     xor esi, esi      // сброс признака ReceivedDigits AdditionalParticipantInfo

ParticipantIdEval:
     inc ebx
     xor eax, eax
     mov al, byte ptr [ebx]
     cmp al, 0x80
     jz CallingPartyNumber
     cmp al, 0x81  
     jz CalledPartyNumber
     cmp al, 0x82
     jz RedirectingNumber
     cmp al, 0x83
     jz RedirectionNumber
     cmp al, 0x84
     jz OriginalCalledNumber
     cmp al, 0x85
     jz CallingPartyNumberNotScreened
     cmp al, 0x86
     jz OperatorSpecific1Number
     cmp al, 0x87
     jz OperatorSpecific2Number
     cmp al, 0x88
     jz OperatorSpecific3Number

     cmp al, 0xA3     // если уже не ParticipantInfo
     jz BearerServiceBeg

     push eax
     push cParticipantId
     push cOther2ValueFieldFrmt
     jmp PrintParseError

CallingPartyNumber:
     push cCallingNumber        // значение ParticipantId       4
     //mov iSpaceNumber, 3
     jmp NumberEval

CalledPartyNumber:
     push cCalledNumber
     //mov iSpaceNumber, 2
     jmp NumberEval

RedirectingNumber:
     push cRedirectingNumber
     //mov iSpaceNumber, 1
     jmp NumberEval

RedirectionNumber:
     push cRedirectionNumber
     jmp NumberEval

OriginalCalledNumber:
     push cOriginalCalledNumber
     //mov iSpaceNumber, 0
     jmp NumberEval

CallingPartyNumberNotScreened:
     push cCallingPartyNumberNotScreened
     jmp NumberEval

OperatorSpecific1Number:
     push cOperatorSpecific1Number
     jmp NumberEval

OperatorSpecific2Number:
     push cOperatorSpecific2Number
     jmp NumberEval

OperatorSpecific3Number:
     push cOperatorSpecific3Number
     jmp NumberEval 

NumberEval:
     inc ebx
     xor ecx, ecx
     mov cl, byte ptr [ebx]  // длина  ParticipantID (Call -ing-ed PartyNumber)

     inc ebx
     dec cl     // уменьшим длину ParticipantID
     xor eax, eax
     mov al, byte ptr [ebx]  // загрузим Odd/Even Ind.
     and al, 0x80
     test al, al
     jz  Even
     cmp al, 0x80
     jz Odd

     push eax
     push cOddEvenNumbInd
     push cOther2ValueFieldFrmt
     jmp PrintParseError

Odd:
     push cOdd
     mov edx, -1   // установка признака Odd
     jmp NatOfAddrEval

Even:
     push cEven                // значение Odd/Even Ind.        3

NatOfAddrEval:
     mov ah, byte ptr [ebx]
     and ah, 0x7F
     test ah, ah
     jz AddrSpare
     cmp ah, 1
     jz AddrSubscriber
     cmp ah, 2
     jz AddrUnknown
     cmp ah, 3
     jz AddrNational
     cmp ah, 4
     jz AddrInternational
     cmp ah, 5

     push cNatOfAddr
     push eax
     jnge notField

     // SpareOrReserved
     cmp ah, 0x70
     jnge AddrSpare
     
     //  Reserved
     push cReservedNat
     jmp NumbPlanIndEval        //   значение Nature Of Address     2

AddrSpare:
     push cSpare
     jmp NumbPlanIndEval

AddrSubscriber:
     push cSubscriber
     jmp NumbPlanIndEval

AddrUnknown:
     push cUnknown
     jmp NumbPlanIndEval

AddrNational:
     push cNational
     jmp NumbPlanIndEval

AddrInternational:
     push cInternationalNumb
     jmp NumbPlanIndEval
     
NumbPlanIndEval: 
     cmp esi, -1
     jz DigitsParticipantInfoEval
     
     inc ebx
     dec cl  // уменьшим длину ParticipantID
     mov al, byte ptr [ebx]  // загрузим Type of numbering plan
     and al, 0x70
     shr al, 4

     cmp al, 1
     jz numbE164
     cmp al, 3
     jz numbX121
     cmp al, 4
     jz numbF69
     cmp al, 5
     jz numbReserv
     cmp al, 6
     jz numbReserv

     push cSpare              // значение Type of numbering plan   1
     jmp DigitsParticipantInfoEval

numbE164:     // E164
     push cE164
     jmp DigitsParticipantInfoEval

numbX121: // X121
     push cX121
     jmp DigitsParticipantInfoEval

numbF69:  // F69
     push cF69
     jmp DigitsParticipantInfoEval

numbReserv:
     push cReserved
     jmp DigitsParticipantInfoEval
     
DigitsParticipantInfoEval:
     mov edi, dword ptr [sDigits]

LoadDigits:
     inc ebx
     mov al, byte ptr [ebx]  // загрузим 2 цифры в TBCD коде
     mov ah, al

     and al, 0xF0
     shr al, 4
	 cmp al, 0x0A    // 0x0A  - *
	 jz Make_A
	 cmp al, 0x0B    // 0x0B  - #
	 jz Make_B
     
     or al, 0x30  // ASCII код 1-й (старшей) цифры
     jmp STOSB1

Make_A:
	 mov al, 0x2A   //  ASCII код  '*'  0x0A
	 jmp STOSB1
	 
Make_B:
	 mov al, 0x23   //  ASCII код  '#'  0x0B
	 jmp STOSB1

STOSB1:
     stosb

     cmp edx, -1    // признак Odd ?
     jnz NextDigit

     mov al, cl  // копируем текущую длину в al для проверки нуля в конце
     dec al     // последний байт цифр ?
     jz SDigitZeroEnding

NextDigit:
     mov al, ah
     and al, 0x0F
	 cmp al, 0x0A  // 0x0A  - *
	 jz Make_A_2
	 cmp al, 0x0B  // 0x0B  - #
	 jz Make_B_2
     or al, 0x30    // ASCII код 2-й (младшей) цифры
     jmp STOSB2

Make_A_2:
	 mov al, 0x2A   //  ASCII код '*'  0x0A
	 jmp STOSB2

Make_B_2:
	 mov al, 0x23   //  ASCII код '#'  0x0B
	 jmp STOSB2
      
STOSB2:
     stosb
     //stosw    // сохраняем слово из ax в es:[edi]
     loop LoadDigits

SDigitZeroEnding:
     mov byte ptr [edi], 0  // закончим строку sDigits нулем

     pop eax   //  восстановим значение Type of numbering plan
     pop ecx   //  восстановим значение Nature of Address

     cmp esi, -1
     jz AdditionalParticipantInfoPrint

     pop edx   //  восстановим значение Odd/Even Ind.
     pop edi   //  восстановим значение ParticipantId

     cmp bCsv, 1
     jz PrintParticipantId_csv

     push sDigits            // sDigits
     push cAdressdigits       // adressdigits
     push eax                // значение Type of numbering plan
     push cNumbPlanInd        // NumbPlanInd
     push ecx                // значение Nature of Address
     push cNatOfAddr          // NatOfAddr
     push edx                // значение Odd/Even Ind.
     push cOddEvenNumbInd     // OddEvenNumbInd
     push edi                // значение ParticipantId
     push cParticipantInfoFrmt
	 //call printf          // печатаем ParticipantInfo
	 //add esp, 40

	 push outfile
     call fprintf
     add esp, 44
     jmp ParticipantIdEval
     
PrintParticipantId_csv:
     //push sDigits
     //push cCsv_Frmt
     //call printf
     push sDigits     // номер ParticipantId
     push cCsv_Frmt
     push cStr_csv_tmp
     call sprintf  // пишем в стоку cStr_csv_tmp sDigits в формате CSV
     add esp, 12

     push 0         // iOffSet
     push cStr_csv_tmp   // cValue
     push edi       // ParticipantId - cField
     push cRecordHeader_s1  //  cHeaderStr
     push cStr_csv         //  cString
     call InsertToCsvString
     add esp, 20
      
     jmp ParticipantIdEval   // на проверку ParticipantId ?

//-------------------- поле bearerService -------------------------
BearerServiceBeg:
     /*cmp bCsv, 1
     jnz LoadBearerLength

     push iSpaceNumber    // кол-во пустых полей в зав-ти от ParticipantId
     call PrintSpace_csv  // печатаем пустые поля после ParticipantId
     add esp, 4

LoadBearerLength: */
     inc ebx
     mov cl, byte ptr [ebx] // длина bearerService
     cmp cl, 3
     jz bearerServiceEval

     push eax
     push cBearerService
     jmp FieldWrongLength
      
bearerServiceEval:
     inc ebx
     mov al, byte ptr [ebx]
     cmp al, 0x0A  //  capability ?
     jnz notBSCapability

     //  capability
   
     inc ebx
     mov cl, byte ptr [ebx]
     cmp cl, 1    // длина capability = 1 ?
     jz BearerServiceCapabilityEval

     push eax
     push cCapability
     jmp FieldWrongLength

BearerServiceCapabilityEval:
     //  длина capability = 1
     inc ebx
     xor eax, eax
     mov al, byte ptr [ebx]

     test al, al  // speech ?
     jz speech
     cmp al, 1  //  audio3dot1kHZ ?
     jz audio31
     cmp al, 2    // uni64 ?
     jz uni64
     cmp al, 3     // uni64withT-A ?
     jz uni64withTA
     cmp al, 4    // multipleRate ?
     jz multipleRate
     cmp al, 5   // packetModeB-Ch ?
     jz PacketModeB_Ch

     push cCapability
     push cBearerService
     push eax
     push cWrongBSCapabilityFrmt
     jmp PrintParseError 

speech:
     push cSpeech
     jmp printBScapability

audio31:
     push cAudio31
     jmp printBScapability

uni64:
     push cUni64
     jmp printBScapability

uni64withTA:
     push cUni64withTA
     jmp printBScapability

multipleRate:
     push cMultipleRate
     jmp printBScapability

PacketModeB_Ch:
     push cPacketModeBCh
     jmp printBScapability

printBScapability:
     cmp bCsv, 1
     jz printBScapability_csv
     
     push cCapability
     push cBearerService
     push cPrintField3stringFrmt
	 //call printf
	 //add esp, 16

	 push outfile
     call fprintf
     add esp, 20
     jmp ServiceUserField

printBScapability_csv:
     /*push cCsv_Frmt
     call printf
     jmp ServiceUserField    */

	 push cCsv_Frmt
	 push cStr_csv_tmp
	 call sprintf    // копируем в cStr_csv_tmp BScapability в формате CSV
	 add esp, 12

	 push 0
	 push cStr_csv_tmp
	 push cBearerCapability
	 push cRecordHeader_s1
	 push cStr_csv
	 call InsertToCsvString  // вставим в cStr_csv значение cValue в место поля
//(char &cString, char *cHeaderStr, char *cField, char *cValue, short iOffSet)
	 add esp, 20
     jmp ServiceUserField

fieldnotBearer:
     push cBearerService
     push eax
     jmp notField


notBSCapability:
     push cCapability
     push eax
     jmp notField

//----------------- поле ServiceUser --------------------------
ServiceUserField:
	 inc ebx
	 xor eax, eax
     mov al, byte ptr [ebx]
     cmp al, 0x84   // serviceUser ?
     jz ServiceUser

     push cServiceUser
     push eax
	 jmp notField

ServiceUser:
     inc ebx
     mov cl, byte ptr [ebx]
     cmp cl, 1                // длина serviceUser = 1 ?
     jz ServiceUserEval

     push eax
     push cServiceUser
     jmp FieldWrongLength

ServiceUserEval:
     //  длина serviceUser OK = 1
     xor eax, eax
     inc ebx

     cmp bCsv, 1
     jz CallIdentificationNumberField
     
     mov al, byte ptr [ebx]
     test al, al   //   serviceUser = callingPartyNumber ?
     jz CallingPartyNumberSU
     cmp al, 1     // calledPartyNumber ?
     jz CalledPartyNumberSU
     cmp al, 2     // redirectingNumber ?
     jz RedirectingNumberSU
     cmp al, 3     // redirectionNumber ?
     jz RedirectionNumberSU
     cmp al, 4    //  originalCalledNumber ?
     jz OriginalCalledNumberSU
     cmp al, 5    //  callingPartyNumberNotScreened ?
     jz CallingPartyNumberNotScreenedSU
     cmp al, 6    // operatorSpecific1Number ?
     jz OperatorSpecific1NumberSU
     cmp al, 7    // operatorSpecific2Number ?
     jz OperatorSpecific2NumberSU
     cmp al, 8    // operatorSpecific3Number ?
     jz OperatorSpecific3NumberSU
     cmp al, 9     // operator ?
     jz OperatorSU
     cmp al, 10    // unknown  ?
	 jz Unknown1

     push cServiceUser
     push eax
     push cOther2ValueFieldFrmt
     jmp PrintParseError

Unknown1:
     push cUnknown   //  unknown
     jmp printServiceUser
           
OperatorSU:
     push cOperator
     jmp printServiceUser

OperatorSpecific3NumberSU:
     push cOperatorSpecific3Number
     jmp printServiceUser

OperatorSpecific2NumberSU:
     push cOperatorSpecific2Number
     jmp printServiceUser

OperatorSpecific1NumberSU:  // operatorSpecific1Number
     push cOperatorSpecific1Number
     jmp printServiceUser

CallingPartyNumberNotScreenedSU:  // ServiceUser = callingPartyNumberNotScreened
     push cCallingPartyNumberNotScreened
     jmp printServiceUser
     
OriginalCalledNumberSU:   // ServiceUser = originalCalledNumber
     push cOriginalCalledNumber
     jmp printServiceUser

RedirectionNumberSU:      // ServiceUser = redirecTIONnumber
     push cRedirectingNumber
     jmp printServiceUser

RedirectingNumberSU:      // ServiceUser = redirecTINGnumber
     push cRedirectingNumber
     jmp printServiceUser
     
CallingPartyNumberSU:     // serviceUser = callingPartyNumber
     push cCallingPartyNumber
     jmp printServiceUser

CalledPartyNumberSU:     // serviceUser = calledPartyNumber
     push cCalledPartyNumber 

printServiceUser:    // печатаем поле ServiceUser
     push cServiceUser
     push cPrintField2stringFrmt
	 //call printf
	 //add esp, 12

	 push outfile
     call fprintf
     add esp, 16
     
//------------------ поле callIdentificationNumber --------------------
CallIdentificationNumberField:
	 inc ebx
	 xor eax, eax
     mov al, byte ptr[ebx]   // загрузим  callIdentificationNumber
     cmp al, 0x86   // callIdentificationNumber ?
     jz CallIdentificationNumber

     push cCallIdentificationNumber
     push eax
	 jmp notField

CallIdentificationNumber:
     inc ebx
     mov cl, byte ptr[ebx]   // загрузим длину callIdentificationNumber
     cmp cl, 4    // длина  callIdentificationNumber = 4 ?
     jz CallIdentificationNumberLengthOK

     push eax
     push cCallIdentificationNumber
     jmp FieldWrongLength

CallIdentificationNumberLengthOK:  // длина  callIdentificationNumber = 4
     inc ebx
     
	 //cmp bCsv, 1
	 //jz CauseField


     mov eax, dword ptr [ebx]   // загрузим callIdentificationNumber
     bswap eax
	 push eax                   // Hex value

	 cmp bCsv, 1
	 jz PrintCSV_CallIdentificationNumber
	 
     push cCallIdentificationNumber
     push cValueIsHexFrmt
	 //call printf      // печатаем callIdentificationNumber
	 //add esp, 12

	 push outfile
     call fprintf
     add esp, 16
	 jmp CauseField

PrintCSV_CallIdentificationNumber:
	 push cCsv_Frmt_hex
	 push cStr_csv_tmp
	 call sprintf  // копируем в cStr_csv_tmp callIdentificationNumber в формате CSV
	 add esp, 12

	 push 0
	 push cStr_csv_tmp
	 push cCallIdentificationNumber
	 push cRecordHeader_s1
	 push cStr_csv
	 call InsertToCsvString  // вставим в cStr_csv значение cValue в место поля
//(char &cString, char *cHeaderStr, char *cField, char *cValue, short iOffSet)
	 add esp, 20
	 
//------------------- поле cause ------------------
CauseField:
	 add ebx, 4
	 xor eax, eax
Cause:
     mov al, byte ptr [ebx]
     cmp al, 0xA8    // cause ?
     jz CauseEval  
     cmp al, 0xA5    
     jz SupplementaryServices
     cmp al, 0xAA
     jz PartialGenerationField
     cmp al, 0xAB
     jz ExchangeInfo

     push cSupplementaryServices
     push eax
	 jmp notField

CauseEval:
     inc ebx
     mov cl, byte ptr [ebx]
     cmp cl, 7          // длина cause = 7 ?
     jz CauseLengthOK

     push eax
     push cCause
     jmp FieldWrongLength

CauseLengthOK:     //  длина cause = 7
     inc ebx
     xor eax, eax
     mov al, byte ptr [ebx]
     cmp al, 3  // causeValue ?
     jz CauseValue
     
     push cCauseValue
     push eax
     jmp notField

CauseValue:
     inc ebx
     inc ebx
     mov ah, byte ptr [ebx]
     inc ebx
     mov al, byte ptr [ebx]
     test ax, ax
     jz Dummy
     cmp ax, 1    // causeValue = Unallocated (unassigned) number ?
     jz UnallocatedNumber
     cmp ax, 2   // No route to specified transit network (national use)
     jz  NoRouteToSpecified
     cmp ax, 3  // No route to destination
     jz NoRouteToDestination
     cmp ax, 4  // send special information tone
     jz SendSpecialInformationTone
     cmp ax, 5  // misdialed trunk prefix (national use)
     jz MisdialedTrunkPrefix
     cmp ax, 6  // channel unacceptable
     jz ChannelUnacceptable
     cmp ax, 7 // call awarded. being delivered in an established channel
     jz CallAwardedBeingDelivered
     cmp ax, 8 //  preemption
     jz Preemption
     cmp ax, 9   // preemption - circuit reserved for reuse
     jz PreemptionCircuitReserved
     cmp ax, 13
     jz Reset_circuit_proc
     cmp ax, 15
     jz Tcc06_tim_exp
     cmp ax, 16   // normal call clearing
     jz NormalCallClearing
     cmp ax, 17    // user busy
     jz UserBusy
     cmp ax, 18  // no user responding
     jz NoUserResponding
     cmp ax, 19  //  no answer from user (user alerted)
     jz NoAnswerFromUser
     cmp ax, 20  // subscriber absent
     jz SubscriberAbsent
	 cmp ax, 21  // call rejected
	 jz CallRejected
	 cmp ax, 22  // call rejected
	 jz NumberChanged
     cmp ax, 25
     jz Exch_routing_error
     cmp ax, 26   // non-selected user clearing
     jz NonSelectedUserClearing
     cmp ax, 27  // destination out of order
     jz DestinationOutOfOrder
     cmp ax, 28  // invalid number format (address incomplete)
     jz InvalidNumberFormatAddrIncomplete
     cmp ax, 29  // facilities rejected
     jz FacilitiesRejected
     cmp ax, 30  // response to STATUS INQUIRY
     jz ResponseToSTATUSINQUIRY
     cmp ax, 31  // normal. unspecified
     jz NormalUnspecified
     cmp ax, 34  // no circuit/channel available
     jz NoCircuitChannelAvailable
     cmp ax, 35  // Call Queued
     jz CallQueued
     cmp ax, 38  // network out of order
     jz NetworkOutOfOrder
     cmp ax, 39  // permanent frame mode connection out-of-service
     jz PermanentFrameModeConnectionOutOfService
     cmp ax, 40  // permanent frame mode connection operational
     jz  PermanentFrameModeConnectionOperational
     cmp ax, 41  // temporary failure
     jz TemporaryFailure
     cmp ax, 42  // switching equipment congestion
     jz SwitchingEquipmentCongestion
     cmp ax, 43  // access information discarded
     jz  AccessInformationDiscarded
     cmp ax, 44  // requested circuit/channel not available
     jz  RequestedCircuitChannelNotAvailable
     cmp ax, 46  // precedence call blocked
     jz PrecedenceCallBlocked
     cmp ax, 47   // resource unavailable, unspecified
     jz  ResourceUnavailableUnspecified
     cmp ax, 49  // Quality of Service not available
     jz  QualityOfServiceNotAvailable
     cmp ax, 50  // requested facility not subscribed
     jz  RequestedFacilityNotSubscribed
     cmp ax, 52  // outgoing calls barred
     jz  OutgoingCallsBarred
     cmp ax, 53  // outgoing calls barred within CUG
     jz  OutgoingCallsBarredWithinCUG
     cmp ax, 54  // incoming calls barred
     jz  IncomingCallsBarred
     cmp ax, 55   // incoming calls barred within CUG
     jz  IncomingCallsBarredWithinCUG
     cmp ax, 57  // bearer capability not authorized
     jz  BearerCapabilityNotAuthorized
     cmp ax, 58  // bearer capability not presently available
     jz  BearerCapabilityNotPresentlyAvailable
     cmp ax, 62  // inconsistency in outgoing information element
     jz  InconsistencyInOutgoingInformationElement
     cmp ax, 63  // service or option not available. unspecified
     jz  ServiceOrOptionNotAvailableUnspecified
     cmp ax, 65  // bearer capability not implemented
     jz  BearerCapabilityNotImplemented
     cmp ax, 66  // channel type not implemented
     jz  ChannelTypeNotImplemented
     cmp ax, 69  // requested facility not implemented
     jz  RequestedFacilityNotImplemented
     cmp ax, 70  // only restricted digital information bearer capability is available (national use)
     jz  OnlyRestrictedDigitalInformationBearerCapabilityIsAvailable
     cmp ax, 79  // service or option not implemented unspecified
     jz  ServiceOrOptionNotImplementedUnspecified
     cmp ax, 81  // invalid call reference value
     jz  InvalidCallReferenceValue
     cmp ax, 82  // identified channel does not exist
     jz  IdentifiedChannelDoesNotExist
     cmp ax, 83  // a suspended call exists, but this call identify does not
     jz  SuspendedCallExistsButThisCallIdentifyDoesNot
     cmp ax, 84   // call identity in use
     jz  CallIdentityInUse
     cmp ax, 85   // no call suspended
     jz  NoCallSuspended
     cmp ax, 86   // call having the requested call identity has been cleared
     jz  CallHavingTheRequestedCallIdentityHasBeenCleared
     cmp ax, 87   // user not a member of CUG
     jz  UserNotMemberOfCUG
     cmp ax, 88   // incompatible destination
     jz  IncompatibleDestination
     cmp ax, 90   // non-existent CUG
     jz  NonExistentCUG
     cmp ax, 91   // invalid transit network selection (national use)
     jz  InvalidTransitNetworkSelection
     cmp ax, 95  // invalid message, unspecified
     jz  InvalidMessageUnspecified
     cmp ax, 96  // mandatory information element is missing
     jz  MandatoryInformationElementIsMissing
     cmp ax, 97  // message type non-existent or not implemented
     jz  MessageTypeNonExistentOrNotImplemented
     cmp ax, 98  // message not compatible with call state or message type non-existent or not implemented
     jz  MessageNotCompatibleWithCallStateOrMessageTypeNonExistentOrNotImplemented
     cmp ax, 99  // Information element / parameter non-existent or not implemented
     jz  InformationElementNotImplemented 
     cmp ax, 100  // Invalid information element contents
     jz  InvalidInformationElementContents
     cmp ax, 101  // message not compatible with call state
     jz  MessageNotCompatibleWithCallState
     cmp ax, 102  // recovery on timer expiry
     jz  RecoveryOnTimerExpiry
     cmp ax, 103  // parameter non-existent or not implemented - passed on (national use)
     jz  ParameterNonExistentOrNotImplementedPassedOn
     cmp ax, 110  // message with unrecognized parameter discarded
     jz MessageWithUnrecognizedParameterDiscarded
     cmp ax, 111  // protocol error, unspecified
     jz  ProtocolErrorUnspecified
     cmp ax, 119
     jz  Cw_dlx_rejected
     cmp ax, 120
     jz  Call_fac_spec_tone
     cmp ax, 121
     jz  Call_fac_rejected
     cmp ax, 122
     jz  Outg_call_screening
     cmp ax, 123
     jz  Intcpt_prio_rls
     cmp ax, 124
     jz  Itcp_allocation_fail
     cmp ax, 127  // Inter-working, unspecified
     jz  InterWorkingUnspecified
     cmp ax, 128
     jge CauseValuesOf128AndHigher // Cause code values of 128 and higher aren't sent over the network and aren't defined in Rec. [Q.850]

     push eax
     push cCauseValue
     push cOther2ValueFieldFrmt
     jmp PrintParseError

Itcp_allocation_fail:
     push cItcp_allocation_fail
     jmp  nextFieldOfCause
Intcpt_prio_rls:
     push cIntcpt_prio_rls
     jmp  nextFieldOfCause
Outg_call_screening:
     push cOutg_call_screening
     jmp  nextFieldOfCause
Call_fac_rejected:
     push cCall_fac_rejected
     jmp  nextFieldOfCause
Call_fac_spec_tone:
     push cCall_fac_spec_tone
     jmp  nextFieldOfCause
Cw_dlx_rejected:
     push cCw_dlx_rejected
     jmp  nextFieldOfCause
Exch_routing_error:
     push cExch_routing_error
     jmp  nextFieldOfCause
Reset_circuit_proc:
     push cReset_circuit_proc
     jmp  nextFieldOfCause
Tcc06_tim_exp:
     push cTcc06_tim_exp
     jmp  nextFieldOfCause
Dummy:
     push cDummy
     jmp  nextFieldOfCause

CauseValuesOf128AndHigher:
     push cCauseValuesOf128AndHigher
     jmp  nextFieldOfCause
             
InterWorkingUnspecified:
     push cInterWorkingUnspecified
     jmp nextFieldOfCause
             
ProtocolErrorUnspecified:
     push cProtocolErrorUnspecified
     jmp nextFieldOfCause

MessageWithUnrecognizedParameterDiscarded:
     push cMessageWithUnrecognizedParameterDiscarded
     jmp  nextFieldOfCause
        
ParameterNonExistentOrNotImplementedPassedOn:
     push cParameterNonExistentOrNotImplementedPassedOn
     jmp nextFieldOfCause
             
RecoveryOnTimerExpiry:
     push cRecoveryOnTimerExpiry
     jmp nextFieldOfCause
             
MessageNotCompatibleWithCallState:
     push cMessageNotCompatibleWithCallState
     jmp nextFieldOfCause
        
InvalidInformationElementContents:
     push cInvalidInformationElementContents
     jmp nextFieldOfCause
        
InformationElementNotImplemented:
     push cInformationElementNotImplemented
     jmp nextFieldOfCause
             
MessageNotCompatibleWithCallStateOrMessageTypeNonExistentOrNotImplemented:
     push cMessageNotCompatibleWithCallStateOrMessageTypeNonExistentOrNotImplemented
     jmp nextFieldOfCause
             
MessageTypeNonExistentOrNotImplemented:
     push cMessageTypeNonExistentOrNotImplemented
     jmp nextFieldOfCause
             
MandatoryInformationElementIsMissing:
     push cMandatoryInformationElementIsMissing
     jmp  nextFieldOfCause
             
InvalidMessageUnspecified:
     push cInvalidMessageUnspecified
     jmp nextFieldOfCause
        
InvalidTransitNetworkSelection:
     push cInvalidTransitNetworkSelection
     jmp nextFieldOfCause
             
NonExistentCUG:
     push cNonExistentCUG
     jmp nextFieldOfCause
        
IncompatibleDestination:
     push cIncompatibleDestination
     jmp nextFieldOfCause
     
UserNotMemberOfCUG:
     push cUserNotMemberOfCUG
     jmp nextFieldOfCause
        
CallHavingTheRequestedCallIdentityHasBeenCleared:
     push cCallHavingTheRequestedCallIdentityHasBeenCleared
     jmp nextFieldOfCause
             
NoCallSuspended:
     push cNoCallSuspended
     jmp  nextFieldOfCause
             
CallIdentityInUse:
     push cCallIdentityInUse
     jmp  nextFieldOfCause
SuspendedCallExistsButThisCallIdentifyDoesNot:
     push cSuspendedCallExistsButThisCallIdentifyDoesNot
     jmp nextFieldOfCause
             
IdentifiedChannelDoesNotExist:
     push cIdentifiedChannelDoesNotExist
     jmp nextFieldOfCause
     
InvalidCallReferenceValue:
     push cInvalidCallReferenceValue
     jmp nextFieldOfCause
        
ServiceOrOptionNotImplementedUnspecified:
     push cServiceOrOptionNotImplementedUnspecified
     jmp nextFieldOfCause
             
OnlyRestrictedDigitalInformationBearerCapabilityIsAvailable:
     push cOnlyRestrictedDigitalInformationBearerCapabilityIsAvailable
     jmp nextFieldOfCause
        
RequestedFacilityNotImplemented:
     push cRequestedFacilityNotImplemented
     jmp nextFieldOfCause
             
ChannelTypeNotImplemented:
     push cChannelTypeNotImplemented
     jmp nextFieldOfCause

BearerCapabilityNotImplemented:
     push cBearerCapabilityNotImplemented
     jmp nextFieldOfCause
        
ServiceOrOptionNotAvailableUnspecified:
     push cServiceOrOptionNotAvailableUnspecified
     jmp nextFieldOfCause

InconsistencyInOutgoingInformationElement:
     push cInconsistencyInOutgoingInformationElement
     jmp nextFieldOfCause
             
BearerCapabilityNotPresentlyAvailable:
     push cBearerCapabilityNotPresentlyAvailable
     jmp nextFieldOfCause

BearerCapabilityNotAuthorized:
     push cBearerCapabilityNotAuthorized
     jmp  nextFieldOfCause
     
IncomingCallsBarredWithinCUG:
     push cIncomingCallsBarredWithinCUG
     jmp nextFieldOfCause

IncomingCallsBarred:
     push cIncomingCallsBarred
     jmp nextFieldOfCause

OutgoingCallsBarredWithinCUG:
     push cOutgoingCallsBarredWithinCUG
     jmp nextFieldOfCause
     
OutgoingCallsBarred:
     push cOutgoingCallsBarred
     jmp nextFieldOfCause
     
RequestedFacilityNotSubscribed:
     push cRequestedFacilityNotSubscribed
     jmp nextFieldOfCause

QualityOfServiceNotAvailable:
     push cQualityOfServiceNotAvailable
     jmp nextFieldOfCause
     
ResourceUnavailableUnspecified:
     push cResourceUnavailableUnspecified
     jmp nextFieldOfCause

PrecedenceCallBlocked:
     push cPrecedenceCallBlocked
     jmp nextFieldOfCause

RequestedCircuitChannelNotAvailable:
     push cRequestedCircuitChannelNotAvailable
     jmp nextFieldOfCause
     
AccessInformationDiscarded:
     push cAccessInformationDiscarded
     jmp  nextFieldOfCause

SwitchingEquipmentCongestion:
     push cSwitchingEquipmentCongestion
     jmp  nextFieldOfCause
     
TemporaryFailure:
     push cTemporaryFailure
     jmp nextFieldOfCause

PermanentFrameModeConnectionOperational:
     push cPermanentFrameModeConnectionOperational
     jmp nextFieldOfCause

PermanentFrameModeConnectionOutOfService:
     push cPermanentFrameModeConnectionOutOfService
     jmp nextFieldOfCause

NetworkOutOfOrder:
     push cNetworkOutOfOrder
     jmp nextFieldOfCause

CallQueued:
     push cCallQueued
     jmp nextFieldOfCause

NoCircuitChannelAvailable:
     push cNoCircuitChannelAvailable
     jmp nextFieldOfCause

NormalUnspecified:
     push cNormalUnspecified
     jmp nextFieldOfCause

ResponseToSTATUSINQUIRY:
     push cResponseToSTATUSINQUIRY
     jmp nextFieldOfCause

InvalidNumberFormatAddrIncomplete:
     push cInvalidNumberFormatAddrIncomplete
     jmp nextFieldOfCause

FacilitiesRejected:
     push cFacilitiesRejected
     jmp nextFieldOfCause

DestinationOutOfOrder:
     push cDestinationOutOfOrder
     jmp nextFieldOfCause

NonSelectedUserClearing:
     push cNonSelectedUserClearing
     jmp nextFieldOfCause

NumberChanged:
	 push cNumberChanged
	 jmp nextFieldOfCause
	 
CallRejected:
	 push cCallRejected
	 jmp nextFieldOfCause

SubscriberAbsent:
     push cSubscriberAbsent
     jmp nextFieldOfCause

NoAnswerFromUser:
     push cNoAnswerFromUser
     jmp nextFieldOfCause

NoUserResponding:
     push cNoUserResponding
     jmp nextFieldOfCause


UserBusy:
     push cUserBusy
     jmp nextFieldOfCause


NormalCallClearing:
     push cNormalCallClearing
     jmp nextFieldOfCause

PreemptionCircuitReserved:
     push cPreemptionCircuitReserved
     jmp nextFieldOfCause

Preemption:
     push cPreemption
     jmp nextFieldOfCause

CallAwardedBeingDelivered:
     push cCallAwardedBeingDelivered
     jmp nextFieldOfCause
     
ChannelUnacceptable:
     push cChannelUnacceptable
     jmp nextFieldOfCause

MisdialedTrunkPrefix:
     push cMisdialedTrunkPrefix
     jmp nextFieldOfCause

SendSpecialInformationTone:
     push cSendSpecialInformationTone
     jmp nextFieldOfCause

NoRouteToDestination:
     push cNoRouteToDestination
     jmp nextFieldOfCause

NoRouteToSpecified:
     push cNoRouteToSpecified
     jmp nextFieldOfCause
     
UnallocatedNumber:
     push cUnallocatedNumber
     jmp nextFieldOfCause

nextFieldOfCause:    //  location
     push eax      // значение CauseValue
     inc ebx 
     mov al, byte ptr [ebx]
     cmp al, 2    // location ?
     jz Location 

     push cLocation
     push eax
     jmp notField

Location:
     inc ebx
     inc ebx
     xor eax, eax 
     mov al, byte ptr [ebx]
     test al, al
     jz User
     cmp al, 1
     jz localUserPrivateNetwork
     cmp al, 2
     jz localUserPublicNetwork
     cmp al, 3
     jz transitNetwork
     cmp al, 4
     jz RemoteUserPublicNetwork
     cmp al, 5
     jz RemoteUsePrivateNetwork
     cmp al, 7
     jz InternationalNetwork
     cmp al, 10
     jz BeyondInterworkPoint

     push eax
     push cLocation
     push cOther2ValueFieldFrmt
     jmp PrintParseError

BeyondInterworkPoint:
     push cBeyondInterworkPoint
     jmp printCause

InternationalNetwork:
     push cInternationalNetwork
     jmp printCause
             
RemoteUsePrivateNetwork:
     push cRemoteUsePrivateNetwork
     jmp printCause

RemoteUserPublicNetwork:
     push cRemoteUserPublicNetwork
     jmp printCause

User:
     push cUser
     jmp printCause

localUserPrivateNetwork:
     push cLocalUserPrivateNetwork
     jmp printCause

localUserPublicNetwork:
     push cLocalUserPublicNetwork
     jmp printCause

transitNetwork:
     push cTransitNetwork
     jmp printCause

printCause:                            // печатаем  поле cause

     pop edi // восстановим строковое значение Location
     pop edx // восстановим hex значение causeValue
     pop ecx // восстановим строковое значение causeValue

     cmp bCsv, 1
     jz PrintCause_csv

     push edi    // строковое значение Location
     push eax   // hex значение Location
     push cLocation   
     push ecx     // строковое значение causeValue
     push edx     // hex значение causeValue
     push cCauseValue
     push cCause
     push cPrintCauseFieldFrmt
	 //call printf
	 //add esp, 32

	 push outfile
     call fprintf
     add esp, 36
     jmp ExchangeInfoField

PrintCause_csv:
     /*push ecx  // строковое значение causeValue
     push cCsv_Frmt
     call printf
     jmp ExchangeInfoField  */

     push ecx  // строковое значение causeValue
     push cCsv_Frmt
     push cStr_csv_tmp
     call sprintf
     add esp, 12

     push 0  //  iOffSet
     push cStr_csv_tmp  // cValue
     push cCause    // cField
     push cRecordHeader_s1
     push cStr_csv
     call InsertToCsvString
//(char &cString, char *cHeaderStr, char *cField, char *cValue, short iOffSet)
     add esp, 20
     jmp ExchangeInfoField
     
//---------------------- поле SupplementaryServices --------------------------
SupplementaryServices:
     inc ebx
     xor ecx, ecx
	 mov cl, byte ptr [ebx]   // длина поля SupplementaryServices
     xor eax, eax

     cmp bCsv, 1
	 jnz SupplementaryServicesPrint

	 add ebx, ecx   // если CSV формат, не печатаем
	 inc ebx  // прибавим 1 байт - учитываем длину поля SupplementaryServices
	 jmp Cause 

SupplementaryServicesPrint:
	 push ecx  // сохраним длину поля SupplementaryServices
	 
     push  cSupplementaryService
	 push  cSupplementaryServices
	 push cSupplementaryServicesFrmt
	 //call printf    // печатаем заголовок поля SupplementaryServices
	 //add esp, 12

	 push outfile
     call fprintf
     add esp, 16
	 pop ecx   // длина поля SupplementaryServices

SupplementaryServiceEval:
     dec cl
	 jl SupplementaryServicesEnd  // если cl < 0

     inc ebx
     mov al, byte ptr [ebx]
     cmp al, 0x30
     jz SupplementaryService
     cmp al, 4
     jz SupplementaryServiceCode
     cmp al, 0x0A
     jz SupplementaryAction

     push cSupplementaryServices
     push eax
	 jmp notField

SupplementaryService:
     inc ebx
     //mov cl, byte ptr [ebx]
     jmp SupplementaryServiceEval

SupplementaryServiceCode:
     inc ebx
     inc ebx
     dec cl
     dec cl
     mov ax, word ptr [ebx]
     bswap eax
     shr eax, 16
     inc ebx 
     dec cl
     dec cl

     cmp ax, 1
     jz SSC_1
     cmp ax, 4
     jz SSC_4
     cmp ax, 12
     jz SSC_12
     cmp ax, 13
     jz SSC_13
     cmp ax, 36
     jz SSC_36
     cmp ax, 37
     jz SSC_37
     cmp ax, 41
     jz SSC_41
     cmp ax, 48
     jz SSC_48
     cmp ax, 50
     jz SSC_50
     cmp ax, 51
     jz SSC_51
     cmp ax, 52
     jz SSC_52
     cmp ax, 55
     jz SSC_55
     cmp ax, 58
     jz SSC_58
     cmp ax, 60
     jz SSC_60
     cmp ax, 67
     jz SSC_67
     cmp ax, 70
     jz SSC_70
     cmp ax, 71
     jz SSC_71
     cmp ax, 82
     jz SSC_82
     cmp ax, 84
     jz SSC_84
     cmp ax, 86
     jz SSC_86
     cmp ax, 98
     jz SSC_98
     cmp ax, 103
     jz SSC_103
     cmp ax, 104
     jz SSC_104
     cmp ax, 113
     jz SSC_113
     cmp ax, 123
     jz SSC_123
     cmp ax, 139
     jz SSC_139
     cmp ax, 148
     jz SSC_148
     cmp ax, 153
     jz SSC_153
     cmp ax, 197
     jz SSC_197
     cmp ax, 206
     jz SSC_206
     cmp ax, 1001
     jz SSC_1001
     cmp ax, 1003
     jz SSC_1003
     cmp ax, 1004
     jz SSC_1004
     
     push cUnknown
     push eax
     jmp SupplementaryServiceEval


SSC_1:
     push cSSC_1
     push eax
     jmp SupplementaryServiceEval
SSC_4:
     push cSSC_4
     push eax
     jmp SupplementaryServiceEval
SSC_12:
     push cSSC_12
     push eax
     jmp SupplementaryServiceEval
SSC_13:
     push cSSC_13
     push eax
     jmp SupplementaryServiceEval
SSC_36:
     push cSSC_36
     push eax
     jmp SupplementaryServiceEval
SSC_37:
     push cSSC_37
     push eax
     jmp SupplementaryServiceEval
SSC_41:
     push cSSC_41
     push eax
     jmp SupplementaryServiceEval
SSC_48:
     push cSSC_48
     push eax
     jmp SupplementaryServiceEval
SSC_50:
     push cSSC_50
     push eax
     jmp SupplementaryServiceEval
SSC_51:
     push cSSC_51
     push eax
     jmp SupplementaryServiceEval
SSC_52:
     push cSSC_52
     push eax
     jmp SupplementaryServiceEval
SSC_55:
     push cSSC_55
     push eax
     jmp SupplementaryServiceEval
SSC_58:
     push cSSC_58
     push eax
     jmp SupplementaryServiceEval
SSC_60:
     push cSSC_60
     push eax
     jmp SupplementaryServiceEval

SSC_67:
     push cSSC_67
     push eax
     jmp SupplementaryServiceEval
SSC_70:
     push cSSC_70
     push eax
     jmp SupplementaryServiceEval
SSC_71:
     push cSSC_71
     push eax
     jmp SupplementaryServiceEval
SSC_82:
     push cSSC_82
     push eax
     jmp SupplementaryServiceEval
SSC_84:
     push cSSC_84
     push eax
     jmp SupplementaryServiceEval
SSC_86:
     push cSSC_86
     push eax
     jmp SupplementaryServiceEval
SSC_98:
     push cSSC_98
     push eax
     jmp SupplementaryServiceEval
SSC_103:
     push cSSC_103
     push eax
     jmp SupplementaryServiceEval
SSC_104:
     push cSSC_103
     push eax
     jmp SupplementaryServiceEval
SSC_113:
     push cSSC_113
     push eax
     jmp SupplementaryServiceEval
SSC_123:
     push cSSC_123
     push eax
     jmp SupplementaryServiceEval
SSC_139:
     push cSSC_139
     push eax
     jmp SupplementaryServiceEval
SSC_148:
     push cSSC_148
     push eax
     jmp SupplementaryServiceEval
SSC_153:
     push cSSC_153
     push eax
     jmp SupplementaryServiceEval
SSC_197:
     push cSSC_197
     push eax
     jmp SupplementaryServiceEval
SSC_206:
     push cSSC_206
     push eax
     jmp SupplementaryServiceEval
SSC_1001:
     push cSSC_1001
     push eax
     jmp SupplementaryServiceEval
SSC_1003:
     push cSSC_1003
     push eax
     jmp SupplementaryServiceEval
SSC_1004:
     push cSSC_1004
     push eax
     jmp SupplementaryServiceEval

SupplementaryAction:
     inc ebx
     inc ebx
     dec cl
     dec cl
     xor eax, eax
     mov al, byte ptr [ebx]
     test al, al
     jz Provision
     cmp al, 1
     jz Withdrawal
     cmp al, 2
     jz Registration
     cmp al, 3
     jz Erasure
     cmp al, 4
     jz Activation
     cmp al, 5
     jz Deactivation
     cmp al, 6
     jz Invocation
     cmp al, 7
     jz Disabling
     cmp al, 8
     jz Interrogation

Interrogation:
     push cInterrogation
     push eax
	 jmp SupplementaryServicePrint

Disabling:
     push cDisabling
     push eax
	 jmp SupplementaryServicePrint

Invocation:
     push cInvocation
     push eax
	 jmp SupplementaryServicePrint

Deactivation:
     push cDeactivation
     push eax
	 jmp SupplementaryServicePrint

Activation:
     push cActivation
     push eax
	 jmp SupplementaryServicePrint

Erasure:
     push cErasure
     push eax
	 jmp SupplementaryServicePrint
     
Registration:
     push cRegistration
     push eax
	 jmp SupplementaryServicePrint

Withdrawal:
     push cWithdrawal
     push eax
	 jmp SupplementaryServicePrint
     
Provision:
     push cProvision
     push eax

SupplementaryServicePrint:
	 pop eax   //     hex значение  SupplementaryAction
	 pop edi   //  string значение  SupplementaryAction
	 pop edx   //     hex значение  SupplementaryServiceCode
	 pop esi   //  string значение  SupplementaryServiceCode

	 push ecx  // сохраним длину поля SupplementaryServices

	 push edi  //  string значение  SupplementaryAction
     push eax      // hex значение  SupplementaryAction
     push cSupplementaryAction
	 push esi  //  string значение  SupplementaryServiceCode
	 push edx      // hex значение  SupplementaryServiceCode
     push cSupplementaryServiceCode 
	 push cSupplementaryServiceFrmt
	 //call printf  // печатаем Supplementary Action и Code
	 //add esp, 28

	 push outfile
     call fprintf
     add esp, 32
	 pop ecx  // длина поля SupplementaryServices
	 jmp SupplementaryServiceEval  // на разбор следующего байта
	 
SupplementaryServicesEnd:
     inc ebx
     jmp Cause

//--------------------------- поле PartialGeneration --------------------
PartialGenerationField:
     inc ebx
     xor ecx, ecx
     xor eax, eax
     mov cl, byte ptr [ebx]    // длина PartialGeneration

PartialGenerationParse:
     inc ebx
     dec cl
     jl PrintPartialGeneration

     mov al, byte ptr [ebx]
     cmp al, 0x80
     jz PartialRecordNumber
     cmp al, 0x81
     jz PartialRecordReason

     push cPartialGeneration
     push eax
     jmp notField

PartialRecordNumber:
     inc ebx  // длина 
     inc ebx
     mov ax, word ptr [ebx]
     inc ebx
     dec cl
     dec cl
     dec cl
     bswap eax
     shr eax, 16
     
     push eax
     jmp PartialGenerationParse

PartialRecordReason:
     inc ebx // длина
     inc ebx
     dec cl
     dec cl
     mov al, byte ptr [ebx]
     test al, al
     jz TimeLimit
     cmp al, 1
     jz ServiceChange
     cmp al, 2
     jz Overflow
     cmp al, 3
     jz NetworkInternalReasons
     cmp al, 4
     jz  LastCDR
     cmp al, 5
     jz TimeChange

     push eax
     push cPartialRecordReason
     push cOther2ValueFieldFrmt
     jmp PrintParseError

TimeChange:
     push cTimeChange
     push eax
     jmp PartialGenerationParse

LastCDR:
     push cLastCDR
     push eax
     jmp PartialGenerationParse

NetworkInternalReasons:
     push cNetworkInternalReasons
     push eax
     jmp PartialGenerationParse

Overflow:
     push cOverflow
     push eax
     jmp PartialGenerationParse

ServiceChange:
     push cServiceChange
     push eax
     jmp PartialGenerationParse
     
TimeLimit:
     push cTimeLimit
     push eax
     jmp PartialGenerationParse

PrintPartialGeneration:
     pop eax  // hex значение PartialRecordReason
     pop edx  // string значение PartialRecordReason
     pop ecx  // hex значение PartialRecordNumber

     cmp bCsv, 1
     jz ExchangeInfoFieldPre
     
     push edx  // string значение PartialRecordReason
     push eax  // hex значение PartialRecordReason
     push cPartialRecordReason
     push ecx  // hex значение PartialRecordNumber
     push cPartialRecordNumber
     push cPartialGeneration
     push cPartialGenerationFrmt
	 //call printf
	 //add esp, 28

	 push outfile
     call fprintf
     add esp, 32

ExchangeInfoFieldPre:
     dec ebx
     
//-------------------------  поле Exchange Info ------------------------
ExchangeInfoField:
	 inc ebx
	 xor eax, eax
     mov al, byte ptr [ebx]
     cmp al, 0xAB    // Exchange Info ?
     jz ExchangeInfo
     cmp al, 0xAA
     jz PartialGenerationField
     cmp al, 0xA5
     jz SupplementaryServices

     push cExchangeInfo
     push eax
	 jmp notField

ExchangeInfo:
     inc ebx
     mov cl, byte ptr [ebx]  // длина  Exchange Info
     inc ebx
     mov al, byte ptr [ebx]
     dec cl
     cmp al, 0x80   // Exchange ID ?
     jz  ExchangeID

     push cExchangeID
     push eax
     jmp notField

ExchangeID:
     inc ebx
     xor ecx, ecx // сброс счетчика
     mov cl, byte ptr [ebx]  // длина строки Exchange ID
     
     lea edi, sDigits    //  sDigits - строка ExchangeID
     mov edi, dword ptr [edi]

LoadStringChars:
     inc ebx
     mov al, byte ptr [ebx]  // загрузим символ строки

     stosb    // сохраняем байт из al в es:[edi]
     loop LoadStringChars
     mov byte ptr [edi], 0  // закончим строку ExchangeID нулем

     cmp bCDRExchangeID, 1      // если строка ExchangeID в CDR заголовке
     jz ExchangeID_CDR_Print
     cmp bCDR_SWversion, 1
     jz SoftwareVersionCDR_Print
     cmp bCDR_FileName, 1
     jz FileNameCDR_Print

     cmp bCsv, 1
     jz ExchangeInfoPrint_csv

     push sDigits
     push cExchangeID
     push cExchangeInfo
     push cPrintField3stringFrmt
	 //call printf           // печатаем   Exchange Info
	 //add esp, 16

	 push outfile
     call fprintf
     add esp, 20             
     jmp NextField

ExchangeInfoPrint_csv:
     /*push sDigits
     push cCsv_Frmt
     call printf  */

     push sDigits     // строка ExchangeID
     push cCsv_Frmt
     push cStr_csv_tmp
     call sprintf  // пишем ExchangeID в формате CSV в стоку cStr_csv_tmp
     add esp, 12

     push 0         // iOffSet
     push cStr_csv_tmp   // cValue - ExchangeID
     push cExchangeID       //  cField 
     push cRecordHeader_s1  //  cHeaderStr
     push cStr_csv         //  cString
     call InsertToCsvString
     add esp, 20
     jmp NextField

// ------- выбор additionalParticipantInfo или CallingPartyCategory ----------
NextField:
	 inc ebx
	 xor eax, eax
     mov al, byte ptr [ebx]
     cmp al, 0xAE   //   additionalParticipantInfo ?
     jz AdditionalParticipantInfo
     cmp al, 0x8F
     jz CallingPartyCategory
     
     push cAdditionalParticipantInfo
     push eax
	 jmp notField

// --------------------- поле additionalParticipantInfo ------------------
AdditionalParticipantInfo:
     inc ebx
	 //mov cl, byte ptr [ebx]  // длина поля AdditionalParticipantInfo
	 inc ebx
	 xor eax, eax
     mov al, byte ptr [ebx]
     cmp al, 0x80
     jz  PhysicalLineCode

     push cPhysicalLineCode
     push eax
	 jmp notField

PhysicalLineCode:
     inc ebx
     xor ecx, ecx // сброс счетчика
     mov cl, byte ptr [ebx]   // длина  Physical Line Code

     mov edi, dword ptr [sPLCDigits]

LoadPLCDigits:
     inc ebx
     xor esi, esi     // сброс признака младшей цифры
     mov ah, byte ptr [ebx]  // загрузим байт TBCD кода - это 2 цифры

     and ah, 0xF0    // получаем старшую цифру в ah
     shr ah, 4       // получаем старшую цифру в ah

     mov al, ah  // сохраним старшую цифру для проверки в al

PLCDigitEval:     
     cmp al, 10        // это цифра?
     jl MakeDigit

     // нет, это 16-ричная буква A-D
     cmp al, 0xA
     jz MakeA
     cmp al, 0xB
     jz MakeB   // взамен MakeB
     cmp al, 0xC
     jz MakeC   // взамен MakeC
     cmp al, 0xD
     jz MakeD
     cmp al, 0xE
     jz MakeE
     cmp al, 0xF
     jz MakeF

     push eax
     push sDigits
     push cWrong2ValueFieldFrmt
     jmp PrintParseError

MakeA:
     mov al, 0x41
     jmp JungerDigitBeg

MakeB:
     mov al, 0x42
     jmp JungerDigitBeg

MakeC:
     mov al, 0x43
     jmp JungerDigitBeg

MakeD:
     mov al, 0x44
     jmp JungerDigitBeg

MakeE:
     mov al, 0x45
     jmp JungerDigitBeg

MakeF:
     mov al, 0x46
     jmp JungerDigitBeg

MakeDigit:
     or al, 0x30     // ASCII код старшей или младшей цифры

JungerDigitBeg:  
     test esi, esi
     jnz NexPLCDigit  // если признак младшей цифры установлен

     mov ah, al      // ASCII код старшей цифры

     mov al, byte ptr [ebx]  // загрузим байт TBCD кода - это 2 цифры
     and al, 0x0F    // младшая цифра
     //mov al, ah
     mov esi, -1   // признак младшей цифры
     jmp PLCDigitEval
     
NexPLCDigit:
     //mov ah, dl  // ASCII код 1-й (старшей) цифры
     cmp cl, 2                  // если счетчик байтов =2
     jnz StorePhysicalLineCodeDigits

         // то вставить разделитель посередине
     push ax   // сохраним содержимое регистра в стек
     mov al, 0x02F  //  разделитель '/'
     stosb     //   сохраним разделитель
     pop ax    // восстановим слово из стека
     
StorePhysicalLineCodeDigits:
     mov dl, al
     mov al, ah
     stosb    // сохраняем байт al в es:[edi]
     mov al, dl
     stosb    // сохраняем байт al в es:[edi]

     dec cl
     jnz LoadPLCDigits

     mov byte ptr [edi], 0  // закончим строку sDigits нулем
     sub edi, 9
	 inc ebx
	 xor eax, eax
     mov al, byte ptr [ebx]
     cmp al, 0x81   //  receivedDigits ?
     jz ReceivedDigits

	 cmp al, 0x8F  // поле callingPartyCategory ?
	 jz AdditionalParticipantInfoWoReceivedDigitsPrint

     push cReceivedDigits
     push eax
     jmp notField

ReceivedDigits:
     mov esi, -1 // установка признака ReceivedDigits AdditionalParticipantInfo
     jmp NumberEval

AdditionalParticipantInfoWoReceivedDigitsPrint:
	 mov eax, cNullStr
	 mov sDigits, eax
	 dec ebx    // назад на поле 0x8F 
	 cmp bCsv, 1
	 jz AdditionalPartInfoPrint_csv

	 push sPLCDigits
	 push cPhysicalLineCode
	 push cAdditionalParticipantInfo
	 push cAdditionalParticipantInfoWoDigitsFrmt
	 //call printf   // печатаем поле  additionalParticipantInfo
	 //add esp, 16

	 push outfile
     call fprintf
     add esp, 20
	 jmp CallingPartyCategoryField
	 
AdditionalParticipantInfoPrint:
     /*
     pop eax   //  восстановим  TypeOfNatAddr
     pop ecx   //  восстановим  Odd/Even Ind.
     */
     cmp bCsv, 1
     jz AdditionalPartInfoPrint_csv

	 push sDigits   // ReceivedDigits
     push cAdressdigits
	 push eax    // TypeOfNatAddr
     push cNatOfAddr
     push ecx    // Odd/Even Ind.
     push cOddEvenNumbInd
	 push cReceivedDigits
	 push sPLCDigits  // PhysicalLineCode
	 push cPhysicalLineCode
	 push cAdditionalParticipantInfo
	 push cAdditionalParticipantInfoFrmt
	 //call printf   // печатаем поле  additionalParticipantInfo
	 //add esp, 44

	 push outfile
     call fprintf
     add esp, 48
	 jmp CallingPartyCategoryField

AdditionalPartInfoPrint_csv:
     /*push sDigits
     push eax     // TypeOfNatAddr
     push sPLCDigits
     push cCsv3ValFrmt
     call printf   */

     push sDigits     // номер ReceivedDigits
     push eax     // TypeOfNatAddr
     push sPLCDigits     // PhysicalLineCode
     push cCsv3ValFrmt
     push cStr_csv_tmp
     call sprintf  // пишем 3 значения в формате CSV в стоку cStr_csv_tmp
	 add esp, 20

     push 0         // iOffSet
     push cStr_csv_tmp   // cValue - ExchangeID
     push cPhysicalLineCode       //  cField 
     push cRecordHeader_s1  //  cHeaderStr
     push cStr_csv         //  cString
     call InsertToCsvString
     add esp, 20
     
// ------------------ поле callingPartyCategory --------------------------
CallingPartyCategoryField:
     inc ebx
     xor eax, eax
     mov al, byte ptr [ebx]
     cmp al, 0x8F  //  callingPartyCategory ?
     jz CallingPartyCategory

     push cCallingPartyCategory
     push eax
	 jmp notField

CallingPartyCategory:
     inc ebx 
     mov cl, byte ptr [ebx]
     cmp cl, 2
     jz CPCRightLength

     push eax
     push cCallingPartyCategory
     jmp FieldWrongLength

CPCRightLength:
     inc ebx
     xor eax, eax
     mov ax, word ptr [ebx]
     bswap eax
     shr eax, 16
     test ax, ax
     jz NOT_DEFINED
     cmp ax, 1
     jz INTERNAT_OPERATOR
     cmp ax, 2
     jz CPCOrdinarySubscriber
     cmp ax, 3
     jz CPCSubscriberPriority
     cmp ax, 4
     jz DATA_CALL
     cmp ax, 5
     jz TEST_CALL
     cmp ax, 6
     jz PUBLIC_PAYPHONE
     cmp ax, 7
     jz SYSTEM_CALL
     cmp ax, 8
     jz L8_NOTDEFINED_BY_SPEC
     cmp ax, 9
     jz L9_NOTDEFINED_BY_SPEC
     cmp ax, 10
     jz L10_NOTDEFINED_BY_SPEC
     cmp ax, 11
     jz NATIONAL_OPERATOR
     cmp ax, 12
     jz NON_PUBLIC_PAYPHONE
     cmp ax, 13
     jz Business_Subscriber
     cmp ax, 14
     jz ATTENDANT
     cmp ax, 15
     jz Business_Subscriber_Prior
     cmp ax, 16
     jz TELEACTION
     cmp ax, 18
     jz TRANSMISSION_TEST_POS
     cmp ax, 19
     jz DISPATCHER_OF_PCN
     cmp ax, 20
     jz EDSS1_SUBSCRIBER
     cmp ax, 21
     jz MOBILE_SUBSCRIBER
     cmp ax, 22
     jz TELETEX_CUSTOMER
     cmp ax, 23
     jz TRUNK_OFF
     cmp ax, 24
     jz COINAFON
     cmp ax, 25
     jz NRT80_OR_NRT80P
     cmp ax, 26
     jz IKZ50_INVOLVED
     cmp ax, 27
     jz LINE_TEST_DESK
     cmp ax, 28
     jz L1TR6_SUBSCRIBER
     cmp ax, 29
     jz PABX
     cmp ax, 30
     jz MT_OPERATOR
     cmp ax, 31
     jz CALL_FORWARDED_TO_FEAFD
     cmp ax, 32
     jz HOTEL_SUBS
     cmp ax, 33
     jz HOTEL_PRIV_METER
     cmp ax, 34
     jz SUBS_CWC
     cmp ax, 35
     jz SUBS_BBG
     cmp ax, 37
     jz TRANSIT_GATES
     cmp ax, 38
     jz TESTCALL_TO_SUBSCRIBER
     cmp ax, 40
     jz OPER_FORW_TRANSF_INTNAT
     cmp ax, 41
     jz SPECIAL_SUBSCRIBER
     cmp ax, 42
     jz SUBS_SPEC_PRIORITY
     cmp ax, 43
     jz RESTRICT_PAYPHONE
     cmp ax, 45
     jz OPERATOR_INTERCEPTION
     cmp ax, 47
     jz SUBSC_NO_RESTRICTION
     cmp ax, 49
     jz INM_CHARGE_SUBSCRIBER
     cmp ax, 148
     jz PAYPHONE_LOC_STD
     cmp ax, 149
     jz PAYPHONE_LOCAL
     cmp ax, 150
     jz TRUNK_TEST
     cmp ax, 151
     jz MESSSYSTEM
     cmp ax, 152
     jz SERVICE_ACCESS
     cmp ax, 198
     jz OPERATOR_FORWARD_TRANSF
     cmp ax, 199
     jz OPERATOR
     cmp ax, 200
     jz SUBSCRIBER_PRICE_REQ
     cmp ax, 202
     jz DATA_CALL_INTERNAT
     cmp ax, 203
     jz PRIORITY_SUBSC_INTERNAT
     cmp ax, 205
     jz RESTRICTED_PAYPHONE
     cmp ax, 206
     jz CCR_SUBSCRIBER
     cmp ax, 207
     jz CALL_DIVERSION
     cmp ax, 208
     jz ORDINARY_SUBSCR_WITH_MM
     cmp ax, 0x2C
     jz CPCPayphone
     cmp ax, 0x2E
     jz  CPCHotel_Subsc_Internat
     cmp ax, 0x30
     jz CPCUnknown
     cmp ax, 0xC9
     jz OrdinarySubscInternat
     cmp ax, 0xCC
     jz SUBSCR_RESTR_SERVICE

     // unrecognized Caling party categories
     push eax
     push cCallingPartyCategory
     push cOther2ValueFieldFrmt
     jmp PrintParseError

TEST_CALL:
     push cTEST_CALL
     jmp CPCPrint
PUBLIC_PAYPHONE:
     push cPUBLIC_PAYPHONE
     jmp CPCPrint
SYSTEM_CALL:
     push cSYSTEM_CALL
     jmp CPCPrint
L8_NOTDEFINED_BY_SPEC:
     push c8_NOTDEFINED_BY_SPEC
     jmp CPCPrint
L9_NOTDEFINED_BY_SPEC:
     push c9_NOTDEFINED_BY_SPEC
     jmp CPCPrint
L10_NOTDEFINED_BY_SPEC:
     push c10_NOTDEFINED_BY_SPEC
     jmp CPCPrint
NATIONAL_OPERATOR:
     push cNATIONAL_OPERATOR
     jmp CPCPrint
NON_PUBLIC_PAYPHONE:
     push cNON_PUBLIC_PAYPHONE
     jmp CPCPrint
Business_Subscriber:
     push cBusiness_Subscriber
     jmp CPCPrint
ATTENDANT:
     push cATTENDANT
     jmp CPCPrint
Business_Subscriber_Prior:
     push cBusiness_Subscriber_Prior
     jmp CPCPrint
TELEACTION:
     push cTELEACTION
     jmp CPCPrint
TRANSMISSION_TEST_POS:
     push cTRANSMISSION_TEST_POS
     jmp CPCPrint
DISPATCHER_OF_PCN:
     push cDISPATCHER_OF_PCN
     jmp CPCPrint
EDSS1_SUBSCRIBER:
     push cEDSS1_SUBSCRIBER
     jmp CPCPrint
MOBILE_SUBSCRIBER:
     push cMOBILE_SUBSCRIBER
     jmp CPCPrint
TELETEX_CUSTOMER:
     push cTELETEX_CUSTOMER
     jmp CPCPrint
TRUNK_OFF:
     push cTRUNK_OFF
     jmp CPCPrint
COINAFON:
     push cCOINAFON
     jmp CPCPrint
NRT80_OR_NRT80P:
     push cNRT80_OR_NRT80P
     jmp CPCPrint
IKZ50_INVOLVED:
     push cIKZ50_INVOLVED
     jmp CPCPrint
LINE_TEST_DESK:
     push cLINE_TEST_DESK
     jmp CPCPrint
L1TR6_SUBSCRIBER:
     push c1TR6_SUBSCRIBER
     jmp CPCPrint
PABX:
     push cPABX
     jmp CPCPrint
MT_OPERATOR:
     push cMT_OPERATOR
     jmp CPCPrint
CALL_FORWARDED_TO_FEAFD:
     push cCALL_FORWARDED_TO_FEAFD
     jmp CPCPrint
HOTEL_SUBS:
     push cHOTEL_SUBS
     jmp CPCPrint
HOTEL_PRIV_METER:
     push cHOTEL_PRIV_METER
     jmp CPCPrint
SUBS_CWC:
     push cSUBS_CWC
     jmp CPCPrint
SUBS_BBG:
     push cSUBS_BBG
     jmp CPCPrint
TRANSIT_GATES:
     push cTRANSIT_GATES
     jmp CPCPrint
TESTCALL_TO_SUBSCRIBER:
     push cTESTCALL_TO_SUBSCRIBER
     jmp CPCPrint
OPER_FORW_TRANSF_INTNAT:
     push cOPER_FORW_TRANSF_INTNAT
     jmp CPCPrint
SPECIAL_SUBSCRIBER:
     push cSPECIAL_SUBSCRIBER
     jmp CPCPrint
SUBS_SPEC_PRIORITY:
     push cSUBS_SPEC_PRIORITY
     jmp CPCPrint
RESTRICT_PAYPHONE:
     push cRESTRICT_PAYPHONE
     jmp CPCPrint
OPERATOR_INTERCEPTION:
     push cOPERATOR_INTERCEPTION
     jmp CPCPrint
SUBSC_NO_RESTRICTION:
     push cSUBSC_NO_RESTRICTION
     jmp CPCPrint
INM_CHARGE_SUBSCRIBER:
     push cINM_CHARGE_SUBSCRIBER
     jmp CPCPrint
PAYPHONE_LOC_STD:
     push cPAYPHONE_LOC_STD
     jmp CPCPrint
PAYPHONE_LOCAL:
     push cPAYPHONE_LOCAL
     jmp CPCPrint
TRUNK_TEST:
     push cTRUNK_TEST
     jmp CPCPrint
MESSSYSTEM:
     push cMESSSYSTEM
     jmp CPCPrint
SERVICE_ACCESS:
     push cSERVICE_ACCESS
     jmp CPCPrint
OPERATOR_FORWARD_TRANSF:
     push cOPERATOR_FORWARD_TRANSF
     jmp CPCPrint
OPERATOR:
     push cOPERATOR
     jmp CPCPrint
SUBSCRIBER_PRICE_REQ:
     push cSUBSCRIBER_PRICE_REQ
     jmp CPCPrint
DATA_CALL_INTERNAT:
     push cDATA_CALL_INTERNAT
     jmp CPCPrint
PRIORITY_SUBSC_INTERNAT:
     push cPRIORITY_SUBSC_INTERNAT
     jmp CPCPrint
RESTRICTED_PAYPHONE:
     push cRESTRICTED_PAYPHONE
     jmp CPCPrint
CCR_SUBSCRIBER:
     push cCCR_SUBSCRIBER
     jmp CPCPrint
CALL_DIVERSION:
     push cCALL_DIVERSION
     jmp CPCPrint
ORDINARY_SUBSCR_WITH_MM:
     push cORDINARY_SUBSCR_WITH_MM
     jmp CPCPrint
DATA_CALL:
     push cDATA_CALL
     jmp CPCPrint
INTERNAT_OPERATOR:
     push cINTERNAT_OPERATOR
     jmp CPCPrint

NOT_DEFINED:
     push cNOT_DEFINED
     jmp CPCPrint

SUBSCR_RESTR_SERVICE:
     push cSUBSCR_RESTR_SERVICE
     jmp CPCPrint
     
OrdinarySubscInternat:
     push cOrdinarySubscInternat
     jmp CPCPrint
     
CPCUnknown:
     push cCPCUnknown
     jmp CPCPrint

CPCHotel_Subsc_Internat:
     push cCPCHotel_Subsc_Internat
     jmp CPCPrint

CPCPayphone:
     push cCPCPayphone
     jmp CPCPrint

CPCSubscriberPriority:
     push cCPCSubscriberPriority
     jmp CPCPrint
          
CPCOrdinarySubscriber:
     push cCPCOrdinarySubscriber
     jmp CPCPrint     

CPCPrint:
     cmp bCsv, 1
     jz CPCPrint_csv

     push eax
     push cCallingPartyCategory
     push cPrintField2string1HexFrmt
	 //call printf
	 //add esp, 16

	 push outfile
     call fprintf
     add esp, 20
     jmp CallingPartyTypeField

CPCPrint_csv:
     /*push cCsv_Frmt
     call printf   */

     push cCsv_Frmt
     push cStr_csv_tmp
     call sprintf  // пишем значение CallingPartyCategory в формате CSV в
     add esp, 12    //  стоку cStr_csv_tmp

     push 0         // iOffSet
     push cStr_csv_tmp   // cValue - CallingPartyCategory
     push cCallingPartyCategory       //  cField 
     push cRecordHeader_s1  //  cHeaderStr
     push cStr_csv         //  cString
     call InsertToCsvString
     add esp, 20

//----------------- поле callingPartyType -----------------------
CallingPartyTypeField:
     inc ebx
     inc ebx
     xor eax, eax
     mov al, byte ptr [ebx]
     cmp al, 0x90
     jz CallingPartyType

     cmp al, 0xB4
     jz TrunGroupOutgoing

     cmp al, 0xB5
     jz TrunkGroupIncoming

     cmp al, 0xB1
     jz ChargingInformation

     cmp al, 0xB8
     jz CallDuration

     cmp al, 0x97
     jz Teleservice

     push cCallingPartyType
     push eax
     jmp notField

CallingPartyType:
     inc ebx
     mov cl, byte ptr [ebx]
     cmp cl, 1   // длина CallingPartyType = 1 ?
     jz CallingPartyTypeEval

     // длина CallingPartyType != 1 - ошибка
     push eax
     push cCallingPartyType
     jmp FieldWrongLength

CallingPartyTypeEval:
     inc ebx
     mov al, byte ptr [ebx]
     test al, al
     jz Analogue
     cmp al, 1
     jz CustomerLink
     cmp al, 2
     jz BasicAccess
     cmp al, 3
     jz PrimaryRateAccess

PrimaryRateAccess:
     push cPrimaryRateAccess
     jmp PrintCallingPartyType

BasicAccess:
     push cBasicAccess
     jmp PrintCallingPartyType

CustomerLink:
     push cCustomerLink
     jmp PrintCallingPartyType

Analogue:
     push cAnalogue
     jmp PrintCallingPartyType


PrintCallingPartyType:
     cmp bCsv, 1
     jz CallingPartyTypePrint_csv

     push eax
     push cCallingPartyType
     push cPrintField2string1HexFrmt
	 //call printf     // печатаем  CallingPartyType
	 //add esp, 16

	 push outfile
     call fprintf
     add esp, 20
     jmp NextField2
     
CallingPartyTypePrint_csv:
     /*push cCsv_Frmt
     call printf  */

     push cCsv_Frmt
     push cStr_csv_tmp
     call sprintf  // пишем значение CallingPartyType в формате CSV в
     add esp, 12    //  стоку cStr_csv_tmp

     push 0         // iOffSet
     push cStr_csv_tmp   // cValue - CallingPartyType
     push cCallingPartyType       //  cField
     push cRecordHeader_s1  //  cHeaderStr
     push cStr_csv         //  cString
     call InsertToCsvString
     add esp, 20

//---------------------------------------------------------------------
NextField2:
     inc ebx
     xor eax, eax
     mov al, byte ptr [ebx]

     cmp al, 0xB1
     jz ChargingInformation
     cmp al, 0xB8
     jz CallDuration
     cmp al, 0xB4
     jz TrunGroupOutgoing
     cmp al, 0xB5
     jz TrunkGroupIncoming
     cmp al, 0xBE
     jz RecordExtensions
     cmp al, 0x97
     jz Teleservice

     push cChargingInformation
     push eax
     jmp notField

//------------- поле TrunGroupOutgoing или TrunkGroupIncoming -----------------     
TrunGroupOutgoing:
     /*cmp bCsv, 1
     jz ParseTrunkGroup  */

     push cTrunkGroupOutgoing
     jmp ParseTrunkGroup
     
TrunkGroupIncoming:
     /*cmp bCsv, 1
     //jz ParseIncTrunkGroup_csv
     jz ParseTrunkGroup   */

     push cTrunkGroupIncoming
     jmp ParseTrunkGroup

/*ParseIncTrunkGroup_csv:
     push 1
     call PrintSpace_csv  */

ParseTrunkGroup:
     inc ebx
     mov cl, byte ptr [ebx]  // длина  TrunkGroupOutgoing
     inc ebx
     xor eax, eax
     mov al, byte ptr [ebx]
     cmp al, 0xA0
     jz TrunkGroupId

     push eax
     push cTrunkGroupId
     push cOther2ValueFieldFrmt
     jmp PrintParseError  

TrunkGroupId:
     inc ebx

     xor ecx, ecx  // обнуляем счетчик
     //mov cl, byte ptr [ebx]  // длина TrunkGroupId
     inc ebx   //  байт: 0x19
     inc ebx   // длина ASCII строки
     mov cl, byte ptr [ebx]  // длина ASCII строки TrunkGroupId
     lea edi, sDigits    //  sDigits - строка TrunkGroupId
     mov edi, dword ptr [edi]
     
LoadStringCharsTrunkGroupId:
     inc ebx
     mov al, byte ptr [ebx]  // загрузим символ строки

     stosb    // сохраняем байт из al в es:[edi]
     loop LoadStringCharsTrunkGroupId
     mov byte ptr [edi], 0  // закончим строку ExchangeID нулем

     pop ecx   // TrunkGroupOutgoing или  TrunkGroupIncoming

     cmp bCsv, 1
     jz TrunkGroupIdPrint_csv

     push sDigits
     push cTrunkGroupId
     push ecx  // TrunkGroupOutgoing или  TrunkGroupIncoming
     push cPrintField3stringFrmt
	 //call printf     // печатаем TrunkGroupId
	 //add esp, 16

	 push outfile
     call fprintf
     add esp, 20
     jmp NextField2

TrunkGroupIdPrint_csv:
     /*xor eax, eax
     mov al, byte ptr [ecx+11]
     cmp al, 0x4F  // 11 байт строки = 'O' -> строка TrunkGroupOutgoing ?
     jz TrunkGroupOutgoingPrint_csv
     cmp al, 0x49
     jz TrunkGroupIncomingPrint_csv

     push eax
     push ecx
     jmp notField   */

//TrunkGroupOutgoingPrint_csv:
     /*push sDigits
     push cCsv_Frmt
     call printf   // печатаем в csv TrunkGroup Outgoing / Incoming  */

	 push ecx   // сохраним TrunkGroupOutgoing или  TrunkGroupIncoming
	 
     push sDigits
     push cCsv_Frmt
     push cStr_csv_tmp
     call sprintf  // пишем значение CallingPartyType в формате CSV в
     add esp, 12    //  стоку cStr_csv_tmp

     pop ecx   // TrunkGroupOutgoing или  TrunkGroupIncoming
     push 0         // iOffSet
     push cStr_csv_tmp   // cValue - CallingPartyType
     push ecx       //  cField - TrunkGroupOutgoing или TrunkGroupIncoming
     push cRecordHeader_s1  //  cHeaderStr
     push cStr_csv         //  cString
     call InsertToCsvString
     add esp, 20
     
/*TrunkGroupIncomingPrint_csv:
     push sDigits
     push cCsv_Frmt
     call printf   // печатаем в csv TrunkGroupIncoming     */

     jmp NextField2
     
//-------------------- поле chargingInformation -------------------------
ChargingInformation:
     inc ebx
	 xor ecx, ecx
     mov cl, byte ptr [ebx]  // длина поля chargingInformation
     inc ebx
	 dec cl
	 xor eax, eax
     mov al, byte ptr [ebx]
     cmp al, 0xA1
     jz RecordedUnitsList
     cmp al, 0x81
     jz RecordedUnitsList
     cmp al, 0xA0
     jz recordedCurrency
     cmp al, 0x80
     jz recordedCurrency
     cmp al, 0xA2
     jz freeOfCharge
     cmp al, 0x82
     jz freeOfCharge
     cmp al, 0xA3
     jz chargeInfoNotAvailable
     cmp al, 0x83
     jz chargeInfoNotAvailable

	 push cChargingInformation
	 push eax
	 jmp notField

recordedCurrency:
freeOfCharge:
	 inc ebx
	 cmp bCsv, 1
	 jz NextField2
	 
	 push cFreeOfCharge
	 push cChargingInformation
     push cChargingInformationFreeOfChargeFrmt
	 //call printf
	 //add esp, 12

	 push outfile
     call fprintf
     add esp, 16
	 jmp NextField2

chargeInfoNotAvailable:

RecordedUnitsList:
     inc ebx
     dec cl
     //mov cl, byte ptr [ebx]  // длина recordedUnitsList
     inc ebx
     dec cl
     mov al, byte ptr [ebx]
     cmp al, 0x30
     jz recordedUnits

     push cRecordedUnits
	 push eax
	 jmp notField

recordedUnits:
     inc ebx
     //mov cl, byte ptr [ebx]  // длина recordedUnits
     dec cl
     inc ebx
	 dec cl
	 xor eax, eax
     mov al, byte ptr [ebx]
	 cmp al, 0x80
	 jz Units
	 cmp al, 0x81
	 jz UnitsNotAvailable

	 push cUnits
	 push eax
     jmp notField

UnitsNotAvailable:
	 cmp bCsv, 1
	 jmp NextField2
	 
	 push cUnitsNotAvailable   // UnitsNotAvailable
	 push cUnits
	 push cRecordedUnits
	 push cRecordedUnitsList
	 push cChargingInformation
	 push cChargingInformationUnitsNotAvailableFrmt
	 //call printf   // печатаем recordedNumberOfUnits
	 //add esp, 28

	 push outfile
     call fprintf
     add esp, 32
	 jmp NextField2

Units:
	 inc ebx
	 mov cl, byte ptr [ebx] // длина поля recordedNumberOfUnits
	 
recordedNumberOfUnitsEval:
     inc ebx
	 dec cl
	 mov al, byte ptr [ebx]  // recordedNumberOfUnits
	 cmp cl, 1
	 jle PrintChargingInformationOnlyOneByteNumberOfUnits

	 // если cl больше 1
	 shl eax, 8
     jmp recordedNumberOfUnitsEval

PrintChargingInformationOnlyOneByteNumberOfUnits:
     cmp bCsv, 1
	 jz RecordedTypeOfUnitsPre

	 push ecx   // сохраним счетчик длины

	 push eax   // recordedNumberOfUnits
	 push cRecordedNumberOfUnits
	 push cUnits
	 push cRecordedUnits
	 push cRecordedUnitsList
	 push cChargingInformation
	 push cChargingInformationFrmt
	 //call printf   // печатаем recordedNumberOfUnits
	 //add esp, 28

	 push outfile
     call fprintf
     add esp, 32

	 pop ecx  // восстановим счетчик длины

RecordedTypeOfUnitsPre:

     test cl, cl
	 jz NextField2
     cmp cl, 1
	 jz RecordedTypeOfUnits

     push ecx
	 push cRecordedTypeOfUnits
	 push cFieldWrongLengthFrmt
	 jmp PrintParseError

RecordedTypeOfUnits:
	 /*test cl, cl
     //jz CallDurationBeg
	 jz NextField2     */

	 // recordedTypeOfUnits
	 xor eax, eax
recordedTypeOfUnitsEval:
	 inc ebx
	 mov al, byte ptr [ebx]   // recordedTypeOfUnits
	 dec cl
     test cl, cl
	 jz printRecordedTypeOfUnits
	 
	 shl eax, 8
	 jmp recordedTypeOfUnitsEval

	 /*push cRecordedTypeOfUnits
	 push eax
	 jmp notField

	 jmp  EndRecord   */

printRecordedTypeOfUnits:
     cmp bCsv, 1
	 jz NextField2
     
     push eax        // recordedTypeOfUnits
     push cRecordedTypeOfUnits
     push cRecordedTypeOfUnitsFrmt
	 //call printf     // печатаем recordedTypeOfUnits
	 //add esp, 12

	 push outfile
     call fprintf
     add esp, 16

	 jmp NextField2
	 
//---------------------- поле callDuration --------------------------
CallDurationBeg:
	 inc ebx
     xor eax, eax
     mov al, byte ptr [ebx]
     cmp al, 0xB8
     jz CallDuration

     cmp al, 0x97
     jz Teleservice

	 jmp EndRecord
	 
CallDuration:
     cmp bCsv, 1
	 jz CallDurationLength
	 
	 push cCallDuration
	 push cCallDurationFrmt
	 //call printf    // печатаем заголовок поля CallDuration
	 //add esp, 8

	 push outfile
     call fprintf
     add esp, 12

CallDurationLength:
	 inc ebx
	 xor ecx, ecx
	 mov cl, byte ptr [ebx]  // длина поля CallDuration
	 //mov dword ptr [iFieldLength], ecx   // длина поля CallDuration  11
	 
CallDurationNex:
	 test cl, cl
	 jz NextField3
	 inc ebx
	 dec cl
	 push ecx   // длина поля CallDuration  10 9 8
	 //dec iFieldLength  // 10 9 8
	 jle NextField3   // если счетчик длины <= 0  - конец поля

	 xor eax, eax
	 mov al, byte ptr [ebx]
     cmp al, 0x80
	 jz ConversationTime
	 cmp al, 0x81
	 jz DurationTimeACM
	 cmp al, 0x82
	 jz DurationTimeB_ans
	 cmp al, 0x83
	 jz DurationTimeNoANM

     push cConversationTime
     push eax
	 jmp notField

DurationTimeNoANM:
     cmp bCsv, 1
	 jz DurationTimeSkip
	 push cDurationTimeNoANM
	 jmp ConversationTimeEvalBeg

DurationTimeB_ans:
     cmp bCsv, 1
	 jz DurationTimeSkip
	 push cDurationTimeB_ans
	 jmp ConversationTimeEvalBeg

DurationTimeACM:
	 cmp bCsv, 1
	 jz DurationTimeSkip
	 push cDurationTimeACM
	 jmp ConversationTimeEvalBeg

DurationTimeSkip:
	 add ebx, ecx   // если поле не ConversationTime и CSV формат, пропускаем
	 jmp NextField3
	 
ConversationTime:
	 push cConversationTime
	 
ConversationTimeEvalBeg:
	 inc ebx
	 dec cl       //  уменьшим длину CallDuration на 1
	 mov ch, byte ptr [ebx]  // длина ConversationTime или другого DurationTime
	 //sub cl, ch   //  уменьшим длину CallDuration
	 /*cmp bCsv, 1
     jnz ConversationTimeEval
      // если csv
     push ecx  // сохраним счетчик длины
	 push 2    // перед CallDuration печатаем пустые поля вместо транкгрупп
     call PrintSpace_csv
	 pop ecx
     pop ecx // восстановим счетчик длины      */

ConversationTimeEval:
	 inc ebx
     mov al, byte ptr [ebx]
	 shl eax, 8
	 dec cl   //  уменьшим длину CallDuration
	 dec ch   // уменьшим длину ConversationTime или другого DurationTime
	 jnz ConversationTimeEval
	 
	 pop esi   //   conversationTime или какой-либо DurationTime
	 mov dword ptr [esp], ecx   // обновим в стеке длину CallDuration

	 shr eax, 8  // conversationTime в сотых долях секундах  =107C
     xor edx, edx
	 mov edi, 100
	 div edi  // eax - секунды  =2A=42, edx - сотые доли  =14=20
	 
	 push edx  //  сотые доли секунды
	 //push eax  //  общее число секунд

	 mov edi, 60
     xor edx, edx
	 div edi  // eax - минуты, edx - секунды
	 push edx   // секунды
	 //push eax  // общее число минут

     xor edx, edx
	 div edi  // eax - часы, edx - минуты
	 push edx   // минуты
     push eax   // часы

     cmp bCsv, 1
     jz CallDurationPrint_csv
     
	 push esi   //   conversationTime или какой-либо DurationTime
     push cConversationTimeFrmt
	 //call printf
	 //add esp, 24

	 push outfile
     call fprintf
	 add esp, 28

	 pop ecx   // длина поля CallDuration
	 jmp CallDurationNex
     
CallDurationPrint_csv:
     /*push cConversationTimeFrmt_csv
     call printf
     add esp, 24   */

	 push cConversationTimeFrmt_csv   // часы, минуты, секунды
     push cStr_csv_tmp
     call sprintf  // пишем значение ConversationTime в формате CSV в
	 add esp, 20    //  стоку cStr_csv_tmp

	 push 0         // iOffSet
     push cStr_csv_tmp   // cValue - ConversationTime
     push cConversationTime       //  cField
     push cRecordHeader_s1  //  cHeaderStr
     push cStr_csv         //  cString
     call InsertToCsvString
	 add esp, 20
	 
	 pop ecx
	 pop ecx  // длина поля CallDuration
	 jmp CallDurationNex

NextField3:
     inc ebx
     mov al, byte ptr [ebx]
     cmp al, 0x97
     jz Teleservice
     cmp al, 0x0BE
     jz RecordExtensions

//------------------- поле recordExtensions ------------------
RecordExtensions:
	 inc ebx
     mov cl, byte ptr [ebx]   // длина recordExtensions

RecordExtensionsEval:
	 inc ebx
	 xor eax, eax
	 mov al, byte ptr [ebx]
     cmp al, 0xA5
     jz RecordExtensionsME

     push cRecordExtensionsME
     push eax
	 jmp notField

RecordExtensionsME:
     inc ebx
     mov cl, byte ptr [ebx]  // длина RecordExtensionsME
     
     inc ebx
     mov al, byte ptr [ebx]
     cmp al, 0xA0
     jz CDE

     push cCDE
     push eax
     jmp notField

CDE:
     inc ebx
     mov cl, byte ptr [ebx]
     cmp cl, 3 // длина CDE =3 ?
     jz CDEEval

	 push eax
     push cCDE
     jmp FieldWrongLength

CDEEval:
     inc ebx
     mov al, byte ptr [ebx]
	 cmp al, 0x80
     jz TypeOfTraffic

     push cTypeOfTraffic
     push eax
     jmp notField

TypeOfTraffic:
     inc ebx
     mov cl, byte ptr [ebx]
     cmp cl, 1 // длина TypeOfTraffic = 1 ?
     jz TypeOfTrafficEval

     push eax
     push cTypeOfTraffic
     jmp FieldWrongLength

TypeOfTrafficEval:
     inc ebx
     xor eax, eax
     mov al, byte ptr [ebx]
     test al, al
	 jz Local
     cmp al, 1
     jz Zonal
     cmp al, 2
     jz Interzonal
     cmp al, 3
	 jz International
	 cmp al, 4
	 jz Facility

     push eax
     push cTypeOfTraffic
     push cOther2ValueFieldFrmt
     jmp PrintParseError

Facility:
	 push cFacility
     jmp TypeOfTrafficFacilityPrint

International:
	 push cInternational
	 jmp TariffCodeBeg

Interzonal:
     push cInterzonal
     jmp TariffCodeBeg

Zonal:
     push cZonal
     jmp TariffCodeBeg

Local:
     push cLocal

TariffCodeBeg:
     inc ebx
     mov al, byte ptr [ebx]
     cmp al, 0x82   // tariffCode ?
     jz TariffCode

     push cTariffCode
     push eax
     jmp notField

TariffCode:
     inc ebx
     xor ecx, ecx
     mov cl, byte ptr [ebx]  // длина tariffCode
     
     lea edi, sDigits    //  sDigits - строка tariffCode
     mov edi, dword ptr [edi]

LoadNexTariffCodeChars:
     inc ebx
     mov al, byte ptr [ebx] // загрузим символ строки
     stosb    // сохраняем байт из al в es:[edi]
     loop LoadNexTariffCodeChars
     
     mov byte ptr [edi], 0  // закончим строку ExchangeID нулем

     pop eax  // восстановим значение TypeOfTraffic

     cmp bCsv, 1
     jz TypeOfTrafficPrint_csv
     
     push sDigits   //  строка TariffCode
	 push cTariffCode
	 push eax    // значение TypeOfTraffic
     push cTypeOfTraffic
     push cCDE
     push cRecordExtensionsME
     push cRecordExtensions
     push cRecordExtensionsFrmt
     //call printf
     //add esp, 32

     push outfile
     call fprintf
     add esp, 36
     jmp EndRecord

TypeOfTrafficFacilityPrint:
     pop eax
	 
     cmp bCsv, 1
     jz TypeOfTrafficPrint_csv

     push eax    // значение TypeOfTraffic
     push cTypeOfTraffic
     push cCDE
     push cRecordExtensionsME
     push cRecordExtensions
     push cRecordExtensionsFacilityFrmt
     //call printf
     //add esp, 24

     push outfile
     call fprintf
     add esp, 28
     jmp EndRecord

TypeOfTrafficPrint_csv:
     /*push eax    // значение TypeOfTraffic
     push cCsv_Frmt
     call printf    */

     push eax    // значение TypeOfTraffic
     push cCsv_Frmt
     push cStr_csv_tmp
     call sprintf  // пишем значение TypeOfTraffic в формате CSV в
     add esp, 12    //  строку cStr_csv_tmp

     push 0         // iOffSet
     push cStr_csv_tmp   // cValue - TypeOfTraffic
     push cTypeOfTraffic       //  cField
     push cRecordHeader_s1  //  cHeaderStr
     push cStr_csv         //  cString
     call InsertToCsvString
     add esp, 20

     /*push cStrEnd
     call printf  */

     /*push cStrEnd
     push cStr_csv
     call StrCat   // добавляем к cStr_csv переход на новую строку
     add esp, 8
     //StrCat(char * Dest, const char * Source)
     // concatenate Source to the end of Dest     */
     
     push cStr_csv
     push cStrEndFrmt
	 //call printf   // печатаем полную строку cStr_csv и переход на новую строку
	 //add esp, 8

     push outfile
     call fprintf
     add esp, 12

     jmp EndRecord

//------------------- поле teleservice -----------------------
Teleservice:
     cmp bCsv, 1
     jz Teleservice_csv

     inc ebx
     mov cl, byte ptr [ebx]
     cmp cl, 2
     jz TeleserviceEval

     push eax
     push cTeleservice
     jmp FieldWrongLength
     
TeleserviceEval:

     inc ebx
     xor eax, eax
     mov ax, word ptr [ebx]
     bswap eax
     shr eax, 16
     cmp ax, 1
     jz Telephony
     cmp ax, 4
     jz FacsimileGroup23
     cmp ax, 33
     jz FacsimileGroup4

     push eax

FacsimileGroup4:
     push cFacsimileGroup4
     jmp PrintTeleservice

FacsimileGroup23:
     push cFacsimileGroup23
     jmp PrintTeleservice

Telephony:
     push cTelephony
     jmp PrintTeleservice 

PrintTeleservice:
     push cTeleservice
     push cPrintField2stringFrmt
	 //call printf     // печатаем  Teleservice
	 //add esp, 12

     push outfile
     call fprintf
     add esp, 16
     
     inc ebx
	 jmp CallDurationBeg

Teleservice_csv:
     inc ebx
     inc ebx
     inc ebx
	 jmp CallDurationBeg
     
// ----------------------------------------------------------------------
fieldnotAB:
     push cFieldAB
     push eax
     jmp notField

FieldWrongLength:
     push cFieldWrongLengthFrmt
     jmp PrintParseError

notField:  // в стеке сначала hex-значение байта из файл.буфера,
           // потом указ-ль на строку
	 pop edx  // hex-значение байта из файл.буфера
	 //pop edi  // указ-ль на строку

     // адрес начала найденной записи в find_rec_buf
	 mov esi, dword ptr [find_rec_buf]
	 mov eax, ebx  // адрес текущего байта найденной записи в find_rec_buf
	 sub eax, esi   // номер текущего байта найденной записи в find_rec_buf
	 mov esi, eax

	 pop ebx  // указ-ль на строку
	 
	 push esi      // position number in find_rec_buf
	 mov eax, bufPos // адрес следующего байта после найденной записи в buf
	 sub eax, buf   // номер следующего байта после найденной записи в buf
 	 push eax       // position number of next record in buf
	 sub eax, findRecSize   // номер первого байта найденной записи в buf
	 add eax, esi  // номер текущего байта найденной записи в buf = N_b
	 push eax    // position number in buf
	 sub edi, buf
	 sub edi, 0xB0
	 push edi   // position in buf
	 add eax, iFilePos  // номер текущего байта найденной записи в файле =
	 sub eax, bytes  //  = N_b + (iFilePos - bytes)

	 push eax      // позиция в файле - position in file
	 //push edi  // указ-ль на строку
	 push ebx   // указ-ль на строку
	 push edx   // hex-значение байта из файл.буфера
	 push fieldNot_frmt
PrintParseError:
	 //call printf
	 //add esp, 16

	 push outfile
	 call fprintf
	 add esp, 20
	 //inc ebx
	 //jmp ParseRecord
	 //jmp Exit1
	 jmp EndPushBtn
	 
EndRecord:  // если конец записи
	 inc iFoundRecordsNumber  // счетчик кол-ва записей
	 cmp iSplitPosition, 0
	 jz EndRecordContinue
	 
	 mov eax, iFoundRecordsNumber
	 cmp eax, iSplitPosition
	 jl EndRecordContinue //если счетчик кол-ва записей < числа разделения

	// если счетчик кол-ва записей БОЛЬШЕ или равно числа CSV строк разделения
	 
	 push iOutSplitFileNbr
	 push cEndOfPart
	 push outfile
	 call fprintf
	 add esp, 12

	 push outfile
	 call fclose
	 add esp, 4

	 mov eax, dword ptr [OutFileNameList]
	 mov eax, dword ptr [eax+0x18]
	 mov eax, dword ptr [eax]
	 mov cOutputFileName, eax
	 push cOutputFileName
	 call find_length
	 add esp, 4
	 add eax, cOutputFileName
	 sub eax, 4

	 cmp iOutSplitFileNbr, 1
	 jz ChangeOutFileName

	 sub eax, 2

ChangeOutFileName:
	 inc iOutSplitFileNbr  // число частей разделения  1...9
	 mov ecx, iOutSplitFileNbr
	 or cl, 0x30 
	 mov byte ptr [eax], '_'   // изменим имя вых. файла добавим '_x'
	 inc eax
	 mov byte ptr [eax], cl  //  x - номер части - 1...9
	 inc eax
	 mov byte ptr [eax],'.'
	 inc eax
	 mov byte ptr [eax],'c'
	 inc eax
	 mov byte ptr [eax],'s'
	 inc eax
	 mov byte ptr [eax],'v'
	 inc eax
	 mov byte ptr [eax], 0
     
	 push cWplus
	 push cOutputFileName
	 call fopen
	 mov outfile, eax
	 // outfile = fopen(cOutputFileName, "w+"); // откроем для записи с добавлением
	 mov eax, iSplitPosition
	 add eax, iDeltaSplit
	 mov iSplitPosition, eax

	 push iOutSplitFileNbr    // печатаем номер части в начало файла
	 push cPart             // в начале следующего файла разбиения
	 push outfile
	 call fprintf
	 add esp, 12

	 push cRecordHeader_s2
	 push cRecordHeader_s1
	 push cRecordHearderFrmt  // печатаем заголовок для CSV формата
	 push outfile             // в следующий файл разбиения
     call fprintf
	 add esp, 16

EndRecordContinue:	 
     //mov al, byte ptr [xNsubsc]  //  загрузить снова первый байт xAsubsc
	 mov edi, bufPos   // текущая позиция buf - конец текущей записи +1
	 //dec edi         // edi - конец текущей записи   = pos_buf
     mov ecx, buf
	 add ecx, bytes  // конец буфера buf +1
	 //dec ecx         // ecx - конец буфера buf

	 mov eax, iFileSize
	 sub eax, iFilePos   // FileSize-FilePos
	 cmp eax, 4
	 jle CircleEndPre   // если (FileSize-FilePos)<=4, проверяем, нужно ли
	                   // выходить из цикла
                     // это сделано, т.к. CDR файл имеет 4 байта-нуля на конце

	 cmp edi, ecx    // сравним позицию буфера pos_buf с его концом end_buf
	 jl BeginCircleRecordRead  // если позиция буфера < его конца, читать дальше

     // если позиция больше - сюда должно попасть в конце полного декодирования
	 // файла, а также сюда маловероятно попадёт, когда конец записи
	 //  будет совпадать с концом буфера
	 jmp NexFileRead

CircleEndPre:
	 mov eax, edi   // = pos_buf
	 sub ecx, eax   // разница конца буфера и его позиции (end_buf - pos_buf)
	 cmp ecx, 4 // сравним разницу конца буфера c его позицией с числом 4
	 jg BeginCircleRecordRead // если разница позиции буфера и его конца > 4,
						 // то читаем дальше   

	 //  если разность (end_buf - pos_buf) <= 4
	 jmp CircleEnd

	}   //  конец asm

EndBuf_lseek:   // загрузка буфера, начиная с начала разрезанной записи
  lseek(handle,iFilePos,SEEK_SET); // установка указателя позиции в файле
NexFileRead:                      // на позицию с начала отрезанной записи
  /*mov eax, iFilePos
  cmp eax, iFileSize  // сравним позицию файла с его размером
  jge CircleEnd    // если больше или равно - на конец  */

  // если позиция файла больше или равна его размеру
  if(iFilePos >= iFileSize) goto CircleEnd;  // то выходим из цикла

	 // если меньше, читаем дальше
  if ((bytes = read(handle, buf, bufsize)) == -1) {
   //perror("file next read failed, error");
   EventListBox->Items->Add("не читается дальше файл");
//Exit1:
   //exit(1);
   //goto EndPushBtn;
   continue;
  }
  iFilePos = iFilePos + bytes;
  Progress1->UserValue = iFilePos;
  asm {
	 //mov edi, bufPos
	 /*mov edi, buf // на начало буфера с первой записью, к-рая была отрезана
	 jmp StartRecord  */
	 mov edi, buf   // адрес первого байта buf
	 jmp BeginCircleRecordRead
  }

  //if(iTBCDlen<bufsize) bufsize=iTBCDlen;
  //} // конец цикла по символам внутри буфера
  //}
  //ifpos = ifpos + bytes;  // нахождение следующей позиции в файле
  //lseek(handle,ifpos,SEEK_CUR); // установка указателя позиции в файле

// чтение из файла в буфер, bytes = размер данных, считанных в конце файла
/*NexFileRead:
  if ((bytes = read(handle, buf, bufsize)) == -1) {
   perror("file next read failed, error");
Exit1:
   exit(1);
  }
  iFilePos = iFilePos + bytes;  // текущая позиция в файле
  //if (iFilePos >= statbuf.st_size) {
  if (iFilePos >= iFileSize) {
   break;
  }       */

 //} // конец цикла по кол-ву частей файла

CircleEnd:  // конец цикла по сравнению позиции файла с его размером
 close(handle);
 iFoundRecordsNumber--;

 sFileName = ExtractFileName(FileNameList->Strings[k]);
 cInputFileName = AnsiString(sFileName).c_str();
 
 if(bFilter) {
  if(bFind) {
   fprintf(outfile,
	"\nFile %s : Found %d records. Search completed OK.\n",
	 cInputFileName, iFoundRecordsNumber);
   EventListBox->Items->Add("Файл " + sFileName+
	" : найдено " + IntToStr(iFoundRecordsNumber)+
	" записей. Поиск завершен успешно.");
  }
  else { 
  fprintf(outfile, "\nFile %s : Search completed. No matches found.\n",
   cInputFileName);
  EventListBox->Items->Add("Файл " +sFileName+
   " : Поиск завершен. Совпадений не найдено.");
  }
 }
 else  {
  fprintf(outfile,
   "\nFile %s : Found %d records. Parsing completed OK.\n",
	cInputFileName, iFoundRecordsNumber);
  EventListBox->Items->Add("Файл " + sFileName +
   " : найдено " + IntToStr(iFoundRecordsNumber)+
   " записей. Раскодирование завершено успешно.");
 }

 if(SomeBtn->Checked) fclose(outfile);
}  // конец цикла по кол-ву файлов
//---------------------------------------------------------------------

 if(OneBtn->Checked) fclose(outfile);
//delete FileNameList;

//iFoundRecordsNumber--;

// печатаем время окончания
 /*time (&iTimeValue);
 strftime(cTimebuf, 32, "%a %d %b %Y %H:%M:%S", localtime(&iTimeValue));
 printf("Time is now %s\n", cTimebuf);  */

 DateTimeToString(sFileName,"dd.mm.yy hh:nn:ss,zzz",Now());
 EventListBox->Items->Add("Время окончания "+sFileName);

 //outfile = fopen(cOutputFileName, "w+");

/* if(bFilter) {
  if(bFind) {
   //printf("\nFound %d records. Search completed OK.\n", iFoundRecordsNumber);
   fprintf(outfile,
	"\nFound %d records. Search completed OK.\n", iFoundRecordsNumber);
   EventListBox->Items->Add("Found " + IntToStr(iFoundRecordsNumber)+
   " records. Search completed OK.");
  }
  else { //printf("\nSearch completed. No matches found.\n");
  fprintf(outfile, "\nSearch completed. No matches found.\n");
  EventListBox->Items->Add("Search completed. No matches found.");
  }
 }
 else  {
 //printf("\nFound %d records. Parsing completed OK.\n", iFoundRecordsNumber);
  fprintf(outfile,
   "\nFound %d records. Parsing completed OK.\n", iFoundRecordsNumber);
  EventListBox->Items->Add("Found " + IntToStr(iFoundRecordsNumber)+
  " records. Parsing completed OK.");
 }                    */
 Progress1->UserValue = 0;
 FileNameList->Clear();
 OutFileNameList->Clear();
 iFoundRecordsNumber =1;
 
 EventListBox->Items->Add("-----------------------");
 DecodeBtn->Enabled = true;
} // конец если выделен файл
 //iIndex = FileListBox1->ItemIndex;
 //FileListBox1->Update();
 //FileListBox1->ItemIndex = iIndex;
 //FileListBox1-
 FileListBox2->Update();
 EventListBox->ItemIndex = EventListBox->Items->Count-1;
EndPushBtn:
}


//---------------------------------------------------------------------------

void __fastcall TForm1::FileListBox2DblClick(TObject *Sender)
{
 //HINSTANCE hRet1;
 AnsiString sParam1, sDir1;
 sParam1 =  FileListBox2->FileName;
 sDir1 = FileListBox2->Directory;

 ShellExecute(        // hRet1 = ShellExecute( =2A
		NULL,
		_T("open"),
		_T(sParam1.c_str()),
		"", // параметры
		_T("C:\\"),
		SW_SHOWNORMAL);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::AnyPartCheckBoxChange(TObject *Sender)
{
 if(AnyPartCheckBox->Checked) {
  AnyPartCheckBox->Caption="Поиск: содержит цифры";
  bAnyPartOfNumber = 1;
 }
 else {
  AnyPartCheckBox->Caption="Поиск: начинается на цифры";
  bAnyPartOfNumber = 0;
 }
 FileListBox1Click(Application);
}
//---------------------------------------------------------------------------


void __fastcall TForm1::TransparentCheckBoxChange(TObject *Sender)
{
 if(TransparentCheckBox->Checked) {
  TransparentCheckBox->Caption="Поиск: прозрачный двоичный";
  bTransparent = 1;
 }
 else {
  TransparentCheckBox->Caption="Поиск: от-записи-к-записи";
  bTransparent = 0;
 }
 FileListBox1Click(Application);
}
//---------------------------------------------------------------------------

