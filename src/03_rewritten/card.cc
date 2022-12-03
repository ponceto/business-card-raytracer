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
// setup
// ---------------------------------------------------------------------------

namespace setup {

const uint32_t world[] = {
#ifdef PONCETO
    0b00000000000000000000000000000000,
    0b11100011001001001110111101110110,
    0b10010100101001010000100000101001,
    0b10010100101101010000100000101001,
    0b11100100101101010000111000101001,
    0b10000100101011010000100000101001,
    0b10000100101011010000100000101001,
    0b10000011001001001110111100100110,
    0b00000000000000000000000000000000
#else /* aek */
    0b00000000000000000000010000000000,
    0b00000000000000000000010000000000,
    0b00000000111000011100010000000000,
    0b00000000000100100010010001000000,
    0b00000000000100100010010010000000,
    0b00000000111100111110010100000000,
    0b00000001000100100000011000000000,
    0b00000001000100100000010100000000,
    0b00000000111100011100010010000000
#endif
};

#ifdef PONCETO
const gl::pos3f camera_position (-19.0f, -19.0f, +15.0f);
const gl::pos3f camera_target   ( -5.0f,   0.0f, +11.0f);
const gl::pos3f camera_top      (-19.0f, -19.0f, +16.0f);
const float     camera_fov      (0.002f);
const float     camera_dof      (256.0f);
const float     camera_focus    ( 25.0f);
const gl::pos3f light_position  ( +5.0f, -15.0f, +15.0f);
const gl::col3f light_color     ( +0.5f,  +0.7f,  +0.9f);
const float     light_power     (+50.0f);
const gl::pos3f floor_position  ( 0.0f,  0.0f, +0.0f);
const gl::vec3f floor_normal    ( 0.0f,  0.0f, +1.0f);
const gl::col3f floor_color1    (+1.0f, +0.3f, +0.3f);
const gl::col3f floor_color2    (+1.0f, +1.0f, +1.0f);
const gl::col3f sky_color       (+0.70f, +0.60f, +1.00f);
const gl::col3f ambient_color   (+0.50f, +0.50f, +0.50f);
#else /* aek */
const gl::pos3f camera_position (-7.0f, -16.0f,  +8.0f);
const gl::pos3f camera_target   (-1.0f,   0.0f,  +8.0f);
const gl::pos3f camera_top      (-7.0f, -16.0f,  +9.0f);
const float     camera_fov      (0.002f);
const float     camera_dof      ( 99.0f);
const float     camera_focus    ( 16.0f);
const gl::pos3f light_position  ( +0.5f,  -9.5f, +16.0f);
const gl::col3f light_color     (+1.00f, +1.00f, +1.00f);
const float     light_power     (+15.0f);
const gl::pos3f floor_position  ( 0.0f,  0.0f, +0.0f);
const gl::vec3f floor_normal    ( 0.0f,  0.0f, +1.0f);
const gl::col3f floor_color1    (+1.0f, +0.3f, +0.3f);
const gl::col3f floor_color2    (+1.0f, +1.0f, +1.0f);
const gl::col3f sky_color       (+0.70f, +0.60f, +1.00f);
const gl::col3f ambient_color   (+0.35f, +0.35f, +0.35f);
#endif

}

// ---------------------------------------------------------------------------
// card::scene
// ---------------------------------------------------------------------------

namespace card {

scene::scene()
    : scene ( rt::camera ( setup::camera_position
                         , setup::camera_target
                         , setup::camera_top
                         , setup::camera_fov
                         , setup::camera_dof
                         , setup::camera_focus )
            , rt::light  ( setup::light_position
                         , setup::light_color
                         , setup::light_power )
            , rt::floor  ( setup::floor_position
                         , setup::floor_normal
                         , setup::floor_color1
                         , setup::floor_color2 )
            , rt::sky    ( setup::sky_color )
            , gl::col3f  ( setup::ambient_color ) )
{
}

}

// ---------------------------------------------------------------------------
// card::raytracer
// ---------------------------------------------------------------------------

