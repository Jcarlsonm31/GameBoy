int s_startingX = 16;
int s_startingY = 18;
int s_XPos = s_startingX;
int s_YPos = s_startingY;
int s_currentRotation = 0;
int s_boardTop = 16;
int s_moveUnits = 4;
int s_XPosOld;
int s_YPosOld;
long s_score = 0;
long s_highScore = 0;
int s_movesPerLevel = 50;
int s_moves = s_movesPerLevel;
int s_currentLevel = 1;
boolean s_gameOver = false;
unsigned long s_moveDelay;
unsigned long s_lastMoveTime = 0;
unsigned int s_gameBoard[36][2];
int s_notes[20] = {165, 196, 233, 277, 330, 392, 466, 523, 659, 784, 880, 1109, 1319, 1568, 1865, 2217, 2637, 3136, 3729, 4435};
int s_introSong[10] = {659, 165, 494, 523, 587, 659, 587, 523, 494, 440};
int s_introDurations[10] = {8,8,8,8,8,16,16,8,8,2};
int s_endSong[9] = {523, 587, 165, 659, 165, 523, 233, 440, 233};

void S_GameLoop() {
  if (gameSetupMode == true) {
    gameSetupMode = false;
    S_ResetGame();
  } else {
    if (s_gameOver == false) {
      if ((millis() - s_lastMoveTime) > s_moveDelay) {
        S_AutoMove();
      } else {
        S_ReadJoystick();
      }
      S_ReadResetButton();
    } else {
      S_ReadButton1();
      S_ReadResetButton();
    }  
  }
}

void S_DrawHead(int pX, int pY) {
  tft.fillRect((pX * s_moveUnits), (pY * s_moveUnits + s_boardTop), 4, 4, WHITE);
}

void S_DrawTail(int pX, int pY) {
  tft.fillRect((pX * s_moveUnits), (pY * s_moveUnits + s_boardTop), 4, 4, TFT_GREEN);
}

// random blockers to make the level more challenging
void S_DrawRandomPieces() {
  int x;
  int pX;
  int pY;
  int bitNum = 0;
  for (int lvl = s_currentLevel; lvl > 0; lvl--) {
    pX = random(1, 31);
    while (abs(pX-s_XPos) < 4) {      
      pX = random(1, 31);
    }
    pY = random(1, 35);
    while (abs(pY-s_YPos) < 4) {      
      pY = random(1, 31);
    }
    if (pX < 16) {
      x = 0;
      bitNum = 15 - pX;
    } else {
      x = 1;
      bitNum = 15 - (pX - 16);
    }
    s_gameBoard[pY][x] = (s_gameBoard[pY][x] | (1 << bitNum));
    tft.fillRect((pX * s_moveUnits), (pY * s_moveUnits + s_boardTop), 4, 4, WHITE);  
  }
}

void S_AutoMove() {
  S_MoveForward();
  s_lastMoveTime = millis();  
}

void S_MoveForward() {
  s_XPosOld = s_XPos;
  s_YPosOld = s_YPos;
  switch(s_currentRotation) {
    case 0 : // north
      s_YPos -= 1;
      if (S_CollisionCheck(s_XPos, s_YPos)) {
        S_EndGame();
      } else {
        S_AddTailToGameBoard(s_XPosOld, s_YPosOld);
        S_DrawTail(s_XPosOld, s_YPosOld);
        S_DrawHead(s_XPos, s_YPos);
        S_CalculateScore();
      }
      break;
    case 1 : // east
      s_XPos += 1;
      if (S_CollisionCheck(s_XPos, s_YPos)) {
        S_EndGame();
      } else {
        S_AddTailToGameBoard(s_XPosOld, s_YPosOld);
        S_DrawTail(s_XPosOld, s_YPosOld);
        S_DrawHead(s_XPos, s_YPos);
        S_CalculateScore();
      }
      break;
    case 2 : // south
      s_YPos += 1;
      if (S_CollisionCheck(s_XPos, s_YPos)) {
        S_EndGame();
      } else {
        S_AddTailToGameBoard(s_XPosOld, s_YPosOld);
        S_DrawTail(s_XPosOld, s_YPosOld);
        S_DrawHead(s_XPos, s_YPos);
        S_CalculateScore();
      }
      break;
    case 3 : // west
      s_XPos -= 1;
      if (S_CollisionCheck(s_XPos, s_YPos)) {
        S_EndGame();
      } else {
        S_AddTailToGameBoard(s_XPosOld, s_YPosOld);
        S_DrawTail(s_XPosOld, s_YPosOld);
        S_DrawHead(s_XPos, s_YPos);
        S_CalculateScore();
      }
      break;
  }  
}

