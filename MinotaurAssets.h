// Player info
struct m_structPlayer {
  char mapX;                // globalMap X coordinate
  char mapY;                // globalMap Y
  unsigned char currentLevel; // current dungeon level
  char forwardDir;          // direction player is facing
  char rightDir;            // direction on player's right
  char backDir;             // direction behind player
  char leftDir;             // diretion on player's left 
  unsigned char armorClass; // Used as a percent chance the monster will miss the attack
  int currentHealth;        // current hit points
  int maxHealth;            // current maximum hit points (rises after battles)
  int currentWeapon;        // index of current weapon in the m_invWeapons struct
  unsigned char numInvItems;   // number of items in your inventory
  unsigned char numInvWeapons; // number of weapons in your inventory
  unsigned int numArrows;   // number of arrows in your quiver
  int monstersKilled;       // stats for monsters killed
  int numAttacks;           // number of attacks
  int numHits;              // number of hits, to calculate hit percentage
};

// Holds the info for monsters
struct m_structMonsters {
  unsigned char itemType;     // MONSTER
  unsigned char mapX;         // globalMap X
  unsigned char mapY;         // globalMap Y
  int hitPoints;              // hitpoints of monster
  unsigned char armorClass;   // Used as a percent chance the player will miss the attack
  unsigned char damage;       // maximum damage the monster does on attack (actual damage is random)
  boolean enabled;            // is the monster alive
  const unsigned char *image; // monster image       
  const unsigned char *weaponImage; // image of the monster's weapon
  unsigned char imageSize;    // size of monster image    
  unsigned char level;        // monster's difficulty level - higher have more hitpoints and do more damage
  unsigned int color;         // monster's color, based on level
};

// Holds the info for items
struct m_structItems {
  unsigned char itemType;      // type of the item
  unsigned char mapX;          // globalMap X
  unsigned char mapY;          // globalMap Y
  unsigned char restorePoints; // hitpoints to be restored, used for potions and flour bags    
  unsigned char armorClass;    // armor class of an armor item 
  unsigned char damage;        // damage for a weapon
  unsigned char reusable; // 0 No, 1 Yes (books/scrolls), 2 Arrows (bows/crossbows)
  boolean inInventory;         // if the item is in your inventory or not
  boolean canUse;              // if you can use it, NO for hallway eyeballs
  const unsigned char *image;  // item image      
  unsigned char imageSize;     // item image size
  unsigned char level;         // item level - higher has more damage, restore points or armor class
  unsigned int color;          // item color based on level
};

// Store data for current room being processed
struct m_structRoomData {
  unsigned char mapX;        // global map X of the room
  unsigned char mapY;        // global map Y of the room
  unsigned char currentWall; // current wall being drawn 0N, 1E, 2S, 3W
  unsigned char room;        // binary wall data for the room
};

// Store data about visibile center rooms while drawing maze M_DrawMaze()
// Need this to correctly order drawing of items after walls
struct m_structVisibleCenterRooms {
  unsigned char virtualX; // position relative to player     
  unsigned char virtualY; // position relative to player
  unsigned char row;      // on the 3x3 viewable grid relative to the player  
  unsigned char column;   // on the 3x3 viewable grid relative to the player       
  boolean visible;        // if items in the room should be draw
};

