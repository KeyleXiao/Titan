using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using System.IO;



    public class DynamicTerrainLoader
    {
       // [MenuItem("Tools/Terrain/Create")]
        public static void Create()
        {
            float size = 256.0f * 2.5f ;
            float heightscale = 0.25f;
            GameObject go = new GameObject();
            go.name = "Terrain";
            Terrain ter = go.AddComponent<Terrain>();
            TerrainData td = new TerrainData();
            td.size = new Vector3(size, size * heightscale, size);
            ter.terrainData = td;
            ter.Flush();
            go.transform.position = new Vector3(-size * 0.5f, -4.0f, -size * 0.5f);
            Selection.activeGameObject = go;
        }

       // [MenuItem("Tools/Terrain/Load Heightfield")]  
        public static void LoadHeightfield()
        {
            GameObject go = Selection.activeGameObject;
            if (go == null) return;

            Terrain ter=go.GetComponent<Terrain>();
            if (ter == null) return;

            string path = EditorUtility.OpenFilePanel("Load Heightfield", Application.dataPath, "jpg");
            if (path != null && path.Length > 0)
            {
                string datapath = Application.dataPath;
                //path = path.Substring(datapath.Length+1);
                //path = "Assets/" + path;
                byte[] data = File.ReadAllBytes(path);
                Texture2D tex = new Texture2D(4, 4);
                tex.LoadImage(data);
                LoadTerrainHeightfieldFromTexture(ter,tex);
            }
        }

        //[MenuItem("Tools/Terrain/Load Surfacefield")]
        public static void LoadSurfacefield()
        {
            GameObject go = Selection.activeGameObject;
            if (go == null) return;

            Terrain ter = go.GetComponent<Terrain>();
            if (ter == null) return;

            string path = EditorUtility.OpenFilePanel("Load Surfacefield", Application.dataPath, "jpg");
            if (path != null && path.Length > 0)
            {
                string datapath = Application.dataPath;
                //path = path.Substring(datapath.Length+1);
                //path = "Assets/" + path;
                byte[] data = File.ReadAllBytes(path);
                Texture2D tex = new Texture2D(4, 4);
                tex.LoadImage(data);
                LoadTerrainMaterialfieldFromTexture(ter, tex);
            }
        }

        public static void LoadTerrainHeightfieldFromTexture(Terrain ter, Texture2D tex, bool bMirrorX = false, bool bMirrorY = false)
        {
            if (tex == null)
            {
                Debug.LogError("tex == null");
                return;
            }

            TerrainData terrainData = ter.terrainData;

            int terwidth = terrainData.heightmapWidth;
            int terheight = terrainData.heightmapHeight;
            int texwidth = tex.width;
            int texheight = tex.height;

            Color[] cs = tex.GetPixels();

            float[,] array = new float[terwidth, terheight];
            for (int j = 0; j < terheight; j++)
            {
                for (int i = 0; i < terwidth; i++)
                {
                    float fu = (float)i / (float)(terwidth - 1);
                    float fv = (float)j / (float)(terheight - 1);
                    if (bMirrorX)
                        fu = 1.0f - fu;
                    if (bMirrorY)
                        fv = 1.0f - fv;
                    float fx = fu * (texwidth - 1);
                    float fy = fv * (texheight - 1);
                    int nx = (int)fx;
                    int ny = (int)fy;

                    Color c = cs[ny * texwidth + nx];
                    float s = c.r / 4.0f;

                    array[j, i] = s;
                }
            }
            terrainData.SetHeights(0, 0, array);

            ter.terrainData = terrainData;
            ter.Flush();
        }

        public static void LoadTerrainMaterialfieldFromTexture(Terrain ter, Texture2D tex, bool bMirrorX = false, bool bMirrorY = false)
        {
            if (tex == null)
            {
                Debug.LogError("tex == null");
                return;
            }

            TerrainData terrainData = ter.terrainData;

            Texture2D[] tmpTextures = new Texture2D[1];
            tmpTextures[0] = tex;
            Vector2 tilesize = new Vector2(terrainData.size.x, terrainData.size.z);
            if (bMirrorX)
                tilesize.x *= -1.0f;
            if (bMirrorY)
                tilesize.y *= -1.0f;
            SplatPrototype[] sps = CreateSplatPrototypes(tmpTextures, tilesize, new Vector2(0, 0));
            terrainData.splatPrototypes = sps;

            ter.terrainData = terrainData;
            ter.Flush();
        }

        private static SplatPrototype CreateSplatPrototype(Texture2D tmpTexture, Vector2 tmpTileSize, Vector2 tmpOffset)
        {
            SplatPrototype outSplatPrototype = new SplatPrototype();
            outSplatPrototype.texture = tmpTexture;
            outSplatPrototype.tileOffset = tmpOffset;
            outSplatPrototype.tileSize = tmpTileSize;
            return outSplatPrototype;
        }

        private static SplatPrototype[] CreateSplatPrototypes(Texture2D[] tmpTextures, Vector2 tmpTileSize, Vector2 tmpOffset)
        {
            int num = tmpTextures.Length;
            SplatPrototype[] outSplatPrototypes = new SplatPrototype[num];
            for (int i = 0; i < num; i++)
            {
                outSplatPrototypes[i] = CreateSplatPrototype(tmpTextures[i], tmpTileSize, tmpOffset);
            }
            return outSplatPrototypes;
        }
    }
