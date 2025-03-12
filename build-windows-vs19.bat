@echo off
setlocal EnableDelayedExpansion

set "DIRECTORY=premake"
set "EXE_FILE=premake5.exe"
set "ZIP_FILE=premake.zip"

:: Create premake directory if it doesn't exist
if not exist "%DIRECTORY%" (
    mkdir %DIRECTORY%
    if !ERRORLEVEL! NEQ 0 (
        echo Error: Failed to create premake directory
        goto :end
    )
)

:: Check if premake5.exe exists in premake directory
if exist "%DIRECTORY%\%EXE_FILE%" (
    echo Premake5 found. Running premake5 with premake5.lua for VS2019...
    %DIRECTORY%\%EXE_FILE% --file=premake5.lua vs2019
    goto :end
)

:: Check if zip file exists in premake directory
if exist "%DIRECTORY%\%ZIP_FILE%" (
    echo Zip file found. Extracting existing zip...
    pushd %DIRECTORY%
    tar -xf %ZIP_FILE% %EXE_FILE%
    if !ERRORLEVEL! NEQ 0 (
        echo Error: Failed to extract existing Premake5 zip.
        popd
        goto :end
    )
    popd
    goto :verify
)

echo Premake5 not found. Downloading...

:: Define download URL (using a stable version from GitHub)
set "URL=https://github.com/premake/premake-core/releases/download/v5.0.0-beta2/premake-5.0.0-beta2-windows.zip"

:: Download using curl into premake directory
curl -L -o %DIRECTORY%\%ZIP_FILE% %URL%
if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to download Premake5.
    goto :end
)

:: Change to premake directory, extract, and return
pushd %DIRECTORY%
tar -xf %ZIP_FILE% %EXE_FILE%
if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to extract Premake5.
    popd
    goto :end
)
popd

:: Clean up zip file
del %DIRECTORY%\%ZIP_FILE%

:verify
:: Verify installation and run
if exist "%DIRECTORY%\%EXE_FILE%" (
    echo Premake5 successfully downloaded/extracted.
    echo Running premake5 with premake5.lua for VS2019...
    %DIRECTORY%\%EXE_FILE% --file=premake5.lua vs2019
) else (
    echo Error: Something went wrong during installation.
)

:end
pause