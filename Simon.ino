unsigned char si_currentLevel = 1;
unsigned char si_highScore = 0;
unsigned char si_currentMove = 0;
boolean si_gameOver = false;
boolean si_computerTurn = true;
int si_notes[4] = {82, 139, 165, 220}; //E2, C#3, E3, A3
unsigned char si_noteArray[50];

void Si_GameLoop() {
  if (gameSetupMode == true) {
    gameSetupMode = false;
    Si_ResetGame();
  } else {
    if (si_gameOver == false) {
      Si_PlayRound();
    } else {
      Si_ReadResetButton();
      Si_ReadButton1();
    }  
  }
}

// Computer plays a note, then the game waits for the user to select the same note
void Si_PlayRound() {
  if (si_computerTurn) {
    Si_ReadResetButton();
    Si_ReadButton1();
    Si_NextSequence();
  } else {
    while (si_currentMove < si_currentLevel) {
      Si_ReadResetButton();
      Si_ReadButton1();
      Si_ReadJoystick();
    }
    if (si_gameOver == false) {
      Si_CheckLevel();
    }
  }
}

// Add another note to the sequence
void Si_NextSequence() {
  si_noteArray[si_currentLevel - 1] = random(0, 4);
  Si_PlayNextSequence();
  si_computerTurn = false;
}

void Si_PlayNextSequence() {
  for (int x=0; x < si_currentLevel; x++) {
    Si_PlayNote(si_noteArray[x]);
    delay(250);
  }
}

void Si_PlayNote(unsigned char note) {
  switch(note) {
    case 0:
      Si_DrawUp(true);
      if (soundON) {
        tone(SPEAKERPIN, si_notes[0], 500);  
      }
      delay(500);
      Si_DrawUp(false);
      break;
    case 1:
      Si_DrawRight(true);
      if (soundON) {
        tone(SPEAKERPIN, si_notes[1], 500);  
      }
      delay(500);
      Si_DrawRight(false);
      break;
    case 2:
      Si_DrawDown(true);
      if (soundON) {
        tone(SPEAKERPIN, si_notes[2], 500);  
      }
      delay(500);
      Si_DrawDown(false);
      break;
    case 3:
      Si_DrawLeft(true);
      if (soundON) {
        tone(SPEAKERPIN, si_notes[3], 500);  
      }
      delay(500);
      Si_DrawLeft(false);
      break;
  }
}

void Si_CheckLevel() {
  if (si_currentLevel == 50) {
    Si_PlayLevelCleared();
    si_gameOver = true;
    Si_GameWon();
  } else {
    Si_EraseLevel();
    si_currentLevel += 1;
    Si_DrawLevel();
    si_currentMove = 0;
    Si_PlayLevelCleared();
    si_computerTurn = true;
  }
}

// Check the player's note against the current note in the sequence
void Si_ProcessMove(unsigned char note) {
  Si_PlayNote(note);
  if (note == si_noteArray[si_currentMove]) {
    si_currentMove++;
  } else {
    Si_GameLost();
  }
}

void Si_ReadJoystick() {
  if ((millis() - joystickPressed) > joystickDelay) {
    if (digitalRead(JOYU) == LOW) {
      Si_ProcessMove(0);      // up
      joystickPressed = millis();    
    } else if (digitalRead(JOYR) == LOW) {
      Si_ProcessMove(1);      // right
      joystickPressed = millis();    
    } else if (digitalRead(JOYD) == LOW) {
      Si_ProcessMove(2);      // down
      joystickPressed = millis();    
    } else if (digitalRead(JOYL) == LOW) {
      Si_ProcessMove(3);      // left
      joystickPressed = millis();    
    }
  }
}

void Si_ReadResetButton() {
  if (digitalRead(RESETBUTTON) == LOW) {
    if ((millis() - buttonPressed) > buttonDelay) {
      si_currentMove = si_currentLevel;
      si_gameOver = true;
      mainMenuMode = true; // exit game
      tft.fillScreen(BLACK);
      buttonPressed = millis();
    }
  }
}

void Si_ReadButton1() {
  if (digitalRead(BUTTON1) == LOW) {
    if ((millis() - buttonPressed) > buttonDelay) {
      Si_ResetGame();
      buttonPressed = millis();
    }
  }    
}

void Si_DrawUp(boolean lit) {
  if (lit) {
    tft.fillRoundRect(34, 50, 60, 25, 4, RED);
  } else {
    tft.fillRoundRect(34, 50, 60, 25, 4, DARKRED);    
  }
}

