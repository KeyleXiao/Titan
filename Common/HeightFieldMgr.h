/*******************************************************************
** 文件名:	MapChfContainer.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-9-29
** 版  本:	1.0
** 描  述:	场景高度场容器管理类
            
********************************************************************/

#pragma once

#include <unordered_map>
#include "Singleton.h"
#include <fstream>
#include "../Server/NmgenRcn/Include/RecastAlloc.h"
#include "../Server/DetourNavMesh/Include/DetourNavMeshEx.h"
#include "../Server/NmgenRcn/Include/NMGen.h"

using namespace std::tr1;

class dtNavMesh;
struct rcCompactHeightfield_Voxel;
enum rcBuildContoursFlags;

#pragma pack(push, 1)

// 高度场(动态障碍出现)生成NavMesh数据配置
struct NvBuildConfig
{
	int tileNum;                // 可用Tile总数量
	int tileNumPerRow;          // 每行所拥有tile数量
	int tileNumPerColum;        // 每列所拥有tile数量

	float mapBoundMinX;         // 地图边缘 最小点X
	float mapBoundMinY;         // 地图边缘 最小点Y
	float mapBoundMinZ;         // 地图边缘 最小点Z

	float mapBoundMaxX;         // 地图边缘 最大点X
	float mapBoundMaxY;         // 地图边缘 最大点Y
	float mapBoundMaxZ;         // 地图边缘 最大点Z

	int tileSize;
	int borderSize;             // Border size.
	float xzCellSize;           // XZ-plane cell size.
	float yCellSize;            // Y-axis cell size.
	float walkableSlope;        // Maximum walkable slope.
	int walkableHeight;         // Walkable height.
	int walkableStep;           // Walkable step.
	int walkableRadius;         // Walkable radius.
	int maxEdgeLength;          // Maximum edge length.
	float edgeMaxDeviation;     // Maximum edge deviation.
	int minRegionArea;          // Minimum region area.
	int mergeRegionArea;        // Merge region area.
	int maxVertsPerPoly;        // Maximum vertices per polygon.
	float detailSampleDistance; // Detail sample distance.
	float detailMaxDeviation;   // Detail maximum deviation.
	rcBuildContoursFlags contourOptions;    // Options to use when building the contour set.
	bool useMonotone;                       // If true, use monotone region generation.

    NvBuildConfig()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SCHF_NODE
{
    rcCompactHeightfield_Voxel*  chf;
    NvBuildConfig*         cfg;

    SCHF_NODE()
    {
        memset(this, 0, sizeof(*this) );
    }
};

/// Represents a span of unobstructed space within a compact heightfield.
struct rcCompactSpan_Voxel
{
    unsigned short y;			///< The lower extent of the span. (Measured from the heightfield's base.)
    unsigned short h;			///< The height of the span.  (Measured from #y.)
};

/// A compact, static heightfield representing unobstructed space.
/// @ingroup recast
struct rcCompactHeightfield_Voxel
{
    int width;					///< The width of the heightfield. (Along the x-axis in cell units.)
    int height;					///< The height of the heightfield. (Along the z-axis in cell units.)
    int spanCount;				///< The number of spans in the heightfield.
    float bmin[3];				///< The minimum bounds in world space. [(x, y, z)]
    float bmax[3];				///< The maximum bounds in world space. [(x, y, z)]
    float cs;					///< The size of each cell. (On the xz-plane.)
    float ch;					///< The height of each cell. (The minimum increment along the y-axis.)
    rcCompactCell* cells;		///< Array of cells. [Size: #width*#height]
    rcCompactSpan_Voxel* spans;	///< Array of spans. [Size: #spanCount]
};

#pragma pack(pop)

struct TileHeader
{
    int key;
    int size;
};

// 场景高度场容器管理类
class HeightFieldMgr
{
public:
    bool load(int nMapID, char* pcszFileName)
    {
        // 格式化下，主城镜像地图和源地图是同样的高度场
        nMapID = FormatMap(nMapID);

        std::string strFile = pcszFileName;
        
        // 文件已经加载过了
        std::map<std::string, SCHF_NODE>::iterator itFileLoad = m_mapFileLoad.find(strFile);
        if(itFileLoad != m_mapFileLoad.end())
        {
            m_mapData[nMapID] = itFileLoad->second;
            return true;
        }

        if(pcszFileName == NULL || strlen(pcszFileName) <= 1)
        {
            return false;
        }

        if(m_mapData.find(nMapID) != m_mapData.end())
        {
            return false;
        }

        // 没有就读取
        SCHF_NODE node;
        if (!read(pcszFileName, node))
        {
            ErrorLn(__FUNCTION__ << ", read failed, file=" << pcszFileName);
            return false;
        }
        m_mapData[nMapID] = node;
        m_mapFileLoad[strFile] = node;
        return true;
    }
    
