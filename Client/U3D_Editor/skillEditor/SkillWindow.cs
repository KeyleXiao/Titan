using UnityEngine;
using UnityEditor;
using System.Collections;
using System.Collections.Generic;
using System;

// 技能编辑窗口
public class SkillWindow : EditorWindow {

	//public SkillControl skillcontrol = null;
	float m_oldTimeScale = 1.0f;

	int nSkillID=1;
	int curTab = 0;			// 当前选中的tab
	bool bOpenSing = false;
	bool bOpenRun  = false;
	bool bOpenAttack = false;
	bool bOpenFollow = false;
	bool bOpenWound  = false;
	bool bOpenComplete = false;

	Vector2 windowOffset = Vector2.zero;
	int curSelectState = -1;
	//SkillControl sc=null;

	[MenuItem("Window/SkillEditor")]
	public static void Init()
	{
		//CSkillManager.Instance.Create ();

		SkillWindow window = (SkillWindow)EditorWindow.GetWindow<SkillWindow> ();
		window.autoRepaintOnSceneChange = true;
		window.Show ();




//		GameObject obj = GameObject.Find ("BattleCamera");
//		if (obj == null) 
//		{
//			return;
//		}
//		window.skillcontrol = obj.GetComponent<SkillControl> ();
	}

    public void OnEnable()
	{
		//RefreshStates (nSkillID);
	}

	public void OnGUI()
	{
//		if (skillcontrol == null) 
//		{
//			GameObject obj = GameObject.Find ("BattleCamera");
//			if (obj == null) 
//			{
//				return;
//			}
//			skillcontrol = obj.GetComponent<SkillControl> ();
//			return;
//		}
//	
//		EditorGUI.DrawRect (new Rect (0, 0, 500, 30), Color.gray);
//		EditorGUI.DrawRect (new Rect (0, 0, 100, 500), Color.gray);

        //GameObject battlecamera = GameObject.Find ("BattleCamera");
        //if (battlecamera != null)
        //{
        //    sc=battlecamera.GetComponent<SkillControl>();
        //}
        //if (sc == null) 
        //{
        //    return;
        //}

		DrawTitle ();
		DrawTabBtn ();
		DrawSubWindow ();

		
		//GC.Collect();

//		ControlGame ();
		//DrawSkillState ();
		//DrawSkillData ();
	}

	void DrawSubWindow()
	{
		BeginWindows ();
		if (curTab == 0)
		{
			GUILayout.Window (1, new Rect (0, 100, 1000, 1000), DrawWindow0, "");
		} 
		else if (curTab == 1)
		{
			GUILayout.Window (1, new Rect (0, 100, 1000, 1000), DrawWindow1, "");
		} 
		else 
		{
			GUILayout.Window (1, new Rect (0, 100, 1000, 1000), DrawWindow2, "");
		}
		EndWindows ();
	}

	// 第一个窗口
	void DrawWindow0(int wndId)
	{
		DrawSkillData (nSkillID);
	}

