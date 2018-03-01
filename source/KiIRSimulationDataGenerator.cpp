#include "KiIRSimulationDataGenerator.h"
#include "KiIRAnalyzerSettings.h"
#include "KiIR_Protocols.h"

#include <AnalyzerHelpers.h>

#include "DebugLog.h"

//#define NANO_SEC		(1E-9)
#define MICRO_SEC		(1E-6)
//#define MILLI_SEC		(1E-3)

KiIRSimulationDataGenerator::KiIRSimulationDataGenerator()
{
}

KiIRSimulationDataGenerator::~KiIRSimulationDataGenerator()
{
}

void KiIRSimulationDataGenerator::Initialize( U32 simulation_sample_rate, KiIRAnalyzerSettings* settings )
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;

	/* ��������� ������������ �� 1 ���, ����� ���� ��� - 1us. �� �����, ����� ������������� ����� � ������� (������) ��� ������������ �������� */
	mClockGenerator.Init(1000000.0, mSimulationSampleRateHz);

	mKiIRSimulationData.SetChannel( mSettings->mInputChannel );
	mKiIRSimulationData.SetSampleRate(simulation_sample_rate);
	mKiIRSimulationData.SetInitialBitState(bcSPACE);
}

U32 KiIRSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel )
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

	/* ���������� �������� ����� �� ������� ������, ����� �� �������� "�� �����", ������ �� ����� �������������� � ��� ���� �� ����� ������������ ������������ */
	mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * 10000));

	/* ���������� ���������� ������� (������� ������) */
	while (mKiIRSimulationData.GetCurrentSampleNumber() < adjusted_largest_sample_requested)
	{
		/* ������������ ������ � ����� ����� ����(!) ������� ����� ���� �� ���� (��������, � Sharp ��� ������) */
		FormFrame(mSettings->mProtocol);
	}

	*simulation_channel = &mKiIRSimulationData;
	return 1;
}

/// <summary>
/// ������������ ����� �������. ��� ����� �������� �� ���������� ������� (� Sharp - ��� ������). ���� �������� �� ������, ��������� �� ����� ������� ������� ���������.
/// </summary>
/// <param name="pProtocol">������������� ���������.</param>
void KiIRSimulationDataGenerator::FormFrame(ProtocolEnum pProtocol)
{
	static U64 lData = 0x0123456789ABCDEFull - 3ull;
	// static U64 lData = 0ull;
	lData += 3ull;
	switch (pProtocol)
	{
	case peMAXID:								/* ��������� �� ����� ������� ��� ������� ��������� */
		for (int i = 0; AllProtocols[i]; ++i)
		{
			FormFrame(AllProtocols[i], lData);
			if (AllProtocols[i]->needRepeat)
			{
				/* �������� ���� ������� */
				FormFrame(AllProtocols[i], lData, 0, true);
				/* ��������� �������� � ����� �������, ����� ��� ���� ����� ������ �������� ��� ������� */
				mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * AllProtocols[i]->repeatInfo.repeatTime));
			}
			else
			{
				/* ��������� �������� � ����� �������, 10 ms */
				mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * 10000));
			}
		}
		break;
	default:									/* ��������� ���� ������� ��� ����������� ��������� */
		/* �������� ����� ������� */
		FormFrame(AllProtocols[pProtocol], lData);
		if (AllProtocols[pProtocol]->needRepeat)
		{
			/* �������� ���� ������� */
			FormFrame(AllProtocols[pProtocol], lData, 0, true);
			/* ��������� �������� � ����� �������, ����� ��� ���� ����� ������ �������� ��� ������� */
			mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * AllProtocols[pProtocol]->repeatInfo.repeatTime));
		}
		else
		{
			/* ��������� �������� � ����� �������, 10 ms */
			mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * 10000));
		}
		break;
	}
}

