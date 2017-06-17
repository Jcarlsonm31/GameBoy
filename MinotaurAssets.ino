// Setup the level by adding monsters and items
void M_InitLevelItems() {
  boolean dupCheck = true;
  unsigned char m_monsterSeed[24];
  unsigned char m_monsterLevelSeed[8];
  unsigned char m_itemSeed[28];
  unsigned char m_itemLevelSeed[8];
  m_doorMonsterID = 0;
  // Calculate what percent you are through the total number of levels
  // Game difficult 1 has 2 levels, 2 has 4 levels, 3 has 8 levels, 4 has 12 levels
  float levelPercent = ((float)m_player.currentLevel / (float)m_numLevels);
  // Special case for difficulty level 1
  if (m_gameDifficulty == 1) {
    if (m_player.currentLevel == 1) {
      levelPercent = 0.24;
    } else if (m_player.currentLevel == 2) {
      levelPercent = 0.26;
    } else if (m_player.currentLevel == 3) {
      levelPercent = 0.51;
    }
  }
  // Seed the level with a weighted random number of monsters and items and weighted random level of those things
  if (levelPercent > 0.75) {
    unsigned char m_monsterSeedTmp[24] = {0,1,2,3,4,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,14,15};
    unsigned char m_monsterLevelSeedTmp[8] = {2,2,3,3,3,3,4,4};
    unsigned char m_itemSeedTmp[28] = {0,0,0,1,1,1,2,2,2,3,3,4,5,6,7,8,12,13,14,14,15,15,16,16,16,17,17,17};
    unsigned char m_itemLevelSeedTmp[8] = {2,3,3,3,4,4,4,4};
    memcpy(m_monsterSeed, m_monsterSeedTmp, sizeof(m_monsterSeed));
    memcpy(m_monsterLevelSeed, m_monsterLevelSeedTmp, sizeof(m_monsterLevelSeed));
    memcpy(m_itemSeed, m_itemSeedTmp, sizeof(m_itemSeed));
    memcpy(m_itemLevelSeed, m_itemLevelSeedTmp, sizeof(m_itemLevelSeed));
  } else if (levelPercent > 0.50) {    
    unsigned char m_monsterSeedTmp[24] = {0,1,2,3,4,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,14,15};
    unsigned char m_monsterLevelSeedTmp[8] = {1,2,2,2,2,2,3,3};
    unsigned char m_itemSeedTmp[28] = {0,0,0,1,1,1,2,2,2,3,3,4,5,6,7,8,12,12,13,13,14,14,15,15,16,16,17,17};
    unsigned char m_itemLevelSeedTmp[8] = {2,2,3,3,3,4,4,4};
    memcpy(m_monsterSeed, m_monsterSeedTmp, sizeof(m_monsterSeed));
    memcpy(m_monsterLevelSeed, m_monsterLevelSeedTmp, sizeof(m_monsterLevelSeed));
    memcpy(m_itemSeed, m_itemSeedTmp, sizeof(m_itemSeed));
    memcpy(m_itemLevelSeed, m_itemLevelSeedTmp, sizeof(m_itemLevelSeed));
  } else if (levelPercent > 0.25) {    
    unsigned char m_monsterSeedTmp[24] = {0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11};
    unsigned char m_monsterLevelSeedTmp[8] = {1,1,1,1,2,2,2,2};
    unsigned char m_itemSeedTmp[28] = {0,0,0,1,1,1,2,2,2,3,3,4,5,6,7,8,9,10,11,12,13,13,14,14,15,15,16,17};
    unsigned char m_itemLevelSeedTmp[8] = {1,2,2,2,3,3,4,4};
    memcpy(m_monsterSeed, m_monsterSeedTmp, sizeof(m_monsterSeed));
    memcpy(m_monsterLevelSeed, m_monsterLevelSeedTmp, sizeof(m_monsterLevelSeed));
    memcpy(m_itemSeed, m_itemSeedTmp, sizeof(m_itemSeed));
    memcpy(m_itemLevelSeed, m_itemLevelSeedTmp, sizeof(m_itemLevelSeed));
  } else {    
    unsigned char m_monsterSeedTmp[24] = {0,0,0,1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,6,6,7,7,8,9};
    unsigned char m_monsterLevelSeedTmp[8] = {1,1,1,1,1,1,2,2};
    unsigned char m_itemSeedTmp[28] = {0,0,0,1,1,1,2,2,2,3,3,4,5,6,7,8,9,9,10,10,11,11,11,12,12,12,13,14};
    unsigned char m_itemLevelSeedTmp[8] = {1,1,1,1,1,2,2,3};
    memcpy(m_monsterSeed, m_monsterSeedTmp, sizeof(m_monsterSeed));
    memcpy(m_monsterLevelSeed, m_monsterLevelSeedTmp, sizeof(m_monsterLevelSeed));
    memcpy(m_itemSeed, m_itemSeedTmp, sizeof(m_itemSeed));
    memcpy(m_itemLevelSeed, m_itemLevelSeedTmp, sizeof(m_itemLevelSeed));
  }

  // ADD MONSTERS
  m_numMonsters = random(6, 11);
  for (int x=0; x<m_numMonsters; x++) {
    dupCheck = true;
    m_monsters[x] = m_masterMonsters[m_monsterSeed[random(0,24)]];
    m_monsters[x].level = m_monsterLevelSeed[random(0,8)];
    // Hit points are random from base points+3 to base points*level + 1
    m_monsters[x].hitPoints = random((m_monsters[x].hitPoints * (m_monsters[x].level-1)+3), (m_monsters[x].hitPoints * m_monsters[x].level+1));
    unsigned char incrementalDamage = ((m_monsters[x].damage * m_gameDifficulty) - m_monsters[x].damage) / 3;
    m_monsters[x].damage += (m_monsters[x].level-1) * incrementalDamage;
    m_monsters[x].color = m_itemColors[m_monsters[x].level-1];
    // Make sure we don't stack monsters/items on top of each other
    while (dupCheck == true) {
      dupCheck = false;
      m_monsters[x].mapX = random(1,11);
      m_monsters[x].mapY = random(1,11);
      for (int y=0; y<x; y++) {
        if ((m_monsters[x].mapX == m_monsters[y].mapX) && (m_monsters[x].mapY == m_monsters[y].mapY)) {
          dupCheck = true;
        }
      }
    }
  }
  // Add Minotaur if on last level or past it (you can keep going past the last level of the game difficulty selected)
  if (m_player.currentLevel >= m_numLevels) {
    dupCheck = true;
    m_monsters[m_numMonsters] = m_masterMonsters[16];
    m_monsters[m_numMonsters].level = m_gameDifficulty;
    m_monsters[m_numMonsters].hitPoints = m_monsters[m_numMonsters].hitPoints * m_gameDifficulty;
    unsigned char incrementalDamage = ((m_monsters[m_numMonsters].damage * m_gameDifficulty) - m_monsters[m_numMonsters].damage) / 3;
    m_monsters[m_numMonsters].damage += (m_monsters[m_numMonsters].level-1) * incrementalDamage;
    while (dupCheck == true) {
      dupCheck = false;
      m_monsters[m_numMonsters].mapX = random(1,11);
      m_monsters[m_numMonsters].mapY = random(1,11);
      for (int y=0; y<m_numMonsters; y++) {
        if ((m_monsters[m_numMonsters].mapX == m_monsters[y].mapX) && (m_monsters[m_numMonsters].mapY == m_monsters[y].mapY)) {
          dupCheck = true;
        }
      }
    }    
    m_numMonsters += 1;
  }
  // Possibly add Door Monster (door monster drops magic lamp)
  if (random(0,100) > 49) {
    if (levelPercent > 0.75) {
      M_AddDoorMonster(random(2,5)); //level 2, 3 or 4 door monster
    } else if (levelPercent > 0.50) {    
      M_AddDoorMonster(random(2,4)); //level 2 or 3 door monster
    } else if (levelPercent > 0.25) {    
      M_AddDoorMonster(2); //level 2 door monster
    }
  }
  
  // ADD ITEMS
  // Fixed eyeballs in the outside hallways
  m_items[0] = m_masterItems[19];
  m_items[0].mapX = 0;
  m_items[0].mapY = 3;
  m_items[1] = m_masterItems[19];
  m_items[1].mapX = 0;
  m_items[1].mapY = 8;
  m_items[2] = m_masterItems[19];
  m_items[2].mapX = 11;
  m_items[2].mapY = 3;
  m_items[3] = m_masterItems[19];
  m_items[3].mapX = 11;
  m_items[3].mapY = 8;
  m_items[4] = m_masterItems[19];
  m_items[4].mapX = 3;
  m_items[4].mapY = 0;
  m_items[5] = m_masterItems[19];
  m_items[5].mapX = 8;
  m_items[5].mapY = 0;
  m_items[6] = m_masterItems[19];
  m_items[6].mapX = 3;
  m_items[6].mapY = 11;
  m_items[7] = m_masterItems[19];
  m_items[7].mapX = 8;
  m_items[7].mapY = 11;
  char m_numItemsToAdd = random(10, 13);
  for (int x=8; x<m_numItemsToAdd+9; x++) {
    dupCheck = true;
    m_items[x] = m_masterItems[m_itemSeed[random(0,28)]];
    m_items[x].level = m_itemLevelSeed[random(0,8)];;
    if ((m_items[x].itemType != FLOUR) && (m_items[x].itemType != ARROWS)) { // Flour & arrows always tan
      m_items[x].color = m_itemColors[m_items[x].level-1];
    }
    switch (m_items[x].itemType) {
      case POTION: // restore points random from base points+3 to base points*level + 1
        m_items[x].restorePoints = random((m_items[x].restorePoints * (m_items[x].level-1)+3), (m_items[x].restorePoints * m_items[x].level+1));
        break;
      case FLOUR:
        m_items[x].restorePoints = m_items[x].restorePoints * m_items[x].level;
        break;
      case WEAPON: // damage random from base points+3 to base points*level + 1
        m_items[x].damage = random((m_items[x].damage * (m_items[x].level-1)+3), (m_items[x].damage * m_items[x].level+1));
        break;
      case ARMOR:
        m_items[x].armorClass = m_items[x].armorClass * m_items[x].level;
        break;
      case GAUNTLETS:
        m_items[x].armorClass = m_items[x].armorClass * m_items[x].level;
        break;
      case HELMET:
        m_items[x].armorClass = m_items[x].armorClass * m_items[x].level;
        break;
      case RING:
        m_items[x].armorClass = m_items[x].armorClass * m_items[x].level;
        break;
      case SHIELD:
        m_items[x].armorClass = m_items[x].armorClass * m_items[x].level;
        break;
    }
    while (dupCheck == true) {
      dupCheck = false;
      m_items[x].mapX = random(1,11);
      m_items[x].mapY = random(1,11);
      for (int y=8; y<x; y++) { //ignore first 8 eyeballs, items can't be placed in the hallways
        if ((m_items[x].mapX == m_items[y].mapX) && (m_items[x].mapY == m_items[y].mapY)) {
          dupCheck = true;
        }
      }
      for (int y=0; y<m_numMonsters; y++) {
        if ((m_items[x].mapX == m_monsters[y].mapX) && (m_items[x].mapY == m_monsters[y].mapY)) {
          dupCheck = true;
        }
      }      
    }
  }
  m_numItems = 8 + m_numItemsToAdd;
  // Add 2 ladders
  char m_ladder1X = 50;
  char m_ladder1Y = 50;
  for (int x=m_numItems; x<m_numItems+2; x++) {
    m_items[x] = m_masterItems[20];
    dupCheck = true;
    while (dupCheck == true) {
      dupCheck = false;
      m_items[x].mapX = random(1,11);
      m_items[x].mapY = random(1,11);
      for (int y=8; y<x; y++) { // ignore first 8 eyeballs
        if ((m_items[x].mapX == m_items[y].mapX) && (m_items[x].mapY == m_items[y].mapY)) {
          dupCheck = true;
        }
      }
      for (int y=0; y<m_numMonsters; y++) {
        if ((m_items[x].mapX == m_monsters[y].mapX) && (m_items[x].mapY == m_monsters[y].mapY)) {
          dupCheck = true;
        }
      }      
      // make sure second ladder isn't right next to first ladder
      if ((abs(m_ladder1X - m_items[x].mapX) < 4) && (abs(m_ladder1Y - m_items[x].mapY) < 4)) {
        dupCheck = true;
      }
    }
    m_ladder1X = m_items[x].mapX;
    m_ladder1Y = m_items[x].mapY;
  }
  m_numItems += 2;      
}

