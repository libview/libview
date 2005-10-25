/* *************************************************************************
 * Copyright (c) 2005 VMware Inc.
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
 * autoDrawer.c -
 *
 *    Subclass of ViewDrawer that encapsulates the behaviour typically required
 *    when using the drawer to implement a menu/toolbar that auto-opens when
 *    moused-over and auto-closes when the mouse leaves.
 */


#include <libview/autoDrawer.h>


struct _ViewAutoDrawerPrivate
{
   gboolean active;
   gboolean pinned;
   gboolean inputUngrabbed;

   guint delayConnection;
   guint delayValue;
   guint overlapPixels;
   guint noOverlapPixels;

   GtkEventBox *evBox;
};

#define VIEW_AUTODRAWER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), VIEW_TYPE_AUTODRAWER, ViewAutoDrawerPrivate))

/* The unaltered parent class. */
static ViewDrawerClass *parentClass;


/*
 *-----------------------------------------------------------------------------
 *
 * ViewAutoDrawerUpdate --
 *
 *      Update the drawer state.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

static void
ViewAutoDrawerUpdate(ViewAutoDrawer *that, // IN
                     gboolean immediate)   // IN
{
   gboolean show;
   double fraction;

   ViewAutoDrawerPrivate *priv = that->priv;
   GtkWidget *widget = GTK_WIDGET(priv->evBox);
   GtkWidget *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(that));

   if (!toplevel || !GTK_WIDGET_TOPLEVEL(toplevel)) {
      // The autoDrawer cannot function properly without a toplevel.
      return;
   }

   if (!priv->active) {
      ViewOvBox_SetMin(VIEW_OV_BOX(that), -1);
      ViewOvBox_SetFraction(VIEW_OV_BOX(that), 0);
      return;
   }

   if (priv->pinned) {
      show = TRUE;
   } else if (priv->inputUngrabbed) {
      int x, y;

      g_assert(gtk_container_get_border_width(GTK_CONTAINER(widget)) == 0);

      gtk_widget_get_pointer(widget, &x, &y);

      show = (guint)x < (guint)widget->allocation.width &&
             (guint)y < (guint)widget->allocation.height;
   } else {
      GtkWindow *window;
      GtkWidget *grabbed = NULL;

      window = GTK_WINDOW(toplevel);
      if (window->group && window->group->grabs) {
         grabbed = GTK_WIDGET(window->group->grabs->data);
      }
      if (!grabbed) {
         grabbed = gtk_grab_get_current();
      }
      g_assert(grabbed);

      if (GTK_IS_MENU(grabbed)) {
         grabbed = gtk_menu_get_attach_widget(GTK_MENU(grabbed));
         g_return_if_fail(grabbed);
      }
      show = gtk_widget_is_ancestor(grabbed, widget);
   }

   ViewOvBox_SetMin(VIEW_OV_BOX(that), priv->noOverlapPixels);

   fraction = show ? 1 : (double)priv->overlapPixels / widget->allocation.height;

   if (G_UNLIKELY(immediate)) {
      ViewOvBox_SetFraction(VIEW_OV_BOX(that), fraction);
   } else {
      ViewDrawer_SetGoal(VIEW_DRAWER(that), fraction);
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewAutoDrawerOnUpdateDelay --
 *
 *      Callback fired when a delayed update happens to update the drawer state.
 *
 * Results:
 *      FALSE to indicate timer should not repeat.
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

static gboolean
ViewAutoDrawerOnUpdateDelay(ViewAutoDrawer *that) // IN
{
   that->priv->delayConnection = 0;
   ViewAutoDrawerUpdate(that, FALSE);

   return FALSE;
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewAutoDrawerScheduleUpdate --
 *
 *      Schedule a delayed update of the drawer state. Any pending updates are
 *      canceled first.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

static void
ViewAutoDrawerScheduleUpdate(ViewAutoDrawer *that) // IN
{
   ViewAutoDrawerPrivate *priv = that->priv;

   if (priv->delayConnection) {
      g_source_remove(priv->delayConnection);
   }

   priv->delayConnection = g_timeout_add(priv->delayValue,
                                         (GSourceFunc)ViewAutoDrawerOnUpdateDelay,
                                         that);
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewAutoDrawerOnOverEnterLeave --
 *
 *      Respond to enter/leave events by doing a delayed update of the drawer
 *      state.
 *
 * Results:
 *      FALSE to indicate event was not handled.
 *
 * Side effects:
 *      Will queue delayed update.
 *
 *-----------------------------------------------------------------------------
 */

