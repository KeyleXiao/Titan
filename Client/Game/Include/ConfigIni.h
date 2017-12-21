/*******************************************************************
** 文件名:	ConfigIni.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-1-13
** 版  本:	1.0
** 描  述:	Ini配置文件

			
********************************************************************/

#ifndef __CONFIG_INI_H__
#define __CONFIG_INI_H__

#include "Singleton.h"

using namespace rkt;

// 实体命令统计
enum ENTITY_CMD_STAT
{
    ENTITY_CMD_STAT_CLOSE = 0,
    ENTITY_CMD_STAT_CMD_PROP,               // 数值命令统计
    ENTITY_CMD_STAT_CMD_ALL,                // 所有命令统计
};

enum
{
    PERF_CHECK_ENTITY_CMD_INVALID	=0,		// 无效的命令
    PERF_CHECK_ENTITY_CMD_SELF		=1<<0,	// 监控自己角色的命令
    PERF_CHECK_ENTITY_CMD_OTHERHERO =1<<1,	// 监控其他角色
    PERF_CHECK_ENTITY_CMD_MONSTER	=1<<2,	// 监控怪物的命令

};

// Ini配置文件
class ConfigIni : public Singleton<ConfigIni>
{
public:
    bool isOpenPropStat()       { return ((m_nEntityStat & ENTITY_CMD_STAT_CMD_PROP)==ENTITY_CMD_STAT_CMD_PROP);}
    bool isOpenCmdStat()       { return ((m_nEntityStat & ENTITY_CMD_STAT_CMD_ALL)==ENTITY_CMD_STAT_CMD_ALL);}

    bool isAccumulate()         { return m_statMode != 0; }
    int getCmdFlag()            { return m_nCmdFlag; }
    int getCmdInterVal()        { return m_nCmdInterVal; }
    int getCmdMinVal()          { return m_nCmdMinOutput; }

    int getMsgFlag()            { return m_nMsgFlag; }
    int getMsgInterVal()        { return m_nMsgInterVal; }
    int getMsgMinVal()          { return m_nMsgMinOutput; }
    int getOpenSpellOutput()    { return m_nOpenSpellOutput; }
    int getSpellMinVal()        { return m_nSpellMinOutput; }

    int getAlertFPS()           { return m_nFPS; }
    int getFPSInterVal()        { return m_nFPSInterVal; }

    ConfigIni()
    {
        // 实体命令过滤
        Ini ini("config.ini");

        // 统计值累积，1=累积,0=不累积
        m_statMode      = ini.getInt("App", "statmode", 0);

        // 命令
        // 开启统计 0.关闭统计 0x1.数值命令统计 0x2.所有命令统计
        m_nEntityStat   = ini.getInt("App", "EntityStat", ENTITY_CMD_STAT_CLOSE);
        m_nCmdFlag      = ini.getInt("App", "EntityCmdFlag", 0);
        m_nCmdInterVal  = ini.getInt("App", "CmdInterVal", 0);
        m_nCmdMinOutput = ini.getInt("App", "CmdMinOutput", 0);

        // 消息
        m_nMsgFlag      = ini.getInt("App", "EntityMsgFlag", 0);
        m_nMsgInterVal  = ini.getInt("App", "MsgInterVal", 0);
        m_nMsgMinOutput = ini.getInt("App", "MsgMinOutput", 0);

        // 是否开启技能延迟输出,默认0不输出
        m_nOpenSpellOutput = ini.getInt("App", "OpenSpellOutput", 0);
        // 技能最小延迟输出，默认0输出所有
        m_nSpellMinOutput = ini.getInt("App", "SpellMinOutput", 0);

        // 检测FPS输出阀值，0将不输出
        m_nFPS = ini.getInt("App", "AlertFPS", 0);
        // 检测FPS统计均值间隔ms，少于或的等于0时默认1000ms
        m_nFPSInterVal = ini.getInt("App", "FpsInterVal",1000);
    }

private:
    // 统计值累积，1=累积,0=不累积
    int m_statMode;

    // 开启统计 0.关闭统计 0x1.数值命令统计 0x2.所有命令统计
    int m_nEntityStat;
    // 监控命令 0x1.主角,0x2.其他角色,0x4.怪物
    int m_nCmdFlag;
    // 监控命令输出时间ms，默认60000，0.将不输出
    int m_nCmdInterVal;
    // 输出单个最小命令总数量，默认0输出所有
    int m_nCmdMinOutput;

    // 实体消息 0x1.主角,0x2.其他角色,0x4.怪物
    int m_nMsgFlag;
    // 监控消息输出时间 ms，默认60000，0.将不输出
    int m_nMsgInterVal;
    // 输出单个最小消息总数量，默认0输出所有
    int m_nMsgMinOutput;

    // 是否开启技能延迟输出,默认0不输出
    int m_nOpenSpellOutput;
    // 技能最小延迟输出，默认0输出所有
    int m_nSpellMinOutput;

    // 检测FPS输出阀值，0将不输出
    int m_nFPS;
    // 检测FPS统计均值间隔ms，少于或的等于0时默认1000ms
    int m_nFPSInterVal;
};

#endif  // __CONFIG_INI_H__
