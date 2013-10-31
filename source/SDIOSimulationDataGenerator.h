#ifndef SDIO_SIMULATION_DATA_GENERATOR
#define SDIO_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
class SDIOAnalyzerSettings;

class SDIOSimulationDataGenerator
{
public:
	SDIOSimulationDataGenerator();
	~SDIOSimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, SDIOAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	SDIOAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected:
	void CreateSerialByte();
	std::string mSerialText;
	U32 mStringIndex;

	SimulationChannelDescriptor mSerialSimulationData;

};
#endif //SDIO_SIMULATION_DATA_GENERATOR