# CritterAI&&Unity导航研究

标签（空格分隔）： 冰川工作

---
#Unity
##Unity中为一个GameObject添加导航信息
- 首先，需要为GameObject添加一个**NavMesh Agent**（用于引导GameObject导航寻路，在**component->Navigation**中）
- 然后需要在对应的Terran或者Plane中，选择**Navigation**控件中进行Bake(烘焙)来产生导航网格路径
- Unity中使用**A***（A star）与**Dijsktra**最短路径算法结合来进行寻路转向等操作

##Unity导航寻路
- 实际上Unity官方的导航寻路也是基于Recast Navigation并进行了深度改造，此开源库的作者于2013年已经加入了Unity Inc

---
#项目
##导航网格的生成
- 静态导航网格可以借助**CritterAI插件**在Unity中直接生成,这个比较简单，生成导航网格的相关参数配置在下面的**网格生成过程中的参数影响(Configuration Parameters)**有详细说明，对照说明进行配置即可，生成的导航网格数据可以同时给客户端和服务器使用。
- 真正的难点在于运行时的导航网格生成，具体做法和流程可以参见Recast Navigation的Demo，下面有详细介绍

##基本寻路流程
>- 点选一个实体进行对应位置的移动操作时，会首先调用`PathAgent::setTargetPos`，在这个函数里面，会在其对应的寻路服务`pPathFindService`中调用`GetPath`来获取对应的行走路径
>- 获取到路径后，每个`PathAgent`里面有一个用于保存行走路径的变量`m_pVec3StraightPath`,该变量用于保存本`PathAgent`的需要进行行走的路径
>- 在`PathCrowd`下面有一个`update`,这个函数在`PathFindService::update`里面不断地调用（实际上就是实时更新`PathAgent`的寻路状态，其实时更新的频率由传入`PathFindService::update`的参数来定）

---
#Recast Navigation
该库使用十分广泛，Unity引擎官方的寻路,unreal的UDK的寻路均基于Recast Navigation开发的

##关于动态障碍的问题
**RecastNavigation**在特定设置下可以支持动态网格生成的,也就是说可以直接使用**RecastNavigation**来做动态障碍,如何做，询问过**RecastNavigation**作者，他是这么回答的:
![QQ截图20151124105735.png-44.3kB][1]
- 目前项目中现有的动态障碍使用的后一种方法，使用**Tile Cache**的方式实现的

##RecastDetour
- 在RecastDetour里面有一个**Sample_SoloMesh.cpp**这样一个简单地demo，这个文件实现了如何建立导航网格的全部过程
- 在RecastDetour里面还有一个**NavMeshTesterTool.cpp**，这个文件则演示了如何使用前面建立的导航网格来进行寻路动作

##Recast Demo
该Demo为一个综合性Demo，里面所演示的内容十分全面(其注释十分详尽)，下面简要说明下目录结构和主要文件，以及主要函数的功能,目前项目中所使用的导航以及导航网格生成动态障碍等等，其设计思想以及代码流程都是基于该Demo所提供的方案，所以说完全弄透这套代码基本导航网格寻路以及动态障碍就差不多了

###目录结构
```c++
Bin	//最终生成的可执行文件
Contrib	//一些第三方DLL
English.lproj 
Include	
Source
Icon.icns
Info.plist
premake4.lua //premake4的lua配置脚本，用于生成VS2010工程等
screenshot.png
```

###主要文件
```c++
CrowdTool.cpp //管理实体移动寻路以及碰撞
NavMeshTesterTool.cpp // 演示Detour如何来寻找路径，单纯的寻找路径
Sample_SoloMesh.cpp //演示如何来生成导航网格
Sample_TempObstacles.cpp //演示如何添加临时障碍物对网格产生的影响，与动态障碍相关
```

