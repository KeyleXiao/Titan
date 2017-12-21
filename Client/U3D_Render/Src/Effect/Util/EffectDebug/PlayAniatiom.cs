/*******************************************************************
** 文件名:	PlayAniatiom.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:	播放动画，只给美术做调试用
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using  Effect;
using System.Reflection;

public class PlayAniatiom : MonoBehaviour {

    public static System.Action<bool> PauseEditor = null;

    [System.Serializable]
    public class AnimationNode
    {
        public string AnimationName = "小强很帅"; 
        public bool AlwaysPlay = false;
        public List<EffectNode> effectList = new List<EffectNode>();
        public List<AudioSource> audioList = new List<AudioSource>();
    }

    /// <summary>
    /// Aniamtor
    /// </summary>
    [HideInInspector]
    public Animator[] m_Animator;
    /// <summary>
    /// 动画的父节点名字，与Aminator一一对应，用来循环播放多个动画，比如弓箭手的动画和弓的动画，有两个Animator，并且他们的动画名都是XXX@Dead，是一一对应的，所有这个属性就填他们对应的XXX@，播动画时候是PatentName+Dead
    /// </summary>
    [HideInInspector]
    public string[] ParentName;

    public List<AnimationNode> animationNode;

    //public GameObject []Effects;
    public bool ShowButtons = true;
    public Transform MoveObject = null;
    private Transform LastMoveObject = null;
    public float DelayStartMove = 0.0f;
    public float MoveSpeed = 0.0f;
    public float MoveTime = 0.0f;
    public float DelayRestPosTime = 0.1f;

    public float Xoffset = 0.0f;

    [HideInInspector]
    public string[] AnimationName;
    public bool PausedWhenClickButton = false;
    public bool StopEffectWhenResetPosition = false;
    public int buttonHeight = 20;
    public int buttonWidth = 120;

    int Rows = 0;
    //EffectNode []mEffectNode = null;

    /// <summary>
    /// 是否在延迟重置位置
    /// </summary>
    bool isOnDelayRestPosTime = false;

    /// <summary>
    /// 是否在延迟开始位移
    /// </summary>
    bool isOnDelayStartMove = false;


    /// <summary>
    /// 是否可以移动
    /// </summary>
    bool isCanMove = false;

    /// <summary>
    /// 是否可以移动
    /// </summary>
    float CurrMoveTime = 0.0f;

    bool isEmeny = false;
    float SizeScale = 1.0f;
    public bool XPCamera = false;
    public AnimationClip XPClip = null;
    MouseOrbit mouseOrbitInstance = null;
    
    Vector3 OrginalPosition;

    public void CopyAnimationNode(List<AnimationNode> ani)
    {
        animationNode.Clear();
        animationNode.AddRange(ani);
    }

	void Start () 
    {
        mouseOrbitInstance = GameObject.FindObjectOfType<MouseOrbit>();
        //m_Animator = GetComponent<Animator>();

        //if (Effects.Length > 0)
        //{
        //    mEffectNode = new EffectNode[Effects.Length];
        //    for (int i = 0; i < Effects.Length; i++)
        //    {
        //        mEffectNode[i] = Effects[i].GetComponent<EffectNode>();
        //        if (!mEffectNode[i])
        //        {
        //            Debug.LogError(Effects[i].name + ",没有挂EffectNode脚本！");
        //            return;
        //        }
        //        mEffectNode[i].Init();
        //    }

        //}
        //else
        //{
        //    Debug.LogWarning("Effects为空，请添加特效，再进入play模式！");
        //}

        if (MoveObject)
        {
            OrginalPosition = MoveObject.position;
            LastMoveObject = MoveObject;
        }

        ImageSetting.SetImageQuality(ImageQuality.Best);
	}

    void OnGUI()
    {
        if (!ShowButtons)
        {
            return;
        }
        int xindex = -1;
        int yindex = 0;
        Rows = Screen.height / buttonHeight;
        for (int i = 0; i < AnimationName.Length; i++)
        {
            string []s = AnimationName[i].Split('@');
            string tempname = s[s.Length - 1];
            yindex = i % Rows;
            if (i % Rows == 0)
            {
                xindex++;
            }
            if (string.IsNullOrEmpty(AnimationName[i]))
            {
                continue;
            }
            if (GUI.Button(new Rect(xindex * buttonWidth + Xoffset, yindex * buttonHeight, buttonWidth, buttonHeight), tempname))
            {
                for (int j = 0; j < m_Animator.Length; j++)
                {
                    m_Animator[j].Play( AnimationName[i]);
                }

                if(IsEnabledXP())
                {
                    PlayCameraAnim(XPClip);
                }

                if (animationNode.Count > 0)
                {
                    for (int j = 0; j < animationNode.Count; j++)
                    {
                        AnimationNode node = animationNode[j];
                        //去掉空格
                        string CorrectName = tempname.Trim();
                        string CorrectName2 = node.AnimationName.Trim();
                        if (CorrectName.Equals(CorrectName2) || node.AlwaysPlay)
                        {
                            foreach(EffectNode n in node.effectList)
                            {
                                if(n)
                                {
                                    bStartTime = true;
                                    timede = 0.0f;
                                    n.Init();
                                    n.StopEffect();
                                    n.ResetScale();
                                    n.PlayEffect(isEmeny ,- 1, Vector3.zero, Vector3.zero,(int)EffectNodeMask.Mask_EnableLihgt);
                                    n.SetScale(SizeScale);
                                }
                            }

                            foreach(AudioSource aud in node.audioList)
                            {
                                if(aud)
                                {
                                    aud.Stop();
                                    aud.Play();
                                }
                            }
                        }

                    }

                    if (MoveSpeed > 0.0001f && MoveTime > 0.0001f)
                    {
                        if (DelayStartMove > 0.01)
                        {
                            if (!isOnDelayStartMove)
                            {
                                isOnDelayStartMove = true;
                                StartCoroutine(WaitForDelayStartMove());
                            }
                            
                        }
                        else
                        {
                            isCanMove = true;
                            CurrMoveTime = MoveTime;
                        }

                    }
                    
                }
                if (PauseEditor != null)
                {
                    PauseEditor(PausedWhenClickButton);
                }
            }
        }

        GUI.Label(new Rect(Screen.width - 100.0f,0,80,20), "时间:" + timede.ToString("F2"));

        Color c = GUI.color;

        GUI.enabled = ImageSetting.GetData().currImageQuality != ImageQuality.Fastest;
        GUI.color = GUI.enabled ? c : Color.red;
        if(GUI.Button(new Rect(Screen.width - 100.0f,20,80,20),"低等画质"))
        {
            ImageSetting.SetImageQuality(ImageQuality.Fastest);
        }
        GUI.enabled = true;
        GUI.color = c;

        GUI.enabled = ImageSetting.GetData().currImageQuality != ImageQuality.Good;
        GUI.color = GUI.enabled ? c : Color.red;
        if (GUI.Button(new Rect(Screen.width - 100.0f, 40, 80, 20), "中等画质"))
        {
            ImageSetting.SetImageQuality(ImageQuality.Good);
        }
        GUI.enabled = true;
        GUI.color = c;

        GUI.enabled = ImageSetting.GetData().currImageQuality != ImageQuality.High;
        GUI.color = GUI.enabled ? c : Color.red;
        if (GUI.Button(new Rect(Screen.width - 100.0f, 60, 80, 20), "高等画质"))
        {
            ImageSetting.SetImageQuality(ImageQuality.High);
        }
        GUI.enabled = true;
        GUI.color = c;


        GUI.enabled = ImageSetting.GetData().currImageQuality != ImageQuality.Best;
        GUI.color = GUI.enabled ? c : Color.red;
        if (GUI.Button(new Rect(Screen.width - 100.0f, 80, 80, 20), "极高画质"))
        {
            ImageSetting.SetImageQuality(ImageQuality.Best);
        }
        GUI.enabled = true;
        GUI.color = c;

        isEmeny = GUI.Toggle(new Rect(Screen.width - 100.0f, 100, 80, 20), isEmeny, "敌方技能");

        SizeScale = GUI.HorizontalSlider(new Rect(Screen.width - 100.0f, 120, 80, 20), SizeScale, 0.01f, 10.0f);
    }
    float timede = 0.0f;
    bool bStartTime = false;

    #region XP
    bool bPlayingCameraAnim = false;
    float cameraAnimStopTick = 0.0f;
    Transform camRecTrans;
    Transform camAnimTrans;
    Transform camFOVTrans;
    float currTick = 0.0f;

    Vector3 orginalCameraPos;
    Quaternion orginalCamerRot;
    float orginalCamerFOV;
    bool IsEnabledXP()
    {
       return XPCamera && XPClip;
    }

    public void PlayCameraAnim(AnimationClip clip)
    {
        if (bPlayingCameraAnim)
        {
            StopCameraAnim();
        }

        orginalCameraPos = Camera.main.transform.position;
        orginalCamerRot = Camera.main.transform.rotation;
        orginalCamerFOV = Camera.main.fieldOfView;
        
        GameObject camGo = new GameObject("CameraAnimRef");
        camRecTrans = camGo.transform;
        GameObject animGo = new GameObject("Anim");
        animGo.transform.parent = camRecTrans;
        camAnimTrans = animGo.transform;
        GameObject camFOVGo = new GameObject("camFOV");
        camFOVTrans = camFOVGo.transform;
        camFOVTrans.parent = animGo.transform;

        camRecTrans.position = transform.position;
        camRecTrans.eulerAngles = transform.eulerAngles;

        Animation anim = animGo.AddComponent<Animation>();
        clip.legacy = true;
        anim.AddClip(clip, clip.name);
        anim.Play(clip.name);

        currTick = 0.0f;
        cameraAnimStopTick = clip.length;

        bPlayingCameraAnim = true;
    }


    public void StopCameraAnim()
    {
        if (!bPlayingCameraAnim)
        {
            return;
        }
        if (camRecTrans)
        {
            GameObject.Destroy(camRecTrans.gameObject);
        }
        Camera.main.transform.position = orginalCameraPos;
        Camera.main.transform.rotation = orginalCamerRot;
        Camera.main.fieldOfView = orginalCamerFOV;
        bPlayingCameraAnim = false;
        currTick = 0.0f;
    }

    private void updateCameraAnim()
    {
        if (!bPlayingCameraAnim)
        {
            return;
        }
        Camera.main.transform.position = camAnimTrans.position;
        Vector3 angle = camAnimTrans.eulerAngles;
        Camera.main.transform.eulerAngles = angle;
        float focal = camFOVTrans.localScale.z;
        float fov = Mathf.Atan(30.0f / focal) / Mathf.PI * 180.0f;
        Camera.main.fieldOfView = fov;

        currTick += Time.deltaTime;
        if (currTick > cameraAnimStopTick)
        {
            StopCameraAnim();
        }
    }

    #endregion
    void LateUpdate()
    {
        if(mouseOrbitInstance)
        {
            mouseOrbitInstance.enabled = !IsEnabledXP();
        }

        if(IsEnabledXP())
        {
            updateCameraAnim();
        }
        if(bStartTime)
        {
            timede += Time.deltaTime;
            if (timede > 100)
            {
                timede = 0;
            }

        }

        if (LastMoveObject != MoveObject)
        {
            OrginalPosition = MoveObject.position;
            LastMoveObject = MoveObject;
        }
        if (isCanMove && MoveObject)
        {
            if (!isOnDelayRestPosTime)
            {
                if (CurrMoveTime >= 0)
                {
                    
                    MoveObject.Translate(transform.forward * MoveSpeed * Time.deltaTime);
                    CurrMoveTime -= Time.deltaTime;
                }
                else
                {
                    isCanMove = false;
                    isOnDelayRestPosTime = true;
                    StartCoroutine(WaitForDelayRestPosTime());
                }
            }
        }
    }

    IEnumerator WaitForDelayStartMove()
    {
        yield return new WaitForSeconds(DelayStartMove);
        isCanMove = true;
        CurrMoveTime = MoveTime;
    }

    IEnumerator WaitForDelayRestPosTime()
    {
        yield return new WaitForSeconds(DelayRestPosTime);
        isOnDelayRestPosTime = false;
        isOnDelayStartMove = false;
        if (MoveObject)
        {
            MoveObject.position = OrginalPosition;
        }
        
        if (StopEffectWhenResetPosition)
        {
            if (animationNode.Count > 0)
            {
                for (int j = 0; j < animationNode.Count; j++)
                {
                    AnimationNode node = animationNode[j];
                    foreach (EffectNode n in node.effectList)
                    {
                        if (n)
                        {
                            n.StopEffect();
                        }
                    }
                }
            }
        }
    }

    void StartAttack()
    {

    }
    void ExitAttack()
    {

    }

    void BindLighting(int ID)
    {

    }                        
}