	// 第二个窗口
	void DrawWindow1(int wndId)
	{
		{
			GUILayout.BeginHorizontal ("Box",GUILayout.Width(300));
			GUILayout.FlexibleSpace();
			newstateindex=EditorGUILayout.Popup("状态",newstateindex,statenames);
			newstatename = statenames[newstateindex];
			GUILayout.FlexibleSpace();
			newstarttime = EditorGUILayout.FloatField ("开始时间:",newstarttime);
			GUILayout.FlexibleSpace();
			newduration = EditorGUILayout.FloatField ("持续时间:",newduration);
			GUILayout.FlexibleSpace();
			if(GUILayout.Button("添加"))
			{
				SkillStateInfo stateInfo = new SkillStateInfo();
				stateInfo.starttime = newstarttime;
				stateInfo.duration = newduration;
				stateInfo.name = newstatename;
				stateInfo.bAdded = true;
				stateinfos.Add(stateInfo);
			}

			{
                //if (CSkillManager.Instance != null) 
                //{
                //    SkillEffectData effect = CSkillManager.Instance.GetSkillEffectData(nSkillID); 
                //    if (effect != null) 
                //    {
                //        bool bCameraEffect=GUILayout.Toggle(effect.bCameraEffect!=0,"镜头效果");
                //        effect.bCameraEffect=bCameraEffect?1:0;
                //        bool bShakeScreen=GUILayout.Toggle(effect.bShakeScreen!=0,"震屏效果");
                //        effect.bShakeScreen=bShakeScreen?1:0;
                //    }
                //}

			}


			GUILayout.EndHorizontal();
		}

		windowOffset = GUI.BeginScrollView (new Rect (0,60,1000,500), windowOffset, new Rect (0,60,1500,1500));
		HandleEvent ();
		DrawSkillStateGraph ();

		GUI.EndScrollView ();
	}
	
	// 第三个窗口
	void DrawWindow2(int wndId)
	{

	}

	void HandleEvent ()
	{
		if (Event.current.type == EventType.MouseUp) 
		{
			curSelectState = GetSelectState(Event.current.mousePosition.y);	
		}

		if (Event.current.type == EventType.ContextClick) 
		{
			Event evt  = Event.current;
			Rect contextRect = new Rect (0, (curSelectState+1)*50, 1500, 50);
			
			Vector2 mousePos = evt.mousePosition;
			if (contextRect.Contains (mousePos))
			{
				// Now create the menu, add items and show it
				GenericMenu menu = new GenericMenu ();
				
				menu.AddItem (new GUIContent ("删除"), false, GenericMenuCallBack, "");
				
				menu.ShowAsContext ();
				
				evt.Use();
			}

		}

		if (Event.current.type == EventType.mouseDrag) 
		{
			if(curSelectState==-1 || curSelectState>=stateinfos.Count)
			{
				return;
			}
			Vector2 pos = Event.current.mousePosition;
			SkillStateInfo info = stateinfos[curSelectState];
			float pos2time = (pos.x-60)/400;
			float left = info.starttime;
			float right = info.starttime+info.duration;
			//float delta = 


			if(Mathf.Abs(pos2time-left)<0.05f && pos2time<(left+info.duration/2))
			{
				info.starttime = pos2time;
				info.duration = right - pos2time;
			}
			else if(Mathf.Abs(right-pos2time)<0.05f && pos2time>(left+info.duration/2))
			{
				info.duration = pos2time-left;
			}

		}
	}

	void GenericMenuCallBack(object obj)
	{
		if (curSelectState < stateinfos.Count && curSelectState >= 0) 
		{
			string name=stateinfos[curSelectState].name;
			if (name=="reserve" || name=="restore" || name=="complete")
			{
				return;//这几个镜头相关的不能直接删
			}
			stateinfos.RemoveAt(curSelectState);
		}

	}

	int GetSelectState(float y)
	{
		int state = ((int)(y-50))/50;
		if(state<0 || state>=stateinfos.Count)
		{
			return -1;
		}
		return state;
	}