###关于Demo中的动态障碍
Demo中的动态障碍内容在`Sample_TempObstacles.cpp`,`DetourTileCache.cpp`,过程如下:
>- 首先，当在Demo中按下添加障碍时，会在Demo中调用`Sample_TempObstacles::addTempObstacle`,然后会生成一个`ObstacleRequest`,然后将这个`request`放入请求队列当中去（实际上是个数组）
>- 随后这个增加障碍物的请求依然会在一个`update`（`dtTileCache::update`）中进行，这个`update`更新的频率也是按照设置的帧率来确定的，在这个`update`中，仅仅只做动态障碍相关的更新，比如去除障碍物，添加障碍物等,
>- `update`首先会对前面的`request`进行处理，然后才会进行**导航网格**的`update`
>- 导航网格的更新会在`dtTileCache::buildNavMeshTile`中进行,基本处理过程在代码中有相应注释，至此，动态地添加障碍物的处理过程完毕

##相关参考资料:
- [RecastNavigation讨论社区（作者回答问题）][2]
- [RecastNavigation官方文档][3]

---
#CritterAI
- 基于**Recast Navigation**制作，**NMGen**生成网格导航数据，至于如何使用这些导航数据，需要看**Detour**(a sister library of Recast),
- 源码地址：
  [memononen@github.com/recastnavigation][4]
- [Detour-2.0.2-Doc][5]

##CAINav基本结构
>- The **Navmesh** class defines the structure and state of the navigation mesh. The only user component that deals directly with this class on a regular basis is the component responsible for creating the mesh and managing its state. Normal navigation clients, the one's needing to perform pathfinding and such, rarely if ever interact directly with the navigation mesh.
>- **The most important class to understand and become comfortable with is the NavmeshQuery class**. It provides almost all of the client navigation features, including A* and Dijkstra path searches, string pulling, point and polygon searches, raycasting, etc.
>- The **NavmeshQueryFilter** is a seemingly minor class, but it has a large impact on navigation. Filters are used to define the costs of traversing the mesh and flags that control which parts of the navigation mesh are included in operations. Multiple filters can be used to customize the navigation behavior on a per client basis.
>- The **PathCorridor class** provides a way of moving along a navigation path without worrying about the minutiae of managing the path.
>- The **CrowdManager** is the big beast of the navigation classes. It not only handles a lot of the path management for you, but also local **steering and dynamic avoidance** between navigation clients. I.e. It can keep your agents from running into each other.

---
###The Navigation Mesh
- 即导航网格，为导航系统的基础数据模型
- 包含以下几个东西：
    - Tiles(导航块，包含了主要的结构和状态信息,导航块是基于具有可塑性的导航网格所构成的，能够在运行时进行变动，这意味着可以实现导航网格的动态变化)
    - Structural Elements(结构元素，两种，多边形和off-mesh connection)
    - State Data(状态信息,用于关联单个多边形和off-mesh connection,包含**area**和**flags**,**area**被用于关联在结构元素中通过所需的消耗，主要影响寻路。**flags**控制结构元素什么时候是**可通过的**。)
    - Tile and Polygon References(理解导航块和多边形引用是熟悉导航网格的关键(后面用**T&P**代指导航块和多边形引用)，T&P本质上来说是无符号整形用于控制导航网格中的结构元素，**T&P是可以用来唯一确定多边形或者off-mesh connection的标志**,导航网格结构控制着T&P的生命周期，改变导航网格的配置或者导航块内部结构会导致**T&P**的无效,**T&P**可以在几种情况下保存下来：
        - 导航块状态改变时保留下来           
        - 运行时loading和unloading导航块时保留
        - 导航网格在正常的序列化和反序列化期间)
    - 至于如何生成导航网格，请参考最下面的**更加具体的参考资料**

###The Navigation Mesh Query
- 按照CAINAV文档的说法，这个是最重要最需要去熟悉理解的一个class，Navigation Mesh Query提供了用于pathfinding所必要的绝大多数特征信息
- 特征，取得的特征通常包含2大类：
    - Pathfinding
    - local search
