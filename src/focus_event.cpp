#include "focus_event.h"

FocusEvent::FocusEvent()
{
    const char* atom = "_NET_ACTIVE_WINDOW";

    // Connect to the X Server
    connection = xcb_connect(NULL, NULL);

    if (!connection || xcb_connection_has_error(connection))
    {
        qFatal("Cannot open display");
        exit(2);
    }

    // Get the active window atom from the X server
    xcb_intern_atom_reply_t *activeWindow = xcb_intern_atom_reply(connection,
                xcb_intern_atom(connection, 0, strlen(atom), atom), NULL);

    if(activeWindow)
        focusChangeAtom = activeWindow->atom;

    // Free resources
    xcb_flush(connection);
    free(activeWindow);
}

FocusEvent::~FocusEvent() {}

bool FocusEvent::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result);

    if (eventType == "xcb_generic_event_t")
    {
        xcb_generic_event_t* event = static_cast<xcb_generic_event_t *>(message);

        // Every event contains an 8-bit type code.
        // The most significant bit in this code is set if the event was generated from a SendEvent request.
        // That's why we need to AND it with either ~0x80 or 0x7F

        if ((event->response_type & ~0x80) == XCB_PROPERTY_NOTIFY)
        {
            xcb_property_notify_event_t *propertyEvent = (xcb_property_notify_event_t*)event;

            if(propertyEvent->atom == focusChangeAtom)
                emit windowFocusChanged(getFocusedApplicationName());
        }
    }

    return false;
}

QString FocusEvent::getFocusedApplicationName()
{
    QString windowName = "";

    xcb_get_property_reply_t *wmClass = NULL;
    xcb_get_input_focus_reply_t *inputFocus = NULL;

    xcb_grab_server(connection);

    inputFocus = xcb_get_input_focus_reply(connection, xcb_get_input_focus(connection), NULL);

    xcb_get_property_cookie_t classCookie =
            xcb_get_property(connection,
                             false,
                             inputFocus->focus,
                             XCB_ATOM_WM_CLASS,
                             XCB_ATOM_STRING,
                             0,
                             2048L);

    xcb_generic_error_t *err;

    wmClass = xcb_get_property_reply(connection, classCookie, &err);

    if(wmClass && wmClass->type == XCB_ATOM_STRING && xcb_get_property_value_length(wmClass) > 0 && !err)
    {
        free(err);
        windowName = reinterpret_cast<char *>(xcb_get_property_value(wmClass));
    }

    // Free resources
    xcb_ungrab_server(connection);
    xcb_flush(connection);
    free(wmClass);
    free(inputFocus);

    return windowName;
}
