#include "stdafx.h"
#include "SchemeMapArea.h"
#include "IClientGlobal.h"
#include "EntityViewDef.h"
#include <string>


/** 
@param   
@param   
@return  
*/
CSchemeMapArea::CSchemeMapArea(void)
{	

}

/** 
@param   
@param   
@return  
*/
CSchemeMapArea::~CSchemeMapArea(void)
{
	Close();
}

/** 
@param   
@param   
@return  
*/
bool CSchemeMapArea::LoadScheme(void)
{
	ISchemeEngine * pSchemeEngine = gClientGlobal->getSchemeEngine();
	if (pSchemeEngine == NULL)
	{
		return false;
	}

	return true;
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeMapArea::Close(void)
{	

}

/** CSV配置载入时通知
@param   type   : 读取器类型
		        : 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
				: READER_XML : 就可以把pReader转换成TiXmlDocument*
@param   pReader：读取CSV的返回接口
@param   szFileName：配置文件名
@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
@retval buffer 
*/ 
bool CSchemeMapArea::OnSchemeLoad(SCRIPT_READER reader, LPCSTR szFileName)
{
    return true;
}

/** 配置动态更新时通知
@param   type   : 读取器类型
		        : 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
				: READER_XML : 就可以把pReader转换成TiXmlDocument*
@param   pReader：读取CSV的返回接口
@param   szFileName：配置文件名
@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
@retval buffer 
*/
bool CSchemeMapArea::OnSchemeUpdate(SCRIPT_READER reader, LPCSTR szFileName)
{
	return OnSchemeLoad(reader, szFileName);
}
