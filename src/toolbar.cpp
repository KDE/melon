#include <QQmlComponent>
#include <QFile>
#include "app.h"
#include "toolbar.h"
#include <NGRestorer.h>
#include <KLocalizedString>
#include <KSharedConfig>

struct SToolBar::Private
{
	QMap<QString, SToolBarItem> components;
	QList<SToolBarItem> items;
	QList<SToolBarItem> defaultItems;
};

QQmlComponent* createComponent(QQmlEngine* engine, const QString& file)
{
	QUrl qmlURL("qrc:/" + file);
	QFile qmlFile(":/" + file);
	Q_ASSERT(qmlFile.open(QFile::ReadOnly));
	const auto data = qmlFile.readAll();
	auto component = new QQmlComponent(engine);
	component->setData(data, qmlURL);

	return component;
}

SToolBar::SToolBar(QObject* parent) : QObject(parent), d(new Private)
{
	d->components["back"] =
		SToolBarItem("back", i18n("Back"), createComponent(sApp->engine.get(), "toolbar/GoBack.qml"));
	d->components["forward"] =
		SToolBarItem("forward", i18n("Forward"), createComponent(sApp->engine.get(), "toolbar/GoForward.qml"));
	d->components["up"] =
		SToolBarItem("up", i18n("Up"), createComponent(sApp->engine.get(), "toolbar/GoUp.qml"));
	d->components["space"] =
		SToolBarItem("space", i18n("Expanding Spacer"), createComponent(sApp->engine.get(), "toolbar/ExpandingSpace.qml"));
	d->components["new-window"] =
		SToolBarItem("new-window", i18n("New Window"), createComponent(sApp->engine.get(), "toolbar/NewWindow.qml"));
	d->components["create-new"] =
		SToolBarItem("create-new", i18n("Create New"), createComponent(sApp->engine.get(), "toolbar/CreateNew.qml"));

	auto config = KSharedConfig::openConfig();
	const auto defaults = QStringList{"back", "forward", "up", "space", "new-window"};
	const auto items = config->group("Toolbar").readEntry("Items", defaults);

	for (auto item : items) {
		d->items << d->components[item];
	}
	for (auto item : defaults) {
		d->defaultItems << d->components[item];
	}

	connect(this, &SToolBar::itemsChanged, this, [=] {
		auto items = QStringList();
		for (auto item : d->items) {
			items << item.kind;
		}
		config->group("Toolbar").writeEntry("Items", items);
	});
}

SToolBar::~SToolBar()
{
}

QList<QVariant> SToolBar::items() const
{
	QList<QVariant> variants;
	for (const auto& item : d->items) {
		variants << QVariant::fromValue(item);
	}
	return variants;
}

QVariantList SToolBar::possibleItems() const
{
	QList<QVariant> variants;
	for (const auto& item : d->components) {
		variants << QVariant::fromValue(item);
	}
	return variants;
}

QVariantList SToolBar::defaultItems() const
{
	QList<QVariant> variants;
	for (const auto& item : d->defaultItems) {
		variants << QVariant::fromValue(item);
	}
	return variants;
}

void SToolBar::addItem(const QString& name)
{
	d->items << d->components[name];
	Q_EMIT itemsChanged();
}

void SToolBar::addItemBefore(const QString& name, int index)
{
	d->items.insert(index, d->components[name]);
	Q_EMIT itemsChanged();
}

void SToolBar::addItemAfter(const QString& name, int index)
{
	d->items.insert(index+1, d->components[name]);
	Q_EMIT itemsChanged();
}

void SToolBar::removeItemAt(int index)
{
	d->items.removeAt(index);
	Q_EMIT itemsChanged();
}

void SToolBar::resetToolbar()
{
	d->items = d->defaultItems;
	Q_EMIT itemsChanged();
}

static NGMetaTypeRegisterer<QList<SToolBarItem>> registerer;
