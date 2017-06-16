int t_boardLeft = 41;
int t_boardRight = 124;
int t_boardTop = 3;
int t_boardBottom = 156;
int t_XPos = 5;
int t_YPos = 0;
int t_moveUnits = 7; // each piece square is 4x4, with border it's 6x6, so 7 pixels moves to the next block
int t_XPosOld = 5;
int t_YPosOld = 0;
long t_score = 0;
long t_highScore = 0;
int t_linesPerLevel = 10;
int t_lines = t_linesPerLevel;
int t_oldLines = t_linesPerLevel;
int t_rowsCleared = 0;
int t_currentLevel = 1;
boolean t_pieceLocked = false;
boolean t_gameOver = false;
unsigned long t_moveDelay = 1000;
unsigned long t_lastMoveTime = 0;
int t_currentPiece = 0;
int t_nextPiece = 1;
int t_currentRotation = 0;
byte t_gameBoard[24][14];
unsigned int t_pieceColor[7] = {TEAL, BLUE, ORANGE, YELLOW, LIGHTGREEN, PURPLE, RED};
int t_notes[20] = {165, 196, 233, 277, 330, 392, 466, 523, 659, 784, 880, 1109, 1319, 1568, 1865, 2217, 2637, 3136, 3729, 4435};
int t_introSong[10] = {659, 165, 494, 523, 587, 659, 587, 523, 494, 440};
int t_introDurations[10] = {8,8,8,8,8,16,16,8,8,2};
int t_endSong[9] = {523, 587, 165, 659, 165, 523, 233, 440, 233};

void T_GameLoop() {
  if (gameSetupMode == true) { // set in the main Arduino loop once a game is selected
    gameSetupMode = false;
    T_ResetGame();
  } else {
    if (t_gameOver == false) {
      if (digitalRead(BUTTON1) == LOW) {
        if ((millis() - buttonPressed) > buttonDelay) {
          T_RotatePiece(t_XPos, t_YPos, true); // rotate left
          buttonPressed = millis();
        }
      } else if (digitalRead(BUTTON2) == LOW) {
        if ((millis() - buttonPressed) > buttonDelay) {
          T_RotatePiece(t_XPos, t_YPos, false); // rotate right
          buttonPressed = millis();
        }
      }
      if ((millis() - t_lastMoveTime) > t_moveDelay) {
        T_AutoMovePiece(); // drop the piece automatically on a timed delay
      } else {
        T_ReadJoystick();
      }
      T_ReadResetButton();
    } else {
      T_ReadResetButton();
      if (digitalRead(BUTTON1) == LOW) {
        if ((millis() - buttonPressed) > buttonDelay) {
          T_ResetGame();
          buttonPressed = millis();
        }
      }    
    }  
  }
}

void T_RotatePiece(int pX, int pY, boolean rotateLeft) {
  int origX = t_XPos;
  int origRotation = t_currentRotation;
  int collision = 0;
  T_ErasePiece(pX, pY);
  if (rotateLeft == true) {
    t_currentRotation -= 1;
    if (t_currentRotation < 0) {
      t_currentRotation = 3;
    }
  } else {
    t_currentRotation += 1;
    if (t_currentRotation > 3) {
      t_currentRotation = 0;
    }    
  }
  // handle wall kick - move right or left from wall and try to rotate again
  collision = T_CollisionCheck();
  if (collision == 0) { // no collision
    T_PlayRotatePiece();    
  } else if (collision == 8) { // hit left wall
    t_XPos++;  // try moving to the right one space and then rotate
    collision = T_CollisionCheck();
    if (T_CollisionCheck() == 0) {
      T_PlayRotatePiece();
    } else if (collision != 8) {
      t_XPos = origX;
      t_currentRotation = origRotation;
    } else { // try moving one more over
      t_XPos++;  
      collision = T_CollisionCheck();
      if (T_CollisionCheck() == 0) {
        T_PlayRotatePiece();
      } else {
        t_XPos = origX;
        t_currentRotation = origRotation;        
      }
    }
  } else if (collision == 9) { // hit right wall
    t_XPos--;  
    collision = T_CollisionCheck();
    if (T_CollisionCheck() == 0) {
      T_PlayRotatePiece();
    } else if (collision != 9) {
      t_XPos = origX;
      t_currentRotation = origRotation;
    } else {
      t_XPos--;  
      collision = T_CollisionCheck();
      if (T_CollisionCheck() == 0) {
        T_PlayRotatePiece();
      } else {
        t_XPos = origX;
        t_currentRotation = origRotation;        
      }
    }
  } else { // hit another piece
    t_XPos = origX;
    t_currentRotation = origRotation;    
  }
  T_DrawPiece(t_XPos, t_YPos, t_pieceColor[t_currentPiece]);
}

