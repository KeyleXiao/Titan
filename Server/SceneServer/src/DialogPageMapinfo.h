/*******************************************************************
** �ļ���:	DialogPageMapinfo.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	��绪
** ��  ��:	2016-6-21
** ��  ��:	1.0
** ��  ��:	
            
********************************************************************/
#pragma once


// CDialogPageMapinfo �Ի���
#include "IServerGlobal.h"

class CDialogPageMapinfo : public CDialog
{
	DECLARE_DYNAMIC(CDialogPageMapinfo)

    enum LIST_COLUMN
    {
        COLUMN_INDEX,           // ���
        COLUMN_MAP_NAME,        // ��ͼ��
        COLUMN_MAP_ID,          // ��̬ID
        COLUMN_SCENE_ID,        // ��̬ID
        COLUMN_PLAYER_NUM,      // ������
        COLUMN_MONSTER_NUM,     // ������
        COLUMN_ENTITY_NUM,      // ʵ������
        COLUMN_MEMORY,          // �ڴ���
    };

public:
	CDialogPageMapinfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogPageMapinfo();

// �Ի�������
	enum { IDD = IDD_DIALOG_PAGEINFO };

public:
	/** ��ʼ���Ի���
	@param   
	@param   
	@return  
	*/
	bool			InitDialog(void);

	/** ���³�����Ϣ
	@param   
	@param   
	@return  
	*/
	virtual void			UpdateSceneInfoToUI(void);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    CListCtrl m_listctrlMapInfo;

private:
    // �ϴθ���tick
    DWORD			m_dwLastUpdateMapInfo;
public:
    afx_msg void OnBnClickedButtonUpdatemapinfo();
};

extern CDialogPageMapinfo *	g_pDialogPageMapinfo;