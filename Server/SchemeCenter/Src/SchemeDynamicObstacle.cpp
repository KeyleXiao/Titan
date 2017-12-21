#include "stdafx.h"
#include "SchemeDynamicObstacle.h"
#include "IServerGlobal.h"
#include <string>

/** 
@param   
@param   
@return  
*/
CSchemeDynamicObstacle::CSchemeDynamicObstacle(void)
{
	// 所有地图配置属性	
	m_mapSchemeInfo.clear();
}

/** 
@param   
@param   
@return  
*/
CSchemeDynamicObstacle::~CSchemeDynamicObstacle(void)
{
	// 所有地图配置属性	
	m_mapSchemeInfo.clear();
}

/** 
@param   
@param   
@return  
*/
bool CSchemeDynamicObstacle::LoadScheme(void)
{
	ISchemeEngine * pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL )
	{
		return false;
	}

    string stringPath = SCP_PATH_FORMAT( DYNAMICOBSTACLE_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this, false);
	if(!bResult)
	{
		ErrorLn("config file load failed! filename=" << stringPath.c_str());
		return false;
	}

	return true;
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeDynamicObstacle::Close(void)
{	
	// 所有地图配置属性	
	m_mapSchemeInfo.clear();
}

/** 取得地图配置信息
@param   
@param   
@return  
*/
DynamicObstacleScheme * CSchemeDynamicObstacle::GetSchemeInfo(int nDynamicMonsterID)
{
	T_SchemeInfo::iterator itM = m_mapSchemeInfo.find( nDynamicMonsterID );
	if(itM != m_mapSchemeInfo.end())
	{
		return &((*itM).second);
	}

	return NULL;
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
bool CSchemeDynamicObstacle::OnSchemeLoad(SCRIPT_READER reader, LPCSTR szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    // 清空
    m_mapSchemeInfo.clear();

    int nLen = 0;
    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for (int nRow=0; nRow<nRecordCount; ++nRow)
    {
        DynamicObstacleScheme item;
        ZeroMemory(&item,sizeof(DynamicObstacleScheme));

        // 动态障碍怪ID
        item.nDynamicMonsterID = pCSVReader->GetInt(nRow, OBS_COL_ID, 0);
        // 形状类型
        item.nShape = pCSVReader->GetInt(nRow, OBS_COL_SHAPE, 0);
        // 如果是四边形则是长度，如果是六边形则是六边形的内接圆半径
        item.fLength = pCSVReader->GetFloat(nRow, OBS_COL_LENGTH, 0);
        // 如果是四边形则是宽度，如果是六边形则是六边形的内接圆半径,与上面的值一样
        item.fWidth = pCSVReader->GetFloat(nRow, OBS_COL_WIDTH, 0);
        // 动态障碍怪旋转角度
        item.fAngle = pCSVReader->GetFloat(nRow, OBS_COL_ANGLE, 0);
        // 动态障碍怪实体是否可穿越标志位
        item.nEntityCampFlag = pCSVReader->GetInt(nRow, OBS_COL_ENTITY_CAMP_FLAG, 0);
        // 动态障碍技能阵营标志位
        item.nSpellCampFlag = pCSVReader->GetInt(nRow, OBS_COL_SPELL_CAMP_FLAG, 0);
        // 动态障碍技能是否可穿越标志位
        item.nSpellFlag = pCSVReader->GetInt(nRow, OBS_COL_SPELL_FLAG, 0);
		// 动态障碍是否预加载（允许在客户端创建前生成）
		item.nPreloadFlag = pCSVReader->GetInt(nRow, OBS_COL_PRELOAD_FLAG, 0);
		

        m_mapSchemeInfo.insert(make_pair<unsigned int, DynamicObstacleScheme>(item.nDynamicMonsterID, item));
    }

	AfterSchemeLoaded();

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
bool CSchemeDynamicObstacle::OnSchemeUpdate(SCRIPT_READER reader, LPCSTR szFileName)
{
	return false;
}