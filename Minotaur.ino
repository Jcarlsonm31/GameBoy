// Minotaur (known as Treasure of tarmin on the original Intellivision) - coded 2017
// Clone of the Intellivision game from 1983
// Requires an Arduino Mega due to memory footrpint
// Mostly faithful to the original with a few changes:
//  No containers and treasure because I never found it useful
//  No war/spiritual stats splits, everything is basically war-based to simplify the game a bit
//  Added an in-game progress map to help with level navigation
//  Had to infer monster stats and difficulty/level progression because of limited documentation about original

#define ITEM   0 // defines used for the various monster and item types
#define ARMOR  1
#define DOORMONSTER 2
#define LADDER 3
#define ARROWS 4
#define POTION 5
#define FLOUR  6
#define LAMP   7
#define EYEBALL 8
#define TREASURE 9
#define MONSTER 10
#define GAUNTLETS 11
#define HELMET 12
#define RING 13
#define SHIELD 14
#define WEAPON 15
#define NOMATCH 100 // used when searching for a monster at a given position and none are found
#define DRAW  1 // for dual purpose draw/erase functions
#define ERASE 0 
#define NORTH 0
#define EAST  1
#define SOUTH 2
#define WEST 3
#define OPEN 0 // defines for wall segment types, 2 is a DOORMONSTER
#define WALL 1
#define DOOR 3
unsigned char m_numLevels = 2; // total dungeon levels, although can keep going down and wraps at 256
unsigned char m_gameDifficulty = 1; // selected game difficulty
char m_difficultyChoice = 0;
int m_monsterHealth = 0;
unsigned char m_monsterAttacking = 0; // index of monster you're fighting in the m_monsters list for the level
boolean m_battleMode = false;
boolean m_playerTurn = false;
boolean m_gameOver = false;
char m_numMonsters = 0; // number of monsters on the level
char m_numItems = 0;    // number of items on the level
boolean m_doorOpen = false; // whether the door in front of you is open or not
unsigned char m_doorMonsterID = 0; // the m_monsters ID of the door monster on the level, or 0 if none present
unsigned long m_gameDuration; // used for stats tracking at end game
unsigned char m_gameMap[12][12]; // holds 4 random 6x6 sub-maps from the 16 possible
int m_progressMap[12]; // bitwise array used to track visited rooms on the map to produce in-game map
struct m_structRoomData m_roomData; // stores data about a room being drawn
struct m_structMonsters m_masterMonsters[18]; // master list of possible monsters
struct m_structItems m_masterItems[22];       // master list of possible items
struct m_structMonsters m_monsters[12]; // list of monsters on level: 6-10 monsters, 1 minotaur, 1 possible door monster
struct m_structItems m_items[24]; // list of items on level: 8 eyeballs, 2 ladders, 1 treasure tarmin, 10-12 items, 1 possible lamp
struct m_structPlayer m_player; // stores player info
struct m_structItems m_invArmor[5]; // armor in your inventory
struct m_structItems m_invWeapons[6]; // weapons in your inventory
struct m_structItems m_invItems[6]; // items in your inventory
boolean m_playerMessageDisplayed = false; // if a player info message is on the display
struct m_structVisibleCenterRooms m_visibleCenterRooms[5]; // tracks visible rooms for item drawing during M_DrawMaze()
unsigned int m_itemColors[4] = {WHITE, TEAL, YELLOW, ORANGE}; // possible monster/item colors by level

void M_GameLoop() {
  if (gameSetupMode == true) {
    M_SelectDifficulty();
  } else {
    if (m_gameOver == false) {
      M_HandleInputs();
    } else {
      M_ReadResetButton();
      if ((digitalRead(BUTTON1) == LOW) || (digitalRead(BUTTON2) == LOW)) {
        if ((millis() - buttonPressed) > buttonDelay) {
          gameSetupMode = true;
          tft.fillScreen(BLACK);
          buttonPressed = millis();
        }
      }    
    }  
  }
}

void M_HandleInputs() {
  M_ReadResetButton();
  M_ReadJoystick();
  M_ReadButton1();
  M_ReadButton2();
}

void M_ReadButton1() {
  unsigned char m_itemIndex;
  unsigned char segmentType;
  if (digitalRead(BUTTON1) == LOW) {
    if ((millis() - buttonPressed) > buttonDelay) {
      buttonPressed = millis();
      // see if theres anything in front of us to interact with
      m_itemIndex = M_CheckForItem(m_player.mapX, m_player.mapY);
      segmentType = M_ReadWall(m_player.mapX, m_player.mapY, m_player.forwardDir);
      // check for a closed door, or a door monster that is now dead (ID = 0)
      if (((segmentType == DOOR) || ((segmentType == DOORMONSTER) && (m_doorMonsterID == 0))) && (m_doorOpen == false)) {
        m_doorOpen = true;
        M_DrawMaze();
        M_CheckForBattleStart(); // is there now a monster in front of us?
        // check for door monster
      } else if ((segmentType == DOORMONSTER) && (m_doorMonsterID > 0) && (m_monsters[m_doorMonsterID].enabled == true)) {
          M_Battle(m_doorMonsterID, true); // (true) first strike the door monster
        // check for item on floor that isn't already in our inventory
      } else if ((m_itemIndex != NOMATCH) && (m_items[m_itemIndex].inInventory == false) && (m_items[m_itemIndex].canUse == true)) {
        M_PickupItem(m_itemIndex);
      }
    }
  }    
}

// manage your inventory
void M_ReadButton2() {
  if (digitalRead(BUTTON2) == LOW) {
    if ((millis() - buttonPressed) > buttonDelay) {
      buttonPressed = millis();
      M_ResetPlayerMessage(); // if any message was on the screen, clear it
      M_ManageInventory();
    }
  }    
}

void M_ReadResetButton() {
  if (digitalRead(RESETBUTTON) == LOW) {
    if ((millis() - buttonPressed) > buttonDelay) {
      mainMenuMode = true; // exit game
      tft.fillScreen(BLACK);
      buttonPressed = millis();
    }
  }
}

void M_ReadJoystick() {
  if ((millis() - joystickPressed) > joystickDelay) {
    if (digitalRead(JOYU) == LOW) {
      M_MoveForward();
      joystickPressed = millis();    
    } else if (digitalRead(JOYR) == LOW) {
      M_TurnRight();
      joystickPressed = millis();    
    } else if (digitalRead(JOYD) == LOW) {
      M_MoveBack();
      joystickPressed = millis();    
    } else if (digitalRead(JOYL) == LOW) {
      M_TurnLeft();
      joystickPressed = millis();    
    } else if (digitalRead(JOYB) == LOW) { // press the joystick inward
      joystickPressed = millis();    
      M_DrawProgressMap();
    }
  }
}