- 寻路依然是基于标准A*与德加斯特拉最短路径算法(单源最短路径)

###The Path Corridor
- 提供一个沿着导航路径走的移动方式

###The Crowd Manager(CrowdManager)
- 简介：拥塞管理器是导航组件中最庞大的一个，它不仅仅需要处理路径管理，同时需要管理原地转向和动态碰撞避免
- 限制以及问题：
    - **最大的限制**在于你必须给定被控制的导航代理实体的position以及速率给crowd manager，你能够更新最大速度和最大加速度，但是为了能够让crowd manager去完成他自己的工作，它不会准许你不断地去覆盖他的位置和速率，也就是说你无法直接控制导航代理实体的移动，这个控制权在crowd manager。
    - **第二个比较重要的限制**是导航代理实体从一个当前的多边形位置到达目标多边形位置，中间所经过的多边形位置的数量不准许超过256个，如果你超过了，那么很有可能导航代理实体会导航失败无法到达指定目的地，解决办法在于在长距离导航路径中间加入中间目的地，说白了太长了一次不行，就分段处理(分治法)。
    - 所有的导航代理实体使用相同的NavmeshQueryFilter
    - 拥塞管理器十分消耗性能，拥塞管理器一次最多能够处理不超过20个导航代理实体

- 更加具体的参考资料:
    - [An Introduction to Navigation][6]

###关于CritterAI中的寻路算法A*
`A*寻路算法`本身资料比较丰富，就不做过多赘述，通常网上给的资料中的`A*`寻路适用于网格为方格子的情况,这里CritterAI中的网格为凸多边形（多为三角形）

- 参考资料：
    - [A* search algorithm From Wikipedia, the free encyclopedia][7] 
    - [A* Pathfinding Tutorial - YouTube][8]

---
##网格生成过程中的参数影响(Configuration Parameters)
- 更加详细请参考：[Configuration Parameters][9]
###cellSize
- 描述：> 0，设定体素(voxel)在X-Z-plane(OpenGL坐标系)上的大小,决定体素(voxel)在对原始几何图形进行采样时的精细度，值越小越精细，越大越粗略
- 影响：较小的值能够产生更加精确接近原始集合图形的网格，减少在生成导航网格过程中生成多边形产生较大偏离度的问题(详见NavMesh的过程的**Generate Detailed Mesh**)，但是会消耗更加多的处理时间和内存占用,**该设置为核心设置，会影响其他所有的参数设置**
- 相关解释：
    体素
    - [体素 - 维基百科][10]

###cellHeight
- 描述：> 0，体素(voxel)在Y-plane(OpenGL坐标系)上的大小,决定体素(voxel)在对原始几何图形进行采样时的精细度，值越小越精细，越大越粗略,仅影响Y-Axis上的图形
- 影响：较小的值能够产生更加精确接近原始集合图形的网格，减少在生成导航网格过程中生成多边形产生较大偏离度的问题(详见NavMesh的过程的**Generate Detailed Mesh**，但是与cellSize不一样，较低的值并不会对内存消耗产生有显著影响)，有一点需要注意，**较低的值虽然能够使网格贴合原始图形，但是如果是凹凸不平的地形，较低的值可能会造成邻接的网格之间产生断裂，导致本来应该连在一起的网格造成分离**
- cellHeight设置较高时，可以看到网格和原始图形间距较大![cellHeight-high][11]
- cellHeight设置较小时，可以看到网格和原始图形贴合比较紧密![cellHeight-low][12]

