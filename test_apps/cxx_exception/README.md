# Test App: CXX Exception

This application tests various utilities that are used for exceptions in esp-idf-cxx.

## Hardware Required

Any ESP32 family development board.

## Build Test App

Before building, you may need to set the target: `idf.py set-target <target`. If you don't do this, the default target (esp32) will be selected. Building the test app uses the same command as any other IDF application:
```
idf.py build
```

## Run Test App

You can run this app manually as well as automatically using pytest and pytest-embedded.

### Run manually

To manually run the app, flash and connect the monitor first:
```
idf.py -p <port> flash monitor
```
Then, choose the tests according to the test menu.

### Run automatically

The following command runs the test app for target esp32.
```
pytest -s --junit-xml=./test_app_results_cxx_exception.xml --embedded-services esp,idf --target=esp32
```
This command will automatically run all tests and evaluate them on the host.

If you want to run on a different target, you need to change --target <target> and also build it for that target.
