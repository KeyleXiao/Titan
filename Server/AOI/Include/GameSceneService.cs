using System;
using System.Reflection;
using System.IO;
using System.Text;

struct AOI_PROXY_PTR
{
};

struct Vector3
{
};

struct BROADCAST_ID
{
};

interface GameSceneService
{
	// ��������,����ID�ͳ���������Ϣ
	bool onCreate( int mapID,int sceneID );

	// ��ȡΨһ����ID
	int getSceneID();

	// ��ȡ��ͼID��ͬһ�ŵ�ͼ���Դ����������
	int getMapID();

	// ����ʵ��
	bool insertEntity( AOI_PROXY_PTR proxy,Vector3 loc,int layer );

	// �Ƴ�ʵ��;
	bool removeEntity( AOI_PROXY_PTR proxy,Vector3 loc,int layer );

	// �ƶ�ʵ��
	bool moveEntity( AOI_PROXY_PTR proxy,Vector3 oldLoc,Vector3 newLoc,int layer );

	// ȡĳ����Χһ����Χ�ڵĸ����ڵ�����ʵ�� 
	// @param dist : ����뾶(ע�⣬���ﲻ�Ǿ�ȷƥ��ģ��ǰ�һ����Ԫ���ڵ����ж��󶼷���,���Ϊ��Ԫ���С)
	unsafe int k_nearest( Vector3 loc,int dist,AOI_PROXY_PTR* pReturnArray,int nArraySize,int layerMask );

	// ��Χ�㲥 
	int broadcast(Vector3 loc,int dist,BROADCAST_ID id,string msg,int layerMask);

	// ȫ����ҹ㲥
	int broadcast_all( BROADCAST_ID id,string msg );

	// ��ȡ�������
	int getPlayerCount();

	// ��ȡ������ҵĶ����б�
	// @param return : �����������ص������Ŀ
	unsafe int getAllPlayers( AOI_PROXY_PTR * pReturnArray,int nArraySize );

	// ���ٵ�ͼ
	void release();
};
