// Draws the dashboard of player info at the bottom of the screen
void M_DrawDashboard() {
  tft.fillRect(0, 90, 128, 160, BLACK);
  if (m_battleMode == true) { // If attacking, draw monsters health instead of compass
     M_DrawMonsterHealth();
  } else {
    M_DrawCompass(DRAW);
    M_DrawLevel(DRAW);
  }
  M_DrawInventoryItems();
  M_DrawInventoryWeapons();
  M_DrawCurrentWeapon();
  M_DrawHealth(DRAW);
  M_DrawArmorClass(DRAW);
  tft.drawFastVLine(55, 91, 69, WHITE);
}

// Draw all of the weapons in your inventory
void M_DrawInventoryWeapons() {
  unsigned char invY = 94;
  tft.setTextSize(1);
  for(int x=0; x<6; x++) {
    if (m_invWeapons[x].inInventory == true) {
      tft.drawBitmap(65, invY, m_invWeapons[x].image, 8, 8, m_invWeapons[x].color);
      tft.setTextColor(m_invWeapons[x].color);    
      if (m_invWeapons[x].damage > 9) {
        tft.setCursor(76, invY);
      } else {
        tft.setCursor(82, invY);
      }
      tft.println(m_invWeapons[x].damage);
      invY += 11;        
    }
  }
}

// Draw all of the items in your inventory
void M_DrawInventoryItems() {
  unsigned char invY = 94;
  tft.setTextSize(1);
  for(int x=0; x<6; x++) {
    if (m_invItems[x].inInventory == true) {
      tft.drawBitmap(100, invY, m_invItems[x].image, 8, 8, m_invItems[x].color);
      tft.setTextColor(m_invItems[x].color);    
      if (m_invItems[x].restorePoints > 9) {
        tft.setCursor(111, invY);
      } else {
        tft.setCursor(117, invY);
      }
      if (m_invItems[x].itemType == LAMP) {
        switch(m_invItems[x].level) {
          case 2:
            tft.println("X"); // X-Ray lamp
            break;          
          case 3:
            tft.println("T"); // Teleport lamp
            break;          
          case 4:
            tft.println("U"); // Item upgrade lamp
            break;          
        }
      } else {
        tft.println(m_invItems[x].restorePoints);
      }
      invY += 11;        
    }
  }
}

// Draw the weapon selected as the current weapon
void M_DrawCurrentWeapon() {
  if (m_player.numInvWeapons == 0) {
    //no weapon
  } else {
    M_DrawScaledBitmap(30, 94, m_invWeapons[m_player.currentWeapon].image, 8, 2, m_invWeapons[m_player.currentWeapon].color); 
    tft.setTextSize(1);
    tft.setTextColor(m_invWeapons[m_player.currentWeapon].color);        
    if (m_invWeapons[m_player.currentWeapon].damage > 9) {
      tft.setCursor(33, 114);
    } else {
      tft.setCursor(35, 114);    
    }
    tft.println(m_invWeapons[m_player.currentWeapon].damage);  
  }
}

// Draw the direction the player is currently facing
void M_DrawCompass(char Draw) {
  tft.setTextSize(2);
  if (Draw == DRAW) {
    tft.setTextColor(TAN);    
  } else {
    tft.setTextColor(BLACK);        
  }
  tft.setCursor(1, 94);
  switch(m_player.forwardDir) {
    case NORTH:
      tft.println("N");  
      break;
    case EAST:
      tft.println("E");  
      break;
    case SOUTH:
      tft.println("S");  
      break;
    case WEST:
      tft.println("W");  
      break;      
  }
}

// While attacking, draw the monsters health instead of the compass
void M_DrawMonsterHealth() {
  M_DrawScaledBitmap(0, 94, m_monsters[m_monsterAttacking].image, 16, 1, m_monsters[m_monsterAttacking].color); 
  tft.setTextSize(1);
  tft.setTextColor(m_monsters[m_monsterAttacking].color);    
  if (m_monsterHealth > 9) {
    tft.setCursor(1, 114);
  } else {
    tft.setCursor(4, 114);    
  }
  tft.println(m_monsterHealth);        
}

// Draw the current dungeon level
void M_DrawLevel(char Draw) {
  tft.setTextSize(1);
  if (Draw == DRAW) {
    tft.setTextColor(TAN);    
  } else {
    tft.setTextColor(BLACK);        
  }
  if (m_player.currentLevel > 9) {
    tft.setCursor(1, 114);
  } else {
    tft.setCursor(4, 114);    
  }
  tft.println(m_player.currentLevel);  
}

