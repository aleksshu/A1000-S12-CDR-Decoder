//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "LMDButton.hpp"
#include "LMDCustomButton.hpp"
#include "LMDBaseControl.hpp"
#include "LMDBaseGraphicControl.hpp"
#include "LMDBaseLabel.hpp"
#include "LMDCustomLabel.hpp"
#include "LMDDockButton.hpp"
#include "LMDLabel.hpp"
#include <FileCtrl.hpp>
#include <Dialogs.hpp>
#include "LMDBaseGraphicButton.hpp"
#include "LMDCustomSpeedButton.hpp"
#include "LMDSpeedButton.hpp"
#include "LMDButtonControl.hpp"
#include "LMDControl.hpp"
#include "LMDCustomBevelPanel.hpp"
#include "LMDCustomControl.hpp"
#include "LMDCustomGroupBox.hpp"
#include "LMDCustomPanel.hpp"
#include "LMDCustomParentPanel.hpp"
#include "LMDGroupBox.hpp"
#include "LMDRadioButton.hpp"
#include "LMDBaseEdit.hpp"
#include "LMDCustomEdit.hpp"
#include "LMDEdit.hpp"
#include "LMDAboutDlg.hpp"
#include "LMDBaseDialog.hpp"
#include "LMDContainerComponent.hpp"
#include "LMDCustomComponent.hpp"
#include <Menus.hpp>
#include "LMDCustomScrollBox.hpp"
#include "LMDListBox.hpp"
#include "LMDBaseMeter.hpp"
#include "LMDGraphicControl.hpp"
//#include "LMDObj.hpp"
#include "LMDCustomProgress.hpp"
#include "LMDProgress.hpp"
#include "LMDCheckBox.hpp"
#include "LMDCustomCheckBox.hpp"
#include "LMDCustomExtSpinEdit.hpp"
#include "LMDCustomMaskEdit.hpp"
#include "LMDSpinEdit.hpp"
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TLMDButton *DecodeBtn;
	TFileListBox *FileListBox1;
	TFileListBox *FileListBox2;
	TLMDLabel *LMDLabel1;
	TLMDLabel *LMDLabel2;
	TLMDDockButton *LMDDockButton1;
	TLMDDockButton *LMDDockButton2;
	TOpenDialog *OpenDialog1;
	TLMDGroupBox *LMDGroupBox1;
	TLMDRadioButton *AnySubBtn;
	TLMDRadioButton *ASubBtn;
	TLMDRadioButton *BSubBtn;
	TLMDGroupBox *LMDGroupBox2;
	TLMDRadioButton *CSV_Btn;
	TLMDRadioButton *TXT_Btn;
	TLMDEdit *NumberEdit;
	TLMDAboutDlg *LMDAboutDlg1;
	TMainMenu *MainMenu1;
	TMenuItem *f1;
	TMenuItem *exit1;
	TMenuItem *about1;
	TMenuItem *about2;
	TLMDListBox *EventListBox;
	TLMDGroupBox *LMDGroupBox3;
	TLMDRadioButton *OneBtn;
	TLMDRadioButton *SomeBtn;
	TLMDEdit *OutFileEdit;
	TLMDLabel *LMDLabel3;
	TLMDEdit *BufEdit;
	TLMDLabel *LMDLabel4;
	TLMDProgress *Progress1;
	TLMDCheckBox *FilterCheckBox;
	TLMDSpinEdit *SplitEdit;
	TLMDCheckBox *SplitCheckBox;
	TLMDCheckBox *TransparentCheckBox;
	TLMDCheckBox *AnyPartCheckBox;
    TLMDSpeedButton *LMDSpeedButton1;
    TLMDSpeedButton *LMDSpeedButton2;
	void __fastcall LMDDockButton2Click(TObject *Sender);
	void __fastcall LMDDockButton1Click(TObject *Sender);
	void __fastcall LMDSpeedButton1Click(TObject *Sender);
	void __fastcall LMDSpeedButton2Click(TObject *Sender);
	void __fastcall AnySubBtnClick(TObject *Sender);
	void __fastcall ASubBtnClick(TObject *Sender);
	void __fastcall BSubBtnClick(TObject *Sender);
	void __fastcall about2Click(TObject *Sender);
	void __fastcall exit1Click(TObject *Sender);
	void __fastcall DecodeBtnClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall CSV_BtnClick(TObject *Sender);
	void __fastcall TXT_BtnClick(TObject *Sender);
	void __fastcall FileListBox1Click(TObject *Sender);
	void __fastcall SomeBtnClick(TObject *Sender);
	void __fastcall NumberEditKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall OneBtnMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall FilterCheckBoxMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall SplitCheckBoxClick(TObject *Sender);
	void __fastcall FileListBox2DblClick(TObject *Sender);
	void __fastcall AnyPartCheckBoxChange(TObject *Sender);
	void __fastcall TransparentCheckBoxChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
