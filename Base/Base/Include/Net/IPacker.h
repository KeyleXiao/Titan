/*******************************************************************
** 文件名:	IPacker.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	01/30/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __I_PACKER_H__
#define __I_PACKER_H__

#include "Config.h"

#ifdef SUPPORT_NET

#include "AsynIoFrame.h"
#include <vector>

namespace rkt{

	/**
	@name            : 打包算法
	@brief           : 把用户发送的数据打成包,收到后再按包解开.打包算法理论上用户是可以定制的
	@return       :
	*/
	struct IPacker
	{
		/**
		@name            : 打包函数
		@brief           : 把用户发送的数据打成包
		@param pRawData  : 用户数据
		@param dwDataLen : 数据长度
		@param PackedData: 打好包后的数据,返回的数据必须是new出来的
		@param dwDataFlag: 数据包标志
		@return          :
		*/
		virtual void Pack( const char * pRawData,DWORD dwDataLen,DWORD dwDataFlag ,std::vector<AsynIoBuffer> & PackedData) = 0;

		/**
		@name            : 解包函数
		@brief           : 把收到的流数据解成包
		@param pStreamData: 收到的流数据
		@param dwLen     : 数据长度
		@param UnpackedData: 解出来后的包数据,返回的数据必须是new出来的
		@return          : 如果解包失败返回false
		*/
		virtual bool UnPack( const char * pStreamData,DWORD dwLen,std::vector<AsynIoBuffer> & UnpackedData ) = 0;

		/**
		@name         : 克隆一个打包器
		@brief        : 创建一个和自己同类型的对象
		@return       :
		*/
		virtual IPacker * Clone() = 0;

		virtual void Release() = 0;
	};
};

#endif // #ifdef SUPPORT_NET

#endif//__I_PACKER_H__
