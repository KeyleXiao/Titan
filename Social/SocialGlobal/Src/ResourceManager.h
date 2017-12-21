/*******************************************************************
** 文件名:	ResourceManager.h  
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2016-12-17
** 版  本:	1.0
** 描  述:	资源管理类
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IResourceManager.h"



class CResourceManager : public IResourceManager
{
#ifndef SAFE_DELETEARRAY
#   define SAFE_DELETEARRAY(data)   if(data) { delete[] (data); data=NULL;};
#endif
    struct ServerResourceData
    {
        BYTE            nEncry;         // 加密方式
        BYTE            nResourceType;  // 资源类型
        BYTE			nSchemeIndex;	// 配置号（从一号脚本开始读取）
        DWORD			nBufferLen;		// 数据长度
        DWORD			dwSize;			// 内存长度
        BYTE *			pData;			// 数据,析构函数释放

        ServerResourceData(void)
        {
            memset(this, 0, sizeof(*this));
        };

        ~ServerResourceData(void)
        {
            SAFE_DELETEARRAY(pData);
            dwSize = 0;
        };

        void clear(void)
        {
            SAFE_DELETEARRAY(pData);
            dwSize = 0;

            memset(this, 0, sizeof(*this));
        };

        void getDataMemory(DWORD nSize)
        {
            SAFE_DELETEARRAY(pData);

            pData = (BYTE*)new char[nSize];
            dwSize = nSize;
        };

        void freeDataMemory(void)
        {
            SAFE_DELETEARRAY(pData);
            dwSize = 0;
        };
    };
    // 数据列表
    typedef DWORD    SCHEMEINDEX;
    typedef map<SCHEMEINDEX, ServerResourceData>     TMAP_RESOURCEDATA;

    typedef DWORD    RESOURCE_TYPE;
    typedef map<RESOURCE_TYPE, TMAP_RESOURCEDATA>    TMAP_RESOURCE;   


    struct RES_BUFF_NODE    
    {
        void  *     pDataBuff;
        int         nSize;

        RES_BUFF_NODE(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };
    typedef map<RESOURCE_TYPE, RES_BUFF_NODE>    TMAP_RESOURCE_BUFFER; 
public:

	/** 
	@param   
	@param   
	@return  
	*/
	CResourceManager(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CResourceManager(void);

	/** 加载
	@return  
	*/
    virtual bool	        load();

	/** 执行Service的on_stop
	@return  
	*/
    virtual void            onStop(void);

	/** 释放
	@note     释放所有资源，并且销毁此对像
	@warning 
	*/
	virtual void			release(void);

    /** 执行收到的消息
	*/
	virtual void            onRecvMessage(const char * pData, size_t nLen);

    /** 获取指定类型的资源
	*/
    virtual int             getData(int nResourceType, void **ppOutBuff);

    /* 是否加载好DLL
    */
    virtual bool            isLoadDLL(void) { return (m_hModule!=NULL); }

    /* 设置是否从PACK中加载LUA
    */
    virtual void            setFromPackLoadLua(bool bFromPack ) { m_bFromPackLoadLua = bFromPack; }

    /* 是否从PACK包中加载LUA
    */
    virtual bool            isLoadLuaFromPack(void) { return m_bFromPackLoadLua; }

    /** 释放指定资源
	*/
    virtual void            releaseData(int nResourceType);

private:
    void*                               m_hModule;
    bool                                m_bFromPackLoadLua;     // 是否从MPK中读取LUA
    // 脚本数据
    TMAP_RESOURCE						m_mapResources;
   TMAP_RESOURCE_BUFFER                 m_mapResourceBuffer;

};
