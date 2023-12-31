#include <QStandardPaths>

#include "folderclassifier.h"

using namespace Qt::StringLiterals;

FolderClassifier::FolderClassifier(QObject* parent) : QObject(parent)
{

}

FolderClassifier::~FolderClassifier()
{

}


QString FolderClassifier::classifyFolder(const QUrl& url)
{
    for (auto path : QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)) {
        const auto furl = QUrl::fromLocalFile(path);
        if (furl == url || furl.isParentOf(url)) {
            return u"photos"_s;
        }
    }

    return QString();
}