// The MIT License (MIT)
//
// Copyright (c) 2013 Erick Fuentes http://erickfuent.es
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

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

		file_stream << time_str << ",";
		
		if (frame.mType == SDIOAnalyzer::FRAME_DIR){
			file_stream << "DIR:";
			if (frame.mData1){
				file_stream << "from Host";
			}else{
				file_stream << "from Slave";
			}
		}else if (frame.mType == SDIOAnalyzer::FRAME_CMD){
			file_stream << "CMD:" << number_str;
		}else if (frame.mType == SDIOAnalyzer::FRAME_ARG){
			file_stream << "ARG:" << number_str;
		}else if (frame.mType == SDIOAnalyzer::FRAME_LONG_ARG){
			file_stream << "LONG_ARG:" << number_str;
		}else if (frame.mType == SDIOAnalyzer::FRAME_CRC){
			file_stream << "CRC:" << number_str;
		}
		
		file_stream << std::endl;

		if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
		{
			file_stream.close();
			return;
		}
	}

	file_stream.close();
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
