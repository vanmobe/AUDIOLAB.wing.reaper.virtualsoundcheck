$ErrorActionPreference = 'Stop'

$pluginName = 'reaper_wingconnector.dll'
$configName = 'config.json'
$targetDir = Join-Path $env:APPDATA 'REAPER\UserPlugins'
$sourceDir = Split-Path -Parent $MyInvocation.MyCommand.Path

New-Item -ItemType Directory -Path $targetDir -Force | Out-Null
Copy-Item -Path (Join-Path $sourceDir $pluginName) -Destination (Join-Path $targetDir $pluginName) -Force
Copy-Item -Path (Join-Path $sourceDir $configName) -Destination (Join-Path $targetDir $configName) -Force

Write-Host "Installed to $targetDir"
Write-Host "Restart REAPER to load the extension."
