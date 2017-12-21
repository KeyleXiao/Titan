using System;
using UnityEngine;

public class GameLogicConfigApi
{
    static GameLogicConfigApi()
    {

    }

    private static bool _bIsLockInWindow = false;
    public static bool bIsLockInWindow
    {
        get
        {
            return _bIsLockInWindow;
        }
        set
        {
            if (!Application.isEditor)
            {
                _bIsLockInWindow = value;
                LockMouseInWindow(_bIsLockInWindow);
            }
        }
    }

    /// <summary>
    /// 鼠标是否限制在游戏窗口内
    /// </summary>
    /// <param name="_bLock">true:限制窗口内，false:不限制</param>
    public static void LockMouseInWindow(bool _bLock)
    {
        RECTAPI CTSRect = new RECTAPI();
        if (_bLock)
        {
            CTSRect = calClientToScreen();
            GameLogicAPI.ClipCursor(ref CTSRect);

        }
        else
        {
            GameLogicAPI.ClipCursor(IntPtr.Zero);
        }
    }

    public static RECTAPI calClientToScreen()
    {

        RECTAPI ClientRectHwnd = new RECTAPI();

        //下面代码限制鼠标在窗口
        //RECTAPI rectHwnd = new RECTAPI();
        //bool resultWindowRect = GameLogicAPI.GetWindowRect(m_WindowHwnd, ref rectHwnd);

        //下面代码限制鼠标在窗口客户区
        bool resultGetClientRect = GameLogicAPI.GetClientRect(Initialize.m_WindowHwnd, out ClientRectHwnd);
        //编辑器下要减去顶部工具栏高度
#if !UNITY_STANDALONE_WIN
        ClientRectHwnd.top += 24;
#endif
        POINTAPI pointCTS = new POINTAPI();
        pointCTS.x = ClientRectHwnd.left; pointCTS.y = ClientRectHwnd.top;
        bool resultCTS = GameLogicAPI.ClientToScreen(Initialize.m_WindowHwnd, ref pointCTS);
        ClientRectHwnd.left = pointCTS.x; ClientRectHwnd.top = pointCTS.y;

        pointCTS.x = ClientRectHwnd.right - 1; pointCTS.y = ClientRectHwnd.bottom - 1;
        resultCTS = GameLogicAPI.ClientToScreen(Initialize.m_WindowHwnd, ref pointCTS);
        ClientRectHwnd.right = pointCTS.x; ClientRectHwnd.bottom = pointCTS.y;

        return ClientRectHwnd;
    }

    /// <summary>
    /// 语言目录
    /// </summary>
    public static string LanguageDirName
    {
        get
        {
            if (m_languageDirName.Length <= 0)
            {
                IntPtr ptr = GameLogicAPI.getLanguageDirName();
                if (ptr != IntPtr.Zero)
                    m_languageDirName = IntPtrHelper.Ptr2Str(ptr);
            }
            return m_languageDirName;
        }
    }
    private static string m_languageDirName = "";

    public static int MonsterDistance
    {
        get
        {
            if (m_monsterDistance == 0)
            {
                m_monsterDistance = GameLogicAPI.getMonsterDistance();
            }
            return m_monsterDistance;
        }
    }
    private static int m_monsterDistance = 0;

    public static int TowerDistance
    {
        get
        {
            if (m_towerDistance == 0)
            {
                m_towerDistance = GameLogicAPI.getTowerDistance();
            }
            return m_towerDistance;
        }
    }
    private static int m_towerDistance = 0;

    public static int MonsterDeadEffect
    {
        get
        {
            if (m_monsterEffect == 0)
            {
                m_monsterEffect = GameLogicAPI.getMonsterDeadEffect();
            }
            return m_monsterEffect;
        }
    }
    private static int m_monsterEffect = 0;

    public static int MoneyEffect
    {
        get
        {
            if (m_moneyEffect == 0)
            {
                m_moneyEffect = GameLogicAPI.getMoneyEffect();
            }
            return m_moneyEffect;
        }
    }
    private static int m_moneyEffect = 0;


    public static int PlayerLv
    {
        get
        {
            if (m_playerLv == 0)
            {
                m_playerLv = GameLogicAPI.getPlayerLv();
            }
            return m_playerLv;
        }
    }
    private static int m_playerLv = 0;

    
}