	// 绘制技能数据
	void DrawSkillData(int id)
	{
        //if (CSkillManager.Instance == null) 
        //{
        //    return;
        //}

        //CSkillData data = CSkillManager.Instance.GetSkillData (id);
        //if(data==null)
        //{
        //    return;
        //}

        //EditorGUILayout.BeginVertical ();
        //EditorGUILayout.TextField ("名字", data.m_strName.ToString());
        //EditorGUILayout.TextField ("描述", data.m_strDesc2.ToString());
        //EditorGUILayout.TextField ("技能属性", data.m_attribute.ToString());
        //EditorGUILayout.TextField ("消耗魔法", data.m_costMp.ToString());
        //EditorGUILayout.TextField ("攻击", data.m_killHp.ToString());
        //EditorGUILayout.TextField ("攻击系数", data.m_coefficient.ToString());
        //EditorGUILayout.TextField ("系数类型",data.m_coeffType.ToString());
        //EditorGUILayout.TextField ("治疗", data.m_cureHp.ToString());
        //EditorGUILayout.TextField ("图标id", data.m_IconID.ToString());
        //EditorGUILayout.TextField ("技能类型", data.m_skillType.ToString());
        //EditorGUILayout.TextField ("伤害类型", data.m_hurtType.ToString());
        //EditorGUILayout.TextField ("作用对象", data.m_targetType.ToString());
        //EditorGUILayout.TextField ("选中类型", data.m_selectType.ToString());
        //EditorGUILayout.TextField ("攻击类型", data.m_attackType.ToString());
        //EditorGUILayout.TextField ("伤害次数", data.m_attackTimes.ToString());
        //EditorGUILayout.TextField ("伤害衰减", data.m_attackDamp.ToString());
        //EditorGUILayout.TextField ("Buff ID", data.m_buffId.ToString());
        //EditorGUILayout.TextField ("Buff Lv", data.m_bufflv.ToString());
        //EditorGUILayout.TextField ("吸血等级", data.m_vampireLv.ToString());
        //EditorGUILayout.TextField ("技能成长", data.m_growNum.ToString());
        //EditorGUILayout.TextField ("激活条件", data.m_needStar.ToString());
        //EditorGUILayout.EndVertical ();
	}



	void DrawState(string name, string path, float start, float duration)
	{
		EditorGUILayout.BeginToggleGroup (name, duration > 0);
		EditorGUILayout.BeginHorizontal ();
		EditorGUILayout.LabelField (path);

		EditorGUILayout.LabelField (start.ToString());
		EditorGUILayout.LabelField (duration.ToString());
		EditorGUILayout.EndHorizontal ();
		EditorGUILayout.EndToggleGroup ();
	}


	void DrawSkillStateTexts()
	{
        //if (CSkillManager.Instance == null) 
        //{
        //    return;
        //}
        //SkillEffectData effect = CSkillManager.Instance.GetSkillEffectData(nSkillID); 
        //if (effect == null) 
        //{
        //    return;
        //}
		
        //bOpenSing = EditorGUILayout.Foldout (bOpenSing,"SingState");
        //if (bOpenSing) 
        //{
        //    for(int i=0; i<effect.singTime.Length/2; i++)
        //    {
        //        EditorGUILayout.TextField("\t开始时间:", effect.singTime[i].ToString());
        //        EditorGUILayout.TextField("\t持续时间:", effect.singTime[i*2+1].ToString());
        //    }
        //}
		
        //bOpenRun = EditorGUILayout.Foldout (bOpenRun,"RunState");
        //if (bOpenRun) 
        //{
        //    for(int i=0; i<effect.runTime.Length/2; i++)
        //    {
        //        EditorGUILayout.TextField("\t开始时间:", effect.runTime[i].ToString());
        //        EditorGUILayout.TextField("\t持续时间:", effect.runTime[i*2+1].ToString());
        //    }
        //}
		
        //bOpenAttack = EditorGUILayout.Foldout (bOpenAttack,"AttackState");
        //if (bOpenAttack) 
        //{
        //    for(int i=0; i<effect.attackTime.Length/2; i++)
        //    {
        //        EditorGUILayout.TextField("\t开始时间:", effect.attackTime[i].ToString());
        //        EditorGUILayout.TextField("\t持续时间:", effect.attackTime[i*2+1].ToString());
        //    }
        //}
		
        //bOpenFollow = EditorGUILayout.Foldout (bOpenFollow,"FollowState");
        //if (bOpenFollow) 
        //{
        //    for(int i=0; i<effect.followTime.Length/2; i++)
        //    {
        //        EditorGUILayout.TextField("\t开始时间:", effect.followTime[i].ToString());
        //        EditorGUILayout.TextField("\t持续时间:", effect.followTime[i*2+1].ToString());
        //    }
        //}
		
        //bOpenWound = EditorGUILayout.Foldout (bOpenWound,"WoundState");
        //if (bOpenWound) 
        //{
        //    for(int i=0; i<effect.woundTime.Length/2; i++)
        //    {
        //        EditorGUILayout.TextField("\t开始时间:", effect.woundTime[i].ToString());
        //        EditorGUILayout.TextField("\t持续时间:", effect.woundTime[i*2+1].ToString());
        //    }
        //}
		
        //bOpenComplete = EditorGUILayout.Foldout (bOpenComplete,"CompleteState");
        //if (bOpenComplete) 
        //{
        //    EditorGUILayout.TextField("\t开始时间:", effect.completeTime.ToString());
        //}
	}

