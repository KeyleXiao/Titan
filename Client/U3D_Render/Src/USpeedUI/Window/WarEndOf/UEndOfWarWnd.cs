using Data.UISoundConfig;
using DG.Tweening;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UEffect;

namespace USpeedUI.WarEndOf
{
    public class UEndOfWarWnd : UIBaseWnd<UViewEndOfWar>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_ENDOFWAR;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        public override string GetPrefabPath()
        {
            return UPath.UDynamicMobaState + "/UEndOfWar/UEndOfWarView";
        }

        public override int GetLoadMask()
        {
            return (int)WndLoadMaskDef.WND_LoadMask_Moba | (int)WndLoadMaskDef.WND_LoadMask_MCAMP;
        }

        public override bool Init()
        {
            // 订阅基础消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_WAREND:
                    {
                        UCommonWarEndMsgData uiInfo = (UCommonWarEndMsgData)data;
                        if (m_wndView == null)
                            LoadView();

                        m_wndView.ShowEndOfWar(uiInfo.isSelfWin, uiInfo.DelayTime, uiInfo.isShowResultWnd);
                      
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class UViewEndOfWar : UIBaseWndView, ITimerHanlder
    {
        // 显示胜利/失败的图片
        public Image StateImage;
        public Image ContinueImage;
        public Image ContinueButton;
        public Text ContinueText;
        public Sprite FailedImage;
        public Sprite VictoryImage;

        public float duration = 1.0f;
        public float ButtonDuration = 1.0f;
        public bool snapping = true;
        public float amplitude = 10;
        public float period = -1;
        public float delay = 1.0f;

        private bool isSelfWin = false;

        private UEffectParamBase EndOfWarEffect = null;

        public override bool Init(IUIWnd wnd)
        {
            return base.Init(wnd);
        }

        public void ShowEndOfWar(bool isWin, float delayTime, bool isShow)
        {
            if(isShow)
            {
                this.isSelfWin = isWin;
                TimerManager.SetTimer(this, 0, delayTime, 1);
            }
            else
            {
                Invoke("onContinueBtnClick", 2f);
            }
        }

        public void ShowEndOfWar(bool bIsWin)
        {
            SetVisible(true);
            StateImage.sprite = USpriteManager.Instance.DefaultSprite;
            if (bIsWin)
            {
                //StateImage.sprite = VictoryImage;

                EndOfWarEffect = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_UEndOfWarView_Victory, _tfAttachParent: StateImage.transform);
                UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref EndOfWarEffect);

                UEffectPrefabParam paramPrefab = (UEffectPrefabParam)EndOfWarEffect;
               // paramPrefab.EffectGameObject.transform.localPosition = new Vector3(25, -8, 0);

				// 播放胜利音效
				playWinSound();
			}                
            else
            {
                //StateImage.sprite = FailedImage;

                EndOfWarEffect = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_UEndOfWarView_Failure, _tfAttachParent: StateImage.transform);
				UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref EndOfWarEffect);

                UEffectPrefabParam paramPrefab = (UEffectPrefabParam)EndOfWarEffect;
               // paramPrefab.EffectGameObject.transform.localPosition = new Vector3(25, -8, 0);

				// 播放失败音效
				playFailedSound();
            }
            //缩放动画
            Sequence ShowEndOfWarEequence = DOTween.Sequence();
            ShowEndOfWarEequence
            //ShowEndOfWarEequence.Append(StateImage.rectTransform.DOSizeDelta(new Vector2(1218, 651), duration, snapping)
            //                                                    .SetDelay(delay)
            //                                                    .SetEase(Ease.OutFlash, amplitude, period))
                                .Append(ContinueImage.DOFade(1.0f, ButtonDuration)
                                                     .SetDelay(delay))
                                .Insert(1, ContinueButton.DOFade(1.0f, ButtonDuration)
                                                         .SetDelay(delay))
                                .Insert(1, ContinueText.DOFade(1.0f, ButtonDuration)
                                                         .SetDelay(delay));
            ShowEndOfWarEequence.OnKill(() => ShowEndOfWarEequence = null);


            // 进入战绩界面前按键设为false
            if (EntityFactory.MainHero != null)
            {
                SkillCtrl sc = EntityFactory.MainHero.GetComponent<SkillCtrl>();
                if (sc != null)
                    sc.enable = false;
            }
            Invoke("onContinueBtnClick", 6f);
        }

        public void onContinueBtnClick()
        {    
            if (EndOfWarEffect != null)
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref EndOfWarEffect);

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_SHOW_SUPPORT_WND, null);

            m_wnd.UnloadView();
        }

		// 播放胜利音效
		private void playWinSound()
		{
			SSchemeUSoundData soundData = new SSchemeUSoundData();
			if (UISoundConfig.Instance.GetData("warzone", "warzonevictory", ref soundData))
			{
				SoundManager.CreateUISound(soundData.USoundID);
			}

		}

		// 播放失败音效
		private void playFailedSound()
		{
			SSchemeUSoundData soundData = new SSchemeUSoundData();
			if (UISoundConfig.Instance.GetData("warzone", "warzonedefeat", ref soundData))
			{
				SoundManager.CreateUISound(soundData.USoundID);
			}
		}

        public void OnTimer(int nTimerID)
        {
            if (nTimerID == 0)
                ShowEndOfWar(isSelfWin);
        }
    }
}
