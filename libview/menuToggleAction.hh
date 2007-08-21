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
 * menuToggleAction.hh --
 *
 *      Subclass of Gtk::ToggleAction that gives a proxy with a popup/submenu.
 */

#ifndef LIBVIEW_MENUTOGGLEACTION_HH
#define LIBVIEW_MENUTOGGLEACTION_HH


#include <map>
#include <gtkmm/menu.h>
#include <gtkmm/toggleaction.h>


namespace view {

class MenuToggleAction
   : public Gtk::ToggleAction
{
public:
   static Glib::RefPtr<MenuToggleAction> create(const Glib::ustring &name,
                                                const Gtk::StockID &stockID,
                                                const Glib::ustring &label,
                                                const Glib::ustring &tooltip,
                                                bool isActive,
                                                Gtk::Menu *menu);
   ~MenuToggleAction();

protected:
   MenuToggleAction(const Glib::ustring &name, const Gtk::StockID &stockID,
                    const Glib::ustring &label, const Glib::ustring &tooltip,
                    bool isActive, Gtk::Menu *menu);

   Gtk::Widget *create_menu_item_vfunc(void);

   void connect_proxy_vfunc(Gtk::Widget *widget);
   void disconnect_proxy_vfunc(Gtk::Widget *widget);

private:
   bool OnButtonPressed(GdkEventButton *event, Gtk::Widget *widget);

   void DetachFromMenu(void);
   // Gtk insists on a detach callback but we don't need one.
   static void OnMenuDetached(GtkWidget *widget, GtkMenu *menu) {}

   // The action takes ownership of the menu.
   Gtk::Menu *mMenu;

   sigc::connection mDetachConnection;

   std::map<Gtk::Widget *, sigc::connection> widgetMap;
};

} // namespace view


#endif // LIBVIEW_MENUTOGGLEACTION_HH
