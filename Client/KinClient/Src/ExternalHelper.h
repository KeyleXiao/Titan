/*******************************************************************
** 文件名:	e:\Rocket\Client\KinClient\Src\ExternalHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2016/3/9
** 版  本:	1.0
** 描  述:	辅助功能类	
** 应  用:	
********************************************************************/

#ifndef __ExternalHelper_h__
#define __ExternalHelper_h__

// 辅助功能类
class CExternalHelper
{
public:
	/// purpose: 构造函数
	CExternalHelper(void);

	/// purpose: 虚构函数
	~CExternalHelper(void);

	/// purpose: 发送消息到社会服
	//@ param  : nSubModule 战队子模块消息码，参考emMsgKin_
	//@ param  : dwKinID 战队ID，非emMsgKin_Kin的消息可以填0
	//@ param  : nActionID 消息码
	//@ param  : pData 消息内容
	//@ param  : nLen 消息内容长度
	void SendKinSocial(BYTE bySubModule,BYTE byActionID, DWORD dwKinID,  void* pData = NULL, int nLen = 0);

	/// purpose: 发送消息
	void SendKinZone(BYTE byActionID, void* pData, int nLen);


private:
};
extern CExternalHelper				g_EHelper;

#endif // __ExternalHelper_h__
 