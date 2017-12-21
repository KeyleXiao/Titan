/*******************************************************************
** �ļ���:	GameIni.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���� (Carl Chen)
** ��  ��:	04/14/2010
** ��  ��:	1.0
** ��  ��:	��Ϸ����Ŀ¼
** Ӧ  ��:

**************************** �޸ļ�¼ ******************************
** �޸���:
** ��  ��:
** ��  ��:
********************************************************************/

#ifndef __GAME_INI_H__
#define __GAME_INI_H__

#include "Singleton.h"
#include <io.h>
#include "Trace.h"

/**
��д��ϷĿ¼�µ�config.ini�ļ�
1.������Ϸ��ͬ�汾�ڲ�ͬ��Ŀ¼��,ÿ���ж�����߼��ǳ��鷳,������Ҫ��װһ��
2.���ڳ����кܶ�ط�����Ҫ��ȡ��������ļ�,������һ��ȫ�ֱ���,ʡ��ÿ�ζ�Ҫ���¹���
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
����Ŀ¼�µ�config.ini
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

	// ȡ����Ϸ��װĿ¼,"Զ��OnLine.exe"���ڵ�Ŀ¼
	const std::string& GetGamePath()
	{
		static std::string strGamePathDir;

		std::string strGameOnlineFileName = "̩̹.exe";

		// _GAMEMARKET_VER_BEGIN_
		//if (getGameMarketID() == GameMarketID_YY)
		//	strGameOnlineFileName = "�Ϲ�ս��OL.exe";
		// _GAMEMARKET_VER_END_

		strGamePathDir = rkt::getWorkDir();
		DWORD dwFindYZ[4] = { 0 };		// �Ƿ��ҵ� Զ��OnLine.exe
		std::string strFindPath[4];		// �ҵ�Ŀ¼
		std::string strGameOnlineFile = "";

		if (strGamePathDir.size() > 0 && strGamePathDir.rfind('\\') > 0)
		{
			// ������һ��
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
			// û�ҵ�ʱ��������һ��
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
			// �����ϵ�2��
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
			// �����û�ҵ�
			if (!bFind)
			{
				WarningLn("ȡ����Ϸ��װĿ¼ʧ��! strGamePathDir=" << strGamePathDir.c_str());
			}
		}
		else
		{
			WarningLn("ȡ����Ϸ��װĿ¼ʧ��! getWorkDir()=" << strGamePathDir.c_str());
		}

		return strGamePathDir;
	}
};

#endif//__GAME_INI_H__