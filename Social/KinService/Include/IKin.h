/*******************************************************************
** �ļ���:	E:\work\w1\speed\Server\MatchServer\Include\IKin.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/15/2015	17:50
** ��  ��:	1.0
** ��  ��:	
********************************************************************/

#pragma once

#include "KinDef.h"

struct IKin
{
    virtual void release() = 0;

    /// purpose: ȡ��ս�ӵ���������
    virtual int getNumProp(DWORD dwPropID) = 0;

    /// purpose: ȡ��ս������
    virtual LPCSTR getName() = 0;

    /// purpose: ɾ����Ա
    //@ param  : nPDBID ���PDBID
    //@ param  : szName �������
    virtual void delMember(DWORD dwPDBID) = 0;

    /// purpose: ȡ��ս�ӳ�Ա����
    virtual int getMemberCount() = 0;

    /** ��ȡս����ս��
	@param dwKinID		:ս��ID 
	*/
    virtual DWORD getTotalFightScore() = 0;

    /** ��ȡս����ս��
	@param dwKinID		:ս��ID 
	*/
    virtual SAutoAccept* getAutoAccept() = 0;
};