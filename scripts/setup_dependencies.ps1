param(
  [string]$Destination
)

$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
if (-not $Destination) {
  $Destination = Join-Path $projectRoot 'external\iPlug2'
}

$iPlugRevision = 'b64192fe18afd9bc9a1fe324db5aceb48f4a0eee'
$iPlugRepository = 'https://github.com/iPlug2/iPlug2.git'

$git = Get-Command git -ErrorAction SilentlyContinue
if (-not $git) {
  throw 'Git was not found. Install Git for Windows, open a new PowerShell window, and run this script again.'
}

$destinationParent = Split-Path -Parent $Destination
New-Item -ItemType Directory -Force -Path $destinationParent | Out-Null

if (-not (Test-Path -LiteralPath $Destination)) {
  & $git.Source clone $iPlugRepository $Destination
  if ($LASTEXITCODE -ne 0) { throw 'Could not clone iPlug2.' }
}

if (-not (Test-Path -LiteralPath (Join-Path $Destination '.git'))) {
  throw "The destination exists but is not an iPlug2 Git checkout: $Destination"
}

& $git.Source -C $Destination fetch --depth 1 origin $iPlugRevision
if ($LASTEXITCODE -ne 0) { throw 'Could not fetch the required iPlug2 revision.' }
& $git.Source -C $Destination checkout --detach $iPlugRevision
if ($LASTEXITCODE -ne 0) { throw 'Could not select the required iPlug2 revision.' }

$requiredFiles = @(
  (Join-Path $Destination 'iPlug2.cmake'),
  (Join-Path $Destination 'LICENSE.txt'),
  (Join-Path $Destination 'Dependencies\IPlug\VST3_SDK\CMakeLists.txt'),
  (Join-Path $Destination 'Dependencies\IPlug\VST3_SDK\LICENSE.txt'),
  (Join-Path $Destination 'Dependencies\IPlug\VST3_SDK\pluginterfaces\base\funknown.h'),
  (Join-Path $Destination 'Dependencies\IPlug\VST3_SDK\public.sdk\source\vst\vstsinglecomponenteffect.cpp')
)
foreach ($file in $requiredFiles) {
  if (-not (Test-Path -LiteralPath $file)) { throw "Required dependency file is missing: $file" }
}

Write-Host "Dependencies are ready: $Destination"
Write-Host "iPlug2 revision: $iPlugRevision"