namespace card {

raytracer::raytracer(base::console& console, card::scene& scene)
    : _console(console)
    , _random1(-0.50, +0.50)
    , _random2(-0.75, +0.75)
    , _scene(scene)
{
}

int raytracer::hit(const rt::ray& ray, rt::hit_result& result)
{
    auto hit_floor = [&]() -> void
    {
        const float distance = -ray.origin.z / ray.direction.z;
        if((distance < result.distance) && (distance > result.distance_min)) {
            result.type     = rt::hit_result::FLOOR_HIT;
            result.distance = distance;
            result.normal   = _scene.floor().normal;
        }
    };

    auto hit_sphere = [&](const gl::pos3f& center, const float radius) -> void
    {
        const gl::vec3f oc(gl::pos3f::difference(ray.origin, center));
#if 0
        /*
         * the complete analytic version
         */
        const float a = gl::vec3f::dot(ray.direction, ray.direction);
        const float b = 2.0f * gl::vec3f::dot(oc, ray.direction);
        const float c = gl::vec3f::dot(oc, oc) - (radius * radius);
        const float delta = ((b * b) - (4.0 * a * c));
        if(delta > 0.0) {
            const float distance = ((-b - ::sqrtf(delta)) / (2.0f * a));
            if((distance < result.distance) && (distance > result.distance_min)) {
                result.type     = rt::hit_result::SPHERE_HIT;
                result.distance = distance;
                result.normal   = gl::vec3f::normalize(oc + ray.direction * result.distance);
            }
        }
#else
        /*
         * the simplified analytic version
         */
        const float b = gl::vec3f::dot(oc, ray.direction);
        const float c = gl::vec3f::dot(oc, oc) - (radius * radius);
        const float delta = ((b * b) - c);
        if(delta > 0.0) {
            const float distance = (-b - ::sqrtf(delta));
            if((distance < result.distance) && (distance > result.distance_min)) {
                result.type     = rt::hit_result::SPHERE_HIT;
                result.distance = distance;
                result.normal   = gl::vec3f::normalize(oc + ray.direction * result.distance);
            }
        }
#endif
    };

    auto hit_spheres = [&]() -> void
    {
        constexpr int cols       = 32;
        constexpr int rows       = countof(setup::world);
        constexpr int col_offset = -16;
        constexpr int row_offset = +3;

        for(int row = 0; row < rows; ++row) {
            uint32_t constexpr lsb = (1 << 0);
            uint32_t           val = setup::world[row];
            if(val == 0) {
                continue;
            }
            for(int col = 0; col < cols; ++col) {
                if(val & lsb) {
                    const float x = static_cast<float>((cols - col) + col_offset);
                    const float y = static_cast<float>(0);
                    const float z = static_cast<float>((rows - row) + row_offset);
                    const float r = 1.0;
                    hit_sphere(gl::pos3f(x, y, z), r);
                }
                if((val >>= 1) == 0) {
                    break;
                }
            }
        }
    };

    hit_floor();
    hit_spheres();

    result.position = (ray.origin + (ray.direction * result.distance));

    return result.type;
}

gl::col3f raytracer::trace(const rt::ray& ray, const int recursion)
{
    const rt::light& light  (_scene.light());
    const rt::floor& floor  (_scene.floor());
    const rt::sky&   sky    (_scene.sky());
    const gl::col3f& ambient(_scene.ambient());

    if(recursion <= 0) {
        return ambient;
    }

    rt::hit_result result;
    if(hit(ray, result) == rt::hit_result::SKY_HIT) {
        return sky.color * ::powf(1.0f - ray.direction.z, 4.0f);
    }

    const gl::pos3f light_pos ( (light.position.x + _random2())
                              , (light.position.y + _random2())
                              , (light.position.z + _random2()) );

    const gl::vec3f light_direction(gl::vec3f::normalize(gl::pos3f::difference(light_pos, result.position)));
    const gl::vec3f reflection(ray.direction + result.normal * (gl::vec3f::dot(result.normal, ray.direction) * -2.0f));
    float distance    = gl::vec3f::length(gl::pos3f::difference(light.position, result.position));
    float attenuation = 1.0f / ::sqrtf(distance / light.power);
    float diffusion   = gl::vec3f::dot(light_direction, result.normal);

    /* cast shadows */ {
        rt::hit_result dummy;
        if((diffusion < 0.0f) || hit(rt::ray(result.position, light_direction), dummy)) {
            diffusion = 0.0f;
        }
    }

    if(result.type == rt::hit_result::FLOOR_HIT) {
        const float x = ::ceilf(result.position.x * 0.2f);
        const float y = ::ceilf(result.position.y * 0.2f);
        const gl::col3f color ( static_cast<int>(x + y) & 1
                              ? floor.color1
                              : floor.color2 );
        return (color * ambient) + (color * light.color * diffusion * attenuation);
    }

    const float p = ::powf(gl::vec3f::dot(light_direction, reflection) * (diffusion > 0.0f), 99.0f);

    return trace(rt::ray(result.position, reflection), (recursion - 1)) * 0.5f + (light.color * p * attenuation);
}

void raytracer::render(ppm::writer& output, const int w, const int h, const int samples, const int recursions)
{
    const int   half_w = (w / 2);
    const int   half_h = (h / 2);
    const float scale  = 255.0f / static_cast<float>(samples);
    const rt::camera& camera(_scene.camera());
    const gl::vec3f   right (gl::vec3f::normalize(gl::vec3f::cross(camera.direction, camera.normal)) * camera.fov);
    const gl::vec3f   down  (gl::vec3f::normalize(gl::vec3f::cross(camera.direction, right        )) * camera.fov);
    const gl::vec3f   corner(camera.direction - (right + down) * 0.5f);

    for(int y = 0; y < h; ++y) {
        for(int x = 0; x < w; ++x) {
            gl::col3f color;
            for(int count = samples; count != 0; --count) {
                const gl::vec3f lens ( ( (right * _random1())
                                       + ( down * _random1()) ) * camera.dof );

                const gl::vec3f dir ( (right * (static_cast<float>(x - half_w + 1) + _random1()))
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
// card::generator
// ---------------------------------------------------------------------------

namespace card {

generator::generator(int argc, char* argv[])
    : base::console(std::cin, std::cout, std::cerr)
    , base::program(argc, argv)
    , _program("card")
    , _output("card.ppm")
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
        card::scene scene;
        card::raytracer raytracer(*this, scene);

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
    cout() << "Usage:" << ' ' << _program << ' ' << "[OPTIONS...]"   << std::endl;
    cout() << ""                                                     << std::endl;
    cout() << "The Business Card Raytracer"                          << std::endl;
    cout() << ""                                                     << std::endl;
    cout() << "Options:"                                             << std::endl;
    cout() << ""                                                     << std::endl;
    cout() << "    --help                  display this help"        << std::endl;
    cout() << "    --output={path}         the output filename"      << std::endl;
    cout() << "    --width={int}           the card width"           << std::endl;
    cout() << "    --height={int}          the card height"          << std::endl;
    cout() << "    --samples={int}         samples per pixel"        << std::endl;
    cout() << "    --recursions={int}      maximum recursions level" << std::endl;
    cout() << ""                                                     << std::endl;
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
