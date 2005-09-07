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
 * viewport.cc --
 *
 *      Viewport subclass that fixes bugs in the base size request handler.
 */

#include <libview/viewport.hh>

namespace view {


/*
 *-----------------------------------------------------------------------------
 *
 * view::Viewport::Viewport --
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

Viewport::Viewport(Gtk::Adjustment &hadjustment, // IN
                   Gtk::Adjustment &vadjustment) // IN
   : Gtk::Viewport(hadjustment, vadjustment)
{

}


/*
 *-----------------------------------------------------------------------------
 *
 * view::Viewport::on_size_request --
 *
 *      Override signal handler to fix Gtk+ bugs that cause the size request
 *      to be inaccurate.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

void
Viewport::on_size_request(Gtk::Requisition *requisition) // OUT
{
   Gtk::Viewport::on_size_request(requisition);

   /*
    * XXX In all GTK+ versions between 1.2 and 2.4,
    *     gtk_viewport_size_request():
    *     o Requests room for the shadow even if there is no shadow.
    *     o Accounts for the border_width 4 times instead of 2 in the height.
    *     We workaround these issues here. --hpreg
    */
   if (get_shadow_type() == Gtk::SHADOW_NONE) {
      Glib::RefPtr<Gtk::Style> style = get_style();
      requisition->width -= 2 * style->get_xthickness();
      requisition->height -= 2 * style->get_ythickness();
   }
   requisition->height -= 2 * get_border_width();
}


} // namespace view

