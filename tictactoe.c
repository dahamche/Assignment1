#include "tictactoe.h"


//Initialize the game with dynamic memory allocation

Game* initialize_game(int size, int num_players) {
    Game *game = (Game*)malloc(sizeof(Game));
    if (!game) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    // Allocate memory for the board
    game->board = (char**)malloc(size * sizeof(char*));
    if (!game->board) {
        printf("Memory allocation failed!\n");
        free(game);
        return NULL;
    }

    for (int i = 0; i < size; i++) {
        game->board[i] = (char*)malloc(size * sizeof(char));
        if (!game->board[i]) {
            printf("Memory allocation failed!\n");
            // Free previously allocated memory
            for (int j = 0; j < i; j++) {
                free(game->board[j]);
            }
            free(game->board);
            free(game);
            return NULL;
        }

        // Initialize board cells
        for (int j = 0; j < size; j++) {
            game->board[i][j] = ' ';
        }
    }

    game->size = size;
    game->num_players = num_players;
    game->current_player = 0;

    // Open log file
    game->log_file = fopen(LOG_FILE, "w");
    if (!game->log_file) {
        printf("Warning: Could not create log file.\n");
    } else {
        fprintf(game->log_file, "=== NEW TIC-TAC-TOE GAME ===\n");
        fprintf(game->log_file, "Board Size: %dx%d\n", size, size);
        fprintf(game->log_file, "Number of Players: %d\n", num_players);
        fprintf(game->log_file, "============================\n\n");
    }

    return game;
}


//Setup players based on game mode
 
void setup_players(Game *game) {
    char symbols[] = {'X', 'O', 'Z'};

    for (int i = 0; i < game->num_players; i++) {
        game->players[i].symbol = symbols[i];

        if (game->num_players == 2 && i == 1) {
            // Part 2: User vs Computer
            game->players[i].type = COMPUTER;
            strcpy(game->players[i].name, "Computer");
        } else if (game->num_players == 3 && i > 0) {
            // Part 3: Multi-player (at least one human)
            printf("Is Player %d human or computer? (h/c): ", i + 1);
            char choice;
            scanf(" %c", &choice);

            if (choice == 'h' || choice == 'H') {
                game->players[i].type = HUMAN;
                printf("Enter name for Player %d: ", i + 1);
                scanf("%s", game->players[i].name);
            } else {
                game->players[i].type = COMPUTER;
                sprintf(game->players[i].name, "Computer_%d", i + 1);
            }
        } else {
            // Human player
            game->players[i].type = HUMAN;
            printf("Enter name for Player %d: ", i + 1);
            scanf("%s", game->players[i].name);
        }
    }
}


//Display the current game board

void display_board(Game *game) {
    printf("\n   ");
    for (int j = 0; j < game->size; j++) {
        printf("   %d", j + 1);
    }
    printf("\n");

    for (int i = 0; i < game->size; i++) {
        printf("%2d ", i + 1);
        for (int j = 0; j < game->size; j++) {
            printf("| %c ", game->board[i][j]);
        }
        printf("|\n");

        printf("   ");
        for (int j = 0; j < game->size; j++) {
            printf("+---");
        }
        printf("+\n");
    }
}


 //Display game instructions

void display_instructions(Game *game) {
    printf("\n=== TIC-TAC-TOE GAME ===\n");
    printf("Board size: %dx%d\n", game->size, game->size);
    printf("Players: %d\n", game->num_players);

    for (int i = 0; i < game->num_players; i++) {
        printf("Player %d: %s (%c) - %s\n",
               i + 1,
               game->players[i].name,
               game->players[i].symbol,
               game->players[i].type == HUMAN ? "Human" : "Computer");
    }

    printf("\nTo win: Get %d symbols in a row (horizontal, vertical, or diagonal)\n", game->size);
    printf("Enter moves as row and column numbers (1-%d)\n", game->size);
}


//Get move input from human player
 
int get_user_move(Game *game, int *row, int *col) {
    printf("\n%s (%c), enter your move (row col): ",
           game->players[game->current_player].name,
           game->players[game->current_player].symbol);

    if (scanf("%d %d", row, col) != 2) {
        // Clear invalid input
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return 0;
    }

    // Convert to 0-based indexing
    (*row)--;
    (*col)--;

    return 1;
}


//Validate if the move is legal
 
int validate_move(Game *game, int row, int col) {
    if (row < 0 || row >= game->size || col < 0 || col >= game->size) {
        printf("Invalid position! Please enter values between 1 and %d.\n", game->size);
        return 0;
    }

    if (game->board[row][col] != ' ') {
        printf("Position already occupied! Choose another position.\n");
        return 0;
    }

    return 1;
}


//Make a move on the board

int make_move(Game *game, int row, int col) {
    if (!validate_move(game, row, col)) {
        return 0;
    }

    game->board[row][col] = game->players[game->current_player].symbol;
    log_move(game, row, col);
    return 1;
}


