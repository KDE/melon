#pragma once

#include <KFileItem>
#include <QQmlEngine>

struct ForeignKFileItem
{
    Q_GADGET

    QML_FOREIGN(KFileItem)
    QML_NAMED_ELEMENT(FileItem)
    QML_UNCREATABLE("enums only")
};
