#ifndef TICTACTOE_H
#define TICTACTOE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Constants
#define MIN_SIZE 3
#define MAX_SIZE 10
#define MAX_PLAYERS 3
#define LOG_FILE "game_log.txt"

// Player types
typedef enum {
    HUMAN,
    COMPUTER
} PlayerType;

// Player structure
typedef struct {
    char symbol;
    PlayerType type;
    char name[50];
} Player;

// Game structure
typedef struct {
    char **board;
    int size;
    int num_players;
    Player players[MAX_PLAYERS];
    int current_player;
    FILE *log_file;
} Game;

// Function prototypes
Game* initialize_game(int size, int num_players);
void setup_players(Game *game);
void display_board(Game *game);
void display_instructions(Game *game);
int get_user_move(Game *game, int *row, int *col);
int validate_move(Game *game, int row, int col);
int make_move(Game *game, int row, int col);
void computer_move(Game *game);
int check_win(Game *game, int row, int col);
int check_draw(Game *game);
void log_move(Game *game, int row, int col);
void log_game_state(Game *game);
void cleanup_game(Game *game);
void play_game(Game *game);


#endif
