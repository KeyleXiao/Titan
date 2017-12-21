/*******************************************************************
** 文件名:	GFxTalentMenu2AS.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	陈言蒙
** 日  期:	2016/2/24
** 版  本:	1.0
** 描  述:	游戏状态 GFx界面显示 团队信息, 拆分C#代码中与AS通讯的逻辑
** 应  用:  
 *          对应Flash Layout文件：GFxTalentMenuAS.swf/gfx
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using Scaleform;
using GameLogic;
using U3D_Render;
using ASpeedGame.Data.TalentPage;
public partial class GFxTalentMenu
{
    #region C#->AS
    // 打包40个发送button基础属性数据发下去
    public void SetTalentButtonBaseInfo()
    {
        if (!Value.IsValueValid(TalentCenterPointFrame))
        {
            return;
        }

        Value vAllTalentArray = this.CreateArray();
        Hashtable allTalentPageTable = (TalentPage.Instance.GetAllTalentPageTable());
      //  Hashtable allTalentEffectTable = (TalentPageEffect.Instance.GetAllTalentEffectTable());
        vAllTalentArray.SetArraySize((uint)allTalentPageTable.Count);

        uint i = 0;
        foreach (int nTalentIdKey in allTalentPageTable.Keys)
        {
            Value ASTalentValue;
            SSchemeTalentConfig sTalentInfo = allTalentPageTable[nTalentIdKey] as SSchemeTalentConfig;
            CreateASTalentInfo(sTalentInfo, out ASTalentValue);
            vAllTalentArray.SetElement(i, ASTalentValue);
            i++;
        }

        if (TalentCenterPointFrame.Invoke("SetTalentButtonInfoAll", vAllTalentArray) == null)
        {
            Trace.LogError("call SetTalentButtonBaseInfo failed");
            return;
        }
    }

    // 所有页的加点数据下发
    public void SetSingleTalentPagePointsInfo(IntPtr ptrParam, int nPtrLen)
    {
        if (!Value.IsValueValid(TalentMenuFrame))
        {
            return;
        }

        Hashtable allTalentPageTable = (TalentPage.Instance.GetAllTalentPageTable());
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
        dTalentPageInfo.Clear();

        cmd_count nCountdata = helper.get<cmd_count>();
        for (int i = 0; i < nCountdata.nCount; ++i)
        {
            TalentPageInfo talentPageInfo = new TalentPageInfo();
            cmd_TalentPage_Points_Single data = helper.get<cmd_TalentPage_Points_Single>();
            string sPoints = "";
            string sTalentId = "";
            for (int j = 0; j < data.nTalentNum; j++)
            {
                int nPoint = data.nTalentPoints[j];
                int nTalentId = data.nTalentId[j];
                sPoints += nPoint.ToString();
                sTalentId += nTalentId.ToString();
                if (j != data.nTalentNum - 1)
                {
                    sTalentId += ";";
                }
                int nTalentType = TalentPage.Instance.GetSingleTalentType(nTalentId);
                if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_ATTACK)
                {

                    talentPageInfo.nAttackPointAll += nPoint;
                }
                else if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_TRICK)
                {
                    talentPageInfo.nTrickPointAll += nPoint;
                }
                else if (nTalentType == (int)TalentType.ENUM_TALENTTYPE_HELP)
                {
                    talentPageInfo.nHelpPointAll += nPoint;
                }
            }

            if (data.szTalentPageName == "")
            {

                data.szTalentPageName = "TalentPage" + (i + 1);
            }

            int nPageIndex = data.nCurPageIndex;
            talentPageInfo.szTalentPageName = data.szTalentPageName;
            dTalentPageInfo[nPageIndex] = talentPageInfo;
            // 将所有天赋页的加点数据和名字数据名字下发下去,暂定30个可配置点数后期会改
            if (TalentMenuFrame.Invoke("SetSingleTalentPagePointsInfo", nPageIndex, data.szTalentPageName, sTalentId, sPoints, 30, talentPageInfo.nAttackPointAll, talentPageInfo.nTrickPointAll, talentPageInfo.nHelpPointAll) == null)
            {

                Trace.LogError("call SetSingleTalentPagePointsInfo failed");
                continue;
            }

        }

        if (nCountdata.nCount != 0)
        {
            if (TalentMenuFrame.Invoke("SetOnePagePointView", 1) == null)
            {
                Trace.LogError("call SetOnePagePointView failed");
                return;
            }
        }

    }

    // 发送当前页的加点数据
    public void CreateASTalentInfo(SSchemeTalentConfig talentInfo, out Value ASTalentValue)
    {
        string sEffectIdArray = talentInfo.szTalentArray;
        string sTalentEffectDesc = "";

        if (sEffectIdArray!="")
        {
            string[] sTalentIdArray = ((string)(sEffectIdArray)).Split(';');
            int nArrayLen = sTalentIdArray.Length;
          
            for (int i = 0; i < nArrayLen; i++)
            {
                SSchemeTalentEffectConfig sTalentEffect = new SSchemeTalentEffectConfig();
                int nEffectArrayId = Convert.ToInt32(sTalentIdArray[i]);
                TalentPageEffect.Instance.GetTalentEffectInfo(nEffectArrayId, ref sTalentEffect);
                sTalentEffectDesc += sTalentEffect.szTalentDESC;
                if (i != nArrayLen - 1)
                {
                    sTalentEffectDesc += ";";
                }
            }
        }
       
        ASObjectEx ASTalentEx;
        ASTalentEx.names = new string[] { "TalentID", "TalentName", "TalentIconID", "TalentType", "TalentLevel", "TalentAllowNum", "TalentMaxAddNum", "TalentEffectDesc" };
        ASTalentEx.values = new object[] { talentInfo.nId, talentInfo.szTalentName, talentInfo.nTalentIconId, talentInfo.nTalentType, talentInfo.nTalentLevel, talentInfo.nTalentAllowNum, talentInfo.nMaxAddNum, sTalentEffectDesc };
        ASTalentEx.types = new Type[] { typeof(int), typeof(string), typeof(int), typeof(int), typeof(int), typeof(int), typeof(int), typeof(string) };
        ASTalentValue = Value.ConvertToASObjectEx(ASTalentEx, this, "Com.Scaleform.Common.TalentInfo");
    }
    // 进入房间，发送数据设置状态
    public Dictionary<int, TalentPageInfo> GetWaitingRoomTalentDropInfo(ref int nCurPageIndex)
    {
        // 设置房间状态的天赋状态
        SetWaitingRoomState();
        // 获得排序号
        GetCurPageSortIndex(ref  nCurPageIndex);
        // 获得天赋数据给房间
        return this.dTalentPageInfo;
    }

    public void SetWaitingRoomState()
    {
        if (!Value.IsValueValid(TalentMenuFrame))
        {
            return ;
        }

        if (TalentMenuFrame.Invoke("SetWaitingRoomState") == null)
        {
            Trace.LogError("call SetWaitingRoomState failed");
            return ;
        }
    }

    public void GetCurPageSortIndex(ref int nCurPageIndex)
    {
        if (!Value.IsValueValid(TalentMenuFrame))
        {
            return;
        }

        nCurPageIndex = TalentMenuFrame.Invoke("GetCurPageSortIndex");
    }

    public void SetWaitingRoomStateOutOfTalentPage()
    {
        if (!Value.IsValueValid(TalentMenuFrame))
        {
            return;
        }

        if (TalentMenuFrame.Invoke("SetInitState") == null)
        {
            Trace.LogError("call SetInitState failed");
            return;
        }

        this.LeaveMovie();
    }
	
    public void ChangeCurPageIndex(int nPageIndex)
    {
        if (!Value.IsValueValid(TalentMenuFrame))
        {
            return;
        }

        if (TalentMenuFrame.Invoke("ChangeCurPageIndex", nPageIndex+1) == null)
        {
            Trace.LogError("call ChangeCurPageIndex failed");
            return;
        }

    }
    #endregion


}