#include "x11event.h"
#include <QDebug>

X11Event::X11Event()
{
    const char* atom = "_NET_ACTIVE_WINDOW";

    xcb_connection_t* conn = xcb_connect(NULL, NULL);

    xcb_intern_atom_reply_t *activeWindow = xcb_intern_atom_reply(conn, xcb_intern_atom(conn, 0, strlen(atom), atom), NULL);

    if(activeWindow)
        focusChangeAtom = activeWindow->atom;

    free(activeWindow);
}

X11Event::~X11Event() {}

bool X11Event::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result);

    if (eventType == "xcb_generic_event_t")
    {
        xcb_generic_event_t* event = static_cast<xcb_generic_event_t *>(message);
        if ((event->response_type & ~0x80) == XCB_PROPERTY_NOTIFY)
        {
            xcb_property_notify_event_t *notifyEvent = (xcb_property_notify_event_t*)event;
            if(notifyEvent->atom == focusChangeAtom)
                emit windowFocusChanged(getFocusedApplicationName());
        }
    }

    return false;
}

QString X11Event::getFocusedApplicationName()
{
    Display *display = XOpenDisplay(NULL);
    Window window;

    int revert;

    XGetInputFocus(display, &window, &revert);

    XClassHint classHint;
    XGetClassHint(display, window, &classHint);

    QString windowName = reinterpret_cast<char *>(classHint.res_name);

    return windowName;
}
