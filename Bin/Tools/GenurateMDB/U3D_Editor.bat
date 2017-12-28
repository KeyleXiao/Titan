rem 项目的Assets路径
set ProjectDir=D:\project\speedex\Bin\Client\Game\Assets
rem Unity安装目录
set UnityDir="D:\Program Files (x86)\Unity5\Editor\Data\MonoBleedingEdge"


%UnityDir%\bin\cli.bat %UnityDir%\lib\mono\4.5\pdb2mdb.exe %ProjectDir%\Editor\U3D_Editor.dll

exit

