/*******************************************************************
** 文件名:	E:\speed\Server\MatchServer\Src\State.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	4/24/2015	11:23
** 版  本:	1.0
** 描  述:	
            状态基类
********************************************************************/

#pragma once


class IState
{
public:
	// 进入状态
	virtual void onEnter() = 0;
	// 离开状态
	virtual void onLeave() = 0;

    virtual void restoreState() = 0;

	uint32	getID(){return m_u32ID;};

    virtual void    release() = 0;


public:
	// 状态ID
	uint32	m_u32ID;


public:
	IState(uint32 u32ID)
		:m_u32ID(u32ID)
	{

	}
	virtual ~IState()
	{

	}
};