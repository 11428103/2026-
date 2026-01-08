#include <iostream>
#include "rlutil.h"  // 引入 rlutil 標頭檔

int main() {
    // 清空螢幕
    rlutil::cls();
    
    // 設定文字顏色為藍色
    rlutil::setColor(rlutil::BLUE);
    std::cout << "Hello from rlutil!" << std::endl;
    
    // 將游標移動到螢幕中央(假設螢幕是 80x25)
    rlutil::locate(40, 12);
    
    // 設定文字顏色為紅色
    rlutil::setColor(rlutil::RED);
    std::cout << "* Center Text *";
    
    // 重設顏色為預設值
    rlutil::resetColor();
    
    // 將游標移到最下方
    rlutil::locate(1, 24);
    std::cout << "Press any key to exit...";
    
    // 等待按鍵
    rlutil::anykey();
    
    return 0;
}