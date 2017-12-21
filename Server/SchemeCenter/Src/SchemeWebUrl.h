/*******************************************************************
** 文件名:	SchemeWebUrl.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/6/2017
** 版  本:	1.0
** 描  述:	游戏Web配置 WebUrl.csv


**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"


// 定义
class CSchemeWebUrl : public ISchemeWebUrl, public ISchemeUpdateSink, public CSchemeCache<CSchemeWebUrl>
{
    enum SCHEME_COLUMN
    {
        COL_ID = 0,                         // ID
        COL_NAME,                           // 名称
        COL_IS_SHOWNAV,                     // 客户端导航显示
        COL_CLIENT_FUNC_TYPE,               // 功能类型(客户端函数channel；0上发到逻辑层)
        COL_IS_AUTOLOGIN,                   // 是否自动登录
        COL_PREV_URL,                       // 前置URL
        COL_FOLLOW_URL,                     // 结尾URL
    };

public:
	//////////////////////////////// ISchemeWebUrl ////////////////////////////////
    // 获取指定配置
    virtual SSchemeWebUrl*                          getByID(DWORD nID) override;

    // 获取所有的配置
    virtual std::map<DWORD, SSchemeWebUrl*>*        getScheme() override;

    //////////////////////////////// ISchemeUpdateSink ////////////////////////////////
    /** CSV配置载入时通知
    @param   type   : 读取器类型
    : 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
    : READER_XML : 就可以把pReader转换成TiXmlDocument*
    @param   pReader：读取CSV的返回接口
    @param   szFileName：配置文件名
    @warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
    @retval buffer
    */
    virtual bool			                        OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);


    /** 配置动态更新时通知
    @param   pCSVReader：读取CSV的返回接口
    @param   szFileName：配置文件名
    @param
    @return
    @note
    @warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
    @retval buffer
    */
    virtual bool			                        OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeWebUrl//////////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool											LoadScheme(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void											Close(void);

    // 释放所有的节点数据
    void                                            ReleaseMapNodes();

	/** 
	@param   
	@param   
	@return  
	*/
    CSchemeWebUrl(void);

private:
    typedef  std::map<DWORD, SSchemeWebUrl*>        URL_SCHEME_MAP;
    URL_SCHEME_MAP                                  m_mapSchemes;
};