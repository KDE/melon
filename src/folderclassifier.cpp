#include <QStandardPaths>

#include "folderclassifier.h"

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
            return "photos";
        }
    }

    return "";
}