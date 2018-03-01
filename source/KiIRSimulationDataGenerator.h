#ifndef KIIR_SIMULATION_DATA_GENERATOR
#define KIIR_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <AnalyzerHelpers.h>
#include "KiIRProtocolsCommon.h"
#include <string>
class KiIRAnalyzerSettings;

class KiIRSimulationDataGenerator
{
public:
	KiIRSimulationDataGenerator();
	~KiIRSimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, KiIRAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	void FormFrame(ProtocolEnum pProtocol);
	void FormFrame(const Protocol *ppProtocol, U64 pData, U8 pFrameIndex = 0, bool pSendRepeat = false);

	void FormBit(const LogicalBit &pLogicalBit, U64 pRepeatCount = 0);
	void FormLeads(const Protocol *ppProtocol, bool pSendRepeat = false);
	void FormFinalBit(const LogicalBit &pFinalBit);
	void FormSynchro(const Protocol *ppProtocol);

protected:
	KiIRAnalyzerSettings	*mSettings;
	U32						mSimulationSampleRateHz;
	ClockGenerator			mClockGenerator;

protected:
	SimulationChannelDescriptor mKiIRSimulationData;

};
#endif //KIIR_SIMULATION_DATA_GENERATOR