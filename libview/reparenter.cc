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
 * reparenter.cc --
 *
 *      Implements the Reparenter class.
 *
 * The problem
 * -----------
 * Unlike gtk_container_remove() + gtk_container_add(),
 * gtk_widget_reparent() tries to avoid unrealizing the widget. It is
 * important when the widget is a VmMks, because unrealizing it breaks the
 * connection to the MKS thread.
 *
 * Unfortunately, gtk_widget_reparent(widget, new parent) is poorly
 * implemented (see gtk_widget_reparent_subwindows()):
 * o If the widget has its own GDK window, then it is reparented to the GDK
 *   window of the new parent.
 * o If the widget does not have its own GDK window, then its children GDK
 *   windows are reparented to the GDK window of the new parent.
 * The catch is: the GDK windows _are also positionned at (0, 0)_ inside the
 * GDK window of the new parent.
 *
 * This implies that the user can visually see the GDK windows jump to the
 * wrong place.
 *
 * Normally this visual effect is just temporary (i.e. to the user it looks
 * like flicker) because the new parent imposes a different size to the
 * widget, so quickly after that, the size allocation code asynchronously
 * kicks in and properly repositions the GDK windows.
 *
 * Unfortunately, there is no guarantee the size allocation code will kick
 * in: the new parent could very well allocate the same size to the
 * widget as the old parent.
 *
 * In fact, this is precisely what happens when you rename a team in the
 * Favorites list, while watching the team's MultiMks: the widget is
 * synchronously reparented to a new parent, then back to its old parent.
 * Consequently, the final parent allocates exactly the same size as the
 * original parent, so the size allocation code does not kick in, and the GDK
 * windows stay at the wrong place!
 *
 * The workaround
 * --------------
 * 1) Instead of having to deal with the many children GDK window of the
 *    widget, we simplify the problem by restricting the client code to only
 *    pass us widgets that have their own GDK window.
 *
 * 2) Let's try to avoid the jump. Ideally, we want to hide the GDK window
 *    while it is at the wrong place, then let the allocation code give it its
 *    proper size and position, then show it again.
 *
 *    The problem is that for the size of the widget to be allocated, it needs
 *    to be visible, and to make it visible we have to use show(), but show()
 *    will show the window at the wrong position, even if only for a very short
 *    time.
 *
 *    So the trick is to lie to GTK: we let it think that the window is
 *    visible, but in secret we ask GDK to hide it.
 *
 * 3) Forcefully mark all the widget's GTK children as needing a size
 *    re-allocation.
 *
 *  --hpreg
 */


#include <libview/defines.h>
#include <libview/reparenter.hh>


namespace view {


/*
 *-----------------------------------------------------------------------------
 *
 * view::Reparenter::Reparenter --
 *
 *      Constructor of a Reparenter.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

Reparenter::Reparenter(Gtk::Widget &widget) // IN
   : mWidget(widget),
     mTrackable(NULL)
{
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::Reparenter::~Reparenter --
 *
 *      Destructor of a Reparenter.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

Reparenter::~Reparenter()
{
   delete mTrackable;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::Reparenter::OnEvent --
 *
 *      Called when a event we are waiting on occurs.
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
Reparenter::OnEvent(void)
{
   if (mCnx || mTrackable) {
      /* Continue to wait until both events have occured. */
      return;
   }

   /* Time to complete workaround 2. */

   if (mWasMapped && !mWidget.has_no_window() && mWidget.is_mapped()) {
      g_assert(mWidget.is_realized());

      /*
       * Now that the widget has been allocated, its GDK window has been
       * properly repositionned, and we do not need to hide it anymore. Show
       * it without modifying its stacking order.
       */
      mWidget.get_window()->show_unraised();
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::Reparenter::OnWidgetSizeAllocate --
 *
 *      "size_allocate" signal handler for a Reparenter's widget.
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
Reparenter::OnWidgetSizeAllocate(void)
{
   mCnx.disconnect();

   OnEvent();
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::Reparenter::OnSlotCalled --
 *
 *      Called when the slot returned by the last Reparent() call is invoked.
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
Reparenter::OnSlotCalled(sigc::trackable &trackable) // IN
{
   g_assert(&trackable == mTrackable);
   delete mTrackable;
   mTrackable = NULL;

   OnEvent();
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::Reparenter::RecurseQueueResize --
 *
 *      Recursively call queue_resize() on 'widget' and its children (if any).
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
Reparenter::RecurseQueueResize(Gtk::Widget &widget) // IN
{
   widget.queue_resize();

   Gtk::Container *container = dynamic_cast<Gtk::Container *>(&widget);
   if (container) {
      container->foreach(sigc::ptr_fun(RecurseQueueResize));
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::Reparenter::Reparent --
 *
 *      Reparent a Reparenter's widget in 'newParent'.
 *
 * Results:
 *      A slot that client code must invoke when it is ready for the GDK window
 *      to be shown again.
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

sigc::slot<void>
Reparenter::Reparent(Gtk::Container &newParent) // IN
{
   /*
    * Workaround 1: We check that property every time this method is
    * called instead of once in the constructor, because that property can
    * change over time (see gtk_event_box_set_visible_window() for example).
    */
   g_assert(!mWidget.has_no_window());

   mCnx.disconnect();
   delete mTrackable;
   mTrackable = NULL;

   mWasMapped = mWidget.is_mapped();
   if (mWasMapped) {
      g_assert(mWidget.is_realized());

      /*
       * Workaround 2: It is OK not to tell GTK (i.e. not to change the
       * result of is_mapped()), because unmapping a GDK window is an
       * idempotent operation.
       */
      mWidget.get_window()->hide();
      mWidget.get_display()->sync();
   }

   mCnx = mWidget.signal_size_allocate().connect(
      sigc::hide(sigc::mem_fun(this, &Reparenter::OnWidgetSizeAllocate)));
   mTrackable = new sigc::trackable();

   mWidget.reparent(newParent);

   /* Workaround 3 */
   RecurseQueueResize(mWidget);

   /*
    * We purposedly bind a reference to the sigc::trackable object to the slot,
    * so we can invalidate the slot (i.e. make sure we won't be called back) at
    * any time by destroying the object.
    */
   return sigc::bind(sigc::mem_fun(this, &Reparenter::OnSlotCalled),
                     sigc::ref(*mTrackable));
}


} /* namespace view */
