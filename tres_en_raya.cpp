/*
Nombre del programador: Shorkka
Nombre del programa: Juego del gato con inteligencia artificial y múltiples dificultades
Descripción: Juego de Tres en Raya contra la computadora con 4 niveles de dificultad,
guardado de partidas y selección de símbolo (X/O) para el jugador, manteniendo que X siempre inicia primero.
*/

// SO soportado
#ifdef _WIN32
#define CLEAR "cls"
#include <windows.h>
#elif defined(unix)||defined(__unix__)||defined(__unix)||defined(__APPLE__)||defined(__MACH__)
#define CLEAR "clear"
#endif

// Librerias
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <cstdio>
#include <cctype>
#include <locale>

using namespace std;

// Constantes
enum MenuOption { NEW_GAME = 0, LOAD_GAME = 1, STATS = 2, EXIT_GAME = 3 };
enum KeyCode { ARRIBA = 72, ABAJO = 80, IZQUIERDA = 75, DERECHA = 77, ENTER = 13, ESC = 27, SAVE = 's' };
enum Difficulty { FACIL = 0, MEDIO = 1, DIFICIL = 2, IMPOSIBLE = 3 };

// Variables globales
int wins = 0;
int losses = 0;
int draws = 0;
int moves = 0;
char currentPlayer = ' ';
int cursorX = 1, cursorY = 1;
bool gameSaved = false;
int difficulty = DIFICIL;
char playerSymbol = 'X';
char computerSymbol = 'O';
bool gameOver = false;

// Tablero de juego
char board[3][3] = {{' ', ' ', ' '},
                   {' ', ' ', ' '},
                   {' ', ' ', ' '}};

// Prototipos de funciones
void clearScreen();
void gotoxy(int x, int y);
int showMainMenu();
int selectDifficulty();
int getKeyPress();
void drawBoard(bool showHelp = true);
bool makeMove(int row, int col);
char checkWinner();
bool isBoardFull();
void saveGame();
bool loadGame();
void playGame(bool isNewGame = true);
void resetGame();
void showStats();
void showInGameMenu();
void computerMoveEasy();
void computerMoveMedium();
void computerMoveHard();
void computerMoveImpossible();
void computerMove();
bool decideFirstMove();
void showEndGameMenu();
void showGameResult(char winner);

// Función principal
int main() {
    // Configurar locale para caracteres especiales
    setlocale(LC_ALL, " "); // Para sistemas en español
    #ifdef _WIN32
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
    #endif
    loadGame();
    srand(time(NULL));

    while (true) {
        int option = showMainMenu();

        switch (option) {
            case NEW_GAME:
                difficulty = selectDifficulty();
                resetGame();
                playGame(true);
                break;
            case LOAD_GAME:
                if (loadGame()) {
                    playGame(false);
                } else {
                    cout << "No se encontró partida guardada. Presione cualquier tecla para continuar...";
                    getch();
                }
                break;
            case STATS:
                showStats();
                break;
            case EXIT_GAME:
                return 0;
            default:
                break;
        }
    }
}

// Implementación de funciones

void clearScreen() {
    system(CLEAR);
}

void gotoxy(int x, int y) {
    #ifdef _WIN32
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    #else
    printf("\033[%d;%df", y+1, x+1);
    #endif
}

int showMainMenu() {
    int selected = 0;
    int key;

    do {
        clearScreen();
        cout << "=== TRES EN RAYA ===" << endl << endl;
        cout << (selected == NEW_GAME ? "> " : "  ") << "Nuevo Juego" << endl;
        cout << (selected == LOAD_GAME ? "> " : "  ") << "Cargar Partida" << endl;
        cout << (selected == STATS ? "> " : "  ") << "Estadísticas" << endl;
        cout << (selected == EXIT_GAME ? "> " : "  ") << "Salir" << endl;

        key = getKeyPress();

        if (key == ARRIBA && selected > 0) {
            selected--;
        } else if (key == ABAJO && selected < 3) {
            selected++;
        }
    } while (key != ENTER);

    return selected;
}

int selectDifficulty() {
    int selected = 0;
    int key;
    string difficultyNames[] = {"Fácil", "Medio", "Difícil", "Imposible"};

    do {
        clearScreen();
        cout << "SELECCIONE DIFICULTAD" << endl << endl;
        for (int i = 0; i <= 3; i++) {
            cout << (selected == i ? "> " : "  ") << difficultyNames[i] << endl;
        }

        key = getKeyPress();

        if (key == ARRIBA && selected > 0) {
            selected--;
        } else if (key == ABAJO && selected < 3) {
            selected++;
        }
    } while (key != ENTER);

    return selected;
}

