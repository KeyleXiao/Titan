/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\SchemeCenter\Src\SchemeRune.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	吴欣伟
** 日  期:	14/3/2016	18:36
** 版  本:	1.0
** 描  述:	
            符文脚本读取
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeDef.h"
#include "SchemeCache.h"

using namespace std;

#ifdef FUNCTION_RUNE_OPEN

class CSchemeRuneInfo : public ISchemeRuneInfo,
                        public ISchemeUpdateSink,
                        public CSchemeCache<CSchemeRuneInfo>
{
	typedef map<unsigned int, RuneInfoScheme>	T_MapRuneInfo;
public:
	///////////////////////////////IRuneInfo///////////////////	
    // 获取某个指定符文的配置数据
	virtual RuneInfoScheme * GetRuneSchemeInfo(int nRuneID);

	///////////////////////////////ISchemeUpdateSink///////////////////
    /** CSV配置载入时通知
    @param   type   : 读取器类型
    : 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
    : READER_XML : 就可以把pReader转换成TiXmlDocument*
    @param   pReader：读取CSV的返回接口
    @param   szFileName：配置文件名
    @warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
    @retval buffer 
    */ 
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);

    /** 配置动态更新时通知
    @param   type   : 读取器类型
				    : 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
				    : READER_XML : 就可以把pReader转换成TiXmlDocument*
    @param   pReader：读取CSV的返回接口
    @param   szFileName：配置文件名
    @warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
    @retval buffer
    */
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	///////////////////////////////CSchemePathFindServiceInfo//////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool LoadScheme(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeRuneInfo(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeRuneInfo(void);

private:
    /** 获取出生点 
	@param   
	@param   
	@return  
	*/
	bool GetRuneProperty(ICSVReader* pCSVReader, size_t nRow, int nIndex, RuneInfoScheme& schemeinfo);

private:
	// 所有寻路服务配置属性	
	T_MapRuneInfo m_SchemeInfo;
};

#endif