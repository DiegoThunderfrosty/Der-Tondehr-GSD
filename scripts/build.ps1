param(
  [string]$IPlug2Dir,
  [string]$Configuration = 'Release',
  [string]$BuildDirectory = 'build\windows',
  [int]$ParallelJobs = 4
)

$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot

if (-not $IPlug2Dir) {
  $IPlug2Dir = Join-Path $projectRoot 'external\iPlug2'
}
if (-not (Test-Path -LiteralPath (Join-Path $IPlug2Dir 'iPlug2.cmake'))) {
  throw 'iPlug2 was not found. Run scripts\setup_dependencies.ps1 first or provide -IPlug2Dir.'
}
if (-not (Test-Path -LiteralPath (Join-Path $IPlug2Dir 'Dependencies\IPlug\VST3_SDK\CMakeLists.txt'))) {
  throw 'The required VST3 SDK was not found inside iPlug2. Run scripts\setup_dependencies.ps1.'
}
$IPlug2Dir = (Resolve-Path -LiteralPath $IPlug2Dir).Path

$cmakeCommand = Get-Command cmake -ErrorAction SilentlyContinue
if ($cmakeCommand) {
  $cmakePath = $cmakeCommand.Source
} else {
  $cmakePath = Get-ChildItem -LiteralPath 'C:\Program Files\Microsoft Visual Studio' -Recurse -Filter cmake.exe -ErrorAction SilentlyContinue |
    Where-Object { $_.FullName -match '\\CMake\\bin\\cmake.exe$' } |
    Select-Object -First 1 -ExpandProperty FullName
}
if (-not $cmakePath) {
  throw 'CMake was not found. Install Visual Studio with Desktop development with C++ and CMake tools.'
}

if ([System.IO.Path]::IsPathRooted($BuildDirectory)) {
  $buildDir = $BuildDirectory
} else {
  $buildDir = Join-Path $projectRoot $BuildDirectory
}

& $cmakePath -S $projectRoot -B $buildDir -A x64 `
  "-DIPLUG2_DIR=$IPlug2Dir" `
  '-DIPLUG_DEPLOY_PLUGINS=OFF'
if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }

& $cmakePath --build $buildDir --config $Configuration --target DerTondehrGSD-vst3 --parallel $ParallelJobs
if ($LASTEXITCODE -ne 0) { throw 'Build failed.' }

$bundle = Join-Path $buildDir 'out\DerTondehrGSD.vst3'
if (-not (Test-Path -LiteralPath $bundle)) {
  throw "Build completed but the expected VST3 bundle was not found: $bundle"
}

Write-Host "VST3 bundle is ready: $bundle"
