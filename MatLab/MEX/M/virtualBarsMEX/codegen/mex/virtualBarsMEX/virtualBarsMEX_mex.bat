@echo off
set MATLAB=C:\PROGRA~1\MATLAB\R2013a
set MATLAB_ARCH=win64
set MATLAB_BIN="C:\Program Files\MATLAB\R2013a\bin"
set ENTRYPOINT=mexFunction
set OUTDIR=.\
set LIB_NAME=virtualBarsMEX_mex
set MEX_NAME=virtualBarsMEX_mex
set MEX_EXT=.mexw64
call mexopts.bat
echo # Make settings for virtualBarsMEX > virtualBarsMEX_mex.mki
echo COMPILER=%COMPILER%>> virtualBarsMEX_mex.mki
echo COMPFLAGS=%COMPFLAGS%>> virtualBarsMEX_mex.mki
echo OPTIMFLAGS=%OPTIMFLAGS%>> virtualBarsMEX_mex.mki
echo DEBUGFLAGS=%DEBUGFLAGS%>> virtualBarsMEX_mex.mki
echo LINKER=%LINKER%>> virtualBarsMEX_mex.mki
echo LINKFLAGS=%LINKFLAGS%>> virtualBarsMEX_mex.mki
echo LINKOPTIMFLAGS=%LINKOPTIMFLAGS%>> virtualBarsMEX_mex.mki
echo LINKDEBUGFLAGS=%LINKDEBUGFLAGS%>> virtualBarsMEX_mex.mki
echo MATLAB_ARCH=%MATLAB_ARCH%>> virtualBarsMEX_mex.mki
echo BORLAND=%BORLAND%>> virtualBarsMEX_mex.mki
echo OMPFLAGS= >> virtualBarsMEX_mex.mki
echo OMPLINKFLAGS= >> virtualBarsMEX_mex.mki
echo EMC_COMPILER=msvc110>> virtualBarsMEX_mex.mki
echo EMC_CONFIG=optim>> virtualBarsMEX_mex.mki
"C:\Program Files\MATLAB\R2013a\bin\win64\gmake" -B -f virtualBarsMEX_mex.mk
