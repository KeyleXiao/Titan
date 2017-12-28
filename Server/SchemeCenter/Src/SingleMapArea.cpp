/*******************************************************************
** 文件名:	CSchemeAllMapsArea.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	4/21/2015
** 版  本:	1.0
** 描  述:	
            单个地图的区域配置
********************************************************************/

#pragma once

#include "stdafx.h"
#include "IServerGlobal.h"
#include "SingleMapArea.h"
#include "Smart_ClassT.h"
#include "HeightFieldMgr.h"
#define SINGLE_MAP_AREA_SHOW_DEBUG

/** 返回指定坐标点所在的区域类型
@param   
@param   
@return  
*/
int CSingleMapArea::getAreaID( Vector3 loc )
{
    if(m_pForestData==nullptr || m_pForestData->pData== NULL) 
        return -1;

    if(loc.x < m_aAreaBmpRangeX[0] || loc.x > m_aAreaBmpRangeX[1] 
        || loc.z < m_aAreaBmpRangeZ[0] || loc.z > m_aAreaBmpRangeZ[1])
        return -1;

    // 对应位图上的像素坐标
    int nBmpX = (int)((float)(loc.x - m_aAreaBmpRangeX[0]) * (float)m_pForestData->nWidthPixel / (float)(m_aAreaBmpRangeX[1] - m_aAreaBmpRangeX[0]));
    int nBmpY = (int)((float)(loc.z - m_aAreaBmpRangeZ[0]) * (float)m_pForestData->nHeightPixel / (float)(m_aAreaBmpRangeZ[1] - m_aAreaBmpRangeZ[0]));

    int nIndex = nBmpX + nBmpY * m_pForestData->nWidthPixel;
    if(nIndex < 0 || nIndex >= m_pForestData->nSize)
        return -1;

    return m_pForestData->pData[nIndex];
}

bool CSingleMapArea::getConvex( int nAreaID, int nIndex, Vector3& convex )
{
    TABLE_AREA_CONVEX::iterator it = m_tableConvex.find(nAreaID);
    if(it == m_tableConvex.end())
    {
        return false;
    }

    if(nIndex < 0 || nIndex >= it->second.size())
    {
        return false;
    }

    convex = it->second[nIndex];

    return true;
}

void CSingleMapArea::getAreas(Vector3* pResult, int& nCnt)
{
	int nIndex = 0;
	for (TABLE_AREA_LOC::iterator it = m_tableAreaLoc.begin(); it != m_tableAreaLoc.end(); ++it)
	{
		if(nIndex >= nCnt)
			break;

		pResult[nIndex++] = it->second;
	}

	nCnt = nIndex;
}

int CSingleMapArea::getAreaIDByColor(BYTE R,BYTE G,BYTE B)
{
     COLORREF color = RGB(R,G,B);
     TABLE_COLOR::iterator it = m_tableColor.find( color );
     if ( it==m_tableColor.end() )
         return -1;
     return it->second;
}

/** 根据xml节点信息，读取地图区域的bmp文件
@name				: 
@param				: 
@return
*/
bool CSingleMapArea::parse( TiXmlElement * pMapElement )
{
    if(pMapElement==NULL)
        return false;

    if(!pMapElement->Attribute( "id",&m_nMapID ))
    {
        ErrorLn("CSingleMapArea::load error:map id was not configed!");
        return false;
    }

    const char* szType = pMapElement->Attribute("type");
    if(szType == NULL)
    {
        ErrorLn("CSingleMapArea::load error:type was not configed!");
        return false;
    }

    if(strcmp(szType, "grass") == 0)
    {
        m_eType = eAreaTypeGrass;
    }
    if(strcmp(szType, "fly") == 0)
    {
        m_eType = eAreaTypeFly;
    }

    if(m_eType == eAreaTypeNone)
    {
        ErrorLn("CSingleMapArea::load error:unknown type=" << szType);
        return false;
    }

    const char* szAreaListFile = pMapElement->Attribute("area_list_file");
    if(szAreaListFile == NULL)
    {
        ErrorLn("CSingleMapArea::load error:area_list_file was not configed!");
        return false;
    }
    m_strAreaListFile += szAreaListFile;  

    const char* szBmpFile = pMapElement->Attribute("area_bmp");
    if (szBmpFile == NULL)
    {
        ErrorLn("CSingleMapArea::load error:area_bmp was not configed!");
        return false;
    }
    m_strBmpFile += szBmpFile; 

    return true;
}

