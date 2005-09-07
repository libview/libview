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
 * spinnerAction.cc --
 *
 *      An action object that allows a spinner to be inserted into a toolbar
 *      using GtkUIManager. This action handles the loading of the animation
 *      frames and propagating frame changes to all proxy widgets. Policy
 *      for when to change frames is left up to consumers/subclasses of the
 *      action.
 */


#include <gtkmm/alignment.h>
#include <gtk/gtkaction.h>
#include <gtk/gtktoolitem.h>

#include <libview/defines.h>
#include <libview/spinnerAction.hh>
#include <libview/spinner.hh>


namespace view {


/*
 *-------------------------------------------------------------------
 *
 * view::SpinnerAction::SpinnerAction --
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

SpinnerAction::SpinnerAction(const Glib::ustring &name,              // IN
                             Gtk::IconSize iconSize,                 // IN
                             const FrameIDVector &frameIDs,          // IN
                             const Glib::ustring &restID,            // IN
                             Glib::RefPtr<Gtk::IconTheme> iconTheme) // IN
   : Gtk::Action(name, Gtk::StockID(restID), "Spinner"),
     mFrameIDs(frameIDs),
     mRestID(restID),
     mIconTheme(iconTheme),
     mRestSize(0)
{
   Gtk::IconSize::lookup(iconSize, mTargetW, mTargetH);

   mIconTheme->signal_changed().connect(
      sigc::mem_fun(this, &SpinnerAction::LoadAllFrames));
   LoadAllFrames();
}


/*
 *-------------------------------------------------------------------
 *
 * view::SpinnerAction::create --
 *
 *      Public named constructor. Required by the Glib::RefPtr
 *      pattern.
 *
 * Results:
 *      Glib::RefPtr to new instance.
 *
 * Side effects:
 *      None
 *
 *-------------------------------------------------------------------
 */

Glib::RefPtr<SpinnerAction>
SpinnerAction::create(const Glib::ustring &name,              // IN
                      Gtk::IconSize iconSize,                 // IN
                      const FrameIDVector &frameIDs,          // IN
                      const Glib::ustring &restID,            // IN
                      Glib::RefPtr<Gtk::IconTheme> iconTheme) // IN
{
   return Glib::RefPtr<SpinnerAction>(new SpinnerAction(name, iconSize,
                                                        frameIDs, restID,
                                                        iconTheme));
}


/*
 *-------------------------------------------------------------------
 *
 * view::SpinnerAction::create_tool_item_vfunc --
 *
 *      Override method to return our custom spinner tool item when
 *      requested by the UIManager.
 *
 *      Remember that if the container hierarchy of the tool item
 *      changes, GetSpinnerFromItem() must be updated to reflect this.
 *
 * Results:
 *      The spinner tool item.
 *
 * Side effects:
 *      None
 *
 *-------------------------------------------------------------------
 */

Gtk::Widget *
SpinnerAction::create_tool_item_vfunc(void)
{
   Gtk::ToolItem *item = Gtk::manage(new Gtk::ToolItem());
   item->show();
   item->set_expand(true);

   item->signal_create_menu_proxy().connect(
      sigc::bind(sigc::ptr_fun(&SpinnerAction::OnToolItemCreateMenuProxy),
                 item), false);

   Gtk::Alignment *alignment = Gtk::manage(new Gtk::Alignment(1.0, 0.5, 0, 0));
   alignment->show();
   item->add(*alignment);

   Spinner *spinner = Gtk::manage(new Spinner(mFrames, mRestFrame));
   spinner->show();
   alignment->add(*spinner);

   return item;
}


/*
 *-------------------------------------------------------------------
 *
 * view::SpinnerAction::LoadAllFrames --
 *
 *      Load the animation frames and set them onto each proxy widget.
 *
 *      It may seem slightly ridiculous to have an algorithm that
 *      could handle a list of stockIDs where each stockID represented
 *      multiple frames but this is to maximise reusability. It is
 *      instead much more likely that either an icon-theme style spinner,
 *      with one stockID containing multiple frames *or* a sane spinner,
 *      where each frame is stored separately would be used. But it's
 *      cleaner to merge both loading paths than to try and special
 *      case.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      Proxies will update and redraw.
 *
 *-------------------------------------------------------------------
 */

void
SpinnerAction::LoadAllFrames(void)
{
   mFrames.clear();

   Gtk::IconInfo info = mIconTheme->lookup_icon(mRestID, -1,
                                                (Gtk::IconLookupFlags)0);
   if (info.gobj()) {
      Glib::RefPtr<Gdk::Pixbuf> buffer = 
         Gdk::Pixbuf::create_from_file(info.get_filename());
      mRestSize = buffer->get_width();
      mRestFrame = buffer->scale_simple(mTargetW, mTargetH, Gdk::INTERP_BILINEAR);
   }

   for (FrameIDVector::size_type i = 0; i < mFrameIDs.size(); i++) {
      LoadFrames(mFrameIDs[i]);
   }

   ForeachSpinner(sigc::mem_fun(this, &SpinnerAction::SpinnerSetFrames));
}


/*
 *-------------------------------------------------------------------
 *
 * view::SpinnerAction::LoadFrames --
 *
 *      Load the frames from the passed stockID and append them to
 *      our frame vector.
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
SpinnerAction::LoadFrames(const Glib::ustring &frameID) // IN
{
   Gtk::IconInfo info = mIconTheme->lookup_icon(frameID, -1,
                                                (Gtk::IconLookupFlags)0);

   if (info.gobj()) {
      Glib::RefPtr<Gdk::Pixbuf> buffer = 
         Gdk::Pixbuf::create_from_file(info.get_filename());

      /*
       * By convention, the spinner animation is stored as a sequence of frames
       * where the frame size is specified by the base size of the icon theme directory 
       * the spinner is stored in. However, because we pull our spinner out of our
       * pixmap directory, there is no way for us to specify the base size, and it will
       * always return as zero. So, in this case, we fallback to using the size of
       * the rest frame on the assumption that the rest frame is the same size as
       * the animation frames. This is a good assumption in general and because we
       * should only need to use this fallback for the spinner we provide (rather than
       * one in a hypothetical icon theme), we can ensure that it is a valid assumption.
       */
      int frameSize = info.get_base_size();
      if (frameSize <= 0) {
         frameSize = mRestSize;
      }

