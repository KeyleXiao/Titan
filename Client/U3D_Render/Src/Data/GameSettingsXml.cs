using System;
using System.Xml;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using HeTuGame.Data;

namespace ASpeedGame.Data.GameSettingsXml
{
    #region 所有游戏设置的属性类型
    /// <summary>
    /// 游戏设置所有属性的基类
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public class GameSettingAttributeBase<T> 
    {
        /// <summary>
        /// 游戏设置的类型值
        /// </summary>
        protected string Attribute_Type;
        public string AType
        {
            get
            {
                return Attribute_Type;
            }
            set
            {
                Attribute_Type = value;
            }
        }

        /// <summary>
        /// 该设置的描述
        /// </summary>
        protected string Attribute_Des;
        public string ADes
        {
            get
            {
                return Attribute_Des;
            }

            set
            {
                Attribute_Des = value;
            }
        }

        /// <summary>
        /// 该设置的当前值
        /// </summary>
        protected T Attribute_Value;
        public T AValue
        {
            get
            {
                return Attribute_Value;
            }
            set
            {
                Attribute_Value = value;
            }
        }

        /// <summary>
        /// 该设置的默认值
        /// </summary>
        protected T Attribute_DefaultValue;
        public T ADefValue
        {
            get
            {
                return Attribute_DefaultValue;
            }
            set
            {
                Attribute_DefaultValue = value;
            }
        }

        /// <summary>
        /// 该设置的前一个值
        /// </summary>
        protected T Attribute_LastValue;
        public T ALastValue
        {
            get
            {
                return Attribute_LastValue;
            }
            set
            {
                Attribute_LastValue = value;
            }
        }

        /// <summary>
        /// 该设置在xml内的节点
        /// </summary>
        protected XmlNode Attribute_XmlNode;
        public XmlNode ANode
        {
            get
            {
                return Attribute_XmlNode;
            }
            set
            {
                Attribute_XmlNode = value;
            }

        }

        /// <summary>
        /// 判断该设置的值是否需要有修改
        /// </summary>
        /// <returns></returns>
        public virtual bool IsDirty() 
        { 
            return true; 
        }

        /// <summary>
        /// 使要改的值生效
        /// </summary>
        public virtual void ValidateSetting(T _value)
        {
            ALastValue = _value;
        }
    }

    /// <summary>
    /// 智能施法属性
    /// </summary>
    public class AutoSpellCasting : GameSettingAttributeBase<bool>
    {
        public AutoSpellCasting()
        {
            this.AType = "bAutoSpellCasting";
        }

        public override bool IsDirty()
        {
            return ALastValue != AValue;
        }

        public override void ValidateSetting(bool _value)
        {
            base.ValidateSetting(_value);
            XmlHelper.SetBoolValue(Attribute_XmlNode, GameSettingsXml.Attribute_value, _value);
        }
    }

    /// <summary>
    /// 当前分辨率的宽属性
    /// </summary>
    public class ResolutionSizeX : GameSettingAttributeBase<int>
    {
        public ResolutionSizeX()
        {
            this.AType = "nResolutionSizeX";
        }

        public override bool IsDirty()
        {
            return ALastValue != AValue;
        }

        public override void ValidateSetting(int _value)
        {
            base.ValidateSetting(_value);
            XmlHelper.SetIntValue(Attribute_XmlNode, GameSettingsXml.Attribute_value, _value);
        }
    }

    /// <summary>
    /// 当前分辨率的高属性
    /// </summary>
    public class ResolutionSizeY : GameSettingAttributeBase<int>
    {
        public ResolutionSizeY()
        {
            this.AType = "nResolutionSizeY";
        }

        public override bool IsDirty()
        {
            return ALastValue != AValue;
        }

        public override void ValidateSetting(int _value)
        {
            base.ValidateSetting(_value);
            XmlHelper.SetIntValue(Attribute_XmlNode, GameSettingsXml.Attribute_value, _value);
        }
    }

