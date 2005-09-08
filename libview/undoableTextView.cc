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
 * undoableTextView.cc --
 *
 *      Implement custom widget to support Undo/Redo for TextViews.
 */


#include <gtkmm/stock.h>
#include <libview/undoableTextView.hh>


namespace view {


/*
 *-----------------------------------------------------------------------------
 *
 * view::EditAction --
 *
 *      Abstract base class for editing events.  Supports merging similar
 *      events and undoing/redoing them.
 *
 *-----------------------------------------------------------------------------
 */

class EditAction
{
public:
   virtual ~EditAction(void) { }

   virtual void Undo(Glib::RefPtr<Gtk::TextBuffer> buffer) = 0;
   virtual void Redo(Glib::RefPtr<Gtk::TextBuffer> buffer) = 0;
   virtual void Merge(EditAction *action) = 0;
   virtual bool GetCanMerge(EditAction *action) = 0;
};


/*
 *-----------------------------------------------------------------------------
 *
 * view::InsertAction --
 *
 *      An EditAction created on text insert events, via typing or paste.
 *
 *-----------------------------------------------------------------------------
 */

class InsertAction
   : public EditAction
{
public:
   InsertAction(const Gtk::TextBuffer::iterator &start,
                const Glib::ustring &text,
                int length);
   virtual ~InsertAction(void);

   virtual void Undo(Glib::RefPtr<Gtk::TextBuffer> buffer);
   virtual void Redo(Glib::RefPtr<Gtk::TextBuffer> buffer);
   virtual void Merge(EditAction *action);
   virtual bool GetCanMerge(EditAction *action);

private:
   Glib::ustring mText;
   unsigned int  mIndex;
   bool          mIsPaste;
};


/*
 *-----------------------------------------------------------------------------
 *
 * view::EraseAction --
 *
 *      An EditAction created on text delete events, via either delete or
 *      backspace or a cut operation.
 *
 *-----------------------------------------------------------------------------
 */

class EraseAction
   : public EditAction
{
public:
   EraseAction(const Gtk::TextBuffer::iterator &start,
               const Gtk::TextBuffer::iterator &end);
   virtual ~EraseAction(void);

   virtual void Undo(Glib::RefPtr<Gtk::TextBuffer> buffer);
   virtual void Redo(Glib::RefPtr<Gtk::TextBuffer> buffer);
   virtual void Merge(EditAction *action);
   virtual bool GetCanMerge(EditAction *action);

private:
   Glib::ustring mText;
   int           mStart;
   int           mEnd;
   bool          mIsForward;
   bool          mIsCut;
};


/*
 *-----------------------------------------------------------------------------
 *
 * view::InsertAction::InsertAction --
 *
 *      Constructor.  Stores the inserted text and buffer offset of this action.
 *      If the inserted text length is greater than 1, denoting a text paste,
 *      this action is unmergable.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

InsertAction::InsertAction(const Gtk::TextBuffer::iterator &start, // IN:
                           const Glib::ustring &text,              // IN:
                           int length)                             // IN:
   : mText(text),
     mIndex(start.get_offset() - length),
     mIsPaste(length > 1) // GTKBUG: No way to tell a 1-char paste.
{
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::InsertAction::InsertAction --
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

InsertAction::~InsertAction(void)
{
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::InsertAction::Undo --
 *
 *      Undo the insert action by deleting the inserted text block beginning at
 *      text buffer offset mIndex and ending at the mIndex plus the inserted
 *      text length.
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
InsertAction::Undo(Glib::RefPtr<Gtk::TextBuffer> buffer) // IN:
{
   buffer->erase(buffer->get_iter_at_offset(mIndex),
                 buffer->get_iter_at_offset(mIndex + mText.size()));
   buffer->move_mark(buffer->get_insert(),
                     buffer->get_iter_at_offset(mIndex));
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::InsertAction::Redo --
 *
 *      Redo the insert action by re-inserting the previously re-inserted text
 *      mText at buffer offset mIndex.
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
InsertAction::Redo(Glib::RefPtr<Gtk::TextBuffer> buffer) // IN:
{
   buffer->move_mark(buffer->get_insert(),
                     buffer->get_iter_at_offset(mIndex));
   buffer->insert(buffer->get_iter_at_offset(mIndex), mText);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::InsertAction::Merge --
 *
 *      Add the text block of the passed action to mText.
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
InsertAction::Merge(EditAction *action) // IN:
{
   InsertAction *insert = static_cast<InsertAction*>(action);
   mText += insert->mText;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::InsertAction::GetCanMerge --
 *
 *      Decide whether a given EditAction can be merged into this instance.
 *      Checking it is an InsertAction, that neither are paste operations, that
 *      the new action begins at the offset where this action ends, that this
 *      action is not a newline, and that the new action does not begin a new
 *      word.
 *
 * Results:
 *      True if the argument action can be merged with this action.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

bool
InsertAction::GetCanMerge(EditAction *action) // IN:
{
   InsertAction *insert = dynamic_cast<InsertAction*>(action);
   if (insert) {
      // Don't group text pastes
      if (mIsPaste || insert->mIsPaste) {
         return false;
      }

      // Must meet eachother
      if (insert->mIndex != mIndex + (int) mText.size()) {
         return false;
      }

      // Don't group more than one line (inclusive)
      if (mText[0] == '\n') {
         return false;
      }

      // Don't group more than one word (exclusive)
      if (insert->mText[0] == ' ' || insert->mText[0] == '\t') {
         return false;
      }

      return true;
   }
   return false;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::EraseAction::EraseAction --
 *
 *      Constructor.  Stores the start and end buffer offsets, and the removed
 *      text of this erase action, and whether is it from a cut operation, and
 *      if the text was deleted using backspace or delete.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

EraseAction::EraseAction(const Gtk::TextBuffer::iterator &start, // IN:
                         const Gtk::TextBuffer::iterator &end)   // IN:
   : mText(start.get_text(end)),
     mStart(start.get_offset()),
     mEnd(end.get_offset()),
     mIsCut(mEnd - mStart > 1) // GTKBUG: No way to tell a 1-char cut.
{
   Gtk::TextIter cursor = start.get_buffer()->get_insert()->get_iter();
   mIsForward = cursor.get_offset() < mStart;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::EraseAction::EraseAction --
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

EraseAction::~EraseAction(void)
{
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::EraseAction::Undo --
 *
 *      Undo the erase action by inserting the the deleted text block mText
 *      beginning at text buffer offset mStart.
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
EraseAction::Undo(Glib::RefPtr<Gtk::TextBuffer> buffer) // IN:
{
   buffer->insert(buffer->get_iter_at_offset(mStart), mText);
   buffer->move_mark(buffer->get_insert(),
                     buffer->get_iter_at_offset(mIsForward ? mStart : mEnd));
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::EraseAction::Redo --
 *
 *      Redo the erase action by re-erasing the previously re-inserted text
 *      mText at buffer offset mStart.
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
EraseAction::Redo(Glib::RefPtr<Gtk::TextBuffer> buffer) // IN:
{
   buffer->erase(buffer->get_iter_at_offset(mStart),
                 buffer->get_iter_at_offset(mEnd));
   buffer->move_mark(buffer->get_insert(),
                     buffer->get_iter_at_offset(mStart));
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::EraseAction::Merge --
 *
 *      Add the text block of the passed action to mText, and adjust the end or
 *      start buffer offsets to include the added text, depending on whether the
 *      passed action offsets meet this one at the beginning or end.
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
EraseAction::Merge(EditAction *action) // IN:
{
   EraseAction *erase = static_cast<EraseAction*>(action);
   if (mStart == erase->mStart) {
      mText += erase->mText;
      mEnd += erase->mEnd - erase->mStart;
   } else {
      mText = erase->mText + mText;
      mStart = erase->mStart;
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::EraseAction::GetCanMerge --
 *
 *      Decide whether a given EditAction can be merged into this instance.
 *      Checking it is an EraseAction, that neither are cut operations, that the
 *      new action offsets meet this action's, that this action is not a
 *      newline, and that the new action does not begin a new word.
 *
 * Results:
 *      True if the argument action can be merged with this action.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

bool
EraseAction::GetCanMerge(EditAction *action) // IN:
{
   EraseAction *erase = dynamic_cast<EraseAction*>(action);
   if (erase) {
      // Don't group separate text cuts
      if (mIsCut || erase->mIsCut) {
         return false;
      }

      // Must meet eachother
      if (mStart != (mIsForward ? erase->mStart : erase->mEnd)) {
         return false;
      }

      // Don't group deletes with backspaces
      if (mIsForward != erase->mIsForward) {
         return false;
      }

      // Don't group more than one line (inclusive)
      if (mText[0] == '\n') {
         return false;
      }

      // Don't group more than one word (exclusive)
      if (erase->mText[0] == ' ' || erase->mText[0] == '\t') {
         return false;
      }

      return true;
   }
   return false;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UndoableTextView::UndoableTextView --
 *
 *      Constructor.  Connects to insert (after handler) and erase (before
 *      handler) signals so we can track edits.  Connect to populate_popup and
 *      key_press_event (before handler) signals.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

UndoableTextView::UndoableTextView(
   const Glib::RefPtr<Gtk::TextBuffer> &buffer) // IN:
   : Gtk::TextView(buffer),
     mFrozenCnt(0),
     mTryMerge(false),
     mAccelGroup(Gtk::AccelGroup::create())
{
   get_buffer()->signal_insert().connect(
      sigc::mem_fun(this, &UndoableTextView::OnInsert));
   get_buffer()->signal_erase().connect(
      sigc::mem_fun(this, &UndoableTextView::OnErase), false);

   signal_populate_popup().connect(
      sigc::mem_fun(this, &UndoableTextView::OnPopulatePopup));
   signal_key_press_event().connect(
      sigc::mem_fun(this, &UndoableTextView::OnKeyPressEvent), false);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UndoableTextView::~UndoableTextView --
 *
 *      Destructor.  Release the memory stored in the undo/redo stacks.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

UndoableTextView::~UndoableTextView(void)
{
   ClearUndoHistory();
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UndoableTextView::OnInsert --
 *
 *      Handler for the insert signal.  Create an InsertAction object and push
 *      it onto the undo stack.
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
UndoableTextView::OnInsert(const Gtk::TextBuffer::iterator &start, // IN:
                           const Glib::ustring &text,              // IN:
                           int length)                             // IN:
{
   if (mFrozenCnt == 0) {
      AddUndoAction(new InsertAction(start, text, length));
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UndoableTextView::OnErase --
 *
 *      Handler for the erase signal.  Create an EraseAction object and push
 *      it onto the undo stack.
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
UndoableTextView::OnErase(const Gtk::TextBuffer::iterator &start, // IN:
                          const Gtk::TextBuffer::iterator &end)   // IN:
{
   if (mFrozenCnt == 0) {
      AddUndoAction(new EraseAction(start, end));
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UndoableTextView::AddUndoAction --
 *
 *      Adds an EditAction object to the undo stack.  If the new action can
 *      be merged with the stack's top action and we are not currently
 *      merge-locked, we merge the new action into the old one and delete the
 *      new action.  Otherwise we push the new action to the top of the stack.
 *      This also clears the redo buffer.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      The action argument may be deleted.
 *
 *-----------------------------------------------------------------------------
 */

void
UndoableTextView::AddUndoAction(EditAction *action) // IN:
{
   if (mTryMerge && !mUndoStack.empty()) {
      EditAction *top = mUndoStack.top();

      if (top->GetCanMerge(action)) {
         top->Merge(action);
         delete action;
         return;
      }
   }

   mUndoStack.push(action);

   // Clear redo stack
   ResetStack(mRedoStack);

   // Try to merge new incoming actions...
   mTryMerge = true;

   if (mUndoStack.size() == 1) {
      undoChangedSignal.emit();
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UndoableTextView::GetCanUndo --
 *
 *      Check if there are EditActions in the undo stack.
 *
 * Results:
 *      True if there is at least one undoable action.  False otherwise.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

bool
UndoableTextView::GetCanUndo(void)
{
   return !mUndoStack.empty();
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UndoableTextView::CanRedo --
 *
 *      Check if there are EditActions in the redo stack.
 *
 * Results:
 *      True if there is at least one redoable action.  False otherwise.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

bool
UndoableTextView::GetCanRedo(void)
{
   return !mRedoStack.empty();
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UndoableTextView::Undo --
 *
 *      Undo the topmost action in the undo stack, and move it to the redo
 *      stack.
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
UndoableTextView::Undo(void)
{
   UndoRedo(mUndoStack, mRedoStack, true /*undo*/);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UndoableTextView::Redo --
 *
 *      Redo the topmost action in the redo stack, and move it to the undo
 *      stack.
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
UndoableTextView::Redo(void)
{
   UndoRedo(mRedoStack, mUndoStack, false /*redo*/);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UndoableTextView::UndoRedo --
 *
 *      Perform an undo or redo operation by popping the topmost undoable action
 *      off the popFrom stack, calling the virtual Undo or Redo member, and
 *      adding the action to the pushTo stack.  If popFrom is now empty or
 *      pushTo is now non-empty, undoChangedSignal is emited.
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
UndoableTextView::UndoRedo(ActionStack &popFrom, // IN:
                           ActionStack &pushTo,  // IN:
                           bool isUndo)          // IN:
{
   if (!popFrom.empty()) {
      EditAction *action = popFrom.top();
      popFrom.pop();

      ++mFrozenCnt;
      if (isUndo) {
         action->Undo(get_buffer());
      } else {
         action->Redo(get_buffer());
      }
      --mFrozenCnt;

      pushTo.push(action);

      // Lock merges until a new undoable event comes in...
      mTryMerge = false;

      if (popFrom.empty() || pushTo.size() == 1) {
         undoChangedSignal.emit();
      }
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UndoableTextView::ResetStack --
 *
 *      Delete all the EditActions in the ActionStack argument.
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
UndoableTextView::ResetStack(ActionStack &stack) // IN:
{
   while (!stack.empty()) {
      delete stack.top();
      stack.pop();
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UndoableTextView::ClearUndoHistory --
 *
 *      Delete the actions in the redo and undo stack, and emit
 *      undoChangedSignal.
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
UndoableTextView::ClearUndoHistory(void)
{
   ResetStack(mUndoStack);
   ResetStack(mRedoStack);
   undoChangedSignal.emit();
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UndoableTextView::OnPopulatePopup --
 *
 *      populate_popup signal handler.  Prepend Undo and Redo menu items,
 *      followed by a separator, to the TextView's right click menu.  Display
 *      their accelerators as Ctrl-z for Undo, and Ctrl-Shift-z as Redo, and
 *      set their sensitivity.
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
UndoableTextView::OnPopulatePopup(Gtk::Menu *menu) // IN:
{
   Gtk::MenuItem *mitem;

   mitem = Gtk::manage(new Gtk::SeparatorMenuItem());
   mitem->show();
   menu->prepend(*mitem);

   mitem = Gtk::manage(new Gtk::ImageMenuItem(Gtk::Stock::UNDO));
   mitem->show();
   menu->prepend(*mitem);
   mitem->set_sensitive(GetCanUndo());
   mitem->add_accelerator("activate",
                          mAccelGroup,
                          GDK_z,
                          Gdk::CONTROL_MASK,
                          Gtk::ACCEL_VISIBLE);
   mitem->signal_activate().connect(
      sigc::mem_fun(this, &UndoableTextView::Undo));

   mitem = Gtk::manage(new Gtk::ImageMenuItem(Gtk::Stock::REDO));
   mitem->show();
   menu->prepend(*mitem);
   mitem->set_sensitive(GetCanRedo());
   mitem->add_accelerator("activate",
                          mAccelGroup,
                          GDK_z,
                          Gdk::CONTROL_MASK | Gdk::SHIFT_MASK,
                          Gtk::ACCEL_VISIBLE);
   mitem->signal_activate().connect(
      sigc::mem_fun(this, &UndoableTextView::Redo));
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::UndoableTextView::OnKeyPressEvent --
 *
 *      Before handler for key_press_event.  Executes an Undo if the key pressed
 *      is Ctrl-z, and executes Redo if the key is Ctrl-Z (Ctrl-Shift-z).
 *
 * Results:
 *      True if the keypress was handled as an Undo or Redo, false otherwise.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

bool
UndoableTextView::OnKeyPressEvent(GdkEventKey *event) // IN:
{
   if (event->state & GDK_CONTROL_MASK) {
      switch (event->keyval) {
      case GDK_z: // Ctrl+z
         Undo();
         return true;
      case GDK_Z: // Ctrl+Shift+z
         Redo();
         return true;
      }
   }
   return false;
}


} // namespace view