char *sPLCDigits = "11111111111111111111111111111111";
char *sDigits ="11111111111111111111111111111111";
char *cStr_csv_tmp =
 "1111111111111111111111111111111111111111111111111111111111111111";
char *cStr_csv_tmp2 =
 "1111111111111111111111111111111111111111111111111111111111111111\
1111111111111111111111111111111111111111111111111111111111111111\
1111111111111111111111111111111111111111111111111111111111111111\
1111111111111111111111111111111111111111111111111111111111111111\
1111111111111111111111111111111111111111111111111111111111111111\
1111111111111111111111111111111111111111111111111111111111111111\
1111111111111111111111111111111111111111111111111111111111111111\
1111111111111111111111111111111111111111111111111111111111111111";
char *cStr_csv =
 "1111111111111111111111111111111111111111111111111111111111111111\
1111111111111111111111111111111111111111111111111111111111111111\
1111111111111111111111111111111111111111111111111111111111111111\
1111111111111111111111111111111111111111111111111111111111111111\
1111111111111111111111111111111111111111111111111111111111111111\
1111111111111111111111111111111111111111111111111111111111111111\
1111111111111111111111111111111111111111111111111111111111111111\
1111111111111111111111111111111111111111111111111111111111111111";
     //";;;;;;;;;;;;;;;;;;;;;;;";

 const char *sOutputResult = "\nOutput result:\n",
 *cRecordContent = "RecordContent",
 *cRecordContentFrmt = "\n%s #%d:\n",
 *cRecordHeader_s1 =
 "RecordType;AnswerTime;;SeizureTime;;PartialTime;;EventTime;;CallingNumber;CalledNumber;RedirectingNumber;OriginalCalledNumber;BearerCapability;CallIdentificationNumber;Cause;ExchangeID;PhysicalLineCode;ReceivedDigits;;CallingPartyCategory;CallingPartyType;TrunkGroupOutgoing;TrunkGroupIncoming;ConversationTime;TypeOfTraffic;",
 *cRecordHeader_s2 =
 ";date;time;date;time;date;time;date;time;adress digits;adress digits;adress digits;adress digits;;;CauseValue;ExchangeInfo;;Nature of Addr.;adress digits;;;TrunkGroupId;TrunkGroupId;;;",
 *cRecordHearderFrmt = "%s\n%s\n",
 *cCsv_Frmt = "=\"%s\"",
 *cCsv_Frmt_hex = "=\"0x%X\"",
 *cCsv2ValFrmt = "=\"%s\";=\"%s\"",
 *cCsv3ValFrmt = "=\"%s\";=\"%s\";=\"%s\"",
 *cCsv_Space = "=\"\";",
 *cStrEndFrmt = "%s\n",
 *cConversationTimeFrmt_csv = "=\"%.2d:%.2d:%.2d\";";
 
 const char *cRecordType = "RecordType",
 *cAnswerTime = "AnswerTime",
 *cSeizureTime = "SeizureTime",
 *cPartialTime = "PartialTime",
 *cCallingNumber = "CallingNumber",
 *cCalledNumber = "CalledNumber",
 *cRedirectingNumber = "RedirectingNumber",
 *cOriginalCalledNumber = "OriginalCalledNumber",
 *cBearerCapability = "BearerCapability",
 *cExchangeID = "ExchangeID",
 *cPhysicalLineCode = "PhysicalLineCode",
 *cReceivedDigits = "ReceivedDigits",
 *cCallingPartyCategory = "CallingPartyCategory",
 *cCallingPartyType = "CallingPartyType",
 *cTrunkGroupOutgoing = "TrunkGroupOutgoing",
 *cTrunkGroupIncoming = "TrunkGroupIncoming",
 *cConversationTime = "ConversationTime",
 *cTypeOfTraffic = "TypeOfTraffic";

 const char *cPrintCauseFieldFrmt =
 "  %s:\n   %s : 0x%X -> %s\n   %s : 0x%X -> %s\n";
 const char *cPrintField3stringFrmt = "  %s:\n   %s -> %s\n";
 const char *cPrintField2stringFrmt = "  %s ->  %s\n";
 const char *cPrintField2string1HexFrmt = "  %s : 0x%X ->  %s\n";
 const char *cPrintStringValueFrmt = "  %s ->  %d\n";
 const char *cPrint2StringValueFrmt = "  %s:\n   %s -> %d\n";
 const char *cWrong2ValueFieldFrmt = "Wrong value of %s: 0x%.2X\n";
 const char *cOther2ValueFieldFrmt = "Unknown value of %s: 0x%.2X\n";
 const char *fieldNot_frmt =
 "Parse error: field \"0x%.2X\" is not \"%s\", position in file -> %d,\n\
position in buf -> %d, position number in buf -> %d,\n\
position nuber of next record in buf -> %d,\n\
position number in find_rec_buf -> %d";
 const char *cFieldWrongLengthFrmt = "%s: field wrong length: 0x%.2X\n";
 const char *cFieldAB = "Field of A- or B-type",
 *cNullStr = "";

 const char *cSearchErr =
 "Search error: Input value found, but number fields not found,\
  because begin of buffer reached";
 const char *cCDRHeaderErrorFrmt = "CDR header error: wrong %s\n";
 const char *cLastCDRerror = "\nLast CDR-record not complete\n",
 *cWplus = "w+",
 *cPart = "Part %d\n",
 *cEndOfPart = "End of part %d";

 const char *cCDR_File = "CDR-File",
 *cHeader = "header",
 *cProductionDateTime = "ProductionDateTime",
 *cSoftwareVersion = "SoftwareVersion",
 *cFileName = "FileName",
 *cpString = "pString",
 *cReasonForOutput = "ReasonForOutput",
 *cAbsoluteTimeEvent = "absoluteTimeEvent",
 *cMaxBlockSizeReached = "maxBlockSizeReached",
 *cMaxTimeIntervalElapsed = "maxTimeIntervalElapsed",
 *cIternalSizeLimitReached = "internalSizeLimitReached",
 *coSAction = "oSAction",
 *cBody = "Body :\n",
 *cProductionDateTimeFormat =
 "  %s:\n       date -> %s\n       time -> %s\n",
 *cCDRHeaderFrmt = "%s\n %s:\n",
 *cExchangeInfoCDR_Frmt = " %s:\n  %s -> %s\n",
 *cSoftwareVersionFrmt = "  %s -> %s\n",
 *cReasonForOutputFrmt = "  %s : %d -> %s\n";



 const char *cAdditionalRecordTypes_ME = "AdditionalRecordTypes-ME",
 *cdB_Subscriber_RecordType = "dB-Subscriber-RecordType",
 *cdB_Trunk_RecordType = "dB_Trunk_RecordType",
 *cdB_SS_RecordType = "dB-SS-RecordType";
 const char *cCall = "call";
 const char *cSupplServiceInputRecord = "supplServiceInputRecord\n";

 const char *cStartTimeStamp = "StartTimeStamp",
 *cEventTime = "EventTime", 
 *sStartTimeFormat = "  %s -> %s:\n       date -> %s\n       time -> %s\n";


 const char *cParticipantInfo = "  Participant Info:\n";
 const char *cParticipantInfoFrmt =
 "   %s:\n       %s -> %s\n       %s -> %s\n       %s -> %s\n       %s -> %s\n";

 const char *cCallingPartyNumber = "callingPartyNumber",
 *cCalledPartyNumber = "calledPartyNumber",
 *cRedirectionNumber = "redirectionNumber",
 *cCallingPartyNumberNotScreened = "callingPartyNumberNotScreened",
 *cOperatorSpecific1Number = "operatorSpecific1Number",
 *cOperatorSpecific2Number = "operatorSpecific2Number",
 *cOperatorSpecific3Number = "operatorSpecific3Number",
 *cParticipantId  = "ParticipantId";

 const char *cInsertErrorFrmt = "Error: no match found in header to insert \"%s\"";

 const char *cOddEvenNumbInd = "Odd/even number indicator";
 const char *cEven = "even";
 const char *cOdd = "odd";
 const char *cNatOfAddr = "Nature of Address";
 const char *cNational = "national numb.";
 const char *cSubscriber = "subscriber number";
 const char *cUnknown = "unknown";
 const char *cInternationalNumb = "international number";
 const char *cNumbPlanInd = "numbering plan ind.";
 const char *cE164 = "ISDN (Telephony) Number Plan E.164";
 const char *cSpare = "spare";
 const char *cX121 = "data numbering plan X.121";
 const char *cF69 = "telex numbering plan F.69";
 const char *cReservedNat = "reserved for national use";
 const char *cAdressdigits = "adress digits";

 const char *cBearerService = "Bearer Service";
 const char *cCapability = "capability";
 const char *cWrongBSCapabilityFrmt = "Wrong value of %s %s: %d";
 const char *cAudio31 = "audio3.1kHZ";
 const char *cSpeech = "speech";
 const char *cUni64 = "uni64";
 const char *cUni64withTA = "uni64withT-A";
 const char *cMultipleRate = "multipleRate";
 const char *cPacketModeBCh = "packetModeB-Ch";
 

 const char *cServiceUser = "Service User";

 
 const char *cOperator = "operator"; 

 const char *cSubscrOrTrunkRecordType = "Subscr Or Trunk RecordType";
 
 const char *cCallIdentificationNumber = "CallIdentificationNumber";
 const char *cValueIsHexFrmt = "  %s -> 0x%X\n";

 const char *cCause = "Cause";
 const char *cCauseValue = "causeValue",
 *cDummy = "Dummy",
 *cUnallocatedNumber = "Unallocated (unassigned) number",
 *cNoRouteToSpecified = "No route to specified transit network (national use)",
 *cNoRouteToDestination = "No route to destination",
 *cSendSpecialInformationTone = "send special information tone",
 *cMisdialedTrunkPrefix = "misdialed trunk prefix (national use)",
 *cChannelUnacceptable = "channel unacceptable",
 *cCallAwardedBeingDelivered = "call awarded. being delivered in an established channel",
 *cPreemption = "preemption",
 *cPreemptionCircuitReserved = "preemption - circuit reserved for reuse",
 *cReset_circuit_proc = "reset_circuit_proc",
 *cTcc06_tim_exp = "tcc06_tim_exp",
 *cNormalCallClearing = "normal call clearing",
 *cUserBusy = "user busy",
 *cNoUserResponding = "no user responding",
 *cNoAnswerFromUser = "no answer from user (user alerted)",
 *cSubscriberAbsent = "cSubscriberAbsent",
 *cCallRejected = "call rejected",
 *cNumberChanged = "number changed",
 *cExch_routing_error = "exch_routing_error",
 *cNonSelectedUserClearing = "non-selected user clearing",
 *cDestinationOutOfOrder = "destination out of order",
 *cInvalidNumberFormatAddrIncomplete = "invalid number format (address incomplete)",
 *cFacilitiesRejected = "facilities rejected",
 *cResponseToSTATUSINQUIRY = "response to STATUS INQUIRY",
 *cNormalUnspecified = "normal. unspecified",
 *cNoCircuitChannelAvailable = "no circuit/channel available",
 *cCallQueued = "Call Queued",
 *cNetworkOutOfOrder = "network out of order",
 *cTemporaryFailure = "temporary failure",
 *cPermanentFrameModeConnectionOutOfService = "permanent frame mode connection out-of-service",
 *cPermanentFrameModeConnectionOperational = "permanent frame mode connection operational",
 *cSwitchingEquipmentCongestion = "switching equipment congestion",
 *cAccessInformationDiscarded = "access information discarded",
 *cRequestedCircuitChannelNotAvailable = "requested circuit/channel not available",
 *cPrecedenceCallBlocked = "precedence call blocked",
 *cResourceUnavailableUnspecified = "resource unavailable, unspecified",
 *cQualityOfServiceNotAvailable = "Quality of Service not available",
 *cRequestedFacilityNotSubscribed = "requested facility not subscribed",
 *cOutgoingCallsBarred = "outgoing calls barred",
 *cOutgoingCallsBarredWithinCUG = "outgoing calls barred within CUG",
 *cIncomingCallsBarred = "incoming calls barred",
 *cIncomingCallsBarredWithinCUG = "incoming calls barred within CUG",
 *cBearerCapabilityNotPresentlyAvailable = "bearer capability not presently available",
 *cBearerCapabilityNotAuthorized = "bearer capability not authorized",
 *cInconsistencyInOutgoingInformationElement = "inconsistency in outgoing information element",
 *cServiceOrOptionNotAvailableUnspecified = "service or option not available. unspecified",
 *cBearerCapabilityNotImplemented = "bearer capability not implemented",
 *cChannelTypeNotImplemented = "channel type not implemented",
 *cRequestedFacilityNotImplemented = "requested facility not implemented",
 *cOnlyRestrictedDigitalInformationBearerCapabilityIsAvailable = "only restricted digital information bearer capability is available (national use)",
 *cServiceOrOptionNotImplementedUnspecified = "service or option not implemented unspecified",
 *cInvalidCallReferenceValue = "invalid call reference value",
 *cIdentifiedChannelDoesNotExist = "identified channel does not exist",
 *cSuspendedCallExistsButThisCallIdentifyDoesNot = "a suspended call exists, but this call identify does not",
 *cCallIdentityInUse = "call identity in use",
 *cNoCallSuspended = "no call suspended",
 *cCallHavingTheRequestedCallIdentityHasBeenCleared = "call having the requested call identity has been cleared",
 *cUserNotMemberOfCUG = "user not a member of CUG",
 *cIncompatibleDestination = "incompatible destination",
 *cNonExistentCUG = "non-existent CUG",
 *cInvalidTransitNetworkSelection = "invalid transit network selection (national use)",
 *cInvalidMessageUnspecified = "invalid message, unspecified",
 *cMandatoryInformationElementIsMissing = "mandatory information element is missing",
 *cMessageTypeNonExistentOrNotImplemented = "message type non-existent or not implemented",
 *cMessageNotCompatibleWithCallStateOrMessageTypeNonExistentOrNotImplemented = "message not compatible with call state or message type non-existent or not implemented",
 *cInformationElementNotImplemented = "Information element / parameter non-existent or not implemented",
 *cInvalidInformationElementContents = "Invalid information element contents",
 *cMessageNotCompatibleWithCallState = "message not compatible with call state",
 *cRecoveryOnTimerExpiry = "recovery on timer expiry",
 *cParameterNonExistentOrNotImplementedPassedOn = "parameter non-existent or not implemented - passed on (national use)",
 *cMessageWithUnrecognizedParameterDiscarded = "message with unrecognized parameter discarded",
 *cProtocolErrorUnspecified = "protocol error, unspecified",
 *cCw_dlx_rejected = "Cw_dlx_rejected",
 *cCall_fac_spec_tone = "Call_fac_spec_tone",
 *cCall_fac_rejected = "Call_fac_rejected",
 *cOutg_call_screening = "Outg_call_screening",
 *cIntcpt_prio_rls = "Intcpt_prio_rls",
 *cItcp_allocation_fail = "Itcp_allocation_fail",
 *cInterWorkingUnspecified = "Interworking, unspecified",
 *cCauseValuesOf128AndHigher = "Cause code values of 128 and higher aren't sent over the network and aren't defined in Rec. [Q.850]";

 const char *cLocation = "location";
 const char *cUser = "user",
 *cLocalUserPrivateNetwork = "localUserPrivateNetwork",
 *cLocalUserPublicNetwork = "localUserPublicNetwork",
 *cTransitNetwork = "transitNetwork",
 *cRemoteUserPublicNetwork = "remoteUserPublicNetwork",
 *cRemoteUsePrivateNetwork = "remoteUsePrivateNetwork",
 *cInternationalNetwork = "internationalNetwork",
 *cBeyondInterworkPoint = "beyondInterworkPoint";

 const char *cExchangeInfo = "Exchange Info";

 const char *cAdditionalParticipantInfo = "Additional Participant Info";

 const char *cAdditionalParticipantInfoFrmt =
  "  %s:\n   %s -> %s\n   %s:\n       %s -> %s\n       %s -> %s\n       %s -> %s\n",
 *cAdditionalParticipantInfoWoDigitsFrmt = "  %s:\n   %s -> %s\n";	

 const char *cCPCOperatorFrench = "operator, language French",
 *cCPCOperatorEnglish = "operator, language English",
 *cCPCOperatorGerman = "operator, language German",
 *cCPCOperatorRussian = "operator, language Russian",
 *cCPCOperatorSpanish = "operator, language Spanish",
 *cCPCOperatorAdminSelect = "(available to Administrations for \
selection a particular language \
by mutual agreement)",
 *cCPCReservedNationalOperator = "reserved (In national networks,\
code 00001001 may be used to indicate that the calling party is a \
national operator)",
 *cCPCOrdinarySubscriber = "ordinary calling subscriber (CAT_1)",
 *cCPCSubscriberPriority  = "calling subscriber with priority",
 *cCPCDataCall = "data call (voice band data)",
 *cCPCTestCall = "test call",
 *cCPCPayphone = "payphone (CAT_6)",
 *cCPCHotel_Subsc_Internat = "hotel subscriber international (CAT_2)",
 *cCPCUnknown = "unknown category (CAT_0)",
 *cOrdinarySubscInternat = "ordinary subscriber international (ORDINARY_SUBSC_INTERNAT)",
 *cSUBSCR_RESTR_SERVICE = "SUBSCR_RESTR_SERVICE",
 *cNOT_DEFINED = "NOT_DEFINED",
 *cINTERNAT_OPERATOR = "INTERNAT_OPERATOR",
 *cDATA_CALL = "DATA_CALL",
 *cTEST_CALL = "TEST_CALL",
 *cPUBLIC_PAYPHONE = "PUBLIC_PAYPHONE",
 *cSYSTEM_CALL = "SYSTEM_CALL",
 *c8_NOTDEFINED_BY_SPEC = "8_NOTDEFINED_BY_SPEC",
 *c9_NOTDEFINED_BY_SPEC = "9_NOTDEFINED_BY_SPEC",
 *c10_NOTDEFINED_BY_SPEC = "10_NOTDEFINED_BY_SPEC",
 *cNATIONAL_OPERATOR = "NATIONAL_OPERATOR",
 *cNON_PUBLIC_PAYPHONE = "NON_PUBLIC_PAYPHONE",
 *cBusiness_Subscriber = "Business_Subscriber",
 *cATTENDANT = "ATTENDANT",
 *cBusiness_Subscriber_Prior = "Business_Subscriber_Prior",
 *cTELEACTION = "TELEACTION",
 *cTRANSMISSION_TEST_POS = "TRANSMISSION_TEST_POS",
 *cDISPATCHER_OF_PCN = "DISPATCHER_OF_PCN",
 *cEDSS1_SUBSCRIBER = "EDSS1_SUBSCRIBER",
 *cMOBILE_SUBSCRIBER = "MOBILE_SUBSCRIBER",
 *cTELETEX_CUSTOMER = "TELETEX_CUSTOMER",
 *cTRUNK_OFF = "TRUNK_OFF",
 *cCOINAFON = "COINAFON",
 *cNRT80_OR_NRT80P = "NRT80_OR_NRT80P",
 *cIKZ50_INVOLVED = "IKZ50_INVOLVED",
 *cLINE_TEST_DESK = "LINE_TEST_DESK",
 *c1TR6_SUBSCRIBER = "1TR6_SUBSCRIBER",
 *cPABX = "PABX",
 *cMT_OPERATOR = "MT_OPERATOR",
 *cCALL_FORWARDED_TO_FEAFD = "CALL_FORWARDED_TO_FEAFD",
 *cHOTEL_SUBS = "HOTEL_SUBS",
 *cHOTEL_PRIV_METER = "HOTEL_PRIV_METER",
 *cSUBS_CWC = "SUBS_CWC",
 *cSUBS_BBG = "SUBS_BBG",
 *cTRANSIT_GATES = "TRANSIT_GATES",
 *cTESTCALL_TO_SUBSCRIBER = "TESTCALL_TO_SUBSCRIBER",
 *cOPER_FORW_TRANSF_INTNAT = "OPER_FORW_TRANSF_INTNAT",
 *cSPECIAL_SUBSCRIBER = "SPECIAL_SUBSCRIBER",
 *cSUBS_SPEC_PRIORITY = "SUBS_SPEC_PRIORITY",
 *cRESTRICT_PAYPHONE = "RESTRICT_PAYPHONE",
