using org.critterai.nav;
using System.IO;

//这里制作输出LOG用，寻路本身没用到U3D，这里的Vector3用了U3D的Vector3，如果是非U3D可以转换为nav自身的Vector3
using UnityEngine;

public class SceneNav
{    
    private Navmesh navmesh = null;
    private NavmeshQuery query;
    private NavmeshQueryFilter mQueryFilter;
    private CrowdManager mCrowdManager;
    private int mMaxQueryNodes = 2048;
    private int mMaxCrowdAgents = 20;
    private float mMaxAgentRadius = 0.5f;
    private float mTurnSpeed = 3.0f;
    private float mMoveSpeed = 3.0f;
    private float mMaxPointDis = 1.0f; //寻路点阈值，距离小于此值认为不需要再寻路
    private CrowdAgentParams mCrowdAgentParams;
    private CrowdAgent mCrowdAgent;
    private string fileName;

    public uint[] polyResult=null;
    public int polyResultCount=0;
    public Vector3[] pointResult = null;
    public int pointResultCount=0;
    private Vector3[] tileBufferPoints=null; //缓存tile的顶点
    private int tileBufferRef = -1;
    private Vector3[] pointResultBuffer = null; //缓存寻路的顶点
    private uint[] polyResultBuffer = null; //缓存寻路的三角形

    public bool loadNavMesh(string fileName)
    {
        NavStatus status = loadMeshData(fileName);
        if ((status & NavStatus.Sucess) == 0)
        {
            Debug.LogError(fileName + "Load failed" + status.ToString());
            mCrowdManager = null;
            return false;
        }        
        return true;
    }
    
    private NavStatus loadMeshData(string fileName)
    {
        Debug.Log("loadMesshDataBegin");
        byte[] serializedMesh = readFileByte(fileName);
        this.fileName = fileName;

        if (serializedMesh == null)
        {
            Debug.Log("serializedMesh == null");
            return NavStatus.Failure | NavStatus.InvalidParam;
        }

        // This roundabout method is used for validation.        
        NavStatus status = Navmesh.Create(serializedMesh, out navmesh);

        if ((status & NavStatus.Sucess) == 0)
        {
            Debug.Log("Navmesh.Create Failed");
            Debug.Log(status.ToString());
            return status;
        }

        //byte[] mDataPack = navmesh.GetSerializedMesh(); //转一遍测试一下

        //if (mDataPack == null)
        //{
        //    Debug.Log("navmesh.GetSerializedMesh() Failed");
        //    Debug.Log(mDataPack.ToString());
        //    return NavStatus.Failure;
        //}

        //mBuildInfo = buildConfig;    = null;

        Debug.Log("loadMesshDataSuccess");        

        return NavStatus.Sucess;

    }

    private byte[] readFileByte(string fileName)
    {
        Debug.Log("readFileByteBegin");
        FileStream pFileStream = null;
        byte[] pReadByte = new byte[0];
        try
        {
            pFileStream = new FileStream(fileName, FileMode.Open, FileAccess.Read);
            BinaryReader r = new BinaryReader(pFileStream);
            r.BaseStream.Seek(0, SeekOrigin.Begin);    //将文件指针设置到文件开
            pReadByte = r.ReadBytes((int)r.BaseStream.Length);
            Debug.Log("readFileByteSuccess");
            return pReadByte;
        }
        catch
        {
            Debug.Log("readFileByteFailed");
            return pReadByte;
        }
        finally
        {
            if (pFileStream != null)
                pFileStream.Close();
        }
    }

    public void resetSpeed(float fMoveSpeed, float fTurnSpeed)
    {
        this.mTurnSpeed = fTurnSpeed;
        this.mMoveSpeed = fMoveSpeed;
        mCrowdAgentParams.maxSpeed = this.mMoveSpeed;        
    }

