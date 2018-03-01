#include "KiIR_Protocols.h"
#include "DebugLog.h"
#include <AnalyzerResults.h>
#include <algorithm>

#include <iostream>
#include <fstream>
#include <sstream>

#define USE_LOGFILE

#if !defined(USE_LOGFILE)
#pragma warning(push)

#pragma warning(disable:4005)

#define LOGTOFILEX(t,v)
#define LOGTOFILE(t,v)
#define CLOSEFILE()

#pragma warning(pop)

#else

// #define USE_LOGFILE_HERE

#endif

/*
* �������� �� ����������� � ������ ��� ��������
*/

/* �� ������� � ������������ */
#define RECALC_STOUS(x,s)	(double(x)*1000000.0/(s))
/* �� ����������� � ������ */
#define RECALC_TIME(x,s)	(double(x)*(s)/1000000.0)

// static 
std::string	ProtocolType::getName(ProtocolTypeEnum pProtocolType)
{
	switch (pProtocolType)
	{
	case ptPulseDistance:
		return "PulseDistance";
	case ptPulseWidth:
		return "PulseWidth";
	case ptPulseDistanceWidth:
		return "PulseDistanceWidth";
	case ptBiPhase:
		return "BiPhase";
	case ptPulsePosition:
		return "PulsePosition";
	}
	return "";
}

class KiBitAnalyzeInfo
{
public:
	KiBitAnalyzeInfo(const LogicalBit &pBit, bool pRequired, U8 pTimesPrecisionPrecents);

	bool	checkBit(U8 pBitIndex, U64 pDistance);

	bool	checkFinalBit(U8 pBitIndex, U64 pDistance);

	bool	isRecognized() const
	{
		return (mBitRecognized[0] && mBitRecognized[1]);
	}

	void clear()
	{
		mBitRecognized[0] = mBitRecognized[1] = false;
	}
	void setRecognized()
	{
		mBitRecognized[0] = mBitRecognized[1] = true;
	}

private:
	bool				mRequired;			// ��������� ������ ����
	bool				mBitRecognized[2];	// ����� ������������� ������ ����
	LogicalBit			mBit;				// ���������� � ����
	U64					mMinBitTime[2];		// ����������� �������� �����
	U64					mMaxBitTime[2];		// ������������ �������� �����
	U8					mTimesPrecisionPercents;	// ��������
};

KiBitAnalyzeInfo::KiBitAnalyzeInfo(const LogicalBit &pBit, bool pRequired, U8 pTimesPrecisionPrecents)
	: mBit(pBit)
	, mTimesPrecisionPercents(pTimesPrecisionPrecents)
	, mRequired(pRequired)
{
	mBitRecognized[0] = mBitRecognized[1] = false;
	if (mRequired)
	{
		U16 lBitTime = mBit.timesInfo.firstPeriodUs;
		mMinBitTime[0] = lBitTime - (lBitTime * mTimesPrecisionPercents / 100);
		mMaxBitTime[0] = lBitTime + (lBitTime * mTimesPrecisionPercents / 100);
		lBitTime = mBit.timesInfo.secondPeriodUs;
		mMinBitTime[1] = lBitTime - (lBitTime * mTimesPrecisionPercents / 100);
		mMaxBitTime[1] = lBitTime + (lBitTime * mTimesPrecisionPercents / 100);
	}
}

bool KiBitAnalyzeInfo::checkBit(U8 pBitIndex, U64 pDistance)
{
#if defined(USE_LOGFILE_HERE)
	LOGTOFILE("### CheckBit Distance", pDistance);
	LOGTOFILE("### CheckBit bitIndex", pBitIndex);
	LOGTOFILE("### CheckBit BitValue", mBit.bitValue);
#endif

	U16 lBitTime = (0 == pBitIndex ? mBit.timesInfo.firstPeriodUs : mBit.timesInfo.secondPeriodUs);

#if defined(USE_LOGFILE_HERE)
	LOGTOFILE("### CheckBit BitTime", lBitTime);
#endif

	bool lResult = (pDistance >= mMinBitTime[pBitIndex] && pDistance <= mMaxBitTime[pBitIndex]);

#if defined(USE_LOGFILE_HERE)
	LOGTOFILE("### CheckBit Result", lResult);
#endif

	mBitRecognized[pBitIndex] = lResult;
	return lResult;
}

/*
* ��� ���������� ����:
* BitIndex = 0: ������ ���� ����������
* BitIndex = 1: ��������� ������ ���� ������
*/
bool KiBitAnalyzeInfo::checkFinalBit(U8 pBitIndex, U64 pDistance)
{
#if defined(USE_LOGFILE_HERE)
	LOGTOFILE("### CheckFinalBit Distance", pDistance);
	LOGTOFILE("### CheckFinalBit bitIndex", pBitIndex);
	LOGTOFILE("### CheckFinalBit BitValue", mBit.bitValue);
#endif

	U16 lBitTime = (0 == pBitIndex ? mBit.timesInfo.firstPeriodUs : mBit.timesInfo.secondPeriodUs);

#if defined(USE_LOGFILE_HERE)
	LOGTOFILE("### CheckFinalBit BitTime", lBitTime);
#endif

	bool lResult = false;

	if (0 == pBitIndex)
	{
		lResult = (pDistance >= mMinBitTime[pBitIndex] && pDistance <= mMaxBitTime[pBitIndex]);
	}
	else
	{
		lResult = (pDistance > mMaxBitTime[pBitIndex]);
	}

#if defined(USE_LOGFILE_HERE)
	LOGTOFILE("### CheckFinalBit Result", lResult);
#endif

	mBitRecognized[pBitIndex] = lResult;
	return lResult;
}

DataCluster::DataCluster()
{
	mStartExtraSampleExists = false;
	mStartSampleNumber = mFinalSampleNumber = 0;
	mStartBitState = bcSPACE;
}

void DataCluster::clear()
{
	mStartExtraSampleExists = false;
	mStartSampleNumber = mFinalSampleNumber = 0;
	mStartBitState = bcSPACE;
	mVectorData.clear();
	mVectorDataOffset = 0;
}


void DataCluster::printAll()
{
	LOGTOFILE("*** StartSample", mStartSampleNumber);
	LOGTOFILE("*** StartExtraSampleExists", mStartExtraSampleExists);
	LOGTOFILE("*** StartState", (bcMARK == mStartBitState ? 1 : 0));
	for (auto lIter = mVectorData.cbegin(); lIter != mVectorData.cend(); ++lIter)
	{
		LOGTOFILE("*** SampleNumber", lIter->mSampleNumber);
		LOGTOFILE("*** Distance", lIter->mDistance);
	}
	LOGTOFILE("*** End of data", mVectorData.size());
}

typedef enum _STATES
{
	sPause = 0,
	sSynchro,
	sLead,
	sData
} STATES;

static void printState(std::ostream &os, STATES pState, U64 pSampleNumber, U64 pDistance, bool pValue)
{
	/* MARK */
	if (pValue)
	{
		switch (pState)
		{
		case sSynchro:
			os << pSampleNumber << ";" << "Synchro MARK" << ";" << pValue << ";;" << pDistance << "\n";
			break;
		case sData:
			os << pSampleNumber << ";" << "Data MARK" << ";" << pValue << ";;" << pDistance << "\n";
			break;
		case sLead:
			os << pSampleNumber << ";" << "Lead MARK" << ";" << pValue << ";;;;;;;;" << pDistance << "\n";
			break;
		case sPause:
			os << pSampleNumber << ";" << "Pause MARK ???" << ";" << pValue << ";;;;;;;;;;;;" << pDistance << "\n";
			break;
		};
	}
	/* SPACE */
	else
	{
		switch (pState)
		{
		case sSynchro:
			/* "0" */
			if (pDistance < 750)
			{
				os << pSampleNumber << ";" << "Synchro Zero SPACE" << ";" << pValue << ";;;;" << pDistance << "\n";
			}
			/* "1" */
			else
			{
				os << pSampleNumber << ";" << "Synchro One SPACE" << ";" << pValue << ";;" << pDistance << "\n";
			}
			break;
		case sData:
			/* "0" */
			if (pDistance < 750)
			{
				os << pSampleNumber << ";" << "Data Zero SPACE" << ";" << pValue << ";;;;" << pDistance << "\n";
			}
			/* "1" */
			else
			{
				os << pSampleNumber << ";" << "Data One SPACE" << ";" << pValue << ";;;;;;" << pDistance << "\n";
			}
			break;
		case sLead:
			os << pSampleNumber << ";" << "Lead SPACE" << ";" << pValue << ";;;;;;;;;;" << pDistance << "\n";
			break;
		case sPause:
			if (pDistance > 30000)
			{
				os << pSampleNumber << ";" << "Pause SPACE" << ";" << pValue << ";;;;;;;;;;;;;;" << pDistance << "\n";
			}
			else
			{
				os << pSampleNumber << ";" << "Pause SPACE" << ";" << pValue << ";;;;;;;;;;;;" << pDistance << "\n";
			}
			break;
		};
	}
}

