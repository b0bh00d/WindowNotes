#include "mimedata.h"

MimeData::MimeData() : QMimeData()
{
}

QStringList MimeData::formats() const
{
    return QMimeData::formats() << "text/plain" << "text/uri-list";
}

QVariant MimeData::retrieveData(const QString& mimeType, QVariant::Type type) const
{
    emit dataRequested(mimeType);

    return QMimeData::retrieveData(mimeType, type);
}
