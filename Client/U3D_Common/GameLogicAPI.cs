using System;
using System.Runtime.InteropServices;
using GameLogic;
using UnityEngine;
using Game_ManagedDef;

// 定义实体ID
using ENTITY_ID = System.Int32;

/**
    封装游戏API,由Unity3d中得c#调用c++
*/
public class GameLogicAPI
{
    public GameLogicAPI()
	{
	}
    #region game.dll

    [DllImport("game.dll", EntryPoint = "getMainWindHwnd")]
    public static extern IntPtr getMainWindHwnd();

    [DllImport("game.dll", EntryPoint = "Start")]
    public static extern void Start(IntPtr pRenderEntry);

    
    [DllImport("game.dll", EntryPoint = "Stop")]
    public static extern void Stop();


    [DllImport("game.dll", EntryPoint = "onFrameUpdate")]
    public static extern void onFrameUpdate();

    [DllImport("game.dll", EntryPoint = "findPath")]
    public static extern bool findPath(IntPtr pContext);

    [DllImport("game.dll", EntryPoint = "getGroundLayer")]
    public static extern float getGroundLayer(float fPosX, float fPosZ, float fAgentHeight);

	/** 接受实体显示层发出的事件
	@param entityID   : 实体ID
	@param eventID    : 事件ID,具体定义见(EntityViewDef.h)
	@param nParam  : 整形参数
	@param strParam: 字符串参数
	@param ptrParam: 结构指针参数
	@return           : 是否接收成功
	*/
    [DllImport("game.dll", EntryPoint="onEntityEvent")]
	private static extern void _onEntityEvent(IntPtr entityID,IntPtr eventID,IntPtr nParam,IntPtr  strParam,IntPtr ptrParam, int len);

	static public void onEntityEvent( ENTITY_ID entityID,int eventID,int nParam,string  strParam,IntPtr ptrParam, int len)
	{
        IntPtrHelper helper = new IntPtrHelper();
        IntPtr strParam_Ptr = helper.Str2Ptr(strParam);

	    _onEntityEvent( new IntPtr(entityID),new IntPtr(eventID),new IntPtr(nParam),strParam_Ptr,ptrParam, len);
	}

	/** 接受显示层发出的事件
	@param eventID : 事件ID,具体定义见(GameViewDef.h)
	@param nParam  : 整形参数
	@param strParam: 字符串参数
	@param ptrParam: 结构指针参数
	@return        : 是否接收成功
	*/
    [DllImport("game.dll", EntryPoint="onGameViewEvent")]
	private static extern void _onGameViewEvent(IntPtr eventID,IntPtr nParam,IntPtr strParam,IntPtr ptrParam, int len);
    
    static public void onGameViewEvent(int eventID, int nParam, string strParam, IntPtr ptrParam, int len)
    {
        IntPtrHelper helper = new IntPtrHelper();
        IntPtr strParam_Ptr = helper.Str2Ptr(strParam);

        _onGameViewEvent(new IntPtr(eventID), new IntPtr(nParam), strParam_Ptr, ptrParam, len);
    }

    /** 
        输出调试信息
	*/
    [DllImport("game.dll", EntryPoint = "traceLog")]
    private static extern void _traceLog(IntPtr msg, IntPtr level);
    static public void traceLog(string msg, int level)
    {
        IntPtrHelper helper = new IntPtrHelper();
        IntPtr strParam_Ptr = helper.Str2Ptr(msg);

        _traceLog(strParam_Ptr, new IntPtr(level));
    }


    [DllImport("game.dll", EntryPoint = "GetTickCountEx")]
    public static extern UInt32 getTickCount();


    public static UInt32 GetUnityTickCount()
    {
        float tick = UnityEngine.Time.time * 1000.0f;
        return (UInt32)tick;
    }

    /** 
        获得当前Tick数
    */
    [DllImport("game.dll", EntryPoint = "getSyncTickCount")]
    public static extern UInt32 getSyncTickCount();

    [DllImport("game.dll", EntryPoint = "getStringLen")]
    public static extern int getStringLen(IntPtr str);

    [DllImport("game.dll", EntryPoint = "getMinimapInfo")]
    public static extern IntPtr _getMinimapInfo(int nMapID);

