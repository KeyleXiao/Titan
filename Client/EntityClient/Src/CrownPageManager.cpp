#include "StdAfx.h"
#include "CrownPageManager.h"


CCrownPageManager::CCrownPageManager()
{
	m_pMaster = NULL;
}
CCrownPageManager::~CCrownPageManager()
{
	m_pMaster = NULL;
}

bool CCrownPageManager::onSerialize(rkt::ibuffer& in)
{

	return true;
}

bool CCrownPageManager::deSerialize(rkt::obuf& out)
{

	return true;
}

bool CCrownPageManager::onLoad( IEntity* pEntity )
{
	m_pMaster = pEntity;

	
	return true;
}


void  CCrownPageManager::onMsgCrownAllGemstoneItem(LPCSTR pszMsg, int nLen)
{
	if(m_pMaster==NULL)
	{
		return ;
	}

	if (pszMsg == NULL)
	{
		return;
	}

	// Ч�鳤��ֵ
	if( nLen!= CROWN_GEMSTONE_MAX_COUNT*sizeof(GEMSTONE_ITEM) )
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<nLen<<"< AllGemstoneItem Size= "<< (CROWN_GEMSTONE_MAX_COUNT*sizeof(GEMSTONE_ITEM)) );
		return ;
	}

	obuf obData1;
	obuf obData2;
	cmd_count cmdCount;
	char *pGemstoneHead=(char*)(pszMsg);	
	for (int i=0;i<CROWN_GEMSTONE_MAX_COUNT;++i)
	{		
		GEMSTONE_ITEM* pGemstoneItem = (GEMSTONE_ITEM*)pGemstoneHead;
		pGemstoneHead = (char*)(pGemstoneItem+1);

		if(pGemstoneItem == NULL)
			continue;

		if(pGemstoneItem->nGemstoneDBID<=0)
			continue;

		cmd_Crown_Gemstone_Item_Info info;
		info.nGemstoneID = pGemstoneItem->nGemstoneID;
		info.nGemstoneDBID = pGemstoneItem->nGemstoneDBID;
		info.byGrade = pGemstoneItem->byGrade;
		info.nExp = pGemstoneItem->nExp;
		info.bAppraisal = pGemstoneItem->bAppraisal;
		info.nAppraisalCount = pGemstoneItem->nAppraisalCount;

		obData2.push_back(&info,sizeof(info));
		pGemstoneItem++;
		cmdCount.nCount++;
	}

	obData1.push_back(&cmdCount,sizeof(cmdCount));
	obData1.push_back(obData2.data(),obData2.size());

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_CROWNG_ALLGEMSTONE_UPDATE,0, 0, obData1.data(), obData1.size());
}

void  CCrownPageManager::onMsgCrownAllCrownPage(LPCSTR pszMsg, int nLen)
{
	if(m_pMaster==NULL)
	{
		return ;
	}

	if (pszMsg == NULL)
	{
		return;
	}

	// Ч�鳤��ֵ
	if(nLen != sizeof(CROWN_PAGE_ITEM)*CROWN_PAGE_MAX_COUNT )
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<nLen<<"< AllCrownPage Size= "<<(sizeof(CROWN_PAGE_ITEM)*CROWN_PAGE_MAX_COUNT) );
		return ;
	}

	obuf obData1;
	obuf obData2;
	cmd_count cmdCount;
	char *pPageHead=(char*)(pszMsg);	
	for (int i=0;i<CROWN_PAGE_MAX_COUNT;++i)
	{
		CROWN_PAGE_ITEM* pGemstoneItem = (CROWN_PAGE_ITEM*)pPageHead;
		pPageHead = (char*)(pGemstoneItem+1);

		if(pGemstoneItem == NULL)
			continue;

		if(pGemstoneItem->nPageDBID <=0)
			continue;

		cmd_Crown_Page_Item_Info info;
		info.nPageDBID =pGemstoneItem->nPageDBID;
		sstrcpyn(info.szName, pGemstoneItem->szName, sizeof(info.szName));
		memcpy(info.crownInlaySlotList, pGemstoneItem->crownInlaySlotList,sizeof(info.crownInlaySlotList));

		obData2.push_back(&info,sizeof(info));
		cmdCount.nCount++;
	}

    // ϵͳ�����º��ڷ��������Զ�����Ĭ�ϸ����ı�ʯҳ���ͻ��˲��������Ϸ�����
	//if(cmdCount.nCount == 0)
	//{
	//	int nMaxPageCount = min(DefaulCrownPageCount, 20);
	//	for(int i=0;i<nMaxPageCount;++i)
	//	{
	//		char szCrownPageName[GAME_NAME_MAXSIZE] = {0};				
	//		// ������һ��
	//		int nCurPageIndex=i+1;
	//		string sDefaultName;

	//		Msg_CrownPage_Save MsgData;
	//		MsgData.nPageDBID=nCurPageIndex;
	//		ssprintf_s(szCrownPageName, sizeof(szCrownPageName), "�˾�ģ��%d", nCurPageIndex);
	//		sstrcpyn(MsgData.szCrownPageName,a2utf8(szCrownPageName),sizeof(MsgData.szCrownPageName));
	//		// ��������������
	//		obuf obdata;
	//		obdata.push_back(&MsgData,sizeof(MsgData));
	//		SendMessageToEntityScene(m_pMaster->getUID(), PART_BANK, CS_MSG_ENTITY_CROWNSAVEPAGE, obdata.data(), obdata.size());

	//		cmd_Crown_Page_Item_Info info;
	//		info.nPageDBID=nCurPageIndex;
	//		ssprintf_s(info.szName, sizeof(info.szName), "�˾�ģ��%d", nCurPageIndex);
	//		sstrcpyn(info.szName,a2utf8(szCrownPageName),sizeof(info.szName));
	//		obData2.push_back(&info,sizeof(info));
	//		cmdCount.nCount++;
	//	}
	//}

	obData1.push_back(&cmdCount,sizeof(cmdCount));
	obData1.push_back(obData2.data(),obData2.size());

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_CROWNG_ALLCROWNPAGE_UPDATE,0, 0, obData1.data(), obData1.size());
}

