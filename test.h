#ifndef TEST_H
#define TEST_H

#include "Shelf2D.h"
#include "ShelfPacker.h"
#include "Item.h"
#include <iostream>


void printItems(const std::vector<Item>& items);
bool overlap2D(const Item& a, const Item& b) ;
bool insideBounds2D(const Item& item, const Vec3& bin);
void assert2DPosition(const Item& item, size_t x, size_t y);
void assert3DPosition(const Item& item, size_t x, size_t y, size_t z);
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
        std::cout << "\nTest 1\n";
        Shelf2D::Shelf2DPacker packer({10, 10, 10});

        Item a(1,1,1);
        Item b(3,3,1);
        Item c(1,2,1);

        packer.pack(a);
        assert2DPosition(a, 0, 0);

        packer.pack(b);
        assert2DPosition(b, 1, 0);

        packer.pack(c);
        assert2DPosition(c, 0, 1);

		assertEQ((size_t)1, packer.getLayerHeight());

        if (!validatePacking(packer, {10,10,10})) {
            std::cout << "Validation failed!\n";
        }
		else {
			std::cout << "Valid packing.\n";
		}
    }

    void testExample2() {
        std::cout << "\nTest 2\n";
        ShelfPacker packer;
        packer.setLimits(5, 4, 4);
        std::vector<std::pair<Shelf2D::Shelf2DPacker, size_t>>& packers2d = packer.packers2d;

        std::vector<Item> items = 
            {Item(1,1,1), Item(3,2,1), Item(1,1,1), Item(1,5,1), Item(2,2,2), Item(2,2,1), Item(2,2,1)};
        std::vector<Vec3> itemPositions = 
            {{0,0,0},     {1,0,0},     {0,1,0},     {0,2,0},     {0,0,1},     {2,0,1},     {0,2,1}           };
        std::vector<size_t> numOf2dPackers = 
            {1,           1,           1,           1,           2,           2,           2           };
        
        for (size_t i = 0; i < items.size(); i++) {
            Item& it = items[i];
            packer.pack(it);
            assert3DPosition(it, itemPositions[i][0], itemPositions[i][1], itemPositions[i][2]);
            assertEQ(numOf2dPackers[i], packers2d.size());
        }
        
        //layer start z
		assertEQ((size_t)0, packers2d[0].second);
		assertEQ((size_t)1, packers2d[1].second);
		//layer height z
        assertEQ((size_t)1, packers2d[0].first.getLayerHeight());
		assertEQ((size_t)2, packers2d[1].first.getLayerHeight());

        for (auto& [p, s] : packers2d){
            if (!validatePacking(p, packer.binSize)) {
                std::cout << "Validation failed!\n";
            }
            else {
                std::cout << "Valid packing.\n";
            }
        }
    }
};



#endif