###minTraversableHeight
- 描述：> 0, 最低可通过高度，设定从底部边界到顶部边界之间的最低高度，该高度为模型可通过的高度
- 影响：会影响场景中一些地形或者组件的可通过范围，比如桌子，如果设定值过低，会导致模型高度就算高过桌子也会从桌子中间传过去，设置过高会导致部分不被准许通过的场景也能穿过，**另外一点，minTraversableHeight的设定值的大小至少得是cellHeight的2倍**
- 当minTraversableHeight设置恰当时，网格不会跑到桌子下面去![minTraversableHeight-right][13]
- 当minTraversableHeight设置的值过低时，网格会跑到桌子下面去![minTraversableHeight-lower][14]

###maxTraversableStep
- 描述：> 0,可跨越不同地形时的高度，设定像是从普通平面移动到楼梯这样的地形是否可通过的高度阀值
- 影响：过低的值可能会导致无法通过原本可通过的地形，比如从平面到楼梯，导航网格生成时可能会发现楼梯的导航网格断裂缺失，导致无法楼梯寻路，设定值过高会导致原本不该通过的小物件能够被角色跨越,**同样，值设定必须大于cellHeight的2倍**
- 当maxTraversableStep设置的值比较恰当时，楼梯表面有网格生成![maxTraversableStep-correct][15]
- 当maxTraversableStep设置的值过低时，楼梯表面没有网格生成![maxTraversableStep-lower][16]
- 当maxTraversableStep设置的值过高时，结果不需要有网格的地方也会生成网格![maxTraversableStep-high][17]

###maxTraversableSlope
- 描述：> 0,最大可通过的斜坡的倾斜度
- 影响：过低的值会导致无法通过原本可通过的地形
- 当maxTraversableSlope设置的值恰当时，网格能够延生到斜面上去![maxTraversableSlope-correct][18]
- 当maxTraversableSlope设置的值过低时，网格没能延生到斜面上![maxTraversableSlope-lower][19]

###clipLedges
- 描述：None, 边缘突出部分是否可以行走
- 影响：

###traversableAreaBorderSize
- 描述：>= 0,可行走区域与阻挡物之前的距离大小
- 影响：**值设定必须大于cellSize的2倍才能产生效果**(因为导航网格的生成的实际上是建立在由voxel所组成的三维世界中)，当开启clipLedges时，实际的border会比较大
- 当traversableAreaBorderSize > 0时，无法行走的边界较大![traversableAreaBorderSize-larger][20]
- 当traversableAreaBorderSize == 0时，无法行走的边界几乎没有，很小![traversableAreaBorderSize-littel][21]

###smoothingThreshold
- 描述：>= 0,当产生用于表示派生区域的距离场时，会被使用该值，
- 影响：这个值会影响区域的形成结构和border检测，通常该值越大得到的区域会越大，该值越小产生的三角形越削瘦，该值还会是的border的尺寸变大，smoothingThreshold的值会直接影响border大小，当smoothingThreshold设置为0时，border依靠在墙和栏杆边，在相同的traversableAreaBorderSize值情况下，smoothingThreshold设置大于0时，会显著增大边界与墙壁或者栏杆的距离
- 相关解释：
    距离场：
    - [Distance transform -Wikipedia][22]
    - [图像处理之距离变换][23]
- 当smoothingThreshold == 0时，可以看到很多削长的三角形，同时网格边界也是标准的![smoothingThreshold-zero][24]![smoothingThreshold-standard-border][25]
- 当smoothingThreshold == 2时，图像中拥有更高的三角形结构,但是同时网格边界也变大了![smoothingThreshold-better-formation][26]![smoothingThreshold-bigger-border][27]

###useConservativeExpansion
- 描述：None, 应用一些算法来避免产生残缺的区域
- 影响：
- 当没有启用useConservativeExpansion时，生成的区域会是一个复杂的多边形区域，而且后面的三角化算法还没法儿处理，这片区域会是松散的网格
![useConservativeExpansion-disable][28]

