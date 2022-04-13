#pragma once

#include <NGLib.h>

class SWindowRestorer : public NGRestorer
{
	Q_OBJECT
	Q_INTERFACES(NGRestorer)

public:
	Q_INVOKABLE explicit SWindowRestorer();
	~SWindowRestorer();

	void restore(QUuid id, const KConfigGroup& state, CompletionHandler completionHandler) override;
};