void DataCluster::printAllToCsvFile(std::ostream &os)
{
	os << "*** StartSample;" << mStartSampleNumber << "\n";
	os << "*** StartExtraSampleExists;" << mStartExtraSampleExists << "\n";
	os << "*** StartState;" << (bcMARK == mStartBitState ? "MARK" : "SPACE") << "\n";
	bool lStartState = (bcMARK == mStartBitState ? 1 : 0);
	/**
	* ����� ���������:
	* 1. ��������� "�������������", 6 MARK ���������.
	* 2. ����� (����� >100000) = SPACE
	* 3. Lead (3500 -> 1700) = MARK + SPACE
	* 4. ������� 1 (64 ���� + Final bit(���� MARK �������))		MARK + SPACE - ������, MARK - final
	* 5. ����� (����� >100000)
	* 6. Lead (3500 -> 1700)
	* 7. ������� 2 (64 ���� + Final bit(���� MARK �������))
	* 8. ����� (����� >100000)
	* 9. Lead (3500 -> 1700)
	* 10. ������� 3 (152 ���� + Final bit(���� MARK �������))
	*
	* MARK ��� ������ � ������ + 2
	* SPACE ��� ������ � ������ + 4 (�������) � +6 (��������)
	*
	* MARK ��� Lead + 8
	* SPACE ��� Lead + 10
	*
	* SPACE ��� Pause + 12
	*/

	STATES lState = sPause;
	U32 lSynchroCount = 0;

	for (auto lIter = mVectorData.cbegin(); lIter != mVectorData.cend(); ++lIter)
	{
		U64 distance = lIter->mDistance;
		if (lStartState)
		{
			/* MARK */
			if (distance < 2000)
			{
				/* ��� �������������, 6 MARK. ����� ���� ��� � ������, ��� � ����� ����� */
				if (sPause == lState)
				{
					lSynchroCount = 0;
					lState = sSynchro;
				}
				if (sSynchro == lState || sData == lState)
				{
					printState(os, lState, lIter->mSampleNumber, distance, lStartState);
				}
				lSynchroCount++;
			}
			else
			{
				if (sLead != lState)
				{
					lState = sLead;
				}
				/* ��� ����� Lead */
				printState(os, lState, lIter->mSampleNumber, distance, lStartState);
			}
		}
		else
		{
			if (distance > 10000)
			{
				lState = sPause;
				printState(os, lState, lIter->mSampleNumber, distance, lStartState);
			}
			else
			{
				if (sSynchro == lState || sData == lState)
				{
					printState(os, lState, lIter->mSampleNumber, distance, lStartState);
				}
				else if (sLead == lState)
				{
					printState(os, lState, lIter->mSampleNumber, distance, lStartState);
					lState = sData;
				}
			}

			/* SPACE */
			// os << lIter->mSampleNumber << ";" << lIter->mDistance << ";" << lStartState << ";;;;;" << lIter->mDistance << "\n";
		}
		lStartState = (0 == lStartState ? 1 : 0);
	}
	os << "*** End of data;" << mVectorData.size() << "\n";
}

/// <summary>
/// ������ ���� ����������. ������������ ��� ������������ ������ ���������� � ���� � �.�.
/// </summary>
const Protocol * const AllProtocols[] =
{
	&NEC_ProtocolInfo,
	&SIRC_ProtocolInfo,
	&SHARP_ProtocolInfo,
	&JVC_ProtocolInfo,
	&RC5_ProtocolInfo,
	&RC5X_ProtocolInfo,
	&RCA_ProtocolInfo,
	&NRC17_ProtocolInfo,
	&SAMSUNG32_ProtocolInfo,
	&SAMSUNG48_ProtocolInfo,
	&PANASONIC_ProtocolInfo,
	&PENTAX_ProtocolInfo,
	&KATHREIN_ProtocolInfo,
	&LEGO_ProtocolInfo,
	&THOMSON_ProtocolInfo,
	&TELEFUNKEN_ProtocolInfo,
	&RCCAR_ProtocolInfo,
	&NUBERT_ProtocolInfo,
	&FAN_ProtocolInfo,
	&SPEAKER_ProtocolInfo,
	&ROOMBA_ProtocolInfo,
	&S100_ProtocolInfo,
	&NIKON_ProtocolInfo,
	&DAIKIN_ProtocolInfo,

	nullptr
};

/*
* ����� ������ ����������� ��� MSB, ��� LSB ����� ������
*/
// ��������� ��� (true - "1", ����� "0")
void ProtocolData::addBit(bool pOne)
{
	U32 lByteIndex = (mBitCount - 1) / 64;

	if (lByteIndex >= mData.size())
	{
		mData.push_back(0);
	}
	U64 lData = mData[lByteIndex];
	lData <<= 1ull;
	if (pOne)
	{
		lData |= 1ull;
	}
	mData[lByteIndex] = lData;
}

// ������� ��������� ���, ��� ��������� ��� � �� ������
void ProtocolData::deleteLastBit()
{

	LOGTOFILE("^^^ deleteLastBit, mBitCount", mBitCount);

	U32 lByteIndex = (mBitCount - 1) / 64;
	U32 lLastBitsCount = mBitCount - lByteIndex * 64;

	LOGTOFILE("^^^ deleteLastBit, lByteIndex", lByteIndex);
	LOGTOFILE("^^^ deleteLastBit, lLastBitsCount", lLastBitsCount);


	/* ���� ��� ���� ��� �� ���������� ����� (64 ���), ����� ������� ��� ��������� ����� */
	if (1 == lLastBitsCount)
	{
		mData.pop_back();
		LOGTOFILE("^^^ deleteLastBit, pop_back, mData.size()", mData.size());
	}
	else
	{
		LOGTOFILE("^^^ deleteLastBit, before shift back mData.size()", mData.size());
		if (lByteIndex < mData.size())
		{
			U64 lData = mData[lByteIndex];
			LOGTOFILE("^^^ deleteLastBit, before shift Data", lData);
			lData >>= 1ull;
			LOGTOFILE("^^^ deleteLastBit, after shift Data", lData);
			mData[lByteIndex] = lData;
		}
	}
	mBitCount--;
	LOGTOFILE("^^^ deleteLastBit, mBitCount", mBitCount);
}

VectorData &ProtocolData::getVectorData(BitsDirection pBitDirection)
{
	if (bdLSB == pBitDirection)
	{
		mReverseData.clear();

		if (mData.size())
		{

			std::stringstream ss;

			U32 lBitCount = mBitCount / 2;

			U64 segment_remaining = lBitCount / 64;
			U64 bits_remaining = lBitCount - segment_remaining * 64;
			segment_remaining += (0 != bits_remaining ? 1 : 0);

			for each (auto lData in mData)
			{
				LOGTOFILEX(";;; lData = ", lData);

				U64 part_bits_count = (segment_remaining > 1 ? 64 : bits_remaining);
				if (part_bits_count > 0)
				{
					U64 bitsMask = 1ull << (part_bits_count - 1);
					for (int i = 0; i < part_bits_count; i++)
					{
						char ch = (0 != (lData & bitsMask) ? '1' : '0');
						ss << ch;
						bitsMask >>= 1ull;
					}
					segment_remaining--;
				}
			}

			segment_remaining = lBitCount / 64;
			bits_remaining = lBitCount - segment_remaining * 64;
			segment_remaining += (0 != bits_remaining ? 1 : 0);

			std::string str = ss.str();

			LOGTOFILE(";;; lData start REVERSE, str.length()", str.length());

			for (auto lIter = str.rbegin(); lIter != str.rend();)
			{
				U64 part_bits_count = (segment_remaining > 1 ? 64 : bits_remaining);
				if (part_bits_count > 0)
				{
					U64 bitsMask = 1ull << (part_bits_count - 1);
					U64 lData = 0;
					for (int i = 0; i < part_bits_count; i++, lIter++)
					{
						char ch = *lIter;
						if ('1' == ch)
						{
							lData |= bitsMask;
						}
						bitsMask >>= 1ull;
					}
					LOGTOFILEX(";;; lData = ", lData);
					mReverseData.push_back(lData);
					segment_remaining--;
				}
			}
		}
		return mReverseData;
	}
	return mData;
}

