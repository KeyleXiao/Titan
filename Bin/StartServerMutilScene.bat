echo off

start /min "�ر�" CloseAll.bat
ping 1 -n 3 -w 2000 2>nul 1>nul

rem ---------------------- ������������ ----------------------

start /min "�ӷ�����"  ./Manager/ManagerServer.exe
ping 1 -n 3 -w 500 2>nul 1>nul
start /min "��������" ./Manager/RootManagerServer.exe
ping 1 -n 3 -w 5000 2>nul 1>nul
start /min "��������" ./Manager/MainManagerServer.exe

rem ping 1 -n 3 -w 1000 2>nul 1>nul
rem start /min "�ͻ���" ./Manager/ManagerClient.exe

ping 1 -n 3 -w 500 2>nul 1>nul
start /min "��������"  ./Cluster/BridgeServer.exe

rem ---------------------- ��������Ⱥ ----------------------

start /min "�罻������"  ./Cluster/SNSServer.exe
start /min "����������"  ./Cluster/VoiceServer.exe
start /min "��������"  ./Cluster/VoiceGateway.exe
start "�໤������"  ./Cluster/GuardServer.exe
start "ʱ�������"  ./Cluster/TimeSync.exe
start /min "���ķ�����"  ./Cluster/CenterServer.exe
ping 1 -n 3 -w 1000 2>nul 1>nul
start /min "���ط�����"  ./Cluster/GatewayServer.exe
ping 1 -n 3 -w 1000 2>nul 1>nul
start /min "��½������"  ./Cluster/LoginServer.exe
ping 1 -n 3 -w 500 2>nul 1>nul

rem ---------------------- ���� ----------------------
start "����" ./Social/SocialServer.exe

rem ---------------------- ���������� ----------------------
start "����������" ./Server/SceneServer.exe  "sceneserver0  auth dev g "
ping 1 -n 5 -w 5000 2>nul 1>nul
start "����������" ./Server/SceneServer.exe  "sceneserver1  auth dev g "
ping 1 -n 5 -w 5000 2>nul 1>nul
start "����������" ./Server/SceneServer.exe  "sceneserver2  auth dev g "
ping 1 -n 5 -w 5000 2>nul 1>nul
start "����������" ./Server/SceneServer.exe  "sceneserver3  auth dev g "
rem --ping 1 -n 5 -w 5000 2>nul 1>nul
rem --start "����������" ./Server/SceneServer.exe  "sceneserver4  auth dev g "


exit