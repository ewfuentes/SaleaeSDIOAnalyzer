#ifndef SDIO_ANALYZER_SETTINGS
#define SDIO_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class SDIOAnalyzerSettings : public AnalyzerSettings
{
public:
	SDIOAnalyzerSettings();
	virtual ~SDIOAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	Channel mClockChannel;
	Channel mCmdChannel;
	Channel mDAT0Channel;
	Channel mDAT1Channel;
	Channel mDAT2Channel;
	Channel mDAT3Channel;
	Channel mInputChannel;
	Channel mBitRate;

protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mClockChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mCmdChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mDAT0ChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mDAT1ChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mDAT2ChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mDAT3ChannelInterface;
};

#endif //SDIO_ANALYZER_SETTINGS
