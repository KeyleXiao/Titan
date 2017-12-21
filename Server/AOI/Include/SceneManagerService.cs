using System;
using System.Reflection;
using System.IO;
using System.Text;

struct SERVICE_PTR
{
};


interface SceneManagerService
{
	// 加载场景
	bool Load() ;

	// 创建场景
	// @param mapID : 传入地图ID，同一张地图可以创建多个副本
	int createScene( int mapID ) ;

	// 关闭一个场景,直接调用场景的release也可以关闭
	bool closeScene( int sceneID ) ;

	// 获取场景对象
	SERVICE_PTR getScene( int sceneID ) ;

	// 获取服务器中当前场景总数
	int getSceneCount() ;

	// 获取所有场景列表
	unsafe int getAllScenes( SERVICE_PTR* pReturnArray,int nArraySize ) ;

	void release() ;
};
