/*******************************************************************
** 文件名:	MovePath.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	CHEN.TAO!~
** 日  期:	8/2/2015
** 版  本:	1.0
** 描  述:	
			移动行为管理类,用于服务器客户端路径同步模拟
********************************************************************/


#include "Vector3.h"
#include "buffer.h"
using rkt::ibuffer;
using rkt::obuffer;

#pragma once

//////////////////////////////////////////////////////////////////////////
enum BEHAVIOURTYPE
{
    BEHAVIOUR_TYPE_NONE,
    BEHAVIOUR_TYPE_WALK, 
    BEHAVIOUR_TYPE_JUMP,
    BEHAVIOUR_TYPE_SHIFT,
    BEHAVIOUR_TYPE_MAX,
};

enum BEHAVIOURSTATE
{
    BEHAVIOUR_STATE_INVALID,
    BEHAVIOUR_STATE_RUNNING,
    BEHAVIOUR_STATE_FINISH,
};

// 行为上下文基类
struct BehaviourCtx
{
    BEHAVIOURSTATE eState;
    DWORD dwStartTick;    // 开始时刻
    DWORD dwLastUpdateTick; // 上次更新时刻
    float fStartSpeed;    // （起始）速度
    bool bKeepLookDir;    // 是否保持朝向 
    Move3 move;          // 当前移动三元组

    BehaviourCtx()
    {
        clear();
    }

    virtual void clear()
    {
        eState = BEHAVIOUR_STATE_INVALID;
        dwStartTick = 0;
        dwLastUpdateTick = 0;
        fStartSpeed = 0;
        bKeepLookDir = false;
        move.vPosition = Vector3();
        move.vRotation = Vector3();
        move.vVelocity = Vector3();
    }

    virtual ibuffer& _cdecl operator<<(ibuffer& ib) = 0;

    virtual BEHAVIOURTYPE getType() const = 0;
};

// 走路上下文
struct WalkCtx : public BehaviourCtx
{
    std::vector<Vector3> vecPoint;  // 路径
    float fPathLen;

    WalkCtx()
    {
        clear();
    }

    virtual void clear()
    {
        vecPoint.clear();
        fPathLen = 0;
        BehaviourCtx::clear();
    }

    template <class buffer_allocator>
    obuffer<buffer_allocator> & _cdecl operator>>(obuffer<buffer_allocator> & ob ) const 
    {
        BYTE byCnt = vecPoint.size();
        ob << dwStartTick << (int16)(fStartSpeed * 10) << (BYTE)(bKeepLookDir ? 1 : 0) << byCnt;

        for(int i = 0; i < byCnt; ++i)
        {
            ob << (int16)(vecPoint[i].x * 10) << (int16)(vecPoint[i].y * 10) << (int16)(vecPoint[i].z * 10); 
        }

        return ob;
    }

    virtual ibuffer& _cdecl operator<<(ibuffer& ib)
    {
        clear();
        BYTE byCnt = 0;
        int16 nSpeed = 0;
        BYTE byKeepLookDir = 0;

        ib >> dwStartTick >> nSpeed >> byKeepLookDir >> byCnt;
        fStartSpeed = (float)nSpeed / 10;
        bKeepLookDir = byKeepLookDir;

        if(ib.fail())
        {
            clear();
            return ib;
        }
        for(int i = 0; i < byCnt; ++i)
        {
            int16 x, y, z;
            ib >> x >> y >> z;
            if(ib.fail())
            {
                clear();
                return ib;
            }

            Vector3 point;
            point.x = (float)x / 10;
            point.y = (float)y / 10;
            point.z = (float)z / 10;
            vecPoint.push_back(point);
        }

        return ib;
    }

    virtual BEHAVIOURTYPE getType() const
    {
        return BEHAVIOUR_TYPE_WALK;
    }
};

// 跳跃上下文
struct JumpCtx : public BehaviourCtx
{
    Vector3 startLoc;
    Vector3 targetLoc;

    JumpCtx()
    {
        clear();
    }

    virtual void clear()
    {
        startLoc = Vector3();
        targetLoc = Vector3();
        BehaviourCtx::clear();
    }

    template <class buffer_allocator>
    obuffer<buffer_allocator> & _cdecl operator>>(obuffer<buffer_allocator> & ob ) const 
    {
        ob << dwStartTick << fStartSpeed << bKeepLookDir << startLoc << targetLoc; 
        return ob;
    }

