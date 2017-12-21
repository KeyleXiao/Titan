/*******************************************************************
** 文件名:	Command.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	02/18/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __GATEWAY_COMMAND_H__
#define __GATEWAY_COMMAND_H__

#include <string>
#include <map>

#include "Common.h"
#include "ICommand.h"

#ifdef SUPPORT_COMMANDFACTORY

#pragma warning(push)
#pragma warning(disable : 4251)

namespace rkt
{

	/**
	@name : 命令参数对象
	@brief: 目前只支持整型和字符串类型
	*/
	class RKT_EXPORT Parameter
	{
	public:
		enum TYPE
		{
			enType_Nil  = 0,
			enType_Int     ,
			enType_String  ,
		};
		
		std::string       m_strValue; // 字串参数值
		int               m_nValue;   // 整型参数值

		TYPE              m_Type;     // 参数类型
		std::string       m_Desc;     // 参数描述
		std::string       m_Detail;   // 详细描述
		bool              m_IsOption; // 是否可选
		bool              m_Selected; // 是否选择了这个参数

		Parameter() : m_Type(enType_Nil),m_IsOption(true),m_Selected(false){}

		Parameter(const int & val) : m_Type(enType_Int),m_nValue(val),m_IsOption(true),m_Selected(false){}

		Parameter(const std::string & val) : m_Type(enType_String),m_strValue(val),m_IsOption(true),m_Selected(false){}
	};

	/**
	@name : 命令对象
	@brief: 实现命令对象的描述/解释/执行
	*/
	class RKT_EXPORT Command : public ICommand
	{
	public:
		/**
		@name         : 取得命令名称
		*/
		virtual const std::string & GetCommandName();

		/**
		@name         : 取得该命令的格式字串
		@brief        : 例如: NETSTAT [-a] [-b] [-e] [-p protocol] [interval]
		@note         : 参数形式约定:
		@note         : 1.[]内为可选参数,()号内为必填参数
		@note         : 2.有名字没有值   ex. [-a]
		@note         : 3.有名字有值     ex. [-p protocol]
		@note         : 4.没名字有值     ex. [interval]
		*/
		virtual const std::string & GetCommandFormat();

		/**
		@name         : 取得该命令功能的简单描述
		@brief        : 
		*/
		virtual const std::string & GetCommandDesc();

		/**
		@name         : 取得该命令的描述字串
		@brief        : 包括参数在内的详细描述
		@note         : 细节描述的格式:
		@note         : 功能描述 + 换行
		@note         : 参数格式 + 换行
		@note         : 参数名   + 参数描述 + 换行
		*/
		virtual const std::string & GetCommandDescDetail();

		/**
		@name         : 从输入的命令行中解释命令参数
		@param cmd_str: 命令行字串
		@return       : 如果格式不对则返回false
		*/
		virtual bool Parse( const std::string & cmd_str );

		/**
		@name         : 执行命令
		@return       : 命令执行成功则返回true,否则使用GetError返回错误提示
		*/
		virtual bool Execute();

		/**
		@name         : 取得错误提示
		@brief        : 如果没出错则这个字串是空
		*/
		virtual const std::string & GetError() const;

		virtual void Release() { delete this; }

		virtual Command * Clone() = 0;

		Command(const std::string & name,const std::string & desc) : m_Name(name),m_Desc(desc),m_nUnnamedParamNum(0){}

	protected:
		/**
		@name         : 给这个命令定义一个字串参数
		@brief        : 
		@param   key  : 参数键值,即[-a] [-b]中的a和b,如果key传空则为[interval]形式
		@param defval : 默认数值
		@param bOption: 是否是可选参数
		@param desc   : 该参数的描述，即[interval]中的interval
		@param detail : 该参数的详细说明
		@return       :
		*/
		bool DeclareStrParam(const std::string & key,const std::string & defval,bool bOption,const std::string & desc,const std::string & detail);

		/**
		@name         : 给这个命令定义一个整形参数
		@brief        : 
		@param   key  : 参数键值,即[-a] [-b]中的a和b,如果key传空则为[interval]形式
		@param defval : 默认数值
		@param bOption: 是否是可选参数
		@param desc   : 该参数的描述，即[interval]中的interval
		@param detail : 该参数的详细说明
		@return       :
		*/
		bool DeclareIntParam(const std::string & key,int defval,bool bOption,const std::string & desc,const std::string & detail);

		bool GetIntParam(const std::string & key,int & value);

		bool GetStrParam(const std::string & key,std::string & value);

		bool IsOptionOpen(const std::string & key);

		bool GetNextSegment(const char * cmd,int & begin_segment,int & segment_len);

		Command(){}

	protected:
		typedef std::map<std::string,Parameter>   PARAM_TABLE;
		PARAM_TABLE     m_Params;    // 参数列表
		std::string     m_Name;      // 命令名称
		std::string     m_Output;    // 错误提示
		std::string     m_Format;    // 命令格式
		std::string     m_Desc;      // 函数功能简单描述
		std::string     m_Detail;    // 包括参数信息在内的详细描述
		int             m_nUnnamedParamNum; // 无名参数个数
	};
};

#pragma warning(pop)

#endif // #ifdef SUPPORT_COMMANDFACTORY

#endif//__GATEWAY_COMMAND_H__