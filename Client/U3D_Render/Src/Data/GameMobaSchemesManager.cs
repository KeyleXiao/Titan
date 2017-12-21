using DataCenter;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using U3D_Render.Common;

namespace ASpeedGame.Data.GameMobaSchemes
{
    public class MobaSchemesConfig
    {
        //字符串之间的分隔符
        public const char separator = ';';
        #region 物品相关
        // 推荐物品配置
        public const string RECOMMEND_GOOD_SCHEME_FILENAME = "MobaRecommendGood.csv";
        //物品配置
        public const string MOBA_GOODS_SCHEME_FILENAME = "MobaGoods.csv";
        //物品标签类型
        public const string MOBA_GOODS_LABEL_DESCRIPTION = "GoodLableDescription.csv";

        // 最大推荐物品数
        public const int RECOMMEND_GOODS_MAX_COUNT = 10;
        //最大推荐物品类型数
        public const int RECOMMEND_GOODS_MAX_TYPE = 5;

        #endregion

        #region 战绩表数据图形表现形式
        //战绩表数据图形表现形式配置
        public const string MOBA_RECORD_TABLE_PIC = "WarRecordTableData.csv";
        #endregion

        #region 称号介绍
        // 称号介绍相关配置
        public const string MOBA_RECORD_TITLE_CONFIG = "TitleConfig.csv";
        #endregion

        #region 英雄信息配置

        //英雄详细信息配置
        public const string DETAILED_PROPERTY_INFO = "DetailedPropertyInfo.csv";
        #endregion

        # region 技能提示配置
        //技能提示配置
        public const string SHOW_SPELL_PROMPT = "ShowSpellPrompt.csv";

		#endregion

		#region 战场大神推荐配置
		public const string GOD_RECOMMD_EQUIPSCHEME = "WarGodRecommdEquip.csv";
		#endregion
	}

    #region 物品配置表相关
    /// <summary>
    /// 与MobaRecommendGood.csv的列对应
    /// </summary>
    public enum EMobaRecommendGoods : int
    {
        EMRG_HeroID = 0,
        /// <summary>物品推荐种类一</summary>
        EMRG_EerlyStage,
        /// <summary>物品推荐种类二</summary>
        EMRG_MiddleStage,
        /// <summary>物品推荐种类三</summary>
        EMRG_DownWindStage,
        /// <summary>物品推荐种类四</summary>
        EMRG_UpWindStage,
        /// <summary>物品推荐种类五</summary>
        EMRG_ConsumeGoods,
        /// <summary>英雄默认页</summary>
        EMRG_DefaultTypes,
        /// <summary>本套推荐装用于的比赛类型</summary>
		EMRG_MatchTypes,
        /// <summary>使用的推荐种类列表</summary>
        EMRG_UsedStage,
        /// <summary>切换物品推荐时替换的物品推荐种类列表</summary>
        EMRG_DispositionStage,
        /// <summary>召唤师技能ID</summary>
        EMRG_SummerID,
        /// <summary>召唤师技能推荐种类一</summary>
        EMRG_SummerEerlyStage,
        /// <summary>召唤师技能推荐种类二</summary>
        EMRG_SummerMiddleStage,
        /// <summary>召唤师技能推荐种类三</summary>
        EMRG_SummerDownWindStage,
        EMRG_Max,
    }
    /// <summary>
    /// 与MobaGoods.csv里面的列对应
    /// </summary>
    public enum EMobaGoods : int
    {
        EMG_ID = 0,                 //A  物品ID
        EMG_Name,                   //B    装备名称
        EMG_Type,                   //C    物品类型(1、装备 2、物品)
        EMG_IconID,                 //D  装备图标ID
        EMG_LabelGroup,             //E  装备标签组
        EMG_Label,                  //F   装备标签子类型
        EMG_IsHeroLimit,            //G 是否英雄专属
        EMG_HeroID,                 //H  英雄ID
        EMG_VocationTypeLimit,      //I   职业限制
        EMG_NeedComposeList,        //J 合成需要装备
        EMG_CanComposeList,         //K  能合装备
        EMG_Cost,                   //L    绝对价格
        EMG_SoldOutCost,            //M 出售价格
        EMG_EffectID,               //N    效果ID组
        EMG_EffectType,             //O  效果类型组
        EMG_RemoveEffectType,       //P    物品移除调用效果
        EMG_InheritEffectID,        //Q   效果ID组
        EMG_InheritEffectType,      //R  效果类型组
        EMG_InheritRemoveEffectType,//S    物品移除调用效果
        EMG_WarLimit,               //T 战场限制
        EMG_IsAutoUse,              //U 自动使用
        EMG_IsSpecialSlot,          //V 特殊槽位物品
        EMG_CanUse,                 //W  是否能主动使用
        EMG_UsedRemove,             //X  使用是否移除
        EMG_CanOverlay,             //Y  是否能叠加
        EMG_MaxOverlayCount,        //Z 最大叠加数量
        EMG_MaxCount,               //AA    最大拥有数
        EMG_UseBuffID,              //AB   使用BuffID
        EMG_UseBuffFreezeID,        //AC     使用Buff冷却ID
        EMG_UseSkillID,             //AD  使用技能ID
        EMG_CountCanUse,            //AE 可使用次数
        EMG_IconList,               //AF 对应图标组
        EMG_Description,            //AG 装备描述
        EMG_SlotDescription,        //AH 装备栏描述
        EMG_PreciousDegree,         //AI 贵重程度
        EMG_Flag,                   //AJ 标识
        EMG_LocationInstructions,   //AK 装备定位说明

    }
    /// <summary>
    /// 存储当前英雄的所有推荐物品
    /// </summary>
    public class MobaHeroRecommendGoods
    {
        //推荐种类1
        public List<SSchemeMobaGoods> RecommendGoods_EerlyList = new List<SSchemeMobaGoods>();
        //推荐种类2
        public List<SSchemeMobaGoods> RecommendGoods_MiddleList = new List<SSchemeMobaGoods>();
        //推荐种类3
        public List<SSchemeMobaGoods> RecommendGoods_DownWindList = new List<SSchemeMobaGoods>();
        //推荐种类4
        public List<SSchemeMobaGoods> RecommendGoods_UpWindList = new List<SSchemeMobaGoods>();
        //推荐种类5
        public List<SSchemeMobaGoods> RecommendGoods_ConsumeGoodsList = new List<SSchemeMobaGoods>();
        //默认物品类型
        public List<int> RecommendGoods_DefaultGoodsTypeList = new List<int>();
        /// <summary>使用的推荐种类列表</summary>
        public List<int> RecommendGoods_UsedStageList = new List<int>();
         /// <summary>切换物品推荐时替换的物品推荐种类列表</summary>
        public List<int> RecommendGoods_DispositionStageList = new List<int>();