// Add a door monster
// Find a door on the map and convert it into a door monster (binary 10 instead of 11 in the room data)
void M_AddDoorMonster(unsigned char level) {
  m_monsters[m_numMonsters] = m_masterMonsters[17];
  m_monsters[m_numMonsters].level = level;
  m_monsters[m_numMonsters].color = m_itemColors[level-1];
  m_monsters[m_numMonsters].hitPoints = m_monsters[m_numMonsters].hitPoints * m_monsters[m_numMonsters].level;
  unsigned char incrementalDamage = ((m_monsters[m_numMonsters].damage * m_gameDifficulty) - m_monsters[m_numMonsters].damage) / 3;
  m_monsters[m_numMonsters].damage += (m_monsters[m_numMonsters].level-1) * incrementalDamage;
  m_monsters[m_numMonsters].enabled = false;
  boolean doorCheck = true;
  while (doorCheck == true) {
    m_monsters[m_numMonsters].mapX = random(1,11);
    m_monsters[m_numMonsters].mapY = random(1,11);
    // Check all 4 walls for a door
    if (M_ReadWall(m_monsters[m_numMonsters].mapX, m_monsters[m_numMonsters].mapY, 0) == DOOR) {
      M_UpdateRoomDoorMonster(m_monsters[m_numMonsters].mapX, m_monsters[m_numMonsters].mapY, 0);
      doorCheck = false;
    } else if (M_ReadWall(m_monsters[m_numMonsters].mapX, m_monsters[m_numMonsters].mapY, 1) == DOOR) {
      M_UpdateRoomDoorMonster(m_monsters[m_numMonsters].mapX, m_monsters[m_numMonsters].mapY, 1);
      doorCheck = false;      
    } else if (M_ReadWall(m_monsters[m_numMonsters].mapX, m_monsters[m_numMonsters].mapY, 2) == DOOR) {
      M_UpdateRoomDoorMonster(m_monsters[m_numMonsters].mapX, m_monsters[m_numMonsters].mapY, 2);
      doorCheck = false;      
    } else if (M_ReadWall(m_monsters[m_numMonsters].mapX, m_monsters[m_numMonsters].mapY, 3) == DOOR) {
      M_UpdateRoomDoorMonster(m_monsters[m_numMonsters].mapX, m_monsters[m_numMonsters].mapY, 3);
      doorCheck = false;      
    }
  }      
  m_monsters[m_numMonsters].enabled = true;
  m_doorMonsterID = m_numMonsters;
  m_numMonsters += 1;
}