// move forward based on the player's direction, make sure we're not blocked
void M_MoveForward() {
  switch(m_player.forwardDir) {
    case NORTH:
      if (!M_ForwardBlocked()) {
        m_player.mapY -= 1;
        if (m_player.mapY < 0) {
          m_player.mapY += 1;
          M_PlayShortBeep();
        }
        m_doorOpen = false;
        M_DrawMaze();
        M_CheckForBattleStart();
      }
      break;
    case EAST:
      if (!M_ForwardBlocked()) {
        m_player.mapX += 1;
        if (m_player.mapX > 11) {
          m_player.mapX -= 1;
          M_PlayShortBeep();
        }
        m_doorOpen = false;
        M_DrawMaze();
        M_CheckForBattleStart();
      }
      break;
    case SOUTH:
      if (!M_ForwardBlocked()) {
        m_player.mapY += 1;
        if (m_player.mapY > 11) {
          m_player.mapY -= 1;
          M_PlayShortBeep();
        }
        m_doorOpen = false;
        M_DrawMaze();
        M_CheckForBattleStart();
      }
      break;
    case WEST:
      if (!M_ForwardBlocked()) {
        m_player.mapX -= 1;
        if (m_player.mapX < 0) {
          m_player.mapX += 1;
          M_PlayShortBeep();
        }
        m_doorOpen = false;
        M_DrawMaze();
        M_CheckForBattleStart();
      }
      break;
  }
  M_UpdateProgressMap();
}

// move backwards based on player's direction, make sure we're not blocked
void M_MoveBack() {
  unsigned char m_monsterIndex;
  switch(m_player.forwardDir) {
    case NORTH:
      if (!M_BackBlocked()) {
        m_player.mapY += 1;
        if (m_player.mapY > 11) {
          m_player.mapY -= 1;
          M_PlayShortBeep();
        }
        m_monsterIndex = M_CheckForMonster(m_player.mapX, m_player.mapY);
        if (m_monsterIndex != NOMATCH) { // there was a monster behind the player
          M_PlayShortBeep();
          m_player.mapY -= 1;
        }
        M_DrawMaze();
      }
      break;
    case EAST:
      if (!M_BackBlocked()) {
        m_player.mapX -= 1;
        if (m_player.mapX < 0) {
          m_player.mapX += 1;
          M_PlayShortBeep();
        }
        m_monsterIndex = M_CheckForMonster(m_player.mapX, m_player.mapY);
        if (m_monsterIndex != NOMATCH) {
          M_PlayShortBeep();
          m_player.mapX += 1;
        }
        M_DrawMaze();
      }
      break;
    case SOUTH:
      if (!M_BackBlocked()) {
        m_player.mapY -= 1;
        if (m_player.mapY < 0) {
          m_player.mapY += 1;
          M_PlayShortBeep();
        }
        m_monsterIndex = M_CheckForMonster(m_player.mapX, m_player.mapY);
        if (m_monsterIndex != NOMATCH) {
          M_PlayShortBeep();
          m_player.mapY += 1;
        }
        M_DrawMaze();
      }
      break;
    case WEST:
      if (!M_BackBlocked()) {
        m_player.mapX += 1;
        if (m_player.mapX > 11) {
          m_player.mapX -= 1;
          M_PlayShortBeep();
        }
        m_monsterIndex = M_CheckForMonster(m_player.mapX, m_player.mapY);
        if (m_monsterIndex != NOMATCH) {
          M_PlayShortBeep();
          m_player.mapX -= 1;
        }
        M_DrawMaze();
      }
      break;
  }
  M_UpdateProgressMap();
}

// Check if a wall or closed door blocks our path
boolean M_ForwardBlocked() {
  unsigned char segmentType;
  if (M_HasTeleportLamp()) {
    return false;
  } else {
    segmentType = M_ReadWall(m_player.mapX, m_player.mapY, m_player.forwardDir);
    if (segmentType == OPEN) {
      return false;
    } else if (((segmentType == DOOR) || ((segmentType == DOORMONSTER) && (m_doorMonsterID == 0))) && (m_doorOpen == true)) {
      return false;
    } else {
      M_PlayShortBeep();
      return true;
    }
  }
}

// Check if a wall or door blocks our path
boolean M_BackBlocked() {
  unsigned char segmentType;
  if (M_HasTeleportLamp()) {
    return false;
  } else {
    segmentType = M_ReadWall(m_player.mapX, m_player.mapY, m_player.backDir);
    if (segmentType == OPEN) {
      return false;
    } else {
      M_PlayShortBeep();
      return true;
    }
  }
}

// turn left and reset all player-relative directions
void M_TurnLeft() {
  M_DrawCompass(ERASE);
  m_player.forwardDir -= 1;
  if (m_player.forwardDir < 0) {
    m_player.forwardDir = 3;
  }
  M_SetPlayerDirections();
  M_DrawCompass(DRAW);
  m_doorOpen = false;
  M_DrawMaze();
  M_CheckForBattleStart();
}

// turn right and reset all player-relative directions
void M_TurnRight() {
  M_DrawCompass(ERASE);
  m_player.forwardDir += 1;
  if (m_player.forwardDir > 3) {
    m_player.forwardDir = 0;
  }
  M_SetPlayerDirections();
  M_DrawCompass(DRAW);
  m_doorOpen = false;
  M_DrawMaze();
  M_CheckForBattleStart();
}

// Handle monster battles
void M_Battle(unsigned char monster, boolean firstStrike) {
  boolean m_playerTurn = false;
  // you have a short delay to make the first strike, otherwise monster attacks first
  unsigned long battleStart = millis();
  unsigned long battleDelay = 2000 - (500 * (m_gameDifficulty-1));
  M_PlayBattleSound();
  m_battleMode = true;
  m_monsterAttacking = monster;
  m_monsterHealth = m_monsters[monster].hitPoints;
  M_DrawDashboard();
  if (firstStrike) { // used against door monsters, you always get first attack
    M_PlayerAttack(monster);
    battleDelay = 0;    
  }
  // delay to give player a chance to attack, otherwise monster attacks
  while ((millis() - battleStart) < battleDelay) {
    if (digitalRead(BUTTON1) == LOW) {
      if ((millis() - buttonPressed) > buttonDelay) {
        buttonPressed = millis();
        M_PlayerAttack(monster);
        battleDelay = 0;
      }
    } else if (digitalRead(BUTTON2) == LOW) {
      if ((millis() - buttonPressed) > buttonDelay) {
        buttonPressed = millis();
        M_ManageInventory();
      }
    } else if ((millis() - joystickPressed) > joystickDelay) {
      if (digitalRead(JOYD) == LOW) {
        M_Retreat();
        m_playerTurn = false;
        joystickPressed = millis();
      }
    }
  }
  while (m_battleMode == true) {
    if (m_playerTurn == true) {
      if (digitalRead(BUTTON1) == LOW) {
        if ((millis() - buttonPressed) > buttonDelay) {
          buttonPressed = millis();
          M_PlayerAttack(monster);
          m_playerTurn = false;
        }
      } else if (digitalRead(BUTTON2) == LOW) {
        if ((millis() - buttonPressed) > buttonDelay) {
          buttonPressed = millis();
          M_ManageInventory();
        }
      } else if ((millis() - joystickPressed) > joystickDelay) {
        if (digitalRead(JOYD) == LOW) {
          M_Retreat(); // pressed down, try to retreat
          m_playerTurn = false;
          joystickPressed = millis();
        }
      }
    } else if (m_monsters[monster].enabled == true) { // if monster is still alive (enabled), it attacks
      M_MonsterAttack(monster);
      m_playerTurn = true;
    }
  }
}

