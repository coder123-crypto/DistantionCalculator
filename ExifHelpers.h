#ifndef EXIFHELPERS_H
#define EXIFHELPERS_H

#include <QStringList>

#include <cmath>
#include <libexif/exif-data.h>

double parseFocalLength(const ExifData *ed)
{
    char buf[1024];
    ExifEntry *entry = exif_content_get_entry(ed->ifd[EXIF_IFD_EXIF], EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT);

    if (entry == nullptr)
        return std::numeric_limits<double>::quiet_NaN();

    exif_entry_get_value(entry, buf, sizeof(buf));

    entry = exif_content_get_entry(ed->ifd[EXIF_IFD_EXIF], EXIF_TAG_FOCAL_LENGTH);
    exif_entry_get_value(entry, buf, sizeof(buf));

    return QString::fromUtf8(buf).split(" ").at(0).toDouble();
}

double parseDouble(const ExifData *ed, const ExifTag tag)
{
    char buf[1024];
    ExifEntry *entry = exif_content_get_entry(ed->ifd[EXIF_IFD_EXIF], tag);

    if (entry == nullptr)
        return std::numeric_limits<double>::quiet_NaN();

    exif_entry_get_value(entry, buf, sizeof(buf));

    return QString::fromUtf8(buf).toDouble();
}

QString parseString(const ExifData *ed, const ExifTag tag)
{
    char buf[1024];
    ExifEntry *entry = exif_content_get_entry(ed->ifd[EXIF_IFD_EXIF], tag);

    if (entry == nullptr)
        return QString();

    exif_entry_get_value(entry, buf, sizeof(buf));

    return QString::fromUtf8(buf);
}


#endif // EXIFHELPERS_H
