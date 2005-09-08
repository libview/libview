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
 * undoableTextView.hh --
 *
 *      Implement custom widget to support Undo/Redo for TextViews.
 */


#ifndef VIEW_UNDOABLE_TEXT_VIEW_HH
#define VIEW_UNDOABLE_TEXT_VIEW_HH


#include <stack>
#include <gtkmm/textbuffer.h>
#include <gtkmm/textview.h>


namespace view {


class EditAction;


class UndoableTextView
   : public Gtk::TextView
{
public:
   UndoableTextView(const Glib::RefPtr<Gtk::TextBuffer> &buffer =
		       Gtk::TextBuffer::create());
   virtual ~UndoableTextView(void);

   bool GetCanUndo(void);
   bool GetCanRedo(void);

   sigc::signal<void> undoChangedSignal;

   void Undo(void);
   void Redo(void);

   void ClearUndoHistory(void);

private:
   void OnInsert(const Gtk::TextBuffer::iterator &start,
		 const Glib::ustring &text,
		 int length);
   void OnErase(const Gtk::TextBuffer::iterator &start,
		const Gtk::TextBuffer::iterator &end);
   void OnPopulatePopup(Gtk::Menu *menu);
   bool OnKeyPressEvent(GdkEventKey *event);

   typedef std::stack<EditAction *> ActionStack;
   ActionStack mUndoStack;
   ActionStack mRedoStack;

   void UndoRedo(ActionStack &popFrom, ActionStack &pushTo, bool isUndo);
   void AddUndoAction(EditAction *action);
   void ResetStack(ActionStack &stack);

   unsigned int mFrozenCnt;
   bool mTryMerge;
   Glib::RefPtr<Gtk::AccelGroup> mAccelGroup;
};


} // namespace view


#endif // VIEW_UNDOABLE_TEXT_VIEW_HH