###minUnconnectedRegionSize
- 描述：> 0, 最小的无法被连接的区域（这里的区域指的是在网格生成之前，某些要与其他区域连接的区域）
- 影响：当一个区域小于设定的这个minUnconnectedRegionSize，在生成网格时不会被考虑进去，也就是说这些区域上面不会生成网格
- 举个例子：在某些场景中，一些物件是不能被寻路的，比如固定场景中的一些岩石等等，当minUnconnectedRegionSize的值设置恰当时，在岩石等这些不能被寻路的物件上面不会生成网格，这样寻路时就不会走这里，当minUnconnectedRegionSize的值设置的过分小时，可能让岩石也生成了一些孤立的小网格（在官方资料中被称为island，就像一些孤岛一样，这里无法被寻路，但是确也生成了网格, 如果游戏中准许actor通过瞬移的方式进入孤岛，那么这里生产的nav网络就是有意义的）
- 当minUnconnectedRegionSize设置恰当时，桌面上不会生成网格,即桌面上不是可行走的![minUnconnectedRegionSize-correct][29]
- 当minUnconnectedRegionSize设置过低时,桌面上会生成一些没有必要的网格![minUnconnectedRegionSize-fault][30]

###mergeRegionSize
- 描述：> 0, 合并区域尺寸，当一个区域小于该尺寸时，如果可以，则会被合并进一些大的区域
- 影响：合理设置值可以规避一些区域产生算法生出一些不必要的小三角形的问题，当该值过小时，会产生一些又细又长的三角形，产生一些很细小的区域，需要把这个值设定到一个合理的大小
- 当mergeRegionSize过小时，网格会有很多削长的三角形![mergeRegionSize-lower][31]
- 当mergeRegionSize比较大时，足够大到能够消除一些削长三角形时![mergeRegionSize-large][32]

###maxEdgeLength
- 描述：>= 0, 指示网格边界的多边形最大的边
- 影响：当设置该值>0时，会在长的多边形边界上增加顶点，同时生产新的边，这样有助于减少相当数量的削长的三角形，当该值set to 0时，会关闭该特性
- 当特性无效时
![maxEdgeLength-disable][33]
- 当特性有效时
![maxEdgeLength-enable][34]

###edgeMaxDeviation
- 描述：>= 0, 网格边界与原始集合图形的偏离量
- 影响：值越小，生成的网格三角形越多越密，也越贴近原始几何图形,同样也会消耗更多的资源来做这些工作
- 当表面边缘匹配完全被关闭时
![edgeMaxDeviation-closed][35]
- 当表面边缘匹配被开启时
![edgeMaxDeviation-open][36]

###maxVertsPerPoly
- 描述：>= 3, 每个多边形的最大顶点数
- 影响：越高的值意味着越复杂的多边形，也意味着越高的性能消耗，**通常情况下6个顶点能够平衡需求和性能**

###contourSampleDistance
- 描述：>= 0, 设置采样距离，类似游戏中的凹凸贴图类似概念，**用于NavMesh过程中的Generate Detailed Mesh**,匹配原始集合图形表面的网格（利用生成更加精细的三角形保证网格来贴合那些凹凸不平的地表）
- 影响：越高的值意味着越贴近原始几何图形表面的最终网格，也意味着越高的性能消耗，**当值在0.9以下时会关闭这个特性**
- 当该特性关闭时,可以看到多边形的边缘一直沿着原始几何图形的表面，但是中央区域缺没有网格
![contourSampleDistance_off][37]
- 当该特性设置为中等程度时，可以发现添加了更多的三角形在中央网格区域![contourSampleDistance_moderate][38]
- 当该特性设置为高程度时，可以发现生成了更多三角形紧密贴合原始图形![contourSampleDistance_high][39]


###contourMaxDeviation
- 描述：>= 0, 设置最大的采样偏移距离，最好和**contourSampleDistance**结合起来看，其效果的精确度受到**contourSampleDistance**的影响
- 影响：当**contourSampleDistance**为0时，这个特性选项无效,在实际使用时发现在这个值越接近0，其生成的网格就越偏离原始图形

