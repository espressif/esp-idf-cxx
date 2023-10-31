# Example: Blink C++ example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates usage of the `memory::resource` C++ class in ESP-IDF.

In this example, it is shown how to select which memory capabilities should be selected to use with C++17 PMR enabled containers. 

## How to use example

### Hardware Required

Any ESP32 family development board.

### Configure the project

No special configurations are required for this example. It maybe necessary to consider the excpetion handling depending on the 
containers usage.

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
D (341) esp_memory_resource: Allocated 2048 bytes at 0x3ffb5888
Heap summary for capabilities 0x00000008:
  At 0x3ffae6e0 len 6432 free 284 allocated 5632 min_free 284
    largest_free_block 256 alloc_blocks 34 free_blocks 1 total_blocks 35
  At 0x3ffb28c8 len 186168 free 171896 allocated 13312 min_free 171896
    largest_free_block 163840 alloc_blocks 5 free_blocks 1 total_blocks 6
  At 0x3ffe0440 len 15072 free 14656 allocated 0 min_free 14656
    largest_free_block 14336 alloc_blocks 0 free_blocks 1 total_blocks 1
  At 0x3ffe4350 len 113840 free 112968 allocated 0 min_free 112968
    largest_free_block 110592 alloc_blocks 0 free_blocks 1 total_blocks 1
  Totals:
    free 299804 allocated 18944 min_free 299804 largest_free_block 163840
D (401) esp_memory_resource: Freed
I (411) main_task: Returned from app_main()

```

