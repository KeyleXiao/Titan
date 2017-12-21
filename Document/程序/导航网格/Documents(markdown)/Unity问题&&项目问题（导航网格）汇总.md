# Unity问题&&项目问题（导航网格）汇总

标签（空格分隔）： 冰川工作

---
#Unity问题
- 大部分的`Component`没有加`Tag`，希望能够和美术协商对每个物件添加`Tag`进行分类
- 之前有过几次导航网格生成卡死问题，主要是双方阵营主基地那边有几个物件十分复杂，主要是`祭坛`，`塔的底座`，`台阶`，`模型`的多边形繁多已经超出了导航网格插件的场景多边形数量的上限（也有可能爆内存），希望与美术协商优化简化场景中一部分物体的模型复杂度以提高客户端性能

---
#动态障碍(项目)
##需求
- 技能需要，比如法师结界技能需要阻挡指定实体
- 战场动态生成地面物体需要，比如兵线的进入战场后的塔，进行寻路时候，需要实体自己能够绕过去
- 其他，比如指定一块地图区域只能让指定的阵营或者实体通过

##设计思路以及设计理由
###原项目中的寻路
　　目前项目中的寻路是基于`Recast Navigation`这个第三方库实现的,服务器使用已经生成好的导航数据来进行广播导航信息，导航数据由同样基于`Recast Navigation`开发的`Unity`第三方插件`CritterAI`(此插件没有实现动态生成网格的功能)来生成,通过`CritterAI`读取游戏场景并生成服务器可加载的`binary`导航数据（即后缀名为.byte的文件）给服务器与客户端来对实体进行寻路操作，所以，**实际上生成导航网格这一步并不在游戏runtime时进行，而是预先已经生成好的导航数据**。

###动态障碍设计中所遇到的困难以及问题
　　从通常的动态障碍设计中，导航数据应该是在runtime期间可以改变，然后再通过不变的寻路算法来加载新的导航数据，然后进行动态寻路，`Recast Navigation`中的确支持runtime期间动态生成导航数据，但是有个**问题**：**要生成导航数据需要原始地形数据，这些地形数据都在客户端（完全掌握这些网格的生成细节处理在项目时间上也不可行），由于我们项目中游戏场景大又复杂，重新生成导航网格不仅仅十分消耗时间而且还不能保证生成的网格一定正确（目前只能人工排查生成好的导航网格是否正确），所以在runtime期间动态改变导航数据不可行**，从各种渠道了解的情况来看，动态障碍可以使用`Unity`的`碰撞检测`来做，但是为了考虑到防作弊反外挂等问题，所有的导航信息必须通过服务器来验证，所以动态障碍这个功能只能由服务器这边来做,不管是小兵怪物还是客户端玩家的动态障碍寻路操作均由服务器控制。
　　
###解决出路以及现在的设计思想
　　根据以上描述，动态生成新的导航数据已经不可行，所以需要在不改变原来导航数据的前提下进行动态障碍寻路，通过分析`Recast Navigation`寻路的方式，流程以及同事的讨论，觉得采用以下方法进行动态障碍开发.
>- 由于导航数据是由`Tile（导航瓷砖，一些指定长度的正方形）`与`多边形构成`,多边形构成Tile，一定数量的Tile构成整个导航数据，所以动态障碍导航方法就是:**只需要知道障碍物落在哪些Tile,然后单独把这些Tile提取出来，根据障碍物对Tile的影响，来对Tile进行人工分割（Tile本身有多边形构成），然后当实体寻路的时候，碰到这些受障碍物影响的Tile，单独做处理，这里单独处理的方式就是在Tile内部再次进行寻路（寻路算法依然参考常规寻路）**，这样等于是在原有的导航网格上面我们再次封装一套专门用于动态障碍寻路所使用的导航网格（以后考虑将动态障碍的导航网格整合进标准的导航网格中，这也算是一个优化点），即当碰到动态障碍时，使用自己封装的网格来寻路，当动态障碍物寻路完成时，可以回到标准寻路当中来。
>- 为了记录改变的Tile的相关信息，所以在原来的`Recast Navigation`中添加了一些新的类型结构，用于扩展Tile的相关信息（包括多边形信息），然后根据上面所说障碍物多边形的来对Tile中的导航网格多边形进行裁剪，裁剪完成后，Tile中的多边形会被区分为可通过的原来的多边形，和包含在裁剪障碍物多边形中的障碍物多边形，然后根据`Recast Navigation`的寻路规则来进行相关处理后，接着对寻路部分进行单独处理即可实现动态障碍寻路
>- 动态障碍物使用实体来实现，项目中动态障碍物的生命周期与实体绑定(可认为障碍物生命周期==实体生命周期)，实体的**创建和销毁由战场和效果来控制**，这样动态障碍完全作为一个通用接口存在，不和具体业务耦合
>- 客户端玩家的动态障碍创建以及寻路也完全由服务器发送消息来控制
>- 增加一个寻路代理类（`DynamicObstacleService_Proxy`）,**战场**以及**效果**全部通过这个代理类来和寻路库进行交互操作

