echo off

taskkill /f /im ManagerClient.exe
taskkill /f /im ManagerServer.exe
taskkill /f /im MainManagerServer.exe
taskkill /f /im RootManagerServer.exe

start /min "�ӷ�����"  ManagerServer.exe
ping 1 -n 3 -w 500 2>nul 1>nul
start /min "��������" RootManagerServer.exe
ping 1 -n 3 -w 5000 2>nul 1>nul
start /min "��������" MainManagerServer.exe

rem ping 1 -n 3 -w 1000 2>nul 1>nul
rem start /min "�ͻ���" ManagerClient.exe

exit