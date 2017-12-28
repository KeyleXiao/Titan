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
// ����Ϣ��� +1����T(����Ϊ֧��λ����������)
template<class T, class TMsg>
void TBuildObufMsg(obuf &obufData, const SGameMsgHead& header, T t, const TMsg &data)
{
	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData << t;

	if (std::is_base_of<ISerializableData, TMsg>::value)
	{
		const ISerializableData& isd = (const ISerializableData&)data;
		isd.toBytes(obufData);
	}
	else
		obufData.push_back(&data, sizeof(TMsg));
}

// ����Ϣ���
// pointer�����ֵ���Ͳ���header���棬TMsgǰ��
template<class TMsg>
void TBuildObufMsg(obuf &obufData, const SGameMsgHead& header, const TMsg &data, const void* pointer = nullptr, size_t size = 0)
{
	obufData.push_back(&header, sizeof(SGameMsgHead));

	if (pointer && size > 0)
	{
		obufData << size;
		obufData.push_back(pointer, size);
	}

	if (std::is_base_of<ISerializableData, TMsg>::value)
	{
		const ISerializableData& isd = (const ISerializableData&)data;
		isd.toBytes(obufData);
	}
	else
		obufData.push_back(&data, sizeof(TMsg));
}

// ����Ϣ���(ʡ��header)
// pointer�����ֵ���Ͳ���header���棬TMsgǰ��
template<class TMsg>
void TBuildObufMsg(obuf &obufData, const TMsg &msg, const void* pointer = nullptr, size_t size = 0)
{
	SGameMsgHead header;
	header.SrcEndPoint = msg.GetSrcEndPoint();
	header.DestEndPoint = msg.GetDestEndPoint();
	header.byKeyModule = msg.GetModuleId();
	header.byKeyAction = msg.GetActionId();

	obufData.push_back(&header, sizeof(SGameMsgHead));

	if (pointer && size > 0)
	{
		obufData << size;
		obufData.push_back(pointer, size);
	}

	if (std::is_base_of<ISerializableData, TMsg>::value)
	{
		const ISerializableData& isd = (const ISerializableData&)msg;
		isd.toBytes(obufData);
	}
	else
		obufData.push_back(&msg, sizeof(TMsg));
}


#pragma pack()