        /// <summary>召唤师技能ID</summary>
        public int RecommendGoods_SummerID;
        //召唤师技能推荐种类1
        public List<SSchemeMobaGoods> RecommendGoods_SummerEerlyList = new List<SSchemeMobaGoods>();
        //召唤师技能推荐种类2
        public List<SSchemeMobaGoods> RecommendGoods_SummerMiddleList = new List<SSchemeMobaGoods>();
        //召唤师技能推荐种类3
        public List<SSchemeMobaGoods> RecommendGoods_SummerDownWindList = new List<SSchemeMobaGoods>();
    }
    /// <summary>
    /// Moba物品类，单个物品在显示层需要用到的类
    /// 注：目前没有完全将配置内容加进来，有需要再添加
    /// 每增加一个属性：
    /// 1、InitMobaGoodsScheme()读表时初始化这个新增加的属性
    /// 2、CreateASMobaGoods()要获取新增的属性设置给ASObject
    /// </summary>
    public class SSchemeMobaGoods
    {
        #region 读配置表初始化的属性
        /// <summary>物品ID</summary>
        public int GoodsID;
        /// <summary>物品名称</summary>
        public string GoodsName;
        /// <summary>物品图标ID</summary>
        public int GoodsIconID;
        /// <summary>物品绝对价格</summary>
        public int GoodsCost;
        /// <summary>物品出售价格</summary>
        public int GoodsSoldOutCost;
        /// <summary>物品描述</summary>
        public string GoodsDes;
        /// <summary>装备栏描述</summary>
        public string GoodsSlotDes;
        /// <summary>物品能合的装备</summary>
        public string GoodsCanComposeList;
        /// <summary>物品合成需要装备</summary>
        public string GoodsNeedComposeList;
        /// <summary>物品类型标签</summary>
        public string GoodsLabel;
        /// <summary>战场限制信息</summary>
        public string GoodsWarLimit;
        /// <summary>物品限制英雄类型</summary>
        public int GoodsVocationLimit;
        /// <summary>物品贵重程度</summary>
        public int GoodsPreciousDegree;
        /// <summary>MOBA_GOODS_FLAG 标识</summary>
        public int GoodsFlag;
        /// <summary>定位说明</summary>
        public string GoodsLocationInstructions;
        #endregion

        public SSchemeMobaGoods()
        {
            GoodsID = 0;
            GoodsName = string.Empty;
            GoodsIconID = 0;
            GoodsCost = 0;
            GoodsSoldOutCost = 0;
            GoodsDes = string.Empty;
            GoodsCanComposeList = string.Empty;
            GoodsNeedComposeList = string.Empty;
            GoodsLabel = string.Empty;
            GoodsWarLimit = string.Empty;
            GoodsSlotDes = string.Empty;
            GoodsVocationLimit = 0;
            GoodsFlag = 0;
            GoodsPreciousDegree = 0;
        }

        public void CopyTo(ref SSchemeMobaGoods _nOtherGoods)
        {
            _nOtherGoods.GoodsID = GoodsID;
            _nOtherGoods.GoodsName = GoodsName;
            _nOtherGoods.GoodsIconID = GoodsIconID;
            _nOtherGoods.GoodsCost = GoodsCost;
            _nOtherGoods.GoodsSoldOutCost = GoodsSoldOutCost;
            _nOtherGoods.GoodsDes = GoodsDes;
            _nOtherGoods.GoodsCanComposeList = GoodsCanComposeList;
            _nOtherGoods.GoodsNeedComposeList = GoodsNeedComposeList;
            _nOtherGoods.GoodsLabel = GoodsLabel;
            _nOtherGoods.GoodsWarLimit = GoodsWarLimit;
            _nOtherGoods.GoodsSlotDes = GoodsSlotDes;
            _nOtherGoods.GoodsVocationLimit = GoodsVocationLimit;
            _nOtherGoods.GoodsPreciousDegree = GoodsPreciousDegree;
        }
    }
    #endregion

