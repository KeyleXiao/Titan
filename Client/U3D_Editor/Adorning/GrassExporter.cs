using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using System.IO;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;

public class ConvexAogrithm
{
    private List<PointF> nodes;
    private Stack<PointF> sortedNodes;
    public PointF[] sor_nodes;
    public ConvexAogrithm(List<PointF> points)
    {
        nodes = points; 
    }
    private double DistanceOfNodes(PointF p0, PointF p1)
    {
        if (p0.IsEmpty || p1.IsEmpty)
            return 0.0;
        return Mathf.Sqrt((p1.X - p0.X) * (p1.X - p0.X) + (p1.Y - p0.Y) * (p1.Y - p0.Y));
    }
    public void GetNodesByAngle(out PointF p0)
    {
        LinkedList<PointF> list_node = new LinkedList<PointF>();
        p0 = GetMinYPoint();
        LinkedListNode<PointF> node = new LinkedListNode<PointF>(nodes[0]);
        list_node.AddFirst(node);
        for (int i = 1; i < nodes.Count; i++)
        {
            int direct = IsClockDirection(p0, node.Value, nodes[i]);
            if (direct == 1)
            {
                list_node.AddLast(nodes[i]);
                node = list_node.Last;
                //node.Value = nodes[i]; 

            }
            else if (direct == -10)
            {
                list_node.Last.Value = nodes[i];
                //node = list_node.Last 
                //node.Value = nodes[i]; 
            }
            else if (direct == 10)
                continue;
            else if (direct == -1)
            {
                LinkedListNode<PointF> temp = node.Previous;
                while (temp != null && IsClockDirection(p0, temp.Value, nodes[i]) == -1)
                {
                    temp = temp.Previous;
                }
                if (temp == null)
                {
                    list_node.AddFirst(nodes[i]);
                    continue;
                }
                if (IsClockDirection(p0, temp.Value, nodes[i]) == -10)
                    temp.Value = nodes[i];
                else if (IsClockDirection(p0, temp.Value, nodes[i]) == 10)
                    continue;
                else
                    list_node.AddAfter(temp, nodes[i]);
            }
        }
        sor_nodes = list_node.ToArray();
        sortedNodes = new Stack<PointF>();
        sortedNodes.Push(p0);
        sortedNodes.Push(sor_nodes[0]);
        sortedNodes.Push(sor_nodes[1]);
        for (int i = 2; i < sor_nodes.Length; i++)
        {

            PointF p2 = sor_nodes[i];
            PointF p1 = sortedNodes.Pop();
            PointF p0_sec = sortedNodes.Pop();
            sortedNodes.Push(p0_sec);
            sortedNodes.Push(p1);

            if (IsClockDirection1(p0_sec, p1, p2) == 1)
            {
                sortedNodes.Push(p2);
                continue;
            }
            while (IsClockDirection1(p0_sec, p1, p2) != 1)
            {
                sortedNodes.Pop();
                p1 = sortedNodes.Pop();
                p0_sec = sortedNodes.Pop();
                sortedNodes.Push(p0_sec);
                sortedNodes.Push(p1);
            }
            sortedNodes.Push(p2);

        }

    }
    private int IsClockDirection1(PointF p0, PointF p1, PointF p2)
    {
        PointF p0_p1 = new PointF(p1.X - p0.X, p1.Y - p0.Y);
        PointF p0_p2 = new PointF(p2.X - p0.X, p2.Y - p0.Y);
        return (p0_p1.X * p0_p2.Y - p0_p2.X * p0_p1.Y) > 0 ? 1 : -1;
    }
    private PointF GetMinYPoint()
    {
        PointF succNode;
        float miny = nodes.Min(r => r.Y);
        IEnumerable<PointF> pminYs = nodes.Where(r => r.Y == miny);
        PointF[] ps = pminYs.ToArray();
        if (pminYs.Count() > 1)
        {
            succNode = pminYs.FirstOrDefault(r => r.X == pminYs.Min(t => t.X));
            nodes.Remove(succNode);
            return succNode;
        }
        else
        {
            nodes.Remove(ps[0]);
            return ps[0];
        }

    }
  