// White for full health, orange for reduced health, red if you're dead
void M_DrawHealth(char Draw) {
  tft.setTextSize(1);
  if (Draw == DRAW) {
    if (m_player.currentHealth > 0) {
      M_DrawScaledBitmap(0, 130, m_masterItems[2].image, 8, 2, WHITE); 
      tft.setTextColor(WHITE);    
    } else {
      M_DrawScaledBitmap(0, 130, m_masterItems[2].image, 8, 2, RED); 
      tft.setTextColor(RED);    
    }
  } else {
    tft.setTextColor(BLACK);        
  }
  if (m_player.currentHealth > 9) {
    tft.setCursor(1, 150);
  } else {
    tft.setCursor(4, 150);    
  }
  tft.println(m_player.currentHealth);  
}

// Draw the player's current armor class
void M_DrawArmorClass(char Draw) {
  unsigned int clr;
  // set armor class color to match item level colors
  if (m_player.armorClass > 39) {
    clr = m_itemColors[3];
  } else if (m_player.armorClass > 26) {
    clr = m_itemColors[2];
  } else if (m_player.armorClass > 13) {
    clr = m_itemColors[1];
  } else {
    clr = m_itemColors[0];
  }
  tft.setTextSize(1);
  if (Draw == DRAW) {
    tft.setTextColor(clr);    
  } else {
    tft.setTextColor(BLACK);        
  }
  if (m_player.armorClass > 9) {
    tft.setCursor(33, 150);
  } else {
    tft.setCursor(35, 150);    
  }
  M_DrawScaledBitmap(30, 130, m_masterItems[8].image, 8, 2, clr); 
  tft.println(m_player.armorClass);  
}

// Box around selected item when managing your inventory
void M_DrawItemChooserBox(unsigned char itemType, unsigned char m_slot, char draw) {
  int x;
  int y = 92;
  unsigned int clr;
  if (itemType == WEAPON) {
    x = 63;
  } else {
    x = 98;
  }
  y += m_slot*11;
  if (draw == DRAW) {
    clr = WHITE;    
  } else {
    clr = BLACK;        
  }
  tft.drawRect(x, y, 26, 12, clr);
}

// Draw a message for the player over the maze portion of the screen
void M_DisplayPlayerMessage(const char* m_message1, const char* m_message2) {
  char length1 = strlen(m_message1);
  char length2 = strlen(m_message2);
  unsigned char xPos;
  if (length1 > length2) { // Center the text on the screen
    xPos = (128 - (length1 * 12)) / 2;
  } else {
    xPos = (128 - (length2 * 12)) / 2;    
  }
  tft.fillRect(0, 20, 128, 55, BLACK);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);    
  tft.setCursor(xPos, 30);
  tft.println(m_message1);  
  tft.setCursor(xPos, 50);
  tft.println(m_message2);  
  m_playerMessageDisplayed = true;
}

// Remove the player message and redraw the maze
void M_ResetPlayerMessage() {
  if (m_playerMessageDisplayed == true) {
    m_playerMessageDisplayed = false;
    M_DrawMaze();
  }
}

