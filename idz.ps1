  Get-Content .\idzhook\resource.rc | ForEach-Object { $_ -replace "1.0.0.0", $env:appveyor_build_version } | Set-Content .\idzhook\resource2.rc
  del .\idzhook\resource.rc
  mv .\idzhook\resource2.rc .\idzhook\resource.rc
  .\sgtools\GoRC.exe /fo .\idzhook\resource.res .\idzhook\resource.rc
  .\sgtools\ResourceHacker.exe -open .\_build64\idzhook\idzhook.dll -save .\_build64\idzhook\idzhook_ver.dll -action add -res .\idzhook\resource.res
  del .\_build64\idzhook\idzhook.dll
  mv .\_build64\idzhook\idzhook_ver.dll .\_build64\idzhook\idzhook.dll