#if 0
// ��������� �������� ������ �������� ����������� �����
U64 ProtocolData::getData(BitsDirection pBitDirection)
{
#if 0

	return mData;

#else

	if (bdLSB == pBitDirection)
	{
		/* �������� ������������������ ����� */
		U64 lBitCount = mBitCount; // >> 1;
		U64 lSourceData = mData[0];
		U64 lData = 0;
		U64 lResultMask = 0x1ull << (lBitCount - 1);
		U64 lSourceMask = 0x1ull;
		for (U32 i = 0; i < lBitCount; ++i)
		{
			if (lSourceData & lSourceMask)
			{
				lData |= lResultMask;
			}
			lResultMask >>= 1ull;
			lSourceMask <<= 1ull;
		}
		return lData;
	}
	else
	{
		return mData[0];
	}

#endif
}
#endif

void KiIR_Protocols::cleanProtocolData()
{
	LOGTOFILE("### Clear Protocol Data", 0);
	mProtocolData.mBitCount = 0;
	mProtocolData.mData.clear();
	mProtocolData.mLeadsOk = false;
	mProtocolData.mRepeatLeadsOk = false;
	mProtocolData.mSynchroOk = false;
	mProtocolData.mDataOk = false;
}

void KiIR_Protocols::setProtocol(U32 pSampleRateHz, const Protocol *ppProtocol)
{
	mSampleRateHz = pSampleRateHz;
	mp_CurrentProtocol = ppProtocol;
}

/* ����������, ���������� � ����������� ����� */
bool KiIR_Protocols::protocolAnalyze(KiIRAnalyzer *ppKiIRAnalyzer, AnalyzerChannelData *ppChannelData, KiIRAnalyzerResults *ppResults, KiIRAnalyzerSettings *ppSettings)
{
	bool lResult = false;

	mpChannelData = ppChannelData;
	mpResults = ppResults;
	mpSettings = ppSettings;
	mpKiIRAnalyzer = ppKiIRAnalyzer;

	/* ���������� ������ ������������� ��������� */
	cleanProtocolData();

	if (!mp_CurrentProtocol)
	{
		/* �������� �� ������, ��������� ������������� ���������. ���� �� �����������, ���������� ������. */
		return lResult;
	}

	/* �������� �������������� �� ���� ��������� */
	switch (mp_CurrentProtocol->protocolType)
	{
	default:
	case ptPulseDistance:
	case ptPulseDistanceWidth:
	case ptPulseWidth:
	case ptPulsePosition:
		lResult = protocolAnalyzeNEC();
		break;
	case ptBiPhase:
		lResult = protocolAnalyzeRC5();
		break;
	}
	return lResult;
}

bool KiIR_Protocols::fillData()
{

	LOGTOFILE("$$$ fillData started", 1);

	/* �������� �� ������� */
	mDataCluster->clear();

	U64 lSumLimitWidth = 0;
	const LogicalBit *lpBit = nullptr;
	BitState lActiveBitLevel = bcSPACE;
	bool lActiveBitLevelInited = false;

#if 1
	/* ���� ��������� ������ �������, ������� � � */
	if (mp_CurrentProtocol->needSynchro)
	{
		U64 lMaxFirstSynchroWidth = 0;
		const LogicalBit &lBitTmp = mp_CurrentProtocol->synchroInfo.bitInfo;
		if (lMaxFirstSynchroWidth < mp_CurrentProtocol->synchroInfo.pauseLengthUs)
		{
			lMaxFirstSynchroWidth = mp_CurrentProtocol->synchroInfo.pauseLengthUs;
		}
		if (lMaxFirstSynchroWidth < mp_CurrentProtocol->repeatInfo.repeatTime)
		{
			lMaxFirstSynchroWidth = mp_CurrentProtocol->repeatInfo.repeatTime;
		}

		lSumLimitWidth += lMaxFirstSynchroWidth; // mp_CurrentProtocol->synchroInfo.pauseLengthUs;
		lActiveBitLevel = mp_CurrentProtocol->synchroInfo.bitInfo.activeLevel;
		lActiveBitLevelInited = true;
	}
	/* ������������� ����� ������ ��������� � Leads, ��� ��������� �������, ��������� ������ ����� ���� ��� � MARK, ��� � �� SPACE */
	else 
#endif
	if (mp_CurrentProtocol->needLeadBits || mp_CurrentProtocol->needRepeatLeadBits)
	{
		/* ������� ����� ��� LeadBits */
		if (mp_CurrentProtocol->needLeadBits)
		{
			U64 lMaxFirstLeadWidth = 0;
			const LogicalBit &lBitTmp = mp_CurrentProtocol->bitLead;
			if (lMaxFirstLeadWidth < lBitTmp.timesInfo.firstPeriodUs)
			{
				lMaxFirstLeadWidth = lBitTmp.timesInfo.firstPeriodUs;
			}
			if (lMaxFirstLeadWidth < lBitTmp.timesInfo.secondPeriodUs)
			{
				lMaxFirstLeadWidth = lBitTmp.timesInfo.secondPeriodUs;
			}
			lSumLimitWidth += lMaxFirstLeadWidth;
		}
		if (mp_CurrentProtocol->needRepeatLeadBits)
		{
			U64 lMaxSecondLeadWidth = 0;
			const LogicalBit &lBitTmp = mp_CurrentProtocol->bitRepeatLead;
			if (lMaxSecondLeadWidth < lBitTmp.timesInfo.firstPeriodUs)
			{
				lMaxSecondLeadWidth = lBitTmp.timesInfo.firstPeriodUs;
			}
			if (lMaxSecondLeadWidth < lBitTmp.timesInfo.secondPeriodUs)
			{
				lMaxSecondLeadWidth = lBitTmp.timesInfo.secondPeriodUs;
			}
			lSumLimitWidth += lMaxSecondLeadWidth;
		}
		/* lSumLeadWidth ������� ��� ����� ����� */

		if (!lActiveBitLevelInited)
		{
			lActiveBitLevel = (mp_CurrentProtocol->needRepeatLeadBits ? mp_CurrentProtocol->bitRepeatLead : mp_CurrentProtocol->bitLead).activeLevel;
			lActiveBitLevelInited = true;
		}
	}
	else
	{
		{
			U64 lMaxOneBitWidth = 0;
			const LogicalBit &lBitTmp = mp_CurrentProtocol->bitOne;
			if (lMaxOneBitWidth < lBitTmp.timesInfo.firstPeriodUs)
			{
				lMaxOneBitWidth = lBitTmp.timesInfo.firstPeriodUs;
			}
			if (lMaxOneBitWidth < lBitTmp.timesInfo.secondPeriodUs)
			{
				lMaxOneBitWidth = lBitTmp.timesInfo.secondPeriodUs;
			}
			lSumLimitWidth += lMaxOneBitWidth;
		}
		{
			U64 lMaxZeroBitWidth = 0;
			const LogicalBit &lBitTmp = mp_CurrentProtocol->bitZero;
			if (lMaxZeroBitWidth < lBitTmp.timesInfo.firstPeriodUs)
			{
				lMaxZeroBitWidth = lBitTmp.timesInfo.firstPeriodUs;
			}
			if (lMaxZeroBitWidth < lBitTmp.timesInfo.secondPeriodUs)
			{
				lMaxZeroBitWidth = lBitTmp.timesInfo.secondPeriodUs;
			}
			lSumLimitWidth += lMaxZeroBitWidth;
		}
		/* lSumLimitWidth ������� ��� ����� ����� */

		if (!lActiveBitLevelInited)
		{
			lActiveBitLevel = mp_CurrentProtocol->bitOne.activeLevel;
			lActiveBitLevelInited = true;
		}
	}
	
	LOGTOFILE("*** Start fill vector. lSumLimitWidth", lSumLimitWidth);

	mDataCluster->setStartSampleNumber(mpChannelData->GetSampleNumber());
	if (mpChannelData->GetBitState() != lActiveBitLevel)
	{
		mDataCluster->setStartExtraSampleExists(true);
		mpChannelData->AdvanceToNextEdge();
	}

	mDataCluster->setStartBitState(mpChannelData->GetBitState());
	int lCount = 0;
	/* ��������� �������, ���� ��� ���������� */
	while (mpChannelData->DoMoreTransitionsExistInCurrentData())
	{
		/* ��������� ������� �������� � ���������� ��������� */
		U64 lCurrentPosition = mpChannelData->GetSampleNumber();
		U64 lNextPosition = mpChannelData->GetSampleOfNextEdge();

		/* ���������� ����� �������� � us */
		U64 lDistance = static_cast<U64>(RECALC_STOUS(lNextPosition - lCurrentPosition, mSampleRateHz));
		LOGTOFILE("*** Distance us", lDistance);

		/* ����������� ����������, ���� ������ �������� ������ ������������ ����� (��������, ������ Lead ��� Synchro) */
		if (lDistance > lSumLimitWidth)
		{
			/* ���� ��������� ������ ������������, ��������� ��������� ����� ������� */
			break;
		}
		else if (0 == lDistance)
		{
			lCount++;
		}
		else
		{
			if (0 != (lCount % 2))
			{
				LOGTOFILE("@@@ lCount ��������, ��� ������ @@@", lCount);
			}
			lCount = 0;
			/* ���������� ���������� ������� � ��� "����������" */
			mDataCluster->push_back({ dtUNKNOWN, lCurrentPosition, lDistance });
		}
		/* ��������� � ���������� �������� */
		mpChannelData->AdvanceToNextEdge();
		/* ��������� ����� ���������� ������, ��������, ��� ���������� ���� */
		mDataCluster->setFinalSampleNumber(mpChannelData->GetSampleNumber());
	}
	LOGTOFILE("*** Vector size", mDataCluster->size());

	/* ���������� true, ���� ����������� ������ �� ������ */
	return !mDataCluster->isEmpty();
}