	void DrawRectangle(Vector2 offset, Vector2 size, Color faceColor, Color outLine)
	{
		Vector3[] vertexs = new Vector3[4];
		vertexs[0] = new Vector3(offset.x, offset.y, 0);
		vertexs[1] = new Vector3(offset.x, offset.y+size.y, 0);
		vertexs[2] = new Vector3(offset.x+size.x, offset.y+size.y, 0);
		vertexs[3] = new Vector3(offset.x+size.x, offset.y, 0);
		
		Handles.DrawSolidRectangleWithOutline(vertexs, faceColor, outLine);
	}
	
	void DrawRow(Vector2 offset, int count, bool bHasNum)
	{
		float cellWidth = 30;
		Handles.DrawLine(new Vector3(offset.x, offset.y, 0), new Vector3(offset.x+cellWidth*count, offset.y, 0));
		if(!bHasNum)
		{
			return;
		}
		for(int i=0; i<=count; i++)
		{
			Handles.color = Color.gray;
			Handles.DrawSolidDisc(new Vector3(offset.x + cellWidth*i, offset.y, 0), Vector3.forward, 2);
			
			GUI.Label(new Rect(offset.x+cellWidth*i, offset.y-15, 20,20), i.ToString());
			
		}
	}

	void DrawCols(float distance, int count)
	{
		for (int i=0;i<count;i++)
		{
			Handles.color=Color.gray;
			Handles.DrawLine(new Vector3(60+distance*i,50,0),new Vector3(60+distance*i,650,0));
		}
	}

	public class SkillStateInfo
	{
		public string name;
		public float starttime;
		public float duration;

		public bool bAdded;
	}
	
	string[] statenames=new string[]{"sing","run","attack","follow","wound","complete"};
	int newstateindex=0;
	string newstatename="";
	float newstarttime=0.0f;
	float newduration=1.0f;
	List<SkillStateInfo> stateinfos = new List<SkillStateInfo> ();

	void DrawSkillStateGraph()
	{
		RefreshStates (nSkillID);

		DrawCols(6,151);
		for (int i=0; i<12; i++) 
		{
			DrawRow( new Vector2(60,100+i*50), 30, false);
		}

		for (int i=0; i<stateinfos.Count; i++) 
		{
			SkillStateInfo sinfo=stateinfos[i];

			float x=sinfo.starttime*200;
			float w=sinfo.duration*200;
			float endtime=sinfo.starttime+sinfo.duration;

			Color color = Color.gray;

			if(curSelectState==i)
			{
				color = Color.red;
			}
			else if(sinfo.bAdded)
			{
				color = Color.yellow;
			}
			DrawRectangle(new Vector2(60+x,100+i*50-50), new Vector2(w,50), color, Color.green);

			GUI.Label(new Rect(0, 100+i*50-20, 60,20), sinfo.name);
			GUI.Label(new Rect(60-10+x, 100+i*50-20, 60,20), sinfo.starttime.ToString());
			GUI.Label(new Rect(60-10+x+w, 100+i*50-20, 60,20), endtime.ToString());
		}

        //if (sc!=null)
        //{
        //    float x=sc.fSkillTime*200;
        //    DrawRectangle(new Vector2(60+x,0), new Vector2(1,650), Color.red, Color.red);
        //}
	}
	
