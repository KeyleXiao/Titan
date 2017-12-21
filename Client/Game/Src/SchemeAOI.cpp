/*******************************************************************
** 文件名:	SchemeAOI.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	6/01/2015
** 版  本:	1.0
** 描  述:	AOI通用配置
********************************************************************/

#include "StdAfx.h"
#include "SchemeAOI.h"
#include "IClientGlobal.h"

/** 
@param   
@param   
@return  
*/
CSchemeAOI::CSchemeAOI(void)
{
    m_fCheckDistance = 10.0f;
    m_nBufTime = 5;
}

/** 
@param   
@param   
@return  
*/
CSchemeAOI::~CSchemeAOI(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeAOI::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
	if ( pSchemeEngine==NULL )
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( "Scp\\AOI.xml" );
	bool bResult = pSchemeEngine->LoadScheme( stringPath.c_str(),(ISchemeUpdateSink *)this );
	if ( !bResult )
	{
		Error("config file load failed! filename=" << stringPath.c_str() << endl);
		return false;
	}

	return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeAOI::Close(void)
{
}

// 获取AOI警戒距离
float CSchemeAOI::getGuardDistance()
{
    return m_fCheckDistance;
}

// 获取警戒缓冲时间
int CSchemeAOI::getGuardBufTime()
{
    return m_nBufTime;
}

/** CSV配置载入时通知
@param   pCSVReader：读取CSV的返回接口
@param   szFileName：配置文件名
@param   
@return  
@note     
@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
@retval buffer 
*/ 
bool CSchemeAOI::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	TiXmlDocument *pXMLReader = reader.pXMLReader;
	if( pXMLReader==NULL || szFileName==NULL || reader.type!=READER_XML )
	{
		return false;
	}

	TiXmlElement *pRootElement = pXMLReader->FirstChildElement( "AOI" );
	if( pRootElement==NULL )
	{
		return false;
	}

	// 场景对像延迟销毁时间
	TiXmlElement *pSubElement = pRootElement->FirstChildElement("guard");
	if( pSubElement==NULL )
	{
		return false;
	}

	pSubElement->QueryFloatAttribute("check_distance", &m_fCheckDistance);
    pSubElement->QueryIntAttribute("buftime", &m_nBufTime);

    ENTITY_TRACELN( "check_distance="<<m_fCheckDistance<<", buftime="<<m_nBufTime );

	return true;
}

/** 配置动态更新时通知
@param   pCSVReader：读取CSV的返回接口
@param   szFileName：配置文件名
@param   
@return  
@note     
@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
@retval buffer 
*/
bool CSchemeAOI::OnSchemeUpdate( SCRIPT_READER reader,const char* szFileName )
{
	return OnSchemeLoad( reader,szFileName );
}