bool KiIR_Protocols::protocolAnalyzeNEC()
{
	LOGTOFILE("$$$ protocolAnalyzeNEC started", 1);

	if (!fillData())
	{
		/* ���� ������ ���, �� � ������ ������ */
		return false;
	}

	LOGTOFILE("$$$ protocolAnalyzeNEC fillData OK", 1);

#if defined(USE_LOGFILE)
	// mDataCluster->printAll();
#endif

	bool lContinue = true;
	if (mp_CurrentProtocol->needSynchro)
	{
		lContinue = searchSynchro();
		if (lContinue)
		{
			LOGTOFILE("$$$ protocolAnalyzeNEC searchSynchro OK", (U64)(lContinue ? 1 : 0));
		}
		else
		{
			LOGTOFILE("$$$ protocolAnalyzeNEC searchSynchro NOT FOUND", (U64)(lContinue ? 1 : 0));
			return false;
		}
	}

	if (lContinue)
	{
		for each (auto lFrameInfo in mp_CurrentProtocol->frameInfoVector)
		{
			LOGTOFILE("$$$ protocolAnalyzeNEC start recognize, frame index", lFrameInfo.frameIndex);
			/* ����� Lead */
			DataType lRes = searchLead();
			if (dtREPEATLEADS == lRes)
			{
				LOGTOFILE("$$$ protocolAnalyzeNEC searchLead FOUND REPEATLEADS", 1);
				/* ����� RepeatLeads ������ ���, ��������� � ���������� ������ */
				continue;
			}
			else if (dtLEADS == lRes)
			{
				if (!recognizeData(lFrameInfo.frameIndex))
				{
					LOGTOFILE("$$$ protocolAnalyzeNEC recognizeData FAILED", 0);
					return false;
				}
				LOGTOFILE("$$$ protocolAnalyzeNEC recognizeData OK", 1);
			}
		}


	}
	return false;
}

/* ������������� RC5 � �������� (���������) ���������� */
bool KiIR_Protocols::protocolAnalyzeRC5()
{
	/* ��� RC-5: BitRate = 1 / 1.778ms �������� 562 Bits/s */
	/*
	* ��� ����������� ������������ ����������� ���������� ������������ ������ ����� ���� "0"
	* ��������! ��� ����� �������� ������ ��� ����������, � ������� ������ ������������ "0" � "1"
	*/
	double half_peroid = mp_CurrentProtocol->bitZero.timesInfo.firstPeriodUs;	// � us

	mT = static_cast<U32>((double(mSampleRateHz) * half_peroid) / 1000000.0);

	mTError = static_cast<U32>((double)mT * mpSettings->mTolerance / 100.0 / 2.0);
	if (mTError < 3)
	{
		mTError = 3;
	}

	LOGTOFILE("mT", mT);
	LOGTOFILE("mTError", mTError);

	/* ������� ��������. ������������ ��� ����������� ������ ��� "0" � "1" */
	switch (mp_CurrentProtocol->protocolEnum)
	{
	case peRC5:
	case peRC5X:
	case peS100:
		mInverted = true;
		break;
	default:
	case peNRC17:
		mInverted = false;
		break;
	}

	mSynchronized = false;
	mBitsForNextByte.clear();
	mUnsyncedLocations.clear();
	if (mp_CurrentProtocol->needLeadBits)
	{
		if (mp_CurrentProtocol->bitLead.activeLevel != mpChannelData->GetBitState())
		{
			mpChannelData->AdvanceToNextEdge();
		}
	}
	else
	{
		mpChannelData->AdvanceToNextEdge();
	}

	LOGTOFILE("Start decode", mpChannelData->GetSampleNumber());

	for (;;)
	{
		SynchronizeManchester();
		ProcessManchesterData();

		mpKiIRAnalyzer->ReportProgress(mpChannelData->GetSampleNumber());
		mpKiIRAnalyzer->CheckIfThreadShouldExit();
	}

	return true;
}

/*
* ��������� ������ (Manchester). ������, ���� ��� ��� ������������������, �.�.
* ��� ������� ���� �� ���� ������������ ������ �������, � ����� ��� ����������� ���-��
* ������������, ���� �� ����������������, ���� �� ������ ���.
*/
void KiIR_Protocols::ProcessManchesterData()
{
	LOGTOFILE("Start ProcessManchesterData, mSynchronized", mSynchronized);
	
	if (mSynchronized == true)
	{
		U64 edge_location = mpChannelData->GetSampleNumber();
		BitState current_bit_state = mpChannelData->GetBitState();

		if (!mInverted)
		{
			if (current_bit_state == bcMARK)	//pos edge, zero
			{
				SaveBit(edge_location, 0);
			}
			else if (current_bit_state == bcSPACE)	//neg edge, one
			{
				SaveBit(edge_location, 1);
			}
		}
		else
		{
			if (current_bit_state == bcMARK)	//pos edge, inverted, one
			{
				SaveBit(edge_location, 1);
			}
			else if (current_bit_state == bcSPACE)		//neg edge, inverted, zero
			{
				SaveBit(edge_location, 0);
			}
		}

		mpChannelData->AdvanceToNextEdge();
		U64 next_edge_location = mpChannelData->GetSampleNumber();
		U64 edge_distance = next_edge_location - edge_location;
		if ((edge_distance > ((2 * mT) - mTError)) && (edge_distance < ((2 * mT) + mTError))) //long
		{
			return;
		}
		else if ((edge_distance >(mT - mTError)) && (edge_distance < (mT + mTError)))	//short
		{
			edge_location = mpChannelData->GetSampleNumber();
			mpChannelData->AdvanceToNextEdge();
			next_edge_location = mpChannelData->GetSampleNumber();
			edge_distance = next_edge_location - edge_location;
			if ((edge_distance > (mT - mTError)) && (edge_distance < (mT + mTError))) //second short (good)
			{
				return;
			}
			else
			{
				Invalidate();
				return;
				//not synced anymore.
			}
		}
		else
		{
			Invalidate();
			return;
			//not synced anymore.
		}
	}
	return;
}

