using RichTextPart;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using USpeedUI.UExtensions;

public class URichText : Text, IPointerClickHandler,IPointerEnterHandler,IPointerExitHandler//,IPointerDownHandler
{
    //public SpriteAsset m_SpriteAsset;

    public URichTextSprite textSprite = null;

    public string m_OutputText = "";

    private bool _isHide = false;

    //没图片的时候防止提交顶点
    public bool IsHide
    {
        set
        {
            _isHide = value;
            if (_isHide)
            {
                //m_spriteMesh = new Mesh();
                //List<Vector3> tempVertices = new List<Vector3>();
                //List<Vector2> tempUvs = new List<Vector2>();
                //List<int> tempTriangles = new List<int>();
                //m_spriteMesh.vertices = tempVertices.ToArray();
                //m_spriteMesh.uv = tempUvs.ToArray();
                //m_spriteMesh.triangles = tempTriangles.ToArray();

                //Renderer.SetMesh(m_spriteMesh);
                //this.UpdateMaterialAndMainTexture();
            }
        }
        get { return _isHide; }
    }

    protected readonly List<URichTextURLInfo> m_urlHrefInfo = new List<URichTextURLInfo>();

    public override void SetVerticesDirty()
    {
        base.SetVerticesDirty();
        m_OutputText = text;
    }

    public void Update()
    {
        if(textSprite != null)
        {
            if (_isHide)
            {
                this.textSprite.gameObject.SetActive(false);
                return;
            }
            else
            {
                this.textSprite.gameObject.SetActive(true);
            }
        }

        OnPointerMove();
    }
    readonly UIVertex[] m_TempVerts = new UIVertex[4];
    protected override void OnPopulateMesh(VertexHelper toFill)
    {
        if (font == null)
            return;

        if (this.supportRichText == false)
            this.supportRichText = true;

        m_OutputText = text;

        URichTextUtil.AnalyzeText(ref m_OutputText);

        //解析后没有富文本数据的走Text::OnPopulateMesh
        if (URichTextUtil.hrefInfos.Count <= 0)
        {
            base.OnPopulateMesh(toFill);
            this.IsHide = true;
            return;
        }

        // We don't care if we the font Texture changes while we are doing our Update.
        // The end result of cachedTextGenerator will be valid for this instance.
        // Otherwise we can get issues like Case 619238.
        m_DisableFontTextureRebuiltCallback = true;

        Vector2 extents = rectTransform.rect.size;

        var settings = GetGenerationSettings(extents);

        cachedTextGenerator.Populate(m_OutputText, settings);

        Rect inputRect = rectTransform.rect;

        // get the text alignment anchor point for the text in local space
        Vector2 textAnchorPivot = GetTextAnchorPivot(alignment);
        Vector2 refPoint = Vector2.zero;
        refPoint.x = (textAnchorPivot.x == 1 ? inputRect.xMax : inputRect.xMin);
        refPoint.y = (textAnchorPivot.y == 0 ? inputRect.yMin : inputRect.yMax);

        // Determine fraction of pixel to offset text mesh.
        Vector2 roundingOffset = PixelAdjustPoint(refPoint) - refPoint;

        // Apply the offset to the vertices
        IList<UIVertex> verts = cachedTextGenerator.verts;  
        float unitsPerPixel = 1 / pixelsPerUnit;
        //Last 4 verts are always a new line...
        int vertCount = verts.Count - 4;

        toFill.Clear();

        //多加一个判断
        if (URichTextUtil.hrefInfos.Count <= 0)
        {
            base.OnPopulateMesh(toFill);
            this.IsHide = true;
            return;
        }

        //对Texture进行特殊处理：
        //UGUIText不支持 <quad/>标签，会出现乱码，
        //通过设置uv为0,清除乱码
        if (URichTextUtil.hrefInfos[0].vertStartIndex * 4 < verts.Count)
        {
            for (int i = 0; i < URichTextUtil.hrefInfos.Count; i++)
            {
                if (URichTextUtil.hrefInfos[i].type == RichType.Texture)
                {
                    //乘以4是因为此处是以顶点为单位而不是索引
                    for (int m = URichTextUtil.hrefInfos[i].vertStartIndex * 4; m < URichTextUtil.hrefInfos[i].vertStartIndex * 4 + 4; m++)
                    {
                        UIVertex tempVertex = verts[m];
                        tempVertex.uv0 = Vector2.zero;
                        verts[m] = tempVertex;
                    }
                }
            }
        }

        if (roundingOffset != Vector2.zero)
        {
            for (int i = 0; i < vertCount; ++i)
            {
                int tempVertsIndex = i & 3;
                m_TempVerts[tempVertsIndex] = verts[i];
                m_TempVerts[tempVertsIndex].position *= unitsPerPixel;
                m_TempVerts[tempVertsIndex].position.x += roundingOffset.x;
                m_TempVerts[tempVertsIndex].position.y += roundingOffset.y;
                if (tempVertsIndex == 3)
                    toFill.AddUIVertexQuad(m_TempVerts);
            }
        }
        else
        {
            for (int i = 0; i < vertCount; ++i)
            {
                int tempVertsIndex = i & 3;
                m_TempVerts[tempVertsIndex] = verts[i];
                m_TempVerts[tempVertsIndex].position *= unitsPerPixel;
                if (tempVertsIndex == 3)
                    toFill.AddUIVertexQuad(m_TempVerts);
            }
        }

        //在toFill处理完后才做坐标处理
        URichTextUtil.ClearHandlersIndex();
        m_urlHrefInfo.Clear();

        bool hasTexture = false;
        //对Text进行handler的额外控制（如：生成图片，改变顶点）

        for (int i = 0; i < URichTextUtil.hrefInfos.Count; i++)
        {
            if (URichTextUtil.hrefInfos[i].type == RichType.Texture)
                hasTexture = true;
            URichTextUtil.HandleTextMesh(URichTextUtil.hrefInfos[i], toFill, this);

            if (URichTextUtil.hrefInfos[i].type == RichType.Url && URichTextUtil.hrefInfos[i].urlHrefInfo.IsValid())
            {
                m_urlHrefInfo.Add(URichTextUtil.hrefInfos[i].urlHrefInfo);
            }                
        }
        

        //Debug.Log(text);
        m_DisableFontTextureRebuiltCallback = false;



        //没有图片的话就删掉or关闭textSprite？
        if (this.textSprite && hasTexture)
        {
            this.IsHide = false;
            this.textSprite.DrawSprite(true);
        }
        else if (this.textSprite && !hasTexture)
        {
            this.IsHide = true;
            //this.textSprite.DrawSprite(true);
        }




    }
    //获得TextSprite
    //如果需要图文并排，才创建TextSprite
    public URichTextSprite GetTextSprite()
    {
        //绘制sprite
        if (this.textSprite == null)
        {
            GameObject spriteObj = new GameObject();
            spriteObj.transform.SetParent(transform);
            spriteObj.transform.localPosition = Vector3.zero;
            spriteObj.transform.localScale = Vector3.one;
            spriteObj.transform.name = "textSprite";
            this.textSprite = spriteObj.AddComponent<URichTextSprite>();
        }
        IsHide = false;
        this.textSprite.Init(this);
        return this.textSprite;
    }