// All rooms and walls are represented by a 3x3 grid with the player at the bottom center position
// Room, Room, Room - row (depth) 2
// Room, Room, Room - row (depth) 1
// Room, Player, Room - row (depth) 0
//
// Row (depth) is the Y (0-2 with 0 being the player row) on that 3x3 grid
// Column is the X on the 3x3 grid (0 being left of player, 1 being the center column, 2 being the right)
// There are 3 rows and 3 columns of flat walls facing player - Row(depth)0, 1, 2 - Column 0 left, 1 center, 2 right
// There are 3 rows and 4 columns of side walls - Row(depth)0, 1, 2 - Column 0 far left, 1 near left, 2 near right, 3 far right
// Some walls & items need to be deferred in drawing until later in the routine for correct ordering
// This routine is incredily ugly, but it correctly draws walls and items in the correct order for visibility
void M_DrawMaze() {
  unsigned char segmentType;
  unsigned char row0FlatMiddle;
  boolean row1FlatLeft = false;
  boolean row1FlatRight = false;
  boolean row0SideLeft = false;
  boolean row0SideRight = false;
  m_playerMessageDisplayed = false; // if there was a player message on the screen, it'll be gone now
  
  tft.fillRect(0, 0, 128, 90, BROWN);
  // row 0 center
  // M_SetVisibleCenterRoom tracks visible center rooms to draw items in those rooms later in routine
  M_SetVisibleCenterRoom(0, M_PlayerVirtualX(0, 0), M_PlayerVirtualY(0, 0), 1, 0);
  row0FlatMiddle = M_ReadWall(m_player.mapX, m_player.mapY, m_player.forwardDir);  
  if ((row0FlatMiddle != OPEN) && (m_doorOpen == false)) {
    // Draw flat middle at end of drawing routine
  } else {
    // row 1 center
    M_SetVisibleCenterRoom(1, M_PlayerVirtualX(0, 1), M_PlayerVirtualY(0, 1), 1, 1);
    segmentType = M_ReadWall(M_PlayerVirtualX(0, 1), M_PlayerVirtualY(0, 1), m_player.forwardDir);  
    if (segmentType != OPEN) {
      M_DrawFlatSegment(1, 1, segmentType);
    } else {
      // row 2 left
      segmentType = M_ReadWall(M_PlayerVirtualX(0, 2), M_PlayerVirtualY(0, 2), m_player.leftDir);  
      if (segmentType != OPEN) {
        M_DrawSideSegment(1, 2, segmentType);
      } else {
        segmentType = M_ReadWall(M_PlayerVirtualX(-1, 2), M_PlayerVirtualY(-1, 2), m_player.forwardDir);  
        if (segmentType != OPEN) {
          M_DrawFlatSegment(0, 2, segmentType);
        } else {
          M_DrawFlatWall(0, 2, BLACK);        
        }
      }
      // row 2 right
      segmentType = M_ReadWall(M_PlayerVirtualX(0, 2), M_PlayerVirtualY(0, 2), m_player.rightDir);  
      if (segmentType != OPEN) {
        M_DrawSideSegment(2, 2, segmentType);
      } else {
        segmentType = M_ReadWall(M_PlayerVirtualX(1, 2), M_PlayerVirtualY(1, 2), m_player.forwardDir);  
        if (segmentType != OPEN) {
          M_DrawFlatSegment(2, 2, segmentType);
        } else {
          M_DrawFlatWall(2, 2, BLACK);        
        }
      }
      // row 2 center
      segmentType = M_ReadWall(M_PlayerVirtualX(0, 2), M_PlayerVirtualY(0, 2), m_player.forwardDir);  
      if (segmentType != OPEN) {
        M_DrawFlatSegment(1, 2, segmentType);
      } else {
        M_DrawFlatWall(1, 2, BLACK);      
      }
      M_SetVisibleCenterRoom(2, M_PlayerVirtualX(0, 2), M_PlayerVirtualY(0, 2), 1, 2);
    }
    if ((row0FlatMiddle == DOOR) && (m_doorOpen == true)) {
      // Don't draw row 1 walls, hidden by row 0 door frame
    } else {
      // row 1 left
      segmentType = M_ReadWall(M_PlayerVirtualX(0, 1), M_PlayerVirtualY(0, 1), m_player.leftDir);  
      if (segmentType != OPEN) {
        M_DrawSideSegment(1, 1, segmentType);
      } else {
        row1FlatLeft = true;
        segmentType = M_ReadWall(M_PlayerVirtualX(-1, 1), M_PlayerVirtualY(-1, 1), m_player.forwardDir);  
        if (segmentType != OPEN) {
          M_DrawFlatSegment(0, 1, segmentType);
        } else {
          M_DrawFlatWall(0, 1, BLACK);        
        }
      }
      // row 1 right
      segmentType = M_ReadWall(M_PlayerVirtualX(0, 1), M_PlayerVirtualY(0, 1), m_player.rightDir);  
      if (segmentType != OPEN) {
        M_DrawSideSegment(2, 1, segmentType);
      } else {
        row1FlatRight = true;
        segmentType = M_ReadWall(M_PlayerVirtualX(1, 1), M_PlayerVirtualY(1, 1), m_player.forwardDir);  
        if (segmentType != OPEN) {
          M_DrawFlatSegment(2, 1, segmentType);
        } else {
          M_DrawFlatWall(2, 1, BLACK);        
        }
      }
    }
  }  
  // row 0 left
  segmentType = M_ReadWall(m_player.mapX, m_player.mapY, m_player.leftDir);  
  if (segmentType != OPEN) {
    M_DrawSideSegment(1, 0, segmentType);
    row0SideLeft = true;
  } else {
    segmentType = M_ReadWall(M_PlayerVirtualX(-1, 0), M_PlayerVirtualY(-1, 0), m_player.forwardDir);  
    if (segmentType != OPEN) {
      M_DrawFlatSegment(0, 0, segmentType);
    } else {
      if (row1FlatLeft == false) {
        segmentType = M_ReadWall(M_PlayerVirtualX(-1, 1), M_PlayerVirtualY(-1, 1), m_player.leftDir);  
        if (segmentType != OPEN) {
          M_DrawSideSegment(0, 1, segmentType);
        } else {
          M_DrawSideWall(0, 1, BLACK);        
        }
      }
      if ((row0FlatMiddle == DOOR) && (m_doorOpen == true) && ((row1FlatLeft == true) || (row0SideLeft == true))) {
        // Don't draw items when door open and can't see them
      } else {
        M_DrawItems(M_PlayerVirtualX(-1, 1), M_PlayerVirtualY(-1, 1), 0, 1);
      }
    }
  }
  // row 0 right
  segmentType = M_ReadWall(m_player.mapX, m_player.mapY, m_player.rightDir);  
  if (segmentType != OPEN) {
    M_DrawSideSegment(2, 0, segmentType);
    row0SideRight = true;
  } else {
    segmentType = M_ReadWall(M_PlayerVirtualX(1, 0), M_PlayerVirtualY(1, 0), m_player.forwardDir);  
    if (segmentType != OPEN) {
      M_DrawFlatSegment(2, 0, segmentType);
    } else {
      if (row1FlatRight == false) {
        segmentType = M_ReadWall(M_PlayerVirtualX(1, 1), M_PlayerVirtualY(1, 1), m_player.rightDir);  
        if (segmentType != OPEN) {
          M_DrawSideSegment(3, 1, segmentType);
        } else {
          M_DrawSideWall(3, 1, BLACK);        
        }
      }
      if ((row0FlatMiddle == DOOR) && (m_doorOpen == true) && ((row1FlatRight == true) || (row0SideRight == true))) {
        // Don't draw items when door open and can't see them
      } else {
        M_DrawItems(M_PlayerVirtualX(1, 1), M_PlayerVirtualY(1, 1), 2, 1);
      }
    }
  }
  row0FlatMiddle = M_ReadWall(m_player.mapX, m_player.mapY, m_player.forwardDir);  
  if (row0FlatMiddle != OPEN) { 
    M_DrawFlatSegment(1, 0, row0FlatMiddle);
  }
  M_DrawVisibleCenterItems();
  // If you have the X-Ray lamp, draw first row of monsters/items over the walls
  if (M_HasXrayLamp() == true) {
    M_DrawItems(M_PlayerVirtualX(-1, 1), M_PlayerVirtualY(-1, 1), 0, 1);
    M_DrawItems(M_PlayerVirtualX(0, 1), M_PlayerVirtualY(0, 1), 1, 1);
    M_DrawItems(M_PlayerVirtualX(1, 1), M_PlayerVirtualY(1, 1), 2, 1);
  }
}