//44=PAYPHONE
 *cOPERATOR_INTERCEPTION = "OPERATOR_INTERCEPTION",
//46=HOTEL_SUBSC_INTERNAT
 *cSUBSC_NO_RESTRICTION = "SUBSC_NO_RESTRICTION",
//48=UNKNOWN
 *cINM_CHARGE_SUBSCRIBER = "INM_CHARGE_SUBSCRIBER",
 *cPAYPHONE_LOC_STD = "PAYPHONE_LOC_STD",
 *cPAYPHONE_LOCAL = "PAYPHONE_LOCAL",
 *cTRUNK_TEST = "TRUNK_TEST",
 *cMESSSYSTEM = "MESSSYSTEM",
 *cSERVICE_ACCESS = "SERVICE_ACCESS",
 *cOPERATOR_FORWARD_TRANSF = "OPERATOR_FORWARD_TRANSF",
 *cOPERATOR = "OPERATOR",
 *cSUBSCRIBER_PRICE_REQ = "SUBSCRIBER_PRICE_REQ",
//201=ORDINARY_SUBSC_INTERNAT
 *cDATA_CALL_INTERNAT = "DATA_CALL_INTERNAT",
 *cPRIORITY_SUBSC_INTERNAT = "PRIORITY_SUBSC_INTERNAT",
