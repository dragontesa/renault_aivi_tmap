/* ***************************************************************************************
* FILE:          Geometry.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  Geometry.h is part of HMI-Base framework Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#ifndef APPUTILS_GEOMETRY_H
#define APPUTILS_GEOMETRY_H

namespace hmibase {
namespace util {
namespace geometry {
struct Size;
}


}
}


namespace hmibase {
namespace util {
namespace geometry {

typedef int Coord;

struct Point
{
   Coord x;
   Coord y;

   inline Point() :
      x(0),
      y(0)
   {
   }

   inline Point(Coord param_x, Coord param_y) :
      x(param_x),
      y(param_y)
   {
   }

   // TODO nodo: add comment
   inline Point& reset()
   {
      x = 0;
      y = 0;
      return *this;
   }

   inline Point operator+(const Point& pt) const
   {
      return Point(x + pt.x,
                   y + pt.y);
   }

   inline Point operator-(const Point& pt) const
   {
      return Point(x - pt.x,
                   y - pt.y);
   }

   inline Point& operator+=(const Point& pt)
   {
      x += pt.x;
      y += pt.y;
      return *this;
   }

   inline Point& operator-=(const Point& pt)
   {
      x -= pt.x;
      y -= pt.y;
      return *this;
   }

   inline bool operator==(const Point& pt) const
   {
      return (x == pt.x) && (y == pt.y);
   }

   inline bool operator!=(const Point& pt) const
   {
      return (x != pt.x) || (y != pt.y);
   }
};


/**
 *  @brief Rect struct
 *
 *  Struct for a rectangle defined by two points
 */
struct Rect
{
   Point ptMin;
   Point ptMax;

   inline Rect() :
      ptMin(0, 0),
      ptMax(-1, -1)
   {
   }

   inline Rect(const Point& pt1, const Point& pt2) :
      ptMin(pt1),
      ptMax(pt2)
   {
   }

   inline Rect(Coord x1, Coord y1, Coord x2, Coord y2) :
      ptMin(x1, y1),
      ptMax(x2, y2)
   {
   }

   /**
    *  @brief Constructor for Rect object.
    *
    *  @param[in]  pt1         left top point of the object
    *  @param[in]  rSize       Size of the created object
    */
   Rect(const Point& pt1, const Size& rSize);

   inline Rect operator+(const Point& pt) const
   {
      return Rect(ptMin.x + pt.x,
                  ptMin.y + pt.y,
                  ptMax.x + pt.x,
                  ptMax.y + pt.y);
   }

   inline Rect operator-(const Point& pt) const
   {
      return Rect(ptMin.x - pt.x,
                  ptMin.y - pt.y,
                  ptMax.x - pt.x,
                  ptMax.y - pt.y);
   }

   inline Rect& operator+=(const Point& pt)
   {
      ptMin += pt;
      ptMax += pt;
      return *this;
   }

   inline Rect& operator-=(const Point& pt)
   {
      ptMin -= pt;
      ptMax -= pt;
      return *this;
   }

   inline bool isEmpty() const
   {
      return ptMin.x > ptMax.x ||
             ptMin.y > ptMax.y;
   }

   inline bool contains(const Point& pt) const
   {
      // guaranteed to return false if the rectangle is empty
      return pt.x >= ptMin.x && pt.x <= ptMax.x &&
             pt.y >= ptMin.y && pt.y <= ptMax.y;
   }

   inline bool contains(const Rect& rc) const
   {
      return contains(rc.ptMin) && contains(rc.ptMax);
   }

   bool intersects(const Rect& rc) const;

   void normalize();

   void setBoundingRect(const Rect& rc1, const Rect& rc2);

   void setIntersection(const Rect& rc1, const Rect& rc2);

   inline Coord getWidth() const
   {
      return (ptMax.x - ptMin.x) + 1;
   }

   inline Coord getHeight() const
   {
      return (ptMax.y - ptMin.y) + 1;
   }

   // TODO nodo: add comment
   inline Point getCenter() const
   {
      return Point(ptMin.x + getWidth() / 2, ptMin.y + getHeight() / 2) ;
   }

   /**
    *  @brief extract size of Rect object
    *
    *  @return     returns the size of the Rect object as type Size
    */
   Size getSize() const;

   Coord getArea() const;

