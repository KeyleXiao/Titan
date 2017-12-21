
/*******************************************************************
** �ļ���: e:\CoGame\SchemeEngine\include\MobaGoods.h
** ��  Ȩ: (C) ���ڱ������缼�����޹�˾(www.weimingtech.com)
** ������: ������
** ��  ��: 2015/3/11
** ��  ��: 1.0
** ��  ��: ս��������ȴ���
** Ӧ  ��: 
**************************** �޸ļ�¼ ******************************/
#pragma once
#include "stdafx.h"
#include "IServerGlobal.h"
#include "TimerHandler.h"
#include "ITimerService.h"
#include "MobaGoodsSchemeDef.h"
/*********************************************************
��Ʒ������ȴ������
    1�����ÿ��Զ�����Ʒ����ʹ�õĴ���
    2��������Ʒ��ʹ�ô���
    3��������Ʒ����ȴʱ��
    4��ע�⣺������Ʒһ����������ȴ��ʹ����ȴ �ͳ�����ȴ��
**********************************************************/

enum MultiCDState
{
    State_Charge,	            // ����״̬
    State_UseInterval,          // ʹ�ü��״̬
    State_Full,                 // ����״̬
};

class CMultiCDGoods :public TimerHandler
{
    enum
    {
        ETimerEventID_GoodsMultipleChargedFreeze    = 0,	    // ���س�����ȴ��ʱ��
        ETimerEventID_GoodsMultipleIntervalFreeze   = 1,        // ����ʹ�ü����ȴ��ʱ��
    };

    
public:
    CMultiCDGoods(void);
    ~CMultiCDGoods(void);

    ////////////////////////////////TimerHandler//////////////////////////////////////////
    /** ��ʱ��
	@param   
	@param   
	@return  
	*/
	virtual void                OnTimer( unsigned long dwTimerID );

    /** ������ȴ�����߼���ʱ
	@param   
	@param   
	@return  
	*/
	virtual void                OnGoodsMultipleCDTimeTimer();

	/** ������ȴ����߼���ʱ
	@param   
	@param   
	@return  
	*/
	virtual void                OnGoodsMultipleIntervalTimeTimer();

    /** ������ȴ����߼���ʱ
	@param   
	@param   
	@return  
	*/
    bool                        inital( __IEntity* pEntity, IMobaGoods* pMobaGood,int nInterval,int OverlayCountLimit,bool bResetUseCount, int bInitalUseCount);

    /** �ͷ�
	@param   
	@param   
	@return  
	*/
    void                        release();

    /** ������Ʒ�ܷ�ʹ��
	@param   
	@param   
	@return  
	*/
    bool                        canUse();

    /** �����ȴ
	@param   
	@param   
	@return  
	*/
    void                        AddFreezeTime(EFREEZE_CLASS ecFreezeClass, int nFreezeID);

    /** �����ȴ
	@param   
	@param   
	@return  
	*/
    bool                        ResetCanUseCount();

    /** ���л����
	@param   
	@param   
	*/
    virtual bool			    onSerialize( rkt::ibuffer & in,int flag );

	virtual bool			    deSerialize( rkt::obuf & out,int flag );

    virtual bool			    onLoad( __IEntity* pEntity, IMobaGoods *pMobaGoods);
    
private:

    /** ��ö�����ȴ���ʱ��
	@param   
	@param   
	@return  ���ʱ��
	*/
    int     getMultipleFreezeIntervalTime();


private:
    __IEntity *				m_pMaster;
    IMobaGoods*             m_pMobaGoods;
    // ������ȴ
    MultiCDState            m_eCDState;                             //  ��ȴ״̬
    bool                    m_bIsExistLeftChargedTime;              //  ʣ����ȴ����ʱ���Ƿ����     
    int                     m_nLeftChargeFreezeTime;                //  ������ȴ����ʣ����ȴʱ��
    int                     m_nUseInterval;                         //  ������ȴʹ�ü��
    int                     m_MultipleOverlayCountLimit;            //  ������ȴ���ܴ���
    DWORD                   m_dwChargeFreezeTick;                   //  ���һ�γ�����ȴʱ�� ����ʹ�ü����ȴ�ֿ���       
    bool                    m_bMultipleResetUseCount;               //  ������ȴ����Ʒ
    int                     m_nNeedSubFreezeTime;                  //  ʹ�ü�����ڳ�����ȴ��ʱ����Ҫ�������ȴʱ��
};

