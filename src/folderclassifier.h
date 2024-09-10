// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QQmlEngine>

class FolderClassifier : public QObject
{

	Q_OBJECT
	QML_NAMED_ELEMENT(FolderClassifier)
	QML_SINGLETON

public:
	explicit FolderClassifier(QObject* parent = nullptr);
	~FolderClassifier();

	Q_INVOKABLE QString classifyFolder(const QUrl& url);
};