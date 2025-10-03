#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


#define MAX_GRID_SIZE 10
#define MIN_GRID_SIZE 3
#define MAX_PLAYERS 3
#define LOG_FILE "game_log.txt"

// Player types
typedef enum {
    HUMAN_PLAYER,
    COMPUTER_PLAYER
} PlayerType;

// Game structure
typedef struct {
    char **board;
    int size;
    int num_players;
    char symbols[MAX_PLAYERS];
    PlayerType player_types[MAX_PLAYERS];
    int current_player;
    int moves_made;
    FILE *log_file;
} Game;

// Function prototypes
Game* initializeGame(int size, int num_players);
void destroyGame(Game *game);
void displayBoard(Game *game);
int validateInput(Game *game, int row, int col);
int makeMove(Game *game, int row, int col);
int checkWinCondition(Game *game);
int checkDraw(Game *game);
void getUserInput(Game *game, int *row, int *col);
void generateComputerMove(Game *game, int *row, int *col);
void logMove(Game *game, int row, int col);
void playGame(Game *game);
void setupPlayers(Game *game);
void displayGameStatus(Game *game);

// Initialize the game board and structures
Game* initializeGame(int size, int num_players) {
    Game *game = (Game*)malloc(sizeof(Game));
    if (!game) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    game->size = size;
    game->num_players = num_players;
    game->current_player = 0;
    game->moves_made = 0;

    // Allocate memory for board
    game->board = (char**)malloc(size * sizeof(char*));
    for (int i = 0; i < size; i++) {
        game->board[i] = (char*)malloc(size * sizeof(char));
        for (int j = 0; j < size; j++) {
            game->board[i][j] = ' ';
        }
    }

    // Initialize player symbols
    game->symbols[0] = 'X';
    game->symbols[1] = 'O';
    game->symbols[2] = 'Z';

    // Open log file
    game->log_file = fopen(LOG_FILE, "w");
    if (game->log_file) {
        fprintf(game->log_file, "=== TIC-TAC-TOE GAME LOG ===\n");
        fprintf(game->log_file, "Grid Size: %dx%d\n", size, size);
        fprintf(game->log_file, "Number of Players: %d\n\n", num_players);
        fflush(game->log_file);
    }

    return game;
}

// Clean up memory and close files
void destroyGame(Game *game) {
    if (!game) return;

    // Free board memory
    for (int i = 0; i < game->size; i++) {
        free(game->board[i]);
    }
    free(game->board);

    // Close log file
    if (game->log_file) {
        fclose(game->log_file);
    }

    free(game);
}

// Display the current game board
void displayBoard(Game *game) {
    printf("\n");

    // Print column headers
    printf("   ");
    for (int i = 0; i < game->size; i++) {
        printf("%3d", i + 1);
    }
    printf("\n");

    // Print board with row numbers
    for (int i = 0; i < game->size; i++) {
        printf("%2d ", i + 1);
        for (int j = 0; j < game->size; j++) {
            printf(" %c ", game->board[i][j]);
            if (j < game->size - 1) printf("|");
        }
        printf("\n");

        if (i < game->size - 1) {
            printf("   ");
            for (int j = 0; j < game->size; j++) {
                printf("---");
                if (j < game->size - 1) printf("+");
            }
            printf("\n");
        }
    }
    printf("\n");
}

// Validate user input for move
int validateInput(Game *game, int row, int col) {
    // Check bounds
    if (row < 0 || row >= game->size || col < 0 || col >= game->size) {
        printf("Invalid position! Please enter row and column between 1 and %d.\n", game->size);
        return 0;
    }

    // Check if position is already occupied
    if (game->board[row][col] != ' ') {
        printf("Position already occupied! Please choose another position.\n");
        return 0;
    }

    return 1;
}

// Make a move on the board
int makeMove(Game *game, int row, int col) {
    if (!validateInput(game, row, col)) {
        return 0;
    }

    game->board[row][col] = game->symbols[game->current_player];
    game->moves_made++;

    // Log the move
    logMove(game, row, col);

    return 1;
}

