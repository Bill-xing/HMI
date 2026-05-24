param(
    [string]$PythonHome = $env:PYTHON_HOME,
    [string]$PythonVersion = $env:PYTHON_VERSION,
    [string]$QtBin = $env:QT_BIN,
    [string]$BuildDir,
    [string]$DistDir
)

$ErrorActionPreference = "Stop"
$RootDir = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path

if (-not $PythonHome) {
    throw "Set PYTHON_HOME to the Python environment used by HMI."
}
$PythonExe = Join-Path $PythonHome "python.exe"
if (-not (Test-Path $PythonExe)) {
    throw "PYTHON_HOME does not contain python.exe: $PythonHome"
}
if (-not $PythonVersion) {
    $PythonVersion = & $PythonExe -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')"
}

if (-not $QtBin) {
    $qmakeCommand = Get-Command qmake.exe -ErrorAction SilentlyContinue
    if ($qmakeCommand) {
        $QtBin = Split-Path $qmakeCommand.Source -Parent
    } else {
        throw "Set QT_BIN to the directory containing qmake.exe and windeployqt.exe."
    }
}

$Qmake = Join-Path $QtBin "qmake.exe"
$WinDeployQt = Join-Path $QtBin "windeployqt.exe"
if (-not (Test-Path $Qmake)) {
    throw "qmake.exe not found: $Qmake"
}
if (-not (Test-Path $WinDeployQt)) {
    throw "windeployqt.exe not found: $WinDeployQt"
}

if (-not $BuildDir) {
    $BuildDir = Join-Path $RootDir "build-HMI-package-windows"
}
if (-not $DistDir) {
    $DistDir = Join-Path $RootDir "dist\windows"
}

$PackageDir = Join-Path $DistDir "HMI"
$RuntimeDir = Join-Path $PackageDir "HMI"
$PythonRuntime = Join-Path $PackageDir "python"
$ZipPath = Join-Path $DistDir "HMI-windows.zip"

New-Item -ItemType Directory -Force -Path $BuildDir, $DistDir | Out-Null

Push-Location $BuildDir
try {
    $env:PYTHON_HOME = $PythonHome
    $env:PYTHON_VERSION = $PythonVersion
    & $Qmake "CONFIG+=release" (Join-Path $RootDir "HMI.pro")

    $make = Get-Command jom.exe -ErrorAction SilentlyContinue
    if (-not $make) { $make = Get-Command nmake.exe -ErrorAction SilentlyContinue }
    if (-not $make) { $make = Get-Command mingw32-make.exe -ErrorAction SilentlyContinue }
    if (-not $make) { throw "No supported make tool found. Install jom, nmake, or mingw32-make." }
    & $make.Source
} finally {
    Pop-Location
}

$ExeCandidates = @(
    (Join-Path $BuildDir "release\HMI.exe"),
    (Join-Path $BuildDir "HMI.exe")
)
$BuiltExe = $ExeCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $BuiltExe) {
    throw "HMI.exe was not found under $BuildDir"
}

Remove-Item -Recurse -Force $PackageDir -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force -Path $PackageDir | Out-Null
Copy-Item $BuiltExe (Join-Path $PackageDir "HMI.exe")
& $WinDeployQt (Join-Path $PackageDir "HMI.exe")

Remove-Item -Recurse -Force $RuntimeDir -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force -Path $RuntimeDir | Out-Null

foreach ($file in @("predict.py", "unet.py", "segmentation.py", "requirements.txt")) {
    Copy-Item (Join-Path $RootDir $file) (Join-Path $RuntimeDir $file)
}
foreach ($dir in @("nets", "utils", "res")) {
    Copy-Item -Recurse (Join-Path $RootDir $dir) (Join-Path $RuntimeDir $dir)
}

if ($env:HMI_INCLUDE_LOCAL_STL_ASSETS -ne "1") {
    $BinaryDir = Join-Path $RuntimeDir "res\binary"
    if (Test-Path $BinaryDir) {
        Get-ChildItem -Path $BinaryDir -Recurse -File -Include *.STL,*.stl | Remove-Item -Force
    }
    Write-Host "note: local STL assets were excluded from this public package."
    Write-Host "      Set HMI_INCLUDE_LOCAL_STL_ASSETS=1 only for private local packages."
}

New-Item -ItemType Directory -Force -Path (Join-Path $RuntimeDir "model_data") | Out-Null
Copy-Item (Join-Path $RootDir "model_data\README.md") (Join-Path $RuntimeDir "model_data\README.md")
$Weight = Join-Path $RootDir "model_data\seam_unet.pth"
if (Test-Path $Weight) {
    Copy-Item $Weight (Join-Path $RuntimeDir "model_data\seam_unet.pth")
} else {
    Write-Warning "model_data\seam_unet.pth was not found; inference will not work in the package."
}

Remove-Item -Recurse -Force $PythonRuntime -ErrorAction SilentlyContinue
$CondaPack = Get-Command conda-pack.exe -ErrorAction SilentlyContinue
if (-not $CondaPack) {
    $EnvCondaPack = Join-Path $PythonHome "Scripts\conda-pack.exe"
    if (Test-Path $EnvCondaPack) {
        $CondaPack = Get-Item $EnvCondaPack
    }
}
if ($CondaPack) {
    $TempArchive = Join-Path ([System.IO.Path]::GetTempPath()) ("hmi-python-" + [System.Guid]::NewGuid() + ".zip")
    & $CondaPack.Source -p $PythonHome -o $TempArchive --force --ignore-missing-files
    New-Item -ItemType Directory -Force -Path $PythonRuntime | Out-Null
    Expand-Archive -Path $TempArchive -DestinationPath $PythonRuntime -Force
    Remove-Item -Force $TempArchive
} else {
    Write-Warning "conda-pack not found; copying PYTHON_HOME directly. Install conda-pack for a more relocatable package."
    robocopy $PythonHome $PythonRuntime /MIR /XD __pycache__ pkgs /XF *.pyc | Out-Null
    if ($LASTEXITCODE -gt 7) {
        throw "robocopy failed with exit code $LASTEXITCODE"
    }
}

$PythonDllName = "python$($PythonVersion -replace '\.', '').dll"
$PythonDll = Join-Path $PythonRuntime $PythonDllName
if (Test-Path $PythonDll) {
    Copy-Item $PythonDll (Join-Path $PackageDir $PythonDllName)
}

Remove-Item -Force $ZipPath -ErrorAction SilentlyContinue
Compress-Archive -Path (Join-Path $PackageDir "*") -DestinationPath $ZipPath -Force

Write-Host "Created:"
Write-Host "  $PackageDir"
Write-Host "  $ZipPath"
