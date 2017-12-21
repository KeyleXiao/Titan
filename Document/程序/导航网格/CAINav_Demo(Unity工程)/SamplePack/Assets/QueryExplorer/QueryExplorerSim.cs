/*
 * Copyright (c) 2011-2012 Stephen A. Pratt
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
using UnityEngine;
using org.critterai.nav.u3d;
using org.critterai.samples.qe;
using org.critterai.samples;

/// <summary>
/// A simulation manager which demonstates navigation mesh query features.
/// </summary>
/// <remarks>
/// <para>Not designed to be used with other simulation managers.</para>
/// </remarks>
public sealed class QueryExplorerSim 
    : MonoBehaviour 
{
    public QEStateType InitialState = QEStateType.GetNearestPointG;
    private string[] toolBarText;

    private NavGroup mHelper;
    private IQueryExplorerState mState;

    public QEStateType State
    {
        get { return (mState == null ? QEStateType.Inactive : mState.StateType); }
    }

    public bool SetState(QEStateType state)
    {
        if (State == QEStateType.Inactive && !Initialize())
            return false;

        if (mState != null)
            mState.Exit();

        mState = null;

        switch (state)
        {
            case QEStateType.GetNearestPointG:
                mState = new GetNearestPointG();
                break;
            case QEStateType.FindDistanceToWall:
                mState = new FindDistanceToWall();
                break;
            case QEStateType.FindPolysByRadius:
                mState = new FindPolysByRadius();
                break;
            case QEStateType.FindPolysByConvex:
                mState = new FindPolysByConvex();
                break;
            case QEStateType.GetPolysLocal:
                mState = new GetPolysLocal();
                break;
            case QEStateType.GetNearestPointL:
                mState = new GetNearestPointL();
                break;
            case QEStateType.GetNearestPointF:
                mState = new GetNearestPointF();
                break;
            case QEStateType.GetPolyHeight:
                mState = new GetPolyHeight();
                break;
            case QEStateType.GetPolys:
                mState = new GetPolys();
                break;
            case QEStateType.GetWallSegments:
                mState = new GetWallSegments();
                break;
            case QEStateType.GetSegments:
                mState = new GetSegments();
                break;
            case QEStateType.FindPath:
                mState = new FindPath();
                break;
            case QEStateType.StraightPath:
                mState = new StraightPath();
                break;
            case QEStateType.Raycast:
                mState = new RaycastQE();
                break;
            case QEStateType.MoveOnSurface:
                mState = new MoveAlongSurface();
                break;
        }

        mState.Enter(mHelper);

        return true;
    }

    private bool Initialize()
    {
        if (mState != null)
            mState.Exit();

        mState = null;

        NavManagerProvider provider =
            (NavManagerProvider)FindObjectOfType(typeof(NavManagerProvider));

        if (provider == null)
        {
            Debug.LogError(string.Format("{0}: There is no {1} in the scene."
                , name, typeof(NavManagerProvider).Name));

            return false;
        }

        NavManager manager = provider.CreateManager();

        if (manager == null)
        {
            Debug.LogError(string.Format("{0}: Could not get the navigation manager.", name));

            return false;
        }

        mHelper = manager.NavGroup;

        return true;
    }

    void Start() 
    {
        // Order must match int values in QEStateType
        toolBarText = new string[]
        {
            "Get Nearest Point (Global)",
            "Get Nearest Point (Local)",
            "Get Nearest Point (Fast)",

            "Find Distance to Wall", 
            "Get Wall Segments",
            "Get Poly Segments",
            "Get Poly Height",
            "Get Polys (AABB Overlap)",

            "Get Polys (Circle Overlap)",
            "Find Polys (Circle Overlap)",
            "Find Polys (Poly Overlap)",

            "Find Path",
            "Get Straight Path",
            "Raycast",
            "Move Along Surface"
        };

        SetState(InitialState);
    }

    void Update() 
    {
        if (Time.timeScale == 0 || mState == null)
            return;

        QEUtil.Raycast();
        mState.Update();
    }

    void OnGUI()
    {
        if (Time.timeScale == 0 || SimGUIUtil.hideGUI)
            return;

        if (State == QEStateType.Inactive)
            // Something broke.
            return;

        QEUtil.buttonZone = new Rect(SimGUIUtil.sysControlZone.xMin
            , SimGUIUtil.sysControlZone.yMax + SimGUIUtil.Margin
            , SimGUIUtil.sysControlZone.width
            , toolBarText.Length * SimGUIUtil.LineHeight + SimGUIUtil.Margin);

        GUILayout.BeginArea(QEUtil.buttonZone, GUI.skin.box);

        QEStateType sn = (QEStateType) GUILayout.SelectionGrid((int)State
            , toolBarText
            , 1
            , GUI.skin.toggle);

        GUILayout.EndArea();

        if (sn != State)
            SetState(sn);

        if (mState != null)
            mState.OnGUI();

        SimGUIUtil.OnGUI();
    }

    void OnRenderObject()
    {
        if (Time.timeScale == 0 || mState == null)
            return;

        mState.OnRenderObject();
    }

    void OnDestroy()
    {
        if (mState != null)
            mState.Exit();
    }
}
