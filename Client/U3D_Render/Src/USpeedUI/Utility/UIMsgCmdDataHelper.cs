/*******************************************************************
** 文件名:	UIMsgCmdDataHelper.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016/8/19
** 版  本:	1.0
** 描  述:	

		
********************************************************************/
using System;

namespace USpeedUI
{
    /// <summary>
    /// UIMsgCmdData构造进行包装类
    /// </summary>
    public class UIMsgCmdDataHelper : Singleton<UIMsgCmdDataHelper>
    {
        public UIMsgCmdData create<T>(WndMsgID msg_id, ref T data, int nParam = 0, string strParam = "") where T : struct
        {
            IntPtrHelper helper = new IntPtrHelper();
            int data_len = System.Runtime.InteropServices.Marshal.SizeOf(data);

            IntPtr ptr = helper.toPtr<T>(ref data);
            return create(msg_id, nParam, strParam, ptr, data_len);
        }

        public UIMsgCmdData create(WndMsgID msg_id, int nParam = 0, string strParam = "")
        {
            return create(msg_id, nParam, strParam, IntPtr.Zero, 0);
        }

        public UIMsgCmdData create(WndMsgID msg_id, int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
        {
            return new UIMsgCmdData((int)msg_id, nParam, strParam, ptrParam, nPtrLen);
        }
    }
}
