# ESP-IDF-C++

This project provides C++ wrapper classes around some components of [esp-idf](https://github.com/espressif/esp-idf). It is organized as a component. For more information how to use ESP-IDF components and how to set up ESP-IDF, please refer to [esp-idf](https://github.com/espressif/esp-idf).

## *NOTE*

This repository hasn't been released yet. After the code is in a state that is convincing, we will release it for the component manager. Before the release, there might be breaking changes.

Some bits that are still missing (non-exhaustive list):
* MQTT C++ classes
* Default pin definition on Kconfig for some examples
* Upload into the component registry (need to be done together with release)

That being said, suggestions are very welcome. To use the project in the current state, follow the usual process for setting up the IDF environment (i.e., `. ./export.sh` inside [esp-idf](https://github.com/espressif/esp-idf)). Then checkout this repository and change to the examples or unit tests and try them right away. The component manager is set up for these to find the esp-idf-cxx component locally.

## Requirements

* ESP-IDF and its requirements

No other special requirements are necessary.
