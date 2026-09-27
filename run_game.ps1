# AirStriker - 2D Space Shooter (PowerShell Launcher)
Write-Host "====================================================================" -ForegroundColor Cyan
Write-Host "  LAUNCHING AIRSTRIKER - 2D SPACE SHOOTER (CO-1 PLAYABLE GAME)      " -ForegroundColor Green
Write-Host "====================================================================" -ForegroundColor Cyan

$root = $PSScriptRoot
if (-not $root) { $root = Get-Location }

$binPath = Join-Path $root "bin\AirStriker_CO1.exe"

if (-not (Test-Path $binPath)) {
    Write-Host "Executable not found. Compiling now..." -ForegroundColor Yellow
    cmd.exe /c "build_all.bat"
}

$glutBin = Join-Path $root "bin\glut32.dll"
if (-not (Test-Path $glutBin)) {
    if (Test-Path "C:\MinGW\bin\glut32.dll") {
        Copy-Item "C:\MinGW\bin\glut32.dll" -Destination $glutBin -Force
    }
}

Write-Host "Starting AirStriker window..." -ForegroundColor Green
Write-Host "Controls: [WASD / Arrows] Move | [SPACE] Laser | [R] Restart | [ESC] Exit" -ForegroundColor Gray
Start-Process -FilePath $binPath -WorkingDirectory (Join-Path $root "bin")
