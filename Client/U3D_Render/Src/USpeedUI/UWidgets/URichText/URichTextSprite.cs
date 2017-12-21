using UnityEngine;
using System.Collections;
using UnityEngine.UI;
using System.Collections.Generic;
using System.Linq;

public class URichTextSprite : MaskableGraphic
{
    //以Sprite在字符串位置作为key
    public Dictionary<Vector3, URichTextSpriteMeshInfo> dicSpriteMeshInfos = new Dictionary<Vector3, URichTextSpriteMeshInfo>();
    /// <summary>
    /// 记录anim sprite在字符串的位置
    /// </summary>
    public List<Vector3> animSpriteMeshInfoIndexs = new List<Vector3>();  
    
    private CanvasRenderer _renderer = null;

    public float frameFps = 4f;

    public CanvasRenderer Renderer
    {
        get
        {
            _renderer = GetComponent<CanvasRenderer>();
            if (_renderer == null)
                _renderer = this.gameObject.AddComponent<CanvasRenderer>();
            return _renderer;
        }
    }

    public bool isInit = false;

    public bool IsInit
    {
        get {
            return isInit;
        }
    }

    public void Init(URichText text)
    {
        if (isInit)
            return;
        
        dicSpriteMeshInfos.Clear();
        animSpriteMeshInfoIndexs.Clear();
        isInit = true;
    }
    float time = 0.0f;
    public void Update()
    {
        if (!isInit)
        {
            return;
        }

        time += Time.deltaTime;
        if(time > 1/ frameFps)
        {
            time = 0;
            if (animSpriteMeshInfoIndexs.Count > 0)
            {
                foreach(var animKey in animSpriteMeshInfoIndexs)
                {
                    if(dicSpriteMeshInfos.ContainsKey(animKey))
                    {
                        URichTextSpriteMeshInfo meshinfo = dicSpriteMeshInfos[animKey];
                        if (meshinfo != null)
                        {
                            int curIndex = meshinfo.spriteIndex;
                            curIndex++;
                            curIndex = curIndex % meshinfo.animLen;
                            meshinfo.spriteIndex = curIndex;
                            RecaculateAnimSpriteMesh(meshinfo);
                        }
                    }
                }
                DrawAnimatorSprite();
            }
        }
    }

    private Texture m_mainTexture;
    public override Texture mainTexture
    {
        get
        {
            if (m_mainTexture == null)
                return s_WhiteTexture;
            return m_mainTexture;
        }
    }
#region //Override

    protected override void OnEnable()
    {
        //不调用父类的OnEnable 他默认会渲染整张图片
        //base.OnEnable();  
    }

#if UNITY_EDITOR
    //在编辑器下 
    protected override void OnValidate()
    {
        base.OnValidate();
        //Debug.Log("Texture ID is " + this.texture.GetInstanceID());
    }
#endif

    protected override void OnRectTransformDimensionsChange()
    {
        // base.OnRectTransformDimensionsChange();
    }

    #endregion

    #region //接口

    public void UpdateSpriteMeshInfo(URichTextSpriteMeshInfo info)
    {
        info.isDirty = true;
        //处于字符串位置一致时确认下，内容也是否一致
        if(dicSpriteMeshInfos.ContainsKey(info.vertices[0]))
        {
            //if (dicSpriteMeshInfos[info.place].animKey != info.animKey)
            {
                dicSpriteMeshInfos[info.vertices[0]] = info;
            }                
        }
        else
        {
            dicSpriteMeshInfos.Add(info.vertices[0], info);
        }

        m_mainTexture = USpriteManager.Instance.GetAnimSprite(info.animKey).texture;
    }

    /// <summary>
    /// 重新绘制整张Sprite
    /// </summary>
    /// <param name="isUpdateMesh"></param>
    public void DrawSprite(bool resetDirty = true)
    {
        if (!isInit)
            return;
        UpdateSpriteMesh();
        this.UpdateMaterialAndMainTexture();

        if (!resetDirty)
            return;

        //先清空不需要重绘的sprite
        Vector3[] tmpKeysNeedRemove = new Vector3[dicSpriteMeshInfos.Keys.Count];
        dicSpriteMeshInfos.Keys.CopyTo(tmpKeysNeedRemove, 0);
        for(int i = tmpKeysNeedRemove.Length - 1; i>=0;i--)
        {
            if(dicSpriteMeshInfos[tmpKeysNeedRemove[i]].isDirty)
            {
                dicSpriteMeshInfos[tmpKeysNeedRemove[i]].isDirty = false;
            }
            else
            {
                dicSpriteMeshInfos.Remove(tmpKeysNeedRemove[i]);
            }
        }
        //再看看哪些是Animation Sprite

        animSpriteMeshInfoIndexs.Clear();
        foreach (var info in dicSpriteMeshInfos)
        {
            if(!string.IsNullOrEmpty(info.Value.animKey) && info.Value.animLen > 1)
            {
                animSpriteMeshInfoIndexs.Add(info.Key);
            }
        }
    }
    /// <summary>
    /// 绘制后 需要更新材质(会调用canvasRenderer.SetTexture(mainTexture))
    /// </summary>
    public void UpdateMaterialAndMainTexture()
    {
        base.UpdateMaterial();
    }
    #endregion