// Draw the current piece on the game board
void T_DrawPiece(int pX, int pY, int pColor) {
  for (int i=0; i < 4; i++) {
    for (int j=0; j < 4; j++) {
      if (t_pieces[t_currentPiece][t_currentRotation][i][j] > 0) {
        tft.fillRect((((pX * t_moveUnits) + t_boardLeft) + (j*t_moveUnits)), (((pY * t_moveUnits) + t_boardTop) + (i*t_moveUnits)), t_moveUnits, t_moveUnits, pColor);        
        tft.drawRect((((pX * t_moveUnits) + t_boardLeft) + (j*t_moveUnits)), (((pY * t_moveUnits) + t_boardTop) + (i*t_moveUnits)), t_moveUnits, t_moveUnits, GRAY);        
      }
    }  
  }
}

void T_ErasePiece(int pX, int pY) {
  for (int i=0; i < 4; i++) {
    for (int j=0; j < 4; j++) {
      if (t_pieces[t_currentPiece][t_currentRotation][i][j] > 0) {
        tft.fillRect((((pX * t_moveUnits) + t_boardLeft) + (j*t_moveUnits)), (((pY * t_moveUnits) + t_boardTop) + (i*t_moveUnits)), t_moveUnits, t_moveUnits, BLACK);        
      }
    }  
  }
}

// Draw the net piece preview in the top left of the screen
void T_DrawNextPiece(int pColor) {
  int pX = 15;
  int pY = 20;
  if (t_nextPiece == 0) {
    pX = 12;
    pY = 17;
  } else if (t_nextPiece == 3) {
    pX = 12;
    pY = 20;    
  }
  for (int i=0; i < 4; i++) {
    for (int j=0; j < 4; j++) {
      if (t_pieces[t_nextPiece][0][i][j] > 0) {
        tft.fillRect((pX + (j*t_moveUnits)), (pY + (i*t_moveUnits)), t_moveUnits, t_moveUnits, pColor);        
        tft.drawRect((pX + (j*t_moveUnits)), (pY + (i*t_moveUnits)), t_moveUnits, t_moveUnits, GRAY);        
      }
    }  
  }
}

void T_EraseNextPiece() {
  int pX = 15;
  int pY = 20;
  if (t_nextPiece == 0) {
    pX = 12;
    pY = 17;
  } else if (t_nextPiece == 3) {
    pX = 12;
    pY = 20;    
  }
  for (int i=0; i < 4; i++) {
    for (int j=0; j < 4; j++) {
      if (t_pieces[t_nextPiece][0][i][j] > 0) {
        tft.fillRect((pX + (j*t_moveUnits)), (pY + (i*t_moveUnits)), t_moveUnits, t_moveUnits, BLACK);        
      }
    }  
  }
}

void T_AutoMovePiece() {
  T_MoveDown();
  t_lastMoveTime = millis();  
}

void T_MoveLeft() {
  t_XPosOld = t_XPos;
  t_YPosOld = t_YPos;
  t_XPos -= 1;
  if (t_XPos <= 0) {
    t_XPos = 0;
  }
  if (T_CollisionCheck() > 0) {
    t_XPos += 1;
  } else {
    T_ErasePiece(t_XPosOld, t_YPosOld);
    T_DrawPiece(t_XPos, t_YPos, t_pieceColor[t_currentPiece]);
  }
}

void T_MoveRight() {
  t_XPosOld = t_XPos;
  t_YPosOld = t_YPos;
  t_XPos += 1;
  if (T_CollisionCheck() > 0) {
    t_XPos -= 1;
  } else {
    T_ErasePiece(t_XPosOld, t_YPosOld);
    T_DrawPiece(t_XPos, t_YPos, t_pieceColor[t_currentPiece]);
  }
}

