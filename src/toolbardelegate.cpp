#include <QMimeData>
#include <KLocalizedString>

#include "app.h"
#include "toolbardelegate.h"

struct SToolBarDelegate::Private
{
	QQmlEngine* engine = nullptr;
	QMap<QString, NGToolBarItem> components;
	QList<QString> orderedComponents;

	void add(const NGToolBarItem& item)
	{
		components[item.identifier()] = item;
		orderedComponents << item.identifier();
	}
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

SToolBarDelegate::SToolBarDelegate(QQmlEngine* engine) : d(new Private)
{
	d->engine = engine;

	d->add(NGToolBarItem("back", i18n("Back"), createComponent(sApp->engine.get(), "toolbar/GoBack.qml")));
	d->add(NGToolBarItem("forward", i18n("Forward"), createComponent(sApp->engine.get(), "toolbar/GoForward.qml")));
	d->add(NGToolBarItem("up", i18n("Up"), createComponent(sApp->engine.get(), "toolbar/GoUp.qml")));
	d->add(NGToolBarItem("space", i18n("Expanding Spacer"), createComponent(sApp->engine.get(), "toolbar/ExpandingSpace.qml")));
	d->add(NGToolBarItem("new-window", i18n("New Window"), createComponent(sApp->engine.get(), "toolbar/NewWindow.qml")));
	d->add(NGToolBarItem("create-new", i18n("Create New"), createComponent(sApp->engine.get(), "toolbar/CreateNew.qml")));
	d->add(NGToolBarItem("search", i18n("Search"), createComponent(sApp->engine.get(), "toolbar/Search.qml")));

}

SToolBarDelegate::~SToolBarDelegate()
{

}

QQmlEngine* SToolBarDelegate::engine()
{
	return d->engine;
}

std::optional<NGToolBarItem>
SToolBarDelegate::itemForIdentifier(NGToolBarController* toolbar, const QString& identifier, bool willBeInsertedIntoToolbar)
{
	Q_UNUSED(toolbar)
	Q_UNUSED(willBeInsertedIntoToolbar)

	if (!d->components.contains(identifier)) {
		return {};
	}

	return d->components[identifier];
}

QList<QString> SToolBarDelegate::possibleItems()
{
	return d->orderedComponents;
}

QList<QString> SToolBarDelegate::defaultitems()
{
	return {"back", "forward", "up", "space", "new-window"};
}