    Mesh m_spriteMesh = null;
    public void DrawAnimatorSprite()
    {
        if (!isInit || !m_spriteMesh)
            return;

        List<Vector2> tempUvs = new List<Vector2>();

        foreach(var info in dicSpriteMeshInfos.Values)
        {
            tempUvs.AddRange(info.uvs);
        }

        m_spriteMesh.uv = tempUvs.ToArray();

        if (m_spriteMesh == null)
            return;

        Renderer.SetMesh(m_spriteMesh);
        this.UpdateMaterialAndMainTexture();
    }

    //动画表情：重新计算uv
    private bool RecaculateAnimSpriteMesh(URichTextSpriteMeshInfo meshInfo)
    {
        int spriteID = 0;
        if (mainTexture == null)
            return false;

        Data.AnimSpriteConfig.SSchemeAnimFrameData frameData = Data.AnimSpriteConfig.AnimSpriteConfig.Instance.GetData(meshInfo.animKey);

        Rect newSpriteRect = frameData.GetFrameRect(meshInfo.spriteIndex);//sInfo.rect;

        //由于位置不变，改变uv即可
        //由于uv原点在左下角，但一般算sprite 坐标时按左上角，所以翻转一下
        Vector2 newTexSize = new Vector2(m_mainTexture.width, m_mainTexture.height);
        meshInfo.uvs[3] = new Vector2(newSpriteRect.x / newTexSize.x, 1 - (newSpriteRect.y + newSpriteRect.height) / newTexSize.y);
                        //new Vector2(newSpriteRect.x / newTexSize.x, newSpriteRect.y / newTexSize.y);
        meshInfo.uvs[1] = new Vector2((newSpriteRect.x + newSpriteRect.width) / newTexSize.x, 1 - newSpriteRect.y / newTexSize.y);
                        //new Vector2((newSpriteRect.x + newSpriteRect.width) / newTexSize.x, (newSpriteRect.y + newSpriteRect.height) / newTexSize.y);
        meshInfo.uvs[2] = new Vector2((newSpriteRect.x + newSpriteRect.width) / newTexSize.x, 1 - (newSpriteRect.y + newSpriteRect.height) / newTexSize.y);
                         //new Vector2((newSpriteRect.x + newSpriteRect.width) / newTexSize.x, newSpriteRect.y / newTexSize.y);
        meshInfo.uvs[0] = new Vector2(newSpriteRect.x / newTexSize.x, 1 - newSpriteRect.y / newTexSize.y); 
                        //new Vector2(newSpriteRect.x / newTexSize.x, (newSpriteRect.y + newSpriteRect.height) / newTexSize.y);
        return true;
    }
    
    //刷新mesh信息：顶点位置，uv
    private void UpdateSpriteMesh()
    {
        if (!isInit)
            return;
        m_spriteMesh = new Mesh();
        m_spriteMesh.name = "URichTextSprite";
        List<Vector3> tempVertices = new List<Vector3>();
        List<Vector2> tempUvs = new List<Vector2>();
        List<int> tempTriangles = new List<int>();

        foreach(var info in dicSpriteMeshInfos.Values)
        {
            //有改变的才需要添加到新的mesh
            if (info.isDirty)
            {
                tempVertices.AddRange(info.vertices);
                tempUvs.AddRange(info.uvs);
                tempTriangles.AddRange(info.triangles);
            }
        }

        //计算顶点绘制顺序
        for (int i = 0; i < tempTriangles.Count; i++)
        {
            if (i % 6 == 0)
            {
                int num = i / 6;
                tempTriangles[i + 0] = 0 + 4 * num;
                tempTriangles[i + 1] = 1 + 4 * num;
                tempTriangles[i + 2] = 2 + 4 * num;

                tempTriangles[i + 3] = 0 + 4 * num;
                tempTriangles[i + 4] = 2 + 4 * num;
                tempTriangles[i + 5] = 3 + 4 * num;
            }
        }

        m_spriteMesh.vertices = tempVertices.ToArray();
        m_spriteMesh.uv = tempUvs.ToArray();
        m_spriteMesh.triangles = tempTriangles.ToArray();
        if (m_spriteMesh == null)
            return;

        Renderer.SetMesh(m_spriteMesh);

    }

}