void M_PlayerAttack(unsigned char monster) {
  unsigned char roll;
  const unsigned char *m_weaponImage;
  roll = random(0,100);
  if (m_player.numInvWeapons > 0) {
    // if using bow/corssbow, check for arrows
    if ((m_invWeapons[m_player.currentWeapon].reusable == 2) && (m_player.numArrows <= 0)) {
      M_PlayShortBeep();
      M_DisplayPlayerMessage("No", "Arrows");
      M_ManageInventory();
      M_ResetPlayerMessage();
    } else {
      m_player.numAttacks += 1;
      // animate the weapon being thrown
      if (m_invWeapons[m_player.currentWeapon].reusable == 1) { // Book or scroll
        m_weaponImage = *(m_smallImages+15);
      } else if (m_invWeapons[m_player.currentWeapon].reusable == 2) { // Bow or crossbow
        m_weaponImage = *(m_smallImages+21);
      } else {
        m_weaponImage = m_invWeapons[m_player.currentWeapon].image;
      }
      M_DrawScaledBitmap(80, 82, m_weaponImage, 8, 1, m_invWeapons[m_player.currentWeapon].color);
      delay(80);
      M_DrawScaledBitmap(80, 82, m_weaponImage, 8, 1, BROWN);
      if (m_monsters[monster].itemType == MONSTER) { // don't draw first two weapon images if door monster, can't erase background correctly
        M_DrawScaledBitmap(75, 77, m_weaponImage, 8, 1, m_invWeapons[m_player.currentWeapon].color);
        delay(80);
        M_DrawScaledBitmap(75, 77, m_weaponImage, 8, 1, BROWN);
        M_DrawScaledBitmap(70, 72, m_weaponImage, 8, 1, m_invWeapons[m_player.currentWeapon].color);
        delay(80);
        M_DrawScaledBitmap(70, 72, m_weaponImage, 8, 1, BROWN);
      }
      // if roll is greater than the monster's armor class, it's a hit
      if (roll > m_monsters[monster].armorClass) {
        m_player.numHits += 1;
        m_monsters[monster].hitPoints -= m_invWeapons[m_player.currentWeapon].damage;
        m_monsterHealth = m_monsters[monster].hitPoints;
        if (m_monsterHealth < 0) {
          m_monsterHealth = 0;
        }
        M_CheckReusableWeapon(); // see if the weapon randomnly broke
        M_DrawDashboard();
        // draw the monster briefly in red when hit
        if (m_monsters[monster].itemType == DOORMONSTER) {
          M_DrawFlatDoor(1, 0, RED);  
        } else {
          M_DrawMapScaledBitmap(1, 1, m_monsters[monster].image, m_monsters[monster].imageSize, RED);
        }
        M_PlayHitSound(false); // false no vibrate
        if (m_monsters[monster].hitPoints > 0) {
          if (m_monsters[monster].itemType == DOORMONSTER) {
            M_DrawFlatDoor(1, 0, m_monsters[monster].color);  
          } else {
            M_DrawMapScaledBitmap(1, 1, m_monsters[monster].image, m_monsters[monster].imageSize, m_monsters[monster].color);              
          }
          delay(1000);
        } else { // monster was killed
          m_player.monstersKilled += 1;
          m_monsters[monster].enabled = false;
          m_battleMode = false;
          if (m_monsters[monster].color == PURPLE) {
            // Minotaur killed, drop the Treasure of tarmin
            m_items[m_numItems] = m_masterItems[21];
            m_items[m_numItems].mapX = m_monsters[monster].mapX;
            m_items[m_numItems].mapY = m_monsters[monster].mapY;
            m_numItems += 1;
          } else if (m_monsters[monster].itemType == DOORMONSTER) {
            // Door monster killed, drop magic lamp
            m_doorMonsterID = 0;
            m_items[m_numItems] = m_masterItems[18];
            m_items[m_numItems].mapX = m_player.mapX;
            m_items[m_numItems].mapY = m_player.mapY;
            m_items[m_numItems].level = m_monsters[monster].level;
            m_items[m_numItems].color = m_itemColors[m_monsters[monster].level-1];
            m_numItems += 1;
          }
          // Increase health after battle
          char m_newHealth = random(1,4);
          m_player.currentHealth += m_newHealth;
          m_player.maxHealth += m_newHealth;
          if (m_player.currentHealth > 199) {
            m_player.currentHealth = 199;
          }
          if (m_player.maxHealth > 199) {
            m_player.maxHealth = 199;
          }
          M_DrawDashboard();
          M_DrawMaze();
        }
      } else { // you missed
        M_PlayMissSound();
        delay(1000);
      }
      M_ResetPlayerMessage();
    }
  }
}

// if player attempted to move backwards, 50% chance of retreating
void M_Retreat() {
  if (random(0,100) > 49) {
    unsigned char m_monsterIndex;
    // make sure we aren't blocked moving back
    switch(m_player.forwardDir) {
      case NORTH:
        if (!M_BackBlocked()) {
          m_player.mapY += 1;
          m_monsterIndex = M_CheckForMonster(m_player.mapX, m_player.mapY+1);
          if (m_monsterIndex != NOMATCH) {
            M_PlayShortBeep();
            delay(1000);
            m_player.mapY -= 1;
          } else {
            m_battleMode = false;
            M_DrawDashboard();
            M_DrawMaze();
          }
        }
        break;
      case EAST:
        if (!M_BackBlocked()) {
          m_player.mapX -= 1;
          m_monsterIndex = M_CheckForMonster(m_player.mapX-1, m_player.mapY);
          if (m_monsterIndex != NOMATCH) {
            M_PlayShortBeep();
            delay(1000);
            m_player.mapX += 1;
          } else {
            m_battleMode = false;
            M_DrawDashboard();
            M_DrawMaze();
          }
        }
        break;
      case SOUTH:
        if (!M_BackBlocked()) {
          m_player.mapY -= 1;
          m_monsterIndex = M_CheckForMonster(m_player.mapX, m_player.mapY-1);
          if (m_monsterIndex != NOMATCH) {
            M_PlayShortBeep();
            delay(1000);
            m_player.mapY += 1;
          } else {
            m_battleMode = false;
            M_DrawDashboard();
            M_DrawMaze();
          }
        }
        break;
      case WEST:
        if (!M_BackBlocked()) {
          m_player.mapX += 1;
          m_monsterIndex = M_CheckForMonster(m_player.mapX+1, m_player.mapY);
          if (m_monsterIndex != NOMATCH) {
            M_PlayShortBeep();
            delay(1000);
            m_player.mapX -= 1;
          } else {
            m_battleMode = false;
            M_DrawDashboard();
            M_DrawMaze();
          }
        }
        break;
    }
  } else {
    M_PlayShortBeep();    
    delay(1000);
  }
}

