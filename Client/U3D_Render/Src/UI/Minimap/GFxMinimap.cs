/*******************************************************************
** 文件名:	GFxMinimap.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/1/28
** 版  本:	1.0
** 描  述:	游戏状态 GFx界面显示 小地图信息
** 应  用:  
 *          对应Flash Layout文件：Minimap.swf/gfx

**************************** 修改记录 ******************************
** 修改人: 林建聪
** 日  期: 2015.3.17
** 描  述: 
 *         1、更改父类为SFMovieEx
 *         2、在获取AS对象时添加检测
********************************************************************/

using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using Scaleform;
using GameLogic;
using System.Runtime.InteropServices;
using U3D_Render;
using War;
using U3D_Render.Utility;

class GFxMinimap : SFMovieEx,IEventHandler
{
    protected InvokeQ MyInvokeQ = null;
    public int mMapID = -1;
    private int mainHeroCamp = -1;
    public Rect SceneRect;

    public override void Init(SFGFxMovieManager _parent, SFManager _sfMgr, SFMovieCreationParams _cp)
    {
        base.Init(_parent, _sfMgr, _cp);
        MyInvokeQ = new InvokeQ();
        bCanSyncTickCount = true;

        EventManager.Subscibe(this, EVENT.ENTITY_SHOW_HERO, 0);
    }

    public override void OnDestroy()
    {
        base.OnDestroy();
        EventManager.UnSubscibe(this, EVENT.ENTITY_SHOW_HERO, 0);
    }
    
    public void Init(int nMapID)
    {
        mMapID = nMapID;
        // 获取小地图信息
        SMinimapInfo sInfo = new SMinimapInfo();
        if (GameLogicAPI.getMinimapInfo(this.mMapID, ref sInfo) == false)
        {
            Trace.LogError("OnRegisterMinimap failed, !GameLogicAPI.getMinimapInfo");
            return;
        }

        // AS初始化小地图--优先级最高

        MyInvokeQ._Invoke("MinimapInit", sInfo.szDarkBg, sInfo.szLightBg, sInfo.szBaseMap, sInfo.fLeft, sInfo.fRight,
            sInfo.fTop, sInfo.fBottom);

        SceneRect = new Rect(sInfo.fLeft, sInfo.fTop, Math.Abs(sInfo.fRight - sInfo.fLeft), Math.Abs(sInfo.fBottom - sInfo.fTop));

        // 创建静态打点
        SMinimapStaticIcon staticIcon = new SMinimapStaticIcon();
        int nFromIndex = -1;
        while (GameLogicAPI.getNextMinimapStaticIcon(this.mMapID, nFromIndex, ref staticIcon) == true)
        {
            if (staticIcon.nAutoCreate != 0)
            {
                CreateStaticObj(staticIcon.nIndex);
            }
            nFromIndex = staticIcon.nIndex;
        }

        // 旋转小地图
        if (this.mMapID != -1 && this.mainHeroCamp != -1)
        {
            int nDefalutAngel, nXOffset, nYOffset = 0;
            GetDefaultRotateInfo(mainHeroCamp, out nDefalutAngel, out nXOffset, out nYOffset);
            MyInvokeQ._Invoke("RotateMinimap", nDefalutAngel, nXOffset, nYOffset);
        }
    }

    public override void OnRegisterDocument(Value _root)
    {
        base.OnRegisterDocument(_root);
        Value MinimapFrame;
        GetMemberWithCheck(_root, "MyMinimapFrame", out MinimapFrame);
        MyInvokeQ.OnRegister(MinimapFrame);
    }

    public void OnRegisterMinimap(Value _root)
    {
        MyInvokeQ.OnRegister(_root);
    }

