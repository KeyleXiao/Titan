/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MatchServer\Include\TeamSchemeDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/18/2015	16:36
** 版  本:	1.0
** 描  述:	组队、团配置相关
********************************************************************/

#pragma once
#include <string>

#define TEAM_COMMAND_SCHEME_FILENAME 			"Scp\\Command.csv"					// 组命令配置名字
// Command.csv
// 组团命令配置信息
struct SGroupCommandSchemeInfo
{
    DWORD				dwID;				// 命令ID,唯一,1~10 为系统常用的ID,11~13 为用户自定义,15以上为扩展命令
    char				szName[32];			// 命令名称
    char				szButtonName[8];	// 按钮名称,一个字的名称，用在小按钮上显示,只有系统命令前8个(ID:1~8)才需要
    int					nType;				// 分类,命令类型: 0.系统 1.自定义名字
    int					nTargetType;		// 目标类型,0.不选目标与目标点,1.选目标,2.选目标点,3.目标为自已
    int					nRight;				// 权限,1.成员有权限,2.队长有权限,4.团长有权限,权限为各种权限和
    int					nImageID;			// 图标ID,,0为无
    int					nAutoMove;			// 收到命令是否移动到指定位置,0:不移动,1：移动,有移动的才显示箭头效果
    int					nMultiShow;			// 主角是否接收多个指示,0.否,只指示最新的命令,1.是,可同时指示多个，如求救
    int					nDelayTime;			// 显示效果时长,单位(秒),0为总是显示
    int					nArrowEffectID;		// 被指挥成员箭头光效ID,有移动的才显示箭头效果
    int					nMemberEffectID;	// 被指挥成员附加光效ID
    int					nMaleSoundID;		// 男音效ID,0为无
    int					nFemaleSoundID;		// 女音效ID,0为无
    int					nTargetPosEffectID;	// 目标指示光效ID
    int					nTargetEffectID;	// 目标附加光效ID
    int					nHeadTitleEffectID;	// 头顶指示称号效果ID,参考TitleEffect.csv
    char				szDesc[256];		// 命令说明,命令说明文字，显示toptip用

    SGroupCommandSchemeInfo(void)
    {
        memset(this, 0, sizeof(*this));
    };
    void clear(void)
    {
        memset(this, 0, sizeof(*this));
    };
};

// 命令脚本列表,以命令ID为关键字
typedef map<DWORD, SGroupCommandSchemeInfo>	TMAP_GroupCommandSchemeInfo;

// 取得组队配置中心
class ISchemeTeamDataCenter
{
public:

    /// purpose: 取得命令脚本定义数据
    virtual const SGroupCommandSchemeInfo* GetCommandSchemeInfo(DWORD dwCommandID) = NULL;

};