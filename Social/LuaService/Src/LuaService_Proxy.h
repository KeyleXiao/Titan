/*******************************************************************
** �ļ���:	LuaService_Proxy.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	12/27/2016
** ��  ��:	1.0
** ��  ��:	Lua�������
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "ILuaService.h"

using namespace rkt;
using namespace std;

class LuaService_Proxy : public ILuaService
{
public:
	////////////////// LuaService_Proxy ////////////////////////////////////////////////////////

	LuaService_Proxy() :m_pContainer(0) {};
	~LuaService_Proxy() {};
public:
	///////////////////////////////////ILuaService////////////////////////////
    /** ִ��һ��lua�ű�
	@param   szLuaFileName �� lua�ļ���
	@return  �ɹ�������true,ʧ�ܣ�false
	*/
	virtual void			LoadLuaFile(const char* szLuaFileName);

	/** ִ��һ���ڴ���lua
	@param   pLuaData : �����lua ����
	@param   nDataLen �����ݳ���
	@return  �ɹ�������true,ʧ�ܣ�false
	*/
	virtual void			RunMemoryLua(const char* pLuaData, int nDataLen);

	/** ִ��һ��lua����
	@param   szFuncName : lua������
	@param   pInParam : �����������ָ��
	@param   nInNum : �����������
	@param   pOutParam : �����������ָ��
	@param   nOutNum : �����������
	@return  �ɹ�������true,ʧ�ܣ�false
	*/
	virtual void			RunLuaFunction(const char* szFunName, CLuaParam * pInParam =0, int nInNum =0/*, CLuaParam * pOutParam =0, int nOutNum =0*/);

	/** �Ƿ����ĳ������
	*/
	virtual bool			IsFunction(const char * szFunName);

	/** ˢ��lua�ű�
	@param   szLuaFileName �� lua�ļ���
	@return  �ɹ�������true,ʧ�ܣ�false
	*/
	virtual void			UpdateLuaFile(const char* szLuaFileName);

    // �ͷ�
	virtual void			release();

public:
	SERVICE_PTR m_pContainer;
};