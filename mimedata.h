#pragma once

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtCore/QMimeData>

/// @class MimeData
/// @brief Interface class to a MIME database

class MimeData : public QMimeData
{
    Q_OBJECT

public:
    MimeData();
    QStringList formats() const;

signals:
    void dataRequested(const QString &mimeType) const;

protected:
    QVariant retrieveData(const QString &mimetype, QVariant::Type type) const;
};

