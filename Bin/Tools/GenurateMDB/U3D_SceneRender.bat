rem ��Ŀ��Assets·��
set ProjectDir="F:\Speed\Bin\Client\Game\Assets"
rem Unity��װĿ¼
set UnityDir="E:\Unity5.4.2p4-32bit\Unity\Editor\Data\MonoBleedingEdge"

%UnityDir%\bin\cli.bat %UnityDir%\lib\mono\4.5\pdb2mdb.exe %ProjectDir%\Scripts\U3D_SceneRender.dll

exit