    #region 战绩表数据图形表现形式相关
    public enum EMobaRecord : int
    {
        EMR_ID,         // 索引id
        EMR_DESC,       // 描述
        EMR_COLOR,      // 颜色
        EMR_DATAMAX,    // 数据最大值

    }
    public class SSchemeRecordConfig
    {
        // 数据索引1
        public int nId;
        // 数据描述
        public string stDesc;
        // 数据颜色
        public string stColor;
        // 数据最大值
        public int nDataNumMax;
    }
    #endregion

    #region 称号介绍相关
    public enum EMobaTitle : int
    {
        EMT_ID,         // 索引id
        EMT_TYPE,       // 称号类型
        EMT_NAME,       // 称号名字
        EMT_DESC,       // 称号描述
    }
    public class SSchemeTitleConfig
    {
        // id
        public int nId;
        // 称号类型
        public string stTitleType;
        // 名字
        public string stName;
        // 描述
        public string stDesc;
    }
    #endregion

    #region 英雄属性相关
    /// <summary>
    /// 与DetailedPropertyInfo.csv 属性ID对应
    /// </summary>
    public enum EMobaDetailedPropertyInfoID : int
    {
        EMDPI_PA,           //物理攻击力
        //EMDPI_AF,         //攻击速度
        EMDPI_PCT_AF,       // 百分比普攻系数
        EMDPI_PDP,          //物理防御穿透
        //EMDPI_PCT_PDP,    //百分比物理防御穿透
        EMDPI_PCTPDrains,   //百分比物理吸血

        EMDPI_MA,           //法术强度
        EMDPI_CRC,          //暴击几率
        EMDPI_MDP,          //法术防御穿透
        //EMDPI_PCT_MDP,    //百分比法术防御穿透
        EMDPI_PCTMDrains,   //百分比法术吸血

        EMDPI_Hp,           //总血量
        EMDPI_PD,           //物理防御
        EMDPI_AttackRange,  //射程(目前无法获取该属性)
        EMDPI_HpReply,      //血量回复

        EMDPI_Mp,           //总法力
        EMDPI_MD,           //法术防御
        EMDPI_PCTCoolReduce,//百分比冷却缩减
        EMDPI_MpReply,      //法力回复

        EMDPI_Max
    }

    public enum EMobaDetailedPropertyInfo : int
    {
        EMDPIR_PropertyID,//属性ID
        EMDPIR_PropertyName,//属性名
        EMDPIR_PropertyDes,//属性描述
    }
    /// <summary>
    /// DetailedPropertyInfo.csv 配置结构
    /// </summary>
    public class MobaDetatiledPropertyInfo
    {
        //属性名
        public string PropertyName;
        //描述
        public string PropertyDes;
    }
    #endregion

    #region 技能提示相关
    /// <summary>
    /// 与MobaSchemesConfig::SHOW_SPELL_PROMPT 配置表的列对应
    /// </summary>
    public enum EShowSpellPrompt
    {
        ESSP_ID,//提示ID
        ESSP_SpellID,//技能ID
        ESSP_SlotID,//槽位ID（目前是只涉及技能槽位)
    }
    /// <summary>
    /// 技能提示类
    /// </summary>
    public class SSchemeShowSpellPrompt
    {
        //提示ID
        public int nID;
        //技能ID
        public int nSpellID;
        //槽位ID
        public int nSlotID;
    }
	#endregion

	#region 大神推荐装备相关
	public enum EGodRecommdEquip : int
	{
		EGRE_LeagueName,        // 联赛名称
		EGRE_LeagueType,        // 联赛类型
		EGRE_Title,             // 称号
		EGRE_HeroID,            // 英雄ID
		EGRE_SchemeName,        // 方案名字
		EGRE_AuthorName,        // 作者名字
		EGRE_EquipList,         // 装备列表

	}
	public class SSchemeGodRecommdEquip
	{
		public string LeagueName;		// 联赛名称
		public int LeagueType;			// 联赛类型
		public string Title;			// 称号
		public int HeroID;				// 英雄ID
		public string SchemeName;		// 方案名字
		public string AuthorName;		// 作者名字
		public int[] nEquipList;		// 装备列表
	}

	#endregion
	public class GameMobaSchemesManager : Singleton<GameMobaSchemesManager>
    {     

        #region 增加战场专用配置时，从这里初始化和卸载
        //加载配置
        public void LoadSchemes(string strDefaultViewConfigPath, int nWarTypeId, int nMatchTypeId)
        {
            InitAboutMobaGoodsScheme(strDefaultViewConfigPath, nWarTypeId, nMatchTypeId);
            InitAboutDetailedPropertyInfo(strDefaultViewConfigPath);
            InitAboutShowSpellPromptScheme(strDefaultViewConfigPath);
            InitMovaRecordTablePic(strDefaultViewConfigPath);
            InitMobaTitleConfig(strDefaultViewConfigPath);
			InitGodRecommdEquip(strDefaultViewConfigPath);
		}

