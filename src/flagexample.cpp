#include "flagexample.h"

struct FlagExample::Private
{
	WhatToSet set = {};
};

FlagExample::FlagExample(QObject* parent) : QObject(parent), d(new Private)
{

}

FlagExample::~FlagExample()
{

}

FlagExample::WhatToSet FlagExample::whatToSet() const
{
	return d->set;
}
void FlagExample::setWhatToSet(WhatToSet value)
{
	if (d->set == value)
		return;

	d->set = value;
	Q_EMIT whatToSetChanged();
}
void FlagExample::resetWhatToSet()
{
	d->set = {};
	Q_EMIT whatToSetChanged();
}
