using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI.Blood
{
    //阵营类型
    public enum GFxCampTpye : int
    {
        CT_Unknow = 0,
        CT_Self = 1, //本地玩家自己(颜值：green)
        CT_Friend = 2, //己方(颜值：blue)
        CT_Enemy = 3, //敌方（颜值：red）
        CT_Neutral = 4, //中立（颜值：yellow）
    };

    //血条类型
    public enum BloodStyleType : int
    {
        Player,    //人物
        Building,  //建筑
        Monster,   //野怪
        Boss,      //大血条
        MAX,
    }

    //血条样式信息
    [Serializable]
    public class BloodStyleInfo
    {
        public BloodStyleType bloodType; //类型
        public ResNode bloodRes;         //资源节点
        public Vector2 bloodSize;        //尺寸
        //缩放
        public Vector2 bloodScale = Vector2.one; 
        //颜色表，按自身、1、2、3。。。n阵营排
        public Color[] bloodColorArry = new Color[] { new Color(31.0f/255,194.0f/255,22.0f/255), new Color(41.0f / 255, 191.0f / 255, 236.0f / 255), new Color(182.0f / 255, 0, 0) };
        public bool bShowRoleName;       //是否显示角色名
        public bool bShowLevel;          //是否显示等级
        public bool bShowBloodNum;       //是否显示血量数值
        public bool bShowMp;             //是否显示魔法值
        public bool bShowShield;         //是否显示护盾
        public bool bCanChangeColor;     //是否可以变色
        public Color ChangedColor;       //变色值
    }

    //血条画布节点
    public struct BloodCanvasNode
    {
        public bool isValid;
        public Vector2 start;
        public Vector2 end;
        public Vector2 vSize;
        public Vector2 scale;
        public int RowIndex;
        public int ColIndex;
    }

    public partial class UBloodManager : MonoBehaviour
    {
        public RenderTexture BloodRenderTexture
        {
            get
            {
                if (m_RenderTexture == null)
                {
                    m_RenderTexture = new RenderTexture(bloodRenderTextureWidth, bloodRenderTextureHeight, 16);
                    m_RenderTexture.filterMode = FilterMode.Point;
                    m_RenderTexture.Create();
                }
                return m_RenderTexture;
            }
        }

        public bool NumInfoVisible
        {
            get { return numInfoVisible; }
        }

        public int PkTargetID
        {
            set { pkTargetID = value; }
            get { return pkTargetID; }
        }

        public Camera renderCam;
        public RectTransform renderCanvas;

        //血条最大行数表，按类型排
        public int[] MaxCellRowCountList = new int[] { 3, 4, 30 };

        [SerializeField]
        public List<BloodStyleInfo> m_BloodStyleList;  //血条样式表

        public float HideOutOfDistance = 50.0f;
        public float extendHideDistanceInGodView = 20.0f;  //相机大视角模式，优化距离扩展值

        private Transform bossBloodParent;      //大血条父节点
        private RenderTexture m_RenderTexture = null;

        private const int bloodRenderTextureWidth = 1920;    //血条RT宽
        private const int bloodRenderTextureHeight = 1080;   //血条RT高

        private UBlood BossBlood = null;
        private Transform bossBloodTarget;          //大血条目标

        private float JitterDuration = 0.5f;        //抖动间隔时间
        private float JitterTimer = 0.0f;           //抖动定时器
        private Vector2 BossBloodJitterX = new Vector2(-10, 10);    //大血条抖动范围—X
        private Vector2 BossBloodJitterY = new Vector2(-10, 10);    //大血条抖动范围—Y
        private Vector2 OrginalBossBloodPos = new Vector2(0, -120); //大血条原始位置

        public int TowerStrikingDistance = 15;        //塔的打击范围
        public float TowerSoldierCheckTime = 0.5f;    //进塔兵检测时间间隔

        //PK目标
        private int pkTargetID = -1;
        //是否显示血量数值信息
        private bool numInfoVisible = false;
        //是否显示主角血量
        private bool actorBloodVisible = true;
        private bool m_bFirstVisibleMask = false;

        static public UBloodManager Instance = null;
    }
}