// Check for win condition
int checkWinCondition(Game *game) {
    char symbol = game->symbols[game->current_player];

    // Check rows
    for (int i = 0; i < game->size; i++) {
        int count = 0;
        for (int j = 0; j < game->size; j++) {
            if (game->board[i][j] == symbol) {
                count++;
            }
        }
        if (count == game->size) return 1;
    }

    // Check columns
    for (int j = 0; j < game->size; j++) {
        int count = 0;
        for (int i = 0; i < game->size; i++) {
            if (game->board[i][j] == symbol) {
                count++;
            }
        }
        if (count == game->size) return 1;
    }

    // Check main diagonal
    int count = 0;
    for (int i = 0; i < game->size; i++) {
        if (game->board[i][i] == symbol) {
            count++;
        }
    }
    if (count == game->size) return 1;

    // Check anti-diagonal
    count = 0;
    for (int i = 0; i < game->size; i++) {
        if (game->board[i][game->size - 1 - i] == symbol) {
            count++;
        }
    }
    if (count == game->size) return 1;

    return 0;
}

// Check for draw condition
int checkDraw(Game *game) {
    return game->moves_made == (game->size * game->size);
}

// Get input from human player
void getUserInput(Game *game, int *row, int *col) {
    int input_row, input_col;

    do {
        printf("Player %d (%c), enter your move (row col): ",
               game->current_player + 1, game->symbols[game->current_player]);

        if (scanf("%d %d", &input_row, &input_col) != 2) {
            printf("Invalid input! Please enter two numbers.\n");
            // Clear input buffer
            while (getchar() != '\n');
            continue;
        }

        // Convert to 0-based indexing
        *row = input_row - 1;
        *col = input_col - 1;

    } while (!validateInput(game, *row, *col));
}

// Generate random move for computer player
void generateComputerMove(Game *game, int *row, int *col) {
    int attempts = 0;
    const int max_attempts = 1000;

    do {
        *row = rand() % game->size;
        *col = rand() % game->size;
        attempts++;

        if (attempts > max_attempts) {
            // Fallback: find first available position
            for (int i = 0; i < game->size; i++) {
                for (int j = 0; j < game->size; j++) {
                    if (game->board[i][j] == ' ') {
                        *row = i;
                        *col = j;
                        return;
                    }
                }
            }
        }
    } while (game->board[*row][*col] != ' ');

    printf("Computer Player %d (%c) chooses position: %d %d\n",
           game->current_player + 1, game->symbols[game->current_player],
           *row + 1, *col + 1);
}

// Log move to file
void logMove(Game *game, int row, int col) {
    if (game->log_file) {
        fprintf(game->log_file, "Move %d: Player %d (%c) -> Position (%d,%d)\n",
                game->moves_made, game->current_player + 1,
                game->symbols[game->current_player], row + 1, col + 1);

        // Log current board state
        fprintf(game->log_file, "Board State:\n");
        for (int i = 0; i < game->size; i++) {
            for (int j = 0; j < game->size; j++) {
                fprintf(game->log_file, "%c ", game->board[i][j]);
            }
            fprintf(game->log_file, "\n");
        }
        fprintf(game->log_file, "\n");
        fflush(game->log_file);
    }
}

