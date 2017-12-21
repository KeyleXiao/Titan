/*******************************************************************
** �ļ���:	IEntity.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���� (Carl Chen)
** ��  ��:	8/1/2014
** ��  ��:	1.0
** ��  ��:	

			ʵ��ӿ�
********************************************************************/


#pragma once

#include "ISerializable.h"
#include "GameDef.h"

#include "EntityDef.h"

class Vector3;
struct __IEntity;

//////////////////////// �ؾ߳������� /////////////////////////////
enum
{
	TANK_CARRYTYPE_UNKNOWN = 0,			// δ֪
	TANK_CARRYTYPE_BUS = 1,				// ����������ֻ�����أ�
	TANK_CARRYTYPE_CHARIOT,				// ս���������أ��ɹ�����
	TANK_CARRYTYPE_BATTLEPLANE,			// ս�����������أ��ɹ������ɿ�Խռλ�飩
	TANK_CARRYTYPE_BMW,					// ˫������
	TANK_CARRYTYPE_TRUCK,				// ����(�����أ��ɹ���)
	TANK_CARRYTYPE_PK,					// pk�ؾ�
	TANK_CARRYTYPE_DYNAMICRELIVE,		// �������ؾ�
	TANK_CARRYTYPE_LOCKBOSS,			// �������ؾ�
	TANK_CARRYTYPE_SHIP,				// ��
};

////////////////////////////�ؾ߶���////////////////////////////////////
struct ITank : public __IEntity
{
	/** ȡ�óƺ�
	@param   
	@param   
	@return  
	*/
	virtual LPCSTR				GetTitle(void) = NULL;

	/** ���óƺ�
	@param   
	@param   
	@return  
	*/
	virtual void				SetTitle(LPCSTR pszTitle) = NULL;

	/** ȡ�ñ�������Ϣ
	@param   
	@param   
	@return  
	*/
	virtual STankSchemeInfo *	GetSchemeInfo(void) = NULL;

	/** ��ɢ����
	@param   
	@param   
	@return  
	*/
	//virtual void				KillSelf(void) = NULL;

	/** �����ϴ��Ƴ���tileλ��
	@param   
	@param   
	@return  
	*/
	virtual void				DebugLastRemoveTile(POINT ptLastTile) = NULL;

	/** ͬ�������л���ͼ
	@param   
	@param   
	@return  
	*/
	//virtual bool				ChangeZone(DWORD dwNewZoneID, POINT ptNewTile) = NULL;

	/** ��ͬ�������л���ͼ
	@param   
	@param   
	@return  
	*/
	//virtual bool				ChangeServer(DWORD dwNewZoneID, POINT ptNewTile, bool bCoercive, DWORD serveridTarget) = NULL;

	/** �������ݿ������ֳ�
	@param   
	@param   
	@return  
	*/
	virtual bool				ExportMapContext(LPSTR pszContext, int &nLen) = NULL;

	/** ȡ����չ����
	@param   
	@param   
	@return  
	*/
	virtual LPCSTR				GetExtendData(void) = NULL;

	/** ������չ����
	@param szExtendData ��չ����
	@param nLen			��չ���ݳ���  
	@return  
	*/
	//virtual bool				SetExtendData(LPCSTR szExtendData, int nLen) = NULL;

	/** �Ƿ�PK�ؾ�
	@return  
	*/
	virtual bool				IsPKTank() = NULL;

	/** PK�ؾ��Ƿ��е�ǰ����
	@return  
	*/
	virtual bool				IfHavePKTankSkill(int nSpellID) = NULL;

	/** �������ļ������buf
	@param   
	@param   
	@return  
	*/
	virtual bool				AddBuffFromScheme(void) = NULL;

	/** �ж��Ƿ�����ʱ�����ؾ�
	@name				: 
	@param				: 
	@return	
	*/
	virtual bool				CanJumpOut(void) = NULL;

	/** �ж��Ƿ���ò�����ʾ
	@name				: 
	@param				: 
	@return
	*/
	virtual bool				IsForbidTip(void) = NULL;

	/** �Ƿ��ƶ�ʱ��Ͻ�����
	@name				: 
	@param				: 
	@returns
	*/
	virtual bool				IsMoveBreakProgBar(void) = NULL;
	/* ���� �����¼�
	@parm:	bBeforeΪ trueʱ��ʾ ����д��ǰ���ô˽ӿ�  
					  false ��ʾ ����д�����ô˽ӿ�
	@parm:  data д��ǰΪ ��ӦҪ�޸ĵ����Ե�ֵ�ĵ�ַд���û��ʹ��
	*/
	virtual void OnPropertyChange(bool bBefore,int prop_id, void * data,int32 len) = NULL;
};
