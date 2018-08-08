#ifndef BOOST_ALG_HPP
#define BOOST_ALG_HPP

#ifndef DISABLE_BOOST_SERIALIZE
    #include <sstream>
#endif

#ifdef __clang__
#undef _MSC_EXTENSIONS
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244)
#pragma warning(disable: 4267)
#endif
#include <boost/geometry.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
// this should be removed to not confuse the compiler
// #include <libnest2d.h>

namespace bp2d {

using libnest2d::TCoord;
using libnest2d::PointImpl;
using Coord = TCoord<PointImpl>;
using libnest2d::PolygonImpl;
using libnest2d::PathImpl;
using libnest2d::Orientation;
using libnest2d::OrientationType;
using libnest2d::getX;
using libnest2d::getY;
using libnest2d::setX;
using libnest2d::setY;
using Box = libnest2d::_Box<PointImpl>;
using Segment = libnest2d::_Segment<PointImpl>;
using Shapes = libnest2d::nfp::Shapes<PolygonImpl>;

}

/**
 * We have to make all the libnest2d geometry types available to boost. The real
 * models of the geometries remain the same if a conforming model for libnest2d
 * was defined by the library client. Boost is used only as an optional
 * implementer of some algorithms that can be implemented by the model itself
 * if a faster alternative exists.
 *
 * However, boost has its own type traits and we have to define the needed
 * specializations to be able to use boost::geometry. This can be done with the
 * already provided model.
 */
namespace boost {
namespace geometry {
namespace traits {

/* ************************************************************************** */
/* Point concept adaptaion ************************************************** */
/* ************************************************************************** */

template<> struct tag<bp2d::PointImpl> {
    using type = point_tag;
};

template<> struct coordinate_type<bp2d::PointImpl> {
    using type = bp2d::Coord;
};

template<> struct coordinate_system<bp2d::PointImpl> {
    using type = cs::cartesian;
};

template<> struct dimension<bp2d::PointImpl>: boost::mpl::int_<2> {};

template<>
struct access<bp2d::PointImpl, 0 > {
    static inline bp2d::Coord get(bp2d::PointImpl const& a) {
        return libnest2d::getX(a);
    }

    static inline void set(bp2d::PointImpl& a,
                           bp2d::Coord const& value) {
        libnest2d::setX(a, value);
    }
};

template<>
struct access<bp2d::PointImpl, 1 > {
    static inline bp2d::Coord get(bp2d::PointImpl const& a) {
        return libnest2d::getY(a);
    }