	void RefreshStates(int skillid)
	{
		stateinfos.Clear();
		
        //if (CSkillManager.Instance == null) 
        //{
        //    return;
        //}
        //SkillEffectData effect = CSkillManager.Instance.GetSkillEffectData(nSkillID); 
        //if (effect == null) 
        //{
        //    return;
        //}

        //if (sc == null) 
        //{
        //    return;
        //}
	
        //if (effect.bCameraEffect!=0) 
        //{
        //    SkillStateInfo sinfo=new SkillStateInfo();
        //    sinfo.name="reserve";
        //    sinfo.starttime=-sc.fCameraPullTime;
        //    sinfo.duration=sc.fCameraPullTime;
        //    sinfo.bAdded = false;
        //    stateinfos.Add(sinfo);
        //}

        //for(int i=0; i<effect.singTime.Length/2; i++)
        //{
        //    SkillStateInfo sinfo=new SkillStateInfo();
        //    sinfo.name="sing";
        //    sinfo.starttime=effect.singTime[i*2+0];
        //    sinfo.duration=effect.singTime[i*2+1];
        //    sinfo.bAdded = false;
        //    stateinfos.Add(sinfo);
        //}
		
        //for(int i=0; i<effect.runTime.Length/2; i++)
        //{
        //    SkillStateInfo sinfo=new SkillStateInfo();
        //    sinfo.name="run";
        //    sinfo.starttime=effect.runTime[i*2+0];
        //    sinfo.duration=effect.runTime[i*2+1];
        //    sinfo.bAdded = false;
        //    stateinfos.Add(sinfo);
        //}
		
        //for(int i=0; i<effect.attackTime.Length/2; i++)
        //{
        //    SkillStateInfo sinfo=new SkillStateInfo();
        //    sinfo.name="attack";
        //    sinfo.starttime=effect.attackTime[i*2+0];
        //    sinfo.duration=effect.attackTime[i*2+1];
        //    sinfo.bAdded = false;
        //    stateinfos.Add(sinfo);
        //}
		
        //for(int i=0; i<effect.followTime.Length/2; i++)
        //{
        //    SkillStateInfo sinfo=new SkillStateInfo();
        //    sinfo.name="follow";
        //    sinfo.starttime=effect.followTime[i*2+0];
        //    sinfo.duration=effect.followTime[i*2+1];
        //    sinfo.bAdded = false;
        //    stateinfos.Add(sinfo);
        //}
		
        //for(int i=0; i<effect.woundTime.Length/2; i++)
        //{
        //    SkillStateInfo sinfo=new SkillStateInfo();
        //    sinfo.name="wound";
        //    sinfo.starttime=effect.woundTime[i*2+0];
        //    sinfo.duration=effect.woundTime[i*2+1];
        //    sinfo.bAdded = false;
        //    stateinfos.Add(sinfo);
        //}

        //float completeTime = effect.completeTime;
		
        //if (effect.bCameraEffect!=0) 
        //{
        //    SkillStateInfo sinfo=new SkillStateInfo();
        //    sinfo.name="restore";
        //    sinfo.starttime=completeTime;
        //    sinfo.duration=sc.fCameraRestoreTime;
        //    sinfo.bAdded = false;
        //    stateinfos.Add(sinfo);

        //    completeTime += sc.fCameraRestoreTime;
        //}
		
        //{
        //    float fReturnDuration=sc.fReturnTime;
        //    if (effect.runTime [1]<=0)
        //    {
        //        fReturnDuration=0.0f;
        //    }

        //    SkillStateInfo sinfo=new SkillStateInfo();
        //    sinfo.name="return";
        //    sinfo.starttime=completeTime;
        //    sinfo.duration=fReturnDuration;
        //    sinfo.bAdded = false;
        //    stateinfos.Add(sinfo);

        //    completeTime += fReturnDuration;
        //}


        //{
        //    SkillStateInfo sinfo=new SkillStateInfo();
        //    sinfo.name="complete";
        //    sinfo.starttime=completeTime;
        //    sinfo.duration=0.01f;
        //    sinfo.bAdded = false;
        //    stateinfos.Add(sinfo);
        //}

	}