void CSingleMapArea::load()
{
    if(m_strAreaListFile.empty() || m_strBmpFile.empty())
    {
        return;
    }

    string strCfgDir = MAPAREA_SCHEME_FILEPATH;
    // 读取区域列表
    string strAreaListFile = strCfgDir + m_strAreaListFile;
    if(!loadAreaList(strAreaListFile.c_str()))
    {
        ErrorLn("load " << strAreaListFile.c_str() << " failed!");
        return;
    }

    // 读取位图
    m_pForestData = CMapAreaContainer::getInstance().loadBmpReturnForestData(this);
}

void CSingleMapArea::print()
{
    float bmin[3];
    float bmax[3];
    if(!gServerGlobal->getHeightFieldMgr()->getMapBounds(m_nMapID, bmin, bmax))
    {
        return;
    }

    TraceLn( "==================start print area of map " << m_nMapID );
    for ( int y=bmax[2] - 1;y>=bmin[2];--y )
    {
        ostrbuf ostr;
        for ( int x=bmin[0];x<bmax[0];++x )
        {
            Vector3 loc; loc.x=x; loc.y=0; loc.z=y;
            ostr << " " << getAreaID(loc);
        }
        TraceLn( ostr.c_str() );
    }
    TraceLn( "==================end print area of map " << m_nMapID );
}

bool CSingleMapArea::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;

    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for (int nRow=0; nRow<nRecordCount; ++nRow)
    {
        int nCol = 0;
        // 区域ID
        int nAreaID = pCSVReader->GetInt(nRow, nCol++, 0);
        // subid(子区域ID)
        int nSubID = pCSVReader->GetInt(nRow, nCol++, 0);
        // 颜色R
        int nColorR = pCSVReader->GetInt(nRow, nCol++, 0);
        // 颜色G
        int nColorG = pCSVReader->GetInt(nRow, nCol++, 0);
        // 颜色B
        int nColorB = pCSVReader->GetInt(nRow, nCol++, 0);

        // 因为csv配置格式问题重复读位图范围
        m_aAreaBmpRangeX[0] = pCSVReader->GetFloat(nRow, nCol++, 0);
        m_aAreaBmpRangeZ[0] = pCSVReader->GetFloat(nRow, nCol++, 0);
        m_aAreaBmpRangeX[1] = pCSVReader->GetFloat(nRow, nCol++, 0);
        m_aAreaBmpRangeZ[1] = pCSVReader->GetFloat(nRow, nCol++, 0);

        // minx
		float fMinX = pCSVReader->GetFloat(nRow, nCol++, 0);

        // minz
		float fMinZ = pCSVReader->GetFloat(nRow, nCol++, 0);

        // maxx
		float fMaxX = pCSVReader->GetFloat(nRow, nCol++, 0);

        // maxz
		float fMaxZ = pCSVReader->GetFloat(nRow, nCol++, 0);
		
		// 记录区域中心位置
		Vector3 locMid = (Vector3(fMinX, 0, fMinZ) + Vector3(fMaxX, 0, fMaxZ)) * 0.5;
		TABLE_AREA_LOC::iterator itOri = m_tableAreaLoc.find(nAreaID);
		if (itOri != m_tableAreaLoc.end())
		{
			itOri->second = (itOri->second + locMid) * 0.5;
		}
		else
		{
			m_tableAreaLoc[nAreaID] = locMid;
		}

        // convex
        char szConvexBuf[512];
        int nConvexBufLen = sizeof(szConvexBuf);
        memset(szConvexBuf, 0, nConvexBufLen);
        pCSVReader->GetString(nRow, nCol, szConvexBuf, nConvexBufLen);
        std::vector<string> vecConvex;
        StringHelper::split(vecConvex, szConvexBuf, ';', " ");
        for(std::vector<string>::iterator it = vecConvex.begin(); it != vecConvex.end(); ++it)
        {
            StringHelper::replace(*it, '{', ' ');
            StringHelper::replace(*it, '}', ' ');

            std::vector<string> vecTemp;
            StringHelper::split(vecTemp, it->c_str(), '|', " ");
            if(vecTemp.size() < 2)
            {
                ErrorLn(__FUNCTION__ << ", size=" << vecTemp.size());
                break;
            }
            m_tableConvex[nAreaID].push_back(Vector3(StringHelper::toFloat(vecTemp[0]), 0, StringHelper::toFloat(vecTemp[1])));
        }

        COLORREF color = RGB(nColorR,nColorG,nColorB);
        if(m_tableColor.find(color) != m_tableColor.end())
        {
            continue;
        }
        m_tableColor[color] = nAreaID;
    }

    return true;
}

