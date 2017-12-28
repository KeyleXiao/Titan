echo off

taskkill /f /im ManagerClient.exe
taskkill /f /im ManagerServer.exe
taskkill /f /im MainManagerServer.exe
taskkill /f /im RootManagerServer.exe

exit