using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine.UI;
using USpeedUI;
using USpeedUI.UWidgets;
using UnityEngine;
using Kin_ManagerDef;
using U3D_Render.Common;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListComponentKin : ListViewItem
    {
        public Text Name;
        public Text Count;
        public Text MasterName;
        public Text KinDes;
        public Text ApplyDes;
        public Image ComponentBG;
        public Sprite SelectBGSprite;
        protected Sprite DefalutBGSprite;

        public virtual void SetData(UListItemKin item)
        {
            if (DefalutBGSprite == null)
                DefalutBGSprite = ComponentBG.sprite;
            Name.text = item.Name;
            Count.text = item.Count;
            MasterName.text = item.MasterName;
            KinDes.text = item.KinDes;
            ApplyDes.text = item.ApplyDes;
        }

        public virtual void SelectColoring()
        {
            // 选中
            if (ComponentBG != null)
                ComponentBG.sprite = SelectBGSprite;

            ComponentBG.type = Image.Type.Sliced;
        }

        public virtual void DefaultColoring()
        {
            // 选中
            if (ComponentBG != null)
                ComponentBG.sprite = DefalutBGSprite;

            ComponentBG.type = Image.Type.Filled;
        }
    }


    public class UListComponentKinMember : ListViewItem
    {
        public Image SexIcon;
        public Text Identity;
        public Text GradIcon;
        public Text PersonName;
        public Text Power;
        public Text Order;
        public Image PowerRankImg;
        public Text State;
        public Text Level;
        public Image ComponentBG;
        public Sprite SelectBGSprite;
        protected Sprite DefalutBGSprite;
        public Image DefaultBG;

        public Image ChangeImage;
        public Text ChangeText;
        public UPopupMenuHandle PopupMenuHandle;
        
       
        public virtual void SelectColoring()
        {
            // 选中
            if (ComponentBG != null)
                ComponentBG.sprite = SelectBGSprite;

            ComponentBG.type = Image.Type.Sliced;
        }

        public virtual void DefaultColoring()
        {
            // 选中
            //SelectBG.gameObject.SetActive(false);
            if (ComponentBG != null)
                ComponentBG.sprite = DefalutBGSprite;

            ComponentBG.type = Image.Type.Filled;
        }
        public virtual void SetData(UListItemKinMember item)
        {
            if (DefalutBGSprite == null)
                DefalutBGSprite = ComponentBG.sprite;

            if (item.nSex < (int)PERSON_SEX.SEX_MAX && item.nSex >= (int)PERSON_SEX.SEX_MALE)
            {
                SexIcon.enabled = true;
                SexIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinSex, WndID.WND_ID_SOCIALCONTACT, item.nSex + 1);
            }
            else
            {
                SexIcon.enabled = false;
            }

            switch ((TKinIdentity)item.IdentityID)
            {
                case TKinIdentity.emKinIdentity_Member:
                    {
                        Identity.text = ULocalizationService.Instance.Get("UIView", "KinMember", "MemberDes");
                    }
                    break;
                case TKinIdentity.emKinIdentity_Elder:
                    {
                        Identity.text = ULocalizationService.Instance.Get("UIView", "KinMember", "ElderDes");
                    }
                    break;
                case TKinIdentity.emKinIdentity_Shaikh:
                    {
                        Identity.text = ULocalizationService.Instance.Get("UIView", "KinMember", "ShaikhDes");
                    }
                    break;
                default:
                    break;
                
            }
            Level.text = "";
            Level.text = item.Level;

            PersonName.text = "";
            PersonName.text = item.PersonName;

            Power.text = "";
            Power.text += item.Power;

            int nChangeOrder = item.nLastOrder - item.nBVOrder;
            if (item.nLastOrder == 0)
            {
                nChangeOrder = 0;
            }
            ChangeText.text = nChangeOrder.ToString();
            if (nChangeOrder >= 0)
            {
                ChangeImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ClanRankChange, WndID.WND_ID_SOCIALCONTACT, 1);
                Color clr;
                ColorUtility.TryParseHtmlString("#fb8507", out clr);
                ChangeText.color = clr;
            }
            else
            {
                ChangeImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ClanRankChange, WndID.WND_ID_SOCIALCONTACT, 2);
                Color clr;
                ColorUtility.TryParseHtmlString("#71bf81", out clr);
                ChangeText.color = clr;
            }

            if (Convert.ToInt32(item.PowerRank) <= (int)TKinOrderMAX.emKinOrderMAX)
            {
                PowerRankImg.enabled = true;
                PowerRankImg.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinBattleOrder, WndID.WND_ID_SOCIALCONTACT, Convert.ToInt32(item.PowerRank));
            }
            else
            {
                PowerRankImg.enabled = false;
            }
            Order.text = item.PowerRank;
            GradIcon.text = item.GradIcon;



            int nGameState = item.isOnline == 0 ? (int)ACTOR_GAME_STATE.GAME_STATE_OFFLINE : item.GameState;
            string strStateColor = UDefines.GetGameStateColor(nGameState);
            string strStateDesc = UDefines.GetGameStateDesc(nGameState);
            State.text = String.Format("<color=#{0}>{1}</color>", strStateColor, ULocalizationService.Instance.Get("UIView", "Common", strStateDesc));
            
            if (item.isOnline == 0)
            {
                // 设置离线颜色
                Color clr;
                ColorUtility.TryParseHtmlString("#adadad", out clr);
                Level.color = clr;
                PersonName.color = clr;
                Power.color = clr;
                Order.color = clr;
                GradIcon.color = clr;
                State.color = clr;
                Identity.color = clr;
            }
            else
            {
                // 设置在线颜色
                Color clr;
                ColorUtility.TryParseHtmlString("#ffffff", out clr);
                Level.color = clr;
                PersonName.color = clr;
                Power.color = clr;
                Order.color = clr;
                GradIcon.color = clr;
                Identity.color = clr;
                ColorUtility.TryParseHtmlString("#18ff00", out clr);
                State.color = clr;
            }

            if(PopupMenuHandle == null)
                PopupMenuHandle = this.gameObject.AddComponent<UPopupMenuHandle>();
            PopupMenuHandle.HandlePopupAction = item.PopupAction;
        }
    }

    public class UListComponentKinAcceptMember : ListViewItem
    {
        public Text PersonName;
        public Text Power;
        public Text Level;

        public Image ComponentBG;
        public Sprite SelectBGSprite;
        protected Sprite DefalutBGSprite;

        public virtual void SetData(UListItemKinAcceptMember item)
        {
            if (DefalutBGSprite == null)
                DefalutBGSprite = ComponentBG.sprite;

            PersonName.text = item.PersonName;
            Level.text      = item.Level;
            Power.text      = item.Power;
         
        }

        public virtual void SelectColoring()
        {
            // 选中
            if (ComponentBG != null)
                ComponentBG.sprite = SelectBGSprite;

            ComponentBG.type = Image.Type.Sliced;
        }

        public virtual void DefaultColoring()
        {
            // 选中
            if (ComponentBG != null)
                ComponentBG.sprite = DefalutBGSprite;

            ComponentBG.type = Image.Type.Filled;
        }
    }


    public class UListComponentKinEvent : ListViewItem
    {
        
        public Text KinEventDes;
        public Image ComponentBG;
        public Sprite SelectBGSprite;
        protected Sprite DefalutBGSprite;
        public Image DefaultBG;

        public virtual void SetData(UListItemKinEvent item)
        {
            if (DefalutBGSprite == null)
                DefalutBGSprite = ComponentBG.sprite;

            string sEventDesc = item.KinEventDes;
            UBB.toHtml(ref sEventDesc, UBB_FORMAT_TYPE.UGUI);
            KinEventDes.text = sEventDesc;
        }

        public virtual void SelectColoring()
        {
            // 选中
            if (ComponentBG != null)
                ComponentBG.sprite = SelectBGSprite;

        }

        public virtual void DefaultColoring()
        {
            // 选中
            //SelectBG.gameObject.SetActive(false);
            if (ComponentBG != null)
                ComponentBG.sprite = DefalutBGSprite;

        }
    }

    public class UListComponentKinGloryCup : ListViewItem
    {
        public Image CupIcon;
        public Text CupName;
        public Text CupEndTime;
        public Text CupConfigDesc;

        public virtual void SetData(UListItemKinGloryCup item)
        {
            Sprite sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinGloryCup, WndID.WND_ID_SOCIALCONTACT, item.nCupRank);
            if (sprite != null)
            {
                CupIcon.sprite = sprite;
            }

            string strRankName = "";
            if (item.nCupRank == 1)
                strRankName = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "Pos1");
            else if (item.nCupRank == 2)
                strRankName = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "Pos2");
            else if (item.nCupRank == 3)
                strRankName = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "Pos3");
            else if (item.nCupRank <= 8)
                strRankName = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "RoundIndex_8");
            CupName.text = String.Format("{0}  {1}", item.MatchName, strRankName);

            DateTime endTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(item.nEndTime);
            endTime = endTime.ToLocalTime();
            CupEndTime.text = String.Format("{0}.{1}.{2}  {3}", endTime.Year, endTime.Month, endTime.Day, item.ConfigName);

            CupConfigDesc.gameObject.SetActive(false);
        }
    }
    
}
