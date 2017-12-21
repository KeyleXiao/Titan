using System;
using System.Reflection;
using System.IO;
using System.Text;

struct ibuffer
{
};

struct obuf
{
};

struct CSID  
{
};

struct CGID  
{
};

struct ServerData
{
};

struct DWORD
{
};

struct WORD
{
};


interface CenterConnectorService
{
		/**
		@name                 : �����ķ��������·�������Ϣ
		@brief                :
		@param  pServerInfo   : ��������Ϣ����,��ͬ�ķ������в�ͬ����չ��Ϣ,����������������и÷��������������ID��
		@param  wLen          : ��������Ϣ����
		@return               :
		*/
		bool UpdateServerInfo(ibuffer info);

		/**
		@name         : ȡ�ñ��ط����������ķ������ϵ�ID
		@brief        :
		*/
		CSID  GetLocalServerCSID();

		/**
		@name         : ȡ�����������з������ĸ���
		@brief        :
		*/
		DWORD GetAllServersCount();

		/**
		@name                : ȡ�÷������б�
		@brief               :
		@param serverPtrArray: ������ָ������,��������ָ���������Ϣ�ṹ��ָ��
		@param dwArratSize   : ָ������Ĵ�С,��ֹԽ��
		@param GroupFilter   : ����ID���й���,���GroupFilterΪINVALID_SERVER_ID���ʾ�����������
		@param wTypeFilter   : �����������͹���,���wTypeFilterΪMSG_SERVERID_UNKNOW���ʾ�����з��������͹���
		@note                : ���緵����ID����1���������ط�����:
		@note                : ServerInfo * serverPtrArray[MAX_SERVER_COUNT];
		@note                : DWORD count=ICenterServerConnector::GetServerList(serverPtrArray,1024,1,MSG_SERVER_GATE);
		@return              :
		*/
		unsafe DWORD GetServerList(ServerData ** serverPtrArray,DWORD dwArratSize,CGID GroupFilter,WORD wTypeFilter);

		/**
		@name                : ȡ�÷�������Ϣ
		@brief               :
		@param  ServerID     : ������ID
		@return              : ���Ŀ������������򷵻ط�������Ϣ,���򷵻�0
		*/
		ServerData GetServerInfo(CSID ServerID);

		/**
		@name                : ���ҳ���������
		@brief               :
		@param    groupID    : ��ID
		@param    dwMapID    : ��ͼID
		@param    x          : λ�� (��Ϊͬһ����ͼ�����ɶ����������ͬά��,���Ի���Ҫ����λ��)
		@param    y          : λ��
		@return              :
		*/
		ServerData FindZoneServer(CGID groupID,DWORD wMapID,DWORD x,DWORD y);

		/**
		@name                : ��ĳ������������һ����Ϣ
		@brief               : ������֮�����Ϣת��
		@param DestServerID  : Ŀ�������ID
		@param pMessage      : ��Ϣ����
		@param wMessageLen   : ��Ϣ����
		@return              : �Ƿ�ɹ����������ķ�����
		*/
		bool  PostMessage(CSID DestServerID,ibuffer msg);

		/**
		@name                : �����������㲥��Ϣ
		@brief               :
		@param DestServerArray:Ŀ�����������
		@param wDestServerCount:Ŀ�����������
		@param pMessage      : ��Ϣ����
		@param wMessageLen   : ��Ϣ����
		@return              : �Ƿ�ɹ����������ķ�����
		*/
		bool  BroadcastMessage(ibuffer id_buff,ibuffer msg);
};