static gboolean
ViewAutoDrawerOnOverEnterLeave(GtkEventBox *evBox,      // IN
                               GdkEventCrossing *event, // IN
                               ViewAutoDrawer *that)    // IN
{
   ViewAutoDrawerScheduleUpdate(that);
   return FALSE;
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewAutoDrawerOnGrabNotify --
 *
 *      Respond to grab notifications by updating the drawer state.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      Might queue delayed update.
 *
 *-----------------------------------------------------------------------------
 */

static void
ViewAutoDrawerOnGrabNotify(GtkEventBox *evBox,   // IN
                           gboolean ungrabbed,   // IN
                           ViewAutoDrawer *that) // IN
{
   ViewAutoDrawerPrivate *priv = that->priv;

   priv->inputUngrabbed = ungrabbed;
   /*
    * We want to update immediately on a grab so the over widget immediately
    * appears along with the grabbing widget, but we delay on ungrab to be
    * friendly.
    */
   priv->inputUngrabbed ? ViewAutoDrawerScheduleUpdate(that) : 
                          ViewAutoDrawerUpdate(that, TRUE);
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewAutoDrawerOnHierarchyChanged --
 *
 *      Respond to changes in the toplevel for the AutoDrawer. A toplevel is
 *      required for the AutoDrawer to calculate its state.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      Drawer state is updated.
 *
 *-----------------------------------------------------------------------------
 */

static void
ViewAutoDrawerOnHierarchyChanged(ViewAutoDrawer *that, // IN
				 GtkWidget *toplevel)  // IN
{
   ViewAutoDrawerUpdate(that, TRUE);
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewAutoDrawerSetOver --
 *
 *      Virtual method override so that the user's over widget is placed
 *      inside the AutoDrawer's event box.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

static void
ViewAutoDrawerSetOver(ViewOvBox *ovBox,  // IN
                      GtkWidget *widget) // IN
{
   ViewAutoDrawer *that = VIEW_AUTODRAWER(ovBox);
   ViewAutoDrawerPrivate *priv = that->priv;
   GtkWidget *oldChild = gtk_bin_get_child(GTK_BIN(priv->evBox));

   if (oldChild) {
      g_object_ref(oldChild);
      gtk_container_remove(GTK_CONTAINER(priv->evBox), oldChild);
   }

   if (widget) {
      gtk_container_add(GTK_CONTAINER(priv->evBox), widget);
   }

   if (oldChild) {
      g_object_unref(oldChild);
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewAutoDrawerInit --
 *
 *      Initialize a ViewAutoDrawer.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

static void
ViewAutoDrawerInit(GTypeInstance *instance, // IN
                   gpointer klass)          // Unused
{
   ViewAutoDrawer *that;
   ViewAutoDrawerPrivate *priv;

   that = VIEW_AUTODRAWER(instance);
   that->priv = VIEW_AUTODRAWER_GET_PRIVATE(that);
   priv = that->priv;

   priv->active = TRUE;
   priv->pinned = FALSE;
   priv->inputUngrabbed = TRUE;
   priv->delayConnection = 0;
   priv->delayValue = 250;
   priv->overlapPixels = 0;
   priv->noOverlapPixels = 1;

   priv->evBox = GTK_EVENT_BOX(gtk_event_box_new());
   gtk_widget_show(GTK_WIDGET(priv->evBox));
   VIEW_OV_BOX_CLASS(parentClass)->set_over(VIEW_OV_BOX(that), GTK_WIDGET(priv->evBox));

   g_signal_connect(priv->evBox, "enter-notify-event",
                    G_CALLBACK(ViewAutoDrawerOnOverEnterLeave), that);
   g_signal_connect(priv->evBox, "leave-notify-event",
                    G_CALLBACK(ViewAutoDrawerOnOverEnterLeave), that);
   g_signal_connect(priv->evBox, "grab-notify",
                    G_CALLBACK(ViewAutoDrawerOnGrabNotify), that);

   g_signal_connect(that, "hierarchy-changed",
                    G_CALLBACK(ViewAutoDrawerOnHierarchyChanged), NULL);

   ViewAutoDrawerUpdate(that, FALSE);
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewAutoDrawerFinalize --
 *
 *      "finalize" method of a ViewAutoDrawer.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

static void
ViewAutoDrawerFinalize(GObject *object) // IN
{
   ViewAutoDrawer *that;

   that = VIEW_AUTODRAWER(object);
   if (that->priv->delayConnection) {
      g_source_remove(that->priv->delayConnection);
   }

   G_OBJECT_CLASS(parentClass)->finalize(object);
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewAutoDrawerClassInit --
 *
 *      Initialize the ViewAutoDrawerClass.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

static void
ViewAutoDrawerClassInit(gpointer klass) // IN
{
   GObjectClass *objectClass = G_OBJECT_CLASS(klass);
   ViewOvBoxClass *ovBoxClass = VIEW_OV_BOX_CLASS(klass);

   parentClass = g_type_class_peek_parent(klass);

   objectClass->finalize = ViewAutoDrawerFinalize;

   ovBoxClass->set_over = ViewAutoDrawerSetOver;

   g_type_class_add_private(objectClass, sizeof(ViewAutoDrawerPrivate));
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewAutoDrawer_GetType --
 *
 *      Get the (memoized) GType of the ViewAutoDrawer GTK+ object.
 *
 * Results:
 *      The GType
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

GType
ViewAutoDrawer_GetType(void)
{
   static GType type = 0;

   if (type == 0) {
      static const GTypeInfo info = {
         sizeof (ViewAutoDrawerClass),
         NULL, /* BaseInit */
         NULL, /* BaseFinalize */
         (GClassInitFunc)ViewAutoDrawerClassInit,
         NULL,
         NULL, /* Class Data */
         sizeof (ViewAutoDrawer),
         0, /* n_preallocs */
         (GInstanceInitFunc)ViewAutoDrawerInit,
      };

      type = g_type_register_static(VIEW_TYPE_DRAWER, "ViewAutoDrawer", &info, 0);
   }

   return type;
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewAutoDrawer_New --
 *
 *      Create a new ViewAutoDrawer GTK+ widget.
 *
 * Results:
 *      The widget
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

GtkWidget *
ViewAutoDrawer_New(void)
{
   return GTK_WIDGET(g_object_new(VIEW_TYPE_AUTODRAWER, NULL));
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewAutoDrawer_SetSlideDelay --
 *
 *      Set the response time of an AutoDrawer in ms., i.e. the time that
 *      elapses between:
 *      - when the AutoDrawer notices a change that can impact the outcome of 
 *        the decision to open or close the drawer,
 *      and
 *      - when the AutoDrawer makes such decision.
 *
 *      Users move the mouse inaccurately. If they temporarily move the mouse in 
 *      or out of the AutoDrawer for less than the reponse time, their move will 
 *      be ignored.
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
ViewAutoDrawer_SetSlideDelay(ViewAutoDrawer *that, // IN
                             guint delay)          // IN
{
   g_return_if_fail(VIEW_IS_AUTODRAWER(that));

   that->priv->delayValue = delay;
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewAutoDrawer_SetOverlapPixels --
 *
 *      Set the number of pixels that the over widget overlaps the under widget
 *      when not open.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      Drawer state is updated.
 *
 *-----------------------------------------------------------------------------
 */

void
ViewAutoDrawer_SetOverlapPixels(ViewAutoDrawer *that, // IN
                                guint overlapPixels)  // IN
{
   g_return_if_fail(VIEW_IS_AUTODRAWER(that));

   that->priv->overlapPixels = overlapPixels;
   ViewAutoDrawerUpdate(that, FALSE);
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewAutoDrawer_SetNoOverlapPixels --
 *
 *      Set the number of pixels that the drawer reserves when not open. The
 *      over widget does not overlap the under widget over these pixels.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      Drawer state is updated.
 *
 *-----------------------------------------------------------------------------
 */

void
ViewAutoDrawer_SetNoOverlapPixels(ViewAutoDrawer *that,  // IN
                                  guint noOverlapPixels) // IN
{
   g_return_if_fail(VIEW_IS_AUTODRAWER(that));

   that->priv->noOverlapPixels = noOverlapPixels;
   ViewAutoDrawerUpdate(that, FALSE);
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewAutoDrawer_SetActive --
 *
 *      Set whether the AutoDrawer is active or not. That is to say, whether
 *      it is acting as a drawer or not. When inactive, the over and under
 *      widget do not overlap and the net result is very much like a vbox.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      Drawer state is updated.
 *
 *-----------------------------------------------------------------------------
 */

void
ViewAutoDrawer_SetActive(ViewAutoDrawer *that, // IN
                         gboolean active)      // IN
{
   g_return_if_fail(VIEW_IS_AUTODRAWER(that));

   that->priv->active = active;
   ViewAutoDrawerUpdate(that, FALSE);
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewAutoDrawer_SetPinned --
 *
 *      Set whether the AutoDrawer is pinned or not. When pinned, the
 *      AutoDrawer will stay open regardless of the state of any other inputs.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      Drawer state is updated.
 *
 *-----------------------------------------------------------------------------
 */

void
ViewAutoDrawer_SetPinned(ViewAutoDrawer *that, // IN
                         gboolean pinned)      // IN
{
   g_return_if_fail(VIEW_IS_AUTODRAWER(that));

   that->priv->pinned = pinned;
   ViewAutoDrawerUpdate(that, pinned);
}
