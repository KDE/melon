#include <QMimeData>
#include <KLocalizedString>
#include <QFile>

#include "app.h"
#include "toolbardelegate.h"

using namespace Qt::StringLiterals;

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
	QUrl qmlURL(u"qrc:/"_s + file);
	QFile qmlFile(u":/"_s + file);
	qmlFile.open(QFile::ReadOnly);
	const auto data = qmlFile.readAll();
	auto component = new QQmlComponent(engine);
	component->setData(data, qmlURL);

	return component;
}

SToolBarDelegate::SToolBarDelegate(QQmlEngine* engine) : d(new Private)
{
	d->engine = engine;

	d->add(NGToolBarItem(u"back"_s, i18n("Back"), createComponent(sApp->engine.get(), u"toolbar/GoBack.qml"_s)));
	d->add(NGToolBarItem(u"forward"_s, i18n("Forward"), createComponent(sApp->engine.get(), u"toolbar/GoForward.qml"_s)));
	d->add(NGToolBarItem(u"up"_s, i18n("Up"), createComponent(sApp->engine.get(), u"toolbar/GoUp.qml"_s)));
	d->add(NGToolBarItem(u"space"_s, i18n("Expanding Spacer"), createComponent(sApp->engine.get(), u"toolbar/ExpandingSpace.qml"_s)));
	d->add(NGToolBarItem(u"new-window"_s, i18n("New Window"), createComponent(sApp->engine.get(), u"toolbar/NewWindow.qml"_s)));
	d->add(NGToolBarItem(u"create-new"_s, i18n("Create New"), createComponent(sApp->engine.get(), u"toolbar/CreateNew.qml"_s)));
	d->add(NGToolBarItem(u"search"_s, i18n("Search"), createComponent(sApp->engine.get(), u"toolbar/Search.qml"_s)));

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
	return {u"back"_s, u"forward"_s, u"up"_s, u"space"_s, u"new-window"_s};
}