//204=SUBSCR_RESTR_SERVICE
 *cRESTRICTED_PAYPHONE = "RESTRICTED_PAYPHONE",
 *cCCR_SUBSCRIBER = "CCR_SUBSCRIBER",
 *cCALL_DIVERSION = "CALL_DIVERSION",
 *cORDINARY_SUBSCR_WITH_MM = "ORDINARY_SUBSCR_WITH_MM";

 const char *cAnalogue = "analogue",
 *cCustomerLink = "customerLink",
 *cBasicAccess = "basicAccess",
 *cPrimaryRateAccess = "primaryRateAccess";

 const char *cChargingInformation = "ChargingInformation",
 *cRecordedUnitsList = "recordedUnitsList",
 *cRecordedUnits = "recordedUnits",
 *cFreeOfCharge = "freeOfCharge",
 *cUnits = "units",
 *cUnitsNotAvailable = "notAvailable",
 *cRecordedNumberOfUnits = "recordedNumberOfUnits",
 *cRecordedTypeOfUnits = "recordedTypeOfUnits",
 *cRecordedTypeOfUnitsFrmt = "     %s -> 0x%X\n",
 *cChargingInformationFrmt =
 "  %s:\n   %s:\n    %s:\n     %s:\n      %s -> %d\n",
 *cChargingInformationUnitsNotAvailableFrmt =
 "  %s:\n   %s:\n    %s:\n     %s:\n      %s",
 *cChargingInformationFreeOfChargeFrmt = "  %s:\n   %s\n";

 const char *cTrunkGroupId = "trunkGroupId";

 const char *cCallDuration = "callDuration",
 *cDurationTimeACM = "DurationTimeACM",
 *cDurationTimeB_ans = "durationTimeB-ans",
 *cDurationTimeNoANM = "durationTimeNoANM",
 *cCallDurationFrmt = "  %s:\n",
 *cConversationTimeFrmt = "   %s -> %.2d:%.2d.%.2d,%.2d\n";

 const char *cSupplementaryServices = "SupplementaryServices",
 *cSupplementaryService = "supplementaryService",
 *cSupplementaryServiceCode = "supplementaryServiceCode",
 *cSupplementaryAction = "supplementaryServiceAction",

 *cProvision = "provision",
 *cWithdrawal = "withdrawal",
 *cRegistration = "registration",
 *cErasure = "erasure",
 *cActivation = "activation",
 *cDeactivation = "deactivation",
 *cInvocation = "invocation",
 *cDisabling = "disabling",
 *cInterrogation = "interrogation",

 *cSupplementaryServiceFrmt =
 "    %s : 0x%X -> %s\n    %s : 0x%X -> %s\n",
 *cSupplementaryServicesFrmt ="  %s:\n   %s:\n",
 *cSSC_1 = "Abbreviated Address",
 *cSSC_4 = "Alarm Call",
 *cSSC_12 = "AOC During the call",
 *cSSC_13 = "AOC at End of the call",
 *cSSC_36 = "Call completion to busy subscriber",
 *cSSC_37 = "Call Completion on no reply",
 *cSSC_41 = "Call deflection",
 *cSSC_48 = "Call forward on busy",
 *cSSC_50 = "Call forward on busy to voice mail",
 *cSSC_51 = "Call forward to fixed announcement",
 *cSSC_52 = "Call forward on no reply",
 *cSSC_55 = "Call forward on no reply to voice mail",
 *cSSC_58 = "Call forward unconditional",
 *cSSC_60 = "Call forward to voice mail",
 *cSSC_67 = "Calling line identification restriction",
 *cSSC_70 = "Connected line identification restriction",
 *cSSC_71 = "Add-on Conference (bridge)",
 *cSSC_82 = "Closed user group",
 *cSSC_84 = "Call waiting",
 *cSSC_86 = "Data Protection",
 *cSSC_98 = "Do not disturb",
 *cSSC_103 = "Fixed Destination call (Immediate/Timeout)",
 //*cSSC_104 = "Fixed Destination call (Immediate/Timeout)",
 *cSSC_113 = "Incoming Call Barring",
 *cSSC_123 = "Hunting",
 *cSSC_139 = "Multiple Subscriber Number",
 *cSSC_148 = "Outgoing Call Barring",
 *cSSC_153 = "Priority",
 *cSSC_197 = "Three Party Service",
 *cSSC_206 = "User to User Signalling 3",
 *cSSC_1001 = "Call forward unconditional to operator",
 *cSSC_1003 = "General deactivation",
 *cSSC_1004 = "Personal Identification Number";

 /* *cSSC_00 = "All supplementary services",
 *cSSC_10 = "All line identification services",
 *cSSC_11 = "Calling number identification presentation",
 *cSSC_12 = "Calling number identification restriction",
 *cSSC_13 = "Connected number identification presentation",
 *cSSC_14 = "Connected number identification restriction",
 *cSSC_20 = "All call forwarding",
 *cSSC_21 = "Call forwarding unconditional",
 *cSSC_28 = "All conditional Call Forwarding",
 *cSSC_29 = "Call forwarding on mobile subscriber busy",
 *cSSC_2A = "Call forwarding on no reply",
 *cSSC_2B = "Call forwarding on subscriber not reachable",
 *cSSC_30 = "All call offering services",
 *cSSC_31 = "Call transfer",
 *cSSC_40 = "All call completion services",
 *cSSC_41 = "Call waiting",
 *cSSC_42 = "Call hold",
 *cSSC_43 = "Completion of calls to busy subscribers",
 *cSSC_50 = "All multi party services",
 *cSSC_51 = "Multi party service",
 *cSSC_60 = "All community of interest services",
 *cSSC_61 = "Closed user groups",
 *cSSC_70 = "All charging supplement services",
 *cSSC_71 = "Advice of charge (charging)",
 *cSSC_72 = "Advice of charge (information)",
 *cSSC_80 = "All additional info transfer services",
 *cSSC_81 = "User to user signalling",
 *cSSC_90 = "All call barring",
 *cSSC_91 = "All Barring of outgoing Call Services",
 *cSSC_92 = "Barring of all outgoing calls",

 *cSSC_93 = "Barring of all outgoing international calls",
 *cSSC_94 = "Outgoing Call Barring",
 //*cSSC_94 = "Barring of all OG international except HPMN",
 
 *cSSC_99 = "All Barring of incoming Call Services",
 *cSSC_9A = "Barring of all incoming calls",
 *cSSC_9B = "Barring of all IC calls when outside HPMN",
 *cSSC_FF = "USSD Message";  */

 const char *cTeleservice = "teleservice",
 *cTelephony = "Telephony",
 *cFacsimileGroup23 = "Facsimile Group 2/3 (Recommendation F.182 [68])",
 *cFacsimileGroup4 = "Facsimile Group 4 Class I (Recommendation F.184 [69])",
 *cTeletexBasicMixedFacsimileGrp4Class23 = "Teletex service, basic and mixed mode of operation (Recommendation F.230 [70]) and facsimile service Group 4, Classes II and III (Recommendation F.184)",
 *cTeletexBasicProcessable = "Teletex service, basic and processable mode of operation (Recommendation F.220 [71])",
 *ccTeletexBasic = "Teletex service, basic mode of operation (Recommendation F.200 [72])",
 *cSyntaxBasedVideotex =  "Syntax based Videotex (Recommendations F.300 [73] and T.102 [74])",
 *cInternationalVideotex = "International Videotex interworking via gateways or interworking units (Recommendations F.300 and T.101 [75])",
 *cTelex = "Telex service (Recommendation F.60 [76])",
 *cMessageHandlingSystems = "Message Handling Systems (MHS) (X.400 - Series Recommendations [77])",
 *cOSIapplication = "OSI application (Note 2) (X.200 - Series Recommendations [78])",
 *cReservedMaintenance = "Reserved for maintenance",
 *cReservedManagement = "Reserved for management",
 *cAudioVisual = "Audio visual (Recommendation F.721 [79])",
 *cReservedAudiovisual = "Reserved for audiovisual services (F.700 - Recommandations Series [80])",
 *cReserved = "Reserved";

 const char *cRecordExtensions = "recordExtensions",
 *cRecordExtensionsME = "recordExtensions-ME",
 *cCDE = "cDE",
 *cLocal = "local",
 *cZonal = "Zonal",
 *cInterzonal = "interzonal",
 *cInternational = "international",
 *cFacility = "facility",
 *cTariffCode = "tariffCode",
 *cRecordExtensionsFrmt =
 "  %s:\n   %s:\n    %s:\n    %s -> %s\n    %s -> %s\n",
  *cRecordExtensionsFacilityFrmt =
 "  %s:\n   %s:\n    %s:\n    %s -> %s\n";

 const char *cPartialGeneration = "PartialGeneration",
 *cPartialRecordNumber = "partialRecordNumber",
 *cPartialRecordReason = "partialRecordReason",
 *cTimeLimit = "timeLimit",
 *cServiceChange = "serviceChange",
 *cOverflow = "overflow",
 *cNetworkInternalReasons = "networkInternalReasons",
 *cLastCDR = "lastCDR",
 *cTimeChange = "timeChange",
 *cPartialGenerationFrmt =
 "  %s:\n   %s -> 0x%X\n   %s : 0x%X -> %s\n";


#endif
