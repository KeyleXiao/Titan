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
using org.critterai.u3d;
using org.critterai.nav;
using org.critterai.nav.u3d;
using org.critterai.samples;
using UnityEngine;

/// <summary>
/// A simulation manager that demonstates path corridor features.
/// </summary>
/// <remarks>
/// <para>Not designed to be used with other simulation managers.</para>
/// </remarks>
public sealed class CorridorExplorerSim 
    : MonoBehaviour 
{

    private const float ControlAreaHeight = 210;
    private const float TimerLength = 2;

    public int maxPathSize = 500;
    public int maxCorners = 4;
    public float optimizationRange = 15;

    public float raycastLength = 150;

    private NavGroup mGroup;

    private BufferHandler<uint> mPath;
    private int mPathCount;
    private NavmeshPoint mPathStart;
    private NavmeshPoint mPathEnd;

    private PathCorridor mCorridor;
    private PathCorridorData mCorridorData;  // For debug render purposes only.

    private Camera mCam;

    private BufferHandler<uint> mCornerPolys;  // Only used for GUI features.

    private Vector3 mOptimizeStart = new Vector3();
    private Vector3 mOptimizeEnd = new Vector3();
    private float mOptimizeTimer = 0;

    public string HelpText
    {
        get
        {
            if (mPathStart.polyRef == 0)
                return string.Format("Set Start Point: [{0}]", StdButtons.SelectA);
            else if (mPathEnd.polyRef == 0)
                return string.Format("Set Goal Point: [{0}]", StdButtons.SelectA);
            else
            {
                return string.Format(
                    "Set Point: Position: [{2}], Target: [{3}], Optimize Vis : [{4}], Goal: [{0}], Start: [{1}]"
                    , StdButtons.SetA, StdButtons.SetB, StdButtons.SelectA, StdButtons.SelectB, StdButtons.SetC);
            }
        }
    }

    private bool InitializeNav()
    {
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

        mGroup = manager.NavGroup;

        maxPathSize = Mathf.Max(1, maxPathSize);

        mPath = new BufferHandler<uint>(
            "Max Path Size", 1, maxPathSize, 1, Mathf.Max(100, maxPathSize));

        mPathStart = new NavmeshPoint();
        mPathEnd = new NavmeshPoint();

        maxCorners = Mathf.Max(1, maxCorners);

        mCornerPolys = 
            new BufferHandler<uint>("Max Corner Size", 1, maxCorners, 1, Mathf.Max(maxCorners, 20));

        HandlePathBufferResize();

        return true;
    }

    private void HandlePathBufferResize()
    {
        mCorridor = new PathCorridor(
            mPath.MaxElementCount, mCornerPolys.MaxElementCount, mGroup.query, mGroup.filter);

        mCorridorData = new PathCorridorData(mPath.MaxElementCount);

        if (mPathEnd.polyRef != 0)
            FindPath();
        else if (mPathStart.polyRef != 0)
            mCorridor.Reset(mPathStart);
    }

    void Awake()
    {
        SimGUIUtil.Initialize();
    }

    void Start() 
    {
        mCam = Camera.main;
        InitializeNav();
        InitializeGUI();
    }

    private void InitializeGUI()
    {
        SimGUIUtil.BuildLabelRegion(true);
        SimGUIUtil.contextControlZone.height = ControlAreaHeight;
        SimGUIUtil.contextActive = true;
    }

    void Update() 
    {
        if (Time.timeScale == 0)
            return;

        mOptimizeTimer -= Time.deltaTime;

        if (!SimGUIUtil.InControlZone(Input.mousePosition))
        {
            Ray ray = mCam.ScreenPointToRay(Input.mousePosition);
            RaycastHit hit;
            if (Physics.Raycast(ray, out hit, raycastLength))
                HandleHit(hit);
        }

        SimGUIUtil.contextHelpText = HelpText;
    }

    private void HandleHit(RaycastHit hit)
    {
        if (Input.GetKeyDown(StdButtons.SelectA))
        {
            if (mPathStart.polyRef == 0 || mPathEnd.polyRef == 0)
            {
                NavmeshPoint point = GetNavmeshPoint(hit.point);
                if (point.polyRef == 0)
                    return;

                if (mPathStart.polyRef == 0)
                {
                    mPathStart = point;
                    mCorridor.Reset(mPathStart);
                    return;
                }
                else
                {
                    mPathEnd = point;
                    FindPath();
                    return;
                }
            }
            else if (mPathCount > 0)
            {
                mCorridor.MovePosition(hit.point);
                return;
            }
        }
        else if (mPathCount > 0)
        {
            if (Input.GetKeyDown(StdButtons.SelectB))
            {
                mCorridor.MoveTarget(hit.point);
                return;
            }
            else if (Input.GetKeyDown(StdButtons.SetA))
            {
                NavmeshPoint point = GetNavmeshPoint(hit.point);
                if (point.polyRef != 0)
                {
                    mPathEnd = point;
                    FindPath();
                }
                return;
            }
            else if (Input.GetKeyDown(StdButtons.SetB))
            {
                NavmeshPoint point = GetNavmeshPoint(hit.point);
                if (point.polyRef != 0)
                {
                    mPathStart = point;
                    FindPath();
                }
                return;
            }
            else if (Input.GetKeyDown(StdButtons.SetC))
            {
                NavmeshPoint point = GetNavmeshPoint(hit.point);
                if (point.polyRef != 0)
                {
                    mCorridor.OptimizePathVisibility(point.point, optimizationRange, true);
                    mOptimizeStart = mCorridor.Position.point;
                    mOptimizeEnd = point.point;
                    mOptimizeTimer = TimerLength;
                }
            }
        }
    }

    private NavmeshPoint GetNavmeshPoint(Vector3 searchPoint)
    {
        NavmeshPoint result;
        mGroup.query.GetNearestPoint(searchPoint, mGroup.extents, mGroup.filter, out result);
        return result;
    }

    private void FindPath()
    {
        NavStatus status = mGroup.query.FindPath(
            mPathStart, mPathEnd, mGroup.filter, mPath.buffer, out mPathCount);

        mCorridor.Reset(mPathStart);
        if (NavUtil.Succeeded(status) && mPathCount > 0)
            mCorridor.SetCorridor(mPathEnd.point, mPath.buffer, mPathCount);
    }

    void OnGUI()
    {
        if (SimGUIUtil.hideGUI || mGroup.mesh == null)
            return;

        LabelRegion labels = SimGUIUtil.labels;
        labels.Clear();

        if (mPathCount == 0)
        {
            labels.Last = "No path.";
            SimGUIUtil.OnGUI();
            return;
        }

        GUILayout.BeginArea(SimGUIUtil.contextControlZone, GUI.skin.box);

        if (GUILayout.Button("Optimize Topology"))
            mCorridor.OptimizePathTopology(true);

        GUI.enabled = (mPathEnd.polyRef != 0);
        if (GUILayout.Button("Replan Path"))
            FindPath();

        GUI.enabled = true;

        mPath.OnGUI();
        if (mPath.HandleResize())
            HandlePathBufferResize();

        mCornerPolys.OnGUI();
        if (mCornerPolys.HandleResize())
            mCorridor.ResizeCornerBuffer(mCornerPolys.MaxElementCount);

        GUILayout.Label("Optimiation Range");

        GUILayout.BeginHorizontal();

        optimizationRange = (int)GUILayout.HorizontalSlider(optimizationRange, 1, 30);

        GUILayout.Label(optimizationRange.ToString());

        GUILayout.EndHorizontal();

        GUILayout.EndArea();

        const int LabelCount = 2;
        int slotIndex = labels.SlotCount - LabelCount;

        string msg = mCorridor.Corners.cornerCount + " of "+ (mCorridor.Corners.MaxCorners - 1);
        labels.Set(slotIndex++, "Detected Corners", msg);

        msg = mPathCount + " of " + mCorridor.MaxPathSize;
        labels.Set(slotIndex++, "Corridor Polygons", msg);

        SimGUIUtil.OnGUI();
    }

    void OnRenderObject()
    {
        if (Time.timeScale == 0 || mGroup.mesh == null)
            return;

        mCorridor.GetData(mCorridorData);
        NavDebug.Draw(mGroup.mesh, false);
        NavDebug.Draw(mGroup.mesh, mCorridorData);
        NavDebug.Draw(mCorridor.Corners);

        Color c = NavDebug.positionColor;
        c.a = 0.25f;

        if (mPathStart.polyRef != 0)
            DebugDraw.DiamondMarker(mPathStart.point, 0.1f, c);

        c = NavDebug.goalColor;
        c.a = 0.25f;

        if (mPathEnd.polyRef != 0)
            DebugDraw.DiamondMarker(mPathEnd.point, 0.1f, c);

        if (mOptimizeTimer > 0)
        {
            c = NavDebug.goalColor;
            c.a = mOptimizeTimer / TimerLength;
            DebugDraw.Arrow(mOptimizeStart, mOptimizeEnd, 0, 0.1f, c);
            DebugDraw.Circle(mOptimizeStart, optimizationRange, c);
        }
    }
}