/*
* ������������� Manchester.
* ������������� ���������� ������ ��� ��������� �������, ������������� ����� ������ ������� (1,778ms),
* �� ����� ��� �������� ����������� � ������������� ������, ����� ��������������.
* �.�. �� ������ � ��� ����� ������� ������� � ������������� �������� � ���� ������.
*/
void KiIR_Protocols::SynchronizeManchester()
{
	LOGTOFILE("Start SynchronizeManchester", mpChannelData->GetSampleNumber());
	// ����� �� ��� ������ ���� �� ��������, ������ ����� �����.
	while (mSynchronized == false)
	{
		mpKiIRAnalyzer->CheckIfThreadShouldExit();
		/* ������ �������� */
		U64 edge_location = mpChannelData->GetSampleNumber();
		mpChannelData->AdvanceToNextEdge();
		/* ����� �������� */
		U64 next_edge_location = mpChannelData->GetSampleNumber();
		/* ������������ �������� */
		U64 edge_distance = next_edge_location - edge_location;
		/* ������������ ����� ����������� */
		if ((edge_distance > (mT - mTError)) && (edge_distance < (mT + mTError))) //short
		{
			mUnsyncedLocations.push_back(edge_location);
		}
		/* ������������ ����� ������� */
		else if ((edge_distance >((2 * mT) - mTError)) && (edge_distance < ((2 * mT) + mTError))) //long
		{
			mUnsyncedLocations.push_back(edge_location);
			mSynchronized = true;
			/* ��������� ������� ����� ����� �������� */
			BitState current_bit_state = mpChannelData->GetBitState();
			U32 bit_value = 0;

			if (mInverted)
			{
				if (current_bit_state == bcSPACE)			// �� �� ������������� �����, ���� ��������
				{
					bit_value = 1;							// ������ ������ ����� ��������������� �������� ��������
				}
				else if (current_bit_state == bcMARK)
				{
					bit_value = 0;
				}
			}
			else
			{
				if (current_bit_state == bcSPACE)			// �� �� ������������� �����, ��� ��������
				{
					bit_value = 0;
				}
				else if (current_bit_state == bcMARK)
				{
					bit_value = 1;
				}
			}

			std::vector<U64> locations_to_save;
			while (mUnsyncedLocations.size() > 0)
			{
				locations_to_save.push_back(mUnsyncedLocations.back());
				mUnsyncedLocations.pop_back();
				if (mUnsyncedLocations.size() > 0)
				{
					mUnsyncedLocations.pop_back();
				}
			}
			std::sort(locations_to_save.begin(), locations_to_save.end());
			U32 count = static_cast<U32>(locations_to_save.size());
			for (U32 i = 0; i < count; ++i)
			{
				SaveBit(locations_to_save[i], bit_value);
			}
			break;
		}
		/* ������������ �������� �� ����������, ���������� ������� � �������� � ������ */
		else
		{
			//back to idle.
			Invalidate();
		}
	}
}

/*
* ���������� ������ ����.
* ��������! ��� ���������� �������, ������ ������ ���� �����!!! ����� ����� ������������!!!
*/
void KiIR_Protocols::SaveBit(U64 location, U32 value)
{
	LOGTOFILE("Start SaveBit, location", location);
	LOGTOFILE("Start SaveBit, value", value);

	/* ���������� ���������� ���� */
	mBitsForNextByte.push_back(std::pair< U64, U64 >(value, location));
	LOGTOFILE("SaveBit, Bits count", mBitsForNextByte.size());

	/* ���������� ���� �� ������� */
	if (value == 1)
	{
		mpResults->AddMarker(location, AnalyzerResults::One, mpSettings->mInputChannel);
	}
	else if (value == 0)
	{
		mpResults->AddMarker(location, AnalyzerResults::Zero, mpSettings->mInputChannel);
	}

	/* ��������� ������� ��� �� ��������� (�������� �������������) */
	// TODO ����� �����������, ��� ������� ������ ����!!!
	U32 bit_count = mp_CurrentProtocol->frameInfoVector[0].bitsCount;
	/* ���-�� ��� ������, ����� ���������� �� bit_count, ���� ���� ������ �� ������, �������� (�������� RC-5) */
	U32 data_bits_count = bit_count;
	/* ������ � �������, ������ ���������� ������, ��������� ��� RC-5, ��������� ����� ���������� ��� ���� */
	U32 bit_index = 0;

	if (peRC5 == mp_CurrentProtocol->protocolEnum)
	{
		/* ����� ������� ��� ����������� �� ���������� ����� � ������ toggle, ������ ��� ��� �� ��� �� ���������� �� �������� ����� */
		bit_count += mp_CurrentProtocol->bitLead.bitCount + 1;
		/*
		* ��������� ������ ������������ ������, ��������� �� toggle ���, ������� �� ���� ���������� � ������.
		* �� ������ ������: ��� ���� �������� ������ ��� MSB, ��� LSB ����� ����
		*/
		bit_index = mp_CurrentProtocol->bitLead.bitCount;

		/* ��������� toggle ��� */
		data_bits_count++;
	}
	else if ((peRC5X == mp_CurrentProtocol->protocolEnum)
		|| (peS100 == mp_CurrentProtocol->protocolEnum))
	{
		/* ����� ������� ��� ����������� �� 1 ���, (������ ��� ���� � ��������, �� ���������) � ������ toggle, ������ ��� ��� �� ��� �� ���������� �� �������� ����� */
		bit_count += mp_CurrentProtocol->bitLead.bitCount + 1;
		/*
		* ��������� ������ ������������ ������, ��������� �� toggle ���, ������� �� ���� ���������� � ������.
		* �� ������ ������: ��� ���� �������� ������ ��� MSB, ��� LSB ����� ����
		*/
		bit_index = mp_CurrentProtocol->bitLead.bitCount;

		/* ��������� toggle ��� */
		data_bits_count++;
	}
	else if (peNRC17 == mp_CurrentProtocol->protocolEnum)
	{
		/* ���������, ��� � ������ ������ ���� ��������� ��� ������ "1" */
		bit_count++;
		/* ������ ��� ����� ������������ */
		bit_index++;
	}
	/* ���� ������ ��������� ���, ������������ ��� ������������������ */
	if (mBitsForNextByte.size() == bit_count)
	{
		U64 lData = 0;
		if (bdMSB == mp_CurrentProtocol->bitsDirection)
		{
			for (U32 i = 0; i < data_bits_count; ++i, ++bit_index)
			{
				lData |= (mBitsForNextByte[bit_index].first << (data_bits_count - i - 1));
			}
		}
		else if (bdLSB == mp_CurrentProtocol->bitsDirection)
		{
			for (U32 i = 0; i < data_bits_count; ++i, ++bit_index)
			{
				lData |= (mBitsForNextByte[bit_index].first << i);
			}
		}
		if ((peRC5 == mp_CurrentProtocol->protocolEnum)
			|| (peRC5X == mp_CurrentProtocol->protocolEnum)
			|| (peS100 == mp_CurrentProtocol->protocolEnum))
		{
			data_bits_count--;
		}
		/* ��������� ����� ��� ����������� ���������� */
		Frame frame;
		frame.mData2 = data_bits_count;
		frame.mType = dtDATAU64;
		frame.mStartingSampleInclusive = mBitsForNextByte[0].second - mT;
		if (peRC5 == mp_CurrentProtocol->protocolEnum)
		{
			/* ������� ��� ���� */
			frame.mEndingSampleInclusive = mBitsForNextByte[1].second + mT;;
			frame.mData1 = 0x3;
			frame.mType = dtLEADS;
			mpResults->AddFrame(frame);

			/* ����� toggle */
			frame.mStartingSampleInclusive = mBitsForNextByte[2].second - mT;
			frame.mEndingSampleInclusive = mBitsForNextByte[2].second + mT;
			U8 lToggle = (0 != (lData & RC5_TOGGLEBIT_MASK) ? 1 : 0);
			frame.mData1 = lToggle;
			frame.mType = dtTOGGLE;
			mpResults->AddFrame(frame);

			/* ��������� - ������ */
			frame.mStartingSampleInclusive = mBitsForNextByte[3].second - mT;
			frame.mType = dtDATAWITHTOGGLE;
			frame.mFlags = lToggle;
		}
		else if (peRC5X == mp_CurrentProtocol->protocolEnum)
		{
			/* ������� ���� ��� */
			frame.mEndingSampleInclusive = mBitsForNextByte[0].second + mT;;
			frame.mData1 = 0x1;
			frame.mType = dtLEADS;
			mpResults->AddFrame(frame);

			/* ����� ������ ������ (7-� ��� �������) */
			frame.mStartingSampleInclusive = mBitsForNextByte[1].second - mT;
			frame.mEndingSampleInclusive = mBitsForNextByte[1].second + mT;
			frame.mData1 = (0 != (lData & RC5X_EXTRADATA_MASK) ? 1 : 0);
			frame.mType = dtEXTRADATA;
			mpResults->AddFrame(frame);

			/* ����� Toggle */
			frame.mStartingSampleInclusive = mBitsForNextByte[2].second - mT;
			frame.mEndingSampleInclusive = mBitsForNextByte[2].second + mT;
			U8 lToggle = (0 != (lData & RC5X_TOGGLEBIT_MASK) ? 1 : 0);
			frame.mData1 = lToggle;
			frame.mType = dtTOGGLE;
			mpResults->AddFrame(frame);

			/* ��������� - ������ */
			frame.mStartingSampleInclusive = mBitsForNextByte[3].second - mT;
			frame.mType = dtDATAWITHEXTRA;
			frame.mFlags = lToggle;

			/* ����������� ������ ������ ���� */
			lData ^= RC5X_EXTRADATA_MASK;
		}
		else if (peS100 == mp_CurrentProtocol->protocolEnum)
		{
			/* ������� ���� ��� */
			frame.mEndingSampleInclusive = mBitsForNextByte[0].second + mT;;
			frame.mData1 = 0x1;
			frame.mType = dtLEADS;
			mpResults->AddFrame(frame);

			/* ����� ������ ������ (8-� ��� �������) */
			frame.mStartingSampleInclusive = mBitsForNextByte[1].second - mT;
			frame.mEndingSampleInclusive = mBitsForNextByte[1].second + mT;
			frame.mData1 = (0 != (lData & S100_EXTRADATA_MASK) ? 1 : 0);
			frame.mType = dtEXTRADATA;
			mpResults->AddFrame(frame);

			/* ����� Toggle */
			frame.mStartingSampleInclusive = mBitsForNextByte[2].second - mT;
			frame.mEndingSampleInclusive = mBitsForNextByte[2].second + mT;
			U8 lToggle = (0 != (lData & S100_TOGGLEBIT_MASK) ? 1 : 0);
			frame.mData1 = lToggle;
			frame.mType = dtTOGGLE;
			mpResults->AddFrame(frame);

			/* ��������� - ������ */
			frame.mStartingSampleInclusive = mBitsForNextByte[3].second - mT;
			frame.mType = dtDATAWITHEXTRA;
			frame.mFlags = lToggle;

			/* ����������� ������ ������ ���� */
			lData ^= S100_EXTRADATA_MASK;
		}
		else if (peNRC17 == mp_CurrentProtocol->protocolEnum)
		{
			frame.mEndingSampleInclusive = mBitsForNextByte[0].second + mT;;
			frame.mData1 = 0x1;
			frame.mType = dtSTARTBIT;
			mpResults->AddFrame(frame);

			frame.mStartingSampleInclusive = mBitsForNextByte[1].second - mT;
			frame.mType = dtDATAU64;
		}
		frame.mEndingSampleInclusive = location + mT;
		/*
		* mData1 �������� ������ ���������� �����, �.�. ��������� "������" ����, � ��� ����� ����, ��������
		* � ��������� RC-5, ��� ���� toggle ���, ������� �� �������� � ������, ����� ��������.
		*/
		if (peRC5X == mp_CurrentProtocol->protocolEnum)
		{
			/* �������� (����������) toggle ��� */
			frame.mData1 = (lData & ~RC5X_TOGGLEBIT_MASK) & RC5X_FULLDATA_MASK;
		}
		else if (peS100 == mp_CurrentProtocol->protocolEnum)
		{
			/* �������� (����������) toggle ��� */
			frame.mData1 = (lData & ~S100_TOGGLEBIT_MASK) & S100_FULLDATA_MASK;
		}
		else
		{
			/* �������� ������, �������� ������ ������ ���-�� ����� */
			frame.mData1 = lData & ((1ull << data_bits_count) - 1ull);
		}
		mpResults->AddFrame(frame);
		/* ���������� ����������� ������ */
		mBitsForNextByte.clear();
		/* ��������� ��������� */
		mpResults->CommitResults();
		mpKiIRAnalyzer->ReportProgress(mpChannelData->GetSampleNumber());
	}
	/* ���� ��� �� ��������� ���, �� ��������� */
}