    [DllImport("game.dll", EntryPoint = "OpenLog")]
    public static extern void OpenLog();

    [DllImport("game.dll", EntryPoint = "getMinimapIconInfo")]
    public static extern IntPtr _getMinimapIconInfo(int nIconID);
    static public bool getMinimapInfo(int nMapID, ref SMinimapInfo data)
    {
        IntPtr ptr = _getMinimapInfo(nMapID);
        if ((int)ptr == 0)
        {
            Debug.LogError("getMinimapInfo(" + nMapID + "), failed");
            return false;
        }

        data = IntPtrHelper.toData<SMinimapInfo>(ptr);

        return true;
    }
    static public bool getMinimapIconInfo(int nIconID, ref SMinimapIcon data)
    {
        IntPtr ptr = _getMinimapIconInfo(nIconID);
        if ((int)ptr == 0)
        {
            return false;
        }

        data = IntPtrHelper.toData<SMinimapIcon>(ptr);
        return true;
    }
    //这个方法比getMinimapIconInfo快很多，无需转换整个数据结构
    static public bool getMinimapIconIsTower(int nIconID)
    {
        IntPtr ptr = _getMinimapIconInfo(nIconID);
        if ((int)ptr == 0)
        {
            return false;
        }

        int nIsTower = Marshal.ReadInt32(ptr, 4);

        return nIsTower != 0;
    }


    [DllImport("game.dll", EntryPoint = "getMinimapStaticObjInfo")]
    public static extern IntPtr _getMinimapStaticObjInfo(int nMapID, int nIndex);
    static public bool getMinimapStaticObjInfo(int nMapID, int nIndex, ref SMinimapStaticIcon data)
    {
        IntPtr ptr = _getMinimapStaticObjInfo(nMapID, nIndex);
        if ((int)ptr == 0)
        {
            Debug.LogWarning("getMinimapStaticObjInfo mapID=" + nMapID + " nIndex=" + nIndex);
            return false;
        }

        data = IntPtrHelper.toData<SMinimapStaticIcon>(ptr);
        return true;
    }

    [DllImport("game.dll", EntryPoint = "getNextMinimapStaticIcon")]
    public static extern IntPtr _getNextMinimapStaticIcon(int nMapID, int nFromIndex);
    static public bool getNextMinimapStaticIcon(int nMapID, int nFromIndex, ref SMinimapStaticIcon data)
    {
        IntPtr ptr = _getNextMinimapStaticIcon(nMapID, nFromIndex);
        if ((int)ptr == 0)
        {
            //Trace.LogError("getNextMinimapStaticIcon mapID=" + nMapID + " nFromIndex=" + nFromIndex);
            return false;
        }

        data = IntPtrHelper.toData<SMinimapStaticIcon>(ptr);
        return true;
    }

    [DllImport("game.dll", EntryPoint = "getHeroBackwardSpeedFactor")]
    public static extern float getHeroBackwardSpeedFactor();

    [DllImport("game.dll", EntryPoint = "getHeroSidewaySpeedFactor")]
    public static extern float getHeroSidewaySpeedFactor();

    [DllImport("game.dll", EntryPoint = "getHeroTalentMaxUseCount")]
    public static extern int getHeroTalentMaxUseCount();

    [DllImport("game.dll", EntryPoint = "getXpSkillChargeTime")]
    public static extern int getXpSkillChargeTime();

    [DllImport("game.dll", EntryPoint = "getNetLatency")]
    public static extern int getNetLatency();

    [DllImport("game.dll", EntryPoint = "getPendingCount")]
    public static extern int getPendingCount();

    [DllImport("game.dll", EntryPoint = "getSight")]
    public static extern UInt32 getSight(int nHeroId);

    [DllImport("game.dll", EntryPoint = "getAlertFPS")]
    public static extern int getAlertFPS();

    [DllImport("game.dll", EntryPoint = "getFPSInterVal")]
    public static extern int getFPSInterVal();

    [DllImport("game.dll", EntryPoint = "getLanguageDirName")]
    public static extern IntPtr getLanguageDirName();

