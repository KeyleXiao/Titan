using UnityEngine;
using System.Collections;

public enum KeyOrder
{
    None = KeyCode.None,
    A = KeyCode.A,
    B = KeyCode.B,
    C = KeyCode.C,
    D = KeyCode.D,
    E = KeyCode.E,
    F = KeyCode.F,
    G = KeyCode.G,
    H = KeyCode.H,
    I = KeyCode.I,
    J = KeyCode.J,
    K = KeyCode.K,
    L = KeyCode.L,
    M = KeyCode.M,
    N = KeyCode.N,
    O = KeyCode.O,
    P = KeyCode.P,
    Q = KeyCode.Q,
    R = KeyCode.R,
    S = KeyCode.S,
    T = KeyCode.T,
    U = KeyCode.U,
    V = KeyCode.V,
    W = KeyCode.W,
    X = KeyCode.X,
    Y = KeyCode.Y,
    Z = KeyCode.Z,
    ZERO =KeyCode .Alpha0,
    ONE = KeyCode.Alpha1,
    TWO = KeyCode.Alpha2,
    THREE = KeyCode.Alpha3,
    FOUR = KeyCode.Alpha4,
    FIVE = KeyCode.Alpha5,
    SIX = KeyCode.Alpha6,
    SEVEN = KeyCode.Alpha7,
    EIGHT = KeyCode.Alpha8,
    NINE = KeyCode.Alpha9,
    SPACE = KeyCode.Space,
    MOUSELEFT = KeyCode.Mouse0,
    MOUSERIGHT = KeyCode.Mouse1,
    F1=KeyCode.F1,
    F2=KeyCode.F2,
    F3=KeyCode.F3,
    F4=KeyCode.F4,
    F5=KeyCode.F5,
    F6=KeyCode.F6,
    F7=KeyCode.F7,
    F8 = KeyCode.F8,
    F9 = KeyCode.F9,
    F10 = KeyCode.F10,
    F11 = KeyCode.F11,
    F12 = KeyCode.F12,
    ALT = KeyCode.LeftAlt,
    //RIGHTALT = KeyCode.RightAlt,
    ESC = KeyCode.Escape,
    SHIFT = KeyCode.LeftShift,
    CTRL = KeyCode.LeftControl,    
    TAB = KeyCode.Tab,
    ENTER=KeyCode.KeypadEnter,
    RETURN =KeyCode.Return,
	BACKQUOTE = KeyCode.BackQuote,
}

// 技能出招表
[System.Serializable]
public class OrderData
{
    // 按键序列
    public KeyOrder[] keySequence = new KeyOrder[0];

    // 对应的技能槽
    public int nSlotID;

    public int nAdvanceUseType; //推荐的施法方式

    // 是否空中限制 0表示都可以， 1表示只能在空中，2表示只能在地面
    public int nAirCons=0;

    [HideInInspector]
    // 是否有效（例如已学习或激活）
    public bool bEnabled;

    // 测试按键是否匹配
    // 返回1表示完全匹配
    // 返回0表示目前仍然匹配，还可以继续按键
    // 返回-1表示完全不匹配
    public int Test(KeyOrder[] orderset,int len)
    {
        if (len < 1)
        {
            return -1;
        }

        int i = 0;
        while (i < keySequence.Length)
        {
            if (i >= len)
            {
                return 0;
            }

            if (keySequence[keySequence.Length - 1 - i] != orderset[len - 1 - i])
            {
                return -1;
            }
            i++;
        }

        return 1;
    }
}


// 技能出招表，目前废弃
[System.Serializable]
public class OrderDataSet : ScriptableObject
{
    public KeyOrder breakSkillOrder = KeyOrder.ESC;
    public OrderData[] orderset;
}