##动态障碍多边形添加部分设计与实现
- 一些必要的说明

```c++
/// Derives a standard polygon reference.
///  @note This function is generally meant for internal use only.
///  @param[in]	salt	The tile's salt value.
///  @param[in]	it		The index of the tile.
///  @param[in]	ip		The index of the polygon within the tile.
/// 多边形引用主要由salt，it,ip(均为无符号)组合而成，salt长度为17位最大可表示（65535）,it长度为11位最大可表示（2047）,
/// ip长度最大为4位最大可表示(15)，可参考decodePolyId中的代码协助理解
/// 动态障碍多边形的引用生成方式类似，但是主要，动态障碍多边形存储在ArrayBuffer中，salt中存有ArrayBuffer的索引下标,这里和原来recast的处理方式不太一样，
/// 原recast中是将多边形索引主要由it和ip合并确定，salt作为辅助插值，ip主要是用来标记一个Tile中的内部多边形下标位，但是Tile中最多能表示16个(m_polyBits这个值只有4位)，
/// 在生成的动态障碍多边形中原本使用ip作为ArrayBuffer的索引下标的存储位，但是ArrayBuffer的索引下标值会超过16，这样会造成存储结果混乱，为了保证即能够存储动态多边形方便
/// 同时不会影响原来多边形的存储，这里采用将ArrayBuffer的索引下标值与DT_DYN_POLY_SALT相加作为salt值制成多边形引用，65535的长度在可预测的范围内能够满足需求.
inline dtPolyRef encodePolyId(unsigned int salt, unsigned int it, unsigned int ip) const
```

- 动态障碍多边形添加(`Server\DetourNavMesh\Source\DetourNavMesh.cpp`)

```c++
/// 向导航网格中动态添加多边形，障碍物多边形, 并在这里调用clipPolygon，进行多边形分割
///  @param[in]	    verts	要动态插入的多边形顶点信息.
///  @param[in]	    nverts	对应verts的顶点数量
///  @param[out]    polyref	最终获得的多边形索引
/// @return The status	flags for the operation.
/* 工作流程：
 * 首先分配好要动态添加的多边形顶点buffer和多边形buffer,
 * 然后取出添加的多边形的每个顶点，遍历多边形每个顶点，
 * 取出因其受到影响的多边形reference，并添加邻接边信息
 */
dtStatus dtNavMesh::addDynamicPolys(float* verts, int nverts, dtPolyRef& obsRef, int nFlag)
{
// ... 详细内容见项目
}
```

##裁剪多边形部分设计与实现
- 多边形裁剪(`Server\DetourNavMesh\Source\DetourNavMesh.cpp`)

