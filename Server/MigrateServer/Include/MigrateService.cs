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
    @name                 : �����������������͹�������Ϣ
    @param                :
    @return               : 
    */
    void postMessage( DWORD serverID,SNetMsgHead head,PACKAGE_PTR msg );

    /**
    @name                 : �е�ͼ�򳡾�
    @param                :
    @return               : 
    */
    bool switchZone( DWORD dwClientID,DWORD dwOldScene,DWORD dwNewScene );
};