    // 小地图添加物体
    public void AddObject(int nObjID, int nIconID, int nCamp, int nSight, byte entityType, int nRelation, int data)
    {
        // 此处优化下，因为已知MinimapIcon.csv中没有0的这个id. 图标0是野怪
        if(nIconID==0)
        {
            //Trace.Log("minimap addobject =" + nObjID + " failed, nIconID==0");
            return;
        }

        SMinimapIcon sIconInfo = new SMinimapIcon();
        if (GameLogicAPI.getMinimapIconInfo(nIconID, ref sIconInfo) == false)
        {
            Trace.Log("minimap AddObject!!!!!!!!, GameLogicAPI.getMinimapIconInfo nIconID=" + nIconID);
            return;
        }
        // 阵营显示中敌方实体不在小地图上显示，不创建
        if (sIconInfo.nShowType == 0 && nRelation == 1)
        {
            Trace.Log("not show " + nObjID);
            return;
        }

        String strFg = null;
        String strBg = null;
        if(nRelation == 0) 
        {
            strFg = sIconInfo.szFriendCampFg;
            strBg = sIconInfo.szFriendCampBg;
        }
        else if(nRelation == 1)
        {
            strFg = sIconInfo.szEnemyCampFg;
            strBg = sIconInfo.szEnemyCampBg;
        }
        else if(nRelation == 2)
        {
            strFg = sIconInfo.szMidCampFg;
            strBg = sIconInfo.szMidCampBg;
       
        }
        else if(nRelation == 3)
        {
            strFg = sIconInfo.szMainHeroFg;
            strBg = sIconInfo.szMainHeroBg;
            mainHeroCamp = nCamp;
        }
        else
        {
            Trace.LogError("minimap AddObject, no defined nRelation=" + nRelation);
            return;
        }

        int nIsPlayer = ((ENTITY_TYPE)entityType == ENTITY_TYPE.TYPE_PLAYER_ROLE) ? 1 : 0;

        bool canDrawHP = true;
        // 普通怪物不用绘制血量
        if ((ENTITY_TYPE)entityType == ENTITY_TYPE.TYPE_MONSTER && data != (int)EWarMonsterExec.EWME_Tower && data != (int)EWarMonsterExec.EWME_Base)
        {
            canDrawHP = false;
        }

        MyInvokeQ._Invoke("OnCreateEntity", nObjID, strBg, strFg, nSight, nCamp, nIsPlayer, nRelation, canDrawHP);

        // 根据主角阵营小地图自动选择到默认位置
        if(nRelation == 3)
        {
            // 尝试旋转小地图
            if (this.mMapID != -1)
            {
                int nDefalutAngel, nXOffset, nYOffset = 0;
                GetDefaultRotateInfo(mainHeroCamp, out nDefalutAngel, out nXOffset, out nYOffset);
                MyInvokeQ._Invoke("RotateMinimap", nDefalutAngel, nXOffset, nYOffset);
            }
        }
    }

    // 小地图移除物体
    public void RemoveObject(int nObjID)
    {
        MyInvokeQ._Invoke("OnDestroyEntity", nObjID);
    }

    // 小地图更新物体位置
    public void UpdateObjectPos(int nObjID, double dXpos, double dZpos, double dAngle)
    {
        int nIsTower = 0;
        EntityView ev = EntityFactory.getEntityViewByID(nObjID);
        if(ev != null && ev.Type == ENTITY_TYPE.TYPE_MONSTER)
        {
            nIsTower = ev.Flag;
        }
        // 有一个bug 建筑物会被移动到原点，暂时不知道原因。 todo.
        if ((nIsTower == (int)EWarMonsterExec.EWME_Tower || nIsTower == (int)EWarMonsterExec.EWME_Base) && dXpos == 0 && dZpos == 0)
        {
            return;
        }

        MyInvokeQ._Invoke("SendEntityCmd", nObjID, 1, dXpos, dZpos, dAngle, nIsTower);
    }

    // 显示隐藏
    public void SetObjectVisible(int nObjID, int nIconID, int nVisible)
    {
        MyInvokeQ._Invoke("SendEntityCmd", nObjID, 2, nVisible, 0, 0, 0);
    }

    public void SetObjHpPercent(ref EntityView ev, int nIconID, int nHpPercent)
    {
        // 只有防御塔需要设置血量
        if (ev != null && ev.Type == ENTITY_TYPE.TYPE_MONSTER && (ev.Flag == (int)EWarMonsterExec.EWME_Tower || ev.Flag == (int)EWarMonsterExec.EWME_Base))
        {
            MyInvokeQ._Invoke("SendEntityCmd", ev.ID, 3, nHpPercent, 0, 0, 0);
        }
    }

