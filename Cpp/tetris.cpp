#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string.h>
#include <thread>  // For threads
#include <chrono>  
#include <conio.h> // For _kbhit() and _getch()

#define WIDTH 8
#define HEIGHT 8

// Tetris pieces (Tetrominoes)
const int tetrominoes[7][4][4] = {
    {{1, 1, 1, 1}, // I-piece
     {0, 0, 0, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},

    {{1, 1, 0, 0}, // O-piece
     {1, 1, 0, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},

    {{0, 1, 1, 0}, // S-piece
     {1, 1, 0, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},

    {{1, 1, 0, 0}, // Z-piece
     {0, 1, 1, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},

    {{1, 1, 1, 0}, // T-piece
     {0, 1, 0, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},

    {{1, 0, 0, 0}, // L-piece
     {1, 1, 1, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},

    {{0, 0, 1, 0}, // J-piece
     {1, 1, 1, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},
};

// Game board
int board[HEIGHT][WIDTH] = {0};

// Current piece
int currentPiece[4][4];
int currentX = 0, currentY = 0;

// Function to copy a tetromino to the current piece
void copyPiece(const int piece[4][4]) {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            currentPiece[i][j] = piece[i][j];
}

// Rotate a piece clockwise
void rotatePiece() {
    int temp[4][4] = {0};
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            temp[j][3 - i] = currentPiece[i][j];
    memcpy(currentPiece, temp, sizeof(currentPiece));
}

// Check for collisions
bool isCollision(int offsetX, int offsetY) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (currentPiece[i][j]) {
                int x = currentX + j + offsetX;
                int y = currentY + i + offsetY;
                if (x < 0 || x >= WIDTH || y >= HEIGHT || (y >= 0 && board[y][x])) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Merge the current piece into the board
void mergePiece() {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (currentPiece[i][j]) {
                board[currentY + i][currentX + j] = currentPiece[i][j];
            }
        }
    }
}

// Clear completed lines
void clearLines() {
    for (int i = HEIGHT - 1; i >= 0; --i) {
        bool full = true;
        for (int j = 0; j < WIDTH; ++j) {
            if (!board[i][j]) {
                full = false;
                break;
            }
        }
        if (full) {
            for (int k = i; k > 0; --k)
               memcpy(board[k], board[k - 1], sizeof(board[k]));
            memset(board[0], 0, sizeof(board[0]));
            ++i; // Recheck this row
        }
    }
}

// Print the game board
void printBoard() {
    system("cls"); // Clear the console
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            if (board[i][j])
                std::cout << "[]";
            else if (i >= currentY && i < currentY + 4 && j >= currentX && j < currentX + 4 && currentPiece[i - currentY][j - currentX])
                std::cout << "[]";
            else
                std::cout << " .";
        }
        std::cout << "\n";
    }
    std::cout << "Controls: A/D to move, W to rotate, S to drop\n";
}

// Game logic
void gameLoop() {
    srand(time(0));
    while (true) {
        // Spawn a new piece if needed
        if (currentY == 0 && isCollision(0, 0)) {
            std::cout << "Game Over!\n";
            break;
        }
        int pieceIndex = rand() % 7;
        copyPiece(tetrominoes[pieceIndex]);
        currentX = WIDTH / 2 - 2;
        currentY = 0;

        while (true) {
            printBoard();

            // Handle input
            if (_kbhit()) {
                char ch = _getch();
                if (ch == 'a' && !isCollision(-1, 0)) currentX--;
                if (ch == 'd' && !isCollision(1, 0)) currentX++;
                if (ch == 's' && !isCollision(0, 1)) currentY++;
                if (ch == 'w') {
                    rotatePiece();
                    if (isCollision(0, 0)) rotatePiece(); // Undo if invalid
                }
            }

            // Move the piece down
            if (!isCollision(0, 1)) {
                currentY++;
            } else {
                mergePiece();
                clearLines();
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Slow down the loop
        }
    }
}
int main() {
    gameLoop();
    return 0;
}
