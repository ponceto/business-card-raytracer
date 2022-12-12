/*
 * The Business Card Raytracer
 *
 * Original author: Andrew Kensler
 *
 * Refactored with love by Olivier Poncet
 */
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <ctime>
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <chrono>
#include <memory>
#include <random>
#include <thread>
#include <iostream>
#include <stdexcept>
#include "card.h"

// ---------------------------------------------------------------------------
// countof: return the number of elements of an array
// ---------------------------------------------------------------------------

#ifndef countof
#define countof(array) (sizeof(array) / sizeof(array[0]))
#endif

// ---------------------------------------------------------------------------
// base::ramdom
// ---------------------------------------------------------------------------

namespace base {

randomizer::randomizer(const float min, const float max)
    : _generator(::time(nullptr))
    , _distributor(min, max)
{
}

}

// ---------------------------------------------------------------------------
// base::console
// ---------------------------------------------------------------------------

namespace base {

console::console ( std::istream& cin
                 , std::ostream& cout
                 , std::ostream& cerr )
    : _cin(cin)
    , _cout(cout)
    , _cerr(cerr)
{
}

}

// ---------------------------------------------------------------------------
// base::program
// ---------------------------------------------------------------------------

namespace base {

program::program(int argc, char* argv[])
    : _arglist(argv, argv + argc)
{
}

}

// ---------------------------------------------------------------------------
// ppm::stream
// ---------------------------------------------------------------------------

namespace ppm {

stream::stream(const std::string& filename)
    : _filename(filename)
    , _stream(nullptr)
    , _buffer(nullptr)
    , _length(0)
    , _width(0)
    , _height(0)
    , _maxval(0)
{
}

stream::~stream()
{
    if(_buffer != nullptr) {
        _buffer = (delete[] _buffer, nullptr);
        _length = 0;
    }
    if(_stream != nullptr) {
        _stream = (static_cast<void>(::fclose(_stream)), nullptr);
    }
}

}

// ---------------------------------------------------------------------------
// ppm::reader
// ---------------------------------------------------------------------------

namespace ppm {

reader::reader(const std::string& filename)
    : stream(filename)
{
}

void reader::open(int& width, int& height, int& maxval)
{
    throw std::runtime_error(std::string("ppm::reader is unable to open") + ',' + ' ' + "not implemented");
}

void reader::fetch()
{
    throw std::runtime_error(std::string("ppm::reader is unable to fetch") + ',' + ' ' + "not implemented");
}

void reader::close()
{
    throw std::runtime_error(std::string("ppm::reader is unable to close") + ',' + ' ' + "not implemented");
}

}

// ---------------------------------------------------------------------------
// ppm::writer
// ---------------------------------------------------------------------------

namespace ppm {

writer::writer(const std::string& filename)
    : stream(filename)
{
}

void writer::open(int width, int height, int maxval)
{
    auto do_check = [&]() -> void
    {
        if(_stream != nullptr) {
            throw std::runtime_error(std::string("ppm::writer is unable to open") + ',' + ' ' + "file is already opened");
        }
        if(_buffer != nullptr) {
            throw std::runtime_error(std::string("ppm::writer is unable to open") + ',' + ' ' + "buffer is already allocated");
        }
        if(_length != 0) {
            throw std::runtime_error(std::string("ppm::writer is unable to open") + ',' + ' ' + "length is already set");
        }
        if(_width > 0) {
            throw std::runtime_error(std::string("ppm::writer is unable to open") + ',' + ' ' + "width is already set");
        }
        if(_height > 0) {
            throw std::runtime_error(std::string("ppm::writer is unable to open") + ',' + ' ' + "height is already set");
        }
        if(_maxval > 0) {
            throw std::runtime_error(std::string("ppm::writer is unable to open") + ',' + ' ' + "maxval is already set");
        }
        if(width <= 0) {
            throw std::runtime_error(std::string("ppm::writer is unable to open") + ',' + ' ' + "invalid width");
        }
        if(height <= 0) {
            throw std::runtime_error(std::string("ppm::writer is unable to open") + ',' + ' ' + "invalid height");
        }
        if(maxval != 255) {
            throw std::runtime_error(std::string("ppm::writer is unable to open") + ',' + ' ' + "invalid maxval");
        }
    };

    auto do_setup = [&]() -> void
    {
        _width  = width;
        _height = height;
        _maxval = maxval;
    };

    auto do_open = [&]() -> void
    {
        if((_stream = ::fopen(_filename.c_str(), "w")) == nullptr) {
            throw std::runtime_error(std::string("ppm::writer is unable to open") + ',' + ' ' + '<' + _filename + '>');
        }
        if((_buffer = new uint8_t[_length = (_height * (_width * 3))]) == nullptr) {
            throw std::runtime_error(std::string("ppm::writer is unable to open") + ',' + ' ' + "error while allocating buffer");
        }
        if(::fprintf(_stream, "P6\n%d %d\n%d\n", _width, _height, _maxval) == EOF) {
            throw std::runtime_error(std::string("ppm::writer is unable to open") + ',' + ' ' + "error while writing");
        }
    };

    auto execute = [&]() -> void
    {
        do_check();
        do_setup();
        do_open();
    };

    return execute();
}

void writer::store()
{
    auto do_check = [&]() -> void
    {
        if(_stream == nullptr) {
            throw std::runtime_error(std::string("ppm::writer is unable to store") + ',' + ' ' + "file is not opened");
        }
        if(_buffer == nullptr) {
            throw std::runtime_error(std::string("ppm::writer is unable to store") + ',' + ' ' + "buffer is not allocated");
        }
    };

    auto do_store = [&]() -> void
    {
        if(::fwrite(_buffer, sizeof(uint8_t), _length, _stream) != _length) {
            throw std::runtime_error(std::string("ppm::writer is unable to store") + ',' + ' ' + "error while writing");
        }
    };

    auto execute = [&]() -> void
    {
        do_check();
        do_store();
    };

    return execute();
}

void writer::close()
{
    auto do_check = [&]() -> void
    {
        if(_stream == nullptr) {
            throw std::runtime_error(std::string("ppm::writer is unable to close") + ',' + ' ' + "file is not opened");
        }
        if(_buffer == nullptr) {
            throw std::runtime_error(std::string("ppm::writer is unable to close") + ',' + ' ' + "buffer is not allocated");
        }
    };

    auto do_close = [&]() -> void
    {
        if(_buffer != nullptr) {
            _buffer = (delete[] _buffer, nullptr);
            _length = 0;
        }
        if(_stream != nullptr) {
            _stream = (static_cast<void>(::fclose(_stream)), nullptr);
        }
    };

    auto execute = [&]() -> void
    {
        do_check();
        do_close();
    };

    return execute();
}

}