    //把动态换行字符串分成一个个Tile
    //获得指定字符串在richText中所占Tile的范围: 
    //rect.xy:左下角
    //          -----
    //         |qqqqq|    第0个Tile
    // --------------    
    //|AAAAAAAAbbbbbb|    第1个Tile
    // --------------    
    //|qqqqq|             第2个Tile
    // -----                               
    public List<Rect> GetTextTiles(int startIndex,int endIndex,List<UIVertex> output)
    {
        //计算顶点:
        //textpos：图片的左上角
        //之前的富文本被隐藏部分也会占用顶点！！
        //每个索引一个顶点...所以要乘以6
        List<Rect> tilesRect = new List<Rect>();

        if (startIndex * 6 + 1 >= output.Count)
            return new List<Rect>();

        float minH = output[startIndex * 6 + 4].position.y;
        float maxH = output[startIndex * 6 + 1].position.y;
        float minW = output[startIndex * 6 + 4].position.x;
        float maxW = output[startIndex * 6 + 1].position.x;
        float minWordSpace = fontSize-1;    //前一个字符的最低高度与后一个字符的最低高度的差距大于minWordSpace，则看做换行
        for (int curIndex = startIndex; curIndex < endIndex + 1; curIndex++)
        {
            if (curIndex * 6 + 1 >= output.Count)   //??
                break;
            //Debug.Log("[" + curIndex + "]: " + (minH - output[curIndex * 6 + 4].position.y) + " " + output[curIndex * 6 + 4].position.y);
            if (minH - output[curIndex * 6 + 4].position.y >= minWordSpace)   //满足换行条件
            {
                tilesRect.Add(new Rect(minW, minH, maxW - minW, maxH - minH));  //保存Rect
                minH = output[curIndex * 6 + 4].position.y;
                maxH = output[curIndex * 6 + 1].position.y;
                minW = output[curIndex * 6 + 4].position.x;
                maxW = output[curIndex * 6 + 1].position.x;
            }
            else
            {
                minH = Math.Min(minH, output[curIndex * 6 + 4].position.y);
                minW = Math.Min(minW, output[curIndex * 6 + 4].position.x);
                maxH = Math.Max(maxH, output[curIndex * 6 + 1].position.y);
                maxW = Math.Max(maxW, output[curIndex * 6 + 1].position.x);
            }
        }
        tilesRect.Add(new Rect(minW, minH, maxW - minW, maxH - minH));
        return tilesRect;
    }

