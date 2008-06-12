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
 * test-auto-drawer.cc
 *
 *      A test program that demonstrates the ViewAutoDrawer widget.
 *
 *      Performs unit tests on the following:
 *        * All methods
 *        * Active/Inactive mode (acts like a vbox when inactive).
 *        * Changes to overlap and noOverlap values.
 *        * Changes to auto-show/hide delay.
 *
 *      When running this test, check that:
 *        * Both the 'under' and 'over' widgets are drawn correctly.
 *        * When inactive, there is no overlap regardless of any other options.
 *        * Delay time to show/hide is respected.
 *        * Moving the mouse within the over box never causes the box to hide.
 *        * Provoking menus to appear from the over box doesn't cause the over
 *          box to disappear and that the over box stays down as long as the
 *          menu is around.
 *        * Keyboard actions to generate popup menus also cause the over box
 *          box to appear (and appear without delay). The over box should
 *          disappear when the menu is dismissed (but respecting the delay).
 */


#include <gtk/gtk.h>
#include <unistd.h>


#include <libview/autoDrawer.h>
#include <libview/drawer.h>
#include <libview/ovBox.h>


static const char *ui = "\
<ui> \
<menubar> \
    <menu name=\"File\" action=\"FileMenuAction\"> \
	<menuitem name=\"New\" action=\"NewAction\"/> \
	<menu name=\"Submenu\" action=\"SubMenuAction\"> \
           <menuitem name=\"Open\" action=\"OpenAction\"/> \
        </menu> \
	<menuitem name=\"Quit\" action=\"QuitAction\" /> \
     </menu> \
</menubar> \
</ui> \
";

static void
activate_action (GtkAction *action)
{
}

static GtkActionEntry entries[] = {
  { "FileMenuAction", NULL, ("_File"), NULL, NULL, NULL, },
  { "SubMenuAction", NULL, ("_Submenu"), NULL, NULL, NULL, },

  {"NewAction", GTK_STOCK_NEW, ("New"), "<control>n", NULL,
   G_CALLBACK (activate_action)},
  {"OpenAction", GTK_STOCK_OPEN, ("Open"), "<control>o", NULL,
   G_CALLBACK (activate_action)},
  {"QuitAction", GTK_STOCK_QUIT, ("Quit"), "<control>q", NULL,
   G_CALLBACK (gtk_main_quit)},
};
static guint n_entries = G_N_ELEMENTS (entries);

GtkWidget *drawer;
GtkWidget *button1;
GtkWidget *button2;
GtkWidget *cb0;
GtkWidget *cb1;
GtkWidget *cb2;
GtkWidget *cb3;
GtkWidget *cb4;
GtkWidget *cb5;
GtkWidget *cb6;
GtkWidget *cb7;


static void
OnActive(GtkWidget *widget,
         gpointer user_data)
{
   ViewAutoDrawer_SetActive(VIEW_AUTODRAWER(drawer),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb0)));
}


static void
OnPinned(GtkWidget *widget,
         gpointer user_data)
{
   ViewAutoDrawer_SetPinned(VIEW_AUTODRAWER(drawer),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb1)));
}


static void
OnSlow(GtkWidget *widget,
       gpointer user_data)
{
   ViewDrawer_SetSpeed(VIEW_DRAWER(drawer), 10,
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb2)) ? 0.01 : 0.2);
}


static void
OnDelay(GtkWidget *widget,
        gpointer user_data)
{
   ViewAutoDrawer_SetSlideDelay(VIEW_AUTODRAWER(drawer),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb3)) ? 1000 : 0);
}


static void
OnNoOverlap(GtkWidget *widget,
            gpointer user_data)
{
   ViewAutoDrawer_SetNoOverlapPixels(VIEW_AUTODRAWER(drawer),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb4)) ? 2 : 0);
}


static void
OnOverlap(GtkWidget *widget,
          gpointer user_data)
{
   ViewAutoDrawer_SetOverlapPixels(VIEW_AUTODRAWER(drawer),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb5)) ? 2 : 0);
}


static void
OnFill(GtkWidget *widget,
       gpointer user_data)
{
   ViewAutoDrawer_SetFill(VIEW_AUTODRAWER(drawer),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb6)));
}


static void
OnCenter(GtkWidget *widget,
         gpointer user_data)
{
   ViewAutoDrawer_SetOffset(VIEW_AUTODRAWER(drawer),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb7)) ? -1 : 10);
}


static void
OnClose(GtkWidget *widget,
         gpointer user_data)
{
   ViewAutoDrawer_Close(VIEW_AUTODRAWER(drawer));
}


