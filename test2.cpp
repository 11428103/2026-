#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cmath>     
#include <string>    
#include <algorithm> 
#include <random>    
#include <windows.h> 
#include "rlutil.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

// ==================== 1. 設定與資料結構 ====================
unsigned char seg_code[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
float orbitAngle = 0.0f;

struct BannerLine {
    int x, y, color;
    string text;
};

struct Particle {
    float x, y, vx, vy;
    char symbol;
    Particle(float _x, float _y, float _vx, float _vy, char _s) 
        : x(_x), y(_y), vx(_vx), vy(_vy), symbol(_s) {}
};

// 【核心修正】動態保護區：涵蓋下移後的文字 (y: 6~26) 與提示字 (y: 30~34)
bool isProtected(int x, int y) {
    if (x >= 5 && x <= 78 && y >= 6 && y <= 26) return true;
    if (x >= 15 && x <= 65 && y >= 30 && y <= 35) return true;
    return false;
}

// ==================== 2. 功能函式定義 ====================

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

void revealBannerJumping() {
    rlutil::cls();
    vector<BannerLine> lines;
    int startY = 7; 
    string happy[] = {"H   H  AAAAA  PPPPP  PPPPP  Y   Y", "H   H  A   A  P   P  P   P   Y Y ", "HHHHH  AAAAA  PPPPP  PPPPP    Y  ", "H   H  A   A  P      P        Y  ", "H   H  A   A  P      P        Y  "};
    for(int i=0; i<5; i++) lines.push_back({10, startY + i, rlutil::LIGHTRED, happy[i]});
    string newYear[] = {"N   N  EEEEE  W   W    Y   Y  EEEEE  AAAAA  RRRR ", "NN  N  E      W   W     Y Y   E      A   A  R   R", "N N N  EEEE   W W W      Y    EEEE   AAAAA  RRRR ", "N  NN  E      WW WW      Y    E      A   A  R R  ", "N   N  EEEEE  W   W      Y    EEEEE  A   A  R  R "};
    for(int i=0; i<5; i++) lines.push_back({10, startY + 6 + i, rlutil::YELLOW, newYear[i]});
    string y2026[] = {"  22222    00000    22222    66666  ", "      2    0   0        2    6      ", "  22222    0   0    22222    66666  ", "  2        0   0    2        6   6  ", "  22222    00000    22222    66666  "};
    for(int i=0; i<5; i++) lines.push_back({20, startY + 13 + i, rlutil::LIGHTCYAN, y2026[i]});

    // 解決編譯警告：使用 std::shuffle
    random_device rd; mt19937 g(rd()); shuffle(lines.begin(), lines.end(), g);
    for(auto &l : lines) {
        rlutil::setColor(l.color); rlutil::locate(l.x, l.y); cout << l.text;
        rlutil::msleep(80);
    }
}

void updateBannerColor(int colorIdx) {
    int colors[] = {rlutil::LIGHTRED, rlutil::YELLOW, rlutil::LIGHTGREEN, rlutil::LIGHTCYAN, rlutil::LIGHTMAGENTA, rlutil::WHITE};
    int startY = 7; 
    rlutil::setColor(colors[colorIdx % 6]);
    rlutil::locate(10, startY); cout << "H   H  AAAAA  PPPPP  PPPPP  Y   Y";
    rlutil::locate(10, startY+1); cout << "H   H  A   A  P   P  P   P   Y Y ";
    rlutil::locate(10, startY+2); cout << "HHHHH  AAAAA  PPPPP  PPPPP    Y  ";
    rlutil::locate(10, startY+3); cout << "H   H  A   A  P      P        Y  ";
    rlutil::locate(10, startY+4); cout << "H   H  A   A  P      P        Y  ";
    // ... (其他部分以此類推更新 startY) ...
    rlutil::setColor(colors[(colorIdx + 2) % 6]);
    rlutil::locate(10, startY+6); cout << "N   N  EEEEE  W   W    Y   Y  EEEEE  AAAAA  RRRR ";
    rlutil::locate(10, startY+10);cout << "N   N  EEEEE  W   W      Y    EEEEE  A   A  R  R ";
    rlutil::setColor(colors[(colorIdx + 4) % 6]);
    rlutil::locate(20, startY+13); cout << "  22222    00000    22222    66666  ";
    rlutil::locate(20, startY+17); cout << "  22222    00000    22222    66666  ";
}

void shootOrbitFirework() {
    float cx = 44.0f, cy = 16.0f; // 旋轉中心點同步下移
    int sc[] = {15, 14, 11, 10, 12, 13}; 
    vector<vector<Particle>> clusters;
    int fx = (int)(cx + cos(orbitAngle) * 35.0f), fy = (int)(cy + sin(orbitAngle) * 10.0f);

    for (int s = 0; s < 2; s++) {
        vector<Particle> sub;
        for (int i = 0; i < 20; i++) {
            float ang = i * 18.0f * (M_PI / 180.0f);
            sub.push_back(Particle((float)fx, (float)fy, cos(ang)*1.2f, sin(ang)*0.6f, '*'));
        }
        clusters.push_back(sub);
    }

    // 動畫迴圈 (快速閃爍並消失)
    for (int t = 0; t < 4; t++) {
        rlutil::setColor(sc[rand() % 6]);
        for (auto &sub : clusters) {
            for (auto &p : sub) {
                int ix = (int)p.x, iy = (int)p.y;
                if (ix > 0 && ix < 80 && iy > 0 && iy < 40 && !isProtected(ix, iy)) {
                    rlutil::locate(ix, iy); cout << p.symbol;
                }
                p.x += p.vx; p.y += p.vy;
            }
        }
        rlutil::msleep(40);
        for (auto &sub : clusters) {
            for (auto &p : sub) {
                int ix = (int)p.x - (int)p.vx, iy = (int)p.y - (int)p.vy;
                if (ix > 0 && ix < 80 && iy > 0 && iy < 40 && !isProtected(ix, iy)) {
                    rlutil::locate(ix, iy); cout << " ";
                }
            }
        }
    }
    orbitAngle += 0.5f;
}

// ==================== 3. 主程式 ====================
int main() {
    srand(time(0));
    rlutil::cls();
    rlutil::hidecursor();

    for (int i = 5; i >= 0; i--) {
        rlutil::cls(); 
        rlutil::setColor(rlutil::WHITE);
        rlutil::locate(25, 9); cout << "READY TO 2026...";
        draw7Segment(35, 12, i, (rand() % 14) + 1);
        Beep(1000, 50);
        rlutil::msleep(900);
    }

    revealBannerJumping();

    while (!kbhit()) { 
        shootOrbitFirework();
        updateBannerColor(rand());
        rlutil::locate(28, 33); // 提示字下移至 33 行
        rlutil::setColor(rlutil::DARKGREY);
        cout << "[ Press any key to Exit ]";
    }

    // 滿足需求 5：清空畫面、字體白色
    rlutil::cls();
    rlutil::showcursor();
    rlutil::setColor(rlutil::WHITE);
    rlutil::locate(1, 1);
    cout << "Happy New Year 2026! 專題圓滿結束。" << endl;
    return 0;
}