bool CSingleMapArea::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return false; 
}

bool CSingleMapArea::loadAreaList(const char* szFileName)
{
    ISchemeEngine * pSchemeEngine = gServerGlobal->getSchemeEngine();
    if(pSchemeEngine == NULL)
    {
        return false;
    }
    string stringPath = SCP_PATH_FORMAT( szFileName );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        ErrorLn("CSingleMapArea::LoadScheme failed! filename=" << stringPath.c_str());
        return false;
    }

    return true;
}



///////////////////////MapAreaContainer///////////////////////////////////////////////////

CSingleMapArea::FOREST_DATA * CMapAreaContainer::loadBmpReturnForestData(CSingleMapArea* pArea)
{
    std::string strBmpFileName = pArea->getBmpFileName();
    std::string strAreaFileName = pArea->getAreaFileName();
    AreaType thisType = pArea->getType();

    MapAreaKey key(strBmpFileName, strAreaFileName, thisType);
    AREA_FOREST_MAP::iterator iter = m_mapAreaForestData.find(key);
    if(iter != m_mapAreaForestData.end()) {
#ifdef SINGLE_MAP_AREA_SHOW_DEBUG
        WarningLn(__FUNCTION__": find bmp file exits, mapID="<<pArea->getMapID()<<",type="<< thisType <<",bmp="<<strBmpFileName.c_str()<<",csv="<<strAreaFileName.c_str());
#endif
        return &(iter->second);
    }

    string strBmpFilePath = MAPAREA_SCHEME_FILEPATH;
    strBmpFilePath += strBmpFileName;
    // 读取bitmap文件
    CSingleMapArea::BMP_DATA data;
    if ( !readBMPFile(strBmpFilePath.c_str(), data) )
    {
        return nullptr;
    }
    
    CSingleMapArea::FOREST_DATA insertData;
    pair<AREA_FOREST_MAP::iterator, bool> result = m_mapAreaForestData.insert(AREA_FOREST_MAP::value_type(key, insertData));
    if( !result.second )
    {
#ifdef SINGLE_MAP_AREA_SHOW_DEBUG
        ErrorLn(__FUNCTION__":insert failed, mapID="<<pArea->getMapID()<<",type="<< thisType <<",bmp="<<strBmpFileName.c_str()<<",csv="<<strAreaFileName.c_str());
#endif
        return nullptr;
    }
   
    CSingleMapArea::FOREST_DATA* pforestData = &(result.first->second);

    // 处理位图数据
    pforestData->nWidthPixel  = data.nWidthPixel;
    pforestData->nHeightPixel = data.nHeightPixel;
    pforestData->nSize        = data.nWidthPixel * data.nHeightPixel;
    pforestData->pData        = new short[data.nWidthPixel * data.nHeightPixel];
    for(int i = 0; i < data.nWidthPixel * data.nHeightPixel; ++i)
    {
        pforestData->pData[i] = -1;
    }

    // 位图坐标系要求  
    //             ^ z
    //             |
    //             |
    //       ------|----->x
    //             | 
    //             | 
    // 位图每行四字节对齐
    int nWidthBytes = (data.nWidthPixel * data.nColorBytes + 3) / 4 * 4;
    for(int x = 0; x < data.nWidthPixel; ++x)
    {
        for(int z = 0; z < data.nHeightPixel; ++z)
        {
            RGBTRIPLE* color = (RGBTRIPLE*)(data.pData+ z * nWidthBytes + x * data.nColorBytes);

            int areaID = pArea->getAreaIDByColor( color->rgbtRed,color->rgbtGreen,color->rgbtBlue );
            if ( areaID >= 0)
            {
                pforestData->pData[x + z * data.nWidthPixel] = areaID;
            }
        }
    }

#ifdef SINGLE_MAP_AREA_SHOW_DEBUG
    TraceLn(__FUNCTION__": load bmp file, mapID="<<pArea->getMapID()<<",type="<< thisType <<",bmp="<<strBmpFileName.c_str()<<",csv="<<strAreaFileName.c_str());
#endif

    return pforestData;
}

