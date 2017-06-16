# GameBoy
An Arduino-based homemade GameBoy and several handmade games.

# Hardware
I used a Meduino, which is a smaller Arduino Mega clone. A mega is required because of the memory footprint of the games.
An Adafruit 1.8" ST7735 based screen with 160x128 resolution using SPI.
Graphics use the TFT_ST7735 library, which is optimized for speed. https://github.com/Bodmer/TFT_ST7735
3 buttons, two for A & B gameplay and a single button on top for game reset.
A 5-way switch for the joystick, with a custom Sugru molded top.
A 2xAA battery holder from a Yamaha guitar.
Sparkfun 5V voltage booster.
On/Off rocker switch.
Internal 1" speaker.
Small vibration motor for force-feedback.

# Software
The main GameBoy.ino file displays the game selection menu and drops into the setup routine of the selected game.

## Minotaur (Treasure of Tarmin)
Copy of the 1983 Intellivision game Treasure of Tarmin (later known as Minotaur)
Utilizes a pseudo-3D maze and pretty faithfully recreates the original game given the limitations of the Arduino and drawing speed. I'm happy with how it turned out.
I've heavily documented the code, but it's straight C and could obviously be done better. (I'm not a professional developer!)

## Tetris
A clone of Tetris

## Simon
A simple implementation of the 1970's handheld Simon game.

## Snake
A simple snake clone