// Draw monsters and items in the maze
void M_DrawItems(unsigned char vX, unsigned char vY, unsigned char sColumn, unsigned char sRow) {
  for(int x=0; x < m_numMonsters; x++) {
    if ((m_monsters[x].mapX == vX) && (m_monsters[x].mapY == vY)) {
      if ((m_monsters[x].enabled == true) && (m_monsters[x].itemType == MONSTER)) { //not a door monster
        M_DrawMapScaledBitmap(sColumn, sRow, m_monsters[x].image, m_monsters[x].imageSize, m_monsters[x].color);      
      }
    }
  }
  for(int y=0; y < m_numItems; y++) {
    if ((m_items[y].mapX == vX) && (m_items[y].mapY == vY)) {
      if (m_items[y].inInventory == false) {
        M_DrawMapScaledBitmap(sColumn, sRow, m_items[y].image, m_items[y].imageSize, m_items[y].color);      
      }
    }    
  }      
}

// Used to track visible items during M_DrawMaze() to defer drawing after walls for correct draw ordering
void M_DrawVisibleCenterItems() {
  for(int z=4; z >= 0; z--) {
    if (m_visibleCenterRooms[z].visible == true) {
      M_DrawItems(m_visibleCenterRooms[z].virtualX, m_visibleCenterRooms[z].virtualY, m_visibleCenterRooms[z].column, m_visibleCenterRooms[z].row);
      m_visibleCenterRooms[z].visible = false;
    }
  }
}

// Used to track visible items during M_DrawMaze() to defer drawing after walls for correct draw ordering
void M_SetVisibleCenterRoom(unsigned char index, unsigned char vX, unsigned char vY, unsigned char sColumn, unsigned char sRow) {
  m_visibleCenterRooms[index].virtualX = vX;
  m_visibleCenterRooms[index].virtualY = vY;
  m_visibleCenterRooms[index].row = sRow;
  m_visibleCenterRooms[index].column = sColumn;
  m_visibleCenterRooms[index].visible = true;
}