        //卸载配置
        public void DestroySchemes()
        {
            DestroyAboutMobaGoodsScheme();
            DestroyAboutDetailedPropertyInfo();
            DestroyAboutShowSpellPromptScheme();
            DestroyAboutMovaRecordTablePic();
            DestroyAboutTitleConfig();
			DestroyGodRecommdEquipConfig();
        }

        #endregion

        #region 物品配置
        #region 属性
        //定义推荐物品初始化完成事件，将推荐物品推送到UI显示
        public delegate void MobaGoodsSchemesInitEnd(MobaGoodsSchemesEventArgs args);
        public static event MobaGoodsSchemesInitEnd MobaGoodsSchemesInitEndEvent;
        public class MobaGoodsSchemesEventArgs : System.EventArgs
        {
            public MobaHeroRecommendGoods RecommendGoods;
            public Dictionary<int, SSchemeMobaGoods> AllMobaGoodsTable;
            public Hashtable LabelDescirptionTable;
        }

        //建一个委托，用于推荐物品配置表获取当前主英雄后将推荐物品配置信息推送到UI界面进行显示


        //推荐物品配置文件读取器
        //这个要在获取当前玩家Moba英雄ID的推荐物品后才能释放移除
        ScpReader MobaRecommendGoodsReader = null;
        private Hashtable labelDescirptionTable;

        /// <summary>
        /// 当前英雄推荐物品
        /// </summary>
        public MobaHeroRecommendGoods CurrentHeroRecommendGoods;
        #endregion

        #region 公共方法
        /// <summary>
        /// 获取指定物品可合成物品配置属性列表
        /// </summary>
        /// <param name="_nSelectedGoodsID">要查找的ID</param>
        /// <returns>可合成物品显示层对象列表，null则无可合成的物品</returns>
        public List<SSchemeMobaGoods> GetCanComposeSchemeGoodsList(int _nSelectedGoodsID)
        {
            List<SSchemeMobaGoods> ComposeGoodsIDArray = new List<SSchemeMobaGoods>();
            List<int> ComposeGoodsIDList = new List<int>();
            ComposeGoodsIDList = GetCanComposeGoodsIDList(_nSelectedGoodsID);

            //没有可合成物品则直接返回
            if (ComposeGoodsIDList == null)
                return null;

            SSchemeMobaGoods goods ;
            foreach (int i in ComposeGoodsIDList)
            {
                if(LogicDataCenter.warMobaGoodsDataManager.GetMobaGoods_Out(i,out goods))
                    ComposeGoodsIDArray.Add(goods);
            }

            return ComposeGoodsIDArray;
        }
        /// <summary>
        /// 获取指定物品合成所需物品ID列表
        /// </summary>
        /// <param name="_nGoodsID"></param>
        /// <returns></returns>
        public List<int> GetSchemeGoodsNeedComposeList(int _nGoodsID)
        {

            SSchemeMobaGoods goods;

            LogicDataCenter.warMobaGoodsDataManager.GetMobaGoods_Out(_nGoodsID,out goods);


            if (goods.GoodsNeedComposeList != null && goods.GoodsNeedComposeList.Length != 0)
            {
                char[] charSeparator = new char[] { MobaSchemesConfig.separator };
                
                return ScpReader.ConvertStringToIntArray(goods.GoodsNeedComposeList, charSeparator);
            }

            return null;
        }
        #endregion

