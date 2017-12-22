echo off

taskkill /f /im ManagerClient.exe
taskkill /f /im ManagerServer.exe
taskkill /f /im MainManagerServer.exe
taskkill /f /im RootManagerServer.exe

start /min "子服务器"  ManagerServer.exe
ping 1 -n 3 -w 500 2>nul 1>nul
start /min "根服务器" RootManagerServer.exe
ping 1 -n 3 -w 5000 2>nul 1>nul
start /min "主服务器" MainManagerServer.exe

rem ping 1 -n 3 -w 1000 2>nul 1>nul
rem start /min "客户端" ManagerClient.exe

exit