void Si_DrawRight(boolean lit) {
  if (lit) {
    tft.fillRoundRect(96, 74, 29, 56, 4, BLUE);
  } else {
    tft.fillRoundRect(96, 74, 29, 56, 4, DARKBLUE);    
  }  
}

void Si_DrawDown(boolean lit) {
  if (lit) {
    tft.fillRoundRect(34, 130, 60, 25, 4, YELLOW);
  } else {
    tft.fillRoundRect(34, 130, 60, 25, 4, DARKYELLOW);    
  }  
}

void Si_DrawLeft(boolean lit) {
  if (lit) {
    tft.fillRoundRect(3, 74, 29, 56, 4, LIGHTGREEN);
  } else {
    tft.fillRoundRect(3, 74, 29, 56, 4, DARKGREEN);    
  }  
}

void Si_PlayIntroSong() {
  if (soundON) {
    tone(SPEAKERPIN, 262, 100);  
    delay(125);
    tone(SPEAKERPIN, 262, 100);  
    delay(125);
    tone(SPEAKERPIN, 392, 100);  
    delay(125);
    tone(SPEAKERPIN, 523, 300);  
    delay(325);
    tone(SPEAKERPIN, 392, 100);  
    delay(125);
    tone(SPEAKERPIN, 523, 300);  
    delay(1000);
  }
}

void Si_PlayLevelCleared() {
  if (soundON) {
    delay(500);
    tone(SPEAKERPIN, 880, 100);  
    delay(125);
    tone(SPEAKERPIN, 880, 100);  
    delay(125);
    tone(SPEAKERPIN, 880, 100);  
    delay(1000);
  }  
}

void Si_PlayEndSong() {
  if (soundON) {
    tone(SPEAKERPIN, 82, 200);  
    delay(250);
    tone(SPEAKERPIN, 82, 200);  
    delay(250);
    tone(SPEAKERPIN, 82, 200);  
  }
}

void Si_GameWon() {
  si_currentMove = si_currentLevel;
  si_gameOver = true;
  tft.fillRect(25, 71, 80, 65, BLACK);          
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.setCursor(30, 78);
  tft.print("YOU");
  tft.setCursor(30, 108);
  tft.print("WON!");
  if (si_currentLevel > si_highScore) {
    EEPROM.put(8, si_currentLevel);
  }
  Si_PlayIntroSong();    
}

void Si_GameLost() {
  delay(250);
  si_currentMove = si_currentLevel;
  si_gameOver = true;
  tft.fillRect(25, 71, 80, 65, BLACK);          
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  if (si_currentLevel > si_highScore) {
    EEPROM.put(8, si_currentLevel);
    tft.setCursor(30, 78);
    tft.print("NEW");
    tft.setCursor(30, 108);
    tft.print("HIGH");
  } else {
    tft.setCursor(30, 78);
    tft.print("GAME");
    tft.setCursor(30, 108);
    tft.print("OVER");
  } 
  Si_PlayEndSong();  
}

void Si_DrawLevel() {
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(40, 30);
  tft.print(si_currentLevel);
}

void Si_EraseLevel() {
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.setCursor(40, 30);
  tft.print(si_currentLevel);
}

void Si_DrawHighScore() {
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(110, 30);
  tft.print(si_highScore);
}

void Si_EraseHighScore() {
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.setCursor(120, 30);
  tft.print(si_highScore);
}

void Si_ResetGame() {
  buttonDelay = 200;
  joystickDelay = 200;
  buttonPressed = millis();
  joystickPressed = millis();
  si_currentLevel = 1;
  si_currentMove = 0;
  si_gameOver = false;
  si_computerTurn = true;
  EEPROM.get(8, si_highScore);
  Si_ClearGameBoard();
  Si_PlayIntroSong();
}

void Si_ClearGameBoard() {
  tft.fillScreen(BLACK);  
  tft.setTextSize(2);
  tft.setTextColor(BLUE);
  tft.setCursor(33, 5);
  tft.println("Simon");
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(5, 30);
  tft.println("Level");
  tft.setCursor(80, 30);
  tft.println("High");
  Si_DrawLevel();
  Si_DrawHighScore();
  tft.drawRoundRect(33, 49, 62, 27, 4, GRAY);  // Button outline top
  tft.drawRoundRect(95, 73, 31, 58, 4, GRAY);  // right
  tft.drawRoundRect(33, 129, 62, 27, 4, GRAY); // bottom
  tft.drawRoundRect(2, 73, 31, 58, 4, GRAY);   // left
  Si_DrawUp(false);
  Si_DrawRight(false);
  Si_DrawDown(false);
  Si_DrawLeft(false);
}

