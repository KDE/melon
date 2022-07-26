#pragma once

#include <QQuickPaintedItem>

class SRubberBand : public QQuickPaintedItem
{

    Q_OBJECT
    QML_NAMED_ELEMENT(RubberBand)

public:
    explicit SRubberBand(QQuickItem* parent = nullptr);
    ~SRubberBand();

    void paint(QPainter* painter) override;
};