    /// <summary>
    /// 高为fAgentHeight米，半径为fAgentRadius米的实体是否可以通过pos点
    /// </summary>
    /// <param name="pos">需要检测的点</param>
    /// <param name="fAgentHeight">实体高度</param>
    /// <param name="fAgentRadius">实体半径</param>
    /// <returns></returns>
    bool canPass(int nMapID, const Vector3& pos, float fAgentHeight = 1.9, float fAgentRadius = 0.1)
    {
        float fCellHeight = 0;
        float fBoundMinY = 0;
        std::vector<rcCompactSpan_Voxel> vecSpans;
        static int offset[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

        float fAgentHeadY = pos.y + fAgentHeight; // 头的坐标
        float fAgentFootY = pos.y;  // 脚的坐标
        fAgentFootY += 0.4 * fAgentHeight; // 允许误差
        fAgentHeadY -= 0.1 * fAgentHeight; // 允许误差

        for(int i = 0; i < 4; ++i)
        {
            float fPosX = pos.x + offset[i][0] * fAgentRadius;
            float fPosZ = pos.z + offset[i][1] * fAgentRadius;

            if(!getSpans(nMapID, fPosX, fPosZ, vecSpans, fCellHeight, fBoundMinY))
            {
                continue;
            }

            bool bPassable = false;

            std::vector<rcCompactSpan_Voxel>::iterator it = vecSpans.begin();
            while(it != vecSpans.end())
            {
                int nCurSpanMin = it->y;
                int nCurSpanMax = it->y + it->h;

                while(++it != vecSpans.end())
                {
                    if(it->y <= nCurSpanMax)
                    {
                        nCurSpanMax = it->y + it->h;
                    }
                    else
                    {
                        break;
                    }
                }

                float fCurSpanMin = fBoundMinY + nCurSpanMin * fCellHeight;
                float fCurSpanMax = fBoundMinY + nCurSpanMax * fCellHeight;

                if(fAgentFootY > fCurSpanMin && fAgentHeadY < fCurSpanMax)
                {
                    bPassable = true;
                    break;
                }
            }

            if(!bPassable)
            {
                return false;
            }
        }

        return true;
    }

    /// <summary>
    /// 贴地面，检测区间为需要检测点的Y坐标为中心，UpCheckDistance +  DownGroundCheckDistance;
    /// </summary>
    /// <param name="pos">需要检测的点</param>
    /// <param name="fUpCheckDistance">上方向距离,如无特俗情况，使用实体的高度最佳</param>
    /// <param name="fDownCheckDistance">下方向距离,默认5.0</param>
    /// <returns></returns>
    bool correctPosFromPhysic(int nMapID, Vector3& pos, float fUpCheckDistance, float fDownCheckDistance)
    {
        std::vector<rcCompactSpan_Voxel> vecSpans; float fCellHeight = 0; float fBoundMinY = 0;
        if(!getSpans(nMapID, pos.x, pos.z, vecSpans, fCellHeight, fBoundMinY))
        {
            return false;
        }

        float fMinY = fBoundMinY;
        if(!vecSpans.empty())
        {
            fMinY = fBoundMinY + vecSpans[0].y * fCellHeight;
        }

        if(pos.y < fMinY)
        {
            pos.y = fMinY;
            return true;
        }

        float fMax = pos.y + fUpCheckDistance;
        float fMin = pos.y - fDownCheckDistance;

        int nMinDistance = INT_MAX;
        float fGround = 0;
        for(std::vector<rcCompactSpan_Voxel>::reverse_iterator it = vecSpans.rbegin(); it != vecSpans.rend(); ++it)
        {
            std::vector<rcCompactSpan_Voxel>::reverse_iterator itNext = it + 1;

            // 这一层和下一层是连着的(下一层高度大于最大值 这一层是下一层拆出来的)
            if(itNext != vecSpans.rend() && it->y == itNext->y + itNext->h)
            {
                continue;
            }

            float fGrd = fBoundMinY + it->y * fCellHeight;

            if(fGrd > fMin && fGrd < fMax)
            {
                float fDis = abs(fGrd - pos.y);
                if(fDis < nMinDistance)
                {
                    nMinDistance = fDis;
                    fGround = fGrd;
                }
            }
        }

        if(nMinDistance < INT_MAX)
        {
            pos.y = fGround;
            return true;
        }

        return false;
    }

    // 获取地图边界
    bool getMapBounds(int nMapID, float bmin[3], float bmax[3])
    {
        nMapID = FormatMap(nMapID);

        TMAP_MAPCHFDATA::iterator itChf = m_mapData.find(nMapID);
        if(itChf == m_mapData.end())
            return false;

        if(itChf->second.chf != NULL)
        {
            bmin[0] = itChf->second.cfg->mapBoundMinX;
            bmin[1] = itChf->second.cfg->mapBoundMinY;
            bmin[2] = itChf->second.cfg->mapBoundMinZ; 

            bmax[0] = itChf->second.cfg->mapBoundMaxX; 
            bmax[1] = itChf->second.cfg->mapBoundMaxY; 
            bmax[2] = itChf->second.cfg->mapBoundMaxZ; 
            return true;
        }

        return false;
    }

    void destroy()
    {
        for (TMAP_MAPCHFDATA::iterator iter = m_mapData.begin(); iter != m_mapData.end(); ++iter)
        {
            SCHF_NODE& node = iter->second;
            if(node.cfg == NULL || node.chf == NULL)
                continue;

            for(int i = 0; i < node.cfg->tileNumPerRow * node.cfg->tileNumPerColum; ++i)
            {
                rcCompactHeightfield_Voxel chf = node.chf[i];
                if(chf.spans) delete [] chf.spans;
                if(chf.cells) delete [] chf.cells;   
            }

            delete node.cfg;
            delete [] node.chf;
        }
        m_mapData.clear();
    }
private:
    bool read(const char* pcszFileName, SCHF_NODE& node)
    {
        if (!pcszFileName)
            return false;

        // 打开文件进行读取和操作数据
        uint flags = Stream::modeRead|Stream::shareRead|Stream::modeExist;
        if (isDebugMode())	flags |= Stream::shareWrite;

        FileSystem* pFS = getFileSystem();
        Stream* pStream = pFS->open( pcszFileName,flags );
        if(pStream == NULL)
        {
            ErrorLn( __FUNCTION__<<": open file " << pcszFileName << " failed!");
            return false;
        }

        if(!pStream->seekToBegin())
            return false;

        // 读config信息
        node.cfg = new NvBuildConfig();
        if(!pStream->read(node.cfg, sizeof(NvBuildConfig)))
            return false;

        int nSize = 0;
        node.chf = new rcCompactHeightfield_Voxel[node.cfg->tileNumPerRow * node.cfg->tileNumPerColum];
        if(node.chf == NULL)
            return false;
        // rcCompactHeightfield_Voxel 无构造函数..
        memset(node.chf, 0, sizeof(rcCompactHeightfield_Voxel) * node.cfg->tileNumPerRow * node.cfg->tileNumPerColum);

        nSize += (node.cfg->tileNumPerRow * node.cfg->tileNumPerColum * sizeof(rcCompactHeightfield_Voxel)); 

        ulong nOffset = sizeof(NvBuildConfig);

        // 读tile列表
        for(int i = 0; i < node.cfg->tileNum; ++i)
        {
            if(!pStream->seek(nOffset))
                return false;

            TileHeader tileHeader;
            if(!pStream->read(&tileHeader, sizeof(tileHeader)))
                return false;

            if(tileHeader.key < 0 || tileHeader.key >= node.cfg->tileNumPerRow * node.cfg->tileNumPerColum)
                return false;

            rcCompactHeightfield_Voxel& chf = node.chf[tileHeader.key];

            int nHeadSize = sizeof(rcCompactHeightfield_Voxel) - sizeof(rcCompactCell*) - sizeof(rcCompactSpan_Voxel*);
            if(!pStream->read(&chf, nHeadSize))
                return false;

            // 读取cells
            chf.cells = new rcCompactCell[chf.width * chf.height]; 
            if(chf.cells == NULL)
                return false;

            if(!pStream->read(chf.cells, chf.width * chf.height * sizeof(rcCompactCell)))
                return false;

            nSize += (chf.width * chf.height * sizeof(rcCompactCell));

            // 读取spans
            chf.spans = new rcCompactSpan_Voxel[chf.spanCount];
            if(chf.spans == NULL)
                return false;

            if(!pStream->read(chf.spans, chf.spanCount * sizeof(rcCompactSpan_Voxel)))
                return false;

            nSize += chf.spanCount * sizeof(rcCompactSpan_Voxel); 

            // 定位到下一个tile数据位置
            nOffset += (sizeof(TileHeader) + tileHeader.size); 
        }

        safeRelease(pStream);

        EmphasisLn("height filed:" << pcszFileName << ", memory size=" << nSize);

        return true;
    }

    int getTileKey(const SCHF_NODE& node, float fPosX, float fPosZ)
    {
        // 是否超出范围
        if(fPosX < node.cfg->mapBoundMinX || fPosX > node.cfg->mapBoundMaxX || fPosZ < node.cfg->mapBoundMinZ || fPosZ > node.cfg->mapBoundMaxZ)
            return -1;

        // 定位tile
        int nIndexX = max(0, ceilf((fPosX - node.cfg->mapBoundMinX) / (node.cfg->xzCellSize * node.cfg->tileSize)) - 1);
        int nIndexZ = max(0, ceilf((fPosZ - node.cfg->mapBoundMinZ) / (node.cfg->xzCellSize * node.cfg->tileSize)) - 1); 

        // 直接定位成功 不需要遍历
        if(!(nIndexX < 0 || nIndexX >= node.cfg->tileNumPerRow || nIndexZ < 0 || nIndexZ >= node.cfg->tileNumPerColum))
        {
            int nTileKey = nIndexX + node.cfg->tileNumPerRow * nIndexZ;
            rcCompactHeightfield_Voxel* pTile = (rcCompactHeightfield_Voxel*)(node.chf + nTileKey);
            if(pTile != NULL && fPosX >= pTile->bmin[0] && fPosX <= pTile->bmax[0] && fPosZ >= pTile->bmin[2] && fPosZ <= pTile->bmax[2])
            {
                return nTileKey;
            }
        }

        // 遍历定位tile
        for(int nTileKey = 0; nTileKey < node.cfg->tileNumPerRow * node.cfg->tileNumPerColum; nTileKey++)
        {
            //没有chf数据 跳过
            rcCompactHeightfield_Voxel* pTile = (rcCompactHeightfield_Voxel*)(node.chf + nTileKey);
            if (pTile != NULL)
            {
                // 如果在该tile范围 就返回
                if(fPosX > pTile->bmin[0] && fPosX < pTile->bmax[0] && fPosZ > pTile->bmin[2] && fPosZ < pTile->bmax[2])
                {
                    return nTileKey;
                }
            }
        }

        return -1;
    }

    bool getSpans(int nMapID, float fPosX, float fPosZ, std::vector<rcCompactSpan_Voxel>& vecSpans, float& fCellHeight, float& fBoundMinY)
    {
        // 格式化下，主城镜像地图和源地图是同样的高度场
        nMapID = FormatMap(nMapID);

        TMAP_MAPCHFDATA::iterator itChf = m_mapData.find(nMapID);
        if(itChf == m_mapData.end())
        {
            return false;
        }

        const SCHF_NODE& node = itChf->second;
        if(node.cfg == NULL || node.chf == NULL)
        {
            return false;
        }

        //定位tile 
        int nTileKey = getTileKey(node, fPosX, fPosZ);
        if(nTileKey < 0)
        {
            WarningLn(__FUNCTION__ << " getTileKey failed, map(" << nMapID << "), pos(" << fPosX << "," << fPosZ << ")");
            return false;
        }

        //没有chf数据
        rcCompactHeightfield_Voxel* pTile = (rcCompactHeightfield_Voxel*)(node.chf + nTileKey);
        if (pTile == NULL)
        {
            WarningLn(__FUNCTION__ << ", get tile data failed, map(" << nMapID << "), pos(" << fPosX << "," << fPosZ << ")");
            return false;
        }

        int cellX = max(0, ceilf((fPosX - pTile->bmin[0]) / pTile->cs) - 1);
        int cellZ = max(0, ceilf((fPosZ - pTile->bmin[2]) / pTile->cs) - 1);

        // 定位cell出错
        if(cellX < 0 || cellX > pTile->width || cellZ < 0 || cellZ > pTile->height)
        {
            WarningLn(__FUNCTION__ << ", cell out of range, map(" << nMapID << "), pos(" << fPosX << "," << fPosZ << ")");
            return false;
        }

        // 取出span
        fCellHeight = node.cfg->yCellSize;
        fBoundMinY = node.cfg->mapBoundMinY;
        vecSpans.clear();
        const rcCompactCell& c = pTile->cells[cellX + cellZ*pTile->width];
        for(int i = c.index; i < c.index + c.count; ++i)
        {
            vecSpans.push_back(pTile->spans[i]);
        }

        return true;
    }

private:
    // <地图ID, SCHF_NODE>
	typedef unordered_map<int, SCHF_NODE>    TMAP_MAPCHFDATA;
    TMAP_MAPCHFDATA                          m_mapData;

    std::map<std::string, SCHF_NODE>         m_mapFileLoad;
};