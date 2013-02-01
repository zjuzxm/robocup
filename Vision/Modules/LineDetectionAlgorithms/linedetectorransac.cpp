#include "linedetectorransac.h"
#include "Vision/visionblackboard.h"
#include "Vision/visionconstants.h"
#include "Vision/GenericAlgorithms/ransac.h"
#include "Vision/VisionTypes/visionline.h"

#include <limits>
#include <stdlib.h>
#include <boost/foreach.hpp>

//plotting
#include <QApplication>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

LineDetectorRANSAC::LineDetectorRANSAC()
{
    m_n = 15;               //min pts to line essentially
    m_k = 40;               //number of iterations per fitting attempt
    m_e = 4.0;              //consensus margin
    m_max_iterations = 10;  //hard limit on number of lines
}

vector<LSFittedLine> LineDetectorRANSAC::run()
{
    VisionBlackboard* vbb = VisionBlackboard::getInstance();

    vector<ColourSegment> v_segments = vbb->getVerticalTransitions(LINE_COLOUR);  //get transitions associated with lines
    vector<ColourSegment> h_segments = vbb->getHorizontalTransitions(LINE_COLOUR);
    vector< pair<VisionLine, vector<Point> > > candidates;
    vector<LSFittedLine> lines;
    vector<Point> points;

    points = getPointsFromSegments(h_segments, v_segments);

    points = pointsUnderGreenHorizon(points, vbb->getGreenHorizon());

    //debugging
    QApplication a(NULL);
    QwtPlot pre(QwtText("Pre"));
    pre.setGeometry(0,0,320,240);
    pre.setAxisScale(QwtPlot::xBottom, 0.0,320);
    pre.setAxisScale(QwtPlot::yLeft,0,240);

    QwtPlotCurve precurve("Pre");
    std::vector<double> xs;
    std::vector<double> ys;
    BOOST_FOREACH(Point& p, points)
    {
        xs.push_back(p.x);
        ys.push_back(p.y);
    }
    QwtPointArrayData * const predata = new QwtPointArrayData(&xs[0],&ys[0],xs.size());
    precurve.setData(predata);
    precurve.attach(&pre);

    pre.show();

    //end debugging

    candidates = RANSAC::findMultipleModels<VisionLine, Point>(points, m_e, m_n, m_k, m_max_iterations);
    for(unsigned int i=0; i<candidates.size(); i++) {
        lines.push_back(LSFittedLine(candidates.at(i).second));
    }
//    o << "lines" << endl;
//    o << lines << endl;
//    o << "colinear" << endl;
//    o << mergeColinear(lines, VisionConstants::RANSAC_MAX_ANGLE_DIFF_TO_MERGE, VisionConstants::RANSAC_MAX_DISTANCE_TO_MERGE) << endl;

    //debugging
    if(vbb->getTransformer().isScreenToGroundValid())
        points = vbb->getTransformer().screenToGroundCartesian(points);

    xs.clear();
    ys.clear();
    double minx = std::numeric_limits<double>::max(),
           maxx = std::numeric_limits<double>::min(),
           miny = std::numeric_limits<double>::max(),
           maxy = std::numeric_limits<double>::min();
    BOOST_FOREACH(Point& p, points)
    {
        minx = min(minx, p.x);
        maxx = max(maxx, p.x);
        miny = min(miny, p.y);
        maxy = max(maxy, p.y);
        xs.push_back(p.x);
        ys.push_back(p.y);
    }

    QwtPlot post(QwtText("Post"));
    post.setGeometry(minx,miny,maxx,maxy);
    post.setAxisScale(QwtPlot::xBottom, minx,maxx);
    post.setAxisScale(QwtPlot::yLeft,miny,maxy);

    QwtPlotCurve postcurve("Post");

    QwtPointArrayData * const postdata = new QwtPointArrayData(&xs[0],&ys[0],xs.size());
    postcurve.setData(postdata);
    postcurve.attach(&post);

    post.show();
    a.exec();
    //end debugging

//    //use generic ransac implementation to fine lines
//    candidates = RANSAC::findMultipleModels<VisionLine, Point>(points, m_e, m_n, m_k, m_max_iterations);
//    lines.clear();
//    for(unsigned int i=0; i<candidates.size(); i++) {
//        lines.push_back(LSFittedLine(candidates.at(i).second));
//    }

//    o << "lines" << endl;
//    o << lines << endl;
//    o << "colinear" << mergeColinear(lines, VisionConstants::RANSAC_MAX_ANGLE_DIFF_TO_MERGE, VisionConstants::RANSAC_MAX_DISTANCE_TO_MERGE) << endl;

//    o.close();

    return mergeColinear(lines, VisionConstants::RANSAC_MAX_ANGLE_DIFF_TO_MERGE, VisionConstants::RANSAC_MAX_DISTANCE_TO_MERGE);
}