/// <summary>
/// ������������ ����� ������� ���������� ��������� (����� �������� �� ���������� �������).
/// </summary>
/// <param name="ppProtocol">��������� �� �������� ���������.</param>
void KiIRSimulationDataGenerator::FormFrame(const Protocol *ppProtocol, U64 pData, U8 pFrameIndex /*= 0*/, bool pSendRepeat /*= false*/)
{
	U64 lData = pData;
	/* ���������� ��������� �����. ����� ��� ������������ ����� ����� ������ � ��������� ���������� */
	U32 lSamplesBetweenFrames = 0;
	U64 lStartSampleNumber = mKiIRSimulationData.GetCurrentSampleNumber();
	if (ppProtocol->needRepeat)
	{
		/*
		* ���� ������ � ��������� ����������, ������������ ���������� ������� �� ������ ������.
		* ����� �� ����������, ���� ��� ������� ������������ �������� ����� �������� (Gap)
		*/
		lSamplesBetweenFrames = mClockGenerator.AdvanceByTimeS(MICRO_SEC * ppProtocol->repeatInfo.repeatTime);
	}

	/* ������� ������ ������������������, ������ ��� ������ �������, ��� �������� � ��� */
	if (ppProtocol->needSynchro && (0 == pFrameIndex))
	{
		FormSynchro(ppProtocol);
	}
	
	/* ������� Leads ���� �����. � ��������� ���������� ��� ���. � ������ ��� ������� ������� �� ���������� ��������� �������. */
	if (ppProtocol->needLeadBits)
	{
		// FormLeads
		FormLeads(ppProtocol, pSendRepeat);
	}
	/*
	* ��������� ��������� ������� ������, ���� ���� ��� ������
	*/
	if (!pSendRepeat
		|| (ppProtocol->needRepeat && ppProtocol->repeatInfo.repeatType != rtSpecialLead))
	{
		/* ��� ��������� ������ ���������� ������� ��� �� ���������, � �������� ������� ����� ���� ������ � ���������� ������, � ����� - ����� ����������� */
		int lBitsCount = ppProtocol->frameInfoVector[pFrameIndex].bitsCountForGenerator; // bitsCountForGenerator;
		if ((peRC5 == ppProtocol->protocolEnum)
			|| (peRC5X == ppProtocol->protocolEnum)
			|| (peS100 == ppProtocol->protocolEnum))
		{
			lBitsCount++;	// ������ ��������� �������� ������������� toggle ���
		}
		U64 lMask = (bdLSB == ppProtocol->bitsDirection ? 0x01UL : 0x1UL << (lBitsCount - 1));
		if (((peRC5 == ppProtocol->protocolEnum) || (peRC5X == ppProtocol->protocolEnum))
			&& pSendRepeat)
		{
			/* ����������� toggle ��� */
			lData ^= RC5_TOGGLEBIT_MASK;
		}
		if ((peS100 == ppProtocol->protocolEnum)
			&& pSendRepeat)
		{
			/* ����������� toggle ��� */
			lData ^= S100_TOGGLEBIT_MASK;
		}
		if (peRCA == ppProtocol->protocolEnum)
		{
			/*
			* ������������ ��������� ������, ��� ������, ��� ������� 12 ��� ���������� ����� �� 12 ��� � ������� 12 ��� �������������.
			*/
			// U8 lLen = ppProtocol->bitsCount / 2;
			U8 lLen = ppProtocol->frameInfoVector[pFrameIndex].bitsCount / 2; // bitsCount / 2;
			lData &= (1 << lLen) - 1;				// ��������� ������ ������� 12 ���
			lData |= lData << lLen;					// �������� ������� 12 ��� � ��������� 12 (���������� 24 ����)
			lData ^= (1 << lLen) - 1;				// ������� 12 ��� �����������
		}
		if (peNRC17 == ppProtocol->protocolEnum)
		{
			/* ��������� ��� ������ "1" */
			// lData |= (1ull << (lBitsCount - 2));
			// lData = 0x13A68ull;
			// lData = 0x15C61ull;
			LOGTOFILEX("*** NRC17, Data", lData);
			LOGTOFILEX("*** NRC17, Mask", lMask);
		}
		LOGTOFILE("*** BitCount", lBitsCount);
		for (int i = 0; i < lBitsCount; ++i)
		{
			if ((0 == i)
				&& (peNRC17 == ppProtocol->protocolEnum))
			{
				// �������
				FormBit(ppProtocol->bitOne);
			}
			if (lData & lMask)
			{
				// �������
				FormBit(ppProtocol->bitOne);
			}
			else
			{
				// ����
				FormBit(ppProtocol->bitZero);
			}
			if (bdLSB == ppProtocol->bitsDirection)
			{
				lMask <<= 1;
			}
			else
			{
				lMask >>= 1;
			}
		}
	}
	
	/* ���� �����, ������� ��������� ��� */
	if (ppProtocol->needFinalBit)
	{
		// FormBit(ppProtocol->bitFinal);
		FormFinalBit(ppProtocol->bitFinal);
	}
	
	/* ��������� ������� ������, ������������� � SPACE � ������ �������� */
	mKiIRSimulationData.TransitionIfNeeded(bcSPACE);
	if (ppProtocol->needRepeat)
	{
		/* ��� ����������, ����� �������� ����� �� ����� ������ */
		if (ritGap == ppProtocol->repeatInfo.repeatIntevalType)
		{
			mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * ppProtocol->repeatInfo.repeatTime));
		}
		else
		{
			lStartSampleNumber += lSamplesBetweenFrames;
			lStartSampleNumber -= mKiIRSimulationData.GetCurrentSampleNumber();
			mKiIRSimulationData.Advance(U32(lStartSampleNumber));
		}
	}
	else
	{
		/* ��������� ��� ���������� � ������� � ���������, ��������� ������������� �������� � 10ms */
		// mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * 10000));
		mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * ppProtocol->frameInfoVector[pFrameIndex].pauseAfterFrameUs));
	}
	/* ��������� �������� */
	if (pFrameIndex > 0)
	{
		return;
	}
	/* ������� ������, ��� ���� */
	// if (ppProtocol->frameCount > 1)
	if (ppProtocol->frameInfoVector.size() > 1)
	{
		for (auto lFrameIndex = 1; lFrameIndex < ppProtocol->frameInfoVector.size(); lFrameIndex++)
		{
			lData = ~lData;
			FormFrame(ppProtocol, lData, lFrameIndex);
		}
#if 0
		if (peSHARP == ppProtocol->protocolEnum)
		{
			lData = ~lData;
			FormFrame(ppProtocol, lData, true);
		}
		else if (peDAIKIN == ppProtocol->protocolEnum)
		{
		}
#endif
	}
}