    [DllImport("game.dll", EntryPoint = "getCanseckillHpColor")]
    public static extern int getCanseckillHpColor(int nMonsterId);

    [DllImport("game.dll", EntryPoint = "getSpellData")]
    public static extern IntPtr getSpellData(int nSpellID);

    [DllImport("game.dll", EntryPoint = "getSpellIntData")]
    public static extern int getSpellIntData(int nSpellID, int nSpellDataIndex);

    [DllImport("game.dll", EntryPoint = "getSpellStrData")]
    public static extern IntPtr getSpellStrData(int nSpellID, int nSpellDataIndex);

    [DllImport("game.dll", EntryPoint = "GetSlotIndexOfSpell")]
    public static extern int GetSlotIndexOfSpell(int nSpellID);

    [DllImport("game.dll", EntryPoint = "getShortcutDes")]
    public static extern IntPtr getShortcutDes(int nIndex, int nType);

    [DllImport("game.dll", EntryPoint = "GetFreezeTime")]
    public static extern int GetFreezeTime(uint dwClassID, uint dwFreezeID, int nIsShowFlag);

    [DllImport("game.dll", EntryPoint = "getMonsterDistance")]
    public static extern int getMonsterDistance();

    [DllImport("game.dll", EntryPoint = "getTowerDistance")]
    public static extern int getTowerDistance();

    [DllImport("game.dll", EntryPoint = "getMonsterDeadEffect")]
    public static extern int getMonsterDeadEffect();

    [DllImport("game.dll", EntryPoint = "getMoneyEffect")]
    public static extern int getMoneyEffect();

    [DllImport("game.dll", EntryPoint = "getHeroStarLvLimit")]
    public static extern int getHeroStarLvLimit(int nHeroID);

    [DllImport("game.dll", EntryPoint = "getHeroUpgradeNeedGold")]
    public static extern int getHeroUpgradeNeedGold(int nHeroID, int nStarLv);

    [DllImport("game.dll", EntryPoint = "getPlayerLv")]
    public static extern int getPlayerLv();

    [DllImport("game.dll", EntryPoint = "getPersonModel")]
    public static extern IntPtr getPersonModel(UInt16 wHeroID);
    static public bool GetPersonModel(UInt16 wHeroID, ref PersonSkinModelItem data)
    {
        IntPtr ptr = getPersonModel(wHeroID);
        if ((int)ptr == 0)
        {
            return false;
        }

        data = IntPtrHelper.toData<PersonSkinModelItem>(ptr);
        return true;
    }

    [DllImport("game.dll", EntryPoint = "getPersonPropAttackRange")]
    public static extern int getPersonPropAttackRange(int nHeroID);

    [DllImport("game.dll", EntryPoint = "getStaticHeroUpNeedExp")]
    public static extern int getStaticHeroUpNeedExp(int nHeroID, int nLv);


    [DllImport("game.dll", EntryPoint = "getCampPersonNumRestriction")]
    public static extern int getCampPersonNumRestriction(int nMatchTypeID, int nCamp);

    [DllImport("game.dll", EntryPoint = "getMatchAllPersonRestriction")]
    public static extern int getMatchAllPersonRestriction(int nMatchTypeID);

    [DllImport("game.dll", EntryPoint = "getAIMilitaryExploitsFlag")]
    public static extern int getAIMilitaryExploitsFlag();

    [DllImport("game.dll", EntryPoint = "isForbitPK")]
    public static extern int isForbitPK();

    [DllImport("game.dll", EntryPoint = "isInWarScene")]
    public static extern int isInWarScene();

    [DllImport("game.dll", EntryPoint = "getPlayerLevel")]
    public static extern int getPlayerLevel(ENTITY_ID viewID);

    [DllImport("game.dll", EntryPoint = "isAdult")]
    public static extern bool isAdult();

    [DllImport("game.dll", EntryPoint = "isOpenSupperStone")]
    public static extern bool isOpenSupperStone();

    [DllImport("game.dll", EntryPoint = "getHeroStarIconID")]
    public static extern int getHeroStarIconID(int nHeroID, int nStarLv);