    private int IsClockDirection(PointF p0, PointF p1, PointF p2)
    {
        PointF p0_p1 = new PointF(p1.X - p0.X, p1.Y - p0.Y);
        PointF p0_p2 = new PointF(p2.X - p0.X, p2.Y - p0.Y);
        if ((p0_p1.X * p0_p2.Y - p0_p2.X * p0_p1.Y) != 0)
            return (p0_p1.X * p0_p2.Y - p0_p2.X * p0_p1.Y) > 0 ? 1 : -1;
        else
            return DistanceOfNodes(p0, p1) > DistanceOfNodes(p0, p2) ? 10 : -10;

    }
    public Stack<PointF> SortedNodes
    {
        get { return sortedNodes; }
    }

    public Stack<PointF> GetResult()
    {
        PointF p;
        GetNodesByAngle(out p); 
        return sortedNodes;
    }


}

public class GrassSubArea
{
    public Renderer mr;
    public PointF[] convex;
    public Hashtable bits;
}

public class GrassArea
{
    public List<GrassSubArea> subareas = new List<GrassSubArea>();

    public static bool IntersectsEx(GrassArea areai,GrassArea areaj)
    {
        foreach (GrassSubArea subi in areai.subareas)
        {
            foreach (GrassSubArea subj in areaj.subareas)
            {
                foreach(DictionaryEntry pairi in subi.bits)
                {
                    if (subj.bits.ContainsKey(pairi.Key))
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    public static bool Intersects(GrassArea areai,GrassArea areaj)
    {
        foreach (GrassSubArea subi in areai.subareas)
        {
            foreach (GrassSubArea subj in areaj.subareas)
            {
                if (subi.mr.bounds.Intersects(subj.mr.bounds))
                {
                    return true;
                }
            }
        }

        return false;
    }

    public void Combine(GrassArea otherarea)
    {
        foreach (GrassSubArea subj in otherarea.subareas)
        {
            subareas.Add(subj);
        }
        otherarea.subareas.Clear();
    }
}


public class ClimbArea
{
    public Renderer mr;
    public PointF[] convex;
    public PointF[] vertex;
    public Hashtable bits;
}

public class GrassExporter : Editor
{


    [MenuItem("Tools/Grass/Export Convex")]
    static void Export1()
    {
        ExportFun(1, 2048, 2048);
    }

    [MenuItem("Tools/Grass/Export Projection")]
    static void Export2()
    {
        ExportFun(2, 2048, 2048);
    }

    [MenuItem("Tools/Grass/Export Bounds")]
    static void Export3()
    {
        ExportFun(3, 2048, 2048);
    }

    [MenuItem("Tools/Grass/Export ClimbAreas")]
    static void Export4()
    {
        ExportClimbArea(1, 2048, 2048);
    }


    static void ExportFun(int method,int imgwidth,int imgheight)
    {
        float fimgwidth = imgwidth;
        float fimgheight = imgheight;
        Rectangle rc = new Rectangle(0, 0, imgwidth, imgheight);
        System.Drawing.Bitmap img = new System.Drawing.Bitmap(imgwidth, imgheight);
        System.Drawing.Graphics gdi = System.Drawing.Graphics.FromImage(img);
        gdi.Clear(System.Drawing.Color.Black);


        //---------------------------------------------------------
        // find renderers
        //---------------------------------------------------------
        List<Renderer> renderers = new List<Renderer>();
        GameObject[] groupgos = Selection.gameObjects;

        foreach (GameObject groupgo in groupgos)
        {
            Renderer[] mrs = groupgo.GetComponentsInChildren<Renderer>();
            if (mrs == null)
            {
                continue;
            }
            renderers.AddRange(mrs);

        }


        //---------------------------------------------------------
        // calc max bound and min bound of whole grass map
        //---------------------------------------------------------
        Vector3 worldmin = new Vector3(9999999.0f, 9999999.0f, 9999999.0f);
        Vector3 worldmax = new Vector3(-9999999.0f, -9999999.0f, -9999999.0f);
        foreach (Renderer mr in renderers)
        {
            Bounds bound = mr.bounds;
            Vector3 min = bound.min;
            Vector3 max = bound.max;

            if (min.x < worldmin.x) worldmin.x = min.x;
            if (min.y < worldmin.y) worldmin.y = min.y;
            if (min.z < worldmin.z) worldmin.z = min.z;
            if (max.x > worldmax.x) worldmax.x = max.x;
            if (max.y > worldmax.y) worldmax.y = max.y;
            if (max.z > worldmax.z) worldmax.z = max.z;
        }

        Vector3 worldsize = worldmax - worldmin;
        Vector3 worldcenter = (worldmax + worldmin) * 0.5f;
        worldsize *= 1.1f;
        worldmax = worldcenter + worldsize * 0.5f;
        worldmin = worldcenter - worldsize * 0.5f;


        //---------------------------------------------------------
        // generate sub area
        //---------------------------------------------------------
        System.Drawing.Brush whitebrush = new System.Drawing.SolidBrush(System.Drawing.Color.White);
        List<GrassArea> areas = new List<GrassArea>();
        for(int k=0;k<renderers.Count;k++)
        {
            Mesh mesh = null;
            Renderer mr = renderers[k] as Renderer;
            if (null == mr)
            {
                continue;
            }

            if (mr.GetType() == typeof(MeshRenderer))
            {
                MeshFilter mf = mr.GetComponent<MeshFilter>();
                if (mf == null)
                {
                    continue;
                }
                mesh = Application.isPlaying ? mf.mesh : mf.sharedMesh;
            }
            else
            {
                SkinnedMeshRenderer skin = mr as SkinnedMeshRenderer;
                mesh = skin.sharedMesh;
            }

            if (mesh == null)
            {
                continue;
            }

            Transform tran = renderers[k].transform;


            List<PointF> nodes = new List<PointF>();
            for (int i = 0; i < mesh.vertexCount; i++)
            {
                Vector3 vertex = mesh.vertices[i];
                Vector3 pos = tran.TransformPoint(vertex);
                PointF point = new PointF();
                point.X = (pos.x - worldmin.x) / worldsize.x * fimgwidth;
                point.Y = fimgheight - 1.0f - (pos.z - worldmin.z) / worldsize.z * fimgheight;
                nodes.Add(point);
            }

            ConvexAogrithm ca = new ConvexAogrithm(nodes);
            Stack<PointF> p_nodes = ca.GetResult();
            PointF[] convex = p_nodes.ToArray();

            gdi.Clear(System.Drawing.Color.Black);
            gdi.FillPolygon(whitebrush, convex);
            gdi.Flush();
 
            System.Drawing.Imaging.BitmapData bmp = img.LockBits(rc, System.Drawing.Imaging.ImageLockMode.ReadOnly, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
            int stride = imgwidth * 4;
            byte[] data=new byte[imgwidth*imgheight*4];
            Marshal.Copy(bmp.Scan0, data, 0, imgwidth * imgheight * 4);
            img.UnlockBits(bmp);
            Hashtable bits = new Hashtable();
            for (int y = 0; y < imgheight; y++)
            {
                for (int x = 0; x < imgwidth; x++)
                {
                    int g = data[y * stride + x * 4 + 1];
                    if (g != 0)
                    {
                        bits[y * imgwidth + x] = g;
                    }
                }
            }

            GrassSubArea subarea = new GrassSubArea();
            subarea.mr = renderers[k];
            subarea.convex = convex;
            subarea.bits = bits;

            GrassArea area = new GrassArea();
            area.subareas.Add(subarea);
            areas.Add(area);

            float fProgress = (float)k / (float)renderers.Count;
            EditorUtility.DisplayProgressBar("Export Grass Map", "Generate Area...", fProgress);
        }
        whitebrush.Dispose();
        whitebrush = null;


        //---------------------------------------------------------
        // intersects and combine
        //---------------------------------------------------------
        int count = 0;
        for (int pass = 0; pass < 10; pass++)
        {
            for (int j = 0; j < areas.Count; j++)
            {
                GrassArea aj = areas[j];

                for (int i = 0; i < j; i++)
                {
                    GrassArea ai = areas[i];
                    if (GrassArea.IntersectsEx(ai,aj))
                    {
                        aj.Combine(ai);
                    }
                }

                count++;

                float fProgress = (float)count / (10.0f*areas.Count);
                EditorUtility.DisplayProgressBar("Export Grass Map", "IntersectsEx...", fProgress);
            }
        }
        EditorUtility.ClearProgressBar();

        //---------------------------------------------------------
        // export
        //---------------------------------------------------------
        string imgpath = EditorUtility.SaveFilePanel("grass map", Application.dataPath, "grassmap", "jpg");
        string csvpath = "";
        if (imgpath.Length > 3)
        {
            csvpath = imgpath.Substring(0, imgpath.Length - 3) + "csv";
        }

        StreamWriter sw = null;
        if (csvpath.Length > 3)
        {
            sw = new StreamWriter(csvpath, false);
        }
        if (sw!=null)
        {
//             sw.WriteLine("?,undefied,undefied,undefied,undefied,undefied,undefied,undefied,undefied,undefied,undefied");
//             sw.WriteLine("?,undefied,undefied,undefied,undefied,undefied,undefied,undefied,undefied,undefied,undefied");
//             sw.WriteLine("#,minx,minz,maxx,maxz");
//             sw.WriteLine("world" + "," + worldmin.x.ToString() + "," + worldmin.z.ToString() + "," + worldmax.x.ToString() + "," + worldmax.z.ToString());
//             sw.WriteLine("#,area,subarea,colorr,colorg,colorb,minx,minz,maxx,maxz,convex");

            {
                sw.WriteLine("area,subarea,colorr,colorg,colorb,bmp_minx,bmp_minz,bmp_maxx,bmp_maxz,minx,minz,maxx,maxz,convex");
                sw.WriteLine("int,int,int,int,int,float,float,float,float,float,float,float,float,string");
            }
        }

        gdi.Clear(System.Drawing.Color.Black);
        int areacount = 0;
        foreach (GrassArea area in areas)
        {
            if (area.subareas.Count <= 0)
            {
                continue;
            }

            int colorindex = areacount + 1;
            int b = colorindex % 4;
            int g = (colorindex / 4) % 4;
            int r = colorindex / (4 * 4);
            r *= 64;
            g *= 64;
            b *= 64;
            if (r > 255) r = 255;
            if (g > 255) g = 255;
            if (b > 255) b = 255;
            System.Drawing.Color c = System.Drawing.Color.FromArgb(r, g, b);
            System.Drawing.Brush brush = new System.Drawing.SolidBrush(c);

            for (int k = 0; k < area.subareas.Count; k++)
            {
                GrassSubArea subarea = area.subareas[k];

                if (method == 1)
                {
                    gdi.FillPolygon(brush, subarea.convex);
                }
                else if (method == 2)
                {
                    MeshFilter mf = subarea.mr.GetComponent<MeshFilter>();
                    if (mf == null)
                    {
                        continue;
                    }

                    Mesh mesh = Application.isPlaying ? mf.mesh : mf.sharedMesh;
                    if (mesh == null)
                    {
                        continue;
                    }

                    Transform tran = mf.transform;
                    for (int j = 0; j < mesh.triangles.Length / 3; j++)
                    {
                        int[] indices = new int[3];
                        for (int i = 0; i < 3; i++)
                        {
                            indices[i] = mesh.triangles[j * 3 + i];
                        }
                        Vector3[] vertices = new Vector3[3];
                        for (int i = 0; i < 3; i++)
                        {
                            vertices[i] = mesh.vertices[indices[i]];
                        }
                        Vector3[] positions = new Vector3[3];
                        for (int i = 0; i < 3; i++)
                        {
                            positions[i] = tran.TransformPoint(vertices[i]);
                        }
                        System.Drawing.PointF[] points = new System.Drawing.PointF[3];
                        for (int i = 0; i < 3; i++)
                        {
                            points[i].X = (positions[i].x - worldmin.x) / worldsize.x * fimgwidth;
                            points[i].Y = fimgheight - 1.0f - (positions[i].z - worldmin.z) / worldsize.z * fimgheight;
                        }

                        gdi.FillPolygon(brush, points);
                    }

                }
                else
                {
                    Bounds bound = subarea.mr.bounds;
                    Vector3 min = bound.min;
                    Vector3 max = bound.max;

                    float px = (min.x - worldmin.x) / worldsize.x * (float)imgwidth;
                    float py = 511.0f - (min.z - worldmin.z) / worldsize.z * (float)imgheight;
                    float pw = (max.x - min.x) / worldsize.x * (float)imgwidth;
                    float ph = (max.z - min.z) / worldsize.z * (float)imgheight;
                    py -= ph;
   
                    gdi.FillRectangle(brush, px, py, pw, ph);
       
                }

                if (sw!=null)
                {
                    string convexstr="";
                    foreach(PointF pt in subarea.convex)
                    {
                        convexstr+="{"+pt.X.ToString()+"|"+pt.Y.ToString()+"};";
                    }
//                     sw.WriteLine("area" + ","
//                         + areacount.ToString() + ","
//                         + k.ToString() + ","
//                         + r.ToString() + ","
//                         + g.ToString() + ","
//                         + b.ToString() + ","
//                         + subarea.mr.bounds.min.x.ToString() + ","
//                         + subarea.mr.bounds.min.z.ToString() + ","
//                         + subarea.mr.bounds.max.x.ToString() + ","
//                         + subarea.mr.bounds.max.z.ToString() + ","
//                         + convexstr
//                         );

                    {
                        sw.WriteLine(areacount.ToString() + ","
                        + k.ToString() + ","
                        + r.ToString() + ","
                        + g.ToString() + ","
                        + b.ToString() + ","
                        + worldmin.x.ToString() + ","
                        + worldmin.z.ToString() + ","
                        + worldmax.x.ToString() + ","
                        + worldmax.z.ToString() + ","
                        + subarea.mr.bounds.min.x.ToString() + ","
                        + subarea.mr.bounds.min.z.ToString() + ","
                        + subarea.mr.bounds.max.x.ToString() + ","
                        + subarea.mr.bounds.max.z.ToString() + ","
                        + convexstr
                        );
                    }
                }
            }

            brush.Dispose();
            brush = null;

            areacount++;
        }

        gdi.Flush();

        if (imgpath.Length > 3)
        {
            img.Save(imgpath);
        }

        if (sw != null)
        {
            sw.Close();
            sw.Dispose();
            sw = null;
        }

        gdi.Dispose();
        gdi = null;
        img.Dispose();
        img = null;

        if (imgpath.Length > 3)
        {
            Debug.Log("generate grass map ok:" + imgpath);
            System.Diagnostics.Process.Start(imgpath, "");
        }
        if (csvpath.Length > 3)
        {
            Debug.Log("generate grass list ok:" + csvpath);
            //System.Diagnostics.Process.Start(csvpath, "");
        }

      
    }



    static void ExportClimbArea(int method, int imgwidth, int imgheight)
    {
        float fimgwidth = imgwidth;
        float fimgheight = imgheight;
        Rectangle rc = new Rectangle(0, 0, imgwidth, imgheight);
        System.Drawing.Bitmap img = new System.Drawing.Bitmap(imgwidth, imgheight);
        System.Drawing.Graphics gdi = System.Drawing.Graphics.FromImage(img);
        gdi.Clear(System.Drawing.Color.Black);


        //---------------------------------------------------------
        // find renderers
        //---------------------------------------------------------
        List<Renderer> renderers = new List<Renderer>();
        GameObject[] groupgos = Selection.gameObjects;

        foreach (GameObject groupgo in groupgos)
        {
            Renderer[] mrs = groupgo.GetComponentsInChildren<Renderer>();
            if (mrs == null)
            {
                continue;
            }
            renderers.AddRange(mrs);

        }


        //---------------------------------------------------------
        // calc max bound and min bound of whole grass map
        //---------------------------------------------------------
        Vector3 worldmin = new Vector3(9999999.0f, 9999999.0f, 9999999.0f);
        Vector3 worldmax = new Vector3(-9999999.0f, -9999999.0f, -9999999.0f);
        foreach (Renderer mr in renderers)
        {
            Bounds bound = mr.bounds;
            Vector3 min = bound.min;
            Vector3 max = bound.max;

            if (min.x < worldmin.x) worldmin.x = min.x;
            if (min.y < worldmin.y) worldmin.y = min.y;
            if (min.z < worldmin.z) worldmin.z = min.z;
            if (max.x > worldmax.x) worldmax.x = max.x;
            if (max.y > worldmax.y) worldmax.y = max.y;
            if (max.z > worldmax.z) worldmax.z = max.z;
        }

        Vector3 worldsize = worldmax - worldmin;
        Vector3 worldcenter = (worldmax + worldmin) * 0.5f;
        worldsize *= 1.1f;
        worldmax = worldcenter + worldsize * 0.5f;
        worldmin = worldcenter - worldsize * 0.5f;


        //---------------------------------------------------------
        // generate sub area
        //---------------------------------------------------------
        System.Drawing.Brush whitebrush = new System.Drawing.SolidBrush(System.Drawing.Color.White);
        List<ClimbArea> areas = new List<ClimbArea>();
        for (int k = 0; k < renderers.Count; k++)
        {
            Mesh mesh = null;
            Renderer mr = renderers[k] as Renderer;
            if (null == mr)
            {
                continue;
            }

            if (mr.GetType() == typeof(MeshRenderer))
            {
                MeshFilter mf = mr.GetComponent<MeshFilter>();
                if (mf == null)
                {
                    continue;
                }
                mesh = Application.isPlaying ? mf.mesh : mf.sharedMesh;
            }
            else
            {
                SkinnedMeshRenderer skin = mr as SkinnedMeshRenderer;
                mesh = skin.sharedMesh;
            }

            if (mesh == null)
            {
                continue;
            }

            Transform tran = renderers[k].transform;


            List<PointF> nodes = new List<PointF>();
            List<PointF> vertexs = new List<PointF>();
            for (int i = 0; i < mesh.vertexCount; i++)
            {
                Vector3 vertex = mesh.vertices[i];
                Vector3 pos = tran.TransformPoint(vertex);
                PointF point = new PointF();
                point.X = (pos.x - worldmin.x) / worldsize.x * fimgwidth;
                point.Y = fimgheight - 1.0f - (pos.z - worldmin.z) / worldsize.z * fimgheight;
                nodes.Add(point);
                PointF vert = new PointF();
                vert.X = pos.x;
                vert.Y = pos.z;
                vertexs.Add(vert);
            }

            ConvexAogrithm ca = new ConvexAogrithm(nodes);
            Stack<PointF> p_nodes = ca.GetResult();
            PointF[] convex = p_nodes.ToArray();

            ConvexAogrithm cv = new ConvexAogrithm(vertexs);
            Stack<PointF> p_verts = cv.GetResult();            

            gdi.Clear(System.Drawing.Color.Black);
            gdi.FillPolygon(whitebrush, convex);
            gdi.Flush();

            System.Drawing.Imaging.BitmapData bmp = img.LockBits(rc, System.Drawing.Imaging.ImageLockMode.ReadOnly, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
            int stride = imgwidth * 4;
            byte[] data = new byte[imgwidth * imgheight * 4];
            Marshal.Copy(bmp.Scan0, data, 0, imgwidth * imgheight * 4);
            img.UnlockBits(bmp);
            Hashtable bits = new Hashtable();
            for (int y = 0; y < imgheight; y++)
            {
                for (int x = 0; x < imgwidth; x++)
                {
                    int g = data[y * stride + x * 4 + 1];
                    if (g != 0)
                    {
                        bits[y * imgwidth + x] = g;
                    }
                }
            }

            //区域点输出规则：
            //飞檐走壁为一个cube，所以区域点为4个顶点的矩形
            //取最短的边开始，顺时针或逆时针方向输出
            //因此，AB CD为短边，BC为长边 
            int minEdgeIndex = 0;
            float minEdgeLength = -1.0f;
            PointF[] e_verts = p_verts.ToArray();
            int e_vertsLen = e_verts.Length;
            for (int i = 0; i < e_vertsLen; i++)
            {
                int j = (i + 1) % e_vertsLen;
                Vector2 pa=new Vector2(e_verts[i].X,e_verts[i].Y);
                Vector2 pb=new Vector2(e_verts[j].X,e_verts[j].Y);
                float edgeLength = (pa - pb).magnitude;
                if (minEdgeLength < 0 || minEdgeLength > edgeLength)
                {
                    minEdgeIndex = i;
                    minEdgeLength = edgeLength;
                }
            }
            PointF[] o_verts = new PointF[e_vertsLen];
            for (int i = 0; i < e_vertsLen; i++)
            {
                int j = (i + minEdgeIndex) % e_vertsLen;
                o_verts[i] = e_verts[j];
            }

            ClimbArea newArea = new ClimbArea();
            newArea.mr = renderers[k];
            newArea.convex = convex;
            newArea.bits = bits;
            newArea.vertex = o_verts;
            areas.Add(newArea);

            float fProgress = (float)k / (float)renderers.Count;
            EditorUtility.DisplayProgressBar("Export Climb Map", "Generate Area...", fProgress);
        }
        whitebrush.Dispose();
        whitebrush = null;
        
        EditorUtility.ClearProgressBar();

        //---------------------------------------------------------
        // export
        //---------------------------------------------------------
        string imgpath = EditorUtility.SaveFilePanel("climbArea map", Application.dataPath, "climbAreaMap", "jpg");
        string csvpath = "";
        if (imgpath.Length > 3)
        {
            csvpath = imgpath.Substring(0, imgpath.Length - 3) + "csv";
        }

        StreamWriter sw = null;
        if (csvpath.Length > 3)
        {
            sw = new StreamWriter(csvpath, false);
        }
        if (sw != null)
        {
            //             sw.WriteLine("?,undefied,undefied,undefied,undefied,undefied,undefied,undefied,undefied,undefied,undefied");
            //             sw.WriteLine("?,undefied,undefied,undefied,undefied,undefied,undefied,undefied,undefied,undefied,undefied");
            //             sw.WriteLine("#,minx,minz,maxx,maxz");
            //             sw.WriteLine("world" + "," + worldmin.x.ToString() + "," + worldmin.z.ToString() + "," + worldmax.x.ToString() + "," + worldmax.z.ToString());
            //             sw.WriteLine("#,area,subarea,colorr,colorg,colorb,minx,minz,maxx,maxz,convex");

            {
                sw.WriteLine("area,subarea,colorr,colorg,colorb,bmp_minx,bmp_minz,bmp_maxx,bmp_maxz,minx,minz,maxx,maxz,convex");
                sw.WriteLine("int,int,int,int,int,float,float,float,float,float,float,float,float,string");
            }
        }

        gdi.Clear(System.Drawing.Color.Black);
        int areacount = 0;
        foreach (ClimbArea area in areas)
        {
            int colorindex = areacount + 1;
            int b = colorindex % 4;
            int g = (colorindex / 4) % 4;
            int r = colorindex / (4 * 4);
            r *= 64;
            g *= 64;
            b *= 64;
            if (r > 255) r = 255;
            if (g > 255) g = 255;
            if (b > 255) b = 255;
            System.Drawing.Color c = System.Drawing.Color.FromArgb(r, g, b);
            System.Drawing.Brush brush = new System.Drawing.SolidBrush(c);

           
                if (method == 1)
                {
                    gdi.FillPolygon(brush, area.convex);
                }
                else if (method == 2)
                {
                    MeshFilter mf = area.mr.GetComponent<MeshFilter>();
                    if (mf == null)
                    {
                        continue;
                    }

                    Mesh mesh = Application.isPlaying ? mf.mesh : mf.sharedMesh;
                    if (mesh == null)
                    {
                        continue;
                    }

                    Transform tran = mf.transform;
                    for (int j = 0; j < mesh.triangles.Length / 3; j++)
                    {
                        int[] indices = new int[3];
                        for (int i = 0; i < 3; i++)
                        {
                            indices[i] = mesh.triangles[j * 3 + i];
                        }
                        Vector3[] vertices = new Vector3[3];
                        for (int i = 0; i < 3; i++)
                        {
                            vertices[i] = mesh.vertices[indices[i]];
                        }
                        Vector3[] positions = new Vector3[3];
                        for (int i = 0; i < 3; i++)
                        {
                            positions[i] = tran.TransformPoint(vertices[i]);
                        }
                        System.Drawing.PointF[] points = new System.Drawing.PointF[3];
                        for (int i = 0; i < 3; i++)
                        {
                            points[i].X = (positions[i].x - worldmin.x) / worldsize.x * fimgwidth;
                            points[i].Y = fimgheight - 1.0f - (positions[i].z - worldmin.z) / worldsize.z * fimgheight;
                        }

                        gdi.FillPolygon(brush, points);
                    }

                }
                else
                {
                    Bounds bound = area.mr.bounds;
                    Vector3 min = bound.min;
                    Vector3 max = bound.max;

                    float px = (min.x - worldmin.x) / worldsize.x * (float)imgwidth;
                    float py = 511.0f - (min.z - worldmin.z) / worldsize.z * (float)imgheight;
                    float pw = (max.x - min.x) / worldsize.x * (float)imgwidth;
                    float ph = (max.z - min.z) / worldsize.z * (float)imgheight;
                    py -= ph;

                    gdi.FillRectangle(brush, px, py, pw, ph);

                }

                if (sw != null)
                {
                    string convexstr = "";
                    foreach (PointF pt in area.vertex)
                    {
                        convexstr += "{" + pt.X.ToString() + "|" + pt.Y.ToString() + "};";
                    }
                    //                     sw.WriteLine("area" + ","
                    //                         + areacount.ToString() + ","
                    //                         + k.ToString() + ","
                    //                         + r.ToString() + ","
                    //                         + g.ToString() + ","
                    //                         + b.ToString() + ","
                    //                         + subarea.mr.bounds.min.x.ToString() + ","
                    //                         + subarea.mr.bounds.min.z.ToString() + ","
                    //                         + subarea.mr.bounds.max.x.ToString() + ","
                    //                         + subarea.mr.bounds.max.z.ToString() + ","
                    //                         + convexstr
                    //                         );

                    {
                        sw.WriteLine(areacount.ToString() + ","
                        + areacount.ToString() + ","
                        + r.ToString() + ","
                        + g.ToString() + ","
                        + b.ToString() + ","
                        + worldmin.x.ToString() + ","
                        + worldmin.z.ToString() + ","
                        + worldmax.x.ToString() + ","
                        + worldmax.z.ToString() + ","
                        + area.mr.bounds.min.x.ToString() + ","
                        + area.mr.bounds.min.z.ToString() + ","
                        + area.mr.bounds.max.x.ToString() + ","
                        + area.mr.bounds.max.z.ToString() + ","
                        + convexstr
                        );
                    }
                }
            

            brush.Dispose();
            brush = null;

            areacount++;
        }

        gdi.Flush();

        if (imgpath.Length > 3)
        {
            img.Save(imgpath);
        }

        if (sw != null)
        {
            sw.Close();
            sw.Dispose();
            sw = null;
        }

        gdi.Dispose();
        gdi = null;
        img.Dispose();
        img = null;

        if (imgpath.Length > 3)
        {
            Debug.Log("generate climb map ok:" + imgpath);
            System.Diagnostics.Process.Start(imgpath, "");
        }
        if (csvpath.Length > 3)
        {
            Debug.Log("generate climb list ok:" + csvpath);
            //System.Diagnostics.Process.Start(csvpath, "");
        }


    }
}
