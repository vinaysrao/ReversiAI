#include "coordinate.h"
#include "reversicommon.h"

using namespace reversi;

Coordinate::Coordinate() {
    x = -1;
    y = -1;
    m_empty = true;
}

Coordinate::Coordinate(int x, int y) : x(x), y(y) {
    m_empty = false;
}

bool Coordinate::empty() {
    return m_empty;
}

string Coordinate::toString() {
    return ReversiCommon::formatMove(ReversiCommon::Move(x, y));
}

bool Coordinate::operator==(const Coordinate &c) const {
    return c.x == x && c.y == y;
}

bool Coordinate::operator<(const Coordinate& c) const {
    if (x < c.x)  return true;
    if (x > c.x)  return false;

    // If we are here, x are equal
    if (y < c.y)  return true;
    if (y > c.y)  return false;

    // If we are here, y are equal too
    return false;
}


vector< Coordinate > Coordinate::between(Coordinate &c) {
    vector<Coordinate> between;

    // Horizontally
    if(x == c.x){
        int minY = min(y, c.y) + 1;
        int maxY = max(y, c.y);
        while(minY < maxY){
            between.push_back(Coordinate(x, minY));
            minY++;
        }
    }
    // Vertically
    else if(y == c.y){
        int minX = min(x, c.x) + 1;
        int maxX = max(x, c.x);
        while(minX < maxX){
            between.push_back(Coordinate(minX, y));
            minX++;
        }
    }
    // Diagonal up right
    else if(y < c.y && x < c.x){
        int xC = x + 1;
        int yC = y + 1;
        while(yC < c.y){
            between.push_back(Coordinate(xC, yC));
            yC++;
            xC++;
        }
    }
    // Diagonal down right
    else if(y > c.y && x < c.x){
        int xC = x + 1;
        int yC = y - 1;
        while(yC > c.y){
            between.push_back(Coordinate(xC, yC));
            yC--;
            xC++;
        }
    }
    // Diagonal up left
    else if(y < c.y && x > c.x){
        int xC = x - 1;
        int yC = y + 1;
        while(yC < c.y){
            between.push_back(Coordinate(xC, yC));
            yC++;
            xC--;
        }
    }
    // Diagonal down left
    else if(y > c.y && x > c.x){
        int xC = x - 1;
        int yC = y - 1;
        while(yC > c.y){
            between.push_back(Coordinate(xC, yC));
            yC--;
            xC--;
        } 
    }
    
    return between;
}
