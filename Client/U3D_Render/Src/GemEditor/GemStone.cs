using UnityEngine;
using System.Collections.Generic;

public class GemStone : MonoBehaviour {
    private bool isInit = true;
    private Material _material = null;
	public Material Material
    {
        get { return _material; }
        set
        {
            _material = value;
            this.GetComponent<MeshRenderer>().material = _material;
        }
    }
    public int level;               //1~5级
    public GemDecalType decalType;   //符文纹理类型
    public GemMeshType meshType;    //宝石模型类型（影响shader类型）

    //环境贴图
    public Cubemap evnCubeMap
    {
        get { return (Cubemap)this.Material.GetTexture("_EnvCubeMap"); }
        set { this.Material.SetTexture("_EnvCubeMap",value); }
    }

    public Color GemColor
    {
        get { return this.Material.GetColor("_3SColor"); }
        set { this.Material.SetColor("_3SColor", value); }
    }

    public Color DecalColor
    {
        get { return this.Material.GetColor("_DecalColor"); }
        set { this.Material.SetColor("_DecalColor", value); }
    }

    //高光颜色
    public Color SpecColor
    {
        get { return this.Material.GetColor("_SpecColor"); }
        set { this.Material.SetColor("_SpecColor", value); }
    }

    //rgb：cubemap的颜色控制
    public Color ReflectColour
    {
        get { return this.Material.GetColor("_ReflectColour"); }
        set { this.Material.SetColor("_ReflectColour", value); }
    }

    //******贴图****//
    public Texture2D MainTex
    {
        get { return (Texture2D)this.Material.GetTexture("_MainTex"); }
        set { this.Material.SetTexture("_MainTex", value); }
    }

    public Texture2D BumpMap
    {
        get { return (Texture2D)this.Material.GetTexture("_BumpMap"); }
        set { this.Material.SetTexture("_BumpMap", value); }
    }

    public Texture2D DecalTex
    {
        get { return (Texture2D)this.Material.GetTexture("_DecalTex"); }
        set { this.Material.SetTexture("_DecalTex", value); }
    }

    public int materialID;

    void OnEnable()
    {
        //switch(meshType)    //shader主要跟模型挂钩
        //{
        //    case GemMeshType.Sphere:
        //        materialID = 0;
        //        break;
        //    case GemMeshType.Oval:
        //        materialID = 0;
        //        break;
        //    case GemMeshType.LowSquareDiamon:
        //        materialID = 1;
        //        break;
        //    case GemMeshType.LowSphereDiamon:
        //        materialID = 1;
        //        break;
        //    case GemMeshType.HighSquareDiamon:
        //        materialID = 1;
        //        break;
        //    case GemMeshType.HighSphereDiamon:
        //        materialID = 1;
        //        break;
        //}
    }
}