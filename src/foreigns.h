// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <kfileitem.h>
#include <QQmlEngine>

class KFileItemForeign : public QObject
{
	Q_OBJECT
	QML_NAMED_ELEMENT(FileItem)
	QML_FOREIGN(KFileItem)
	QML_UNCREATABLE("Enums only")
};
