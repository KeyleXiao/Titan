/*******************************************************************
** �ļ���:	PlayerMoneyManager.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	��绪
** ��  ��:	7/4/2017
** ��  ��:	1.0
** ��  ��:  ��ҽ�Ǯ������

********************************************************************/

#ifndef __PLAYER_MONEY_MANAGER_H__
#define __PLAYER_MONEY_MANAGER_H__

#include "IBankPart.h"



// ��ҽ�Ǯ������
class CPlayerMoneyManager : public IPlayerMoneyManager
{
public:
    ///////////////////////////////// IPlayerMoneyManager //////////////////////////
    /** ȡ�õ�ȯ�һ���ҵĻ���
    @return : 
    */
    virtual double getExchangeRate(void);

    /** �Ƿ�����ӻ�۳����ң�ֻ����һ�ֻ��ң�֧��һ����ȯת��ҹ��ܣ�
    @param nMoney:      ������ȯ/���������ע������Ϊ�۳�������Ϊ����
    @param nMoneyType:  �������� GAME_MONEY_TYPE,���Ϊ
    @return : 
    */
    virtual bool canAddMoneyEx(int nMoney, BYTE nMoneyType);

    /** �Ƿ�����ӻ�۳�����
    @param nTicket:  ������ȯ������ע������Ϊ�۳�������Ϊ����
    @param nGlod  :  �������������ע������Ϊ�۳�������Ϊ����
    @return : 
    */
    virtual bool canAddMoney(int nTicket, int nGlod);

    /** ���ӻ�۳�����
    @param nTicket: ��ӵĵ�ȯ��������ʾ�۳����
    @param nGlod  : ��ӵĽ�ң�������ʾ�۳����
    @param  : nReason ��ӵ�ԭ�򣨲���OSS_RESOURCE_ADD_RANGE/���ĵ�;��OSS_RESOURCE_OPT_DEL/ת��OSS_RESOURCE_OPT_TRANSIN/ת��OSS_RESOURCE_OPT_TRANSOUT��
    @param  : pszRemark ��ӵĵ�ȯ��־�ı�ע������־�ã�
    @param  : nSubclassID, ����ID��������Ʒ������ƷID��û������Ĭ��0
    @param  : nNum ��������������ƷID������Ʒ����
	@param  : bAddictLimit���Ƿ��ǳ�������
    @ note  : ����ӵ�ȯ�����ύ���ݿ⣨��������ģ���ύ�ģ���û�з���ǰ����ʱ�����ύ��ȯ�ǻ�ʧ�ܵ�
    */
    virtual bool addMoney(int nTicket, int nGlod, int nReason, const char * pszRemark=0, int nSubclassID=0, short nNum=0, bool bAddictLimit=true);

    // ����Ticket���ݿ����
    virtual void onDBReturnTicketAdd(int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
    ///////////////////////////////// CPlayerMoneyManager //////////////////////////

	// ���캯��
	CPlayerMoneyManager();

	// ��������
	~CPlayerMoneyManager();

	// ��ʼ��
	bool init(CPlayerBankPart *pBankPart);


private:
    // ȡ�����Ƕ���
    __IEntity *  getMaster() const;

    // ִ�����ݿ����
    bool executeMoneyDataBase(int nTicket, int nTickReasonID, int nGold, int nGlodReasonID, const char * pszRemark=0, int nSubclassID=0, short nNum=0, int nReserverID=0);

private:

	CPlayerBankPart *   m_pBankPart;

    // �Ƿ����ڲ�����Ǯ����ȯ����ң���DB��DB���غ��Զ�����false
    bool                m_isOperatingToDB;
};

#endif	// __PLAYER_MONEY_MANAGER_H__