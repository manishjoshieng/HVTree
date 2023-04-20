#include "include/HVTree.h"
#include<iostream>


int main() {
    std::cout<<"Hello World from HVTree!!\n";

    std::deque<Location> points = {
        {0,0},
        {0,5},
        {1,5},
        {3,2},
        {5,7},
        {9,8},
        {3,8}
    };

    HVTree tree;
    tree.build(points);
    auto result = tree.query(Rect(0,0,3,3));
    for(auto p : result){
        std::cout<<"{" + std::to_string(p._x) +" "+std::to_string(p._y) + "}\n";
    }
}
