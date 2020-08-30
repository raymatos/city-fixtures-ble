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

V2 -
Option to add a LCD screen with a QR code - this code changes every 5 mins, the idea here is that the user will need to scan the QR code to gain access to the floor or elevator controller.


## ESP32 Firmware

### Additional libraries
    
- ArduinoJson