// remove single use weapons from inventory and see if a reusable weapon randomnly broke
void M_CheckReusableWeapon() {
  if (m_invWeapons[m_player.currentWeapon].reusable == 0) {
    m_invWeapons[m_player.currentWeapon].inInventory = false;
    m_player.numInvWeapons -= 1;
    M_PackInventoryItems();
    if ((m_player.currentWeapon+1) > m_player.numInvWeapons) {
      m_player.currentWeapon = m_player.numInvWeapons - 1;
      if (m_player.currentWeapon < 0) {
        m_player.currentWeapon = 0;
      }
    }
  } else if (m_invWeapons[m_player.currentWeapon].reusable == 1) {
    M_CheckWeaponBreak();
  } else if (m_invWeapons[m_player.currentWeapon].reusable == 2) {
    m_player.numArrows -= 1;
    M_CheckWeaponBreak();
  }
}

void M_CheckWeaponBreak() {
  if (random(0,100) < (m_gameDifficulty)) {
    m_invWeapons[m_player.currentWeapon].inInventory = false;
    m_player.numInvWeapons -= 1;
    M_PackInventoryItems();
    if ((m_player.currentWeapon+1) > m_player.numInvWeapons) {
      m_player.currentWeapon = m_player.numInvWeapons - 1;
      if (m_player.currentWeapon < 0) {
        m_player.currentWeapon = 0;
      }
    }  
    M_PlayShortBeep();
    M_DisplayPlayerMessage("Weapon", "Broke!");
    delay(750);
  }
}

void M_MonsterAttack(unsigned char monster) {
  unsigned char roll;
  roll = random(0,100);
  // animate the monster's weapon being thrown
  if (m_monsters[monster].itemType == MONSTER) { // don't draw first two weapon images if door monster, can't erase background correctly
    M_DrawScaledBitmap(70, 72, m_monsters[monster].weaponImage, 8, 1, m_monsters[monster].color);
    delay(80);
    M_DrawScaledBitmap(70, 72, m_monsters[monster].weaponImage, 8, 1, BROWN);
    M_DrawScaledBitmap(75, 77, m_monsters[monster].weaponImage, 8, 1, m_monsters[monster].color);
    delay(80);
    M_DrawScaledBitmap(75, 77, m_monsters[monster].weaponImage, 8, 1, BROWN);
  }
  M_DrawScaledBitmap(80, 82, m_monsters[monster].weaponImage, 8, 1, m_monsters[monster].color);
  delay(80);
  M_DrawScaledBitmap(80, 82, m_monsters[monster].weaponImage, 8, 1, BROWN);
  // if roll above player's armor class, it's a hit
  if (roll > m_player.armorClass) {
    m_player.currentHealth -= random(1, m_monsters[monster].damage+1);
    tft.fillRect(0, 90, 128, 160, RED);
    M_PlayHitSound(true); // true to vibrate
    M_DrawDashboard();
    if (m_player.currentHealth <= 0) { // player was killed
      M_DrawHealth(ERASE);
      m_player.currentHealth = 0;
      M_DrawHealth(DRAW);
      m_battleMode = false;
      M_GameLost();
    }
  } else {
    M_PlayMissSound();
  }  
}

// use an item on the floor right in front of you
void M_PickupItem(unsigned char item) {
  boolean replaced = false;
  if (m_items[item].itemType == LADDER) { // go down to the next level
    M_PlayLadderSound();
    M_DrawLevel(ERASE);
    if (m_player.currentLevel == 255) { // levels wrap at 255
      m_player.currentLevel = 0;
    }
    m_player.currentLevel += 1;  
    M_DrawLevel(DRAW);
    M_InitMap();
    M_InitLevelItems();
    M_ClearProgressMap(); // reset in-game map for the next dungeon level
    M_UpdateProgressMap();
  } else if (m_items[item].itemType == FLOUR) { // flour immediately restores health
    if (m_player.currentHealth < m_player.maxHealth) {
      M_DrawHealth(ERASE);
      m_player.currentHealth += m_items[item].restorePoints;
      if (m_player.currentHealth > m_player.maxHealth) {
        m_player.currentHealth = m_player.maxHealth;
      }
      M_DrawHealth(DRAW);
      M_PlayPickupSound();
      m_items[item].inInventory = true;
    } else {
      M_PlayShortBeep();
      M_DisplayPlayerMessage("Full", "Health"); 
      delay(750);
    }
  } else if (m_items[item].itemType == ARROWS) {
    M_PlayPickupSound();
    m_player.numArrows += 20;
    m_items[item].inInventory = true;
    char m_arrowTmp[5];
    sprintf(m_arrowTmp, "%d", m_player.numArrows);
    M_DisplayPlayerMessage(m_arrowTmp, "Arrows");
    delay(750);
  } else if ((m_items[item].itemType == POTION) || (m_items[item].itemType == LAMP)) {
    for(int x=0; x<6; x++) { // Copy to inventory and mark original as picked up if inventory slot is free
      if ((m_invItems[x].inInventory == false) && (m_items[item].inInventory == false)){
        m_invItems[x] = m_items[item];
        m_items[item].inInventory = true;
        m_invItems[x].inInventory = true;
      }
    }
    if (m_items[item].inInventory == true) { // item was added, show the player message for the lamp type if picked up
      M_PlayPickupSound();
      if ((m_items[item].itemType == LAMP) && (m_items[item].level == 2)) {
        M_DisplayPlayerMessage("X-Ray", "Lamp"); 
        delay(750);    
      } else if ((m_items[item].itemType == LAMP) && (m_items[item].level == 3)) {
        M_DisplayPlayerMessage("Teleport", "Lamp"); 
        delay(750);            
      } else if ((m_items[item].itemType == LAMP) && (m_items[item].level == 4)) {
        M_UpgradeItemsLamp();
        M_DisplayPlayerMessage("Items", "Upgraded"); 
        delay(750);            
      }
      m_player.numInvItems += 1;
    } else { // inventory was full, so show the replace inventory item selector to swap for an existing item
      replaced = M_ReplaceItem(item);
      if (replaced == true) { // the player picked an item to replace, otherwise do nothing
        M_PlayPickupSound();
        if ((m_items[item].itemType == LAMP) && (m_items[item].level == 2)) {
          M_DisplayPlayerMessage("X-Ray", "Lamp"); 
          delay(750);    
        } else if ((m_items[item].itemType == LAMP) && (m_items[item].level == 3)) {
          M_DisplayPlayerMessage("Teleport", "Lamp"); 
          delay(750);            
        } else if ((m_items[item].itemType == LAMP) && (m_items[item].level == 4)) {
          M_UpgradeItemsLamp();
          M_DisplayPlayerMessage("Items", "Upgraded"); 
          delay(750);            
        }
      }
    }
    M_DrawDashboard();
  } else if (m_items[item].itemType == WEAPON) {
    for(int x=0; x<6; x++) { // Copy to inventory and mark original as picked up
      if ((m_invWeapons[x].inInventory == false) && (m_items[item].inInventory == false)){
        m_invWeapons[x] = m_items[item];
        m_items[item].inInventory = true;
        m_invWeapons[x].inInventory = true;
      }
    }
    if (m_items[item].inInventory == true) {
      M_PlayPickupSound();
      m_player.numInvWeapons += 1;
    } else { // inventory was full, show replace weapon selector to swap for existing weapon
      replaced = M_ReplaceWeapon(item);
      if (replaced == true) {
        M_PlayPickupSound();
      }
    }
    M_DrawDashboard();
  } else if (m_items[item].itemType == TREASURE) { // picked up the Treasure of Tarmin
    M_GameWon();
    return;
  } else if ((m_items[item].itemType == ARMOR) || (m_items[item].itemType == GAUNTLETS) || (m_items[item].itemType == HELMET)
            || (m_items[item].itemType == RING) || (m_items[item].itemType == SHIELD)) {
    for(int x=0; x<5; x++) { // Replace current armor type if it exists and the new one has a better armor class
      if ((m_invArmor[x].inInventory == true) && (m_invArmor[x].itemType == m_items[item].itemType) && (m_items[item].inInventory == false)){
        if (m_items[item].armorClass > m_invArmor[x].armorClass) {
          m_player.armorClass -= m_invArmor[x].armorClass;
          m_invArmor[x] = m_items[item];
          m_player.armorClass += m_items[item].armorClass;
        }  
        m_items[item].inInventory = true;
        M_PlayPickupSound();
      }
    }
    for(int x=0; x<5; x++) { // No armor of that type to replace, so copy to inventory and mark original as picked up
      if ((m_invArmor[x].inInventory == false) && (m_items[item].inInventory == false)){
        m_invArmor[x] = m_items[item];
        m_player.armorClass += m_items[item].armorClass;
        m_items[item].inInventory = true;
        m_invArmor[x].inInventory = true;
        M_PlayPickupSound();
      }
    }
    M_DrawDashboard();
  }
  M_DrawMaze();
}

