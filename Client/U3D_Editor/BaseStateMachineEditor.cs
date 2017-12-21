using UnityEditor;
using UnityEngine;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System;
using UnityEditorInternal;

[InitializeOnLoad]
public class BaseStateMachineEditor
{
    static BaseStateMachineEditor()
    {
        BaseStateMachine.BuildStateMapFun -= BaseStateMachine.BuildStateMapImp;
        BaseStateMachine.BuildStateMapFun += BaseStateMachineEditor.BuildStateMapImp;
    }

    private static void EnumStatesImp(Animator animator, ref Hashtable statedic, UnityEditor.Animations.AnimatorStateMachine sm)
    {
        int stateCount = sm.states.Length;

        for (int i = 0; i < stateCount; i++)
        {

            UnityEditor.Animations.AnimatorState state = sm.states[i].state;
            if (state != null)
            {
                string statename = state.name;
                if (statename == null)
                {
                    continue;
                }

                statedic[statename] = state;

            }
        }

        int stateMachineCount = sm.stateMachines.Length;

        for (int i = 0; i < stateMachineCount; i++)
        {
            UnityEditor.Animations.AnimatorStateMachine childsm = sm.stateMachines[i].stateMachine;
            if (childsm != null)
            {
                EnumStatesImp(animator, ref statedic, childsm);
            }
        }
    }

    private static bool BuildStateMapImp(Animator animator, ref Hashtable statedic)
    {
        if (animator == null)
        {
            return false;
        }

        UnityEditor.Animations.AnimatorController ac = (UnityEditor.Animations.AnimatorController)animator.runtimeAnimatorController;
        if (ac == null)
        {
            return false;
        }

        int layerCount = ac.layers.Length;

        for (int k = 0; k < layerCount; k++)
        {
            UnityEditor.Animations.AnimatorControllerLayer layer = ac.layers[k];
            if (layer == null)
            {
                continue;
            }

            UnityEditor.Animations.AnimatorStateMachine sm = layer.stateMachine;

            EnumStatesImp(animator, ref statedic, sm);

        }

        return true;
    }


}


