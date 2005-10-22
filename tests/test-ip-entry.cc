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
   view::IPEntry mEntry1;
   view::IPEntry mEntry2;
   Gtk::Entry mEntry3;
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
{
   set_title("IPEntry Test");
   set_border_width(12);
   set_default_size(300, 200);

   Gtk::VBox *vbox = Gtk::manage(new Gtk::VBox(false, 0));
   vbox->show();
   add(*vbox);

   mEntry1.show();
   vbox->pack_start(mEntry1, false, false);

   mEntry2.show();
   vbox->pack_start(mEntry2, false, false);

   /*
    * Real Gtk::Entry to visually check the proper alignment of the delimiters
    * of the FieldEntry. You can also use it to cut/paste and DnD between both
    * kinds of entries.
    */
   mEntry3.show();
   vbox->pack_start(mEntry3, false, false);
   mEntry3.set_text("222.111.222.111");

   /* Set value by quad-dotted. */
   mEntry1.SetIP("10.0.0.1");
   if (mEntry1.GetText() != "10.0.0.1") {
      printf("Test 1 failed\n");
   }

   /*
    * Set a field position beyond the length of a field, check that we cap
    * it.
    */
   mEntry1.SetCurrentField(1, 3);
   size_t posInField;
   if (!(   mEntry1.GetCurrentField(&posInField) == 1
         && posInField == 1)) {
      printf("Test 2 failed\n");
   }

   /* Set value by number. */
   mEntry1.SetDotlessIP((192 << 24) + (168 << 16) + (0 << 8) + (3 << 0));
   if (mEntry1.GetText() != "192.168.0.3") {
      printf("Test 3 failed\n");
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
