using System;
using System.Reflection;
using System.IO;
using System.Text;

struct SERVICE_PTR
{
};


interface SceneManagerService
{
	// ���س���
	bool Load() ;

	// ��������
	// @param mapID : �����ͼID��ͬһ�ŵ�ͼ���Դ����������
	int createScene( int mapID ) ;

	// �ر�һ������,ֱ�ӵ��ó�����releaseҲ���Թر�
	bool closeScene( int sceneID ) ;

	// ��ȡ��������
	SERVICE_PTR getScene( int sceneID ) ;

	// ��ȡ�������е�ǰ��������
	int getSceneCount() ;

	// ��ȡ���г����б�
	unsafe int getAllScenes( SERVICE_PTR* pReturnArray,int nArraySize ) ;

	void release() ;
};
