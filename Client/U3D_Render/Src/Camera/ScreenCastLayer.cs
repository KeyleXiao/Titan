using UnityEngine;
using System.Collections;

public class ScreenCastLayer : MonoBehaviour {

    public GameObject CastlayerObject;

    /// <summary>
    /// 临时加个设置英雄的选中框层
    /// </summary>
    private bool isHero = false;

    public void ResetAllMembers()
    {
        isHero = false;
        CastlayerObject = null;
    }

	// Use this for initialization
	public void Start () {
        if (CastlayerObject)
        {
            Collider[] cols = CastlayerObject.GetComponents<Collider>();
            foreach (Collider col in cols)
            {
                //目标检测必须是Trigger!
                if (col)
                {
                    col.isTrigger = true;
                }
            }

            if (isHero)
                CastlayerObject.GetComponent<Collider>().enabled = false;
            //else if (m_layerName!="")
            //    CastlayerObject.layer = LayerMask.NameToLayer(m_layerName);

            CastlayerObject.name = "ScreenCastLayer";
        }

	}

    public void setHero(bool isHero)
    {
        if (isHero )//&& CastlayerObject!=null)
        {
            //CastlayerObject.layer = LayerMask.NameToLayer(Config.LayerHero);
            this.isHero = true;
        }
    }


    //string m_layerName = "";
    //public void setLayer(string layer)
    //{
    //    if (CastlayerObject)
    //    {
    //        CastlayerObject.layer = LayerMask.NameToLayer(layer);
    //    }
    //    m_layerName = layer;

    //}
}
