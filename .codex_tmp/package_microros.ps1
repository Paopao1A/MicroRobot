$ErrorActionPreference = "Stop"

$component = "E:\ESP32_Project\MicroRobot\extra_components\micro_ros_espidf_component"
$uros = Join-Path $component "micro_ros_src"
$install = Join-Path $uros "install"
$work = Join-Path $uros "libmicroros"
$target = Join-Path $component "libmicroros.a"
$ar = "E:\ESP-IDF\Espressif\tools\xtensa-esp32s3-elf\esp-12.2.0_20230208\xtensa-esp32s3-elf\bin\xtensa-esp32s3-elf-ar.exe"

if (Test-Path $work) {
    Remove-Item -LiteralPath $work -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $work | Out-Null

foreach ($archive in Get-ChildItem -LiteralPath (Join-Path $install "lib") -Filter "*.a") {
    $name = [IO.Path]::GetFileNameWithoutExtension($archive.Name)
    $dir = Join-Path $work $name
    New-Item -ItemType Directory -Force -Path $dir | Out-Null
    Push-Location $dir
    try {
        & $ar x $archive.FullName
        foreach ($obj in Get-ChildItem -Filter "*.obj") {
            Move-Item -LiteralPath $obj.FullName -Destination (Join-Path $work "$name-$($obj.Name)") -Force
        }
    } finally {
        Pop-Location
    }
    Remove-Item -LiteralPath $dir -Recurse -Force
}

if (Test-Path $target) {
    Remove-Item -LiteralPath $target -Force
}

Push-Location $work
try {
    $objects = @(Get-ChildItem -Filter "*.obj" | ForEach-Object { $_.Name })
    for ($i = 0; $i -lt $objects.Count; $i += 25) {
        $end = [Math]::Min($i + 24, $objects.Count - 1)
        & $ar rc $target @($objects[$i..$end])
    }
    & $ar s $target
} finally {
    Pop-Location
}

Remove-Item -LiteralPath $work -Recurse -Force

$includeDst = Join-Path $component "include"
if (Test-Path $includeDst) {
    Remove-Item -LiteralPath $includeDst -Recurse -Force
}
Copy-Item -LiteralPath (Join-Path $install "include") -Destination $includeDst -Recurse
