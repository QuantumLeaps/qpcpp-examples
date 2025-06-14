@setlocal

@set HOMEDIR=%CD%
set LOG=%HOMEDIR%

::goto skip

set DIR=self_test\test
cd %HOMEDIR%\..\%DIR%
make -j8 LOG=%LOG% OPT=c
if %ERRORLEVEL% neq 2 goto err

set DIR=blinky\test
cd %HOMEDIR%\..\%DIR%
make -j8 LOG=%LOG% OPT=c
if %ERRORLEVEL% neq 0 goto err

set DIR=dpp\test_philo
cd %HOMEDIR%\..\%DIR%
make -j8 LOG=%LOG% OPT=c
if %ERRORLEVEL% neq 0 goto err

set DIR=dpp\test_table
cd %HOMEDIR%\..\%DIR%
make -j8 LOG=%LOG% OPT=c
if %ERRORLEVEL% neq 0 goto err

set DIR=dpp\test_dpp
cd %HOMEDIR%\..\%DIR%
make -j8 LOG=%LOG% OPT=c
if %ERRORLEVEL% neq 0 goto err

set DIR=dpp-comp\test_philo
cd %HOMEDIR%\..\%DIR%
make -j8 LOG=%LOG% OPT=c
if %ERRORLEVEL% neq 0 goto err

set DIR=dpp-comp\test_table
cd %HOMEDIR%\..\%DIR%
make -j8 LOG=%LOG% OPT=c
if %ERRORLEVEL% neq 0 goto err

set DIR=dpp-comp\test_dpp
cd %HOMEDIR%\..\%DIR%
make -j8 LOG=%LOG% OPT=c
if %ERRORLEVEL% neq 0 goto err

set DIR=evt_par\test
cd %HOMEDIR%\..\%DIR%
make -j8 LOG=%LOG% OPT=c
if %ERRORLEVEL% neq 0 goto err

set DIR=qep_hsm\test
cd %HOMEDIR%\..\%DIR%
make -j8 LOG=%LOG% OPT=c
if %ERRORLEVEL% neq 0 goto err

set DIR=qep_msm\test
cd %HOMEDIR%\..\%DIR%
make -j8 LOG=%LOG% OPT=c
if %ERRORLEVEL% neq 0 goto err

set DIR=start_seq\test
cd %HOMEDIR%\..\%DIR%
make -j8 LOG=%LOG% OPT=c
if %ERRORLEVEL% neq 0 goto err

:cleanup
@echo Cleanup...
cd %HOMEDIR%\..
for /d /r %%i in (build_host) do @rmdir /s/q "%%i"
@echo OK

@chdir /d %HOMEDIR%
exit /b

:err
@chdir /d %HOMEDIR%
@echo %DIR% Has Test Errors... 1>&2

@endlocal
