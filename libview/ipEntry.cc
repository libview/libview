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
 *     Entry box for IP addresses. Checks for validity as users type.
 */


#include <libview/ipEntry.hh>
#include <sigc++/adaptors/compose.h>
#include <errno.h>
#include <sstream>


namespace view {


/*
 *-----------------------------------------------------------------------------
 *
 * view::IPEntry::IPEntry --
 *
 *     Constructor.
 *
 * Results:
 *     None.
 *
 * Side Effects:
 *     None.
 *
 *-----------------------------------------------------------------------------
 */

IPEntry::IPEntry(Mode mode) // IN:
   : FieldEntry(4, 3, '.'),
     mMode(mode)
{
   currentFieldChanged.connect(sigc::mem_fun(this, &IPEntry::NormalizeField));
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::IPEntry::SetIP --
 *
 *      Sets the IP address of the entry.
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
IPEntry::SetIP(const Glib::ustring& ip) // IN:
{
   SetText(ip);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::IPEntry::GetIP --
 *
 *      Returns the IP address of the entry.
 *
 * Results:
 *      The IP address in text form.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

Glib::ustring
IPEntry::GetIP(void)
   const
{
   return GetText();
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::IPEntry::SetDotlessIP --
 *
 *      Sets the dotless (numeric form) IP address.
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
IPEntry::SetDotlessIP(unsigned long ip) // IN:
{
   switch (mMode) {
   case IPV4:
      for (unsigned int i = 0; i < GetFieldCount(); i++) {
         std::ostringstream stream;
         stream << ((ip << (i * 8)) >> 24);
         SetFieldText(i, stream.str());
      }
      break;

   default:
      g_assert_not_reached();
   }

}


/*
 *-----------------------------------------------------------------------------
 *
 * view::IPEntry::GetDotlessIP --
 *
 *      Returns the dotless (numeric form) IP address.
 *
 * Results:
 *      The dotless IP address.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

unsigned long
IPEntry::GetDotlessIP(void)
   const
{
   unsigned long ip = 0;

   switch (mMode) {
   case IPV4:
      for (unsigned int i = 0; i < GetFieldCount(); i++) {
         int oct = atoi(GetFieldText(i).c_str());

         if (oct < 0 || oct > 255) {
            return 0;
         }

         ip |= (oct << ((3 - i) * 8));
      }

      break;

   default:
      g_assert_not_reached();
   }

   return ip;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::IPEntry::GetIsFieldValid --
 *
 *      Tests if the specified text is valid for a field.
 *
 * Results:
 *      true if the text is valid, or false.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

bool
IPEntry::GetIsFieldValid(const Glib::ustring& str) // IN: Field to validate
    const
{
   for (int i = 0; i < str.length(); i++) {
      if (str[i] < '0' || str[i] > '9') {
         return false;
      }
   }

   return atoi(str.c_str()) < 256;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::IPEntry::GetAllowedFieldChars --
 *
 *      Returns a list of allowed characters for IP fields. This takes into
 *      account IPV4 and IPV6.
 *
 * Results:
 *      The list of allowed characters.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

Glib::ustring
IPEntry::GetAllowedFieldChars(size_t field) // IN:
   const
{
   Glib::ustring chars = "";

   switch (mMode) {
   case IPV4:
      chars = "0123456789";
      break;

   case IPV6:
      chars = "0123456789ABCDEF";
      break;

   default:
      g_assert_not_reached();
   }

   return chars;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::IPEntry::on_focus_out_event --
 *
 *      Focus out event handler. Calls NormalizeField with the IP entry's
 *      current field.
 *
 * Results:
 *      The result of the parent class's on_focus_out_event.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

bool
IPEntry::on_focus_out_event(GdkEventFocus* event) // IN: Focus event
{
   bool result = FieldEntry::on_focus_out_event(event);

   NormalizeField(GetCurrentField());

   return result;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::IPEntry::NormalizeField --
 *
 *      Normalizes the text in the current field. This essentially strips out
 *      leading zeros from the field. This happens any time the focus is
 *      removed or the current field changes.
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
IPEntry::NormalizeField(unsigned int field) // IN: The field
{
   Glib::ustring text = GetFieldText(field);

   if (text != "") {
      std::ostringstream stream;
      stream << atoi(text.c_str());
      SetFieldText(field, stream.str());
   }
}


} // namespace view