/*
* ����� ���� ����������� ��������.
* ����������, ���� �����, ��� ������������� ������.
*/
void KiIR_Protocols::Invalidate()
{
	LOGTOFILE("Invalidate now", 0);
	mSynchronized = false;
	mBitsForNextByte.clear();
	mUnsyncedLocations.clear();
}

/*
* ����� ������ ������������������
*/
bool KiIR_Protocols::searchSynchro()
{
	bool lResult = false;
	if (!mp_CurrentProtocol)
	{
		return lResult;
	}

	// ���������� true, ���� Synchro �� ������������
	if (!mp_CurrentProtocol->needSynchro)
	{
		return true;
	}

	KiBitAnalyzeInfo lBitInfo(mp_CurrentProtocol->synchroInfo.bitInfo, mp_CurrentProtocol->needSynchro, mpSettings->mTolerance);
	KiBitAnalyzeInfo lBitFinal(mp_CurrentProtocol->synchroInfo.bitInfo, mp_CurrentProtocol->needSynchro, mpSettings->mTolerance);
	BitState lCurrentBitState = mDataCluster->getStartBitState();
	int lBitsCount = mp_CurrentProtocol->synchroInfo.infoBitsCount;
	if (!lBitsCount)
	{
		lBitsCount = 1;
	}
	lBitsCount += lBitsCount;
	/* ���� ��������� ��������� ���, �� ����������� ������ �� ���� ��� ������ */
	if (mp_CurrentProtocol->synchroInfo.needFinalBit)
	{
		lBitsCount += 2;
	}

	int lBitIndex = 0;
	U64 lOffsetIndex = 0;
	U64 lFinalOffset = 0;
	OneDistanceInfoVector &lVectorData = mDataCluster->getVectorData();

	LOGTOFILE("&&& Start search Synchro", 0);

	DataType lFinalDataType = dtUNKNOWN;

	for (auto lIter = lVectorData.begin(); (lIter != lVectorData.end()) && lBitsCount; ++lIter, ++lBitIndex, --lBitsCount, lOffsetIndex++)
	{
		OneDistanceInfo &lDistanceInfo = *lIter;

		LOGTOFILE("&&& Check Synchro, lBitsCount", lBitsCount);

		lBitIndex %= 2;
		/* �������� ������� ������� ���� */
		if (lBitInfo.checkBit(lBitIndex, lDistanceInfo.mDistance))
		{
			LOGTOFILE("&&& Check Synchro, found Bit", 1);
			lDistanceInfo.mDataType = dtSYNCHRODATA;
			lFinalDataType = dtSYNCHRODATA;
			lFinalOffset = lOffsetIndex;
		}
		/* ���� �� ��������, �� ��������� ��������� ���, ������ ����� ����� ���������� ���� */
		else if (lBitFinal.checkFinalBit(lBitIndex, lDistanceInfo.mDistance))
		{
			LOGTOFILE("&&& Check Synchro, found Final Bit", 1);
			lDistanceInfo.mDataType = dtSYNCHROPAUSE;
			lBitInfo.setRecognized();
			lFinalDataType = dtSYNCHROPAUSE;
		}
		/* ���� ������ �� ����������, �� � ������ ������ */
		if (dtUNKNOWN == lDistanceInfo.mDataType)
		{
			lFinalDataType = dtUNKNOWN;
			break;
		}
	}
	if (lBitInfo.isRecognized() && dtSYNCHROPAUSE == lFinalDataType && lOffsetIndex > 0)
	{
		LOGTOFILE("=== Synchro recognized, OffsetIndex", lOffsetIndex);
		Frame lFrame;
		lFrame.mData1 = 1;
		lFrame.mType = dtSYNCHRODATA;
		lFrame.mFlags = 0;
		lFrame.mStartingSampleInclusive = lVectorData[0].mSampleNumber;
		lFrame.mEndingSampleInclusive = lVectorData[lOffsetIndex - 1].mSampleNumber;

		mpResults->AddFrame(lFrame);
		// mpResults->AddMarker(lFrame.mStartingSampleInclusive, AnalyzerResults::MarkerType::Start, mpSettings->mInputChannel);
		mpResults->CommitResults();
		mProtocolData.mSynchroOk = true;

		mDataCluster->setVectorDataOffset(lOffsetIndex + 1);

		return true;
	}
	return false;
}