void T_MoveDown() {
  t_XPosOld = t_XPos;
  t_YPosOld = t_YPos;
  t_YPos += 1;
  if (T_CollisionCheck() > 0) {
    T_PlayDroppedPiece();
    T_LockPiece();
  } else {
    T_ErasePiece(t_XPosOld, t_YPosOld);
    T_DrawPiece(t_XPos, t_YPos, t_pieceColor[t_currentPiece]);
  }
}

// Read the game board and see if we've hit another piece or a wall
int T_CollisionCheck() {
  int pX = 0;
  int pY = 0;
  for (int i=t_YPos; i < (t_YPos+4); i++) {
    for (int j=t_XPos; j < (t_XPos+4); j++) {
      if ((t_gameBoard[i][j] > 0) && (t_pieces[t_currentPiece][t_currentRotation][pY][pX] > 0)) {
        return (t_gameBoard[i][j]);
      }
      pX++;
    }
    pX = 0;
    pY++;
  }
  return false; 
}

// Piece has collided with another piece or the bottom of the game board
// Lock it in place by copying it to the gameboard
void T_LockPiece() {
  t_YPos -= 1;
  T_AddPieceToGameBoard();
  T_CheckClearedRows();
  if (t_rowsCleared > 0) {
    T_RefreshGameBoard();
  }
  T_CalculateScore();
  T_CheckEndGame();
  if (t_gameOver == false) {
    T_CheckLevel();
    t_XPos = 5;
    t_YPos = 0;
    t_XPosOld = t_XPos;
    t_YPosOld = t_YPos;
    t_currentRotation = 0;
    T_SelectNextPiece();
    T_DrawPiece(t_XPos, t_YPos, t_pieceColor[t_currentPiece]);
    t_pieceLocked = true;
  }
}

// See how many rows we've cleared so far
void T_CheckClearedRows() {
  int columnCount = 0;
  for (int i=2; i < 22; i++) {
    for (int j=2; j < 12; j++) {
      if (t_gameBoard[i][j] > 0) {
        columnCount++;
      }
      if (columnCount == 10) {
        t_rowsCleared++;
        T_ClearRow(i); 
      }
    }
    columnCount = 0;
  }  
}

// Remove a cleared row from the game board
void T_ClearRow(int row) {
  for (int i=row; i > 0; i--) {
    for (int j=2; j < 12; j++) {
      t_gameBoard[i][j] = t_gameBoard[i-1][j];
    }
  }
}

// Check the top 2 hidden rows to see if a piece exists. If so, a piece has gone off the top of the
// game board and the game is over
void T_CheckEndGame() {
  for (int i=0; i < 2; i++) {
    for (int j=2; j < 12; j++) {
      if ((t_gameBoard[i][j] > 0) && (t_gameOver == false)) {
        t_gameOver = true;
        T_EndGame();
      }
    }
  }  
}

void T_CalculateScore() {
  T_EraseScore();
  t_score += 10 * (t_currentLevel + 1); //score for dropping piece
  if (t_rowsCleared > 0) {
    T_PlayRowsCleared();
  }
  switch(t_rowsCleared) {
    case 1 :
      t_score += 40 * (t_currentLevel + 1);
      break;
    case 2 :
      t_score += 100 * (t_currentLevel + 1);
      break;
    case 3 :
      t_score += 300 * (t_currentLevel + 1);
      break;
    case 4 :
      t_score += 1200 * (t_currentLevel + 1);
      break;
  }
  T_DrawScore();
  t_oldLines = t_lines;
  t_lines -= t_rowsCleared;  
  t_rowsCleared = 0;
}

// Redraw the level indicator
void T_CheckLevel() {
  if (t_lines <= 0) {
    tft.setTextColor(BLACK);
    tft.setCursor(5, 80);
    tft.println(t_oldLines);
    t_oldLines = 0;
    tft.setTextColor(WHITE);
    tft.setCursor(5, 80);
    tft.println(t_oldLines);
    t_lines = t_linesPerLevel;
    t_moveDelay -= 75;
    if (t_moveDelay < 50) {
      t_moveDelay = 50;
    }
    delay(500);
    T_DropCurtain();
    tft.setTextColor(BLACK);
    tft.setCursor(5, 102);
    tft.println(t_currentLevel);
    t_currentLevel += 1;
    tft.setTextColor(WHITE);
    tft.setCursor(5, 102);
    tft.println(t_currentLevel);
    T_ClearGameBoard();
  }
  tft.setTextColor(BLACK);
  tft.setCursor(5, 80);
  tft.println(t_oldLines);
  tft.setTextColor(WHITE);
  tft.setCursor(5, 80);
  tft.println(t_lines);
}

