#include "NGLib.h"

#include <QStyle>
#include <QStyleOptionRubberBand>

#include "rubberband.h"

SRubberBand::SRubberBand(QQuickItem* parent) : QQuickPaintedItem(parent)
{
}

SRubberBand::~SRubberBand()
{
}

void SRubberBand::paint(QPainter* painter)
{
    QStyleOptionRubberBand opt;
    opt.state = QStyle::State_None;
    opt.palette = qApp->palette();
    opt.shape = QRubberBand::Rectangle;
    opt.opaque = false;
    opt.rect = contentsBoundingRect().toRect();
    qApp->style()->drawControl(QStyle::CE_RubberBand, &opt, painter);
}
