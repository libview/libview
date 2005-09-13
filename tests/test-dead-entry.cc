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
 * test-dead-entry.cc
 *
 *      A test program that demonstrates the view::DeadEntry widget.
 */


#include <gtkmm/checkbutton.h>
#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <libview/deadEntry.hh>


class AppWindow
   : public Gtk::Window
{
public:
   AppWindow();

private:
   void OnEditableCheckToggled(void);

   Gtk::CheckButton mEditableCheck;
   Gtk::Entry mEntry;
   view::DeadEntry mDeadEntry;
};


AppWindow::AppWindow()
{
   set_title("DeadEntry Test");
   set_border_width(12);
   set_default_size(300, 200);

   Gtk::VBox *vbox = new Gtk::VBox(false, 6);
   vbox->show();
   add(*vbox);

   mEntry.show();
   vbox->pack_start(mEntry, false, true, 0);
   mEntry.set_text("Standard Gtk::Entry");

   mDeadEntry.show();
   vbox->pack_start(mDeadEntry, false, true, 0);
   mDeadEntry.set_text("view::DeadEntry");

   mEditableCheck.show();
   vbox->pack_start(mEditableCheck, false, true, 0);
   mEditableCheck.set_label("Editable entries");

   sigc::slot<void> editableCheckToggledSlot =
      sigc::mem_fun(this, &AppWindow::OnEditableCheckToggled);
   mEditableCheck.signal_toggled().connect(editableCheckToggledSlot);
   editableCheckToggledSlot();
}


void
AppWindow::OnEditableCheckToggled(void)
{
   bool editable = mEditableCheck.get_active();

   mEntry.set_editable(editable);
   mDeadEntry.set_editable(editable);
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
