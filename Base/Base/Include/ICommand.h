/*******************************************************************
** 文件名:	ICommand.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	02/21/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __INTERFACE_COMMAND_H__
#define __INTERFACE_COMMAND_H__

#ifdef SUPPORT_COMMANDFACTORY

#include <string>

namespace rkt
{
	/**
	@name : 命令接口
	@brief:
	*/
	struct ICommand
	{
		/**
		@name         : 取得命令名称
		*/
		virtual const std::string & GetCommandName() = 0;

		/**
		@name         : 从输入的命令行中解释命令参数
		@param cmd_str: 命令行字串
		@return       : 如果格式不对则返回false
		*/
		virtual bool Parse( const std::string & cmd_str ) = 0;

		/**
		@name         : 执行命令
		@return       : 命令执行成功则返回true,否则使用GetError返回错误提示
		*/
		virtual bool Execute() = 0;

		/**
		@name         : 取得错误提示
		@brief        : 
		*/
		virtual const std::string & GetError() const = 0;

		/**
		@name         : 从当前的模版克隆一个相同的命令对象
		@brief        :
		*/
		virtual ICommand * Clone() = 0;

		/**
		@name         : 销毁命令对象
		@brief        :
		*/
		virtual void Release() = 0;
	};

	/**
	@name : 命令工厂
	@brief:
	*/
	struct ICommandFactory
	{
		/**
		@name              : 注册一个命令
		@brief             :
		@param pCmdTemplate: 该命令的模版对象,以后通过这个模版创建新命令,程序退出时工厂会Release这个对象
		@return            :
		*/
		virtual void RegisterCommand(ICommand * pCmdTemplate) = 0;

		/**
		@name         : 根据输入的命令行,创建一个执行该语句的命令对象
		@brief        : 如果没有这个命令则返回0
		@param cmd_line: 命令行语句
		@return       : 命令对象
		*/
		virtual ICommand * CreateCommand( const char * cmd_line ) = 0;
	};

};

/**
@name         : 获得命令工厂实例
@brief        :
*/
//COMMAND_API ICommandFactory * GetCommandFactory();

RKT_API rkt::ICommandFactory * GetCommandFactory();
#define	GetCommandFactoryProc	GetCommandFactory


#endif // #ifdef SUPPORT_COMMANDFACTORY

#endif//__INTERFACE_COMMAND_H__