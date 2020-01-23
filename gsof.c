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

  XEvent e;
  Window rootWindow = DefaultRootWindow(display);
  XSelectInput(display, rootWindow, SubstructureNotifyMask);

  Atom netWmName = XInternAtom(display, "_NET_WM_NAME", True);  // we want _NET_WM_NAME, not WM_NAME
  Atom wmClass = XInternAtom(display, "WM_CLASS", True);
  Atom wmNormalHints = XInternAtom(display, "WM_NORMAL_HINTS", True);
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
      unsigned char* classProperties = NULL; 
      XGetWindowProperty(display, e.xcreatewindow.window, netWmName, 0, (~0L), False, AnyPropertyType, &type, &format, &nItem, &bytesAfter, &properties);
      XGetWindowProperty(display, e.xcreatewindow.window, wmClass,   0, (~0L), False, AnyPropertyType, &type, &format, &nItem, &bytesAfter, &classProperties);
      // printf("_NET_WM_TITLE:%s, classProperties:%s;\n", properties, classProperties);

      // XTextProperty *windowTitle;
      // XGetWMName(display, e.xcreatewindow.window, windowTitle);
      // printf("WM_TITLE: %s\n", windowTitle->value);

      if (hasError) {
        hasError = 0;
        continue;
      }

      // this catches spectacle -sc --region
      if (properties != NULL) {
        if (strcmp("Spectacle", properties) == 0) {
          XMoveWindow(display, e.xcreatewindow.window, 0, 0);
        }
      };

      // this catches spectacle --region (and seelcting rectangular region from gui)
      if (properties == NULL) {
        if (classProperties && strcmp("spectacle", classProperties) == 0) {
          XMoveWindow(display, e.xcreatewindow.window, 0, 0);
        }
      }

      free(properties);
      free(classProperties);
      // free(windowTitle);
    }
  }

  return 0;
}