// Take the given door wall segment (binary 11) and turn it into a door monster (binary 10)
// Also update the adjoining room so the door draws as a door monster from both sides
void M_UpdateRoomDoorMonster(unsigned char x, unsigned char y, unsigned char wallSegment) {
  M_GetRoomData(x, y);
  switch(wallSegment) {
    // Set appropriate bit to 0. Position 6 is North, 4 East, 2, South, 0 West
    case NORTH: // North wall of room
      m_gameMap[m_roomData.mapY][m_roomData.mapX] &= ~(1 << 6);
      m_gameMap[m_roomData.mapY-1][m_roomData.mapX] &= ~(1 << 2);
      break;
    case EAST: // East wall
      m_gameMap[m_roomData.mapY][m_roomData.mapX] &= ~(1 << 4);
      m_gameMap[m_roomData.mapY][m_roomData.mapX+1] &= ~(1 << 0);
      break;
    case SOUTH: // South wall
      m_gameMap[m_roomData.mapY][m_roomData.mapX] &= ~(1 << 2);
      m_gameMap[m_roomData.mapY+1][m_roomData.mapX] &= ~(1 << 6);
      break;
    case WEST: // West wall
      m_gameMap[m_roomData.mapY][m_roomData.mapX] &= ~(1 << 0);
      m_gameMap[m_roomData.mapY][m_roomData.mapX-1] &= ~(1 << 4);
      break;
  }
}

