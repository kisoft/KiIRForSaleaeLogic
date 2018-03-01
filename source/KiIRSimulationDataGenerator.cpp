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

	/* Генератор рассчитываем на 1 МГц, чтобы один шаг - 1us. Он нужен, чтобы пересчитывать время в попугаи (сэмплы) при формировании задержек */
	mClockGenerator.Init(1000000.0, mSimulationSampleRateHz);

	mKiIRSimulationData.SetChannel( mSettings->mInputChannel );
	mKiIRSimulationData.SetSampleRate(simulation_sample_rate);
	mKiIRSimulationData.SetInitialBitState(bcSPACE);
}

U32 KiIRSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel )
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

	/* Необходимо оставить паузу до первого фрейма, иначе он начнется "за углом", точнее не будет распознаваться и для него не будет отображаться длительность */
	mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * 10000));

	/* Собственно заполнение фреймов (сколько влезет) */
	while (mKiIRSimulationData.GetCurrentSampleNumber() < adjusted_largest_sample_requested)
	{
		/* Формирование фрейма и паузы после него(!) Фремйов может быть не один (например, в Sharp два фрейма) */
		FormFrame(mSettings->mProtocol);
	}

	*simulation_channel = &mKiIRSimulationData;
	return 1;
}

/// <summary>
/// Формирование одной посылки. Она может состоять из нескольких фреймов (в Sharp - два фрейма). Если протокол не указан, выводится по одной посылке каждого протокола.
/// </summary>
/// <param name="pProtocol">Идентификатор протокола.</param>
void KiIRSimulationDataGenerator::FormFrame(ProtocolEnum pProtocol)
{
	static U64 lData = 0x0123456789ABCDEFull - 3ull;
	// static U64 lData = 0ull;
	lData += 3ull;
	switch (pProtocol)
	{
	case peMAXID:								/* Формируем по одной посылке для каждого протокола */
		for (int i = 0; AllProtocols[i]; ++i)
		{
			FormFrame(AllProtocols[i], lData);
			if (AllProtocols[i]->needRepeat)
			{
				/* Отправка кода повтора */
				FormFrame(AllProtocols[i], lData, 0, true);
				/* Добавляем задержку в конце посылки, чтобы она была вдвое больше задержки для повтора */
				mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * AllProtocols[i]->repeatInfo.repeatTime));
			}
			else
			{
				/* Добавляем задержку в конце посылки, 10 ms */
				mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * 10000));
			}
		}
		break;
	default:									/* Формируем одну посылку для конкретного протокола */
		/* Отправка одной посылки */
		FormFrame(AllProtocols[pProtocol], lData);
		if (AllProtocols[pProtocol]->needRepeat)
		{
			/* Отправка кода повтора */
			FormFrame(AllProtocols[pProtocol], lData, 0, true);
			/* Добавляем задержку в конце посылки, чтобы она была вдвое больше задержки для повтора */
			mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * AllProtocols[pProtocol]->repeatInfo.repeatTime));
		}
		else
		{
			/* Добавляем задержку в конце посылки, 10 ms */
			mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * 10000));
		}
		break;
	}
}