	List<string> skilllist=null;
	string[] skilllistex=null;

	//游戏控制
	void DrawTitle()
	{
		EditorGUILayout.BeginVertical ();

        //EditorGUILayout.BeginHorizontal (GUILayout.Width(500));
        //float fOldSpeed = (float)BattleScene.nGameSpeed;
        //float fNewSpeed = EditorGUILayout.Slider ("游戏速度",fOldSpeed, -10, 10);
        //BattleScene.nGameSpeed = (int)fNewSpeed;
        //BattleScene.fTimeFactor = 1.0f / Mathf.Pow (2.0f, fNewSpeed);
        //EditorGUILayout.EndHorizontal ();

		EditorGUILayout.BeginHorizontal (GUILayout.Width(500));
		//暂停继续
        //if (sc != null) 
        //{
        //    sc.fSkillTime=EditorGUILayout.Slider ("当前技能时间",sc.fSkillTime, -5, 10);
        //    if (sc.IsPause())
        //    {
        //        if (GUILayout.Button ("resume"))
        //        {
        //            sc.Resume();
        //        }
        //    }
        //    else
        //    {
        //        if (GUILayout.Button ("pause"))
        //        {
        //            sc.Pause();
        //        }
        //    }
        //}
		EditorGUILayout.EndHorizontal ();


		EditorGUILayout.Space ();


        //if (sc.IsPlaying())//正在放技能时不允许修改
        //{
        //    nSkillID=sc.GetSkillID();
        //    EditorGUILayout.BeginHorizontal (GUILayout.Width(500));
        //    int nNewValue = EditorGUILayout.IntField ("SkillID", nSkillID);
        //    EditorGUILayout.EndHorizontal ();
        //}
        //else
		{
			GUILayout.BeginHorizontal ("Box",GUILayout.Width(600));

			GUILayout.BeginHorizontal ("Box",GUILayout.Width(150));
			int nOldValue = nSkillID;
			int nNewValue = EditorGUILayout.IntField ("EffectViewID:", nOldValue);
			if (nNewValue != nOldValue) 
			{
				nSkillID=nNewValue;
			}
			GUILayout.EndHorizontal ();

			GUILayout.BeginHorizontal ("Box",GUILayout.Width(150));
            //if (CSkillManager.Instance != null) 
            //{
            //    if (skilllist==null)
            //    {
            //        skilllist=new List<string>();
            //        for(int i=1;i<200;i++)
            //        {
            //            CSkillData data = CSkillManager.Instance.GetSkillData (i);
            //            if(data!=null)
            //            {
            //                string title=i.ToString()+":"+data.m_strName;
            //                skilllist.Add(title);
            //            }
            //        }
            //        skilllistex= skilllist.ToArray();
            //    }
            //    int oldskillselect=nSkillID-1;
            //    int newskillselect=EditorGUILayout.Popup("技能选择:",oldskillselect,skilllistex);
            //    if (newskillselect!=oldskillselect)
            //    {
            //        string[] splitstrings=skilllistex[newskillselect].Split(':');
            //        nSkillID=int.Parse(splitstrings[0]);
            //    }
            //}
			GUILayout.EndHorizontal ();

			GUILayout.BeginHorizontal ("Box",GUILayout.Width(150));
			if (GUILayout.Button ("释放"))
			{
				SimulateFight();
			}
			GUILayout.EndHorizontal ();

			GUILayout.BeginHorizontal ("Box",GUILayout.Width(150));
			GUILayout.Button ("Save");
			GUILayout.EndHorizontal ();

			
			GUILayout.EndHorizontal ();
		}

		EditorGUILayout.Space ();

		EditorGUILayout.EndVertical ();
	}