// Master default list of possible monsters and items
void M_InitMasterMonsters() {
  // Ant
  m_masterMonsters[0].itemType = MONSTER;
  m_masterMonsters[0].mapX = 0;
  m_masterMonsters[0].mapY = 0;
  m_masterMonsters[0].hitPoints = 4;
  m_masterMonsters[0].armorClass = 1;
  m_masterMonsters[0].damage = 5;
  m_masterMonsters[0].enabled = true;
  m_masterMonsters[0].image = *(m_largeImages+1);
  m_masterMonsters[0].weaponImage = *(m_smallImages+13);
  m_masterMonsters[0].imageSize = 16;
  m_masterMonsters[0].level = 1;
  m_masterMonsters[0].color = m_itemColors[0];
  // Dwarf
  m_masterMonsters[1].itemType = MONSTER;
  m_masterMonsters[1].mapX = 0;
  m_masterMonsters[1].mapY = 0;
  m_masterMonsters[1].hitPoints = 5;
  m_masterMonsters[1].armorClass = 2;
  m_masterMonsters[1].damage = 5;
  m_masterMonsters[1].enabled = true;
  m_masterMonsters[1].image = *(m_largeImages+3);
  m_masterMonsters[1].weaponImage = *(m_smallImages+13);
  m_masterMonsters[1].imageSize = 16;
  m_masterMonsters[1].level = 1;
  m_masterMonsters[1].color = m_itemColors[0];
  // Dwarf Shield
  m_masterMonsters[2].itemType = MONSTER;
  m_masterMonsters[2].mapX = 0;
  m_masterMonsters[2].mapY = 0;
  m_masterMonsters[2].hitPoints = 5;
  m_masterMonsters[2].armorClass = 3;
  m_masterMonsters[2].damage = 5;
  m_masterMonsters[2].enabled = true;
  m_masterMonsters[2].image = *(m_largeImages+4);
  m_masterMonsters[2].weaponImage = *(m_smallImages+13);
  m_masterMonsters[2].imageSize = 16;
  m_masterMonsters[2].level = 1;
  m_masterMonsters[2].color = m_itemColors[0];
  // Scorpion
  m_masterMonsters[3].itemType = MONSTER;
  m_masterMonsters[3].mapX = 0;
  m_masterMonsters[3].mapY = 0;
  m_masterMonsters[3].hitPoints = 7;
  m_masterMonsters[3].armorClass = 3;
  m_masterMonsters[3].damage = 7;
  m_masterMonsters[3].enabled = true;
  m_masterMonsters[3].image = *(m_largeImages+9);
  m_masterMonsters[3].weaponImage = *(m_smallImages+15);
  m_masterMonsters[3].imageSize = 16;
  m_masterMonsters[3].level = 1;
  m_masterMonsters[3].color = m_itemColors[0];
  // Skeleton
  m_masterMonsters[4].itemType = MONSTER;
  m_masterMonsters[4].mapX = 0;
  m_masterMonsters[4].mapY = 0;
  m_masterMonsters[4].hitPoints = 9;
  m_masterMonsters[4].armorClass = 4;
  m_masterMonsters[4].damage = 7;
  m_masterMonsters[4].enabled = true;
  m_masterMonsters[4].image = *(m_largeImages+10);
  m_masterMonsters[4].weaponImage = *(m_smallImages+14);
  m_masterMonsters[4].imageSize = 16;
  m_masterMonsters[4].level = 1;
  m_masterMonsters[4].color = m_itemColors[0];
  // Skeleton Shield
  m_masterMonsters[5].itemType = MONSTER;
  m_masterMonsters[5].mapX = 0;
  m_masterMonsters[5].mapY = 0;
  m_masterMonsters[5].hitPoints = 9;
  m_masterMonsters[5].armorClass = 5;
  m_masterMonsters[5].damage = 7;
  m_masterMonsters[5].enabled = true;
  m_masterMonsters[5].image = *(m_largeImages+13);
  m_masterMonsters[5].weaponImage = *(m_smallImages+14);
  m_masterMonsters[5].imageSize = 16;
  m_masterMonsters[5].level = 1;
  m_masterMonsters[5].color = m_itemColors[0];
  // Cloaked Skeleton
  m_masterMonsters[6].itemType = MONSTER;
  m_masterMonsters[6].mapX = 0;
  m_masterMonsters[6].mapY = 0;
  m_masterMonsters[6].hitPoints = 15;
  m_masterMonsters[6].armorClass = 5;
  m_masterMonsters[6].damage = 9;
  m_masterMonsters[6].enabled = true;
  m_masterMonsters[6].image = *(m_largeImages+11);
  m_masterMonsters[6].weaponImage = *(m_smallImages+9);
  m_masterMonsters[6].imageSize = 16;
  m_masterMonsters[6].level = 1;
  m_masterMonsters[6].color = m_itemColors[0];
  // Cloaked Skeleton Shield
  m_masterMonsters[7].itemType = MONSTER;
  m_masterMonsters[7].mapX = 0;
  m_masterMonsters[7].mapY = 0;
  m_masterMonsters[7].hitPoints = 15;
  m_masterMonsters[7].armorClass = 6;
  m_masterMonsters[7].damage = 9;
  m_masterMonsters[7].enabled = true;
  m_masterMonsters[7].image = *(m_largeImages+12);
  m_masterMonsters[7].weaponImage = *(m_smallImages+9);
  m_masterMonsters[7].imageSize = 16;
  m_masterMonsters[7].level = 1;
  m_masterMonsters[7].color = m_itemColors[0];
  // Snake
  m_masterMonsters[8].itemType = MONSTER;
  m_masterMonsters[8].mapX = 0;
  m_masterMonsters[8].mapY = 0;
  m_masterMonsters[8].hitPoints = 20;
  m_masterMonsters[8].armorClass = 7;
  m_masterMonsters[8].damage = 11;
  m_masterMonsters[8].enabled = true;
  m_masterMonsters[8].image = *(m_largeImages+14);
  m_masterMonsters[8].weaponImage = *(m_smallImages+15);
  m_masterMonsters[8].imageSize = 16;
  m_masterMonsters[8].level = 1;
  m_masterMonsters[8].color = m_itemColors[0];
  // Giant
  m_masterMonsters[9].itemType = MONSTER;
  m_masterMonsters[9].mapX = 0;
  m_masterMonsters[9].mapY = 0;
  m_masterMonsters[9].hitPoints = 25;
  m_masterMonsters[9].armorClass = 9;
  m_masterMonsters[9].damage = 13;
  m_masterMonsters[9].enabled = true;
  m_masterMonsters[9].image = *(m_largeImages+6);
  m_masterMonsters[9].weaponImage = *(m_smallImages+17);  
  m_masterMonsters[9].imageSize = 16;
  m_masterMonsters[9].level = 1;
  m_masterMonsters[9].color = m_itemColors[0];
  // Giant Shield
  m_masterMonsters[10].itemType = MONSTER;
  m_masterMonsters[10].mapX = 0;
  m_masterMonsters[10].mapY = 0;
  m_masterMonsters[10].hitPoints = 25;
  m_masterMonsters[10].armorClass = 12;
  m_masterMonsters[10].damage = 13;
  m_masterMonsters[10].enabled = true;
  m_masterMonsters[10].image = *(m_largeImages+7);
  m_masterMonsters[10].weaponImage = *(m_smallImages+17);  
  m_masterMonsters[10].imageSize = 16;
  m_masterMonsters[10].level = 1;
  m_masterMonsters[10].color = m_itemColors[0];
  // Alligator
  m_masterMonsters[11].itemType = MONSTER;
  m_masterMonsters[11].mapX = 0;
  m_masterMonsters[11].mapY = 0;
  m_masterMonsters[11].hitPoints = 30;
  m_masterMonsters[11].armorClass = 12;
  m_masterMonsters[11].damage = 15;
  m_masterMonsters[11].enabled = true;
  m_masterMonsters[11].image = *(m_largeImages+0);
  m_masterMonsters[11].weaponImage = *(m_smallImages+15);  
  m_masterMonsters[11].imageSize = 16;
  m_masterMonsters[11].level = 1;
  m_masterMonsters[11].color = m_itemColors[0];
  // Ghoul
  m_masterMonsters[12].itemType = MONSTER;
  m_masterMonsters[12].mapX = 0;
  m_masterMonsters[12].mapY = 0;
  m_masterMonsters[12].hitPoints = 35;
  m_masterMonsters[12].armorClass = 14;
  m_masterMonsters[12].damage = 18;
  m_masterMonsters[12].enabled = true;
  m_masterMonsters[12].image = *(m_largeImages+5);
  m_masterMonsters[12].weaponImage = *(m_smallImages+9);  
  m_masterMonsters[12].imageSize = 16;
  m_masterMonsters[12].level = 1;
  m_masterMonsters[12].color = m_itemColors[0];
  // Dragon
  m_masterMonsters[13].itemType = MONSTER;
  m_masterMonsters[13].mapX = 0;
  m_masterMonsters[13].mapY = 0;
  m_masterMonsters[13].hitPoints = 40;
  m_masterMonsters[13].armorClass = 15;
  m_masterMonsters[13].damage = 20;
  m_masterMonsters[13].enabled = true;
  m_masterMonsters[13].image = *(m_largeImages+2);
  m_masterMonsters[13].weaponImage = *(m_smallImages+13);  
  m_masterMonsters[13].imageSize = 16;
  m_masterMonsters[13].level = 1;
  m_masterMonsters[13].color = m_itemColors[0];
  // Wraith
  m_masterMonsters[14].itemType = MONSTER;
  m_masterMonsters[14].mapX = 0;
  m_masterMonsters[14].mapY = 0;
  m_masterMonsters[14].hitPoints = 50;
  m_masterMonsters[14].armorClass = 16;
  m_masterMonsters[14].damage = 25;
  m_masterMonsters[14].enabled = true;
  m_masterMonsters[14].image = *(m_largeImages+15);
  m_masterMonsters[14].weaponImage = *(m_smallImages+17);  
  m_masterMonsters[14].imageSize = 16;
  m_masterMonsters[14].level = 1;
  m_masterMonsters[14].color = m_itemColors[0];
  // Wraith Shield
  m_masterMonsters[15].itemType = MONSTER;
  m_masterMonsters[15].mapX = 0;
  m_masterMonsters[15].mapY = 0;
  m_masterMonsters[15].hitPoints = 50;
  m_masterMonsters[15].armorClass = 18;
  m_masterMonsters[15].damage = 25;
  m_masterMonsters[15].enabled = true;
  m_masterMonsters[15].image = *(m_largeImages+16);
  m_masterMonsters[15].weaponImage = *(m_smallImages+15);  
  m_masterMonsters[15].imageSize = 16;
  m_masterMonsters[15].level = 1;
  m_masterMonsters[15].color = m_itemColors[0];
  // Minotaur
  m_masterMonsters[16].itemType = MONSTER;
  m_masterMonsters[16].mapX = 0;
  m_masterMonsters[16].mapY = 0;
  m_masterMonsters[16].hitPoints = 60;
  m_masterMonsters[16].armorClass = 20;
  m_masterMonsters[16].damage = 30;
  m_masterMonsters[16].enabled = true;
  m_masterMonsters[16].image = *(m_largeImages+8);
  m_masterMonsters[16].weaponImage = *(m_smallImages+17);  
  m_masterMonsters[16].imageSize = 16;
  m_masterMonsters[16].level = 1;
  m_masterMonsters[16].color = PURPLE;
  // Door Monster
  m_masterMonsters[17].itemType = DOORMONSTER;
  m_masterMonsters[17].mapX = 0;
  m_masterMonsters[17].mapY = 0;
  m_masterMonsters[17].hitPoints = 50;
  m_masterMonsters[17].armorClass = 15;
  m_masterMonsters[17].damage = 25;
  m_masterMonsters[17].enabled = true;
  m_masterMonsters[17].image = *(m_largeImages+18);
  m_masterMonsters[17].imageSize = 16;
  m_masterMonsters[17].level = 1;
  m_masterMonsters[17].color = m_itemColors[0];
}

