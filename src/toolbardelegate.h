// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "NGLibQuick.h"

class SToolBarDelegate : public NGToolBarDelegate
{

	struct Private;
	QScopedPointer<Private> d;

public:
	explicit SToolBarDelegate(QQmlEngine* engine);
	~SToolBarDelegate();

	QQmlEngine* engine() override;
	std::optional<NGToolBarItem> itemForIdentifier(NGToolBarController* toolbar, const QString& identifier, bool willBeInsertedIntoToolbar) override;
	QList<QString> possibleItems() override;
	QList<QString> defaultitems() override;
};