//Generate computer move
 
void computer_move(Game *game) {
    int row, col;

    printf("\n%s is thinking...\n", game->players[game->current_player].name);

    // Simple random strategy
    do {
        row = rand() % game->size;
        col = rand() % game->size;
    } while (game->board[row][col] != ' ');

    game->board[row][col] = game->players[game->current_player].symbol;

    printf("%s played at position (%d, %d)\n",
           game->players[game->current_player].name, row + 1, col + 1);

    log_move(game, row, col);
}


//Check if current player has won

int check_win(Game *game, int row, int col) {
    char symbol = game->players[game->current_player].symbol;
    int size = game->size;

    // Check row
    int count = 0;
    for (int j = 0; j < size; j++) {
        if (game->board[row][j] == symbol) {
            count++;
        }
    }
    if (count == size) return 1;

    // Check column
    count = 0;
    for (int i = 0; i < size; i++) {
        if (game->board[i][col] == symbol) {
            count++;
        }
    }
    if (count == size) return 1;

    // Check main diagonal
    if (row == col) {
        count = 0;
        for (int i = 0; i < size; i++) {
            if (game->board[i][i] == symbol) {
                count++;
            }
        }
        if (count == size) return 1;
    }

    // Check anti-diagonal
    if (row + col == size - 1) {
        count = 0;
        for (int i = 0; i < size; i++) {
            if (game->board[i][size - 1 - i] == symbol) {
                count++;
            }
        }
        if (count == size) return 1;
    }

    return 0;
}


//Check if game is a draw
 
int check_draw(Game *game) {
    for (int i = 0; i < game->size; i++) {
        for (int j = 0; j < game->size; j++) {
            if (game->board[i][j] == ' ') {
                return 0; // Still have empty spaces
            }
        }
    }
    return 1; // Board is full
}


//Log the move to file
 
void log_move(Game *game, int row, int col) {
    if (game->log_file) {
        fprintf(game->log_file, "Move: %s (%c) -> Position (%d, %d)\n",
                game->players[game->current_player].name,
                game->players[game->current_player].symbol,
                row + 1, col + 1);

        log_game_state(game);
        fflush(game->log_file);
    }
}


//Log current game state to file

void log_game_state(Game *game) {
    if (!game->log_file) return;

    fprintf(game->log_file, "Current Board State:\n");
    for (int i = 0; i < game->size; i++) {
        fprintf(game->log_file, "|");
        for (int j = 0; j < game->size; j++) {
            fprintf(game->log_file, " %c |", game->board[i][j]);
        }
        fprintf(game->log_file, "\n");
    }
    fprintf(game->log_file, "\n");
}


//Main game loop
 
void play_game(Game *game) {
    int row, col;
    int game_over = 0;

    display_instructions(game);

    while (!game_over) {
        display_board(game);

        // Get move based on player type
        if (game->players[game->current_player].type == HUMAN) {
            do {
                if (!get_user_move(game, &row, &col)) {
                    printf("Invalid input format! Please enter two numbers.\n");
                    continue;
                }
            } while (!make_move(game, row, col));
        } else {
            computer_move(game);
            row = -1; col = -1; // Find the last move made by computer
            for (int i = 0; i < game->size && row == -1; i++) {
                for (int j = 0; j < game->size && col == -1; j++) {
                    if (game->board[i][j] == game->players[game->current_player].symbol) {
                        // This is a simple way to find last move, not perfect but works for demo
                        row = i; col = j;
                    }
                }
            }
        }

        // Check for win
        if (check_win(game, row, col)) {
            display_board(game);
            printf("\n %s (%c) WINS! \n",
                   game->players[game->current_player].name,
                   game->players[game->current_player].symbol);

            if (game->log_file) {
                fprintf(game->log_file, "GAME RESULT: %s (%c) WINS!\n",
                        game->players[game->current_player].name,
                        game->players[game->current_player].symbol);
            }
            game_over = 1;
        } else if (check_draw(game)) {
            display_board(game);
            printf("\n Game is a DRAW! \n");

            if (game->log_file) {
                fprintf(game->log_file, "GAME RESULT: DRAW!\n");
            }
            game_over = 1;
        }

        // Move to next player
        if (!game_over) {
            game->current_player = (game->current_player + 1) % game->num_players;
        }
    }
}


//Clean up allocated memory and close files
 
void cleanup_game(Game *game) {
    if (game) {
        // Free board memory
        if (game->board) {
            for (int i = 0; i < game->size; i++) {
                if (game->board[i]) {
                    free(game->board[i]);
                }
            }
            free(game->board);
        }

        // Close log file
        if (game->log_file) {
            fprintf(game->log_file, "=== GAME ENDED ===\n");
            fclose(game->log_file);
        }

        free(game);
    }
}