        #region 私有方法
        /// <summary>
        /// 从配置获取各推荐物品
        /// </summary>
        /// <param name="_list">推荐物品列表</param>
        /// <param name="_schemesStage">推荐物品配置值</param>
        private void SetRecommendGoodsStage(List<SSchemeMobaGoods> _list, string _schemesStage)
        {
            char[] charSeparator = new char[] { MobaSchemesConfig.separator };
            List<int> intArray = ScpReader.ConvertStringToIntArray(_schemesStage, charSeparator);

            for (int index = 0; index < intArray.Count; index++)
            {
                SSchemeMobaGoods goods = null;

                if (!LogicDataCenter.warMobaGoodsDataManager.GetMobaGoods_Out(intArray[index], out goods))
                    goods = new SSchemeMobaGoods();

                _list.Add(goods);
            }
        }
        private void SetRecommendGoodsStage(List<int> _list, string _schemesStage)
        {
            char[] charSeparator = new char[] { MobaSchemesConfig.separator };
            List<int> intArray = ScpReader.ConvertStringToIntArray(_schemesStage, charSeparator);

            for (int index = 0; index < intArray.Count; index++)
            {
                _list.Add(intArray[index]);
            }
        }
        /// <summary>
        /// 根据物品ID查找可合成物品列表
        /// </summary>
        /// <param name="_goodsID">要查找的物品ID</param>
        /// <returns>可合成物品列表ID</returns>
        private List<int> GetCanComposeGoodsIDList(int _goodsID)
        {
            SSchemeMobaGoods goods;
            LogicDataCenter.warMobaGoodsDataManager.GetMobaGoods_Out(_goodsID, out goods);

            if (goods != null && goods.GoodsCanComposeList != null && goods.GoodsCanComposeList.Length != 0)
            {
                char[] charSeparator = new char[] { MobaSchemesConfig.separator };

                return ScpReader.ConvertStringToIntArray(goods.GoodsCanComposeList, charSeparator);
            }

            return null;
        }
        /// <summary>
        /// 初始化当前英雄的推荐物品
        /// </summary>
        /// <param name="_defalutViewConfigPath"></param>
        private void InitMobaRecommendGoods(string _defalutViewConfigPath)
        {
            string strRecommendGoodsPath = _defalutViewConfigPath + MobaSchemesConfig.RECOMMEND_GOOD_SCHEME_FILENAME;

            MobaRecommendGoodsReader = new ScpReader(strRecommendGoodsPath,true, 2);
            CurrentHeroRecommendGoods = new MobaHeroRecommendGoods();

            //定义主英雄实体创建委托
            EntityFactory.CreateMainHeroEntityEvent -= SetCurrentHeroRecommendGoods;
            EntityFactory.CreateMainHeroEntityEvent += SetCurrentHeroRecommendGoods;
        }
        /// <summary>
        /// 初始化Moba物品配置列表，并加载
        /// </summary>
        /// <param name="_defaultViewConfigPath"></param>
        private void InitMobaGoodsScheme(string _defaultViewConfigPath, int nWarTypeId, int nMatchTypeId)
        {
            DataCenter.LogicDataCenter.warMobaGoodsDataManager.Load(nWarTypeId, nMatchTypeId);
        }
        /// <summary>
        /// 初始化moba物品标签配置列表
        /// </summary>
        /// <param name="_defaultViewConfigPath"></param>
        private void InitMobaGoodsLabelDescirption(string _defaultViewConfigPath)
        {
            labelDescirptionTable = new Hashtable();
            GameGoodsLabelDesConfig.GameGoodsLabelDesConfig.Instance.GetLabelDescirptionTable(ref labelDescirptionTable);
            //string strLabelDescriptionPath = _defaultViewConfigPath + MobaSchemesConfig.MOBA_GOODS_LABEL_DESCRIPTION;
            //ScpReader LabelDescriptionReader = new ScpReader(strLabelDescriptionPath, true, 2);

            //labelDescirptionTable = new Hashtable();
            //for (int i = 0; i < LabelDescriptionReader.GetRecordCount(); i++)
            //{
            //    int LabelKey = LabelDescriptionReader.GetInt(i, 1, 0);
            //    string LabelValue = LabelDescriptionReader.GetString(i, 0, "");

            //    labelDescirptionTable.Add(LabelKey, LabelValue);
            //}

            //LabelDescriptionReader.Dispose();
        }
        /// <summary>
        /// 创建当前玩家英雄后设置
        /// </summary>
        /// <param name="_heroID">当前英雄ID</param>
        private void SetCurrentHeroRecommendGoods(CreateMainHeroEntityEventArgs e)
        {
            if (MobaRecommendGoodsReader == null)
                return;

            int SchemesHeroID = 0;
            for (int i = 0; i < MobaRecommendGoodsReader.GetRecordCount(); i++)
            {
				// 判断英雄ID
				SchemesHeroID = MobaRecommendGoodsReader.GetInt(i, (int)EMobaRecommendGoods.EMRG_HeroID, 0);
                if (SchemesHeroID == e.MainHeroID)
                {
					// 判断比赛类型
					string strMatchType = MobaRecommendGoodsReader.GetString(i, (int)EMobaRecommendGoods.EMRG_MatchTypes, "");
					char[] charSeparator = new char[] { MobaSchemesConfig.separator };
					List<int> intArray = ScpReader.ConvertStringToIntArray(strMatchType, charSeparator);
					if (!intArray.Contains((int)e.nMatchTypeID))
						continue;

					// 获取装备配置
                    SetRecommendGoodsStage(CurrentHeroRecommendGoods.RecommendGoods_EerlyList, MobaRecommendGoodsReader.GetString(i, (int)EMobaRecommendGoods.EMRG_EerlyStage, "0"));
                    SetRecommendGoodsStage(CurrentHeroRecommendGoods.RecommendGoods_MiddleList, MobaRecommendGoodsReader.GetString(i, (int)EMobaRecommendGoods.EMRG_MiddleStage, "0"));
                    SetRecommendGoodsStage(CurrentHeroRecommendGoods.RecommendGoods_DownWindList, MobaRecommendGoodsReader.GetString(i, (int)EMobaRecommendGoods.EMRG_DownWindStage, "0"));
                    SetRecommendGoodsStage(CurrentHeroRecommendGoods.RecommendGoods_UpWindList, MobaRecommendGoodsReader.GetString(i, (int)EMobaRecommendGoods.EMRG_UpWindStage, "0"));
                    SetRecommendGoodsStage(CurrentHeroRecommendGoods.RecommendGoods_ConsumeGoodsList, MobaRecommendGoodsReader.GetString(i, (int)EMobaRecommendGoods.EMRG_ConsumeGoods, "0"));
                    SetRecommendGoodsStage(CurrentHeroRecommendGoods.RecommendGoods_DefaultGoodsTypeList, MobaRecommendGoodsReader.GetString(i, (int)EMobaRecommendGoods.EMRG_DefaultTypes, "0"));
                    SetRecommendGoodsStage(CurrentHeroRecommendGoods.RecommendGoods_UsedStageList, MobaRecommendGoodsReader.GetString(i, (int)EMobaRecommendGoods.EMRG_UsedStage, "0"));
                    SetRecommendGoodsStage(CurrentHeroRecommendGoods.RecommendGoods_DispositionStageList, MobaRecommendGoodsReader.GetString(i, (int)EMobaRecommendGoods.EMRG_DispositionStage, "0"));

                    CurrentHeroRecommendGoods.RecommendGoods_SummerID = MobaRecommendGoodsReader.GetInt(i, (int)EMobaRecommendGoods.EMRG_SummerID, 0);
                    SetRecommendGoodsStage(CurrentHeroRecommendGoods.RecommendGoods_SummerEerlyList, MobaRecommendGoodsReader.GetString(i, (int)EMobaRecommendGoods.EMRG_SummerEerlyStage, "0"));
                    SetRecommendGoodsStage(CurrentHeroRecommendGoods.RecommendGoods_SummerMiddleList, MobaRecommendGoodsReader.GetString(i, (int)EMobaRecommendGoods.EMRG_SummerMiddleStage, "0"));
                    SetRecommendGoodsStage(CurrentHeroRecommendGoods.RecommendGoods_SummerDownWindList, MobaRecommendGoodsReader.GetString(i, (int)EMobaRecommendGoods.EMRG_SummerDownWindStage, "0"));
                }
            }
            MobaRecommendGoodsReader.Dispose();
            MobaRecommendGoodsReader = null;

            EntityFactory.CreateMainHeroEntityEvent -= SetCurrentHeroRecommendGoods;

            if (MobaGoodsSchemesInitEndEvent != null)
            {
                MobaGoodsSchemesEventArgs a = new MobaGoodsSchemesEventArgs();
                a.RecommendGoods = CurrentHeroRecommendGoods;
                a.LabelDescirptionTable = labelDescirptionTable;
                MobaGoodsSchemesInitEndEvent(a);
            }
        }