// All images black and white BMP, converted to byte arrays using
// image2cpp - http://javl.github.io/image2cpp/
const unsigned char PROGMEM m_largeImages[19][32] = {
	{
		 // 0 'alligator'
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x07, 0xe0, 0x0f, 0x30, 0x1e, 0x00, 
    0x3e, 0xf0, 0x3f, 0xd8, 0x7f, 0xf4, 0x7f, 0xda, 0xcf, 0xcc, 0x03, 0xc0, 0x06, 0x70, 0x0c, 0x10
	},
	{
		 // 1 'ant'
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x00, 0x41, 0x00, 0x22, 0x0f, 0x3e, 
    0x7f, 0xdd, 0xff, 0xf7, 0xff, 0xa2, 0x7f, 0x94, 0x8e, 0x41, 0x84, 0x41, 0x48, 0x22, 0x04, 0x00 
	},
	{
		 // 2 'dragon'
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x0c, 0x6c, 0x1f, 0x56, 0x1c, 0x6b, 0x3b, 
    0x35, 0xb0, 0x7f, 0xf0, 0x9f, 0xf8, 0x9f, 0xf8, 0xdd, 0xf0, 0x58, 0xb0, 0x19, 0xb0, 0x0d, 0x9c 
	},
	{
		 // 3 'dwarf'
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x07, 0xc0, 0x07, 0xc0, 0x03, 0x80, 0x1f, 0xf0, 
    0x3f, 0xf8, 0x77, 0xdc, 0x6f, 0xec, 0x6f, 0xec, 0x07, 0xc0, 0x06, 0xc0, 0x06, 0xc0, 0x0e, 0xe0 
	},
	{
		 // 4 'dwarf_shield'
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x07, 0xc0, 0x07, 0xc0, 0x03, 0xb0, 0x1f, 0x78, 
    0x3e, 0xfc, 0x76, 0xcc, 0x6e, 0xfc, 0x6f, 0x78, 0x07, 0xb0, 0x07, 0xc0, 0x06, 0xc0, 0x0e, 0xe0 
	},
	{
		 // 5 'ghoul'
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x06, 0xc0, 0x04, 0x40, 0x02, 0x80, 0x1f, 0xf0, 
    0x3f, 0xf8, 0x37, 0xd8, 0x67, 0xcc, 0x4f, 0xe4, 0x0f, 0xe0, 0x1f, 0xf0, 0x1f, 0xf0, 0x3f, 0xf8 
	},
	{
		 // 6 'giant'
		0x03, 0x80, 0x07, 0xc0, 0x07, 0xc0, 0x07, 0xc0, 0x03, 0x80, 0x1f, 0xf0, 0x3f, 0xf8, 0x37, 0xd8, 
		0x37, 0xd8, 0x37, 0xd8, 0x27, 0xc8, 0x07, 0xc0, 0x06, 0xc0, 0x06, 0xc0, 0x06, 0xc0, 0x0e, 0xe0 
	},
	{
		 // 7 'giant_shield'
		0x03, 0x80, 0x07, 0xc0, 0x07, 0xc0, 0x07, 0xc8, 0x03, 0x9c, 0x1f, 0xbe, 0x3f, 0x77, 0x37, 0x77, 
		0x37, 0xbe, 0x37, 0xdc, 0x27, 0xc8, 0x07, 0xc0, 0x06, 0xc0, 0x06, 0xc0, 0x06, 0xc0, 0x0e, 0xe0 
	},
	{
		 // 8 'minotaur'
		0x04, 0x40, 0x03, 0x80, 0x05, 0x40, 0x06, 0xd0, 0x02, 0xb8, 0x0f, 0xb8, 0x1f, 0x7c, 0x36, 0xee, 
		0x36, 0xee, 0x17, 0x7c, 0x07, 0xb8, 0x07, 0xb8, 0x06, 0xd0, 0x06, 0xc0, 0x02, 0x80, 0x0e, 0xe0 
	},
	{
		 // 9 'scorpion'
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x01, 0xd8, 0x03, 0x80, 0x07, 0xc0, 
    0x07, 0xc0, 0x35, 0x58, 0x5f, 0xf4, 0x8f, 0xe2, 0x90, 0x12, 0x94, 0x52, 0x48, 0x24, 0x04, 0x40 
	},
	{
		 // 10 'skeleton'
		0x03, 0x80, 0x05, 0x40, 0x06, 0xc0, 0x03, 0x80, 0x01, 0x00, 0x0e, 0xe0, 0x15, 0x50, 0x12, 0x90, 
		0x25, 0x48, 0x03, 0x80, 0x04, 0x40, 0x07, 0xc0, 0x04, 0x40, 0x08, 0x20, 0x08, 0x20, 0x18, 0x30 
	},
	{
		 // 11 'skeleton_cloaked'
		0x03, 0x80, 0x05, 0x40, 0x06, 0xc0, 0x03, 0x80, 0x01, 0x00, 0x0e, 0xe0, 0x17, 0xd0, 0x37, 0xd8, 
		0x27, 0xc8, 0x07, 0xc0, 0x07, 0xc0, 0x07, 0xc0, 0x07, 0xc0, 0x0f, 0xe0, 0x0f, 0xe0, 0x1f, 0xf0 
	},
	{
		 // 12 'skeleton_cloaked_shield'
		0x03, 0x80, 0x05, 0x40, 0x06, 0xc0, 0x03, 0x80, 0x01, 0x10, 0x0e, 0xb8, 0x17, 0xec, 0x37, 0xec, 
		0x27, 0xb8, 0x07, 0x90, 0x07, 0xc0, 0x07, 0xc0, 0x07, 0xc0, 0x0f, 0xe0, 0x0f, 0xe0, 0x1f, 0xf0 
	},
	{
		 // 13 'skeleton_shield'
		0x03, 0x80, 0x05, 0x40, 0x06, 0xc0, 0x03, 0x80, 0x01, 0x10, 0x0e, 0xb8, 0x15, 0x6c, 0x12, 0x6c, 
		0x25, 0x38, 0x03, 0x90, 0x04, 0x40, 0x07, 0xc0, 0x04, 0x40, 0x08, 0x20, 0x08, 0x20, 0x18, 0x30 
	},
	{
		 // 14 'snake'
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0xfc, 0x01, 0xe4, 0x01, 0xf0, 
    0x00, 0xd8, 0x00, 0xc0, 0x70, 0x60, 0xff, 0x30, 0xc7, 0xf2, 0xc0, 0x39, 0x70, 0x66, 0x3f, 0xe0 
	},
	{
		 // 15 'wraith'
		0x03, 0x80, 0x06, 0xc0, 0x04, 0x40, 0x06, 0xc0, 0x03, 0x80, 0x0f, 0xe0, 0x1f, 0xf0, 0x37, 0xd8, 
		0x27, 0xc8, 0x27, 0xc8, 0x07, 0xc0, 0x07, 0xc0, 0x07, 0xc0, 0x0f, 0xe0, 0x0f, 0xe0, 0x1f, 0xf0 
	},
	{
		 // 16 'wraith_shield'
		0x03, 0x80, 0x06, 0xc0, 0x04, 0x40, 0x06, 0xc0, 0x03, 0x98, 0x0f, 0xbc, 0x1f, 0x7e, 0x37, 0x66, 
		0x27, 0x7e, 0x27, 0xbc, 0x07, 0xd8, 0x07, 0xc0, 0x07, 0xc0, 0x0f, 0xe0, 0x0f, 0xe0, 0x1f, 0xf0
	},
	{
		 // 17 'tarmin_treasure'
		0x00, 0x00, 0x03, 0x80, 0x06, 0xc0, 0x0d, 0x60, 0x1a, 0xb0, 0x35, 0x58, 0x0e, 0xac, 0x33, 0xfc, 
    0x38, 0x00, 0x3f, 0xfc, 0x37, 0xfc, 0x3f, 0xfc, 0x1f, 0xfc, 0x07, 0xfc, 0x01, 0xfc, 0x00, 0x00
	},
  {   // 18 'door_monster'
    0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8, 
    0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8, 0x7f, 0xf8
  }
};

