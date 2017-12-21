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
using org.critterai.nav;
using org.critterai.samples;

/// <summary>
/// A simulation manager that demonstates the crowd manager.
/// </summary>
/// <remarks>
/// <para>
/// Not designed to be used concurrently with other simulation managers.
/// </para>
/// </remarks>
[RequireComponent(typeof(StringList))]
public class CrowdDemoSim 
    : MonoBehaviour 
{
    /*
     * Design note:
     * 
     * This demo overrides any pre-configured agent component configuration.
     * So there is no need to set that up beforehand.
     */
    public GameObject agentA;
    public GameObject agentB;
    public GameObject agentC;

    public byte agentGroupA;
    public byte agentGroupB;
    public byte agentGroupC;

    private Camera mCam;

    private NavManager mManager;
    private NavGroup mNav;

    private string[] agentNames;

    private Agent mActiveAgent;
    private Agent mHitAgent;

	void Start () 
    {
        if (agentA == null || agentB == null|| agentC == null)
        {
            Debug.LogError(name + ": One or more agents are not assigned.");
            enabled = false;
            return;
        }

        NavManagerProvider provider = 
            (NavManagerProvider)FindObjectOfType(typeof(NavManagerProvider));

        if (provider == null)
        {
            Debug.LogError(string.Format("{0}: There is no {1} in the scene."
                , name , typeof(NavManagerProvider).Name));

            enabled = false;
            return;
        }

        NavManager.ActiveManager = provider.CreateManager();  // Provides error reporting.

        if (NavManager.ActiveManager == null)
        {
            enabled = false;
            return;
        }

        mManager = NavManager.ActiveManager;
        mNav = mManager.NavGroup;

        SimGUIUtil.contextHelpText = string.Format(
            "Agent: Add: [{0}], Select: [{1}], Move Selected: [{2}]"
            , StdButtons.SetA, StdButtons.SelectB, StdButtons.SelectA);

        SimGUIUtil.contextControlZone.height = SimGUIUtil.LineHeight + SimGUIUtil.Margin;
        SimGUIUtil.contextActive = true;

        mCam = Camera.main;

        agentNames = GetComponent<StringList>().values;
	}

	void Update ()
    {
        if (Time.frameCount % 30 == 0)
            System.GC.Collect();

        SimGUIUtil.Update();

        if (!SimGUIUtil.hasHit)
        {
            mHitAgent = null;
            return;
        }

        mHitAgent = SimGUIUtil.hit.collider.gameObject.GetComponent<Agent>();

        // Handle Keyboard Input

        // The second input isn't shown in the help text.  But it is a logical
        // usability input.
        if (Input.GetKeyDown(StdButtons.SelectB)
            || (mActiveAgent == null && Input.GetKeyDown(StdButtons.SelectA)))
        {
            SetActiveAgent(mHitAgent);
            return;
        }

        if (Input.GetKeyDown(StdButtons.SetA) && mManager.GetAgentCount() < mManager.MaxAgents)
        {
            AddWanderAgent(SimGUIUtil.hit.point);
            return;
        }
	}

    void LateUpdate()
    {
        mManager.LateUpdate();
    }

    void OnDestroy()
    {
        NavManager.ActiveManager = null;
    }

    private void SetActiveAgent(Agent agent)
    {
        if (mActiveAgent == agent)
            return;

        if (mActiveAgent != null)
        {
            // Deactivate the current agent.
            mActiveAgent.UserControlled = false;
            mActiveAgent.DebugEnabled = false;
            mActiveAgent = null;
        }

        if (agent == null)
            return;

        mActiveAgent = agent;
        mActiveAgent.UserControlled = true;
        mActiveAgent.DebugEnabled = true;

        // Debug.Log("Selected: " + mActiveAgent.name);
    }

    private void AddWanderAgent(Vector3 position)
    {
        if (mNav.crowd.AgentCount == mNav.crowd.MaxAgents)
            return;

        // Snap position to the navigation mesh.

        NavmeshPoint navpos;
        NavStatus status =
            mNav.query.GetNearestPoint(position, mNav.extents, mNav.filter, out navpos);

        if (NavUtil.Failed(status) || navpos.polyRef == 0)
            return;

        position = navpos.point;

        // Create the base agent.

        int selector = mNav.crowd.AgentCount % 3;

        GameObject ao;
        byte ag;

        switch (selector)
        {
            case 0:
                ao = (GameObject)Instantiate(agentA);
                ag = agentGroupA;
                break;
            case 1:
                ao = (GameObject)Instantiate(agentB);
                ag = agentGroupB;
                break;
            default:
                ao = (GameObject)Instantiate(agentC);
                ag = agentGroupC;
                break;
        }

        ao.name = agentNames[mNav.crowd.AgentCount % agentNames.Length];

        // Position the agent.

        Transform trans = ao.transform;

        trans.position = position;

        position.y = 0;
        Vector3 campos = mCam.gameObject.transform.position;
        campos.y = 0;
        trans.rotation = Quaternion.LookRotation(campos - position);

        // Add/Initialize the agent component.

        CrowdDemoAgent agent = ao.GetComponent<CrowdDemoAgent>();
        if (agent == null)
            agent = ao.AddComponent<CrowdDemoAgent>();

        agent.agentGroup = ag;
    }

    void OnGUI()
    {
        if (mHitAgent != null)
        {
            Vector3 pos = mHitAgent.transform.position;
            pos.y += 2;

            pos = mCam.WorldToScreenPoint(pos);

            Rect rect = new Rect(pos.x - 10, Screen.height - pos.y - 15, 100, 22);

            GUI.Label(rect, mHitAgent.name);
        }

        GUILayout.BeginArea(SimGUIUtil.contextControlZone, GUI.skin.box);
        GUILayout.BeginHorizontal();

        GUILayout.Label("Agent Count: " + mNav.crowd.AgentCount);

        if (GUILayout.Button("Clear Agents"))
        {
            Agent[] agents = (Agent[])FindSceneObjectsOfType(typeof(Agent));
            foreach (Agent agent in agents)
            {
                Destroy(agent.gameObject);
            }
            mActiveAgent = null;
        }

        GUILayout.EndHorizontal();
        GUILayout.EndArea();

        SimGUIUtil.OnGUI();
    }
}