	void SimulateFight()
	{
        //GameObject battlecameraobj=GameObject.Find("BattleCamera");
        //HeroController hc=battlecameraobj.GetComponent<HeroController>();
        //List<Hero> srcs=new List<Hero>();
        //List<Hero> dests=new List<Hero>();
        //srcs.Add(hc.GetCurrentEntity());
        //GameObject battleunitobjects=BattleUnitManager.Instance.gameObject;
        //for(int i=0;i<battleunitobjects.transform.childCount;i++)
        //{
        //    GameObject childobj=battleunitobjects.transform.GetChild(i).gameObject;
        //    CEntityView EntityView=childobj.GetComponent<CEntityView>();
        //    if (EntityView == null) {
        //        continue;
        //    }
        //    Hero Entity = EntityView.GetEntity ();
        //    if (Entity == null) {
        //        continue;
        //    }
        //    if (BattleClientHelp.IsEnemy (Entity.UID)) 
        //    {
        //        dests.Add(Entity);
        //    }
        //}

        //List<List<SBattleHurt>> hurtss=new List<List<SBattleHurt>>();
        //List<List<int>> suckss=new List<List<int>>();
        //for (int i=0; i<srcs.Count; i++) 
        //{
        //    List<SBattleHurt> hurts=new List<SBattleHurt>();
        //    List<int> sucks=new List<int>();
        //    for (int j=0; j<dests.Count; j++) 
        //    {
        //        SBattleHurt hurt=new SBattleHurt();
        //        hurt.bCirt=(UnityEngine.Random.Range(0,1)!=0);
        //        hurt.nHurt=UnityEngine.Random.Range(0,100);
        //        hurts.Add(hurt);
        //        int suck=UnityEngine.Random.Range(0,10);
        //        sucks.Add(suck);
        //    }
        //    hurtss.Add(hurts);
        //    suckss.Add(sucks);
        //}

        //Debug.Log ("SimulateFight");

        //sc.Attack (srcs, dests, hurtss, suckss, nSkillID*1000+1, (int)ESkillStateFlag.All);

	}
	
	// 绘制分页按钮
	void DrawTabBtn()
	{
		string[] pops = new string[3]{"技能数据","编辑技能曲线","其他"};
		GUILayout.BeginVertical (GUILayout.Width(90));
		GUILayout.BeginHorizontal ("Box");
		curTab = GUILayout.SelectionGrid (curTab, pops, 3);
		GUILayout.EndHorizontal ();
		GUILayout.EndVertical ();
	}




//	// 绘制游戏状态
//	void DrawSkillState()
//	{
//		EditorGUILayout.BeginHorizontal ();
//		EditorGUILayout.LabelField ("当前技能id:", skillcontrol.m_skillId.id.ToString ());
//		EditorGUILayout.LabelField ("技能已进行时间:", skillcontrol.m_fStartTime.ToString ());
//		EditorGUILayout.EndHorizontal ();
//		for (int i=0; i<skillcontrol.m_CurStates.Count; i++) 
//		{
//			GUILayout.BeginVertical();
//			SkillState state = skillcontrol.m_CurStates[i];
//			GUILayout.Label (state.GetStateType().ToString() + "\t持续时间:" + state.m_duration);
//			float percent = 0;
//			if(state.m_duration>0)
//			{
//				percent = state.m_currentTime/state.m_duration;
//			}
//			GUILayout.HorizontalSlider(percent, 0, 1);
//			GUILayout.EndVertical();
//		}
//	}
//

}