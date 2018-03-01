#include "KiIRAnalyzerResults.h"
#include "DebugLog.h"
#include <AnalyzerHelpers.h>
#include "KiIRAnalyzer.h"
#include "KiIRAnalyzerSettings.h"
#include <iostream>
#include <fstream>
#include <sstream>

// #define USE_LOGFILE_HERE

std::string GetDisplayBaseName(DisplayBase display_base)
{
	switch (display_base)
	{
	case DisplayBase::ASCII:
		return "ASCII";
	case DisplayBase::AsciiHex:
		return "AsciiHex";
	case DisplayBase::Binary:
		return "Binary";
	case DisplayBase::Decimal:
		return "Decimal";
	case DisplayBase::Hexadecimal:
		return "Hexadecimal";
	}
	return "Unknown";
}

// Группируем вывод на 64 битные сегменты
std::string GetU64ArrayNumberStr(const VectorData &pBlock, U64 num_bits, DisplayBase display_base)
{
	const int BUFF_SIZE = 256;
	char number_str[BUFF_SIZE];

#if defined(USE_LOGFILE_HERE)
	LOGTOFILE("num_bits", num_bits);
#endif

	std::string ret_val;
	// int cnt_u64;
	U32 segments_remaining = static_cast<U32>(num_bits / 64);
	U32 bits_remaining = static_cast<U32>(num_bits - segments_remaining * 64);

	segments_remaining += (bits_remaining != 0 ? 1 : 0);

#if defined(USE_LOGFILE_HERE)
	LOGTOFILE("segments_remaining", segments_remaining);
	LOGTOFILE("bits_remaining", bits_remaining);

	LOGTOFILE(GetDisplayBaseName(display_base).c_str(), 0);
#endif

#if 1
	for each (auto lData in pBlock)
	{
		/* Сегменты делятся дефисом */
		if (!ret_val.empty())
		{
			ret_val += '-';
		}

#if defined(USE_LOGFILE_HERE)
		LOGTOFILEX("*** val = ", lData);
#endif

		/* Формируем строку */
		AnalyzerHelpers::GetNumberString(lData, display_base, segments_remaining > 1 ? 64 : bits_remaining, number_str, BUFF_SIZE);
#if defined(USE_LOGFILE_HERE)
		LOGTOFILEX(number_str, lData);
#endif

		ret_val += number_str;
		segments_remaining--;
	}
#else	
	/* Сегментов может быть не более 4 */
	for (cnt_u64 = 0; cnt_u64 < 4 && segments_remaining > 0; ++cnt_u64, segments_remaining--)
	{
		/* Сегменты делятся дефисом */
		if (!ret_val.empty())
		{
			ret_val += '-';
		}

		U64 val = *i;

		LOGTOFILEX("*** val = ", val);

		/* Формируем строку */
		AnalyzerHelpers::GetNumberString(val, display_base, segments_remaining > 1 ? 64 : bits_remaining, number_str, BUFF_SIZE);
		LOGTOFILEX(number_str, val);

		ret_val += number_str;
		i++;
	}
#endif
#if defined(USE_LOGFILE_HERE)
	LOGTOFILEX(ret_val.c_str(), 0);
#endif
	return ret_val;
}

