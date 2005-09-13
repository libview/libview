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
 * test-header-bgbox.cc
 *
 *      A test program that demonstrates the view::BaseBGBox and
 *      view::Header widgets.
 */


#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <libview/baseBGBox.hh>
#include <libview/header.hh>
#include <libview/wrapLabel.hh>


class AppWindow
   : public Gtk::Window
{
public:
   AppWindow();

private:
   view::BaseBGBox mBGBox;
   view::Header mHeader;
};


AppWindow::AppWindow()
   : mHeader("Themed Header Widget")
{
   set_title("BaseBGBox and Header Test");
   set_default_size(300, 200);

   mBGBox.show();
   add(mBGBox);
   mBGBox.set_border_width(12);

   Gtk::VBox *vbox = Gtk::manage(new Gtk::VBox(false, 6));
   vbox->show();
   mBGBox.pack_start(*vbox, true, true, 0);

   mHeader.show();
   vbox->pack_start(mHeader, false, true, 0);

   view::WrapLabel *label = Gtk::manage(new view::WrapLabel(
      "The background for this window is themed. It's the same widget now "
      "used for the Summary page background on VMware Workstation."));
   label->show();
   vbox->pack_start(*label, true, true, 0);
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