    private URichTextUnderLineInfo uinfo = null;
    private float oldFontSize;  //下划线信息目测只跟字体大小有关0.0
    public Color32 UnderLineColor = Color.black;
    //获得下划线"_"在该Label的信息
    public URichTextUnderLineInfo GetUnderLineInfo()
    {

        if (uinfo == null || oldFontSize!=this.fontSize)    //字体大小变了，要更新下划线信息
        {
            uinfo = new URichTextUnderLineInfo();
            //计算"_"
            Vector2 extents = this.rectTransform.rect.size;
            //generator生成的顶点是以4为单位
            var settings = this.GetGenerationSettings(extents);
            this.cachedTextGenerator.Populate("_", settings);
            IList<UIVertex> unLineVerts = this.cachedTextGenerator.verts;
            uinfo.width = unLineVerts[1].position.x - unLineVerts[0].position.x;


            for (int i = 0; i < 4; i++)
                uinfo.uvs[i] = unLineVerts[i].uv0;

                
            oldFontSize = this.fontSize;
        }
        uinfo.lineColor = UnderLineColor;
        return uinfo;
    }


    [Serializable]
    public class RichTextHrefEvent : UnityEvent<string> { }

    /// <summary>
    /// 超链接文本区域点击事件
    /// </summary>
    [SerializeField]
    public RichTextHrefEvent onHrefClick = new RichTextHrefEvent();

    /// <summary>
    /// 超链接文本区域移入事件
    /// </summary>
    [SerializeField]
    public RichTextHrefEvent onHrefOver = new RichTextHrefEvent();

    /// <summary>
    /// 超链接文本区域移出事件
    /// </summary>
    [SerializeField]
    public RichTextHrefEvent onHrefOut = new RichTextHrefEvent();


    /// <summary>
    /// 点击事件检测是否点击到超链接文本
    /// </summary>
    /// <param name="eventData"></param>
    public void OnPointerClick(PointerEventData eventData)
    {
        if (m_urlHrefInfo.Count <=0)
            return;

        Vector2 lp;
        RectTransformUtility.ScreenPointToLocalPointInRectangle(
            rectTransform, eventData.position, eventData.pressEventCamera, out lp);

        foreach(var info in m_urlHrefInfo)
        {
            foreach (var rect in info.m_tagTextRects)
            {
                if (rect.Contains(lp))
                {
                    onHrefClick.Invoke(info.strHrefEvent.ToString());

                    if (StringExtension.IsUrl(ref info.strHrefEvent))
                        Application.OpenURL(info.strHrefEvent);
                }
            }
        }
    }

    public void OnPointerExit(PointerEventData eventData)
    {
        m_nPointerState = 1;
    }

    public void OnPointerEnter(PointerEventData eventData)
    {
        m_nPointerState = 0;
    }

    /// <summary>
    /// 鼠标在文本上的状态
    /// -1：没有进入文本
    /// 0：进入文本
    /// 1：进入后再离开文本
    /// </summary>
    private int m_nPointerState = -1;
    /// <summary>
    /// 光标在文本上移动监测
    /// </summary>
    private void OnPointerMove()
    {
        OnURLPointerMove();
    }

    /// <summary>
    /// 响应超链接文本区域光标移动
    /// </summary>
    private void OnURLPointerMove()
    {
        if (m_urlHrefInfo.Count <=0 || m_nPointerState == -1)
            return;

        //离开了文本
        if(m_nPointerState == 1)
        {
            m_nPointerState = -1;
            if(m_nOldURLRect != null)
            {
                onHrefOut.Invoke(m_nOldURLRect.strHrefEvent.ToString());
                //Debug.Log("exit url rect:event:" + m_nOldURLRect.strHrefEvent.ToString());
            }            
            m_nOldURLRect = null;
            return;
        }

        Vector2 lp;
        UPointerEventDataEx eventData = EventSystem.current.CurrentPointerEventData();
        RectTransformUtility.ScreenPointToLocalPointInRectangle(
            rectTransform, eventData.position, USpeedUI.UISystem.Instance.GetCamera(), out lp);

        //在文本内，检测进入/离开某个url字块区域
        foreach(var info in m_urlHrefInfo)
        {
            foreach (var rect in info.m_tagTextRects)
            {
                if (rect.Contains(lp))
                {
                    if (m_nOldURLRect == null)
                    {
                        m_nOldURLRect = info;
                        onHrefOver.Invoke(info.strHrefEvent.ToString());
                        //Debug.Log("enter url rect:" + rect + " event:" + info.strHrefEvent.ToString());
                    }
                    else
                    {
                        if (!info.Equals(m_nOldURLRect))
                        {
                            //Debug.Log("exit url rect: event:" + m_nOldURLRect.strHrefEvent.ToString());
                            onHrefOut.Invoke(info.strHrefEvent.ToString());
                            m_nOldURLRect = info;
                            onHrefOver.Invoke(info.strHrefEvent.ToString());
                            //Debug.Log("enter url rect:event:" + info.strHrefEvent.ToString());
                        }
                    }
                    return;
                }
            }

        }

        //光标还在文本区域内，但不在URL字块区域
        if (m_nOldURLRect != null)
        {
            //Debug.Log("exit url rect: event:"  + m_nOldURLRect.strHrefEvent.ToString());
            onHrefOut.Invoke(m_nOldURLRect.strHrefEvent.ToString());
            m_nOldURLRect = null;
        }
    }

    private URichTextURLInfo m_nOldURLRect;
}

