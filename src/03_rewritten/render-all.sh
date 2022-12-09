#!/bin/sh

# ----------------------------------------------------------------------------
# command-line options and constants
# ----------------------------------------------------------------------------

arg_script=$(basename "${0:-not-set}")
opt_usage='no'
opt_error='no'
opt_renderer="./card.bin"
opt_width="512"
opt_height="512"
opt_samples="64"
opt_scenes="
aek
ponceto
smiley
simple
"

# ----------------------------------------------------------------------------
# parse the remaining options
# ----------------------------------------------------------------------------

while [ "${#}" -gt '0' ]
do
    case "${1}" in
        *=*)
            arg_value="$(expr "${1}" : '[^=]*=\(.*\)')"
            ;;
        *)
            arg_value=''
            ;;
    esac
    case "${1}" in
        default)
            opt_width="512"
            opt_height="512"
            opt_samples="64"
            ;;
        tiny)
            opt_width="384"
            opt_height="216"
            ;;
        small)
            opt_width="480"
            opt_height="270"
            ;;
        medium)
            opt_width="960"
            opt_height="540"
            ;;
        large)
            opt_width="1440"
            opt_height="810"
            ;;
        huge)
            opt_width="1920"
            opt_height="1080"
            ;;
        q8)
            opt_samples="8"
            ;;
        q16)
            opt_samples="16"
            ;;
        q32)
            opt_samples="32"
            ;;
        q64)
            opt_samples="64"
            ;;
        q128)
            opt_samples="128"
            ;;
        q256)
            opt_samples="256"
            ;;
        q512)
            opt_samples="512"
            ;;
        q1024)
            opt_samples="1024"
            ;;
        help)
            opt_usage='yes'
            ;;
        --help)
            opt_usage='yes'
            ;;
        *)
            opt_error='yes'
            ;;
    esac
    shift
done

# ----------------------------------------------------------------------------
# display help if needed
# ----------------------------------------------------------------------------

if [ "${opt_usage}" = 'yes' ] || [ "${opt_error}" = 'yes' ]
then
    cat << ____EOF
Usage: ${arg_script} [ARGUMENTS...]

Arguments:

    help, --help            display this help

    default                 resolution of 512x512, 64 samples per pixel

    tiny                    set resolution to 384x216
    small                   set resolution to 480x270
    medium                  set resolution to 960x540
    large                   set resolution to 1440x810
    huge                    set resolution to 1920x1080

    q8                      set quality to 8 samples per pixel
    q16                     set quality to 16 samples per pixel
    q32                     set quality to 32 samples per pixel
    q64                     set quality to 64 samples per pixel
    q128                    set quality to 128 samples per pixel
    q256                    set quality to 256 samples per pixel
    q512                    set quality to 512 samples per pixel
    q1024                   set quality to 1024 samples per pixel

____EOF
    if [ "${opt_error}" = 'yes' ]
    then
        exit 1
    fi
    exit 0
fi

# ----------------------------------------------------------------------------
# sanity checks
# ----------------------------------------------------------------------------

if [ ! -x "${opt_renderer}" ]
then
    echo "*** error: ${opt_renderer} was not found ***"
    exit 1
fi

# ----------------------------------------------------------------------------
# debug mode
# ----------------------------------------------------------------------------

set -x

# ----------------------------------------------------------------------------
# render all scenes
# ----------------------------------------------------------------------------

for scene in ${opt_scenes}
do
    ${opt_renderer} \
		--scene="${scene}" \
		--width="${opt_width}" \
		--height="${opt_height}" \
		--samples="${opt_samples}" \
		--output="${scene}-${opt_width}x${opt_height}-q${opt_samples}.ppm"
    if [ "${?}" != 0 ]
    then
        echo "*** error: an error occured while rendering ***"
        exit 1
    fi
done

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
