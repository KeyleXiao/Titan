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
	ְ��: 
	1.�����û����ӣ�����Actor����
	2.���յ���������Ϣ��ת����Actor����
	3.����UID,��ȡActor����ӿ�
*/
interface ActorService
{
	//////////////////////////////// IEntity ///////////////////////////////////////////////
	// ȡ��ʵ��ID
	UID getUID();

	// ȡ��ʵ������
	string getName();

	// ȡ�õ�ǰ���ڳ���ID
	int getSceneID();

	// ȡ��ʵ��λ��
	Vector3 getLocation();

	// �յ�������Ϣ
	void handleMessage( PACKAGE_PTR msg );

	// �������ݸ��ͻ���
	int send2Client( PACKAGE_PTR package);
	
	// ������Ϣ����
	unsafe bool setMessageQueue(void * msg_queue);

	/////////////////////// IEntityProperty ///////////////////////////////////////////////
	// ȡ��ĳ�����Ե�����
	string getPropertyType( int prop_id );

	// ȡ��ĳ�����Եı�־����(ͨ�������л���־)
	int getPropertyFlag( int prop_id );

	// ��ȡʵ������(�������ͣ�
	bool getProperty_Bool( int prop_id);

	// ����ʵ������(�������ͣ�
	bool setProperty_Bool( int prop_id,bool value);

	// ��ȡʵ������(����,ʵ���ڲ�֧����int,byte,short�������ʹ�)
	int getProperty_Integer( int prop_id);

	// ����ʵ������(����,ʵ���ڲ�֧����int,byte,short�������ʹ�)
	bool setProperty_Integer( int prop_id,int value);

	// ����������ֵ(ע���ڲ���Ҫ����������)
	bool addProperty_Integer( int prop_id,int delta);

	// ��ȡʵ������(�ַ���)
	string getProperty_String( int prop_id );

	// ����ʵ������(�ַ���)
	bool setProperty_String( int prop_id,string value);

	// ��ȡʵ������(�Զ���ṹ,������Դ��ڷ���)
	string getProperty_custom( int prop_id );

	// ����ʵ������(�Զ���ṹ)
	bool setProperty_custom( int prop_id,string data );

	/////////////////////// IEntityAOI ///////////////////////////////////////////////
	// ȡ����Ұ�뾶
	int getSight();

	// ������Ұ�뾶
	void setSight( int radii );

	// �Ƿ�����
	bool isHide();

	// ��������״̬
	void setHide( bool state );

	void release();
};