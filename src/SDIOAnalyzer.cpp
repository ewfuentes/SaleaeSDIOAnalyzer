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

#include "SDIOAnalyzer.h"
#include "SDIOAnalyzerSettings.h"
#include <AnalyzerChannelData.h>


U32 sdCRC7( U32 crc, U8 messageByte )
{
    for( int ibit = 0; ibit < 8; ibit++ )
    {
        crc <<= 1;
        if( ( messageByte ^ crc ) & 0x80 )
            crc ^= 0x09;
        messageByte <<= 1;
    }
    return ( crc & 0x7F );
}


SDIOAnalyzer::SDIOAnalyzer()
    : Analyzer2(),
      mSettings( new SDIOAnalyzerSettings() ),
      mSimulationInitilized( false ),
      mAlreadyRun( false ),
      packetState( WAITING_FOR_PACKET ),
      frameState( TRANSMISSION_BIT )
{
    SetAnalyzerSettings( mSettings.get() );
    UseFrameV2();
}

SDIOAnalyzer::~SDIOAnalyzer()
{
    KillThread();
}

void SDIOAnalyzer::SetupResults()
{
    mResults.reset( new SDIOAnalyzerResults( this, mSettings.get() ) );
    SetAnalyzerResults( mResults.get() );

    // mResults->AddChannelBubblesWillAppearOn(mSettings->mClockChannel);
    mResults->AddChannelBubblesWillAppearOn( mSettings->mCmdChannel );
    // mResults->AddChannelBubblesWillAppearOn(mSettings->mDAT0Channel);
    // mResults->AddChannelBubblesWillAppearOn(mSettings->mDAT1Channel);
    // mResults->AddChannelBubblesWillAppearOn(mSettings->mDAT2Channel);
    // mResults->AddChannelBubblesWillAppearOn(mSettings->mDAT3Channel);
}

void SDIOAnalyzer::WorkerThread()
{
    mAlreadyRun = true;

    mClock = GetAnalyzerChannelData( mSettings->mClockChannel );
    mCmd = GetAnalyzerChannelData( mSettings->mCmdChannel );
    mDAT0 = mSettings->mDAT0Channel == UNDEFINED_CHANNEL ? nullptr : GetAnalyzerChannelData( mSettings->mDAT0Channel );
    mDAT1 = mSettings->mDAT1Channel == UNDEFINED_CHANNEL ? nullptr : GetAnalyzerChannelData( mSettings->mDAT1Channel );
    mDAT2 = mSettings->mDAT2Channel == UNDEFINED_CHANNEL ? nullptr : GetAnalyzerChannelData( mSettings->mDAT2Channel );
    mDAT3 = mSettings->mDAT3Channel == UNDEFINED_CHANNEL ? nullptr : GetAnalyzerChannelData( mSettings->mDAT3Channel );

    mClock->AdvanceToNextEdge();
    mCmd->AdvanceToAbsPosition( mClock->GetSampleNumber() );
    if( mDAT0 )
        mDAT0->AdvanceToAbsPosition( mClock->GetSampleNumber() );
    if( mDAT1 )
        mDAT1->AdvanceToAbsPosition( mClock->GetSampleNumber() );
    if( mDAT2 )
        mDAT2->AdvanceToAbsPosition( mClock->GetSampleNumber() );
    if( mDAT3 )
        mDAT3->AdvanceToAbsPosition( mClock->GetSampleNumber() );

    for( ;; )
    {
        PacketStateMachine();

        mResults->CommitResults();
        ReportProgress( mClock->GetSampleNumber() );
    }
}