    static inline void set(bp2d::PointImpl& a,
                           bp2d::Coord const& value) {
        libnest2d::setY(a, value);
    }
};


/* ************************************************************************** */
/* Box concept adaptaion **************************************************** */
/* ************************************************************************** */

template<> struct tag<bp2d::Box> {
    using type = box_tag;
};

template<> struct point_type<bp2d::Box> {
    using type = bp2d::PointImpl;
};

template<> struct indexed_access<bp2d::Box, min_corner, 0> {
    static inline bp2d::Coord get(bp2d::Box const& box) {
        return bp2d::getX(box.minCorner());
    }
    static inline void set(bp2d::Box &box, bp2d::Coord const& coord) {
        bp2d::setX(box.minCorner(), coord);
    }
};

template<> struct indexed_access<bp2d::Box, min_corner, 1> {
    static inline bp2d::Coord get(bp2d::Box const& box) {
        return bp2d::getY(box.minCorner());
    }
    static inline void set(bp2d::Box &box, bp2d::Coord const& coord) {
        bp2d::setY(box.minCorner(), coord);
    }
};

template<> struct indexed_access<bp2d::Box, max_corner, 0> {
    static inline bp2d::Coord get(bp2d::Box const& box) {
        return bp2d::getX(box.maxCorner());
    }
    static inline void set(bp2d::Box &box, bp2d::Coord const& coord) {
        bp2d::setX(box.maxCorner(), coord);
    }
};

template<> struct indexed_access<bp2d::Box, max_corner, 1> {
    static inline bp2d::Coord get(bp2d::Box const& box) {
        return bp2d::getY(box.maxCorner());
    }
    static inline void set(bp2d::Box &box, bp2d::Coord const& coord) {
        bp2d::setY(box.maxCorner(), coord);
    }
};

/* ************************************************************************** */
/* Segment concept adaptaion ************************************************ */
/* ************************************************************************** */

template<> struct tag<bp2d::Segment> {
    using type = segment_tag;
};

template<> struct point_type<bp2d::Segment> {
    using type = bp2d::PointImpl;
};

template<> struct indexed_access<bp2d::Segment, 0, 0> {
    static inline bp2d::Coord get(bp2d::Segment const& seg) {
        return bp2d::getX(seg.first());
    }
    static inline void set(bp2d::Segment &seg, bp2d::Coord const& coord) {
        auto p = seg.first(); bp2d::setX(p, coord); seg.first(p);
    }
};

template<> struct indexed_access<bp2d::Segment, 0, 1> {
    static inline bp2d::Coord get(bp2d::Segment const& seg) {
        return bp2d::getY(seg.first());
    }
    static inline void set(bp2d::Segment &seg, bp2d::Coord const& coord) {
        auto p = seg.first(); bp2d::setY(p, coord); seg.first(p);
    }
};

template<> struct indexed_access<bp2d::Segment, 1, 0> {
    static inline bp2d::Coord get(bp2d::Segment const& seg) {
        return bp2d::getX(seg.second());
    }
    static inline void set(bp2d::Segment &seg, bp2d::Coord const& coord) {
        auto p = seg.second();  bp2d::setX(p, coord); seg.second(p);
    }
};

template<> struct indexed_access<bp2d::Segment, 1, 1> {
    static inline bp2d::Coord get(bp2d::Segment const& seg) {
        return bp2d::getY(seg.second());
    }
    static inline void set(bp2d::Segment &seg, bp2d::Coord const& coord) {
        auto p = seg.second(); bp2d::setY(p, coord); seg.second(p);
    }
};


/* ************************************************************************** */
/* Polygon concept adaptation *********************************************** */
/* ************************************************************************** */

// Connversion between libnest2d::Orientation and order_selector ///////////////

template<bp2d::Orientation> struct ToBoostOrienation {};

template<>
struct ToBoostOrienation<bp2d::Orientation::CLOCKWISE> {
    static const order_selector Value = clockwise;
};

template<>
struct ToBoostOrienation<bp2d::Orientation::COUNTER_CLOCKWISE> {
    static const order_selector Value = counterclockwise;
};

static const bp2d::Orientation RealOrientation =
        bp2d::OrientationType<bp2d::PolygonImpl>::Value;

// Ring implementation /////////////////////////////////////////////////////////

// Boost would refer to ClipperLib::Path (alias bp2d::PolygonImpl) as a ring
template<> struct tag<bp2d::PathImpl> {
    using type = ring_tag;
};

template<> struct point_order<bp2d::PathImpl> {
    static const order_selector value =
            ToBoostOrienation<RealOrientation>::Value;
};

// All our Paths should be closed for the bin packing application
template<> struct closure<bp2d::PathImpl> {
    static const closure_selector value = closed;
};

// Polygon implementation //////////////////////////////////////////////////////

template<> struct tag<bp2d::PolygonImpl> {
    using type = polygon_tag;
};

template<> struct exterior_ring<bp2d::PolygonImpl> {
    static inline bp2d::PathImpl& get(bp2d::PolygonImpl& p) {
        return libnest2d::shapelike::getContour(p);
    }

    static inline bp2d::PathImpl const& get(bp2d::PolygonImpl const& p) {
        return libnest2d::shapelike::getContour(p);
    }
};

template<> struct ring_const_type<bp2d::PolygonImpl> {
    using type = const bp2d::PathImpl&;
};

template<> struct ring_mutable_type<bp2d::PolygonImpl> {
    using type = bp2d::PathImpl&;
};

template<> struct interior_const_type<bp2d::PolygonImpl> {
   using type = const libnest2d::THolesContainer<bp2d::PolygonImpl>&;
};

template<> struct interior_mutable_type<bp2d::PolygonImpl> {
   using type = libnest2d::THolesContainer<bp2d::PolygonImpl>&;
};

template<>
struct interior_rings<bp2d::PolygonImpl> {