// let the player choose an existing weapon to replace with the newly picked up weapon
boolean M_ReplaceWeapon(unsigned char item) {
  unsigned char m_slot = 0; // 6 slots in your weapon inventory
  boolean finished = false;
  struct m_structItems tmpItem;
  M_DrawItemChooserBox(WEAPON, m_slot, DRAW);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);        
  tft.setCursor(76, 77);
  tft.println(m_items[item].damage);  
  while (finished == false) {
    if (digitalRead(BUTTON1) == LOW) {
      // swap for the weapon highlighted
      if ((millis() - buttonPressed) > buttonDelay) {
        tmpItem = m_items[item];
        tmpItem.inInventory = true;
        m_items[item] = m_invWeapons[m_slot];
        m_invWeapons[m_slot] = tmpItem;
        m_invWeapons[m_slot].inInventory = true;
        m_items[item].inInventory = false;
        m_items[item].mapX = m_player.mapX;
        m_items[item].mapY = m_player.mapY;
        buttonPressed = millis();
        return true;
      }
    } 
    if (digitalRead(BUTTON2) == LOW) { // exit inventory management
      if ((millis() - buttonPressed) > buttonDelay) {
        buttonPressed = millis();
        return false;
      }
    }   
    if ((millis() - joystickPressed) > joystickDelay) { // move the weapon chooser box up or down
      if (digitalRead(JOYU) == LOW) {
        if (m_slot > 0) {
          M_DrawItemChooserBox(WEAPON, m_slot, ERASE);
          m_slot -= 1;
          M_DrawItemChooserBox(WEAPON, m_slot, DRAW);
        }
        joystickPressed = millis();    
      } else if (digitalRead(JOYD) == LOW) {
        if (m_slot < (m_player.numInvWeapons-1)) {
          M_DrawItemChooserBox(WEAPON, m_slot, ERASE);
          m_slot += 1;
          M_DrawItemChooserBox(WEAPON, m_slot, DRAW);
        }
        joystickPressed = millis();    
      }
    }
  }
}

// replace existing item with new one
boolean M_ReplaceItem(unsigned char item) {
  unsigned char m_slot = 0; // 6 item inventory slots
  boolean finished = false;
  struct m_structItems tmpItem;
  M_DrawItemChooserBox(ITEM, m_slot, DRAW);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);        
  tft.setCursor(76, 77);
  tft.println(m_items[item].restorePoints);      
  while (finished == false) {
    if (digitalRead(BUTTON1) == LOW) {
      if ((millis() - buttonPressed) > buttonDelay) {
        tmpItem = m_items[item];
        tmpItem.inInventory = true;
        m_items[item] = m_invItems[m_slot];
        m_invItems[m_slot] = tmpItem;
        m_invItems[m_slot].inInventory = true;          
        m_items[item].inInventory = false;
        m_items[item].mapX = m_player.mapX;
        m_items[item].mapY = m_player.mapY;
        buttonPressed = millis();
        return true;
      }
    } 
    if (digitalRead(BUTTON2) == LOW) { // exit inventory management
      if ((millis() - buttonPressed) > buttonDelay) {
        buttonPressed = millis();
        return false;
      }
    }   
    if ((millis() - joystickPressed) > joystickDelay) { // move the item chooser box up or down
      if (digitalRead(JOYU) == LOW) {
        if (m_slot > 0) {
          M_DrawItemChooserBox(ITEM, m_slot, ERASE);
          m_slot -= 1;
          M_DrawItemChooserBox(ITEM, m_slot, DRAW);
        }
        joystickPressed = millis();    
      } else if (digitalRead(JOYD) == LOW) {
        if (m_slot < (m_player.numInvItems-1)) {
          M_DrawItemChooserBox(ITEM, m_slot, ERASE);
          m_slot += 1;
          M_DrawItemChooserBox(ITEM, m_slot, DRAW);
        }
        joystickPressed = millis();    
      }
    }
  }
}

