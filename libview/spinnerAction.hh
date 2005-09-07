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
 * spinnerAction.hh --
 *
 *      An action object that allows a spinner to be inserted into a toolbar
 *      using GtkUIManager. This action handles the loading of the animation
 *      frames and propagating frame changes to all proxy widgets. Policy
 *      for when to change frames is left up to consumers/subclasses of the
 *      action.
 */

#ifndef LIBVIEW_SPINNERACTION_HH
#define LIBVIEW_SPINNERACTION_HH


#include <gtkmm/action.h>
#include <gtkmm/icontheme.h>
#include <gtkmm/toolitem.h>


namespace view {


class Spinner;


class SpinnerAction
   : public Gtk::Action
{
public:
   typedef std::vector<Glib::ustring> FrameIDVector;

   static Glib::RefPtr<SpinnerAction> create(const Glib::ustring &name,
                                             Gtk::IconSize iconSize,
                                             const FrameIDVector &frameIDs,
                                             const Glib::ustring &restID,
                                             Glib::RefPtr<Gtk::IconTheme> iconTheme);

   void Advance(void);
   void Rest(void);

protected:
   SpinnerAction(const Glib::ustring &name,
                 Gtk::IconSize iconSize,
                 const FrameIDVector &frameIDs,
                 const Glib::ustring &restID,
                 Glib::RefPtr<Gtk::IconTheme> iconTheme);

   Gtk::Widget *create_tool_item_vfunc(void);

private:
   typedef std::vector<Glib::RefPtr<Gdk::Pixbuf> > FrameVector;

   void LoadAllFrames(void);
   void LoadFrames(const Glib::ustring &frameID);

   void ForeachSpinner(sigc::slot<void, Spinner *> functor);
   void SpinnerSetFrames(Spinner *spinner);
   static void SpinnerAdvance(Spinner *spinner);
   static void SpinnerRest(Spinner *spinner);
   static Spinner *GetSpinnerFromItem(Gtk::ToolItem *item);

   static bool OnToolItemCreateMenuProxy(Gtk::ToolItem *item);

   FrameIDVector mFrameIDs;
   Glib::ustring mRestID;

   FrameVector mFrames;
   Glib::RefPtr<Gdk::Pixbuf> mRestFrame;

   Glib::RefPtr<Gtk::IconTheme> mIconTheme;
   int mTargetW;
   int mTargetH;
   int mRestSize;
};

} // namespace view


#endif // LIBVIEW_SPINNERACTION_HH
