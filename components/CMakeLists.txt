set(src_dirs
            hardware/LED
            hardware/BEEP
            hardware/ADC
            hardware/MOTOR
            hardware/PID
            App/App_Init
            App/App_Publisher
            App/App_Subscriber
            Task/Task_Init
            Task/Task_MotionControl
            Task/Task_LedControl
            Task/Task_BeepControl)

set(include_dirs
            hardware/LED
            hardware/BEEP
            hardware/ADC
            hardware/MOTOR
            hardware/PID
            App/App_Init
            App/App_Publisher
            App/App_Subscriber
            Task/Task_Init
            Task/Task_MotionControl
            Task/Task_LedControl
            Task/Task_BeepControl)

set(requires
            driver
            esp_adc)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})
component_compile_options(-ffast-math -O3 -Wno-error=format -Wno-format)
