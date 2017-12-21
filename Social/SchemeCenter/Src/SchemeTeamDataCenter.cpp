/*******************************************************************
** 文件名:	SchemeTeamDataCenter.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/01/2015
** 版  本:	1.0
** 描  述:	组队相关配置
********************************************************************/

#include "StdAfx.h"
#include "SchemeTeamDataCenter.h"
#include "ISocialGlobal.h"
#include "TeamDef.h"

/** 
@param   
@param   
@return  
*/
CSchemeTeamDataCenter::CSchemeTeamDataCenter(void)
{
	
}

/** 
@param   
@param   
@return  
*/
CSchemeTeamDataCenter::~CSchemeTeamDataCenter(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeTeamDataCenter::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gSocialGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( TEAM_COMMAND_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        ErrorLn(__FUNCTION__":config file load failed! filename=" << stringPath.c_str());
        return false;
    }

	return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeTeamDataCenter::Close(void)
{
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
bool CSchemeTeamDataCenter::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    bool bRet = false;
    string strConfigPath = SCP_PATH_FORMAT( TEAM_COMMAND_SCHEME_FILENAME );

    if(strcmp(szFileName, strConfigPath.c_str()) == 0)
    {
        bRet = LoadCommandScheme(reader.pCSVReader);
    }

    if(!bRet)
    {
        return false;
    }	

    AfterSchemeLoaded();

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
bool CSchemeTeamDataCenter::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeTeamDataCenter* pNewInstance = NewInstance();
	if(pNewInstance == NULL)
	{
		return false;
	}

	if(pNewInstance->OnSchemeLoad(reader, szFileName))
	{
		return true;
	}
	else
	{
		safeDelete(pNewInstance);
		return false;
	}
}


const SGroupCommandSchemeInfo* CSchemeTeamDataCenter::GetCommandSchemeInfo(DWORD dwCommandID)
{
    if (dwCommandID<=0 || dwCommandID>GROUP_COMMAND_MAX_ID)
    {
        return NULL;
    }

    // 找到命令记录
    TMAP_GroupCommandSchemeInfo::iterator iter = m_mapCommandSchemeInfo.find(dwCommandID);

    if (iter==m_mapCommandSchemeInfo.end())
    {
        return NULL;
    }

    return &(iter->second);
}

bool CSchemeTeamDataCenter::LoadCommandScheme(ICSVReader * pCSVReader)
{
    // 要将以前的数据清掉
    m_mapCommandSchemeInfo.clear();

    // 加载脚本
    SGroupCommandSchemeInfo CommandInfo;

    // 加载每一条记录
    int nNameLen = 0;
    int nCount = pCSVReader->GetRecordCount();
    for(int i = 0; i < nCount; i++)
    {	
        CommandInfo.clear();

        CommandInfo.dwID			= pCSVReader->GetInt(i, 0, 0);		// 命令ID,唯一,1~10 为系统常用的ID,11~13 为用户自定义,15以上为扩展命令
        // 找到记录
        TMAP_GroupCommandSchemeInfo::iterator iter = m_mapCommandSchemeInfo.find(CommandInfo.dwID);

        if(CommandInfo.dwID == 0 || CommandInfo.dwID>GROUP_COMMAND_MAX_ID || iter!=m_mapCommandSchemeInfo.end())
        {
            // 不要跳过，要策划来改，因为已经给予的称号或者即将要给的，都有问题
            Error("加载[命令]配置文件失败,配置文件有重复ID,或无效ID,行="<<i<<",ID="<< CommandInfo.dwID << endl);
            return false;
        }

        nNameLen =sizeof(CommandInfo.szName);
        pCSVReader->GetString(i, 1, CommandInfo.szName,nNameLen );	// 命令名称

        nNameLen =sizeof(CommandInfo.szButtonName);
        pCSVReader->GetString(i, 2, CommandInfo.szButtonName,nNameLen );	// 按钮名称,一个字的名称，用在小按钮上显示,只有系统命令前8个(ID:1~8)才需要

        CommandInfo.nType			= pCSVReader->GetInt(i, 3, 0);		// 分类,命令类型: 0.系统 1.自定义名字
        CommandInfo.nTargetType		= pCSVReader->GetInt(i, 4, 0);		// 目标类型,0.不选目标与目标点,1.选目标,2.选目标点,3.目标为自已
        CommandInfo.nRight			= pCSVReader->GetInt(i, 5, 0);		// 权限,1.成员有权限,2.队长有权限,4.团长有权限,权限为各种权限和
        CommandInfo.nImageID		= pCSVReader->GetInt(i, 6, 0);		// 图标ID
        CommandInfo.nAutoMove		= pCSVReader->GetInt(i, 7, 0);		// 收到命令是否移动到指定位置,0:不移动,1：移动,有移动的才显示箭头效果
        CommandInfo.nMultiShow		= pCSVReader->GetInt(i, 8, 0);		// 主角是否接收多个指示,0.否,只指示最新的命令,1.是,可同时指示多个，如求救	
        CommandInfo.nDelayTime		= pCSVReader->GetInt(i, 9, 0);		// 显示效果时长,单位(秒),0为总是显示	
        CommandInfo.nArrowEffectID	= pCSVReader->GetInt(i, 10, 0);		// 被指挥成员箭头光效ID,有移动的才显示箭头效果		
        CommandInfo.nMemberEffectID	= pCSVReader->GetInt(i, 11, 0);		// 被指挥成员附加光效ID			
        CommandInfo.nMaleSoundID	= pCSVReader->GetInt(i, 12, 0);		// 男音效ID,0为无	
        CommandInfo.nFemaleSoundID	= pCSVReader->GetInt(i, 13, 0);		// 女音效ID,0为无		
        CommandInfo.nTargetPosEffectID	= pCSVReader->GetInt(i, 14, 0);	// 目标指示光效ID	
        CommandInfo.nTargetEffectID	= pCSVReader->GetInt(i, 15, 0);		// 目标附加光效ID
        CommandInfo.nHeadTitleEffectID	= pCSVReader->GetInt(i, 16, 0);		// 头顶指示称号效果ID,参考TitleEffect.csv

        nNameLen =sizeof(CommandInfo.szDesc);
        pCSVReader->GetString(i, 17, CommandInfo.szDesc,nNameLen );		// 命令说明,命令说明文字，显示toptip用

        // 加入到Map中
        m_mapCommandSchemeInfo.insert(TMAP_GroupCommandSchemeInfo::value_type(CommandInfo.dwID,CommandInfo));
    }
    return true;
}