    public void CreateQuery(float fMoveSpeed, float fTurnSpeed)
    {
        this.mTurnSpeed = fTurnSpeed;
        this.mMoveSpeed = fMoveSpeed;
        NavStatus status = NavmeshQuery.Create(navmesh, mMaxQueryNodes, out query);
        if ((status & NavStatus.Sucess) == 0)
        {
            Debug.LogError(
                fileName + ": Aborted initialization. Failed query creation: " + status.ToString());
            mCrowdManager = null;
            return;
        }

        mCrowdManager = CrowdManager.Create(mMaxCrowdAgents, mMaxAgentRadius, navmesh);
        if (mCrowdManager == null)
        {
            Debug.LogError(fileName + ": Aborted initialization. Failed crowd creation.");
        }

        CrowdAvoidanceParams mCrowdParam = CrowdAvoidanceParams.CreateStandardMedium();

        mCrowdManager.SetAvoidanceConfig(0, mCrowdParam);
        mCrowdAgentParams = new CrowdAgentParams();
        mCrowdAgentParams.avoidanceType = 0;
        mCrowdAgentParams.collisionQueryRange = 3.2f;
        mCrowdAgentParams.height = 1.8f;
        mCrowdAgentParams.maxAcceleration = 8.0f;
        mCrowdAgentParams.maxSpeed = this.mMoveSpeed;
        mCrowdAgentParams.pathOptimizationRange = 12.0f;
        mCrowdAgentParams.radius = 1.0f;
        mCrowdAgentParams.separationWeight = 2.0f;
        mCrowdAgentParams.updateFlags = CrowdUpdateFlags.AnticipateTurns | CrowdUpdateFlags.ObstacleAvoidance | CrowdUpdateFlags.CrowdSeparation | CrowdUpdateFlags.OptimizeVis | CrowdUpdateFlags.OptimizeTopo;

        polyResult=new uint[300];
        pointResult = new Vector3[300];
        tileBufferPoints = new Vector3[3000];
        pointResultBuffer = new Vector3[300];
        polyResultBuffer = new uint[300];
        NavmeshTile tile = navmesh.GetTile(0);
        int count = tile.GetVerts(tileBufferPoints);
        Debug.Log("Tile "+tile.GetTileRef()+" count:"+count);
        if (count > 3000)
        {
            tileBufferPoints = new Vector3[count];
        }
        tileBufferRef = -1;

        //NavmeshPoly[] polys=new NavmeshPoly[3000];
        //int polyCount;
        //polyCount=tile.GetPolys(polys);
        //for (int i = 0; i < polyCount; i++)
        //{
        //    NavmeshPoly poly = polys[i];
        //    //if (poly.Type == NavmeshPolyType.OffMeshConnection)
        //    {
        //        Debug.Log("Poly" + i+"type"+poly.Type.ToString());
        //    }
        //}
    }

    public bool StartNavMove(Vector3 startPos,Vector3 endPos)
    {
        if (mCrowdManager == null)
        {
            Debug.LogError(fileName + ": Aborted initialization. Failed crowd creation.");
            return false;
        }

       
        NavmeshPoint startPoint;
        NavmeshPoint endPoint;

        NavStatus status0 = query.GetNearestPoint(startPos, Vector3.one, mCrowdManager.QueryFilter, out startPoint);
        NavStatus status1 = query.GetNearestPoint(endPos, Vector3.one, mCrowdManager.QueryFilter, out endPoint);

        Debug.Log("Find path from" + startPoint.point.ToString() + "To" + endPoint.point.ToString());

        generateNavPath(startPoint, endPoint);
        return true;
    }

    public bool updateTransform(out Vector3 position, ref Quaternion rotation)
    {
        mCrowdManager.Update(Time.deltaTime);
        //Debug.Log("pos" + mCrowdAgent.Position);
        position=mCrowdAgent.Position;
        Quaternion desRotation = Quaternion.LookRotation(new Vector3(mCrowdAgent.DesiredVelocity.x, 0, mCrowdAgent.DesiredVelocity.z));
        rotation = Quaternion.Slerp(rotation, desRotation, Time.deltaTime * this.mTurnSpeed);
        return true;
    }

    public Vector3 GetNearestPoint(Vector3 position)
    {
        NavmeshPoint point;
        NavStatus status0 = query.GetNearestPoint(position, Vector3.one, mCrowdManager.QueryFilter, out point);
        return point.point;
    }

    private NavStatus generateNavPath(NavmeshPoint start, NavmeshPoint end)
    {
        if (mCrowdAgent == null)
        {
            mCrowdAgent = mCrowdManager.AddAgent(start.point, mCrowdAgentParams);
            mCrowdAgent.RequestMoveTarget(end);
        }
        else
        {
            if (mCrowdAgent.Position != end.point)
            {
                mCrowdManager.RemoveAgent(mCrowdAgent);
                mCrowdAgent = mCrowdManager.AddAgent(start.point, mCrowdAgentParams);
                mCrowdAgent.RequestMoveTarget(end);
            }
        }

        //return status;
        return NavStatus.Sucess;
    }

    public bool GeneratePolyPath(Vector3 startPos, Vector3 endPos)
    {
        polyResult.Initialize();

        NavmeshPoint startPoint;
        NavmeshPoint endPoint;
        NavStatus status0 = query.GetNearestPoint(startPos, Vector3.one, mCrowdManager.QueryFilter, out startPoint);
        NavStatus status1 = query.GetNearestPoint(endPos, Vector3.one, mCrowdManager.QueryFilter, out endPoint);

        NavStatus status = query.FindPath(startPoint, endPoint
           , mCrowdManager.QueryFilter, polyResult
           , out polyResultCount);

        //Debug.Log("GenerateNavPathResult:" + startPoint.point.ToString() + "~" + endPoint.point.ToString() + ":" + polyResultCount.ToString());
        //string result = "";
        //for (int i = 0; i < polyResultCount; i++)
        //{
        //    result += polyResult[i].ToString() + ";";
        //}
        //Debug.Log(result);

        if ((status & NavStatus.Sucess) == 0)
        {
            return false;
        }        
        return true;
    }

