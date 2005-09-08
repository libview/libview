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
 * test-drawer.cc
 *
 *      A test program that demonstrates the ViewDrawer widget.
 *
 *      Performs unit tests on the following:
 *        * All methods
 *        * 'under' and 'over' children behave as if they were not in this
 *          container.
 *        * Locked ('over' child fully displayed and not overlapping) mode.
 *        * Change of goal and step while the previous goal has not been
 *          reached yet.
 *
 *      When running this test, check that:
 *        * Both the 'under' and 'over' widgets are drawn correctly.
 *        * You can click on the buttons.
 *        * Buttons continue to do what they are meant to do, even if you
 *          click them while the drawer is moving.
 */


#include <gtk/gtk.h>
#include <unistd.h>


#include <libview/drawer.h>
#include <libview/ovBox.h>


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
