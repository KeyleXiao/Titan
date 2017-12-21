/*******************************************************************
** 文件名:	SkySystemManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/11/3
** 版  本:	1.0
** 描  述:	天空系统
** 应  用:  	天空系统--星星

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections.Generic;
using System.IO;

namespace SkySystem
{
    public class SkyStars 
	{
		private float starSizeScale = 1.0f;
        private const string StarsDataConfig = "SkySystemResource/SkyStarsData";

		private List<CombineInstance> starQuad = new List<CombineInstance>();
		
		private struct Star
		{
			public Vector3 position;
			public Color color;
		}

		public Mesh InitializeStars()
		{
			float starDistance = (Camera.main !=null)? Camera.main.farClipPlane - 10f :
									(Camera.current != null)? Camera.current.farClipPlane : 990f ;
			float starSize = starDistance / 100 * starSizeScale;

			//加载星星配置，这个必须得有
            TextAsset data = Resources.Load<TextAsset>(StarsDataConfig);
			if (data == null){
				Debug.LogError("找不到星星配置文件");
				return null;
			}

			const int numberOfStars = 9110;
            Star[] stars = new Star[numberOfStars];

			using (BinaryReader reader = new BinaryReader(new MemoryStream(data.bytes)))
			{
				for (int i = 0; i < numberOfStars; i++)
				{
					stars[i].position.x = reader.ReadSingle();
					stars[i].position.z = reader.ReadSingle();
					stars[i].position.y = reader.ReadSingle(); // Z-up to Y-up

					stars[i].position = Vector3.Scale (stars[i].position,new Vector3(-1f,1f,-1f));

					stars[i].color.r = reader.ReadSingle();
					stars[i].color.g = reader.ReadSingle();
					stars[i].color.b = reader.ReadSingle();

					// 取出亮度，根据亮度来优化排序
					float magnitude = Vector3.Dot(new Vector3(stars[i].color.r,stars[i].color.g,stars[i].color.b), new Vector3(0.22f,0.707f,0.071f));
					
					stars[i].color.a = magnitude ;

                    //亮度太小或者太大，都过滤了
                    if (stars[i].color.a < 1.62e-2f || stars[i].color.a > 2.2f)
                    {
                        continue; 
                    } 

					CombineInstance ci = new CombineInstance();
					ci.mesh = createQuad(starSize);

					ci.transform = BillboardMatrix(stars[i].position * starDistance);

					Color[] colors = {stars[i].color,stars[i].color,stars[i].color,stars[i].color};
					ci.mesh.colors = colors;

					starQuad.Add(ci);

				}
			}


			Mesh m = new Mesh();
			m.name = "SkyStartsMesh";
			m.CombineMeshes(starQuad.ToArray());
			m.Optimize();
			// over size mesh bounds to avoid camera frustum culling for Vertex transformation in shader 
			m.bounds = new Bounds ( Vector3.zero, Vector3.one * 2e9f); // less than 2,147,483,648
//			m.hideFlags = HideFlags.DontSave;
			return m;
		}

		protected Mesh createQuad (float size){

			Vector3[] Vertices = 
			{
				new Vector3( 1, 1, 0) * size,
				new Vector3(-1, 1, 0) * size,
				new Vector3( 1,-1, 0) * size,
				new Vector3(-1,-1, 0) * size
			};

			Vector2[] uv = 
			{ 
				new Vector2(0, 1), 
				new Vector2(1, 1),
				new Vector2(0, 0),
				new Vector2(1, 0)
			};

			int[] triangles = new int[6]
			{
				0, 2, 1,
				2, 3, 1
			};

			Mesh m = new Mesh();
			
			m.vertices = Vertices;
			m.uv = uv;
			m.triangles = triangles;
			m.RecalculateNormals();
			//m.hideFlags = HideFlags.DontSave;
			return m;
		}

		private Matrix4x4 BillboardMatrix (Vector3 particlePosition)
		{
			Vector3 direction = particlePosition - Vector3.zero;
			direction.Normalize();
			
			Vector3 particleRight = Vector3.Cross(direction, Vector3.up);
			particleRight.Normalize();
			
			Vector3 particleUp = Vector3.Cross(particleRight, direction);
			
			Matrix4x4 matrix = new Matrix4x4();

			matrix.SetColumn(0, particleRight);
			matrix.SetColumn(1, particleUp);
			matrix.SetColumn(2, direction);
			matrix.SetColumn(3, particlePosition);

			return matrix;
		}
	}
}
