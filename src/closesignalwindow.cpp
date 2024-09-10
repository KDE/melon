#include "closesignalwindow.h"

SCloseSignalWindow::SCloseSignalWindow(QWidget *parent) : QMainWindow(parent)
{
}

SCloseSignalWindow::~SCloseSignalWindow()
{
}

void SCloseSignalWindow::closeEvent(QCloseEvent *event)
{
	Q_UNUSED(event)
	Q_EMIT closing();
}
