/*******************************************************************
** �ļ���:	DialogPageSetting.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	��绪
** ��  ��:	2016-6-21
** ��  ��:	1.0
** ��  ��:	
            
********************************************************************/
#pragma once


// CDialogPageSetting �Ի���

class CDialogPageSetting : public CDialog
{
	DECLARE_DYNAMIC(CDialogPageSetting)

public:
	CDialogPageSetting(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogPageSetting();

// �Ի�������
	enum { IDD = IDD_DIALOG_PAGESETTING };

public:
	/** ��ʼ���Ի���
	@return  
	*/
	bool			InitDialog(void);

	/** ˢ�¶Ի���
	@param   
	@return  
	*/
	bool			RefreshDialog(int nZoneSvrID);

	/** ִ��ˢ��������Ϣ
	@param   
	@return  
	*/
	void			OnRefresh();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    int m_nSceneSvrID;
    CString m_stringAreaName;
    CString m_stringWorldID;
};

extern CDialogPageSetting * g_pDialogPageSetting;