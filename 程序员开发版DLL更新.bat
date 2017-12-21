echo 更新Dll
pause
set TARGET_DIR=\\172.16.0.198\SpeedBin

echo 更新serverlist.xml
copy %TARGET_DIR%\serverlist.xml Bin\serverlist.xml

echo 更新管理器
copy %TARGET_DIR%\Manager\*.exe Bin\Manager\.
copy %TARGET_DIR%\Manager\*.dll Bin\Manager\.
copy %TARGET_DIR%\Manager\*.pdb Bin\Manager\.


echo 更新场景服
copy %TARGET_DIR%\Server\*.exe Bin\Server\.
copy %TARGET_DIR%\Server\*.dll Bin\Server\.
copy %TARGET_DIR%\Server\*.pdb Bin\Server\.

echo 更新社会服
copy %TARGET_DIR%\Social\*.exe Bin\Social\.
copy %TARGET_DIR%\Social\*.dll Bin\Social\.
copy %TARGET_DIR%\Social\*.pdb Bin\Social\.

echo 更新服务器集群
copy %TARGET_DIR%\Cluster\*.exe Bin\Cluster\.
copy %TARGET_DIR%\Cluster\*.dll Bin\Cluster\.
copy %TARGET_DIR%\Cluster\*.pdb Bin\Cluster\.

echo 更新世界服务器
copy %TARGET_DIR%\WorldServer\*.* Bin\WorldServer\.


echo 更新工具
copy %TARGET_DIR%\Tools\*.exe Bin\Tools\.
copy %TARGET_DIR%\Tools\*.dll Bin\Tools\.
copy %TARGET_DIR%\Tools\*.pdb Bin\Tools\.

echo 更新客户端
copy %TARGET_DIR%\Client\bin\*.dll			Bin\Client\Bin\.
copy %TARGET_DIR%\Client\bin\*.pdb			Bin\Client\Bin\.
copy %TARGET_DIR%\Client\bin\*.dll			Bin\Client\Game\.
copy %TARGET_DIR%\Client\bin\*.pdb			Bin\Client\Game\.
copy %TARGET_DIR%\Server\Framework.dll			Bin\Client\Bin\.
copy %TARGET_DIR%\Server\Framework.pdb			Bin\Client\Bin\.
copy %TARGET_DIR%\Server\Framework.dll			Bin\Client\Game\.
copy %TARGET_DIR%\Server\Framework.pdb			Bin\Client\Game\.

echo 更新WEB相关文件
set WebFolder=Bin\Client\Bin\PluginServer\
if not exist %WebFolder% md %WebFolder%
set WebLanguageFolder=Bin\Client\Bin\PluginServer\locales\
if not exist %WebLanguageFolder% md %WebLanguageFolder%

copy %TARGET_DIR%\Client\Bin\PluginServer\*.*		Bin\Client\Bin\PluginServer\.
copy %TARGET_DIR%\Client\Bin\PluginServer\locales\*.*	Bin\Client\Bin\PluginServer\locales\.

copy %TARGET_DIR%\Client\bin\log4net.dll		Bin\Client\Bin\PluginServer\.
copy %TARGET_DIR%\Client\bin\log4net.pdb		Bin\Client\Bin\PluginServer\.
copy %TARGET_DIR%\Client\bin\MessageLibrary.dll		Bin\Client\Bin\PluginServer\.
copy %TARGET_DIR%\Client\bin\MessageLibrary.pdb		Bin\Client\Bin\PluginServer\.
copy %TARGET_DIR%\Client\bin\SharedMemory.dll		Bin\Client\Bin\PluginServer\.
copy %TARGET_DIR%\Client\bin\SharedMemory.pdb		Bin\Client\Bin\PluginServer\.
copy %TARGET_DIR%\Client\bin\Xilium.CefGlue.dll		Bin\Client\Bin\PluginServer\.
copy %TARGET_DIR%\Client\bin\Xilium.CefGlue.pdb		Bin\Client\Bin\PluginServer\.
copy %TARGET_DIR%\Client\bin\SharedPluginServer.exe	Bin\Client\Bin\PluginServer\.
copy %TARGET_DIR%\Client\bin\SharedPluginServer.pdb	Bin\Client\Bin\PluginServer\.
copy %TARGET_DIR%\Client\bin\MessageLibrary.dll		Bin\Client\Game\Assets\Scripts\.
copy %TARGET_DIR%\Client\bin\MessageLibrary.pdb		Bin\Client\Game\Assets\Scripts\.
copy %TARGET_DIR%\Client\bin\SharedMemory_unity.dll	Bin\Client\Game\Assets\Scripts\.
copy %TARGET_DIR%\Client\bin\SharedMemory_unity.pdb	Bin\Client\Game\Assets\Scripts\.

