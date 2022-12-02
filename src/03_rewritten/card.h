/*
 * The Business Card Raytracer
 *
 * Original author: Andrew Kensler
 *
 * Refactored with love by Olivier Poncet
 */
#ifndef __BUSINESS_CARD_RAYTRACER_H__
#define __BUSINESS_CARD_RAYTRACER_H__

// ---------------------------------------------------------------------------
// base::using
// ---------------------------------------------------------------------------

namespace base {

using arglist                    = std::vector<std::string>;
using steady_clock               = std::chrono::steady_clock;
using mersenne_twister           = std::mt19937;
using uniform_float_distribution = std::uniform_real_distribution<float>;

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
// base::ramdom
// ---------------------------------------------------------------------------

namespace base {

class randomizer
{
public:
    randomizer(const float min, const float max);

    virtual ~randomizer() = default;

    float operator()()
    {
        return _distributor(_generator);
    }

protected:
    mersenne_twister           _generator;
    uniform_float_distribution _distributor;
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
// gl::vec3f
// ---------------------------------------------------------------------------

namespace gl {

class vec3f
{
public:
    vec3f()
        : vec3f(0.0f, 0.0f, 0.0f)
    {
    }

    vec3f(const float vx, const float vy, const float vz)
        : x(vx)
        , y(vy)
        , z(vz)
    {
    }

    vec3f operator+() const
    {
        return vec3f ( (+x)
                     , (+y)
                     , (+z) );
    }

    vec3f operator-() const
    {
        return vec3f ( (-x)
                     , (-y)
                     , (-z) );
    }

    vec3f operator+(const vec3f& vector) const
    {
        return vec3f ( (x + vector.x)
                     , (y + vector.y)
                     , (z + vector.z) );
    }

