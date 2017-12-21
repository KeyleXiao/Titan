#pragma once
#include "Task.h"
#include "PerformanceStat.h"

struct SCost
{
    int     nCallTimes;
    float   fTotalTimeCost;
    
    SCost()
    {
        Restore();
    }

    void Restore()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SCostStat
{
    std::map<int, SCost> mapControllerCommandCost; 
    SCost costLoadModel;
    SCost costCreateEntityView;
    SCost costDestroyEntityView;
    std::map<int, SCost> mapEntityCommandCost;
    SCost costLoadPrefab;
    SCost costLoadSkin;

    LARGE_INTEGER liStart;
    
    SCostStat()
    {
        Restart();
    }

    void Restart()
    {
        mapControllerCommandCost.clear();
        costLoadModel.Restore();
        costCreateEntityView.Restore();
        costDestroyEntityView.Restore();
        mapEntityCommandCost.clear();
        costLoadPrefab.Restore();
        costLoadSkin.Restore();

        QueryPerformanceCounter(&liStart);
    }
};

class CRenderViewTask : public CTask
{
public:
    void init(void* pEntry, int nIndex);
	void sendControllerCommand(int nCmdID,int nParam,const char* pszParam, void* pParam, int nLen);
	void loadModel(EntityViewItem& ite); 
	void createEntityView(EntityViewItem& ite);
	void destroyEntityView(int nEntityID);
	void sendEntityCommand(int nEntityID, int nEntityType, int nCmdID, int nParam, const char* pszParam, void* pParam, int nLen);
    void loadPrefab(EntityViewItem& ite);
    void loadSkin(EntityViewItem& ite);

    virtual void print(void);
protected:
    virtual void onStart();
    virtual void onStop();
    virtual void doMessage(Message* pMsg);
    virtual int  mytype() { return TASK_VIEW; }

private:
	int m_nIndex;
    IRenderViewStruct m_RenderViewEntry;

    SCostStat m_costStat;
    void PrintCostStat();

#ifdef PERFORMANCE_CLIENT_TEST
    CPerformanceStat  m_entityCmdStat;
    int               m_nCmdFlag;
    bool              m_isOpenCmdStat;
#endif
};