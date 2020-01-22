#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h> // Every Xlib program must include this
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <unistd.h>

int hasError = 0;

static int error_handler(Display* display, XErrorEvent *error) {
  hasError = 1;
  return 0;
}

int main() {
  Display* display = XOpenDisplay(0);
  const char spectacleWindowName[] = "Spectacle";

  XEvent e;
  Window rootWindow = DefaultRootWindow(display);
  XSelectInput(display, rootWindow, SubstructureNotifyMask);

  Atom netWmName = XInternAtom(display, "_NET_WM_NAME", True);  // we want _NET_WM_NAME, not WM_NAME
  Atom type;
  int format;
  int error;
  unsigned long nItem, bytesAfter;

  XSetErrorHandler(error_handler);

  while (1) {
    XNextEvent(display, &e);

    // we want to catch this event (possibly also configure notify, but that's less certain)
    if (e.type == CreateNotify || e.type == ConfigureNotify) {
      unsigned char* properties = NULL;
      XGetWindowProperty(display, e.xcreatewindow.window, netWmName, 0, (~0L), False, AnyPropertyType, &type, &format, &nItem, &bytesAfter, &properties);
      // printf("_NET_WM_TITLE:%s\n", properties);

      // XTextProperty *windowTitle;
      // XGetWMName(display, e.xcreatewindow.window, windowTitle);
      // printf("WM_TITLE: %s\n", windowTitle->value);

      if (hasError) {
        hasError = 0;
        continue;
      }

      if (properties && strcmp(spectacleWindowName, properties) == 0) {
        XMoveWindow(display, e.xcreatewindow.window, 0, -0);
      }

      free(properties);
      // free(windowTitle);
    }
  }

  return 0;
}