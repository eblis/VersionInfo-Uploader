@echo off
if NOT EXIST "symbols\%1 - %3%4" ( 
	mkdir "symbols\%1 - %3%4"
)

xcopy %1\%2\*.pdb "symbols\%1 - %3%4\*" /EXCLUDE:symbols_exclude.txt /Y

d:\usr\PowerArchiver\pacl\pacomp.exe -p -a -r -c2 "symbols - %1.zip" "symbols\*.*"
rmdir "symbols\" /Q /S