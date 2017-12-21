using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class URichTextSpriteMeshInfo{
    public int spriteIndex;     //序列帧动画的index
    public string animKey;  //序列帧动画的名字
    public int animLen =1;     //序列帧长度
    public int place;   //在字符串的位置
    public bool isDirty = true;
    public Vector3[] vertices = new Vector3[4];
    public Vector2[] uvs = new Vector2[4];
    public Color[] colors = new Color[4];
    public int[] triangles = new int[6];
}