// Once the level is passed (10 rows have been cleard), draw a curtain of blocks that drops down the screen
void T_DropCurtain() {
  int playNote = 0;
  boolean rowEmpty = true;
  for (int i=2; i < 22; i++) {
    for (int j=2; j < 12; j++) {
      if (t_gameBoard[i][j] > 0) {
        rowEmpty = false;
      }
    }
    if (rowEmpty == true) {
      for (int j=2; j < 12; j++) {
        tft.fillRect(((j * t_moveUnits) + t_boardLeft), ((i * t_moveUnits) + t_boardTop), t_moveUnits, t_moveUnits, TEAL);        
        tft.drawRect(((j * t_moveUnits) + t_boardLeft), ((i * t_moveUnits) + t_boardTop), t_moveUnits, t_moveUnits, GRAY);
      }
      T_PlayCurtainRow(playNote);
      delay(150);
      playNote++;
    } else {
      break;
    }
  }  
}

// Redraw the game board
void T_RefreshGameBoard() {
  tft.fillRect((t_boardLeft+14), (t_boardTop+14), ((t_boardRight+1)-(t_boardLeft+14)), ((t_boardBottom+1) - (t_boardTop+14)), BLACK);
  for (int i=2; i < 22; i++) {
    for (int j=2; j < 12; j++) {
      if (t_gameBoard[i][j] > 0) {
        tft.fillRect(((j * t_moveUnits) + t_boardLeft), ((i * t_moveUnits) + t_boardTop), t_moveUnits, t_moveUnits, t_pieceColor[(t_gameBoard[i][j]) - 1]);        
        tft.drawRect(((j * t_moveUnits) + t_boardLeft), ((i * t_moveUnits) + t_boardTop), t_moveUnits, t_moveUnits, GRAY);        
      }
    }
  }
}

// When a piece is locked (hits the bottom or another piece), this copies it to the game board
void T_AddPieceToGameBoard() {
  int pX = 0;
  int pY = 0;
  for (int i=t_YPos; i < (t_YPos+4); i++) {
    for (int j=t_XPos; j < (t_XPos+4); j++) {
      if (t_gameBoard[i][j] == 0) { // don't overwrite current pieces
        t_gameBoard[i][j] = t_pieces[t_currentPiece][t_currentRotation][pY][pX];
      }
      pX++;
    }
    pX = 0;
    pY++;
  }  
}

void T_SelectNextPiece() {
  t_currentPiece = t_nextPiece;
  T_EraseNextPiece();
  t_nextPiece = random(0, 7);
  T_DrawNextPiece(t_pieceColor[t_nextPiece]);
}

void T_ReadJoystick() {
  if ((millis() - joystickPressed) > joystickDelay) {
    if (digitalRead(JOYD) == LOW) {
      T_MoveDown();
    }  
    if (t_pieceLocked == false) {
      if (digitalRead(JOYL) == LOW) {
      T_MoveLeft();
      }
      if (digitalRead(JOYR) == LOW) {
      T_MoveRight();
      }
    } else {
      t_pieceLocked = false;
    }
    joystickPressed = millis();    
  }
}

void T_ReadResetButton() {
  if (digitalRead(RESETBUTTON) == LOW) {
    if ((millis() - buttonPressed) > buttonDelay) {
      mainMenuMode = true; // exit game
      tft.fillScreen(BLACK);
      buttonPressed = millis();
    }
  }
}

void T_EraseScore() {
  tft.setTextColor(BLACK);
  tft.setCursor(5, 124);
  tft.println(t_score);    
}

void T_DrawScore() {
  tft.setTextColor(WHITE);
  tft.setCursor(5, 124);
  tft.println(t_score);  
}

void T_PlayIntroSong() {
  if (soundON) {
    for (int thisNote = 0; thisNote < 10; thisNote++){
      int noteDuration = 1000/t_introDurations[thisNote];
      tone(SPEAKERPIN, t_introSong[thisNote],noteDuration);
      int pauseBetweenNotes = noteDuration * 1.40;
      delay(pauseBetweenNotes);
      noTone(SPEAKERPIN);
    }
  }
}

