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
 * spinner.cc --
 *
 *      The spinner widget instantiated by the SpinnerAction. The widget is
 *      intentionally very lightweight because we expect multiple instances
 *      linked to a single action - which is where the heavy lifting should
 *      take place.
 *
 *      So, the widget only defines the mechanics of switching frames.
 *      Loading the frames and setting an animation policy is left to the
 *      action.
 */


#include <libview/spinner.hh>


namespace view {


/*
 *-------------------------------------------------------------------
 *
 * view::Spinner::Spinner --
 *
 *      Constructor. See SetFrames for parameter rationale.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-------------------------------------------------------------------
 */

Spinner::Spinner(FrameVector &frames,                 // IN
                 Glib::RefPtr<Gdk::Pixbuf> restFrame) // IN
   : Gtk::Image()
{
   SetFrames(frames, restFrame);
}


/*
 *-------------------------------------------------------------------
 *
 * view::Spinner::SetFrames --
 *
 *      Set the frames that the spinner will show. Note that the
 *      call takes a reference to a frame vector. The widget is
 *      designed to be used from a SpinnerAction, so to keep it
 *      lightweight, the interface is designed with the expectation
 *      that an external object will maintain the frames, to be
 *      shared with multiple spinner instances.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      Spinner will be reset to the rest frame.
 *
 *-------------------------------------------------------------------
 */

void
Spinner::SetFrames(FrameVector &frames,                 // IN
                   Glib::RefPtr<Gdk::Pixbuf> restFrame) // IN
{
   mFrames = &frames;
   mRestFrame = restFrame;

   Rest();
}


/*
 *-------------------------------------------------------------------
 *
 * view::Spinner::Advance --
 *
 *      Set the current frame to the next one in the sequence or
 *      back to the first one, if the last one has been reached.
 *
 *      This has the robust in the face of an empty set of frames,
 *      in case something goes wrong and an IconTheme lookup returns
 *      nothing.
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
Spinner::Advance(void)
{
   if (mFrames->empty()) {
      Rest();
   } else {
      if (static_cast<unsigned int>(++mCurrentFrame) >= mFrames->size()) {
         mCurrentFrame = 0;
      }
      set((*mFrames)[mCurrentFrame]);
   }
}


/*
 *-------------------------------------------------------------------
 *
 * view::Spinner::Rest --
 *
 *      Set the spinner back to the rest frame.
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
Spinner::Rest(void)
{
   mCurrentFrame = static_cast<FrameVector::size_type>(-1);
   set(mRestFrame);
}


} // namespace view
