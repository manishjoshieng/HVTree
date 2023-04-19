
#include<deque>
#include<set>
#include<algorithm>

#include "../include/HVTree.h"

using namespace std;


Rect::Rect(double xMin, double yMin , double xMax, double yMax):
    _xMin(xMin),
    _xMax(xMax),
    _yMin(yMin),
    _yMax(yMax)
{}

 bool 
 Rect::operator == (const Rect& other){
    if (_xMin == other._xMin &&
        _xMax == other._xMax &&
        _yMin == other._yMin &&
        _yMax == other._yMax  ) {
            return true;
    }
    return false;
 }


bool 
Rect::isContaine(const Location& loc){
    if (loc._x >= _xMin &&
        loc._x <= _xMax &&
        loc._y >= _yMin &&
        loc._y <= _yMax) {
            return true;
    }
    return false;
}

bool 
Rect::isWithin(const Rect& box) {
    if (this->isContaine(Location(box._xMin,box._yMin)) && 
        this->isContaine(Location(box._xMax,box._yMax))){
        return true;
    }
    return false;
}

Rect
Rect::BoundingBox(const deque<Location>& locations) { 
    set<double> xCoords;
    set<double> yCoords;

    std::for_each(locations.begin(), locations.end(),[&](const Location& loc){
        xCoords.emplace(loc._x);
        yCoords.emplace(loc._y);
    });

    return Rect(*xCoords.begin(),
                *yCoords.begin(),
                *xCoords.rbegin(),
                *yCoords.rbegin());
}


pair<Rect, Rect> 
Rect::HalfBox(const Rect& box, bool verical){
    if(verical) {
        Rect firstHalf(box._xMin,box._yMin,box._xMax/2.0 , box._yMax);
        Rect secondHalf(box._xMax/2.0 , box._yMin, box._xMax , box._yMax);
        return std::pair<Rect,Rect>(firstHalf,secondHalf);
    } else {
        Rect firstHalf(box._xMin,box._yMin,box._xMax, box._yMax/2.0);
        Rect secondHalf(box._xMin , box._yMax/2.0, box._xMax , box._yMax);
        return std::pair<Rect,Rect>(firstHalf,secondHalf);
    }
}

void 
HVTree::build(deque<Location>& points){
    Rect box = Rect::BoundingBox(points);
    _root = buildInternal(points, box);
}

deque<Location> 
HVTree::query(Rect queryBox){
    deque<Location> result = queryInternal(queryBox, _root);
    return result;
}

HVTree::Node*
HVTree::buildInternal(deque<Location>& points, Rect box, bool vertical) {
    if (points.empty()) return nullptr;
    Node* node = new Node(points, box);
    auto size = points.size();
    if (size==1) {
        node->_isLeafNode = true;
        return node;
    }
    deque<Location> firstHalfPoints, secondHalfPoints;
    std::pair<Rect,Rect> boxPair;

    if(vertical){
        boxPair = Rect::HalfBox(node->_boundary,true);
    }else{
        boxPair = Rect::HalfBox(node->_boundary,false);
    }
    
    std::for_each(points.begin(),points.end(),[&](Location loc){
        if(boxPair.first.isContaine(loc)){
            firstHalfPoints.push_back(loc);
        }else {
            secondHalfPoints.push_back(loc);
        }
    });
    
    node->_leftChild = buildInternal(firstHalfPoints,boxPair.first, !vertical);
    node->_rightChild = buildInternal(secondHalfPoints,boxPair.second, !vertical);
        
    return node;
}

deque<Location> 
HVTree::queryInternal(Rect queryBox, Node* root){
    deque<Location> result;
    if(root==nullptr) return result;

    if(root->_isLeafNode){
        if(queryBox.isContaine(root->_points.front())){
            result.push_back(root->_points.front());
            return result;
        }
    } else {
        auto lChild = root->_leftChild;
        auto rChild = root->_rightChild;

        if(root->_boundary == queryBox){
            //We are lucky here :)
            return root->_points;
        } else if (lChild && lChild->_boundary.isWithin(queryBox)){
            return queryInternal(queryBox,root->_leftChild);
        } else if(rChild && rChild->_boundary.isWithin(queryBox)) {
            return queryInternal(queryBox,root->_rightChild);
        } else if (lChild && rChild) {
            if(lChild->_boundary.isContaine(Location(queryBox._xMin,queryBox._yMin))&&
               lChild->_boundary.isContaine(Location(queryBox._xMax,queryBox._yMin))){
                auto result1 = queryInternal(Rect(queryBox._xMin,
                                                  queryBox._yMin,
                                                  queryBox._xMax,
                                                  lChild->_boundary._yMax),
                                            lChild);
                auto result2 = queryInternal(Rect(queryBox._xMin,
                                                  lChild->_boundary._yMax,
                                                  queryBox._xMax,
                                                  queryBox._yMax),
                                            rChild);
                result1.insert(result1.end(),result2.begin(),result2.end());
                return result1;
            } else {
                auto result1 = queryInternal(Rect(queryBox._xMin,
                                                  queryBox._yMin,
                                                  lChild->_boundary._xMax,
                                                  queryBox._yMax),
                                            lChild);
                auto result2 = queryInternal(Rect(lChild->_boundary._xMax,
                                                  queryBox._yMin,
                                                  queryBox._xMax,
                                                  queryBox._yMax),
                                            rChild);
                result1.insert(result1.end(),result2.begin(),result2.end());
                return result1;
            }
        } else if(lChild || rChild){
            auto child = lChild? lChild : rChild;
            if(child->_boundary.isWithin(queryBox)){
                return queryInternal(queryBox,child);
            } else if(child->_boundary.isContaine(Location(queryBox._xMin,queryBox._yMin))){
                return queryInternal(Rect(
                    queryBox._xMin,
                    queryBox._yMin,
                    queryBox._xMax < child->_boundary._xMax ? queryBox._xMax : child->_boundary._xMax,
                    queryBox._yMax < child->_boundary._yMax ? queryBox._yMax : child->_boundary._yMax
                ),child);
            }
        }
    }
    return result;
}