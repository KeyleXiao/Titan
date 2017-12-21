using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace U3D_Render
{
    /// <summary>
    /// 输入处理
    /// </summary>
    class EntityInputHandler
    {
        //玩家鼠标按下
        public delegate void MouseButtonDownEntityEventHanlder(MouseButton button);
        public static event MouseButtonDownEntityEventHanlder MouseButtonDownEntityEvent;


        /// <summary>
        /// 鼠标按下处理
        /// </summary>
        /// <param name="button">0,1,2 左、中、右</param>
        public static void MouseButtonDownHandle(MouseButton button)
        {
            if (MouseButtonDownEntityEvent != null)
            {
                MouseButtonDownEntityEvent(button);
            }
        }
    }
}