// ---------------------------------------------------------------------------
// rt::hit_result
// ---------------------------------------------------------------------------

namespace rt {

}

// ---------------------------------------------------------------------------
// rt::ray
// ---------------------------------------------------------------------------

namespace rt {

}

// ---------------------------------------------------------------------------
// rt::camera
// ---------------------------------------------------------------------------

namespace rt {

camera::camera ( const pos3f& camera_position
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

camera::camera ( const pos3f& camera_position
               , const vec3f& camera_direction
               , const vec3f& camera_normal
               , const float  camera_fov
               , const float  camera_dof
               , const float  camera_focus )
    : position(camera_position)
    , direction(camera_direction, true)
    , normal(camera_normal, true)
    , fov(camera_fov)
    , dof(camera_dof)
    , focus(camera_focus)
{
}

}

// ---------------------------------------------------------------------------
// rt::light
// ---------------------------------------------------------------------------

namespace rt {

light::light ( const pos3f& light_position
             , const col3f& light_color
             , const float  light_power )
    : position(light_position)
    , color(light_color)
    , power(light_power)
{
}

}

// ---------------------------------------------------------------------------
// rt::sky
// ---------------------------------------------------------------------------

namespace rt {

sky::sky ( const col3f& sky_color
         , const col3f& sky_ambient )
         : color(sky_color)
         , ambient(sky_ambient)
{
}

}

// ---------------------------------------------------------------------------
// rt::object
// ---------------------------------------------------------------------------

namespace rt {

object::object()
    : _color0(0.5f, 0.5f, 0.5f)
    , _color1(1.0f, 0.3f, 0.3f)
    , _color2(1.0f, 1.0f, 1.0f)
    , _reflect(0.0f)
    , _refract(0.0f)
    , _eta(1.0f)
    , _specular(0.0f)
{
}

}

// ---------------------------------------------------------------------------
// rt::floor
// ---------------------------------------------------------------------------

namespace rt {

floor::floor ( const pos3f& floor_position
             , const vec3f& floor_normal
             , const float  floor_scale )
    : object()
    , _position(floor_position)
    , _normal(floor_normal, true)
    , _scale(floor_scale)
{
}

bool floor::hit(const ray& ray, hit_result& result) const
{
    auto color = [&]() -> const col3f&
    {
        const float x = ::ceilf(result.position.x * _scale);
        const float y = ::ceilf(result.position.y * _scale);
        const int   c = static_cast<int>(x + y) & 1;

        return (c != 0 ? _color1 : _color2);
    };

    constexpr float distance_min = hit_result::DISTANCE_MIN;
    const     float distance_max = result.distance;
    const     float distance_hit = -ray.origin.z / ray.direction.z;
    if((distance_hit > distance_min) && (distance_hit < distance_max)) {
        const vec3f length((ray.direction * distance_hit));
        result.distance = distance_hit;
        result.position = pos3f(ray.origin + length);
        result.normal   = _normal;
        result.color    = color();
        result.reflect  = _reflect;
        result.refract  = _refract;
        result.eta      = _eta;
        result.specular = _specular;
        return true;
    }
    return false;
}

}

// ---------------------------------------------------------------------------
// rt::sphere
// ---------------------------------------------------------------------------

namespace rt {

sphere::sphere ( const pos3f& sphere_position
               , const float  sphere_radius )
    : object()
    , _position(sphere_position)
    , _radius(sphere_radius)
{
}

bool sphere::hit(const ray& ray, hit_result& result) const
{
    const vec3f oc(pos3f::difference(ray.origin, _position));
#if 0
    /*
     * the complete analytic version
     */
    const float a = vec3f::dot(ray.direction, ray.direction);
    const float b = 2.0f * vec3f::dot(oc, ray.direction);
    const float c = vec3f::dot(oc, oc) - (_radius * _radius);
    const float delta = ((b * b) - (4.0f * a * c));
    if(delta > 0.0f) {
        constexpr float distance_min = hit_result::DISTANCE_MIN;
        const     float distance_max = result.distance;
        const     float distance_hit = ((-b - ::sqrtf(delta)) / (2.0f * a));
        if((distance_hit > distance_min) && (distance_hit < distance_max)) {
            const vec3f length((ray.direction * distance_hit));
            result.distance = distance_hit;
            result.position = pos3f(ray.origin + length);
            result.normal   = vec3f(oc + length, true);
            result.color    = _color0;
            result.reflect  = _reflect;
            result.refract  = _refract;
            result.eta      = _eta;
            result.specular = _specular;
            return true;
        }
    }
#else
    /*
     * the simplified analytic version
     */
    const float b = vec3f::dot(oc, ray.direction);
    const float c = vec3f::dot(oc, oc) - (_radius * _radius);
    const float delta = ((b * b) - c);
    if(delta > 0.0f) {
        constexpr float distance_min = hit_result::DISTANCE_MIN;
        const     float distance_max = result.distance;
        const     float distance_hit = (-b - ::sqrtf(delta));
        if((distance_hit > distance_min) && (distance_hit < distance_max)) {
            const vec3f length((ray.direction * distance_hit));
            result.distance = distance_hit;
            result.position = pos3f(ray.origin + length);
            result.normal   = vec3f(oc + length, true);
            result.color    = _color0;
            result.reflect  = _reflect;
            result.refract  = _refract;
            result.eta      = _eta;
            result.specular = _specular;
            return true;
        }
    }
#endif
    return false;
}

}

// ---------------------------------------------------------------------------
// rt::scene
// ---------------------------------------------------------------------------

namespace rt {

scene::scene ( const camera& scene_camera
             , const light&  scene_light
             , const sky&    scene_sky )
    : _camera(scene_camera)
    , _light(scene_light)
    , _sky(scene_sky)
    , _objects()
{
}

}

// ---------------------------------------------------------------------------
// rt::raytracer
// ---------------------------------------------------------------------------

namespace rt {

raytracer::raytracer(const scene& scene)
    : _scene(scene)
    , _random1(-0.50f, +0.50f)
    , _random2(-0.75f, +0.75f)
{
}

bool raytracer::hit(const ray& ray, hit_result& result)
{
    bool status = false;

    const auto& objects = _scene.get_objects();
    for(auto& object : objects) {
        status |= object->hit(ray, result);
    }

    return status;
}

col3f raytracer::trace(const rt::ray& ray, const int recursion)
{
    const rt::light& light = _scene.get_light();
    const rt::sky&   sky   = _scene.get_sky();

    if(recursion <= 0) {
        return sky.ambient;
    }

    rt::hit_result result;
    if(hit(ray, result) == false) {
        return sky.color * ::powf(1.0f - ray.direction.z, 4.0f);
    }

    const pos3f light_pos ( (light.position.x + _random2())
                          , (light.position.y + _random2())
                          , (light.position.z + _random2()) );

    const rt::ray light_ray(result.position, pos3f::difference(light_pos, result.position));

    const rt::ray reflected_ray(ray.reflect(result.distance, result.normal));

    const rt::ray refracted_ray(ray.refract(result.distance, result.normal, result.eta));

    const float light_distance(vec3f::length(pos3f::difference(light.position, result.position)));

    float diffusion = vec3f::dot(light_ray.direction, result.normal);

    if(diffusion < 0.0f) {
        diffusion = 0.0f;
    }

    /* cast_shadows */ {
        rt::hit_result dummy;
        if(hit(light_ray, dummy) != false) {
            diffusion = 0.0f;
        }
    }

    rt::col3f   final_color;
    rt::col3f   light_color(light.color * (1.0f / ::sqrtf(light_distance / light.power)));
    const float specular_factor = result.specular;
    const float refract_factor  = result.refract;
    const float reflect_factor  = result.reflect;
    const float diffuse_factor  = (1.0f - (reflect_factor + refract_factor)) * diffusion;
    const float ambient_factor  = (1.0f - (reflect_factor + refract_factor)) * 1.0f;

    auto ambient_color = [&]() -> void
    {
        if(ambient_factor > 0.0f) {
            final_color += ((result.color * sky.ambient) * ambient_factor);
        }
    };

    auto diffuse_color = [&]() -> void
    {
        if(diffuse_factor > 0.0f) {
            final_color += ((result.color * light_color) * diffuse_factor);
        }
    };

    auto reflect_color = [&]() -> void
    {
        if(reflect_factor > 0.0f) {
            final_color += (trace(reflected_ray, (recursion - 1)) * reflect_factor);
        }
    };

    auto refract_color = [&]() -> void
    {
        if(refract_factor > 0.0f) {
            final_color += (trace(refracted_ray, (recursion - 1)) * refract_factor);
        }
    };

    auto specular_color = [&]() -> void
    {
        if(specular_factor > 0.0f) {
            const float phong = ::powf(vec3f::dot(light_ray.direction, reflected_ray.direction) * (diffusion > 0.0f), specular_factor);
            final_color += (light_color * phong);
        }
    };

    ambient_color();
    diffuse_color();
    reflect_color();
    refract_color();
    specular_color();

    return final_color;
}

}

// ---------------------------------------------------------------------------
// rt::renderer
// ---------------------------------------------------------------------------

namespace rt {

renderer::renderer(const scene& scene)
    : _scene(scene)
{
}

void renderer::render(ppm::writer& output, const int w, const int h, const int samples, const int recursions, const int threads)
{
    const rt::camera& camera(_scene.get_camera());
    const int   half_w = (w / 2);
    const int   half_h = (h / 2);
    const float scale  = 255.0f / static_cast<float>(samples);
    const float fov    = (camera.fov * 512.0f) / static_cast<float>(h < w ? h : w);
    const vec3f right (vec3f::normalize(vec3f::cross(camera.direction, camera.normal)) * fov);
    const vec3f down  (vec3f::normalize(vec3f::cross(camera.direction, right        )) * fov);
    const vec3f corner(camera.direction - (right + down) * 0.5f);

    auto clamp = [](const int val) -> uint8_t
    {
        constexpr int min = 0;
        constexpr int max = 255;

        if(val < min) {
            return min;
        }
        if(val > max) {
            return max;
        }
        return val;
    };

    struct tile_record
    {
        tile_record()
            : x()
            , y()
            , w()
            , h()
        {
        }

        tile_record(int tile_x, int tile_y, int tile_w, int tile_h)
            : x(tile_x)
            , y(tile_y)
            , w(tile_w)
            , h(tile_h)
        {
        }

        int x;
        int y;
        int w;
        int h;
    };

    std::mutex               mutex;
    std::queue<tile_record>  render_tiles;
    std::vector<std::thread> render_threads;

    auto pop_tile = [&](tile_record& tile) -> bool
    {
        const std::lock_guard<std::mutex> lock(mutex);

        if(render_tiles.empty()) {
            return false;
        }
        else {
            tile = render_tiles.front();
            render_tiles.pop();
        }
        return true;
    };

    auto create_tiles = [&]() -> void
    {
        std::lock_guard<std::mutex> lock(mutex);

        const int tile_size = 64;
        for(int y = 0; y < h; y += tile_size) {
            for(int x = 0; x < w; x += tile_size) {
                tile_record tile(x, y, tile_size, tile_size);
                if((tile.x + tile.w) >= w) {
                    tile.w = w - tile.x;
                }
                if((tile.y + tile.h) >= h) {
                    tile.h = h - tile.y;
                }
                render_tiles.push(tile);
            }
        }
    };

    auto render_tile = [&](const tile_record& tile) -> void
    {
        rt::raytracer raytracer(_scene);
        const int x1 = tile.x;
        const int y1 = tile.y;
        const int x2 = tile.x + tile.w;
        const int y2 = tile.y + tile.h;
        const int row_stride = (w * 3);
        uint8_t*  buffer = output.data() + ((tile.y * row_stride) + (tile.x * 3));
        for(int y = y1; y < y2; ++y) {
            uint8_t* bufptr = buffer;
            for(int x = x1; x < x2; ++x) {
                col3f color;
                for(int count = samples; count != 0; --count) {
                    const vec3f lens ( ( (right * raytracer.random1())
                                       + ( down * raytracer.random1()) ) * camera.dof );

                    const vec3f dir ( (right * (static_cast<float>(x - half_w + 1) + raytracer.random1()))
                                    + ( down * (static_cast<float>(y - half_h + 1) + raytracer.random1()))
                                    + corner );

                    const ray primary_ray(camera.position + lens, (dir * camera.focus - lens));

                    color += raytracer.trace(primary_ray, recursions);
                }
                color *= scale;
                *bufptr++ = clamp(static_cast<int>(color.r));
                *bufptr++ = clamp(static_cast<int>(color.g));
                *bufptr++ = clamp(static_cast<int>(color.b));
            }
            buffer += row_stride;
        }
    };

    auto render_loop = [&]() -> void
    {
        tile_record tile;
        while(pop_tile(tile) != false) {
            render_tile(tile);
        }
    };

    auto start_threads = [&]() -> void
    {
        for(int thread = 0; thread < threads; ++thread) {
            render_threads.push_back(std::thread(render_loop));
        }
    };

    auto join_threads = [&]() -> void
    {
        for(auto& render_thread : render_threads) {
            render_thread.join();
        }
    };

    auto execute = [&]() -> void
    {
        create_tiles();
        start_threads();
        join_threads();
    };

    return execute();
}

}

// ---------------------------------------------------------------------------
// card::scene_factory
// ---------------------------------------------------------------------------

namespace card {

scene_factory::scene_factory(const std::string& scene_name)
    : _name(scene_name)
    , _world()
    , _camera_position()
    , _camera_target()
    , _camera_top()
    , _camera_fov()
    , _camera_dof()
    , _camera_focus()
    , _light_position()
    , _light_color()
    , _light_power()
    , _sky_color()
    , _sky_ambient()
    , _floor_position()
    , _floor_normal()
    , _floor_color1()
    , _floor_color2()
    , _floor_scale()
    , _floor_reflect()
    , _floor_refract()
    , _floor_specular()
    , _sphere_radius()
    , _sphere_color()
    , _sphere_reflect()
    , _sphere_refract()
    , _sphere_eta()
    , _sphere_specular()
{
    initialize();
}

std::shared_ptr<rt::scene> scene_factory::create(const std::string& scene_name)
{
    scene_factory sb(scene_name);

    return sb.build();
}

void scene_factory::initialize()
{
    for(auto& world : _world) {
        world = 0;
    }

    _camera_position = rt::pos3f (+3.5f, -5.0f, +1.7f);
    _camera_target   = rt::pos3f (+0.25f, 0.0f, +1.0f);
    _camera_top      = rt::pos3f (+3.5f, -5.0f, +2.7f);
    _camera_fov      = float     (0.002f);
    _camera_dof      = float     (99.0f);
    _camera_focus    = float     (5.0f);
    _light_position  = rt::pos3f (+7.0f, -5.0f, +3.0f);
    _light_color     = rt::col3f (+0.90f, +0.95f, +1.00f);
    _light_power     = float     (20.0f);
    _sky_color       = rt::col3f (+0.25f, +0.75f, +1.00f);
    _sky_ambient     = rt::col3f (+0.35f, +0.35f, +0.35f);
    _floor_position  = rt::pos3f (0.0f, 0.0f, 0.0f);
    _floor_normal    = rt::vec3f (0.0f, 0.0f, 1.0f);
    _floor_color1    = rt::col3f (+1.00f, +0.30f, +0.30f);
    _floor_color2    = rt::col3f (+1.00f, +1.00f, +1.00f);
    _floor_scale     = float     (1.0f);
    _floor_reflect   = float     (0.2f);
    _floor_refract   = float     (0.0f);
    _floor_specular  = float     (0.0f);
    _sphere_radius   = float     (1.0f);
    _sphere_color    = rt::col3f (0.20f, 0.25f, 0.15f);
    _sphere_reflect  = float     (0.5f);
    _sphere_refract  = float     (0.0f);
    _sphere_eta      = float     (1.0f);
    _sphere_specular = float     (50.0f);

    if(_name == "aek") {
        return initialize_aek();
    }
    if(_name == "ponceto") {
        return initialize_ponceto();
    }
    if(_name == "smiley") {
        return initialize_smiley();
    }
    if(_name == "simple") {
        return initialize_simple();
    }
    if(_name == "spheres") {
        return initialize_spheres();
    }
    throw std::runtime_error(std::string("invalid scene") + ' ' + '<' + _name + '>');
}

void scene_factory::initialize_aek()
{
    int row = 0;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000010000000000;
    _world[row++] = 0b00000000000000000000010000000000;
    _world[row++] = 0b00000000111000011100010000000000;
    _world[row++] = 0b00000000000100100010010001000000;
    _world[row++] = 0b00000000000100100010010010000000;
    _world[row++] = 0b00000000111100111110010100000000;
    _world[row++] = 0b00000001000100100000011000000000;
    _world[row++] = 0b00000001000100100000010100000000;
    _world[row++] = 0b00000000111100011100010010000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;

    _camera_position = rt::pos3f (-7.0f, -16.0f,  +8.0f);
    _camera_target   = rt::pos3f (-1.0f,   0.0f,  +8.0f);
    _camera_top      = rt::pos3f (-7.0f, -16.0f,  +9.0f);
    _camera_fov      = float     (0.002f);
    _camera_dof      = float     ( 99.0f);
    _camera_focus    = float     ( 16.0f);
    _light_position  = rt::pos3f ( +0.5f,  -9.5f, +16.0f);
    _light_color     = rt::col3f (+1.00f, +1.00f, +1.00f);
    _light_power     = float     (+15.0f);
    _sky_color       = rt::col3f (+0.70f, +0.60f, +1.00f);
    _sky_ambient     = rt::col3f (+0.35f, +0.35f, +0.35f);
    _floor_scale     = float     (0.2f);
    _floor_reflect   = float     (0.0f);
    _sphere_radius   = float     (1.0f);
    _sphere_color    = rt::col3f (+0.0f, +0.0f, +0.0f);
    _sphere_reflect  = float     (0.7f);
    _sphere_refract  = float     (0.0f);
    _sphere_specular = float     (99.0f);
}

void scene_factory::initialize_ponceto()
{
    int row = 0;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b11100011001001001110111101110110;
    _world[row++] = 0b10010100101001010000100000101001;
    _world[row++] = 0b10010100101101010000100000101001;
    _world[row++] = 0b11100100101101010000111000101001;
    _world[row++] = 0b10000100101011010000100000101001;
    _world[row++] = 0b10000100101011010000100000101001;
    _world[row++] = 0b10000011001001001110111100100110;

    _camera_position = rt::pos3f (-19.0f, -19.0f, +15.0f);
    _camera_target   = rt::pos3f ( -5.0f,   0.0f,  +7.0f);
    _camera_top      = rt::pos3f (-19.0f, -19.0f, +16.0f);
    _camera_fov      = float     (0.002f);
    _camera_dof      = float     (256.0f);
    _camera_focus    = float     ( 25.0f);
    _light_position  = rt::pos3f ( +5.0f, -15.0f, +15.0f);
    _light_color     = rt::col3f ( +0.5f,  +0.7f,  +0.9f);
    _light_power     = float     (+50.0f);
    _sky_color       = rt::col3f (+0.70f, +0.60f, +1.00f);
    _sky_ambient     = rt::col3f (+0.50f, +0.50f, +0.50f);
    _floor_scale     = float     (0.2f);
    _floor_reflect   = float     (0.3f);
    _sphere_radius   = float     (0.75f);
    _sphere_color    = rt::col3f (+1.0f, +0.8f, +0.0f);
    _sphere_reflect  = float     (0.7f);
    _sphere_refract  = float     (0.0f);
    _sphere_specular = float     (99.0f);
}

void scene_factory::initialize_smiley()
{
    int row = 0;
    _world[row++] = 0b00000000000001111110000000000000;
    _world[row++] = 0b00000000000110000001100000000000;
    _world[row++] = 0b00000000001000000000010000000000;
    _world[row++] = 0b00000000010000000000001000000000;
    _world[row++] = 0b00000000010001100110001000000000;
    _world[row++] = 0b00000000100001100110000100000000;
    _world[row++] = 0b00000000100000000000000100000000;
    _world[row++] = 0b00000000100000000000000100000000;
    _world[row++] = 0b00000000100000000000000100000000;
    _world[row++] = 0b00000000100100000000100100000000;
    _world[row++] = 0b00000000100010000001000100000000;
    _world[row++] = 0b00000000010001111110001000000000;
    _world[row++] = 0b00000000010000000000001000000000;
    _world[row++] = 0b00000000001000000000010000000000;
    _world[row++] = 0b00000000000110000001100000000000;
    _world[row++] = 0b00000000000001111110000000000000;

    _camera_position = rt::pos3f (+19.0f, -17.0f, +15.0f);
    _camera_target   = rt::pos3f ( +2.0f,   0.0f, +8.0f);
    _camera_top      = rt::pos3f (+19.0f, -17.0f, +16.0f);
    _camera_fov      = float     (0.002f);
    _camera_dof      = float     (384.0f);
    _camera_focus    = float     ( 24.0f);
    _light_position  = rt::pos3f ( -5.0f, -15.0f, +16.0f);
    _light_color     = rt::col3f ( +0.7f,  +0.8f,  +0.9f);
    _light_power     = float     (+50.0f);
    _sky_color       = rt::col3f (+0.50f, +0.40f, +1.00f);
    _sky_ambient     = rt::col3f (+0.50f, +0.50f, +0.50f);
    _floor_scale     = float     (0.3f);
    _floor_reflect   = float     (0.3f);
    _sphere_radius   = float     (1.0f);
    _sphere_color    = rt::col3f (+0.1f, +0.2f, +0.3f);
    _sphere_reflect  = float     (0.7f);
    _sphere_refract  = float     (0.0f);
    _sphere_specular = float     (99.0f);
}

void scene_factory::initialize_simple()
{
    int row = 0;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000000000000000000000;
    _world[row++] = 0b00000000000000100100000000000000;

    _floor_scale     = float     (0.7f);
    _floor_reflect   = float     (0.3f);
    _sphere_radius   = float     (1.0f);
    _sphere_color    = rt::col3f (0.15f, 0.35f, 0.25f);
    _sphere_reflect  = float     (0.20f);
    _sphere_refract  = float     (0.70f);
    _sphere_eta      = float     (0.70f);
    _sphere_specular = float     (90.0f);
}

void scene_factory::initialize_spheres()
{
    _camera_position = rt::pos3f (+4.0f, -4.0f, +2.0f);
    _camera_target   = rt::pos3f (+0.0f, +0.0f, +1.0f);
    _camera_top      = rt::pos3f (+4.0f, -4.0f, +3.0f);
    _camera_fov      = float     (0.002f);
    _camera_dof      = float     (45.0f);
    _camera_focus    = float     (5.0f);
    _light_position  = rt::pos3f (-3.0f, -7.0f, +5.0f);
    _light_color     = rt::col3f (+1.00f, +1.00f, +1.00f);
    _light_power     = float     (15.0f);
    _floor_color1    = rt::col3f (+0.10f, +0.10f, +0.10f);
    _floor_color2    = rt::col3f (+0.90f, +0.90f, +0.90f);
}

std::shared_ptr<rt::scene> scene_factory::build()
{
    auto add_floor = [&](rt::scene& scene) -> void
    {
        std::shared_ptr<rt::floor> obj = std::make_shared<rt::floor>(_floor_position, _floor_normal, _floor_scale);
        obj->set_color1( _floor_color1);
        obj->set_color2( _floor_color2);
        obj->set_reflect( _floor_reflect);
        obj->set_refract( _floor_refract);
        obj->set_specular( _floor_specular);

        scene.add(obj);
    };

    auto add_spheres = [&](rt::scene& scene) -> void
    {
        constexpr int cols         = 32;
        constexpr int rows         = countof(_world);
        constexpr float col_offset = -16.0f;
        constexpr float row_offset =   0.0f;

        for(int row = 0; row < rows; ++row) {
            uint32_t constexpr lsb = (1 << 0);
            uint32_t           val = _world[row];
            if(val == 0) {
                continue;
            }
            for(int col = 0; col < cols; ++col) {
                if(val & lsb) {
                    const float x = static_cast<float>(cols - col) + col_offset;
                    const float y = static_cast<float>(0);
                    const float z = static_cast<float>(rows - row) + row_offset;
                    const float r = _sphere_radius;
                    std::shared_ptr<rt::sphere> obj = std::make_shared<rt::sphere>(rt::pos3f(x, y, z), r);
                    obj->set_color0(_sphere_color);
                    obj->set_reflect(_sphere_reflect);
                    obj->set_refract(_sphere_refract);
                    obj->set_eta(_sphere_eta);
                    obj->set_specular(_sphere_specular);

                    scene.add(obj);
                }
                if((val >>= 1) == 0) {
                    break;
                }
            }
        }
    };

    const rt::camera camera ( _camera_position
                            , _camera_target
                            , _camera_top
                            , _camera_fov
                            , _camera_dof
                            , _camera_focus );

    const rt::light light   ( _light_position
                            , _light_color
                            , _light_power );

    const rt::sky sky       ( _sky_color
                            , _sky_ambient );

    const std::shared_ptr<rt::scene> scene(std::make_shared<rt::scene>(camera, light, sky));

    add_floor(*scene);

    add_spheres(*scene);

    if(_name == "aek") {
        build_aek(*scene);
    }
    if(_name == "ponceto") {
        build_ponceto(*scene);
    }
    if(_name == "smiley") {
        build_smiley(*scene);
    }
    if(_name == "simple") {
        build_simple(*scene);
    }
    if(_name == "spheres") {
        build_spheres(*scene);
    }
    return scene;
}

void scene_factory::build_aek(rt::scene& scene)
{
}

void scene_factory::build_ponceto(rt::scene& scene)
{
}

void scene_factory::build_smiley(rt::scene& scene)
{
}

void scene_factory::build_simple(rt::scene& scene)
{
}

void scene_factory::build_spheres(rt::scene& scene)
{
    auto add_sphere = [&] ( const rt::pos3f& position
                          , const float      radius
                          , const rt::col3f& color
                          , const float      reflect
                          , const float      refract
                          , const float      eta
                          , const float      specular )
    {
        std::shared_ptr<rt::sphere> obj = std::make_shared<rt::sphere>(position, 1.0f);
        obj->set_color0(color);
        obj->set_reflect(reflect);
        obj->set_refract(refract);
        obj->set_eta(eta);
        obj->set_specular(specular);
        scene.add(obj);
    };

    add_sphere(rt::pos3f(-1.5f, +1.5f, +1.0f), 1.0f, rt::col3f(0.0f, 0.0f, 0.5f), 0.5f, 0.0f, 1.00f, 45.0f);
    add_sphere(rt::pos3f(+0.0f, -1.5f, +1.0f), 1.0f, rt::col3f(0.8f, 0.8f, 0.0f), 0.1f, 0.3f, 0.91f, 90.0f);
    add_sphere(rt::pos3f(+1.5f, +1.5f, +1.0f), 1.0f, rt::col3f(0.0f, 0.8f, 0.0f), 0.3f, 0.3f, 0.50f, 75.0f);
}

}

// ---------------------------------------------------------------------------
// card::generator
// ---------------------------------------------------------------------------

namespace card {

generator::generator(int argc, char* argv[])
    : base::console(std::cin, std::cout, std::cerr)
    , base::program(argc, argv)
    , _program("card")
    , _output("card.ppm")
    , _scene("aek")
    , _card_w(512)
    , _card_h(512)
    , _samples(64)
    , _recursions(8)
    , _threads(1)
{
}

void generator::main()
{
    base::profiler profiler("raytrace");

    auto check = [&]() -> void
    {
        if(_output.empty()) {
            throw std::runtime_error("invalid filename");
        }
        if(_card_w <= 0) {
            throw std::runtime_error("invalid card width");
        }
        if(_card_h <= 0) {
            throw std::runtime_error("invalid card height");
        }
    };

    auto begin = [&]() -> void
    {
        cout() << profiler.name() << ':' << ' ' << "rendering ..." << std::endl;
        profiler.reset();
    };

    auto end = [&]() -> void
    {
        cout() << profiler.name() << ':' << ' ' << profiler.elapsed() << 's' << std::endl;
        profiler.reset();
    };

    auto render = [&]() -> void
    {
        ppm::writer output(_output);
        const std::shared_ptr<rt::scene> scene(scene_factory::create(_scene));
        rt::renderer renderer(*scene);

        output.open(_card_w, _card_h, 255);
        begin();
        renderer.render(output, _card_w, _card_h, _samples, _recursions, _threads);
        end();
        output.store();
        output.close();
    };

    auto execute = [&]() -> void
    {
        check();
        render();
    };

    return execute();
}

bool generator::parse()
{
    auto has_option = [](const std::string& argument, const char* prefix) -> bool
    {
        const size_t pos = 0;
        const size_t len = ::strlen(prefix);
        if(argument.compare(pos, len, prefix) == 0) {
            return true;
        }
        return false;
    };

    auto invalid_argument = [&](const std::string& argument) -> void
    {
        throw std::runtime_error(std::string("invalid argument") + ' ' + '<' + argument + '>');
    };

    auto get_str_val = [](const std::string& argument) -> std::string
    {
        const char* equ = ::strchr(argument.c_str(), '=');
        if(equ != nullptr) {
            return std::string(++equ);
        }
        return std::string();
    };

    auto get_int_val = [](const std::string& argument) -> int
    {
        const char* equ = ::strchr(argument.c_str(), '=');
        if(equ != nullptr) {
            return ::atoi(++equ);
        }
        return 0;
    };

    auto set_program = [&](const std::string& argument) -> void
    {
        const char* sep = ::strrchr(argument.c_str(), '/');
        if(sep != nullptr) {
            _program = ++sep;
        }
    };

    auto set_output = [&](const std::string& argument) -> void
    {
        _output = get_str_val(argument);
    };

    auto set_scene = [&](const std::string& argument) -> void
    {
        _scene = get_str_val(argument);
    };

    auto set_card_w = [&](const std::string& argument) -> void
    {
        _card_w = get_int_val(argument);
        if(_card_w <= 0) {
            invalid_argument(argument);
        }
    };

    auto set_card_h = [&](const std::string& argument) -> void
    {
        _card_h = get_int_val(argument);
        if(_card_h <= 0) {
            invalid_argument(argument);
        }
    };

    auto set_samples = [&](const std::string& argument) -> void
    {
        _samples = get_int_val(argument);
        if(_samples <= 0) {
            invalid_argument(argument);
        }
    };

    auto set_recursions = [&](const std::string& argument) -> void
    {
        _recursions = get_int_val(argument);
        if(_recursions <= 0) {
            invalid_argument(argument);
        }
    };

    auto set_threads = [&](const std::string& argument) -> void
    {
        _threads = get_int_val(argument);
        if(_threads <= 0) {
            invalid_argument(argument);
        }
    };

    auto execute = [&]() -> bool
    {
        int argi = 0;
        for(auto& argument : _arglist) {
            if(argi == 0) {
                set_program(argument);
            }
            else if(argument == "-h") {
                return false;
            }
            else if(argument == "--help") {
                return false;
            }
            else if(has_option(argument, "--output=")) {
                set_output(argument);
            }
            else if(has_option(argument, "--scene=")) {
                set_scene(argument);
            }
            else if(has_option(argument, "--width=")) {
                set_card_w(argument);
            }
            else if(has_option(argument, "--height=")) {
                set_card_h(argument);
            }
            else if(has_option(argument, "--samples=")) {
                set_samples(argument);
            }
            else if(has_option(argument, "--recursions=")) {
                set_recursions(argument);
            }
            else if(has_option(argument, "--threads=")) {
                set_threads(argument);
            }
            else {
                invalid_argument(argument);
            }
            ++argi;
        }
        return true;
    };

    return execute();
}

void generator::usage()
{
    cout() << "Usage:" << ' ' << _program << ' ' << "[OPTIONS...]"           << std::endl;
    cout() << ""                                                             << std::endl;
    cout() << "The Business Card Raytracer"                                  << std::endl;
    cout() << ""                                                             << std::endl;
    cout() << "Options:"                                                     << std::endl;
    cout() << ""                                                             << std::endl;
    cout() << "    --help                  display this help"                << std::endl;
    cout() << "    --output={path}         the output filename"              << std::endl;
    cout() << "    --scene={scene}         the scene to render"              << std::endl;
    cout() << "    --width={int}           the card width"                   << std::endl;
    cout() << "    --height={int}          the card height"                  << std::endl;
    cout() << "    --samples={int}         samples per pixel"                << std::endl;
    cout() << "    --recursions={int}      maximum recursions level"         << std::endl;
    cout() << "    --threads={int}         number of threads"                << std::endl;
    cout() << ""                                                             << std::endl;
    cout() << "Scenes:"                                                      << std::endl;
    cout() << ""                                                             << std::endl;
    cout() << "    - aek"                                                    << std::endl;
    cout() << "    - ponceto"                                                << std::endl;
    cout() << "    - smiley"                                                 << std::endl;
    cout() << "    - simple"                                                 << std::endl;
    cout() << "    - spheres"                                                << std::endl;
    cout() << ""                                                             << std::endl;
}

}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    card::generator program(argc, argv);

    try {
        if(program.parse()) {
            program.main();
        }
        else {
            program.usage();
        }
    }
    catch(const std::exception& e) {
        const char* what(e.what());
        program.cerr() << "error: " << what << std::endl;
        return EXIT_FAILURE;
    }
    catch(...) {
        const char* what("unhandled exception");
        program.cerr() << "error: " << what << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
