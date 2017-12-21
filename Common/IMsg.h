/*******************************************************************
** �ļ���:	IMsg.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/11/29
** ��  ��:	1.0
** ��  ��:	������Ϣ�Ļ���
** Ӧ  ��:
**************************** �޸ļ�¼ ******************************
** �޸���:
** ��  ��:
** ��  ��:
********************************************************************/
#pragma once


#pragma pack(1)

///////////////////////////////////////////////////////////////////
// �̶���С����Ϣ����	������Ϣ��ֱ�ӿ���������ȥ�ģ����Բ������麯����
template<class T>
struct IFixMsg
{
	// ���캯�����ڻ����г�ʼ�����������๤����
	IFixMsg() { memset(this, 0, sizeof(T)); }
};
#define FixMsgStruct(MsgName)	struct MsgName : public IFixMsg<MsgName>




/*******************************************************************
// ���߷���
********************************************************************/
// ����Ϣ���
template<class TMsg>
void TBuildObufMsg(obuf &obufData, const SGameMsgHead& header, TMsg &data)
{
	obufData.push_back(&header, sizeof(SGameMsgHead));

	if (std::is_base_of<ISerializableData, TMsg>::value)
	{
		const ISerializableData& isd = (const ISerializableData&)data;
		isd.toBytes(obufData);
	}
	else
		obufData.push_back(&data, sizeof(TMsg));
}

// ������Ϣ
template<class TConn, class TMsg>
void TSendMsg(TConn* pConnection, const SGameMsgHead& header, TMsg& data)
{
	if (pConnection == NULL) return;

	obuf obufData;
	TBuildObufMsg(obufData, header, data);

	const DWORD dwLen = (DWORD)obufData.size();
	Assert(dwLen == obufData.size());
	pConnection->SendData(obufData.data(), dwLen);
}

#pragma pack()