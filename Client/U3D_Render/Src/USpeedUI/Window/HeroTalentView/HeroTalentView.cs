/*******************************************************************
** ** 文件名:	HeroTalentView.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/11/27
** 版  本:	1.0
** 描  述:	主城英雄详情-天赋列表显示
		
********************************************************************/

using UnityEngine;
using UnityEngine.UI;
using UIWidgets;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;

namespace USpeedUI
{
	public class HeroTalentNumSelectButton : MonoBehaviour
	{
		public Text NumText;
		public Image ActiveImage;
		public GameObject LevelConpomentObj;
		public Toggle ButtonToggle;

		protected bool m_bActive;

		protected int m_nLevelNum;
		protected int m_nSlotId;

		private bool m_bLock = false;
		protected HeroTalentNumSelectGroup SelectButtonGroup = null;

		public void InitTalentNumButtonInfo(int nLevel, HeroTalentNumSelectGroup buttonGroup)
		{
			NumText.text = nLevel.ToString();
			m_nLevelNum = nLevel;
			SelectButtonGroup = buttonGroup;
		}

		public virtual bool Active
		{
			set { m_bActive = value; }
			get { return m_bActive; }
		}

		// 是否锁住,(已经激活了,所以不允许操作了)
		public bool Lock
		{
			set { m_bLock = value; }
			get { return m_bLock; }
		}

		public int LevelNum
		{
			set { m_nLevelNum = value; }
			get { return m_nLevelNum; }
		}

		public int SlotId
		{
			set { m_nSlotId = value; }
			get { return m_nSlotId; }
		}

		public virtual void OnHeroTalentNumButtonClick(bool toggle)
		{
			if (SelectButtonGroup != null)
			{
				SelectButtonGroup.OnTalentNumButtonClick(m_nLevelNum, Active, m_nSlotId);
			}
		}

		public void ToggleSelectOn()
		{
			ButtonToggle.isOn = true;

		}

		public bool IsLock()
		{
			if (Lock)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		public void SetNumPageLevelContent(bool bReady)
		{
			if (LevelConpomentObj == null)
			{
				return;
			}
			LevelConpomentObj.SetActive(bReady);
		}
	}

	public class HeroTalentNumSelectGroup : MonoBehaviour
	{
		// 天赋页按钮
		public HeroTalentNumSelectButton[] NumSelectButtonArray;
		public ToggleGroup ToggleGroup;

		// 当前选择的天赋
		protected int m_nCurIndex = 0;

		protected HeroTalentViewPart m_TalentView;

		// 选择是否超出范围
		public bool IsOutofNumSelectArray(int nSlotId)
		{
			if (NumSelectButtonArray == null)
			{
				return true;
			}

			if (nSlotId < 0 || nSlotId >= NumSelectButtonArray.Length)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		public void InitTalentNumButtonInfo(ref int[] LevelArray, HeroTalentViewPart talentView)
		{
            m_TalentView = talentView;

            int nConfigLevelCount = LevelArray.Length;
            NumSelectButtonArray = new HeroTalentNumSelectButton[nConfigLevelCount];

            string sPrefabName = "UI/Prefabs/HeroTalentView/TalentNumButtonInStatic";
            ResNode NumButtonPrefab = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, sPrefabName);
            if (NumButtonPrefab == null)
            {
                Debug.Log("NumButtonPrefab Is Null");
                return;
            }

            for (int i = 0; i < nConfigLevelCount; ++i)
            {
                GameObject NumButtonObject = NumButtonPrefab.InstanceMainRes();
                if (NumButtonObject == null)
                {
                    return;
                }

                HeroTalentNumSelectButton item = NumButtonObject.GetComponent<HeroTalentNumSelectButton>();
                item.InitTalentNumButtonInfo(LevelArray[i], this);
                item.ButtonToggle.group = ToggleGroup;
                item.SlotId = i;
                NumButtonObject.transform.SetParent(this.transform, false);
                NumSelectButtonArray[i] = item;
            }
        }

		public void ResetData()
		{
			NumSelectButtonArray = null;

			// 清除子节点
			GameUtil.DestroyChild(gameObject.transform);

			m_nCurIndex = 0;
		}

		// 获取下个激活页的下标
		public int GetMinNextActivePageIndex()
		{
			if (NumSelectButtonArray == null)
			{
				return -1;
			}

			int nMinActiveIndex = -1;
			for (int i = 0; i < NumSelectButtonArray.Length; i++)
			{
				if (NumSelectButtonArray[i].Active)
				{
					nMinActiveIndex = i;
					break;
				}


			}
			if (nMinActiveIndex == -1 || m_nCurIndex == nMinActiveIndex)
			{
				return -1;
			}



			return nMinActiveIndex;
		}

		public void ChangeTogglePageIndex(int nSlotId)
		{
			if (IsOutofNumSelectArray(nSlotId))
			{
				return;
			}

			ClearAllToggle();
			SetNumPageToggle(nSlotId, true);
			m_nCurIndex = nSlotId;
		}

		private void ClearAllToggle()
		{
			for (int i = 0; i < NumSelectButtonArray.Length; i++)
			{
				SetNumPageToggle(i, false);
			}
		}

		public int GetGroupIndexLevel(int nSlotId)
		{
			if (IsOutofNumSelectArray(nSlotId))
			{
				return -1;
			}

			return NumSelectButtonArray[nSlotId].LevelNum;
		}

		public int GroupNumIndex
		{
			set { m_nCurIndex = value; }
			get { return m_nCurIndex; }
		}