    vec3f operator-(const vec3f& vector) const
    {
        return vec3f ( (x - vector.x)
                     , (y - vector.y)
                     , (z - vector.z) );
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

    vec3f& operator+=(const vec3f& vector)
    {
        x += vector.x;
        y += vector.y;
        z += vector.z;
        return *this;
    }

    vec3f& operator-=(const vec3f& vector)
    {
        x -= vector.x;
        y -= vector.y;
        z -= vector.z;
        return *this;
    }

    vec3f& operator*=(const float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    vec3f& operator/=(const float scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
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

    float x;
    float y;
    float z;
};

}

// ---------------------------------------------------------------------------
// gl::pos3f
// ---------------------------------------------------------------------------

namespace gl {

class pos3f
{
public:
    pos3f()
        : pos3f(0.0f, 0.0f, 0.0f)
    {
    }

    pos3f(const float px, const float py, const float pz)
        : x(px)
        , y(py)
        , z(pz)
    {
    }

    pos3f operator+() const
    {
        return pos3f ( (+x)
                     , (+y)
                     , (+z) );
    }

    pos3f operator-() const
    {
        return pos3f ( (-x)
                     , (-y)
                     , (-z) );
    }

    pos3f operator+(const vec3f& vector) const
    {
        return pos3f ( (x + vector.x)
                     , (y + vector.y)
                     , (z + vector.z) );
    }

    pos3f operator-(const vec3f& vector) const
    {
        return pos3f ( (x - vector.x)
                     , (y - vector.y)
                     , (z - vector.z) );
    }

    pos3f operator*(const float scalar) const
    {
        return pos3f ( (x * scalar)
                     , (y * scalar)
                     , (z * scalar) );
    }

    pos3f operator/(const float scalar) const
    {
        return pos3f ( (x / scalar)
                     , (y / scalar)
                     , (z / scalar) );
    }

    pos3f& operator+=(const vec3f& vector)
    {
        x += vector.x;
        y += vector.y;
        z += vector.z;
        return *this;
    }

    pos3f& operator-=(const vec3f& vector)
    {
        x -= vector.x;
        y -= vector.y;
        z -= vector.z;
        return *this;
    }

    pos3f& operator*=(const float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    pos3f& operator/=(const float scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    static vec3f difference(const pos3f& lhs, const pos3f& rhs)
    {
        return vec3f ( (lhs.x - rhs.x)
                     , (lhs.y - rhs.y)
                     , (lhs.z - rhs.z) );
    }

    float x;
    float y;
    float z;
};

}

// ---------------------------------------------------------------------------
// gl::col3f
// ---------------------------------------------------------------------------

namespace gl {

class col3f
{
public:
    col3f()
        : col3f(0.0f, 0.0f, 0.0f)
    {
    }

    col3f(const float cr, const float cg, const float cb)
        : r(cr)
        , g(cg)
        , b(cb)
    {
    }

    col3f operator+(const col3f& color) const
    {
        return col3f ( (r + color.r)
                     , (g + color.g)
                     , (b + color.b) );
    }

    col3f operator-(const col3f& color) const
    {
        return col3f ( (r - color.r)
                     , (g - color.g)
                     , (b - color.b) );
    }

    col3f operator*(const col3f& color) const
    {
        return col3f ( (r * color.r)
                     , (g * color.g)
                     , (b * color.b) );
    }

    col3f operator*(const float scalar) const
    {
        return col3f ( (r * scalar)
                     , (g * scalar)
                     , (b * scalar) );
    }

    col3f operator/(const float scalar) const
    {
        return col3f ( (r / scalar)
                     , (g / scalar)
                     , (b / scalar) );
    }

    col3f& operator+=(const col3f& color)
    {
        r += color.r;
        g += color.g;
        b += color.b;
        return *this;
    }

    col3f& operator-=(const col3f& color)
    {
        r -= color.r;
        g -= color.g;
        b -= color.b;
        return *this;
    }

    col3f& operator*=(const float scalar)
    {
        r *= scalar;
        g *= scalar;
        b *= scalar;
        return *this;
    }

    col3f& operator/=(const float scalar)
    {
        r /= scalar;
        g /= scalar;
        b /= scalar;
        return *this;
    }

    float r;
    float g;
    float b;
};

}

// ---------------------------------------------------------------------------
// gl::camera
// ---------------------------------------------------------------------------

namespace gl {

class camera
{
public:
    camera()
        : camera ( pos3f(0.0f, -1.0f, +1.0f)
                 , vec3f(0.0f, +1.0f, +1.0f)
                 , vec3f(0.0f,  0.0f, +1.0f) )
    {
    }

    camera(const pos3f& camera_position, const pos3f& camera_target, const pos3f& camera_up)
        : camera ( camera_position
                 , pos3f::difference(camera_target, camera_position)
                 , pos3f::difference(camera_up    , camera_position) )
    {
    }

    camera(const pos3f& camera_position, const vec3f& camera_direction, const vec3f& camera_normal)
        : position(camera_position)
        , direction(vec3f::normalize(camera_direction))
        , normal(vec3f::normalize(camera_normal))
    {
    }

    pos3f position;
    vec3f direction;
    vec3f normal;
};

}

// ---------------------------------------------------------------------------
// gl::light
// ---------------------------------------------------------------------------

namespace gl {

class light
{
public:
    light()
        : light ( pos3f(-100.0f, -100.0f, +100.0f)
                , col3f(  +1.0f,   +1.0f,   +1.0f)
                , 1.0f )
    {
    }

    light(const pos3f& light_position, const col3f& light_color, const float power)
        : position(light_position)
        , color(light_color)
        , power(power)
    {
    }

    pos3f position;
    col3f color;
    float power;
};

}

// ---------------------------------------------------------------------------
// gl::floor
// ---------------------------------------------------------------------------

namespace gl {

class floor
{
public:
    floor()
        : floor ( pos3f( 0.0f,  0.0f, +0.0f)
                , vec3f( 0.0f,  0.0f, +1.0f)
                , col3f(+1.0f, +0.3f, +0.3f)
                , col3f(+1.0f, +1.0f, +1.0f) )
    {
    }

    floor ( const pos3f& plane_position
          , const vec3f& plane_normal
          , const col3f& plane_color1
          , const col3f& plane_color2 )
        : position(plane_position)
        , normal(vec3f::normalize(plane_normal))
        , color1(plane_color1)
        , color2(plane_color2)
    {
    }

    pos3f position;
    vec3f normal;
    col3f color1;
    col3f color2;
};

}

// ---------------------------------------------------------------------------
// gl::sky
// ---------------------------------------------------------------------------

namespace gl {

class sky
{
public:
    sky()
        : sky(col3f(+0.7f, +0.6f, +1.0f))
    {
    }

    sky(const col3f& c)
        : color(c)
    {
    }

    col3f color;
};

}

// ---------------------------------------------------------------------------
// gl::ray
// ---------------------------------------------------------------------------

namespace gl {

class ray
{
public:
    ray(const pos3f& ray_origin, const vec3f& ray_direction)
        : origin(ray_origin)
        , direction(vec3f::normalize(ray_direction))
    {
    }

    pos3f origin;
    vec3f direction;
};

}

// ---------------------------------------------------------------------------
// card::HitType
// ---------------------------------------------------------------------------

namespace card {

enum HitType
{
    kSkyHit    = 0,
    kFloorHit  = 1,
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

    void render(ppm::writer&, const int w, const int h);

protected:
    gl::col3f trace(const gl::ray& ray, const int depth);

    int hit(const gl::ray& ray, gl::pos3f& position, gl::vec3f& normal);

protected:
    const gl::camera   _camera;
    const gl::light    _light;
    const gl::floor    _floor;
    const gl::sky      _sky;
    const gl::col3f    _ambiant;
    const float        _fov;
    const float        _dof;
    const float        _focus;
    const int          _samples;
    const int          _recursions;
    base::randomizer   _random1;
    base::randomizer   _random2;
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