   inline int getSimpleArea() const
   {
      return getWidth() * getHeight();
   }

   inline void reset()
   {
      ptMin.x = 0;
      ptMin.y = 0;
      ptMax.x = -1;
      ptMax.y = -1;
   }
};


struct Size
{
   /// width of the Size object
   Coord width;

   /// height of the Size object
   Coord height;

   /**
    *  @brief Default Constructor for Size object.
    *
    *  The size of the constructed object will be 0.
    */
   inline Size() :
      width(0),
      height(0)
   {
   }

   /**
    *  @brief Constructor for Size object.
    *
    *  @param[in]  nWidth      width of the created object
    *  @param[in]  nHeight     height of the created object
    */
   inline Size(const Coord nWidth, const Coord nHeight) :
      width(nWidth),
      height(nHeight)
   {
   }

   /**
    *  @brief Constructor for Size object.
    *
    *  @param[in]  rcRect  Rect object, whose width and height are taken
    */
   inline explicit Size(const Rect& rcRect) :
      width(rcRect.getWidth()),
      height(rcRect.getHeight())
   {
   }

   /**
    *  @brief The equal operator.
    *
    *  @param[in]  rSize       Size to compare with
    *
    *  @return     true, if the components (width, height) are equal
    *              false else
    */
   inline bool operator==(const Size& rSize) const
   {
      return (width == rSize.width) && (height == rSize.height);
   }

   /**
    *  @brief The not equal operator.
    *
    *  @param[in]  rSize       Size to compare with
    *
    *  @return     true, if one of the components (width, height) is different
    *              false else
    */
   inline bool operator!=(const Size& rSize) const
   {
      return (width != rSize.width) || (height != rSize.height);
   }

   /**
    *  @brief The assignment operator.
    *
    *  @param[in]  rSize       The object to copy
    *
    *  @return     the copied object
    */
   inline Size& operator=(const Size& rSize)
   {
      if (this != &rSize)
      {
         width = rSize.width;
         height = rSize.height;
      }
      return *this;
   }

   /**
    *  @brief  checks if both, width and height are > 0 each
    *
    *  @return     * true, if width > 0 and height > 0
    *              * false else
    */
   inline bool isPositive() const
   {
      return (width > 0) && (height > 0);
   }

   /**
    *  @brief  returns the positive area (i.e. width * height) of the object if width and height >= 0; else: 0
    *
    *  @return     positive area (i.e. width * height) of the object
    *              * width * height, if width >= 0 and height >= 0
    *              * 0 else
    */
   inline Coord getPosArea() const
   {
      Coord result = 0;
      if (isPositive())
      {
         result = (Coord)getArea();
      }
      return result;
   }

   /**
    *  @brief  returns the area (i.e. width * height) of the object
    *
    *  @param[in]      pDrawContext            the DrawContext, by which the method can call drawing functions
    *
    *  @return         area (i.e. width * height) of the object
    */
   inline int getArea() const
   {
      return width * height;
   }

   /**
    *  @brief  set size (width, height) of the calling object to the corresponding maximum
    *
    *  The maximum will be determined for each component (width, height)
    *  between the calling object and the parameter.
    *
    *  @param[in]      rSize       size object, whose components (width, height) will be used
    *                              as input for determining the maximum
    */
   inline Size& setSizeToMax(const Size& rSize)
   {
      // set min. of width
      if (width < rSize.width)
      {
         width = rSize.width;
      }

      // set min. of height
      if (height < rSize.height)
      {
         height = rSize.height;
      }

      return *this;
   }

   /**
    *  @brief  set size (width, height) of the calling object to the corresponding minimum
    *
    *  The minimum will be determined for each component (width, height)
    *  between the calling object and the parameter.
    *
    *  @param[in]      rSize       size object, whose components (width, height) will be used
    *                              as input for determining the minimum
    */
   inline Size& setSizeToMin(const Size& rSize)
   {
      // set min. of width
      if (width > rSize.width)
      {
         width = rSize.width;
      }

      // set min. of height
      if (height > rSize.height)
      {
         height = rSize.height;
      }

      return *this;
   }

   /**
    *  @brief  sets size (width, height) of the calling object to 0
    */
   inline Size& reset()
   {
      width = 0;
      height = 0;

      return *this;
   }
};


}
}


}

#endif //APPUTILS_GEOMETRY_H