    public class ServerIPAddress : GameSettingAttributeBase<string>
    {
        public ServerIPAddress()
        {
            this.AType = "strServerIPAddress";
        }

        public override bool IsDirty()
        {
            return ALastValue != AValue;
        }

        public override void ValidateSetting(string _value)
        {
            base.ValidateSetting(_value);
            XmlHelper.SetStringValue(ANode, GameSettingsXml.Attribute_value, _value);
        }
    }

    /// <summary>
    /// 当前配置文件中保存的已经输入过的用户名
    /// </summary>
    public class LoginedUserName: GameSettingAttributeBase<string>
    {
        public LoginedUserName()
        {
            this.AType = "strLoginedinUserName";
        }

        public override bool IsDirty()
        {
            return ALastValue != AValue;
        }

        public override void ValidateSetting(string _value)
        {
            base.ValidateSetting(_value);
            XmlHelper.SetStringValue(ANode, GameSettingsXml.Attribute_value, _value);
        }
    }
    /// <summary>
    /// 是否需要将鼠标锁在窗口客户区内
    /// </summary>
    public class MouseLockInWindow : GameSettingAttributeBase<int>
    {
        public MouseLockInWindow()
        {
            this.AType = "nMouseLockInWindow";
        }

        public override bool IsDirty()
        {
            return ALastValue != AValue;
        }

        public override void ValidateSetting(int _value)
        {
            base.ValidateSetting(_value);
            XmlHelper.SetIntValue(ANode, GameSettingsXml.Attribute_value, _value);
        }
    }

    /// <summary>
    /// 是否开启多线程
    /// </summary>
    public class EnableMultiThread : GameSettingAttributeBase<bool>
    {
        public EnableMultiThread()
        {
            this.AType = "bEnableMultiThread";
        }

        public override bool IsDirty()
        {
            return ALastValue != AValue;
        }

        public override void ValidateSetting(bool _value)
        {
            base.ValidateSetting(_value);
            XmlHelper.SetBoolValue(ANode, GameSettingsXml.Attribute_value, _value);
        }
    }

    /// <summary>
    /// 是否禁音
    /// </summary>
    public class MuteAudio : GameSettingAttributeBase<bool>
    {
        public MuteAudio()
        {
            this.AType = "bMuteAudio";
        }

        public override bool IsDirty()
        {
            return ALastValue != AValue;
        }

        public override void ValidateSetting(bool _value)
        {
            base.ValidateSetting(_value);
            XmlHelper.SetBoolValue(Attribute_XmlNode, GameSettingsXml.Attribute_value, _value);
        }
    }

    /// <summary>
    /// 当前主音量
    /// </summary>
    public class MainAudioValue : GameSettingAttributeBase<int>
    {
        public MainAudioValue()
        {
            this.AType = "nMainAudioValue";
        }

        public override bool IsDirty()
        {
            return ALastValue != AValue;
        }

        public override void ValidateSetting(int _value)
        {
            base.ValidateSetting(_value);
            XmlHelper.SetIntValue(Attribute_XmlNode, GameSettingsXml.Attribute_value, _value);
        }
    }

    /// <summary>
    /// 当前游戏背景音乐的属性
    /// </summary>
    public class CurrentBGMValue : GameSettingAttributeBase<int>
    {
        public CurrentBGMValue()
        {
            this.AType = "nCurrentBGMValue";
        }

        public override bool IsDirty()
        {
            return ALastValue != AValue;
        }

        public override void ValidateSetting(int _value)
        {
            base.ValidateSetting(_value);
            XmlHelper.SetIntValue(Attribute_XmlNode, GameSettingsXml.Attribute_value, _value);
        }
    }

    /// <summary>
    /// 当前游戏技能音效的属性
    /// </summary>
    public class CurrentSkillAudioValue : GameSettingAttributeBase<int>
    {
        public CurrentSkillAudioValue()
        {
            this.AType = "nCurrentSkillAudioValue";
        }

