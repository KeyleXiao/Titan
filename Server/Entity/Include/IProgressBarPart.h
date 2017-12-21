/*******************************************************************
** �ļ���:	IProgressBarPart.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���� (Carl Chen)
** ��  ��:	2/26/2015
** ��  ��:	1.0
** ��  ��:	

			����������
********************************************************************/


#pragma once

#include "IEntity.h"
#include "IEventEngine.h"


// �����ֳ�
struct ProgressBarContext
{
	int         nWndStyle;                // ���ڷ��,���������������ʾ��ͬ�Ľ���������Ϊ��ͬ��λ�ã����ȵȵ�
	std::string szWndTxt;                 // ��������,������ҵ�ǰ������ʲô
	unsigned long  nTime;                 // ����ʱ��(����)  
	int			nBreakFlag;               // ��ϱ�ʶ
	IEventExecuteSink * pCallBack;        // �ص��ӿ�,�ɹ��ᷢ�� EVENT_ENTITY_PROGRESS_OK �¼�
	bool                bSendClient;      // �Ƿ��͸��ͻ���,Ĭ��Ϊtrue,����Ϊ�˼������ݰ�����������Ҫ���Ĵ���߼�
	UID                 uidTarget;       // �����ϱ�ʶ�а��������⣬���������������д����ö��󳬳�һ����������
	float               fDistanceRange;   // �����ϱ�ʶ�а��������⣬���������������д
    int                 nSpellID;         // ����ID������ͬ���ܴ�ϣ���Ϊ��Щ��������ʩ����ϣ�Ȼ��ʹ���������д�Ͻ�����
};

/**
	���������� : ������
	1�����ܶ���
	2��������Ʒ����
	3�����ؾ߶���
	�ȵ�һ����Ҫ�����ĳ���
*/

struct __IProgressBarPart : public __IEntityPart
{
	
	/**
	����������
	@param context   : �����ֳ�
	@return          : ��ǰ���ڶ����򷵻�false,�ɽ����¼�
						EVENT_ENTITY_PROGRESS_OK,					// �����ɹ�
						EVENT_ENTITY_PROGRESS_BREAK,	     		// �����ж�

	@�ر�ע�⣺ �����˽������Ĳ���������ص�����Ҫ���٣������CloseProgressBar����ǰ�Ľ�����ֹͣ���������Ұָ��
	*/
	virtual bool StartProgressBar( ProgressBarContext * context ) = 0;

	/**
	�رս�����
	@param pSink : ���pSink=0��ֱ�ӹرս������������Ϊ0����ֻ��pSinkƥ���˲Źر�,��������Ұָ��
	@param bBreak: �Ƿ��ͱ�����¼�
	*/
	virtual void CloseProgressBar(IEventExecuteSink * pSink = 0,bool bBreak=true) = 0;

	/**
	ȡ�õ�ǰ�Ķ����ֳ�,Ϊ0���ʾ��ǰû�ж���
	*/
	virtual ProgressBarContext * GetCurrentContext() = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __IProgressBarPart * CreateEntityProgressBar();
#	define	CreateEntityProgressBarProc	CreateEntityProgressBar
#else													/// ��̬��汾
typedef __IProgressBarPart * (RKT_CDECL * procCreateEntityProgressBar)();
#	define	CreateEntityProgressBarProc	DllApi<procCreateEntityProgressBar>::load(MAKE_DLL_NAME(ENTITY), "CreateEntityProgressBar")
#endif

