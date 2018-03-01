#ifndef KIIR_ANALYZER_H
#define KIIR_ANALYZER_H

#include <Analyzer.h>
#include "KiIRAnalyzerResults.h"
#include "KiIRSimulationDataGenerator.h"
#include "KiIRProtocolsCommon.h"
#include "KiIR_Protocols.h"

class KiIRAnalyzerSettings;
class ANALYZER_EXPORT KiIRAnalyzer : public Analyzer2
{
public:
	KiIRAnalyzer();
	virtual ~KiIRAnalyzer();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

	virtual void SetupResults();

protected: //vars
#pragma warning(push)
#pragma warning(disable:4251)
	std::auto_ptr< KiIRAnalyzerSettings > mSettings;
	std::auto_ptr< KiIRAnalyzerResults > mResults;
	AnalyzerChannelData* mKiIR;

	std::auto_ptr< KiIR_Protocols > mProtocols;

	KiIRSimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;
#pragma warning(pop)
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //KIIR_ANALYZER_H
