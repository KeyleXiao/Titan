/*******************************************************************
** �ļ���:	MovePath.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	CHEN.TAO!~
** ��  ��:	8/2/2015
** ��  ��:	1.0
** ��  ��:	
			�ƶ���Ϊ������,���ڷ������ͻ���·��ͬ��ģ��
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

// ��Ϊ�����Ļ���
struct BehaviourCtx
{
    BEHAVIOURSTATE eState;
    DWORD dwStartTick;    // ��ʼʱ��
    DWORD dwLastUpdateTick; // �ϴθ���ʱ��
    float fStartSpeed;    // ����ʼ���ٶ�
    bool bKeepLookDir;    // �Ƿ񱣳ֳ��� 
    Move3 move;          // ��ǰ�ƶ���Ԫ��

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

// ��·������
struct WalkCtx : public BehaviourCtx
{
    std::vector<Vector3> vecPoint;  // ·��
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

// ��Ծ������
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

// ���ģʽ
enum ShiftMode
{
    shift_mode_invalid,         // ��Чģʽ
    shift_mode_forward_loc,     // ����Ŀ���
    shift_mode_forward_entity,  // ׷��ʵ��
};

// ���������
struct ShiftCtx : public BehaviourCtx
{
    friend class ShiftBehaviour;

    ShiftMode shiftMode;// ���ģʽ
    int nPriority;      // ������ȼ�
    int nAnimation;     // ����������(MOVE_TYPE)
    Vector3 startLoc;   // ������
    Vector3 targetLoc;  // ���Ŀ���
    UID uidTarget;      // Ŀ��ʵ��
    int maxTime;        // ������ʱ��

    float fHeight;        // ���ɸ߶�
    bool  bUseHeight;     // �Ƿ�ʹ�ø߶�(�Ƿ���������)
    bool bPassive;        // �Ƿ񱻶�λ��
    float fCloseDistance; // ��Ŀ�걣�־��� ��Ŀ�����<fCloseDistance���� 
    float fBreakDistance; // ��Ŀ�����>fBreakDistance����
    DWORD dwSpellStartTick;  // ����ʹ��ʱ��  
    bool bCanCrossObstacle;  // �Ƿ�ɿ�Խ�ϰ�
    int nHitObstacleAddBuffID; // ײ���ϰ�ʱ����buffID
    int nHitObstacleAddBuffLevel; // buff�ȼ�
    int nMaxUpAngle;         // ����������(0-90)
   
    UID uidFinishBuff_Target;   // ������buffĿ��
    UID uidFinishBuff_Operator; // ������buff �����
    int nFinishBuff_ID;         // ������buffID
    int nFinishBuff_Level;      // ������buffLevel

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
    bool bArrived;         // ׷��Ŀ��Ļ�Ҫ֧����ǰ����һ�ξ���
};

// ��Ϊ
struct Behaviour
{
    virtual BEHAVIOURTYPE getType() const = 0;

    virtual BehaviourCtx& getCtx() = 0;

    virtual bool getStartPoint(Vector3& ptStart) = 0;

    virtual bool getTargetPoint(Vector3& ptTarget) = 0;

    virtual bool isRunning() = 0;

    virtual bool run() = 0;

    virtual void update(DWORD dwTick, Vector3* pTargetLoc = NULL) = 0;

    // ���������� ֻ����dwTick֮��Ĳ���
    virtual void adjustCtx(DWORD dwTick) = 0;

    virtual void release() = 0;
};

// ��·��Ϊ
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

    // ������һ��������� �Լ���ǰ��λ��
    // ����-1��ʾ �ѵ��յ�
    int getWalkProgress(DWORD dwNow, Move3& move)
    {
        if(m_ctx.vecPoint.empty() || m_ctx.dwStartTick == 0)
        {
            return -1;
        }

        int nElapse = dwNow - m_ctx.dwStartTick;
        if(nElapse < 0)
        {
            // ��һ���������
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

            if ( fPass > fLen ) // �ݽ�����һ��·��
            {
                fPass -= fLen;
                continue;
            }

            move.vVelocity = vLine * (m_ctx.fStartSpeed / fLen);    // ��ά˲ʱ�ٶ�
            move.vPosition = m_ctx.vecPoint[i-1] + vLine * (fPass / fLen);   // λ��
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

// ��Ծ��Ϊ �����߸�������Ծֹͣ����أ�
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

        // ��ֱ�������������˶�
        float curY = m_ctx.startLoc.y + ((startSpeed.y) * fElapse - 0.5f * 9.8f * fElapse * fElapse);

        // ˮƽ����������ֱ���˶�
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

// �����Ϊ
class ShiftBehaviour : public Behaviour
{
    ShiftCtx m_ctx;
    float m_fVertA;  // ��ֱ������ٶ�
    float m_fVertV0; // ��ֱ������ٶ�
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
            // ���������㹻����Ŀ�� ֱ��ֹͣ
            if(m_ctx.fCloseDistance < -0.001 && (m_ctx.targetLoc - m_ctx.move.vPosition).getLength() <= abs(m_ctx.fCloseDistance))
                return false;

            m_ctx.bArrived = false;
        }

        // �ǲ���������
        if(m_ctx.shiftMode == shift_mode_forward_loc && m_ctx.bUseHeight) 
        {
            // ��ֱ�������������������˶�
            // (����ʱ��t1(δ֪) ����ʱ��t2(δ֪) ��ʱ��tt(��֪) ���ٶ�a(δ֪) �����߶�h1(��֪) ����߶�h2(��֪))
            // t1 + t2 = tt;
            // 0.5*a*t1*t1 = h1;
            // 0.5*a*t2*t2 = h2;  �������� ����δ֪��
            // ==> t1 = tt - tt/(sqrt(h1/h2) + 1); t2 = tt - t1; a = 2*h1 / (t1*t1); ==>��ʼ�ٶ�V0= a*t1
            // Ŀ�������õĸ߶Ȼ��ߵĻ� �������ǲ��ܾ���Ŀ���� ��Ŀ���Y�������Ϊ���Y����
            if(m_ctx.targetLoc.y > m_ctx.startLoc.y + m_ctx.fHeight)
            {
                m_ctx.targetLoc.y = m_ctx.startLoc.y;
            }
            float fH1 = m_ctx.fHeight;
            float fH2 = m_ctx.fHeight - (m_ctx.targetLoc.y - m_ctx.startLoc.y);
            float fExpectTime = (m_ctx.targetLoc - m_ctx.startLoc).getLength() * (1 / m_ctx.fStartSpeed); 

            float fT1 = fExpectTime - fExpectTime / (sqrt(fH1/fH2) + 1);
            m_fVertA = -2.0f * fH1 / (fT1 * fT1);  // ���ٶ�����
            m_fVertV0 = -m_fVertA * fT1;  // ��ʼ�ٶ�����
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

            m_ctx.move.vVelocity = vLine * (m_ctx.fStartSpeed / fLen);    // ��ά˲ʱ�ٶ�
            m_ctx.move.vPosition = m_ctx.startLoc + vLine * (fPass / fLen);   // λ��

            // ��ֱ��������������
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
            // ��Ŀ����볬��һ��ֵ����(����0���Ƚ�)
            if(!equalto0(m_ctx.fBreakDistance) && fDistance >= abs(m_ctx.fBreakDistance))
            {
                m_ctx.eState = BEHAVIOUR_STATE_FINISH; 
                return;
            }

            // �������� ������ʾ��δ��Ŀ����ʱ��ͣ����  ������ʾ����Ŀ�����ټ�����ǰһ�ξ���ͣ����
            // ������������ ��δ׷��Ŀ��ʵ���ʱ��������Ŀ���
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
                // ֹͣ
                m_ctx.move.vPosition = m_ctx.targetLoc;
                m_ctx.move.vVelocity = Vector3(0, 0, 0);
                m_ctx.eState = BEHAVIOUR_STATE_FINISH; 
                return;
            }

            m_ctx.move.vVelocity = dirForward * (m_ctx.fStartSpeed / fForwardLen);    // ��ά˲ʱ�ٶ�
            m_ctx.move.vPosition = m_ctx.move.vPosition + dirForward * (fPass / fForwardLen);   // λ��
            if(!m_ctx.bKeepLookDir) m_ctx.move.vRotation = dirForward.toEulerAngles();
        }

        m_ctx.dwLastUpdateTick = dwNow; // ��¼�ϴθ���ʱ��
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

