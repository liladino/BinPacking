#ifndef TEST_H
#define TEST_H

#include "Shelf2D.h"
#include "ShelfPacker.h"
#include "Item.h"
#include <iostream>


void printItems(const std::vector<Item>& items);
bool overlap2D(const Item& a, const Item& b) ;
bool insideBounds2D(const Item& item, const Vec3& bin);
void assertPosition(const Item& item, size_t x, size_t y);
template<typename T>
void assertEQ(T exp, T act);

struct ShelfTester{ 
    bool validatePacking(Shelf2D::Shelf2DPacker& packer, const Vec3& bin) {
        auto& items = packer.placed;

        // 1. Bounds check
        for (const auto& it : items) {
            if (!insideBounds2D(it, bin)) {
                std::cout << "Out of bounds!\n";
                return false;
            }
        }

        // 2. Overlap check
        for (size_t i = 0; i < items.size(); i++) {
            for (size_t j = i + 1; j < items.size(); j++) {
                if (overlap2D(items[i], items[j])) {
                    std::cout << "Overlap detected!\n";
                    return false;
                }
            }
        }

        return true;
    }

    // void randomTest() {
    //     Shelf2D::Shelf2DPacker packer({100, 100, 100});

    //     std::vector<Item> items;

    //     for (int i = 0; i < 100; i++) {
    //         Item it(rand()%20 + 1, rand()%20 + 1, 1);

    //         if (packer.pack(it)) {
    //             items.push_back(it);
    //         }
    //     }

    //     if (!validatePacking(packer, {100,100,100})) {
    //         cout << "Random test failed!\n";
    //     } else {
    //         cout << "Random test OK\n";
    //     }
    // }
    
    void testExample1() {
        Shelf2D::Shelf2DPacker packer({10, 10, 10});

        Item a(1,1,1);
        Item b(3,3,1);
        Item c(1,2,1);

        packer.pack(a);
        assertPosition(a, 0, 0);

        packer.pack(b);
        assertPosition(b, 1, 0);

        packer.pack(c);
        assertPosition(c, 0, 1);

		assertEQ((size_t)1, packer.getLayerHeight());

        if (!validatePacking(packer, {10,10,10})) {
            std::cout << "Validation failed!\n";
        }
		else {
			std::cout << "Valid packing.\n";
		}
    }
};



#endif