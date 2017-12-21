
using UnityEditor;
using System.Collections.Generic;
using Effect.TrailsSystem;

[InitializeOnLoad]
public class ExecutionOrderSetter
{
    static ExecutionOrderSetter()
    {
        //List<string> scriptlist = new List<string>
        //{   
        //    typeof (Effect_TrailRenderer_Base).Name,
        //    typeof (Effect_Trail).Name,
        //    typeof (Effect_SmoothTrail).Name,
        //    typeof (Effect_SmokeTrail).Name,
        //    typeof (Effect_SmokePlume).Name,
        //    //typeof (Effect_RandomSmoothTrail).Name
        //};

        //foreach (MonoScript monoScript in MonoImporter.GetAllRuntimeMonoScripts())
        //{
        //    if (!scriptlist.Contains(monoScript.name)) continue;

        //    if (MonoImporter.GetExecutionOrder(monoScript) != 1000)
        //        MonoImporter.SetExecutionOrder(monoScript, 1000);
        //}

        //string InputManagerName = typeof(InputManager).Name;
        //foreach (MonoScript monoScript in MonoImporter.GetAllRuntimeMonoScripts())
        //{
        //    if (!InputManagerName.Contains(monoScript.name)) continue;

        //    if (MonoImporter.GetExecutionOrder(monoScript) != -500)
        //    {
        //        MonoImporter.SetExecutionOrder(monoScript, -500);
        //    }

        //}

        //string Effect_BillBoardName = typeof(Effect.Effect_BillBoard).Name;
        //foreach (MonoScript monoScript in MonoImporter.GetAllRuntimeMonoScripts())
        //{
        //    if (!Effect_BillBoardName.Contains(monoScript.name)) continue;

        //    if (MonoImporter.GetExecutionOrder(monoScript) != 1000)
        //    {
        //        MonoImporter.SetExecutionOrder(monoScript, 1000);
        //    }

        //}
    }
}