    static inline libnest2d::THolesContainer<bp2d::PolygonImpl>& get(
            bp2d::PolygonImpl& p)
    {
        return libnest2d::shapelike::holes(p);
    }

    static inline const libnest2d::THolesContainer<bp2d::PolygonImpl>& get(
            bp2d::PolygonImpl const& p)
    {
        return libnest2d::shapelike::holes(p);
    }
};

/* ************************************************************************** */
/* MultiPolygon concept adaptation ****************************************** */
/* ************************************************************************** */

template<> struct tag<bp2d::Shapes> {
    using type = multi_polygon_tag;
};

}   // traits
}   // geometry

// This is an addition to the ring implementation of Polygon concept
template<>
struct range_value<bp2d::PathImpl> {
    using type = bp2d::PointImpl;
};

template<>
struct range_value<bp2d::Shapes> {
    using type = bp2d::PolygonImpl;
};

}   // boost

/* ************************************************************************** */
/* Algorithms *************************************************************** */
/* ************************************************************************** */

namespace libnest2d { // Now the algorithms that boost can provide...

template<>
inline double pointlike::distance(const PointImpl& p1,
                                  const PointImpl& p2 )
{
    return boost::geometry::distance(p1, p2);
}

template<>
inline double pointlike::distance(const PointImpl& p,
                                  const bp2d::Segment& seg )
{
    return boost::geometry::distance(p, seg);
}

// Tell libnest2d how to make string out of a ClipperPolygon object
template<>
inline bool shapelike::intersects(const PathImpl& sh1,
                                  const PathImpl& sh2)
{
    return boost::geometry::intersects(sh1, sh2);
}

// Tell libnest2d how to make string out of a ClipperPolygon object
template<>
inline bool shapelike::intersects(const PolygonImpl& sh1,
                                  const PolygonImpl& sh2)
{
    return boost::geometry::intersects(sh1, sh2);
}

// Tell libnest2d how to make string out of a ClipperPolygon object
template<>
inline bool shapelike::intersects(const bp2d::Segment& s1,
                                  const bp2d::Segment& s2)
{
    return boost::geometry::intersects(s1, s2);
}

#ifndef DISABLE_BOOST_AREA
template<>
inline double shapelike::area(const PolygonImpl& shape, const PolygonTag&)
{
    return boost::geometry::area(shape);
}
#endif

template<>
inline bool shapelike::isInside<PolygonImpl>(const PointImpl& point,
                                const PolygonImpl& shape)
{
    return boost::geometry::within(point, shape);
}

template<>
inline bool shapelike::isInside(const PolygonImpl& sh1,
                                const PolygonImpl& sh2)
{
    return boost::geometry::within(sh1, sh2);
}

template<>
inline bool shapelike::touches( const PolygonImpl& sh1,
                                const PolygonImpl& sh2)
{
    return boost::geometry::touches(sh1, sh2);
}

template<>
inline bool shapelike::touches( const PointImpl& point,
                                const PolygonImpl& shape)
{
    return boost::geometry::touches(point, shape);
}

#ifndef DISABLE_BOOST_BOUNDING_BOX
template<>
inline bp2d::Box shapelike::boundingBox(const PolygonImpl& sh,
                                        const PolygonTag&)
{
    bp2d::Box b;
    boost::geometry::envelope(sh, b);
    return b;
}

template<>
inline bp2d::Box shapelike::boundingBox<PolygonImpl>(const bp2d::Shapes& shapes)
{
    bp2d::Box b;
    boost::geometry::envelope(shapes, b);
    return b;
}
#endif

#ifndef DISABLE_BOOST_CONVEX_HULL
template<>
inline PolygonImpl shapelike::convexHull(const PolygonImpl& sh)
{
    PolygonImpl ret;
    boost::geometry::convex_hull(sh, ret);
    return ret;
}

template<>
inline PolygonImpl shapelike::convexHull(const bp2d::Shapes& shapes)
{
    PolygonImpl ret;
    boost::geometry::convex_hull(shapes, ret);
    return ret;
}
#endif

#ifndef DISABLE_BOOST_ROTATE
template<>
inline void shapelike::rotate(PolygonImpl& sh, const Radians& rads)
{
    namespace trans = boost::geometry::strategy::transform;

    PolygonImpl cpy = sh;
    trans::rotate_transformer<boost::geometry::radian, Radians, 2, 2>
            rotate(rads);

    boost::geometry::transform(cpy, sh, rotate);
}
#endif

#ifndef DISABLE_BOOST_TRANSLATE
template<>
inline void shapelike::translate(PolygonImpl& sh, const PointImpl& offs)
{
    namespace trans = boost::geometry::strategy::transform;

    PolygonImpl cpy = sh;
    trans::translate_transformer<bp2d::Coord, 2, 2> translate(
                bp2d::getX(offs), bp2d::getY(offs));

    boost::geometry::transform(cpy, sh, translate);
}
#endif

#ifndef DISABLE_BOOST_OFFSET
template<>
inline void shapelike::offset(PolygonImpl& sh, bp2d::Coord distance)
{
    PolygonImpl cpy = sh;
    boost::geometry::buffer(cpy, sh, distance);
}
#endif

#ifndef DISABLE_BOOST_NFP_MERGE
template<>
inline bp2d::Shapes Nfp::merge(const bp2d::Shapes& shapes,
                               const PolygonImpl& sh)
{
    bp2d::Shapes retv;
    boost::geometry::union_(shapes, sh, retv);
    return retv;
}
#endif

#ifndef DISABLE_BOOST_SERIALIZE
template<> inline std::string shapelike::serialize<libnest2d::Formats::SVG>(
        const PolygonImpl& sh, double scale)
{
    std::stringstream ss;
    std::string style = "fill: none; stroke: black; stroke-width: 1px;";

    using namespace boost::geometry;
    using Pointf = model::point<double, 2, cs::cartesian>;
    using Polygonf = model::polygon<Pointf>;

    Polygonf::ring_type ring;
    Polygonf::inner_container_type holes;
    ring.reserve(shapelike::contourVertexCount(sh));

    for(auto it = shapelike::cbegin(sh); it != shapelike::cend(sh); it++) {
        auto& v = *it;
        ring.emplace_back(getX(v)*scale, getY(v)*scale);
    };

    auto H = shapelike::holes(sh);
    for(PathImpl& h : H ) {
        Polygonf::ring_type hf;
        for(auto it = h.begin(); it != h.end(); it++) {
            auto& v = *it;
            hf.emplace_back(getX(v)*scale, getY(v)*scale);
        };
        holes.push_back(hf);
    }

    Polygonf poly;
    poly.outer() = ring;
    poly.inners() = holes;
    auto svg_data = boost::geometry::svg(poly, style);

    ss << svg_data << std::endl;

    return ss.str();
}
#endif

#ifndef DISABLE_BOOST_UNSERIALIZE
template<>
inline void shapelike::unserialize<libnest2d::Formats::SVG>(
        PolygonImpl& sh,
        const std::string& str)
{
}
#endif

template<> inline std::pair<bool, std::string>
shapelike::isValid(const PolygonImpl& sh)
{
    std::string message;
    bool ret = boost::geometry::is_valid(sh, message);

    return {ret, message};
}

}



#endif // BOOST_ALG_HPP
