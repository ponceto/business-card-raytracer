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
// vec3f: vector implementation
// ---------------------------------------------------------------------------

struct vec3f
{
    float x, y, z;

    vec3f operator+(vec3f r)
    {
        return vec3f(x + r.x, y + r.y, z + r.z);
    }

    vec3f operator*(float r)
    {
        return vec3f(x * r, y * r, z * r);
    }

    float operator%(vec3f r)
    {
        return x * r.x + y * r.y + z * r.z;
    }

    vec3f()
    {
    }

    vec3f operator^(vec3f r)
    {
        return vec3f(y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x);
    }

    vec3f(float a, float b, float c)
    {
        x = a;
        y = b;
        z = c;
    }

    vec3f operator!()
    {
        return *this * (1 / ::sqrtf(*this % *this));
    }
};

// ---------------------------------------------------------------------------
// G: world
// ---------------------------------------------------------------------------

int G[] = {
    247570,
    280596,
    280600,
    249748,
    18578,
    18577,
    231184,
    16,
    16
};

// ---------------------------------------------------------------------------
// R: random number generator
// ---------------------------------------------------------------------------

float R()
{
    return static_cast<float>(::rand()) / static_cast<float>(RAND_MAX);
}

// ---------------------------------------------------------------------------
// T: trace
// ---------------------------------------------------------------------------

int T(vec3f o, vec3f d, float &t, vec3f &n)
{
    t = 1e9;
    int m = 0;
    float p = -o.z / d.z;
    if(.01 < p) {
        t = p, n = vec3f(0, 0, 1), m = 1;
    }
    for(int k = 19; k--;) {
        for(int j = 9; j--;) {
            if(G[j] & 1 << k) {
                vec3f p = o + vec3f(-k, 0, -j - 4);
                float b = p % d, c = p % p - 1, q = b * b - c;
                if(q > 0) {
                    float s = -b - ::sqrtf(q);
                    if(s < t && s > .01) {
                        t = s, n = !(p + d * t), m = 2;
                    }
                }
            }
        }
    }
    return m;
}

// ---------------------------------------------------------------------------
// S: sample
// ---------------------------------------------------------------------------

vec3f S(vec3f o, vec3f d)
{
    float t;
    vec3f n;
    int m = T(o, d, t, n);
    if(!m) {
        return vec3f(.7, .6, 1) * ::powf(1 - d.z, 4);
    }
    vec3f h = o + d * t, l = !(vec3f(9 + R(), 9 + R(), 16) + h * -1), r = d + n * (n % d * -2);
    float b = l % n;
    if(b < 0 || T(h, l, t, n)) {
        b = 0;
    }
    float p = ::powf(l % r * (b > 0), 99);
    if(m & 1) {
        h = h * .2;
        return (static_cast<int>(::ceilf(h.x) + ::ceilf(h.y)) & 1 ? vec3f(3, 1, 1) : vec3f(3, 3, 3)) * (b * .2 + .1);
    }
    return vec3f(p, p, p) + S(h, r) * .5;
}

// ---------------------------------------------------------------------------
// raytrace
// ---------------------------------------------------------------------------

void raytrace(ppm::writer& output, const int w, const int h)
{
    vec3f g = !vec3f(-6, -16, 0), a = !(vec3f(0, 0, 1) ^ g) * .002, b = !(g ^ a) * .002, c = (a + b) * -256 + g;
    for(int y = h; y--;) {
        for(int x = w; x--;) {
            vec3f p(13, 13, 13);
            for(int r = 64; r--;) {
                vec3f t = a * (R() - .5) * 99 + b * (R() - .5) * 99;
                p = S(vec3f(17, 16, 8) + t, !(t * -1 + (a * (R() + x) + b * (y + R()) + c) * 16)) * 3.5 + p;
            }
            output.store(static_cast<int>(p.x), static_cast<int>(p.y), static_cast<int>(p.z));
        }
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

    auto loop = [&]() -> void
    {
        ppm::writer output(_output);

        output.open(_card_w, _card_h, 255);
        raytrace(output, _card_w, _card_h);
        output.close();
    };

    auto end = [&]() -> void
    {
        cout() << profiler.name() << ':' << ' ' << profiler.elapsed() << 's' << std::endl;
        profiler.reset();
    };

    auto execute = [&]() -> void
    {
        check();
        begin();
        loop();
        end();
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
