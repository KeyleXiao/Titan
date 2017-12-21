/*******************************************************************
** 文件名:	GameIni.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	04/14/2010
** 版  本:	1.0
** 描  述:	游戏配置目录
** 应  用:

**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#ifndef __GAME_INI_H__
#define __GAME_INI_H__

#include "Singleton.h"
#include <io.h>
#include "Trace.h"

/**
读写游戏目录下的config.ini文件
1.由于游戏不同版本在不同的目录下,每次判断这个逻辑非常麻烦,所以需要封装一下
2.由于程序中很多地方都需要读取这个配置文件,所以做一个全局变量,省得每次都要重新构造
*/
class GameIni : public Ini, public Singleton<GameIni>
{
public:
	GameIni()
	{
		std::string strGamePathDir = rkt::getWorkDir();
		strGamePathDir += "\\config.ini";
		setPath(strGamePathDir.c_str());
	}
};


/**
更新目录下的config.ini
*/
class UpdateIni : public Ini, public Singleton<UpdateIni>
{
public:
	UpdateIni()
	{
		char config_file[MAX_PATH];
		std::string strRootPath = GetGamePath();
		sprintf(config_file, "%s\\config.ini", strRootPath.c_str());
		setPath(config_file);
	}

	// 取得游戏安装目录,"远征OnLine.exe"所在的目录
	const std::string& GetGamePath()
	{
		static std::string strGamePathDir;

		std::string strGameOnlineFileName = "泰坦.exe";

		// _GAMEMARKET_VER_BEGIN_
		//if (getGameMarketID() == GameMarketID_YY)
		//	strGameOnlineFileName = "上古战神OL.exe";
		// _GAMEMARKET_VER_END_

		strGamePathDir = rkt::getWorkDir();
		DWORD dwFindYZ[4] = { 0 };		// 是否找到 远征OnLine.exe
		std::string strFindPath[4];		// 找到目录
		std::string strGameOnlineFile = "";

		if (strGamePathDir.size() > 0 && strGamePathDir.rfind('\\') > 0)
		{
			// 往上找一层
			size_t nPos = strGamePathDir.rfind('\\');
			if (nPos != std::string::npos)
			{
				strGamePathDir = strGamePathDir.substr(0, nPos);
				strFindPath[0] = strGamePathDir;
				strGameOnlineFile = strGamePathDir + "\\" + strGameOnlineFileName;
				if (_access(strGameOnlineFile.c_str(), 0) != -1)
				{
					dwFindYZ[0] = 1;
				}
			}
			// 没找到时再往上找一层
			nPos = strGamePathDir.rfind('\\');
			if (nPos != std::string::npos)
			{
				strGamePathDir = strGamePathDir.substr(0, nPos);
				strFindPath[1] = strGamePathDir;
				strGameOnlineFile = strGamePathDir + "\\" + strGameOnlineFileName;
				if (_access(strGameOnlineFile.c_str(), 0) != -1)
				{
					dwFindYZ[1] = 1;
				}
			}
			bool bFind = false;
			// 优先上第2层
			if (dwFindYZ[1] == 1)
			{
				strGamePathDir = strFindPath[1];
				bFind = true;
			}
			else if (dwFindYZ[0] == 1)
			{
				strGamePathDir = strFindPath[0];
				bFind = true;
			}
			// 最后还是没找到
			if (!bFind)
			{
				WarningLn("取得游戏安装目录失败! strGamePathDir=" << strGamePathDir.c_str());
			}
		}
		else
		{
			WarningLn("取得游戏安装目录失败! getWorkDir()=" << strGamePathDir.c_str());
		}

		return strGamePathDir;
	}
};

#endif//__GAME_INI_H__