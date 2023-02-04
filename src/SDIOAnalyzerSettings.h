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
