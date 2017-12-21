/*******************************************************************
** 文件名:	VoiceServerSetting.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2011-05-16
** 版  本:	1.0
** 描  述:	语音服务器设定信息
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once


#ifndef __VOICE_SERVER_SETTING_H__
#define __VOICE_SERVER_SETTING_H__

#include "singleton.h"
#include <string>
#include "VoiceDef.h"

class VoiceServerSetting : public Singleton<VoiceServerSetting>
{
public:
	// 读入配置文件
	bool VoiceServerSetting::LoadSetting(void)
	{
		Ini ini("VoiceServer.ini");
		int tracelevel = ini.getInt("Setting","TraceLevel",3);
		int autostart = ini.getInt("Setting","AutoStart",1);
		int autohide  = ini.getInt("Setting","AutoHide",1);
		int enableping= ini.getInt("Setting","EnablePing",1);
		int maxConn	= ini.getInt("Setting","ServerConnMax",10000);
		int maxNetSpeed	= ini.getInt("Setting","NetSpeedMax",1000);
		int clientPort = ini.getInt("Setting","ClientPort", 12850);
		int serverPort = ini.getInt("Setting","ServerPort", 12800);
		int qualityType = ini.getInt("Setting","QualityType", VoiceQuality_High);
		
		m_dwTraceLevel = tracelevel;
		m_bAutoStart   = autostart;
		m_bAutoHide    = autohide;
		m_bEnablePing  = (enableping!=0);
		m_dwMaxConnection = maxConn;
		m_dwClientPort = clientPort;
		m_dwServerPort = serverPort;
		m_dwMaxNetSpeed = maxNetSpeed;
		m_dwQualityType = qualityType;

		// 超出了就用默认
		if (m_dwQualityType<VoiceQuality_Low || m_dwQualityType>VoiceQuality_High)
		{
			m_dwQualityType = VoiceQuality_High;
		}

		ErrorLn(_GT("最大连接数：") << m_dwMaxConnection<<_GT(",最大带宽：")<<m_dwMaxNetSpeed<<_GT("Mbps,默认语音品质类型：")<<m_dwQualityType);

		m_CodeInfo.Info.speex.nBand = 2;
		m_CodeInfo.Info.speex.nQuality = 9;

		int nChannelCount = ini.getInt( "Setting", "ChannelCount", 2 );
		if (nChannelCount>2)
		{
			nChannelCount = 2;
		}
		m_CodeInfo.nChannelCount = (BYTE)nChannelCount;

		int nType = ini.getInt( "Setting", "CodecType", VoiceCodeType_Celt );
		m_CodeInfo.nType = nType;
		if ( nType > 1)
		{
			m_CodeInfo.nType = VoiceCodeType_Speex;
		}

		/*
			Speex是一套主要针对语音的开源免费，无专利保护的音频压缩格式。
			Speex工程着力于通过提供一个可以替代高性能语音编解码来降低语音应用输入门槛 。
			另外，相对于其它编解码器，Speex也很适合网络应用，在网络应用上有着自己独特的优势。
			同时，Speex还是GNU工程的一部分，在改版的BSD协议中得到了很好的支持。

			VBR也称为动态比特率编码，使用这个方式时，你可以选择从最差音质/最大压缩比到最好音质/最低压缩比之间的种种过渡级数，
			在MP3文件编码之时，程序会尝试保持所选定的整个文件的品质，将选择适合音乐文件不同部分的不同比特率来编码。
			主要优点是可以让整首歌的音质都能大致达到我们的品质要求，缺点是编码时无法估计压缩后的文件大小。

			我们默认采用SPEEX压缩

		*/
		switch (m_CodeInfo.nType)
		{
		case VoiceCodeType_Speex:
			// 动态比特率
			m_CodeInfo.Info.speex.bVBREnabled = true;
			m_CodeInfo.Info.speex.bVBREnabled = (ini.getInt( "Setting", "EnableVBR", 1 ) == 1) ? true:false;

			// 影响声音质量，0 - 8KHZ 最差，1 - 16KHZ 中等，2 - 32KHZ 最好
			m_CodeInfo.Info.speex.nBand = ini.getInt( "Setting", "Band", 2 );		

			if (m_CodeInfo.Info.speex.nBand > 2)
			{
				m_CodeInfo.Info.speex.nBand = 1;
			}

			ErrorLn(_GT("Speex声音频段：") << m_CodeInfo.Info.speex.nBand);
			// 声音质量控制，0 ~ 10， 0 最差，10 最好
			m_CodeInfo.Info.speex.nQuality = ini.getInt( "Setting", "Quality", 7 );	
			if (m_CodeInfo.Info.speex.nQuality > 10)
			{
				m_CodeInfo.Info.speex.nQuality = 7;
			}

			ErrorLn(_GT("Speex声音质量：") << m_CodeInfo.Info.speex.nQuality);
			break;

		/*
		
		CELT编解码器是一种音频压缩算法。 像MP3，Vorbis和AAC，它是适合传输高品质的音乐。 与这些格式CELT施加很小的​​延迟的信号，甚至低于典型的Speex，GSM，G.729等格式的语音为中心的。

		用CELT应用程序开发人员可以构建软件，它允许在互联网上的音乐家同台演出，或简单地建立伟大的冠冕堂皇的电话系统。 为什么不是你的电话响好您的立体声吗？

		像其他开放和支配技术访问Xiph.Org基金会CELT不需要特许权使用费和没有繁复的发牌。 
		
		*/

		case VoiceCodeType_Celt:
			m_CodeInfo.Info.celt.bVBREnabled = (ini.getInt( "Setting", "EnableVBR", 1 ) == 1) ? true:false;
			m_CodeInfo.Info.celt.nSampleRate = ini.getInt( "Setting", "SampleRate", 48000 );
			if (m_CodeInfo.Info.celt.nSampleRate < 32000 || m_CodeInfo.Info.celt.nSampleRate > 96000)
			{
				if (m_CodeInfo.Info.celt.nSampleRate < 32000)
					m_CodeInfo.Info.celt.nSampleRate = 32000;
				else
					m_CodeInfo.Info.celt.nSampleRate = 96000;
			}
			m_CodeInfo.Info.celt.nVBRTargetBitrate = ini.getInt( "Setting", "TargetVBRBitRate", 32000 );

			ErrorLn(_GT("CELT采样质量：") << m_CodeInfo.Info.celt.nSampleRate);
			ErrorLn(_GT("CELT比特率：") << m_CodeInfo.Info.celt.nVBRTargetBitrate);

			break;
		}

		m_szPassword =  ini.getString( "Setting", "ServerPassword", "" );


		return true;
	}

	VoiceServerSetting::VoiceServerSetting() : 
		m_dwServerPort(0)
		, m_dwClientPort(0)
		, m_dwTraceLevel(0)
		, m_bAutoStart(FALSE)
		, m_bAutoHide(FALSE)
		, m_bEnablePing(TRUE)
		, m_dwMaxConnection(0)
		, m_dwMaxNetSpeed(0)
		, m_dwQualityType(0)
		{}

public:
	DWORD			m_dwServerPort;				// 服务器监听端口
	DWORD			m_dwClientPort;				// 客户端监听端口
	DWORD           m_dwTraceLevel;				// trace日志级别
	BOOL			m_bAutoStart;				// 自动启动
	BOOL			m_bAutoHide;				// 自动隐藏窗口
	BOOL			m_bIsHide;
	bool			m_bEnablePing;

	SVoiceCodecInfo		m_CodeInfo;				// 语音参数配置
	std::string		m_szPassword;				// 语音服务器密码

	DWORD			m_dwMaxConnection;			// 最大连接数
	DWORD			m_dwMaxNetSpeed;			// 最大网络带宽(Mbps)
	DWORD			m_dwQualityType;			// 频道语音品质类型  EMVoiceQualityType
};

#endif//__VOICE_SERVER_SETTING_H__