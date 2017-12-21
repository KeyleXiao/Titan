/******************************************************************
** 文件名:	HandleCommandClose.h
** 版  权:	(C)  深圳冰川网络技术有限公司
** 创建人:
** 日  期:
** 版  本:	1.0
** 描  述:	靠近指定坐标		
** 应  用:  
******************************************************************/
#ifndef __HandleCommandClose_H__
#define __HandleCommandClose_H__

#include "IHandleCommand.h"
/**
@name : 靠近指定坐标
@brief: 
*/
class HandleCommandClose : public IHandleCommand
{
public:
	/////////////////////////////////////////IHandleCommand/////////////////////////////////////////
	virtual void release();
	virtual bool run();
	virtual void update();
	virtual void printInfo(ulong time)
	{
		WarningLn("HandleCommandClose更新超时 200 毫秒" << time);
	}

	virtual DWORD getTimeOut();

    // 取得命令类型
    virtual int getComandType()
    {
        return CommandType_Close;
    }

public:
	/////////////////////////////////////////HandleCommandClose/////////////////////////////////////////
	// 构造函数
	HandleCommandClose(const SHandleCommand_Close &data);
	// 析构函数
	~HandleCommandClose();

private:
	SHandleCommand_Close			m_Data;
	DWORD							m_dwLastMovePosTick;	// 上次移动时间
	DWORD							m_dwTimeOut;			// 超时时间
};

#endif // __HandleCommandClose_H__