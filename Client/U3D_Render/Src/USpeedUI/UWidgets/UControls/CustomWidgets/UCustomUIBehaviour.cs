using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.EventSystems;

namespace USpeedUI.UWidgets
{
    public class UCustomUIBehaviour : UIBehaviour
    {
        protected Transform m_Transform;
        public Transform GetTransform
        {
            get
            {
                if(m_Transform == null)
                {
                    m_Transform = GetComponent<Transform>();
                }
                return m_Transform;
            }
        }

        protected RectTransform m_RectTransform;
        public RectTransform GetRectTransform
        {
            get
            {
                if(m_RectTransform == null)
                {
                    m_RectTransform = GetComponent<RectTransform>();
                }
                return m_RectTransform;
            }
        }

        public void GetComponentInChildren<T>(string _strChildGameObjectName ,out T _com)
        {
            Transform tf = GetRectTransform.FindChild(_strChildGameObjectName);
            if(tf != null)
            {
                _com = tf.GetComponent<T>();
            }
            else
            {
                _com = default(T);
            }

        }
    }
}
