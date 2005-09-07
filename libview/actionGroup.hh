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
 * actionGroup.hh --
 *
 *      Simple subclass of Gtk::ActionGroup to add a merge priority member.
 */


#ifndef LIBVIEW_ACTION_GROUP_HH
#define LIBVIEW_ACTION_GROUP_HH


#include <gtkmm/actiongroup.h>


namespace view {

class ActionGroup
   : public Gtk::ActionGroup
{

public:
   static Glib::RefPtr<ActionGroup> create(const Glib::ustring &name = Glib::ustring(),
                                           int pos = 0)
   {
      return Glib::RefPtr<ActionGroup>(new ActionGroup(name, pos));
   }

   int GetPos(void) { return mPos; }

protected:
   ActionGroup(const Glib::ustring &name = Glib::ustring(), int pos = 0)
      : Gtk::ActionGroup(name), mPos(pos) {}

private:
   int mPos;
};


} // namespace view


#endif // LIBVIEW_ACTION_GROUP_HH
