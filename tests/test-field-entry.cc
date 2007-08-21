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
 * test-field-entry.cc
 *
 *      A test program that demonstrates the view::FieldEntry widget.
 */


#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <pangomm/tabarray.h>
#include <libview/ipEntry.hh>
#include <libview/fieldEntry.hh>


class SerialEntry
   : public view::FieldEntry
{
public:
   SerialEntry(Alignment fieldAlignment);

protected:
   void FilterField(Glib::ustring& fieldText) const;
   Glib::ustring GetAllowedFieldChars(size_t field) const;
};


class AppWindow
   : public Gtk::Window
{
public:
   AppWindow();

private:
   view::IPEntry mEntry1;
   SerialEntry mEntry2;
   SerialEntry mEntry3;
   SerialEntry mEntry4;
};

SerialEntry::SerialEntry(Alignment fieldAlignment) // IN:
   : FieldEntry(4, 5, '-', fieldAlignment)
{
}

void
SerialEntry::FilterField(Glib::ustring& str) // IN/OUT:
   const
{
   str = str.uppercase();
}

Glib::ustring
SerialEntry::GetAllowedFieldChars(size_t field) // IN:
   const
{
   return "0123456789ABCDEF";
}

AppWindow::AppWindow()
   : mEntry2(SerialEntry::LEFT),
     mEntry3(SerialEntry::CENTER),
     mEntry4(SerialEntry::RIGHT)
{
   set_title("FieldEntry Test");
   set_border_width(12);
   set_default_size(300, 200);

   Gtk::VBox *vbox = Gtk::manage(new Gtk::VBox(false, 0));
   vbox->show();
   add(*vbox);

   mEntry1.show();
   vbox->pack_start(mEntry1, false, false);
   mEntry3.SetText("192.168.0.1");

   mEntry2.show();
   vbox->pack_start(mEntry2, false, false);
   mEntry2.SetText("191BD-74CBA-00917-BAB51");

   mEntry3.show();
   vbox->pack_start(mEntry3, false, false);
   mEntry3.SetText("191BD-74CBA-00917-BAB51");

   mEntry4.show();
   vbox->pack_start(mEntry4, false, false);
   mEntry4.SetText("191BD-74CBA-00917-BAB51");
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
