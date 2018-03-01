#pragma once

#ifndef __KIIR_PROTOCOLS_H
#define __KIIR_PROTOCOLS_H

#include "KiIRProtocolsCommon.h"
#include "KiIR_NEC.h"
#include "KiIR_SIRC.h"
#include "KiIR_SHARP.h"
#include "KiIR_JVC.h"
#include "KiIR_RC5.h"
#include "KiIR_RC5X.h"
#include "KiIR_RCA.h"
#include "KiIR_NRC17.h"
#include "KiIR_SAMSUNG32.h"
#include "KiIR_SAMSUNG48.h"
#include "KiIR_PANASONIC.h"
#include "KiIR_PENTAX.h"
#include "KiIR_KATHREIN.h"
#include "KiIR_LEGO.h"
#include "KiIR_THOMSON.h"
#include "KiIR_TELEFUNKEN.h"
#include "KiIR_RCCAR.h"
#include "KiIR_NUBERT.h"
#include "KiIR_FAN.h"
#include "KiIR_SPEAKER.h"
#include "KiIR_ROOMBA.h"
#include "KiIR_S100.h"
#include "KiIR_NIKON.h"
#include "KiIR_DAIKIN.h"

#include "KiIRAnalyzerResults.h"
#include "KiIRAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

/* Список всех протоколов */
extern const Protocol * const AllProtocols[];

typedef enum _DataType
{
	dtLEADS = 0,
	dtREPEATLEADS,
	dtDATAU64,
	dtDATAVECTOR,
	dtDATAWITHTOGGLE,
	dtDATAWITHEXTRA,
	dtREPEAT,
	dtFINALBIT,
	dtTOGGLE,
	dtSTARTBIT,
	dtEXTRADATA,
	dtSYNCHRODATA,
	dtSYNCHROPAUSE,
	dtUNKNOWN
} DataType;

typedef struct _ProtocolData
{
	bool	mSynchroOk;			// Распознан Synchro
	bool	mLeadsOk;			// Распознан обычный Lead
	bool	mRepeatLeadsOk;		// Распознан Lead для повтора
	bool	mDataOk;			// Распознаны данные
	VectorData	mData;			// Вектор распознанных данных (данные могут содержать более 64 бит)
	VectorData	mReverseData;	// Вектор распознанных данных (данные могут содержать более 64 бит)
	U32		mBitCount;			// Количество распознанных битов

	void addBit(bool pOne);		// Добавляем бит (true - "1", иначе "0")
	void deleteLastBit();		// Удаляем последний бит, это финальный бит и он лишний
	VectorData &getVectorData(BitsDirection pBitDirection);	// Формируем выходные данные согласно направлению битов
} ProtocolData;

typedef struct _OneDistanceInfo
{
	DataType	mDataType;
	U64			mSampleNumber;
	U64			mDistance;
} OneDistanceInfo;

typedef std::vector<OneDistanceInfo> OneDistanceInfoVector;

class DataCluster
{
public:
	DataCluster();

	void clear();

	void setStartBitState(const BitState &pBitState)
	{
		mStartBitState = pBitState;
	}
	BitState getStartBitState() const
	{
		return mStartBitState;
	}
	void push_back(const OneDistanceInfo &&pOneDistanceInfo)
	{
		mVectorData.push_back(pOneDistanceInfo);
	}
	void setFinalSampleNumber(U64 pFinalSampleNumber)
	{
		mFinalSampleNumber = pFinalSampleNumber;
	}
	U64 getFinalSampleNumber() const
	{
		return mFinalSampleNumber;
	}
	size_t size() const
	{
		return mVectorData.size();
	}
	bool isEmpty() const
	{
		return (0 == size());
	}
	OneDistanceInfoVector &getVectorData()
	{
		return mVectorData;
	}
	U64 getVectorDataOffset() const
	{
		return mVectorDataOffset;
	}
	void setVectorDataOffset(U64 pVectorDataOffset)
	{
		mVectorDataOffset = pVectorDataOffset;
	}
	void setStartExtraSampleExists(bool pValue)
	{
		mStartExtraSampleExists = pValue;
	}
	void setStartSampleNumber(U64 pStartSampleNumber)
	{
		mStartSampleNumber = pStartSampleNumber;
	}