        public override bool IsDirty()
        {
            return ALastValue != AValue;
        }

        public override void ValidateSetting(int _value)
        {
            base.ValidateSetting(_value);
            XmlHelper.SetIntValue(Attribute_XmlNode, GameSettingsXml.Attribute_value, _value);
        }
    }

    /// <summary>
    /// 当前游戏系统音效的属性
    /// </summary>
    public class CurrentSystemAudioValue : GameSettingAttributeBase<int>
    {
        public CurrentSystemAudioValue()
        {
            this.AType = "nCurrentSystemAudioValue";
        }

        public override bool IsDirty()
        {
            return ALastValue != AValue;
        }

        public override void ValidateSetting(int _value)
        {
            base.ValidateSetting(_value);
            XmlHelper.SetIntValue(Attribute_XmlNode, GameSettingsXml.Attribute_value, _value);
        }
    }

    /// <summary>
    /// 当前游戏语音音效的属性
    /// </summary>
    public class CurrentSpeechAudioValue : GameSettingAttributeBase<int>
    {
        public CurrentSpeechAudioValue()
        {
            this.AType = "nCurrentSpeechAudioValue";
        }

        public override bool IsDirty()
        {
            return ALastValue != AValue;
        }

        public override void ValidateSetting(int _value)
        {
            base.ValidateSetting(_value);
            XmlHelper.SetIntValue(Attribute_XmlNode, GameSettingsXml.Attribute_value, _value);
        }
    }

    /// <summary>
    /// 是否显示血量数字信息
    /// </summary>
    public class ShowBloodInfo : GameSettingAttributeBase<bool>
    {
        public ShowBloodInfo()
        {
            this.AType = "bShowBloodInfo";
        }

        public override bool IsDirty()
        {
            return ALastValue != AValue;
        }

        public override void ValidateSetting(bool _value)
        {
            base.ValidateSetting(_value);
            XmlHelper.SetBoolValue(Attribute_XmlNode, GameSettingsXml.Attribute_value, _value);
        }
    }

    /// <summary>
    /// 是否显示主角血量
    /// </summary>
    public class ShowPlayerBlood : GameSettingAttributeBase<bool>
    {
        public ShowPlayerBlood()
        {
            this.AType = "bShowPlayerBlood";
        }

        public override bool IsDirty()
        {
            return ALastValue != AValue;
        }

        public override void ValidateSetting(bool _value)
        {
            base.ValidateSetting(_value);
            XmlHelper.SetBoolValue(Attribute_XmlNode, GameSettingsXml.Attribute_value, _value);
        }
    }

    /// <summary>
    /// 是否显示角色名称
    /// </summary>
    public class ShowRoleName : GameSettingAttributeBase<bool>
    {
        public ShowRoleName()
        {
            this.AType = "bShowRoleName";
        }

        public override bool IsDirty()
        {
            return ALastValue != AValue;
        }

        public override void ValidateSetting(bool _value)
        {
            base.ValidateSetting(_value);
            XmlHelper.SetBoolValue(Attribute_XmlNode, GameSettingsXml.Attribute_value, _value);
        }
    }

    /// <summary>
    /// 是否显示战队名称
    /// </summary>
    public class ShowKinName : GameSettingAttributeBase<bool>
    {
        public ShowKinName()
        {
            this.AType = "bShowKinName";
        }

        public override bool IsDirty()
        {
            return ALastValue != AValue;
        }

        public override void ValidateSetting(bool _value)
        {
            base.ValidateSetting(_value);
            XmlHelper.SetBoolValue(Attribute_XmlNode, GameSettingsXml.Attribute_value, _value);
        }
    }

    /// <summary>
    /// 是否显示帮会名称
    /// </summary>
    public class ShowClanName : GameSettingAttributeBase<bool>
    {
        public ShowClanName()
        {
            this.AType = "bShowClanName";
        }

