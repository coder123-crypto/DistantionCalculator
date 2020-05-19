#ifndef EXIFHELPERS_H
#define EXIFHELPERS_H

#include <QStringList>

#include <libexif/exif-data.h>

double parseFocalLength(const ExifData *ed)
{
    char buf[1024];
    ExifEntry *entry = exif_content_get_entry(ed->ifd[EXIF_IFD_EXIF], EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT);
    exif_entry_get_value(entry, buf, sizeof(buf));

    entry = exif_content_get_entry(ed->ifd[EXIF_IFD_EXIF], EXIF_TAG_FOCAL_LENGTH);
    exif_entry_get_value(entry, buf, sizeof(buf));

    return QString::fromUtf8(buf).split(" ").at(0).toDouble();
}

double parseDouble(const ExifData *ed, const ExifTag tag)
{
    char buf[1024];
    ExifEntry *entry = exif_content_get_entry(ed->ifd[EXIF_IFD_EXIF], tag);
    exif_entry_get_value(entry, buf, sizeof(buf));

    return QString::fromUtf8(buf).toDouble();
}

QString parseString(const ExifData *ed, const ExifTag tag)
{
    char buf[1024];
    ExifEntry *entry = exif_content_get_entry(ed->ifd[EXIF_IFD_EXIF], tag);
    exif_entry_get_value(entry, buf, sizeof(buf));

    return QString::fromUtf8(buf);
}


#endif // EXIFHELPERS_H
