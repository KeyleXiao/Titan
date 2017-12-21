using UnityEngine;  
using System.Collections;  
using System.Collections.Generic;
using UnityEditor;  
using System.IO;




public class CustomExporter : Editor  
{


	public static bool CheckPlatform(BuildTarget target)
	{
		if (EditorUserBuildSettings.activeBuildTarget != target)
		{
			string title="Target Platform is different from Current Platform, Please Switch the Platform First";
			string msg="Current Platform:"+EditorUserBuildSettings.activeBuildTarget.ToString()+ "Target Platform:" + target.ToString();
			EditorUtility.DisplayDialog(title,msg, "OK");
			return false;
		}
		return true;
	}

	private static string[] GetPathsFromAssets(Object[] assets)
	{
		string[] SelectedAssetPaths=new string[assets.Length];
		
		//generate paths
		for(int i=0;i<assets.Length;i++)   
		{  
			string path=AssetDatabase.GetAssetPath(assets[i]);
            path = path.Replace("Assets/" + OrgFolder + "/", "");
			string dir=System.IO.Path.GetDirectoryName(path);
			string filename=System.IO.Path.GetFileNameWithoutExtension(path);
			path=dir+"/"+filename;
			SelectedAssetPaths[i]=path;
		}   

		return SelectedAssetPaths;
	}

	private static string[] ForbidenExtensions = {".meta",".exr",".asset"};
	private static bool IsForbidenExtension(string path) 
	{
		string fileExt = Path.GetExtension(path).ToLower();
		foreach (string ForbidenExtension in ForbidenExtensions) 
		{
			if (fileExt==ForbidenExtension)
			{
				return true;
			}
		}
		return false;
	}
	
	private static bool IsDirectory(string path)
	{
		System.IO.FileAttributes fa = System.IO.File.GetAttributes(path);
		bool isDirectory = false;
		if ((fa & FileAttributes.Directory) != 0)
		{
			isDirectory = true;
		}
		return isDirectory;
	}
	
	private static string GetFullPath(string relPath)
	{
		string dataPath = Application.dataPath; 
		string fullPath = Path.Combine(dataPath.Substring(0, dataPath.Length - 6), relPath);
		return fullPath;
	}
	
	private static List<string> assetpathlist = new List<string> ();
	private static void AddFilesFromDir(string path)
	{
		string dataPath = Application.dataPath; 
		string fullPath = GetFullPath(path);
		
		if(assetpathlist.Contains(path)) 
		{
			return;
		}
		
		if(!IsDirectory(fullPath)) 
		{
			assetpathlist.Add(path);
			return;
		}
		
		//get the system file paths of all the files in the asset folder
		string[] filePaths = Directory.GetFiles(fullPath);
		
		// enumerate through the list of files loading the assets they represent and getting their type
		foreach (string filepath in filePaths) 
		{
			if(IsForbidenExtension(filepath)) {
				continue;
			}
			
			if(assetpathlist.Contains(filepath)) 
			{
				continue;
			}
			
			string relPath = filepath.Substring(dataPath.Length-6);
			assetpathlist.Add(relPath);
		}
		
		string[] dirPaths = Directory.GetDirectories (fullPath);
		foreach (string dirpath in dirPaths) 
		{
			string relPath = dirpath.Substring(dataPath.Length-6);
			AddFilesFromDir(relPath);
		}
	}

	private static Object[] RecursionGetAllObjectsFromPath(string path)
	{
		assetpathlist.Clear ();
		AddFilesFromDir (path);
		List<Object> objList = new List<Object>();
		foreach (string assetpath in assetpathlist) 
		{
			Object obj = AssetDatabase.LoadMainAssetAtPath(assetpath);
			if(obj != null)
			{
				objList.Add(obj);
			}
		}
		return objList.ToArray();
	}



	//打包资源
	[MenuItem("Build/Android/Export Package")]  
	static void CreateAssetBunldesAndroidEach ()  
	{  
		if (!CheckPlatform (BuildTarget.Android)) {
			return;
		}
		
		CreateAssetBunldesEachByEach ("Android",BuildTarget.Android);
	}
	
