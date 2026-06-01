$ErrorActionPreference = "Stop"

. "E:\ESP-IDF\Espressif\frameworks\esp-idf-v5.1.2\export.ps1"

$component = "E:\ESP32_Project\MicroRobot\extra_components\micro_ros_espidf_component"
$idf = "E:/ESP-IDF/Espressif/frameworks/esp-idf-v5.1.2"

$env:PATH = "$component;C:\Program Files\Git\usr\bin;C:\Program Files\Git\bin;E:\mingw64\bin;$env:PATH"

$includePaths = @(
    "E:/ESP32_Project/MicroRobot/build/config",
    "$idf/components/xtensa/include",
    "$idf/components/xtensa/esp32s3/include",
    "$idf/components/newlib/platform_include",
    "$idf/components/freertos/FreeRTOS-Kernel/include",
    "$idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/include",
    "$idf/components/freertos/esp_additions/include/freertos",
    "$idf/components/freertos/esp_additions/include",
    "$idf/components/freertos/esp_additions/arch/xtensa/include",
    "$idf/components/lwip/include",
    "$idf/components/lwip/include/apps",
    "$idf/components/lwip/lwip/src/include",
    "$idf/components/lwip/port/include",
    "$idf/components/lwip/port/freertos/include",
    "$idf/components/lwip/port/esp32xx/include",
    "$idf/components/lwip/port/esp32xx/include/arch",
    "$idf/components/esp_common/include",
    "$idf/components/esp_system/include",
    "$idf/components/esp_system/port/include/private",
    "$idf/components/esp_hw_support/include",
    "$idf/components/heap/include",
    "$idf/components/log/include",
    "$idf/components/soc/include",
    "$idf/components/soc/esp32s3/include",
    "$idf/components/hal/include",
    "$idf/components/hal/esp32s3/include",
    "$idf/components/esp_rom/include",
    "$idf/components/esp_rom/include/esp32s3",
    "$idf/components/esp_rom/esp32s3"
)

$idfIncludes = ($includePaths | ForEach-Object { "-I$_" }) -join " "

Set-Location $component
. "micro_ros_dev\install\local_setup.ps1"
Set-Location "micro_ros_src"

colcon build `
    --parallel-workers 1 `
    --merge-install `
    --packages-ignore-regex=.*_cpp `
    --metas "$component/colcon.meta" `
    --cmake-args `
    -G "MinGW Makefiles" `
    --no-warn-unused-cli `
    -DCMAKE_MAKE_PROGRAM="E:/mingw64/bin/mingw32-make.exe" `
    -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=OFF `
    -DTHIRDPARTY=ON `
    -DBUILD_SHARED_LIBS=OFF `
    -DBUILD_TESTING=OFF `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_TOOLCHAIN_FILE="$component/esp32_toolchain.cmake" `
    -DCMAKE_VERBOSE_MAKEFILE=OFF `
    "-DIDF_INCLUDES=$idfIncludes" `
    -DCMAKE_C_STANDARD=17 `
    -DUCLIENT_C_STANDARD=17
