#pragma once

#include "NGRestorer.h"
#include "NGSavable.h"
#include "NGSelfSavable.h"

class BoxBoxSchema : public NGSelfSavable
{
public:
	explicit BoxBoxSchema();
	~BoxBoxSchema();

	NGDefineSelfSavableMethods
};

class BoxBoxObjectSchema : public NGSelfSavable
{
public:
	explicit BoxBoxObjectSchema();
	~BoxBoxObjectSchema();

	NGDefineSelfSavableMethods
};

class BoxBoxObjectPropertySchema : public NGSelfSavable
{
public:
	explicit BoxBoxObjectPropertySchema();
	~BoxBoxObjectPropertySchema();
};

class BoxBoxObjectAttributeSchema : public BoxBoxObjectPropertySchema
{
public:
	explicit BoxBoxObjectAttributeSchema();
	~BoxBoxObjectAttributeSchema();

	NGDefineSelfSavableMethods
};

class BoxBoxObjectRelationshipSchema : public BoxBoxObjectPropertySchema
{
public:
	explicit BoxBoxObjectRelationshipSchema();
	~BoxBoxObjectRelationshipSchema();

	NGDefineSelfSavableMethods
};
