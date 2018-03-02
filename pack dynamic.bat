for /F "tokens=5-9* delims=. " %%i in (docs\VIUploader_readme.txt) do (call :Pack %%i %%j %%k %%l; exit)

:Pack
d:\usr\PowerArchiver\pacl\pacomp.exe -a -c2 "VIUploader (dynamic) %1.%2.%3.%4.zip" @files_release_dynamic.txt
d:\usr\PowerArchiver\pacl\pacomp.exe -p -a -c2 "VIUploader (dynamic) %1.%2.%3.%4.zip" docs\*.* *.caca
call "pack symbols.bat" VIUploader "Release (dynamic)" %1.%2.%3.%4 dynamic
exit

error:
echo "Error packing VIUploader"
