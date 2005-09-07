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
 * reparenter.hh --
 *
 *      An object to workaround the multiple bugs in gtk_widget_reparent().
 */

#ifndef LIBVIEW_REPARENTER_HH
#   define LIBVIEW_REPARENTER_HH


#include <gtkmm/container.h>


namespace view {


class Reparenter
{
public:
   Reparenter(Gtk::Widget &widget);
   ~Reparenter();

   sigc::slot<void> Reparent(Gtk::Container &newParent);

private:
   static void RecurseQueueResize(Gtk::Widget &widget);

   void OnEvent(void);
   void OnWidgetSizeAllocate(void);
   void OnSlotCalled(sigc::trackable &trackable);

   Gtk::Widget &mWidget;
   sigc::connection mCnx;
   sigc::trackable *mTrackable;
   bool mWasMapped;
};


} /* namespace view */


#endif /* LIBVIEW_REPARENTER_HH */