/// <summary>
/// ������������ Leads. ��� ����� ���� ��� ���� ���, ��� � ���������.
/// </summary>
/// <param name="ppProtocol">�������� ���������.</param>
void KiIRSimulationDataGenerator::FormLeads(const Protocol *ppProtocol, bool pSendRepeat /*= false*/)
{
	/* ������������ Lead ��� ������ ������� */
	if (pSendRepeat)
	{
		if (ppProtocol->needRepeat)
		{
			switch (ppProtocol->repeatInfo.repeatType)
			{
			case rtCodeWithoutLead:
				/* ������ - ��� ������� ��� Leads */
				return;
			case rtSpecialLead:
				/* ��� ���� ������� ���� ����������� Lead */
				FormBit(ppProtocol->bitRepeatLead, ppProtocol->bitRepeatLead.bitCount);
				break;
			case rtNormal:
				/* ������� �������, ��������� Lead */
				FormBit(ppProtocol->bitLead, ppProtocol->bitLead.bitCount);
				break;
			}
		}
		else
		{
			/* ���� � ��������� ����������� ����������� �������� �������, �� ������� ������� Lead */
			FormBit(ppProtocol->bitLead, ppProtocol->bitLead.bitCount);
		}
	}
	else
	{
		/* ���� ��� �� ������, �� ������� ������� Lead */
		FormBit(ppProtocol->bitLead, ppProtocol->bitLead.bitCount);
	}
}

/// <summary>
/// ������������ ������ ������������������.
/// </summary>
/// <param name="ppProtocol">�������� ���������.</param>
void KiIRSimulationDataGenerator::FormSynchro(const Protocol *ppProtocol)
{
	const SynchroInfo &lSynchroInfo = ppProtocol->synchroInfo;
	if (lSynchroInfo.infoBitsCount != 0)
	{
		FormBit(lSynchroInfo.bitInfo, lSynchroInfo.infoBitsCount);
	}
	if (lSynchroInfo.needFinalBit)
	{
		FormFinalBit(lSynchroInfo.bitInfo);
	}
	if (0 != lSynchroInfo.pauseLengthUs)
	{
		mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * lSynchroInfo.pauseLengthUs));
	}
}


/// <summary>
/// ������������ ������ ����. ��� ������ ��� transition.
/// </summary>
/// <param name="pLogicalBit">�������� ���� �� ��������� �������� ���������.</param>
void KiIRSimulationDataGenerator::FormBit(const LogicalBit &pLogicalBit, U64 pRepeatCount /* = 0 */)
{
	if (0 == pRepeatCount)
	{
		pRepeatCount = 1;
	}
	for (U64 i = 0; i < pRepeatCount; i++)
	{
		// mKiIRSimulationData.TransitionIfNeeded((bcMARK == pLogicalBit.activeLevel ? bcMARK : bcSPACE));
		mKiIRSimulationData.TransitionIfNeeded(pLogicalBit.activeLevel);
		mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * pLogicalBit.timesInfo.firstPeriodUs));
		mKiIRSimulationData.Transition();
		mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * pLogicalBit.timesInfo.secondPeriodUs));
	}
}

void KiIRSimulationDataGenerator::FormFinalBit(const LogicalBit &pFinalBit)
{
	mKiIRSimulationData.TransitionIfNeeded(pFinalBit.activeLevel);
	mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * pFinalBit.timesInfo.firstPeriodUs));
	mKiIRSimulationData.Transition();
}
