#include "2048.h"

int is_gameover;

int current_score, top_score[GAME_SCORE_COUNT];

int **game_board, num = 4;

int main(int argc, char const *argv[])
{
    init_game();
    load_scores();

    if (argc == 2) {
        num = atoi(argv[1]);
        if (num < 4)
            num = 3;                /* limit at least 3x3 */
        else if (HEIGHT * num > LINES)
            num = LINES / HEIGHT;   /* limit not to draw out screen */
    }

    start_game();

    endwin();
    return 0;
}

/* init screen and seed */
void init_game()
{
    srand(time(NULL));
    initscr();
    curs_set(0);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
}

void start_game()
{
    int ch, moved = 0;
    is_gameover = 0;
    current_score = 0;

    game_board = (int **) malloc(num * sizeof(int *));
    for (int i = 0; i < num; ++i)
        game_board[i] = (int *) calloc(num, sizeof(int));

    generate_num(2);
    refresh_game();

    while ((ch = getch()) != GAME_KEY_EXIT) {
        switch (ch) {
            case KEY_LEFT:
            case GAME_KEY_LEFT:
                moved = move_left_right(GAME_KEY_LEFT);
                break;
            case KEY_RIGHT:
            case GAME_KEY_RIGHT:
                moved = move_left_right(GAME_KEY_RIGHT);
                break;
            case KEY_UP:
            case GAME_KEY_UP:
                moved = move_up_down(GAME_KEY_UP);
                break;
            case KEY_DOWN:
            case GAME_KEY_DOWN:
                moved = move_up_down(GAME_KEY_DOWN);
                break;
            case GAME_KEY_RESTART:
                goto FREE_RESOURCE;
            default:
                break;
        }
        if (moved) {
            generate_num(1);
            check_gameover();
            refresh_game();
        } else if (!is_gameover) {
            attron(A_BLINK);
            mvprintw(0, (COLS - sizeof(GAME_TEXT_NOTMOVE)) / 2, GAME_TEXT_NOTMOVE);
            attroff(A_BLINK);
        }
    }

FREE_RESOURCE:
    for (int i = 0; i < num; ++i)
        free(game_board[i]);
    free(game_board);
    save_scores();
    if (ch == GAME_KEY_RESTART)
        start_game();
}

/* Draw grid border  */
void board(WINDOW *win, int starty, int startx, int lines, int cols,
        int tile_width, int tile_height)
{
    int endy, endx, i, j;
    endy = starty + lines * tile_height;
    endx = startx + cols * tile_width;

    for (j = starty; j <= endy; j += tile_height)
        for (i = startx; i < endx; ++i)
            mvwaddch(win, j, i, ACS_HLINE);
    for (i = startx; i <= endx; i += tile_width)
        for (j = starty; j <= endy; ++j)
            mvwaddch(win, j, i, ACS_VLINE);
    mvwaddch(win, starty, startx, ACS_ULCORNER);
    mvwaddch(win, endy, startx, ACS_LLCORNER);
    mvwaddch(win, starty, endx, ACS_URCORNER);
    mvwaddch(win, endy, endx, ACS_LRCORNER);
    for (j = starty + tile_height; j <= endy - tile_height; j += tile_height) {
        mvwaddch(win, j, startx, ACS_LTEE);
        mvwaddch(win, j, endx, ACS_RTEE);
        for (i = startx + tile_width; i <= endx - tile_width; i += tile_width)
            mvwaddch(win, j, i, ACS_PLUS);
    }
    for (i = startx + tile_width; i <= endx - tile_width; i += tile_width) {
        mvwaddch(win, starty, i, ACS_TTEE);
        mvwaddch(win, endy, i, ACS_BTEE);
    }
    wrefresh(win);
}

/* Check if you can continue the game */
void check_gameover()
{
    int x, y;
    for (x = 0; x < num; ++x) {
        for (y = 0; y < num; ++y) {
            if (game_board[x][y] == 0) return;
            if (x + 1 < num && game_board[x + 1][y] == game_board[x][y]) return;
            if (y + 1 < num && game_board[x][y + 1] == game_board[x][y]) return;
        }
    }
    update_top_score();
    is_gameover = 1;
}

/* Generate "count" random numbers(2 or 4) */
void generate_num(int count)
{
    int x, y, num_space, i, position;

    num_space = 0;

    for (x = 0; x < num; ++x)
        for (y = 0; y < num; ++y)
            if (game_board[x][y] == 0)
                num_space++;

    while (count > 0 && num_space > 0) {
        position = rand() % num_space;
        i = 0;
        for (x = 0; x < num && i <= position; ++x)
            for (y = 0; y < num && i <= position; ++y)
                if (game_board[x][y] == 0 && i++ == position)
                        game_board[x][y] = rand() % 100 < PERCENT_2 ? 2 : 4;
        count--;
        num_space--;
    }
}

