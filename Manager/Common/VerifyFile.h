/*******************************************************************
** 文件名:	VerifyFile.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2010-4-27
** 版  本:	1.0
** 描  述:	校检文件类
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __VerifyFile_H__
#define __VerifyFile_H__

#pragma once

#include <list>
#include <map>
using namespace std;


// 校检文件节点信息
struct SVerifyFileInfo
{
	DWORD dwID;						// 序号ID
	DWORD dwFileSize;				// 文件大小
	unsigned char VerifyData[20];	// 校检码数据
	string strFileName;				// 文件名

	SVerifyFileInfo(void)
	{
		clear();
	};
	void clear(void)
	{
		dwID = 0;
		dwFileSize = 0;
		strFileName.clear();
		memset(VerifyData, 0, sizeof(VerifyData));
	};
};

// 校检文件类
class CVerifyFile
{
	#define SAVEVERIFYFILEHEADFLAG		2010518		// 保存校检文件头打头标识
	#define MG_VerifyGetFileListMax		1000		// 能取的最多文件数

	// 保存校检文件头信息
	struct SSaveVerifyFileHead
	{
		DWORD dwHeadFlag;				// 打头标识 SAVEVERIFYFILEHEADFLAG,否则无效文件
		DWORD dwCounts;					// 节点数
		DWORD dwDataSize;				// 总数据大小
		DWORD dwDateTime;				// 时间
		DWORD dwType;					// 类型

		SSaveVerifyFileHead(void)
		{
			memset(this, 0, sizeof(*this));
			dwHeadFlag = SAVEVERIFYFILEHEADFLAG;
		};
	};
	// 保存校检文件节点信息
	struct SSaveVerifyFileNode
	{
		DWORD dwFileSize;				// 文件大小
		unsigned char VerifyData[20];	// 校检码数据
		WORD nStrLens;					// 文件名长度
		// char szFileName[nStrLens];	// 文件名,加在所有节点数据后

		SSaveVerifyFileNode(void)
		{
			memset(this, 0, sizeof(*this));
		};
	};

public:

	typedef std::map<string,SVerifyFileInfo> TMAP_FileList;

	TMAP_FileList m_mapList;

	CVerifyFile();
	
	virtual ~CVerifyFile();

	// 创建
	bool Create();
	// 释放
	void Release();
	// 取得ID
	DWORD GetID();
	// 设置ID
	void SetID(DWORD dwID);
	// 输出描述
	std::string ToString();
	// 设置显示信息
	void SetTitle(const char * szTitle);
	// 取得打包数据
	const BYTE * GetFileData();
	// 取得打包数据长度
	DWORD GetFileSize();
	// 清除数据
	void Clear();
	// 清除内存数据
	void ClearFileData();
	// 从文件中加载校检列表信息
	bool LoadFromFile(const char * szFileName);
	// 从指定目录中加载校检列表信息,checkList为指定列表,removeList为指定排除列表,为空列出目录所有,默认最多10层子目录,bAddMode=true为增加模式
	bool LoadFromPath(const char * szPathName,bool bAddMode=false,std::list<string> *checkList=NULL,std::list<string> *removeList=NULL,DWORD dwMaxLayer=10);
	// 从内存中加载校检列表信息
	bool LoadFromMemory(const BYTE * pBufData,DWORD dwSize);
	// 从另外的校检文件对象加载校检列表信息
	bool LoadFromOther(CVerifyFile &OtherVerify);
	// 保存校检列表信息到指定文件
	bool SaveDataToFile(const char * szFileName);
	// 从指定文件加载校检信息
	bool GetVerifyInfo(const char * szFileName,SVerifyFileInfo &verifyNode);
	// 从指定目录得到所有文件列表
	bool GetFileList(const char * szPathName,std::list<string> &checkList,DWORD dwMaxLayer=10);
	// 从列表中找到指定文件名称校检信息
	SVerifyFileInfo * GetUser(const char * szFileName);
	// 导出所有文件列表
	bool ExportFileList(std::list<string> &outList);

	// 比较两个校检文件节点信息是否一致,srcNode源,destNode目标
	static bool CheckVerifyNode(SVerifyFileInfo &srcNode,SVerifyFileInfo &destNode);
	// 比较两个校检信息是否一致,srcNode源,destNode目标
	static bool CheckVerify(CVerifyFile &srcNode,CVerifyFile &destNode,const char * szLogFileName=NULL);

private:
	// 方法
	// 取得内存
	void GetDataMemory(DWORD dwSize);
	// 释放内存
	void FreeDataMemory();
	// 取得保存文件数据内容,并分配内存
	void GetSaveVerifyData();
	// 从指定目录得到文件列表
	bool DoGetFileList(string strRootPathName,string strPathName,std::list<string> &checkList,DWORD &dwLayer,DWORD dwMaxLayer=10);

private:

	DWORD m_dwID;			// 本地ID
	DWORD m_dwFileSize;		// 文件内容数据长度
	DWORD m_dwMemorySize;	// 内存长度
	BYTE * m_pFileData;		// 数据,析构函数释放

public:

	std::string m_strFileName;		// 文件名
	std::string m_strName;			// 校检名
	bool m_bLoad;					// 文件已加载
	DWORD m_dwTypeID;				// 类型ID
	DWORD m_dwState;				// 状态
	std::string m_strTitle;			// 文件信息说明
};

#endif//__VerifyFile_H__