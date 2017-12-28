echo off

start /min "关闭" CloseAll.bat
ping 1 -n 3 -w 2000 2>nul 1>nul

rem ---------------------- 服务器管理器 ----------------------

start /min "子服务器"  ./Manager/ManagerServer.exe
ping 1 -n 5 -w 500 2>nul 1>nul
start /min "根服务器" ./Manager/RootManagerServer.exe
ping 1 -n 5 -w 5000 2>nul 1>nul
start /min "主服务器" ./Manager/MainManagerServer.exe
ping 1 -n 5 -w 500 2>nul 1>nul

start /min "主服务器"  ./Cluster/BridgeServer.exe

rem ---------------------- 服务器集群 ----------------------

start /min "社交服务器"  ./Cluster/SNSServer.exe
start /min "语音服务器"  ./Cluster/VoiceServer.exe
start /min "语音网关"    ./Cluster/VoiceGateway.exe
start /min "监护服务器"  ./Cluster/GuardServer.exe

start "时间服务器"  ./Cluster/TimeSync.exe
start /min "中心服务器"  ./Cluster/CenterServer.exe
ping 1 -n 3 -w 1000 2>nul 1>nul
start /min "网关服务器"  ./Cluster/GatewayServer.exe
ping 1 -n 3 -w 1000 2>nul 1>nul
start /min "登陆服务器"  ./Cluster/LoginServer.exe
ping 1 -n 3 -w 500 2>nul 1>nul

rem ---------------------- 社会服 ----------------------
start "社会服" ./Social/SocialServer.exe
ping 3 -n 5 -w 1000 2>nul 1>nul

rem ---------------------- 场景服务器 ----------------------
start "场景服务器" ./Server/SceneServer.exe  "sceneserver0  auth dev g "
ping 3 -n 5 -w 1000 2>nul 1>nul


exit