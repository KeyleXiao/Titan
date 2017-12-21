using UnityEngine;
using DG.Tweening;
using UnityEngine.UI;
using System;

namespace USpeedUI.UWidgets.UAnimator
{
    public interface ICustomViewAnim
    {
        void DesignAnim();
        void InitAnim();
        Tweener GetTweener(float _fDuration);
        int GetID();
    }

    public class UViewCustomAnimCollection : MonoBehaviour
    {
        public ICustomViewAnim[] ObjectCustomAnimList;
        protected bool _bInit;
        public bool test;
        public int id;

        protected void Init()
        {
            if (_bInit)
                return;

            _bInit = true;
            ObjectCustomAnimList = gameObject.GetComponents<ICustomViewAnim>();

        }

        public void Start()
        {
            Init();
        }

        public void Update()
        {
            if (test)
            {
                test = false;
                GetTweener(id, 1);
            }
        }

        protected int GetAnim(int _nID)
        {

            if (ObjectCustomAnimList == null)
                return -1;
            for (int i = 0; i < ObjectCustomAnimList.Length; ++i)
            {
                if (ObjectCustomAnimList[i].GetID() == _nID)
                    return i;
            }
            return -1;
        }

        public Tweener GetTweener(int _nID, float _fDuration)
        {
            if (!_bInit)
                Init();

            int index = GetAnim(_nID);
            if (index != -1)
            {
                return ObjectCustomAnimList[index].GetTweener(_fDuration);
            }
            return null;
        }

        public void InitAnim(int _nID)
        {
            if (!_bInit)
                Init();

            int index = GetAnim(_nID);
            if (index != -1)
            {
                ObjectCustomAnimList[index].InitAnim();
            }
        }

        public void DesignAnim(int _nID)
        {
            if (!_bInit)
                Init();

            int index = GetAnim(_nID);
            if (index != -1)
            {
                ObjectCustomAnimList[index].DesignAnim();
            }
        }
    }

    [RequireComponent(typeof(UViewCustomAnimCollection))]
    public class UVCA_HorizontalLayoutSpacing : MonoBehaviour, ICustomViewAnim
    {
        public HorizontalLayoutGroup HorizontalLayout;
        public DOTWrapper_Float TweenValue;
        public int ID;

        public Tweener GetTweener(float _fDruation)
        {
            if (HorizontalLayout == null)
                return null;
            return DOTween.To(() => HorizontalLayout.spacing, x => HorizontalLayout.spacing = x, TweenValue.EndValue, _fDruation);
        }

        public int GetID()
        {
            return ID;
        }

        public void InitAnim()
        {
            if (HorizontalLayout == null)
                return;
            HorizontalLayout.spacing = TweenValue.BeginValue;
        }

        public void DesignAnim()
        {
            if (HorizontalLayout == null)
                return;
            HorizontalLayout.spacing = TweenValue.EndValue;
        }
    }

    [RequireComponent(typeof(UViewCustomAnimCollection))]
    public class UVCA_GridLayoutSpacing : MonoBehaviour, ICustomViewAnim
    {
        public GridLayoutGroup GridLayout;
        public DOTWrapper_Vector2 TweenValue;
        public int ID;

        public Tweener GetTweener(float _fDruation)
        {
            if (GridLayout == null)
                return null;
            return DOTween.To(() => GridLayout.spacing, x => GridLayout.spacing = x, TweenValue.EndValue, _fDruation);
        }
        public int GetID()
        {
            return ID;
        }

        public void InitAnim()
        {
            if (GridLayout == null)
                return;
            GridLayout.spacing = TweenValue.BeginValue;
        }

        public void DesignAnim()
        {
            if (GridLayout == null)
                return;
            GridLayout.spacing = TweenValue.EndValue;
        }
    }
}
