/*******************************************************************
** 文件名:	FileReader.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2016-12-23
** 版  本:	1.0
** 描  述:	文件读取类
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __CONTROL_FILE_READER_H__
#define __CONTROL_FILE_READER_H__


#include <list>
#include <map>
using namespace std;

#include "Singleton.h"
#include "ISchemeEngine.h"



/** 文件读取类
*/
class CFileReader : public Singleton<CFileReader>
{
public:
	// 构造函数
	CFileReader(void);

    // 析构函数
    ~CFileReader(void);

    /** 载入文件
    @param   pszFileName：文件名
    @param   
    @return  
    */
    bool                    Load(const char* pszFileName);

	/** 获取文件大小
	@return  
	*/
    rkt::uint               GetLength(void);

	/** 获取文件内容
	@return  
	*/
    void *                  GetBuffer(void);

	/** 主动关闭
	@return  
	*/
    void                    Close(void);



private:
    void*                   m_pFileData;  // 文件数据
    rkt::uint               m_nFileLen;   // 文件大小
};



#endif //__CONTROL_FILE_READER_H__