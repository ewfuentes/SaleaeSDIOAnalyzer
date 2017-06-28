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

	// check channel selection
	{
		Channel d0, d1, d2, d3;
		d0 = mDAT0ChannelInterface->GetChannel();
		d1 = mDAT1ChannelInterface->GetChannel();
		d2 = mDAT2ChannelInterface->GetChannel();
		d3 = mDAT3ChannelInterface->GetChannel();

		if (d1 == UNDEFINED_CHANNEL && d2 == UNDEFINED_CHANNEL && d3 == UNDEFINED_CHANNEL)
		{
			// this is valid, continue
		}
		else if (d1 != UNDEFINED_CHANNEL && d2 != UNDEFINED_CHANNEL && d3 != UNDEFINED_CHANNEL)
		{
			// this is also valid, continue
		}
		else
		{
			// invalid combination
			SetErrorText("Invalid data line selection. If D0 is set, either all or none of the other data lines must be set.");
			return false;
		}

		std::vector<Channel> channels;
		channels.push_back(d0);
		channels.push_back(d1);
		channels.push_back(d2);
		channels.push_back(d3);
		channels.push_back(mClockChannelInterface->GetChannel());
		channels.push_back(mCmdChannelInterface->GetChannel());

		if (AnalyzerHelpers::DoChannelsOverlap(channels.data(), channels.size()) == true)
		{
			SetErrorText("Channel selections must be unique");
			return false;
		}
	}

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
	AddChannel( mDAT1Channel, "DAT1", mDAT1Channel != UNDEFINED_CHANNEL);
	AddChannel( mDAT2Channel, "DAT2", mDAT2Channel != UNDEFINED_CHANNEL);
	AddChannel( mDAT3Channel, "DAT3", mDAT3Channel != UNDEFINED_CHANNEL);
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

	AddChannel( mClockChannel, "Clock", true );
	AddChannel( mCmdChannel, "Cmd", true );
	AddChannel( mDAT0Channel, "DAT0", true );
	AddChannel( mDAT1Channel, "DAT1", mDAT1Channel != UNDEFINED_CHANNEL);
	AddChannel( mDAT2Channel, "DAT2", mDAT2Channel != UNDEFINED_CHANNEL);
	AddChannel( mDAT3Channel, "DAT3", mDAT3Channel != UNDEFINED_CHANNEL);

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
