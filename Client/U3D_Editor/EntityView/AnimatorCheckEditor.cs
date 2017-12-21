using System;
using UnityEngine;
using UnityEditor;
using System.Collections;
using System.Collections.Generic;
using System.IO; 

[CustomEditor(typeof(AnimatorCheck))]
public class AnimatorCheckEditor : Editor
{
    protected AnimatorCheck Instance;
    public UnityEditor.Animations.AnimatorController animator = null;
    public AnimationClip emptyClip = null;
    public bool fillSpeedParam = false;
    public int attackLayerIndex = 1;

    public void OnEnable()
    {
        Instance = target as AnimatorCheck;
    }

    #region GUI
    public override void OnInspectorGUI()
    {
        animator = (UnityEditor.Animations.AnimatorController)EditorGUILayout.ObjectField("Controller:", animator, typeof(UnityEditor.Animations.AnimatorController), true);
        emptyClip = (AnimationClip)EditorGUILayout.ObjectField("emptyClip:", emptyClip, typeof(AnimationClip), true);
        attackLayerIndex = EditorGUILayout.IntField("攻击层的位置（最上层为0）:", attackLayerIndex);
        fillSpeedParam = EditorGUILayout.Toggle("为状态机自动填充速度参数：", fillSpeedParam);
        if (GUILayout.Button("emptyClip填充，程序部专用！"))
        {
            if (emptyClip == null)
            {
                Debug.LogError("emptyClip 为空！");
            }
            else if (animator == null)
            {
                Debug.LogError("Controller 为空！");
            }
            else
            {
                FillWithEmptyClip();
            }
        }

        if (GUILayout.Button("导出Controller统计"))
        {
            //foreach (var clip in animator.animationClips)
            //{
            //    Debug.Log("Clip " + clip.name);
            //}

            if (animator == null)
            {
                Debug.LogError("Controller 为空！");
            }
            else
            {
                OutputControllInfo();
            }
        }
    }
    #endregion

    #region emptyClip填充
    private void FillWithEmptyClip()
    {
        foreach (UnityEditor.Animations.AnimatorControllerLayer lay in animator.layers)
        {
            Debug.Log("Layer "+lay.name);
            FillAnimatorState(0, lay.stateMachine);
        }
        EditorUtility.SetDirty(animator);
        AssetDatabase.SaveAssets();
        AssetDatabase.Refresh();
    }

    private void FillAnimatorState(int depth, UnityEditor.Animations.AnimatorStateMachine stateMachine)
    {
        foreach (var sta in stateMachine.stateMachines)
        {
            Debug.Log("Depth=" + depth + " stateMachines " + sta.stateMachine);
            FillAnimatorState(depth + 1, sta.stateMachine);
        }

        foreach (UnityEditor.Animations.ChildAnimatorState sta in stateMachine.states)
        {
            UnityEditor.Animations.BlendTree blendTree=sta.state.motion as UnityEditor.Animations.BlendTree;
            if (blendTree != null)
            {
                Debug.LogError("Depth=" + depth + " blendTree need to be refined" + sta.state.motion);
                int mCount = blendTree.children.Length;

                //foreach (var cmotion in blendTree.children)
                for (int i = 0; i < mCount;i++)
                {
                    Debug.Log("Blend Clip " + blendTree.children[i].motion);
                    Vector2 position = blendTree.children[i].position;
                    blendTree.AddChild(emptyClip, position);
                }
                for (int i = 0; i < mCount; i++)
                {
                    blendTree.RemoveChild(0);                    
                }
                sta.state.motion = blendTree;
            }
            else
            {
                Debug.Log("Depth=" + depth + " Clip " + sta.state.motion);
                sta.state.motion = emptyClip;
            }
        }
    }

    #endregion


    #region 导出Controll统计
    private void OutputControllInfo()
    {
        initTagInfo();
        tagCount = new Dictionary<string, List<string>>();
        prefClipName = "";
        int layerIndex=0;
        foreach (UnityEditor.Animations.AnimatorControllerLayer lay in animator.layers)
        {
            //Debug.Log("Layer " + lay.name);
            string speedParameter = "";
            if (fillSpeedParam)
            {
                if (layerIndex == 0)
                {
                    speedParameter = "moveSpeed";
                }
                else
                {
                    speedParameter = "attackSpeed";
                }
            }            
            traverseStateInfo(0, lay.name + "层下", lay.stateMachine, speedParameter);
            layerIndex++;
        }

        foreach (UnityEditor.Animations.AnimatorControllerLayer lay in animator.layers)
        {
            //Debug.Log("Layer "+lay.name);
            traverseTransformInfo(0, lay.name+"层下", lay.stateMachine);
        }

        outputClipTagInfo();

        stateDict = new Dictionary<string, UnityEditor.Animations.AnimatorState>();
        traverseFillStateDict(0, "", animator.layers[attackLayerIndex].stateMachine);
        traverseCheckStateDict(0, "", animator.layers[attackLayerIndex+1].stateMachine);
        finalCheckAttackLayer();
    }


