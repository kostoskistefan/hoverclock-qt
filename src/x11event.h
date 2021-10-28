#ifndef X11EVENT_H
#define X11EVENT_H

#include <xcb/xcb.h>
#include <QObject>
#include <QAbstractNativeEventFilter>

class X11Event : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

    public:
        X11Event();
        ~X11Event();

        virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;

    signals:
        void windowFocusChanged();

    private:
        xcb_atom_t focusChangeAtom;
};

#endif // X11EVENT_H