/* ����� Lead */
DataType KiIR_Protocols::searchLead()
{
	if (!mp_CurrentProtocol)
	{
		return dtUNKNOWN;
	}

	// ���������� dtLEADS, ���� Lead �� ������������
	if (!mp_CurrentProtocol->needLeadBits)
	{
		return dtLEADS;
	}

	KiBitAnalyzeInfo lLeadBit(mp_CurrentProtocol->bitLead, mp_CurrentProtocol->needLeadBits, mpSettings->mTolerance);
	KiBitAnalyzeInfo lRepeatLeadBit(mp_CurrentProtocol->bitRepeatLead, mp_CurrentProtocol->needRepeatLeadBits, mpSettings->mTolerance);
	BitState lCurrentBitState = mDataCluster->getStartBitState();
	int lBitsCount = mp_CurrentProtocol->bitLead.bitCount;
	if (!lBitsCount)
	{
		lBitsCount = 1;
	}
	/* ���������� ��������� ������� */
	lBitsCount += lBitsCount;

	int lBitIndex = 0;
	OneDistanceInfoVector &lVectorData = mDataCluster->getVectorData();
	U64 lOffsetIndex = 0;
	U64 lStartOffset = 0;
	bool lFillStarted = false;

	LOGTOFILE("&&& Start search Lead, lBitsCount", lBitsCount);
	LOGTOFILE("&&& Start search Lead, lVectorData.size()", lVectorData.size());

	for (U64 i = 0; (i < lVectorData.size()) && lBitsCount; ++i, ++lBitIndex, /*--lBitsCount, */ lOffsetIndex++)
	{
		OneDistanceInfo &lDistanceInfo = lVectorData[i];
		if (dtUNKNOWN != lDistanceInfo.mDataType)
		{
			LOGTOFILE("&&& Start search Lead, found filled data, lBitsCount", lBitsCount);

			if (lFillStarted)
			{
				lLeadBit.clear();
				lRepeatLeadBit.clear();
				break;
			}

			lStartOffset++;
			continue;
		}

		if (!lFillStarted)
		{
			lFillStarted = true;
		}

		LOGTOFILE("&&& Check Lead, lBitsCount", lBitsCount);

		lBitIndex %= 2;
		if (lLeadBit.checkBit(lBitIndex, lDistanceInfo.mDistance))
		{
			lDistanceInfo.mDataType = dtLEADS;
			LOGTOFILE("&&& Check Lead, found dtLEADS", 1);
		}
		if (lRepeatLeadBit.checkBit(lBitIndex, lDistanceInfo.mDistance))
		{
			lDistanceInfo.mDataType = dtREPEATLEADS;
			LOGTOFILE("&&& Check Lead, found dtREPEATLEADS", 1);
		}
		/* ���� ������ �� ����������, �� � ������ ������ */
		if (dtUNKNOWN == lDistanceInfo.mDataType)
		{
			LOGTOFILE("&&& Check Lead, NOT FOUND", 0);
			break;
		}
		lBitsCount--;
	}
	LOGTOFILE("&&& End search Lead, OffsetIndex", lOffsetIndex);
	if (lLeadBit.isRecognized())
	{
		LOGTOFILE("=== LeadBit recognized, StartOffset", lStartOffset);
		Frame lFrame;
		lFrame.mData1 = 1;
		lFrame.mType = dtLEADS;
		lFrame.mFlags = 0;
		lFrame.mStartingSampleInclusive = lVectorData[lStartOffset].mSampleNumber;
		lFrame.mEndingSampleInclusive = lVectorData[lOffsetIndex].mSampleNumber;

		mpResults->AddFrame(lFrame);
		// mpResults->AddMarker(lFrame.mStartingSampleInclusive, AnalyzerResults::MarkerType::Start, mpSettings->mInputChannel);
		mpResults->CommitResults();
		mProtocolData.mLeadsOk = true;

		return dtLEADS;
	}
	else if (lRepeatLeadBit.isRecognized())
	{
		LOGTOFILE("=== RepeatLeadBit recognized", 1);
		Frame lFrame;
		lFrame.mData1 = 1;
		lFrame.mType = dtREPEATLEADS;
		lFrame.mFlags = 0;
		lFrame.mStartingSampleInclusive = lVectorData[lStartOffset].mSampleNumber;
		// lFrame.mEndingSampleInclusive = lVectorData[lOffsetIndex].mSampleNumber;
		lFrame.mEndingSampleInclusive = mpChannelData->GetSampleNumber();

		mpResults->AddFrame(lFrame);
		// mpResults->AddMarker(lFrame.mStartingSampleInclusive, AnalyzerResults::MarkerType::Start, mpSettings->mInputChannel);
		mpResults->CommitResults();
		mProtocolData.mRepeatLeadsOk = true;
		return dtREPEATLEADS;
	}
	return dtUNKNOWN;
}

/* ������������� ������ (Lead ��� ��������). ���������� � Lead ������� � mp_CurrentProtocol. */

// TODO � ��������� ����� � ������� �� ������� ������� ���������� ���� (������ ��� SIRC) ���� ��������, ���� ������� ����� ���������� �� 1, ��� �����������

