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
 * visibilityBox.hh --
 *
 *      Declarations for the view::VisibilityBox class.
 */

#ifndef LIBVIEW_VISIBILITYBOX_HH
#   define LIBVIEW_VISIBILITYBOX_HH


#include <gtkmm/box.h>
#include <sigc++/connection.h>
#include <list>


namespace view {


class VisibilityBox
   : public Gtk::HBox
{
public:
   enum Mode {
      TRACK,
      SHOW,
      HIDE,
   };

   VisibilityBox(void);
   void SetMode(Mode value);

protected:
   /* Re-implemented Gtk::Container methods. */
   void on_add(Gtk::Widget *widget);
   void on_remove(Gtk::Widget *widget);

private:
   void Disconnect(void);
   void UpdateVisibility(void);
   void UpdateVisibilityWhenTracking(void);
   bool IsVisible(Gtk::Widget *target);
   bool IsVisibilityContainerVisible(Gtk::Container *target);

   Mode mMode;
   Gtk::Widget *mChild;
   bool mTracking;
   std::list<sigc::connection> mCnxs;
   sigc::slot<void> mVisibilityChangedSlot;
   sigc::slot<void, Gtk::Widget *> mChildrenChangedSlot;
};


} /* namespace view */


#endif /* LIBVIEW_VISIBILITYBOX_HH */
