#include <SPI.h>
#include <EEPROM.h>
#include <TFT_ST7735.h> // TFT_ST7735 graphics library optimized for speed - https://github.com/Bodmer/TFT_ST7735
#include "TetrisPieces.h"
#include "SnakeBoard.h"
#include "MinotaurAssets.h"

// Uses an Arduino Mega clone called the Meduino, which is much smaller
#define BUTTON1 37
#define BUTTON2 35
#define VIBRATION 6
#define RESETBUTTON A13
#define JOYU    25
#define JOYD    14 
#define JOYL    39
#define JOYR    23
#define JOYB    41
#define SPEAKERPIN A15
#define TFT_CS     10
#define TFT_RST    9
#define TFT_DC     22
#define BLACK ST7735_BLACK
#define RED 0xF800
#define DARKRED 0xC020
#define YELLOW 0xF7E0
#define DARKYELLOW 0xCE80
#define ORANGE 0xFDC4
#define BLUE 0x041F
#define MEDBLUE 0x00DD
#define DARKBLUE 0x02B5
#define PURPLE 0xF81A
#define LIGHTGREEN 0x07E0
#define GREEN 0x07C0
#define MEDGREEN 0x1742
#define DARKGREEN 0x0520
#define TEAL 0x279F
#define TAN 0xFF73
#define WHITE 0xFFFF
#define GRAY 0xAD75
#define LIGHTGRAY 0xBDF7
#define BROWN 0x7421
TFT_ST7735 tft = TFT_ST7735(); //128x160 pixels screen, oriented vertically

// EEPROM Data
// 0-3 Tetris high score (long)
// 4-7 Snake high score (long)
// 8 Simon high score (char)
unsigned long buttonPressed = 0; // use delay timers to debounce buttons and joystick
unsigned long buttonDelay = 200;
unsigned long joystickPressed = 0;
unsigned long joystickDelay = 200;
unsigned long menuColorSelected = millis();
unsigned long menuColorDelay = 300;
#define NUMGAMES 5
char mainMenuChoices[NUMGAMES][15] = { "Minotaur", "Tetris", "Snake", "Simon", "Sound" };
char mainMenuChoice = 0;
boolean mainMenuMode = true;
boolean soundON = true;
boolean gameSetupMode = true;
char mainMenuColor = 0;
unsigned int mainMenuColors[7] = {BLUE, TEAL, GREEN, YELLOW, ORANGE, RED, PURPLE};

void setup(void) {
  Serial.begin(9600);
  randomSeed(analogRead(A3));
  pinMode(BUTTON1, INPUT);
  digitalWrite(BUTTON1, HIGH);
  pinMode(BUTTON2, INPUT);
  digitalWrite(BUTTON2, HIGH);
  pinMode(RESETBUTTON, INPUT);
  digitalWrite(RESETBUTTON, HIGH);
  pinMode(JOYU, INPUT);
  digitalWrite(JOYU, HIGH);
  pinMode(JOYD, INPUT);
  digitalWrite(JOYD, HIGH);
  pinMode(JOYL, INPUT);
  digitalWrite(JOYL, HIGH);
  pinMode(JOYR, INPUT);
  digitalWrite(JOYR, HIGH);
  pinMode(JOYB, INPUT);
  digitalWrite(JOYB, HIGH);
  pinMode(SPEAKERPIN, OUTPUT);
  digitalWrite(SPEAKERPIN, LOW);
  pinMode(VIBRATION, OUTPUT);
  digitalWrite(VIBRATION, LOW);
  tft.init();   // initialize a ST7735S chip, black tab
  tft.fillScreen(BLACK);
  buttonPressed = millis();
  joystickPressed = millis();
}

// Display the main game chooser menu
// Once a game is chosen, exit the main menu mode and run the game setup mode for the chosen game
void loop(void) {
  if (mainMenuMode == true) {
    buttonDelay = 200;
    joystickDelay = 200;
    DisplayMainMenu();
    MainMenuInput();
  } else {
    switch(mainMenuChoice) {
    case 0 :
      M_GameLoop();  // Minotaur
      break;
    case 1 :
      T_GameLoop();  // Tetris
      break;
    case 2 :
      S_GameLoop();  // Snake
      break;
    case 3 :
      Si_GameLoop(); // Simon
      break;
    }
  }
}

// Draw the main game chooser menu
void DisplayMainMenu() {
  if ((millis() - menuColorSelected) > menuColorDelay) {
    mainMenuColor += 1;
    if (mainMenuColor > 6) {
      mainMenuColor = 0;
    }
    menuColorSelected = millis();
  }
  tft.setTextSize(2);
  tft.setTextColor(mainMenuColors[mainMenuColor]);
  tft.setCursor(17, 15);
  tft.println("Game Boy"); 
  for (int x=0; x < NUMGAMES; x++) {
    DrawMainMenuChoices(x, mainMenuChoice);  
  }
}

// Draw the game list and highlight the current choice
void DrawMainMenuChoices(int currentItem, int menuChoice) {
  tft.setTextSize(1);
  if (currentItem == menuChoice) {
    tft.setTextColor(LIGHTGREEN);    
  } else {
    tft.setTextColor(WHITE);
  }
  tft.setCursor(20, (50+(currentItem * 15)));
  tft.print(currentItem+1);     
  tft.print(". ");     
  tft.setCursor(37, (50+(currentItem * 15)));
  if (currentItem == NUMGAMES-1) {
    if (soundON == true) {
      tft.print("Sound ON");     
    } else {      
      tft.print("Sound OFF");     
    }
  } else {    
    tft.print(mainMenuChoices[currentItem]);
  }
}

// Let the player choose a game
void MainMenuInput() {
  if (digitalRead(BUTTON1) == LOW) {
    if ((millis() - buttonPressed) > buttonDelay) {
      if (mainMenuChoice == NUMGAMES-1) {
        tft.setCursor(37, (50+(mainMenuChoice * 15)));
        tft.setTextColor(BLACK);
        if (soundON) {
          tft.print("Sound ON");     
          tft.setCursor(37, (50+(mainMenuChoice * 15)));
          tft.setTextColor(BLUE);
          tft.print("Sound OFF");     
        } else {
          tft.print("Sound OFF");     
          tft.setCursor(37, (50+(mainMenuChoice * 15)));
          tft.setTextColor(BLUE);
          tft.print("Sound ON");               
        }
        soundON = !soundON;
      } else {
        mainMenuMode = false; // start game
        gameSetupMode = true; // execute game setup mode
        tft.fillScreen(BLACK);
      }
      buttonPressed = millis();
    }
  } else if (digitalRead(JOYD) == LOW) {
      if ((millis() - joystickPressed) > joystickDelay) {
        mainMenuChoice++;
        if (mainMenuChoice > NUMGAMES-1) {
          mainMenuChoice = NUMGAMES-1;
        }
        joystickPressed = millis();    
      }  
  } else if (digitalRead(JOYU) == LOW) {
      if ((millis() - joystickPressed) > joystickDelay) {
        mainMenuChoice--;
        if (mainMenuChoice < 0) {
          mainMenuChoice = 0;
        }
        joystickPressed = millis();    
      }
  }
}

