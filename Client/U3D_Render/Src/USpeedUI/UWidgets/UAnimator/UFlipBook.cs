using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using Data.AnimSpriteConfig;

namespace USpeedUI.UWidgets.UAnimator
{
    [RequireComponent(typeof(RawImage))]
    public class UFlipBook : MonoBehaviour
    {
        public float frameFps = 4f; 
        public Texture rawTexture
        {
            get 
            {
                if (!m_rawTexture)
                    m_rawTexture = USpriteManager.Instance.DefaultSprite.texture;
                return m_rawTexture;
            }
        }
        private Texture m_rawTexture;
        public RawImage rawImage
        {
            get
            {
                if(!m_rawImage)
                    m_rawImage = this.gameObject.GetComponent<RawImage>();
                return m_rawImage;
            }
        }
        private RawImage m_rawImage;

        private string m_AnimKey;
        private SSchemeAnimFrameData frameData;
        private int animIndex;
        private bool isInit;

        public void Awake()
        {
            animIndex = 0;
        }

        public void Init(string animKey)
        {
            m_AnimKey = animKey;

            frameData = AnimSpriteConfig.Instance.GetData(m_AnimKey);

            if (frameData == null)
                return;

            m_rawTexture = USpriteManager.Instance.GetAnimSprite(m_AnimKey).texture;

            rawImage.texture = rawTexture;

            Rect spriteRect = frameData.GetFrameRect(animIndex);

            spriteRect.x = frameData.vec2AnimStartPos.x / rawTexture.width;
            spriteRect.y = 1 - (frameData.vec2AnimStartPos.y + spriteRect.height)/rawTexture.height;
            spriteRect.width = spriteRect.width / rawTexture.width;
            spriteRect.height = spriteRect.height / rawTexture.height;

            rawImage.uvRect = spriteRect;

            isInit = true;
        }

        float time = 0f;
        public void Update()
        {
            if (!isInit || null == frameData)
                return;

            if (frameData.nAnimFrame < 2)
                return;

            time += Time.deltaTime;
            if (time > 1 / frameFps)
            {
                time = 0;
                int curIndex = animIndex;
                curIndex++;
                curIndex = curIndex % frameData.nAnimFrame;
                animIndex = curIndex;

                Rect spriteRect = frameData.GetFrameRect(animIndex);

                spriteRect.x = spriteRect.x / rawTexture.width;
                spriteRect.y = 1 - (spriteRect.y + spriteRect.height) / rawTexture.height;
                spriteRect.width = spriteRect.width / rawTexture.width;
                spriteRect.height = spriteRect.height / rawTexture.height;

                rawImage.uvRect = spriteRect;
            }
        }
    }
}