const unsigned char PROGMEM m_smallImages[22][8] = {
  {
     // 0 'flour'
    0x81, 0x7e, 0x5a, 0x66, 0x5a, 0x7e, 0x81, 0x00
  },
	{
		 // 1 'potion_large'
		0x10, 0x7c, 0x10, 0x38, 0x7c, 0xfe, 0xfe, 0x7c 
	},
	{
		 // 2 'potion_small'
		0x00, 0x10, 0x38, 0x10, 0x38, 0x7c, 0x38, 0x00 
	},
	{
		 // 3 'arrows'
		0x00, 0x60, 0x90, 0xa0, 0x7f, 0x7f, 0x00, 0x00
	},
	{
		 // 4 'armor'
		0xc3, 0xff, 0x7e, 0x7e, 0xff, 0x7e, 0x3c, 0x00 
	},
	{
		 // 5 'gauntlets'
		0x00, 0x70, 0x18, 0x2c, 0x1e, 0x0e, 0x04, 0x00 
	},
	{
		 // 6 'helmet'
		0x00, 0x10, 0x38, 0x6c, 0xc6, 0xc6, 0x44, 0x00 
	},
	{
		 // 7 'ring'
		0x00, 0x00, 0x18, 0x24, 0x24, 0x18, 0x00, 0x00 
	},
	{
		 // 8 'shield'
		0x00, 0x7e, 0x7e, 0x7e, 0x3c, 0x3c, 0x18, 0x00
	},
	{
		 // 9 'axe'
		0x20, 0x70, 0xf8, 0xcc, 0xc6, 0x03, 0x01, 0x00 
	},
	{
		 // 10 'book'
		0x30, 0x78, 0xdc, 0xee, 0x7c, 0x3a, 0x1c, 0x08 
	},
	{
		 // 11 'bow'
		0x08, 0x10, 0x20, 0x20, 0x20, 0x20, 0x10, 0x08 
	},
	{
		 // 12 'crossbow'
		0x20, 0x40, 0x80, 0xff, 0xff, 0x80, 0x40, 0x20 
	},
	{
		 // 13 'fireball'
		0x44, 0xa2, 0x3c, 0x38, 0x78, 0x8a, 0x44, 0x00 
	},
	{
		 // 14 'knife'
		0x00, 0x40, 0x20, 0x14, 0x08, 0x14, 0x00, 0x00 
	},
	{
		 // 15 'lightning'
		0x00, 0xc0, 0x70, 0x38, 0x1e, 0x07, 0x02, 0x00 
	},
	{
		 // 16 'scroll'
		0x04, 0x08, 0x1d, 0x3e, 0x7c, 0xb8, 0x10, 0x20 
	},
	{
		 // 17 'spear'
		0xc0, 0xc0, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
	},
  {
     // 18 'lamp'
    0x00, 0x08, 0x1d, 0xdf, 0x7f, 0x3e, 0x00, 0x00
  },
	{
		 // 19 'eyeball'
		0x18, 0x24, 0x5a, 0xb5, 0xbd, 0x5a, 0x24, 0x18 
	},
	{
		 // 20 'ladder'
		0x42, 0x7e, 0x42, 0x7e, 0xc3, 0xff, 0x7e, 0x00 
	},
  {
     // 21 'arrow'
    0x00, 0x70, 0x60, 0x50, 0x08, 0x04, 0x02, 0x00
  }
};