bool KiIR_Protocols::recognizeData(U8 pFrameIndex)
{
	bool lResult = false;
	if (!mp_CurrentProtocol)
	{
		return lResult;
	}

	KiBitAnalyzeInfo lBitZero(mp_CurrentProtocol->bitZero, true, mpSettings->mTolerance);
	KiBitAnalyzeInfo lBitOne(mp_CurrentProtocol->bitOne, true, mpSettings->mTolerance);
	KiBitAnalyzeInfo lBitFinal(mp_CurrentProtocol->bitFinal, mp_CurrentProtocol->needFinalBit, mpSettings->mTolerance);

	BitState lCurrentBitState = mDataCluster->getStartBitState();

	int lBitIndex = 0;
	int lDataBitCount = 0;
	bool lLastBitIsData = false;
	U64 lCenterLocation = 0;
	U64 lStartOffset = 0;
	U64 lFinalOffset = 0;
	U64 lEndOffset = 0;
	bool lStartRecognized = false;
	DataType lLastDataType = dtUNKNOWN;

	OneDistanceInfoVector &lVectorData = mDataCluster->getVectorData();
	LOGTOFILE("### VectorData BitCount", lVectorData.size());
	for (U32 i = 0; i < lVectorData.size(); ++i, lEndOffset++)
	{
		OneDistanceInfo &lDistanceInfo = lVectorData[i];

		if (dtUNKNOWN != lDistanceInfo.mDataType)
		{
			if (lStartRecognized)
			{
				/* ������ */
				lBitOne.clear();
				lBitZero.clear();
				lBitFinal.clear();
				break;
			}
			lStartOffset++;
			continue;
		}

		if (!lStartRecognized)
		{
			lStartRecognized = true;
		}

		if (0 != (lBitIndex % 2))
		{
			lCenterLocation = lDistanceInfo.mSampleNumber;
		}

		LOGTOFILE("### Current VectorData Index for Unknown data", i);

		int lBitValue = -1;
		if (lBitOne.checkBit(lBitIndex, lDistanceInfo.mDistance))
		{
			// TODO ����� ��� ������, �������� ������ ���� ������
			lDistanceInfo.mDataType = dtDATAVECTOR;
			lLastDataType = dtDATAVECTOR;
			lBitValue = 1;
			if (0 == lBitIndex)
			{
				lFinalOffset = i;
			}
		}
		if (lBitZero.checkBit(lBitIndex, lDistanceInfo.mDistance))
		{
			// TODO ����� ��� ������, �������� ������ ���� ������
			lDistanceInfo.mDataType = dtDATAU64;
			lLastDataType = dtDATAVECTOR;
			lBitValue = 0;
			if (0 == lBitIndex)
			{
				lFinalOffset = i;
			}
		}

		if (1 == lBitIndex)
		{
			lDataBitCount += (lBitOne.isRecognized() || lBitZero.isRecognized() ? 1 : 0);
		}

		if (mp_CurrentProtocol->needFinalBit)
		{
			if (lBitFinal.checkFinalBit(lBitIndex, lDistanceInfo.mDistance))
			{
				if (lBitValue < 0)
				{
					lDistanceInfo.mDataType = dtFINALBIT;
					lLastDataType = dtFINALBIT;
					lBitValue = -99;
				}
			}
		}
		/* ���� ������ �� ����������, �� � ������ ������ */
		if (dtUNKNOWN == lDistanceInfo.mDataType)
		{
			LOGTOFILE("=== Data is UNKNOWN", i);
			break;
		}
		if (mp_CurrentProtocol->needFinalBit)
		{
			if (dtFINALBIT == lDistanceInfo.mDataType)
			{
				LOGTOFILE("=== FinalBit, deleteLastBit", i);
				// lFinalOffset = lEndOffset;

				/* ���� ���� ��������� ���, �� �� ������� � ������ � ��� ����� �������, ������� mBitCount ���������� ������ ������ */
				mProtocolData.deleteLastBit();
				break;
			}
		}
		else
		{
#if 1
			/*
			* ��������� ��� ����� ���� "����������", �� �����������, ������ ��������� ��������� �� ��� ���
			* � ������������, �������� ���� (�� ������ ����� ��������), �� ������������� �������������
			* ������� ����, ��� ��� ���������.
			*/
			// if (i == (lVectorData.size() - 1) && !lBitOne.isRecognized() && !lBitZero.isRecognized())
			if (lDataBitCount == mp_CurrentProtocol->frameInfoVector[pFrameIndex].bitsCount && !lBitOne.isRecognized() && !lBitZero.isRecognized())
			{
				if (1 == lBitValue)
				{
					lBitOne.setRecognized();
					lLastBitIsData = true;
				}
				else if (0 == lBitValue)
				{
					lBitZero.setRecognized();
					lLastBitIsData = true;
				}
			}
#endif
		}

		if (dtDATAVECTOR == lLastDataType) // lBitOne.isRecognized() || lBitZero.isRecognized())
		{
			mProtocolData.mBitCount++;
			LOGTOFILE("### Current BitCount", mProtocolData.mBitCount);
			if (lBitOne.isRecognized())
			{
				/* �������� ��� 1 */
				mProtocolData.addBit(1);
				mpResults->AddMarker(lCenterLocation, AnalyzerResults::One, mpSettings->mInputChannel);
				// LOGTOFILE("### Add One, Data size", mProtocolData.getVectorData(mp_CurrentProtocol->bitsDirection).size());
			}
			else if (lBitZero.isRecognized())
			{
				/* �������� ��� 0 */
				mProtocolData.addBit(0);
				mpResults->AddMarker(lCenterLocation, AnalyzerResults::Zero, mpSettings->mInputChannel);
				// LOGTOFILE("### Add Zero, Data size", mProtocolData.getVectorData(mp_CurrentProtocol->bitsDirection).size());
			}
		}
		lCurrentBitState = (bcMARK == lCurrentBitState ? bcSPACE : bcMARK);
		++lBitIndex;
		if (lBitIndex > 1)
		{
			lBitIndex = 0;
			lBitOne.clear();
			lBitZero.clear();
			lBitFinal.clear();
		}
	}

	if (lStartOffset >= 0
		&& (lEndOffset > lStartOffset))
	{
		// LOGTOFILEX("=== Data recognized, Data size", mProtocolData.getVectorData(mp_CurrentProtocol->bitsDirection).size());
		Frame lFrame;

		lFrame.mData1 = mpResults->AddBlock(mProtocolData.getVectorData(mp_CurrentProtocol->bitsDirection));

		LOGTOFILE("=== Data recognized, Data index", lFrame.mData1);

		lFrame.mType = dtDATAVECTOR;

		lFrame.mData2 = (mProtocolData.mBitCount + 1) / 2;
		lFrame.mFlags = 0;
		lFrame.mStartingSampleInclusive = lVectorData[lStartOffset].mSampleNumber;
		if (mp_CurrentProtocol->needFinalBit)
		{
			lFrame.mEndingSampleInclusive = lVectorData[lFinalOffset].mSampleNumber;
		}
		else
		{
			U64 lLastSampleNumber = mDataCluster->getFinalSampleNumber();
#if 1
			if (lLastBitIsData)
			{
				/* ��������� ��������, ������� ����������� �� ��������� ������ ������. ���� ����� ��������� (SIRC) */
				lLastSampleNumber += static_cast<U64>(RECALC_TIME(mp_CurrentProtocol->bitOne.timesInfo.secondPeriodUs, mSampleRateHz));

				/* DEBUG ONLY */
				//U64 lDelta = static_cast<U64>(RECALC_TIME(mp_CurrentProtocol->bitOne.timesInfo.secondPeriodUs, mSampleRateHz));
				//LOGTOFILE("$$$ Delta", lDelta);
				//LOGTOFILE("$$$ BitOne firstPeriodUS", mp_CurrentProtocol->bitOne.timesInfo.firstPeriodUs);
				//LOGTOFILE("$$$ BitOne secondPeriodUS", mp_CurrentProtocol->bitOne.timesInfo.secondPeriodUs);
				//LOGTOFILE("$$$ BitZero firstPeriodUS", mp_CurrentProtocol->bitZero.timesInfo.firstPeriodUs);
				//LOGTOFILE("$$$ BitZero secondPeriodUS", mp_CurrentProtocol->bitZero.timesInfo.secondPeriodUs);
				//LOGTOFILE("$$$ FinalSampleNumber", mDataCluster->getFinalSampleNumber());
				//LOGTOFILE("$$$ LastSampleNumber", lLastSampleNumber);
				//lDelta = static_cast<U64>(RECALC_STOUS(lDelta, mSampleRateHz));
				//// #define RECALC_TIME(x,s)	((x)*(s)/1000000.0)
				//LOGTOFILE("$$$ Second PeriodUS", mp_CurrentProtocol->bitOne.timesInfo.secondPeriodUs);
				//LOGTOFILE("$$$ mSampleRateHz", mSampleRateHz);
				//LOGTOFILE("$$$ Delta in US", lDelta);
			}
#endif
			lFrame.mEndingSampleInclusive = lLastSampleNumber;
		}

		mpResults->AddFrame(lFrame);
		if (!lBitFinal.isRecognized())
		{
			mpResults->CommitPacketAndStartNewPacket();
		}
		mpResults->CommitResults();
		mProtocolData.mDataOk = true;
	}
	if (lBitFinal.isRecognized())
	{
		LOGTOFILE("=== Data Final Bit recognized. Final end sample", mDataCluster->getFinalSampleNumber());
		Frame lFrame;
		lFrame.mData1 = 1;
		lFrame.mType = dtFINALBIT;
		lFrame.mFlags = 0;
		lFrame.mStartingSampleInclusive = lVectorData[lFinalOffset].mSampleNumber;
		lFrame.mEndingSampleInclusive = mDataCluster->getFinalSampleNumber();

		mpResults->AddFrame(lFrame);
		mpResults->CommitPacketAndStartNewPacket();
		mpResults->CommitResults();
		return true;
	}
	return mProtocolData.mDataOk;
}

#if 0
bool KiIR_Protocols::fillDataDAIKIN()
{
	mDataCluster->clear();

	U64 lSumLimitWidth = 0;
	const LogicalBit *lpBit = nullptr;
	BitState lActiveBitLevel = bcMARK;

	mDataCluster->setStartSampleNumber(mpChannelData->GetSampleNumber());
	if (mpChannelData->GetBitState() != lActiveBitLevel)
	{
		mDataCluster->setStartExtraSampleExists(true);
		mpChannelData->AdvanceToNextEdge();
	}

	mDataCluster->setStartBitState(mpChannelData->GetBitState());
	int lCount = 0;
	while (mpChannelData->DoMoreTransitionsExistInCurrentData())
	{
		U64 lCurrentPosition = mpChannelData->GetSampleNumber();
		U64 lNextPosition = mpChannelData->GetSampleOfNextEdge();
		U64 lDistance = static_cast<U64>(RECALC_STOUS(lNextPosition - lCurrentPosition, mSampleRateHz));
		LOGTOFILE("*** Distance us", lDistance);

		mDataCluster->push_back({ dtUNKNOWN, lCurrentPosition, lDistance });
		mpChannelData->AdvanceToNextEdge();
		/* ��������� ����� ���������� ������, ��������, ��� ���������� ���� */
		mDataCluster->setFinalSampleNumber(mpChannelData->GetSampleNumber());
	}
	LOGTOFILE("*** Vector size", mDataCluster->size());
	return !mDataCluster->isEmpty();
}
#endif