        /// <summary>
        /// 初始化Moba战场与物品配置表相关的方法
        /// </summary>
        /// <param name="strDefaultViewConfigPath"></param>
        private void InitAboutMobaGoodsScheme(string strDefaultViewConfigPath, int nWarTypeId, int nMatchTypeId)
        {
            InitMobaGoodsLabelDescirption(strDefaultViewConfigPath);

            InitMobaGoodsScheme(strDefaultViewConfigPath, nWarTypeId, nMatchTypeId);
            //这个要在物品最后初始化
            InitMobaRecommendGoods(strDefaultViewConfigPath);
        }
        /// <summary>
        /// 移除与战场物品相关的配置属性
        /// </summary>
        private void DestroyAboutMobaGoodsScheme()
        {
            if (MobaRecommendGoodsReader != null)
            {
                MobaRecommendGoodsReader.Dispose();
                MobaRecommendGoodsReader = null;
            }
            if (labelDescirptionTable != null)
            {
                labelDescirptionTable.Clear();
                labelDescirptionTable = null;
            }
            if (CurrentHeroRecommendGoods != null)
            {
                CurrentHeroRecommendGoods.RecommendGoods_EerlyList.Clear();
                CurrentHeroRecommendGoods.RecommendGoods_MiddleList.Clear();
                CurrentHeroRecommendGoods.RecommendGoods_UpWindList.Clear();
                CurrentHeroRecommendGoods.RecommendGoods_DownWindList.Clear();
                CurrentHeroRecommendGoods.RecommendGoods_ConsumeGoodsList.Clear();
                CurrentHeroRecommendGoods = null;
            }

            LogicDataCenter.warMobaGoodsDataManager.UnLoad();
        }
        #endregion
        #endregion

        #region 英雄信息配置
        private Hashtable DetailedPropertyInfoTable;

        #region 公共方法
        /// <summary>
        /// 根据属性ID获取对应的属性名字
        /// </summary>
        /// <param name="_propertyID"></param>
        /// <returns></returns>
        public string GetDetailedPropertyName(int _propertyID)
        {
            if (DetailedPropertyInfoTable.ContainsKey(_propertyID))
            {
                MobaDetatiledPropertyInfo property = DetailedPropertyInfoTable[_propertyID] as MobaDetatiledPropertyInfo;
                return property.PropertyName;
            }
            return "";
        }

        /// <summary>
        /// 根据属性ID获取对应的属性描述
        /// </summary>
        /// <param name="_propertyID"></param>
        /// <returns></returns>
        public string GetDetailedPropertyDes(int _propertyID)
        {
            if (DetailedPropertyInfoTable.ContainsKey(_propertyID))
            {
                MobaDetatiledPropertyInfo property = DetailedPropertyInfoTable[_propertyID] as MobaDetatiledPropertyInfo;
                return property.PropertyDes;
            }
            return "";
        }
        #endregion