    [DllImport("game.dll", EntryPoint = "getHeroSkinConfig")]
    public static extern IntPtr getHeroSkinConfig(int nSkinID);
    public static bool GetHeroSkinInfo(int nSkinID, ref SkinInfoScheme _sData)
    {
        IntPtr infoPtr = getHeroSkinConfig(nSkinID);
        if ((int)infoPtr == 0)
        {
            Debug.LogWarning("getHeroSkinConfig 获取ID:" + nSkinID + "的皮肤信息无效");
            return false;
        }

        _sData = IntPtrHelper.toData<SkinInfoScheme>(infoPtr);

        return true;
    }

    [DllImport("game.dll", EntryPoint = "getHeroStarConfig")]
    public static extern IntPtr getHeroStarConfig(int nHeroID, int nStarLv);
    public static bool getHeroStarConfig(int nHeroID, int nStarLv, ref SHeroStarSchemeInfo _sData)
    {
        IntPtr infoPtr = getHeroStarConfig(nHeroID, nStarLv);
        if ((int)infoPtr == 0)
        {
            Debug.LogWarning(String.Format("getHeroStarConfig 获取HeroID:{0}, StarLv:{1}的星级信息无效", nHeroID, nStarLv));
            return false;
        }

        _sData = IntPtrHelper.toData<SHeroStarSchemeInfo>(infoPtr);
        return true;
    }

    [DllImport("game.dll", EntryPoint = "getPlayerNoviceGuideTypeData")]
    public static extern int getPlayerNoviceGuideTypeData();

    [DllImport("game.dll", EntryPoint = "getLifeHeroTaskData")]
    public static extern int getLifeHeroTaskData();

    //动态障碍
    [DllImport("MapPathFindClient.dll", EntryPoint = "getNavMesh")]
    public static extern void getNavMeshVert(float[] verts,int tileID);

    [DllImport("MapPathFindClient.dll", EntryPoint = "getNavMeshMaxTile")]
    public static extern int getNavMeshMaxTile();

    [DllImport("MapPathFindClient.dll", EntryPoint = "getNavMeshTileSize")]
    public static extern int getNavMeshTileSize(int tileID);

    [DllImport("game.dll", EntryPoint = "IsExistBuff")]
    public static extern bool IsExistBuff(uint dwBuffID);

    [DllImport("game.dll", EntryPoint = "getCurRoomMatchType")]
    public static extern uint getCurRoomMatchType();

    [DllImport("game.dll", EntryPoint = "getCurRoomType")]
    public static extern int getCurRoomType();

    [DllImport("game.dll", EntryPoint = "getCurrentRoomID")]
    public static extern uint getCurrentRoomID();

    [DllImport("game.dll", EntryPoint = "LoadScp")]
    private static extern int _LoadScp(string _strPath,ref IntPtr _ptrByteArray);
    public static bool LoadScp(string _strPath,out byte[] _byteArray)
    {
        bool result = false;
        IntPtr scpPtr = new IntPtr();        
        int length = _LoadScp(_strPath,ref scpPtr);

        _byteArray = null;
        if (null != scpPtr && IntPtr.Zero != scpPtr)
        {
            _byteArray = new byte[length];

            System.Runtime.InteropServices.Marshal.Copy(scpPtr, _byteArray, 0, length);
            result = true;
        }
        
        return result;
    }

    [DllImport("game.dll", EntryPoint = "FreeScpBuff")]
    public static extern void FreeScpBuff();

    [DllImport("game.dll", EntryPoint = "getTeamID")]
    public static extern int getTeamID();

    [DllImport("game.dll", EntryPoint = "getHotkeyIndex")]
    public static extern int getHotkeyIndex();

    [DllImport("game.dll", EntryPoint = "getMapHotkeyIndex")]
    public static extern int getMapHotkeyIndex(int nMapID);

    [DllImport("game.dll", EntryPoint = "getUnlockLevel")]
    public static extern int getUnlockLevel(int wVocation);

    [DllImport("game.dll", EntryPoint = "isEnableEntityRightMenu")]
    public static extern bool isEnableEntityRightMenu();

    [DllImport("game.dll", EntryPoint = "isMapEnableEntityRightMenu")]
    public static extern bool isMapEnableEntityRightMenu(int nMapID);