static void
OnAddWidget(GtkUIManager *manager,
            GtkWidget *widget,
            GtkHBox *hbox)
{
   gtk_widget_show(widget);
   gtk_box_pack_start(GTK_BOX(hbox), widget, FALSE, FALSE, 0);
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

  g_signal_connect(G_OBJECT(window), "destroy",
                   G_CALLBACK(gtk_main_quit), NULL);

  drawer = ViewAutoDrawer_New();
  gtk_widget_show(drawer);

  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox);
  ViewOvBox_SetOver(VIEW_OV_BOX(drawer), hbox);

  GtkActionGroup *group = gtk_action_group_new("MainGroup");
  gtk_action_group_add_actions(group, entries, n_entries, NULL);

  GtkUIManager *manager = gtk_ui_manager_new();
  g_signal_connect(manager, "add_widget", G_CALLBACK(OnAddWidget), hbox);
  gtk_ui_manager_insert_action_group(manager, group, 0);
  gtk_ui_manager_add_ui_from_string(manager, ui, -1, NULL);

  button2 = gtk_button_new_with_label("Close");
  gtk_widget_show(button2);
  gtk_box_pack_end(GTK_BOX(hbox), button2, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(button2), "clicked",
                   G_CALLBACK(OnClose), NULL);
  OnClose(NULL, NULL);

  GtkWidget *label = gtk_label_new("text");
  gtk_widget_show(label);
  gtk_box_pack_end(GTK_BOX(hbox), label, FALSE, FALSE, 0);

  GtkWidget *entry = gtk_entry_new();
  gtk_widget_show(entry);
  gtk_box_pack_end(GTK_BOX(hbox), entry, FALSE, FALSE, 0);

  vbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox);
  ViewOvBox_SetUnder(VIEW_OV_BOX(drawer), vbox);

  button1 = gtk_button_new_with_label("Dummy");
  gtk_widget_show(button1);
  gtk_box_pack_start(GTK_BOX(vbox), button1, TRUE, TRUE, 0);

  entry = gtk_entry_new();
  gtk_widget_show(entry);
  gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

  cb0 = gtk_check_button_new_with_label("Active");
  gtk_widget_show(cb0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb0), TRUE);
  gtk_box_pack_start(GTK_BOX(vbox), cb0, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(cb0), "toggled",
                   G_CALLBACK(OnActive), NULL);
  OnActive(NULL, NULL);

  cb1 = gtk_check_button_new_with_label("Pinned");
  gtk_widget_show(cb1);
  gtk_box_pack_start(GTK_BOX(vbox), cb1, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(cb1), "toggled",
                   G_CALLBACK(OnPinned), NULL);
  OnPinned(NULL, NULL);

  cb2 = gtk_check_button_new_with_label("Slow");
  gtk_widget_show(cb2);
  gtk_box_pack_start(GTK_BOX(vbox), cb2, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(cb2), "toggled",
                   G_CALLBACK(OnSlow), NULL);
  OnSlow(NULL, NULL);

  /*
   * Pack the drawer late to verify that calls to set
   * properties can be made while the drawer has no toplevel.
   */
  gtk_container_add(GTK_CONTAINER(window), drawer);

  cb3 = gtk_check_button_new_with_label("Long Delay");
  gtk_widget_show(cb3);
  gtk_box_pack_start(GTK_BOX(vbox), cb3, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(cb3), "toggled",
                   G_CALLBACK(OnDelay), NULL);
  OnDelay(NULL, NULL);

  cb4 = gtk_check_button_new_with_label("No-overlap region");
  gtk_widget_show(cb4);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb4), TRUE);
  gtk_box_pack_start(GTK_BOX(vbox), cb4, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(cb4), "toggled",
                   G_CALLBACK(OnNoOverlap), NULL);
  OnNoOverlap(NULL, NULL);

  cb5 = gtk_check_button_new_with_label("Overlap region");
  gtk_widget_show(cb5);
  gtk_box_pack_start(GTK_BOX(vbox), cb5, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(cb5), "toggled",
                   G_CALLBACK(OnOverlap), NULL);
  OnOverlap(NULL, NULL);

  cb6 = gtk_check_button_new_with_label("Fill width");
  gtk_widget_show(cb6);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb6), TRUE);
  gtk_box_pack_start(GTK_BOX(vbox), cb6, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(cb6), "toggled",
                   G_CALLBACK(OnFill), NULL);
  OnFill(NULL, NULL);

  cb7 = gtk_check_button_new_with_label("Center");
  gtk_widget_show(cb7);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb7), TRUE);
  gtk_box_pack_start(GTK_BOX(vbox), cb7, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(cb7), "toggled",
                   G_CALLBACK(OnCenter), NULL);
  OnCenter(NULL, NULL);

  gtk_main();

  return 0;
}
