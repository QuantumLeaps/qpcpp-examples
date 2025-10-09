@setlocal
@set HOMEDIR=%CD%

::@echo off

@set TRG=host
@echo Target    : %TRG%
@echo Target    : %TRG% > log_%TRG%.txt
@set TESTDIR=%HOMEDIR%\..
@set LOGDIR=%HOMEDIR%
@set MAKEFILE=Makefile
@set LOGEXT=log
@set LOGSEP=%LOGDIR%\log_sect_sep.txt

:: goto start

:: unit tests ================================================================
set TEST=blinky
cd %TESTDIR%\%TEST%\test
del *.log *.cov
make -j8 -f %MAKEFILE% LOG=. OPT=c
if %ERRORLEVEL% neq 0 goto err
copy /b/y *.cov + %LOGSEP% + *.log + %LOGSEP% + *.cpp.gcov %LOGDIR%\TUN_%TEST%-%TRG%.%LOGEXT%

set TEST=dpp
cd %TESTDIR%\%TEST%\test_dpp
del *.log *.cov
make -j8 -f %MAKEFILE% LOG=. OPT=c
if %ERRORLEVEL% neq 0 goto err
copy /b/y *.cov + %LOGSEP% + *.log + %LOGSEP% + *.cpp.gcov %LOGDIR%\TUN_%TEST%_dpp-%TRG%.%LOGEXT%

set TEST=dpp
cd %TESTDIR%\%TEST%\test_philo
del *.log *.cov
make -j8 -f %MAKEFILE% LOG=. OPT=c
if %ERRORLEVEL% neq 0 goto err
copy /b/y *.cov + %LOGSEP% + *.log + %LOGSEP% + *.cpp.gcov %LOGDIR%\TUN_%TEST%_philo-%TRG%.%LOGEXT%

set TEST=dpp
cd %TESTDIR%\%TEST%\test_table
del *.log *.cov
make -j8 -f %MAKEFILE% LOG=. OPT=c
if %ERRORLEVEL% neq 0 goto err
copy /b/y *.cov + %LOGSEP% + *.log + %LOGSEP% + *.cpp.gcov %LOGDIR%\TUN_%TEST%_table-%TRG%.%LOGEXT%

set TEST=dpp-comp
cd %TESTDIR%\%TEST%\test_dpp
del *.log *.cov
make -j8 -f %MAKEFILE% LOG=. OPT=c
if %ERRORLEVEL% neq 0 goto err
copy /b/y *.cov + %LOGSEP% + *.log + %LOGSEP% + *.cpp.gcov %LOGDIR%\TUN_%TEST%_dpp-%TRG%.%LOGEXT%

set TEST=dpp-comp
cd %TESTDIR%\%TEST%\test_philo
del *.log *.cov
make -j8 -f %MAKEFILE% LOG=. OPT=c
if %ERRORLEVEL% neq 0 goto err
copy /b/y *.cov + %LOGSEP% + *.log + %LOGSEP% + *.cpp.gcov %LOGDIR%\TUN_%TEST%_philo-%TRG%.%LOGEXT%

set TEST=dpp-comp
cd %TESTDIR%\%TEST%\test_table
del *.log *.cov
make -j8 -f %MAKEFILE% LOG=. OPT=c
if %ERRORLEVEL% neq 0 goto err
copy /b/y *.cov + %LOGSEP% + *.log + %LOGSEP% + *.cpp.gcov %LOGDIR%\TUN_%TEST%_table-%TRG%.%LOGEXT%

set TEST=evt_par
cd %TESTDIR%\%TEST%\test
del *.log *.cov
make -j8 -f %MAKEFILE% LOG=. OPT=c
if %ERRORLEVEL% neq 0 goto err
copy /b/y *.cov + %LOGSEP% + *.log + %LOGSEP% + *.cpp.gcov %LOGDIR%\TUN_%TEST%-%TRG%.%LOGEXT%

set TEST=self_test
cd %TESTDIR%\%TEST%\test
del *.log *.cov
make -j8 -f %MAKEFILE% LOG=. OPT=c
@echo 2 errors expected!
:: if %ERRORLEVEL% neq 2 goto err
copy /b/y *.cov + %LOGSEP% + *.log + %LOGSEP% + *.cpp.gcov %LOGDIR%\TUN_%TEST%-%TRG%.%LOGEXT%

set TEST=start_seq
cd %TESTDIR%\%TEST%\test
del *.log *.cov
make -j8 -f %MAKEFILE% LOG=. OPT=c
if %ERRORLEVEL% neq 0 goto err
copy /b/y *.cov + %LOGSEP% + *.log + %LOGSEP% + *.cpp.gcov %LOGDIR%\TUN_%TEST%-%TRG%.%LOGEXT%

:cleanup
@echo Final cleanup...
cd %TESTDIR%
@for /d /r . %%d in (build_host) do @if exist "%%d" rd /s /q "%%d"
@echo OK

@cd /d %HOMEDIR%
@del log_%TRG%.txt
exit /b

:err
@chdir /d %HOMEDIR%
@echo %DIR% Has Test Errors... 1>&2

@endlocal
