#ifndef COORDINATE_H
#define COORDINATE_H

#include <string>
#include <vector>

using namespace std;

class Coordinate
{
public:
    int x;
    int y;
    bool m_empty;

    Coordinate();
    Coordinate(int x, int y);

    bool empty();
    string toString();
    bool operator==(const Coordinate &c) const;
    bool operator<(const Coordinate &c) const;
    vector<Coordinate> between(Coordinate &c);
};

#endif // COORDINATE_H