void M_ResetVisibleCenterRooms() {
  for(int p=0; p < 5; p++) {
    m_visibleCenterRooms[p].visible = false;
  }
}

// Side segments have 4 columns (0 far left, 1 near left, 2 near right, 3 far right) and 3 rows (depths 0, 1, 2)
void M_DrawSideSegment(unsigned char sColumn, unsigned char sRow, unsigned char segType) {
  unsigned int clr;
  if ((m_roomData.mapX + m_roomData.mapY) & 1) { // check even or odd room and alternate colors
    switch (m_roomData.currentWall) {
      case NORTH:
        clr = MEDGREEN;
        break;
      case EAST:
        clr = GREEN;
        break;
      case SOUTH:
        clr = MEDGREEN;
        break;
      case WEST:
        clr = GREEN;
        break;
    }
  } else {    
    switch (m_roomData.currentWall) {
      case NORTH:
        clr = GREEN;
        break;
      case EAST:
        clr = MEDGREEN;
        break;
      case SOUTH:
        clr = GREEN;
        break;
      case WEST:
        clr = MEDGREEN;
        break;
    }
  }
  if (segType == WALL) {
    M_DrawSideWall(sColumn, sRow, clr);
  } else if ((segType == DOORMONSTER) && (sRow == 0) && (m_doorMonsterID > 0) && (m_monsters[m_doorMonsterID].enabled == true)) { // door monster, only show if right near it
    M_DrawSideWall(sColumn, sRow, clr);
    M_DrawSideDoor(sColumn, sRow, m_monsters[m_doorMonsterID].color);  
  } else if ((segType == DOOR) || (segType == DOORMONSTER)) { // door or door monster far away (show as normal door)
    M_DrawSideWall(sColumn, sRow, clr);
    M_DrawSideDoor(sColumn, sRow, BLUE);
  }  
}

// Flat segments have 3 columns (o left, 1 cetner, 2 right) and 3 rows (depths 0, 1, 2)
void M_DrawFlatSegment(unsigned char sColumn, unsigned char sRow, unsigned char segType) {
  unsigned int clr;
  if ((m_roomData.mapX + m_roomData.mapY) & 1) { // check even or odd room and alternate colors
    switch (m_roomData.currentWall) {
      case NORTH:
        clr = MEDGREEN;
        break;
      case EAST:
        clr = GREEN;
        break;
      case SOUTH:
        clr = MEDGREEN;
        break;
      case WEST:
        clr = GREEN;
        break;
    }
  } else {    
    switch (m_roomData.currentWall) {
      case NORTH:
        clr = GREEN;
        break;
      case EAST:
        clr = MEDGREEN;
        break;
      case SOUTH:
        clr = GREEN;
        break;
      case WEST:
        clr = MEDGREEN;
        break;
    }
  }
  if (segType == WALL) {
    M_DrawFlatWall(sColumn, sRow, clr);
  } else if ((segType == DOORMONSTER) && (sRow == 0) && (m_doorMonsterID > 0) && (m_monsters[m_doorMonsterID].enabled == true)) { // door monster, only show if right near it
    M_DrawFlatWall(sColumn, sRow, clr);
    M_DrawFlatDoor(sColumn, sRow, m_monsters[m_doorMonsterID].color);  
  } else if ((segType == DOOR) || (segType == DOORMONSTER)) { // door or door monster far away (show as normal door)
    if ((m_doorOpen == true) && (sRow == 0) && (sColumn == 1)) {
      M_DrawFlatWallOpenDoor(clr);
    } else {
      M_DrawFlatWall(sColumn, sRow, clr);
      M_DrawFlatDoor(sColumn, sRow, BLUE);
    }
  }  
}

// Read the static wall coordinate arrays for all wall segments, based on column and row being drawn
void M_DrawSideWall(byte m_wc, byte m_wr, unsigned int clr) { // wall column, wall row in the 3x3 viewable grid
  tft.fillTriangle(m_sideWalls[m_wr][m_wc][0][0], m_sideWalls[m_wr][m_wc][0][1], m_sideWalls[m_wr][m_wc][1][0], m_sideWalls[m_wr][m_wc][1][1], m_sideWalls[m_wr][m_wc][2][0], m_sideWalls[m_wr][m_wc][2][1], clr);   
  tft.fillTriangle(m_sideWalls[m_wr][m_wc][0][0], m_sideWalls[m_wr][m_wc][0][1], m_sideWalls[m_wr][m_wc][2][0], m_sideWalls[m_wr][m_wc][2][1], m_sideWalls[m_wr][m_wc][3][0], m_sideWalls[m_wr][m_wc][3][1], clr);   
}