unsigned char m_maps[16][6][6] = {
	// 16 possible map segments, each room's walls stored as a byte
	// Order of walls Top, Right, Bottom, Left
	// 00 open, 01 wall, 11 door
  // Door 11 can be set to 10 by the code to insert a Door Monster
  // TTRRBBLL
  // 11010001 (Door, wall, empty, wall)
  //
  // See TarminMap.png for visuals of the map segments
  // Map segments below follow the .PNG file, top row first, left to right
	{
		{B00010001, B01000001, B01010000, B11010001, B01000001, B01010000},
		{B00010001, B00000101, B00110100, B00110011, B00000111, B00010100},
		{B00110001, B01000111, B01000100, B00000000, B01000100, B01110100},
		{B00010001, B01000001, B01110000, B00110011, B01000011, B01010000},
		{B00010001, B00000101, B00010100, B00011101, B00000101, B00010100},
		{B00000101, B01000100, B01000100, B11000100, B01000100, B01000100}
	},
	{
		{B00010001, B01011101, B01011101, B11011101, B01000001, B01010100},
		{B00010001, B11011101, B11010001, B11011101, B00110001, B01010111},
		{B00110001, B11001111, B00000000, B11111100, B00000011, B01110100},
		{B00010001, B11011101, B00011101, B11011101, B00110001, B01010111},
		{B00010001, B11010101, B11010101, B11011101, B00000101, B01010100},
		{B00000101, B01000100, B01000100, B11000100, B01000100, B01000100}
	},
	{
		{B00010001, B01000101, B01110100, B11111111, B01000111, B01010100},
		{B00010001, B01011101, B01011101, B11011101, B01011101, B01011101},
		{B00110001, B11001111, B11001100, B11000100, B11001100, B11111100},
		{B00010001, B11011101, B11110101, B01010111, B11010101, B11010101},
		{B00010001, B11000101, B01110100, B01111111, B01000111, B01010100},
		{B00000101, B01000100, B01000100, B11000100, B01000100, B01000100}
	},
	{
		{B00010001, B01000001, B01110000, B11111111, B01000011, B01010000},
		{B00010001, B00000101, B00010100, B11011101, B00000101, B00010100},
		{B00110001, B01001111, B01110000, B11110011, B01000111, B01110000},
		{B00010001, B11010101, B00010001, B00010001, B01110101, B00010011},
		{B00010001, B01000101, B00010100, B00111101, B01010111, B00010101},
		{B00000101, B01000100, B01000100, B11000100, B01000100, B01000100}
	},
	{
		{B00010001, B01010001, B01110101, B11010011, B01000001, B01010100},
		{B00010001, B00000101, B01001100, B00010100, B00010001, B01011101},
		{B00110001, B01001111, B11110000, B01110111, B00001111, B11010100},
		{B00010001, B11010101, B00010001, B01000001, B11000100, B01010000},
		{B00010001, B01000101, B00010100, B00111101, B01010111, B00010101},
		{B00000101, B01000100, B01000100, B11000100, B01000100, B01000100}
	},
	{
		{B00010001, B01001101, B01010000, B11000001, B01000100, B01010000},
		{B00010001, B11010001, B00010001, B00110001, B01010111, B00010001},
		{B00110001, B00010011, B00111101, B00000111, B01000100, B00110100},
		{B00010001, B00000001, B11000100, B01001100, B01000100, B01010100},
		{B00010001, B00010101, B01000101, B11001100, B01000100, B01010100},
		{B00000101, B01000100, B01000100, B11000100, B01000100, B01000100}
	},
	{
		{B00010001, B01000001, B01010000, B11010001, B01000001, B01010000},
		{B00010001, B00000101, B00011100, B00111101, B00000111, B00010100},
		{B00110001, B01000111, B11110100, B11111111, B01001111, B01110100},
		{B00010001, B01000001, B01110000, B11010011, B11000001, B01010000},
		{B00010001, B00000101, B00010100, B00011101, B00000101, B00010100},
		{B00000101, B01000100, B01000100, B11000100, B01000100, B01000100}
	},
	{
		{B00010001, B01010001, B01010001, B11010001, B01010001, B01010001},
		{B00010001, B00011101, B00011101, B00011101, B00011101, B00011101},
		{B00110001, B11001111, B11001100, B11001100, B11001100, B11111100},
		{B00010001, B11010001, B11010001, B11010001, B11010001, B11010001},
		{B00010001, B00010101, B00010101, B00010101, B00010101, B00010101},
		{B00000101, B01000100, B01000100, B11000100, B01000100, B01000100}
	},
	{
		{B00010001, B01000001, B01010000, B11010001, B01000001, B01010000},
		{B00010001, B00000101, B00011100, B00011101, B00000101, B00011100},
		{B00110001, B01000111, B11110100, B11000011, B01000100, B11111100},
		{B00010001, B01000001, B01110000, B00010011, B01000001, B11010000},
		{B00010001, B00000101, B00010100, B00011101, B00000101, B00010100},
		{B00000101, B01000100, B01000100, B11000100, B01000100, B01000100}
	},
	{
		{B00010001, B01000001, B01000100, B11000100, B01110000, B01010011},
		{B00010001, B00010001, B01000001, B01010000, B00010001, B00010101},
		{B00110001, B00010011, B00000101, B00011100, B00000101, B01111100},
		{B00010001, B00001101, B01000100, B11010000, B01000001, B11010000},
		{B00010001, B11000101, B01010100, B00011101, B00000101, B00010100},
		{B00000101, B01000100, B01000100, B11000100, B01000100, B01000100}
	},
	{
		{B00010001, B01010001, B01011101, B11010001, B01011101, B01010001},
		{B00010001, B00000101, B11011100, B00010001, B11001101, B00010100},
		{B00110001, B01000111, B11000100, B00001100, B11000100, B01110100},
		{B00010001, B01110001, B01000011, B11110000, B01000011, B01010000},
		{B00010001, B00010101, B00000101, B00011100, B00000101, B00010100},
		{B00000101, B01000100, B01000100, B11000100, B01000100, B01000100}
	},
	{
		{B00010001, B01010001, B01000001, B11000100, B01010000, B01010001},
		{B00010001, B00010001, B00010001, B01010001, B00010001, B00010001},
		{B00110001, B00010011, B00010101, B00010001, B00011101, B00110001},
		{B00010001, B00000101, B01000100, B00001100, B11000100, B00010100},
		{B00010001, B01000101, B01000100, B11001100, B01000100, B01010100},
		{B00000101, B01000100, B01000100, B11000100, B01000100, B01000100}
	},
	{
		{B00010001, B01000001, B01010100, B11010001, B01000101, B01010000},
		{B00010001, B00011101, B01000001, B00000100, B01010000, B00011101},
		{B00110001, B11001111, B00110000, B01010111, B00000001, B11111100},
		{B00010001, B11010001, B00000101, B01000000, B00010100, B11010001},
		{B00010001, B00000101, B01010100, B00011101, B01000101, B00010100},
		{B00000101, B01000100, B01000100, B11000100, B01000100, B01000100}
	},
	{
		{B00010001, B01000001, B01110000, B11110011, B01000111, B01011100},
		{B00010001, B00000101, B00010100, B00000101, B01010000, B11010101},
		{B00110001, B01000111, B01110000, B01110111, B00000111, B01110100},
		{B00010001, B01011101, B00000101, B01010000, B01000001, B01010000},
		{B00010001, B11000101, B01110100, B00111111, B00000111, B00010100},
		{B00000101, B01000100, B01000100, B11000100, B01000100, B01000100}
	},
	{
		{B00010001, B01000101, B01011100, B11010001, B01000001, B01010000},
		{B00010001, B01110101, B11000011, B00011100, B00001101, B00010100},
		{B00110001, B01000111, B00010000, B11000101, B11010100, B01110001},
		{B00010001, B01110001, B00001111, B01000000, B01000100, B00010000},
		{B00010001, B00000101, B11010100, B00011101, B01110101, B00010111},
		{B00000101, B01000100, B01000100, B11000100, B01000100, B01000100}
	},
	{
		{B00010001, B01010001, B01110001, B11010111, B01110001, B01010011},
		{B00010001, B00011101, B00000101, B01000100, B00010100, B00011101},
		{B00110001, B11001111, B01000100, B01000100, B01000100, B11111100},
		{B00010001, B11010001, B01000001, B01000100, B01010000, B11010001},
		{B00010001, B00110101, B00010111, B01111101, B00010111, B00010101},
		{B00000101, B01000100, B01000100, B11000100, B01000100, B01000100}
	}
};