    private void traverseTransformInfo(int depth, string path, UnityEditor.Animations.AnimatorStateMachine stateMachine)
    {
        foreach (var sta in stateMachine.stateMachines)
        {
            //Debug.Log("Depth=" + depth + " stateMachines " + sta.stateMachine);
            string newPath = path;
            newPath = newPath + sta.stateMachine.name + "->";
            traverseTransformInfo(depth + 1, newPath, sta.stateMachine);
        }

        foreach (UnityEditor.Animations.ChildAnimatorState sta in stateMachine.states)
        {
            //Debug.Log(sta.state.name + " Tag " + sta.state.tag);
            foreach (var transform in sta.state.transitions)
            {
                if (transform != null)
                {
                    if (transform.destinationState != null)
                    {
                        //Debug.Log(path+sta.state.name + " Transform To " + transform.destinationState.name);
                    }
                    else
                    {
                        Debug.LogError(path+sta.state.name + "有一个动作过渡，他的终点没有指向明确的动作（有可能只是指向了一个子状态机，这比较危险），请检查");
                        continue;
                    }
                }
                else
                {
                    Debug.LogError(path+sta.state.name + "存在一个空的动作过渡，请检查");
                    continue;
                }

                if (transform.hasExitTime)
                {
                    if (transform.duration + transform.exitTime > 1.0f)
                    {
                        Debug.LogError(path + sta.state.name + "至" + transform.destinationState.name + "的过渡，因为duration+exitTime>1.0，可能会导致源动作会播放两遍，请检查");
                    }
                    if (transform.duration > 1.0f)
                    {
                        Debug.LogError(path + sta.state.name + "至" + transform.destinationState.name + "的过渡，因为duration>1.0，可能会导致目标动作会播放两遍，请检查");
                    }

                    if (transform.conditions!=null && transform.conditions.Length > 0)
                    {
                        Debug.LogError(path + sta.state.name + "至" + transform.destinationState.name + "的过渡，同时有ExitTime和过渡条件，表示满足退出条件且播放时间足够了才会触发，请检查");
                    }
                }
                else if (transform.conditions == null || transform.conditions.Length == 0)
                {
                    Debug.LogError(path + sta.state.name + "至" + transform.destinationState.name + "的过渡，即没有ExitTime也没有过渡条件，表示他永远不会被触发，请检查");
                }

                if (sta.state.tag == "AttackMove" || sta.state.tag == "AttackMove1")
                {
                    if ((transform.hasExitTime && transform.exitTime < 0.99f) || transform.duration > 0.01f)
                    {
                        Debug.LogError(path + sta.state.name + "至" + transform.destinationState.name + "的过渡配置了AttackMove，必须与下一个动作生切（ExitTime =1.0, Duration > 0.0f），不然可能会被拉回，请检查");
                    }
                }
                
            }
        }
    }

    private string prefClipName = "";
    private bool bCheckClipName(string checkName)
    {
        int index = checkName.IndexOf("@");
        if (index > 0)
        {
            string prefName = checkName.Substring(0, index);
            if (prefClipName == "")
            {
                prefClipName = prefName;
            }
            else if (prefClipName != prefName)
            {
                return false;
            }

        }
        return true;
    }

    struct TagDesc
    {
        public string tagName;
        public string tagDesc;

        public TagDesc(string name, string desc)
        {
            tagName = name;
            tagDesc = desc;
        }
    }