// Setup player types for the game
void setupPlayers(Game *game) {
    printf("\n=== PLAYER SETUP ===\n");

    for (int i = 0; i < game->num_players; i++) {
        printf("Player %d (%c) - Choose type:\n", i + 1, game->symbols[i]);
        printf("1. Human Player\n");
        printf("2. Computer Player\n");
        printf("Enter choice (1-2): ");

        int choice;
        while (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2)) {
            printf("Invalid choice! Please enter 1 or 2: ");
            while (getchar() != '\n'); // Clear buffer
        }

        game->player_types[i] = (choice == 1) ? HUMAN_PLAYER : COMPUTER_PLAYER;
        printf("Player %d set as %s\n\n", i + 1,
               (choice == 1) ? "Human" : "Computer");
    }

    // Ensure at least one human player
    int human_count = 0;
    for (int i = 0; i < game->num_players; i++) {
        if (game->player_types[i] == HUMAN_PLAYER) {
            human_count++;
        }
    }

    if (human_count == 0) {
        printf("Warning: At least one player must be human. Setting Player 1 as human.\n");
        game->player_types[0] = HUMAN_PLAYER;
    }
}

// Display current game status
void displayGameStatus(Game *game) {
    printf("\n=== CURRENT GAME STATUS ===\n");
    printf("Grid Size: %dx%d\n", game->size, game->size);
    printf("Players: %d\n", game->num_players);
    printf("Moves Made: %d\n", game->moves_made);
    printf("Current Player: %d (%c)\n",
           game->current_player + 1, game->symbols[game->current_player]);
}

// Main game loop
void playGame(Game *game) {
    int row, col;

    printf("\n=== GAME STARTED ===\n");
    printf("Grid positions are numbered from 1 to %d\n", game->size);
    printf("Enter moves as: row column (e.g., 1 1 for top-left)\n");

    displayBoard(game);

    while (1) {
        displayGameStatus(game);

        // Get move based on player type
        if (game->player_types[game->current_player] == HUMAN_PLAYER) {
            getUserInput(game, &row, &col);
        } else {
            generateComputerMove(game, &row, &col);
        }

        // Make the move
        if (makeMove(game, row, col)) {
            displayBoard(game);

            // Check for win
            if (checkWinCondition(game)) {
                printf("\nGAME OVER! Player %d (%c) WINS! \n",
                       game->current_player + 1, game->symbols[game->current_player]);

                if (game->log_file) {
                    fprintf(game->log_file, "WINNER: Player %d (%c)\n",
                            game->current_player + 1, game->symbols[game->current_player]);
                }
                break;
            }

            // Check for draw
            if (checkDraw(game)) {
                printf("\n GAME OVER! IT'S A DRAW! \n");

                if (game->log_file) {
                    fprintf(game->log_file, "RESULT: DRAW\n");
                }
                break;
            }

            // Switch to next player
            game->current_player = (game->current_player + 1) % game->num_players;
        }
    }

    printf("\nGame log has been saved to '%s'\n", LOG_FILE);
}

// Main function with menu system
int main() {
    srand(time(NULL)); // Seed random number generator

    printf("=============================\n");
    printf("     TIC-TAC-TOE GAME   \n");
    printf("=============================\n");

    int size, num_players;

    // Get grid size
    do {
        printf("\nEnter grid size (%d-%d): ", MIN_GRID_SIZE, MAX_GRID_SIZE);
        if (scanf("%d", &size) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n'); // Clear buffer
            continue;
        }

        if (size < MIN_GRID_SIZE || size > MAX_GRID_SIZE) {
            printf("Grid size must be between %d and %d!\n", MIN_GRID_SIZE, MAX_GRID_SIZE);
        }
    } while (size < MIN_GRID_SIZE || size > MAX_GRID_SIZE);

    // Get number of players
    do {
        printf("Enter number of players (2-3): ");
        if (scanf("%d", &num_players) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n'); // Clear buffer
            continue;
        }

        if (num_players < 2 || num_players > MAX_PLAYERS) {
            printf("Number of players must be 2 or 3!\n");
        }
    } while (num_players < 2 || num_players > MAX_PLAYERS);

    // Initialize game
    Game *game = initializeGame(size, num_players);
    if (!game) {
        printf("Failed to initialize game!\n");
        return 1;
    }

    // Setup players
    setupPlayers(game);

    // Play the game
    playGame(game);

    // Cleanup
    destroyGame(game);

    printf("\nThank you for playing!\n");
    return 0;
}
