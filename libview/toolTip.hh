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
 * toolTip.hh --
 *
 *      A tooltip look-alike widget that can be shown on demand.
 */

#ifndef LIBVIEW_TOOLTIP_HH
#define LIBVIEW_TOOLTIP_HH


#include <gtkmm/window.h>

#include <libview/motionTracker.hh>


namespace view {


class ToolTip
   : public Gtk::Window
{
public:
   ToolTip(Gtk::Widget &target, const Glib::ustring &markup);

protected:
   bool on_button_press_event(GdkEventButton *event);
   bool on_expose_event(GdkEventExpose *event);
   void on_show(void);

private:
   bool OnTimeout(void);
   void UpdatePosition(void);

   Gtk::Widget &mTarget;
   MotionTracker mTracker;
};


} // namespace view


#endif // LIBVIEW_TOOLTIP_HH
