/******************************************************************
** 文件名:	HandleCommandPursuit.h
** 版  权:	(C)  深圳冰川网络技术有限公司
** 创建人:
** 日  期:
** 版  本:	1.0
** 描  述:	靠近目标		
** 应  用:  
******************************************************************/
#ifndef __HandleCommandPursuit_H__
#define __HandleCommandPursuit_H__

#include "IHandleCommand.h"
/**
@name : 靠近目标
@brief: 
*/
class HandleCommandPursuit : public IHandleCommand
{
public:
	/////////////////////////////////////////IHandleCommand/////////////////////////////////////////
	virtual void release();
	virtual bool run();
	virtual void update();
	virtual void printInfo(ulong time)
	{
		WarningLn("HandleCommandPursuit更新超时 200 毫秒" << time);
	}

	virtual DWORD getTimeOut();

    // 取得命令类型
    virtual int getComandType()
    {
        return CommandType_Pursuit;
    }

public:
	/////////////////////////////////////////HandleCommandPursuit/////////////////////////////////////////
	// 构造函数
	HandleCommandPursuit(const SHandleCommand_Pursuit &data);
	// 析构函数
	~HandleCommandPursuit();

private:
	SHandleCommand_Pursuit			m_Data;

	DWORD							m_dwLastMovePosTick;	// 上次移动时间
	DWORD							m_dwTimeOut;			// 超时时间
};

#endif // __HandleCommandPursuit_H__