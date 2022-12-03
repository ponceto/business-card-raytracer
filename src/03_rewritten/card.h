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
        : vec3f ( 0.0f
                , 0.0f
                , 0.0f )
    {
    }

    vec3f ( const float vec_x
          , const float vec_y
          , const float vec_z )
        : x(vec_x)
        , y(vec_y)
        , z(vec_z)
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
        : pos3f ( 0.0f
                , 0.0f
                , 0.0f )
    {
    }

    pos3f ( const float pos_x
          , const float pos_y
          , const float pos_z )
        : x(pos_x)
        , y(pos_y)
        , z(pos_z)
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
        : col3f ( 0.0f
                , 0.0f
                , 0.0f )
    {
    }

    col3f ( const float color_r
          , const float color_g
          , const float color_b )
        : r(color_r)
        , g(color_g)
        , b(color_b)
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
// rt::hit_result
// ---------------------------------------------------------------------------

namespace rt {

class hit_result
{
public:
    hit_result()
        : type(SKY_HIT)
        , distance(distance_max)
        , position()
        , normal()
    {
    }

    static constexpr float distance_max = std::numeric_limits<float>::max();
    static constexpr float distance_min = std::numeric_limits<float>::epsilon();
    static constexpr int   SKY_HIT      = 0;
    static constexpr int   FLOOR_HIT    = 1;
    static constexpr int   SPHERE_HIT   = 2;

    int       type;
    float     distance;
    gl::pos3f position;
    gl::vec3f normal;
};

}

// ---------------------------------------------------------------------------
// rt::ray
// ---------------------------------------------------------------------------

namespace rt {

class ray
{
public:
    ray ( const gl::pos3f& ray_origin
        , const gl::vec3f& ray_direction )
        : origin(ray_origin)
        , direction(gl::vec3f::normalize(ray_direction))
    {
    }

    gl::pos3f origin;
    gl::vec3f direction;
};

}

// ---------------------------------------------------------------------------
// rt::camera
// ---------------------------------------------------------------------------

namespace rt {

class camera
{
public:
    camera ( const gl::pos3f& camera_position
           , const gl::pos3f& camera_target
           , const gl::pos3f& camera_up
           , const float      camera_fov
           , const float      camera_dof
           , const float      camera_focus )
        : camera ( camera_position
                 , gl::pos3f::difference(camera_target, camera_position)
                 , gl::pos3f::difference(camera_up    , camera_position)
                 , camera_fov
                 , camera_dof
                 , camera_focus )
    {
    }

    camera ( const gl::pos3f& camera_position
           , const gl::vec3f& camera_direction
           , const gl::vec3f& camera_normal
           , const float      camera_fov
           , const float      camera_dof
           , const float      camera_focus )
        : position(camera_position)
        , direction(gl::vec3f::normalize(camera_direction))
        , normal(gl::vec3f::normalize(camera_normal))
        , fov(camera_fov)
        , dof(camera_dof)
        , focus(camera_focus)
    {
    }

    gl::pos3f position;
    gl::vec3f direction;
    gl::vec3f normal;
    float     fov;
    float     dof;
    float     focus;
};

}

// ---------------------------------------------------------------------------
// rt::light
// ---------------------------------------------------------------------------

namespace rt {

class light
{
public:
    light ( const gl::pos3f& light_position
          , const gl::col3f& light_color
          , const float      light_power )
        : position(light_position)
        , color(light_color)
        , power(light_power)
    {
    }

    gl::pos3f position;
    gl::col3f color;
    float     power;
};

}

// ---------------------------------------------------------------------------
// rt::floor
// ---------------------------------------------------------------------------

namespace rt {

class floor
{
public:
    floor ( const gl::pos3f& floor_position
          , const gl::vec3f& floor_normal
          , const gl::col3f& floor_color1
          , const gl::col3f& floor_color2 )
        : position(floor_position)
        , normal(gl::vec3f::normalize(floor_normal))
        , color1(floor_color1)
        , color2(floor_color2)
    {
    }

    gl::pos3f position;
    gl::vec3f normal;
    gl::col3f color1;
    gl::col3f color2;
};

}

// ---------------------------------------------------------------------------
// rt::sky
// ---------------------------------------------------------------------------

namespace rt {

class sky
{
public:
    sky(const gl::col3f& sky_color)
        : color(sky_color)
    {
    }

    gl::col3f color;
};

}

// ---------------------------------------------------------------------------
// card::scene
// ---------------------------------------------------------------------------

namespace card {

class scene
{
public:
    scene();

    scene ( const rt::camera& scene_camera
          , const rt::light&  scene_light
          , const rt::floor&  scene_floor
          , const rt::sky&    scene_sky
          , const gl::col3f&  scene_ambient )
        : _camera(scene_camera)
        , _light(scene_light)
        , _floor(scene_floor)
        , _sky(scene_sky)
        , _ambient(scene_ambient)
    {
    }

    virtual ~scene() = default;

    auto camera() const -> const rt::camera&
    {
        return _camera;
    }

    auto light() const -> const rt::light&
    {
        return _light;
    }

    auto floor() const -> const rt::floor&
    {
        return _floor;
    }

    auto sky() const -> const rt::sky&
    {
        return _sky;
    }

    auto ambient() const -> const gl::col3f&
    {
        return _ambient;
    }

protected:
    rt::camera _camera;
    rt::light  _light;
    rt::floor  _floor;
    rt::sky    _sky;
    gl::col3f  _ambient;
};

}

// ---------------------------------------------------------------------------
// card::raytracer
// ---------------------------------------------------------------------------

namespace card {

class raytracer
{
public:
    raytracer(base::console& console, card::scene& scene);

    virtual ~raytracer() = default;

    void render(ppm::writer&, const int w, const int h, const int samples, const int recursions);

protected:
    gl::col3f trace(const rt::ray& ray, const int depth);

    int hit(const rt::ray& ray, rt::hit_result& result);

protected:
    base::console&   _console;
    base::randomizer _random1;
    base::randomizer _random2;
    card::scene&     _scene;
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
    int         _samples;
    int         _recursions;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __BUSINESS_CARD_RAYTRACER_H__ */
