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
 * uiGroup.hh --
 *
 *      The ui::UIGroup object encapsulates a UI Definition fragment
 *      that needs to be constructed programmatically.
 *      It is much friendlier than being forced to work with raw XML markup.
 *
 *      This is not an intelligent object in anyway - it essentially serves
 *      to cache Gtk::UIManager::add_ui calls to be applied later. The producer
 *      calls AddUI on the object in the same way it would call add_ui on the
 *      UIManager if it was immediately modifying the UI. It then signals the
 *      owner of the UIManager using whatever means are desired and provides
 *      this UIGroup object in some way. The consumer than merges the UI Def
 *      into the UI Manager.
 *
 */

#ifndef LIBVIEW_UIGROUP_HH
#define LIBVIEW_UIGROUP_HH


#include <gtkmm/uimanager.h>
#include <vector>


namespace view {


class UIGroup
   : public Glib::Object
{
public:
   typedef Gtk::UIManagerItemType UIManagerItemType;

   UIGroup();
   ~UIGroup();

   void AddUI(const Glib::ustring &path, const Glib::ustring &name,
              const Glib::ustring &action, UIManagerItemType type = Gtk::UI_MANAGER_AUTO,
              bool top = true);
   void AddUISeparator(const Glib::ustring &path, const Glib::ustring &name = "",
                       UIManagerItemType type = Gtk::UI_MANAGER_AUTO, bool top = true);
   void Clear(void);

   void Merge(Glib::RefPtr<Gtk::UIManager> uiManager) const;
   void Unmerge(Glib::RefPtr<Gtk::UIManager> uiManager) const;

   bool IsMerged(void) const;

   // Producers should call this to signal consumers that the group has changed.
   void EmitChanged(void);

   // Consumers should connect to this signal, to be informed when the UIGroup changes.
   sigc::signal<void, const UIGroup *> changedSignal;

private:

   struct UIEntry {
      Glib::ustring path;
      Glib::ustring name;
      Glib::ustring action;
      UIManagerItemType type;
      bool top;

      bool isSeparator;
   };

   typedef std::vector<UIEntry>::const_iterator const_iterator;
   typedef Gtk::UIManager::ui_merge_id ui_merge_id;

   std::vector<UIEntry> mUIEntries;
   mutable ui_merge_id mMergeID;

   // There are no invalid values of ui_merge_id so we need a separate flag
   mutable bool mMerged;
};


} // namespace view


#endif // LIBVIEW_UIGROUP_HH
