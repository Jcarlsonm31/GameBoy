// Board stored as a 36 x 4 bytes (2 ints) array, which gives a 32x36 grid of bits
// 1 stored at outside edge of map, all other bits 0
// map updated with 1's to track trail of snake and any obstacles for collision detection
unsigned int s_backupGameBoard[36][2] = {
	{65535, 65535},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{32768, 1},
	{65535, 65535}
};
