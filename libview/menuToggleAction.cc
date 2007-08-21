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
 * menuToggleAction.cc --
 *
 *      Subclass of Gtk::ToggleAction that gives a proxy with a popup/submenu.
 */


#include <gtkmm/toggletoolbutton.h>
#include <gtk/gtkmenu.h>

#include <libview/menuToggleAction.hh>


namespace view {


/*
 *-------------------------------------------------------------------
 *
 * view::MenuToggleAction::MenuToggleAction --
 *
 *      Constructor.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-------------------------------------------------------------------
 */

MenuToggleAction::MenuToggleAction(const Glib::ustring &name,    // IN
                                   const Gtk::StockID &stockID,  // IN
                                   const Glib::ustring &label,   // IN
                                   const Glib::ustring &tooltip, // IN
                                   bool isActive,                // IN
                                   Gtk::Menu *menu)              // IN
   : Gtk::ToggleAction(name, stockID, label, tooltip, isActive),
     mMenu(menu)
{
}


/*
 *-------------------------------------------------------------------
 *
 * view::MenuToggleAction::~MenuToggleAction --
 *
 *      Destructor.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-------------------------------------------------------------------
 */

MenuToggleAction::~MenuToggleAction()
{
   delete mMenu;
}


/*
 *-------------------------------------------------------------------
 *
 * view::MenuToggleAction::create --
 *
 *      Public named constructor. Required by the Glib::RefPtr
 *      pattern.
 *
 * Results:
 *      Glib::RefPtr to the new instance.
 *
 * Side effects:
 *      None
 *
 *-------------------------------------------------------------------
 */

Glib::RefPtr<MenuToggleAction>
MenuToggleAction::create(const Glib::ustring &name,    // IN
                         const Gtk::StockID &stockID,  // IN
                         const Glib::ustring &label,   // IN
                         const Glib::ustring &tooltip, // IN
                         bool isActive,                // IN
                         Gtk::Menu *menu)              // IN
{
   return Glib::RefPtr<MenuToggleAction>(
      new MenuToggleAction(name, stockID, label, tooltip, isActive, menu));
}


/*
 *-------------------------------------------------------------------
 *
 * view::MenuToggleAction::create_menu_item_vfunc --
 *
 *      Virtual method override to create a submenu if the action
 *      was passed a menu, or just create a regular check menu
 *      item if not.
 *
 * Results:
 *      Menu item.
 *
 * Side effects:
 *      None
 *
 *-------------------------------------------------------------------
 */

Gtk::Widget *
MenuToggleAction::create_menu_item_vfunc(void)
{
   if (mMenu) {
      Gtk::MenuItem *item = new Gtk::MenuItem();
      item->set_submenu(*mMenu);
      return item;
   } else {
      return Gtk::ToggleAction::create_menu_item_vfunc();
   }
}


/*
 *-------------------------------------------------------------------
 *
 * view::MenuToggleAction::connect_proxy_vfunc --
 *
 *      Virtual method override to connect proxy widgets to this
 *      action. Menu items are treated as normal, while buttons have
 *      a mouse press handler attached to show a context menu.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      Side effects of Gtk::ToggleAction::connect_proxy_vfunc.
 *
 *-------------------------------------------------------------------
 */

void
MenuToggleAction::connect_proxy_vfunc(Gtk::Widget *widget) // IN
{
   Gtk::ToggleToolButton *toolButton = dynamic_cast<Gtk::ToggleToolButton *>(widget);
   if (toolButton) {
      widgetMap[widget] =
         toolButton->get_child()->signal_button_press_event().connect(
            sigc::bind(sigc::mem_fun(this, &MenuToggleAction::OnButtonPressed), widget),
            false);
   }

   /*
    * XXX: In gtk+ <= 2.6.7, gtk_toggle_action_connect_proxy is buggy
    * and tries to call gtk_check_menu_item_set_active on the proxy
    * despite only doing a GTK_IS_MENU_ITEM check.
    * Unfortunately, we can't work around this in the context of gtkmm,
    * so we have to live with the warning. Fortunately that will be
    * suppressed in release builds. The fix will appear in 2.6.8.
    * http://bugzilla.gnome.org/show_bug.cgi?id=304089
    */

   Gtk::ToggleAction::connect_proxy_vfunc(widget);
}


/*
 *-------------------------------------------------------------------
 *
 * view::MenuToggleAction::disconnect_proxy_vfunc --
 *
 *      Virtual method override to disconnect proxy widgets from this
 *      action.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      Side effects of Gtk::ToggleAction::disconnect_proxy_vfunc.
 *
 *-------------------------------------------------------------------
 */

void
MenuToggleAction::disconnect_proxy_vfunc(Gtk::Widget *widget) // IN
{
   Gtk::ToggleToolButton *toolButton = dynamic_cast<Gtk::ToggleToolButton *>(widget);
   if (toolButton) {
      g_assert(widgetMap.find(toolButton->get_child()) != widgetMap.end());

      widgetMap[widget].disconnect();
      widgetMap.erase(widget);
   }
   Gtk::ToggleAction::disconnect_proxy_vfunc(widget);
}


/*
 *-------------------------------------------------------------------
 *
 * view::MenuToggleAction::OnButtonPressed --
 *
 *      Callback for when a button press event is received by a
 *      tool button proxy. Shows a context menu on right click.
 *
 * Results:
 *      true if menu was shown (event handled), false if not.
 *
 * Side effects:
 *      None
 *
 *-------------------------------------------------------------------
 */

bool
MenuToggleAction::OnButtonPressed(GdkEventButton *event, // IN
                                  Gtk::Widget *widget)   // IN
{
   if (event->button == 3) {
      if (mMenu) {
         /*
          * It is generally desirable to associate a popupmenu with the
          * widget that invoked it, so we attempt to do so. However,
          * the GtkMenu model is illsuited to our case where we have
          * one menu and many widgets so we must try and make do. We assume
          * that if the menu already has an attach widget, we should let
          * things be. That attach_widget will be some proxy, and that's
          * as good as any other.
          */
         if (!mMenu->get_attach_widget()) {
            gtk_menu_attach_to_widget(mMenu->gobj(),
                                      widget->gobj(),
                                      MenuToggleAction::OnMenuDetached);
            /*
             * The standard pattern is to leave the widget attached and let
             * the menu disconnect itself in its finalize method. However, in
             * our case, the menu has a longer lifetime than the widget and
             * GtkMenu does not track the widget, so there are problems when
             * the menu finally goes away. To avoid this, we explicitly
             * disconnect, once the menu is dismissed.
             */
            mDetachConnection = mMenu->signal_unmap().connect(
               sigc::mem_fun(this, &MenuToggleAction::DetachFromMenu));

         }
         mMenu->popup(event->button, event->time);
      }      
      return true;
   } else {
      return false;
   }
}


/*
 *-------------------------------------------------------------------
 *
 * view::MenuToggleAction::DetachFromMenu --
 *
 *      Callback to detach from the action's menu that disconnects
 *      itself as well.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-------------------------------------------------------------------
 */

void
MenuToggleAction::DetachFromMenu(void)
{
   mDetachConnection.disconnect();
   mMenu->detach();
}


} // namespace view
