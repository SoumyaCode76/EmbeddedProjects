# STM32 Development Environment Setup
# Run this script to configure your development environment
# Usage: .\setup_env.ps1

Write-Host "Setting up STM32 Development Environment..." -ForegroundColor Green

# OpenOCD Installation Path
$env:OPENOCD_HOME = "C:\openocd\xpack-openocd-0.12.0-3"
$env:PATH += ";$env:OPENOCD_HOME\bin"

Write-Host "[OK] OpenOCD configured: $env:OPENOCD_HOME" -ForegroundColor Green

# Verify OpenOCD installation
if (Test-Path "$env:OPENOCD_HOME\bin\openocd.exe") {
    Write-Host "[OK] OpenOCD found and working:" -ForegroundColor Green
    & "$env:OPENOCD_HOME\bin\openocd.exe" --version 2>&1 | Select-Object -First 1
} else {
    Write-Host "[ERROR] OpenOCD not found!" -ForegroundColor Red
}

# Check ARM GCC Toolchain
$armGccPath = "D:\ST\STM32CubeCLT_1.16.0\GNU-tools-for-STM32\bin"
if (Test-Path "$armGccPath\arm-none-eabi-gcc.exe") {
    Write-Host "[OK] ARM GCC Toolchain found: $armGccPath" -ForegroundColor Green
    $env:ARM_GCC_PATH = $armGccPath
    $env:PATH += ";$armGccPath"
} else {
    Write-Host "[WARNING] ARM GCC Toolchain not found at $armGccPath" -ForegroundColor Yellow
}

# Check CMake
try {
    $cmakeVersion = cmake --version 2>&1 | Select-Object -First 1
    Write-Host "[OK] CMake: $cmakeVersion" -ForegroundColor Green
} catch {
    Write-Host "[ERROR] CMake not found!" -ForegroundColor Red
}

Write-Host "`nEnvironment setup complete!" -ForegroundColor Green
Write-Host "You can now use VS Code debugging and build tasks." -ForegroundColor Cyan