		public void SetNumPageToggle(int nSlotId, bool bOn)
		{
			if (IsOutofNumSelectArray(nSlotId))
			{
				return;
			}

			NumSelectButtonArray[nSlotId].ButtonToggle.isOn = bOn;
		}

		public bool SetNumPageActive(int nSlotId, bool bActive)
		{
			if (IsOutofNumSelectArray(nSlotId))
			{
				return false;
			}

			// 是否已经激活过了
			if (NumSelectButtonArray[nSlotId].IsLock())
			{
				return false;
			}

			// 设置为可激活状态
			NumSelectButtonArray[nSlotId].Active = bActive;

			return true;
		}

		public bool GetGroupIndexActive(int nSlotId)
		{
			if (IsOutofNumSelectArray(nSlotId))
			{
				return false;
			}

			return NumSelectButtonArray[nSlotId].Active;
		}

		public void SetNumPageLock(int nSlotId, bool bLock)
		{
			if (IsOutofNumSelectArray(nSlotId))
			{
				return;
			}

			NumSelectButtonArray[nSlotId].Lock = bLock;
		}

		public int GetHeroTalentPageIndexByLevel(int nLevel)
		{
			if (NumSelectButtonArray == null)
			{
				return -1;
			}

			foreach (var item in NumSelectButtonArray)
			{
				if (item.LevelNum == nLevel)
				{
					return item.SlotId;
				}
			}

			return -1;
		}

		public void SetNumPageLevelContent(int nSlotId, bool bReady)
		{
			if (IsOutofNumSelectArray(nSlotId))
			{
				return;
			}

			NumSelectButtonArray[nSlotId].SetNumPageLevelContent(bReady);
		}

		public void OnTalentNumButtonClick(int nLevel, bool bCanActive, int nSlotID)
		{
			GroupNumIndex = nSlotID;

			if (m_TalentView != null)
				m_TalentView.OnTalentNumButtonClick(nLevel, bCanActive);
		}
	}

	public class HeroTalentViewPart : MonoBehaviour
    {
        public HeroTalentNumSelectGroup NumGroup;
        public UListHeroTalentList HeroTalentList;

        protected ObservableList<UListHeroTalentData> HeroTalentData = new ObservableList<UListHeroTalentData>();


        public virtual void InitTalentNumButtonInfo(ref int[] LevelArray)
        {
            NumGroup.InitTalentNumButtonInfo(ref LevelArray, this);
            const int nInitPageIndex = 0;
			NumGroup.SetNumPageToggle(nInitPageIndex, true);
			NumGroup.GroupNumIndex = nInitPageIndex;
        }

        public void AddHeroTalentListData(UListHeroTalentData data)
        {
            HeroTalentData.Add(data);
        }

        public virtual void SetHeroTalentListData()
        {
            HeroTalentList.DataSource = HeroTalentData;
        }

        public virtual void OnTalentNumButtonClick(int nLevel, bool bCanActive)
        {
            if (nLevel <= 0)
            {
                return;
            }

            ObservableList<UListHeroTalentData> NewTalentData = new ObservableList<UListHeroTalentData>();
            foreach (UListHeroTalentData data in HeroTalentData)
            {
                if (data.nTalentLevel == nLevel)
                {
                    NewTalentData.Add(data);
                }
            }

            int nSortIndex = 0;
            foreach (UListHeroTalentData data in NewTalentData)
            {
                data.nSortIndex = ++nSortIndex;
            }

            HeroTalentList.DataSource = NewTalentData;
        }

        public void ClearHeroTalentData()
        {
            HeroTalentData.Clear();
            NumGroup.ResetData();
            HeroTalentList.DataSource = HeroTalentData;
            SetTalentOpenButtonColorChange(false);
        }

        public virtual void SetTalentOpenButtonInWarVisible(bool bSHow)
        {
        }

        public virtual void ShowMainTalentInWar(bool bSHow, bool bSendMessage = true)
        {
        }

        public virtual void SetLevelActiveHeroTalentInWar(int nLevel)
        {
        }

        public virtual void SetTalentOpenButtonColorChange(bool bChange)
        {
        }

		public virtual void onUpdateLevelInWar(int nUpdatelv)
		{

		}

		public virtual void doActivedTalentInReconnectInWar()
		{

		}

		#region 新手引导部分
		public void AddGuideWidget(UGuideWidgetMsgData _msgData)
        {
            
            for (int i = 0; i < HeroTalentData.Count; ++i)
            {
                if (HeroTalentData[i].nTalentLevel == (int)_msgData.oUserData)
                {
                    HeroTalentData[i].bUseGuideWidget = true;
                    HeroTalentData[i].nGuideID = _msgData.nGuideID;
                    HeroTalentData[i].nGuideStepID = _msgData.nGuideStepID;
                    HeroTalentData[i].nGuideEffectID = _msgData.nGuideEffectID;
                    break;
                }
            }
        }

        public void RemoveGuideWidget(UGuideWidgetMsgData _msgData)
        {
            for (int i = 0; i < HeroTalentData.Count; ++i)
            {
                if (HeroTalentData[i].nTalentLevel == (int)_msgData.oUserData)
                {
                    HeroTalentData[i].bUseGuideWidget = false;
                    HeroTalentData[i].nGuideID = _msgData.nGuideID;
                    HeroTalentData[i].nGuideStepID = _msgData.nGuideStepID;
                    break;
                }
            }
        }
        #endregion
    }
}