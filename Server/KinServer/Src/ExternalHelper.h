/*******************************************************************
** 文件名:	e:\Rocket\Server\KinServer\Src\ExternalHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	xiaobao
** 日  期:	2009/3/9
** 版  本:	1.0
** 描  述:	辅助功能类
** 应  用:	
********************************************************************/

#ifndef __ExternalHelper_h__
#define __ExternalHelper_h__

#include "IActorService.h"
#include "IChatScene.h"
#include "IChatSceneService.h"
#include "SharePropertyDef.h"
#include "IShareServer.h"

// 定义
class CExternalHelper
{
public:

    /// purpose: 发送消息
    //@ param  : nSubModule 战队子模块消息码，参考emMsgKin_
    //@ param  : dwKinID 战队ID，非emMsgKin_Kin的消息可以填0
    //@ param  : nActionID 消息码
    //@ param  : pData 消息内容
    //@ param  : nLen 消息内容长度
    void SendKinSocial(BYTE byKeyAction, int nActionID, DWORD dwKinID,void* pData, int nLen);

    /** 显示提示文字
	*/
	void showSystemMessage(PDBID dwPDBID, EMChatTipID nTipID, LPCSTR szParam = NULL);

    IActorService * getActorByPDBID( PDBID pdbid );

    /** 获取共享人物接口
	@param   
	@param   
	@return  
	*/
    SSharePersonProperty                   getSharePerson(DWORD dwPlayerID);

	// 构造函数
	CExternalHelper(void);

	// 虚构函数
	~CExternalHelper(void);
};
extern CExternalHelper				g_EHelper;

#endif // __ExternalHelper_h__