    virtual ibuffer& _cdecl operator<<(ibuffer& ib)
    {
        clear();
        ib >> dwStartTick >> fStartSpeed >> bKeepLookDir >> startLoc >> targetLoc;
        if(ib.fail())
        {
            clear();
        }
      
        return ib;
    }

    virtual BEHAVIOURTYPE getType() const
    {
        return BEHAVIOUR_TYPE_JUMP;
    }
};

// 冲锋模式
enum ShiftMode
{
    shift_mode_invalid,         // 无效模式
    shift_mode_forward_loc,     // 冲向目标点
    shift_mode_forward_entity,  // 追踪实体
};

// 冲锋上下文
struct ShiftCtx : public BehaviourCtx
{
    friend class ShiftBehaviour;

    ShiftMode shiftMode;// 冲锋模式
    int nPriority;      // 冲锋优先级
    int nAnimation;     // 冲锋表现类型(MOVE_TYPE)
    Vector3 startLoc;   // 冲锋起点
    Vector3 targetLoc;  // 冲锋目标点
    UID uidTarget;      // 目标实体
    int maxTime;        // 冲锋最大时间

    float fHeight;        // 击飞高度
    bool  bUseHeight;     // 是否使用高度(是否是抛物线)
    bool bPassive;        // 是否被动位移
    float fCloseDistance; // 与目标保持距离 与目标距离<fCloseDistance则打断 
    float fBreakDistance; // 与目标距离>fBreakDistance则打断
    DWORD dwSpellStartTick;  // 技能使用时间  
    bool bCanCrossObstacle;  // 是否可跨越障碍
    int nHitObstacleAddBuffID; // 撞到障碍时触发buffID
    int nHitObstacleAddBuffLevel; // buff等级
    int nMaxUpAngle;         // 冲锋最大仰角(0-90)
   
    UID uidFinishBuff_Target;   // 冲锋完加buff目标
    UID uidFinishBuff_Operator; // 冲锋完加buff 添加者
    int nFinishBuff_ID;         // 冲锋完加buffID
    int nFinishBuff_Level;      // 冲锋完加buffLevel

    ShiftCtx()
    {
        clear();
    }

    virtual void clear()
    {
        shiftMode = shift_mode_invalid; 
        nAnimation = 0;
        startLoc = Vector3();
        targetLoc = Vector3();
        uidTarget = INVALID_UID;
        maxTime = 0;
        fHeight = 0;
        bUseHeight = false;
        bPassive = false;
        fCloseDistance = 0;
        fBreakDistance = 0;
        dwSpellStartTick = 0;
        bCanCrossObstacle = false;
        nHitObstacleAddBuffID = 0;
        nHitObstacleAddBuffLevel = 0;
        nMaxUpAngle = 0;
        bArrived = false;
        uidFinishBuff_Target = INVALID_UID;
        uidFinishBuff_Operator = INVALID_UID;
        nFinishBuff_ID = 0;
        nFinishBuff_Level = 0;
        BehaviourCtx::clear();
    }

    template <class buffer_allocator>
    obuffer<buffer_allocator> & _cdecl operator>>(obuffer<buffer_allocator> & ob ) const 
    {
        ob << dwStartTick << fStartSpeed << bKeepLookDir << shiftMode << nAnimation << startLoc
            << targetLoc << uidTarget << maxTime << fHeight << bPassive << fCloseDistance << fBreakDistance << dwLastUpdateTick << move;      
        return ob;
    }

    virtual ibuffer& _cdecl operator<<(ibuffer& ib)
    {
        clear();
        ib >> dwStartTick >> fStartSpeed >> bKeepLookDir >> shiftMode >> nAnimation >> startLoc
            >> targetLoc >> uidTarget >> maxTime >> fHeight >> bPassive >> fCloseDistance >> fBreakDistance >> dwLastUpdateTick >> move;  
        
        if(ib.fail())
        {
            clear();
        }

        return ib;
    }

    virtual BEHAVIOURTYPE getType() const
    {
        return BEHAVIOUR_TYPE_SHIFT;
    }

protected:
    bool bArrived;         // 追踪目标的还要支持往前多冲锋一段距离
};

// 行为
struct Behaviour
{
    virtual BEHAVIOURTYPE getType() const = 0;

    virtual BehaviourCtx& getCtx() = 0;

    virtual bool getStartPoint(Vector3& ptStart) = 0;

    virtual bool getTargetPoint(Vector3& ptTarget) = 0;