      for (int y = 0; buffer->get_height() - y >= frameSize; y += frameSize) {
         for (int x = 0; buffer->get_width() - x >= frameSize; x += frameSize) {
            mFrames.push_back(
               Gdk::Pixbuf::create_subpixbuf(buffer, x, y, frameSize, frameSize)->
                  scale_simple(mTargetW, mTargetH, Gdk::INTERP_BILINEAR));
         }
      }
   }
}


/*
 *-------------------------------------------------------------------
 *
 * view::SpinnerAction::Advance --
 *
 *      Advance each proxy spinner widget forward one frame.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None.
 *
 *-------------------------------------------------------------------
 */

void
SpinnerAction::Advance(void)
{
   ForeachSpinner(sigc::ptr_fun(&SpinnerAction::SpinnerAdvance));
}


/*
 *-------------------------------------------------------------------
 *
 * view::SpinnerAction::Rest --
 *
 *      Reset each proxy spinner widget back to the rest frame.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None.
 *
 *-------------------------------------------------------------------
 */

void
SpinnerAction::Rest(void)
{
   ForeachSpinner(sigc::ptr_fun(&SpinnerAction::SpinnerRest));
}


/*
 *-------------------------------------------------------------------
 *
 * view::SpinnerAction::ForeachSpinner --
 *
 *      Helper to apply a functor to each spinner proxy widget.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      Anything - depends on the functor.
 *
 *-------------------------------------------------------------------
 */

void
SpinnerAction::ForeachSpinner(sigc::slot<void, Spinner *> functor) // IN
{
   // gtkmm < 2.6.3 wraps get_proxies incorrectly.
#if GTKMM_CHECK_VERSION(2, 6, 3)
   std::vector<Gtk::Widget *> proxies(get_proxies());
   for (unsigned int i = 0; i < proxies.size(); i++) {
      Gtk::ToolItem *item;
      if ((item = dynamic_cast<Gtk::ToolItem *>(proxies[i]))) {
#else
   for (GSList *i = gtk_action_get_proxies(GTK_ACTION(gobj())); i; i = i->next) {
      if (GTK_IS_TOOL_ITEM(i->data)) {
         Gtk::ToolItem *item = Glib::wrap(GTK_TOOL_ITEM(i->data));
#endif
         Spinner *spinner = GetSpinnerFromItem(item);
         functor(spinner);
      }
   }
}


/*
 *-------------------------------------------------------------------
 *
 * view::SpinnerAction::SpinnerAdvance --
 * view::SpinnerAction::SpinnerRest --
 * view::SpinnerAction::SpinnerSetFrames --
 *
 *      Helpers to call methods on a spinner widget. Required because
 *      libsigc++ isn't powerful enough to create mem_fun slots where
 *      the object is specified later as a parameter. I've declared
 *      them inline, but I doubt that the compiler can inline them
 *      through a slot because we have to pass the address of the
 *      function. No harm, so I'll do it anyway.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-------------------------------------------------------------------
 */

inline void
SpinnerAction::SpinnerAdvance(Spinner *spinner) // IN
{
   spinner->Advance();
}


inline void
SpinnerAction::SpinnerRest(Spinner *spinner) // IN
{
   spinner->Rest();
}


inline void
SpinnerAction::SpinnerSetFrames(Spinner *spinner) // IN
{
   spinner->SetFrames(mFrames, mRestFrame);
}


/*
 *-------------------------------------------------------------------
 *
 * view::SpinnerAction::GetSpinnerFromItem --
 *
 *      Helper method to reduce verbosity by encapsulating two
 *      casts. It's inline so there's no overhead. Think of it as
 *      a typesafe scoped #define.
 *
 * Results:
 *      The spinner
 *
 * Side effects:
 *      None.
 *
 *-------------------------------------------------------------------
 */

inline Spinner *
SpinnerAction::GetSpinnerFromItem(Gtk::ToolItem *item) // IN
{
   return
      static_cast<Spinner *>(static_cast<Gtk::Bin *>(item->get_child())->get_child());
}


/*
 *-------------------------------------------------------------------
 *
 * view::SpinnerAction::OnToolItemCreateMenuProxy --
 *
 *      Callback on signal to create menu proxy (when toolbar goes
 *      into overflow mode). Right now it blocks the base handler
 *      and sets NULL to indicate no menu item.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None.
 *
 *-------------------------------------------------------------------
 */

bool
SpinnerAction::OnToolItemCreateMenuProxy(Gtk::ToolItem *item) // IN
{
   gtk_tool_item_set_proxy_menu_item(item->gobj(), "dummy", NULL);
   return true;
}


} // namespace view
