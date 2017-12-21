using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI;
using U3D_Render.Common;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListComponentRankRecord : ListViewItem
    {
        public GameObject ItemView;
        public Text Season;
        public Text Rank;
        public Text WinCount;
        public Text WinPro;
        public Image[] MostUseHero;
        public UTooltipTrigger[] MostUseHeroTrigger;

        public Image BgIcon;
        public Image SelectIcon;
    
        public virtual void SetData(UListItemRankRecord item)
        {
            if (item.node.wSeasonID <= 0)
            {
                BgIcon.gameObject.SetActive(true);
                ItemView.SetActive(false);
                BgIcon.raycastTarget = false;
                return;
            }
            BgIcon.gameObject.SetActive(true);
            BgIcon.raycastTarget = true;
            ItemView.SetActive(true);

            DateTime beginTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(item.node.nBeginTime);
            beginTime = beginTime.ToLocalTime();
            DateTime endTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(item.node.nEndTime);
            endTime = endTime.ToLocalTime();
            Season.text = string.Format("({0}){1}年{2}月--{3}年{4}月", item.node.szSeasonTitle, beginTime.Year, beginTime.Month, endTime.Year, endTime.Month);

            int nMatchType = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MATCHTYPE);
            IntPtr ptr = GameLogicAPI.getActorRankNameByGrade(nMatchType, item.node.wRankGrade);
            string strRankName = "";
            if (ptr != IntPtr.Zero)
                strRankName = IntPtrHelper.Ptr2Str(ptr);
            Rank.text = strRankName;

            WinCount.text = item.node.wWinCount.ToString();

            float fWinPro = item.node.wMatchCount > 0 ? (float)item.node.wWinCount / item.node.wMatchCount : 0;
            WinPro.text = string.Format("{0:F0}%", fWinPro * 100);

            int nIndex = 0;
            for (int i = 0; i < item.node.wHeroRecord.Length; i = i + 3)
            {
                int nHeroID = item.node.wHeroRecord[i];
                int nMatchCount = item.node.wHeroRecord[i + 1];
                int nWinCount = item.node.wHeroRecord[i + 2];
                float fHeroWinPro = nMatchCount > 0 ? (float)nWinCount / nMatchCount : 0;

                MostUseHero[nIndex].sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_RANK_RECORD, 1, 4, nHeroID);

                string strTip = String.Format("胜率：{0}%<br>总场次：{1}", fHeroWinPro * 100, nMatchCount);
                UBB.toHtml(ref strTip);
                MostUseHeroTrigger[nIndex].SetText(UTooltipParamName.BodyText, strTip);

                nIndex++;
            }
        }

        public virtual void SelectColoring()
        {
            SelectIcon.gameObject.SetActive(true);
        }

        public virtual void DefaultColoring()
        {
            SelectIcon.gameObject.SetActive(false);
        }
    }
}