        public override bool IsDirty()
        {
            return ALastValue != AValue;
        }

        public override void ValidateSetting(bool _value)
        {
            base.ValidateSetting(_value);
            XmlHelper.SetBoolValue(Attribute_XmlNode, GameSettingsXml.Attribute_value, _value);
        }
    }
    #endregion

    /************************************************************************/

    /************************************************************************/
    public class GameSettingsXml
    {
        public static string Attribute_Type = "type";
        public static string Attribute_value = "value";
        public static string Attribute_des = "des";
        public static string Attribute_def = "def";
        //属性数组
        private List<GameSettingAttributeBase<int>> IntTypeAttributeList = new List<GameSettingAttributeBase<int>>();
        private List<GameSettingAttributeBase<bool>> BoolTypeAttributeList = new List<GameSettingAttributeBase<bool>>();
        private List<GameSettingAttributeBase<string>> StringTypeAttributeList = new List<GameSettingAttributeBase<string>>();
        //修改数组
        private List<GameSettingAttributeBase<int>> IntTypeAttributeDirtyList = null;
        private List<GameSettingAttributeBase<bool>> BoolTypeAttributeDirtyList = null;
        private List<GameSettingAttributeBase<string>> StringTypeAttributeDirtyList = null;
        public List<GameSettingAttributeBase<int>> intTypeAttributeDirtyList
        {
            get
            {
                return IntTypeAttributeDirtyList;
            }
        }
        public List<GameSettingAttributeBase<bool>> boolTypeAttributeDirtyList
        {
            get
            {
                return BoolTypeAttributeDirtyList;
            }
        }
        public List<GameSettingAttributeBase<string>> stringTypeAttributeDirtyList
        {
            get
            {
                return StringTypeAttributeDirtyList;
            }
        }

        #region 添加一个设置要修改的地方，还有要继承的父类GameSettingAttributeBase
        #region 设置属性
        public AutoSpellCasting autoSpellCasting;
        public ResolutionSizeX resolutionSizeX;
        public ResolutionSizeY resolutionSizeY;
        public ServerIPAddress serverIPAddress;
        public LoginedUserName loginedUserName;
        public MouseLockInWindow mouseLockInWindow;
        public EnableMultiThread enableMultiThread;
        public MuteAudio muteAudio;
        public MainAudioValue mainAudioValue;
        public CurrentBGMValue currentBGMValue;
        public CurrentSkillAudioValue currentSkillAudioValue;
        public CurrentSystemAudioValue currentSystemAudioValue;
        public CurrentSpeechAudioValue currentSpeechAudioValue;
        public ShowBloodInfo showBloodInfo;
        public ShowPlayerBlood showPlayerBlood;
        public ShowRoleName showRoleName;
        public ShowKinName showKinName;
        public ShowClanName showClanName;
        #endregion
               
