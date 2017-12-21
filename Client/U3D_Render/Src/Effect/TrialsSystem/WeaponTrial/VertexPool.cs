using UnityEngine;
using System.Collections;
using System.Collections.Generic;


namespace Effect
{
    namespace EffectCore
    {
        public class VertexPool
        {
            public class VertexSegment
            {
                public int VertStart;
                public int IndexStart;
                public int VertCount;
                public int IndexCount;
                public VertexPool Pool;

                public VertexSegment(int start, int count, int istart, int icount, VertexPool pool)
                {
                    VertStart = start;
                    VertCount = count;
                    IndexCount = icount;
                    IndexStart = istart;
                    Pool = pool;
                }


                public void ClearIndices()
                {
                    for (int i = IndexStart; i < IndexStart + IndexCount; i++)
                    {
                        Pool.Indices[i] = 0;
                    }

                    Pool.IndiceChanged = true;
                }

            }

            public Vector3[] Vertices;
            public int[] Indices;
            public Vector2[] UVs;
            public Color[] Colors;

            //用来装额外的参数到shader里
            public Vector2[] UVs2;

            public bool IndiceChanged;
            public bool ColorChanged;
            public bool UVChanged;
            public bool VertChanged;
            public bool UV2Changed;



            public Mesh Mesh;
            public Material Material;

            protected int VertexTotal;
            protected int VertexUsed;
            protected int IndexTotal = 0;
            protected int IndexUsed = 0;
            public bool FirstUpdate = true;

            protected bool VertCountChanged;


            public const int BlockSize = 108;

            public float BoundsScheduleTime = 1f;
            public float ElapsedTime = 0f;


            protected List<VertexSegment> SegmentList = new List<VertexSegment>();

            public void RecalculateBounds()
            {
                Mesh.RecalculateBounds();
            }

            public VertexPool(Mesh mesh, Material material)
            {
                VertexTotal = VertexUsed = 0;
                VertCountChanged = false;
                Mesh = mesh;
                Material = material;
                InitArrays();

                IndiceChanged = ColorChanged = UVChanged = UV2Changed = VertChanged = true;
            }



            public VertexSegment GetRopeVertexSeg(int maxcount)
            {
                VertexSegment segment = GetVertices(maxcount * 2, (maxcount - 1) * 6);
                return segment;
            }



            public Material GetMaterial()
            {
                return Material;
            }



            public VertexSegment GetVertices(int vcount, int icount)
            {
                int vertNeed = 0;
                int indexNeed = 0;
                if (VertexUsed + vcount >= VertexTotal)
                {
                    vertNeed = (vcount / BlockSize + 1) * BlockSize;
                }
                if (IndexUsed + icount >= IndexTotal)
                {
                    indexNeed = (icount / BlockSize + 1) * BlockSize;
                }
                VertexUsed += vcount;
                IndexUsed += icount;
                if (vertNeed != 0 || indexNeed != 0)
                {
                    EnlargeArrays(vertNeed, indexNeed);
                    VertexTotal += vertNeed;
                    IndexTotal += indexNeed;
                }

                VertexSegment ret = new VertexSegment(VertexUsed - vcount, vcount, IndexUsed - icount, icount, this);

                return ret;
            }


            void InitDefaultShaderParam(Vector2[] uv2)
            {
                for (int i = 0; i < uv2.Length; i++)
                {
                    //默认参数
                    uv2[i].x = 1f;

                    //默认参数
                    uv2[i].y = 0f;
                }
            }

            protected void InitArrays()
            {
                Vertices = new Vector3[4];
                UVs = new Vector2[4];
                UVs2 = new Vector2[4];
                Colors = new Color[4];
                Indices = new int[6];
                VertexTotal = 4;
                IndexTotal = 6;

                InitDefaultShaderParam(UVs2);
            }



            public void EnlargeArrays(int count, int icount)
            {
                Vector3[] tempVerts = Vertices;
                Vertices = new Vector3[Vertices.Length + count];
                tempVerts.CopyTo(Vertices, 0);

                Vector2[] tempUVs = UVs;
                UVs = new Vector2[UVs.Length + count];
                tempUVs.CopyTo(UVs, 0);

                Vector2[] tempUVs2 = UVs2;
                UVs2 = new Vector2[UVs2.Length + count];
                tempUVs2.CopyTo(UVs2, 0);
                InitDefaultShaderParam(UVs2);

                Color[] tempColors = Colors;
                Colors = new Color[Colors.Length + count];
                tempColors.CopyTo(Colors, 0);

                int[] tempTris = Indices;
                Indices = new int[Indices.Length + icount];
                tempTris.CopyTo(Indices, 0);

                VertCountChanged = true;
                IndiceChanged = true;
                ColorChanged = true;
                UVChanged = true;
                VertChanged = true;
                UV2Changed = true;
            }

            public void LateUpdate()
            {
                if (VertCountChanged)
                {
                    Mesh.Clear();
                }

                // 修改顶点
                Mesh.vertices = Vertices;
                if (UVChanged)
                {
                    Mesh.uv = UVs;
                }

                if (UV2Changed)
                {
                    Mesh.uv2 = UVs2;
                }

                if (ColorChanged)
                {
                    Mesh.colors = Colors;
                }

                if (IndiceChanged)
                {
                    Mesh.triangles = Indices;
                }

                ElapsedTime += Time.deltaTime;
                if (ElapsedTime > BoundsScheduleTime || FirstUpdate)
                {
                    RecalculateBounds();
                    ElapsedTime = 0f;
                }

                //跳过第一次更新。
                if (ElapsedTime > BoundsScheduleTime)
                    FirstUpdate = false;

                VertCountChanged = false;
                IndiceChanged = false;
                ColorChanged = false;
                UVChanged = false;
                UV2Changed = false;
                VertChanged = false;
            }
        }
    }
}