void M_InitMasterItems() {
  // Flour
  m_masterItems[0].itemType = FLOUR;
  m_masterItems[0].mapX = 0;
  m_masterItems[0].mapY = 0;
  m_masterItems[0].restorePoints = 10;
  m_masterItems[0].armorClass = 0;
  m_masterItems[0].damage = 0;  
  m_masterItems[0].reusable = 0;
  m_masterItems[0].canUse = true;
  m_masterItems[0].inInventory = false;
  m_masterItems[0].image = *(m_smallImages+0);
  m_masterItems[0].imageSize = 8;
  m_masterItems[0].level = 1;
  m_masterItems[0].color = TAN;        
  // Potion Large
  m_masterItems[1].itemType = POTION;
  m_masterItems[1].mapX = 0;
  m_masterItems[1].mapY = 0;
  m_masterItems[1].restorePoints = 20;
  m_masterItems[1].armorClass = 0;
  m_masterItems[1].damage = 0;  
  m_masterItems[1].reusable = 0;
  m_masterItems[1].canUse = true;
  m_masterItems[1].inInventory = false;
  m_masterItems[1].image = *(m_smallImages+1);
  m_masterItems[1].imageSize = 8;
  m_masterItems[1].level = 1;
  m_masterItems[1].color = m_itemColors[0];        
  // Potion Small
  m_masterItems[2].itemType = POTION;
  m_masterItems[2].mapX = 0;
  m_masterItems[2].mapY = 0;
  m_masterItems[2].restorePoints = 10;
  m_masterItems[2].armorClass = 0;
  m_masterItems[2].damage = 0;  
  m_masterItems[2].reusable = 0;
  m_masterItems[2].canUse = true;
  m_masterItems[2].inInventory = false;
  m_masterItems[2].image = *(m_smallImages+2);
  m_masterItems[2].imageSize = 8;
  m_masterItems[2].level = 1;
  m_masterItems[2].color = m_itemColors[0];        
  // Arrows
  m_masterItems[3].itemType = ARROWS;
  m_masterItems[3].mapX = 0;
  m_masterItems[3].mapY = 0;
  m_masterItems[3].restorePoints = 0;
  m_masterItems[3].armorClass = 0;
  m_masterItems[3].damage = 0;  
  m_masterItems[3].reusable = 0;
  m_masterItems[3].canUse = true;
  m_masterItems[3].inInventory = false;
  m_masterItems[3].image = *(m_smallImages+3);
  m_masterItems[3].imageSize = 8;
  m_masterItems[3].level = 1;
  m_masterItems[3].color = TAN;        
  // Armor
  m_masterItems[4].itemType = ARMOR;
  m_masterItems[4].mapX = 0;
  m_masterItems[4].mapY = 0;
  m_masterItems[4].restorePoints = 0;
  m_masterItems[4].armorClass = 5;
  m_masterItems[4].damage = 0;  
  m_masterItems[4].reusable = 1;
  m_masterItems[4].canUse = true;
  m_masterItems[4].inInventory = false;
  m_masterItems[4].image = *(m_smallImages+4);
  m_masterItems[4].imageSize = 8;
  m_masterItems[4].level = 1;
  m_masterItems[4].color = m_itemColors[0];        
  // Gauntlets
  m_masterItems[5].itemType = GAUNTLETS;
  m_masterItems[5].mapX = 0;
  m_masterItems[5].mapY = 0;
  m_masterItems[5].restorePoints = 0;
  m_masterItems[5].armorClass = 2;
  m_masterItems[5].damage = 0;  
  m_masterItems[5].reusable = 1;
  m_masterItems[5].canUse = true;
  m_masterItems[5].inInventory = false;
  m_masterItems[5].image = *(m_smallImages+5);
  m_masterItems[5].imageSize = 8;
  m_masterItems[5].level = 1;
  m_masterItems[5].color = m_itemColors[0];        
  // Helmet
  m_masterItems[6].itemType = HELMET;
  m_masterItems[6].mapX = 0;
  m_masterItems[6].mapY = 0;
  m_masterItems[6].restorePoints = 0;
  m_masterItems[6].armorClass = 2;
  m_masterItems[6].damage = 0;  
  m_masterItems[6].reusable = 1;
  m_masterItems[6].canUse = true;
  m_masterItems[6].inInventory = false;
  m_masterItems[6].image = *(m_smallImages+6);
  m_masterItems[6].imageSize = 8;
  m_masterItems[6].level = 1;
  m_masterItems[6].color = m_itemColors[0];        
  // Ring
  m_masterItems[7].itemType = RING;
  m_masterItems[7].mapX = 0;
  m_masterItems[7].mapY = 0;
  m_masterItems[7].restorePoints = 0;
  m_masterItems[7].armorClass = 2;
  m_masterItems[7].damage = 0;  
  m_masterItems[7].reusable = 1;
  m_masterItems[7].canUse = true;
  m_masterItems[7].inInventory = false;
  m_masterItems[7].image = *(m_smallImages+7);
  m_masterItems[7].imageSize = 8;
  m_masterItems[7].level = 1;
  m_masterItems[7].color = m_itemColors[0];        
  // Shield
  m_masterItems[8].itemType = SHIELD;
  m_masterItems[8].mapX = 0;
  m_masterItems[8].mapY = 0;
  m_masterItems[8].restorePoints = 0;
  m_masterItems[8].armorClass = 2;
  m_masterItems[8].damage = 0;  
  m_masterItems[8].reusable = 1;
  m_masterItems[8].canUse = true;
  m_masterItems[8].inInventory = false;
  m_masterItems[8].image = *(m_smallImages+8);
  m_masterItems[8].imageSize = 8;
  m_masterItems[8].level = 1;
  m_masterItems[8].color = m_itemColors[0];        
  // Bow
  m_masterItems[9].itemType = WEAPON;
  m_masterItems[9].mapX = 0;
  m_masterItems[9].mapY = 0;
  m_masterItems[9].restorePoints = 0;
  m_masterItems[9].armorClass = 0;
  m_masterItems[9].damage = 6;  
  m_masterItems[9].reusable = 2;
  m_masterItems[9].canUse = true;
  m_masterItems[9].inInventory = false;
  m_masterItems[9].image = *(m_smallImages+11);
  m_masterItems[9].imageSize = 8;
  m_masterItems[9].level = 1;
  m_masterItems[9].color = m_itemColors[0];        
  // Scroll
  m_masterItems[10].itemType = WEAPON;
  m_masterItems[10].mapX = 0;
  m_masterItems[10].mapY = 0;
  m_masterItems[10].restorePoints = 0;
  m_masterItems[10].armorClass = 0;
  m_masterItems[10].damage = 5;  
  m_masterItems[10].reusable = 1;
  m_masterItems[10].canUse = true;
  m_masterItems[10].inInventory = false;
  m_masterItems[10].image = *(m_smallImages+16);
  m_masterItems[10].imageSize = 8;
  m_masterItems[10].level = 1;
  m_masterItems[10].color = m_itemColors[0];        
  // Knife
  m_masterItems[11].itemType = WEAPON;
  m_masterItems[11].mapX = 0;
  m_masterItems[11].mapY = 0;
  m_masterItems[11].restorePoints = 0;
  m_masterItems[11].armorClass = 0;
  m_masterItems[11].damage = 10;  
  m_masterItems[11].reusable = 0;
  m_masterItems[11].canUse = true;
  m_masterItems[11].inInventory = false;
  m_masterItems[11].image = *(m_smallImages+14);
  m_masterItems[11].imageSize = 8;
  m_masterItems[11].level = 1;
  m_masterItems[11].color = m_itemColors[0];        
  // Axe
  m_masterItems[12].itemType = WEAPON;
  m_masterItems[12].mapX = 0;
  m_masterItems[12].mapY = 0;
  m_masterItems[12].restorePoints = 0;
  m_masterItems[12].armorClass = 0;
  m_masterItems[12].damage = 15;  
  m_masterItems[12].reusable = 0;
  m_masterItems[12].canUse = true;
  m_masterItems[12].inInventory = false;
  m_masterItems[12].image = *(m_smallImages+9);
  m_masterItems[12].imageSize = 8;
  m_masterItems[12].level = 1;
  m_masterItems[12].color = m_itemColors[0];        
  // Fireball
  m_masterItems[13].itemType = WEAPON;
  m_masterItems[13].mapX = 0;
  m_masterItems[13].mapY = 0;
  m_masterItems[13].restorePoints = 0;
  m_masterItems[13].armorClass = 0;
  m_masterItems[13].damage = 15;  
  m_masterItems[13].reusable = 0;
  m_masterItems[13].canUse = true;
  m_masterItems[13].inInventory = false;
  m_masterItems[13].image = *(m_smallImages+13);
  m_masterItems[13].imageSize = 8;
  m_masterItems[13].level = 1;
  m_masterItems[13].color = m_itemColors[0];        
  // Book
  m_masterItems[14].itemType = WEAPON;
  m_masterItems[14].mapX = 0;
  m_masterItems[14].mapY = 0;
  m_masterItems[14].restorePoints = 0;
  m_masterItems[14].armorClass = 0;
  m_masterItems[14].damage = 10;  
  m_masterItems[14].reusable = 1;
  m_masterItems[14].canUse = true;
  m_masterItems[14].inInventory = false;
  m_masterItems[14].image = *(m_smallImages+10);
  m_masterItems[14].imageSize = 8;
  m_masterItems[14].level = 1;
  m_masterItems[14].color = m_itemColors[0];        
  // Crossbow
  m_masterItems[15].itemType = WEAPON;
  m_masterItems[15].mapX = 0;
  m_masterItems[15].mapY = 0;
  m_masterItems[15].restorePoints = 0;
  m_masterItems[15].armorClass = 0;
  m_masterItems[15].damage = 15;  
  m_masterItems[15].reusable = 2;
  m_masterItems[15].canUse = true;
  m_masterItems[15].inInventory = false;
  m_masterItems[15].image = *(m_smallImages+12);
  m_masterItems[15].imageSize = 8;
  m_masterItems[15].level = 1;
  m_masterItems[15].color = m_itemColors[0];        
  // Lightning
  m_masterItems[16].itemType = WEAPON;
  m_masterItems[16].mapX = 0;
  m_masterItems[16].mapY = 0;
  m_masterItems[16].restorePoints = 0;
  m_masterItems[16].armorClass = 0;
  m_masterItems[16].damage = 20;  
  m_masterItems[16].reusable = 0;
  m_masterItems[16].canUse = true;
  m_masterItems[16].inInventory = false;
  m_masterItems[16].image = *(m_smallImages+15);
  m_masterItems[16].imageSize = 8;
  m_masterItems[16].level = 1;
  m_masterItems[16].color = m_itemColors[0];        
  // Spear
  m_masterItems[17].itemType = WEAPON;
  m_masterItems[17].mapX = 0;
  m_masterItems[17].mapY = 0;
  m_masterItems[17].restorePoints = 0;
  m_masterItems[17].armorClass = 0;
  m_masterItems[17].damage = 25;  
  m_masterItems[17].reusable = 0;
  m_masterItems[17].canUse = true;
  m_masterItems[17].inInventory = false;
  m_masterItems[17].image = *(m_smallImages+17);
  m_masterItems[17].imageSize = 8;
  m_masterItems[17].level = 1;
  m_masterItems[17].color = m_itemColors[0];        
  // Lamp
  m_masterItems[18].itemType = LAMP;
  m_masterItems[18].mapX = 0;
  m_masterItems[18].mapY = 0;
  m_masterItems[18].restorePoints = 0;
  m_masterItems[18].armorClass = 0;
  m_masterItems[18].damage = 0;  
  m_masterItems[18].reusable = 1;
  m_masterItems[18].canUse = true;
  m_masterItems[18].inInventory = false;
  m_masterItems[18].image = *(m_smallImages+18);
  m_masterItems[18].imageSize = 8;
  m_masterItems[18].level = 2; // Level 2 = X-Ray Lamp, Level 3 = Teleport, Level 4 = Item Upgrade
  m_masterItems[18].color = m_itemColors[0];        
  // Eyeball
  m_masterItems[19].itemType = EYEBALL;
  m_masterItems[19].mapX = 0;
  m_masterItems[19].mapY = 0;
  m_masterItems[19].restorePoints = 0;
  m_masterItems[19].armorClass = 0;
  m_masterItems[19].damage = 0;  
  m_masterItems[19].reusable = 0;
  m_masterItems[19].canUse = false;
  m_masterItems[19].inInventory = false;
  m_masterItems[19].image = *(m_smallImages+19);
  m_masterItems[19].imageSize = 8;
  m_masterItems[19].level = 1;
  m_masterItems[19].color = MEDGREEN;        
  // Ladder
  m_masterItems[20].itemType = LADDER;
  m_masterItems[20].mapX = 0;
  m_masterItems[20].mapY = 0;
  m_masterItems[20].restorePoints = 0;
  m_masterItems[20].armorClass = 0;
  m_masterItems[20].damage = 0;  
  m_masterItems[20].reusable = 1;
  m_masterItems[20].canUse = true;
  m_masterItems[20].inInventory = false;
  m_masterItems[20].image = *(m_smallImages+20);
  m_masterItems[20].imageSize = 8;
  m_masterItems[20].level = 1;
  m_masterItems[20].color = MEDBLUE;        
  // Treasure of Tarmin
  m_masterItems[21].itemType = TREASURE;
  m_masterItems[21].mapX = 0;
  m_masterItems[21].mapY = 0;
  m_masterItems[21].restorePoints = 0;
  m_masterItems[21].armorClass = 0;
  m_masterItems[21].damage = 0;  
  m_masterItems[21].reusable = false;
  m_masterItems[21].canUse = true;
  m_masterItems[21].inInventory = false;
  m_masterItems[21].image = *(m_largeImages+17);
  m_masterItems[21].imageSize = 16;
  m_masterItems[21].level = 1;
  m_masterItems[21].color = YELLOW;        
}