/// <summary>
/// Формирование одной посылки указанного протокола (может состоять из нескольких фреймов).
/// </summary>
/// <param name="ppProtocol">Указатель на описание протокола.</param>
void KiIRSimulationDataGenerator::FormFrame(const Protocol *ppProtocol, U64 pData, U8 pFrameIndex /*= 0*/, bool pSendRepeat /*= false*/)
{
	U64 lData = pData;
	/* Запоминаем начальный семпл. Нужен для формирования паузы после фрейма в некоторых протоколах */
	U32 lSamplesBetweenFrames = 0;
	U64 lStartSampleNumber = mKiIRSimulationData.GetCurrentSampleNumber();
	if (ppProtocol->needRepeat)
	{
		/*
		* Если повтор в протоколе существует, рассчитываем количество семплов от начала фрейма.
		* Может не пригодится, если для повтора используется интервал между фреймами (Gap)
		*/
		lSamplesBetweenFrames = mClockGenerator.AdvanceByTimeS(MICRO_SEC * ppProtocol->repeatInfo.repeatTime);
	}

	/* Наличие синхро последовательности, только для первой посылки, при повторах её нет */
	if (ppProtocol->needSynchro && (0 == pFrameIndex))
	{
		FormSynchro(ppProtocol);
	}
	
	/* Сначала Leads если нужно. В некоторых протоколах его нет. В других это сложная система из нескольких изменений сигнала. */
	if (ppProtocol->needLeadBits)
	{
		// FormLeads
		FormLeads(ppProtocol, pSendRepeat);
	}
	/*
	* Некоторые протоколы выводят данные, даже если это повтор
	*/
	if (!pSendRepeat
		|| (ppProtocol->needRepeat && ppProtocol->repeatInfo.repeatType != rtSpecialLead))
	{
		/* Для тестового фрейма используем счетчик бит из структуры, в реальном сигнале могут быть фреймы с переменной длиной, в тесте - длина фиксирована */
		int lBitsCount = ppProtocol->frameInfoVector[pFrameIndex].bitsCountForGenerator; // bitsCountForGenerator;
		if ((peRC5 == ppProtocol->protocolEnum)
			|| (peRC5X == ppProtocol->protocolEnum)
			|| (peS100 == ppProtocol->protocolEnum))
		{
			lBitsCount++;	// Данные протоколы содержат дополнительно toggle бит
		}
		U64 lMask = (bdLSB == ppProtocol->bitsDirection ? 0x01UL : 0x1UL << (lBitsCount - 1));
		if (((peRC5 == ppProtocol->protocolEnum) || (peRC5X == ppProtocol->protocolEnum))
			&& pSendRepeat)
		{
			/* Инвертируем toggle бит */
			lData ^= RC5_TOGGLEBIT_MASK;
		}
		if ((peS100 == ppProtocol->protocolEnum)
			&& pSendRepeat)
		{
			/* Инвертируем toggle бит */
			lData ^= S100_TOGGLEBIT_MASK;
		}
		if (peRCA == ppProtocol->protocolEnum)
		{
			/*
			* Сформировать корректно данные, это значит, что младшие 12 бит сдвигаются влево на 12 бит и младшие 12 бит инвертируются.
			*/
			// U8 lLen = ppProtocol->bitsCount / 2;
			U8 lLen = ppProtocol->frameInfoVector[pFrameIndex].bitsCount / 2; // bitsCount / 2;
			lData &= (1 << lLen) - 1;				// Оставляем только младшие 12 бит
			lData |= lData << lLen;					// Копируем младшие 12 бит в следующие 12 (получается 24 бита)
			lData ^= (1 << lLen) - 1;				// Младшие 12 бит инвертируем
		}
		if (peNRC17 == ppProtocol->protocolEnum)
		{
			/* Стартовый бит всегда "1" */
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
				// Единица
				FormBit(ppProtocol->bitOne);
			}
			if (lData & lMask)
			{
				// Единица
				FormBit(ppProtocol->bitOne);
			}
			else
			{
				// Ноль
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
	
	/* Если нужно, выводим финальный бит */
	if (ppProtocol->needFinalBit)
	{
		// FormBit(ppProtocol->bitFinal);
		FormFinalBit(ppProtocol->bitFinal);
	}
	
	/* Завершаем посылку данных, переключаемся в SPACE и делаем задержку */
	mKiIRSimulationData.TransitionIfNeeded(bcSPACE);
	if (ppProtocol->needRepeat)
	{
		/* Для промежутка, нужно отмерить время от конца фрейма */
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
		/* Поскольку нет информации о повторе в протоколе, формируем фиксированную задержку в 10ms */
		// mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * 10000));
		mKiIRSimulationData.Advance(mClockGenerator.AdvanceByTimeS(MICRO_SEC * ppProtocol->frameInfoVector[pFrameIndex].pauseAfterFrameUs));
	}
	/* Прерываем рекурсию */
	if (pFrameIndex > 0)
	{
		return;
	}
	/* Фреймов больше, чем один */
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
/// Формирование Leads. Это может быть как один бит, так и несколько.
/// </summary>
/// <param name="ppProtocol">Описание протокола.</param>
void KiIRSimulationDataGenerator::FormLeads(const Protocol *ppProtocol, bool pSendRepeat /*= false*/)
{
	/* Формирование Lead для фрейма повтора */
	if (pSendRepeat)
	{
		if (ppProtocol->needRepeat)
		{
			switch (ppProtocol->repeatInfo.repeatType)
			{
			case rtCodeWithoutLead:
				/* Повтор - это посылка БЕЗ Leads */
				return;
			case rtSpecialLead:
				/* Для кода повтора есть специальный Lead */
				FormBit(ppProtocol->bitRepeatLead, ppProtocol->bitRepeatLead.bitCount);
				break;
			case rtNormal:
				/* Обычная посылка, формируем Lead */
				FormBit(ppProtocol->bitLead, ppProtocol->bitLead.bitCount);
				break;
			}
		}
		else
		{
			/* Если в протоколе отсутствует возможность отправки повтора, то выводим обычный Lead */
			FormBit(ppProtocol->bitLead, ppProtocol->bitLead.bitCount);
		}
	}
	else
	{
		/* Если это не повтор, то выводим обычный Lead */
		FormBit(ppProtocol->bitLead, ppProtocol->bitLead.bitCount);
	}
}

/// <summary>
/// Формирование синхро последовательности.
/// </summary>
/// <param name="ppProtocol">Описание протокола.</param>
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
/// Формирование одного бита. Это всегда два transition.
/// </summary>
/// <param name="pLogicalBit">Описание бита из структуры описания протокола.</param>
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