void S_TurnLeft() {
  s_currentRotation -= 1;
  if (s_currentRotation < 0) {
    s_currentRotation = 3;
  }
}

void S_TurnRight() {
  s_currentRotation += 1;
  if (s_currentRotation > 3) {
    s_currentRotation = 0;
  }
}

// read gameboard to see if we're about to hit a 1-bit
boolean S_CollisionCheck(int pX, int pY) {
  int x;
  int bitNum = 0;
  if (pX < 16) {
    x = 0;
    bitNum = 15 - pX;
  } else {
    x = 1;
    bitNum = 15 - (pX - 16);
  }
  if (s_gameBoard[pY][x] & (1 << bitNum)) {
    return true;
  } else {
    return false;
  }
}

void S_CalculateScore() {
  S_EraseScore();
  s_score += (10 * s_currentLevel);
  S_DrawScore();
  S_EraseMoves();
  s_moves -= 1;
  S_DrawMoves();  
  S_CheckLevel();
}

void S_CheckLevel() {
  if (s_moves <= 0) {
    s_movesPerLevel += 10;
    s_moves = s_movesPerLevel;
    s_moveDelay -= 50;
    if (s_moveDelay < 50) {
      s_moveDelay = 50;
    }
    s_XPos = s_startingX;
    s_YPos = s_startingY;
    s_currentLevel += 1;
    tft.fillRect(20, 50, 95, 75, BLACK);          
    tft.setTextSize(3);
    tft.setTextColor(WHITE);
    tft.setCursor(25, 55);
    tft.print("Level");
    tft.setCursor(60, 85);
    tft.print(s_currentLevel);
    S_PlayRowsCleared();
    delay(3000);
    S_ClearGameBoard();
    S_DrawLevel();
    S_DrawMoves();
    S_DrawScore();
    S_DrawHighScore();
    S_DrawRandomPieces();
  }
}

// add a 1-bit to the appropriate spot on the game board using bitwise manipulation
void S_AddTailToGameBoard(int pX, int pY) {
  int x;
  int bitNum = 0;
  if (pX < 16) {
    x = 0;
    bitNum = 15 - pX;
  } else {
    x = 1;
    bitNum = 15 - (pX - 16);
  }
  s_gameBoard[pY][x] = (s_gameBoard[pY][x] | (1 << bitNum));
}

void S_ReadJoystick() {
  if ((millis() - joystickPressed) > joystickDelay) {
    if (digitalRead(JOYU) == LOW) {
      S_CalculateMove(0);
      joystickPressed = millis();    
    } else if (digitalRead(JOYR) == LOW) {
      S_CalculateMove(1);
      joystickPressed = millis();    
    } else if (digitalRead(JOYD) == LOW) {
      S_CalculateMove(2);
      joystickPressed = millis();    
    } else if (digitalRead(JOYL) == LOW) {
      S_CalculateMove(3);
      joystickPressed = millis();    
    }
  }
}

void S_ReadResetButton() {
  if (digitalRead(RESETBUTTON) == LOW) {
    if ((millis() - buttonPressed) > buttonDelay) {
      mainMenuMode = true; // exit game
      tft.fillScreen(BLACK);
      buttonPressed = millis();
    }
  }
}

void S_ReadButton1() {
  if (digitalRead(BUTTON1) == LOW) {
    if ((millis() - buttonPressed) > buttonDelay) {
      S_ResetGame();
      buttonPressed = millis();
    }
  }    
}

void S_CalculateMove(int s_joystickDirection) {
  switch(s_joystickDirection) {
    case 0 : //up
      if (s_currentRotation == 0) { //up
        // no-op
      } else if (s_currentRotation == 1) { //right
        S_TurnLeft();
      } else if (s_currentRotation == 2) { //down
        // no-op
      } else { //left
        S_TurnRight();
      }
      break;
    case 1 : //right
      if (s_currentRotation == 0) { //up
        S_TurnRight();
      } else if (s_currentRotation == 1) { //right
        // no-op
      } else if (s_currentRotation == 2) { //down
        S_TurnLeft();
      } else { //left
        // no-op
      }
      break;
    case 2 : //down
      if (s_currentRotation == 0) { //up
        // no-op
      } else if (s_currentRotation == 1) { //right
        S_TurnRight();
      } else if (s_currentRotation == 2) { //down
        // no-op
      } else { //left
        S_TurnLeft();
      }
      break;
    case 3 : //left
      if (s_currentRotation == 0) { //up
        S_TurnLeft();
      } else if (s_currentRotation == 1) { //right
        // no-op
      } else if (s_currentRotation == 2) { //down
        S_TurnRight();
      } else { //left
        // no-op
      }
      break;
  }    
}

