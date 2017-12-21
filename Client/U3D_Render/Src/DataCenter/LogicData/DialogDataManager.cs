using GameLogic;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using UIWidgets;
using USpeedUI;

namespace DataCenter
{
    public class DialogDataManager : Singleton<DialogDataManager>, IModelData
    {
        /// <summary>
        /// 有需要的话将这部分改用反射处理
        /// </summary>
        protected Dictionary<string, Func<bool>> m_dicFuncCallback = new Dictionary<string, Func<bool>>();
        public void Init()
        {
            m_dicFuncCallback.Add("forceLogin", forceLogin);
        }
        public void Clear()
        {
            m_dicFuncCallback.Clear();
        }

        public DialogActions GetNewAction(EMessageBoxType _eType, Func<bool> _okCallback, Func<bool> _cancelCallback)
        {
            DialogActions result = new DialogActions();

            if ((_eType & EMessageBoxType.EMBT_OK) > 0)
                result.Add(ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), _okCallback);
            else if ((_eType & EMessageBoxType.EMBT_Cancel) > 0)
                result.Add(ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), _cancelCallback);
            else if ((_eType & EMessageBoxType.EMBT_OKCancel) > 0)
            {
                result.Add(ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), _okCallback);
                result.Add(ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), _cancelCallback);
            }

            return result;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="_eType">按钮类型</param>
        /// <param name="_strOkCallback">确定按钮点击回调函数名</param>
        /// <param name="_strCancelCallback">取消按钮点击回调函数名</param>
        /// <returns></returns>
        public DialogActions GetNewAction(EMessageBoxType _eType, string _strOkCallback = null, string _strCancelCallback = null)
        {
            return GetNewAction(_eType, GetCallback(_strOkCallback), GetCallback(_strCancelCallback));
        }

        //有需要的话将这里改用反射处理
        private Func<bool> GetCallback(string _strFuncName)
        {
            Func<bool> result = null;

            if(string.IsNullOrEmpty(_strFuncName))
            {
                return result;
            }

            if(!m_dicFuncCallback.TryGetValue(_strFuncName,out result))
            {
                Trace.Log("没有注册回调方法名：" + _strFuncName+"如不需要可忽略次打印。");
                result = null;
            }

            return result;
        }
        //直接用反射部分未完成支持
        public MethodInfo GetMethod(string name)
       {
            BindingFlags flag = BindingFlags.Instance | BindingFlags.NonPublic;
            Type type = instance.GetType();
            MethodInfo method = type.GetMethod(name, flag);
            return method;
        }
		//强制登录
        protected bool forceLogin()
        {
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_FORCE_LOGIN);
            return true;
        }
    }
}
