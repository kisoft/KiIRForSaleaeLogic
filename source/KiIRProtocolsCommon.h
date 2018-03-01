#pragma once

#ifndef __KIIRPROTOCOLSCOMMON_H
#define __KIIRPROTOCOLSCOMMON_H

#include "Analyzer.h"

/// <summary>
/// ���������� ���������� �����
/// </summary>
#define FLOAT_BITS_COUNT	(-1)
#define	ALLPROTOCOLS		(-1)

typedef std::vector<U64> VectorData;

/// <summary>
/// ������������ ���� ��������� ����������. peMAXID - �������������� �����������
/// </summary>
typedef enum _ProtocolEnum
{
	peNEC = 0,
	peSIRC,
	peSHARP,
	peJVC,
	peRC5,
	peRC5X,
	peRCA,
	peNRC17,
	peSAMSUNG32,
	peSAMSUNG48,
	pePANASONIC,
	pePENTAX,
	peKATHREIN,
	peLEGO,
	peTHOMSON,
	peTELEFUNKEN,
	peRCCAR,
	peNUBERT,
	peFAN,
	peSPEAKER,
	peROOMBA,
	peS100,
	peNIKON,
	peDAIKIN,

	peMAXID = ALLPROTOCOLS
} ProtocolEnum;

/// <summary>
/// ����������� �����, �.�. ����� ���� ���� � ������ ������
/// </summary>
typedef enum BitsDirection
{
	bdLSB = 0,
	bdMSB,
	bdUNKNOWN = bdMSB
};

/* ��������� ������ */
#define bcMARK BIT_LOW
#define bcSPACE BIT_HIGH

/// <summary>
/// ��������� ������ ��� ����������� ������ ����. � us
/// </summary>
typedef struct _BitTimes
{
	U16				firstPeriodUs;
	U16				secondPeriodUs;
} BitTimes;

/// <summary>
/// ���������� �� ����������� ������ ����. ��� ����� ���� ��� ���������� ���� ������, ��� � Lead/Trail ����
/// </summary>
typedef struct _LogicalBit
{
	union {
		U8				bitValue;					// �������� ���� (0 ��� 1). ��� Lead - ���������� �����
		U8				bitCount;					// ���������� �����. ������������ ��� ������ Leads
	};
	BitState		activeLevel;				// �������� ������� ��� ���� (MARK or SPACE)
	BitTimes		timesInfo;					// ��������� ��������� ����������� ����

	_LogicalBit(
		U8 pBbitValue = 0
		, const BitState &pActiveLevel = BitState()
		, const BitTimes &pTimesInfo = BitTimes()
		)
		: bitValue(pBbitValue)
		, activeLevel(pActiveLevel)
		, timesInfo(pTimesInfo)
	{}

} LogicalBit;

/// <summary>
/// ���� ����� ��������
/// </summary>
typedef enum _RepeatType
{
	rtNormal = 0,								// ������ ����������� ��� � ���� ��� ����� �������������
	rtSpecialLead,								// ��� ������� ������������ ����������� ������������������, ��������, ����������� Lead
	rtCodeWithoutLead							// ����������� ��� ��� Lead
} RepeatType;

/// <summary>
/// ��� ��������� �������. ������ ������� (ripPeriod) ��� ������������ ����� ����� �������� (ritGap)
/// </summary>
typedef enum _RepeatIntervalType
{
	ritPeriod = 0,
	ritGap
} RepeatIntervalType;

typedef struct _RepeatInfo
{
	RepeatType			repeatType;				// ��� �������
	RepeatIntervalType	repeatIntevalType;		// ��� ����� ��� �������: ������ ������� ��� ����� ����� ��������
	U32					repeatTime;				// ���������� ������ �������
} RepeatInfo;

typedef enum _ProtocolTypeEnum
{
	ptPulseDistance = 0,
	ptPulseWidth,
	ptPulseDistanceWidth,
	ptBiPhase,
	ptPulsePosition,
	ptMaxProtocolType
} ProtocolTypeEnum;

typedef struct _ProtocolType
{
	static std::string	getName(ProtocolTypeEnum pProtocolType);
} ProtocolType;

