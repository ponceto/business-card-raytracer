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
using mutex_locker               = std::lock_guard<std::mutex>;

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

    auto data() const -> uint8_t*
    {
        return _buffer;
    }

    auto size() const -> size_t
    {
        return _length;
    }

    auto width() const -> int
    {
        return _width;
    }

    auto height() const -> int
    {
        return _height;
    }

    auto maxval() const -> int
    {
        return _maxval;
    }

protected:
    const std::string _filename;
    FILE*             _stream;
    uint8_t*          _buffer;
    size_t            _length;
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

    void fetch();

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

    void store();

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
        : x(0.0f)
        , y(0.0f)
        , z(0.0f)
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

    vec3f ( const float vec_x
          , const float vec_y
          , const float vec_z
          , const bool  normalize_vector )
        : x(vec_x)
        , y(vec_y)
        , z(vec_z)
    {
        if(normalize_vector != false) {
            const float veclen = length(*this);
            x /= veclen;
            y /= veclen;
            z /= veclen;
        }
    }

    vec3f(const vec3f& vec, const bool normalize_vector)
        : x(vec.x)
        , y(vec.y)
        , z(vec.z)
    {
        if(normalize_vector != false) {
            const float veclen = length(*this);
            x /= veclen;
            y /= veclen;
            z /= veclen;
        }
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
// gl::rec4i
// ---------------------------------------------------------------------------

namespace gl {

class rec4i
{
public:
    rec4i()
        : x(0)
        , y(0)
        , w(0)
        , h(0)
    {
    }

    rec4i ( int rec_x
          , int rec_y
          , int rec_w
          , int rec_h )
        : x(rec_x)
        , y(rec_y)
        , w(rec_w)
        , h(rec_h)
    {
        if(w < 0) {
            w = -w;
            x -= w;
        }
        if(h < 0) {
            h = -h;
            y -= h;
        }
    }

    int x;
    int y;
    int w;
    int h;
};

}

// ---------------------------------------------------------------------------
// rt::using
// ---------------------------------------------------------------------------

namespace rt {

using vec3f = gl::vec3f;
using pos3f = gl::pos3f;
using col3f = gl::col3f;
using rec4i = gl::rec4i;

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
        , eta()
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
    float eta;
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
        , direction(ray_direction, true)
    {
    }

    ray reflect(const float distance, const vec3f& normal) const
    {
        const pos3f o(origin + (direction * (distance - hit_result::DISTANCE_MIN)));
        const vec3f d(direction + normal * (vec3f::dot(normal, direction) * -2.0f));

        return ray(o, d);
    }

    ray refract(const float distance, const vec3f& normal, const float eta) const
    {
        const float dot = vec3f::dot(normal, direction);
        const float k   = 1.0f - (eta * eta) * (1.0f - (dot * dot));
        const pos3f o(origin + (direction * (distance + hit_result::DISTANCE_MIN)));
        const vec3f d(k < 0.0f ? direction : (direction * eta) - (normal * (eta * dot + ::sqrtf(k))));

        return ray(o, d);
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
           , const float  camera_focus );

    camera ( const pos3f& camera_position
           , const vec3f& camera_direction
           , const vec3f& camera_normal
           , const float  camera_fov
           , const float  camera_dof
           , const float  camera_focus );

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
          , const float  light_power );

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
        , const col3f& sky_ambient );

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
    object();

    virtual ~object() = default;

    virtual bool hit(const ray&, hit_result&) const = 0;

    void set_color0(const col3f& color0)
    {
        _color0 = color0;
    }

    void set_color1(const col3f& color1)
    {
        _color1 = color1;
    }

    void set_color2(const col3f& color2)
    {
        _color2 = color2;
    }

    void set_reflect(const float reflect)
    {
        _reflect = reflect;
    }

    void set_refract(const float refract)
    {
        _refract = refract;
    }

    void set_eta(const float eta)
    {
        _eta = eta;
    }

    void set_specular(const float specular)
    {
        _specular = specular;
    }

    using shared_ptr = std::shared_ptr<object>;
    using vector     = std::vector<shared_ptr>;

protected:
    col3f _color0;
    col3f _color1;
    col3f _color2;
    float _reflect;
    float _refract;
    float _eta;
    float _specular;
};

}

// ---------------------------------------------------------------------------
// rt::plane
// ---------------------------------------------------------------------------

namespace rt {

class plane final
    : public object
{
public:
    plane ( const pos3f& plane_position
          , const vec3f& plane_normal
          , const float  plane_scale );

    virtual ~plane() = default;

    virtual bool hit(const ray&, hit_result&) const override;

protected:
    pos3f _position;
    vec3f _normal;
    float _scale;
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
    sphere ( const pos3f& sphere_position
           , const float  sphere_radius );

    virtual ~sphere() = default;

    virtual bool hit(const ray&, hit_result&) const override;

protected:
    pos3f _position;
    float _radius;
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
          , const sky&    scene_sky );

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
// rt::raytracer
// ---------------------------------------------------------------------------

namespace rt {

class raytracer
{
public:
    raytracer(const scene&);

    virtual ~raytracer() = default;

    col3f trace(const ray&, const int depth);

    bool hit(const ray&, hit_result& result);

    double random1()
    {
        return _random1();
    }

    double random2()
    {
        return _random2();
    }

protected:
    const scene&     _scene;
    base::randomizer _random1;
    base::randomizer _random2;
};

}

// ---------------------------------------------------------------------------
// rt::renderer
// ---------------------------------------------------------------------------

namespace rt {

class renderer
{
public:
    renderer(const scene&);

    virtual ~renderer() = default;

    void render ( ppm::writer& output
                , const int    samples
                , const int    recursions
                , const int    threads );

protected:
    const scene&             _scene;
    std::mutex               _mutex;
    std::queue<rec4i>        _tiles;
    std::vector<std::thread> _threads;

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

    void initialize_aek();

    void initialize_ponceto();

    void initialize_smiley();

    void initialize_simple();

    void initialize_spheres();

    std::shared_ptr<rt::scene> build();

    void build_aek(rt::scene&);

    void build_ponceto(rt::scene&);

    void build_smiley(rt::scene&);

    void build_simple(rt::scene&);

    void build_spheres(rt::scene&);

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
    float       _sphere_eta;
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
    int         _threads;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __BUSINESS_CARD_RAYTRACER_H__ */
