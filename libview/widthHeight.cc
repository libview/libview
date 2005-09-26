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
 * widthHeight.cc --
 *
 *      Implements the WidthHeight class.
 */


#include <libview/widthHeight.hh>


namespace view {


/*
 *-----------------------------------------------------------------------------
 *
 * view::WidthHeight::WidthHeight --
 *
 *      Constructor of a WidthHeight.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

WidthHeight::WidthHeight(Dimension driving,     // IN: The driving dimension
                         size_t minDrivingSize) // IN: The minimum driving size
   : mDriving(driving),
     mMinDrivingSize(minDrivingSize),
     mDrivenSize(0)
{
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::WidthHeight::SetDrivenSize --
 *
 *      Set the driven size of a WidthHeight.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

void
WidthHeight::SetDrivenSize(size_t size) // IN
{
   if (mDrivenSize == size) {
      return;
   }
   mDrivenSize = size;
   queue_resize();
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::WidthHeight::on_size_request --
 *
 *      "size_request" method of a WidthHeight.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

void
WidthHeight::on_size_request(Gtk::Requisition *requisition) // OUT
{
   Gtk::Widget *child = get_child();
   if (child && child->is_visible()) {
      child->size_request(*requisition);
   }

   switch (mDriving) {
   case WIDTH:
      requisition->width = mMinDrivingSize;
      requisition->height = mDrivenSize;
      break;
   case HEIGHT:
      requisition->width = mDrivenSize;
      requisition->height = mMinDrivingSize;
      break;
   default:
      g_assert_not_reached();
      break;
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::WidthHeight::on_size_allocate --
 *
 *      "size_allocate" method of a WidthHeight.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

void
WidthHeight::on_size_allocate(Gtk::Allocation &allocation) // IN
{
   bool changed = false;

   if (mForceDrivingSizeChanged) {
      changed = true;
      mForceDrivingSizeChanged = false;
   } else {
      switch (mDriving) {
      case WIDTH:
         changed = allocation.get_width() != get_allocation().get_width();
         break;
      case HEIGHT:
         changed = allocation.get_height() != get_allocation().get_height();
         break;
      default:
         g_assert_not_reached();
         break;
      }
   }

   set_allocation(allocation);

   Gtk::Widget *child = get_child();
   if (child && child->is_visible()) {
      child->size_allocate(allocation);
   }

   if (changed) {
      drivingSizeChanged.emit();
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::WidthHeight::on_add --
 *
 *      "add" method of a WidthHeight.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

void
WidthHeight::on_add(Gtk::Widget *widget) // IN
{
   mForceDrivingSizeChanged = true;
   Gtk::Bin::on_add(widget);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::WidthHeight::GetDrivingSize --
 *
 *      Retrieve the driving size of a WidthHeight.
 *
 * Results:
 *      The driving size
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

size_t
WidthHeight::GetDrivingSize(void)
   const
{
   switch (mDriving) {
   case WIDTH:
      return get_allocation().get_width();
   case HEIGHT:
      return get_allocation().get_height();
   default:
      g_assert_not_reached();
      return 0;
   }
}


} /* namespace view */
