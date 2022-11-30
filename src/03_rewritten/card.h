/*
 * The Business Card Raytracer
 *
 * Original author: Andrew Kensler
 *
 * Refactored with love by Olivier Poncet
 *
 */
#ifndef __BUSINESS_CARD_RAYTRACER_H__
#define __BUSINESS_CARD_RAYTRACER_H__

// ---------------------------------------------------------------------------
// base::using
// ---------------------------------------------------------------------------

namespace base {

using arglist      = std::vector<std::string>;
using steady_clock = std::chrono::steady_clock;

}

// ---------------------------------------------------------------------------
// base::profiler
// ---------------------------------------------------------------------------

namespace base {

class profiler
{
public:
    profiler(const std::string& name)
        : _name(name)
        , _time(steady_clock::now())
    {
    }

    auto name() const -> const std::string&
    {
        return _name;
    }

    auto reset() -> void
    {
        _time = steady_clock::now();
    }

    auto elapsed() const -> double
    {
        return std::chrono::duration<double>(steady_clock::now() - _time).count();
    }

private:
    const std::string        _name;
    steady_clock::time_point _time;
};

}

// ---------------------------------------------------------------------------
// base::console
// ---------------------------------------------------------------------------

namespace base {

class console
{
public:
    console ( std::istream& cin
            , std::ostream& cout
            , std::ostream& cerr );

    virtual ~console() = default;

    auto cin() const -> std::istream&
    {
        return _cin;
    }

    auto cout() const -> std::ostream&
    {
        return _cout;
    }

    auto cerr() const -> std::ostream&
    {
        return _cerr;
    }

protected:
    std::istream& _cin;
    std::ostream& _cout;
    std::ostream& _cerr;
};

}

// ---------------------------------------------------------------------------
// base::program
// ---------------------------------------------------------------------------

namespace base {

class program
{
public:
    program(int argc, char* argv[]);

    virtual ~program() = default;

    virtual void main() = 0;

protected:
    const arglist _arglist;
};

}

// ---------------------------------------------------------------------------
// ppm::stream
// ---------------------------------------------------------------------------

namespace ppm {

class stream
{
public:
    stream(const std::string& filename);

    virtual ~stream();

protected:
    const std::string _filename;
    FILE*             _stream;
    int               _width;
    int               _height;
    int               _maxval;
};

}

// ---------------------------------------------------------------------------
// ppm::reader
// ---------------------------------------------------------------------------

namespace ppm {

class reader
    : public stream
{
public:
    reader(const std::string& filename);

    virtual ~reader() = default;

    void open(int& width, int& height, int& maxval);

    void fetch(int& r, int& g, int& b);

    void close();
};

}

// ---------------------------------------------------------------------------
// ppm::writer
// ---------------------------------------------------------------------------

namespace ppm {

class writer
    : public stream
{
public:
    writer(const std::string& filename);

    virtual ~writer() = default;

    void open(int width, int height, int maxval);

    void store(int r, int g, int b);

    void close();
};

}

// ---------------------------------------------------------------------------
// vec3f: vector implementation
// ---------------------------------------------------------------------------

namespace card {

class vec3f
{
public:
    vec3f()
        : x()
        , y()
        , z()
    {
    }

    vec3f(const float a, const float b, const float c)
        : x(a)
        , y(b)
        , z(c)
    {
    }

    vec3f operator+(const vec3f& rhs) const
    {
        return vec3f ( (x + rhs.x)
                     , (y + rhs.y)
                     , (z + rhs.z) );
    }

    vec3f operator-(const vec3f& rhs) const
    {
        return vec3f ( (x - rhs.x)
                     , (y - rhs.y)
                     , (z - rhs.z) );
    }

    vec3f operator*(const float scalar) const
    {
        return vec3f ( (x * scalar)
                     , (y * scalar)
                     , (z * scalar) );
    }

    vec3f operator/(const float scalar) const
    {
        return vec3f ( (x / scalar)
                     , (y / scalar)
                     , (z / scalar) );
    }

    static float length(const vec3f& vec)
    {
        return ::sqrtf ( (vec.x * vec.x)
                       + (vec.y * vec.y)
                       + (vec.z * vec.z) );
    }

    static float dot(const vec3f& lhs, const vec3f& rhs)
    {
        return ( (lhs.x * rhs.x)
               + (lhs.y * rhs.y)
               + (lhs.z * rhs.z) );
    }

    static vec3f cross(const vec3f& lhs, const vec3f& rhs)
    {
        return vec3f ( (lhs.y * rhs.z - lhs.z * rhs.y)
                     , (lhs.z * rhs.x - lhs.x * rhs.z)
                     , (lhs.x * rhs.y - lhs.y * rhs.x) );
    }

    static vec3f normalize(const vec3f& vec)
    {
        const float veclen = length(vec);

        return vec3f ( (vec.x / veclen)
                     , (vec.y / veclen)
                     , (vec.z / veclen) );
    }

public:
    float x;
    float y;
    float z;
};

}

// ---------------------------------------------------------------------------
// card::HitType
// ---------------------------------------------------------------------------

namespace card {

enum HitType
{
    kNoHit     = 0,
    kPlaneHit  = 1,
    kSphereHit = 2,
};

}

// ---------------------------------------------------------------------------
// card::raytracer
// ---------------------------------------------------------------------------

namespace card {

class raytracer
{
public:
    raytracer();

    virtual ~raytracer() = default;

    void raytrace(ppm::writer&, const int w, const int h);

protected:
    vec3f sample(const vec3f& origin, const vec3f& direction);

    int trace(const vec3f& origin, const vec3f& direction, float& distance, vec3f& normal);

protected:
    const vec3f _camera_position;
    const vec3f _camera_target;
    const vec3f _camera_normal;
    const vec3f _camera_direction;
    const vec3f _light_pos;
    const vec3f _ambiant_color;
    const vec3f _sky_color;
    const vec3f _floor_color1;
    const vec3f _floor_color2;
    const float _fov;
    const float _dof;
};

}

// ---------------------------------------------------------------------------
// card::generator
// ---------------------------------------------------------------------------

namespace card {

class generator final
    : public base::console
    , public base::program
{
public:
    generator(int argc, char* argv[]);

    virtual ~generator() = default;

    virtual void main() override;

    bool parse();

    void usage();

protected:
    std::string _program;
    std::string _output;
    int         _card_w;
    int         _card_h;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __BUSINESS_CARD_RAYTRACER_H__ */
