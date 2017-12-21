echo 更新Dll
pause
set TARGET_DIR=\\172.16.0.198\SpeedBin64

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

del Bin\Client\Game\GameAPI_Wrapper.dll
del Bin\Client\Game\U3D_Render.dll
del Bin\Client\Game\U3D_Editor.dll
del Bin\Client\Game\U3D_ImageEffect.dll
del Bin\Client\Game\U3D_StandardAssets.dll
del Bin\Client\Game\U3D_UIFramework.dll

copy %TARGET_DIR%\Client\bin\GameAPI_Wrapper.dll	Bin\Client\Game\Assets\Scripts\.
copy %TARGET_DIR%\Client\bin\U3D_Render.dll		Bin\Client\Game\Assets\Scripts\.
copy %TARGET_DIR%\Client\bin\U3D_ImageEffect.dll	Bin\Client\Game\Assets\Scripts\.
copy %TARGET_DIR%\Client\bin\U3D_UIFramework.dll	Bin\Client\Game\Assets\Scripts\.
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
copy \\172.16.0.198\SpeedLib64\*.*  Lib\.

pause