int getKeyPress() {
    int ch = getch();
    if (ch == 0 || ch == 224) {
        return getch();
    }
    return tolower(ch);
}

void drawBoard(bool showHelp) {
    clearScreen();
    cout << "TRES EN RAYA - Jugador: " << playerSymbol << " vs Computadora: " << computerSymbol << endl;
    cout << "Victorias: " << wins << " - Derrotas: " << losses << " - Empates: " << draws << endl << endl;

    for (int i = 0; i < 3; i++) {
        cout << " ";
        for (int j = 0; j < 3; j++) {
            cout << " " << board[i][j] << " ";
            if (j < 2) cout << "|";
        }
        cout << endl;

        if (i == cursorY && !gameOver) {
            cout << "  ";
            for (int j = 0; j < 3; j++) {
                if (j == cursorX) {
                    cout << " ↓ ";
                } else {
                    cout << "   ";
                }
                if (j < 2) cout << " ";
            }
            cout << endl;
        }

        if (i < 2) cout << "-----------" << endl;
    }

    if (showHelp && !gameOver) {
        cout << endl << "Controles:" << endl;
        cout << "Flechas: Mover cursor" << endl;
        cout << "Enter: Colocar ficha" << endl;
        cout << "S: Guardar partida" << endl;
        cout << "ESC: Menú principal" << endl;
    }
}

void showInGameMenu() {
    int selected = 0;
    int key;

    do {
        clearScreen();
        cout << "MENÚ DE PAUSA" << endl << endl;
        cout << (selected == 0 ? "> " : "  ") << "Continuar partida" << endl;
        cout << (selected == 1 ? "> " : "  ") << "Guardar partida" << endl;
        cout << (selected == 2 ? "> " : "  ") << "Salir al menú principal" << endl;

        key = getKeyPress();

        if (key == ARRIBA && selected > 0) {
            selected--;
        } else if (key == ABAJO && selected < 2) {
            selected++;
        }
    } while (key != ENTER);

    switch (selected) {
        case 0: // Continuar
            return;

        case 1: // Guardar
            saveGame();
            break;

        case 2: // Salir
            if (!gameSaved) {
                int selectedConfirm = 0;
                int keyConfirm;

                do {
                    clearScreen();
                    cout << "¿ESTÁS SEGURO? LA PARTIDA NO ESTÁ GUARDADA" << endl << endl;
                    cout << (selectedConfirm == 0 ? "> " : "  ") << "Sí, salir sin guardar" << endl;
                    cout << (selectedConfirm == 1 ? "> " : "  ") << "No, volver al menú" << endl;

                    keyConfirm = getKeyPress();

                    if (keyConfirm == ARRIBA && selectedConfirm > 0) {
                        selectedConfirm--;
                    } else if (keyConfirm == ABAJO && selectedConfirm < 1) {
                        selectedConfirm++;
                    }
                } while (keyConfirm != ENTER);

                if (selectedConfirm == 0) {
                    gameSaved = true;
                } else {
                    return;
                }
            }
            return;
    }
}

bool makeMove(int row, int col) {
    if (gameOver || row < 0 || row > 2 || col < 0 || col > 2 || board[row][col] != ' ') {
        return false;
    }

    board[row][col] = currentPlayer;
    moves++;
    gameSaved = false;
    return true;
}

char checkWinner() {
    // Verificar filas
    for (int i = 0; i < 3; i++) {
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
            return board[i][0];
        }
    }

    // Verificar columnas
    for (int j = 0; j < 3; j++) {
        if (board[0][j] != ' ' && board[0][j] == board[1][j] && board[1][j] == board[2][j]) {
            return board[0][j];
        }
    }

    // Verificar diagonales
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        return board[0][0];
    }
    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
        return board[0][2];
    }

    return ' ';
}

bool isBoardFull() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                return false;
            }
        }
    }
    return true;
}

bool decideFirstMove() {
    return (rand() % 2) == 0;
}

void computerMoveEasy() {
    int row, col;
    do {
        row = rand() % 3;
        col = rand() % 3;
    } while (board[row][col] != ' ');

    board[row][col] = computerSymbol;
}

void computerMoveMedium() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                board[i][j] = computerSymbol;
                if (checkWinner() == computerSymbol) {
                    return;
                }
                board[i][j] = ' ';
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                board[i][j] = playerSymbol;
                if (checkWinner() == playerSymbol) {
                    board[i][j] = computerSymbol;
                    return;
                }
                board[i][j] = ' ';
            }
        }
    }

    computerMoveEasy();
}

