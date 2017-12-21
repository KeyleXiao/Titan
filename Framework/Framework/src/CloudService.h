/*******************************************************************
** 文件名:	CloudService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/13/2014
** 版  本:	1.0
** 描  述:	框架云服务

			
********************************************************************/

#pragma once

#include <string>
#include <vector>
#include "Singleton.h"
using namespace rkt;
using namespace std;

// 云连接
class CloudConnector;

/**
    框架云服务:
	1.监听一个端口，维护与部署在其他机器上的框架之间的连接，和相关数据的同步
*/

class CloudService : public Singleton<CloudService>
{
protected:
	int             m_cloud_id;     // 云ID
	std::string     m_ipstr;		// 本机IP
	unsigned short  m_port;			// 本机端

	typedef std::vector<CloudConnector*>  CONN_LIST;
	CONN_LIST       m_conn_list;    // 连接数组

public:
	CloudService() : m_cloud_id(0),m_port(0){}
	virtual ~CloudService(){}

	// 取得云ID
	virtual int get_id() const { return m_cloud_id; }

	// 取得本机IP
	virtual const char * get_ip_str() const { return m_ipstr.c_str(); }

	// 取得本机端口
	virtual unsigned short get_port() const { return m_port; }


};