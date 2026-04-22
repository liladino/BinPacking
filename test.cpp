#include "test.h"

using namespace std;

void printItems(const std::vector<Item>& items) {
    for (const auto& it : items) {
        cout << it << "; ";
    }
}

bool overlap2D(const Item& a, const Item& b) {
    return !(a.getPos(0) + a[0] <= b.getPos(0) ||
            b.getPos(0) + b[0] <= a.getPos(0) ||
            a.getPos(1) + a[1] <= b.getPos(1) ||
            b.getPos(1) + b[1] <= a.getPos(1));
}

bool insideBounds2D(const Item& item, const Vec3& bin) {
    return item.getPos(0) + item[0] <= bin[0] &&
        item.getPos(1) + item[1] <= bin[1];
}

void assert2DPosition(const Item& item, size_t x, size_t y) {
    if (item.getPos(0) != x || item.getPos(1) != y) {
        cout << "Wrong placement! Expected (" 
                << x << "," << y << ") got ("
                << item.getPos(0) << "," 
                << item.getPos(1) << ")\n";
        return;
    }
    cout << "Assert success" << endl;
}

void assert3DPosition(const Item& item, size_t x, size_t y, size_t z) {
    if (item.getPos(0) != x || item.getPos(1) != y || item.getPos(2) != z) {
        cout << "Wrong placement! Expected (" 
                << x << ", " << y << ", " << z << ") got ("
                << item.getPos(0) << ", " 
                << item.getPos(1) << ", " 
                << item.getPos(2) << ")\n";
        return;
    }
    cout << "Assert success" << endl;
}

template<typename T>
void assertEQ(T exp, T act){
    if (exp != act){
        cout << "Fail! Expected (" << exp << ") got (" << act << ")\n";
    }
    cout << "Assert success" << endl;
}

int main(){
    ShelfTester test;
    test.testExample1();
    test.testExample2();
}