    // 小地图点击事件
    //  yPos忽略，只是为了凑参数个数 
    // opCode: 0-进攻信号 1-撤退信号 2-上帝视角 3-寻路 
    public void OnMinimapClick(float xPos, float yPos, float zPos, int opCode)
    {
        Vector3 vec3Pos = new Vector3(xPos, yPos, zPos);

        SendMinimapClickPosToSkillCtrl(vec3Pos);

        // 进攻撤退信号
        if (EntityFactory.MainHero != null)
        {
            MouseCtrl mouseCtrl = EntityFactory.MainHero.GetComponentInChildren<MouseCtrl>();
            if (mouseCtrl != null)
            {
                if (opCode != (int)EMinimapClickType.EMCT_GodEyeMove)
                {
                    if (mouseCtrl.OnMinimapClick(xPos, yPos, zPos))
                        return;
                }
                else if (mouseCtrl.IsTeamCommandStart())
                {
                    // 拖信息标示UI时屏蔽上帝视角消息
                    MyInvokeQ._Invoke("CancelGodEyeMove");
                    return;
                }
            }
        }

        // 进入上帝视角
        if(opCode == (int)EMinimapClickType.EMCT_GodEyeMode || opCode == (int)EMinimapClickType.EMCT_GodEyeMove)
        {
            // 如果角色处于战斗状态,不能进上帝视角
            if(EntityViewCommandHandler.mainHeroState == 3)
            {
                GameUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_WAR_CHANGE_GODEYE_IN_BATTLE);
                return;
            }

            GodEyeByMainHero(vec3Pos);
            return;
        }

