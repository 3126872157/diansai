#include <stdio.h>


#define EMPTY 0
#define BLACK 1
#define WHITE -1


int result[3] = {0, 0, 0}; // ��һΪ��ʾ״̬��0������1�º���2�°���3��λ���ƶ�������4�������ڶ�λ��ʾĿ��λ�ã�1~9������λ��ʾ��ʼλ�ã�0��������ץȡ1~9��λ���ƶ������ӵĵط�
int step_color = BLACK;    // ��¼��ǰ�������ɫ
int lastBoard[9] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
int board[9]; //�����������̣�1Ϊ���Է���-1Ϊ��ҷ���0Ϊ�ո�
int now_board[9]= {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}; //��ǰ����״̬

// ��Ⲣ�ָ����ƶ�������
void detectAndRestorePiece(void) {
    for (int i = 0; i < 9; i++) {
        if (lastBoard[i] != EMPTY && now_board[i] == EMPTY) {
            for (int j = 0; j < 9; j++) {
                if (now_board[j] == lastBoard[i] && lastBoard[j] == EMPTY) {
                    printf("�������ӱ��ƶ�����λ�� %d �ƶ���λ�� %d\n", i, j);
                    break;
                }
            }
        }
    }
}

// ����������Ƿ��л�ʤ
int is_Win() {
    for (int i = 0; i < 3; i++) {
        if (board[i] != 0 && board[i] == board[i + 3] && board[i] == board[i + 6]) // ��
            return (board[i] == WHITE ? 0 : 1);
        if (board[3 * i] != 0 && board[3 * i] == board[3 * i + 1] && board[3 * i] == board[3 * i + 2]) // ��
            return (board[3 * i] == WHITE ? 0 : 1);
    }

    if ((board[0] == WHITE && board[4] == WHITE && board[8] == WHITE) || (board[2] == WHITE && board[4] == WHITE && board[6] == WHITE))
        return 0;
    if ((board[0] == BLACK && board[4] == BLACK && board[8] == BLACK) || (board[2] == BLACK && board[4] == BLACK && board[6] == BLACK))
        return 1;

    return -1;
}

// ��ֵ����
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
    if (is_Win() >= 0) return eval(); // ����Ƿ���һ����ʤ
    if (step & 1) a = -100;
    else b = 100;
    int positions[9];
    int num_positions = 0;
    // �ҳ����п������λ��
    for (int i = 0; i < 9; i++) {
            if (board[i] == 0) {
                positions[num_positions++] = i;
            }
    }

    if (num_positions == 0) {
        return eval(); 
    }// ƽ�ֵ����

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
    printf("\n����״̬:\n");
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
        // ��ʼ������
        for (int i = 0; i < 9; i++) {
            now_board[i] = 0;
        }
        printf("����������,0�������,1�������\n");
        scanf("%d", &player);
        int i;
        for (i = 0; i < 9; i++) {
            if (player == 1) {
                renew_step_color();
                for (int i = 0; i < 9; i++) {
                    board[i] = now_board[i] * step_color;
                }
                int x = -1;
                MinMaxSearch(&x, 1, -100, 100); // �ҵ������߷�
                if (x == -1) {
                    break;
                }
                board[x] = BLACK; // ��¼�߷�
                for (int i = 0; i < 9; i++) {
                    now_board[i] = board[i] * step_color;
                }
            printBoard();
            } else {
                printBoard();
                renew_step_color();
                printf("��������Ҫ�µ�λ��,���̴����Ͻǿ�ʼ���Ϊ1~9:\n");
                int x;
                scanf("%d", &x);
                x--;
                while (x < 0 || x >= 9 || now_board[x] != 0) {
                    printf("��λ�ò����£�����ѡһ��\n");
                    scanf("%d", &x);
                    x--;
                }
                now_board[x] = step_color;
            }
            player = !player;
            int Winer = is_Win();
            if (Winer >= 0) {
                printBoard();
                if (Winer==1) printf("����Ӯ��\n");
                else printf("���Ӯ��\n");
                break;
            }
            
        }
        if (i == 9) {
            printBoard();
            printf("ƽ��\n");
        }
        printf("�Ƿ����,1�������,0��������\n");
        int op;
        scanf("%d", &op);
        if (!op) break;
    }
    return 0;
}