#ifndef __INDEXFILEREADER_H__
#define __INDEXFILEREADER_H__

#pragma once

#include "Common.h"
#include "ISchemeEngine.h"

using namespace rkt;

#include "FileSystem/Stream.h"
#include "FileSystem/FileStream.h"
#include "Api.h"
#include "FileSystem/VirtualFileSystem.h"

namespace rkt {

#define MAX_INDEX_NUM				16
#define INDEX_FILE_VER				101
//行压缩时临时使用，每行的最长数据长度
#define MAX_LINE_BUFF_LEN			1024*1024
#define INDEX_FILE_NAME_LENGTH		1024

class RKT_EXPORT CIndexFileReader
{
public:
	CIndexFileReader();
	virtual ~CIndexFileReader();

public:
	// 常驻内存保留
	static void DestroyAllFileMem();
	static bool Decompress(char* outBuffer, unsigned int* outSize, char* inBuffer, unsigned int inSize, int type);

public:
	//打开文件
	bool OpenIndexFile(const char * file_name);
	//关闭文件
	void CloseFile();

	//设置当前使用的索引id和索引的值,如果有相同索引值的话只返回第一个
	//-1是按行索引
	bool SetIndex(int find_index_id, int find_num);

	//多索引设置，根据多个索引确定，如果有相同的话只返回第一个
	//参数为查找的id，以及对应的值，索引数目
	bool SetIndex(int find_index_id[], int find_num[], int find_count);

	//设置为下一个符合当前索引条件的位置,返回是否有下一个
	bool SetNextIndexPos();

	//读取
	bool GetBool(int row_num, bool bDef);
	int	 GetInt(int row_num, int nDef);
	float GetFloat(int row_num, float fDef);
	bool GetString(int row_num, char* pszOutBuffer, int &nLen);

	//兼容csv接口版本
	bool GetBool(int not_use, int row_num, bool bDef, bool not_use2=false)		{ return GetBool(row_num, bDef);};
	int	 GetInt(int not_use, int row_num, int nDef, bool not_use2=false)		{ return GetInt(row_num, nDef); };
	float GetFloat(int not_use, int row_num, float fDef, bool not_use2=false)	{ return GetFloat(row_num, fDef); };
	bool GetString(int not_use, int row_num, char* pszOutBuffer, int &nLen, bool not_use2=false)	{ return GetString(row_num, pszOutBuffer, nLen); };

private:
	// 获取压缩情况下指针位置
	bool GetDecompressBuffPos();
	// 获取位置
	bool GetReadPos(int row_num);

private:
	char*	m_pBuffer;							// 缓存地址
	int		m_nRowCount;						// 行数
	int		m_nColumnCount;						// 列数
	char*	m_line_off;							// 行偏移地址地址
	char*	m_pColumTypes;						// 列类型

	int m_nIndexNum;							// 索引数目
	int m_IndexID[MAX_INDEX_NUM];				// 索引id
	char * index_off_off[MAX_INDEX_NUM];		// 索引的偏移地址

	bool m_bSuccessfulOpenFile;
	bool m_bSuccessfulFind;

	char *FindBufferHead;						// 找到的内存的地址
	int m_nFindBufferLen;						// 找到的内存长度

	char *m_pNowReadPos;						// 在当前找到的内存中读取的位置

	//为了支持相同索引的多次查找，记录一下上一次查找时的位置
	char *m_pLastFindLineOffPos;				// 上次查找的偏移指针的位置
	char *m_pLastFindIndexOffPos[MAX_INDEX_NUM];// 上次查找时各个索引指针位置
	int m_nLastFindIndexNum[MAX_INDEX_NUM];		// 上次查找的各个索引的数据
	int m_nLastFindIndexCount;					// 上次查找的索引数目
	int m_nLastFindLine;						// 上次查到到第几行了

	/*
	*****************************
	特殊属性，文件缓冲是否常驻内存，常驻内存的文件只在第一次OpenIndexFile的时候读取文件缓冲，
	之后文件内容放在缓冲里面，防止在读取文件的时候外部频繁解压文件浪费时间
	常驻内存的时候调用DestroyAllFileMem()删除所有内存
	*/
	bool	m_bAlwaysInMem;
	char m_sFileName[INDEX_FILE_NAME_LENGTH];

	//----------是否安装行压缩了以及压缩类型
	int m_nFileType;
	char * m_pDecompressBuff;
};

} //*namespace rkt/

#endif