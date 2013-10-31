#include "SDIOAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "SDIOAnalyzer.h"
#include "SDIOAnalyzerSettings.h"
#include <iostream>
#include <fstream>

SDIOAnalyzerResults::SDIOAnalyzerResults( SDIOAnalyzer* analyzer, SDIOAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

SDIOAnalyzerResults::~SDIOAnalyzerResults()
{
}

void SDIOAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );

	char number_str1[128];
	char number_str2[128];
	if (frame.mType == SDIOAnalyzer::FRAME_DIR){
		if (frame.mData1){
			AddResultString("H");
			AddResultString("Host");
			AddResultString("DIR: Host");
		}else{
			AddResultString("S");
			AddResultString("Slave");
			AddResultString("DIR: Slave");
		}
	}else if (frame.mType == SDIOAnalyzer::FRAME_CMD){
		AnalyzerHelpers::GetNumberString( frame.mData1, Decimal, 6, number_str1, 128 );
		AddResultString("CMD ", number_str1);
	}else if (frame.mType == SDIOAnalyzer::FRAME_ARG){
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 32, number_str1, 128 );
		AddResultString("ARG ", number_str1);
	}else if (frame.mType == SDIOAnalyzer::FRAME_LONG_ARG){
		AnalyzerHelpers::GetNumberString (frame.mData1, display_base, 64, number_str1, 128);
		AnalyzerHelpers::GetNumberString (frame.mData2, display_base, 64, number_str2, 128);
		AddResultString("LONG: ", number_str1, number_str2);

	}else if (frame.mType == SDIOAnalyzer::FRAME_CRC){
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 7, number_str1, 128 );
		AddResultString("CRC ", number_str1);
	}
}

void SDIOAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
	// std::ofstream file_stream( file, std::ios::out );

	// U64 trigger_sample = mAnalyzer->GetTriggerSample();
	// U32 sample_rate = mAnalyzer->GetSampleRate();

	// file_stream << "Time [s],Value" << std::endl;

	// U64 num_frames = GetNumFrames();
	// for( U32 i=0; i < num_frames; i++ )
	// {
	// 	Frame frame = GetFrame( i );
		
	// 	char time_str[128];
	// 	AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

	// 	char number_str[128];
	// 	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );

	// 	file_stream << time_str << "," << number_str << std::endl;

	// 	if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
	// 	{
	// 		file_stream.close();
	// 		return;
	// 	}
	// }

	// file_stream.close();
}

void SDIOAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
	// Frame frame = GetFrame( frame_index );
	// ClearResultStrings();

	// char number_str[128];
	// AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
	// AddResultString( number_str );
}

void SDIOAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	ClearResultStrings();
	AddResultString( "not supported" );
}

void SDIOAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	ClearResultStrings();
	AddResultString( "not supported" );
}