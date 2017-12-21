/*******************************************************************
** 文件名:	DialogPageSetting.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-6-21
** 版  本:	1.0
** 描  述:	
            
********************************************************************/
#pragma once


// CDialogPageSetting 对话框

class CDialogPageSetting : public CDialog
{
	DECLARE_DYNAMIC(CDialogPageSetting)

public:
	CDialogPageSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogPageSetting();

// 对话框数据
	enum { IDD = IDD_DIALOG_PAGESETTING };

public:
	/** 初始化对话框
	@return  
	*/
	bool			InitDialog(void);

	/** 刷新对话框
	@param   
	@return  
	*/
	bool			RefreshDialog(int nZoneSvrID);

	/** 执行刷新配置信息
	@param   
	@return  
	*/
	void			OnRefresh();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    int m_nSceneSvrID;
    CString m_stringAreaName;
    CString m_stringWorldID;
};

extern CDialogPageSetting * g_pDialogPageSetting;