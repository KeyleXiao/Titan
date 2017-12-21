///*******************************************************************
//** 文件名:	HeroSelectStateMachine.cs
//** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
//** 创建人:	谭强均
//** 日  期:	2015/3/20
//** 版  本:	1.0
//** 描  述:	英雄选择界面状态机
//** 应  用:  用于控制英雄选择界面英雄的状态机

//**************************** 修改记录 ******************************/

//using UnityEngine;
//using System.Collections;
//using System.Collections.Generic;
//public class HeroSelectStateMachine : MonoBehaviour
//{
//    Animator m_Animator;
//    Transform m_transform;

//    public const string MoveFornt = "前进";
//    public const string MoveBack = "后退";
//    public const string MoveLeft = "左移";
//    public const string MoveRight = "右移";
//    // Use this for initialization

//    int m_HeroID = 0;

//    List<string> AniName = new List<string>();
//    List<float> AniLength = new List<float>();

//    string earlyCallAniName = "";
//    float RoateSpeed = 4320.0f;

//    float currAnimationDuration = 0.0f;
//    float AnimationDurationTimer = 0.0f;
//    Vector3 MouseRot;

//    public void Start()
//    {
//        m_Animator = GetComponent<Animator>();
//        m_transform = transform;

//        //PlayAnimation(earlyCallAniName);
//        //StartCoroutine(paaaa());
//    }

//    public void Update()
//    {
//        if (InputManager.GetMouseButton(0))
//        {
//            float y = -InputManager.GetAxis(InputAxisSet.Axis_MouseX);
//            MouseRot.y += y * RoateSpeed * Time.deltaTime;
//        }
//        transform.localPosition = HeroSelectScene.configInstance.SelectedPos;
//        transform.localEulerAngles = HeroSelectScene.configInstance.GetHeroRot(m_HeroID) + MouseRot;

//        if (currAnimationDuration <= 0)
//        {
//            return;
//        }

//        if (AnimationDurationTimer > currAnimationDuration)
//        {
//            currAnimationDuration = -1.0f;
//            ResetAnimation();
//            return;
//        }
//        AnimationDurationTimer += Time.deltaTime;

//    }

//    //IEnumerator paaaa()
//    //{
//    //    while (true)
//    //    {
//    //        PlayRandomAnimation();
//    //        yield return new WaitForSeconds(3.0f);
//    //    }
//    //}
//    public void PlayRandomAnimation()
//    {
//        string aniName = AniName[UnityEngine.Random.Range(0, AniName.Count)];
//        PlayAnimation(aniName);
//    }

//    public void Init(int HeroID)
//    {
//        m_HeroID = HeroID;
//        Dictionary<int ,List<string>> AniNameTable = new Dictionary<int,List<string>>();
//        Dictionary<int, List<string>> AniLengthTable = new Dictionary<int, List<string>>();
//        HeroSelectScene.configInstance.DecodeData(AniNameTable, AniLengthTable);

        
//        if (AniNameTable.ContainsKey(m_HeroID))
//        {
//            AniName.AddRange(AniNameTable[m_HeroID]);
//            AniLength.AddRange(AniLengthTable[m_HeroID].ConvertAll<float>((string s) =>
//                {
//                    float temp = 0;
//                    if (!float.TryParse(s, out temp))
//                    {
//                        return -1;
//                    }
//                    return temp;
//                }));
//        }
//        AniNameTable = null;
//        AniLengthTable = null;
//    }

//    void PlayAnimation(string name)
//    {
//        if (string.IsNullOrEmpty(name))
//        {
//            return;
//        }
//        if (AniName.Count <= 0)
//        {
//            return;
//        }
//        if (!AniName.Contains(name))
//        {
//            return;
//        }

//        if (!m_Animator)
//        {
//            earlyCallAniName = name;
//            return;
//        }

//        currAnimationDuration = AniLength[AniName.IndexOf(name)];
//        AnimationDurationTimer = 0.0f;
//        if (name.Equals(MoveFornt))
//        {
//            m_Animator.SetFloat("moveforward", 1.0f);
//            m_Animator.SetFloat("moveright", 0.0f);
//            return;
//        }

//        if (name.Equals(MoveBack))
//        {
//            m_Animator.SetFloat("moveforward", -1.0f);
//            m_Animator.SetFloat("moveright", 0.0f);
//            return;
//        }

//        if (name.Equals(MoveLeft))
//        {
//            m_Animator.SetFloat("moveforward", 0.0f);
//            m_Animator.SetFloat("moveright", -1.0f);
//            return;
//        }

//        if (name.Equals(MoveRight))
//        {
//            m_Animator.SetFloat("moveforward", 0.0f);
//            m_Animator.SetFloat("moveright", 1.0f);
//            return;
//        }

//        for (int i = 0; i < m_Animator.layerCount; i++)
//        {
//            if (m_Animator.HasState(i, Animator.StringToHash(name)))
//            {
//                m_Animator.Play(name);
//                break;
//            }
//        }
//    }

//    void ResetAnimation()
//    {
//        if (m_Animator)
//        {
//            m_Animator.SetFloat("moveforward", 0.0f);
//            m_Animator.SetFloat("moveright", 0.0f);
//        }
//    }
//}