// Routines for drawing the wall segments - filled triangles for angled walls and filled rectangles for straight walls
// Static X,Y coordinates used for all drawing, based on position of wall relative to the player
// All rooms and walls are represented by a 3x3 grid with the player at the bottom center position
// Room, Room, Room - depth 2
// Room, Room, Room - depth 1
// Room, Player, Room - depth 0
//
// There are 3 depths of flat walls facing player
// There are 4 columns of side walls - far left, near left, near right, far right
unsigned char m_sideWalls[3][4][4][2] = {
  // points for bottom left, top left, top right, bottom right
  { // depth 0
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}},            // far left
    {{0, 89}, {0, 0}, {15, 10}, {15, 80}},       // near left
    {{113, 80}, {113, 10}, {127, 0}, {127, 89}}, // near right
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}}             // far right
  },
  { // depth 1
    {{0, 65}, {0, 25}, {15, 30}, {15, 60}},      
    {{15, 80}, {15, 10}, {37, 25}, {37, 66}},
    {{91, 66}, {91, 25}, {113, 10}, {113, 80}},
    {{113, 60}, {113, 30}, {127, 25}, {127, 65}}
  },
  { // depth 2
    {{15, 60}, {15, 30}, {37, 37}, {37, 53}},    
    {{37, 66}, {37, 25}, {52, 35}, {52, 56}},
    {{76, 56}, {76, 35}, {91, 25}, {91, 66}},
    {{91, 53}, {91, 37}, {113, 30}, {113, 60}}
  }      
};

