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
 * baseBGBox.cc --
 *
 *      An HBox that draws its background using the base colour for
 *      the current state. How exciting.
 *
 *      Now, because of the tortuous journey to get here, I'm including
 *      the summary of why this class works the way it does:
 *
 *      One officially sanctioned use of EventBoxes is to set a background
 *      colour for a child widget. However, many themes ignore, abuse and
 *      otherwise harrass event boxes and prevent them from fullfilling this
 *      basic task.
 *
 *      Examples:
 *      1) crux and smokey-blue deliberately force the appearance of the
 *         event box to be identical with the parent widget.
 *      2) gtk-qt-engine abuses the symbolic colours, so they all look
 *         the same - at least in an event box.
 *
 *      So, what's the solution? Don't use an event box. Based on my
 *      reading of the gtk-qt-engine code, I worked out that if you
 *      instead take a non-window widget and do a draw_rectangle()
 *      inside its allocation, then the colour wouldn't get mangled.
 *      The added bonus is that because we no-longer inherit from
 *      EventBox, crux and smokey-blue will not override our rendering
 *      style.
 */

#include <libview/baseBGBox.hh>


namespace view {


/*
 *-----------------------------------------------------------------------------
 *
 * view::BaseBGBox::BaseBGBox --
 *
 *      Constructor.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

BaseBGBox::BaseBGBox(Palette palette) // IN
   : mPalette(palette)
{
}


/*
 *-------------------------------------------------------------------
 *
 * view::BaseBGBox::on_expose_event --
 *
 *      Expose event handler. Paint the background of the widget using
 *      the current state's base colour.
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
BaseBGBox::on_expose_event(GdkEventExpose *event) // IN
{
   if (is_drawable()) {
      const Gtk::Allocation allocation(get_allocation());

      // Don't cache the gc to avoid worrying about style changes.
      Glib::RefPtr<Gtk::Style> style = get_style();
      Gtk::StateType state = get_state();
      Glib::RefPtr<Gdk::GC> gc;
      switch (mPalette) {
      case BASE:
         gc = style->get_base_gc(state);
         break;
      case BG:
         gc = style->get_bg_gc(state);
         break;
      case FG:
         gc = style->get_fg_gc(state);
         break;
      default:
         g_assert_not_reached();
      }
      get_window()->draw_rectangle(gc, true,
                                   allocation.get_x(), allocation.get_y(),
                                   allocation.get_width(), allocation.get_height());
   }
   return Gtk::HBox::on_expose_event(event);
}


} // namespace view
