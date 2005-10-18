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
 * test-ip-entry.cc
 *
 *      A test program that demonstrates the view::IPEntry widget.
 */


#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <pangomm/tabarray.h>
#include <libview/ipEntry.hh>
#include <libview/fieldEntry.hh>


class AppWindow
   : public Gtk::Window
{
public:
   AppWindow();

private:
   view::IPEntry mIPEntry;
   view::FieldEntry mEntry;
};


#if 0
bool
MyEntry::IsValidIP(const Glib::ustring& ip)
   const
{
   int groupChars = 0;

   for (int i = 0; i < ip.length(); i++) {
      if (ip[i] >= '0' && ip[i] <= '9') {
         groupChars++;
      } else if (ip[i] != '.') {
         return false;
      }

      if (ip[i] == '.' || i == ip.length() - 1) {
         if (groupChars == 0 || groupChars > 3) {
            return false;
         }

         groupChars = 0;
      }
   }

   return true;
}
#endif


AppWindow::AppWindow()
	: mEntry(4, '.', 3)
{
   set_title("IPEntry Test");
   set_border_width(12);
   set_default_size(300, 200);

   Gtk::VBox *vbox = Gtk::manage(new Gtk::VBox(false, 0));
   vbox->show();
   add(*vbox);

   mIPEntry.show();
   vbox->pack_start(mIPEntry, false, false, 0);

   mEntry.show();
   vbox->pack_start(mEntry, false, false, 0);
   mEntry.set_text("192.168.0.3");
   //mEntry.set_text("1.2.3.4");
   //mEntry.set_text("1\t.\t1\t.\t0\t.\t1");
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
