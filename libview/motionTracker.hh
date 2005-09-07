/* *************************************************************************
 * Copyright (c) 2005 VMware, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * *************************************************************************/

/*
 * motionTracker.hh --
 *
 *      The motion tracker is a signal object that will emit whenever the
 *      widget it is tracking moves. This is necessary because gtk does
 *      not provide a nice mechanism to do this, so we must encapsulate
 *      the evilness.
 *
 *      As the motion tracker is just a specialised signal, it can be added
 *      as a member of a widget class so that motion notification appears
 *      as just another notifiable event on the widget.
 *
 *      If the target happens to disappear while the tracker is still alive,
 *      it will completely disconnect itself from all windows and become inert.
 */

#ifndef LIBVIEW_MOTIONTRACKER_HH
#define LIBVIEW_MOTIONTRACKER_HH


#include <gtkmm/widget.h>

#include "libview/weakPtr.hh"

namespace view {


class MotionTracker
   : public sigc::signal<void>
{
public:
   MotionTracker(Gtk::Widget &target);
   ~MotionTracker();

private:
   typedef std::vector<WeakPtr<Gdk::Window> > WindowVector;

   void ConnectWindows(void);
   void DisconnectWindows(void);
   void ReconnectWindows(void);

   static GdkFilterReturn OnXEvent(GdkXEvent *gdkXEvent, GdkEvent *event,
                                   gpointer data);

   Gtk::Widget &mTarget;
   WindowVector mWindows;
};


} // namespace view

#endif // LIBVIEW_MOTIONTRACKER_HH

