#include "KiIRAnalyzer.h"
#include "KiIRAnalyzerSettings.h"
#include "DebugLog.h"
#include <AnalyzerChannelData.h>

KiIRAnalyzer::KiIRAnalyzer()
	: Analyzer2()
	, mSettings( new KiIRAnalyzerSettings() )
	, mSimulationInitilized( false )
	, mProtocols(new KiIR_Protocols())
{
	SetAnalyzerSettings( mSettings.get() );
}

KiIRAnalyzer::~KiIRAnalyzer()
{
	KillThread();
}

// virtual 
/// <summary>
/// Setups the results.
/// </summary>
void KiIRAnalyzer::SetupResults()
{
	mResults.reset(new KiIRAnalyzerResults(this, mSettings.get()));
	SetAnalyzerResults(mResults.get());
	mResults->AddChannelBubblesWillAppearOn(mSettings->mInputChannel);
}

/// <summary>
/// Основная нить, используется для формирования фреймов протокола.
/// </summary>
void KiIRAnalyzer::WorkerThread()
{
	mKiIR = GetAnalyzerChannelData(mSettings->mInputChannel);

	LOGTOFILE("Search protocol", mSettings->mProtocol);
	ProtocolEnum lProtocolEnum = mSettings->mProtocol;
	const Protocol *lpProtocol = nullptr;
	if (peMAXID != lProtocolEnum)
	{
		for (auto i = 0; AllProtocols[i]; ++i)
		{
			if (lProtocolEnum == AllProtocols[i]->protocolEnum)
			{
				lpProtocol = AllProtocols[i];
				break;
			}
		}
	}
	mProtocols->setProtocol(GetSampleRate(), lpProtocol);
	LOGTOFILE("Protocol found", mSettings->mProtocol);

	while (mKiIR->DoMoreTransitionsExistInCurrentData())
	{
		LOGTOFILE("--- Start protocolAnalyze", 1);
		if (!mProtocols->protocolAnalyze(this, mKiIR, mResults.get(), mSettings.get()))
		{
			LOGTOFILE("--- ProtocolAnalyze, result", 0);
			CheckIfThreadShouldExit();
			mKiIR->AdvanceToNextEdge();
		}
		CheckIfThreadShouldExit();
		LOGTOFILE("--- End protocolAnalyze", 0);
	}
	LOGTOFILE("--- End WorkerThread", 1);
}

bool KiIRAnalyzer::NeedsRerun()
{
	return false;
}

U32 KiIRAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 KiIRAnalyzer::GetMinimumSampleRateHz()
{
	return 1000000; // mSettings->mBitRate * 4;
}

const char* KiIRAnalyzer::GetAnalyzerName() const
{
#if defined(_DEBUG)
	return "KiIR Debug";
#else
	return "KiIR";
#endif
}

const char* GetAnalyzerName()
{
#if defined(_DEBUG)
	return "KiIR Debug";
#else
	return "KiIR";
#endif
}

Analyzer* CreateAnalyzer()
{
	return new KiIRAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}