unsigned char m_sideDoors[3][4][4][2] = {
  // points for bottom left, top left, top right, bottom right
  { // depth 0
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}},            // far left
    {{0, 89}, {0, 20}, {7, 24}, {7, 85}},       // near left
    {{120, 85}, {120, 24}, {127, 20}, {127, 89}}, // near right
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}}             // far right
  },
  { // depth 1
    {{0, 65}, {0, 34}, {8, 36}, {8, 62}},      
    {{21, 77}, {21, 29}, {31, 34}, {31, 70}},
    {{97, 70}, {97, 34}, {107, 29}, {107, 77}},
    {{120, 63}, {120, 36}, {127, 34}, {127, 65}}
  },
  { // depth 2
    {{23, 58}, {23, 39}, {31, 41}, {31, 55}},    
    {{42, 63}, {42, 37}, {48, 40}, {48, 59}},
    {{80, 59}, {80 , 40}, {86, 37}, {86, 63}},
    {{98, 55}, {98, 41}, {105, 39}, {105, 58}}
  }      
};

unsigned char m_flatWalls[3][3][4] = {
  // x, y, width, height
  { // depth 0
    {0, 10, 15, 71},            // far left
    {15, 10, 99, 71},            // middle
    {114, 10, 14, 71}             // far right
  },
  { // depth 1
    {0, 25, 37, 42},
    {37, 25, 55, 42},
    {91, 25, 37, 42}
  },
  { // depth 2
    {27, 35, 25, 22},
    {52, 35, 25, 22},
    {77, 35, 25, 22}
  }      
};

unsigned char m_flatDoors[3][3][4] = {
  // x, y, width, height
  { // depth 0
    {0, 0, 0, 0},            // far left
    {47, 25, 35, 56},            // middle
    {0, 0, 0, 0}             // far right
  },
  { // depth 1
    {0, 35, 19, 32},
    {54, 35, 20, 32},
    {109, 35, 19, 32}
  },
  { // depth 2
    {35, 40, 10, 17},
    {60, 40, 10, 17},
    {85, 40, 10, 17}
  }      
};
