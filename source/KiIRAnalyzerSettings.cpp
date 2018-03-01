#include "KiIRAnalyzerSettings.h"
#include <AnalyzerHelpers.h>
#include "KiIR_Protocols.h"
#include <sstream>

#include "DebugLog.h"

KiIRAnalyzerSettings::KiIRAnalyzerSettings()
	: mInputChannel(UNDEFINED_CHANNEL)
	, mProtocol(peMAXID)
{
	mInputChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
#if defined(_DEBUG)
	mInputChannelInterface->SetTitleAndTooltip("KiIR Debug", "KiIR Debug");
#else
	mInputChannelInterface->SetTitleAndTooltip("KiIR", "KiIR");
#endif
	mInputChannelInterface->SetChannel(mInputChannel);

	mProtocolInterface.reset(new AnalyzerSettingInterfaceNumberList());

	mProtocolInterface->SetTitleAndTooltip("Protocol", "Select Protocol");
	for (int i = 0; AllProtocols[i] != nullptr; ++i)
	{
		const Protocol *lp_Protocol = AllProtocols[i];
		mProtocolInterface->AddNumber(lp_Protocol->protocolEnum, lp_Protocol->protocolName, lp_Protocol->protocolName);
		LOGTOFILE(lp_Protocol->protocolName, lp_Protocol->protocolEnum);
	}

	mProtocolInterface->AddNumber(peMAXID, "Unknown", "Research protocol");
	LOGTOFILE("Unknown", peMAXID);

	mProtocol = AllProtocols[0]->protocolEnum;
	mProtocolInterface->SetNumber(mProtocol);
	LOGTOFILE("Select protocol", mProtocol);

	mToleranceInterface.reset(new AnalyzerSettingInterfaceInteger());
	mToleranceInterface->SetTitleAndTooltip("Tolerance (%)", "Set Tolerance");
	mToleranceInterface->SetMin(0);
	mToleranceInterface->SetMax(100);
	mToleranceInterface->SetInteger(25);

	mUseLSBOnlyInterface.reset(new AnalyzerSettingInterfaceBool());
	mUseLSBOnlyInterface->SetTitleAndTooltip("IRremote compatible", "Code compatible with IRremote library");
	mUseLSBOnlyInterface->SetValue(false);

	AddInterface(mInputChannelInterface.get());
	AddInterface(mProtocolInterface.get());
	AddInterface(mToleranceInterface.get());
	AddInterface(mUseLSBOnlyInterface.get());

	AddExportOption(0, "Export as text/csv file");
	AddExportExtension(0, "text", "txt");
	AddExportExtension(0, "csv", "csv");

	ClearChannels();
#if defined(_DEBUG)
	AddChannel(mInputChannel, "KiIR Debug", false);
#else
	AddChannel(mInputChannel, "KiIR", false);
#endif
}

KiIRAnalyzerSettings::~KiIRAnalyzerSettings()
{
}

static void formProtocolDetails(ProtocolEnum pProtocolEnum, std::stringstream &ss)
{
	ss << AllProtocols[pProtocolEnum]->protocolName
		<< " : " << (bdLSB == AllProtocols[pProtocolEnum]->bitsDirection ? "LSB" : "MSB");

#if 1
	ss << " [";
	auto i = 0;
	for each (auto lFrameInfo in AllProtocols[pProtocolEnum]->frameInfoVector)
	{
		if (0 != i)
		{
			ss << ", ";
		}
		if (FLOAT_BITS_COUNT == lFrameInfo.bitsCount)
		{
			ss << "float";
		}
		else
		{
			ss << (int) lFrameInfo.bitsCount;
		}
		i++;
	}
	ss << "], ";
#else
	if (FLOAT_BITS_COUNT == AllProtocols[pProtocolEnum]->bitsCount)
	{
		ss << " [float";
	}
	else
	{
		ss << " [" << (int)AllProtocols[pProtocolEnum]->bitsCount;
	}
#endif
	ss << ProtocolType::getName(AllProtocols[pProtocolEnum]->protocolType).c_str();
}


bool KiIRAnalyzerSettings::SetSettingsFromInterfaces()
{
	mInputChannel = mInputChannelInterface->GetChannel();
	mProtocol = static_cast<ProtocolEnum>(static_cast<int>(mProtocolInterface->GetNumber()));
	LOGTOFILE("SetSettingsFromInterfaces set mProtocol", mProtocol);
	mTolerance = mToleranceInterface->GetInteger();
	mUseLSBOnly = mUseLSBOnlyInterface->GetValue();

	ClearChannels();

	if (peMAXID == mProtocol)
	{
		AddChannel(mInputChannel, "Unknown LSB", true);
	}
	else
	{
		std::stringstream ss;
		::formProtocolDetails(mProtocol, ss);
		AddChannel(mInputChannel, ss.str().c_str(), true);
	}

	return true;
}

void KiIRAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mInputChannelInterface->SetChannel(mInputChannel);
	mProtocolInterface->SetNumber(mProtocol);
	LOGTOFILE("UpdateInterfacesFromSettings set mProtocolInterface to", mProtocol);
	mToleranceInterface->SetInteger(mTolerance);
	mUseLSBOnlyInterface->SetValue(mUseLSBOnly);
}

void KiIRAnalyzerSettings::LoadSettings(const char* settings)
{
	SimpleArchive text_archive;
	text_archive.SetString(settings);

	text_archive >> mInputChannel;
	int temp;
	text_archive >> temp;
	LOGTOFILE("LoadSettings temp", temp);
	mProtocol = static_cast<ProtocolEnum>(temp);
	LOGTOFILE("LoadSettings set mProtocol", mProtocol);
	text_archive >> mTolerance;
	text_archive >> mUseLSBOnly;

	/* Проверка на допустимые значения. В первый раз считывается мусор или что-то еще непонятное, у меня читалось 128, что не является ProtocolEnum */
	bool lProtocolIsChecked = false;
	for (int i = 0; AllProtocols[i]; ++i)
	{
		if (mProtocol == AllProtocols[i]->protocolEnum)
		{
			lProtocolIsChecked = true;
			break;
		}
	}
	
	if (!lProtocolIsChecked && peMAXID != mProtocol)
	{
		mProtocol = peMAXID;
	}
	LOGTOFILE("LoadSettings normalized mProtocol", mProtocol);

	if (mTolerance < 0 || mTolerance > 100)
	{
		mTolerance = 25;
	}

	ClearChannels();
	if (peMAXID == mProtocol)
	{
		AddChannel(mInputChannel, "Unknown LSB", true);
	}
	else
	{
		std::stringstream ss;
		::formProtocolDetails(mProtocol, ss);
		AddChannel(mInputChannel, ss.str().c_str(), true);
	}

	UpdateInterfacesFromSettings();
}

const char* KiIRAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mInputChannel;
	text_archive << mProtocol;
	LOGTOFILE("SaveSettings save mProtocol", mProtocol);
	text_archive << mTolerance;
	text_archive << mUseLSBOnly;

	return SetReturnString(text_archive.GetString());
}
