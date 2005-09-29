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
 * ipEntry.hh --
 *
 *      Entry box for IP addresses. Checks for validity as users type. signals
 *      when entry's done (user hits enter or the box loses focus).
 */


#ifndef LIBVIEW_IP_ENTRY_HH
#define LIBVIEW_IP_ENTRY_HH


#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>


namespace view {


class IPEntry
   : public Gtk::Frame
{
public:
   IPEntry(void);
   long GetIP(void);
   void SetIP(long ip);
   void SetEditable(bool edit);

   sigc::signal<void, long> entryDone;

private:
   void OnDone(GdkEventFocus* event, unsigned int oct);
   void OnChanged(unsigned int oct);

   Gtk::HBox mBox;
   std::vector<Gtk::Entry *> mOcts;
   std::vector<Gtk::Entry *> mDots;
   bool mDirty;
   bool mEditable;
   bool mSetting;
   bool mDontSkip;
};


} /* namespace view */


#endif /* LIBVIEW_IP_ENTRY_HH */