	[DllImport("game.dll", EntryPoint = "getEntityLocation")]
	public static extern IntPtr getEntityLocation(ENTITY_ID viewID);
	public static bool GetEntityLocation(ENTITY_ID viewID, ref entity_location location)
	{
		IntPtr locationPtr = getEntityLocation(viewID);
		if(locationPtr == IntPtr.Zero)
		{
            Debug.LogWarning("getEntityLocation 获取Entity位置错误");
			return false;
		}

		location = IntPtrHelper.toData<entity_location>(locationPtr);

		return true;
	}

	[DllImport("game.dll", EntryPoint = "getPlayerUserID")]
	public static extern uint getPlayerUserID(ENTITY_ID viewID);

	[DllImport("game.dll", EntryPoint = "ExistKeyWordOfName")]
	public static extern bool ExistKeyWordOfName(string strInStr);

	[DllImport("game.dll", EntryPoint = "filterKeyWordOfName")]
	public static extern int filterKeyWordOfName(string strInStr, ref IntPtr _ptrByteArray);
	public static bool FilterKeyWordOfName(string strInStr, out byte[] byteOutStr)
	{
		byteOutStr = null;
		bool result = false;
		IntPtr strPtr = new IntPtr();

		int nOutSize = filterKeyWordOfName(strInStr, ref strPtr);
		if (nOutSize == 0)
		{
			return false;
		}

		if (null != strPtr && IntPtr.Zero != strPtr)
		{
			byteOutStr = new byte[nOutSize];

			System.Runtime.InteropServices.Marshal.Copy(strPtr, byteOutStr, 0, nOutSize);
			result = true;
		}

		return result;
	}

	[DllImport("game.dll", EntryPoint = "filterKeyWord")]
	public static extern int filterKeyWord(string strInStr, ref IntPtr _ptrByteArray);
	public static bool FilterKeyWord(string strInStr, out byte[] byteOutStr)
	{
		byteOutStr = null;
		bool result = false;
		IntPtr strPtr = new IntPtr();

		int nOutSize = filterKeyWord(strInStr, ref strPtr);
		if(nOutSize == 0)
		{
			return false;
		}
			
		if (null != strPtr && IntPtr.Zero != strPtr)
		{
			byteOutStr = new byte[nOutSize];

			System.Runtime.InteropServices.Marshal.Copy(strPtr, byteOutStr, 0, nOutSize);
			result = true;
		}

		return result;
	}

    [DllImport("game.dll", EntryPoint = "getPlayerPDBID")]
    public static extern uint getPlayerPDBID(ENTITY_ID viewID);

    [DllImport("game.dll", EntryPoint = "getSelectEffectID")]
    public static extern int getSelectEffectID(int nSkinID);

    [DllImport("game.dll", EntryPoint = "toCoinLighting")]
    public static extern IntPtr toCoinLighting(int vocation, int type);

    [DllImport("game.dll", EntryPoint = "checkIsFpsHero")]
    public static extern bool checkIsFpsHero(int wVocation);

    [DllImport("game.dll", EntryPoint = "isRealNameAuthed")]
    public static extern bool isRealNameAuthed();

    [DllImport("game.dll", EntryPoint = "getHeroStarFrameID")]
    public static extern int getHeroStarFrameID(int nHeroID, int nStarLv);

    [DllImport("game.dll", EntryPoint = "getHeroStarEffectID")]
    public static extern int getHeroStarEffectID(int nHeroID, int nStarLv);

    [DllImport("game.dll", EntryPoint = "getMonsterHeadIconID")]
    public static extern int getMonsterHeadIconID(int nMonsterID);

    [DllImport("game.dll", EntryPoint = "getConfigWarID")]
    public static extern int getConfigWarID();

    [DllImport("game.dll", EntryPoint = "getTimeCountKillInterval")]
    public static extern int getTimeCountKillInterval(int nWarID);

    [DllImport("game.dll", EntryPoint = "getWarEndWndDelayTime")]
    public static extern int getWarEndWndDelayTime();

    [DllImport("game.dll", EntryPoint = "bIsComputerPDBID")]
    public static extern int bIsComputerPDBID(int nPBDID);
    