KiIRAnalyzerResults::KiIRAnalyzerResults( KiIRAnalyzer* analyzer, KiIRAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

KiIRAnalyzerResults::~KiIRAnalyzerResults()
{
}


void KiIRAnalyzerResults::GetTextsForPacketSegmentFrame(const Frame &pFrame, DisplayBase display_base, std::stringstream &ss)
{
	// const VectorData &block(mBlocks[pFrame.mData1]);
	ss << GetU64ArrayNumberStr(mBlocks[pFrame.mData1], pFrame.mData2, display_base);
}


/*
* Функция формирует данные для вывода на экран (подпись для данных на графике)
* Функция может формировать несколько разных строк (нужная строка будет отображаться в зависимости от её размера. Т.е. отображается наибольшая, которая влезет).
*/
void KiIRAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );

	char number_str[256];

	std::stringstream ss;

	if (dtLEADS == frame.mType)
	{
		AddResultString("L");
		AddResultString("Lead");
	}
	else if (dtREPEATLEADS == frame.mType)
	{
		AddResultString("R");
		AddResultString("Repeat");
	}
	else if (dtSYNCHRODATA == frame.mType)
	{
		AddResultString("Y");
		AddResultString("sYnchro");
	}
	else if (dtTOGGLE == frame.mType)
	{
		ss << frame.mData1;							// Простой вариант, без скобок: 1
		AddResultString(ss.str().c_str());

		ss.str("");
		ss << "T (" << frame.mData1 << ")";			// T (1)
		AddResultString(ss.str().c_str());

		ss.str("");
		ss << "Toggle (" << frame.mData1 << ")";	// Toggle (1)
		AddResultString(ss.str().c_str());
	}
	else if (dtSTARTBIT == frame.mType)
	{
		ss << frame.mData1;							// Простой вариант, без скобок: 1
		AddResultString(ss.str().c_str());

		ss.str("");
		ss << "S (" << frame.mData1 << ")";			// S (1)
		AddResultString(ss.str().c_str());

		ss.str("");
		ss << "Start (" << frame.mData1 << ")";	// Start (1)
		AddResultString(ss.str().c_str());
	}
	else if (dtEXTRADATA == frame.mType)
	{
		ss << frame.mData1;							// Простой вариант, без скобок: 1
		AddResultString(ss.str().c_str());

		ss.str("");
		ss << "X (" << frame.mData1 << ")";			// X (1)
		AddResultString(ss.str().c_str());

		ss.str("");
		ss << "eXtra (" << frame.mData1 << ")";		// eXtra (1)
		AddResultString(ss.str().c_str());
	}
	else if (dtFINALBIT == frame.mType)
	{
		AddResultString("F");
		AddResultString("Final");
	}
	else if ((dtDATAU64 == frame.mType)
		|| (dtDATAWITHTOGGLE == frame.mType)
		|| (dtDATAWITHEXTRA == frame.mType))
	{
		U32 lWidth = static_cast<U32>(frame.mData2);
		if (lWidth < 1)
		{
			lWidth = 16;
		}
		if (lWidth > 64)
		{
			lWidth = 64;
		}
		AnalyzerHelpers::GetNumberString(frame.mData1, display_base, lWidth, number_str, 128);
		ss << number_str << " [" << frame.mData2 << "]";
		AddResultString(ss.str().c_str());

		ss.str("");
		if (dtDATAWITHEXTRA == frame.mType)
		{
			ss << number_str << " [" << frame.mData2 << " bits w/eXtra]";
		}
		else
		{
			ss << number_str << " [" << frame.mData2 << " bits]";
		}
		AddResultString(ss.str().c_str());
	}
	else if (dtDATAVECTOR == frame.mType)
	{
		GetTextsForPacketSegmentFrame(frame, display_base, ss);
		ss << " [" << frame.mData2 << " bits]";
		AddResultString(ss.str().c_str());
	}
}

/* Экспорт в файл */
// TODO Доделать корректный вывод длинных чисел (более 64 бит)
void KiIRAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
	std::ofstream file_stream( file, std::ios::out );

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	file_stream << "Time [s],Value" << std::endl;

	U64 num_frames = GetNumFrames();
	for( U32 i=0; i < num_frames; i++ )
	{
		Frame frame = GetFrame( i );
		
		char time_str[128];
		AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

		char number_str[128];
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );

		file_stream << time_str << "," << number_str << std::endl;

		if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
		{
			file_stream.close();
			return;
		}
	}

	file_stream.close();
}

/*
* Функция формирует короткую строку, которая отображается в таблице распозанных данных
* В отличчи от Bubble, здесь нужна только одна строка.
*/
void KiIRAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
	Frame frame = GetFrame( frame_index );
	ClearTabularText();

	char number_str[256];

	std::stringstream ss;

	if (dtLEADS == frame.mType)
	{
		//AddTabularText("Lead");
	}
	else if (dtREPEATLEADS == frame.mType)
	{
		AddTabularText("Repeat");
	}
	else if (dtSYNCHRODATA == frame.mType)
	{
		AddTabularText("Synchro");
	}
	else if (dtTOGGLE == frame.mType)
	{
		//AddTabularText("Toggle");
	}
	else if (dtSTARTBIT == frame.mType)
	{
		//AddTabularText("Start");
	}
	else if (dtFINALBIT == frame.mType)
	{
		//AddTabularText("Final Bit");
	}
	else if ((dtDATAU64 == frame.mType)
		|| (dtDATAWITHTOGGLE == frame.mType)
		|| (dtDATAWITHEXTRA == frame.mType))
	{
		U32 lWidth = static_cast<U32>(frame.mData2);
		if (lWidth < 1)
		{
			lWidth = 16;
		}
		if (lWidth > 64)
		{
			lWidth = 64;
		}
		AnalyzerHelpers::GetNumberString(frame.mData1, display_base, lWidth, number_str, 128);
		ss << number_str << " [" << frame.mData2 << " bits";
		if (dtDATAWITHEXTRA == frame.mType)
		{
			ss << ", w/eXtra";
		}
		if ((dtDATAWITHTOGGLE == frame.mType)
			|| (dtDATAWITHEXTRA == frame.mType))
		{
			ss << ", toggle = " << (int)frame.mFlags;
		}
		ss << "]";
		AddTabularText(ss.str().c_str());
	}
	else if (dtDATAVECTOR == frame.mType)
	{
		GetTextsForPacketSegmentFrame(frame, display_base, ss);
		ss << " [" << frame.mData2 << " bits]";
		AddTabularText(ss.str().c_str());
	}
}

void KiIRAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	ClearResultStrings();
	AddResultString( "not supported" );
}

void KiIRAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	ClearResultStrings();
	AddResultString( "not supported" );
}