        public void LoadFormXMLNode(XmlNode xmlNode)
        {
            InitSetting();

            //CommonSettings
            XmlNodeList CommonSettingsNodeList = xmlNode.SelectNodes("CommonSettings/sub");
            foreach (XmlNode node in CommonSettingsNodeList)
            {
                if (XmlHelper.GetStringValue(node, Attribute_Type, "") == resolutionSizeX.AType)
                {
                    InitIntAttribute(resolutionSizeX, node);
                    //Trace.Log("LoadFormXMLNode resolutionSizeX.value:" + resolutionSizeX.AValue);
                }
                if (XmlHelper.GetStringValue(node, Attribute_Type, "") == resolutionSizeY.AType)
                {
                    InitIntAttribute(resolutionSizeY, node);
                    //Trace.Log("LoadFormXMLNode resolutionSizeY.value:" + resolutionSizeY.AValue);
                }
                if (XmlHelper.GetStringValue(node,Attribute_Type,"") == serverIPAddress.AType)
                {
                    InitStringAttribute(serverIPAddress, node);
                }
                if (XmlHelper.GetStringValue(node, Attribute_Type, "") == loginedUserName.AType)
                {
                    InitStringAttribute(loginedUserName, node);
                }
                if (XmlHelper.GetStringValue(node, Attribute_Type, "") == mouseLockInWindow.AType)
                {
                    InitIntAttribute(mouseLockInWindow, node);
                }
                if (XmlHelper.GetStringValue(node, Attribute_Type, "") == enableMultiThread.AType)
                {
                    InitBoolAttribute(enableMultiThread, node);
                }
                if (XmlHelper.GetStringValue(node, Attribute_Type, "") == muteAudio.AType)
                {
                    InitBoolAttribute(muteAudio, node);
                }
                if (XmlHelper.GetStringValue(node, Attribute_Type, "") == mainAudioValue.AType)
                {
                    InitIntAttribute(mainAudioValue, node);
                }
                if (XmlHelper.GetStringValue(node, Attribute_Type, "") == currentBGMValue.AType)
                {
                    InitIntAttribute(currentBGMValue, node);
                }
                if (XmlHelper.GetStringValue(node, Attribute_Type, "") == currentSkillAudioValue.AType)
                {
                    InitIntAttribute(currentSkillAudioValue, node);
                }
                if (XmlHelper.GetStringValue(node, Attribute_Type, "") == currentSystemAudioValue.AType)
                {
                    InitIntAttribute(currentSystemAudioValue, node);
                }
                if (XmlHelper.GetStringValue(node, Attribute_Type, "") == currentSpeechAudioValue.AType)
                {
                    InitIntAttribute(currentSpeechAudioValue, node);
                }
                if (XmlHelper.GetStringValue(node, Attribute_Type, "") == showRoleName.AType)
                {
                    InitBoolAttribute(showRoleName, node);
                }
                if (XmlHelper.GetStringValue(node, Attribute_Type, "") == showKinName.AType)
                {
                    InitBoolAttribute(showKinName, node);
                }
                if (XmlHelper.GetStringValue(node, Attribute_Type, "") == showClanName.AType)
                {
                    InitBoolAttribute(showClanName, node);
                }
            }

            //WarSettings
            XmlNodeList WarSettingsNodeList = xmlNode.SelectNodes("WarSettings/sub");
            foreach (XmlNode node in WarSettingsNodeList)
            {
                if (XmlHelper.GetStringValue(node, Attribute_Type, "") == autoSpellCasting.AType)
                {
                    InitBoolAttribute(autoSpellCasting, node);
                }
                if (XmlHelper.GetStringValue(node, Attribute_Type, "") == showBloodInfo.AType)
                {
                    InitBoolAttribute(showBloodInfo, node);
                }
                if (XmlHelper.GetStringValue(node, Attribute_Type, "") == showPlayerBlood.AType)
                {
                    InitBoolAttribute(showPlayerBlood, node);
                }
            }
        }

        public void UnLoadXMLNode()
        {
            autoSpellCasting = null;
            resolutionSizeX = null;
            resolutionSizeY = null;
            serverIPAddress = null;
            loginedUserName = null;
            mouseLockInWindow = null;
            muteAudio = null;
            mainAudioValue = null;
            currentBGMValue = null;
            currentSkillAudioValue = null;
            currentSystemAudioValue = null;
            currentSpeechAudioValue = null;
            showBloodInfo = null;
            showPlayerBlood = null;
            showRoleName = null;
            showKinName = null;
            showClanName = null;
        }

