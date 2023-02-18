#include <string>
#include <vector>

// 卡牌数据结构
struct Card {
    int color; // 0 black 1 red 2 green 3 blue 4 yellow
    int type;  // 0 ~ 9, 10 flip, 11 pass, 12 add, 13 change color.
    Card(int c, int t) : color(c), type(t) {}
    std::string to_string() const {
        std::string str = "{\"color\":\"";
        switch (color) {
        case 0: str += "black"; break;
        case 1: str += "red"; break;
        case 2: str += "green"; break;
        case 3: str += "blue"; break;
        case 4: str += "yellow"; break;
        default: str += "error";
        }
        str += "\",\"type\":\"";
        switch (type) {
        case 0: str += "0"; break;
        case 1: str += "1"; break;
        case 2: str += "2"; break;
        case 3: str += "3"; break;
        case 4: str += "4"; break;
        case 5: str += "5"; break;
        case 6: str += "6"; break;
        case 7: str += "7"; break;
        case 8: str += "8"; break;
        case 9: str += "9"; break;
        case 10: str += "flip"; break;
        case 11: str += "pass"; break;
        case 12: str += "add"; break;
        case 13: str += "change"; break;
        default: str += "error";
        }
        return str + "\"}";
    }
};
