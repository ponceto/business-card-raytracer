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

namespace setup {

const gl::pos3f camera_position (-17.0f, -16.0f,  +8.0f);
const gl::pos3f camera_target   (-11.0f,   0.0f,  +8.0f);
const gl::pos3f camera_top      (-17.0f, -16.0f,  +9.0f);
const gl::pos3f light_position  (-10.0f, -10.0f, +16.0f);
const gl::col3f ambiant_color   (+13.0f, +13.0f, +13.0f);

const float fov = 0.002f;
const float dof = 99.0f;

}

}

// ---------------------------------------------------------------------------
// card::raytracer::randomize : range [+0.0 ; +1.0]
// ---------------------------------------------------------------------------

namespace card {

float raytracer::randomize()
{
    return static_cast<float>(::rand()) / static_cast<float>(RAND_MAX);
}

}

// ---------------------------------------------------------------------------
// card::raytracer::trace
// ---------------------------------------------------------------------------

namespace card {

int raytracer::trace(const gl::ray& ray, gl::pos3f& position, gl::vec3f& normal)
{
    float constexpr distance_max = 1e9;
    float constexpr distance_min = 0.01;
    int   constexpr cols         = 32;
    int   constexpr rows         = countof(world);
    int   constexpr col_offset   = -25;
    int   constexpr row_offset   = +3;
    int             hit_type     = card::kNoHit;
    float           hit_distance = distance_max;

    auto hit_floor = [&]() -> void
    {
        const float hit = -ray.origin.z / ray.direction.z;
        if((hit < hit_distance) && (hit > distance_min)) {
            hit_type     = card::kPlaneHit;
            hit_distance = hit;
            normal       = _floor.normal;
        }
    };

    auto hit_sphere = [&](const gl::pos3f& center, const float radius) -> void
    {
        const gl::vec3f oc(gl::pos3f::difference(ray.origin, center));
#if 0
        /*
         * complete analytic version
         */
        const float a = gl::vec3f::dot(ray.direction, ray.direction);
        const float b = 2.0f * gl::vec3f::dot(oc, ray.direction);
        const float c = gl::vec3f::dot(oc, oc) - (radius * radius);
        const float delta = ((b * b) - (4.0 * a * c));
        if(delta > 0.0) {
            const float hit = ((-b - ::sqrtf(delta)) / (2.0f * a));
            if((hit < hit_distance) && (hit > distance_min)) {
                hit_type     = card::kSphereHit;
                hit_distance = hit;
                normal       = gl::vec3f::normalize(oc + ray.direction * hit_distance);
            }
        }
#else
        /*
         * simplified analytic version
         */
        const float b = gl::vec3f::dot(oc, ray.direction);
        const float c = gl::vec3f::dot(oc, oc) - (radius * radius);
        const float delta = ((b * b) - c);
        if(delta > 0.0) {
            const float hit = (-b - ::sqrtf(delta));
            if((hit < hit_distance) && (hit > distance_min)) {
                hit_type     = card::kSphereHit;
                hit_distance = hit;
                normal       = gl::vec3f::normalize(oc + ray.direction * hit_distance);
            }
        }
#endif
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

    position = (ray.origin + (ray.direction * hit_distance));

    return hit_type;
}

}

// ---------------------------------------------------------------------------
// card::raytracer::sample
// ---------------------------------------------------------------------------

namespace card {

gl::col3f raytracer::sample(const gl::ray& ray)
{
    gl::pos3f hit_position;
    gl::vec3f hit_normal;
    const int hit_type = trace(ray, hit_position, hit_normal);

    if(hit_type == card::kNoHit) {
        return _sky.color * ::powf(1.0f - ray.direction.z, 4.0f);
    }

    const gl::vec3f light_direction(gl::vec3f::normalize(gl::pos3f::difference(gl::pos3f(_light.position.x + randomize(), _light.position.y + randomize(), _light.position.z), hit_position)));
    const gl::vec3f reflection(ray.direction + hit_normal * (gl::vec3f::dot(hit_normal, ray.direction) * -2.0f));
    float diffusion = gl::vec3f::dot(light_direction, hit_normal);

    /* shadows */ {
        gl::pos3f dummy;
        if((diffusion < 0.0f) || trace(gl::ray(hit_position, light_direction), dummy, hit_normal)) {
            diffusion = 0.0f;
        }
    }

    if(hit_type == card::kPlaneHit) {
        const float x = ::ceilf(hit_position.x * 0.2f);
        const float y = ::ceilf(hit_position.y * 0.2f);
        const int tile = static_cast<int>(x + y) & 1;
        return (tile ? _floor.color1 : _floor.color2) * (diffusion * 0.2f + 0.1f);
    }

    const float p = ::powf(gl::vec3f::dot(light_direction, reflection) * (diffusion > 0.0f), 99.0f);

    return sample(gl::ray(hit_position, reflection)) * 0.5f + gl::col3f(p, p, p);
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
    const int   samples = 64; // number of ray traced
    const gl::vec3f right  (gl::vec3f::normalize(gl::vec3f::cross(_camera.direction, _camera.normal)) * _fov);
    const gl::vec3f down   (gl::vec3f::normalize(gl::vec3f::cross(_camera.direction, right         )) * _fov);
    const gl::vec3f corner (_camera.direction - (right + down) * 0.5f); // lower right corner

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
            gl::col3f color(_ambiant);
            for(int count = 0; count < samples; ++count) {
                const gl::vec3f lens ( ( (right * R1())
                                       + ( down * R1()) ) * _dof );

                const gl::vec3f dir ( (right * (static_cast<float>(x - half_w) + R0()))
                                    + ( down * (static_cast<float>(y - half_h) + R0()))
                                    + corner );

                const gl::ray ray(_camera.position + lens, (dir * 16.0f - lens));

                color += sample(ray) * 3.5f;
            }
            output.store ( static_cast<int>(color.r)
                         , static_cast<int>(color.g)
                         , static_cast<int>(color.b) );
        }
    }
}

}

// ---------------------------------------------------------------------------
// card::raytracer
// ---------------------------------------------------------------------------

namespace card {

raytracer::raytracer()
    : _camera(setup::camera_position, setup::camera_target, setup::camera_top)
    , _light(setup::light_position)
    , _floor()
    , _sky()
    , _ambiant(setup::ambiant_color)
    , _fov(setup::fov)
    , _dof(setup::dof)
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
