/*******************************************************************
** 文件名:	NetServiceProvider.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 版  本:	1.0
** 描  述:	网络供应商包装类
** 应  用:		

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __NETSERVICE_PROVIDER_H__
#define __NETSERVICE_PROVIDER_H__


#include "Common.h"


#ifdef __cplusplus
extern "C" {
#endif

    // 网络供应商类型
    enum NETWORK_SERVICE_PROVIDER
    {
        NET_PROVIDER_DX = 1,            // 电信网络
        NET_PROVIDER_LT = 2,            // 联通网络
        NET_PROVIDER_YD = 3,            // 移动网络
    };

    /** 
    @name         : 执行查询回调函数
    @param int    : 网络供应商类型
    @return       :
    */
    typedef  void  (*QueryProviderCallBack)(int providerType);

    /** 设置查询域名
    @return       :
    */
    RKT_API void NetworkSerivceProviderDomainSet( const char* szDomain );

    /** 执行查询
    @return
    */
    RKT_API bool NetworkSerivceProviderQuery( QueryProviderCallBack callback );

    /** 取消查询
    @return
    */
    RKT_API bool NetworkSerivceProviderCancelQuery();

#ifdef __cplusplus
}
#endif



#endif  // __NETSERVICE_PROVIDER_H__
