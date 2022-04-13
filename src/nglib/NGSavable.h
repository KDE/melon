#pragma once

#include <QString>
#include <KConfigGroup>

class NGSavable
{
public:
	struct SaveInformation
	{
		// the name of the class responsible for restoring the state
		QString className;
		// whether or not the save was successful
		bool ok;
	};

	virtual QUuid identifier() const = 0;
	virtual SaveInformation save(KConfigGroup& state) const = 0;
};