typedef struct _SynchroInfo
{
	U32				infoBitsCount;				// ���������� ��� � ������ ������������������
	LogicalBit		bitInfo;					// �������� ���� ������ ������������������
	bool			needFinalBit;				// ������� ���������� ���� ������������������
	U64				pauseLengthUs;				// ������������ ����� ����� ������ ������������������ (������������ � us)

	_SynchroInfo(
		U32 pInfoBitsCount = 0
		, const LogicalBit &pBitInfo = _LogicalBit()
		, bool pNeedFinalBit = false
		, const LogicalBit &pBitFinal = _LogicalBit()
		, U64 pPauseLengthUs = 0
		)
		: infoBitsCount(pInfoBitsCount)
		, bitInfo(pBitInfo)
		, needFinalBit(pNeedFinalBit)
		, pauseLengthUs(pPauseLengthUs)
	{}
} SynchroInfo;

typedef struct _FrameInfo
{
	U8				frameIndex;					// ����� ������ (0..n)
	U8				bitsCount;					// ���������� ���
	U8				bitsCountForGenerator;		// ���������� ��� ��� ��������� ������
	U64				pauseAfterFrameUs;			// ����� ����� ������
} FrameInfo;

/// <summary>
/// �������� ������ ���������
/// </summary>
typedef struct _Protocol
{
public:	// Data
	const ProtocolEnum		protocolEnum;				// ������������� ���������
	const char				*protocolName;				// �������� ���������
	const ProtocolTypeEnum	protocolType;				// ��� ��������� (������������ ��� �������� ������ ������� �����������)
#if 1
	const std::vector<FrameInfo>	frameInfoVector;	// ������ ������� ������
#else
	const U8				bitsCount;					// ���������� ���
	const U8				bitsCountForGenerator;		// ���������� ��� ��� ��������� ������
	const U8				frameCount;					// ���������� �������
#endif
	const BitsDirection		bitsDirection;				// ������ LSB ��� MSB
	const LogicalBit		bitZero;					// �������� ���� "0"
	const LogicalBit		bitOne;						// �������� ���� "1"
	const bool				needLeadBits;				// ������� ����� ��� Lead, ������ ��
	const LogicalBit		bitLead;					// �������� Lead
	const bool				needRepeatLeadBits;			// ������� ����� ��� Repeat Lead, ������ ���
	const LogicalBit		bitRepeatLead;				// �������� Repeat Lead
	const bool				needRepeat;					// ������� ������� � ���������
	const RepeatInfo		repeatInfo;					// ���������� �� �������
	const bool				needFinalBit;				// ������� ��������� �� ��������� ����
	const LogicalBit		bitFinal;					// ������ ���������� ����
	const bool				needSynchro;				// ������� ������� ���������������
	const SynchroInfo		synchroInfo;				// ������ � ������ ������������������

	_Protocol(
		const ProtocolEnum &pProtocolEnum
		, const char *pProtocolName
		, const ProtocolTypeEnum &pProtocolTypeEnum
#if 1
		, const std::initializer_list<FrameInfo> &pFrameInfoVector
#else
		, U8 pBitsCount
		, U8 pBitsCountForGenerator
		, U8 pFrameCount
#endif
		, const BitsDirection &pBitsDirection
		, const LogicalBit &pBitZero
		, const LogicalBit &pBitOne
		, bool pNeedLeadBits
		, const LogicalBit &pBitLead
		, bool pNeedRepeatLeadBits
		, const LogicalBit &pBitRepeatLead
		, bool pNeedRepeat
		, const RepeatInfo &pRepeatInfo
		, bool pNeedFinalBit
		, const LogicalBit &pBitFinal
		, bool pNeedSynchro = false
		, const SynchroInfo &pSynchroInfo = SynchroInfo()
		)
		: protocolEnum(pProtocolEnum)
		, protocolName(pProtocolName)
		, protocolType(pProtocolTypeEnum)
#if 1
		, frameInfoVector(pFrameInfoVector)
#else
		, bitsCount(pBitsCount)
		, bitsCountForGenerator(pBitsCountForGenerator)
		, frameCount(pFrameCount)
#endif
		, bitsDirection(pBitsDirection)
		, bitZero(pBitZero)
		, bitOne(pBitOne)
		, needLeadBits(pNeedLeadBits)
		, bitLead(pBitLead)
		, needRepeatLeadBits(pNeedRepeatLeadBits)
		, bitRepeatLead(pBitRepeatLead)
		, needRepeat(pNeedRepeat)
		, repeatInfo(pRepeatInfo)
		, needFinalBit(pNeedFinalBit)
		, bitFinal(pBitFinal)
		, needSynchro(pNeedSynchro)
		, synchroInfo(pSynchroInfo)
	{}
} Protocol;

#endif;