        #region 私有方法
        /// <summary>
        /// 初始化英雄信息配置
        /// </summary>
        /// <param name="strDefaultViewConfigPath"></param>
        private void InitAboutDetailedPropertyInfo(string strDefaultViewConfigPath)
        {
            string strDetailedPropertyInfoPath = strDefaultViewConfigPath + MobaSchemesConfig.DETAILED_PROPERTY_INFO;

            ScpReader DetailedPropertyInfoReader = new ScpReader(strDetailedPropertyInfoPath, true, 2);

            DetailedPropertyInfoTable = new Hashtable();
            int PropertyID = 0;
            for (int i = 0; i < DetailedPropertyInfoReader.GetRecordCount(); i++)
            {
                MobaDetatiledPropertyInfo PropertyInfo = new MobaDetatiledPropertyInfo();
                PropertyID = DetailedPropertyInfoReader.GetInt(i, (int)EMobaDetailedPropertyInfo.EMDPIR_PropertyID, -1);
                if (PropertyID == -1)
                    continue;
                PropertyInfo.PropertyName = DetailedPropertyInfoReader.GetString(i, (int)EMobaDetailedPropertyInfo.EMDPIR_PropertyName, "");
                PropertyInfo.PropertyDes = DetailedPropertyInfoReader.GetString(i, (int)EMobaDetailedPropertyInfo.EMDPIR_PropertyDes, "");

                UBB.toHtml(ref PropertyInfo.PropertyDes);

                DetailedPropertyInfoTable.Add(PropertyID, PropertyInfo);
            }
        }

        private void DestroyAboutDetailedPropertyInfo()
        {
            if (DetailedPropertyInfoTable != null)
            {
                DetailedPropertyInfoTable.Clear();
                DetailedPropertyInfoTable = null;
            }
        }
        #endregion
        #endregion

        #region 技能提示相关
        private Hashtable ShowSpellPromptTable;
        /// <summary>
        /// 根据技能提示ID 获取对应配置数据
        /// </summary>
        /// <param name="nID"></param>
        /// <returns></returns>
        public SSchemeShowSpellPrompt GetSpellPromptInfo(int nID)
        {
            SSchemeShowSpellPrompt result = null;
            if (ShowSpellPromptTable.ContainsKey(nID))
            {
                result = ShowSpellPromptTable[nID] as SSchemeShowSpellPrompt;
            }
            return result;
        }
        /// <summary>
        /// 初始化技能提示
        /// </summary>
        /// <param name="strDefaultViewConfigPath"></param>
        private void InitAboutShowSpellPromptScheme(string strDefaultViewConfigPath)
        {
            string strLabelDescriptionPath = strDefaultViewConfigPath + MobaSchemesConfig.SHOW_SPELL_PROMPT;


            ScpReader ShowSpellPromptReader = new ScpReader(strLabelDescriptionPath, true, 2);

            ShowSpellPromptTable = new Hashtable();
            for (int i = 0; i < ShowSpellPromptReader.GetRecordCount(); i++)
            {
                SSchemeShowSpellPrompt data = new SSchemeShowSpellPrompt();
                data.nID = ShowSpellPromptReader.GetInt(i, (int)EShowSpellPrompt.ESSP_ID, -1);
                if (data.nID == -1)
                    break;
                data.nSpellID = ShowSpellPromptReader.GetInt(i, (int)EShowSpellPrompt.ESSP_SpellID, 0);
                data.nSlotID = ShowSpellPromptReader.GetInt(i, (int)EShowSpellPrompt.ESSP_SlotID, 0);


                ShowSpellPromptTable.Add(data.nID, data);
            }

            ShowSpellPromptReader.Dispose();
        }
        private void DestroyAboutShowSpellPromptScheme()
        {
            if (ShowSpellPromptTable != null)
            {
                ShowSpellPromptTable.Clear();
                ShowSpellPromptTable = null;
            }
        }
        #endregion
        
        #region 称号介绍
        /// <summary>
        /// 初始化称号
        /// </summary>
        private void InitMobaTitleConfig(string _defaultViewConfigPath)
        {
            string stPath = _defaultViewConfigPath + MobaSchemesConfig.MOBA_RECORD_TITLE_CONFIG;
            ScpReader reader = new ScpReader(stPath, true, 2);
            allTitleConfig = new Hashtable();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeTitleConfig sConfig = new SSchemeTitleConfig();
                sConfig.nId = reader.GetInt(i, (int)EMobaTitle.EMT_ID, 0);
                sConfig.stTitleType = reader.GetString(i, (int)EMobaTitle.EMT_TYPE, "");
                sConfig.stName = reader.GetString(i, (int)EMobaTitle.EMT_NAME, "");
                sConfig.stDesc = reader.GetString(i, (int)EMobaTitle.EMT_DESC, "");
                if (sConfig.nId == 0)
                {
                    continue;
                }
                allTitleConfig[sConfig.nId] = sConfig;
            }
            reader.Dispose();
        }

        private void DestroyAboutTitleConfig()
        {
            if(allTitleConfig != null)
            {
                allTitleConfig.Clear();
                allTitleConfig = null;
            }
        }

        // 获取称号配置表的所有数据
        public Hashtable getTitleAllData()
        {
            return allTitleConfig;
        }

