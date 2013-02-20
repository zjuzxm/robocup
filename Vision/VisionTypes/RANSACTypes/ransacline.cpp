#include "ransacline.h"

RANSACLine::RANSACLine()
{

}

bool RANSACLine::regenerate(const vector<Point>& pts)
{
    if(pts.size() == 2) {
        setLineFromPoints(pts.at(0).ground, pts.at(1).ground);
        return true;
    }
    else {
        return false;
    }
}

double RANSACLine::calculateError(Point p) const
{
    return getLinePointDistance(p);
}