// Determine whether or not we are in a packet
void SDIOAnalyzer::PacketStateMachine()
{
    if( packetState == WAITING_FOR_PACKET )
    {
        // If we are not in a packet, let's advance to the next edge on the
        // command line
        mCmd->AdvanceToNextEdge();
        U64 sampleNumber = mCmd->GetSampleNumber();
        lastFallingClockEdge = sampleNumber;
        mClock->AdvanceToAbsPosition( sampleNumber );
        // After advancing to the next command line edge the clock can either
        // high or low.  If it is high, we need to advance two clock edges.  If
        // it is low, we only need to advance one clock edge.
        if( mClock->GetBitState() == BIT_HIGH )
        {
            mClock->AdvanceToNextEdge();
        }

        mClock->AdvanceToNextEdge();
        sampleNumber = mClock->GetSampleNumber();

        mCmd->AdvanceToAbsPosition( sampleNumber );
        if( mDAT0 )
            mDAT0->AdvanceToAbsPosition( sampleNumber );
        if( mDAT1 )
            mDAT1->AdvanceToAbsPosition( sampleNumber );
        if( mDAT2 )
            mDAT2->AdvanceToAbsPosition( sampleNumber );
        if( mDAT3 )
            mDAT3->AdvanceToAbsPosition( sampleNumber );

        if( mCmd->GetBitState() == BIT_LOW )
        {
            packetState = IN_PACKET;
        }
    }
    else if( packetState == IN_PACKET )
    {
        mClock->AdvanceToNextEdge();
        U64 sampleNumber = mClock->GetSampleNumber();

        mCmd->AdvanceToAbsPosition( sampleNumber );
        if( mDAT0 )
            mDAT0->AdvanceToAbsPosition( sampleNumber );
        if( mDAT1 )
            mDAT1->AdvanceToAbsPosition( sampleNumber );
        if( mDAT2 )
            mDAT2->AdvanceToAbsPosition( sampleNumber );
        if( mDAT3 )
            mDAT3->AdvanceToAbsPosition( sampleNumber );

        if( mClock->GetBitState() == BIT_HIGH )
        {
            mResults->AddMarker( mClock->GetSampleNumber(), AnalyzerResults::UpArrow, mSettings->mClockChannel );
            if( FrameStateMachine() == 1 )
            {
                packetState = WAITING_FOR_PACKET;
            }
        }
        else
        {
            lastFallingClockEdge = mClock->GetSampleNumber();
        }
    }
}

// This state machine will deal with accepting the different parts of the
// transmitted information.  In order to correctly interpret the data stream,
// we need to be able to distinguish between 4 different kinds of packets.
// They are:
//  - Command
//  - Short Response
//  - Long Response
//  - Data