void computerMoveHard() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                board[i][j] = computerSymbol;
                if (checkWinner() == computerSymbol) {
                    return;
                }
                board[i][j] = ' ';
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                board[i][j] = playerSymbol;
                if (checkWinner() == playerSymbol) {
                    board[i][j] = computerSymbol;
                    return;
                }
                board[i][j] = ' ';
            }
        }
    }

    if (board[1][1] == ' ') {
        board[1][1] = computerSymbol;
        return;
    }

    int corners[4][2] = {{0,0}, {0,2}, {2,0}, {2,2}};
    for (int i = 0; i < 4; i++) {
        int randCorner = rand() % 4;
        if (board[corners[randCorner][0]][corners[randCorner][1]] == ' ') {
            board[corners[randCorner][0]][corners[randCorner][1]] = computerSymbol;
            return;
        }
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                board[i][j] = computerSymbol;
                return;
            }
        }
    }
}

void computerMoveImpossible() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                board[i][j] = computerSymbol;
                if (checkWinner() == computerSymbol) {
                    return;
                }
                board[i][j] = ' ';
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                board[i][j] = playerSymbol;
                if (checkWinner() == playerSymbol) {
                    board[i][j] = computerSymbol;
                    return;
                }
                board[i][j] = ' ';
            }
        }
    }

    if (board[1][1] == ' ') {
        board[1][1] = computerSymbol;
        return;
    }

    int oppositeCorners[4][2][2] = {
        {{0,0},{2,2}},
        {{0,2},{2,0}},
        {{2,2},{0,0}},
        {{2,0},{0,2}}
    };

    for (int i = 0; i < 4; i++) {
        if (board[oppositeCorners[i][0][0]][oppositeCorners[i][0][1]] == playerSymbol &&
            board[oppositeCorners[i][1][0]][oppositeCorners[i][1][1]] == ' ') {
            board[oppositeCorners[i][1][0]][oppositeCorners[i][1][1]] = computerSymbol;
            return;
        }
    }

    int corners[4][2] = {{0,0}, {0,2}, {2,0}, {2,2}};
    for (int i = 0; i < 4; i++) {
        if (board[corners[i][0]][corners[i][1]] == ' ') {
            board[corners[i][0]][corners[i][1]] = computerSymbol;
            return;
        }
    }

    int sides[4][2] = {{0,1}, {1,0}, {1,2}, {2,1}};
    for (int i = 0; i < 4; i++) {
        if (board[sides[i][0]][sides[i][1]] == ' ') {
            board[sides[i][0]][sides[i][1]] = computerSymbol;
            return;
        }
    }
}

void computerMove() {
    switch (difficulty) {
        case FACIL:
            computerMoveEasy();
            break;
        case MEDIO:
            computerMoveMedium();
            break;
        case DIFICIL:
            computerMoveHard();
            break;
        case IMPOSIBLE:
            computerMoveImpossible();
            break;
        default:
            computerMoveHard();
            break;
    }
}

void saveGame() {
    ofstream file("tres_en_raya.sav", ios::binary);
    if (file.is_open()) {
        file.write((char*)&difficulty, sizeof(difficulty));
        file.write((char*)&playerSymbol, sizeof(playerSymbol));
        file.write((char*)&computerSymbol, sizeof(computerSymbol));
        file.write((char*)&currentPlayer, sizeof(currentPlayer));
        file.write((char*)&moves, sizeof(moves));
        file.write((char*)&wins, sizeof(wins));
        file.write((char*)&losses, sizeof(losses));
        file.write((char*)&draws, sizeof(draws));
        file.write((char*)&gameOver, sizeof(gameOver));

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                file.write((char*)&board[i][j], sizeof(board[i][j]));
            }
        }

        file.close();
        gameSaved = true;

        gotoxy(0, 12);
        cout << "Partida guardada correctamente!";
        gotoxy(cursorX * 4 + 1, cursorY * 2 + 5);
    } else {
        gotoxy(0, 12);
        cout << "Error al guardar la partida!";
        gotoxy(cursorX * 4 + 1, cursorY * 2 + 5);
    }
}

bool loadGame() {
    ifstream file("tres_en_raya.sav", ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.read((char*)&difficulty, sizeof(difficulty));
    file.read((char*)&playerSymbol, sizeof(playerSymbol));
    file.read((char*)&computerSymbol, sizeof(computerSymbol));
    file.read((char*)&currentPlayer, sizeof(currentPlayer));
    file.read((char*)&moves, sizeof(moves));
    file.read((char*)&wins, sizeof(wins));
    file.read((char*)&losses, sizeof(losses));
    file.read((char*)&draws, sizeof(draws));
    file.read((char*)&gameOver, sizeof(gameOver));

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            file.read((char*)&board[i][j], sizeof(board[i][j]));
        }
    }

    file.close();
    gameSaved = true;
    return true;
}