    private List<TagDesc> tagDescs;
    private void initTagInfo()
    {
        tagDescs = new List<TagDesc>();        
        tagDescs.Add(new TagDesc("AttackUp","上下半身分离"));
        tagDescs.Add(new TagDesc("AttackUp1", "特殊上下半身分离，腰部直接生切，不做过渡"));
        tagDescs.Add(new TagDesc("AttackMove", "边走边打，注意位移应该放在Main骨骼上，当与目标物体距离小于creaturePropety.attackMoveRange则停止移动"));
        tagDescs.Add(new TagDesc("ForceAttackMove", "强制边走边打，忽略creaturePropety.attackMoveRange的距离限制"));
        tagDescs.Add(new TagDesc("BlendAttack", "区分方向的边走边打，根据给出的冲锋目标点，匹配BlendTree不同的方向，做不同方向的动作"));
        tagDescs.Add(new TagDesc("MovingUp", "此动作做到一半时移动英雄，会打断它"));
        tagDescs.Add(new TagDesc("IDLE", "攻击受击动作层，不做任何动作的状态标签"));
        tagDescs.Add(new TagDesc("Dead", "死亡动作标签，这个动作会根据已经死亡的时间从中间开始播放，不然进入视野的英雄每次都从头开始播放死亡动画的话，玩家会以为死亡时间有问题"));
        tagDescs.Add(new TagDesc("???", "未约定实际作用的标识"));
    }

    private Dictionary<string, List<string>> tagCount;
    private void outputClipTagInfo()
    {
        StreamWriter sw;
        string filePath=AssetDatabase.GetAssetPath(animator);
        if (filePath.LastIndexOf('.') > 0)
        {
            filePath = filePath.Substring(0, filePath.LastIndexOf('.'));
        }
        sw = File.CreateText(filePath + "TagInfo.txt");

        sw.WriteLine("标签分类统计");
        foreach (KeyValuePair<string, List<string>> kv in tagCount)
        {
            TagDesc desc=tagDescs[tagDescs.Count-1];
            bool found = false;
            foreach (TagDesc tagdesc in tagDescs)
            {
                if (kv.Key==tagdesc.tagName)
                {
                    desc=tagdesc;
                    found = true;
                    break;
                }
            }

            if (found)
            {
                sw.WriteLine("标签：" + kv.Key + "；作用：" + desc.tagDesc);
                List<string> allPath = kv.Value;
                foreach (string path in allPath)
                {
                    sw.WriteLine("   " + path);
                }
            }
        }

        if (!tagCount.ContainsKey("Dead") || tagCount["Dead"].Count<3)
        {
            Debug.LogError("为了达到死亡动作不从头播放的效果，理论上移动层、攻击层和上半身攻击层都要有Tag为Dead的死亡动作，您是不想要这个效果？还是标漏了？");
        }

        sw.Close();
        sw.Dispose();//文件流释放 

    }

    private void traverseStateInfo(int depth, string path, UnityEditor.Animations.AnimatorStateMachine stateMachine, string speedParameter)
    {
        foreach (var sta in stateMachine.stateMachines)
        {
            //Debug.Log("Depth=" + depth + " stateMachines " + sta.stateMachine);
            string newPath = path;
            newPath = newPath + sta.stateMachine.name + "->";
            traverseStateInfo(depth + 1, newPath, sta.stateMachine, speedParameter);
        }

        foreach (UnityEditor.Animations.ChildAnimatorState sta in stateMachine.states)
        {
            string tag=sta.state.tag;            
            
            if (sta.state.speedParameter != speedParameter)
            {
                sta.state.speedParameter = speedParameter;

                if (speedParameter != "")
                {
                    sta.state.speedParameterActive = true;
                }
            }

            if (tag!="")
            {
                if (!tagCount.ContainsKey(tag))
                {
                    tagCount[tag] = new List<string>();
                    
                }
                tagCount[tag].Add(path + sta.state.name);
            }

            UnityEditor.Animations.BlendTree blendTree = sta.state.motion as UnityEditor.Animations.BlendTree;
            if (blendTree != null)
            {
                int mCount = blendTree.children.Length;
                foreach (var cmotion in blendTree.children)
                //for (int i = 0; i < mCount; i++)
                {
                    //Debug.Log("Blend Clip " + blendTree.children[i].motion);
                    //if (blendTree.children[i].motion!=null && !bCheckClipName(blendTree.children[i].motion.name))
                    if (cmotion.motion != null && !bCheckClipName(cmotion.motion.name))
                    {
                        Debug.LogError(path + "下" + blendTree.name + "的动作" + cmotion.motion.name + "的前缀名和其他动作不同，是不是把其他人物的动作给拖进来了？");
                    }
                }
            }
            else
            {
                if (sta.state.tag == "BlendAttack")
                {
                    Debug.LogError(path + "下的动作" + sta.state.motion.name + "配置了BlendAttack，它的动作必须是一个BlendTree，请检查");
                }
                if (sta.state.motion != null)
                {
                    if (!bCheckClipName(sta.state.motion.name))
                    {
                        Debug.LogError(path + "下的动作" + sta.state.motion.name + "的前缀名和其他动作不同，是不是把其他人物的动作给拖进来了？");
                    }                
                }
            }
        }
    }


