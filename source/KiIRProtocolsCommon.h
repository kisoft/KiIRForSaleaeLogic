#pragma once

#ifndef __KIIRPROTOCOLSCOMMON_H
#define __KIIRPROTOCOLSCOMMON_H

#include "Analyzer.h"

/// <summary>
/// Переменное количество битов
/// </summary>
#define FLOAT_BITS_COUNT	(-1)
#define	ALLPROTOCOLS		(-1)

typedef std::vector<U64> VectorData;

/// <summary>
/// Перечисление всех доступных протоколов. peMAXID - автоматическое определение
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
/// Направление битов, т.е. какие биты идут в начале фрейма
/// </summary>
typedef enum BitsDirection
{
	bdLSB = 0,
	bdMSB,
	bdUNKNOWN = bdMSB
};

/* Инверсная логика */
#define bcMARK BIT_LOW
#define bcSPACE BIT_HIGH

/// <summary>
/// Временные данные для кодирования одного бита. В us
/// </summary>
typedef struct _BitTimes
{
	U16				firstPeriodUs;
	U16				secondPeriodUs;
} BitTimes;

/// <summary>
/// Информация по кодированию одного бита. Это может быть как логические биты данных, так и Lead/Trail биты
/// </summary>
typedef struct _LogicalBit
{
	union {
		U8				bitValue;					// Значение бита (0 или 1). Для Lead - количество битов
		U8				bitCount;					// Количество битов. Используется для вывода Leads
	};
	BitState		activeLevel;				// Активный уровень для бита (MARK or SPACE)
	BitTimes		timesInfo;					// Временные параметры кодирования бита

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
/// Типы кодов повторов
/// </summary>
typedef enum _RepeatType
{
	rtNormal = 0,								// Просто повторяется код с теми или иными особенностями
	rtSpecialLead,								// Для повтора используется специальная последовательность, например, специальный Lead
	rtCodeWithoutLead							// Повторяется код без Lead
} RepeatType;

/// <summary>
/// Тип интервала повтора. Период фреймов (ripPeriod) или длительность паузы между фреймами (ritGap)
/// </summary>
typedef enum _RepeatIntervalType
{
	ritPeriod = 0,
	ritGap
} RepeatIntervalType;

typedef struct _RepeatInfo
{
	RepeatType			repeatType;				// Тип повтора
	RepeatIntervalType	repeatIntevalType;		// Тип паузы при повторе: период фреймов или пауза между фреймами
	U32					repeatTime;				// Собственно период повтора
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
	U32				infoBitsCount;				// Количество бит в синхро последовательности
	LogicalBit		bitInfo;					// Описание бита синхро последовательности
	bool			needFinalBit;				// Наличие финального бита последовательности
	U64				pauseLengthUs;				// Длительность паузы после синхро последовательности (длительность в us)

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
	U8				frameIndex;					// Номер фрейма (0..n)
	U8				bitsCount;					// Количество бит
	U8				bitsCountForGenerator;		// Количество бит для тестового фрейма
	U64				pauseAfterFrameUs;			// Пауза после фрейма
} FrameInfo;

/// <summary>
/// Описание одного протокола
/// </summary>
typedef struct _Protocol
{
public:	// Data
	const ProtocolEnum		protocolEnum;				// Идентификатор протокола
	const char				*protocolName;				// Название протокола
	const ProtocolTypeEnum	protocolType;				// Тип протокола (используется для простого вызова нужного анализатора)
#if 1
	const std::vector<FrameInfo>	frameInfoVector;	// Данные каждого фрейма
#else
	const U8				bitsCount;					// Количество бит
	const U8				bitsCountForGenerator;		// Количество бит для тестового фрейма
	const U8				frameCount;					// Количество фреймов
#endif
	const BitsDirection		bitsDirection;				// Начало LSB или MSB
	const LogicalBit		bitZero;					// Описание бита "0"
	const LogicalBit		bitOne;						// Описание бита "1"
	const bool				needLeadBits;				// Наличие битов для Lead, обычно да
	const LogicalBit		bitLead;					// Описание Lead
	const bool				needRepeatLeadBits;			// Наличие битов для Repeat Lead, обычно нет
	const LogicalBit		bitRepeatLead;				// Описание Repeat Lead
	const bool				needRepeat;					// Наличие повтора в протоколе
	const RepeatInfo		repeatInfo;					// Информация оп повторе
	const bool				needFinalBit;				// Наличие конечного не значащего бита
	const LogicalBit		bitFinal;					// Данные финального бита
	const bool				needSynchro;				// Признак наличия синхроимпульсов
	const SynchroInfo		synchroInfo;				// Данные о синхро последовательности

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
