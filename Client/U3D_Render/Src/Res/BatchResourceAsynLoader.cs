using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Effect;

public class ResourceRequestInfo
{
    public string name;
    public ResourceRequest rr;
    public float begintime;
}



public class BatchResourceAsynLoader
{
    List<Object> objs = null;

    public IEnumerator BatchAsynLoad(List<ResourceRequestInfo> requests, System.Type t, int batchnumber=100)
    {
        objs = new List<Object>();
        float totalbegintime = Time.time;
        finish = false;
   
        Queue<ResourceRequestInfo> unprocessqueue = new Queue<ResourceRequestInfo>();
        for (int k = 0; k < requests.Count; k++)
        {
            ResourceRequestInfo rri = requests[k];
            unprocessqueue.Enqueue(rri);
        }

        Dictionary<ResourceRequestInfo, bool> processingqueue = new Dictionary<ResourceRequestInfo, bool>();
        Queue<ResourceRequestInfo> donequeue = new Queue<ResourceRequestInfo>();
        Queue<ResourceRequestInfo> finishqueue = new Queue<ResourceRequestInfo>();

        while (unprocessqueue.Count > 0 || processingqueue.Count > 0 || finishqueue.Count<requests.Count)
        {
            float curtime = Time.time;

            while (processingqueue.Count < batchnumber && unprocessqueue.Count > 0)
            {
                ResourceRequestInfo rri = unprocessqueue.Dequeue();
                rri.rr = Resources.LoadAsync(rri.name, t);
                rri.begintime = curtime;
                processingqueue[rri] = true;
            }

            donequeue.Clear();
            foreach (KeyValuePair<ResourceRequestInfo,bool>keypair in processingqueue)
            {
                ResourceRequestInfo rri = keypair.Key;
                if (rri.rr.isDone)
                {
                    Object asset = (Object)rri.rr.asset;//Resources.Load<LightingEffect>("Effect/" + assetname);
                    if (asset != null)
                    {
                        objs.Add(asset);
                    }
                    donequeue.Enqueue(rri);
                }
                else if (curtime - rri.begintime > 600.0f)
                {
                    Trace.LogWarning("BatchResourceAsynLoader overtime resouce=" + rri.name);
                    donequeue.Enqueue(rri);
                }
            }

            foreach (ResourceRequestInfo rri in donequeue)
            {
                processingqueue.Remove(rri);
                finishqueue.Enqueue(rri);
            }
            donequeue.Clear();

            //Debug.Log("unprocessqueue=" + unprocessqueue.Count.ToString() + ",processingqueue=" + processingqueue.Count.ToString() + ",finishqueue=" + finishqueue.Count.ToString() + "/" + requests.Count.ToString());
            yield return new WaitForEndOfFrame();
        }

        float totalfinishtime = Time.time;
        float totaldeltatime = totalfinishtime - totalbegintime;
        Trace.Log("BatchAsynLoad finish, cost time=" + totaldeltatime.ToString() + ",finishnumber=" + finishqueue.Count.ToString());

        finish = true;
    }


    public T[] GetResult<T>() where T : Object
    {
        T[] array = new T[objs.Count];
        int i = 0;
        foreach (Object obj in objs)
        {
            array[i] = (T)obj;
            i++;
        }
        return array;
    }

    public bool finish = false;
}