/** @file visionfieldobject.h
*   @class VisionFieldObject
*   @author Shannon Fenn: shannon.fenn@uon.edu.au
*   @brief Abstract parent class for internal representation of field objects.
*/

#ifndef VISIONFIELDOBJECT_H
#define VISIONFIELDOBJECT_H

#include "Vision/basicvisiontypes.h"

#include "Vision/VisionTypes/Interfaces/publishable.h"
#include "Vision/VisionTypes/Interfaces/optimisable.h"

#include "Infrastructure/NUBlackboard.h"
#include "Tools/Math/Vector3.h"
#include "Tools/Math/Vector2.h"

#include <vector>

using namespace std;
using namespace Vision;

class VisionFieldObject : public Publishable, public Optimisable
{
public:
    VisionFieldObject();
    virtual ~VisionFieldObject() {}

    VFO_ID getID() const {return m_id;}
    string getName() const {return getVFOName(m_id);}

    const Point& getLocation() const {return m_location;}
    //! @brief returns the screen location in pixels (relative to the top left).
    Vector2<double> getLocationPixels() const;
    //! @brief returns the angular screen location (relative to the camera) in radians.
    Vector2<double> getLocationAngular() const;
    //! @brief returns the screen size in pixels.
    Vector2<double> getScreenSize() const { return m_size_on_screen; }
    //! @brief returns the field position relative to the robot.
    virtual Vector3<double> getRelativeFieldCoords() const {return m_location.relativeRadial;}

protected:
    Point m_location;                       //! @variable The location of the object (includes screen, radial and ground position).
    Vector2<double> m_size_on_screen;          //! @variable The width and height on screen in pixels.

    VFO_ID m_id;
    float m_confidence;   //! unused
    float m_error;        //! unused
    Vector3<double> m_spherical_error;       //! @variable The error in each of the spherical dimensions.
    bool valid;                             //! @variable Whether the object is valid.
    bool distance_valid;                    //! @variable Whether the distance is valid.
};

#endif // VISIONFIELDOBJECT_H
