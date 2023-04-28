@echo off

@SET HEXFILE= .\runf_app_pure.hex

:DOWNLOAD_App
@cls
@Mode con cols=80 lines=62
@echo ***************************************
@echo ******     Download App       *********
@echo ***************************************
@%CompileTools%\StLink\ST-LINK_CLI.exe -c SWD UR -P %HEXFILE% -HardRst
@echo.

if NOT "%1"=="NoWait" (
   @pause
)