    [DllImport("game.dll", EntryPoint = "getContactTaskSwitch")]
    public static extern int getContactTaskSwitch();

    [DllImport("game.dll", EntryPoint = "isMainMap")]
    public static extern bool isMainMap(int nMapID);

    [DllImport("game.dll", EntryPoint = "getHeroStarNeedCard")]
    public static extern int getHeroStarNeedCard(int nHeroID, int nStarLv, bool bIsLifeHero);

    [DllImport("game.dll", EntryPoint = "getMasterFromGameWorldID")]
    public static extern int getMasterFromGameWorldID();

    [DllImport("game.dll", EntryPoint = "getActorClientAccount")]
    public static extern IntPtr getActorClientAccount();

    [DllImport("game.dll", EntryPoint = "getActorRankIconID")]
    public static extern int getActorRankIconID(int nMatchType, int nRankScore);

    [DllImport("game.dll", EntryPoint = "getRankIconIDByGrade")]
    public static extern int getRankIconIDByGrade(int nMatchType, int nGrade);

    [DllImport("game.dll", EntryPoint = "getMatchtTypeBaseKingRankScore")]
    public static extern int getMatchtTypeBaseKingRankScore(int nMatchType);


    [DllImport("game.dll", EntryPoint = "getActorRankStar")]
    public static extern int getActorRankStar(int nMatchType, int nRankScore);

    [DllImport("game.dll", EntryPoint = "getActorCurRankMaxStar")]
    public static extern int getActorCurRankMaxStar(int nMatchType, int nRankScore);

    [DllImport("game.dll", EntryPoint = "getActorRankName")]
    public static extern IntPtr getActorRankName(int nMatchType, int nRankScore);

    [DllImport("game.dll", EntryPoint = "getActorRankNameByGrade")]
    public static extern IntPtr getActorRankNameByGrade(int nMatchType, int nRankScore);

    [DllImport("game.dll", EntryPoint = "getCardCount")]
    public static extern int getCardCount(int heroID);

    [DllImport("game.dll", EntryPoint = "GetCurNetClass")]
    public static extern int GetCurNetClass();

    [DllImport("game.dll", EntryPoint = "getWarSummerID")]
    public static extern int getWarSummerID();

    [DllImport("game.dll", EntryPoint = "isHeroCanUpStar")]
    public static extern bool isHeroCanUpStar(int heroID, int starLv, bool bIsLifeHero);

    [DllImport("game.dll", EntryPoint = "getCurGameMode")]
    public static extern EGameMode getCurGameMode();
    
    [DllImport("game.dll", EntryPoint = "getGameModeByMatchType")]
    public static extern EGameMode getGameModeByMatchType(int nMatchType);

    [DllImport("game.dll", EntryPoint = "getMatchNameByMatchType")]
    public static extern IntPtr getMatchNameByMatchType(int nMatchType);

    [DllImport("game.dll", EntryPoint = "bIsGM")]
    public static extern bool bIsGM(int nPBDID);

    [DllImport("game.dll", EntryPoint = "IsContinuePlay")]
    public static extern bool IsContinuePlay();

    [DllImport("game.dll", EntryPoint = "GetSlotSpellID")]
    public static extern IntPtr getSlotSpellID(int nHeroID);
    public static SlotSpellID GetSlotSpellID(int nHeroID)
    {
        IntPtr ptr = getSlotSpellID(nHeroID);
        if (ptr == IntPtr.Zero)
        {
            Debug.LogWarning("GetSlotSpellID 获取slot错误");
            return new SlotSpellID();
        }

        return IntPtrHelper.toData<SlotSpellID>(ptr);
    }

    [DllImport("game.dll", EntryPoint = "GetWarHeroNextLevelExp")]
    public static extern int GetWarHeroNextLevelExp(int nEntityID);

    [DllImport("game.dll", EntryPoint = "clanCanPublicLegendCup")]
    public static extern bool clanCanPublicLegendCup();
    

    #endregion

    #region user32.dll
    [DllImport("user32.dll", CharSet = CharSet.Auto)]
    public static extern bool SetWindowText(IntPtr hwnd, string lPstring);

