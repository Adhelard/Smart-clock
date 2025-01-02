# Smart Clock Project

## Overview
The **Smart Clock** is an Arduino-based project that combines multiple functionalities to create a smart, interactive clock. This project integrates real-time clock (RTC), weather monitoring, voice recognition, and multimedia playback capabilities to provide a feature-rich clock suitable for modern applications.

---

## Features

### Core Features
1. **Real-Time Clock (RTC)**
   - Keeps accurate time using the RTC DS1302 module.
   - Displays both standard and Hijri calendars.

2. **Weather Monitoring**
   - Reads temperature and humidity using the DHT11 sensor.

3. **Display Options**
   - **LCD (16x2)**: Displays time, calendar, and interaction prompts.
   - **Matrix Dot Display (MAX7219)**: Displays animations, text, and real-time data dynamically.

4. **Audio Integration**
   - Plays audio files such as alarms or recitations using the DFPlayer Mini module.
   - Integrates voice recognition using the DFRobot DF2301Q library to control modes or features.

5. **Motion Detection**
   - Utilizes a PIR sensor to detect movement and trigger display changes or interaction prompts.

6. **Alarm System**
   - Provides alarms using a buzzer and customizable alert times.

---

## Components

### Hardware
- **Microcontroller**: Arduino Uno or compatible board.
- **LCD**: 16x2 I2C-based Liquid Crystal Display.
- **RTC Module**: DS1302 Real-Time Clock module.
- **DHT11 Sensor**: Temperature and humidity sensor.
- **Matrix Dot Display**: MAX7219 for dynamic text and animations.
- **Audio Player**: DFPlayer Mini for audio playback.
- **Buzzer**: For alarms and notifications.
- **PIR Sensor**: For motion detection.

### Software Libraries
1. `DFRobot_DF2301Q` - For voice recognition.
2. `DFPlayer_Mini_Mp3` - For audio playback.
3. `DMD2` - For controlling matrix displays.
4. `LiquidCrystal_I2C` - For LCD control.
5. `RtcDS1302` - For managing RTC functionality.
6. `DHT` - For temperature and humidity readings.

---

## Wiring Diagram

| Component       | Pin Connections         |
|------------------|-------------------------|
| LCD I2C         | SDA to A4, SCL to A5   |
| RTC DS1302      | IO to Pin 4, CLK to Pin 5, CE to Pin 2 |
| DHT11           | Data to Pin 3          |
| Buzzer          | Pin 10                 |
| PIR Sensor      | Pin 12                 |
| DFPlayer Mini   | RX to Pin 16, TX to Pin 17 |
| MAX7219         | CS to Pin 5, CLK to Pin 18, DIN to Pin 23 |

---

## How to Use

1. **Hardware Setup**
   - Connect all components according to the wiring diagram.
   - Ensure proper power supply to the Arduino and connected modules.

2. **Software Installation**
   - Install the required libraries mentioned in the "Software Libraries" section.
   - Upload the code to the Arduino board using the Arduino IDE.

3. **Features Interaction**
   - **Time Display**: Displays the current time and date on the LCD and Matrix Dot display.
   - **Weather Data**: Shows temperature and humidity on the display.
   - **Alarm**: Set alarms and get notified via the buzzer.
   - **Voice Commands**: Use voice commands to control features like switching modes or playing audio.

---

## Future Improvements
- Add support for API integration to fetch prayer times or other data.
- Expand to include multiple alarms and advanced voice commands.
- Add WiFi functionality for remote monitoring and control.

---

## License
This project is open-source and distributed under the MIT License. Feel free to use, modify, and share it as needed.

---

## Author
[Your Name] - Developer of the Smart Clock project. Reach out for collaboration or questions!

