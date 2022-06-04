#pragma once

#include <QObject>
#include <QQmlEngine>

struct SToolBarItem
{
	Q_GADGET

public:
	Q_PROPERTY(QString itemKind MEMBER kind)
	QString kind;

	Q_PROPERTY(QQmlComponent* itemComponent MEMBER component)
	QQmlComponent* component;

	Q_PROPERTY(QString guiName MEMBER guiName)
	QString guiName;

	SToolBarItem() : kind(), component(nullptr), guiName() { }
	SToolBarItem(const QString& item, const QString& guiName, QQmlComponent* component) : kind(item), component(component), guiName(guiName)
	{
	}
};
Q_DECLARE_METATYPE(SToolBarItem)

class SToolBar : public QObject
{

	Q_OBJECT
	QML_NAMED_ELEMENT(ToolBar)
	QML_UNCREATABLE("Only usable from c++")

	struct Private;
	QScopedPointer<Private> d;

public:
	explicit SToolBar(QObject* parent = nullptr);
	~SToolBar();

	Q_PROPERTY(QVariantList items READ items NOTIFY itemsChanged)
	QVariantList items() const;
	Q_SIGNAL void itemsChanged();

	Q_PROPERTY(QVariantList possibleItems READ possibleItems CONSTANT)
	QVariantList possibleItems() const;

	Q_PROPERTY(QVariantList defaultItems READ defaultItems CONSTANT)
	QVariantList defaultItems() const;

	Q_INVOKABLE void removeItemAt(int index);
	Q_INVOKABLE void addItem(const QString& name);
	Q_INVOKABLE void addItemBefore(const QString& name, int index);
	Q_INVOKABLE void addItemAfter(const QString& name, int index);
	Q_INVOKABLE void resetToolbar();
};
