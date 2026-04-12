call "D:\vs\VC\Auxiliary\Build\vcvarsall.bat" x64
call "D:\qt514\5.14.2\msvc2017_64\bin\qtenv2.bat"
cd /d "D:\projectQT\MicroScop\build-MIDpt-Desktop_Qt_5_14_2_MSVC2017_64bit-Debug\debug
windeployqt --debug "MIDpt.exe"
echo Package Over
pause