(Get-Content .\resource.rc).replace('1.0.0.0',$env:appveyor_build_version) | Set-Content .\resource2.rc
del .\resource.rc
mv .\resource2.rc .\resource.rc
.\sgtools\GoRC.exe /fo .\resource.res .\resource.rc