	//generate resource list
	[MenuItem("Build/Android/Generate List for All Packages")]  
	static void GenerateListFromAssetBunldesAndroid()  
	{  
		GenerateListFromAssetBunldesPlatform ("Android");
	}  

//	//convert resource list
//	[MenuItem("Build/Android/Convert Atom List To Zen List")]  
//	static void ConvertAtomListToZenListAndroid()  
//	{  
//		ConvertAtomListToZenListPlatform ("Android");
//	}  


	

	[MenuItem("Build/Iphone/Export Package")]  
	static void CreateAssetBunldesIphoneEach ()  
	{  
		if (!CheckPlatform (BuildTarget.iOS)) {
			return;
		}
		
		CreateAssetBunldesEachByEach ("Iphone",BuildTarget.iOS);
	}

	//generate resource list
	[MenuItem("Build/Iphone/Generate List for All Packages")]  
	static void GenerateListFromAssetBunldesIphone()  
	{  
		GenerateListFromAssetBunldesPlatform ("Iphone");
	}  

//	//convert resource list
//	[MenuItem("Build/Iphone/Convert Atom List To Zen List")]  
//	static void ConvertAtomListToZenListIphone()  
//	{  
//		ConvertAtomListToZenListPlatform ("Iphone");
//	}  


	//打包资源
	[MenuItem("Build/WebPlayer/Export Package")]  
	static void CreateAssetBunldesWebPlayerEach ()  
	{  
		if (EditorUserBuildSettings.activeBuildTarget != BuildTarget.StandaloneWindows &&
		    EditorUserBuildSettings.activeBuildTarget != BuildTarget.StandaloneWindows64 &&
		    EditorUserBuildSettings.activeBuildTarget != BuildTarget.WebPlayer &&
		    EditorUserBuildSettings.activeBuildTarget != BuildTarget.WebPlayerStreamed &&
		    EditorUserBuildSettings.activeBuildTarget != BuildTarget.StandaloneOSXIntel &&
		    EditorUserBuildSettings.activeBuildTarget != BuildTarget.StandaloneOSXIntel64 &&
		    EditorUserBuildSettings.activeBuildTarget != BuildTarget.StandaloneOSXUniversal 
		    )
			return;
		
		CreateAssetBunldesEachByEach ("web",BuildTarget.StandaloneWindows);
	}
	
	//generate resource list
	[MenuItem("Build/WebPlayer/Generate List for All Packages")]  
	static void GenerateListFromAssetBunldesWebPlayer()  
	{  
		GenerateListFromAssetBunldesPlatform ("web");
	}

