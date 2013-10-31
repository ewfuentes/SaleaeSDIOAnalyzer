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

	enum frameTypes {FRAME_DIR, FRAME_CMD, FRAME_ARG, FRAME_LONG_ARG, FRAME_CRC};

protected: //vars
	std::auto_ptr< SDIOAnalyzerSettings > mSettings;
	std::auto_ptr< SDIOAnalyzerResults > mResults;

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
	enum packetStates {WAITING_FOR_PACKET, IN_PACKET};
	U32 packetState;

	U32 FrameStateMachine();
	enum frameStates {TRANSMISSION_BIT, COMMAND, ARGUMENT, CRC7, STOP};
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
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //SDIO_ANALYZER_H
