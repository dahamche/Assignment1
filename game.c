#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_GRID_SIZE 10
#define MIN_GRID_SIZE 3
#define MAX_PLAYERS 3
#define LOG_FILE "game_log.txt"

//player types
typedef enum{
	HUMAN_PLAYER,
	COMPUTER_PLAYER
} PlayerType;

//Game structure
typedef struct{
	char **board;
	int size;
	int num_players;
	char symbols[MAX_PLAYERS];
	PlayerType player_types[MAX_PLAYERS];
	int current_player;
	int moves_made;
	FILE *log_file;
} Game;
