#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cmath>     
#include <string>    // 支援字串處理
#include <windows.h> // 支援 Beep 音效
#include "rlutil.h"

using namespace std;

// ==================== 1. 設定與資料結構 ====================
unsigned char seg_code[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};

struct Particle {
    float x, y;
    float vx, vy;
    char symbol;
    Particle(float _x, float _y, float _vx, float _vy, char _s) 
        : x(_x), y(_y), vx(_vx), vy(_vy), symbol(_s) {}
};

// 【核心修正】保護區域：確保煙火不會塗掉 Banner (3-22行) 以及 底部提示字 (26行)
bool isProtected(int x, int y) {
    // 保護大字報區
    if (x >= 5 && x <= 78 && y >= 2 && y <= 22) return true;
    // 保護底部提示字區 (第 26 行周邊)
    if (x >= 20 && x <= 60 && y >= 25 && y <= 27) return true;
    return false;
}

// ==================== 2. 功能函式定義 ====================

// 繪製七段顯示器 (倒數用)
void draw7Segment(int x, int y, int num, int color) {
    unsigned char code = seg_code[num];
    for (int i = 0; i < 5; i++) { rlutil::locate(x, y + i); cout << "      "; }
    rlutil::setColor(color);
    if (code & 0x01) { rlutil::locate(x + 1, y); cout << "==="; }
    if (code & 0x20) { rlutil::locate(x, y + 1); cout << "||"; }
    if (code & 0x02) { rlutil::locate(x + 4, y + 1); cout << "||"; }
    if (code & 0x40) { rlutil::locate(x + 1, y + 2); cout << "==="; }
    if (code & 0x10) { rlutil::locate(x, y + 3); cout << "||"; }
    if (code & 0x04) { rlutil::locate(x + 4, y + 3); cout << "||"; }
    if (code & 0x08) { rlutil::locate(x + 1, y + 4); cout << "==="; }
}

// 【新功能】逐行顯現 Banner (出現後即持久不消失)
void revealBannerGradually() {
    rlutil::cls();
    int startY = 3;
    
    // 第一部分: HAPPY
    rlutil::setColor(rlutil::LIGHTRED);
    string happy[] = {
        "H   H  AAAAA  PPPPP  PPPPP  Y   Y",
        "H   H  A   A  P   P  P   P   Y Y ",
        "HHHHH  AAAAA  PPPPP  PPPPP    Y  ",
        "H   H  A   A  P      P        Y  ",
        "H   H  A   A  P      P        Y  "
    };
    for(int i=0; i<5; i++) {
        rlutil::locate(10, startY + i); cout << happy[i];
        rlutil::msleep(150);
    }

    // 第二部分: NEW YEAR
    rlutil::setColor(rlutil::YELLOW);
    string newYear[] = {
        "N   N  EEEEE  W   W    Y   Y  EEEEE  AAAAA  RRRR ",
        "NN  N  E      W   W     Y Y   E      A   A  R   R",
        "N N N  EEEE   W W W      Y    EEEE   AAAAA  RRRR ",
        "N  NN  E      WW WW      Y    E      A   A  R R  ",
        "N   N  EEEEE  W   W      Y    EEEEE  A   A  R  R "
    };
    for(int i=0; i<5; i++) {
        rlutil::locate(10, startY + 6 + i); cout << newYear[i];
        rlutil::msleep(150);
    }

    // 第三部分: 2026
    rlutil::setColor(rlutil::LIGHTCYAN);
    string y2026[] = {
        "  22222    00000    22222    66666  ",
        "      2    0   0        2    6      ",
        "  22222    0   0    22222    66666  ",
        "  2        0   0    2        6   6  ",
        "  22222    00000    22222    66666  "
    };
    for(int i=0; i<5; i++) {
        rlutil::locate(20, startY + 13 + i); cout << y2026[i];
        rlutil::msleep(150);
    }
}

