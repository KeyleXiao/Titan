#include "stdafx.h"
#include "LuaService_Proxy.h"
#include "StubMacroDef.h"

////// ILuaService ////////////////////////////////////////////////////////////////////
/** ִ��һ��lua�ű�
@param   szLuaFileName �� lua�ļ���
@return  �ɹ�������true,ʧ�ܣ�false
*/
void LuaService_Proxy::LoadLuaFile(const char* szLuaFileName) 
{
	if (m_pContainer == 0)
	{
		return;
	}
	obuf256 t_data;
	t_data << szLuaFileName;

	BUILD_MSG_BUFFER( ILuaService::LoadLuaFile,t_data);

	m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
};

//////////////////////////////////////////////////////////////////////////////////
/** ִ��һ���ڴ���lua
@param   pLuaData : �����lua ����
@param   nDataLen �����ݳ���
@return  �ɹ�������true,ʧ�ܣ�false
*/
void LuaService_Proxy::RunMemoryLua(const char* pLuaData, int nDataLen)
{
	if (m_pContainer == 0)
	{
		return;
	}
	obuf256 t_data;
	t_data << pLuaData << nDataLen;

	BUILD_MSG_BUFFER( ILuaService::RunMemoryLua,t_data);

	m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
};

//////////////////////////////////////////////////////////////////////////////////
/** ִ��һ��lua����
@param   szFuncName : lua������
@param   pInParam : �����������ָ��
@param   nInNum : �����������
@param   pOutParam : �����������ָ��
@param   nOutNum : �����������
@return  �ɹ�������true,ʧ�ܣ�false
*/
void LuaService_Proxy::RunLuaFunction(const char* szFunName, CLuaParam * pInParam, int nInNum/*, CLuaParam * pOutParam, int nOutNum*/)
{
	if (m_pContainer == 0)
	{
		return;
	}
	obuf256 t_data;
	t_data << szFunName;	// LUA����
	t_data << nInNum;	// �����������
	if (nInNum > 0)
	{
		for (int i=0; i<nInNum; ++i)
		{
			t_data << pInParam[i];
		}
	}

	BUILD_MSG_BUFFER( ILuaService::RunLuaFunction,t_data);

	m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
};

/** �Ƿ����ĳ������
*/
bool LuaService_Proxy::IsFunction(const char * szFunName)
{
	if (m_pContainer == 0)
	{
		return false;
	}
    obuf256 t_data;
	t_data << szFunName;

	BUILD_MSG_BUFFER( ILuaService::IsFunction,t_data);

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf,MSG_FLAG_READ_ONLY);
	RETURN_FROM_MSG_BUF(bool);
	return false;
};


/** ˢ��lua�ű�
@param   szLuaFileName �� lua�ļ���
@return  �ɹ�������true,ʧ�ܣ�false
*/
void LuaService_Proxy::UpdateLuaFile(const char* szLuaFileName)
{
	if (m_pContainer == 0)
	{
		return;
	}
    obuf256 t_data;
    t_data << szLuaFileName;

    BUILD_MSG_BUFFER( ILuaService::UpdateLuaFile,t_data);

	m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}


// �ͷ�
void LuaService_Proxy::release()
{
	if (m_pContainer == 0)
	{
		return;
	}
	obuf256 t_data;

	BUILD_MSG_BUFFER( ILuaService::release,t_data);

	m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
};