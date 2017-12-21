#ifndef __EMOJIMANAGER_H__
#define __EMOJIMANAGER_H__

#include "IBankPart.h"

class CPlayerBankPart;

class CEmojiManager
{
public:
	// 构造
	CEmojiManager();

	// 析构
	~CEmojiManager();

	// 序列化
	bool onSerialize(ibuffer& in);

	// 反序列化
	bool deSerialize(obuf& out);

	// 初始化
	bool init(CPlayerBankPart * pBankPart, __IEntity * pMaster, int nLoginMode);

    void restore();

	// 释放
	void release();
	
	// 是否可以添加表情包
	bool canAddEmoji(BYTE byEmojiType);

	// 添加表情包
	bool addEmoji(BYTE byEmojiType);

	// DB读表情包结果
	void onDBReturnEmojiInfo(vector<DBREQ_TABLE_VALUE_GOODS> * pvecGoods);

private:
	// 更新表情包数据到客户端
	void updateEmojiInfoToClient();

	// 检测是否拥有免费的Emoji表情
	void checkHaveFreeEmoji();

private:
	__IEntity * m_pMaster;
	CPlayerBankPart * m_pBankPart;

	// 表情包信息
	vector<BYTE>	m_vecEmojiInfo;
};


#endif