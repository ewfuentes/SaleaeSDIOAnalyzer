#include "SDIOAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


SDIOAnalyzerSettings::SDIOAnalyzerSettings()
:	mClockChannel( UNDEFINED_CHANNEL ),
	mCmdChannel( UNDEFINED_CHANNEL ),
	mDAT0Channel( UNDEFINED_CHANNEL ),
	mDAT1Channel( UNDEFINED_CHANNEL ),
	mDAT2Channel( UNDEFINED_CHANNEL ),
	mDAT3Channel( UNDEFINED_CHANNEL )
{
	mClockChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mCmdChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mDAT0ChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mDAT1ChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mDAT2ChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mDAT3ChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );

	mClockChannelInterface->SetTitleAndTooltip( "Clock", "Standard SDIO" );
	mCmdChannelInterface->SetTitleAndTooltip( "Command", "Standard SDIO" );
	mDAT0ChannelInterface->SetTitleAndTooltip( "DAT0", "Standard SDIO" );
	mDAT1ChannelInterface->SetTitleAndTooltip( "DAT1", "Standard SDIO" );
	mDAT2ChannelInterface->SetTitleAndTooltip( "DAT2", "Standard SDIO" );
	mDAT3ChannelInterface->SetTitleAndTooltip( "DAT3", "Standard SDIO" );

	mClockChannelInterface->SetChannel( mClockChannel );
	mCmdChannelInterface->SetChannel( mCmdChannel );
	mDAT0ChannelInterface->SetChannel( mDAT0Channel );
	mDAT1ChannelInterface->SetChannel( mDAT1Channel );
	mDAT2ChannelInterface->SetChannel( mDAT2Channel );
	mDAT3ChannelInterface->SetChannel( mDAT3Channel );

	mClockChannelInterface->SetSelectionOfNoneIsAllowed( false );
	mCmdChannelInterface->SetSelectionOfNoneIsAllowed( false );
	mDAT0ChannelInterface->SetSelectionOfNoneIsAllowed( false );
	mDAT1ChannelInterface->SetSelectionOfNoneIsAllowed( true );
	mDAT2ChannelInterface->SetSelectionOfNoneIsAllowed( true );
	mDAT3ChannelInterface->SetSelectionOfNoneIsAllowed( true );


	AddInterface( mClockChannelInterface.get() );
	AddInterface( mCmdChannelInterface.get() );
	AddInterface( mDAT0ChannelInterface.get() );
	AddInterface( mDAT1ChannelInterface.get() );
	AddInterface( mDAT2ChannelInterface.get() );
	AddInterface( mDAT3ChannelInterface.get() );

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	// AddChannel( mInputChannel, "Serial", false );
	AddChannel( mClockChannel, "Clock", false );
	AddChannel( mCmdChannel, "Command", false );
	AddChannel( mDAT0Channel, "DAT0", false );
	AddChannel( mDAT1Channel, "DAT1", false );
	AddChannel( mDAT2Channel, "DAT2", false );
	AddChannel( mDAT3Channel, "DAT3", false );
}

SDIOAnalyzerSettings::~SDIOAnalyzerSettings()
{
}

bool SDIOAnalyzerSettings::SetSettingsFromInterfaces()
{
	// mInputChannel = mInputChannelInterface->GetChannel();
	// mBitRate = mBitRateInterface->GetInteger();

	mClockChannel = mClockChannelInterface->GetChannel();
	mCmdChannel = mCmdChannelInterface->GetChannel();
	mDAT0Channel = mDAT0ChannelInterface->GetChannel();
	mDAT1Channel = mDAT1ChannelInterface->GetChannel();
	mDAT2Channel = mDAT2ChannelInterface->GetChannel();
	mDAT3Channel = mDAT3ChannelInterface->GetChannel();

	ClearChannels();
	// AddChannel( mInputChannel, "SDIO", true );

	AddChannel( mClockChannel, "Clock", true );
	AddChannel( mCmdChannel, "Command", true );
	AddChannel( mDAT0Channel, "DAT0", true );
	AddChannel( mDAT1Channel, "DAT1", true );
	AddChannel( mDAT2Channel, "DAT2", true );
	AddChannel( mDAT3Channel, "DAT3", true );
	return true;
}

void SDIOAnalyzerSettings::UpdateInterfacesFromSettings()
{
	// mInputChannelInterface->SetChannel( mInputChannel );
	// mBitRateInterface->SetInteger( mBitRate );
	mClockChannelInterface->SetChannel( mClockChannel );
	mCmdChannelInterface->SetChannel( mCmdChannel );
	mDAT0ChannelInterface->SetChannel( mDAT0Channel );
	mDAT1ChannelInterface->SetChannel( mDAT1Channel );
	mDAT2ChannelInterface->SetChannel( mDAT2Channel );
	mDAT3ChannelInterface->SetChannel( mDAT3Channel );
}

void SDIOAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	text_archive >> mClockChannel;
	text_archive >> mCmdChannel;
	text_archive >> mDAT0Channel;
	text_archive >> mDAT1Channel;
	text_archive >> mDAT2Channel;
	text_archive >> mDAT3Channel;

	ClearChannels();
	AddChannel( mInputChannel, "SDIO", true );

	UpdateInterfacesFromSettings();
}

const char* SDIOAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mClockChannel;
	text_archive << mCmdChannel;
	text_archive << mDAT0Channel;
	text_archive << mDAT1Channel;
	text_archive << mDAT2Channel;
	text_archive << mDAT3Channel;
	// text_archive << mInputChannel;
	// text_archive << mBitRate;

	return SetReturnString( text_archive.GetString() );
}
