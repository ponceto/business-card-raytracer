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

    vec3f checked() const
    {
        auto check = [](const float value) -> float
        {
            return (value != value ? 0.0f : value);
        };

        return vec3f ( check(x)
                     , check(y)
                     , check(z) );
    }

    vec3f clamped() const
    {
        auto clamp = [](const float value) -> float
        {
            return value;
        };

        return vec3f ( clamp(x)
                     , clamp(y)
                     , clamp(z) );
    }

    vec3f normalized() const
    {
        const float veclen = length(*this);

        return vec3f ( (x / veclen)
                     , (y / veclen)
                     , (z / veclen) );
    }

    static float length(const vec3f& rhs)
    {
        return ::sqrtf ( (rhs.x * rhs.x)
                       + (rhs.y * rhs.y)
                       + (rhs.z * rhs.z) );
    }

    static float length2(const vec3f& rhs)
    {
        return ( (rhs.x * rhs.x)
               + (rhs.y * rhs.y)
               + (rhs.z * rhs.z) );
    }

    static vec3f normalize(const vec3f& rhs)
    {
        const float veclen = length(rhs);

        return vec3f ( (rhs.x / veclen)
                     , (rhs.y / veclen)
                     , (rhs.z / veclen) );
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

    pos3f checked() const
    {
        auto check = [](const float value) -> float
        {
            return (value != value ? 0.0f : value);
        };

        return pos3f ( check(x)
                     , check(y)
                     , check(z) );
    }

    pos3f clamped() const
    {
        auto clamp = [](const float value) -> float
        {
            return value;
        };

        return pos3f ( clamp(x)
                     , clamp(y)
                     , clamp(z) );
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

    col3f checked() const
    {
        auto check = [](const float value) -> float
        {
            return (value != value ? 0.0f : value);
        };

        return col3f ( check(r)
                     , check(g)
                     , check(b) );
    }

    col3f clamped() const
    {
        auto clamp = [](const float value) -> float
        {
            constexpr float min = 0.0f;
            constexpr float max = 1.0f;

            if(value < min) {
                return min;
            }
            if(value > max) {
                return max;
            }
            return value;
        };

        return col3f ( clamp(r)
                     , clamp(g)
                     , clamp(b) );
    }

    float r;
    float g;
    float b;
};

}

// ---------------------------------------------------------------------------
// rt::using
// ---------------------------------------------------------------------------

namespace rt {

using vec3f = gl::vec3f;
using pos3f = gl::pos3f;
using col3f = gl::col3f;

}

// ---------------------------------------------------------------------------
// rt::hit_result
// ---------------------------------------------------------------------------

namespace rt {

class hit_result
{
public:
    hit_result()
        : distance(DISTANCE_MAX)
        , position()
        , normal()
        , color()
        , reflect()
        , refract()
        , specular()
    {
    }

    static constexpr float DISTANCE_MAX = 1e+9f;
    static constexpr float DISTANCE_MIN = 1e-5f;

    float distance;
    pos3f position;
    vec3f normal;
    col3f color;
    float reflect;
    float refract;
    float specular;
};

}

// ---------------------------------------------------------------------------
// rt::ray
// ---------------------------------------------------------------------------

namespace rt {

class ray
{
public:
    ray ( const pos3f& ray_origin
        , const vec3f& ray_direction )
        : origin(ray_origin)
        , direction(ray_direction.normalized())
    {
    }

    pos3f origin;
    vec3f direction;
};

}

// ---------------------------------------------------------------------------
// rt::camera
// ---------------------------------------------------------------------------

namespace rt {

class camera
{
public:
    camera ( const pos3f& camera_position
           , const pos3f& camera_target
           , const pos3f& camera_up
           , const float  camera_fov
           , const float  camera_dof
           , const float  camera_focus )
        : camera ( camera_position
                 , pos3f::difference(camera_target, camera_position)
                 , pos3f::difference(camera_up    , camera_position)
                 , camera_fov
                 , camera_dof
                 , camera_focus )
    {
    }

    camera ( const pos3f& camera_position
           , const vec3f& camera_direction
           , const vec3f& camera_normal
           , const float  camera_fov
           , const float  camera_dof
           , const float  camera_focus )
        : position(camera_position)
        , direction(camera_direction.normalized())
        , normal(camera_normal.normalized())
        , fov(camera_fov)
        , dof(camera_dof)
        , focus(camera_focus)
    {
    }

    pos3f position;
    vec3f direction;
    vec3f normal;
    float fov;
    float dof;
    float focus;
};

}

// ---------------------------------------------------------------------------
// rt::light
// ---------------------------------------------------------------------------

namespace rt {

class light
{
public:
    light ( const pos3f& light_position
          , const col3f& light_color
          , const float  light_power )
        : position(light_position)
        , color(light_color)
        , power(light_power)
    {
    }

    pos3f position;
    col3f color;
    float power;
};

}

// ---------------------------------------------------------------------------
// rt::sky
// ---------------------------------------------------------------------------