void showGameResult(char winner) {
    gameOver = true;
    drawBoard(false);

    if (winner == playerSymbol) {
        cout << endl << "¡FELICIDADES! ¡HAS GANADO!" << endl;
        wins++;
    } else if (winner == computerSymbol) {
        cout << endl << "¡LO SIENTO! LA COMPUTADORA HA GANADO" << endl;
        losses++;
    } else {
        cout << endl << "¡EMPATE! NADIE HA GANADO" << endl;
        draws++;
    }

    // Guardar estadísticas
    ofstream statsOut("tres_en_raya_stats.sav", ios::binary);
    if (statsOut.is_open()) {
        statsOut.write((char*)&wins, sizeof(wins));
        statsOut.write((char*)&losses, sizeof(losses));
        statsOut.write((char*)&draws, sizeof(draws));
        statsOut.close();
    }

    cout << endl << "Presiona ENTER para volver al menú principal...";
    while (getKeyPress() != ENTER);
}

void playGame(bool isNewGame) {
    char winner = ' ';
    int key;

    while (true) {
        if (currentPlayer == computerSymbol && !gameOver) {
            computerMove();
            moves++;
            winner = checkWinner();
            if (winner != ' ' || isBoardFull()) {
                showGameResult(winner);
                break;
            }
            currentPlayer = playerSymbol;
        }

        if (!gameOver) {
            drawBoard();
            gotoxy(cursorX * 4 + 1, cursorY * 2 + 5);
        }

        key = getKeyPress();

        if (gameOver && key == ENTER) {
            break;
        }

        switch (key) {
            case ARRIBA:
                if (!gameOver && cursorY > 0) cursorY--;
                break;
            case ABAJO:
                if (!gameOver && cursorY < 2) cursorY++;
                break;
            case IZQUIERDA:
                if (!gameOver && cursorX > 0) cursorX--;
                break;
            case DERECHA:
                if (!gameOver && cursorX < 2) cursorX++;
                break;
            case ENTER:
                if (!gameOver && currentPlayer == playerSymbol && makeMove(cursorY, cursorX)) {
                    winner = checkWinner();
                    if (winner != ' ' || isBoardFull()) {
                        showGameResult(winner);
                        return;
                    }
                    currentPlayer = computerSymbol;
                }
                break;
            case SAVE:
                if (!gameOver) saveGame();
                break;
            case ESC:
                if (!gameOver) {
                    showInGameMenu();
                    if (gameSaved) return;
                }
                break;
            default:
                break;
        }
    }
}

void showEndGameMenu() {
    int selected = 0;
    int key;

    do {
        clearScreen();
        cout << "¿QUÉ DESEAS HACER AHORA?" << endl << endl;
        cout << (selected == 0 ? "> " : "  ") << "Jugar otra partida" << endl;
        cout << (selected == 1 ? "> " : "  ") << "Volver al menú principal" << endl;

        key = getKeyPress();

        if (key == ARRIBA && selected > 0) {
            selected--;
        } else if (key == ABAJO && selected < 1) {
            selected++;
        }
    } while (key != ENTER);

    if (selected == 0) {
        resetGame();
        playGame(true);
    }
}

void resetGame() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }
    moves = 0;
    cursorX = 1;
    cursorY = 1;
    gameOver = false;

    bool playerFirst = decideFirstMove();
    cout << "Lanzando moneda... (cara o sello)" << endl;
    Sleep(3000);
    if (playerFirst) {
        currentPlayer = 'X';
        cout << "¡Cara! Tú empiezas primero (X)." << endl;
    } else {
        currentPlayer = 'O';
        cout << "¡Sello! La computadora empieza primero (O)." << endl;
    }
    Sleep(2000);

    gameSaved = false;
}

void showStats() {
    clearScreen();
    cout << "ESTADÍSTICAS" << endl << endl;
    cout << "Victorias: " << wins << endl;
    cout << "Derrotas: " << losses << endl;
    cout << "Empates: " << draws << endl;
    cout << "Total de partidas: " << (wins + losses + draws) << endl << endl;

    string difficultyNames[] = {"Fácil", "Medio", "Difícil", "Imposible"};
    cout << "Dificultad actual: " << difficultyNames[difficulty] << endl;

    cout << endl << "Presione cualquier tecla para continuar...";
    getch();
}
