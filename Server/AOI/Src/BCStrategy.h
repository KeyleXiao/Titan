/*******************************************************************
** 文件名:	BCStrategy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	3/09/2015
** 版  本:	1.0
** 描  述:	广播策略类
            
********************************************************************/

#pragma once

#include "ISceneManager.h"
#include "AOI_Manager.h"
#include <hash_map>
#include <vector>
#include <list>

struct IActorService;

// 影响广播策略的相关因子
struct StrategyFactor
{
	Vector3 loc;		// 广播物件三维坐标
	bool validloc;		// 坐标合法性？

	StrategyFactor(int* l = NULL)
	{
		validloc = (l != NULL);
		if (validloc)
		{
			loc.x = (float)l[0]; loc.z = (float)l[1];
		}
	}
};

struct AOI_Proxy;

class BCStrategy
{
public:
	BCStrategy()
	{

	}

	virtual ~BCStrategy()
	{

	}

	virtual BCStrategy* Clone() = 0;

	virtual void Dispatch(AOI_Proxy* pClient, AOI_Proxy* pSrcObj, PACKAGE_PTR package);

	virtual void Run(AOI_Proxy* pClient);

	void RemoveMessage(AOI_Proxy* obj);

protected:
	virtual int GetCoolTick(AOI_Proxy* pClient, AOI_Proxy* pSrcObj);

	float GetDistance(AOI_Proxy* pClient, AOI_Proxy* pSrcObj);

public:
	static DWORD s_dwSendCounts;			// 当前进程发包总数量
	static DWORD s_dwSustainedTime;			// 持续进程发包持续时间

protected:
	struct PreparedPackage
	{
		AOI_Proxy* srcobj;
		PACKAGE_PTR package;
		DWORD sendtick;
	};

	list<PreparedPackage> m_packagelist;
};

class BCStrategyEnter : public BCStrategy
{
public:
	virtual BCStrategy* Clone()
	{
		return new BCStrategyEnter();
	}
};

class BCStrategyLeave : public BCStrategy
{
	virtual BCStrategy* Clone()
	{
		return new BCStrategyLeave();
	}
};

class BCStrategyMove : public BCStrategy
{
	virtual BCStrategy* Clone()
	{
		return new BCStrategyMove();
	}

	virtual int GetCoolTick(AOI_Proxy* pClient, AOI_Proxy* pSrcObj);
};

class BCStrategyAction : public BCStrategy
{
	virtual BCStrategy* Clone()
	{
		return new BCStrategyAction();
	}
};

class BCStrategyState: public BCStrategy
{
	virtual BCStrategy* Clone()
	{
		return new BCStrategyState();
	}
};

class BCStrategyProperty: public BCStrategy
{
	virtual BCStrategy* Clone()
	{
		return new BCStrategyProperty();
	}
};

class BCStrategyLevel: public BCStrategy
{
	virtual BCStrategy* Clone()
	{
		return new BCStrategyLevel();
	}
};

class BCStrategyExp: public BCStrategy
{
	virtual BCStrategy* Clone()
	{
		return new BCStrategyExp();
	}
};

class BCStrategyMaxHP: public BCStrategy
{
	virtual BCStrategy* Clone()
	{
		return new BCStrategyMaxHP();
	}
};

class BCStrategyHP: public BCStrategy
{
	virtual BCStrategy* Clone()
	{
		return new BCStrategyHP();
	}
};

class BCStrategyMaxMP: public BCStrategy
{
	virtual BCStrategy* Clone()
	{
		return new BCStrategyMaxMP();
	}
};

class BCStrategyMP: public BCStrategy
{
	virtual BCStrategy* Clone()
	{
		return new BCStrategyMP();
	}

	virtual int GetCoolTick(AOI_Proxy* pClient, AOI_Proxy* pSrcObj);
};

class BCStrategyPropertySet: public BCStrategy
{
	virtual BCStrategy* Clone()
	{
		return new BCStrategyPropertySet();
	}
};

class BCStrategyEffect: public BCStrategy
{
	virtual BCStrategy* Clone()
	{
		return new BCStrategyEffect();
	}
};

class BCStrategyBuff: public BCStrategy
{
	virtual BCStrategy* Clone()
	{
		return new BCStrategyBuff();
	}
};

class BCStrategySpell: public BCStrategy
{
	virtual BCStrategy* Clone()
	{
		return new BCStrategySpell();
	}
};

class BCStrategyScene: public BCStrategy
{
	virtual BCStrategy* Clone()
	{
		return new BCStrategyScene();
	}
};

class BCStrategyCreate: public BCStrategy
{
    virtual BCStrategy* Clone()
    {
        return new BCStrategyCreate();
    }
};

class BCStrategyDestroy: public BCStrategy
{
    virtual BCStrategy* Clone()
    {
        return new BCStrategyDestroy();
    }
};