```c++
////////////////////////////////////////////////////////////////////////////////////////////
/* 算法: 利用Sutherland-Hodgman algorithm对多边形进行裁剪。
 * 假设待裁剪多边形为M，裁剪多边形为Ni(i=0,n)；
 * 对于切割多边形中的某条边AB，计算边所在直线AB与待裁剪多边形的交点；
 * 那么，待裁剪多边形被直线AB切割为两部分，在外面的部分即为新增的子多边形，里面的部分赋值为M；
 * 逆时针遍历下一条边，直至每条边都遍历完为止。
 * 此算法有缺陷，有一些情况没有考虑到
 * QuickTutorial 地址： https://www.youtube.com/watch?v=S091lKYWbSs
 * 缺陷处理：https://www.youtube.com/watch?v=Euuw72Ymu0M
 * 算法实现参考(伪代码部分)：https://en.wikipedia.org/wiki/Sutherland%E2%80%93Hodgman_algorithm
 * 这里使用Sutherland-Hodgman算法进行裁剪会产生凹多边形，还需要对裁剪的多边形进行去凹多边形化
 * ATTENTION！！！
 * 保存顶点的一个最基本的原则就是，在切割多边形内部，线段向量终点一定是保存的，交点也是要保存的
 */
////////////////////////////////////////////////////////////////////////////////////////////
// 已经经过严格测算验证，多边形切割没有任何问题
dtStatus dtNavMesh::clipPolygon(dtPolyRef ref)
{
// ... 内容省略，详见项目
}
```

- 多边形裁剪后的相邻多边形边界链接信息计算(`Server\DetourNavMesh\Source\DetourNavMesh.cpp`)

```c++
/* 此函数功能：
 * 1. 将一个含有动态子多边形的多边形每个与父多边形同边的子多边形进行映射
 * （即当进行寻路时候，遇到同边的子多边形开始在父多边形内部进行子多边形的寻路）
 * 2. 为子多边形进行Link设置，即每个边进行关联操作
 */
//*/
void dtNavMesh::calChildPolysLinks(dtPolyRef ref)
{
// ... 详细代码见项目
}
```

##寻路部分设计与实现
###寻路部分设计
- 首先当寻路遇到包含有障碍物部分的多边形时，激活该多边形内部寻路（这里还有些边界条件需要处理），由于原来寻路会遍历多边形周围的邻接多边形来寻找哪个邻接多边形是最优寻路策略，所以这里内部寻路采取的方式是当遍历到一个一个邻接多边形时，以这个邻接多边形与原多边形的邻接边为出口（出口坐标位邻接边的中心点坐标），以当前寻路点为入口，在原多边形内部进行寻路，看看能不能够到达这个邻接多边形，如果可以，那继续走原来的寻路流程，不行就continue掉这个邻接多边形，原多边形内部寻路的过程如下:
    >* 首先通过当前寻路实体的坐标和与邻接多边形同边的内部出口坐标，寻找内部起点子多边形，以及终点多边形,如果障碍物多边形完全覆盖了终点子多边形与邻接多边形的邻接边部分，则认为改条路径不可达，放弃此路径，选择下一个邻接多边形继续进行寻路
    >* 当找到可达的内部寻路路径时，接下来的内部寻路的工作和recast navigation中的完全一致

