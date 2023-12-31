#include <kfileitem.h>
#include <QQmlEngine>

class KFileItemForeign : public QObject
{
	Q_OBJECT
	QML_NAMED_ELEMENT(FileItem)
	QML_FOREIGN(KFileItem)
	QML_UNCREATABLE("Enums only")
};
