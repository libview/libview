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
 * drawer.c -
 *
 *      Implementation of a GTK+ drawer, i.e. a widget that opens and closes by
 *      sliding smoothly, at constant speed, over another one.
 */


#include <libview/drawer.h>
#include <libview/ovBox.h>


/* The unaltered parent class. */
static ViewOvBoxClass *parentClass;


struct _ViewDrawer {
   /* Must come first. */
   ViewOvBox parent;

   unsigned int period;
   double step;
   double goal;
   struct {
      gboolean pending;
      guint id;
   } timer;
};


struct _ViewDrawerClass {
   /* Must come first. */
   ViewOvBoxClass parent;
};


/*
 *-----------------------------------------------------------------------------
 *
 * ViewDrawerInit --
 *
 *      Initialize a ViewDrawer.
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
ViewDrawerInit(GTypeInstance *instance, // IN
             gpointer klass)          // Unused
{
   ViewDrawer *that;

   that = (ViewDrawer *)instance;
   that->period = 10;
   that->step = 0.2;
   that->timer.pending = FALSE;
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewDrawerFinalize --
 *
 *      "finalize" method of a ViewOvBox.
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
ViewDrawerFinalize(GObject *object) // IN
{
   ViewDrawer *that;

   that = VIEW_DRAWER(object);
   if (that->timer.pending) {
      gtk_timeout_remove(that->timer.id);
      that->timer.pending = FALSE;
   }

   G_OBJECT_CLASS(parentClass)->finalize(object);
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewDrawerClassInit --
 *
 *      Initialize the ViewDrawerClass.
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
ViewDrawerClassInit(gpointer klass) // IN
{
   G_OBJECT_CLASS(klass)->finalize = ViewDrawerFinalize;

   parentClass = gtk_type_class(VIEW_TYPE_OV_BOX);
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewDrawer_GetType --
 *
 *      Get the (memoized) GtkType of the ViewDrawer GTK+ object.
 *
 * Results:
 *      The GtkType
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

GtkType
ViewDrawer_GetType(void)
{
   static GtkType type = 0;

   if (type == 0) {
      static const GtkTypeInfo info = {
         "ViewDrawer",
         sizeof (ViewDrawer),
         sizeof (ViewDrawerClass),
         ViewDrawerClassInit,
         ViewDrawerInit,
         /* reserved_1 */ NULL,
         /* reserved_2 */ NULL,
         NULL,
      };

      type = gtk_type_unique(VIEW_TYPE_OV_BOX, &info);
   }

   return type;
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewDrawer_New --
 *
 *      Create a new ViewDrawer GTK+ widget.
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
ViewDrawer_New(void)
{
   ViewDrawer *that;

   that = VIEW_DRAWER(gtk_type_new(VIEW_TYPE_DRAWER));

   return GTK_WIDGET(that);
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewDrawerOnTimer --
 *
 *      Timer callback of a ViewDrawer. If we have reached the goal, deschedule
 *      the timer. Otherwise make progress towards the goal, and keep the timer
 *      scheduled.
 *
 * Results:
 *      TRUE if the timer must be rescheduled.
 *      FALSE if the timer must not be rescheduled.
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

static gint
ViewDrawerOnTimer(gpointer data) // IN
{
   ViewDrawer *that;
   double fraction;

   that = VIEW_DRAWER(data);

   fraction = ViewOvBox_GetFraction(VIEW_OV_BOX(that));
   /*
    * Comparing double values with '==' is most of the time a bad idea, due to
    * the inexact representation of values in binary (see
    * http://www2.hursley.ibm.com/decimal/decifaq1.html and http://boost.org/libs/test/doc/components/test_tools/floating_point_comparison.html).
    * But in this particular case it is legitimate. --hpreg
    */
   if (that->goal == fraction) {
      return that->timer.pending = FALSE;
   }

   ViewOvBox_SetFraction(VIEW_OV_BOX(that),
                       that->goal > fraction
                          ? MIN(fraction + that->step, that->goal)
                          : MAX(fraction - that->step, that->goal));
   return TRUE;
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewDrawer_SetSpeed --
 *
 *      Set the 'period' (in ms.) and 'step' properties of a ViewDrawer, which
 *      determine the speed and smoothness of the drawer's motion.
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
ViewDrawer_SetSpeed(ViewDrawer *that,      // IN
                  unsigned int period, // IN
		  double step)         // IN
{
   that->period = period;
   if (that->timer.pending) {
      gtk_timeout_remove(that->timer.id);
      that->timer.id = gtk_timeout_add(that->period, ViewDrawerOnTimer, that);
   }
   that->step = step;
}


/*
 *-----------------------------------------------------------------------------
 *
 * ViewDrawer_SetGoal --
 *
 *      Set the 'goal' property of a ViewDrawer, i.e. how much the drawer should
 *      be opened when it is done sliding.
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
ViewDrawer_SetGoal(ViewDrawer *that, // IN
                 double goal)    // IN
{
   if (goal < 0 || goal > 1) {
      return;
   }

   that->goal = goal;
   if (that->timer.pending == FALSE) {
      that->timer.id = gtk_timeout_add(that->period, ViewDrawerOnTimer, that);
      that->timer.pending = TRUE;
   }
}


#if 0
/*
 * Unit testing of the ViewDrawer code:
 * o All methods
 * o 'under' and 'over' children behave as if they were not in this container.
 * o Locked ('over' child fully displayed and not overlapping) mode.
 * o Change of goal and step while the previous goal has not been reached yet.
 *
 * When running this test, check that:
 * o Both the 'under' and 'over' widgets are drawn correctly.
 * o You can click on the buttons.
 * o Buttons continue to do what they are meant to do, even if you click them
 *   while the drawer is moving.
 */


#include <gtk/gtk.h>
#include <unistd.h>


#include <libview/vmDrawer.h>
#include <libview/vmOvBox.h>


GtkWidget *drawer;
GtkWidget *button1;
GtkWidget *button2;
GtkWidget *cb1;
GtkWidget *cb2;


static void
OnOpen(GtkWidget *widget,
       gpointer user_data)
{
   ViewDrawer_SetGoal(VIEW_DRAWER(drawer), 1);
}


static void
OnClose(GtkWidget *widget,
        gpointer user_data)
{
   ViewDrawer_SetGoal(VIEW_DRAWER(drawer), 0);
}


static void
OnLocked(GtkWidget *widget,
         gpointer user_data)
{
   ViewOvBox_SetMin(VIEW_OV_BOX(drawer),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb1)) ? -1 : 0);
}