int move_left_right(int direction)
{
    int x, y, k, delta, moved = 0;

    for (y = 0; y < num; ++y) {
        switch (direction) {
            case GAME_KEY_LEFT:
                k = 0;
                x = 1;
                delta = 1;
                break;
            case GAME_KEY_RIGHT:
                k = num - 1;
                x = num - 2;
                delta = -1;
                break;
        }
        while ((direction == GAME_KEY_LEFT && x < num) ||
               (direction == GAME_KEY_RIGHT && x >= 0)) {
            if (game_board[x][y] != 0) {
                if (game_board[k][y] == 0) {
                    game_board[k][y] = game_board[x][y];
                    game_board[x][y] = 0;
                    x += delta;
                    moved = 1;
                } else {
                    if (game_board[x][y] == game_board[k][y]) {
                        game_board[k][y] += game_board[x][y];
                        game_board[x][y] = 0;
                        current_score += game_board[k][y];
                        x += delta;
                        k += delta;
                        moved = 1;
                    } else {
                        k += delta;
                        if (k == x)
                            x += delta;
                    }
                }
            } else {
                x += delta;
            }
        }
    }

    return moved;
}

int move_up_down(int direction)
{
    int x, y, k, delta, moved = 0;

    for (x = 0; x < num; ++x) {
        switch (direction) {
            case GAME_KEY_UP:
                k = 0;
                y = 1;
                delta = 1;
                break;
            case GAME_KEY_DOWN:
                k = num - 1;
                y = num - 2;
                delta = -1;
                break;
        }
        while ((direction == GAME_KEY_UP && y < num) ||
               (direction == GAME_KEY_DOWN && y >= 0)) {
            if (game_board[x][y] != 0) {
                if (game_board[x][k] == 0) {
                    game_board[x][k] = game_board[x][y];
                    game_board[x][y] = 0;
                    y += delta;
                    moved = 1;
                } else {
                    if (game_board[x][y] == game_board[x][k]) {
                        game_board[x][k] += game_board[x][y];
                        game_board[x][y] = 0;
                        current_score += game_board[x][k];
                        y += delta;
                        k += delta;
                        moved = 1;
                    } else {
                        k += delta;
                        if (k == y)
                            y += delta;
                    }
                }
            } else {
                y += delta;
            }
        }
    }

    return moved;
}

/* Refresh the game interface */
void refresh_game()
{
    int i, j, deltax, deltay, offset;
    int startx, starty;

    starty = (LINES - num * HEIGHT) / 2;
    startx = (COLS - num * WIDTH) / 2;
    clear();
    board(stdscr, starty, startx, num, num, WIDTH, HEIGHT);
    deltay = HEIGHT / 2;
    deltax = WIDTH / 2;
    mvprintw(starty + num * HEIGHT - 2, startx + num * WIDTH + 1, GAME_TEXT_RESTART);
    mvprintw(starty + num * HEIGHT - 1, startx + num * WIDTH + 1, GAME_TEXT_EXIT);

    attron(A_BOLD);
    mvprintw(starty - 2, (COLS - sizeof(GAME_TEXT_TITLE)) / 2, GAME_TEXT_TITLE);
    mvprintw(starty, startx + num * WIDTH + 1, GAME_TEXT_SCORE, current_score);
    mvprintw(starty + 1, startx + num * WIDTH + 1, GAME_TEXT_TOPSCORE);
    for (i = 0; i < GAME_SCORE_COUNT; ++i) {
        mvprintw(starty + i + 2, startx + num * WIDTH + 1, "%2d : %8d", i, top_score[i]);
    }

    for (i = 0; i < num; ++i)
        for (j = 0; j < num; ++j)
            if (game_board[i][j] != 0) {
                offset = game_board[i][j] > 1000 ? 2 : game_board[i][j] > 100 ? 1 : 0;
                mvprintw(starty + j * HEIGHT + deltay,
                         startx + i * WIDTH + deltax - offset,
                         "%d", game_board[i][j]);
            }

    if (is_gameover) {
        attron(A_BLINK);
        mvprintw(starty + num * HEIGHT + 1, (COLS - sizeof(GAME_TEXT_OVER)) / 2, GAME_TEXT_OVER);
        attroff(A_BLINK);
    }

    attroff(A_BOLD);
    refresh();
}

/* Read scores from a file */
void load_scores()
{
    FILE *f = fopen(GAME_SCORE_FILE, "rb");
    if (f) {
        fread(top_score, sizeof(int), GAME_SCORE_COUNT, f);
        fclose(f);
    }
}

/* Save score to file */
void save_scores()
{
    FILE *f = fopen(GAME_SCORE_FILE, "wb");
    if (f) {
        fwrite(top_score, sizeof(int), GAME_SCORE_COUNT, f);
        fclose(f);
    }
}

/* Update current score to the top score */
void update_top_score()
{
    int pos;
    for (pos = 0; pos < GAME_SCORE_COUNT; ++pos)
        if (current_score > top_score[pos])
            break;
    if (pos < GAME_SCORE_COUNT) {
        for (int i = GAME_SCORE_COUNT - 1; i > pos; --i) {
            top_score[i] = top_score[i - 1];
        }
        top_score[pos] = current_score;
    }
}
