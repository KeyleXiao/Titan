/*******************************************************************
** �ļ���:	DialogPageSetting.cpp 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	��绪
** ��  ��:	2016-6-21
** ��  ��:	1.0
** ��  ��:	
            
********************************************************************/

#include "stdafx.h"
#include "SceneServer.h"
#include "DialogPageSetting.h"
#include "afxdialogex.h"
#include "SceneServerControl.h"

#include "IServerGlobal.h"
#include "IManagerConnectorService.h"
#include "DManagerModule.h"



CDialogPageSetting * g_pDialogPageSetting = NULL;

IMPLEMENT_DYNAMIC(CDialogPageSetting, CDialog)

CDialogPageSetting::CDialogPageSetting(CWnd* pParent /*=NULL*/)
    : CDialog(CDialogPageSetting::IDD, pParent)
    , m_nSceneSvrID(0)
    , m_stringAreaName(_T(""))
    , m_stringWorldID(_T(""))
{
    g_pDialogPageSetting = this;
}

CDialogPageSetting::~CDialogPageSetting()
{

}



void CDialogPageSetting::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_SCENE_SVRID, m_nSceneSvrID);
    DDX_Text(pDX, IDC_EDIT_WORLD_NAME, m_stringAreaName);
    DDX_Text(pDX, IDC_EDIT_SCENE_WORLD_ID, m_stringWorldID);
}

BEGIN_MESSAGE_MAP(CDialogPageSetting, CDialog)

END_MESSAGE_MAP()


/** ��ʼ���Ի���
@param   
@param   
@return  
*/
bool CDialogPageSetting::InitDialog(void)
{
    CWnd * pTeampWnd = NULL;
    //ENABLE_WINDOW(IDC_CHECK_LASTSUCCSTOP, false);

    return true;
}


/** ˢ�¶Ի���
@param   
@param   
@return  
*/
bool CDialogPageSetting::RefreshDialog(int nZoneSvrID)
{
    // ������ID
    m_nSceneSvrID = nZoneSvrID;

    g_SceneServerControl.SetUIControl(UICONTROL_REFRESH_SETTING);

    return true;
}


/** ִ��ˢ��������Ϣ
@param   
@return  
*/
void CDialogPageSetting::OnRefresh()
{

    IManagerConnectorService * pManagerConnectorService = gServerGlobal->getManagerConnectorService();
    if(pManagerConnectorService)
    {
        SGameWorldConnectorInfo info = pManagerConnectorService->GetGameWorldInfo();

        // ��Ϸ����ID
        m_stringWorldID.Format(_T("%d"), info.dwWorldID);

        
        // ��Ϸ������
        m_stringAreaName = CString(utf82w(info.szWorldName));

        if (info.bIsPublic)
        {
            CButton *pWind = (CButton*)GetDlgItem(IDC_CHK_ISPUBLIC);
            if(pWind)
            {
                pWind->SetCheck(1);
            }
        }
    }
    
    //// ��ʱ���̼��
    //m_nTickSaveInterval = pInfo->nTickSaveInterval;

    //// �رշ�����ʱ��
    //m_nStopSvrNoteLong = pInfo->nCloseServiceInterval;

    //// �Զ���ͣ������
    //m_stringStopCustom = pInfo->szCustomStop;

    UpdateData(FALSE);
}