        private void InitSetting()
        {
            //bool Type
            autoSpellCasting = new AutoSpellCasting();
            mouseLockInWindow = new MouseLockInWindow();
            muteAudio = new MuteAudio();
            showBloodInfo = new ShowBloodInfo();
            showPlayerBlood = new ShowPlayerBlood();
            showRoleName = new ShowRoleName();
            showKinName = new ShowKinName();
            showClanName = new ShowClanName();
            //int Type
            resolutionSizeX = new ResolutionSizeX();
            resolutionSizeY = new ResolutionSizeY();
            mainAudioValue = new MainAudioValue();
            currentBGMValue = new CurrentBGMValue();
            currentSkillAudioValue = new CurrentSkillAudioValue();
            currentSystemAudioValue = new CurrentSystemAudioValue();
            currentSpeechAudioValue = new CurrentSpeechAudioValue();
            //string Type
            serverIPAddress = new ServerIPAddress();
            loginedUserName = new LoginedUserName();
            enableMultiThread = new EnableMultiThread();
        }
        #endregion

        /// <summary>
        /// 加载时将xml文件设置存到对应的实例里
        /// </summary>
        private void InitIntAttribute(GameSettingAttributeBase<int> Attribute,XmlNode node)
        {
            Attribute.ANode = node;
            Attribute.ALastValue = Attribute.AValue = XmlHelper.GetIntValue(Attribute.ANode, Attribute_value, -1);
            Attribute.ADefValue = XmlHelper.GetIntValue(Attribute.ANode, Attribute_def, -1);
            Attribute.ADes = XmlHelper.GetStringValue(Attribute.ANode, Attribute_des, "");

            IntTypeAttributeList.Add(Attribute);
        }
        private void InitBoolAttribute(GameSettingAttributeBase<bool> Attribute, XmlNode node)
        {
            Attribute.ANode = node;
            Attribute.ALastValue = Attribute.AValue = XmlHelper.GetBoolValue(Attribute.ANode, Attribute_value, false);
            Attribute.ADefValue = XmlHelper.GetBoolValue(Attribute.ANode, Attribute_def, false);

            Attribute.ADes = XmlHelper.GetStringValue(Attribute.ANode, Attribute_des, "");

            BoolTypeAttributeList.Add(Attribute);
        }
        private void InitStringAttribute(GameSettingAttributeBase<string> Attribute, XmlNode node)
        {
            Attribute.ANode = node;
            Attribute.ALastValue = Attribute.AValue = XmlHelper.GetStringValue(Attribute.ANode, Attribute_value,"");
            Attribute.ADefValue = XmlHelper.GetStringValue(Attribute.ANode, Attribute_def, "");

            Attribute.ADes = XmlHelper.GetStringValue(Attribute.ANode, Attribute_des, "");

            StringTypeAttributeList.Add(Attribute);
        }

        //检测是否有设置属性被更改
        public bool CheckSettingsAttIsDirty()
        {
            bool res = false;
            Trace.Log("GameSettingsXml:CheckSettingsAttIsDirty");
            //检测Int类型属性
            foreach (GameSettingAttributeBase<int> intAtt in IntTypeAttributeList)
            {
                if (intAtt.IsDirty())
                {
                    if (IntTypeAttributeDirtyList == null)
                    {
                        IntTypeAttributeDirtyList = new List<GameSettingAttributeBase<int>>();
                    }
                    IntTypeAttributeDirtyList.Add(intAtt);
                    res = true;
                }
            }

            //检测bool型属性
            foreach (GameSettingAttributeBase<bool> boolAtt in BoolTypeAttributeList)
            {
                if (boolAtt.IsDirty())
                {
                    if (BoolTypeAttributeDirtyList == null)
                    {
                        BoolTypeAttributeDirtyList = new List<GameSettingAttributeBase<bool>>();
                    }
                    BoolTypeAttributeDirtyList.Add(boolAtt);
                    res = true;
                }
            }

            //检测string类型的属性
            foreach (GameSettingAttributeBase<string> strAtt in StringTypeAttributeList)
            {
                if (strAtt.IsDirty())
                {
                    if (StringTypeAttributeDirtyList == null)
                    {
                        StringTypeAttributeDirtyList = new List<GameSettingAttributeBase<string>>();
                    }
                    StringTypeAttributeDirtyList.Add(strAtt);
                    res = true;
                }
            }

            return res;
        }