- 修改后的`Recast Navigation`寻路部分(`Server\DetourNavMesh\Source\DetourNavMeshQuery.cpp`)
```c++
/// @par
///
/// If the end polygon cannot be reached through the navigation graph,
/// the last polygon in the path will be the nearest the end polygon.
///
/// If the path array is to small to hold the full result, it will be filled as 
/// far as possible from the start polygon toward the end polygon.
///
/// The start and end positions are used to calculate traversal costs. 
/// (The y-values impact the result.)
///
dtStatus dtNavMeshQuery::findPath(dtPolyRef startRef, dtPolyRef endRef,
								  const float* startPos, const float* endPos,
								  const dtQueryFilter* filter,
								  dtPolyRef* path, int* pathCount, const int maxPath,
                                  int nFlag) const
{
//...... 省略

            // go across internal polygons
			// 穿越内部多边形,动态障碍有关
            // 这里的处理思路是：
            // 首先在含有障碍物的导航网格搜寻邻接多边形时，判断能够跑到邻接多边形位置区，如果可以，则可以走接下来的路
            // TODO:现在是从标准的邻接多边形绕过，这样绕路距离会比较大,可以当进行内部寻路，
            // 然后走到邻接多边形的位置时候进行邻接边界的位置坐标重新计算(使用内部动态子多边形的边界来进行计算)

            // 这里取得邻居多边形的邻接边的2个顶点坐标
            float left[3] = {0.0f};
            float right[3] = {0.0f};

            if (dtStatusFailed(getPortalPoints(bestRef, bestPoly, bestTile, 
                neighbourRef, neighbourPoly, neighbourTile, left, right)))
                return DT_FAILURE | DT_INVALID_PARAM;

            if (extend && (extend->nFlag & nFlag))
			{
                if ( dtStatusSucceed(this->findInnerPath(
                        &bestNode->id,
                        extend,
                        bestNode->pos, neighbourNode->pos,
                        left, right,
                        filter,
                        path, pathCount, 128))
                    )
                {
                    while(!m_childOpenList->empty())
                        m_openList->push(m_childOpenList->pop());
                }
                else
                {
                    continue;
                }
			}
            //*/
//...... 省略
}
```

- 多边形内部寻路部分(`Server\DetourNavMesh\Source\DetourNavMeshQuery.cpp`)
```c++
    /// 动态多边形内部寻路
    /// 多边形内部寻路（动态障碍,以下描述多边形为标准多边形，子多边形为标准多边形的内部多边形）
    ///  @param[in]		polyRef	            标准多边形的多边形引用
    ///  @param[in]		extend	            网格的extend数据
    ///  @param[in]		leftEdgeNeighbour	标准多边形与标准多边形的邻居多边形的邻接边的2个顶点坐标
    ///  @param[in]		rightEdgeNeighbour	标准多边形与标准多边形的邻居多边形的邻接边的2个顶点坐标
    ///  @param[in]		startPos	        内部寻路起点坐标
    ///  @param[in]		endPos		        总寻路重点坐标
    ///  @param[in]		filter		        
    ///  @param[out]	path	            内部寻路路径
    ///  @param[in]		pathCount		    内部寻路坐标路径点数量
    ///  @param[in]		maxPath		        内部寻路坐标最大路径点数量
    /// @return 如果寻路成功，即可以在多边形内部走到目的邻接多边形endRef,则返回true，否则返回false
    dtStatus dtNavMeshQuery::findInnerPath(
        dtPolyRef* polyRef,                                              // 标准多边形的多边形引用
        dtPolyExtend* extend,                                            // 标准多边形的extend
        const float* leftEdgeNeighbour, const float* rightEdgeNeighbour, // 标准多边形与标准多边形的邻居多边形的邻接边的2个顶点坐标
        const float* startPos, float* endPos,                            // 起始坐标，终点坐标（标准多边形内部子寻路）
        const dtQueryFilter* filter,
        dtPolyRef* path, int* pathCount, int maxPath) const;             // 内部寻路路径
{
    ...内容略，详见项目
}
```

##待优化点
- 可以动态多边形内部与外部标准多边形统一起来寻路，增加精确度（需要对Unity寻路插件进行二次开发，最好是有时间再做）

##遗留问题
###多动态障碍物叠加问题
目前动态障碍并不是直接依赖导航网格库中的,而是在原有网格基础上自己在外部封装一层`ExtendPoly`,所以进行多动态障碍叠加时，需要自己来计算多动态障碍物叠加后所产生的多边形，目前已知的思路和问题点主要是以下几个：

- 首先多个有相交的动态障碍多边形的顶点需要合并成一个大的动态障碍多边形, （如图所示），障碍多边形`A`与障碍多边形`B`，有公共区域`C`,由于公共区域`C`是重合部分，需要把abc部分合并为一个总的大障碍物多边形，怎么计算比较好，后面还会有一个关联问题，这里障碍物是切割网格的，会有个先后问题（后切割网格的障碍物，如何与之前切割的网格的障碍物统一起来）：
![QQ截图20160411145551.jpg-9.4kB][1]

