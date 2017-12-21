/*******************************************************************
** �ļ���:	NetServiceProvider.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	��绪
** ��  ��:	1.0
** ��  ��:	���繩Ӧ�̰�װ��
** Ӧ  ��:		

**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/

#ifndef __NETSERVICE_PROVIDER_H__
#define __NETSERVICE_PROVIDER_H__


#include "Common.h"


#ifdef __cplusplus
extern "C" {
#endif

    // ���繩Ӧ������
    enum NETWORK_SERVICE_PROVIDER
    {
        NET_PROVIDER_DX = 1,            // ��������
        NET_PROVIDER_LT = 2,            // ��ͨ����
        NET_PROVIDER_YD = 3,            // �ƶ�����
    };

    /** 
    @name         : ִ�в�ѯ�ص�����
    @param int    : ���繩Ӧ������
    @return       :
    */
    typedef  void  (*QueryProviderCallBack)(int providerType);

    /** ���ò�ѯ����
    @return       :
    */
    RKT_API void NetworkSerivceProviderDomainSet( const char* szDomain );

    /** ִ�в�ѯ
    @return
    */
    RKT_API bool NetworkSerivceProviderQuery( QueryProviderCallBack callback );

    /** ȡ����ѯ
    @return
    */
    RKT_API bool NetworkSerivceProviderCancelQuery();

#ifdef __cplusplus
}
#endif



#endif  // __NETSERVICE_PROVIDER_H__
