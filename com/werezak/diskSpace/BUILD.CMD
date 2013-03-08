@echo off

rem The Java compiler and the gcc compiler must be on the path.

set JDK=C:\Progra~1\Java\jdk1.6.0
set CLASSPATH=D:\;.

@echo Compiling Java ...
javac CheckDiskSpace.java
javac DiskSpace.java

@echo Creating Native class .H header ...
javah -jni com.werezak.diskSpace.DiskSpace 
del DiskSpace.h
rename com_werezak_diskSpace_DiskSpace.h DiskSpace.h

@echo Compiling C ...
gcc -c -mno-cygwin -I%JDK%/include -I%JDK%/include/win32 DiskSpace.c
rem -Wl,--add-stdcall-alias -shared -o inde.dll inde.c -lz

@echo Making DLL ...
rem gcc -mno-cygwin -I%JDK%/include -I%JDK%/include/win32 $(GCC_INCLUDE) -Wl,--add-stdcall-alias -shared -o DiskSpace.o DiskSpace.dll
gcc -mno-cygwin -I%JDK%/include -I%JDK%/include/win32 -Wl,--add-stdcall-alias -shared -o DiskSpace.dll DiskSpace.o

@echo Installing ...
copy DiskSpace.dll %SystemRoot%\system32\DiskSpace.dll

@echo Testing ...
java CheckDiskSpace d:\