-  多个障碍物的生成和销毁之间往往不是同时进行的，下面以2个障碍物来举例：首先出现的障碍物对网格进行了切分，然后由出现一个障碍物和之前障碍物有叠加，也要进行切分，但是计算的时候需要和前面的多边形统一起来，如果2个障碍物都还没有销毁，那么寻路的时候需要将这个2个障碍物合并处理作为一个障碍物来进行动态障碍寻路，如果其中一个销毁，则需要把动态障碍网格还原成一个障碍物的情况来寻路，也就是说既需要合并多边形障碍物，也需要保留原来单独的障碍物信息

#一些注意要点
##Unity导航网格插件
由于项目中所用的导航网格插件在2012年原作者已经不再维护，当时所用的`unity版本为3.4`，目前我们项目中所使用为`unity 5.2`这个版本，从Unity 5开始逐步淘汰部分接口，很不幸，导航网格生成插件中所使用的一个`shader`就是被淘汰的一个,客户端升级完Unity版本后需要同步编译一遍导航网格插件，编译之前需要替换引用:

- 首先打开`critterai-master`文件夹，打开`build`下面的`unity`文件夹，里面有VS工程文件，使用`VS2010`打开工程，首先依次替换`project`的引用部分(红框部分，注意编译的时候要选择release版本),如下图：
![{3438704B-D692-426A-B4F5-3793AA8704D4}.jpg-99.5kB][2]

- 同时还要注意，首先要编译好`cai-nav-rcn`和`cai-nmgen-rcn`这2个project，这2个project是recast navigation这个寻路库的核心部分，编译完这2个project会在工程目录下面的`bin`目录下的`release`中生成2个dll,插件中的C#中的部分project均会依赖着2个dll，务必请根据工程引用提示按需加入

- 在最开始遇到Unity新版本中导航插件无法使用是由于插件代码中的与Unity交互部分使用了老旧版的shader，导致在Unity中生成网格进行预览时，画出的网格都是色块，无法识别，
    - **解决办法**:
    - 找到`..\critterai-master\src\main\Assets\CAI\util-u3d\DebugDraw.cs`这个文件，找到下面这段代码
    
    ```cs
    public static Material SimpleMaterial
        {
            get
            {
                if (!mSimpleMaterial)
                {
                    /* //原来项目中的不可用shader
                    mSimpleMaterial = new Material(
                        "Shader \"Lines/Colored Blended\" {"
                        + "SubShader { Pass { "
                        + "	BindChannels { Bind \"Color\",color } "
                        + "	Blend SrcAlpha OneMinusSrcAlpha "
                        + "	ZWrite Off Cull Off Fog { Mode Off } "
                        + "} } }");
                    */
                    // 使用了unity中的新shader替换
                    //////////////////////////////////////////////////////
                    var drawLine = Shader.Find("Hidden/Internal-Colored");
                    mSimpleMaterial = new Material(drawLine);
                    //////////////////////////////////////////////////////
                    mSimpleMaterial.hideFlags = HideFlags.HideAndDontSave;
                    mSimpleMaterial.shader.hideFlags = HideFlags.HideAndDontSave;
                }
                return mSimpleMaterial;
            }
        }
    ```

