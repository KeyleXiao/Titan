using System;
using System.Reflection;
using System.IO;
using System.Text;

struct ibuffer
{
};

struct obuf
{
};

struct CSID  
{
};

struct CGID  
{
};

struct ServerData
{
};

struct DWORD
{
};

struct WORD
{
};

struct PACKAGE_PTR
{
};

struct SNetMsgHead
{
};

interface MigrateService
{
    /**
    @name                 : 处理其它服务器发送过来的消息
    @param                :
    @return               : 
    */
    void postMessage( DWORD serverID,SNetMsgHead head,PACKAGE_PTR msg );

    /**
    @name                 : 切地图或场景
    @param                :
    @return               : 
    */
    bool switchZone( DWORD dwClientID,DWORD dwOldScene,DWORD dwNewScene );
};