        /// <summary>
        /// 保存游戏设置
        /// </summary>
        public void SaveGameSettingsDirty()
        {
            CheckSettingsAttIsDirty();
            //保存int值
            if (IntTypeAttributeDirtyList != null)
            {
                foreach (GameSettingAttributeBase<int>intAtt in IntTypeAttributeDirtyList)
                {
                    intAtt.ValidateSetting(intAtt.AValue);
                }
                IntTypeAttributeDirtyList.Clear();
                IntTypeAttributeDirtyList = null;
            }

            //保存bool值
            if (BoolTypeAttributeDirtyList != null)
            {
                foreach (GameSettingAttributeBase<bool> boolAtt in BoolTypeAttributeDirtyList)
                {
                    boolAtt.ValidateSetting(boolAtt.AValue);
                }
                BoolTypeAttributeDirtyList.Clear();
                BoolTypeAttributeDirtyList = null;
            }

            //保存string值
            if (StringTypeAttributeDirtyList != null)
            {
                foreach (GameSettingAttributeBase<string> strAtt in StringTypeAttributeDirtyList)
                {
                    strAtt.ValidateSetting(strAtt.AValue);
                }
                StringTypeAttributeDirtyList.Clear();
                StringTypeAttributeDirtyList = null;
            }

            GameSettingsXmlManager.Instance.SaveGameSettings();

        }
        /// <summary>
        /// 将游戏设置还原到上次保存的值
        /// </summary>
        public void RestoreLastGameSettings()
        {
            //保存int值
            if (IntTypeAttributeDirtyList != null)
            {
                foreach (GameSettingAttributeBase<int> intAtt in IntTypeAttributeDirtyList)
                {
                    intAtt.AValue = intAtt.ALastValue;
                }
                IntTypeAttributeDirtyList.Clear();
                IntTypeAttributeDirtyList = null;
            }

            //保存bool值
            if (BoolTypeAttributeDirtyList != null)
            {
                foreach (GameSettingAttributeBase<bool> boolAtt in BoolTypeAttributeDirtyList)
                {
                    boolAtt.AValue = boolAtt.ALastValue;
                }
                BoolTypeAttributeDirtyList.Clear();
                BoolTypeAttributeDirtyList = null;
            }

            //保存string值
            if (StringTypeAttributeDirtyList != null)
            {
                foreach (GameSettingAttributeBase<string> strAtt in StringTypeAttributeDirtyList)
                {
                    strAtt.AValue = strAtt.ALastValue;
                }
                StringTypeAttributeDirtyList.Clear();
                StringTypeAttributeDirtyList = null;
            }
        }
        /// <summary>
        /// 将游戏设置还原到默认值
        /// </summary>
        public void RestoreDefaultGameSettings()
        {
            //保存int值
            if (IntTypeAttributeDirtyList != null)
            {
                foreach (GameSettingAttributeBase<int> intAtt in IntTypeAttributeDirtyList)
                {
                    intAtt.ValidateSetting(intAtt.ADefValue);
                }
                IntTypeAttributeDirtyList.Clear();
                IntTypeAttributeDirtyList = null;
            }

            //保存bool值
            if (BoolTypeAttributeDirtyList != null)
            {
                foreach (GameSettingAttributeBase<bool> boolAtt in BoolTypeAttributeDirtyList)
                {
                    boolAtt.ValidateSetting(boolAtt.ADefValue);
                }
                BoolTypeAttributeDirtyList.Clear();
                BoolTypeAttributeDirtyList = null;
            }

            //保存string值
            if (StringTypeAttributeDirtyList != null)
            {
                foreach (GameSettingAttributeBase<string> strAtt in StringTypeAttributeDirtyList)
                {
                    strAtt.ValidateSetting(strAtt.ADefValue);
                }
                StringTypeAttributeDirtyList.Clear();
                StringTypeAttributeDirtyList = null;
            }
        }

