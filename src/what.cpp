#include <QImage>
#include <QPainter>
#include <QTimer>
#include "what.h"

#undef signals
#undef signal
#undef emit
#include <gtk/gtk.h>

struct What::Private
{
    GtkWidget* window;
    GtkWidget* button;
};

What::What(QQuickItem* parent) : QQuickPaintedItem(parent), d(new Private)
{
    d->window = gtk_offscreen_window_new();
    d->button = gtk_button_new_with_label("deine mutter");

    gtk_container_add(GTK_CONTAINER(d->window), d->button);
    gtk_widget_show_all(d->window);
    gtk_widget_queue_draw(d->button);
    gtk_widget_show_all(d->button);

    int width = gtk_widget_get_allocated_width(d->button);
    int height = gtk_widget_get_allocated_height(d->button);

    setImplicitSize(width, height);
    QTimer::singleShot(50, [this] { update(); });
}

What::~What()
{

}

void What::paint(QPainter* painter)
{
    auto pixbuf = gtk_offscreen_window_get_pixbuf((GtkOffscreenWindow*)d->window);
    GError* err = nullptr;
    if (err != nullptr) {
        qWarning() << err->message;
    }
    QImage image(gdk_pixbuf_get_pixels(pixbuf), gdk_pixbuf_get_width(pixbuf), gdk_pixbuf_get_height(pixbuf), gdk_pixbuf_get_rowstride(pixbuf), QImage::Format_RGBA8888);

    painter->drawImage(0, 0, image);

    g_object_unref(pixbuf);
}
