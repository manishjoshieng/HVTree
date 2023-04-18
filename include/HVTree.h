#pragma once

#include<deque>
#include<set>
using namespace std;
struct Location{
    int _x;
    int _y;
    Location(int x=0, int y=0):_x(x),_y(y){}
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
                 Node* leftChild= nullptr,
                 Node* rightChild=nullptr
                 ):
                _boundary(),
                _points(points),
                _leftChild(leftChild),
                _rightChild(rightChild),
                _isLeafNode(false)
            {
                _boundary = Rect::BoundingBox(_points);
            }
        };

        Node* buildInternal(deque<Location>& points, Rect box, bool vertical=false);

        deque<Location> queryInternal(Rect queryBox, Node* root);

        //Data
        Node* _root;
};