namespace rt {

class sky
{
public:
    sky ( const col3f& sky_color
        , const col3f& sky_ambient )
        : color(sky_color)
        , ambient(sky_ambient)
    {
    }

    col3f color;
    col3f ambient;
};

}

// ---------------------------------------------------------------------------
// rt::object
// ---------------------------------------------------------------------------

namespace rt {

class object
{
public:
    object()
        : reflect(0.0f)
        , refract(0.0f)
        , specular(0.0f)
    {
    }

    virtual ~object() = default;

    virtual bool hit(const ray&, hit_result&) const = 0;

    using shared_ptr = std::shared_ptr<object>;
    using vector     = std::vector<shared_ptr>;

    float reflect;
    float refract;
    float specular;
};

}

// ---------------------------------------------------------------------------
// rt::floor
// ---------------------------------------------------------------------------

namespace rt {

class floor final
    : public object
{
public:
    floor ( const pos3f& floor_position
          , const vec3f& floor_normal
          , const col3f& floor_color1
          , const col3f& floor_color2
          , const float  floor_scale )
        : object()
        , position(floor_position)
        , normal(floor_normal.normalized())
        , color1(floor_color1)
        , color2(floor_color2)
        , scale(floor_scale)
    {
    }

    virtual ~floor() = default;

    virtual bool hit(const ray&, hit_result&) const override;

    pos3f position;
    vec3f normal;
    col3f color1;
    col3f color2;
    float scale;
};

}

// ---------------------------------------------------------------------------
// rt::sphere
// ---------------------------------------------------------------------------

namespace rt {

class sphere final
    : public object
{
public:
    sphere ( const pos3f& sphere_center
           , const col3f& sphere_color
           , const float  sphere_radius )
        : object()
        , center(sphere_center)
        , color(sphere_color)
        , radius(sphere_radius)
    {
    }

    virtual ~sphere() = default;

    virtual bool hit(const ray&, hit_result&) const override;

    pos3f center;
    col3f color;
    float radius;
};

}

// ---------------------------------------------------------------------------
// rt::scene
// ---------------------------------------------------------------------------

namespace rt {

class scene
{
public:
    scene ( const camera& scene_camera
          , const light&  scene_light
          , const sky&    scene_sky )
        : _camera(scene_camera)
        , _light(scene_light)
        , _sky(scene_sky)
        , _objects()
    {
    }

    virtual ~scene() = default;

    auto get_camera() const -> const camera&
    {
        return _camera;
    }

    auto get_light() const -> const light&
    {
        return _light;
    }

    auto get_sky() const -> const sky&
    {
        return _sky;
    }

    auto get_objects() const -> const auto&
    {
        return _objects;
    }

    template <typename T>
    void add(T& object_ptr)
    {
        _objects.push_back(std::move(object_ptr));
    }

protected:
    camera         _camera;
    light          _light;
    sky            _sky;
    object::vector _objects;
};

}

// ---------------------------------------------------------------------------
// card::raytracer
// ---------------------------------------------------------------------------

namespace card {

class raytracer
{
public:
    raytracer(base::console& console, const rt::scene& scene);

    virtual ~raytracer() = default;

    void render(ppm::writer&, const int w, const int h, const int samples, const int recursions);

protected:
    rt::col3f trace(const rt::ray& ray, const int depth);

    bool hit(const rt::ray& ray, rt::hit_result& result);

protected:
    base::console&   _console;
    base::randomizer _random1;
    base::randomizer _random2;
    const rt::scene& _scene;
};

}

// ---------------------------------------------------------------------------
// card::scene_factory
// ---------------------------------------------------------------------------

namespace card {

class scene_factory
{
public:
    scene_factory(const std::string& scene_name);

    virtual ~scene_factory() = default;

    static std::shared_ptr<rt::scene> create(const std::string& scene_name);

protected:
    void initialize();

    void initialize_default();

    void initialize_aek();

    void initialize_ponceto();

    void initialize_smiley();

    void initialize_simple();

    std::shared_ptr<rt::scene> build();

protected:
    std::string _name;
    uint32_t    _world[16];
    rt::pos3f   _camera_position;
    rt::pos3f   _camera_target;
    rt::pos3f   _camera_top;
    float       _camera_fov;
    float       _camera_dof;
    float       _camera_focus;
    rt::pos3f   _light_position;
    rt::col3f   _light_color;
    float       _light_power;
    rt::col3f   _sky_color;
    rt::col3f   _sky_ambient;
    rt::pos3f   _floor_position;
    rt::vec3f   _floor_normal;
    rt::col3f   _floor_color1;
    rt::col3f   _floor_color2;
    float       _floor_scale;
    float       _floor_reflect;
    float       _floor_refract;
    float       _floor_specular;
    float       _sphere_radius;
    rt::col3f   _sphere_color;
    float       _sphere_reflect;
    float       _sphere_refract;
    float       _sphere_specular;
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
    std::string _scene;
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
