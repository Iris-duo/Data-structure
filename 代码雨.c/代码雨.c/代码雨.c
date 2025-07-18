#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include <string.h>

// 控制台窗口配置（调整为更通用的大小，避免超出屏幕）
#define SCREEN_WIDTH 120   //在屏幕上显示的宽度（用列数表示，一列表示1）
#define SCREEN_HEIGHT 30   //在屏幕上显示的高度（用行数表示，一行代表1）
#define MIN_SPEED 1               //流动的最小速度
#define MAX_SPEED 3             //流动的最大速度
#define MIN_LENGTH 5         //流动的最小长度
#define MAX_LENGTH 15      //流动的最大长度

// 字符列属性结构体
//因为这个代码雨的流向是纵向的，所以我们将此结构体重命名为列的英文
typedef struct {
    int yPos;
    int speed;
    int length;
} Column;

// 全局变量
Column* columns = NULL;   // 字符列数组（初始化为NULL，避免野指针）
HANDLE hConsole;          // 控制台句柄，HANDLE是一种数据类型，用于表示一个对象的句柄
//句柄可以理解为是操作系统用来表示和管理诸如文件、窗口、控制台登各种资源的一个数值

// 整数转字符串（安全版本）
void intToString(int num, char* buffer, size_t bufferSize) {
    // 明确指定缓冲区大小，避免溢出
    sprintf_s(buffer, bufferSize, "%d", num);
}

// 设置光标位置（增加边界检查）
void setCursorPos(int x, int y) {
    // 确保坐标在屏幕范围内
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return;
    }

    //COORD是一个结构体，主要用于表示控制台中的坐标位置，通常与控制台中的输入、输出配合使用
    //COORD结构体包含两个成员变量，用于描述二维坐标
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hConsole, coord);
}

// 隐藏光标
void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo = { 1, FALSE };
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

// 设置文本颜色（RGB值）
void setColor(int r, int g, int b) {
    // 用更安全的格式输出，避免字符串截断
    printf("\x1B[38;2;%d;%d;%dm", r, g, b);
}

// 生成随机字符（确保索引安全）
char getRandomChar() {
    const char chars[] = "0123456789abcdefABCDEF";
    size_t charCount = sizeof(chars) - 1;  // 计算有效字符数
    return chars[rand() % charCount];      // 确保索引不越界
}

// 重置列属性
void resetColumn(Column* column) {
    if (column == NULL)
        return;                     // 防空指针
    column->yPos = rand() % (SCREEN_HEIGHT * 2) - SCREEN_HEIGHT;
    column->speed = MIN_SPEED + rand() % (MAX_SPEED - MIN_SPEED + 1);
    column->length = MIN_LENGTH + rand() % (MAX_LENGTH - MIN_LENGTH + 1);
}

// 初始化所有列（增加内存分配检查）
void initColumns() {
    // 释放旧内存（如果存在）
    if (columns != NULL) {
        free(columns);
    }
    // 动态分配内存并检查是否成功
    columns = (Column*)malloc(SCREEN_WIDTH * sizeof(Column));
    if (columns == NULL)
    {
        printf("内存分配失败！");
        exit(1);  // 分配失败则退出
    }
    // 初始化每一列
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        resetColumn(&columns[i]);
    }
}

// 绘制一帧动画（修复越界问题）
void drawFrame() {
    if (columns == NULL)
        return;                     // 防空指针

    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
        // 检查列索引有效性
        if (x < 0 || x >= SCREEN_WIDTH)
        {
            continue;
        }

        Column* col = &columns[x];  // 用指针简化访问
        for (int j = 0; j < col->length; j++)
        {
            int y = col->yPos - j;
            // 只绘制屏幕内的字符
            if (y < 0 || y >= SCREEN_HEIGHT)
            {
                continue;
            }

            setCursorPos(x, y);

            // 颜色设置：头部白色，尾部渐变绿色
            if (j == 0)
            {
                setColor(255, 255, 255);  // 头部高亮
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

        // 更新下落位置
        col->yPos += col->speed;

        // 超出屏幕则重置
        if (col->yPos > SCREEN_HEIGHT + col->length)
        {
            resetColumn(col);
        }
    }
}

// 初始化控制台（修复窗口大小设置）
void initConsole() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE)
    {
        printf("获取控制台句柄失败！");
        exit(1);
    }
    hideCursor();

    // 设置窗口大小（用足够大的缓冲区避免拼接错误）
    char widthStr[30], heightStr[30], modeCmd[150];
    intToString(SCREEN_WIDTH, widthStr, sizeof(widthStr));
    intToString(SCREEN_HEIGHT, heightStr, sizeof(heightStr));
    // 确保命令完整
    sprintf_s(modeCmd, sizeof(modeCmd), "mode con cols=%s lines=%s", widthStr, heightStr);
    system(modeCmd);

    // 启用虚拟终端模式（增加错误检查）
    DWORD mode;
    if (GetConsoleMode(hConsole, &mode))
    {
        SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    else
    {
        // 不支持虚拟终端模式时用基础颜色
        system("color 02");  // 备选方案：黑底绿字
    }
}

// 主函数
int main() {
    srand((unsigned int)time(NULL));  // 初始化随机数

    // 初始化控制台和列
    initConsole();
    initColumns();

    // 主循环：ESC退出
    while (1) {
        if (_kbhit()) {
            int key = _getch();
            if (key == 27) {  // ESC键
                break;
            }
        }

        drawFrame();
        Sleep(40);  // 控制速度
    }

    // 清理资源
    if (columns != NULL) {
        free(columns);  // 释放内存
        columns = NULL;
    }
    // 恢复控制台默认设置
    setColor(255, 255, 255);
    printf("\x1B[0m");  // 重置属性
    setCursorPos(0, SCREEN_HEIGHT);
    system("color 07");  // 恢复默认颜色

    return 0;
}
