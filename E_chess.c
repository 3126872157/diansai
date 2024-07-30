#include "E_chess.h"

// 检测并恢复被移动的棋子
void detectAndRestorePiece() {
    for (int i = 0; i < 9; i++) {
        if (lastBoard[i] != EMPTY && now_board[i] == EMPTY) {
            for (int j = 0; j < 9; j++) {
                if (now_board[j] == lastBoard[i] && lastBoard[j] == EMPTY) {
                    result[0] = 1;
                    result[1] = i;
                    result[2] = j;
                    break;
                }
            }
        }
    }
}

// 检查棋盘上是否有获胜
int is_Win() {
    for (int i = 0; i < 3; i++) {
        if (board[i] != 0 && board[i] == board[i + 3] && board[i] == board[i + 6]) // 行
            return (board[i] == WHITE ? 0 : 1);
        if (board[3 * i] != 0 && board[3 * i] == board[3 * i + 1] && board[3 * i] == board[3 * i + 2]) // 列
            return (board[3 * i] == WHITE ? 0 : 1);
    }

    if ((board[0] == WHITE && board[4] == WHITE && board[8] == WHITE) || (board[2] == WHITE && board[4] == WHITE && board[6] == WHITE))
        return 0;
    if ((board[0] == BLACK && board[4] == BLACK && board[8] == BLACK) || (board[2] == BLACK && board[4] == BLACK && board[6] == BLACK))
        return 1;

    return -1;
}

// 估值函数
int eval() {
    int res = 0;
    for (int i = 0; i < 9; i++)
        if (board[i] == EMPTY)
            res++;
    int flag = is_Win();
    if (flag == 1)
        return (res + 1);
    if (flag == 0)
        return -(res + 1);
    return 0;
}

int MinMaxSearch(int *idx, int step, int a, int b) {
    if (is_Win() >= 0) return eval(); // 检查是否有一方获胜
    if (step & 1) a = -100;
    else b = 100;
    int positions[9];
    int num_positions = 0;
    // 找出所有可下棋的位置
    for (int i = 0; i < 9; i++) {
            if (board[i] == 0) {
                positions[num_positions++] = i;
            }
    }

    if (num_positions == 0) {
        return eval(); 
    }// 平局的情况

    for (int i = 0; i < num_positions; i++) {
        int x = positions[i];
        int t = x;
        board[x] = (step & 1) ? 1 : -1;
        int Sonval = MinMaxSearch(&x, step + 1, a, b);
        board[t] = 0;

        if (step & 1) {
            if (a < Sonval) {
                a = Sonval;
                if (step == 1) *idx = positions[i];
                if (a >= b) break;
            }
        } else {
            if (b > Sonval) {
                b = Sonval;
                if (a >= b) break;
            }
        }
    }
    if (step & 1) return a;
    else return b;
}

void renew_step_color() {
    int cnt = 0;
    for (int i = 0; i < 9; i++) {
        if (now_board[i] == BLACK) cnt++;
        if (now_board[i] == WHITE) cnt--;
    }
    if (cnt > 0) {
        step_color = WHITE;
    } else if (cnt == 0) {
        step_color = BLACK;
    } else {
        result[0] = 2;
        result[1] = 4;
        result[2] = 1;
        return;
    }
}

void next_step(){//更新result
    result[0] = 0;//开始计算
    detectAndRestorePiece();
    if (result[0] != 0) {
        return;
    }
    renew_step_color();
    if (result[0] != 0) {
        return;
    }
    for (int i = 0; i < 9; i++) {
        board[i] = now_board[i] * step_color;
    }
    int Winer = is_Win();
    if (Winer >= 0) {
        if ((Winer==1&&step_color==BLACK)||(Winer==0&&step_color==WHITE)){
            result[0] = 2;
            result[1] = 1;
            result[2] = 0;
            return;
        }
        else if((Winer==0&&step_color==BLACK)||(Winer==1&&step_color==WHITE)){
            result[0] = 2;
            result[1] = 2;
            result[2] = 0;
            return;
        }
    }
    int x = -1;
    MinMaxSearch(&x, 1, -100, 100); // 找到最优走法
    if (x == -1) {//平局
        result[0] = 2;
        result[1] = 3;
        result[2] = 0;
        return;
    }
    result[0] = 1;
    result[1] = x;
    result[2] = 5*(-1*step_color+1);//更新数据
}

