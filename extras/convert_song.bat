@echo off

if "%~1"=="" (
    echo Drag an MP3 file onto this BAT file.
    pause
    exit /b
)

set /p OUTPUT=Output name (without .pcm): 

ffmpeg -i "%~1" -ar 11025 -ac 1 -c:a pcm_s8 -f s8 "%OUTPUT%.pcm"

echo.
echo Done! Saved as %OUTPUT%.pcm
pause
