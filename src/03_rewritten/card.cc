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
#include <chrono>
#include <memory>
#include <random>
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
    , _width(0)
    , _height(0)
    , _maxval(0)
{
}

stream::~stream()
{
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

void reader::fetch(int& r, int& g, int& b)
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

void writer::store(int r, int g, int b)
{
    auto clamp = [&](const int val) -> uint8_t
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

    auto do_check = [&]() -> void
    {
        if(_stream == nullptr) {
            throw std::runtime_error(std::string("ppm::writer is unable to store") + ',' + ' ' + "file is not opened");
        }
    };

    auto do_write = [&]() -> void
    {
        const uint8_t buffer[3] = {
            clamp(r), clamp(g), clamp(b)
        };
        if(::fwrite(buffer, sizeof(buffer), 1, _stream) != 1) {
            throw std::runtime_error(std::string("ppm::writer is unable to store") + ',' + ' ' + "error while writing");
        }
    };

    auto execute = [&]() -> void
    {
        do_check();
        do_write();
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
    };

    auto do_close = [&]() -> void
    {
        _stream = (static_cast<void>(::fclose(_stream)), nullptr);
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
// rt::object
// ---------------------------------------------------------------------------

namespace rt {

}

// ---------------------------------------------------------------------------
// rt::floor
// ---------------------------------------------------------------------------

namespace rt {

bool floor::hit(const ray& ray, hit_result& result) const
{
    auto color = [&]() -> const col3f&
    {
        const float x = ::ceilf(result.position.x * scale);
        const float y = ::ceilf(result.position.y * scale);
        const int   c = static_cast<int>(x + y) & 1;

        return (c != 0 ? color1 : color2);
    };

    constexpr float distance_min = hit_result::DISTANCE_MIN;
    const     float distance_max = result.distance;
    const     float distance_hit = -ray.origin.z / ray.direction.z;
    if((distance_hit > distance_min) && (distance_hit < distance_max)) {
        result.distance = distance_hit - distance_min;
        result.position = (ray.origin + (ray.direction * result.distance));
        result.normal   = normal;
        result.color    = color();
        result.refract  = refract;
        result.reflect  = reflect;
        result.specular = specular;
        return true;
    }
    return false;
}

}

// ---------------------------------------------------------------------------
// rt::sphere
// ---------------------------------------------------------------------------

namespace rt {

bool sphere::hit(const ray& ray, hit_result& result) const
{
    const vec3f oc(pos3f::difference(ray.origin, center));
#if 0
    /*
     * the complete analytic version
     */
    const float a = vec3f::dot(ray.direction, ray.direction);
    const float b = 2.0f * vec3f::dot(oc, ray.direction);
    const float c = vec3f::dot(oc, oc) - (radius * radius);
    const float delta = ((b * b) - (4.0 * a * c));
    if(delta > 0.0) {
        constexpr float distance_min = hit_result::DISTANCE_MIN;
        const     float distance_max = result.distance;
        const     float distance_hit = ((-b - ::sqrtf(delta)) / (2.0f * a));
        if((distance_hit > distance_min) && (distance_hit < distance_max)) {
            result.distance = distance_hit - distance_min;
            result.position = (ray.origin + (ray.direction * result.distance));
            result.normal   = vec3f::normalize(oc + ray.direction * result.distance);
            result.color    = color;
            result.refract  = refract;
            result.reflect  = reflect;
            result.specular = specular;
            return true;
        }
    }
#else
    /*
     * the simplified analytic version
     */
    const float b = vec3f::dot(oc, ray.direction);
    const float c = vec3f::dot(oc, oc) - (radius * radius);
    const float delta = ((b * b) - c);
    if(delta > 0.0) {
        constexpr float distance_min = hit_result::DISTANCE_MIN;
        const     float distance_max = result.distance;
        const     float distance_hit = (-b - ::sqrtf(delta));
        if((distance_hit > distance_min) && (distance_hit < distance_max)) {
            result.distance = distance_hit - distance_min;
            result.position = (ray.origin + (ray.direction * result.distance));
            result.normal   = vec3f::normalize(oc + ray.direction * result.distance);
            result.color    = color;
            result.refract  = refract;
            result.reflect  = reflect;
            result.specular = specular;
            return true;
        }
    }
#endif
    return false;
}

}

// ---------------------------------------------------------------------------
// card::raytracer
// ---------------------------------------------------------------------------

namespace card {

raytracer::raytracer(base::console& console, const rt::scene& scene)
    : _console(console)
    , _random1(-0.50, +0.50)
    , _random2(-0.75, +0.75)
    , _scene(scene)
{
}

bool raytracer::hit(const rt::ray& ray, rt::hit_result& result)
{
    bool status = false;

    const auto& objects = _scene.get_objects();
    for(auto& object : objects) {
        status |= object->hit(ray, result);
    }

    return status;
}

rt::col3f raytracer::trace(const rt::ray& ray, const int recursion)
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

    const rt::pos3f light_pos ( (light.position.x + _random2())
                              , (light.position.y + _random2())
                              , (light.position.z + _random2()) );

    const rt::vec3f light_dir(rt::vec3f::normalize(rt::pos3f::difference(light_pos, result.position)));

    const rt::vec3f reflection(ray.direction + result.normal * (rt::vec3f::dot(result.normal, ray.direction) * -2.0f));
    float distance    = rt::vec3f::length(rt::pos3f::difference(light.position, result.position));
    float attenuation = 1.0f / ::sqrtf(distance / light.power);
    float diffusion   = rt::vec3f::dot(light_dir, result.normal);

    /* cast shadows */ {
        if(diffusion < 0.0f) {
            diffusion = 0.0f;
        }
        else {
            rt::hit_result dummy;
            if(hit(rt::ray(result.position, light_dir), dummy) != false) {
                diffusion = 0.0f;
            }
        }
    }

    const float reflect_coef  = result.reflect;
    const float refract_coef  = result.refract;
    const float diffuse_coef  = 1.0f - (reflect_coef + refract_coef);
    const float specular_coef = result.specular;

    rt::col3f diffuse_color;
    rt::col3f reflect_color;
    rt::col3f specular_color;

    if(diffuse_coef > 0.0f) {
        diffuse_color = ((result.color * sky.ambient) + (result.color * light.color * diffusion * attenuation)) * diffuse_coef;
    }

    if(reflect_coef > 0.0f) {
        reflect_color = trace(rt::ray(result.position, reflection), (recursion - 1)) * reflect_coef;
    }

    if(specular_coef > 0.0f) {
        const float phong = ::powf(rt::vec3f::dot(light_dir, reflection) * (diffusion > 0.0f), specular_coef);
        specular_color = (light.color * phong * attenuation);
    }

    return diffuse_color + reflect_color + specular_color;
}

void raytracer::render(ppm::writer& output, const int w, const int h, const int samples, const int recursions)
{
    const rt::camera& camera(_scene.get_camera());
    const int   half_w = (w / 2);
    const int   half_h = (h / 2);
    const float scale  = 255.0f / static_cast<float>(samples);
    const float fov    = (camera.fov * 512.0) / static_cast<float>(h < w ? h : w);
    const rt::vec3f right (rt::vec3f::normalize(rt::vec3f::cross(camera.direction, camera.normal)) * fov);
    const rt::vec3f down  (rt::vec3f::normalize(rt::vec3f::cross(camera.direction, right        )) * fov);
    const rt::vec3f corner(camera.direction - (right + down) * 0.5f);

    for(int y = 0; y < h; ++y) {
        for(int x = 0; x < w; ++x) {
            rt::col3f color;
            for(int count = samples; count != 0; --count) {
                const rt::vec3f lens ( ( (right * _random1())
                                       + ( down * _random1()) ) * camera.dof );

                const rt::vec3f dir ( (right * (static_cast<float>(x - half_w + 1) + _random1()))
                                    + ( down * (static_cast<float>(y - half_h + 1) + _random1()))
                                    + corner );

                const rt::ray ray(camera.position + lens, (dir * camera.focus - lens));

                color += trace(ray, recursions);
            }
            color *= scale;
            output.store ( static_cast<int>(color.r)
                         , static_cast<int>(color.g)
                         , static_cast<int>(color.b) );
        }
    }
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
    initialize_default();

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
    throw std::runtime_error(std::string("invalid scene") + ' ' + '<' + _name + '>');
}

void scene_factory::initialize_default()
{
    for(auto& world : _world) {
        world = 0;
    }

    _camera_position = rt::pos3f (+3.5f, -5.0f, +1.7f);
    _camera_target   = rt::pos3f (+0.25f, 0.0f, +1.0f);
    _camera_top      = rt::pos3f (+3.5f, -5.0f, +2.7f);
    _camera_fov      = float     (0.002f);
    _camera_dof      = float     (99.0);
    _camera_focus    = float     (5.0);
    _light_position  = rt::pos3f (+7.0f, -5.0f, +3.0f);
    _light_color     = rt::col3f (+0.90f, +0.95f, +1.00f);
    _light_power     = float     (20.0f);
    _sky_color       = rt::col3f (+0.25, +0.75f, +1.00f);
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
    _sphere_specular = float     (50.0f);
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
    _sphere_radius   = float     (1.0f);
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
}

std::shared_ptr<rt::scene> scene_factory::build()
{
    auto add_floor = [&](rt::scene& scene) -> void
    {
        std::shared_ptr<rt::floor> obj = std::make_shared<rt::floor> ( _floor_position
                                                                     , _floor_normal
                                                                     , _floor_color1
                                                                     , _floor_color2
                                                                     , _floor_scale );
        obj->reflect  = _floor_reflect;
        obj->refract  = _floor_refract;
        obj->specular = _floor_specular;

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
                    std::shared_ptr<rt::sphere> obj = std::make_shared<rt::sphere> ( rt::pos3f(x, y, z)
                                                                                   , _sphere_color
                                                                                   , r );
                    obj->reflect  = _sphere_reflect;
                    obj->refract  = _sphere_refract;
                    obj->specular = _sphere_specular;

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

    return scene;
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
        card::raytracer raytracer(*this, *scene);

        output.open(_card_w, _card_h, 255);
        begin();
        raytracer.render(output, _card_w, _card_h, _samples, _recursions);
        end();
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
    };

    auto set_card_h = [&](const std::string& argument) -> void
    {
        _card_h = get_int_val(argument);
    };

    auto set_samples = [&](const std::string& argument) -> void
    {
        _samples = get_int_val(argument);
    };

    auto set_recursions = [&](const std::string& argument) -> void
    {
        _recursions = get_int_val(argument);
    };

    auto invalid_argument = [&](const std::string& argument) -> void
    {
        throw std::runtime_error(std::string("invalid argument") + ' ' + '<' + argument + '>');
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
    cout() << ""                                                             << std::endl;
    cout() << "Scenes:"                                                      << std::endl;
    cout() << ""                                                             << std::endl;
    cout() << "    - aek"                                                    << std::endl;
    cout() << "    - ponceto"                                                << std::endl;
    cout() << "    - smiley"                                                 << std::endl;
    cout() << "    - simple"                                                 << std::endl;
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
