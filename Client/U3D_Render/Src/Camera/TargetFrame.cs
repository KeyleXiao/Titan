/*******************************************************************
** 文件名:	TargetFrame.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/1/5
** 版  本:	1.0
** 描  述:	用于显示当前攻击对象
** 应  用:  	攻击对象选定时显示该锁定框

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;

public class TargetFrame : MonoBehaviour
{
    //private GameObject TL;
    //private GameObject TR;
    //private GameObject BL;
    //private GameObject BR;
    //private Transform MyParent;

    public struct SelectTargetParam
    {
        public int nTargetId;
        public bool bIngoreBossMark;
    }

    private Transform myTrans;    
    public Transform targetTrans;
    public CharacterController TargetController;
    //private MeshFilter MyMeshFilter ;
    //private bool IsAttachTo = false;

    public float BillboardScale = 0.5f;

    public float WidthTarget = 50.0f;

    public static System.Action<SelectTargetParam> OnSelectedTarget = null;
    public static System.Action<int> OnDisSelectedTarget = null;

	// Use this for initialization
    public void Start()
    {
        //gameObject.camera.enabled = false;
        myTrans = transform;
        //MyMeshFilter = GetComponent<MeshFilter>();
       

        //TL.gameObject.SetActive(false);
        //TR.gameObject.SetActive(false);
        //BL.gameObject.SetActive(false);
        //BR.gameObject.SetActive(false);
	}
	
	// Update is called once per frame
    public void Update() 
    {
        //画瞄准框的，暂时废弃
        //if (IsAttachTo && targetTrans !=null )
        //{
        //    if (targetTrans.GetComponentInParent<BaseStateMachine>() == null || targetTrans.GetComponentInParent<BaseStateMachine>().transform==null)
        //    {
        //        Debug.LogError("TargetFrame.update can't find BaseStateMachine");
        //        return;
        //    }

        //    if (targetTrans.GetComponentInParent<BaseStateMachine>().transform.collider != null)
        //    {
        //        PositionUpdate(targetTrans.GetComponentInParent<BaseStateMachine>().transform);
        //    }
        //    else
        //    {
        //        PositionUpdate(targetTrans);
        //    }

        //    //ReCalBillboardSize();
        //}
	}

    //public void DisplayFrame(RaycastHit currentCastInfo)
    //{
    //    if (TL == null || TR == null || BL == null || BR == null)
    //    {
    //        Trace.LogError("TL or TR or BL or BR is null");
    //        return;
    //    }


    //    Vector3 boundScreenMaxPoint = Initialize.mainCam.WorldToScreenPoint(currentCastInfo.collider.bounds.max);
    //    Vector3 boundScreenMinPoint = Initialize.mainCam.WorldToScreenPoint(currentCastInfo.collider.bounds.min);

    //    //Screen
    //    //Top Left
    //    Vector3 ScreenTLP = new Vector3(boundScreenMinPoint.x, boundScreenMaxPoint.y);
    //    //Top Right
    //    Vector3 ScreenTRP = new Vector3(boundScreenMaxPoint.x, boundScreenMaxPoint.y);
    //    //Bottom Left 
    //    Vector3 ScreenBLP = new Vector3(boundScreenMinPoint.x, boundScreenMinPoint.y);
    //    //Bottom Right
    //    Vector3 ScreenBRP = new Vector3(boundScreenMaxPoint.x, boundScreenMinPoint.y);

    //    //World
    //    Vector3 WorldTLP = Initialize.mainCam.ScreenToWorldPoint(ScreenTLP);
    //    Vector3 WorldTRP = Initialize.mainCam.ScreenToWorldPoint(ScreenTRP);
    //    Vector3 WorldBLP = Initialize.mainCam.ScreenToWorldPoint(ScreenBLP);
    //    Vector3 WorldBRP = Initialize.mainCam.ScreenToWorldPoint(ScreenBRP);

    //    //TargetFrame GameObject
    //    Vector3 TargetTLP = WorldTLP;// myTrans.InverseTransformPoint(WorldTLP);
    //    Vector3 TargetTRP = WorldTRP;// myTrans.InverseTransformPoint(WorldTRP);
    //    Vector3 TargetBLP = WorldBLP;// myTrans.InverseTransformPoint(WorldBLP);
    //    Vector3 TargetBRP = WorldBRP;// myTrans.InverseTransformPoint(WorldBRP);

    //    //set point 
    //    TL.transform.SetPosition( TargetTLP;
    //    TR.transform.SetPosition( TargetTRP;
    //    BL.transform.SetPosition( TargetBLP;
    //    BR.transform.SetPosition( TargetBRP;

    //    //gameObject.camera.enabled = true;

    //    //Is Invoking == true => reset
    //    if (IsInvoking("HideFrame"))
    //    {
    //        CancelInvoke("HideFrame");
    //        Invoke("HideFrame", 1.0f);
    //    }
    //}



    /// <summary>
    /// 在ScreenRayCast 里调用
    /// </summary>
    /// <param name="CastObject">攻击选定对象</param>
    public void AttachToTarget(Transform targetTransform)
    {
        if (targetTrans != null)
        {
            DisAttachToTarget();
        }

        targetTrans = targetTransform;
        BaseStateMachine machine = targetTrans.GetComponentInParent<BaseStateMachine>();
        if (machine != null)
        {
            machine.outLineCount = machine.outLineCount+1;
            if (null != OnSelectedTarget)
            {
                SelectTargetParam param;
                param.nTargetId = machine.entityID;
                param.bIngoreBossMark = false;
                OnSelectedTarget(param);
            }
            //GfxBloodManager.ChangeBossBloodInfo(machine.entityID);

            
            
            DataCenter.LogicDataCenter.frontSightDataManager.FrontSightColor = machine.outlineColor;
            
            
        }



        //IsAttachTo = true;


        //画瞄准框的，暂时废弃
        //Bounds ControllerBounds;
        //targetTrans = CastObject.transform;
        //float disWidth = 0;
        //float disHeight = 0;

        //if (targetTrans.GetComponentInParent<BaseStateMachine>() != null)
        //    TargetController = targetTrans.GetComponentInParent<BaseStateMachine>().GetComponent<CharacterController>();//targetTrans.parent.GetComponent<CharacterController>();


        ////判断对象是否有Character Controller 碰撞盒
        //if (TargetController!= null)
        //{
        //   // Debug.LogError("Use Target Controller bounds");
        //    ControllerBounds = TargetController.bounds;


        //    disWidth = ControllerBounds.size.x;
        //    disHeight = ControllerBounds.size.y;
        //    //disWidth = CastObject.collider.bounds.size.x;
        //    //disHeight = CastObject.collider.bounds.size.y;
        //}
        //else
        //{
        //    ControllerBounds = CastObject.collider.bounds;

        //    Vector3 bounds_00 = new Vector3(ControllerBounds.min.x, ControllerBounds.max.y, ControllerBounds.min.z);
        //    Vector3 bounds_01 = new Vector3(ControllerBounds.max.x, ControllerBounds.max.y, ControllerBounds.min.z);
        //    Vector3 bounds_03 = new Vector3(ControllerBounds.min.x, ControllerBounds.max.y, ControllerBounds.max.z);
        //    Vector3 bounds_07 = ControllerBounds.min;
        //    Vector3 bounds_04 = new Vector3(ControllerBounds.min.x, ControllerBounds.min.y, ControllerBounds.max.z);


        //    //计算碰撞盒当前的宽高
        //    if (Mathf.Abs(bounds_00.x - bounds_01.x) > disWidth)
        //        disWidth = Mathf.Abs(bounds_00.x - bounds_01.x);
        //    if (Mathf.Abs(bounds_00.x - bounds_03.x) > disWidth)
        //        disWidth = Mathf.Abs(bounds_00.x - bounds_03.x);

        //    if (Mathf.Abs(bounds_00.y - bounds_07.y) > disHeight)
        //        disHeight = Mathf.Abs(bounds_00.y - bounds_07.y);
        //    if (Mathf.Abs(bounds_00.y - bounds_04.y) > disHeight)
        //        disHeight = Mathf.Abs(bounds_00.y - bounds_04.y);
        //}



        //disWidth *= BillboardScale;
        //disHeight *= BillboardScale;

        //CreateBillboardMesh(disWidth, disHeight);

        ////MyParent = myTrans.parent;

        ////myTrans.localPosition = Vector3.zero;

        //IsAttachTo = true;

    }

    public void DisAttachToTarget()
    {
        if (targetTrans != null)
        {
            BaseStateMachine machine = targetTrans.GetComponentInParent<BaseStateMachine>();
            if (machine != null)
            {
                machine.outLineCount = machine.outLineCount - 1;
                if (null != OnDisSelectedTarget)
                {
                    OnDisSelectedTarget(machine.entityID);
                }
                //GfxBloodManager.ChangeBossBloodInfo(-1);
            }
        }
        
        DataCenter.LogicDataCenter.frontSightDataManager.FrontSightColor = Color.white;

        myTrans.parent = null;
        //MyParent = null;
        //MyMeshFilter.mesh = null;
        TargetController = null;
        targetTrans = null;
        //IsAttachTo = false;
    }

    ///// <summary>
    ///// 创建锁定框的公告板 片面
    ///// </summary>
    ///// <param name="width"></param>
    ///// <param name="height"></param>
    //void CreateBillboardMesh(float width, float height)
    //{
    //    Mesh mesh = new Mesh();
    //    MyMeshFilter.mesh = mesh;
        

    //    //Vertices
    //    Vector3[] veritices = new Vector3[4];
    //    veritices[0] = new Vector3(-width/2.0f, -height/2.0f, 0);
    //    veritices[1] = new Vector3(width/2.0f, -height/2.0f, 0);
    //    veritices[2] = new Vector3(-width / 2.0f, height/2.0f, 0);
    //    veritices[3] = new Vector3(width/2.0f, height/2.0f, 0); 
    //    mesh.vertices = veritices;

    //    //triangles
    //    int[] tri = new int[6];
    //    tri[0] = 0;
    //    tri[1] = 1;// 2;
    //    tri[2] = 2;// 1;

    //    tri[3] = 2;
    //    tri[4] = 1;// 3;
    //    tri[5] = 3;// 1;
    //    mesh.triangles = tri;

    //    //normal
    //    Vector3[] normals = new Vector3[4];
    //    normals[0] = Vector3.forward;
    //    normals[1] = Vector3.forward;
    //    normals[2] = Vector3.forward;
    //    normals[3] = Vector3.forward;
    //    mesh.normals = normals;

    //    //uv
    //    Vector2[] uv = new Vector2[4];
    //    uv[0] = new Vector2(0, 0);
    //    uv[1] = new Vector2(1, 0);
    //    uv[2] = new Vector2(0, 1);
    //    uv[3] = new Vector2(1, 1);
    //    mesh.uv = uv;

    //}

    ////这种计算未涉及Target是否有parrent情况
    //void ReCalBillboardSize()
    //{
    //    if (MyMeshFilter.mesh == null)
    //        return;
        
    //    Vector3[] verts = new Vector3[MyMeshFilter.mesh.vertexCount];

    //    verts = MyMeshFilter.mesh.vertices;

    //    //centerPos = myTrans.parent.localToWorldMatrix * centerPos;
        
    //    Vector3 leftPos = myTrans.parent.position+myTrans.rotation*verts[0];
    //    Vector3 rightPos =  myTrans.parent.position+myTrans.rotation*verts[1];
    //    //Debug.Log("centerPos" + centerPos.ToString());
    //    //Debug.Log("LeftPos" + leftPos.ToString());
    //    //Debug.Log("rightPos" + rightPos.ToString());


    //    Vector3 vert0InScreen = Initialize.mainCam.WorldToScreenPoint(leftPos);
    //    Vector3 vert1InScreen = Initialize.mainCam.WorldToScreenPoint(rightPos);

    //    float VertScreenWidth = Mathf.Abs(vert0InScreen.x - vert1InScreen.x);

    //    if (VertScreenWidth > 0.0f)
    //    {
    //        float NewScale = WidthTarget / VertScreenWidth;
    //        Vector3 NewBillboardScale = new Vector3(NewScale, NewScale, NewScale);
    //        myTrans.localScale = NewBillboardScale;
    //    }
    //}

    ///// <summary>
    ///// 如果CastTarget 是ScreenRayCastObject的话，要拿父节点更新位置,所以把具体更新的部分抽出来
    ///// </summary>
    //void PositionUpdate(Transform currTransform)
    //{
    //    //Vector3 v = Initialize.mainCam.transform.position - currTransform.position;
    //    //v.x = v.z = 0.0f;
    //    //myTrans.LookAt(Initialize.mainCam.transform.position - v);
    //    float targetYOffset = 0;
    //    if(((currTransform.collider)as CharacterController) != null)
    //        targetYOffset = currTransform.collider.bounds.size.y / 2;

         
    //    Vector3 TargetPoint = Vector3.zero;

    //    //myTrans.SetPosition( currTransform.position;
    //    myTrans.LookAt(Initialize.mainCam.transform.position);

    //    Vector3 v = currTransform.position - Initialize.mainCam.transform.position;
    //    //v.x = Mathf.Sqrt(Mathf.Pow(v.magnitude, 2) - Mathf.Pow(Mathf.Abs(Initialize.mainCam.transform.position.y - currTransform.position.y), 2));
    //    //v.y = currTransform.position.y;
   
    //    float vMag = v.magnitude;
    //    v.Normalize();

    //    if (vMag < 10.0f)
    //    {
    //         if (vMag < 3.5f)
    //        {
    //            return;
    //        }
    //         myTrans.SetPosition( currTransform.position - v;// *2.0f;

    //    }
    //    else
    //    {
    //        if (vMag > 11.5f)
    //            targetYOffset = 0;
    //        myTrans.SetPosition( Initialize.mainCam.transform.position + v * 5.0f;
            
    //    }
    //    //根据Character Controller的高度调整偏移
    //    myTrans.position += targetYOffset * myTrans.up;
    //   // myTrans.localRotation = new Quaternion(0, myTrans.localRotation.y, 0, myTrans.localRotation.w);

    //}

    //public float TargetFrameTimer = 1;
    //private bool IsTargetFrameTimerBegin = false;
    ////开始定时器
    //private void BeginTargetTimer()
    //{
    //    if (!IsInvoking("HideFrame"))
    //    {
    //        Invoke("HideFrame", TargetFrameTimer);
    //        IsTargetFrameTimerBegin = true;
    //    }
    //}
    ////检测定时器是否结束
    //public bool IsTargetFrameTimer()
    //{
    //    bool res = false;
    //    if (!IsTargetFrameTimerBegin)
    //    {
    //        BeginTargetTimer();
    //        return false;
    //    }

    //    if (IsInvoking("HideFrame"))
    //    {
    //        res = false;
    //    }
    //    else
    //    {
    //        CancelInvoke("HideFrame");
    //        IsTargetFrameTimerBegin = false;

    //        res = true;
    //    }

    //    return res;
    //}
    // void HideFrame()
    //{
    //    Debug.Log("HideFrame");
    //   // gameObject.camera.enabled = false;
    //}
}

