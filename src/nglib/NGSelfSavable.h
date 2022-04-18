#pragma once

#include "NGRestorer.h"
#include "NGSavable.h"

class NGSelfSavable : public NGRestorer, public NGSavable
{

};

#define NGDefineSelfSavableMethods \
	NGDefineRestorerMethods \
	NGDefineSavableMethods