    static string[] packexts = new string[] { "*.prefab", "*.unity" };
    static string OrgFolder = "Artist";
	static void CreateAssetBunldesEachByEach (string strplatform,BuildTarget platform)  
	{  
		Caching.CleanCache ();  

		string SrcListFile = Application.dataPath + "/Editor/ExportList.txt";
		if (!System.IO.File.Exists (SrcListFile)) {
			return;
		}

		string[] SrcPaths = System.IO.File.ReadAllLines (SrcListFile);
        List<string> SrcSubRelDirectories = new List<string>();
        List<string> SrcSubRelFiles = new List<string> ();
		foreach (string SrcPath in SrcPaths)
        {
            {
                string[] SrcSubDirFullPaths = System.IO.Directory.GetDirectories(Application.dataPath + "/" + OrgFolder + "/" + SrcPath);
                foreach (string SrcSubFullPath in SrcSubDirFullPaths)
                {
                    string SrcSubName = System.IO.Path.GetFileName(SrcSubFullPath);
                    string SrcSubRelPath = SrcPath + "/" + SrcSubName;
                    SrcSubRelPath = SrcSubRelPath.Replace("\\", "/");
                    SrcSubRelDirectories.Add(SrcSubRelPath);
                }
            }
            foreach(string packext in packexts)
            {
                string[] SrcSubFileFullPaths = System.IO.Directory.GetFiles(Application.dataPath + "/" + OrgFolder + "/" + SrcPath, packext);
                foreach (string SrcSubFullPath in SrcSubFileFullPaths)
                {
                    string SrcSubName = System.IO.Path.GetFileName(SrcSubFullPath);
                    string SrcSubRelPath = SrcPath + "/" + SrcSubName;
                    SrcSubRelPath = SrcSubRelPath.Replace("\\", "/");
                    SrcSubRelFiles.Add(SrcSubRelPath);
                }
            }
        }

		int nCount = 0;
        foreach (string SrcSubRelPath in SrcSubRelDirectories) 
		{
				string[] SrcSubRelPathSplits= SrcSubRelPath.Split('/');
				
				string DestPackName="";
				for(int i=0;i<SrcSubRelPathSplits.Length-1;i++)
				{
					DestPackName+=SrcSubRelPathSplits[i]+"_";
				}
				if (SrcSubRelPathSplits.Length-1>=0)
				{
					DestPackName+=SrcSubRelPathSplits[SrcSubRelPathSplits.Length-1];
				}
				string DestPackagePath = Application.dataPath +  "/AssetBundles/"+strplatform+"/"+DestPackName+".unity3d";  
				string DestListPath = Application.dataPath +  "/AssetBundles/"+strplatform+"/"+DestPackName+".txt";

                string SrcSubFullPath = Application.dataPath + "/" + OrgFolder + "/" + SrcSubRelPath;
				
				Object[] SelectedAsset = RecursionGetAllObjectsFromPath(SrcSubFullPath);
				string[] SelectedAssetPaths = GetPathsFromAssets (SelectedAsset);

				if (SelectedAsset==null || SelectedAssetPaths==null ||
					SelectedAsset.Length<=0 || SelectedAssetPaths.Length<=0)
				{
					continue;
				}
									
				StreamWriter sw = new StreamWriter(DestListPath,false);
				sw.WriteLine ("#,Path,Type,Package");
				for(int i=0;i<SelectedAsset.Length;i++) 
				{  
					Object obj=SelectedAsset[i];
					string path=SelectedAssetPaths[i];
					sw.WriteLine(i.ToString()+","+path+","+obj.GetType().ToString()+","+DestPackName);
				}   
				sw.Close ();
									
				if (BuildPipeline.BuildAssetBundleExplicitAssetNames (SelectedAsset,
									                                  SelectedAssetPaths,
				                                                      DestPackagePath, 
									                                  BuildAssetBundleOptions.CollectDependencies| BuildAssetBundleOptions.CompleteAssets,
									                                  platform)) 
				{  
					Debug.Log("Generate Asset Bundle OK:"+DestPackagePath);  
					AssetDatabase.Refresh ();  
				}
				else
				{  
					Debug.Log("Generate Asset Bundle Fail:"+DestPackagePath);  
				}

				System.GC.Collect();
				nCount++;

//				float fProgress=(float)nCount/(float)(SrcSubRelPaths.Count);
//				EditorUtility.DisplayProgressBar("Generate Asset Bundles Each by Each","Generate Asset Bundle:"+DestPackName+"...",fProgress);
		}


        foreach (string SrcSubRelPath in SrcSubRelFiles)
        {

            string[] SrcSubRelPathSplits = SrcSubRelPath.Split('/');

            string DestPackName = "";
            for (int i = 0; i < SrcSubRelPathSplits.Length - 1; i++)
            {
                DestPackName += SrcSubRelPathSplits[i] + "_";
            }
            if (SrcSubRelPathSplits.Length - 1 >= 0)
            {
                DestPackName += Path.GetFileNameWithoutExtension(SrcSubRelPathSplits[SrcSubRelPathSplits.Length - 1]);
            }

            string DestPackagePath = Application.dataPath + "/AssetBundles/" + strplatform + "/" + DestPackName + ".unity3d";
            string DestListPath = Application.dataPath + "/AssetBundles/" + strplatform + "/" + DestPackName + ".txt";

            string SrcSubFullPath = Application.dataPath + "/" + OrgFolder + "/" + SrcSubRelPath;
            if (File.Exists(SrcSubFullPath) == false) { continue; };

            string srcName = Path.GetFileNameWithoutExtension(SrcSubRelPath);
            string ext = Path.GetExtension(SrcSubRelPath).ToLower();
            if (ext == ".unity")
            {
                StreamWriter sw = new StreamWriter(DestListPath, false);
                sw.WriteLine("#,Path,Type,Package");
                sw.WriteLine("0" + "," + srcName + "," + "scene" + "," + DestPackName);
                sw.Close();

                bool ret1 = UnityEditor.SceneManagement.EditorSceneManager.OpenScene(SrcSubFullPath) != null;
                EditorApplication.Step();
                string TempFullLevel1 = Path.GetDirectoryName(SrcSubFullPath) + "/" + Path.GetFileNameWithoutExtension(SrcSubFullPath) + ".temp";
                File.Copy(SrcSubFullPath, TempFullLevel1, true);
                bool ret2 = UnityEditor.SceneManagement.EditorSceneManager.OpenScene(TempFullLevel1) != null;

                while (true)
                {
                    EditorApplication.Step();
                    if (Path.GetFileNameWithoutExtension(UnityEditor.SceneManagement.EditorSceneManager.GetActiveScene().name) == Path.GetFileNameWithoutExtension(SrcSubFullPath))
                    {
                        break;
                    }
                }
                EditorApplication.Step();

                string scenepath = UnityEditor.SceneManagement.EditorSceneManager.GetActiveScene().name;
                string fullscenepath = Path.GetFullPath(scenepath);
                string absfolder = Path.GetDirectoryName(fullscenepath) + "/" + Path.GetFileNameWithoutExtension(fullscenepath)+"_temp";
                string folder = Path.GetDirectoryName(scenepath) + "/" + Path.GetFileNameWithoutExtension(scenepath) + "_temp";
                if (Directory.Exists(absfolder))
                {
                    Directory.Delete(absfolder, true);
                }
                Directory.CreateDirectory(absfolder);

                AdorningExporterEx.ExportFun(true, BuildTarget.StandaloneWindows);
                AdorningExporterEx.Clear();
                bool ret3 = UnityEditor.SceneManagement.EditorSceneManager.SaveScene(UnityEditor.SceneManagement.EditorSceneManager.GetActiveScene());
                EditorApplication.Step();

                string TempFullLevel2 = absfolder + "/" + Path.GetFileNameWithoutExtension(SrcSubFullPath) + ".unity";
                File.Copy(TempFullLevel1, TempFullLevel2, true);

                string[] levels = new string[] { TempFullLevel2 };
                string scenepkgpath = absfolder + "/scene.scenepkg";
                string msg = BuildPipeline.BuildStreamedSceneAssetBundle(levels, scenepkgpath, platform);
                {
                    Debug.Log("Generate Scene Pkg OK:" + scenepkgpath);
                }

                bool ret4 = UnityEditor.SceneManagement.EditorSceneManager.OpenScene(SrcSubFullPath) != null;
                EditorApplication.Step();
                File.Delete(TempFullLevel2);
                File.Delete(TempFullLevel1);

                //合成一个我们的包
                FileStream fs = new FileStream(DestPackagePath, FileMode.OpenOrCreate);
                BinaryWriter bw = new BinaryWriter(fs);

                string[] scenepkgs=Directory.GetFiles(absfolder, "*.scenepkg");
                bw.Write(scenepkgs.Length);
                foreach (string scenepkg in scenepkgs)
                {
                    string filename = Path.GetFileNameWithoutExtension(scenepkg);
                    bw.Write(filename);
                    byte[] data = File.ReadAllBytes(scenepkg);
                    bw.Write(data.Length);
                    bw.Write(data);
                    bw.Flush();
                }

                string[] adorningpkgs = Directory.GetFiles(absfolder, "*.adorningpkg");
                bw.Write(adorningpkgs.Length);
                foreach (string adorningpkg in adorningpkgs)
                {
                    string filename = Path.GetFileNameWithoutExtension(adorningpkg);
                    bw.Write(filename);
                    byte[] data = File.ReadAllBytes(adorningpkg);
                    bw.Write(data.Length);
                    bw.Write(data);
                    bw.Flush();
                }

                string[] matpkgs = Directory.GetFiles(absfolder, "*.matpkg");
                bw.Write(matpkgs.Length);
                foreach (string matpkg in matpkgs)
                {
                    string filename = Path.GetFileNameWithoutExtension(matpkg);
                    bw.Write(filename);
                    byte[] data = File.ReadAllBytes(matpkg);
                    bw.Write(data.Length);
                    bw.Write(data);
                    bw.Flush();
                }

                string[] meshpkgs = Directory.GetFiles(absfolder, "*.meshpkg");
                bw.Write(meshpkgs.Length);
                foreach (string meshpkg in meshpkgs)
                {
                    string filename = Path.GetFileNameWithoutExtension(meshpkg);
                    bw.Write(filename);
                    byte[] data = File.ReadAllBytes(meshpkg);
                    bw.Write(data.Length);
                    bw.Write(data);
                    bw.Flush();
                }

                bw.Close();
                fs.Close();
                fs.Dispose();

                if (Directory.Exists(absfolder))
                {
                    Directory.Delete(absfolder, true);
                }

                AssetDatabase.Refresh();
                EditorApplication.Step();

                Debug.Log("Generate Asset Bundle OK:" + DestPackagePath);
            }
            else
            {
                Object srcAssetObj = null;
                Object[] srcFilterAssets = AssetDatabase.LoadAllAssetsAtPath(SrcSubFullPath);
                foreach (Object o in srcFilterAssets)
                {
                    if (o.name.ToLower() == srcName.ToLower() && o.GetType() == typeof(GameObject))
                    {
                        srcAssetObj = o;
                        break;
                    }
                }
                if (srcAssetObj == null) { continue; };
                string srcAssetPath = Path.GetDirectoryName(SrcSubRelPath) + "/" + Path.GetFileNameWithoutExtension(SrcSubRelPath);

                Object[] SelectedAsset = new Object[] { srcAssetObj };
                string[] SelectedAssetPaths = new string[] { srcAssetPath };

                if (SelectedAsset == null || SelectedAssetPaths == null ||
                    SelectedAsset.Length <= 0 || SelectedAssetPaths.Length <= 0)
                {
                    continue;
                }

                StreamWriter sw = new StreamWriter(DestListPath, false);
                sw.WriteLine("#,Path,Type,Package");
                for (int i = 0; i < SelectedAsset.Length; i++)
                {
                    Object obj = SelectedAsset[i];
                    string path = SelectedAssetPaths[i];
                    sw.WriteLine(i.ToString() + "," + path + "," + obj.GetType().ToString() + "," + DestPackName);
                }
                sw.Close();


                if (BuildPipeline.BuildAssetBundleExplicitAssetNames(SelectedAsset,
                                                                      SelectedAssetPaths,
                                                                      DestPackagePath,
                                                                      BuildAssetBundleOptions.CollectDependencies | BuildAssetBundleOptions.CompleteAssets,
                                                                      platform))
                {
                    Debug.Log("Generate Asset Bundle OK:" + DestPackagePath);
                    AssetDatabase.Refresh();
                }
                else
                {
                    Debug.Log("Generate Asset Bundle Fail:" + DestPackagePath);
                }
            }

            System.GC.Collect();
            nCount++;

            //				float fProgress=(float)nCount/(float)(SrcSubRelPaths.Count);
            //				EditorUtility.DisplayProgressBar("Generate Asset Bundles Each by Each","Generate Asset Bundle:"+DestPackName+"...",fProgress);
        }

		//EditorUtility.ClearProgressBar ();
	} 



