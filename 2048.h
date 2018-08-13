#ifndef __GAME_2048__
#define __GAME_2048__

#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

/* 每一格的宽和高 */
#define WIDTH 6
#define HEIGHT 4
// #define WIDTH 4
// #define HEIGHT 2

/* 游戏中随机出现数字2和4, 定义数字2出现概率百分比 */
#define PERCENT_2   90

/* 游戏按键控制上下左右, 重新开始和退出 */
#define GAME_KEY_LEFT       'a'
#define GAME_KEY_RIGHT      'd'
#define GAME_KEY_UP         'w'
#define GAME_KEY_DOWN       's'
#define GAME_KEY_RESTART    'r'
#define GAME_KEY_EXIT       'q'

/* 游戏中的一些提示文字 */
#define GAME_TEXT_TITLE     " --- 2048 ---"
#define GAME_TEXT_SCORE     " score: %6d"
#define GAME_TEXT_TOPSCORE  " --- Top 5 ---"
#define GAME_TEXT_NOTMOVE   " !cannot move this way!"
#define GAME_TEXT_OVER      " GAME OVER!"
#define GAME_TEXT_RESTART   " 'r': Restart"
#define GAME_TEXT_EXIT      " 'q': Quit"

#define GAME_SCORE_FILE     "game_2048_records.dat"
#define GAME_SCORE_COUNT    5

void board(WINDOW *win, int starty, int startx, int lines, int cols,
           int tile_width, int tile_height);
void check_gameover();
int move_left_right(int direction);
int move_up_down(int direction);
void generate_num(int count);
void refresh_game();
void load_scores();
void save_scores();
void update_top_score();
void init_game();
void start_game();

#endif /* __GAME_2048__ */