U32 SDIOAnalyzer::FrameStateMachine( void )
{
    switch( frameState )
    {
    case TRANSMISSION_BIT:
    {
        Frame frame;
        frame.mStartingSampleInclusive = lastFallingClockEdge;
        frame.mEndingSampleInclusive = mClock->GetSampleOfNextEdge() - 1;
        frame.mFlags = 0;
        frame.mData1 = mCmd->GetBitState();
        frame.mType = FRAME_DIR;
        mResults->AddFrame( frame );

        FrameV2 frame_v2;
        frame_v2.AddBoolean( "DIR", frame.mData1 );
        mResults->AddFrameV2( frame_v2, "DIR", frame.mStartingSampleInclusive, frame.mEndingSampleInclusive );

        // The transmission bit tells us the origin of the packet
        // If the bit is high the packet comes from the host
        // If the bit is low, the packet comes from the slave
        isCmd = mCmd->GetBitState();

        frameState = COMMAND;
        frameCounter = 6;

        startOfNextFrame = ( frame.mEndingSampleInclusive + 1 );
        temp = 0;
        lastCommand = 0;
        expectedCRC = 0;
    }
    break;

    case COMMAND:
    {
        temp = ( temp << 1 ) | mCmd->GetBitState();

        frameCounter--;
        if( frameCounter == 0 )
        {
            Frame frame;
            frame.mStartingSampleInclusive = startOfNextFrame;
            frame.mEndingSampleInclusive = mClock->GetSampleOfNextEdge() - 1;
            frame.mFlags = 0;
            frame.mData1 = temp & 0x3F;
            frame.mData2 = isCmd ? 1 : 0;
            frame.mType = FRAME_CMD;
            mResults->AddFrame( frame );

            FrameV2 frame_v2;
            frame_v2.AddByte( "CMD", frame.mData1 );
            frame_v2.AddBoolean( "DIR", frame.mData2 );
            mResults->AddFrameV2( frame_v2, "COMMAND", frame.mStartingSampleInclusive, frame.mEndingSampleInclusive );

            expectedCRC = sdCRC7( 0, ( frame.mData2 << 6 ) | frame.mData1 );

            // Once we have the arguement

            lastCommand = frame.mData1;

            // Find the expected length of the next reponse based on the command
            if( isCmd )
            {
                if( app )
                {
                    // Deal with the application commands first
                    // All Application commands have a 48 bit response
                    respLength = 32;
                }
                else
                {
                    // Deal with standard commands now
                    // CMD2, CMD9 and CMD10 respond with R2
                    if( temp == 2 || temp == 9 || temp == 10 )
                    {
                        respLength = 127;
                        respType = RESP_LONG;
                    }
                    else
                    {
                        // All others have 48 bit responses
                        respLength = 32;
                        respType = RESP_NORMAL;
                    }
                }
            }

            frameState = ARGUMENT;
            startOfNextFrame = frame.mEndingSampleInclusive + 1;
            temp = 0;

            frameCounter = isCmd ? 32 : respLength;
        }
    }
    break;

    case ARGUMENT:
    {
        temp = temp << 1 | mCmd->GetBitState();

        frameCounter--;

        if( !isCmd && frameCounter == 1 && respType == RESP_LONG )
        {
            temp = temp << 1;

            Frame frame;
            frame.mStartingSampleInclusive = startOfNextFrame;
            frame.mEndingSampleInclusive = mClock->GetSampleOfNextEdge() - 1;
            frame.mFlags = lastCommand;
            frame.mData1 = temp2;
            frame.mData2 = temp;
            frame.mType = FRAME_LONG_ARG;
            mResults->AddFrame( frame );

            FrameV2 frame_v2;
            frame_v2.AddByte( "ARG1", frame.mData1 );
            frame_v2.AddByte( "ARG2", frame.mData2 );
            mResults->AddFrameV2( frame_v2, "LONG ARG", frame.mStartingSampleInclusive, frame.mEndingSampleInclusive );

            for( signed int i = 24; i >= 0; i -= 8 )
                expectedCRC = sdCRC7( expectedCRC, 0xFF & ( frame.mData1 >> i ) );

            for( signed int i = 24; i >= 0; i -= 8 )
                expectedCRC = sdCRC7( expectedCRC, 0xFF & ( frame.mData2 >> i ) );

            frameState = STOP;
            frameCounter = 1;
            startOfNextFrame = frame.mEndingSampleInclusive + 1;
            temp = 0;
        }
        else if( frameCounter == 0 )
        {
            Frame frame;
            frame.mStartingSampleInclusive = startOfNextFrame;
            frame.mEndingSampleInclusive = mClock->GetSampleOfNextEdge() - 1;
            frame.mFlags = lastCommand;
            frame.mData1 = temp;
            frame.mType = FRAME_ARG;
            mResults->AddFrame( frame );

            FrameV2 frame_v2;
            frame_v2.AddByte( "ARG", frame.mData1 );
            mResults->AddFrameV2( frame_v2, "NORMAL ARG", frame.mStartingSampleInclusive, frame.mEndingSampleInclusive );

            for( signed int i = 24; i >= 0; i -= 8 )
                expectedCRC = sdCRC7( expectedCRC, 0xFF & ( frame.mData1 >> i ) );

            frameState = CRC7;
            frameCounter = 7;
            startOfNextFrame = frame.mEndingSampleInclusive + 1;
            temp = 0;
        }
        else if( frameCounter == 63 && !isCmd )
        {
            temp2 = temp;
            temp = 0;
        }
    }
    break;

    case CRC7:
    {
        temp = temp << 1 | mCmd->GetBitState();

        frameCounter--;
        if( frameCounter == 0 )
        {
            temp &= 0x7F;

            Frame frame;
            frame.mStartingSampleInclusive = startOfNextFrame;
            frame.mEndingSampleInclusive = mClock->GetSampleOfNextEdge() - 1;
            frame.mFlags = 0;
            frame.mData1 = temp;
            frame.mData2 = ( temp == expectedCRC );
            frame.mType = FRAME_CRC;
            mResults->AddFrame( frame );

            FrameV2 frame_v2;
            frame_v2.AddByte( "CRC", frame.mData1 );
            frame_v2.AddBoolean( "PASS", frame.mData2 );
            mResults->AddFrameV2( frame_v2, "CRC", frame.mStartingSampleInclusive, frame.mEndingSampleInclusive );

            frameState = STOP;
            startOfNextFrame = frame.mEndingSampleInclusive + 1;
            temp = 0;
        }
    }
    break;

    case STOP:
    {
        frameState = TRANSMISSION_BIT;
        return 1;
    }

    default:
        break;
    }

    return ( 0 );
}

bool SDIOAnalyzer::NeedsRerun( void )
{
    return ( !mAlreadyRun );
}

U32 SDIOAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate,
                                          SimulationChannelDescriptor** simulation_channels )
{
    return 0;
}

U32 SDIOAnalyzer::GetMinimumSampleRateHz( void )
{
    return 25000;
}

const char* SDIOAnalyzer::GetAnalyzerName( void ) const
{
    return "SDIO";
}

const char* GetAnalyzerName( void )
{
    return "SDIO";
}

Analyzer* CreateAnalyzer( void )
{
    return new SDIOAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
    delete analyzer;
}