    virtual bool isRunning() = 0;

    virtual bool run() = 0;

    virtual void update(DWORD dwTick, Vector3* pTargetLoc = NULL) = 0;

    // 调整上下文 只保留dwTick之后的部分
    virtual void adjustCtx(DWORD dwTick) = 0;

    virtual void release() = 0;
};

// 走路行为
class WalkBehaviour : public Behaviour
{
    WalkCtx m_ctx;
public:
    static Behaviour* create()
    {
        return new WalkBehaviour;
    }

    virtual BEHAVIOURTYPE getType() const
    {
        return BEHAVIOUR_TYPE_WALK;
    }

    virtual BehaviourCtx& getCtx()
    {
        return m_ctx;
    }

    virtual bool getStartPoint(Vector3& ptStart)
    {
        if(m_ctx.vecPoint.size() == 0)
        {
            return false;
        }

        ptStart = m_ctx.vecPoint.front();

        return true;
    }

    virtual bool getTargetPoint(Vector3& ptTarget)
    {
        if(m_ctx.vecPoint.size() == 0)
        {
            return false;
        }

        ptTarget = m_ctx.vecPoint.back();

        return true;
    }

    virtual bool isRunning()
    {
        return m_ctx.eState == BEHAVIOUR_STATE_RUNNING;
    }

    virtual bool run()
    {     
        m_ctx.eState = BEHAVIOUR_STATE_INVALID;

        int nPointNum = (int)m_ctx.vecPoint.size();
        if(nPointNum < 2 || m_ctx.dwStartTick == 0)
        {
            return false;
        }

        m_ctx.fPathLen = 0;
        for(int i = 1; i < nPointNum; ++i)
        {
            m_ctx.fPathLen += (m_ctx.vecPoint[i] - m_ctx.vecPoint[i - 1]).getLength();
        }

        m_ctx.eState = BEHAVIOUR_STATE_RUNNING;

        m_ctx.dwLastUpdateTick = m_ctx.dwStartTick;

        return true;
    }

    virtual void update(DWORD dwNow, Vector3* pTargetLoc = NULL)
    {
        if(m_ctx.eState != BEHAVIOUR_STATE_RUNNING)
            return;

        int nProgress = getWalkProgress(dwNow, m_ctx.move);
        if(nProgress < 0)
        {
            m_ctx.eState = BEHAVIOUR_STATE_FINISH; 
        }

        m_ctx.dwLastUpdateTick = dwNow; 
    }

    virtual void adjustCtx(DWORD dwTick)
    {
        if(!isRunning())
            return;

        Move3 move;
        int nNextIndex = getWalkProgress(dwTick, move);
        if(nNextIndex >= 0)
        {
            std::vector<Vector3> remain;

            remain.push_back(move.vPosition);

            for(int i = nNextIndex; i < m_ctx.vecPoint.size(); ++i)
            {
                remain.push_back(m_ctx.vecPoint[i]);
            }
            m_ctx.vecPoint = remain;

            run();
        }
        else
        {
            m_ctx.move.vPosition = m_ctx.vecPoint.back(); 
            m_ctx.vecPoint.clear();
            m_ctx.eState = BEHAVIOUR_STATE_FINISH;
        }
        m_ctx.dwStartTick = dwTick;
        m_ctx.dwLastUpdateTick = dwTick;
    }

    virtual void release()
    {
        delete this;
    }

private:
    WalkBehaviour(){}

    // 返回下一个点的索引 以及当前的位置
    // 返回-1表示 已到终点
    int getWalkProgress(DWORD dwNow, Move3& move)
    {
        if(m_ctx.vecPoint.empty() || m_ctx.dwStartTick == 0)
        {
            return -1;
        }

        int nElapse = dwNow - m_ctx.dwStartTick;
        if(nElapse < 0)
        {
            // 下一个点是起点
            move.vPosition = m_ctx.vecPoint.front();
            return 0;
        }

        float fPass = m_ctx.fStartSpeed / 1000 * nElapse;

        int nPointNum = (int)m_ctx.vecPoint.size();
        for(int i = 1; i < nPointNum; ++i)
        {
            Vector3 vLine = m_ctx.vecPoint[i] - m_ctx.vecPoint[i-1]; 
            float fLen = vLine.getLength();
            if(fLen > -0.0001 && fLen < 0.0001)
            {
                continue;
            }

            if ( fPass > fLen ) // 递进到下一个路点
            {
                fPass -= fLen;
                continue;
            }

            move.vVelocity = vLine * (m_ctx.fStartSpeed / fLen);    // 三维瞬时速度
            move.vPosition = m_ctx.vecPoint[i-1] + vLine * (fPass / fLen);   // 位置
            if(!m_ctx.bKeepLookDir) move.vRotation = vLine.toEulerAngles();

            return i;
        }

        move.vVelocity = Vector3();
        move.vPosition = m_ctx.vecPoint.back();
        if(!m_ctx.bKeepLookDir && nPointNum > 1) 
        {
            move.vRotation = (m_ctx.vecPoint[nPointNum - 1] - m_ctx.vecPoint[nPointNum - 2]).toEulerAngles();
        }

        return -1;
    }
};