void S_PlayIntroSong() {
  if (soundON) {
    for (int thisNote = 0; thisNote < 10; thisNote++){
      int noteDuration = 1000/s_introDurations[thisNote];
      tone(SPEAKERPIN, s_introSong[thisNote],noteDuration);
      int pauseBetweenNotes = noteDuration * 1.40;
      delay(pauseBetweenNotes);
      noTone(SPEAKERPIN);
    }
  }
}

void S_PlayRowsCleared() {
  if (soundON) {
    tone(SPEAKERPIN, 440, 35);
    delay(35);
    tone(SPEAKERPIN, 880, 35);
    delay(35);
    tone(SPEAKERPIN, 1760, 35);  
  }
}

void S_PlayCrashSound() {
  if (soundON) {
    analogWrite(VIBRATION, 100);
    delay(100);
    analogWrite(VIBRATION, 0);
    unsigned long S_time = millis();
    while ((millis() - S_time) <= 500) {
      tone(SPEAKERPIN, random(100, 1000));
    }  
    noTone(SPEAKERPIN);
  }
}

void S_PlayEndSong() {
  if (soundON) {
    for (int thisNote = 0; thisNote < 9; thisNote++){
      tone(SPEAKERPIN, s_endSong[thisNote], 125);
      delay(175);
      noTone(SPEAKERPIN);
    }
  }
}

void S_EndGame() {
  S_PlayCrashSound();
  delay(700);
  tft.fillRect(25, 50, 80, 65, BLACK);          
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.setCursor(30, 57);
  tft.print("GAME");
  tft.setCursor(30, 87);
  tft.print("OVER");
  if (s_score > s_highScore) {
    EEPROM.put(4, s_score);
  }
  S_PlayEndSong();
  s_gameOver = true;
}

void S_DrawScore() {
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(35, 0);
  tft.print(s_score);
}

void S_EraseScore() {
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.setCursor(35, 0);
  tft.print(s_score);
}

void S_DrawHighScore() {
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(35, 8);
  tft.print(s_highScore);
}

void S_EraseHighScore() {
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.setCursor(35, 8);
  tft.print(s_highScore);
}

void S_DrawLevel() {
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(109, 0);
  tft.print(s_currentLevel);
}

void S_EraseLevel() {
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.setCursor(109, 0);
  tft.print(s_currentLevel);
}

void S_DrawMoves() {
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(109, 8);
  tft.print(s_moves);  
}

void S_EraseMoves() {
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.setCursor(109, 8);
  tft.print(s_moves);  
}

void S_ResetGame() {
  buttonPressed = 0;
  buttonDelay = 200;
  joystickPressed = 0;
  joystickDelay = 50;
  S_ClearGameBoard();
  s_moveDelay = 400;
  s_movesPerLevel = 50;
  s_lastMoveTime = 0;
  s_currentRotation = 0;
  s_XPos = s_startingX;
  s_YPos = s_startingY;
  s_score = 0;
  s_currentLevel = 1;
  s_moves = s_movesPerLevel;
  s_gameOver = false;
  EEPROM.get(4, s_highScore);
  S_DrawLevel();
  S_DrawMoves();
  S_DrawScore();
  S_DrawHighScore();
  S_PlayIntroSong();
  S_DrawHead(s_XPos, s_YPos);
  S_DrawRandomPieces();
  buttonPressed = millis();
  joystickPressed = millis();
}

void S_ClearGameBoard() {
  memcpy(s_gameBoard, s_backupGameBoard, sizeof s_backupGameBoard);
  tft.fillScreen(BLACK);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(1, 0);
  tft.print("Score ");
  tft.setCursor(1, 8);
  tft.print("High ");
  tft.setCursor(76, 0);
  tft.print("Level ");
  tft.setCursor(76, 8);
  tft.print("Moves ");
  tft.fillRect(0, s_boardTop, 4, 144, WHITE);
  tft.fillRect(124, s_boardTop, 4, 144, WHITE);
  tft.fillRect(0, s_boardTop, 128, 4, WHITE);
  tft.fillRect(0, 156, 128, 4, WHITE);
}



