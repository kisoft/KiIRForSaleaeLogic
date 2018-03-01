#ifndef KIIR_ANALYZER_RESULTS
#define KIIR_ANALYZER_RESULTS

#include <AnalyzerResults.h>
#include "KiIRProtocolsCommon.h"

class KiIRAnalyzer;
class KiIRAnalyzerSettings;

class KiIRAnalyzerResults : public AnalyzerResults
{
public:
	KiIRAnalyzerResults( KiIRAnalyzer* analyzer, KiIRAnalyzerSettings* settings );
	virtual ~KiIRAnalyzerResults();

	virtual void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base );
	virtual void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id );

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base );
	virtual void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base );
	virtual void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base );

	size_t AddBlock(const VectorData &pVectorData)
	{
		mBlocks.push_back(pVectorData);
		return mBlocks.size() - 1;
	}

	void GetTextsForPacketSegmentFrame(const Frame& f, DisplayBase display_base, std::stringstream &ss);

protected: //functions

protected:  //vars
	KiIRAnalyzerSettings* mSettings;
	KiIRAnalyzer* mAnalyzer;

	std::vector<VectorData>	mBlocks;
};

#endif //KIIR_ANALYZER_RESULTS
