cmake_minimum_required( VERSION 3.23 )
project( LegoEsp32SunTracker )

set( CMAKE_CXX_STANDARD 14 )

add_executable( LegoEsp32SunTracker
		LegoEsp32SunTracker.ino
		LegoEsp32SunTracker.h
		NetworkFunctions.ino
		privateInfo.h
		)

include_directories( .
		C:/Users/Adam/AppData/Local/Arduino15/packages/arduino/hardware/avr/1.8.5/cores/arduino/
		C:/Code/Arduino/libraries/
		C:/Code/Arduino/libraries/Servo/src/
		C:/Code/Arduino/libraries/ESP32Servo/src/
		C:/Code/Arduino/libraries/ArduinoJson/
		C:/Code/Arduino/libraries/PubSubClient/src/
		C:/Code/Arduino/libraries/Adafruit_PWM_Servo_Driver_Library/
		"C:/Program Files (x86)/Arduino/hardware/arduino/avr/cores/arduino/"
		"C:/Program Files (x86)/Arduino/hardware/tools/avr/lib/gcc/avr/7.3.0/include/"
		C:/Users/Adam/AppData/Local/Arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/avr/include/
		C:/Users/Adam/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.4/cores/esp32/
		C:/Users/Adam/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.4/libraries/
		C:/Users/Adam/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.4/libraries/ArduinoOTA/src/
		C:/Users/Adam/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.4/libraries/ESPmDNS/src/
		C:/Users/Adam/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.4/libraries/WiFi/src/
		C:/Users/Adam/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.4/tools/sdk/esp32/
		C:/Users/Adam/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.4/tools/sdk/esp32/include/
		C:/Users/Adam/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.4/tools/sdk/esp32/include/esp_common/include/
		C:/Users/Adam/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.4/tools/sdk/esp32/include/esp_rom/include/
		C:/Users/Adam/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.4/tools/sdk/esp32/include/esp_system/include/
		C:/Users/Adam/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.4/tools/sdk/esp32/include/esp32-camera/driver/include/
		C:/Users/Adam/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.4/tools/sdk/esp32/include/log/include/
		C:/Users/Adam/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.4/tools/sdk/esp32/qspi_qspi/include/
		C:/Users/Adam/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.4/variants/esp32/
		C:/Users/Adam/AppData/Local/Arduino15/packages/esp32/tools/xtensa-esp32-elf-gcc/gcc8_4_0-esp-2021r2-patch3/xtensa-esp32-elf/
		C:/Users/Adam/AppData/Local/Arduino15/packages/esp32/tools/xtensa-esp32-elf-gcc/gcc8_4_0-esp-2021r2-patch3/xtensa-esp32-elf/include/
		)

set_target_properties( LegoEsp32SunTracker PROPERTIES LINKER_LANGUAGE CXX )
