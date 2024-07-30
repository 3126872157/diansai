#include <stdio.h>


#define EMPTY 0
#define BLACK 1
#define WHITE -1


int result[3] = {0, 0, 0}; // 第一为表示状态：0计算中1下黑棋2下白棋3复位被移动的棋子4结束。第二位表示目标位置：1~9。第三位表示起始位置：0从棋盘外抓取1~9复位被移动的棋子的地方
int step_color = BLACK;    // 记录当前步棋的颜色
int lastBoard[9] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
int board[9]; //辅助计算棋盘，1为电脑方，-1为玩家方，0为空格
int now_board[9]= {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}; //当前棋盘状态

// 检测并恢复被移动的棋子
void detectAndRestorePiece(void) {
    for (int i = 0; i < 9; i++) {
        if (lastBoard[i] != EMPTY && now_board[i] == EMPTY) {
            for (int j = 0; j < 9; j++) {
                if (now_board[j] == lastBoard[i] && lastBoard[j] == EMPTY) {
                    printf("发现棋子被移动：从位置 %d 移动到位置 %d\n", i, j);
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
void printBoard() {
    printf("\n棋盘状态:\n");
    for (int i = 0; i < 9; i++) {
        printf("%c ", now_board[i] == WHITE ? 'X' : (now_board[i] == BLACK ? '0' : '_'));
        if (i % 3 == 2)
            printf("\n");
    }
}

void renew_step_color() {
    int cnt = 0;
    for (int i = 0; i < 9; i++) {
        if (now_board[i] == BLACK) cnt++;
        if (now_board[i] == WHITE) cnt--;
    }

    if (cnt > 0) {
        step_color = WHITE;
        printf("white\n");
    } else if (cnt == 0) {
        step_color = BLACK;
        printf("black\n");
    } else {
        printf("error\n");
    }
}
int main() {
    int player = 0;
    while (1) {
        // 初始化棋盘
        for (int i = 0; i < 9; i++) {
            now_board[i] = 0;
        }
        printf("请输入先手,0代表玩家,1代表电脑\n");
        scanf("%d", &player);
        int i;
        for (i = 0; i < 9; i++) {
            if (player == 1) {
                renew_step_color();
                for (int i = 0; i < 9; i++) {
                    board[i] = now_board[i] * step_color;
                }
                int x = -1;
                MinMaxSearch(&x, 1, -100, 100); // 找到最优走法
                if (x == -1) {
                    break;
                }
                board[x] = BLACK; // 记录走法
                for (int i = 0; i < 9; i++) {
                    now_board[i] = board[i] * step_color;
                }
            printBoard();
            } else {
                printBoard();
                renew_step_color();
                printf("请输入需要下的位置,棋盘从左上角开始编号为1~9:\n");
                int x;
                scanf("%d", &x);
                x--;
                while (x < 0 || x >= 9 || now_board[x] != 0) {
                    printf("该位置不能下，请另选一个\n");
                    scanf("%d", &x);
                    x--;
                }
                now_board[x] = step_color;
            }
            player = !player;
            int Winer = is_Win();
            if (Winer >= 0) {
                printBoard();
                if (Winer==1) printf("电脑赢了\n");
                else printf("玩家赢了\n");
                break;
            }
            
        }
        if (i == 9) {
            printBoard();
            printf("平局\n");
        }
        printf("是否继续,1代表继续,0代表不玩了\n");
        int op;
        scanf("%d", &op);
        if (!op) break;
    }
    return 0;
}