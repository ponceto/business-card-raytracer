# BUSINESS CARD RAYTRACER

This is the refactored source code of Andrew Kensler's famous `Business Card Raytracer`.

This basically the original source code with some useful classes and utilities to provide command-line parsing, file output, etc...

![Business Card Raytracer](../../doc/aek-original.png)

## BUILD

```
make
```

or

```
make all
```

## CLEAN

```
make clean
```

## RUN

```
make run
```

The output image is named `card.ppm`.

You can also run the generated program and the generated image is written by default in a file named `card.ppm`.

Some options are available on the command-line. Add `--help` to display the available options.

```
Usage: card.bin [OPTIONS...]

The Business Card Raytracer

Options:

    --help                  display this help
    --output={path}         the output filename
    --width={int}           the card width
    --height={int}          the card height

```

The following example will generate a file named `card.ppm`:

```
./card.bin
```

The following example will generate a file named `output.ppm`:

```
./card.bin --output=output.ppm
```