// manage your inventory to choose a new current weapon or use a potion or magic lamp
void M_ManageInventory() {
  char m_slot = 0; // 6 slots for weapons and items
  unsigned char column; // weapon column or item column
  boolean finished;
  struct m_structItems tmpItem;
  if (m_player.numInvWeapons > 0) { // set the initial location of item chooser box
    column = WEAPON;
    M_DrawItemChooserBox(column, m_slot, DRAW);
    finished = false;
  } else if (m_player.numInvItems > 0) {
    column = ITEM;
    M_DrawItemChooserBox(column, m_slot, DRAW);
    finished = false;
  } else {
    finished = true;
  }
  while (finished == false) {
    if (digitalRead(BUTTON1) == LOW) {
      if ((millis() - buttonPressed) > buttonDelay) {
        buttonPressed = millis();
        if (column == WEAPON) { // set a new current weapon from inventory
          tmpItem = m_invWeapons[0];
          m_invWeapons[0] = m_invWeapons[m_slot];
          m_invWeapons[m_slot] = tmpItem;
          m_player.currentWeapon = 0;
          M_DrawDashboard();
          M_PlayPickupSound();
          if (m_invWeapons[m_player.currentWeapon].reusable == 2) {
            char m_arrowTmp[5];
            sprintf(m_arrowTmp, "%d", m_player.numArrows);
            M_DisplayPlayerMessage(m_arrowTmp, "Arrows");
            delay(750);
            M_DrawMaze();
          }
          finished = true;
        } else { // show the lamp type or use the upgrade lamp
          if ((m_invItems[m_slot].itemType == LAMP) && (m_invItems[m_slot].level == 2)) {
            M_DisplayPlayerMessage("X-Ray", "Lamp"); 
            delay(750);    
            M_DrawMaze();
          } else if ((m_invItems[m_slot].itemType == LAMP) && (m_invItems[m_slot].level == 3)) {
            M_DisplayPlayerMessage("Teleport", "Lamp"); 
            delay(750);            
            M_DrawMaze();
          } else if ((m_invItems[m_slot].itemType == LAMP) && (m_invItems[m_slot].level == 4)) {
            M_PlayPickupSound();
            M_DisplayPlayerMessage("Items", "Upgraded"); 
            delay(750);            
            M_UpgradeItemsLamp();
            M_DrawDashboard();
            M_DrawItemChooserBox(column, m_slot, DRAW);
            M_DrawMaze();
          }
          if (m_invItems[m_slot].itemType == POTION) { // use a potion
            if (m_player.currentHealth >= m_player.maxHealth) {
              M_ResetPlayerMessage();
              M_PlayShortBeep();
              M_DisplayPlayerMessage("Full", "Health");  
              delay(750);            
              M_DrawDashboard();
              M_DrawMaze();
              finished = true;
            } else {
              m_player.currentHealth += m_invItems[m_slot].restorePoints;
              if (m_player.currentHealth > m_player.maxHealth) {
                m_player.currentHealth = m_player.maxHealth;  
              }
              m_invItems[m_slot].inInventory = false;
              m_player.numInvItems -= 1;
              M_PackInventoryItems();
              M_PlayPotionSound();
              M_DrawDashboard();
              finished = true;
            }
          }
        }
      }
    } 
    if (digitalRead(BUTTON2) == LOW) {
      if ((millis() - buttonPressed) > buttonDelay) {
        buttonPressed = millis();
        M_DrawDashboard();
        M_ResetPlayerMessage();
        return;
      }
    }   
    // move the item chooser box up/down inventory slots or left/right across columns
    if ((millis() - joystickPressed) > joystickDelay) {
      if (digitalRead(JOYU) == LOW) {
        if (m_slot > 0) {
          M_DrawItemChooserBox(column, m_slot, ERASE);
          m_slot -= 1;
          M_DrawItemChooserBox(column, m_slot, DRAW);
        }
        M_ResetPlayerMessage();
        joystickPressed = millis();    
      } else if (digitalRead(JOYD) == LOW) {
        if (((column == WEAPON) && (m_slot < (m_player.numInvWeapons-1))) || ((column == ITEM) && (m_slot < (m_player.numInvItems-1)))) {
          M_DrawItemChooserBox(column, m_slot, ERASE);
          m_slot += 1;
          M_DrawItemChooserBox(column, m_slot, DRAW);
        }
        M_ResetPlayerMessage();
        joystickPressed = millis();    
      } else if (digitalRead(JOYL) == LOW) {
        if (m_player.numInvWeapons > 0) {
          M_DrawItemChooserBox(column, m_slot, ERASE);
          if (m_slot > (m_player.numInvWeapons-1)) {
            m_slot = m_player.numInvWeapons-1;
          }
          column = WEAPON;
          M_DrawItemChooserBox(column, m_slot, DRAW);
        }
        M_ResetPlayerMessage();
        joystickPressed = millis();    
      } else if (digitalRead(JOYR) == LOW) {
        if (m_player.numInvItems > 0) {
          M_DrawItemChooserBox(column, m_slot, ERASE);
          if (m_slot > (m_player.numInvItems-1)) {
            m_slot = m_player.numInvItems-1;
          }
          column = ITEM;
          M_DrawItemChooserBox(column, m_slot, DRAW);
        }
        M_ResetPlayerMessage();
        joystickPressed = millis();    
      }
    }
  }
}

// Upgrade all armor, weapons and items to level 4
void M_UpgradeItemsLamp() {
  for (int x=0; x<5; x++) {
    if (m_invArmor[x].inInventory == true) {
      switch(m_invArmor[x].level) {
        case 1:
          m_invArmor[x].level = 4;
          m_player.armorClass += m_invArmor[x].armorClass * 3;
          m_invArmor[x].armorClass += m_invArmor[x].armorClass * 3;
          break;
        case 2:
          m_invArmor[x].level = 4;
          m_player.armorClass += m_invArmor[x].armorClass;
          m_invArmor[x].armorClass += m_invArmor[x].armorClass;
          break;
        case 3:
          m_invArmor[x].level = 4;
          m_player.armorClass += m_invArmor[x].armorClass / 3;
          m_invArmor[x].armorClass += m_invArmor[x].armorClass / 3;
          break;
      }
    }
  }
  for (int x=0; x<6; x++) {
    if (m_invWeapons[x].inInventory == true) {
      switch(m_invWeapons[x].level) {
        case 1:
          m_invWeapons[x].level = 4;
          m_invWeapons[x].damage += m_invWeapons[x].damage * 3;
          m_invWeapons[x].color = m_itemColors[3];
          break;
        case 2:
          m_invWeapons[x].level = 4;
          m_invWeapons[x].damage += m_invWeapons[x].damage;
          m_invWeapons[x].color = m_itemColors[3];
          break;
        case 3:
          m_invWeapons[x].level = 4;
          m_invWeapons[x].damage += m_invWeapons[x].damage / 3;
          m_invWeapons[x].color = m_itemColors[3];
          break;
      }      
    }
  }
  for (int x=0; x<6; x++) {
    if ((m_invItems[x].inInventory == true) && (m_invItems[x].itemType == POTION)) {
      switch(m_invItems[x].level) {
        case 1:
          m_invItems[x].level = 4;
          m_invItems[x].restorePoints += m_invItems[x].restorePoints * 3;
          m_invItems[x].color = m_itemColors[3];
          break;
        case 2:
          m_invItems[x].level = 4;
          m_invItems[x].restorePoints += m_invItems[x].restorePoints;
          m_invItems[x].color = m_itemColors[3];
          break;
        case 3:
          m_invItems[x].level = 4;
          m_invItems[x].restorePoints += m_invItems[x].restorePoints / 3;
          m_invItems[x].color = m_itemColors[3];
          break;
      }            
    }
  }  
}

// pack the inventory item slots starting at 0, to remove any gaps from used items
void M_PackInventoryItems() {
  for (int x=0; x<6; x++) {
    if (m_invWeapons[x].inInventory == false) {
      for (int y=x+1; y<6; y++) {      
        if (m_invWeapons[y].inInventory == true) {
          m_invWeapons[x] = m_invWeapons[y];
          m_invWeapons[y].inInventory = false;
          y = 6;
        }
      }
    }
  }
  for (int x=0; x<6; x++) {
    if (m_invItems[x].inInventory == false) {
      for (int y=x+1; y<6; y++) {      
        if (m_invItems[y].inInventory == true) {
          m_invItems[x] = m_invItems[y];
          m_invItems[y].inInventory = false;
          y = 6;
        }
      }
    }
  }
}

