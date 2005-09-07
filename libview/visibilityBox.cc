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
 * visibilityBox.cc --
 *
 *      Implementation of the view::VisibilityBox class. A VisibilityBox
 *      is conceptually a Gtk::Bin which manages its own visibility based on
 *      whether its child has any visible children (recursively) or not.
 *
 * The problem
 * -----------
 * Imagine the following widget hierarchy:
 *
 *    2 Gtk::Frame
 *    3    Gtk::Hbox
 *    4       Gtk::Image
 *    6       Gtk::HBox
 *    7          Gtk::Image
 *    8          Gtk::Image
 *
 * Initially, all widgets are shown and 3 uses a non-zero spacing.
 *
 * Now hide 7 and 8. Because 6 is still shown, 3 displays some spacing between
 * 4 and ... nothing, which looks ugly.
 *
 * Now hide 4. Because 3 and 2 are still shown, the user sees an empty frame,
 * which looks ugly.
 *
 * The solution
 * ------------
 * We could add ad-hoc visibility management code to take care of this. But it
 * is distracting, error-prone, and very often the widgets are provided by
 * different components, and we are reluctant to add visibility management APIs
 * between the components because such APIs are not clean:
 * o They have nothing to do with the actual purpose of the components.
 * o They tend to expose too much of the internal implementation of components.
 *
 * Enter the VisibilityBox. Just insert VisibilityBox widgets in the hierarchy
 * (1 and 5):
 *
 *    1 VisibilityBox
 *    2    Gtk::Frame
 *    3       Gtk::Hbox
 *    4          Gtk::Image
 *    5          VisibilityBox
 *    6             Gtk::HBox
 *    7                Gtk::Image
 *    8                Gtk::Image
 *
 * Initially, all widgets are shown and 3 uses a non-zero spacing.
 *
 * Now hide 7 and 8. 5 tracks these changes and hides itself, so 3 only
 * displays 4, without ugly spacing.
 *
 * Now hide 4. 1 tracks this change and hides itself, so nothing is displayed
 * at all.
 *
 * In summary
 * ----------
 * There are widgets that provide content (4, 7, 8) and there are widget that
 * layout content (2, 3, 6). There is no point in laying out content that does
 * not exist. GTK+ does not handle this well, and VisibilityBox remedies that
 * by providing a generic "insert-and-forget" solution which allow developers
 * to focus on more important things.
 *
 *   --hpreg
 */


#include <libview/defines.h>
#include <libview/visibilityBox.hh>


namespace view {


/*
 *-----------------------------------------------------------------------------
 *
 * view::VisibilityBox::VisibilityBox --
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

VisibilityBox::VisibilityBox(void)
   : mMode(TRACK),
     mChild(NULL),
     mTracking(false)
{
   mVisibilityChangedSlot =
      sigc::mem_fun(this, &VisibilityBox::UpdateVisibilityWhenTracking);
   mChildrenChangedSlot = sigc::hide(mVisibilityChangedSlot);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::VisibilityBox::Disconnect --
 *
 *      Disconnect from all signals we have previously connected to.
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
VisibilityBox::Disconnect(void)
{
   for (std::list<sigc::connection>::iterator i = mCnxs.begin();
        i != mCnxs.end(); i++) {
      (*i).disconnect();
   }
   mCnxs.clear();
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::VisibilityBox::IsVisible --
 *
 *      Determine the visibility of a descendant.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

bool
VisibilityBox::IsVisible(Gtk::Widget *widget) // IN
{
   if (!widget->gobj()) {
      /* The widget is being destroyed. */
      return false;
   }

   if (!widget->is_visible()) {
      mCnxs.insert(mCnxs.end(),
                   widget->signal_show().connect(mVisibilityChangedSlot));
      return false;
   }

   mCnxs.insert(mCnxs.end(),
                widget->signal_hide().connect(mVisibilityChangedSlot));

   Gtk::Container *container = dynamic_cast<Gtk::Container *>(widget);
   return (container && !dynamic_cast<VisibilityBox *>(widget))
      ? IsVisibilityContainerVisible(container) : true;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::VisibilityBox::IsVisibilityContainerVisible --
 *
 *      Determine the visibility of a visibility container. A visibility
 *      container is a Gtk::Container that contains (as in "prevents escape
 *      of") the visible state of its descendants. Regular GTK+ containers
 *      (Gtk::Table, Gtk::Box, ...) are visibility containers. A VisibilityBox
 *      is not a visibility container: it leaks some of the visible state of
 *      its descendants.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

bool
VisibilityBox::IsVisibilityContainerVisible(Gtk::Container *container) // IN
{
   /*
    * XXX As of GTK+ 2.4.13, the "add"/"remove" signals are only fired if
    *     the child is added/removed by calling gtk_container_[add|remove]().
    *     This means that there is absolutely no way to be notified when a
    *     child is added to a container via a more specialized function, for
    *     example when a child is added to a GtkHBox via gtk_box_pack_start().
    *
    *     This is a clear bug to me, and until it is fixed, it makes
    *     VisibilityBox harder to use. :( --hpreg
    */
   mCnxs.insert(mCnxs.end(),
                container->signal_add().connect(mChildrenChangedSlot));
   mCnxs.insert(mCnxs.end(),
                container->signal_remove().connect(mChildrenChangedSlot));

   Glib::ListHandle<Gtk::Widget *> children = container->get_children();
   for (Glib::ListHandle<Gtk::Widget *>::const_iterator i = children.begin();
        i != children.end(); i++) {
      if (IsVisible(*i)) {
         return true;
      }
   }

   return false;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::VisibilityBox::UpdateVisibilityWhenTracking --
 *
 *      Update the visibility of a VisibilityBox that is currently tracking.
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
VisibilityBox::UpdateVisibilityWhenTracking(void)
{
   g_assert(mTracking);
   Disconnect();
   IsVisible(mChild) ? show() : hide();
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::VisibilityBox::SetMode --
 *
 *      Set the "mode" property of a VisibiltyBox.
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
VisibilityBox::SetMode(Mode value) // IN
{
   mMode = value;
   UpdateVisibility();
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::VisibilityBox::on_add --
 *
 *      Overridden parent class method. Only allow for one child, and start
 *      tracking its visibility.
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
VisibilityBox::on_add(Gtk::Widget *widget) // IN
{
   g_assert(!mChild && widget);
   mChild = widget;
   UpdateVisibility();

   Gtk::HBox::on_add(widget);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::VisibilityBox::on_remove --
 *
 *      Overridden parent class method.
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
VisibilityBox::on_remove(Gtk::Widget *widget) // IN
{
   g_assert(mChild == widget);
   mChild = NULL;
   UpdateVisibility();

   Gtk::HBox::on_remove(widget);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::VisibilityBox::UpdateVisibility --
 *
 *      Update the visibility of a VisibilityBox.
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
VisibilityBox::UpdateVisibility(void)
{
   bool tracking = mChild && mMode == TRACK;
   if (mTracking != tracking) {
      mTracking = tracking;

      if (mTracking) {
         /* Start tracking. */
         UpdateVisibilityWhenTracking();
      } else {
         /* Stop tracking. */
         Disconnect();
      }
   }

   if (mTracking) {
      return;
   }

   /*
    * Update the visibility of a VisibilityBox that is not currently
    * tracking.
    */

   g_assert(!mTracking);
   bool visible = false;
   switch (mMode) {
   case TRACK:
      visible = mChild;
      break;
   case SHOW:
      visible = true;
      break;
   case HIDE:
      visible = false;
      break;
   default:
      g_assert_not_reached();
      break;
   }
   visible ? show() : hide();
}


} /* namespace view */