void M_InitMap() {
  // Game map is a 12x12 grid, with the outer ring being an empty hallway
  // Game map made up of 4 map segments - top left, top right, bottom right, bottom left
  // Map segments randomaly chosen from 16 possible map segments (m_map array)
  // Each map segment is 6x6 with an outer hallway on 2 sides and the internal rooms (5x5)
  // All 4 map segments oriented N,E,S,W for the bottom left position
  // Need to rotate them for top left, top right and bottom right positions once copied to master gameMap
  unsigned char rnd;
  rnd = random(0,16);
  M_CopyGameSegment(0, rnd); // Top left of gameMap
  rnd = random(0,16);
  M_CopyGameSegment(1, rnd); // Top right of gameMap
  rnd = random(0,16);
  M_CopyGameSegment(2, rnd); // Bottom right of gameMap
  rnd = random(0,16);
  M_CopyGameSegment(3, rnd); // Bottom left of gameMap
  M_RotateGameMap();
}

// Copy the static map segment into the correct position on the gameMap
// 0 top left of gameMap, 1 top right, 2 bottom right, 3 bottom left
void M_CopyGameSegment(unsigned char mapPosition, unsigned char mapSegment) {
  unsigned char xOffset = 0;
  unsigned char yOffset = 0;
  if (mapPosition == 1) {
    xOffset = 6;
  } else if (mapPosition == 2) {
    xOffset = 6;
    yOffset = 6;
  } else if (mapPosition == 3) {
    yOffset = 6;
  }
  for (int x=0; x<6; x++) {
    for (int y=0; y<6; y++) {
      m_gameMap[x+xOffset][y+yOffset] = m_maps[mapSegment][x][y];
    }
  }
}

