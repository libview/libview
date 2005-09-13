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
 * test-wrap-label.cc
 *
 *      A test program that demonstrates the view::WrapLabel widget.
 */


#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <libview/wrapLabel.hh>


class AppWindow
   : public Gtk::Window
{
public:
   AppWindow();

private:
   view::WrapLabel mLabel;
};


AppWindow::AppWindow()
{
   set_title("WrapLabel Test");
   set_border_width(12);
   set_default_size(300, 200);

   mLabel.show();
   add(mLabel);
   mLabel.set_alignment(0, 0);
   mLabel.set_markup("This is a very long label that should span many lines. "
                    "It's a good example of what the WrapLabel can do, and "
                    "includes formatting, like <b>bold</b>, <i>italic</i>, "
                    "and <u>underline</u>. The window can be wrapped to any "
                    "width, unlike the standard Gtk::Label, which is set to "
                    "a certain wrap width.");
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