void M_DrawFlatWall(byte m_wc, byte m_wr, unsigned int clr) { // wall column, wall row in the 3x3 viewable grid
  tft.fillRect(m_flatWalls[m_wr][m_wc][0], m_flatWalls[m_wr][m_wc][1], m_flatWalls[m_wr][m_wc][2], m_flatWalls[m_wr][m_wc][3], clr);   
}

void M_DrawFlatWallOpenDoor(int clr) { // always right in front of you
  tft.fillRect(15, 10, 99, 15, clr);   
  tft.fillRect(15, 25, 32, 56, clr);   
  tft.fillRect(82, 25, 32, 56, clr);   
}

void M_DrawFlatDoor(byte m_wc, byte m_wr, unsigned int clr) { // wall column, wall row in the 3x3 viewable grid
  tft.fillRect(m_flatDoors[m_wr][m_wc][0], m_flatDoors[m_wr][m_wc][1], m_flatDoors[m_wr][m_wc][2], m_flatDoors[m_wr][m_wc][3], clr);   
}

void M_DrawSideDoor(byte m_wc, byte m_wr, unsigned int clr) { // wall column, wall row in the 3x3 viewable grid
  tft.fillTriangle(m_sideDoors[m_wr][m_wc][0][0], m_sideDoors[m_wr][m_wc][0][1], m_sideDoors[m_wr][m_wc][1][0], m_sideDoors[m_wr][m_wc][1][1], m_sideDoors[m_wr][m_wc][2][0], m_sideDoors[m_wr][m_wc][2][1], clr);   
  tft.fillTriangle(m_sideDoors[m_wr][m_wc][0][0], m_sideDoors[m_wr][m_wc][0][1], m_sideDoors[m_wr][m_wc][2][0], m_sideDoors[m_wr][m_wc][2][1], m_sideDoors[m_wr][m_wc][3][0], m_sideDoors[m_wr][m_wc][3][1], clr);   
}

// Find wall segment for a room based on X,Y on globalMap and direction
// Read the appropriate 2 bits in the byte data for the room
unsigned char M_ReadWall(unsigned char wX, unsigned char wY, unsigned char wDir) {
  M_GetRoomData(wX, wY);
  switch(wDir) {
    case NORTH:
      m_roomData.currentWall = 0;
      return (m_roomData.room >> 6) & ~(~0 << 2); // most significant 2 bits for north wall
      break;
    case EAST:
      m_roomData.currentWall = 1;
      return (m_roomData.room >> 4) & ~(~0 << 2);
      break;
    case SOUTH:
      m_roomData.currentWall = 2;
      return (m_roomData.room >> 2) & ~(~0 << 2);
      break;
    case WEST:
      m_roomData.currentWall = 3;
      return (m_roomData.room >> 0) & ~(~0 << 2); // least significant bits for west wall
      break;
  }  
}

// Read the byte of data representing the room's 4 walls
void M_GetRoomData(unsigned char wX, unsigned char wY) {
  m_roomData.mapX = wX;
  m_roomData.mapY = wY;
  m_roomData.room = m_gameMap[m_roomData.mapY][m_roomData.mapX];  
}

// Store the player's left, right and back compass direction based on forward direction
// 0 North, 1 East, 2 South, 3 West
void M_SetPlayerDirections() {
  char newDir;
  newDir = m_player.forwardDir - 1;
  if (newDir < 0) {
    newDir = 3;
  }
  m_player.leftDir = newDir;
  newDir = m_player.forwardDir + 1;
  if (newDir > 3) {
    newDir = 0;
  }
  m_player.rightDir = newDir;
  switch(m_player.forwardDir) {
    case NORTH:
      m_player.backDir = SOUTH;
      break;
    case EAST:
      m_player.backDir = WEST;
      break;
    case SOUTH:
      m_player.backDir = NORTH;
      break;
    case WEST:
      m_player.backDir = EAST;
      break;
  }
}


// Find a virtual X (and Y) position based on given offset relative to current position and player direction
// Used to draw and find items relative to where player is
// vX offset = -1|0|1 (left of player by 1 column, player's position, right of player by 1 column)
// vY offset = 0|1|2 which is the row (depth) from player (0 is the player's row)
unsigned char M_PlayerVirtualX(char vX, char vY) {
  switch(m_player.forwardDir) {
    case NORTH:
      return m_player.mapX + vX;
      break;
    case EAST:
      return m_player.mapX + vY;
      break;
    case SOUTH:
      return m_player.mapX - vX;
      break;
    case WEST:
      return m_player.mapX - vY;
      break;
  }
}