void  CCrownPageManager::onMsgUpdateCrownGemstoneItem(LPCSTR pszMsg, int nLen)
{
	if(m_pMaster==NULL)
	{
		return ;
	}

	if (pszMsg == NULL)
	{
		return;
	}

	// Ч�鳤��ֵ
	int nEfficacyLen=sizeof(GEMSTONE_ITEM);
	if(nLen!=nEfficacyLen)
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<nLen<<"< GemstoneItem Size= "<<nEfficacyLen);
		return ;
	}

	ibuffer inBuffer(pszMsg, nLen);
	GEMSTONE_ITEM  crownGemstone; 
	inBuffer>>crownGemstone;

	if(crownGemstone.nGemstoneDBID<=0)
		return;

	cmd_Crown_Gemstone_Item_Info info;
	info.nGemstoneID = crownGemstone.nGemstoneID;
	info.nGemstoneDBID = crownGemstone.nGemstoneDBID;
	info.byGrade = crownGemstone.byGrade;
	info.nExp = crownGemstone.nExp;
	info.bAppraisal = crownGemstone.bAppraisal;
	info.nAppraisalCount = crownGemstone.nAppraisalCount;

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_CROWNG_GEMSTONE_UPDATE,0, 0, &info, sizeof(info));
}

void CCrownPageManager::onMsgAppraisalCrownGemstoneRet(LPCSTR pszMsg, int nLen)
{
	if(m_pMaster==NULL)
	{
		return ;
	}

	if (pszMsg == NULL)
	{
		return;
	}

	// Ч�鳤��ֵ
	int nEfficacyLen = sizeof(msg_entity_gemstone_appraisal_ret);
	if(nLen != nEfficacyLen)
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<nLen<<"< GemstoneItem Size= "<<nEfficacyLen);
		return ;
	}

	msg_entity_gemstone_appraisal_ret *pAppraisalRet = (msg_entity_gemstone_appraisal_ret*)pszMsg;

	if(pAppraisalRet->nGemstoneDBID <= 0)
		return;

	cmd_Crown_Gemstone_Appraisal_Ret cmd;
	cmd.nGemstoneDBID = pAppraisalRet->nGemstoneDBID;
	cmd.nCritNum = pAppraisalRet->nCritNum;
	cmd.nAppreaisalRet = (pAppraisalRet->bAppraisalRet ? 1 : 0);

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_CROWNG_GEMSTONE_APPRAISALRET, 0, 0, &cmd, sizeof(cmd_Crown_Gemstone_Appraisal_Ret));
}

void  CCrownPageManager::onMsgCrownGemstoneProduceInfo(LPCSTR pszMsg,int nLen)
{
	if(m_pMaster==NULL)
	{
		return ;
	}

	if (pszMsg == NULL)
	{
		return;
	}

	// Ч�鳤��ֵ
	int nEfficacyLen=sizeof(DBREQ_PARAM_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION);
	if(nLen!=nEfficacyLen)
	{
		ErrorLn(__FUNCTION__":DataLengthFail!  DataLength ="<<nLen<<"< DBREQ_PARAM_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION Size= "<<nEfficacyLen);
		return ;
	}

	ibuffer inBuffer(pszMsg, nLen);
	DBREQ_PARAM_CROWN_GEMSTONE_PRODUCE_DISTRIBUTION  crownProduct; 
	inBuffer>>crownProduct;

	cmd_Crown_Gemstone_Product_Info info;
	info.bGemstoneProduceEnd = crownProduct.bGemstoneProduceEnd;
	info.bOpenWinEnd = crownProduct.bOpenWinEnd;
	memcpy(info.cobblestonGemstoneIDList,crownProduct.cobblestonGemstoneIDList,sizeof(info.cobblestonGemstoneIDList));
	memcpy(info.windowGemstoneIDList,crownProduct.windowGemstoneIDList,sizeof(info.windowGemstoneIDList));
	memcpy(info.cobStoneIDList,crownProduct.cobStoneIDList,sizeof(info.cobStoneIDList));
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_CROWNG_GEMSTONE_PRODUCE,0, 0, &info, sizeof(info));
}