    private Dictionary<string, UnityEditor.Animations.AnimatorState> stateDict;

    private void traverseFillStateDict(int depth, string path, UnityEditor.Animations.AnimatorStateMachine stateMachine)
    {
        foreach (var sta in stateMachine.stateMachines)
        {
            //Debug.Log("Depth=" + depth + " stateMachines " + sta.stateMachine);
            string newPath = path;
            newPath = newPath + sta.stateMachine.name + "->";
            traverseFillStateDict(depth + 1, newPath, sta.stateMachine);
        }

        foreach (UnityEditor.Animations.ChildAnimatorState sta in stateMachine.states)
        {
            if (sta.state != null)
            {
                stateDict[sta.state.name] = sta.state;
            }
        }
    }

    private bool compareTranslation(UnityEditor.Animations.AnimatorStateTransition transA, UnityEditor.Animations.AnimatorStateTransition transB)
    {
        if (transA.destinationState.name != transB.destinationState.name)
        {
            return false;
        }

        if ((transA.hasExitTime && transB.hasExitTime && Mathf.Abs(transA.exitTime - transB.exitTime) < 0.1f)
        || (!transA.hasExitTime && !transB.hasExitTime))
        {
            //go on
        }
        else
        {
            return false;
        }
        
        if (Mathf.Abs(transA.duration - transB.duration) > 0.1f)
        {
            return false;
        }

        if (transA.conditions != null && transB.conditions != null)
        {
            if (transA.conditions.Length != transB.conditions.Length)
            {
                return false;
            }

            foreach (var conA in transA.conditions)
            {
                bool found = false;
                foreach (var conB in transB.conditions)
                {
                    if (conA.mode == conB.mode && conA.parameter == conB.parameter && conA.threshold == conB.threshold)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    return false;
                }
            }

        }
        else if (transA.conditions == null && transB.conditions == null)
        {
            //go on
        }
        else
        {
            return false;
        }

        return true;
    }

    private void traverseCheckStateDict(int depth, string path, UnityEditor.Animations.AnimatorStateMachine stateMachine)
    {
        foreach (var sta in stateMachine.stateMachines)
        {
            //Debug.Log("Depth=" + depth + " stateMachines " + sta.stateMachine);
            string newPath = path;
            newPath = newPath + sta.stateMachine.name + "->";
            traverseCheckStateDict(depth + 1, newPath, sta.stateMachine);
        }

        foreach (UnityEditor.Animations.ChildAnimatorState sta in stateMachine.states)
        {
            if (sta.state != null)
            {
                if (!stateDict.ContainsKey(sta.state.name))
                {
                    Debug.LogError("攻击层和上半身攻击层配置应该是一样的，可是"+path+"动作" + sta.state.name + "只有上半身攻击层有，或者在同层的不同子状态机里有同名，请检查");
                    continue;
                }

                UnityEditor.Animations.AnimatorState attackState = stateDict[sta.state.name];

                if (attackState.transitions.Length != sta.state.transitions.Length)
                {
                    Debug.LogError("攻击层和上半身攻击层配置应该是一样的，可是" + path + "动作" + sta.state.name + "在两层里面的动作切换数目不一致，请检查");
                    continue;
                }

                foreach (var upTransform in sta.state.transitions)
                {
                    bool found = false;
                    foreach (var attackTransform in attackState.transitions)
                    {
                        if (compareTranslation(upTransform, attackTransform))
                        {
                            found = true;
                            break;
                        }
                    }

                    if (!found)
                    {
                        Debug.LogError("攻击层和上半身攻击层配置应该是一样的，可是攻击层" + sta.state.name + "到" + upTransform.destinationState.name + "在上半身攻击层没找到完全匹配的过渡，请检查");
                    }
                }

                stateDict.Remove(sta.state.name);
            }
        }
    }

    private void finalCheckAttackLayer()
    {
        foreach (var kv in stateDict)
        {
            Debug.LogError("攻击层和上半身攻击层配置应该是一样的，可是" + "动作" + kv.Key + "只有攻击层有请检查");
        }

    }

    #endregion
}