	static bool bEncrypt=false;

	static void GenerateListFromAssetBunldesPlatform(string platform)  
	{  
		Caching.CleanCache ();  

		//generate resource list
		{
			string ListPath = Application.dataPath +  "/AssetBundles/"+platform+"/list.csv";  
			StreamWriter sw = new StreamWriter(ListPath,false);
			sw.WriteLine ("#,Path,Type,Package");
			
			string[] strPackagePaths=System.IO.Directory.GetFiles (Application.dataPath + "/AssetBundles/"+platform,"*.txt");
			for(int j=0;j<strPackagePaths.Length;j++)
			{
				string strPackagePath=strPackagePaths[j];
				string packname=System.IO.Path.GetFileNameWithoutExtension(strPackagePath);
				
				string[] lines = System.IO.File.ReadAllLines(strPackagePath);
				
				if (lines!=null && lines.Length>1)
				{
					for(int i=1;i<lines.Length;i++) 
					{  
						string szline=lines[i];
						string[] cels = szline.Split ("," [0]);
						cels[1]=cels[1].ToLower();
						string szfinal="";
						for(int k=0;k<cels.Length;k++)
						{
							szfinal+=cels[k];
							if (k!=cels.Length-1)
							{
								szfinal+=",";
							}
						}
						sw.WriteLine(szfinal);
					}   
				}
				
				sw.Flush();
				
				float fProgress=(float)j/(float)(strPackagePaths.Length);
				EditorUtility.DisplayProgressBar("Generate Resource List","generate list for:"+strPackagePath+"...",fProgress);
			}
			
			sw.Close ();	
		}


		//generate package list
		{
			string ListPath = Application.dataPath +  "/AssetBundles/"+platform+"/package.csv";  

			Hashtable oldversions=new Hashtable();
			if (System.IO.File.Exists(ListPath))
			{
				string oldpackagecfgtext=System.IO.File.ReadAllText(ListPath);
				ScpReader oldpackagecfg=new ScpReader(oldpackagecfgtext,"oldpackagelist");
				int n=oldpackagecfg.GetRecordCount();
				for(int j=0;j<n;j++)
				{
					string packname=oldpackagecfg.GetString(j,1,"");
					int ver=oldpackagecfg.GetInt(j,2,0);
					if (packname.Length>0 && ver>0)
					{
						oldversions[packname]=ver;
					}
				}
				oldpackagecfg.Dispose();
			}

			StreamWriter sw = new StreamWriter(ListPath,false);
			sw.WriteLine ("#,name,version,filelength,datatimevalue,datatimestring");
			
			string[] strPackagePaths=System.IO.Directory.GetFiles (Application.dataPath + "/AssetBundles/"+platform,"*.txt");
			for(int j=0;j<strPackagePaths.Length;j++)
			{
				string strTextPath=strPackagePaths[j];
				string packname=Path.GetFileNameWithoutExtension(strTextPath);
				string dir=Path.GetDirectoryName(strTextPath);
				string strPackagePath=dir+"/"+packname+".unity3d";
				string strEncryptPath=dir+"/"+packname+".zen";
				if (!File.Exists(strPackagePath))
				{
					continue;
				}
				long filelength=0;
				System.DateTime lasttime;
				if (bEncrypt)
				{
					byte[] inputdata=File.ReadAllBytes(strPackagePath);
					if (inputdata==null)
					{
						continue;
					}
					string key=packname+"*^$@&%#!";
					byte[] outputdata=AssetSystem.EncryptDES(inputdata,key);
					if (outputdata==null)
					{
						continue;
					}
					File.WriteAllBytes(strEncryptPath,outputdata);
					byte[] testdata=File.ReadAllBytes(strEncryptPath);
					filelength=testdata.Length;
					if (filelength<=0 || filelength!=outputdata.Length)
					{
						continue;
					}
					lasttime=File.GetLastWriteTime(strEncryptPath);
				}
				else
				{
					byte[] inputdata=File.ReadAllBytes(strPackagePath);
					if (inputdata==null)
					{
						continue;
					}
					filelength=inputdata.Length;
					if (filelength<=0)
					{
						continue;
					}
					lasttime=File.GetLastWriteTime(strPackagePath);
				}

				int ver=1;
				if (oldversions.Contains(packname))
				{
					ver=(int)oldversions[packname];
				}
				if (ver<=0)
				{
					continue;
				}
				sw.WriteLine(j.ToString()+","+packname+","+ver.ToString()+","+filelength.ToString()+","+lasttime.ToFileTime().ToString()+","+lasttime.ToString());
				sw.Flush();
				
				float fProgress=(float)j/(float)(strPackagePaths.Length);
				EditorUtility.DisplayProgressBar("Generate package List","generate list for:"+strPackagePath+"...",fProgress);
			}
			
			sw.Close ();	
		}
		
		EditorUtility.ClearProgressBar();
	}

 
}  