/** 读取一个位图文件，将结果保存到data中
@name				: 
@param				: 文件名
@return
*/
bool CMapAreaContainer::readBMPFile( const char * szFileName,CSingleMapArea::BMP_DATA & data )
{
    if(szFileName == nullptr || strlen(szFileName) < 2)
        return false;

    FileSystem* fs = getFileSystem();
    Stream* stream = fs->open( szFileName );
    if ( stream==NULL )
    {
        ErrorLn( __FUNCTION__":open file " << szFileName << " error!" );
        return false;
    }

    // 在函数退出时自动关闭
    auto_close<Stream> ac(stream);

    BITMAPFILEHEADER bmpHead;
    stream->read( &bmpHead, sizeof(bmpHead) );
    if ( bmpHead.bfType!=0x4d42 )
    {
        ErrorLn( __FUNCTION__": invalid filetype " << bmpHead.bfType );
        return false;
    }

    // 位图数据字节数
    int nDataSize = bmpHead.bfSize-bmpHead.bfOffBits;

    switch (bmpHead.bfOffBits)
    {
    case (sizeof(BITMAPFILEHEADER)+sizeof(BITMAPCOREHEADER)):
        {
            BITMAPCOREHEADER coreHead;
            stream->read( &coreHead,sizeof(coreHead) );
            data.nColorBytes = coreHead.bcBitCount/8;
            data.nWidthPixel = coreHead.bcWidth;
            data.nHeightPixel = coreHead.bcHeight;
        }
        break;

    case (sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)):
        {
            BITMAPINFOHEADER infoHead;
            stream->read( &infoHead, sizeof(infoHead) );
            data.nColorBytes = infoHead.biBitCount/8;
            if ( infoHead.biCompression!=BI_RGB )
            {
                ErrorLn( "bmp file "<<szFileName<<" is compressed, please generate a uncompressed bmp file!\n" );
                return false;
            }
            data.nWidthPixel = infoHead.biWidth;
            data.nHeightPixel = infoHead.biHeight;
        }
        break;

    default:
        break;
    }

    // 必须保证nHeightPixel为正数, 而且图片左下角即表示位图信息开始的地方
    if ( data.nHeightPixel<=0 || data.nColorBytes<=0 )
    {
        return false;
    }

    // 读取具体位图数据
    data.pData = new uint8[nDataSize];
    if ( data.pData==NULL )
        return false;

    return stream->read( data.pData,nDataSize );
}
