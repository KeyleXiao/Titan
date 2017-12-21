using UnityEngine;
using System.Collections;
using System.Collections.Generic;



// 动作工厂 
// 全局单一类，整个游戏只有一个实例
// 负责载入复杂动作配置

public class AnimationEffectFactory : MonoBehaviour
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public static AnimationEffectFactory Instance = null;

    private Dictionary<int, AnimationEffect> mAnimEffectTemplateList = new Dictionary<int, AnimationEffect>();


    public Object[] objs1 = null;
    public Object[] objs2 = null;
    public AnimationEffect[] effect_list = null;


    public void Awake()
    {
        Instance = this;
        StartCoroutine(LoadDataEnumerator());

    }

    public void OnDestroy()
    {
        StopCoroutine(LoadDataEnumerator());
        if (objs1 != null)
        {
            foreach (Object obj in objs1)
            {
                //AssetSystem.UnloadAsset(obj);
            }
        }
        objs1 = null;
        if (objs2 != null)
        {
            foreach (Object obj in objs2)
            {
                //AssetSystem.UnloadAsset(obj);
            }
        }
        objs2 = null;
        effect_list = null;
        UnLoadAnimEffectData();
        Resources.UnloadUnusedAssets();
    }

    public AnimationEffect CreateAnimation(int nID)
    {
        if (!mAnimEffectTemplateList.ContainsKey(nID))
        {
            Trace.Warning("AnimationEffect creates failed. Id " + nID.ToString() + "has not loaded.");
            return null;
        }

        AnimationEffect newEffect = GameObject.Instantiate(mAnimEffectTemplateList[nID]);
        return newEffect;
    }



    private void LoadByEditorImp(string effectname)
    {
        AnimationEffect effect = Resources.Load<AnimationEffect>("Animation/" + effectname);
        if (effect != null)
        {
            mAnimEffectTemplateList[effect.getID()] = effect;
        }
    }

    public static void LoadByEditor(string effectname)
    {
        if (!Application.isPlaying)
        {
            return;
        }
        if (Instance != null)
        {
            Instance.LoadByEditorImp(effectname);
        }
    }

    private bool skip = true;

    private IEnumerator LoadDataEnumerator()
    {
        //这里要跳两帧才能让登录界面先刷出来同时让用户无法感觉到后台在悄悄加载
        if (skip)
        {
            yield return new WaitForEndOfFrame();
            yield return new WaitForEndOfFrame();
        }

        mAnimEffectTemplateList.Clear();

        if (Application.isEditor) //编辑器需要逐个加载，因此分帧处理
        {
            List<ResourceRequestInfo> requests = new List<ResourceRequestInfo>();
            string[] assetfiles = System.IO.Directory.GetFiles(Application.dataPath + "/Resources/Animation", "*.asset");
            foreach (string assetfile in assetfiles)
            {
                string assetname = System.IO.Path.GetFileNameWithoutExtension(assetfile);
                ResourceRequestInfo rri = new ResourceRequestInfo();
                rri.name = "Animation/" + assetname;
                requests.Add(rri);
            }

            BatchResourceAsynLoader brl = new BatchResourceAsynLoader();
            StartCoroutine(brl.BatchAsynLoad(requests, typeof(AnimationEffect)));
            while (!brl.finish)
            {
                yield return new WaitForEndOfFrame();
            }
            effect_list = brl.GetResult<AnimationEffect>();

        }
        else //导出的已经打包到Resource包，一并读取并不慢，因此不分帧
        {
            effect_list = Resources.LoadAll<AnimationEffect>("Animation");
        }

        if (skip)
        {
            yield return new WaitForEndOfFrame();
        }

        foreach (AnimationEffect effect in effect_list)
        {
            if (mAnimEffectTemplateList.ContainsKey(effect.getID()))
            {
                Trace.Warning("the AnimEffect id is already exist!" + effect.getID().ToString());
                AnimationEffect conflitEffect = mAnimEffectTemplateList[effect.getID()];
                Trace.Warning("Name1:" + conflitEffect.name + "       Name2:" + effect.name);
                continue;
            }

            mAnimEffectTemplateList[effect.getID()] = effect;
        }

        Trace.Log("Loaded " + mAnimEffectTemplateList.Count.ToString() + " AnimationEffect(s)!");

        yield return true;
    }

    string[] SearchFiles(string folder)
    {
        List<string> paths = new List<string>();
        string[] selffiles = System.IO.Directory.GetFiles(folder, "*.controller");
        foreach (string selffile in selffiles)
        {
            paths.Add(selffile);
        }

        string[] subfolders = System.IO.Directory.GetDirectories(folder);
        foreach (string subfolder in subfolders)
        {
            string[] subfiles = SearchFiles(subfolder);
            foreach (string subfile in subfiles)
            {
                paths.Add(subfile);
            }
        }

        return paths.ToArray();
    }


    private IEnumerator LoadAnimClipEnumerator()
    {
        yield return new WaitForSeconds(0.1f);

        //objs1=Resources.LoadAll("Skin");

        if (Application.isEditor) //编辑器需要逐个加载，因此分帧处理
        {
            List<ResourceRequestInfo> requests = new List<ResourceRequestInfo>();
            string conpath = Application.dataPath + "/Resources/Controller";
            string[] assetfiles = SearchFiles(conpath);
            foreach (string assetfile in assetfiles)
            {
                int t1 = System.Environment.TickCount;
                string assetname = assetfile.Substring(conpath.Length + 1);
                assetname = assetname.Substring(0, assetname.Length - 11);
                assetname = assetname.Replace("\\", "/");

                ResourceRequestInfo rri = new ResourceRequestInfo();
                rri.name = "Controller/" + assetname;
                requests.Add(rri);
            }

            BatchResourceAsynLoader brl = new BatchResourceAsynLoader();
            StartCoroutine(brl.BatchAsynLoad(requests, typeof(Object), 50));
            while (!brl.finish)
            {
                yield return new WaitForEndOfFrame();
            }
            objs2 = brl.GetResult<Object>();

        }
        else//导出的已经打包到Resource包，一并读取并不慢，因此不分帧
        {
            objs2 = Resources.LoadAll("Controller");
        }

        Trace.Log("Loaded " + objs2.Length.ToString() + " Controller(s)!");



        ////这里预载一定要保证顺序，先Controller，再Skin，否则Skin加载时依赖Controller会重复搞
        //while (true)
        //{
        //    int t1 = System.Environment.TickCount;
        //    bool ret=SkinManager.PreLoad();
        //    if (ret == false)
        //    {
        //        break;
        //    }
        //    int t2 = System.Environment.TickCount;
        //    float dt = (t2 - t1) * 0.001f;
        //    if (dt < 0.01f) dt = 0.01f;
        //    yield return new WaitForSeconds(dt);//工作一天(dt)，休息1天(dt)
        //}

        yield return true;
    }

    public void UnLoadAnimEffectData()
    {
        foreach (KeyValuePair<int, AnimationEffect> pair in mAnimEffectTemplateList)
        {
            AssetSystem.UnloadAsset(pair.Value);
        }
        mAnimEffectTemplateList.Clear();
    }
}
