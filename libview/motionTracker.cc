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
 * motionTracker.cc --
 *
 *      The motion tracker is a signal object that will emit whenever the
 *      widget it is tracking moves. This is necessary because gtk does
 *      not provide a nice mechanism to do this, so we must encapsulate
 *      the evilness.
 */


#include <libview/motionTracker.hh>
#include <X11/Xlib.h>


namespace view {


/*
 *-------------------------------------------------------------------
 *
 * view::MotionTracker::MotionTracker --
 *
 *      Constructor.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-------------------------------------------------------------------
 */

MotionTracker::MotionTracker(Gtk::Widget &target) // IN
   : mTarget(target)
{
   mTarget.signal_unrealize().connect_notify(
      sigc::mem_fun(this, &MotionTracker::DisconnectWindows));
   mTarget.signal_realize().connect_notify(
      sigc::mem_fun(this, &MotionTracker::ReconnectWindows));
   ConnectWindows();

   mTarget.signal_size_allocate().connect_notify(sigc::hide(make_slot()));
}


/*
 *-------------------------------------------------------------------
 *
 * view::MotionTracker::~MotionTracker --
 *
 *      Destructor.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-------------------------------------------------------------------
 */

MotionTracker::~MotionTracker()
{
   DisconnectWindows();
}


/*
 *-------------------------------------------------------------------
 *
 * view::MotionTracker::ConnectWindows --
 *
 *      Add event filters to all parent GdkWindows of the target
 *      widget.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-------------------------------------------------------------------
 */

void
MotionTracker::ConnectWindows(void)
{
   Glib::RefPtr<Gdk::Window> window = mTarget.get_window();
   while (window) {
      window->add_filter(MotionTracker::OnXEvent, this);

      /*
       * We don't need to know directly that a parent window has disappeared.
       * We only need to know to skip the non-existent window when disconnecting.
       */
      mWindows.push_back(WeakPtr<Gdk::Window>(window.operator->()));

      window = window->get_parent();
   }
}


/*
 *-------------------------------------------------------------------
 *
 * view::MotionTracker::DisconnectWindows --
 *
 *      Remove event filters from parent GdkWindows - if they're still
 *      around.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-------------------------------------------------------------------
 */

void
MotionTracker::DisconnectWindows(void)
{
   for (WindowVector::size_type i = 0; i < mWindows.size(); i++) {
      if (mWindows[i]) {
         mWindows[i]->remove_filter(MotionTracker::OnXEvent, this);
      }
   }
   mWindows.clear();
}


/*
 *-------------------------------------------------------------------
 *
 * view::MotionTracker::ReconnectWindows --
 *
 *      Helper to do a disconnect/connect cycle. 
 *
 * Results:
 *      None
 *
 * Side effects:
 *      Anything - the signal emits itself.
 *
 *-------------------------------------------------------------------
 */

void
MotionTracker::ReconnectWindows(void)
{
   DisconnectWindows();
   ConnectWindows();

   // We should explicitly notify after this change.
   emit();
}


/*
 *-------------------------------------------------------------------
 *
 * view::MotionTracker::OnXEvent --
 *
 *      Filter callback to be attached to parent X windows. We use this
 *      to respond to X events that GDK hides from us.
 *
 * Results:
 *      GDK_FILTER_CONTINUE - to allow for full handling of the event.
 *
 * Side effects:
 *      Anything - the signal emits itself if a ConfigureNotify
 *      event is received.
 *
 *-------------------------------------------------------------------
 */

GdkFilterReturn
MotionTracker::OnXEvent(GdkXEvent *gdkXEvent, // IN
                        GdkEvent *event,      // IN
                        gpointer data)        // IN
{
   MotionTracker *tracker = reinterpret_cast<MotionTracker *>(data);

   /*
    * Both ConfigureNotify and ReparentNotify will be received if
    * StructureNotifyMask is in the X window event mask. GDK explicitly
    * sets this, so we know we will see both types of event here.
    */
   XEvent *xEvent = reinterpret_cast<XEvent *>(gdkXEvent);
   if (xEvent->type == ConfigureNotify) {
      /*
       * GDK discards ConfigureNotify for all but toplevel windows.
       */
      tracker->emit();
   } else if (xEvent->type == ReparentNotify) {
      /*
       * GDK does not translate ReparentNotify. This is the only place
       * to detect it.
       */
      tracker->ReconnectWindows();
   }

   return GDK_FILTER_CONTINUE;
}


} // namespace view
