/*
 * The Business Card Raytracer
 *
 * Original author: Andrew Kensler
 *
 * Refactored with love by Olivier Poncet
 *
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
// world
// ---------------------------------------------------------------------------

namespace {

const uint32_t world[] = {
    0b00000000000000000000100000000000,
    0b00000000000000000000100000000000,
    0b00000001110000111000100000000000,
    0b00000000001001000100100010000000,
    0b00000000001001000100100100000000,
    0b00000001111001111100101000000000,
    0b00000010001001000000110000000000,
    0b00000010001001000000101000000000,
    0b00000001111000111000100100000000
};

}

// ---------------------------------------------------------------------------
// random number generator in range [+0.0 ; +1.0]
// ---------------------------------------------------------------------------

float randomize()
{
    return static_cast<float>(::rand()) / static_cast<float>(RAND_MAX);
}

// ---------------------------------------------------------------------------
// card::raytracer::trace
// ---------------------------------------------------------------------------

namespace card {

int raytracer::trace(const vec3f& origin, const vec3f& direction, float& distance, vec3f& normal)
{
    double constexpr distance_max = 1e9;
    double constexpr distance_min = 0.01;
    int    constexpr cols         = 32;
    int    constexpr rows         = countof(world);
    int    constexpr col_offset   = 7;
    int    constexpr row_offset   = 12;
    int              hit_type     = card::kNoHit;

    auto hit_init = [&]() -> void
    {
        distance = distance_max;
    };

    auto hit_plane = [&]() -> void
    {
        const float plane = -origin.z / direction.z;
        if(plane > distance_min) {
            hit_type = card::kPlaneHit;
            distance = plane;
            normal   = vec3f(0.0f, 0.0f, 1.0f);
        }
    };

    auto hit_sphere = [&](const float x, const float y, const float z) -> void
    {
        const vec3f p = origin + vec3f(x, y, z);
        const float b = vec3f::dot(p, direction);
        const float c = vec3f::dot(p, p) - 1.0f;
        const float q = b * b - c;
        if(q > 0) {
            const float sphere = -b - ::sqrtf(q);
            if((sphere < distance) && (sphere > distance_min)) {
                hit_type = card::kSphereHit;
                distance = sphere;
                normal   = vec3f::normalize(p + direction * distance);
            }
        }
    };

    auto hit_spheres = [&]() -> void
    {
        for(int row = 0; row < rows; ++row) {
            uint32_t constexpr lsb = (1 << 0);
            uint32_t           val = world[row];
            if(val == 0) {
                continue;
            }
            for(int col = 0; col < cols; ++col) {
                if(val & lsb) {
                    const float x = static_cast<float>(col - col_offset);
                    const float y = static_cast<float>(0.0f);
                    const float z = static_cast<float>(row - row_offset);
                    hit_sphere(x, y, z);
                }
                if((val >>= 1) == 0) {
                    break;
                }
            }
        }
    };

    hit_init();
    hit_plane();
    hit_spheres();

    return hit_type;
}

}

// ---------------------------------------------------------------------------
// card::raytracer::sample
// ---------------------------------------------------------------------------

namespace card {

vec3f raytracer::sample(const vec3f& origin, const vec3f& direction)
{
    float distance;
    vec3f normal;
    const int hit_type = trace(origin, direction, distance, normal);

    if(hit_type == card::kNoHit) {
        return _sky_color * ::powf(1.0f - direction.z, 4.0f);
    }

    const vec3f h = origin + direction * distance;
    const vec3f l = vec3f::normalize(vec3f(_light_pos.x + randomize(), _light_pos.y + randomize(), _light_pos.z) - h);
    const vec3f r = direction + normal * (vec3f::dot(normal, direction) * -2.0f);
    float b = vec3f::dot(l, normal);
    if((b < 0.0f) || trace(h, l, distance, normal)) {
        b = 0.0f;
    }
    const float p = ::powf(vec3f::dot(l, r) * (b > 0.0f), 99.0f);

    if(hit_type == card::kPlaneHit) {
        const float x = ::ceilf(h.x * 0.2f);
        const float y = ::ceilf(h.y * 0.2f);
        const int tile = static_cast<int>(x + y) & 1;
        return (tile ? _floor_color1 : _floor_color2) * (b * 0.2f + 0.1f);
    }
    return sample(h, r) * 0.5f + vec3f(p, p, p);
}

}

// ---------------------------------------------------------------------------
// card::raytracer::raytrace
// ---------------------------------------------------------------------------

namespace card {

void raytracer::raytrace(ppm::writer& output, const int w, const int h)
{
    const int   half_w  = (w / 2);
    const int   half_h  = (h / 2);
    const int   samples = 64;     // number of ray traced
    const vec3f right   = vec3f::normalize(vec3f::cross(_camera_direction, _camera_normal)) * _fov;
    const vec3f down    = vec3f::normalize(vec3f::cross(_camera_direction, right )) * _fov;
    const vec3f corner  = _camera_direction - (right + down) * 0.5f; // lower right corner

    auto R0 = [&]() -> float
    {
        return randomize();
    };

    auto R1 = [&]() -> float
    {
        return randomize() - 0.5f;
    };

    for(int y = 0; y < h; ++y) {
        for(int x = 0; x < w; ++x) {
            vec3f color(_ambiant_color);
            for(int count = 0; count < samples; ++count) {
                const vec3f pos = ( (right * R1())
                                  + ( down * R1()) ) * _dof;
                const vec3f dir = (right * (static_cast<float>(x - half_w) + R0()))
                                + ( down * (static_cast<float>(y - half_h) + R0()))
                                + corner;
                const vec3f ray = vec3f::normalize(dir * 16.0f - pos);
                color = color + sample(_camera_position + pos, ray) * 3.5f;
            }
            output.store ( static_cast<int>(color.x)
                         , static_cast<int>(color.y)
                         , static_cast<int>(color.z) );
        }
    }
}

}

// ---------------------------------------------------------------------------
// card::raytracer
// ---------------------------------------------------------------------------

namespace card {

raytracer::raytracer()
    : _camera_position (-17.0f, -16.0f,  +8.0f)
    , _camera_target   (-11.0f,   0.0f,  +8.0f)
    , _camera_normal   (  0.0f,   0.0f,  +1.0f)
    , _camera_direction(vec3f::normalize(_camera_target - _camera_position))
    , _light_pos       (-10.0f, -10.0f, +16.0f)
    , _ambiant_color   (+13.0f, +13.0f, +13.0f)
    , _sky_color       ( +0.7f,  +0.6f,  +1.0f)
    , _floor_color1    ( +3.0f,  +1.0f,  +1.0f)
    , _floor_color2    ( +3.0f,  +3.0f,  +3.0f)
    , _fov(0.002f)
    , _dof(99.0f)
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
        cout() << profiler.name() << ':' << ' ' << "processing ..." << std::endl;
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
        card::raytracer raytracer;

        output.open(_card_w, _card_h, 255);
        begin();
        raytracer.raytrace(output, _card_w, _card_h);
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
    cout() << "Usage:" << ' ' << _program << ' ' << "[OPTIONS...]"  << std::endl;
    cout() << ""                                                    << std::endl;
    cout() << "The Business Card Raytracer"                         << std::endl;
    cout() << ""                                                    << std::endl;
    cout() << "Options:"                                            << std::endl;
    cout() << ""                                                    << std::endl;
    cout() << "    --help                  display this help"       << std::endl;
    cout() << "    --output={path}         the output filename"     << std::endl;
    cout() << "    --width={int}           the card width"          << std::endl;
    cout() << "    --height={int}          the card height"         << std::endl;
    cout() << ""                                                    << std::endl;
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
