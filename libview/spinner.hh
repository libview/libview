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
 * spinner.hh --
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

#ifndef LIBVIEW_SPINNER_HH
#define LIBVIEW_SPINNER_HH


#include <gtkmm/image.h>
#include <gdkmm/pixbuf.h>


namespace view {


class Spinner
   : public Gtk::Image
{
public:
   typedef std::vector<Glib::RefPtr<Gdk::Pixbuf> > FrameVector;

   Spinner(FrameVector &frames, Glib::RefPtr<Gdk::Pixbuf> restFrame);

   void SetFrames(FrameVector &frames, Glib::RefPtr<Gdk::Pixbuf> restFrame);

   void Advance(void);
   void Rest(void);

private:
   const FrameVector *mFrames;
   Glib::RefPtr<Gdk::Pixbuf> mRestFrame;

   FrameVector::size_type mCurrentFrame;
};


} // namespace view

#endif
