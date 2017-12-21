using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

namespace U3D_Editor.Effect.DecalSystem
{
    class DecalEditorUtil
    {
        private static bool isInit = false;
        public static void InitCallBack()
        {
            if (isInit)
                return;
#if UNITY_EDITOR
            EditorApplication.playmodeStateChanged += OnGameStateChange;
            isInit = true;
#endif
        }
#if UNITY_EDITOR
        public static void OnGameStateChange()
        {
            //Debug.Log(EditorApplication.isPlayingOrWillChangePlaymode + " " + EditorApplication.isPlaying);
            ClearDealShowCubePool();
        }
#endif
        private static string decalAltasPath = "/Artist/Effect/textures/tiehua/DecalMark/dapheng_he.png";
        private static string decalPath_default = "/Artist/Effect/textures/tiehua/DecalMark/dapheng01.png";

        private static Dictionary<string, GameObject> decalShowCubePool = new Dictionary<string, GameObject>();

        public static void ClearDealShowCubePool()
        {
            foreach(KeyValuePair<string,GameObject> cubePair in decalShowCubePool)
            {
                GameObject.DestroyImmediate(cubePair.Value);
                //Debug.Log("destroy Gameobject: " + cubePair.Key);
            }
            decalShowCubePool.Clear();
        }

        public static GameObject GetDecalShowCube(string name)
        {
            //这个阶段不允许新建物体
            if(EditorApplication.isPlayingOrWillChangePlaymode && !EditorApplication.isPlaying)
            {
                return null;
            }
            if (decalShowCubePool.ContainsKey(name))
            {
                if (decalShowCubePool[name] == null)
                {
                    decalShowCubePool.Remove(name);
                    return GetDecalShowCube(name);
                }
                else
                    return decalShowCubePool[name];
            }
            else
            {
                Debug.Log("new Gameobject: " + name);
                GameObject decal = GameObject.CreatePrimitive(PrimitiveType.Cube);
                decal.GetComponent<MeshRenderer>().material = GetDefaultDecalMaterial();
                decal.name = name;
                decalShowCubePool.Add(name, decal);
                return decal;
            }
        }

        private static Material editorDecalMaterial;
        private static Sprite editorDecalSprite;
        public static Material GetDefaultDecalMaterial()
        {
            if (editorDecalMaterial == null)
            {
                editorDecalMaterial = new Material(Shader.Find("Decal/Default"));
            }
            return editorDecalMaterial;
        }

        public static Sprite GetDefaultDecalSprite()
        {
            if (editorDecalSprite == null)
            {
                editorDecalSprite = AssetDatabase.LoadAssetAtPath<Sprite>(Application.dataPath + decalPath_default);
            }
            return editorDecalSprite;
        }

        public static Sprite DrawDecalSpriteList(Sprite sprite)
        {
            string path = "Assets" + decalAltasPath;
            Object[] objs = AssetDatabase.LoadAllAssetsAtPath(path);
            List<Sprite> sprites = new List<Sprite>();
            foreach (Object o in objs)
            {
                if (o is Sprite) sprites.Add((Sprite)o);
            }

            return DrawSpriteList(sprite, sprites.ToArray());
        }

        private static Sprite DrawSpriteList(Sprite sprite, Sprite[] list)
        {
            GUILayout.BeginVertical(GUI.skin.box, GUILayout.MinHeight(50));
            for (int i = 0, y = 0; i < list.Length; y++)
            {
                GUILayout.BeginHorizontal();
                for (int x = 0; x < 5; x++, i++)
                {
                    Rect rect = GUILayoutUtility.GetAspectRect(1);
                    if (i < list.Length)
                    {
                        Sprite spr = list[i];
                        bool selected = DrawItem(rect, spr, sprite == spr);
                        if (selected) sprite = spr;
                    }
                }
                GUILayout.EndHorizontal();
            }
            GUILayout.EndVertical();
            return sprite;
        }

        private static bool DrawItem(Rect rect, Sprite sprite, bool selected)
        {
            if (selected)
            {
                GUI.color = Color.blue;
                GUI.DrawTexture(rect, EditorGUIUtility.whiteTexture);
                GUI.color = Color.white;
            }

            Texture texture = sprite.texture;
            Rect texRect = sprite.rect;
            texRect.x /= texture.width;
            texRect.y /= texture.height;
            texRect.width /= texture.width;
            texRect.height /= texture.height;
            GUI.DrawTextureWithTexCoords(rect, texture, texRect);

            selected = Event.current.type == EventType.MouseDown && rect.Contains(Event.current.mousePosition);
            if (selected)
            {
                GUI.changed = true;
                Event.current.Use();
                return true;
            }
            return false;
        }
    }

}
