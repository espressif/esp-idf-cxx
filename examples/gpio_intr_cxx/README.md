# Example: GPIO interrupt C++ example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates usage of the `GPIO_Intr` C++ class in ESP-IDF.

In this example, the `sdkconfig.defaults` file sets the `CONFIG_COMPILER_CXX_EXCEPTIONS` option. 
This enables both compile time support (`-fexceptions` compiler flag) and run-time support for C++ exception handling.
This is necessary for the C++ APIs.

## How to use example

The example demonstrates the usage of lambda function, static function and non static member method
as callback for GPIO interrupts.
It also highlights the registration of several callbacks to the same GPIO interrupt.

### Hardware Required

Any ESP32 family development board.

Connect a line on the 3.3V output of the ESP32 family development board.

Use the line to trigger a state change from LOW to HIGH on the GPIOs configured as input with pull-down in the main.cpp (by default GPIO 12, 13 and 27 are set).

### Configure the project

```
idf.py menuconfig
```

### Build and Flash

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
...
I (314) cpu_start: Starting scheduler on PRO CPU.
I (317) gpio: GPIO[10]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
initializing interrupt on GPIO 10
interrupt handler added successfully
interrupt triggered 32
interrupt triggered 35
interrupt triggered 36
interrupt triggered 50
interrupt triggered 54
interrupt triggered 55
interrupt triggered 63
interrupt triggered 64

```

