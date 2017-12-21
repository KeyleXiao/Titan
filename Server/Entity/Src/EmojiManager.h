#ifndef __EMOJIMANAGER_H__
#define __EMOJIMANAGER_H__

#include "IBankPart.h"

class CPlayerBankPart;

class CEmojiManager
{
public:
	// ����
	CEmojiManager();

	// ����
	~CEmojiManager();

	// ���л�
	bool onSerialize(ibuffer& in);

	// �����л�
	bool deSerialize(obuf& out);

	// ��ʼ��
	bool init(CPlayerBankPart * pBankPart, __IEntity * pMaster, int nLoginMode);

    void restore();

	// �ͷ�
	void release();
	
	// �Ƿ������ӱ����
	bool canAddEmoji(BYTE byEmojiType);

	// ��ӱ����
	bool addEmoji(BYTE byEmojiType);

	// DB����������
	void onDBReturnEmojiInfo(vector<DBREQ_TABLE_VALUE_GOODS> * pvecGoods);

private:
	// ���±�������ݵ��ͻ���
	void updateEmojiInfoToClient();

	// ����Ƿ�ӵ����ѵ�Emoji����
	void checkHaveFreeEmoji();

private:
	__IEntity * m_pMaster;
	CPlayerBankPart * m_pBankPart;

	// �������Ϣ
	vector<BYTE>	m_vecEmojiInfo;
};


#endif