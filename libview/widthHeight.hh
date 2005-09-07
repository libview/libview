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
 * widthHeight.hh --
 *
 *      A Gtk::Bin whose driven dimension is a function of its driving
 *      dimension.
 */

#ifndef LIBVIEW_WIDTH_HEIGHT_HH
#   define LIBVIEW_WIDTH_HEIGHT_HH


#include <gtkmm/bin.h>
#include <sigc++/signal.h>


namespace view {


class WidthHeight
   : public Gtk::Bin
{
public:
   enum Dimension {
      WIDTH,
      HEIGHT,
   };

   WidthHeight(Dimension driving, size_t minDrivingSize);
   size_t GetDrivingSize(void) const;
   void SetDrivenSize(size_t size);

   /* This signal is never emitted spuriously. */
   sigc::signal<void> drivingSizeChanged;

protected:
   void on_size_request(Gtk::Requisition *requisition);
   void on_size_allocate(Gtk::Allocation &allocation);
   void on_add(Gtk::Widget *widget);

private:
   Dimension mDriving;
   size_t const mMinDrivingSize;
   size_t mDrivenSize;
   bool mForceDrivingSizeChanged;
};


} /* namespace view */


#endif /* LIBVIEW_WIDTH_HEIGHT_HH */