del Bin\Client\Game\Assets\Plugins\MessageLibrary.dll
del Bin\Client\Game\Assets\Plugins\MessageLibrary.pdb
del Bin\Client\Game\Assets\Plugins\SharedMemory_unity.dll
del Bin\Client\Game\Assets\Plugins\SharedMemory_unity.pdb

del Bin\Client\Bin\log4net.dll
del Bin\Client\Bin\log4net.pdb
del Bin\Client\Bin\SharedMemory.dll
del Bin\Client\Bin\SharedMemory.pdb
del Bin\Client\Bin\Xilium.CefGlue.dll
del Bin\Client\Bin\Xilium.CefGlue.pdb
del Bin\Client\Bin\SharedPluginServer.exe
del Bin\Client\Bin\SharedPluginServer.pdb

del Bin\Client\Game\log4net.dll
del Bin\Client\Game\log4net.pdb
del Bin\Client\Game\MessageLibrary.dll
del Bin\Client\Game\MessageLibrary.pdb
del Bin\Client\Game\SharedMemory.dll
del Bin\Client\Game\SharedMemory.pdb
del Bin\Client\Game\Xilium.CefGlue.dll
del Bin\Client\Game\Xilium.CefGlue.pdb
del Bin\Client\Game\SharedPluginServer.exe
del Bin\Client\Game\SharedPluginServer.pdb
del Bin\Client\Game\SharedMemory_unity.dll
del Bin\Client\Game\SharedMemory_unity.pdb


del Bin\Client\Game\GameAPI_Wrapper.dll
del Bin\Client\Game\U3D_Render.dll
del Bin\Client\Game\U3D_Editor.dll
del Bin\Client\Game\U3D_ImageEffect.dll
del Bin\Client\Game\U3D_StandardAssets.dll
del Bin\Client\Game\U3D_UIFramework.dll
del Bin\Client\Game\U3D_SceneRender.dll
del Bin\Client\Game\U3D_Common.dll

copy %TARGET_DIR%\Client\bin\GameAPI_Wrapper.dll	Bin\Client\Game\Assets\Scripts\.
copy %TARGET_DIR%\Client\bin\U3D_Render.dll		Bin\Client\Game\Assets\Scripts\.
copy %TARGET_DIR%\Client\bin\U3D_ImageEffect.dll	Bin\Client\Game\Assets\Scripts\.
copy %TARGET_DIR%\Client\bin\U3D_UIFramework.dll	Bin\Client\Game\Assets\Scripts\.
copy %TARGET_DIR%\Client\bin\U3D_SceneRender.dll 	Bin\Client\Game\Assets\Scripts\.
copy %TARGET_DIR%\Client\bin\U3D_Common.dll 		Bin\Client\Game\Assets\Scripts\.
copy %TARGET_DIR%\Client\bin\U3D_Editor.dll		Bin\Client\Game\Assets\Editor\.
copy %TARGET_DIR%\Client\bin\U3D_StandardAssets.dll	"Bin\Client\Game\Assets\Standard Assets\."

copy %TARGET_DIR%\Client\bin\avcodec-57.dll		Bin\Client\Game\.
copy %TARGET_DIR%\Client\bin\avdevice-57.dll		Bin\Client\Game\.
copy %TARGET_DIR%\Client\bin\avfilter-6.dll		Bin\Client\Game\.
copy %TARGET_DIR%\Client\bin\avformat-57.dll		Bin\Client\Game\.
copy %TARGET_DIR%\Client\bin\avutil-55.dll		Bin\Client\Game\.
copy %TARGET_DIR%\Client\bin\postproc-54.dll		Bin\Client\Game\.
copy %TARGET_DIR%\Client\bin\swresample-2.dll		Bin\Client\Game\.
copy %TARGET_DIR%\Client\bin\swscale-4.dll		Bin\Client\Game\.

echo 更新Lib
copy \\172.16.0.198\SpeedLib\*.*  Lib\.

pause