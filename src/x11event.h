#ifndef X11EVENT_H
#define X11EVENT_H

#include <X11/Xutil.h>

#undef Bool
#undef CursorShape
#undef Expose
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef FontChange
#undef None
#undef Status
#undef Unsorted

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
        void windowFocusChanged(QString windowName);

    private:
        xcb_atom_t focusChangeAtom;

        QString getFocusedApplicationName();
};

#endif // X11EVENT_H
