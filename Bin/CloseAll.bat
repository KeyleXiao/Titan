echo off

rem ---------------------- 服务器管理器 ----------------------

taskkill /f /im ManagerClient.exe
taskkill /f /im ManagerServer.exe
taskkill /f /im MainManagerServer.exe
taskkill /f /im RootManagerServer.exe
taskkill /f /im BridgeServer.exe

rem ---------------------- 服务器集群 ----------------------
taskkill /f /im CenterServer.exe
taskkill /f /im GatewayServer.exe
taskkill /f /im LoginServer.exe
taskkill /f /im TimeSync.exe
taskkill /f /im GuardServer.exe
rem ---------------------- 社会服 ----------------------
taskkill /f /im SocialServer.exe

rem ---------------------- 场景服务器 ----------------------
taskkill /f /im SceneServer.exe

rem -----------------------语音服务器---------------
taskkill /f /im VoiceServer.exe

rem -----------------------语音网关------------------
taskkill /f /im VoiceGateway.exe

rem -----------------------社交服务器------------------
taskkill /f /im SNSServer.exe

exit