	void printAll();

	void printAllToCsvFile(std::ostream &os);

private:
	OneDistanceInfoVector			mVectorData;				// Данные считываем заранее, упреждающее чтение. Заполняется для известных протоколов, потом производится распознавание.
	U64								mVectorDataOffset;			// Первый индекс, который еще не использовался для распознавания
	BitState						mStartBitState;				// Начальное состояние данных, расположенных в mVectorData
	U64								mStartSampleNumber;			// Первый семпл до выборки, может совпадать с первым сэмплом выборки
	bool							mStartExtraSampleExists;	// Признак того, что mStartSampleNumber не совпадает с первым сэмплом выборки
	U64								mFinalSampleNumber;			// Последний сэмпл для выборки
};

class KiIR_Protocols;

#include "KiIRAnalyzer.h"

/// <summary>
/// Анализ данных протоколов
/// </summary>
class KiIR_Protocols
{
public:
	KiIR_Protocols(const Protocol *pp_Protocol = nullptr)
		: mp_CurrentProtocol(pp_Protocol)
		, mSampleRateHz(1000000)
		, mDataCluster(new DataCluster())
	{}
	virtual ~KiIR_Protocols() {}

	/// <summary>
	/// Анализ входных данных. Анализ производится по текущему протоколу или с поиском нужного протокола.
	/// </summary>
	/// <returns>true - распознавание успешно.</returns>
	bool	protocolAnalyze(KiIRAnalyzer *ppKiIRAnalyzer, AnalyzerChannelData *pp_ChannelData, KiIRAnalyzerResults *ppResults, KiIRAnalyzerSettings *ppSettings);

	/// <summary>
	/// Установка протокола.
	/// </summary>
	void	setProtocol(U32 pSampleRateHz, const Protocol *pp_Protocol);

protected:
	/* Модули, привязанные к протоколу, здесь нет автоматического распознавания */

	/* Распознавание Synchro. Информацию о Synchro находим в mp_CurrentProtocol. */
	bool	searchSynchro();

	/* Распознавание Lead. Информацию о Lead находим в mp_CurrentProtocol. */
	DataType	searchLead();

	/* Распознавание данных (Lead уже известен). Информацию о Lead находим в mp_CurrentProtocol. */
	bool	recognizeData(U8 pFrameIndex);

	/* Распознавание NEC и подобных протоколов */
	bool	protocolAnalyzeNEC();

	/* Распознавание RC5 и подобных (BiPhase conding, типа Манчестер) протоколов */
	bool	protocolAnalyzeRC5();

	/* Выполняем упреждающее чтение */
	bool	fillData();

private:
	void	cleanProtocolData();
	/* Для Манчестера */
	void	ProcessManchesterData();
	void	SynchronizeManchester();
	void	SaveBit(U64 location, U32 value);
	void	Invalidate();

private:
	const Protocol					*mp_CurrentProtocol;		// Заданный протокол
	ProtocolData					mProtocolData;				// Данные результатов распознавания
	std::unique_ptr<DataCluster>	mDataCluster;				// Набор данных для разбора
	/* Для Манчестер протоколов */
	U32								mT;							// Длительность полупериода в попугаях
	U32								mTError;					// Длительность отклонения (ошибки) длительности в попугаях
	std::vector< std::pair< U64, U64 > > mBitsForNextByte;		// Распозанные данные в виде: value, location
	std::vector<U64>				mUnsyncedLocations;			// Временный буфер при анализе данных
	bool							mSynchronized;				// Признак наличия синхронизации
	bool							mInverted;					// "0" и "1" могут кодироваться разными фронтами

	/* Внимание! Эти данные внешние и удалять из нельзя */
	AnalyzerChannelData				*mpChannelData;		// Данные для анализа
	KiIRAnalyzerResults				*mpResults;			// Результаты
	KiIRAnalyzerSettings			*mpSettings;		// Установки
	U32								mSampleRateHz;		// Частота дискретизации
	KiIRAnalyzer					*mpKiIRAnalyzer;	// Класс анализатора. Требуется вызывать некоторые методы
};

#endif