// 跳跃行为 调用者负责监测跳跃停止（落地）
class JumpBehaviour : public Behaviour
{
    JumpCtx m_ctx;
public:
    static Behaviour* create()
    {
        return new JumpBehaviour();
    }

    virtual BEHAVIOURTYPE getType() const
    {
        return BEHAVIOUR_TYPE_JUMP;
    }

    virtual BehaviourCtx& getCtx()
    {
        return m_ctx;
    }

    virtual bool getStartPoint(Vector3& ptStart)
    {
        if(m_ctx.dwStartTick == 0)
        {
            return false;
        }

        ptStart = m_ctx.startLoc;
        return true;
    }

    virtual bool getTargetPoint(Vector3& ptTarget)
    {
        if(m_ctx.dwStartTick == 0)
        {
            return false;
        }

        ptTarget = m_ctx.targetLoc;
        return true;
    }

    virtual bool isRunning()
    {
        return m_ctx.eState == BEHAVIOUR_STATE_RUNNING;
    }

    virtual bool run()
    {
        m_ctx.eState = BEHAVIOUR_STATE_INVALID;
        if(m_ctx.dwStartTick == 0)
            return false;
        m_ctx.eState = BEHAVIOUR_STATE_RUNNING;
        m_ctx.dwLastUpdateTick = m_ctx.dwStartTick;
        return true;
    }

    virtual void update(DWORD dwNow, Vector3* pTargetLoc = NULL)
    {
        if(!isRunning())
        {
            return;
        }

        if(dwNow < m_ctx.dwStartTick)
        {
            return;
        }

        float fElapse = ((float)dwNow - (float)m_ctx.dwStartTick) / (float)1000; 
        Vector3 startSpeed = (m_ctx.targetLoc - m_ctx.startLoc).normalize() * m_ctx.fStartSpeed;

        // 垂直方向自由落体运动
        float curY = m_ctx.startLoc.y + ((startSpeed.y) * fElapse - 0.5f * 9.8f * fElapse * fElapse);

        // 水平方向做匀速直线运动
        Vector3 ptNow = m_ctx.startLoc + startSpeed * fElapse;
        ptNow.y = curY;

        m_ctx.move.vPosition = ptNow;
        m_ctx.move.vVelocity = startSpeed;
        m_ctx.move.vVelocity.y -= 9.8f * fElapse;

        if(!m_ctx.bKeepLookDir)
        {
            Vector3 ptLookDir = m_ctx.targetLoc - m_ctx.startLoc;
            ptLookDir.y = 0;
            m_ctx.move.vRotation = ptLookDir.toEulerAngles();
        }

        m_ctx.dwLastUpdateTick = dwNow;
    }

    virtual void adjustCtx(DWORD dwTick)
    {
        if(!isRunning())
        {
            return;
        }
        update(dwTick);
        m_ctx.startLoc = m_ctx.move.vPosition;
        m_ctx.dwStartTick = dwTick;

        m_ctx.dwLastUpdateTick = dwTick;
    }

    virtual void release()
    {
        delete this;
    }
private:
    JumpBehaviour(){}
};

// 冲锋行为
class ShiftBehaviour : public Behaviour
{
    ShiftCtx m_ctx;
    float m_fVertA;  // 垂直方向加速度
    float m_fVertV0; // 垂直方向初速度
public:
    static Behaviour* create()
    {
        return new ShiftBehaviour();
    }

    virtual BEHAVIOURTYPE getType() const
    {
        return BEHAVIOUR_TYPE_SHIFT;
    }

    virtual BehaviourCtx& getCtx()
    {
        return m_ctx;
    }