void T_PlayRotatePiece() {
  if (soundON) {
    tone(SPEAKERPIN, 100, 10);
    delay(10);
  }
}

void T_PlayDroppedPiece() {
  if (soundON) {
    tone(SPEAKERPIN, 100, 10);
    delay(10);
    tone(SPEAKERPIN, 700, 10);
    delay(10);
    tone(SPEAKERPIN, 500, 10);
    delay(10);
    tone(SPEAKERPIN, 200, 10);
    delay(10);
    tone(SPEAKERPIN, 800, 10);
    delay(10);
  }
}

void T_PlayRowsCleared() {
  if (soundON) {
    tone(SPEAKERPIN, 440, 35);
    delay(35);
    tone(SPEAKERPIN, 880, 35);
    delay(35);
    tone(SPEAKERPIN, 1760, 35);  
  }
}

void T_PlayCurtainRow(int row) {
  if (soundON) {
    tone(SPEAKERPIN, t_notes[row], 100);
  }
}

void T_PlayEndSong() {
  if (soundON) {
    for (int thisNote = 0; thisNote < 9; thisNote++){
      tone(SPEAKERPIN, t_endSong[thisNote], 125);
      delay(175);
      noTone(SPEAKERPIN);
    }
  }
}

void T_EndGame() {
  tft.fillRect(55, 45, 70, 63, BLACK);          
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.setCursor(55, 50);
  tft.print("GAME");
  tft.setCursor(55, 80);
  tft.print("OVER");
  if (t_score > t_highScore) {
    EEPROM.put(0, t_score);
  }
  T_PlayEndSong();
}

void T_ResetGame() {
  buttonPressed = 0;
  buttonDelay = 150;
  joystickPressed = 0;
  joystickDelay = 75;
  t_moveDelay = 1000;
  t_lastMoveTime = 0;
  t_currentRotation = 0;
  t_XPos = 5;
  t_YPos = 0;
  t_XPosOld = 5;
  t_YPosOld = 0;
  t_score = 0;
  t_currentLevel = 1;
  t_lines = t_linesPerLevel;
  t_rowsCleared = 0;
  t_gameOver = false;
  tft.fillScreen(BLACK);
  EEPROM.get(0, t_highScore);
  tft.setTextSize(1);
  tft.setTextColor(BLUE);
  tft.setCursor(8, 50);
  tft.println("Tetris");
  tft.setTextColor(WHITE);
  tft.setCursor(14, 9);
  tft.println("Next");
  tft.setCursor(3, 70);
  tft.println("Lines");
  tft.setCursor(5, 80);
  tft.println(t_lines);
  tft.setCursor(3, 92);
  tft.println("Level");
  tft.setCursor(5, 102);
  tft.println(t_currentLevel);
  tft.setCursor(3, 114);
  tft.println("Score");
  tft.setCursor(5, 124);
  tft.println("0");
  tft.setCursor(3, 138);
  tft.println("High");
  tft.setCursor(5, 148);
  tft.println(t_highScore);
  tft.drawRect(8, 5, 35, 35, WHITE);
  tft.fillRect(((t_boardLeft+14) - 3), (t_boardTop+14), 3, 160, WHITE);
  tft.fillRect((t_boardRight + 1), (t_boardTop+14), 3, 160, WHITE);
  tft.fillRect((t_boardLeft+14), (t_boardBottom + 1), 70, 3, WHITE);
  T_ClearGameBoard();
  t_nextPiece = random(0, 7);
  t_currentPiece = random(0, 7);
  T_DrawNextPiece(t_pieceColor[t_nextPiece]);
  T_PlayIntroSong();
  T_DrawPiece(t_XPos, t_YPos, t_pieceColor[t_currentPiece]);
  buttonPressed = millis();
  joystickPressed = millis();
}

void T_ClearGameBoard() {
  memcpy(t_gameBoard, t_backupGameBoard, sizeof t_backupGameBoard);
  tft.fillRect((t_boardLeft+14), (t_boardTop+14), ((t_boardRight+1)-(t_boardLeft+14)), ((t_boardBottom+1) - (t_boardTop+14)), BLACK);
}