// 快速變色函式 (不清除畫面，只覆蓋顏色)
void updateBannerColor(int colorIdx) {
    int colors[] = {rlutil::LIGHTRED, rlutil::YELLOW, rlutil::LIGHTGREEN, rlutil::LIGHTCYAN, rlutil::LIGHTMAGENTA, rlutil::WHITE};
    int startY = 3;
    
    rlutil::setColor(colors[colorIdx % 6]);
    rlutil::locate(10, startY);     cout << "H   H  AAAAA  PPPPP  PPPPP  Y   Y";
    rlutil::locate(10, startY + 1); cout << "H   H  A   A  P   P  P   P   Y Y ";
    rlutil::locate(10, startY + 2); cout << "HHHHH  AAAAA  PPPPP  PPPPP    Y  ";
    rlutil::locate(10, startY + 3); cout << "H   H  A   A  P      P        Y  ";
    rlutil::locate(10, startY + 4); cout << "H   H  A   A  P      P        Y  ";

    rlutil::setColor(colors[(colorIdx + 2) % 6]);
    rlutil::locate(10, startY + 6); cout << "N   N  EEEEE  W   W    Y   Y  EEEEE  AAAAA  RRRR ";
    rlutil::locate(10, startY + 7); cout << "NN  N  E      W   W     Y Y   E      A   A  R   R";
    rlutil::locate(10, startY + 8); cout << "N N N  EEEE   W W W      Y    EEEE   AAAAA  RRRR ";
    rlutil::locate(10, startY + 9); cout << "N  NN  E      WW WW      Y    E      A   A  R R  ";
    rlutil::locate(10, startY + 10);cout << "N   N  EEEEE  W   W      Y    EEEEE  A   A  R  R ";

    rlutil::setColor(colors[(colorIdx + 4) % 6]);
    rlutil::locate(20, startY + 13); cout << "  22222    00000    22222    66666  ";
    rlutil::locate(20, startY + 14); cout << "      2    0   0        2    6      ";
    rlutil::locate(20, startY + 15); cout << "  22222    0   0    22222    66666  ";
    rlutil::locate(20, startY + 16); cout << "  2        0   0    2        6   6  ";
    rlutil::locate(20, startY + 17); cout << "  22222    00000    22222    66666  ";
}

// 密集群聚煙火 (多中心同時爆炸)
void shootFireworkCluster() {
    int groupX = (rand() % 60) + 10;
    int groupY = (rand() % 15) + 4;
    int sparkColors[] = {15, 14, 11, 10, 12, 13}; 
    vector<vector<Particle>> clusters;

    int subCount = (rand() % 2) + 3; // 產生 3~4 個密集的中心
    for (int s = 0; s < subCount; s++) {
        vector<Particle> sub;
        int cx = groupX + (rand() % 9 - 4);
        int cy = groupY + (rand() % 5 - 2);
        for (int i = 0; i < 40; i++) {
            float angle = i * (9.0f) * (3.14159f / 180.0f);
            float p = (rand() % 5 / 5.0f) + 1.2f; 
            sub.push_back(Particle((float)cx, (float)cy, cos(angle) * 4.0f * p, sin(angle) * 2.0f * p, '*'));
        }
        clusters.push_back(sub);
    }

    for (int t = 0; t < 12; t++) {
        for (auto &sub : clusters) {
            rlutil::setColor(sparkColors[rand() % 6]);
            for (auto &p : sub) {
                int ix = (int)p.x, iy = (int)p.y;
                if (ix > 0 && ix < 80 && iy > 0 && iy < 25 && !isProtected(ix, iy)) {
                    rlutil::locate(ix, iy); cout << p.symbol;
                }
                p.x += p.vx;
                p.y += p.vy + (t * 0.15f);
                if (t > 4) p.symbol = '+';
                if (t > 8) p.symbol = '.';
            }
        }
        rlutil::msleep(40); 
        for (auto &sub : clusters) {
            for (auto &p : sub) {
                int ix = (int)p.x - (int)p.vx, iy = (int)p.y - (int)(p.vy + (t * 0.15f));
                if (ix > 0 && ix < 80 && iy > 0 && iy < 25 && !isProtected(ix, iy)) {
                    rlutil::locate(ix, iy); cout << " ";
                }
            }
        }
    }
}

// ==================== 3. 主程式 ====================
int main() {
    srand(time(0));
    rlutil::cls();
    rlutil::hidecursor();

    // 階段一：倒數
    for (int i = 9; i >= 0; i--) {
        rlutil::cls(); 
        rlutil::setColor(rlutil::WHITE);
        rlutil::locate(25, 5); cout << "COUNTDOWN TO 2026...";
        draw7Segment(35, 8, i, (rand() % 14) + 1);
        if (i > 0) Beep(850, 80); else Beep(1600, 450);
        rlutil::msleep(900);
    }

    // 階段二：逐行顯現 (完成後即持久顯示)
    revealBannerGradually();

    // 階段三：主迴圈 (快速變色與密集群聚煙火)
    int frame = 0;
    while (!kbhit()) {
        // 每 2 幀變換顏色，達成霓虹燈效果
        if (frame % 2 == 0) {
            updateBannerColor(frame / 2);
        }
        
        shootFireworkCluster();
        frame++;
        
        // 【修正點】將退出提示移到第 26 行
        rlutil::locate(28, 26); 
        rlutil::setColor(rlutil::DARKGREY);
        cout << "[ Press any key to Exit ]";
    }

    // 結束善後
    rlutil::cls();
    rlutil::showcursor();
    rlutil::setColor(rlutil::WHITE);
    rlutil::locate(1, 1);
    cout << "Happy New Year 2026!" << endl;
    return 0;
}