// All 16 map segments stored with North as up. Need to rotate them on the gameMap
// based on their position
void M_RotateGameMap() {
  int n=6;
  unsigned char xOffset = 0;
  unsigned char yOffset = 0;
  unsigned char tmp;
  // top left segment, rotate 3 times counter-clockwise
  for (int z=0; z<3; z++) {
    for (int i=0; i<n/2; i++){
      for (int j=i; j<n-i-1; j++){
        tmp = M_RotateRoomLeft(m_gameMap[i+xOffset][j+yOffset]);
        m_gameMap[i+xOffset][j+yOffset] = M_RotateRoomLeft(m_gameMap[j+xOffset][n-i-1+yOffset]);
        m_gameMap[j+xOffset][n-i-1+yOffset] = M_RotateRoomLeft(m_gameMap[n-i-1+xOffset][n-j-1+yOffset]);
        m_gameMap[n-i-1+xOffset][n-j-1+yOffset] = M_RotateRoomLeft(m_gameMap[n-j-1+xOffset][i+yOffset]);
        m_gameMap[n-j-1+xOffset][i+yOffset]=tmp;
      }
    }
  }
  // top right segment, rotate 2 times counter-clockwise
  xOffset = 0;
  yOffset = 6; // x/y reversed because map segments are accessed as [y][x] in m_maps array
  for (int z=0; z<2; z++) {
    for (int i=0; i<n/2; i++){
      for (int j=i; j<n-i-1; j++){
        tmp = M_RotateRoomLeft(m_gameMap[i+xOffset][j+yOffset]);
        m_gameMap[i+xOffset][j+yOffset] = M_RotateRoomLeft(m_gameMap[j+xOffset][n-i-1+yOffset]);
        m_gameMap[j+xOffset][n-i-1+yOffset] = M_RotateRoomLeft(m_gameMap[n-i-1+xOffset][n-j-1+yOffset]);
        m_gameMap[n-i-1+xOffset][n-j-1+yOffset] = M_RotateRoomLeft(m_gameMap[n-j-1+xOffset][i+yOffset]);
        m_gameMap[n-j-1+xOffset][i+yOffset]=tmp;
      }
    }
  }
  // bottom right segment, rotate 1 time counter-clockwise
  xOffset = 6;
  yOffset = 6;
  for (int i=0; i<n/2; i++){
    for (int j=i; j<n-i-1; j++){
      tmp = M_RotateRoomLeft(m_gameMap[i+xOffset][j+yOffset]);
      m_gameMap[i+xOffset][j+yOffset] = M_RotateRoomLeft(m_gameMap[j+xOffset][n-i-1+yOffset]);
      m_gameMap[j+xOffset][n-i-1+yOffset] = M_RotateRoomLeft(m_gameMap[n-i-1+xOffset][n-j-1+yOffset]);
      m_gameMap[n-i-1+xOffset][n-j-1+yOffset] = M_RotateRoomLeft(m_gameMap[n-j-1+xOffset][i+yOffset]);
      m_gameMap[n-j-1+xOffset][i+yOffset]=tmp;
    }
  }
  // bottom left segment doesn't need rotation, all map segments stored North up in the bottom left position rotation
}

// Rotates the binary number representing the room
unsigned char M_RotateRoomLeft(unsigned char room) {
   /* In room<<2, last 2 bits are 0. To put first 2 bits of room at 
     last, do bitwise OR of room<<s with room >>(8 - 2) */
   return (room << 2)|(room >> (8 - 2));
}

// Clear the in-game progress map that tracks which rooms you've visited.
void M_ClearProgressMap() {
  for(int x=0; x<12; x++) {
    m_progressMap[x] = 0;
  }
}

void M_UpdateProgressMap() {
  m_progressMap[m_player.mapY] = m_progressMap[m_player.mapY] | (1 << m_player.mapX);
}

void M_PrintProgressMap() {
  Serial.println(" ");
  for(int x=0; x<12; x++) {
      for(int z=0; z < 16; z++) {
        Serial.print((m_progressMap[x] >> z) & 1);
        Serial.print(" ");
        //Serial.print(m_progressMap[x][y] & (1 << z));
      }
  Serial.println(" ");
  }
  Serial.println(" ");
}


