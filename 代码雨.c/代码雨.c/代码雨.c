#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include <string.h>

// ����̨�������ã�����Ϊ��ͨ�õĴ�С�����ⳬ����Ļ��
#define SCREEN_WIDTH 120   //����Ļ����ʾ�Ŀ�ȣ���������ʾ��һ�б�ʾ1��
#define SCREEN_HEIGHT 30   //����Ļ����ʾ�ĸ߶ȣ���������ʾ��һ�д���1��
#define MIN_SPEED 1               //��������С�ٶ�
#define MAX_SPEED 3             //����������ٶ�
#define MIN_LENGTH 5         //��������С����
#define MAX_LENGTH 15      //��������󳤶�

// �ַ������Խṹ��
//��Ϊ��������������������ģ��������ǽ��˽ṹ��������Ϊ�е�Ӣ��
typedef struct {
    int yPos;
    int speed;
    int length;
} Column;

// ȫ�ֱ���
Column* columns = NULL;   // �ַ������飨��ʼ��ΪNULL������Ұָ�룩
HANDLE hConsole;          // ����̨�����HANDLE��һ���������ͣ����ڱ�ʾһ������ľ��
//����������Ϊ�ǲ���ϵͳ������ʾ�͹��������ļ������ڡ�����̨�Ǹ�����Դ��һ����ֵ

// ����ת�ַ�������ȫ�汾��
void intToString(int num, char* buffer, size_t bufferSize) {
    // ��ȷָ����������С���������
    sprintf_s(buffer, bufferSize, "%d", num);
}

// ���ù��λ�ã����ӱ߽��飩
void setCursorPos(int x, int y) {
    // ȷ����������Ļ��Χ��
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return;
    }

    //COORD��һ���ṹ�壬��Ҫ���ڱ�ʾ����̨�е�����λ�ã�ͨ�������̨�е����롢������ʹ��
    //COORD�ṹ�����������Ա����������������ά����
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hConsole, coord);
}

// ���ع��
void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo = { 1, FALSE };
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

// �����ı���ɫ��RGBֵ��
void setColor(int r, int g, int b) {
    // �ø���ȫ�ĸ�ʽ����������ַ����ض�
    printf("\x1B[38;2;%d;%d;%dm", r, g, b);
}

// ��������ַ���ȷ��������ȫ��
char getRandomChar() {
    const char chars[] = "0123456789abcdefABCDEF";
    size_t charCount = sizeof(chars) - 1;  // ������Ч�ַ���
    return chars[rand() % charCount];      // ȷ��������Խ��
}

// ����������
void resetColumn(Column* column) {
    if (column == NULL)
        return;                     // ����ָ��
    column->yPos = rand() % (SCREEN_HEIGHT * 2) - SCREEN_HEIGHT;
    column->speed = MIN_SPEED + rand() % (MAX_SPEED - MIN_SPEED + 1);
    column->length = MIN_LENGTH + rand() % (MAX_LENGTH - MIN_LENGTH + 1);
}

// ��ʼ�������У������ڴ�����飩
void initColumns() {
    // �ͷž��ڴ棨������ڣ�
    if (columns != NULL) {
        free(columns);
    }
    // ��̬�����ڴ沢����Ƿ�ɹ�
    columns = (Column*)malloc(SCREEN_WIDTH * sizeof(Column));
    if (columns == NULL)
    {
        printf("�ڴ����ʧ�ܣ�");
        exit(1);  // ����ʧ�����˳�
    }
    // ��ʼ��ÿһ��
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        resetColumn(&columns[i]);
    }
}

// ����һ֡�������޸�Խ�����⣩
void drawFrame() {
    if (columns == NULL)
        return;                     // ����ָ��

    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
        // �����������Ч��
        if (x < 0 || x >= SCREEN_WIDTH)
        {
            continue;
        }

        Column* col = &columns[x];  // ��ָ��򻯷���
        for (int j = 0; j < col->length; j++)
        {
            int y = col->yPos - j;
            // ֻ������Ļ�ڵ��ַ�
            if (y < 0 || y >= SCREEN_HEIGHT)
            {
                continue;
            }

            setCursorPos(x, y);

            // ��ɫ���ã�ͷ����ɫ��β��������ɫ
            if (j == 0)
            {
                setColor(255, 255, 255);  // ͷ������
            }
            else
            {
                int green = 255 - (j * 15);
                if (green < 50)
                    green = 50;
                setColor(0, green, 0);
            }

            putchar(getRandomChar());
        }

        // ��������λ��
        col->yPos += col->speed;

        // ������Ļ������
        if (col->yPos > SCREEN_HEIGHT + col->length)
        {
            resetColumn(col);
        }
    }
}

// ��ʼ������̨���޸����ڴ�С���ã�
void initConsole() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE)
    {
        printf("��ȡ����̨���ʧ�ܣ�");
        exit(1);
    }
    hideCursor();

    // ���ô��ڴ�С�����㹻��Ļ���������ƴ�Ӵ���
    char widthStr[30], heightStr[30], modeCmd[150];
    intToString(SCREEN_WIDTH, widthStr, sizeof(widthStr));
    intToString(SCREEN_HEIGHT, heightStr, sizeof(heightStr));
    // ȷ����������
    sprintf_s(modeCmd, sizeof(modeCmd), "mode con cols=%s lines=%s", widthStr, heightStr);
    system(modeCmd);

    // ���������ն�ģʽ�����Ӵ����飩
    DWORD mode;
    if (GetConsoleMode(hConsole, &mode))
    {
        SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    else
    {
        // ��֧�������ն�ģʽʱ�û�����ɫ
        system("color 02");  // ��ѡ�������ڵ�����
    }
}

// ������
int main() {
    srand((unsigned int)time(NULL));  // ��ʼ�������

    // ��ʼ������̨����
    initConsole();
    initColumns();

    // ��ѭ����ESC�˳�
    while (1) {
        if (_kbhit()) {
            int key = _getch();
            if (key == 27) {  // ESC��
                break;
            }
        }

        drawFrame();
        Sleep(40);  // �����ٶ�
    }

    // ������Դ
    if (columns != NULL) {
        free(columns);  // �ͷ��ڴ�
        columns = NULL;
    }
    // �ָ�����̨Ĭ������
    setColor(255, 255, 255);
    printf("\x1B[0m");  // ��������
    setCursorPos(0, SCREEN_HEIGHT);
    system("color 07");  // �ָ�Ĭ����ɫ

    return 0;
}
