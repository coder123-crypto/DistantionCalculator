cmake_minimum_required(VERSION 3.5)

project(libexif LANGUAGES C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(libexif STATIC
    config.h
    canon/exif-mnote-data-canon.c
    canon/mnote-canon-entry.c
    canon/mnote-canon-tag.c
    canon/exif-mnote-data-canon.h
    canon/mnote-canon-entry.h
    canon/mnote-canon-tag.h
    fuji/exif-mnote-data-fuji.c
    fuji/mnote-fuji-entry.c
    fuji/mnote-fuji-tag.c
    fuji/exif-mnote-data-fuji.h
    fuji/mnote-fuji-entry.h
    fuji/mnote-fuji-tag.h
    olympus/exif-mnote-data-olympus.c
    olympus/mnote-olympus-entry.c
    olympus/mnote-olympus-tag.c
    olympus/exif-mnote-data-olympus.h
    olympus/mnote-olympus-entry.h
    olympus/mnote-olympus-tag.h
    pentax/exif-mnote-data-pentax.c
    pentax/mnote-pentax-entry.c
    pentax/mnote-pentax-tag.c
    pentax/exif-mnote-data-pentax.h
    pentax/mnote-pentax-entry.h
    pentax/mnote-pentax-tag.h
    exif-byte-order.c
    exif-content.c
    exif-data.c
    exif-entry.c
    exif-format.c
    exif-ifd.c
    exif-loader.c
    exif-log.c
    exif-mem.c
    exif-mnote-data.c
    exif-tag.c
    exif-utils.c
    _stdint.h
    exif.h
    exif-byte-order.h
    exif-content.h
    exif-data.h
    exif-data-type.h
    exif-entry.h
    exif-format.h
    exif-ifd.h
    exif-loader.h
    exif-log.h
    exif-mem.h
    exif-mnote-data.h
    exif-mnote-data-priv.h
    exif-system.h
    exif-tag.h
    exif-utils.h
    i18n.h
)

target_compile_definitions(libexif PRIVATE LIBEXIF_LIBRARY)
