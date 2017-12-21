/*******************************************************************
** 文件名:	AOI_MapForest.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	3/24/2015
** 版  本:	1.0
** 描  述:	MOBA类AOI之丛林管理类
********************************************************************/

#include "stdafx.h"
#include "AOI_MapForest.h"
#include "FileSystem/VirtualFileSystem.h"
#include "ISchemeCenter.h"
#include "SchemeDef.h"
#include <windows.h>

bool AOI_MapForest::load( int nMapID,int nWidth,int nHeight,const char * filename )
{
    m_nWidth = nWidth;
    m_nHeight = nHeight;

    // 读取bitmap文件
    FileSystem* fs = getFileSystem();
    Stream* stream = fs->open( filename );
    if ( stream==NULL )
    {
        ErrorLn( "AOI_MapForest::load open file" << filename << " error!" );
        return false;
    }

    BITMAPFILEHEADER bmpHead;
    stream->read( &bmpHead, sizeof(bmpHead) );
    if ( bmpHead.bfType!=0x4d42 )
    {
        ErrorLn( "AOI_MapForest::load invalid filetype " << bmpHead.bfType );
        stream->close();
        return false;
    }

    // 位图数据字节数
    int nDataSize = bmpHead.bfSize-bmpHead.bfOffBits;
    TraceLn( "bitmap data size is " << nDataSize );

    int nColorBytes = 0;		// 单个像素所占字节数
    int nWidthPixel = 0;		// 图片宽有多少像素
    int nHeightPixel = 0;		// 图片高有多少像素

    switch (bmpHead.bfOffBits)
    {
    case (sizeof(BITMAPFILEHEADER)+sizeof(BITMAPCOREHEADER)):
        {
            BITMAPCOREHEADER coreHead;
            stream->read( &coreHead,sizeof(coreHead) );
            nColorBytes = coreHead.bcBitCount/8;
            nWidthPixel = coreHead.bcWidth;
            nHeightPixel = coreHead.bcHeight;
        }
        break;

    case (sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)):
        {
            BITMAPINFOHEADER infoHead;
            stream->read( &infoHead, sizeof(infoHead) );
            nColorBytes = infoHead.biBitCount/8;
            if ( infoHead.biCompression!=BI_RGB )
            {
                ErrorLn("bmp file is compressed, please generate a uncompressed bmp file!\n");
                stream->close();
                return false;
            }
            nWidthPixel = infoHead.biWidth;
            nHeightPixel = infoHead.biHeight;
        }
        break;

    default:
        break;
    }

    // 必须保证nHeightPixel为正数, 而且图片左下角即表示位图信息开始的地方
    if ( nHeightPixel<0 || nColorBytes<=0 )
    {
        stream->close();
        return false;
    }

    // 读取具体位图数据
    BYTE* bitmap = new BYTE[nDataSize];
    stream->read( bitmap,nDataSize );

    m_bitmap = new BYTE[m_nWidth*m_nHeight];
    memset( m_bitmap,0,m_nWidth*m_nHeight );

    for ( int x=0;x<m_nWidth;++x )
    {
        // 将m_nWidth*m_nHeight的坐标换算成nWidthPixel*nHeightPixel坐标
        // 即(x,y)->(i,j)
        int i = x*nWidthPixel/m_nWidth;
        for ( int y=0;y<m_nHeight;++y )
        {
            int j = y*nHeightPixel/m_nHeight;
            RGBTRIPLE* color = (RGBTRIPLE*)(bitmap+(i+j*nWidthPixel)*nColorBytes);
            m_bitmap[x+y*m_nWidth] = getForestID( RGB(color->rgbtRed,color->rgbtGreen,color->rgbtBlue) );
        }
    }

    delete [] bitmap;
    stream->close();
    return true;
}

int AOI_MapForest::getForestID( int nX,int nY )
{
    if ( m_bitmap==NULL )
        return FOREST_NULL;

    // 假定：坐标从0开始, 即nX:[0, m_nWidth), nY:[0, m_nHeight)
    if ( nX<0||nX>=m_nWidth||nY<0||nY>=m_nHeight )
        return FOREST_NULL;

    return m_bitmap[nX+nY*m_nWidth];
}

void AOI_MapForest::release()
{
    AOI_MapForestFactory::getInstance().releaseMapForest(m_nMapID);
}


/////////////////////////////////////AOI_MapForestFactory//////////////////////
AOI_MapForest* AOI_MapForestFactory::createMapForest( int nMapID,int nWidth,int nHeight )
{
    AOI_MapForest* pForest = NULL;

    T_FORESTS::iterator itr = m_forests.find(nMapID);
    if (itr == m_forests.end())
    {
        pForest = new AOI_MapForest;
        if (pForest == NULL)
            return NULL;

        if ( !pForest->load(nMapID,nWidth,nHeight) )
        {
            delete pForest;
            return NULL;
        }

        m_forests.insert( make_pair(nMapID,pForest) );
    }
    else
    {
        pForest = itr->second;
    }

    ++pForest->m_nRef;
    return pForest;
}

void AOI_MapForestFactory::releaseMapForest( int mapid )
{
    T_FORESTS::iterator itr = m_forests.find(mapid);
    if ( itr!=m_forests.end() )
    {
        AOI_MapForest* pForest = itr->second;
        if ( --pForest->m_nRef==0 )
        {
            delete pForest;
            m_forests.erase(itr);
        }
    }
}
