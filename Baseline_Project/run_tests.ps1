# Build and run unit tests for the STM32 project
param(
    [switch]$Clean = $false,
    [switch]$RunTests = $true
)

$ProjectRoot = $PSScriptRoot
$TestBuildDir = Join-Path $ProjectRoot "build_tests"

Write-Host "Setting up unit tests build environment..." -ForegroundColor Green

# Clean build directory if requested
if ($Clean -and (Test-Path $TestBuildDir)) {
    Write-Host "Cleaning test build directory..." -ForegroundColor Yellow
    Remove-Item $TestBuildDir -Recurse -Force
}

# Create build directory if it doesn't exist
if (!(Test-Path $TestBuildDir)) {
    New-Item -ItemType Directory -Path $TestBuildDir | Out-Null
}

# Configure CMake for tests
Write-Host "Configuring CMake for tests..." -ForegroundColor Blue
Set-Location $TestBuildDir
$configureResult = & cmake .. -DBUILD_TESTS=ON 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    Write-Host $configureResult
    Set-Location $ProjectRoot
    exit 1
}

# Build tests
Write-Host "Building tests..." -ForegroundColor Blue
$buildResult = & cmake --build . --config Debug 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    Write-Host $buildResult
    Set-Location $ProjectRoot
    exit 1
}

# Run tests if requested
if ($RunTests) {
    Write-Host "Running unit tests..." -ForegroundColor Green
    $testResult = & ctest --output-on-failure --verbose 2>&1
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Tests failed!" -ForegroundColor Red
        Write-Host $testResult
    } else {
        Write-Host "All tests passed!" -ForegroundColor Green
    }
    Write-Host $testResult
}

Set-Location $ProjectRoot
Write-Host "Done!" -ForegroundColor Green