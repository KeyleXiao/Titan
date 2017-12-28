/*******************************************************************
** 文件名:	IMsg.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/29
** 版  本:	1.0
** 描  述:	所有消息的基类
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/
#pragma once


#pragma pack(1)

///////////////////////////////////////////////////////////////////
// 固定大小的消息基类	这种消息是直接拷贝到流里去的，所以不能有虚函数。
template<class T>
struct IFixMsg
{
	// 构造函数。在基类中初始化，减轻子类工作量
	IFixMsg() { memset(this, 0, sizeof(T)); }
};
#define FixMsgStruct(MsgName)	struct MsgName : public IFixMsg<MsgName>




/*******************************************************************
// 工具方法
********************************************************************/
// 将消息打包 +1参数T(必须为支持位拷贝的类型)
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

// 将消息打包
// pointer如果有值，就插在header后面，TMsg前面
template<class TMsg>
void TBuildObufMsg(obuf &obufData, const SGameMsgHead& header, const TMsg &data, const void* pointer = nullptr, size_t length = 0)
{
	obufData.push_back(&header, sizeof(SGameMsgHead));

	if (pointer)
		obufData.push_back(pointer, length);

	if (std::is_base_of<ISerializableData, TMsg>::value)
	{
		const ISerializableData& isd = (const ISerializableData&)data;
		isd.toBytes(obufData);
	}
	else
		obufData.push_back(&data, sizeof(TMsg));
}


#pragma pack()