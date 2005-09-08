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
 * deadEntry.cc --
 *
 *     An entry widget that sets itself greyed out when not editable.
 */


#include "deadEntry.hh"


namespace view {


/*
 *-----------------------------------------------------------------------------
 *
 * view::DeadEntry::DeadEntry --
 *
 *      Constructor.  Listens for "editable" property changes, connects to
 *      "style_changed" signal, and sets the initial editable value to %false.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

DeadEntry::DeadEntry(void)
   : Gtk::Entry(),
     mStyleChangedRunning(false)
{
   property_editable().signal_changed().connect(
	sigc::mem_fun(this, &DeadEntry::EditableChanged));

   signal_style_changed().connect(
	sigc::mem_fun(this, &DeadEntry::StyleChanged));

   set_editable(false);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::DeadEntry::EditableChanged --
 *
 *      "editable" Property change notifier.  If editable is %false, applies a
 *      "greyed-out background" to the entry's frame by using the theme's
 *      insensitive text base color and insensitive text color.
 *
 *      NOTE: Some themes like Simple and Mist have bugs whereby desensitized
 *      entrys don't change color, so these changes will have no visual effect.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

void
DeadEntry::EditableChanged(void)
{
   if (get_editable()) {
      /* Reset the colors */
      unset_base(Gtk::STATE_NORMAL);
      unset_text(Gtk::STATE_NORMAL);
   } else {
      ensure_style();

      modify_base(Gtk::STATE_NORMAL,
		  get_style()->get_base(Gtk::STATE_INSENSITIVE));
      modify_text(Gtk::STATE_NORMAL,
		  get_style()->get_text(Gtk::STATE_INSENSITIVE));
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::DeadEntry::StyleSet --
 *
 *      "style_set" Signal handler.  Calls EditableChanged() to reapply style
 *      modifications using the new theme.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

void
DeadEntry::StyleChanged(const Glib::RefPtr<Gtk::Style> &oldStyle) // IN
{
   /*
    * modify_base/text() emit another style_set signal. So avoid a recursion
    * crash...
    */
   if (!mStyleChangedRunning) {
      mStyleChangedRunning = true;
      EditableChanged();
      mStyleChangedRunning = false;
   }
}


} /* namespace view */
