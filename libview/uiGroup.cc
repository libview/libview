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
 * uiGroup.cc --
 *
 *      This module implements the view::UIGroup class
 */

#include <libview/uiGroup.hh>


namespace view {


/*
 *-----------------------------------------------------------------------------
 *
 * view::UIGroup::UIGroup --
 *
 *      Constructor.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

UIGroup::UIGroup()
   : mMergeID(0),
     mMerged(false)
{

}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UIGroup::~UIGroup --
 *
 *      Destructor.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

UIGroup::~UIGroup()
{

}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UIGroup::AddUI --
 *
 *      Add a UI element to our vector. The parameters are the exact ones that
 *      will be passed to Gtk::UIManager::add_ui at merge time.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

void
UIGroup::AddUI(const Glib::ustring &path,   // IN: Point in UI Def to insert element
               const Glib::ustring &name,   // IN: Name of element
               const Glib::ustring &action, // IN: Action that element will proxy for
               UIManagerItemType type,      // IN: Type of element
               bool top)                    // IN: Is element inserted at top of parent
{
   mUIEntries.push_back((UIEntry){path, name, action, type, top, false});
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UIGroup::AddUISeparator --
 *
 *      Add a separator element to our vector. The parameters are the exact ones that
 *      will be passed to Gtk::UIManager::add_ui_separator at merge time.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

void
UIGroup::AddUISeparator(const Glib::ustring &path, // IN: Point in UI Def to insert
                        const Glib::ustring &name, // IN: Name of separator
                        UIManagerItemType type,    // IN: Type of separator
                        bool top)                  // IN: Is separator inserted at top
{
   mUIEntries.push_back((UIEntry){path, name, "", type, top, true});
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UIGroup::Clear --
 *
 *      Remove all elements from UI group, so it can be reused.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

void
UIGroup::Clear(void)
{
   mUIEntries.clear();
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UIGroup::Merge --
 *
 *      Conditionally merge the new UI entries into the UIManager. If this group
 *      was previously merged, unmerge first. If there are no UI entries, do not
 *      bother creating a merge id.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

void
UIGroup::Merge(Glib::RefPtr<Gtk::UIManager> uiManager) // IN: UIManager
   const
{
   if (mUIEntries.size()) {
      Unmerge(uiManager);

      mMergeID = uiManager->new_merge_id();

      for (const_iterator i = mUIEntries.begin(); i != mUIEntries.end(); i++) {
         if ((*i).isSeparator) {
            uiManager->add_ui_separator(mMergeID, (*i).path, (*i).name, (*i).type, 
                                        (*i).top);
         } else {
            uiManager->add_ui(mMergeID, (*i).path, (*i).name, (*i).action, (*i).type, 
                              (*i).top);
         }
      }
      mMerged = true;
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UIGroup::Unmerge --
 *
 *      Unmerge this UI group if it was already merged. Otherwise, do nothing.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

void
UIGroup::Unmerge(Glib::RefPtr<Gtk::UIManager> uiManager) // IN: UIManager
   const
{
   if (IsMerged()) {
      uiManager->remove_ui(mMergeID);
      mMerged = false;
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UIGroup::IsMerged --
 *
 *      Returns whether or not the UIGroup is currently merged.
 *
 * Results:
 *      true if merged, or false otherwise.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

bool
UIGroup::IsMerged(void)
   const
{
   return mMerged;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UIGroup::EmitChanged --
 *
 *      Emit the changed signal on this UIGroup and pass the UIGroup as a
 *      parameter. This is the way to emit the signal. Do not call emit
 *      directly - this method may do more in the future.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

void
UIGroup::EmitChanged(void)
{
   changedSignal.emit(this);
}


} // namespace view
