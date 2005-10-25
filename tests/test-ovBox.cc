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
 * test-ovBox.cc
 *
 *      A test program that demonstrates the ViewOvBox widget.
 *
 *      Performs unit tests on the following:
 *        * All methods are invoked.
 *        * The 'under' and 'over' children start large then shrink.
 *
 *      When running this test, check that:
 *        * Both the 'under' and 'over' widgets are drawn correctly, even if
 *          you reduce the toplevel window's height to its minimum at each
 *          step.
 */


#include <gtk/gtk.h>
#include <unistd.h>


#include <libview/ovBox.h>


GtkWidget *ov;
GtkWidget *label1;
GtkWidget *label2;


static gint
timer(gpointer data) // Unused
{
   static unsigned int phase = 0;

   g_warning("timer phase %u", phase);

   switch (phase - 3) {
   case 0:
      g_warning("Setting label1");
      gtk_label_set_text(GTK_LABEL(label1), "Try\nme");
      break;

   case 1:
      ViewOvBox_SetFraction(VIEW_OV_BOX(ov), 0);
      break;

   case 2:
      ViewOvBox_SetFraction(VIEW_OV_BOX(ov), 0.1);
      break;

   case 3:
      ViewOvBox_SetFraction(VIEW_OV_BOX(ov), 0.2);
      break;

   case 4:
      ViewOvBox_SetFraction(VIEW_OV_BOX(ov), 0.3);
      break;

   case 5:
      ViewOvBox_SetFraction(VIEW_OV_BOX(ov), 0.4);
      break;

   case 6:
      ViewOvBox_SetFraction(VIEW_OV_BOX(ov), 0.5);
      break;

   case 7:
      ViewOvBox_SetFraction(VIEW_OV_BOX(ov), 0.6);
      break;

   case 8:
      g_warning("Setting label2");
      gtk_label_set_text(GTK_LABEL(label2), "ABCDE");
      break;

   case 9:
      ViewOvBox_SetFraction(VIEW_OV_BOX(ov), 0.7);
      break;

   case 10:
      ViewOvBox_SetFraction(VIEW_OV_BOX(ov), 0.8);
      break;

   case 11:
      ViewOvBox_SetFraction(VIEW_OV_BOX(ov), 0.9);
      break;

   case 12:
      ViewOvBox_SetFraction(VIEW_OV_BOX(ov), 1);
      break;

   default:
      break;
   }

   phase++;
   return 1;
}


int
main(int argc,    // IN
     char **argv) // IN
{
   GtkWidget *window;

   gtk_init(&argc, &argv);
    
   window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   gtk_widget_show(window);

   ov = ViewOvBox_New();
   gtk_widget_show(ov);
   gtk_container_add(GTK_CONTAINER(window), ov);

   label2 = gtk_label_new("ABCDE\nFGHIJ");
   gtk_widget_show(label2);
   ViewOvBox_SetOver(VIEW_OV_BOX(ov), label2);

   label1 = gtk_label_new("Try\nto\noverlap\nme");
   gtk_widget_show(label1);
   ViewOvBox_SetUnder(VIEW_OV_BOX(ov), label1);

   ViewOvBox_SetMin(VIEW_OV_BOX(ov), 10);
   g_timeout_add(2000, timer, NULL);

   gtk_main();
    
   return 0;
}
