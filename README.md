# city-fixtures-ble

    Control an elevator floor selection panel with an app and BLE.
    The hardware should put out a BLE peripheral that will allow a user to select a floor.

## Components

- Controller (ESP32)
- Relay Module
- Mobile Application

## Functionalities

_The controller should be able to be setup as “Elevator” or “Floor”_

### Elevator

- Set the number of floors (A-Z,0-1), Map each floor to a relay number (1-N) (this can be done via a config file)
- Set the name of the elevator
- Push the name of the elevator and available floors via BLE

### Floor

- Map the Up relay and the down relay, or just one of the 2
- Set the name of the floor
- Push the name of the Floor and available options

### BLE Service & Characteristics

- BLE device name: default `Fixture` able to change via characteristic

- Fixture BLE Service: ```0f7e0001-3e26-454e-9669-1a8b67b52161```

- Configuration characteristic: `0f7e0003-b5a3-f393-e0a9-e50e24dcca9e`

- Force realy characteristic: `0f7e0004-b5a3-f393-e0a9-e50e24dcca9e"`

- Peripheral charateristic: `0f7e0002-b5a3-f393-e0a9-e50e24dcca9e`
    
    - notify
        
        *Every 4sec, it notifies current sensor variables etc.*

- Systemtime characteristic: `0d7e0006-b5a3-f393-e0a9-e50e24dcca9e`
    - read
        
        *Every 5sec, it will notify system date & time*

        ```
        2020-09-01T09:08
        ```

    - write

        *You could change system date & time by writing value for this characteristic*

        ```
        2020-09-01T09:08
        ```

## ESP32 Firmware

### Dev environment
- Arduino IDE 1.8.12 or later 
- ESP32 arduino core latest stable version 

### Additional libraries
- ArduinoJson


**********

## V2

Option to add a LCD screen with a QR code - this code changes every 5 mins, the idea here is that the user will need to scan the QR code to gain access to the floor or elevator controller.
