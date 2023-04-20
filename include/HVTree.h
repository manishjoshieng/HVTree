#pragma once

#include<deque>
#include<set>
#include<iostream>
using namespace std;
struct Location{
    double _x;
    double _y;
    Location(double x=0, double y=0):_x(x),_y(y){}
    friend ostream& operator << (ostream& os, const Location& r){
         os << "Location: {"<<r._x<<" "<<r._y<<"}\n";
        return os;
    }
};

struct Rect{
    double _xMin;
    double _xMax;
    double _yMin;
    double _yMax;

    Rect(double xMin = 0, 
         double yMin = 0, 
         double xMax = 0, 
         double yMax = 0);

    bool operator == (const Rect& other);

    friend ostream& operator << (ostream& os, const Rect& r){
         os << "Box: {"<<r._xMin<<" "<<r._yMin<<" "<<r._xMax<<" "<<r._yMax<<"}\n";
        return os;
    }

    bool isContaine(const Location& loc);

    bool isWithin(const Rect& box);

    static Rect BoundingBox(const deque<Location>& locations);

    static pair<Rect, Rect> HalfBox(const Rect& box, bool verical = false);
};


class HVTree{

    public:
        //Public API
        void build(deque<Location>& points);

        deque<Location> query(Rect queryBox);
    private:

        //Internal API
        struct Node {
            Rect                _boundary;
            deque<Location>     _points;
            Node*               _leftChild;
            Node*               _rightChild;
            bool                _isLeafNode;

            Node (deque<Location>& points,
                 Rect boundary, 
                 Node* leftChild= nullptr,
                 Node* rightChild=nullptr
                 ):
                _points(points),
                _boundary(boundary),
                _leftChild(leftChild),
                _rightChild(rightChild),
                _isLeafNode(false)
            {}
        };

        Node* buildInternal(deque<Location>& points, Rect box, bool vertical=false);

        deque<Location> queryInternal(Rect queryBox, Node* root);

        //Data
        Node* _root;
};