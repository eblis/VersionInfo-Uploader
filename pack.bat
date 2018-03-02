for /F "tokens=5-9* delims=. " %%i in (docs\VIUploader_readme.txt) do (call :Pack %%i %%j %%k %%l; exit)

:Pack
d:\usr\PowerArchiver\pacl\pacomp.exe -a -c2 "VIUploader %1.%2.%3.%4.zip" @files_release.txt
d:\usr\PowerArchiver\pacl\pacomp.exe -p -a -c2 "VIUploader %1.%2.%3.%4.zip" docs\*.* *.caca
call "pack symbols.bat" VIUploader Release %1.%2.%3.%4
exit

error:
echo "Error packing VIUploader"