    [DllImport("user32.dll", CharSet = CharSet.Auto)]
    public static extern int GetWindowText(IntPtr hwnd, System.Text.StringBuilder lpString, int nMaxCount);

    [DllImport("user32.dll")]
    public extern static IntPtr GetActiveWindow();

    [DllImport("user32.dll")]
    public extern static IntPtr GetForegroundWindow();

    [DllImport("user32.dll")]
    public extern static IntPtr FindWindow(string lpClassName, string lpWindowName);

    [DllImport("user32.dll", SetLastError = true)]
    public static extern IntPtr FindWindowEx(IntPtr parentHandle, IntPtr childAfter, string className, string windowTitle);

    [DllImport("user32.dll")]
    [return: MarshalAs(UnmanagedType.Bool)]
    public static extern bool GetWindowRect(IntPtr hWnd, ref RECTAPI lpRect);

    [DllImport("user32.dll")]
    public static extern int SetCursorPos(int x, int y);

    [DllImport("User32.dll")]
    public static extern Int32 GetCursorPos(ref POINTAPI lpPoint);

    [DllImport("user32.dll", CharSet = CharSet.Auto, ExactSpelling = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    public static extern bool ClipCursor(ref RECTAPI rcClip);

    [DllImport("User32.dll")]
    public static extern Int32 ClipCursor(IntPtr p);

    [DllImport("user32.dll")]
    [return: MarshalAs(UnmanagedType.Bool)]
    public static extern bool GetClipCursor(out RECTAPI rcClip);

    [DllImport("user32.dll", CharSet = CharSet.Auto)]
    public static extern bool SetWindowPos(IntPtr hWnd, IntPtr hPos, int X, int Y, int cx, int cy, uint uFlags);

    [DllImport("user32.dll")]
    public static extern bool ClientToScreen(IntPtr hWnd, ref POINTAPI lpPoint);

    [DllImport("user32.dll")]
    public static extern bool GetClientRect(IntPtr hwnd,out RECTAPI lpRect);

    [DllImport("user32.dll")]
    public static extern int GetSystemMetrics(int nIndex);

    [DllImport("user32.dll")]
    public static extern IntPtr GetKeyboardLayout(int idThread);

    [DllImport("user32.dll")]
    public static extern uint LoadKeyboardLayout(string pwszKLID, uint Flags);

    [DllImport("user32.dll")]
    public static extern IntPtr ActivateKeyboardLayout(IntPtr hkl, uint Flags);

    [DllImport("user32.dll")]
    public static extern int GetKeyboardLayoutList(int nBuff, [Out, MarshalAs(UnmanagedType.LPArray)] IntPtr[] hkls);

    [DllImport("user32.dll")]
    public static extern bool FlashWindowEx(ref FLASHWINFO pwfi);

    [DllImport("User32.dll")]
    public static extern int SetWindowLong(IntPtr hWnd, int nIndex, int dwNewLong);

    /// <summary>
    /// 设值
    /// </summary>
    [DllImport("User32.dll")]
    public static extern Boolean SystemParametersInfo(UInt32 uiAction,UInt32 uiParam, IntPtr pvParam,UInt32 fWinIni);
    /// <summary>
    /// 取值
    /// </summary>
    [DllImport("User32.dll")]
    public static extern Boolean SystemParametersInfo(UInt32 uiAction, UInt32 uiParam,out IntPtr pvParam, UInt32 fWinIni);

    #endregion

    ///// <summary>
    ///// 取得人物皮肤信息
    ///// </summary>
    ///// <param name="nVocation">职业</param>
    ///// <param name="nSex">性别</param>
    ///// <param name="nSkin">皮肤ID</param>
    ///// <returns></returns>
    //[DllImport("game.dll", EntryPoint = "GetPersonSkinModelItem")]
    //public static extern   PersonSkinModelItem GetPersonSkinModelItem(int nVocation, int nSex, int nSkin);


    ///// <summary>
    ///// 取得怪物配置信息
    ///// </summary>
    ///// <param name="nMonsterID">怪物ID</param>
    ///// <returns></returns>
    //[DllImport("game.dll", EntryPoint = "GetMonsterScheme")]
    //public static extern SMonsterScheme GetMonsterScheme(int nMonsterID);
}