unsigned char M_PlayerVirtualY(char vX, char vY) {
  switch(m_player.forwardDir) {
    case NORTH:
      return m_player.mapY - vY;
      break;
    case EAST:
      return m_player.mapY + vX;
      break;
    case SOUTH:
      return m_player.mapY + vY;
      break;
    case WEST:
      return m_player.mapY - vX;
      break;
  }
}

// General drawing routine for scaled bitmaps anywhere on the screen
void M_DrawScaledBitmap(char x, char y, const unsigned char *image, unsigned char imageSize, unsigned char scale, unsigned int color) {
  char i, j, byteWidth = imageSize / 8;
  for (j = 0; j < imageSize; j++) {
    for (i = 0; i < imageSize; i++ ) {
      if (pgm_read_byte(image + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        tft.fillRect(x + i*scale, y + j*scale, scale, scale, color);
      }
    }
  }
}

// Draw items/monsters into the maze with a scale based on distance from player
// xPos 0-left, 1-center, 2-right of player
// yPos is row (depth) from player (0 player's row, 1, 2)
// Uses fixed X&Y offsets based on position of item in the 3x3 viewable maze grid and image size
void M_DrawMapScaledBitmap(char xPos, char yPos, const unsigned char *image, unsigned char imageSize, unsigned int color) {
  char i, j, byteWidth = imageSize / 8;
  char x;
  char y;
  char scale;
  // X & Y position in 3x3 viewable room grid
  switch(yPos) {
    case 0: // Can't see 0, 2
      if (xPos == 1) {
        x = 41;
        y = 37;
        scale = 3;
        if (image == *(m_largeImages+17)) { // special case for treasure of tarmin
          x += 16;
          y += 37;
          scale = 1;
        }
        if (imageSize == 8) {
          if (image == *(m_smallImages+19)) { // special case for hallway eyeball
            x += 20;
            y += 45;
            scale = 1;
          } else {
            x += 16;
            y += 37;
            scale = 2;
          }
        }
      }
      break;
    case 1:
      if (xPos == 0) {
        x = 0;
        y = 40;
        scale = 2;
      } else if (xPos == 1) {
        x = 49;
        y = 40;
        scale = 2;        
      } else {
        x = 100;
        y = 40;
        scale = 2;        
      }
      if (image == *(m_largeImages+17)) { // special case for treasure of tarmin
        x += 8;
        y += 21;
        scale = 1;
      }
      if (imageSize == 8) {
        if (image == *(m_smallImages+20)) { // special case for ladder
          x += 8;
          y += 21;
          scale = 2;
        } else {
          x += 12;
          y += 29;
          scale = 1;
        }
      }
      break;
    case 2: // Can't see 0, 2
      if (xPos == 1) {
        x = 57;
        y = 45;
        scale = 1;        
        if (imageSize == 8) {
          x += 4;
          y += 13;
        }
      }
      break;
  }
  for (j = 0; j < imageSize; j++) {
    for (i = 0; i < imageSize; i++ ) {
      if (pgm_read_byte(image + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        tft.fillRect(x + i*scale, y + j*scale, scale, scale, color);
      }
    }
  }
}

// Draw an in-game map showing rooms you've visited and the location of ladders
// Uses m_progressMap[12] which is a twelve int array. Uses bit operators to store visited rooms in the 12x12 overall gameMap
void M_DrawProgressMap() {
  boolean notDone = false;
  unsigned char segType;
  unsigned char xOffset = 10;
  unsigned char yOffset = 9;
  tft.fillScreen(BLACK);
  for(int y=0; y<12; y++) {
    for(int x=0; x<12; x++) {
      // Walk through each visisted room and draw it's walls and any ladders
      if (((m_progressMap[y] >> x) & 1) == 1) { // room has been visited
        tft.fillRect(xOffset+(x*9), yOffset+(y*9), 8, 8, BROWN); // draw an 8x8 floor for the room 
        for (int z=0; z<m_numItems; z++) { // check if there's a ladder in the room, draw it
          if ((m_items[z].itemType == LADDER) && (m_items[z].mapX == x) && (m_items[z].mapY == y)) {
            M_DrawScaledBitmap(xOffset+(x*9), yOffset+(y*9)+1, m_items[z].image, m_items[z].imageSize, 1, m_items[z].color); 
          }
        } 
        // Draw all 4 walls of the room
        segType = (m_gameMap[y][x] >> 6) & ~(~0 << 2); // North wall
        if (segType == WALL) {
          tft.drawFastHLine(xOffset+(x*9)-1, yOffset+(y*9)-1, 10, LIGHTGREEN);
        } else if ((segType == DOORMONSTER) || (segType == DOOR)) {
          tft.drawFastHLine(xOffset+(x*9)-1, yOffset+(y*9)-1, 10, BLUE);          
        }
        segType = (m_gameMap[y][x] >> 4) & ~(~0 << 2); // East wall
        if (segType == WALL) {
          tft.drawFastVLine(xOffset+(x*9)+8, yOffset+(y*9)-1, 10, LIGHTGREEN);          
        } else if ((segType == DOORMONSTER) || (segType == DOOR)) {
          tft.drawFastVLine(xOffset+(x*9)+8, yOffset+(y*9)-1, 10, BLUE);                    
        }
        segType = (m_gameMap[y][x] >> 2) & ~(~0 << 2); // South wall
        if (segType == WALL) {
          tft.drawFastHLine(xOffset+(x*9)-1, yOffset+(y*9)+8, 10, LIGHTGREEN);
        } else if ((segType == DOORMONSTER) || (segType == DOOR)) {
          tft.drawFastHLine(xOffset+(x*9)-1, yOffset+(y*9)+8, 10, BLUE);          
        }
        segType = (m_gameMap[y][x] >> 0) & ~(~0 << 2); // West wall
        if (segType == WALL) {
          tft.drawFastVLine(xOffset+(x*9)-1, yOffset+(y*9)-1, 10, LIGHTGREEN);          
        } else if ((segType == DOORMONSTER) || (segType == DOOR)) { 
          tft.drawFastVLine(xOffset+(x*9)-1, yOffset+(y*9)-1, 10, BLUE);                    
        }
      }
    }
  }
  // Draw an arrow showing current player location and direction
  switch(m_player.forwardDir) {
    case NORTH:
      tft.fillTriangle(xOffset+(m_player.mapX*9), yOffset+(m_player.mapY*9)+7, xOffset+(m_player.mapX*9)+3, yOffset+(m_player.mapY*9), xOffset+(m_player.mapX*9)+6, yOffset+(m_player.mapY*9)+7, WHITE);
      break;
    case EAST:
      tft.fillTriangle(xOffset+(m_player.mapX*9), yOffset+(m_player.mapY*9), xOffset+(m_player.mapX*9)+7, yOffset+(m_player.mapY*9)+3, xOffset+(m_player.mapX*9), yOffset+(m_player.mapY*9)+6, WHITE);
      break;
    case SOUTH:
      tft.fillTriangle(xOffset+(m_player.mapX*9), yOffset+(m_player.mapY*9), xOffset+(m_player.mapX*9)+6, yOffset+(m_player.mapY*9), xOffset+(m_player.mapX*9)+3, yOffset+(m_player.mapY*9)+7, WHITE);
      break;
    case WEST:
      tft.fillTriangle(xOffset+(m_player.mapX*9)+7, yOffset+(m_player.mapY*9), xOffset+(m_player.mapX*9)+7, yOffset+(m_player.mapY*9)+6, xOffset+(m_player.mapX*9), yOffset+(m_player.mapY*9)+3, WHITE);
      break;          
  }
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(26, 130);
  tft.print("Dungeon Level");     
  if (m_player.currentLevel > 99) {
    tft.setCursor(39, 145);
  } else if (m_player.currentLevel > 9) {
    tft.setCursor(45, 145);
  } else {
    tft.setCursor(52, 145);
  }
  tft.print(m_player.currentLevel);
  tft.setCursor(62, 145);
  tft.print("/");
  tft.setCursor(72, 145);
  tft.print(m_numLevels);
  while (notDone == false) {
    if ((digitalRead(BUTTON1) == LOW) || (digitalRead(BUTTON2) == LOW)) {
      if ((millis() - buttonPressed) > buttonDelay) {
        tft.fillScreen(BLACK);
        M_DrawMaze();
        M_DrawDashboard();
        buttonPressed = millis();
        return;
      }
    } 
    if ((millis() - joystickPressed) > joystickDelay) {
      if (digitalRead(JOYB) == LOW) {
        tft.fillScreen(BLACK);
        M_DrawMaze();
        M_DrawDashboard();
        joystickPressed = millis();    
        return;
      }
    } 
  }
}

