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

#ifndef SDIO_ANALYZER_H
#define SDIO_ANALYZER_H

#include <Analyzer.h>
#include "SDIOAnalyzerResults.h"
#include "SDIOSimulationDataGenerator.h"

class SDIOAnalyzerSettings;
class ANALYZER_EXPORT SDIOAnalyzer : public Analyzer
{
  public:
    SDIOAnalyzer();
    virtual ~SDIOAnalyzer();
    virtual void WorkerThread();

    virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
    virtual U32 GetMinimumSampleRateHz();

    virtual const char* GetAnalyzerName() const;
    virtual bool NeedsRerun();

    enum frameTypes
    {
        FRAME_DIR,
        FRAME_CMD,
        FRAME_ARG,
        FRAME_LONG_ARG,
        FRAME_CRC
    };

  protected: // vars
    std::auto_ptr<SDIOAnalyzerSettings> mSettings;
    std::auto_ptr<SDIOAnalyzerResults> mResults;

    AnalyzerChannelData* mClock;
    AnalyzerChannelData* mCmd;
    AnalyzerChannelData* mDAT0;
    AnalyzerChannelData* mDAT1;
    AnalyzerChannelData* mDAT2;
    AnalyzerChannelData* mDAT3;

    SDIOSimulationDataGenerator mSimulationDataGenerator;
    bool mSimulationInitilized;

  private:
    bool mAlreadyRun;

    U64 lastFallingClockEdge;
    U64 startOfNextFrame;
    void PacketStateMachine();
    enum packetStates
    {
        WAITING_FOR_PACKET,
        IN_PACKET
    };
    U32 packetState;

    U32 FrameStateMachine();
    enum frameStates
    {
        TRANSMISSION_BIT,
        COMMAND,
        ARGUMENT,
        CRC7,
        STOP
    };
    U32 frameState;
    U32 frameCounter;

    bool app;
    bool isCmd;
    U8 respLength;
    U8 respType;

    U64 temp;
    U64 temp2;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer();
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif // SDIO_ANALYZER_H
