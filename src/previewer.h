#pragma once

#include <QObject>
#include <QQmlEngine>
#include <KFileItem>
#include <QPixmap>

class SPreviewer : public QObject, public QQmlParserStatus
{

    Q_OBJECT
    QML_NAMED_ELEMENT(Previewer)
    Q_INTERFACES(QQmlParserStatus)

    struct Private;
    QScopedPointer<Private> d;

public:
	explicit SPreviewer(QObject* parent = nullptr);
	~SPreviewer();

    void classBegin() override { }
    void componentComplete() override;

    Q_PROPERTY(QPixmap pixmap READ pixmap NOTIFY pixmapChanged)
    QPixmap pixmap() const;
    Q_SIGNAL void pixmapChanged();

    Q_PROPERTY(bool pixmapValid READ pixmapValid NOTIFY pixmapChanged)
    bool pixmapValid() const { return !pixmap().isNull(); }

    Q_PROPERTY(KFileItem fileItem READ fileItem WRITE setFileItem NOTIFY fileItemChanged REQUIRED)
    KFileItem fileItem() const;
    void setFileItem(KFileItem item);
    Q_SIGNAL void fileItemChanged();

    Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY sizeChanged REQUIRED)
    QSize size() const;
    void setSize(const QSize& size);
    Q_SIGNAL void sizeChanged();

};
