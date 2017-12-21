/*******************************************************************
** 文件名:	FileReader.cpp 
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


#include "stdafx.h"
#include "FileReader.h "


//////////////////////////////////////////////////////////////////////////


CFileReader::CFileReader(void)
    : m_pFileData(0)     // 文件数据
    , m_nFileLen(0)      // 文件大小
{

    string strWorkDir = rkt::getWorkDir();
    // 设置搜索路径
    string stringScpDir = strWorkDir;
    stringScpDir += "\\Data";

    std::string strPatchName = strWorkDir + "\\Data\\Data.mpk";


    rkt::Ini config("config.ini");
    // 是否读外包先
    bool bOutFileFirst =  config.getInt("App", "ReadOutFileFirst", 0);
    if(bOutFileFirst)
    {
        getFileSystem()->addFindPath(stringScpDir.c_str());

        if (isFile(strPatchName.c_str())) {
            getFileSystem()->addFindPath( strPatchName.c_str() );
        }
    }else
    {
        if (isFile(strPatchName.c_str())) {
            getFileSystem()->addFindPath( strPatchName.c_str() );
        }
        getFileSystem()->addFindPath(stringScpDir.c_str());
    }
}


// 析构函数
CFileReader::~CFileReader(void)
{
    Close();
}


/** 找开文件
@param   pszFileName：文件名
@param   
@return  
*/
bool CFileReader::Load(const char* pszFileName)
{
    // 清理上次内存
    Close();

    if (!pszFileName || *pszFileName == 0)
        return false;

    uint flags = Stream::modeRead|Stream::shareRead|Stream::modeExist;
    if (isDebugMode()) flags |= Stream::shareWrite;

    FileSystem* fs = getFileSystem();
    // 打开
    Stream* stream = fs->open(pszFileName, flags);
    if (!stream)
    {
        return false;
    }

    bool bResult = true;

    do
    {
        m_nFileLen = stream->getLength();
        if (m_nFileLen <= 0)
        {
            bResult = false;
            break;
        }

        // 文件数据
        m_pFileData = malloc(m_nFileLen);
        if ( m_pFileData ==0 )
        {
            bResult = false;
            break;
        }

        stream->seekToBegin();

        if (!stream->read(m_pFileData, m_nFileLen))
        {
            ErrorLn("Read file failed, filename:"<<pszFileName);
            bResult = false;
            break;
        }

    } while(false);

    stream->close();
    safeRelease(stream);

    // 异常时执行重置
    if(!bResult)
    {
        Close();
    }

    return bResult;
}


// 获取文件大小
rkt::uint CFileReader::GetLength(void)
{
    return m_nFileLen;
}

// 获取文件内容
void *  CFileReader::GetBuffer(void)
{
    return m_pFileData;
}

// 主动关闭
void    CFileReader::Close(void)
{
    if(m_pFileData != NULL)
        free(m_pFileData);

    m_pFileData = 0;

    m_nFileLen = 0;
}
