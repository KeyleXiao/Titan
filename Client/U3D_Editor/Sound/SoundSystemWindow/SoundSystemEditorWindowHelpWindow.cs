/*******************************************************************
** 文件名:	SoundSystemEditorWindowHelpWindow.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	声音系统
** 应  用:  	声音编辑器帮助窗口

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using System.IO;

namespace SoundSystem
{
    public partial class SoundSystemEditorWindowHelpWindow : EditorWindow
    {
        public static SoundSystemEditorWindowHelpWindow EditorInstance = null;

        public static void OpenSoundSystemEditorWindowHelpWindow()
        {
            EditorInstance = EditorWindow.GetWindow<SoundSystemEditorWindowHelpWindow>(false, "HelpWindow", true);
            EditorInstance.Show();
        }
        Vector2 scrollview = Vector2.zero;
        public void OnGUI()
        {
            scrollview = GUILayout.BeginScrollView(scrollview);

            GUILayout.Label("资源导入:");
            GUILayout.Label("1.资源只能放在Sounds目录下");
            GUILayout.Label("2.不能放在Sounds目录下的BootCampSound文件夹");
            GUILayout.Label("3.文件结构弄得好看一点，到时候资源包会以文件夹为单位进行打包");

            GUILayout.Space(18);
            GUILayout.Label("使用方法:");
            GUILayout.Label("1.点击<<新增配置>>按钮，新增一个配置，在中间上部会显示配置的信息");
            GUILayout.Label("2.点击右边的音效资源，资源信息会显示在中下部,可以点击下面的<<播放>>按钮试听这个资源，然后点击红色的按钮<<插入音效到配置>>，将这个音效插入到配置中");
            GUILayout.Label("3.重复以上两步，完成之后，点击<<导出配置>>按钮，将配置保存起来。");
            GUILayout.Label("4.数据输入，记得按回车键，才会真正的输入数据！！Unity问题，我也无解！！！");

            GUILayout.Space(18);
            GUILayout.Label("导出相关:");
            GUILayout.Label("1.如果配置中的资源为空,这个配置则不会导出!");
            GUILayout.Label("2.当资源都配置好了之后，打开64位Unity,点击Tools/AssetBundleEditor,点击构建完整版资源包,操作完成之后,");
            GUILayout.Label("XXX/Game/Asset/StreamingAssets/ResourcePackage/sound文件夹下的资源包!");

            GUILayout.Space(18);
            GUILayout.Label("上传相关:");
            GUILayout.Label("1.csv文件,现在导出的csv文件是放在对应的Game/Scp目录下的，读取也是读这里。上传的时候，要传上来，当然另一个scp目录一样要复制一个，然后传上来");
            GUILayout.Label("2.资源包目前是放在XXX/Game/Asset/StreamingAssets/ResourcePackage下，这里会有一个sound文件夹，导出的资源包放在这里，记得一起传上来。");
            GUILayout.Label("3.遇到两个人同时上传资源包的情况下，直接使用别人的，然后打开编辑器重新构建一次资源包，再传上去!");

            GUILayout.Space(18);
            GUILayout.Label("注意事项:");
            GUILayout.Label("1.关于在运行模式实时刷新配置的问题，当修改了配置之后，点击 刷新配置 到游戏，会将配置重新刷入到游戏中，目前是全部刷入。");
            GUILayout.Label("2.刷新资源列表，导入新的资源或者移动资源，点击这个按钮可以将新的资源导入的编辑器中，一般修改后会自动刷新。<<如无特殊情况，尽量别移动资源>>");

            Color old = GUI.color;
            GUI.color = Color.red;
            GUILayout.Label("3.一般移动资源，会导致配置中的资源引用丢失，导出配置单的时候会有提示，这个时候，请点击<<匹配资源>>按钮进行资源匹配，随着资源和配置的增多，匹配过程会越来越长,");
            GUILayout.Label("所以，如无特殊情况，尽量别移动资源,匹配失败的资源会输出在控制台!!");
            GUI.color = old;


            GUILayout.EndScrollView();

        }
    }
}
