//==========================================================================
/**
* @file	  : LocaleConfig.h
* @author : zhanggx <zgx510417336@126.com>
* created : 2010-10-11  11:22
* purpose : 国际化功能选项配置类
*/
//==========================================================================

#ifndef __LocaleConfig_h__
#define __LocaleConfig_h__

#include "Common.h"
#include "Singleton.h"


/* LNID
LNID	Version		Lauguage	Nation
000		多字节版	简体		中国大陆
101		国际版		简体		马来西亚
201		国际版		繁体		港澳
202		国际版		繁体		台湾
301		国际版		越南语		越南
*/

//该宏定义编译对应的国际版本
#define VERSION_LNID  201

//版本ID定义
#define Version_Malay		101	// Malay
#define Version_HongKong	201	// HongKong
#define Version_TaiWan		202 // TaiWan
/**例如
#if defined(VERSION_LNID)&&(VERSION_LNID == Version_Malay)
#elif defined(VERSION_LNID)&&(VERSION_LNID == Version_HongKong)
#endif
*/

namespace rkt {

	struct ILocaleConfig
	{	
		virtual void release() = 0;

		// 是否开启聊天关键字过滤
		virtual int isKeywordEnabled() const = 0;
		virtual int isInviteEnabled() const = 0;
	};

	class LocaleConfig : public ILocaleConfig, public SingletonEx<LocaleConfig>
	{
	public:
		
		LocaleConfig();
		virtual ~LocaleConfig();

		bool Load(void);
		virtual void release();
		virtual int isKeywordEnabled() const;
		virtual int isInviteEnabled() const;
		

	private:
		bool			m_bLoaded;			// 是否载入
		int				m_isKeywordEnabled;	// 聊天关键字过滤 1开启，0关闭，默认为1
		int				m_isInviteEnabled;	// 招募 1开启，0关闭，默认0
	};

	RKT_API ILocaleConfig*	getLocaleConfig();


}/// namespace rkt
#endif///__LocaleConfig_h__