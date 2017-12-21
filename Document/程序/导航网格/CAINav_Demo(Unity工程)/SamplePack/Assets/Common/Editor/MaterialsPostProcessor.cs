/*
 * Copyright (c) 2011 Stephen A. Pratt
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
using UnityEngine;
using UnityEditor;
using System.IO;

public class MaterialsPostProcessor 
    : AssetPostprocessor
{
    public Material OnAssignMaterialModel(Material material, Renderer renderer)
    {

        const string MatDirName = "Materials";
        const string RootDirName = "Assets";

        string basePath = Path.GetDirectoryName(assetPath);
        string matPath;

        int count = 0;

        while (true)
        {
            matPath = basePath + "/" + MatDirName;
            // Debug.Log("Base: " + basePath + ", MatPath: " + matPath);
            if (Directory.Exists(matPath))
                // Found a materials directory.
                break;
            if (Path.GetDirectoryName(basePath).EndsWith(RootDirName))
            {
                // There are no material directories anywhere in the path.
                matPath = basePath;
                break;
            }
            if (count > 20)
            {
                // Emergency exit.
                matPath = Path.GetDirectoryName(assetPath);
                break;
            }
            count++;
            basePath = basePath.Substring(0
                , basePath.LastIndexOf('/'));
        }

        matPath += Path.DirectorySeparatorChar 
            + material.name.Replace(':', '_') + ".mat";
        // Debug.Log(Path.GetDirectoryName(matPath));

        Material result = (Material)
            AssetDatabase.LoadAssetAtPath(matPath, typeof(Material));
        if (result == null)
        {
            material.shader = Shader.Find("Diffuse");
            AssetDatabase.CreateAsset(material, matPath);
            Debug.Log("New Material: " + matPath);
            return material;
        }

        return result;
    }
}