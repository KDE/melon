// SPDX-FileCopyrightText: 2022 Janet Blackquill <uhhadd@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <NGLib.h>

class SWindowRestorer : public QObject, public NGRestorer
{
	Q_OBJECT
	Q_INTERFACES(NGRestorer)

public:
	Q_INVOKABLE explicit SWindowRestorer();
	~SWindowRestorer();

	void restore(QUuid id, const KConfigGroup& state, CompletionHandler completionHandler) override;
};
