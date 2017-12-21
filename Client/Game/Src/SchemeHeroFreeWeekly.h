/*******************************************************************
** �ļ���:	SchemeHeroFreeWeekly.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	2016/8/23
** ��  ��:	1.0
** ��  ��:	����HeroFreeWeekly.csv

********************************************************************/
#pragma once

#include "SchemeDef.h"
#include "ISchemeCenter.h"
#include "ISchemeEngine.h"

class CSchemeHeroFreeWeekly : public ISchemeHeroFreeWeekly, public ISchemeUpdateSink
{
public:
	/////////////////////////////////ISchemeHeroFreeWeekly/////////////////////////////////////////
	// ȡ������Ӣ���б�
	virtual int				GetFreeHeroListByLv( int arrayFreeHero[], int arrayLen, int nLv );
	// �Ƿ�Ϊ����Ӣ��
	virtual bool			CheckIsWeekFree( const int nHeroID, const int nLv );

	/////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	virtual bool            OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool            OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////////////CSchemeHeroFreeWeekly/////////////////////////////////////////
	/** ����ű�
	@param   
	@param   
	@return  
	*/
	bool					LoadScheme(void);

	/** �ر�
	@param   
	@param   
	@return  
	*/
	void					Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeHeroFreeWeekly(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeHeroFreeWeekly(void);

private:
	time_t MakeTime(char * ch);

private:
	// ����Ӣ������
	typedef std::vector<SHeroFreeWeeklyScheme> SHeroFreeWeeklyVec;
	SHeroFreeWeeklyVec vecHeroFree;     
	
	// �ϴμ������Ӣ���б�
	static const int nFreeHeroArrayLen = 2 * HEROFREEWEEKLY_FREEID;
	int arrayFreeHeroID[nFreeHeroArrayLen];
};