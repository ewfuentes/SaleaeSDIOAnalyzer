// The MIT License (MIT)
//
// Copyright (c) 2013 Erick Fuentes http://erickfuent.es
// Copyright (c) 2014 Kuy Mainwaring http://logiblock.com
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
    : AnalyzerResults(), mSettings( settings ), mAnalyzer( analyzer )
{
}

SDIOAnalyzerResults::~SDIOAnalyzerResults()
{
}

void SDIOAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
    ClearResultStrings();
    Frame frame = GetFrame( frame_index );

    char number_str1[ 128 ];
    char number_str2[ 128 ];

    switch( frame.mType )
    {
    case SDIOAnalyzer::FRAME_DIR:
        if( frame.mData1 )
        {
            AddResultString( "H" );
            AddResultString( "Host" );
            AddResultString( "DIR: Host" );
        }
        else
        {
            AddResultString( "S" );
            AddResultString( "Slave" );
            AddResultString( "DIR: Slave" );
        }
        break;

    case SDIOAnalyzer::FRAME_CMD:
        AnalyzerHelpers::GetNumberString( frame.mData1, Decimal, 6, number_str1, 128 );
        AddResultString( ( frame.mData2 ) ? "C" : "R", number_str1 );
        AddResultString( ( frame.mData2 ) ? "CMD" : "RSP", number_str1 );
        break;

    case SDIOAnalyzer::FRAME_ARG:
        AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 32, number_str1, 128 );
        AddResultString( "ARG ", number_str1 );
        break;

    case SDIOAnalyzer::FRAME_LONG_ARG:
        AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 64, number_str1, 128 );
        AnalyzerHelpers::GetNumberString( frame.mData2, display_base, 64, number_str2, 128 );
        AddResultString( "LONG: ", number_str1, number_str2 );
        break;

    case SDIOAnalyzer::FRAME_CRC:
        AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 7, number_str1, 128 );
        AddResultString( frame.mData2 ? "O" : "X" );
        AddResultString( frame.mData2 ? "CRC" : "BAD" );
        AddResultString( frame.mData2 ? "CRC OK" : "BAD CRC" );
        break;

    default:
        break;
    }
}

void SDIOAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
    std::ofstream file_stream( file, std::ios::out );
    char value_str[ 128 ];
    U64 trigger_sample = mAnalyzer->GetTriggerSample();
    U32 sample_rate = mAnalyzer->GetSampleRate();
    U64 num_frames = GetNumFrames();
    enum state
    {
        STATE_START,
        STATE_DIR,
        STATE_CMD,
        STATE_ARG,
        STATE_CRC,
        STATE_END,
        STATE_ERR,
    } current_state = STATE_START;

    file_stream << "Time[s],IDX,DIR,CMD,ARG1,ARG2,CRC,PASS" << std::endl;

    for( U32 i = 0; i < num_frames; i++ )
    {
        Frame frame = GetFrame( i );

        switch( current_state )
        {
        case STATE_START:
            AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, value_str, 128 );

            file_stream << value_str;

            file_stream << "," << i;

            if( frame.mType != SDIOAnalyzer::FRAME_DIR )
            {
                current_state = STATE_ERR;
                break;
            }

            current_state = STATE_DIR;
            break;

        case STATE_DIR:
            if( frame.mType != SDIOAnalyzer::FRAME_DIR )
            {
                current_state = STATE_ERR;
                break;
            }
            if( frame.mData1 )
            {
                file_stream << ",C";
            }
            else
            {
                file_stream << ",R";
            }
            current_state = STATE_CMD;
            break;

        case STATE_CMD:
            if( frame.mType != SDIOAnalyzer::FRAME_CMD )
            {
                current_state = STATE_ERR;
                break;
            }
            AnalyzerHelpers::GetNumberString( frame.mData1, Decimal, 6, value_str, 128 );
            file_stream << "," << value_str;
            current_state = STATE_ARG;
            break;

        case STATE_ARG:
            if( frame.mType == SDIOAnalyzer::FRAME_ARG )
            {
                AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 32, value_str, 128 );
                file_stream << "," << value_str;
                file_stream << ",";
                break;
            }
            else if( frame.mType == SDIOAnalyzer::FRAME_LONG_ARG )
            {
                AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 64, value_str, 128 );
                file_stream << "," << value_str;

                AnalyzerHelpers::GetNumberString( frame.mData2, display_base, 64, value_str, 128 );
                file_stream << "," << value_str;
            }
            else
            {
                current_state = STATE_ERR;
                break;
            }
            current_state = STATE_CRC;

        case STATE_CRC:
            if( frame.mType != SDIOAnalyzer::FRAME_CRC )
            {
                current_state = STATE_ERR;
                break;
            }

            AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 7, value_str, 128 );
            file_stream << "," << value_str;
            if( frame.mData2 )
            {
                file_stream << ",1";
            }
            else
            {
                file_stream << ",0";
            }
            current_state = STATE_END;
            break;

        case STATE_END:
            file_stream << std::endl;

            if( UpdateExportProgressAndCheckForCancel( i, num_frames ) )
            {
                file_stream.close();
                return;
            }
            current_state = STATE_START;
            break;

        case STATE_ERR:
            file_stream << ",ERROR";
            current_state = STATE_END;
            break;

        default:
            current_state = STATE_ERR;
            break;
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
