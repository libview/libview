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
 * toolTip.cc --
 *
 *      A tooltip look-alike widget that can be shown on demand.
 */


#include <gtkmm/label.h>

#include <libview/toolTip.hh>


namespace view {


/*
 *-------------------------------------------------------------------
 *
 * view::ToolTip::ToolTip --
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

ToolTip::ToolTip(Gtk::Widget &target,         // IN
                 const Glib::ustring &markup) // IN
   : Gtk::Window(Gtk::WINDOW_POPUP),
     mTarget(target),
     mTracker(target)
{
   // This is how a GtkTooltip window is set up.
   set_app_paintable(true);
   set_resizable(false);
   set_name("gtk-tooltips");
   set_border_width(4);

   // We need to intercept this to allow us to delete the tooltip when clicked.
   add_events(Gdk::BUTTON_PRESS_MASK);

   Gtk::Label *label = Gtk::manage(new Gtk::Label());
   label->show();
   add(*label);
   label->set_markup(markup);
   label->set_line_wrap(true);
   label->set_alignment(0.5, 0.5);

   mTracker.connect(sigc::mem_fun(this, &ToolTip::UpdatePosition));
}


/*
 *-------------------------------------------------------------------
 *
 * view::ToolTip::on_button_press_event --
 *
 *      Button press event handler. Deletes the tip.
 *
 * Results:
 *      true - event is handled.
 *
 * Side effects:
 *      Widget is deleted.
 *
 *-------------------------------------------------------------------
 */

bool
ToolTip::on_button_press_event(GdkEventButton *event) // IN
{
   Gtk::Window::on_button_press_event(event);

   delete this;
   return true;
}


/*
 *-------------------------------------------------------------------
 *
 * view::ToolTip::on_expose_event --
 *
 *      Expose event handler. Paint the window like a tooltip.
 *
 * Results:
 *      Result of parent handler.
 *
 * Side effects:
 *      None
 *
 *-------------------------------------------------------------------
 */

bool
ToolTip::on_expose_event(GdkEventExpose *event) // IN
{
   /*
    * It is legitmate to wonder why the size request is being used
    * here rather than the allocation. The simple answer is that
    * that is what GtkTooltips does. In practice, this is ok because
    * the window manager should never prevent the tooltip's window
    * being allocated at the requested size. But if we ever see
    * problems related to that, you know what to do. :-) --plangdale
    */
   Gtk::Requisition req;
   size_request(req);

   get_style()->paint_flat_box(get_window(),
                               Gtk::STATE_NORMAL,
                               Gtk::SHADOW_OUT,
                               Gdk::Rectangle(),
                               *this,
                               "tooltip",
                               0,
                               0,
                               req.width,
                               req.height);

   return Gtk::Window::on_expose_event(event);
}


/*
 *-------------------------------------------------------------------
 *
 * view::ToolTip::on_show --
 *
 *      Show event handler. Timer is set to delete widget later.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      Widget will be deleted by timeout later.
 *
 *-------------------------------------------------------------------
 */

void
ToolTip::on_show(void)
{
   UpdatePosition();

   Gtk::Window::on_show();

   Glib::signal_timeout().connect(
      sigc::mem_fun(this, &ToolTip::OnTimeout), 5000);
}


/*
 *-------------------------------------------------------------------
 *
 * view::ToolTip::OnTimeout --
 *
 *      Timeout callback. Deletes widget.
 *
 * Results:
 *      false to prevent timeout being re-queued.
 *
 * Side effects:
 *      None
 *
 *-------------------------------------------------------------------
 */

bool
ToolTip::OnTimeout(void)
{
   delete this;
   return false;
}


/*
 *-------------------------------------------------------------------
 *
 * view::ToolTip::UpdatePosition --
 *
 *      Reposition the tip in the right place relative to the target
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
ToolTip::UpdatePosition(void)
{
   Gtk::Requisition requisition;
   size_request(requisition);
   int w = requisition.width;
   int h = requisition.height;

   /*
    * The positioning algorithm comes from GtkTooltips with some
    * minor simplifications because we don't care where the mouse
    * pointer is. I'm fully annotating it because it's not terribly
    * intuitive. --plangdale
    */

   // x,y are initialised to the widget's window's x,y.
   int x, y;
   mTarget.get_window()->get_origin(x, y);

   Gtk::Allocation allocation = mTarget.get_allocation();

   // x,y are offset for non-window widgets to get the widget's x,y in root co-ordinates.
   x += allocation.get_x();
   y += allocation.get_y();

   // x is shifted to the centre of the widget.
   x += allocation.get_width() / 2;

   /*
    * x is shifted to the left by the tooltip's centre point + 4,
    * so the tooltip is now 4 pixels to the right of where it would be
    * if the widget centre-line was aligned with the tooltip's centre-line.
    */
   x -= (w / 2 + 4);

   /*
    * Now, the ideal x co-ordinate has been established, but we must
    * verify if it is acceptable given screen constraints.
    */
   Glib::RefPtr<Gdk::Screen> screen = mTarget.get_screen();
   Gdk::Rectangle monitor;
   screen->get_monitor_geometry(screen->get_monitor_at_point(x, y), monitor);

   /*
    * If the right edge of the tooltip is off the right edge of
    * the screen, move x to the left as much as is needed to
    * get the tooltip in the screen.
    *   or
    * If the left edge of the tooltip is off the left edge of
    * the screen, move x to the right as much as is needed to
    * get the tooltip on the screen. 
    */
   if ((x + w) > monitor.get_x() + monitor.get_width())
   {
      x -= (x + w) - (monitor.get_x() + monitor.get_width());
   } else if (x < monitor.get_x()) {
      x = monitor.get_x();
   }

   /*
    * If the position of the bottom of the tooltip, if placed in
    * the ideal location, would be off the bottom of the screen,
    * then flip the tooltip to be 4 pixels above the widget.
    *   or
    * Put it in the ideal location, 4 pixels below the widget.
    */
   if ((y + h + allocation.get_height() + 4) > monitor.get_y() + monitor.get_height()) {
      y = y - h - 4;
   } else {
      y = y + allocation.get_height() + 4;
   }

   move(x, y);
}


} // namespace view