// check if a monster is 1 space in front of the player - start battle
void M_CheckForBattleStart() {
  unsigned char segmentType;
  switch(m_player.forwardDir) {
   case NORTH:
    for(int x=0; x < m_numMonsters; x++) {                                                                                 // Don't auto trigger door monster
      if ((m_monsters[x].enabled == true) && ((m_monsters[x].mapX == m_player.mapX) && (m_monsters[x].mapY == m_player.mapY-1)) && (m_monsters[x].itemType == MONSTER)) {
        // make sure a door or wall isn't blocking the monster from view
        segmentType = M_ReadWall(m_player.mapX, m_player.mapY, m_player.forwardDir);
        if ((segmentType == OPEN) || ((segmentType == DOOR) && (m_doorOpen == true))) {
          M_Battle(x, false);
        }        
      }
    }
    break;
   case EAST:
    for(int x=0; x < m_numMonsters; x++) {                                                                                 // Don't auto trigger door monster
      if ((m_monsters[x].enabled == true) && ((m_monsters[x].mapX == m_player.mapX+1) && (m_monsters[x].mapY == m_player.mapY)) && (m_monsters[x].itemType == MONSTER)) {
        segmentType = M_ReadWall(m_player.mapX, m_player.mapY, m_player.forwardDir);
        if ((segmentType == OPEN) || ((segmentType == DOOR) && (m_doorOpen == true))) {
          M_Battle(x, false);
        }        
      }
    }
    break;
   case SOUTH:
    for(int x=0; x < m_numMonsters; x++) {                                                                                 // Don't auto trigger door monster
      if ((m_monsters[x].enabled == true) && ((m_monsters[x].mapX == m_player.mapX) && (m_monsters[x].mapY == m_player.mapY+1)) && (m_monsters[x].itemType == MONSTER)) {
        segmentType = M_ReadWall(m_player.mapX, m_player.mapY, m_player.forwardDir);
        if ((segmentType == OPEN) || ((segmentType == DOOR) && (m_doorOpen == true))) {
          M_Battle(x, false);
        }        
      }
    }
    break;
   case WEST:
    for(int x=0; x < m_numMonsters; x++) {                                                                                 // Don't auto trigger door monster
      if ((m_monsters[x].enabled == true) && ((m_monsters[x].mapX == m_player.mapX-1) && (m_monsters[x].mapY == m_player.mapY)) && (m_monsters[x].itemType == MONSTER)) {
        segmentType = M_ReadWall(m_player.mapX, m_player.mapY, m_player.forwardDir);
        if ((segmentType == OPEN) || ((segmentType == DOOR) && (m_doorOpen == true))) {
          M_Battle(x, false);
        }        
      }
    }
    break;
  }    
}

// used for collision detection when moving
unsigned char M_CheckForMonster(unsigned char vX, unsigned char vY) {
  for(int x=0; x < m_numMonsters; x++) {                                                                 // Don't auto trigger door monster
    if ((m_monsters[x].enabled == true) && ((m_monsters[x].mapX == vX) && (m_monsters[x].mapY == vY)) && (m_monsters[x].itemType == MONSTER)) {
      return x;
    }
  }
  return NOMATCH;      
}

// check for an item at a given location, used to draw for M_DrawMaze();
unsigned char M_CheckForItem(unsigned char vX, unsigned char vY) {
  for(int y=m_numItems-1; y >= 0; y--) { // count backwards in case item is placed on top of hallway eyeball, will find item first
    if ((m_items[y].mapX == vX) && (m_items[y].mapY == vY)) {
      return y;
    }    
  }
  return NOMATCH;      
}

// see if the player has an X-Ray lamp in the inventory, can see monsters/items through the first layer of walls
boolean M_HasXrayLamp() {
  for (int x=0; x<6; x++) {
    if ((m_invItems[x].itemType == LAMP) && (m_invItems[x].inInventory == true) && (m_invItems[x].level == 2)) {
      return true;
    }
  }  
  return false;
}

// see if the player has a Teleport lamp in the inventory, can teleport through walls when moving
boolean M_HasTeleportLamp() {
  for (int x=0; x<6; x++) {
    if ((m_invItems[x].itemType == LAMP) && (m_invItems[x].inInventory == true) && (m_invItems[x].level == 3)) {
      return true;
    }
  }  
  return false;
}

// reset everything for a new game
void M_ResetGame() {
  tft.fillScreen(BLACK);  
  buttonDelay = 200;
  joystickDelay = 200;
  buttonPressed = millis();
  joystickPressed = millis();
  m_gameDuration = millis();
  m_gameOver = false;
  M_ResetPlayer();
  M_InitWorldItems();
  M_InitLevelItems();
  M_ResetInventory();
  M_ResetVisibleCenterRooms();
  M_DrawMaze();
  M_DrawDashboard();
  M_PlayStartGame();
}

// initialize the map, monsters and items at game start
void M_InitWorldItems() {
  M_InitMap();
  M_InitMasterMonsters();
  M_InitMasterItems();
  M_ClearProgressMap();
  M_UpdateProgressMap();
}

// reset player inventory at game start
void M_ResetInventory() {
  for (int x=0; x<5; x++) {
    m_invArmor[x].inInventory = false;
  }
  for (int x=0; x<6; x++) {
    m_invWeapons[x].inInventory = false;
  }
  for (int x=0; x<6; x++) {
    m_invItems[x].inInventory = false;
  }
  m_player.numInvItems = 0;
  m_player.numInvWeapons = 1;
  m_player.currentWeapon = 0;
  m_player.numArrows = 20;
  // Start with a bow
  m_invWeapons[0] = m_masterItems[9];
  m_invWeapons[0].inInventory = true;
}

// reset player stats at game start
void M_ResetPlayer() {
  m_player.currentLevel = 1;
  m_player.mapX = 2;
  m_player.mapY = 11;
  m_player.forwardDir = EAST;
  M_SetPlayerDirections();
  m_player.armorClass = 0;
  m_player.currentHealth = 20;
  m_player.maxHealth = 20;
  m_player.monstersKilled = 0;
  m_player.numAttacks = 0;
  m_player.numHits = 0;
}

// show the game won screen
void M_GameWon() {
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);        
  tft.setCursor(15, 2);
  tft.println("You won!");
  M_DrawScaledBitmap(45, 25, *(m_largeImages+17), 16, 2, YELLOW); 
  tft.setTextSize(1);
  tft.setTextColor(WHITE);        
  tft.setCursor(0, 80);
  tft.println("Monsters killed:");
  tft.setCursor(100, 80);
  tft.println(m_player.monstersKilled);
  tft.setCursor(0, 95);
  tft.println("Hit %:");
  tft.setCursor(40, 95);
  tft.println((int)(((float)m_player.numHits / (float)m_player.numAttacks) * 100));
  tft.setCursor(0, 110);
  tft.println("Time:");
  tft.setCursor(40, 110);
  int m_hours = ((millis() - m_gameDuration) % 86400000) / 3600000;
  int m_minutes = (((millis() - m_gameDuration) % 86400000) % 3600000) / 60000;
  char m_min[3];
  snprintf(m_min, 3, "%02d", m_minutes);
  tft.println(m_hours);
  tft.setCursor(45, 110);
  tft.println(":");
  tft.setCursor(50, 110);
  tft.println(m_min);  
  M_PlayGameWon();
  m_gameOver = true;
}

// show a message when the player dies
void M_GameLost() {
  M_PlayGameLost();
  M_DisplayPlayerMessage("You've", "Died!");
  m_gameOver = true;
}

