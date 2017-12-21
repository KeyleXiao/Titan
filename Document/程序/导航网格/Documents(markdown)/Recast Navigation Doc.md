# Recast Navigation Doc

标签（空格分隔）： 冰川工作

---
#目录结构
>* DebugUtils（Debug工具，作者为了便于测试所写的工具）
>* Detour（导航网格定义已经相关工具）
>* DetourCrowd（寻路，转向，碰撞检测等等）
>* DetourTileCache（Tile缓存，主要用于动态障碍，重新生成网格，为了快速动态rebuild网格而生）
>* Docs（项目介绍文档，不是API参考）
>* Recast（导航网格生成工具，包含了如何把原始地形的相关一步一步转换成导航网格）
>* RecastDemo（用于演示Recast&&Detour的demo小项目）

---
#功能模块划分
　　从大的模块划分来说，**Recast Navigation**分为2个大模块:
>* Recast模块（导航网格的生成相关，主要是原始Geometry数据处理以及相关处理算法），从命名方式上来说`rc`打头的各种调用以及变量均为该模块所属
>* Detour模块（寻路相关，包含有寻路路径搜索，碰撞检测等等）,从命名方式上来说`dt`打头的各种调用以及变量均为该模块所属
　　

接下来按照`VS2010`工程来对每个模块进行说明：
##Detour
　　该模块主要定义了`寻路所需的相关数据（导航网格）以及相关状态`，配合`Recast`使用，`Detour`并不限定一定要是`Recast`生成的网格，作者表示可以基于任何导航网格来使用`Detour`来寻路,当然配合`recast`来使用效果最佳，其主要文件以及比较重要的相关代码为以下：

- DetourAlloc.h
    - 导航网格相关的内存分配 

```c++
/// Allocates a memory block.
///  @param[in]		size	The size, in bytes of memory, to allocate.
///  @param[in]		hint	A hint to the allocator on how long the memory is expected to be in use.
///  @return A pointer to the beginning of the allocated memory block, or null if the allocation failed.
/// @see dtFree
void* dtAlloc(int size, dtAllocHint hint);

/// Deallocates a memory block.
///  @param[in]		ptr		A pointer to a memory block previously allocated using #dtAlloc.
/// @see dtAlloc
void dtFree(void* ptr);
```

- DetourCommon.h
    - 导航网格工具集,基本上都是一些`inline函数`以及`template函数`

- DetourNavMesh.h
    - `NavMesh`的定义，即导航网格的定义
    - `poly`多边形的定义
    - `Tile`的定义

```c++
/// A navigation mesh based on tiles of convex polygons.
/// @ingroup detour
class dtNavMesh

/// Defines a polyogn within a dtMeshTile object.
/// @ingroup detour
struct dtPoly

/// Defines a navigation mesh tile.
/// @ingroup detour
struct dtMeshTile
```

- DetourNavMeshQuery.h
    - `Navmesh`的相关特征获取
    - 寻路路径的计算
    - 各种`寻路路径`获取方式的定义

```c++
/// Provides the ability to perform pathfinding related queries against
/// a navigation mesh.
/// @ingroup detour
class dtNavMeshQuery
```

##DetourCrowd
　　此模块看成是进行障碍避免,寻找路径走廊等操作
```c++
Todo
```

##DetourTileCache
　　此模块专为处理动态障碍而生，`dtTileCache`基本上在内容上与标准导航网格类似，但是为了在`runtime`期间加速网格的生成，存储了额外的一些信息，但是要重新生成`网格`的过程中依然需要`原始geometry数据`

- DetourTileCache.h

```c++
//压缩过的导航瓷砖
struct dtCompressedTile
{
	unsigned int salt;						///< Counter describing modifications to the tile.
	struct dtTileCacheLayerHeader* header;
	unsigned char* compressed;
	int compressedSize;
	unsigned char* data;
	int dataSize;
	unsigned int flags;
	dtCompressedTile* next;
};

class dtTileCache
```

##Recast
　　导航网格生成工具集,包含依靠原始geometry一步一步生成导航网格的所有步骤,这一部分的介绍以及原理请参考另外一份编写的`导航网格研究`文档

##RecastDemo
　　`Recast Navigation`的演示项目,Demo中大部分演示Sample都是从`class Sample`(`Sample.h`)中继承下来的，一般而言,代码如下:
```c++
class Sample
{
...
    virtual void handleSettings();
	virtual void handleTools();
	virtual void handleDebugMode();
	virtual void handleClick(const float* s, const float* p, bool shift);
	virtual void handleToggle();
	virtual void handleStep();
	virtual void handleRender();
	virtual void handleRenderOverlay(double* proj, double* model, int* view);
	virtual void handleMeshChanged(class InputGeom* geom);
	virtual bool handleBuild();  //大部分初始化过程，都在这里进行，比如导航网格创建,动态障碍中的TileCache创建等
	virtual void handleUpdate(const float dt);
...
```

- 想了解导航网格生成的全过程，请看`Sample_SoloMesh.h`,其主要build过程请参看以下函数:

```c++
bool Sample_SoloMesh::handleBuild()
```

- 想了解`Recast Navigation`如何寻路的，请看`NavMeshTesterTool.h`，其主要工作流程请参看以下函数:

```c++
void NavMeshTesterTool::handleToggle()
```

- 想了解`Recast Navigation`如何做到动态障碍检测的,请看`Sample_TempObstacle.h`,这里使用的是`dtTileCache`的方式实现的

##Demo的VS2010工程
- Demo需要使用premake4来建立VS2010工程，premake4已经在导航网格资料中给出，需要把`premake4.exe`可执行程序复制到`RecastDemo`目录下，运行命令行`premake4 vs2010`,即可在`RecastDemo\Build`目录下建立VS2010工程
- 编译Demo需要你SDL第三方DLL，该DLL也已经在导航网格资料中给出，请将`SDL`目录放置于`\RecastDemo\Contrib\`目录下即可
- Debug模式编译偶尔会遇到闪退的情况，目前原因未知

#Recast Navigation在实际开发中遇到的问题
- **DetourNavmesh**中,Tile中保存的顶点可能有重复，在动态障碍开发中，对多边形进行切割这一步里面，在原始多边形中取顶点信息，会发现相当一部分多边形顶点信息有重复的相同点坐标（多于2个），这里需要研究为什么会有多余的点

---
#相关资料
- [Recast Navigation API Reference][1]
- [Recast Navigation Github地址][2]

  [1]: http://www.stevefsp.org/projects/rcndoc/prod/index.html
  [2]: https://github.com/recastnavigation/recastnavigation