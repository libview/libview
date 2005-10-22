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
 * utils.cc --
 *
 *      A set of general utility functions.
 */


#include <libview/utils.hh>


namespace view {
namespace utils {


/*
 *-----------------------------------------------------------------------------
 *
 * view::utils::GetMaxCharWidth --
 *
 *      Returns the widest character in the set of passed characters,
 *      taking into account the style used in the specified widget.
 *
 * Results:
 *      The width of the widest character.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

size_t
GetLargestCharStrWidth(Gtk::Widget& widget,    // IN: The target widget
                       Glib::ustring& charset, // IN: The set of characters
                       size_t numDups)         // IN: Number of chars to dup.
{
   g_return_val_if_fail(numDups > 0, 0);

   size_t maxWidth = 0;
   Glib::RefPtr<Pango::Layout> layout = widget.create_pango_layout("");

   for (int i = 0; i < charset.length(); i++) {
      layout->set_text(Glib::ustring(numDups, charset[i]));

      int width;
      int height;
      layout->get_pixel_size(width, height);

      if (width > maxWidth) {
         maxWidth = width;
      }
   }

   return maxWidth;
}


} /* namespace utils */
} /* namespace view */
