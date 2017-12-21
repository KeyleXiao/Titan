/*******************************************************************
** 文件名:	WindowsShotcutHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	5/25/2017
** 版  本:	1.0
** 描  述:	windows快捷键控制类
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __WINDOWS_SHOTCUT_HELPER_H__
#define __WINDOWS_SHOTCUT_HELPER_H__


#include "Singleton.h"


/** 文件读取类
*/
class CWindowsShotcutHelper : public Singleton<CWindowsShotcutHelper>
{
public:
	// 构造函数
	CWindowsShotcutHelper(void)
    {
        m_StartupStickyKeys.cbSize = sizeof(STICKYKEYS);
        m_StartupStickyKeys.dwFlags = 0; //(SKF_AVAILABLE | SKF_STICKYKEYSON | SKF_AUDIBLEFEEDBACK);

        // 持续按下NUM LOCK键5秒钟以启用TOgg lekeys键 TOgglekeys引起响铃声你按下CAPS LOCK,NUM LOCK或SCROLL LOCK键时.
        m_StartupToggleKeys.cbSize = sizeof(TOGGLEKEYS);
        m_StartupToggleKeys.dwFlags = 0; // (TKF_AVAILABLE | TKF_TOGGLEKEYSON);

        // allow a user with disabilities to set the keyboard repeat rate (RepeatKeys), acceptance delay (SlowKeys), and bounce rate (BounceKeys).
        m_StartupFilterKeys.cbSize = sizeof(FILTERKEYS);
        m_StartupFilterKeys.dwFlags = 0;


        // Save the current sticky/toggle/filter key settings so they can be restored them later
        SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(STICKYKEYS), &m_StartupStickyKeys, 0);
        SystemParametersInfo(SPI_GETTOGGLEKEYS, sizeof(TOGGLEKEYS), &m_StartupToggleKeys, 0);
        SystemParametersInfo(SPI_GETFILTERKEYS, sizeof(FILTERKEYS), &m_StartupFilterKeys, 0);

        // Disable when full screen
        //AllowAccessibilityShortcutKeys( true );
    }

    // 析构函数
    ~CWindowsShotcutHelper(void)
    {
 
        // Restore back when going to windowed or shutting down
        //AllowAccessibilityShortcutKeys( false );
    }

    
     
    void AllowAccessibilityShortcutKeys( bool bAllowKeys )
    {
        if( bAllowKeys )
        {
            // Restore StickyKeys/etc to original state and enable Windows key      
            STICKYKEYS sk = m_StartupStickyKeys;
            TOGGLEKEYS tk = m_StartupToggleKeys;
            FILTERKEYS fk = m_StartupFilterKeys;
            
            SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &m_StartupStickyKeys, 0);
            SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(TOGGLEKEYS), &m_StartupToggleKeys, 0);
            SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &m_StartupFilterKeys, 0);
            
            //WarningLn(__FUNCTION__": allow key=true");
        }
        else
        {
            //WarningLn(__FUNCTION__": allow key=false");

            // Disable StickyKeys/etc shortcuts but if the accessibility feature is on, 
            // then leave the settings alone as its probably being usefully used     
            STICKYKEYS skOff = m_StartupStickyKeys;
            if( (skOff.dwFlags & SKF_STICKYKEYSON) == 0 )
            {
                // Disable the hotkey and the confirmation
                skOff.dwFlags &= ~SKF_HOTKEYACTIVE;
                skOff.dwFlags &= ~SKF_CONFIRMHOTKEY;
     
                SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &skOff, 0);
            }
     
            TOGGLEKEYS tkOff = m_StartupToggleKeys;
            if( (tkOff.dwFlags & TKF_TOGGLEKEYSON) == 0 )
            {
                // Disable the hotkey and the confirmation
                tkOff.dwFlags &= ~TKF_HOTKEYACTIVE;
                tkOff.dwFlags &= ~TKF_CONFIRMHOTKEY;
     
                SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(TOGGLEKEYS), &tkOff, 0);
            }
     
            FILTERKEYS fkOff = m_StartupFilterKeys;
            if( (fkOff.dwFlags & FKF_FILTERKEYSON) == 0 )
            {
                // Disable the hotkey and the confirmation
                fkOff.dwFlags &= ~FKF_HOTKEYACTIVE;
                fkOff.dwFlags &= ~FKF_CONFIRMHOTKEY;
     
                SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &fkOff, 0);
            }
        }
    }
private:
    STICKYKEYS  m_StartupStickyKeys;        // 粘滞键
    TOGGLEKEYS  m_StartupToggleKeys;
    FILTERKEYS  m_StartupFilterKeys;
};



#endif //__CONTROL_FILE_READER_H__