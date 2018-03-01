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

/* ������ ���� ���������� */
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
	bool	mSynchroOk;			// ��������� Synchro
	bool	mLeadsOk;			// ��������� ������� Lead
	bool	mRepeatLeadsOk;		// ��������� Lead ��� �������
	bool	mDataOk;			// ���������� ������
	VectorData	mData;			// ������ ������������ ������ (������ ����� ��������� ����� 64 ���)
	VectorData	mReverseData;	// ������ ������������ ������ (������ ����� ��������� ����� 64 ���)
	U32		mBitCount;			// ���������� ������������ �����

	void addBit(bool pOne);		// ��������� ��� (true - "1", ����� "0")
	void deleteLastBit();		// ������� ��������� ���, ��� ��������� ��� � �� ������
	VectorData &getVectorData(BitsDirection pBitDirection);	// ��������� �������� ������ �������� ����������� �����
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
	OneDistanceInfoVector			mVectorData;				// ������ ��������� �������, ����������� ������. ����������� ��� ��������� ����������, ����� ������������ �������������.
	U64								mVectorDataOffset;			// ������ ������, ������� ��� �� ������������� ��� �������������
	BitState						mStartBitState;				// ��������� ��������� ������, ������������� � mVectorData
	U64								mStartSampleNumber;			// ������ ����� �� �������, ����� ��������� � ������ ������� �������
	bool							mStartExtraSampleExists;	// ������� ����, ��� mStartSampleNumber �� ��������� � ������ ������� �������
	U64								mFinalSampleNumber;			// ��������� ����� ��� �������
};

class KiIR_Protocols;

#include "KiIRAnalyzer.h"

/// <summary>
/// ������ ������ ����������
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
	/// ������ ������� ������. ������ ������������ �� �������� ��������� ��� � ������� ������� ���������.
	/// </summary>
	/// <returns>true - ������������� �������.</returns>
	bool	protocolAnalyze(KiIRAnalyzer *ppKiIRAnalyzer, AnalyzerChannelData *pp_ChannelData, KiIRAnalyzerResults *ppResults, KiIRAnalyzerSettings *ppSettings);

	/// <summary>
	/// ��������� ���������.
	/// </summary>
	void	setProtocol(U32 pSampleRateHz, const Protocol *pp_Protocol);

protected:
	/* ������, ����������� � ���������, ����� ��� ��������������� ������������� */

	/* ������������� Synchro. ���������� � Synchro ������� � mp_CurrentProtocol. */
	bool	searchSynchro();

	/* ������������� Lead. ���������� � Lead ������� � mp_CurrentProtocol. */
	DataType	searchLead();

	/* ������������� ������ (Lead ��� ��������). ���������� � Lead ������� � mp_CurrentProtocol. */
	bool	recognizeData(U8 pFrameIndex);

	/* ������������� NEC � �������� ���������� */
	bool	protocolAnalyzeNEC();

	/* ������������� RC5 � �������� (BiPhase conding, ���� ���������) ���������� */
	bool	protocolAnalyzeRC5();

	/* ��������� ����������� ������ */
	bool	fillData();

private:
	void	cleanProtocolData();
	/* ��� ���������� */
	void	ProcessManchesterData();
	void	SynchronizeManchester();
	void	SaveBit(U64 location, U32 value);
	void	Invalidate();

private:
	const Protocol					*mp_CurrentProtocol;		// �������� ��������
	ProtocolData					mProtocolData;				// ������ ����������� �������������
	std::unique_ptr<DataCluster>	mDataCluster;				// ����� ������ ��� �������
	/* ��� ��������� ���������� */
	U32								mT;							// ������������ ����������� � ��������
	U32								mTError;					// ������������ ���������� (������) ������������ � ��������
	std::vector< std::pair< U64, U64 > > mBitsForNextByte;		// ����������� ������ � ����: value, location
	std::vector<U64>				mUnsyncedLocations;			// ��������� ����� ��� ������� ������
	bool							mSynchronized;				// ������� ������� �������������
	bool							mInverted;					// "0" � "1" ����� ������������ ������� ��������

	/* ��������! ��� ������ ������� � ������� �� ������ */
	AnalyzerChannelData				*mpChannelData;		// ������ ��� �������
	KiIRAnalyzerResults				*mpResults;			// ����������
	KiIRAnalyzerSettings			*mpSettings;		// ���������
	U32								mSampleRateHz;		// ������� �������������
	KiIRAnalyzer					*mpKiIRAnalyzer;	// ����� �����������. ��������� �������� ��������� ������
};

#endif
