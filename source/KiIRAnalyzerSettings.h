#ifndef KIIR_ANALYZER_SETTINGS
#define KIIR_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>
#include "KiIRProtocolsCommon.h"

class KiIRAnalyzerSettings : public AnalyzerSettings
{
public:
	KiIRAnalyzerSettings();
	virtual ~KiIRAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	Channel			mInputChannel;
	ProtocolEnum	mProtocol;
	U32				mTolerance;
	bool			mUseLSBOnly;

protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mInputChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList >	mProtocolInterface;
	std::auto_ptr< AnalyzerSettingInterfaceInteger >	mToleranceInterface;
	std::auto_ptr< AnalyzerSettingInterfaceBool >		mUseLSBOnlyInterface;
};

#endif //KIIR_ANALYZER_SETTINGS
