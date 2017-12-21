using System;
using System.Reflection;
using System.IO;
using System.Text;

struct UID
{
};

struct Vector3
{
};

struct PACKAGE_PTR
{
};


/**
	职责: 
	1.接受用户连接，创建Actor服务
	2.将收到的网络消息，转发给Actor处理
	3.根据UID,获取Actor代理接口
*/
interface ActorService
{
	//////////////////////////////// IEntity ///////////////////////////////////////////////
	// 取得实体ID
	UID getUID();

	// 取得实体名称
	string getName();

	// 取得当前所在场景ID
	int getSceneID();

	// 取得实体位置
	Vector3 getLocation();

	// 收到网络消息
	void handleMessage( PACKAGE_PTR msg );

	// 发送数据给客户端
	int send2Client( PACKAGE_PTR package);
	
	// 设置消息队列
	unsafe bool setMessageQueue(void * msg_queue);

	/////////////////////// IEntityProperty ///////////////////////////////////////////////
	// 取得某个属性的类型
	string getPropertyType( int prop_id );

	// 取得某个属性的标志数据(通常是序列化标志)
	int getPropertyFlag( int prop_id );

	// 获取实体属性(布尔类型）
	bool getProperty_Bool( int prop_id);

	// 设置实体属性(布尔类型）
	bool setProperty_Bool( int prop_id,bool value);

	// 获取实体属性(数字,实际内部支持用int,byte,short三种类型存)
	int getProperty_Integer( int prop_id);

	// 设置实体属性(数字,实际内部支持用int,byte,short三种类型存)
	bool setProperty_Integer( int prop_id,int value);

	// 增加属性数值(注意内部需要控制上下限)
	bool addProperty_Integer( int prop_id,int delta);

	// 获取实体属性(字符串)
	string getProperty_String( int prop_id );

	// 设置实体属性(字符串)
	bool setProperty_String( int prop_id,string value);

	// 获取实体属性(自定义结构,如果属性存在返回)
	string getProperty_custom( int prop_id );

	// 设置实体属性(自定义结构)
	bool setProperty_custom( int prop_id,string data );

	/////////////////////// IEntityAOI ///////////////////////////////////////////////
	// 取得视野半径
	int getSight();

	// 调整视野半径
	void setSight( int radii );

	// 是否隐身
	bool isHide();

	// 设置隐身状态
	void setHide( bool state );

	void release();
};