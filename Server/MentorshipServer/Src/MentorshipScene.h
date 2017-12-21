/*******************************************************************
** 文件名:	MentorshipScene.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	11/29/2016
** 版  本:	1.0
** 描  述:	师徒场景
********************************************************************/

#ifndef __MENTORSHIPSCENE_H__
#define __MENTORSHIPSCENE_H__

#include "IMessageHandler.h"
#include "IServiceContainer.h"
#include "MentorshipSceneService.h"
#include "IMentorshipScene.h"

class MentorshipScene : public IMentorshipScene
{
public:
	MentorshipScene();
	virtual ~MentorshipScene();

public:
	// IMentorshipScene
	//////////////////////////////////////////////////////////////////////////
	virtual bool			load();
    // 执行释放Service
    virtual void            onStop();
	virtual void			release();
	virtual SERVICE_PTR		getMentorshipSceneService();

private:
	bool createMentorshipSceneService();

private:
	SERVICE_PTR m_MentorshipSceneServices;
};
#endif // __MENTORSHIPSCENE_H__