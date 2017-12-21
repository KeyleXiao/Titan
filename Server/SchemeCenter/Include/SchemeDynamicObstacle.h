/*******************************************************************
** �ļ���:	E:\work\w1\speed\Server\SchemeCenter\Src\SchemePathFindService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����ΰ
** ��  ��:	1/2/2016	18:36
** ��  ��:	1.0
** ��  ��:	
            ��̬�ϰ����ñ�
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeDef.h"
#include "SchemeCache.h"

using namespace std;

class CSchemeDynamicObstacle : public IDynamicObstacleInfo, 
                               public ISchemeUpdateSink,
                               public CSchemeCache<CSchemeDynamicObstacle>
{
    enum EMDynamicObsSchemeColunm
    {
        OBS_COL_ID = 0,						    // ��̬�ϰ���ID

        OBS_COL_SHAPE,							// ��̬�ϰ���״
        OBS_COL_LENGTH,                         // ������ı������ǳ��ȣ���������������������ε��ڽ�Բ�뾶
        OBS_COL_WIDTH,                          // ������ı������ǿ�ȣ���������������������ε��ڽ�Բ�뾶,�������ֵһ�� 
        OBS_COL_ANGLE,                          // ��̬�ϰ���ת�Ƕ�
        OBS_COL_ENTITY_CAMP_FLAG,				// ��̬�ϰ�ʵ���Ƿ�ɴ�Խ��־λ
        OBS_COL_SPELL_CAMP_FLAG,				// ��̬�ϰ�������Ӫ��־λ
        OBS_COL_SPELL_FLAG,						// ��̬�ϰ������Ƿ�ɴ�Խ��־λ
		OBS_COL_PRELOAD_FLAG,				    // ��̬�ϰ��Ƿ�Ԥ���أ������ڿͻ��˴���ǰ���ɣ�

        OBS_COL_MAX,                            // ��̬�ϰ��������
    };

	typedef map<unsigned int, DynamicObstacleScheme>	T_SchemeInfo;
public:
	///////////////////////////////IDynamicObstacleInfo///////////////////	
	virtual DynamicObstacleScheme * GetSchemeInfo(int nDynamicMonsterID);

	///////////////////////////////ISchemeUpdateSink///////////////////	
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	///////////////////////////////CSchemePathFindServiceInfo//////////////////////
	/** ����ű�
	@param   
	@param   
	@return  
	*/
	bool LoadScheme(void);

	/** �ر�
	@param   
	@param   
	@return  
	*/
	void Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeDynamicObstacle(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeDynamicObstacle(void);

private:
	// ����Ѱ·������������	
	T_SchemeInfo m_mapSchemeInfo;
};
