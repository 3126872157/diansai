#ifndef __E_CHESS_H__
#define __E_CHESS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define EMPTY 0
#define BLACK 1
#define WHITE -1

extern int result[3] = {0, 0, 0}; // 第一为表示状态：0计算中 1下棋指令 2游戏结束
                            //第二位表示目标位置：1~9。                                                 游戏结束情况下：1黑胜2白胜3平局4错误
                            //第三位表示起始位置：0从黑棋盘抓取;1~9复位被移动的棋子的地方;10从白棋盘抓取      错误的情况下：1白棋先下
extern int centers_pos[9][2];      // 中心位置数组存在这里
extern int now_board[9]= {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}; //当前棋盘状态,串口传入数据更新到这里
int step_color = BLACK;    // 记录当前步棋的颜色
int lastBoard[9] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
int board[9]; //辅助计算棋盘，1为电脑方，-1为玩家方，0为空格


void detectAndRestorePiece();
int is_Win();
int eval();
int MinMaxSearch(int *idx, int step, int a, int b);
void renew_step_color();
void renew_result();

//只用调用这个函数，其他是内部的
int next_step();      //获得下一步棋位置，之后可以读取数组next_step_pos


#ifdef __cplusplus
}
#endif

#endif /* __E_CHESS_H__*/