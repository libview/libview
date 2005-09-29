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
 * ipEntry.cc --
 *
 *      Entry box for IP addresses. Checks for validity as users type. signals
 *      when entry's done (user hits enter or the box loses focus).
 */


#include <sstream>
#include <libview/ipEntry.hh>


namespace view {


/*
 *-----------------------------------------------------------------------------
 *
 * view::IPEntry::IPEntry --
 *
 *      Constructor.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

IPEntry::IPEntry(void)
   : Frame(),
     mDirty(false),
     mEditable(true),
     mSetting(false),
     mDontSkip(false)
{
   set_shadow_type(Gtk::SHADOW_IN);

   mBox.show();
   add(mBox);

   for (unsigned int i = 0; i < 4; i++) {
      mOcts.push_back(Gtk::manage(new Gtk::Entry()));
      mOcts[i]->show();

      if (i == 3) {
         mBox.pack_start(*mOcts[i], true, true);
         mOcts[i]->set_alignment(0);
      } else {
         mBox.pack_start(*mOcts[i], false, false);
         mOcts[i]->set_alignment(.5);
      }

      mOcts[i]->set_has_frame(false);
      mOcts[i]->set_max_length(4);
      mOcts[i]->set_width_chars(3);
      mOcts[i]->signal_focus_out_event().connect(
         sigc::bind_return(sigc::bind(sigc::mem_fun(this,
                                                    &IPEntry::OnDone), i),
                           false));
      mOcts[i]->signal_activate().connect(
         sigc::bind(sigc::mem_fun(this, &IPEntry::OnDone),
                    static_cast<GdkEventFocus *>(NULL), i));
      mOcts[i]->signal_changed().connect(
         sigc::bind(sigc::mem_fun(this, &IPEntry::OnChanged), i));

      if (i < 3) {
         mDots.push_back(Gtk::manage(new Gtk::Entry()));
         mDots[i]->show();
         mBox.pack_start(*mDots[i], false, false);
         mDots[i]->set_has_frame(false);
         mDots[i]->set_width_chars(1);
         mOcts[i]->set_alignment(.5);
         mDots[i]->set_editable(false);
         mDots[i]->property_can_focus() = false;
         mDots[i]->set_text(".");
      }
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::IPEntry::SetEditable --
 *
 *      Sets the editable status of the entry.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

void
IPEntry::SetEditable(bool edit) // IN
{
   mEditable = edit;

   for (unsigned int i = 0; i < 4; i++) {
      mOcts[i]->set_editable(mEditable);
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::IPEntry::OnDone --
 *
 *      Callback for when the entry loses focus or the user hits enter.
 *      Fills in a blank octet with 0.
 *      Emits the entryDone signal if the entry has been changed.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

void
IPEntry::OnDone(GdkEventFocus* event, // IN
                unsigned int oct)     // IN
{
   if (mEditable) {
      if (mOcts[oct]->get_text().empty()) {
         mSetting = true;
         mOcts[oct]->set_text("0");
         mSetting = false;
         mDirty = true;
         mDontSkip = false;
      }

      if (mDirty) {
         entryDone.emit(GetIP());
         mDirty = false;
      }
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::IPEntry::OnChanged --
 *
 *      Callback for when the text in an octet changes.  Erases any
 *      non-numeric characters and caps each octet at 255.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

void
IPEntry::OnChanged(unsigned int oct) // IN
{
   if (mEditable && !mSetting) {
      bool changed = false;
      bool focusOut = false;
      Glib::ustring str = mOcts[oct]->get_text();

      for (unsigned int i = 0; i < str.size(); i++) {
         focusOut = (str[i] == '.' && i == (str.size() - 1) && !mDontSkip);
         if (str[i] < '0' || str[i] > '9' || i > 2) {
            str.erase(i, 1);
            i--;
            changed = true;
         }
      }

      mDontSkip = (mDontSkip && str.empty());

      if (changed) {
         mSetting = true;
         mOcts[oct]->set_text(str);
         mSetting = false;
      }

      if (atoi(str.c_str()) > 255) {
         mSetting = true;
         mOcts[oct]->set_text("255");
         mSetting = false;
      }

      mDirty = true;

      if (str.size() >= 3) {
         focusOut = true;
         mDontSkip = true;
      }

      if (focusOut && oct < 3) {
         mOcts[oct + 1]->grab_focus();
         OnDone(NULL, oct);
      }
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::IPEntry::GetIP --
 *
 *      Returns the IP in the entry as a 32-bit long int.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

long
IPEntry::GetIP(void)
{
   long ip = 0;

   for (unsigned int i = 0; i < 4; i++) {
      int oct = atoi(mOcts[i]->get_text().c_str());

      if (oct < 0 || oct > 255) {
         return 0;
      }

      ip |= oct << ((3 - i) * 8);
   }

   return ip;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::IPEntry::SetIP --
 *
 *      Takes a 32-bit long int and sets the displayed IP.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

void
IPEntry::SetIP(long ip) // IN
{
   for (unsigned int i = 0; i < 4; i++) {
      mSetting = true;
      std::ostringstream intStream;
      intStream << ((static_cast<unsigned long>(ip) << (i * 8)) >> 24);
      mOcts[i]->set_text(intStream.str());
      mSetting = false;
   }

   mDirty = false;
}


} // namespace view
