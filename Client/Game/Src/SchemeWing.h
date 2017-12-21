#pragma once
#include "ISchemeCenter.h"
#include "ISchemeEngine.h"
#include "EntityDef.h"

using namespace rkt;

class CSchemeWing : public ISchemeWing, public ISchemeUpdateSink
{
public:
     /////////////////////////ISchemeWing/////////////////////////
	/** ȡ�ó��������Ϣ
	@param nID:	���ID
	@return ���������Ϣ
	*/
	virtual SWing* getWing(int nID);

    bool LoadScheme();

    void Close(void);



    virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

private:
    // ����
    typedef map<int, SWing>		SCHEME_MAP;
    SCHEME_MAP					m_schemeMap;
};