    virtual bool getStartPoint(Vector3& ptStart)
    {
        if(m_ctx.shiftMode == shift_mode_invalid)
        {
            return false;
        }

        ptStart = m_ctx.startLoc;
        return true;
    }

    virtual bool getTargetPoint(Vector3& ptTarget)
    {
        if(m_ctx.shiftMode == shift_mode_invalid)
        {
            return false;
        }

        ptTarget = m_ctx.targetLoc;
        return true;
    }

    virtual bool isRunning()
    {
        return m_ctx.eState ==  BEHAVIOUR_STATE_RUNNING;
    }

    virtual bool run()
    {
        m_ctx.eState = BEHAVIOUR_STATE_INVALID;
        if(m_ctx.shiftMode == shift_mode_invalid || m_ctx.dwStartTick == 0)
            return false;

        if(m_ctx.shiftMode == shift_mode_forward_entity)
        {
            // 如果本身就足够贴近目标 直接停止
            if(m_ctx.fCloseDistance < -0.001 && (m_ctx.targetLoc - m_ctx.move.vPosition).getLength() <= abs(m_ctx.fCloseDistance))
                return false;

            m_ctx.bArrived = false;
        }

        // 是不是抛物线
        if(m_ctx.shiftMode == shift_mode_forward_loc && m_ctx.bUseHeight) 
        {
            // 垂直方向作类似自由落体运动
            // (上升时间t1(未知) 下落时间t2(未知) 总时间tt(已知) 加速度a(未知) 上升高度h1(已知) 下落高度h2(已知))
            // t1 + t2 = tt;
            // 0.5*a*t1*t1 = h1;
            // 0.5*a*t2*t2 = h2;  三个方程 三个未知数
            // ==> t1 = tt - tt/(sqrt(h1/h2) + 1); t2 = tt - t1; a = 2*h1 / (t1*t1); ==>初始速度V0= a*t1
            // 目标点比配置的高度还高的话 抛物线是不能经过目标点的 把目标点Y坐标调整为起点Y坐标
            if(m_ctx.targetLoc.y > m_ctx.startLoc.y + m_ctx.fHeight)
            {
                m_ctx.targetLoc.y = m_ctx.startLoc.y;
            }
            float fH1 = m_ctx.fHeight;
            float fH2 = m_ctx.fHeight - (m_ctx.targetLoc.y - m_ctx.startLoc.y);
            float fExpectTime = (m_ctx.targetLoc - m_ctx.startLoc).getLength() * (1 / m_ctx.fStartSpeed); 

            float fT1 = fExpectTime - fExpectTime / (sqrt(fH1/fH2) + 1);
            m_fVertA = -2.0f * fH1 / (fT1 * fT1);  // 加速度向下
            m_fVertV0 = -m_fVertA * fT1;  // 初始速度向上
        }

        m_ctx.eState = BEHAVIOUR_STATE_RUNNING;
        m_ctx.move.vVelocity = (m_ctx.targetLoc - m_ctx.startLoc).normalize() * m_ctx.fStartSpeed;

        m_ctx.dwLastUpdateTick = m_ctx.dwStartTick;

        return true;
    }