---
##NavMesh的生成过程
>- Voxelization - Create a solid heightfield from the source geometry.
>- Generate Regions - Detect the top surface area of the solid heightfield and divide it up into regions of contiguous spans.
>- Generate Contours - Detect the contours of the regions and form them into simple polygons.
>- Generate Polygon Mesh - Sub-divide the contours into convex polygons.
>- Generate Detailed Mesh - Triangulate the polygon mesh and add height detail.

###Voxelization（体素化）
- 把原始的几何图形转换成高度域
- 相关解释：
    高度域：
    - [Heightmap - From Wikipedia, the free encyclopedia][40]
    - [Unity中基于高度图的地形系统总结][41]

###Generate Regions
- 生成各个多边形区域(目前生成的多边形有多个小网格组成，边缘都还带有网格，在下一步中会进行简化)，生成的区域主要为简单的多边形(使用**分水岭算法**生成),分水岭算法还会生成很多NULL regions
- 分水岭算法基本在这一步骤中的基本过程，Using the watershed analogy, the spans which are furthest from a border represent the lowest points in the watershed. A border span represents the highest possible water level. The main loop iterates, starting at the lowest point in the watershed, then increments with each loop until the highest allowed water level is reached.

###Generate Contours([Contour Generation Detail][42])
- 生成各个多边形的边界轮廓并标记起来，在上一步骤中，生成的多边形由于是由很多的小网格组成，则会有十分多的小网格顶点，这里实际上不需要这么多顶点，只需要不同区域之前划分的边界处的必要顶点即可，这里需要通过**Douglas-Peucker algorithm**简化这些边缘轮廓，基本思路就是把多个必要点(mandatory vertices)之间密密麻麻的小点近似成一条直线，这条直线就是简化后的不同的regions区域.

###Convex Polygon Generation([Convex Polygon Generation][43])
- 合并重复边界轮廓信息，整合到网格里面去 
- (**这一步骤中的主要重点工作**)由于凹多边形在对于导航网格而言是无用的，这一步骤需要对上面一步中生成的多边形中的凹多边形进行子分割其边界轮廓，让其分化成凸多边形，主要步骤参考此步骤链接
- 聚集各个多边形之间的邻接信息（即多边形邻接信息）

###Detail Mesh Generation([Detail Mesh Generation][44])
- 前面步骤中生成的多边形网格在高度域上可能会与原始网格表现不一致（主要是在y轴方向上），生成的多边形网格与原始网格会发生偏离，这一步中就是为了修正这些偏移
- 对于每一个多边形：
    - 对多边形进行外形边缘进行采样，为那些偏离到一定高度补丁值得多边形添加顶点,这些顶点用于采样分割
    - 对多边形进行德劳内三角化
    - 对多边形内表面进行采样，为那些偏离到一定高度补丁值得多边形添加顶点
    - 继续对新增顶点的多边形进行德劳内三角化
- 更多信息参考上面此步骤链接
- 相关知识：**德劳内三角化**(最大化三角形内其最小角的角度，避免出现极度"削瘦"的三角形), 