- 生成网格文件不仅仅需要navmesh文件同时需要需要生成binary文件，服务器读取数据是读binary文件，如果重新从`github`的master分支中fork一个工程，原始代码中没有生成binary文件的的代码，需要手动添加
    - **文件路径**
    - `critterai-master\src\main\Assets\CAI\nav-u3d\Editor\CAIBakedNavmeshEditor.cs`

    ```c#
    private static void SaveMesh(CAIBakedNavmesh targ, string filePath)
    {
        if (filePath.Length == 0 || !targ.HasNavmesh)
            return;
    
        FileStream fs = null;
        BinaryFormatter formatter = new BinaryFormatter();
    
        try
        {
            fs = new FileStream(filePath, FileMode.Create);
            formatter.Serialize(fs, targ.GetNavmesh().GetSerializedMesh());
        }
        catch (System.Exception ex)
        {
            Debug.LogError(targ.name + ": BakedNavmesh: Save bytes failed: "
                + ex.Message);
        }
        finally
        {
            if (fs != null)
                fs.Close();
        }
    
        // 生成Binary文件
        ////////////////////////////////////////////////////////
        FileStream pFileStream = null;
        try
        {
            byte[] meshData = targ.GetNavmesh().GetSerializedMesh();
            pFileStream = new FileStream(filePath+".byte", FileMode.OpenOrCreate);
            pFileStream.Write(meshData, 0, meshData.Length);
        }
        catch
        {
            //return false;
        }
        finally
        {
            if (pFileStream != null)
                pFileStream.Close();
        }
        ////////////////////////////////////////////////////////
    
        //return true;
    }
    ```

##Unity画动态障碍物

```cs
#region 画动态障碍物
private Dictionary<uint,GameObject> dynamicObstacles;
public void DrawDynamicObstacle(cmd_DynamicObstacle_Info param)
{
    if (dynamicObstacles.ContainsKey(param.uidTarget))
    {
        GameObject oldObject = dynamicObstacles[param.uidTarget];
        GameObject.Destroy(oldObject);
        dynamicObstacles.Remove(param.uidTarget);            
    }

    if (param.nIsAdd == 1)
    {
        GameObject newObstacle = new GameObject();
        newObstacle.name = "Obstacle" + param.uidTarget;
        for (int i = 0; i < param.nPosVertexNum; i++)
        {
            int nextIndex = i + 1;
            if (nextIndex >= param.nPosVertexNum) nextIndex = 0;
            Vector3 posa = new Vector3(param.nPosVertex[i * 3], param.nPosVertex[i * 3 + 1], param.nPosVertex[i * 3 + 2]);
            //Debug.Log("Vector:" + posa.ToString());
            GameObject newVector = GameObject.CreatePrimitive(PrimitiveType.Sphere);                
            Transform newVectorTransform = newVector.transform;
            GameObject.Destroy(newVectorTransform.GetComponent<Collider>());
            newVectorTransform.parent = newObstacle.transform;
            newVectorTransform.localScale = new Vector3(0.1f, 0.1f, 0.1f);
            newVectorTransform.position = posa;                

            Vector3 posb = new Vector3(param.nPosVertex[nextIndex * 3], param.nPosVertex[nextIndex * 3 + 1], param.nPosVertex[nextIndex * 3 + 2]);
            GameObject newEdge = GameObject.CreatePrimitive(PrimitiveType.Cube);                
            Transform newEdgeTransform = newEdge.transform;
            GameObject.Destroy(newEdgeTransform.GetComponent<Collider>());
            newEdgeTransform.parent = newObstacle.transform;
            newEdgeTransform.localScale = new Vector3(0.1f, 5.0f, (posb - posa).magnitude);
            newEdgeTransform.position = (posa + posb) / 2.0f;
            newEdgeTransform.LookAt(posb);                
        }
        dynamicObstacles[param.uidTarget] = newObstacle;
    }

}
#endregion
```

##其他
- 在进行多边形裁剪算法实现时，为了检测裁剪的多边形是否正确，需要对裁剪的图像进行人工判断，裁剪后的输出信息只有顶点坐标信息，这里我给出一个坐标绘制矢量图工具`SVG`，这是一个`HTML`中的一个组件，可以只给出顶点坐标，然后在网页上生成多边形（这里注意SVG的坐标系情况）


  [1]: http://static.zybuluo.com/wuxinwei/y7r1qv1ltpdqvppsaddp9bun/QQ%E6%88%AA%E5%9B%BE20160411145551.jpg
  [2]: http://static.zybuluo.com/wuxinwei/v7zmlb9fmft6qafaxjgymzql/%7B3438704B-D692-426A-B4F5-3793AA8704D4%7D.jpg