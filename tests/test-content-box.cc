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
 * test-content-box.cc
 *
 *      A test program that demonstrates the view::ContentBox widget.
 */


#include <gtkmm/box.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/frame.h>
#include <gtkmm/image.h>
#include <gtkmm/main.h>
#include <gtkmm/stock.h>
#include <gtkmm/window.h>
#include <libview/contentBox.hh>


class AppWindow
   : public Gtk::Window
{
public:
   AppWindow();

private:
   void OnShowImageToggled(Gtk::CheckButton* check, Gtk::Image& image);

   view::ContentBox mContentBox1;
   view::ContentBox mContentBox2;
   Gtk::Frame mFrame1;
   Gtk::Frame mFrame2;
   Gtk::HBox mHBox1;
   Gtk::HBox mHBox2;
   Gtk::Image mImage1;
   Gtk::Image mImage2;
   Gtk::Image mImage3;
};


AppWindow::AppWindow()
   : mHBox1(false, 6),
     mHBox2(false, 6),
     mImage1(Gtk::Stock::DIALOG_WARNING, Gtk::ICON_SIZE_DIALOG),
     mImage2(Gtk::Stock::DIALOG_ERROR, Gtk::ICON_SIZE_DIALOG),
     mImage3(Gtk::Stock::DIALOG_INFO, Gtk::ICON_SIZE_DIALOG)
{
   set_title("ContentBox Test");
   set_border_width(12);

   Gtk::VBox *vbox = Gtk::manage(new Gtk::VBox(false, 6));
   vbox->show();
   add(*vbox);

   Gtk::CheckButton *checkButton;
   checkButton = Gtk::manage(new Gtk::CheckButton("Show warning icon"));
   checkButton->show();
   vbox->pack_start(*checkButton, false, false, 0);
   checkButton->set_active(true);
   sigc::slot<void> slot =
      sigc::bind(sigc::mem_fun(this, &AppWindow::OnShowImageToggled),
                 checkButton, sigc::ref(mImage1));
   checkButton->signal_toggled().connect(slot);
   slot();

   checkButton = Gtk::manage(new Gtk::CheckButton("Show error icon"));
   checkButton->show();
   vbox->pack_start(*checkButton, false, false, 0);
   checkButton->set_active(true);
   slot = sigc::bind(sigc::mem_fun(this, &AppWindow::OnShowImageToggled),
                     checkButton, sigc::ref(mImage2));
   checkButton->signal_toggled().connect(slot);
   slot();

   checkButton = Gtk::manage(new Gtk::CheckButton("Show info icon"));
   checkButton->show();
   vbox->pack_start(*checkButton, false, false, 0);
   checkButton->set_active(true);
   slot = sigc::bind(sigc::mem_fun(this, &AppWindow::OnShowImageToggled),
                     checkButton, sigc::ref(mImage3));
   checkButton->signal_toggled().connect(slot);
   slot();

   vbox->pack_start(mContentBox1, false, false, 0);

   mFrame1.show();
   mContentBox1.add(mFrame1);
   mFrame1.set_shadow_type(Gtk::SHADOW_IN);

   mHBox1.show();
   mFrame1.add(mHBox1);
   mHBox1.add(mImage1);
   gtk_box_set_child_packing(GTK_BOX(mHBox1.gobj()),
                             GTK_WIDGET(mImage1.gobj()), FALSE, FALSE, 0,
                             GTK_PACK_START);

   mHBox1.add(mContentBox2);
   gtk_box_set_child_packing(GTK_BOX(mHBox1.gobj()),
                             GTK_WIDGET(mContentBox2.gobj()), FALSE, FALSE, 0,
                             GTK_PACK_START);

   mFrame2.show();
   mContentBox2.add(mFrame2);
   mFrame2.set_shadow_type(Gtk::SHADOW_IN);

   mHBox2.show();
   mFrame2.add(mHBox2);
   mHBox2.add(mImage2);
   gtk_box_set_child_packing(GTK_BOX(mHBox2.gobj()),
                             GTK_WIDGET(mImage2.gobj()), FALSE, FALSE, 0,
                             GTK_PACK_START);

   mHBox2.add(mImage3);
   gtk_box_set_child_packing(GTK_BOX(mHBox2.gobj()),
                             GTK_WIDGET(mImage3.gobj()), FALSE, FALSE, 0,
                             GTK_PACK_START);
}

void
AppWindow::OnShowImageToggled(Gtk::CheckButton* checkButton,
                              Gtk::Image& image)
{
   if (checkButton->get_active()) {
      image.show();
   } else {
      image.hide();
   }
}

int
main(int argc,     // IN:
     char *argv[]) // IN:
{
   Gtk::Main kit(&argc, &argv);
   AppWindow app;
   Gtk::Main::run(app);

   return 0;
}