static void
OnSlow(GtkWidget *widget,
       gpointer user_data)
{
   ViewDrawer_SetSpeed(VIEW_DRAWER(drawer), 10,
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb2)) ? 0.01 : 0.2);
}


int
main(int argc,
     char **argv)
{
  GtkWidget *window;
  GtkWidget *vbox;

  gtk_init(&argc, &argv);
    
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_show(window);

  drawer = ViewDrawer_New();
  gtk_widget_show(drawer);
  gtk_container_add(GTK_CONTAINER(window), drawer);

  button2 = gtk_button_new_with_label("Close");
  gtk_widget_show(button2);
  ViewOvBox_SetOver(VIEW_OV_BOX(drawer), button2);
  gtk_signal_connect(GTK_OBJECT(button2), "clicked",
                     GTK_SIGNAL_FUNC(OnClose), NULL);

  vbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox);
  ViewOvBox_SetUnder(VIEW_OV_BOX(drawer), vbox);

  button1 = gtk_button_new_with_label("Open");
  gtk_widget_show(button1);
  gtk_box_pack_start(GTK_BOX(vbox), button1, TRUE, TRUE, 0);
  gtk_signal_connect(GTK_OBJECT(button1), "clicked",
                     GTK_SIGNAL_FUNC(OnOpen), NULL);

  cb1 = gtk_check_button_new_with_label("Locked");
  gtk_widget_show(cb1);
  gtk_box_pack_start(GTK_BOX(vbox), cb1, FALSE, FALSE, 0);
  gtk_signal_connect(GTK_OBJECT(cb1), "toggled",
                     GTK_SIGNAL_FUNC(OnLocked), NULL);
  OnLocked(NULL, NULL);

  cb2 = gtk_check_button_new_with_label("Slow");
  gtk_widget_show(cb2);
  gtk_box_pack_start(GTK_BOX(vbox), cb2, FALSE, FALSE, 0);
  gtk_signal_connect(GTK_OBJECT(cb2), "toggled",
                     GTK_SIGNAL_FUNC(OnSlow), NULL);
  OnSlow(NULL, NULL);

  gtk_main();
    
  return 0;
}
#endif