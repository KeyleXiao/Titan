echo off

rem ---------------------- ������������ ----------------------

taskkill /f /im ManagerClient.exe
taskkill /f /im ManagerServer.exe
taskkill /f /im MainManagerServer.exe
taskkill /f /im RootManagerServer.exe
taskkill /f /im BridgeServer.exe

rem ---------------------- ��������Ⱥ ----------------------
taskkill /f /im CenterServer.exe
taskkill /f /im GatewayServer.exe
taskkill /f /im LoginServer.exe
taskkill /f /im TimeSync.exe
taskkill /f /im GuardServer.exe
rem ---------------------- ���� ----------------------
taskkill /f /im SocialServer.exe

rem ---------------------- ���������� ----------------------
taskkill /f /im SceneServer.exe

rem -----------------------����������---------------
taskkill /f /im VoiceServer.exe

rem -----------------------��������------------------
taskkill /f /im VoiceGateway.exe

rem -----------------------�罻������------------------
taskkill /f /im SNSServer.exe

exit