        /// <summary>
        /// 将所有游戏设置还原到默认值
        /// </summary>
        public void AllRestoreDefaultGameSettings()
        {
            //保存int值
            if (IntTypeAttributeList != null)
            {
                foreach (GameSettingAttributeBase<int> intAtt in IntTypeAttributeList)
                {
                    intAtt.AValue = intAtt.ALastValue = intAtt.ADefValue;
                    intAtt.ValidateSetting(intAtt.ADefValue);
                }
                if (IntTypeAttributeDirtyList != null)
                {
                    IntTypeAttributeDirtyList.Clear();
                    IntTypeAttributeDirtyList = null;
                }
            }

            //保存bool值
            if (BoolTypeAttributeList != null)
            {
                foreach (GameSettingAttributeBase<bool> boolAtt in BoolTypeAttributeList)
                {
                    boolAtt.AValue = boolAtt.ALastValue = boolAtt.ADefValue;
                    boolAtt.ValidateSetting(boolAtt.ADefValue);
                }
                if (BoolTypeAttributeDirtyList != null)
                {
                    BoolTypeAttributeDirtyList.Clear();
                    BoolTypeAttributeDirtyList = null;
                }
            }

            //保存string值
            if (StringTypeAttributeList != null)
            {
                foreach (GameSettingAttributeBase<string> strAtt in StringTypeAttributeList)
                {
                    strAtt.AValue = strAtt.ALastValue = strAtt.ADefValue;
                    strAtt.ValidateSetting(strAtt.ADefValue);
                }
                if (StringTypeAttributeDirtyList != null)
                {
                    StringTypeAttributeDirtyList.Clear();
                    StringTypeAttributeDirtyList = null;
                }
            }
            GameSettingsXmlManager.Instance.SaveGameSettings();
        }
    }

    public class GameSettingsXmlManager
    {

        /// <summary>
        /// 游戏设置文档管理实例
        /// </summary>
        private static GameSettingsXmlManager singleton = new GameSettingsXmlManager();

        private string xmlStringFile;
        private XmlDocument xmlDocument ;

        /// <summary>
        /// 当前游戏设置文档
        /// </summary>
        private GameSettingsXml gameSettingsModel;
        public GameSettingsXml GameSettingsModel
        {
            get
            {
                return gameSettingsModel;
            }
        }

        /// <summary>
        /// 获取游戏设置文档管理实例
        /// </summary>
        /// <returns>游戏设置管理实例管理实例</returns>
        public static GameSettingsXmlManager Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new GameSettingsXmlManager(); 
            }
        }

        /// <summary>
        /// 以string方式载入资源
        /// </summary>
        /// <param name="xmlString">xml 文件路径</param>
        public void Load(string xmlString)
        {
            if(xmlDocument == null)
            {
                xmlDocument = new XmlDocument();
                xmlDocument.Load(xmlString);
                xmlStringFile = xmlString;
                Load(xmlDocument);
            }
        }

        /// <summary>
        /// 载入摄像机配置表 并匹配ID 
        /// </summary>
        /// <param name="xmlDocument"></param>
        private void Load(XmlDocument xmlDocument)
        {
            if(xmlDocument != null)
            {
                XmlNode Item = xmlDocument.SelectSingleNode("/GameSettings");
                gameSettingsModel = new GameSettingsXml();
                gameSettingsModel.LoadFormXMLNode(Item);
            }
        }

        public void UnLoad()
        {
            if(gameSettingsModel != null)
            {
                gameSettingsModel.UnLoadXMLNode();
                gameSettingsModel = null;
            }
            xmlDocument = null;
            singleton = null;
        }

        /// <summary>
        /// 重新保存游戏设置xml文件
        /// </summary>
        public void SaveGameSettings()
        {
           if(xmlDocument != null)
           {
                xmlDocument.Save(xmlStringFile);
           }
        }
    }
}