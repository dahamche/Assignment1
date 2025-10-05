#include "tictactoe.h"

int main() {
    srand(time(NULL)); // Seed for random number generation

    int size, num_players, mode;
    Game *game = NULL;

    printf("=== Tic-Tac-Toe ===\n\n");

    // Get board size
    do {
        printf("Enter board size (%d-%d): ", MIN_SIZE, MAX_SIZE);
        if (scanf("%d", &size) != 1 || size < MIN_SIZE || size > MAX_SIZE) {
            printf("Invalid size! Please enter a number between %d and %d.\n", MIN_SIZE, MAX_SIZE);
            // Clear invalid input
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        break;
    } while (1);

    // Get game mode
    printf("\nSelect game mode:\n");
    printf("1. Two Player (Human vs Human)\n");
    printf("2. User vs Computer\n");
    printf("3. Multi-Player (3 players)\n");
    printf("Enter choice (1-3): ");

    do {
        if (scanf("%d", &mode) != 1 || mode < 1 || mode > 3) {
            printf("Invalid choice! Please enter 1, 2, or 3: ");
            // Clear invalid input
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        break;
    } while (1);

    // Set number of players based on mode
    switch (mode) {
        case 1:
        case 2:
            num_players = 2;
            break;
        case 3:
            num_players = 3;
            break;
        default:
            printf("Invalid mode selected!\n");
            return 1;
    }

    // Initialize game
    game = initialize_game(size, num_players);
    if (!game) {
        printf("Failed to initialize game!\n");
        return 1;
    }

    // Setup players
    setup_players(game);

    // Play the game
    play_game(game);

    // Ask if they want to play again
    char play_again;
    printf("\nWould you like to play again? (y/n): ");
    scanf(" %c", &play_again);

    // Cleanup
    cleanup_game(game);

    if (play_again == 'y' || play_again == 'Y') {
        printf("\n");
        main(); // Recursive call to restart
    } else {
        printf("\nThanks for playing! Check '%s' for game history.\n", LOG_FILE);
    }

    return 0;
}