        // 根据id获得称号数据
        public SSchemeTitleConfig getTitleConfigAccordID(int nID)
        {
            if (allTitleConfig.ContainsKey(nID))
            {
                return allTitleConfig[nID] as SSchemeTitleConfig;
            }
            return null;
        }
        #endregion

        #region 战绩表数据图形表现形式配置
        // 战绩表数据
        private Hashtable allDataConfig;

        // 称号描述数据
        private Hashtable allTitleConfig;
        /// <summary>初始化moba战绩表图形表示相关配置 </summary>
        private void InitMovaRecordTablePic(string _defaultViewConfigPath)
        {
            string stPath = _defaultViewConfigPath + MobaSchemesConfig.MOBA_RECORD_TABLE_PIC;
            ScpReader reader = new ScpReader(stPath, true, 2);

            allDataConfig = new Hashtable();
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeRecordConfig sConfig = new SSchemeRecordConfig();
                sConfig.nId = reader.GetInt(i, (int)EMobaRecord.EMR_ID, 0);
                sConfig.stDesc = reader.GetString(i, (int)EMobaRecord.EMR_DESC, "");
                sConfig.stColor = reader.GetString(i, (int)EMobaRecord.EMR_COLOR, "");
                sConfig.nDataNumMax = reader.GetInt(i, (int)EMobaRecord.EMR_DATAMAX, 0);
                if (sConfig.nId == 0)
                {
                    continue;
                }
                allDataConfig[sConfig.nId] = sConfig;
            }

            reader.Dispose();
        }
        private void DestroyAboutMovaRecordTablePic()
        {
            if (allDataConfig != null)
            {
                allDataConfig.Clear();
                allDataConfig = null;
            }
        }

        // 获取所有数据
        public Hashtable getPicAllData()
        {
            return allDataConfig;
        }

        // 根据id获得某个具体值
        public SSchemeRecordConfig getRecordConfigAccordID(int nID)
        {
            if (allDataConfig.ContainsKey(nID))
            {
                return allDataConfig[nID] as SSchemeRecordConfig;
            }
            return null;
        }
		#endregion

		#region 大神推荐装备<英雄ID,推荐方案信息>
		private Dictionary<int, List<SSchemeGodRecommdEquip>> godRecommdEquipConfigList;

		private void InitGodRecommdEquip(string _defaultViewConfigPath)
		{
			string stPath = _defaultViewConfigPath + MobaSchemesConfig.GOD_RECOMMD_EQUIPSCHEME;
			ScpReader reader = new ScpReader(stPath, true, 2);

			godRecommdEquipConfigList  = new Dictionary<int, List<SSchemeGodRecommdEquip>>();
			for (int i = 0; i < reader.GetRecordCount(); i++)
			{
				SSchemeGodRecommdEquip sConfig = new SSchemeGodRecommdEquip();
				sConfig.LeagueName = reader.GetString(i, (int)EGodRecommdEquip.EGRE_LeagueName, "");
				sConfig.LeagueType = reader.GetInt(i, (int)EGodRecommdEquip.EGRE_LeagueType, 0);
				sConfig.Title = reader.GetString(i, (int)EGodRecommdEquip.EGRE_Title, "");
				sConfig.HeroID = reader.GetInt(i, (int)EGodRecommdEquip.EGRE_HeroID, 0);
				sConfig.SchemeName = reader.GetString(i, (int)EGodRecommdEquip.EGRE_SchemeName, "");
				sConfig.AuthorName = reader.GetString(i, (int)EGodRecommdEquip.EGRE_AuthorName, "");

				sConfig.nEquipList = new int[USpeedUI.UDefines.nEquipSchemeMaxEquipNum];
				string strEquipList = reader.GetString(i, (int)EGodRecommdEquip.EGRE_EquipList, "");
				char[] charSeparator = new char[] { MobaSchemesConfig.separator };

				List<int> intList = ScpReader.ConvertStringToIntArray(strEquipList, charSeparator);
				if(intList.Count != USpeedUI.UDefines.nEquipSchemeMaxEquipNum)
				{
					UnityEngine.Debug.LogError("装备列表数量不对,请策划检查配置:" + stPath + ",第" + i+1 + "条数据");
					return;
				}
				for(int j = 0; j < USpeedUI.UDefines.nEquipSchemeMaxEquipNum; j++)
				{
					sConfig.nEquipList[j] = intList[j];
				}

				if (!godRecommdEquipConfigList.ContainsKey(sConfig.HeroID))
					godRecommdEquipConfigList.Add(sConfig.HeroID, new List<SSchemeGodRecommdEquip>());

				godRecommdEquipConfigList[sConfig.HeroID].Add(sConfig);
			}

			reader.Dispose();
		}

		private void DestroyGodRecommdEquipConfig()
		{
			if (godRecommdEquipConfigList != null)
				godRecommdEquipConfigList.Clear();

		}

		public List<SSchemeGodRecommdEquip> getGodRecommdEquipList(int nHeroID)
		{
			if (!godRecommdEquipConfigList.ContainsKey(nHeroID))
				return null;

			return godRecommdEquipConfigList[nHeroID];
		}

		#endregion
	}
}