for /F "tokens=5-9* delims=. " %%i in (docs\VIUploader_readme.txt) do (call :Pack %%i %%j %%k %%l; exit)

:Pack
d:\usr\PowerArchiver\pacl\pacomp.exe -p -a -c2 "VIUploader src %1.%2.%3.%4.zip" @files_source.txt -x*.zip -x*.ncb -x*.user
exit

error:
echo "Error packing VIUploader"
