#pragma once

#include <QObject>

class SOrgFreedesktopFilemanager1 : public QObject
{

    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.FileManager1");

public:
    explicit SOrgFreedesktopFilemanager1(QObject* parent = nullptr);
    ~SOrgFreedesktopFilemanager1();

    Q_SCRIPTABLE void ShowFolders(const QStringList& uriList, const QString& startupID);
    Q_SCRIPTABLE void ShowItems(const QStringList& uriList, const QString& startupID);
    Q_SCRIPTABLE void ShowItemProperties(const QStringList& uriList, const QString& startupID);
};