    //用递归的方式生成路径点，因为直接用MoveAlongSurface寻路找不到太复杂的路径
    private void generatePointPath(NavmeshPoint startPoint, NavmeshPoint endPoint)
    {
        int nVisitedCount;
        Vector3 targetPos;       

        NavStatus status = query.MoveAlongSurface(startPoint, endPoint.point, mCrowdManager.QueryFilter
            , out targetPos, polyResultBuffer, out nVisitedCount);
        float dis=Vector3.Distance(targetPos, endPoint.point);        
        if (Vector3.Distance(targetPos, endPoint.point) > mMaxPointDis)
        {
            int nPolyCount;
            //点路径过为复杂，计算失败，根据三角形路径拆为两段递归计算
            status = query.FindPath(startPoint, endPoint
                , mCrowdManager.QueryFilter, polyResultBuffer
                , out nPolyCount);
            if (nPolyCount > 2)
            {
                uint midPoly = polyResultBuffer[nPolyCount / 2];
                NavmeshTile tile;
                NavmeshPoly poly;
                navmesh.GetTileAndPoly(midPoly, out tile, out poly);

                if (tile.GetTileRef() != tileBufferRef)
                {
                    tile.GetVerts(tileBufferPoints);
                }
                
                int minIndex = -1;
                float minDistance = 0.0f;
                for (int i = 0; i < poly.vertCount; i++)
                {
                    ushort index = poly.indices[i];                  
                    Vector3 pos = tileBufferPoints[index];
                    //Debug.Log("pos=" + pos.ToString());
                    float distance=Vector3.Distance(pos,endPoint.point);
                    if (minIndex < 0 || distance < minDistance)
                    {
                        minDistance = distance;
                        minIndex = index;
                    }
                }
                Vector3 midPos = tileBufferPoints[minIndex];
                NavmeshPoint midPoint;
                query.GetNearestPoint(midPos, Vector3.one, mCrowdManager.QueryFilter, out midPoint);
                generatePointPath(startPoint, midPoint);
                generatePointPath(midPoint,endPoint);
                return;
            }
            else if (nPolyCount == 1)
            {
                uint PolyIndex = polyResultBuffer[0];
                NavmeshTile tile;
                NavmeshPoly poly;
                navmesh.GetTileAndPoly(PolyIndex, out tile, out poly);
                //if (poly.Type == NavmeshPolyType.OffMeshConnection)
                {
                    Debug.Log(poly.Type.ToString()+":" + startPoint.point.ToString() + endPoint.point.ToString());
                }

                int minIndex = -1;
                float minDistance = 0.0f;
                for (int i = 0; i < poly.vertCount; i++)
                {
                    ushort index = poly.indices[i];
                    Vector3 pos = tileBufferPoints[index];
                    Debug.Log("pos=" + pos.ToString());
                    float distance = Vector3.Distance(pos, endPoint.point);
                    if (minIndex < 0 || distance < minDistance)
                    {
                        minDistance = distance;
                        minIndex = index;
                    }
                }
            }
            else
            {
                //中间没有多个多边形，直接过去
                Debug.Log("go stright:"+startPoint.point.ToString() + endPoint.point.ToString());
                if ((pointResultCount<1) || (Vector3.Distance(startPoint.point,pointResult[pointResultCount-1])>mMaxPointDis))
                {
                    pointResult[pointResultCount] = startPoint.point;
                    pointResultCount++;
                }
                pointResult[pointResultCount] = endPoint.point;
                pointResultCount++;
            }
        }
        else
        {
            //已经找到简单路径，输出
            int pathCount;            
            query.GetStraightPath(startPoint.point, targetPos
                , polyResultBuffer, 0, nVisitedCount
                , pointResultBuffer
                , null, null, out pathCount);
            if ((pointResultCount < 1) || (Vector3.Distance(pointResultBuffer[0], pointResult[pointResultCount - 1]) > mMaxPointDis))
            {
                pointResult[pointResultCount] = pointResultBuffer[0];
                pointResultCount++;
            }
            for (int i = 1; i < pathCount; i++)
            {
                pointResult[pointResultCount] = pointResultBuffer[i];
                pointResultCount++;
            }
         }

    }

    public bool GeneratePointPath(Vector3 startPos,Vector3 endPos)
    {        
        if (mCrowdManager == null)
        {
            Debug.LogError(fileName + ": Aborted initialization. Failed crowd creation.");
            return false;
        }

        NavmeshPoint startPoint;
        NavmeshPoint endPoint;

        NavStatus status0 = query.GetNearestPoint(startPos, Vector3.one, mCrowdManager.QueryFilter, out startPoint);
        NavStatus status1 = query.GetNearestPoint(endPos, Vector3.one, mCrowdManager.QueryFilter, out endPoint);

        //Debug.Log("Find path from" + startPoint.point.ToString() + "To" + endPoint.point.ToString());

        pointResultCount = 0;
        generatePointPath(startPoint, endPoint);
        return true;
    }
}