        // 寻路
        if(opCode == (int)EMinimapClickType.EMCT_Navigate)
        {
            BaseStateMachine sm = EntityFactory.MainHero.GetComponent<BaseStateMachine>();
            if (sm != null)
            {
                // 需要确定一下y。 todo
                sm.AutoMoveTo(vec3Pos);
            }

            return;
        }
    }

    public void SetMouseAttachType(int attachType)
    {
        if (EntityFactory.MainHero != null)
        {
            MouseCtrl mouseCtrl = EntityFactory.MainHero.GetComponentInChildren<MouseCtrl>();
            if (mouseCtrl != null)
                mouseCtrl.SetAttachType((MouseAttachFuncType)attachType);
        }
    }

    public void OnSwitchGodEyeMode(bool isGodEyeMode, Vector3 vPoint)
    {
        int mode = 0;
        if(isGodEyeMode)
        {
            mode = 1;
            PopupMovieManager.Instance.PushMovie(this);
        }

        MyInvokeQ._Invoke("OnSwitchGodEyeMode", mode, vPoint.x, vPoint.z);
    }

    public void MarkMinimap(float xPos, float zPos, int sigType)
    {
        MyInvokeQ._Invoke("MarkMinimap", xPos, zPos, sigType);
    }

    public void MinimapEntityAttachEffect(int nObjID, int nEffectType)
    {
        MyInvokeQ._Invoke("EntityAttachEffect", nObjID, nEffectType);
    }

    public void DestroyAllStaticObj()
    {
        MyInvokeQ._Invoke("DestroyAllStaticObj");
    }
    

    public void CreateStaticObj(int objIndex)
    {
        SMinimapStaticIcon data = new SMinimapStaticIcon();
        // 读静态打点配置   
        if(GameLogicAPI.getMinimapStaticObjInfo(this.mMapID, objIndex, ref data) == false)
        {
            Trace.LogError("CreateStaticObj(" + objIndex + "), mapID=" + this.mMapID + " failed");
            return;
        }

        MyInvokeQ._Invoke("CreateStaticObj", objIndex, data.szName, data.szIconName, data.nX, data.nZ, data.szTipStr, data.nTipBgColor);  
    }

    public void DestroyStaticObj(int objIndex)
    {
        MyInvokeQ._Invoke("DestroyStaticObj", objIndex);
    }

    // 创建野怪（静态打点）倒计时
    public void CreateCountDown(int objIndex, int countDown)
    {
         SMinimapStaticIcon data = new SMinimapStaticIcon();
        // 读静态打点配置   
        if(GameLogicAPI.getMinimapStaticObjInfo(this.mMapID, objIndex, ref data) == false)
        {
            return;
        }

        double fcountDown = countDown;
        //fcountDown = fcountDown / 1000;
        MyInvokeQ._Invoke("CreateCountDown", objIndex, data.nX, data.nZ, System.Math.Ceiling(fcountDown));
    }

    private int GetGodEyeRotateAngle(int nCamp)
    {
        // 获取小地图信息
        SMinimapInfo sInfo = new SMinimapInfo();
        if (GameLogicAPI.getMinimapInfo(this.mMapID, ref sInfo) == false)
        {
            return 0;
        }

        string[] strRotation = ((string)(sInfo.szDefaultRotation)).Split(';');
        for (int idx = 0; idx + 1 < strRotation.Length; idx = idx + 4)
        {
            int _nCamp = Convert.ToInt32(strRotation[idx]);
            if (_nCamp == nCamp)
            {
                return Convert.ToInt32(strRotation[idx + 1]) + sInfo.nOriRotation;
            }
        }

        return 0;
    }

    private void GetDefaultRotateInfo(int _nCamp,out int _nAngle,out int _nXOffset,out int _nYOffset)
    {
        // 获取小地图信息
        SMinimapInfo sInfo = new SMinimapInfo();
        if (GameLogicAPI.getMinimapInfo(this.mMapID, ref sInfo) == false)
        {
            _nAngle = _nXOffset = _nYOffset = 0;
            return;
        }

        string[] strRotation = ((string)(sInfo.szDefaultRotation)).Split(';');
        for (int idx = 0; idx + 1 < strRotation.Length; idx = idx + 4)
        {
            int nCamp = Convert.ToInt32(strRotation[idx]);
            if (_nCamp == nCamp)
            {
                _nAngle   = Convert.ToInt32(strRotation[idx + 1]); 
                _nXOffset = Convert.ToInt32(strRotation[idx + 2]);
                _nYOffset = Convert.ToInt32(strRotation[idx + 3]);
                return;
            }
        }
        _nAngle = _nXOffset = _nYOffset = 0;
        return;
    }

    // 批量更新小地图实体位置
    public void BatchUpdateEntityPos(Dictionary<int, EntityPosAngleInfo> hTable)
    {
        // 要更新的列表
        Dictionary<EntityView, EntityPosAngleInfo> updateList = new Dictionary<EntityView, EntityPosAngleInfo>();
        foreach (var keypair in hTable)
        {
            EntityPosAngleInfo info = (EntityPosAngleInfo)keypair.Value;

            EntityView ev = EntityFactory.getEntityViewByID(info.id);
            if (ev == null)
            {
                continue;
            }
            //if (ev.CanUpdateMinimap())
            {
                updateList.Add(ev, info);
            }
        }
        if (updateList.Count == 0)
            return;

        //根据ID创建AS对象，并存入数组，传到AS
        Value batchArray = this.CreateArray();
        batchArray.SetArraySize((uint)updateList.Count);

        uint index = 0;
        foreach (var item in updateList)
        {
            EntityView ev = item.Key;
            EntityPosAngleInfo info = item.Value;

            // 防御塔
            int nIsTower = 0;
            if (ev != null && ev.Type == ENTITY_TYPE.TYPE_MONSTER)
            {
                nIsTower = ev.Flag;
            }

            ASObjectEx entityPos;
            entityPos.values = new object[] { info.id, info.pos.x, info.pos.z, info.dir.y, nIsTower };
            entityPos.names     = new string[]  { "entityID", "fPosX", "fPosZ", "fAngle", "isTower" };
            entityPos.types     = new Type[]    { typeof(int), typeof(float), typeof(float), typeof(float), typeof(int) };

            Value asPos = Value.ConvertToASObjectEx(entityPos, this, "Com.Scaleform.Common.MinimapEntityPos");
            if (!Value.IsValueValid(asPos))
            {
                Trace.Warning("ConvertToASObjectEx failed asPos == null");
            }
            // 小地图更新后处理
            ev.OnUpdateMinimap();

            batchArray.SetElement(index, asPos);
            ++index;
        }

        MyInvokeQ._Invoke("BatchUpdateEntityPos", batchArray);
    }

    public override void LeaveMovie()
    {
        SoldierCamera.MainInstance.LeaveGodEyeMode();
    }

    private void SendMinimapClickPosToSkillCtrl(Vector3 _vec3Pos)
    {
        SkillCtrl sc = EntityFactory.MainHero.GetComponent<SkillCtrl>();
        if(sc != null)
        {
            sc.MinimapClickPosInScene = _vec3Pos;
        }
    }

    /// <summary>
    /// 主角控制上帝视角
    /// </summary>
    /// <param name="_vec3Pos"></param>
    public void GodEyeByMainHero(Vector3 _vec3Pos)
    {
        if (EntityFactory.MainHero != null)
        {
            _vec3Pos.y = EntityFactory.MainHero.transform.position.y;
        }
        if (SoldierCamera.MainInstance.cameraMode != SoldierCamera.CameraControlMode.GodEyeControl)
        {
            EnterGodEyeMode(_vec3Pos);
            return;
        }
        ChangeGodEyePos(_vec3Pos);
    }

    /// <summary>
    /// 根据实体ID控制上帝视角 位置
    /// </summary>
    /// <param name="_nGodEyeState">0:进入上帝视角状态；1：移动上帝视角位置</param>
    /// <param name="_nEntityID"></param>
    /// <param name="_eEntityType"></param>
    public void GodEyeBySelectedEntity(int _nEntityID,ENTITY_TYPE _eEntityType = ENTITY_TYPE.TYPE_PLAYER_ROLE)
    {
        EntityView evSelectedEntity = null;

        evSelectedEntity = GetSelectedEntity(_nEntityID, _eEntityType);

        if (evSelectedEntity == null)
            return;

        if (SoldierCamera.MainInstance.cameraMode != SoldierCamera.CameraControlMode.GodEyeControl)
        {
            EnterGodEyeMode(evSelectedEntity.gameObject.transform.position);
            return;
        }

        ChangeGodEyePos(evSelectedEntity.gameObject.transform.position);
    }
    private void EnterGodEyeMode(Vector3 _vec3Pos)
    {
        Vector3 angle = new Vector3(50.0f, 180.0f + GetGodEyeRotateAngle(this.mainHeroCamp), 0.0f);

        SoldierCamera.MainInstance.EnterGodEyeMode(_vec3Pos, angle, new Vector3(0, 0, -1), new Vector3(0, 0, 0), 18.0f, 60.0f);
    }
    private void ChangeGodEyePos(Vector3 _vec3Pos)
    {
        SoldierCamera.MainInstance.ChangeGodEyePos(_vec3Pos);
    }

    private EntityView GetSelectedEntity(int _nEntityID, ENTITY_TYPE _eEntityType)
    {
        switch (_eEntityType)
        {
            case ENTITY_TYPE.TYPE_PLAYER_ROLE:
                return EntityFactory.getPlayerViewByID(_nEntityID);
            case ENTITY_TYPE.TYPE_MONSTER:
                return EntityFactory.getMonsterViewByID(_nEntityID);
            default:
                return EntityFactory.getEntityViewByID(_nEntityID);
        }
    }

    public void OnEvent(U3D_Render.Utility.EVENT eventID, int sourceID, System.Object objEvent)
    {
        switch(eventID)
        {
            case EVENT.ENTITY_SHOW_HERO:
                {
                    if (objEvent == null)
                    {
                        return;
                    }
                    onENTITY_SHOW_HERO(objEvent);
                }
                break;
            default:
                break;
        }
    }

    protected void onENTITY_SHOW_HERO(System.Object objEvent)
    {
        U3D_Render.Utility.SEntityShowHero pEvent = (U3D_Render.Utility.SEntityShowHero)objEvent;
        if (pEvent.ev == null)
        {
            return;
        }

        GFxMinimapManager.Instance.SetObjectVisible(pEvent.ev.ID, pEvent.ev.createinfo.nMinimapIcon, pEvent.bIsHide ? 0 : 1);
    }
    

}