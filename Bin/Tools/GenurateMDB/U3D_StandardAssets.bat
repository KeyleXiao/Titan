rem 项目的Assets路径
set ProjectDir=d:\project\speedex\Bin\Client\Game\Assets
rem Unity安装目录
set UnityDir="D:\Program Files (x86)\Unity5\Editor\Data\MonoBleedingEdge"


%UnityDir%\bin\cli.bat %UnityDir%\lib\mono\4.5\pdb2mdb.exe %ProjectDir%\Standard Assets\U3D_StandardAssets.dll


exit