---
#参考资料
- [CritterAI Documentation][45]
- [Navigation and Pathfinding][46]
- [CSDN博客，其中Unity部分总结不错][47]


  [1]: http://static.zybuluo.com/wuxinwei/8zok6ub1vbtms7aqv9pssmia/QQ%E6%88%AA%E5%9B%BE20151124105735.png
  [2]: https://groups.google.com/forum/#!forum/recastnavigation
  [3]: http://www.stevefsp.org/projects/rcndoc/prod/index.html
  [4]: https://github.com/memononen/recastnavigation
  [5]: http://masagroup.github.io/recastdetour/
  [6]: http://www.critterai.org/projects/cainav/doc/#
  [7]: https://en.wikipedia.org/wiki/A*_search_algorithm
  [8]: https://www.youtube.com/watch?v=KNXfSOx4eEE
  [9]: http://www.critterai.org/projects/nmgen_study/config.html
  [10]: https://zh.wikipedia.org/wiki/%E9%AB%94%E7%B4%A0
  [11]: http://www.critterai.org/projects/nmgen_study/media/images/floorplan_heightoffset_bad.jpg
  [12]: http://www.critterai.org/projects/nmgen_study/media/images/floorplan_heightoffset_good.jpg
  [13]: http://www.critterai.org/projects/nmgen_study/media/images/main_walkheight_norm.jpg
  [14]: http://www.critterai.org/projects/nmgen_study/media/images/main_walkheight_low.jpg
  [15]: http://www.critterai.org/projects/nmgen_study/media/images/main_maxstep_norm.jpg
  [16]: http://www.critterai.org/projects/nmgen_study/media/images/main_maxstep_low.jpg
  [17]: http://www.critterai.org/projects/nmgen_study/media/images/main_maxstep_high.jpg
  [18]: http://www.critterai.org/projects/nmgen_study/media/images/ramp_walkableslope_normal.jpg
  [19]: http://www.critterai.org/projects/nmgen_study/media/images/ramp_walkableslope_low.jpg
  [20]: http://www.critterai.org/projects/nmgen_study/media/images/main_bordersize_on.jpg
  [21]: http://www.critterai.org/projects/nmgen_study/media/images/main_bordersize_off.jpg
  [22]: https://en.wikipedia.org/wiki/Distance_transform
  [23]: http://blog.csdn.net/jia20003/article/details/8932489
  [24]: http://www.critterai.org/projects/nmgen_study/media/images/main_smoothing_off.jpg
  [25]: http://www.critterai.org/projects/nmgen_study/media/images/main_bordereffect_false.jpg
  [26]: http://www.critterai.org/projects/nmgen_study/media/images/main_smoothing_on.jpg
  [27]: http://www.critterai.org/projects/nmgen_study/media/images/main_bordereffect_true.jpg
  [28]: http://www.critterai.org/projects/nmgen_study/media/images/main_conservexpand_flow.jpg
  [29]: http://www.critterai.org/projects/nmgen_study/media/images/main_minregion_good.jpg
  [30]: http://www.critterai.org/projects/nmgen_study/media/images/main_minregion_bad.jpg
  [31]: http://www.critterai.org/projects/nmgen_study/media/images/main_mergeregion_off.jpg
  [32]: http://www.critterai.org/projects/nmgen_study/media/images/main_mergeregion_on.jpg
  [33]: http://www.critterai.org/projects/nmgen_study/media/images/main_maxedge_off.jpg
  [34]: http://www.critterai.org/projects/nmgen_study/media/images/main_maxedge_on.jpg
  [35]: http://www.critterai.org/projects/nmgen_study/media/images/odd_contour_off.jpg
  [36]: http://www.critterai.org/projects/nmgen_study/media/images/odd_contour_maxedge.jpg
  [37]: http://www.critterai.org/projects/nmgen_study/media/images/contour_contour_off.jpg
  [38]: http://www.critterai.org/projects/nmgen_study/media/images/contour_contour_mod.jpg
  [39]: http://www.critterai.org/projects/nmgen_study/media/images/contour_contour_high.jpg
  [40]: https://en.wikipedia.org/wiki/Heightmap
  [41]: http://blog.csdn.net/haohan_meng/article/details/21316429
  [42]: http://www.critterai.org/projects/nmgen_study/contourgen.html
  [43]: http://www.critterai.org/projects/nmgen_study/polygen.html
  [44]: http://www.critterai.org/projects/nmgen_study/detailgen.html
  [45]: http://www.critterai.org/projects/cainav/doc/
  [46]: http://docs.unity3d.com/Manual/Navigation.html
  [47]: http://blog.csdn.net/haohan_meng