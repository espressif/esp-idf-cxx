# ESP-IDF-C++

This project provides C++ wrapper classes around some components of [esp-idf](https://github.com/espressif/esp-idf). It is organized as a component for the [IDF component manager](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-component-manager.html). You can find this component [in the component registry](https://components.espressif.com/components/espressif/esp-idf-cxx). 

## *NOTE*
This component is in a beta-release phase. Some bits that are still missing (non-exhaustive list):
* MQTT C++ classes
* Default pin definition on Kconfig for some examples

A road map and detailed release document will be announced soon.

## Requirements

* ESP-IDF and its requirements.
  Please follow the [ESP-IDF "Get Started" Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html) to download, install and setup esp-idf.

No other special requirements are necessary.

## Usage

Set up the IDF environment (i.e., `. ./export.sh` inside [esp-idf](https://github.com/espressif/esp-idf)). Then go to your project directory, use `idf.py add-dependency espressif/esp-idf-cxx^1.0.0-beta ` (should only be done once) and you should be able to use this component.
