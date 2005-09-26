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
 * contentBox.cc --
 *
 *      Implementation of the view::ContentBox class. A ContentBox
 *      is conceptually a Gtk::Bin which manages its own visibility based on
 *      whether its child has actual content to show (i.e. any visible
 *      children, recursively) or not.
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
 * is distracting, error-prone, and very often the widgets in this hierarchy
 * are provided by different components, and we are reluctant to add visibility
 * management APIs between the components because such APIs are not clean:
 * o They have nothing to do with the actual purpose of the components.
 * o They tend to expose too much of the internal implementation of components.
 *
 * Enter the ContentBox. Just insert ContentBox widgets in the hierarchy
 * (1 and 5):
 *
 *    1 ContentBox
 *    2    Gtk::Frame
 *    3       Gtk::Hbox
 *    4          Gtk::Image
 *    5          ContentBox
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
 * There are widgets that provide actual content to show (4, 7, 8) and there
 * are widget that simply layout that content (2, 3, 6). There is no point in
 * laying out content that does not exist. GTK+ does not handle this well, and
 * ContentBox remedies that by providing a generic "insert-and-forget" solution
 * which allow developers to focus on more important things.
 *
 *   --hpreg
 */


#include <libview/contentBox.hh>


namespace view {


/*
 *-----------------------------------------------------------------------------
 *
 * view::ContentBox::ContentBox --
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

ContentBox::ContentBox(void)
   : mMode(TRACK),
     mChild(NULL),
     mTracking(false)
{
   mVisibilityChangedSlot =
      sigc::mem_fun(this, &ContentBox::UpdateVisibilityWhenTracking);
   mChildrenChangedSlot = sigc::hide(mVisibilityChangedSlot);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::ContentBox::Disconnect --
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
ContentBox::Disconnect(void)
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
 * view::ContentBox::WidgetHasContent --
 *
 *      Determine if a Gtk::Widget descendant has actual content to show.
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
ContentBox::WidgetHasContent(Gtk::Widget *widget) // IN
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

   /*
    * Optimization: A ContentBox is a container, but it already sets its
    * visibility according to whether it has actual content to show or not. So
    * there is no need to recurse through it to determine that.
    */

   Gtk::Container *container = dynamic_cast<Gtk::Container *>(widget);
   return (container && !dynamic_cast<ContentBox *>(widget))
      ? ContainerHasContent(container) : true;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::ContentBox::ContainerHasContent --
 *
 *      Determine if a Gtk::Container descendant has actual content to show.
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
ContentBox::ContainerHasContent(Gtk::Container *container) // IN
{
   /*
    * XXX As of GTK+ 2.4.13, the "add"/"remove" signals are only fired if
    *     the child is added/removed by calling gtk_container_[add|remove]().
    *     This means that there is absolutely no way to be notified when a
    *     child is added to a container via a more specialized function, for
    *     example when a child is added to a GtkHBox via gtk_box_pack_start().
    *
    *     This is a clear bug to me, and until it is fixed, it makes
    *     ContentBox less convenient to use in non-Glade code. :( --hpreg
    */
   mCnxs.insert(mCnxs.end(),
                container->signal_add().connect(mChildrenChangedSlot));
   mCnxs.insert(mCnxs.end(),
                container->signal_remove().connect(mChildrenChangedSlot));

   Glib::ListHandle<Gtk::Widget *> children = container->get_children();
   for (Glib::ListHandle<Gtk::Widget *>::const_iterator i = children.begin();
        i != children.end(); i++) {
      if (WidgetHasContent(*i)) {
         return true;
      }
   }

   return false;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::ContentBox::UpdateVisibilityWhenTracking --
 *
 *      Update the visibility of a ContentBox that is currently tracking.
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
ContentBox::UpdateVisibilityWhenTracking(void)
{
   g_assert(mTracking);
   Disconnect();
   WidgetHasContent(mChild) ? show() : hide();
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::ContentBox::SetMode --
 *
 *      Set the "mode" property of a ContentBox.
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
ContentBox::SetMode(Mode value) // IN
{
   mMode = value;
   UpdateVisibility();
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::ContentBox::on_add --
 *
 *      Overridden parent class method. Only allow for one child.
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
ContentBox::on_add(Gtk::Widget *widget) // IN
{
   g_assert(!mChild && widget);
   mChild = widget;
   UpdateVisibility();

   Gtk::HBox::on_add(widget);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::ContentBox::on_remove --
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
ContentBox::on_remove(Gtk::Widget *widget) // IN
{
   g_assert(mChild == widget);
   mChild = NULL;
   UpdateVisibility();

   Gtk::HBox::on_remove(widget);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::ContentBox::UpdateVisibility --
 *
 *      Update the visibility of a ContentBox.
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
ContentBox::UpdateVisibility(void)
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

   /* Update the visibility of a ContentBox that is not currently tracking. */

   g_assert(!mTracking);
   bool visible = false;
   switch (mMode) {
   case TRACK:
   case HIDE:
      break;
   case SHOW:
      visible = true;
      break;
   default:
      g_assert_not_reached();
      break;
   }
   visible ? show() : hide();
}


} /* namespace view */