// select game difficulty at startup
void M_SelectDifficulty() {
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(17, 15);
  tft.println("Minotaur"); 
  M_DrawScaledBitmap(30, 40, *(m_largeImages+8), 16, 2, PURPLE); 
  M_DrawScaledBitmap(65, 56, *(m_largeImages+17), 16, 1, YELLOW); 
  M_DrawDifficultyChoices(m_difficultyChoice);  
  M_DifficultyInput();
}

// manage the difficulty selection menu
void M_DrawDifficultyChoices(int menuChoice) {
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(25, 83);
  tft.print("Difficulty:");     
  if (menuChoice == 0) {
    tft.setTextColor(LIGHTGREEN);    
  } else {
    tft.setTextColor(WHITE);
  }
  tft.setCursor(40, 98);
  tft.print("2 levels");     
  if (menuChoice == 1) {
    tft.setTextColor(LIGHTGREEN);    
  } else {
    tft.setTextColor(WHITE);
  }
  tft.setCursor(40, 113);
  tft.print("4 levels");     
  if (menuChoice == 2) {
    tft.setTextColor(LIGHTGREEN);    
  } else {
    tft.setTextColor(WHITE);
  }
  tft.setCursor(40, 128);
  tft.print("8 levels");     
  if (menuChoice == 3) {
    tft.setTextColor(LIGHTGREEN);    
  } else {
    tft.setTextColor(WHITE);
  }
  tft.setCursor(34, 143);
  tft.print("12 levels");     
}

// read input on the difficulty selection menu
void M_DifficultyInput() {
  if (digitalRead(BUTTON1) == LOW) {
    if ((millis() - buttonPressed) > buttonDelay) {
      buttonPressed = millis();
      gameSetupMode = false;
      m_gameDifficulty = m_difficultyChoice + 1;
      switch (m_gameDifficulty) {
        case 1:
          m_numLevels = 2;
          break;
        case 2:
          m_numLevels = 4;
          break;
        case 3:
          m_numLevels = 8;
          break;
        case 4:
          m_numLevels = 12;
          break;
      }
      M_ResetGame();
    }
  } else if (digitalRead(JOYD) == LOW) {
      if ((millis() - joystickPressed) > joystickDelay) {
        m_difficultyChoice++;
        if (m_difficultyChoice > 3) {
          m_difficultyChoice = 3;
        }
        joystickPressed = millis();    
      }  
  } else if (digitalRead(JOYU) == LOW) {
      if ((millis() - joystickPressed) > joystickDelay) {
        m_difficultyChoice--;
        if (m_difficultyChoice < 0) {
          m_difficultyChoice = 0;
        }
        joystickPressed = millis();    
      }
  }
}

// various sound effects
void M_PlayShortBeep() {
  if (soundON) {
    tone(SPEAKERPIN, 100, 100);
  }
}

void M_PlayBattleSound() {
  if (soundON) {
    tone(SPEAKERPIN, 73, 50);
    delay(50);
    tone(SPEAKERPIN, 93, 50);
    delay(50);
    tone(SPEAKERPIN, 117, 50);
    delay(50);
    tone(SPEAKERPIN, 147, 50);
    delay(50);
    tone(SPEAKERPIN, 185, 50);
    delay(50);
    tone(SPEAKERPIN, 233, 50);
    delay(50);
    tone(SPEAKERPIN, 185, 50);
    delay(50);
    tone(SPEAKERPIN, 147, 50);
    delay(50);
    tone(SPEAKERPIN, 117, 50);
    delay(50);
    tone(SPEAKERPIN, 93, 50);
    delay(50);
    tone(SPEAKERPIN, 73, 50);
    delay(50);
  }
}

void M_PlayPickupSound() {
  if (soundON) {
    tone(SPEAKERPIN, 1245, 20);
  }
}

// also uses the vibration motor
void M_PlayHitSound(boolean m_vibrate) {
  if (soundON) {
    if (m_vibrate) {
      analogWrite(VIBRATION, 100);
      delay(50);
      analogWrite(VIBRATION, 0);
    }
    unsigned long S_time = millis();
    while ((millis() - S_time) <= 500) {
      tone(SPEAKERPIN, random(100, 1000));
    }  
    noTone(SPEAKERPIN);
  }
}

void M_PlayPotionSound() {
  if (soundON) {
    tone(SPEAKERPIN, 1175, 75);
    delay(75);
    tone(SPEAKERPIN, 1319, 75);
    delay(75);
    tone(SPEAKERPIN, 1480, 75);
  }
}

void M_PlayMissSound() {
  if (soundON) {
    tone(SPEAKERPIN, 104, 100);
    delay(200);
    tone(SPEAKERPIN, 73, 100);
  }
}

void M_PlayGameWon() {
  if (soundON) {
    tone(SPEAKERPIN, 294, 100);
    delay(300);
    tone(SPEAKERPIN, 294, 100);
    delay(150);
    tone(SPEAKERPIN, 294, 100);
    delay(150);
    tone(SPEAKERPIN, 587, 700);
  }
}

void M_PlayGameLost() {
  if (soundON) {
    tone(SPEAKERPIN, 147, 150);
    delay(300);
    tone(SPEAKERPIN, 113, 150);
    delay(300);
    tone(SPEAKERPIN, 104, 500);
  }
}

void M_PlayStartGame() {
  if (soundON) {
    tone(SPEAKERPIN, 73, 25);
    delay(30);
    tone(SPEAKERPIN, 93, 25);
    delay(30);
    tone(SPEAKERPIN, 117, 25);
    delay(30);
    tone(SPEAKERPIN, 147, 25);
    delay(30);
    tone(SPEAKERPIN, 185, 25);
    delay(30);
    tone(SPEAKERPIN, 233, 25);
    delay(30);
    tone(SPEAKERPIN, 294, 25);
    delay(30);
    tone(SPEAKERPIN, 370, 25);
    delay(30);
    tone(SPEAKERPIN, 466, 25);
    delay(30);
    tone(SPEAKERPIN, 587, 25);
    delay(30);
  }  
}

void M_PlayLadderSound() {
  if (soundON) {
    tone(SPEAKERPIN, 587, 25);
      analogWrite(VIBRATION, 100);
    delay(30);
      analogWrite(VIBRATION, 0);
    tone(SPEAKERPIN, 466, 25);
    delay(30);
    tone(SPEAKERPIN, 370, 25);
      analogWrite(VIBRATION, 75);
    delay(30);
      analogWrite(VIBRATION, 0);
    tone(SPEAKERPIN, 294, 25);
    delay(30);
    tone(SPEAKERPIN, 233, 25);
      analogWrite(VIBRATION, 50);
    delay(30);
      analogWrite(VIBRATION, 0);
    tone(SPEAKERPIN, 185, 25);
    delay(30);
    tone(SPEAKERPIN, 147, 25);
      analogWrite(VIBRATION, 25);
    delay(30);
      analogWrite(VIBRATION, 0);
    tone(SPEAKERPIN, 117, 25);
    delay(30);
    tone(SPEAKERPIN, 93, 25);
      analogWrite(VIBRATION, 10);
    delay(30);
      analogWrite(VIBRATION, 0);
    tone(SPEAKERPIN, 73, 25);
    delay(30);
  }  
}

