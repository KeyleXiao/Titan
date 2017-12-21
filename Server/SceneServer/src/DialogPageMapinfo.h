/*******************************************************************
** 文件名:	DialogPageMapinfo.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-6-21
** 版  本:	1.0
** 描  述:	
            
********************************************************************/
#pragma once


// CDialogPageMapinfo 对话框
#include "IServerGlobal.h"

class CDialogPageMapinfo : public CDialog
{
	DECLARE_DYNAMIC(CDialogPageMapinfo)

    enum LIST_COLUMN
    {
        COLUMN_INDEX,           // 序号
        COLUMN_MAP_NAME,        // 地图名
        COLUMN_MAP_ID,          // 静态ID
        COLUMN_SCENE_ID,        // 动态ID
        COLUMN_PLAYER_NUM,      // 人物数
        COLUMN_MONSTER_NUM,     // 怪物数
        COLUMN_ENTITY_NUM,      // 实体总数
        COLUMN_MEMORY,          // 内存量
    };

public:
	CDialogPageMapinfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogPageMapinfo();

// 对话框数据
	enum { IDD = IDD_DIALOG_PAGEINFO };

public:
	/** 初始化对话框
	@param   
	@param   
	@return  
	*/
	bool			InitDialog(void);

	/** 更新场景信息
	@param   
	@param   
	@return  
	*/
	virtual void			UpdateSceneInfoToUI(void);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    CListCtrl m_listctrlMapInfo;

private:
    // 上次更新tick
    DWORD			m_dwLastUpdateMapInfo;
public:
    afx_msg void OnBnClickedButtonUpdatemapinfo();
};

extern CDialogPageMapinfo *	g_pDialogPageMapinfo;