/*******************************************************************
** �ļ���: e:\CoGame\SchemeEngine\include\MobaLeechdom.h
** ��  Ȩ: (C) ���ڱ������缼�����޹�˾(www.weimingtech.com)
** ������: ������
** ��  ��: 2015/3/11
** ��  ��: 1.0
** ��  ��: ս��������Ϣ
** Ӧ  ��: 
**************************** �޸ļ�¼ ******************************/
#pragma once

#include "stdafx.h"
#include "MobaGoodsDef.h"
#include "MobaGoodsSchemeDef.h"
#include "IEntity.h"
#include "MobaGoods.h"

class CMobaLeechdom: public CMobaGoods
{
public:
	CMobaLeechdom();

	~CMobaLeechdom();

	virtual void				Initial(__IEntity * pPerson, int nSlotIndex, int nGoodID);

	virtual void				AddGoodsEffect();

	virtual void				RemoveGoodsEffect();

	/** ����ʹ�ÿ���ʹ�ô���
	@param   
	@param   
	@return  �������Ĵ�����Ʒ��ʹ�õĴ���
	*/
	virtual bool				ResetCanUseCount();

	/** �Ƴ�
	@param   
	@param   
	*/
	virtual bool				Remove();


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

	virtual void				Release();

private:
};