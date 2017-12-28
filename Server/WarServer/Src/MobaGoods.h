/*******************************************************************
** �ļ���: e:\CoGame\SchemeEngine\include\MobaGoods.h
** ��  Ȩ: (C) ���ڱ������缼�����޹�˾(www.weimingtech.com)
** ������: ������
** ��  ��: 2015/3/11
** ��  ��: 1.0
** ��  ��: ս��������Ϣ
** Ӧ  ��: 
**************************** �޸ļ�¼ ******************************/
#pragma once

#include "stdafx.h"
#include "MobaGoodsSchemeDef.h"
#include "MobaGoodsProp.h"
#include "IEntity.h"
#include "IFreezeServer.h"
#include "SpellDef_ManagedDef.h"
#include "EntityHelper.h"
#include "ISpell.h"
#include "ITimerService.h"
#include "MultiCDGoods.h"

using namespace SPELL;
class CMobaGoods: public IMobaGoods
{
public:
	CMobaGoods();

	virtual ~CMobaGoods();

	virtual void				Initial(__IEntity * pPerson, int nSlotIndex, int nGoodID);

	virtual void				AddGoodsEffect();

	virtual void				RecomputeGoodsEffect();

	virtual void				RemoveGoodsEffect();

	virtual bool				CanAddOverlayCount();

	virtual bool				AddOverlayCount();

	virtual int					GetOverLayCount();

	virtual void				SetOverLayCount(int nOverLayCount);

    virtual void                addCreateMonsterInheritEffect(UID uid);

	/** �Ƿ��ǳ�����Ʒ
	@param
	@param
	@return
	*/
	virtual bool 				bIsMultipleCDGoods();

    /** ���ø��²�λ����
	@param   
	@param   
	@return 
	*/
    virtual void				UpdateGoodSlotInfo();

	/** ��ȡ��Ʒ����ID
	@param   
	@param   
	@return 
	*/
	virtual int 				GetSkillID();

	/** �Ƴ�
	@param   
	@param   
	*/
	virtual bool				Remove();


	/** �����ƷID
	@param   
	@param   
	@return  
	*/
	virtual int					GetGoodID();

	/** �����Ʒ��λ
	@param   
	@param   
	@return  
	*/
	virtual int					GetSlotIndex();

	/** �����Ʒ��ʹ������
	@param   
	@param   
	@return  
	*/
	virtual int					GetCanUseCount();

	/** �ܷ�ʹ��
	@param   
	@param   
	@return  
	*/
	virtual bool				CanUse();

	/** �ܷ�ʹ��
	@param   
	@param   
	@return  
	*/
	virtual bool				Use();


	/** ȡ��������Ϣ
	@param   
	@param   
	@return  
	*/
	virtual SSchemeMobaGoods*	GetSchemeInfo();

	/** ȡ����������
	@param   
	@param   
	@return  
	*/
	virtual __IEntity *			GetMaster();

	/** ��������
	@param   
	@param   
	@return  
	*/
	virtual void				ConsumedOverlayCount();

	/** ����ʹ�ô���
	@param   
	@param   
	@return  
	*/
	virtual void				ConsumedUseCount();

	/** ��ȡ��ʹ�õĴ��� 
	@param   
	@param   
	@return  �ܵ��ӵ���Ʒ�����ۻ������� ���Ĵ�����Ʒ�ķ�����ʹ�õĴ���
	*/
	virtual int					GetCanUseTimes();

	/** �����ȴ
	@param   
	@param   
	@return  
	*/
	virtual void				AddFreezeTime(EFREEZE_CLASS FreezeClass, int nFreezeID);

	/** ��ȡ��Ʒ�Ƿ��������
	@param   
	@param   
	@return 
	*/
	virtual bool 				GetIsNewAddGoods() ;

	/** ������Ʒ�Ƿ��������
	@param   
	@param   
	@return 
	*/
	virtual void 				SetIsNewAddGoods(bool bState);

	/** ��ȡ��Ʒ����
	@param   
	@param   
	@return  �ܵ��ӵ���Ʒ�����ۻ������� ���Ĵ�����Ʒ�ķ�����ʹ�õĴ���
	*/
	virtual emMobaGoodType		GetGoodsType();

	/** ����ʹ�ÿ���ʹ�ô���
	@param   
	@param   
	@return  �������Ĵ�����Ʒ��ʹ�õĴ���
	*/
	virtual bool				ResetCanUseCount();
  
    /** ���ÿ���ʹ�õĴ���
	@param   
	@param   
	@return 
	*/
	virtual void				SetCanUseCount(int nCanUseCount);
    
	/** ���ö�����ȴ��ʼ��������ȴ��������
	@param   
	@param   
	@return  
	*/
	virtual void                SetMultipleCoolDown(int nInterval,int OverlayCountLimit,bool bResetUseCount, int bInitalUseCount);
	

    /** �ͷ�
	@param   
	@param   
	@return  
	*/
	virtual void				Release();

    /** ���л����
	@param   
	@param   
	*/
    virtual bool			    onSerialize( rkt::ibuffer & in,int flag );

	virtual bool			    deSerialize( rkt::obuf & out,int flag );

    virtual bool			    onLoad( __IEntity* pEntity, int nLoginMode);

    void                        recordeMultipleCDInfo();


private:

	__IEntity *				m_pMaster;
	bool					m_bIsNewAdd;
	int						m_nGoodID;
	int						m_nSlotIndex;
	int						m_nCurOverlayCount; //	��ǰ��������
	int						m_nCanUseCount;		//	��ʹ�ô���
	emMobaGoodType			m_GoodsType;
    bool                    m_bMultipleCDGoods;
	
	//Ч��ID Ч��״̬�ṹ
	typedef map<int,SMobaGoodEffectInfo>	T_MAP_EFFECT;
	T_MAP_EFFECT			m_mapEffect;

    T_MAP_EFFECT            m_mapInheritEffect;         //�ٻ�����̳�����

    CMultiCDGoods*          m_pMultiCDGoods;
};