/*******************************************************************
** 文件名:	CSingleMapArea.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	4/21/2015
** 版  本:	1.0
** 描  述:	
            维护单个地图区域的类声明
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeDef.h"
#include <map>

#include <unordered_map>
#include "Singleton.h"

// 单个地图的区域
class CSingleMapArea : public ISingleMapArea, public ISchemeUpdateSink
{
public:
    struct BMP_DATA
    {
        int nColorBytes;    // 单个像素所占字节数
        int nWidthPixel;    // 图片宽有多少像素
        int nHeightPixel;   // 图片高有多少像素
        uint8* pData;       // bmp数据

        BMP_DATA()
        {
            memset( this,0,sizeof(*this) );
        }
        ~BMP_DATA()
        {
            if ( pData!=NULL ) delete [] pData;
            pData = NULL;
        }
    };

    struct FOREST_DATA
    {
        int nWidthPixel;    // 宽有多少像素
        int nHeightPixel;   // 高有多少像素
        short* pData;       // 草丛数据(byte怕以后草丛太多表示不了)
        int nSize;

        FOREST_DATA()
        {
            memset( this,0,sizeof(*this) );
        }
        ~FOREST_DATA()
        {
            if ( pData!=NULL ) delete [] pData;
            pData = NULL;
        }
    };

public:

	CSingleMapArea() : m_nMapID(0), m_eType(eAreaTypeNone), m_pForestData()
    {
    }

	~CSingleMapArea()
    {
        m_pForestData = nullptr;
    }

    bool parse( TiXmlElement * pElement );
    
    /** 根据xml节点信息，读取地图区域的bmp文件
    @name				: 
    @param				: 
    @return
    */
    void load();

    void print();

    /** 返回指定坐标点所在的区域ID
	@param   
	@param   
	@return  
	*/
	virtual int getAreaID( Vector3 loc );

    // nIndex: 顶点索引 0-->n
    virtual bool getConvex( int nAreaID, int nIndex, Vector3& convex);

	virtual void getAreas(Vector3* pResult, int& nCnt);

    int getAreaIDByColor( BYTE R,BYTE G,BYTE B );


    inline int getMapID()
	{
		return m_nMapID;
    }

    AreaType getType()  { return m_eType;  }
    const std::string getBmpFileName() const { return m_strBmpFile; }
    const std::string getAreaFileName() const { return m_strAreaListFile; }




    virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

private:

    bool loadAreaList( const char * szFileName );

private:
    typedef std::map< COLORREF,int > TABLE_COLOR;
    // 区域颜色表，有了这张表就可以直接根据颜色RGB值映射到对应的区域ID
    TABLE_COLOR m_tableColor;

    typedef std::map< int, std::vector<Vector3> > TABLE_AREA_CONVEX;
    // 区域顶点表 
    TABLE_AREA_CONVEX m_tableConvex;

	typedef std::map< int, Vector3 > TABLE_AREA_LOC;
	// 区域位置表
	TABLE_AREA_LOC m_tableAreaLoc;

    int         m_nMapID;
    AreaType    m_eType;

    std::string m_strBmpFile;
    std::string m_strAreaListFile;

    float       m_aAreaBmpRangeX[2];
    float       m_aAreaBmpRangeZ[2];
    FOREST_DATA* m_pForestData;
};


class CMapAreaContainer : public Singleton<CMapAreaContainer>
{
    struct MapAreaKey
    {
        AreaType    m_eType;
        std::string m_strBmpFile;
        std::string m_strAreaListFile;

        MapAreaKey(string strBmpFile, string strAreaListFile, AreaType areaType) : m_eType(areaType), m_strBmpFile(strBmpFile), m_strAreaListFile(strAreaListFile)
        {
        }
    };
    struct MapAreaCmpFunc
    {
        bool operator()(const MapAreaKey& lhs, const MapAreaKey& rhs)
        {
            if (lhs.m_eType < rhs.m_eType)
                return true;
            else if(lhs.m_eType == rhs.m_eType)
            {
                if (lhs.m_strBmpFile < rhs.m_strBmpFile)
                    return true;
                else if(lhs.m_strBmpFile == rhs.m_strBmpFile)
                {
                    if (lhs.m_strAreaListFile < rhs.m_strAreaListFile)
                        return true;
                }
            }
            return false;
        }
    };

public:
    /** 读取一个位图文件，将结果保存到data中
    @name				: 
    @param				: 文件名
    @return
    */
    CSingleMapArea::FOREST_DATA * loadBmpReturnForestData(CSingleMapArea* pArea);

    /** 读取一个位图文件，将结果保存到data中
    @name				: 
    @param				: 文件名
    @return
    */
    bool readBMPFile( const char * szFileName,CSingleMapArea::BMP_DATA & data );

private:
    typedef std::map<MapAreaKey, CSingleMapArea::FOREST_DATA, MapAreaCmpFunc>  AREA_FOREST_MAP;
    AREA_FOREST_MAP  m_mapAreaForestData;
};