    virtual void update(DWORD dwNow, Vector3* pTargetLoc = NULL)
    {
        if(!isRunning())
            return;

        int nElapse = dwNow - m_ctx.dwStartTick;
        if(nElapse < 0)
        {
            return;
        }

        if(m_ctx.shiftMode == shift_mode_forward_loc)
        {
            bool bTimeOut = false;
            if(m_ctx.maxTime > 0 && nElapse >= m_ctx.maxTime)
            {
                nElapse = m_ctx.maxTime; 
                bTimeOut = true;
            }

            float fPass = m_ctx.fStartSpeed / 1000 * nElapse;

            Vector3 vLine = m_ctx.targetLoc - m_ctx.startLoc; 
            float fLen = vLine.getLength();
            if(fLen < 0.0001 || fPass > fLen) 
            {
                m_ctx.move.vPosition = m_ctx.targetLoc;
                m_ctx.move.vVelocity = Vector3(0, 0, 0);
                m_ctx.eState = BEHAVIOUR_STATE_FINISH;
                return;
            }

            m_ctx.move.vVelocity = vLine * (m_ctx.fStartSpeed / fLen);    // 三维瞬时速度
            m_ctx.move.vPosition = m_ctx.startLoc + vLine * (fPass / fLen);   // 位置

            // 垂直方向类自由落体
            if(m_ctx.bUseHeight)
            {
                float fElapse = nElapse;
                fElapse /= 1000;
                m_ctx.move.vPosition.y = m_ctx.startLoc.y + m_fVertV0 * fElapse + 0.5 * m_fVertA * (fElapse * fElapse); 

                m_ctx.move.vVelocity += Vector3(0, 1, 0)*(m_fVertV0 + m_fVertA * fElapse);
            }

            if(!m_ctx.bKeepLookDir) m_ctx.move.vRotation = vLine.toEulerAngles();
            
            if(bTimeOut)
                m_ctx.eState = BEHAVIOUR_STATE_FINISH; 

        }else if(m_ctx.shiftMode == shift_mode_forward_entity)
        {
            if(nElapse >= m_ctx.maxTime || pTargetLoc == NULL)
            {
                m_ctx.eState = BEHAVIOUR_STATE_FINISH; 
                return;
            }

            float fDistance = (m_ctx.move.vPosition - *pTargetLoc).getLength();
            // 与目标距离超过一定值则打断(等于0不比较)
            if(!equalto0(m_ctx.fBreakDistance) && fDistance >= abs(m_ctx.fBreakDistance))
            {
                m_ctx.eState = BEHAVIOUR_STATE_FINISH; 
                return;
            }

            // 贴近距离 负数表示还未到目标点的时候停下来  正数表示到了目标点后再继续往前一段距离停下来
            // 根据贴近距离 还未追到目标实体的时候调整冲锋目标点
            if(!equalto0(m_ctx.fCloseDistance) && !m_ctx.bArrived)
            {
                m_ctx.targetLoc = *pTargetLoc + (*pTargetLoc - m_ctx.move.vPosition).normalize() * m_ctx.fCloseDistance; 
            }

            int nElapseSinceLastFrame = dwNow - m_ctx.dwLastUpdateTick;
            float fPass = m_ctx.fStartSpeed * (float)nElapseSinceLastFrame / (float)1000;

            if(!m_ctx.bArrived && fPass >= fDistance)
            {
                m_ctx.bArrived = true;
            }

            Vector3 dirForward = m_ctx.targetLoc - m_ctx.move.vPosition;
            float fForwardLen = dirForward.getLength();
            
            if(fPass >= fForwardLen)
            {
                // 停止
                m_ctx.move.vPosition = m_ctx.targetLoc;
                m_ctx.move.vVelocity = Vector3(0, 0, 0);
                m_ctx.eState = BEHAVIOUR_STATE_FINISH; 
                return;
            }

            m_ctx.move.vVelocity = dirForward * (m_ctx.fStartSpeed / fForwardLen);    // 三维瞬时速度
            m_ctx.move.vPosition = m_ctx.move.vPosition + dirForward * (fPass / fForwardLen);   // 位置
            if(!m_ctx.bKeepLookDir) m_ctx.move.vRotation = dirForward.toEulerAngles();
        }

        m_ctx.dwLastUpdateTick = dwNow; // 记录上次更新时刻
    }

    virtual void adjustCtx(DWORD dwTick)
    {
        if(!isRunning())
        {
            return;
        }
        update(dwTick);
        m_ctx.startLoc = m_ctx.move.vPosition;
        m_ctx.dwStartTick = dwTick;
        m_ctx.dwLastUpdateTick = dwTick;
    }

    virtual void release()
    {
        delete this;
    }

private:
    ShiftBehaviour(){}
};

template <class buffer_allocator>
obuffer<buffer_allocator>& _cdecl operator<<(obuffer<buffer_allocator>& ob, const BehaviourCtx& val)
{
    switch(val.getType())
    {
    case BEHAVIOUR_TYPE_WALK:
        {
            WalkCtx& ctx = (WalkCtx&)const_cast<BehaviourCtx&>(val);
            return ctx.operator>>(ob);
        }
    case BEHAVIOUR_TYPE_JUMP:
        {
            JumpCtx& ctx = (JumpCtx&)const_cast<BehaviourCtx&>(val);
            return ctx.operator>>(ob);
        }
    case BEHAVIOUR_TYPE_SHIFT:
        {
            ShiftCtx& ctx = (ShiftCtx&)const_cast<BehaviourCtx&>(val);
            return ctx.operator>>(ob);
        }
    }

    ErrorLn(__FUNCTION__ << ", type=" << val.getType());
    return ob;
}

inline ibuffer& _cdecl operator>>(ibuffer& ib, BehaviourCtx& val)
{
    val.operator<<(ib);
    return ib;
}

