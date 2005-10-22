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
 * fieldEntry.cc --
 *
 *     An entry widget that separates data by fields. Such uses could be
 *     an IP entry widget or a serial number entry widget.
 */


#include <libview/fieldEntry.hh>
#include <libview/utils.hh>
#include <gtk/gtkentry.h>


namespace view {


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::FieldEntry --
 *
 *      Constructor.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

FieldEntry::FieldEntry(size_t fieldCount,               // IN: Number of fields
                       size_t maxFieldWidth,            // IN: Max Field width
                                                        //     in chars
                       Glib::ustring::value_type delim) // IN: Delimiter text
   : mMaxFieldWidth(maxFieldWidth),
     mDelim(delim),
     mTabs(0)
{
   g_return_if_fail(fieldCount > 0);
   g_return_if_fail(delim != '\0');
   g_return_if_fail(maxFieldWidth > 0);

   /*
    * XXX When the entry uses tabs, and its allocation is narrow, moving the
    *     cursor in the entry makes the entry scroll, although all text is
    *     visible. It happens because, see the comment for on_expose_event(),
    *     the GtkEntry sometimes creates a new PangoLayout, and then before the
    *     widget is exposed, the layout is used to compute the value of the
    *     "scroll offset" property. Because we didn't get a chance to set our
    *     tab stops in the GtkEntry's PangoLayout, the PangoLayout uses its
    *     default of one tab stop every 8 spaces. It then believes that our
    *     marked up string is way larger than it really is, and starts
    *     scrolling. We workaround that by synchronously forcing back the value
    *     of the "scroll offset" property to 0 every time it changes. --hpreg
    */

   property_scroll_offset().signal_changed().connect(
      sigc::mem_fun(this, &FieldEntry::OnScrollOffsetChanged));

   set_editable(true);

   Field f;
   f.dirty = false;
   mFields.resize(fieldCount, f);

   ComputeLayout();
   ApplyLayout();
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::OnScrollOffsetChanged --
 *
 *      Signal handler for when the "scroll_offset" property changes.
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
FieldEntry::OnScrollOffsetChanged()
{
   /*
    * A comment in gtkentry.h says that this field is read-only and private.
    * But it is part of the ABI, so it will not go away until GTK+ 3.0, and
    * I'm writing to it to workaround a GTK deficiency. So yes, it is clearly
    * an abuse, but no I'm not ashamed of it. --hpreg
    */

   gobj()->scroll_offset = 0;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::SetText --
 *
 *      Sets the text of this entry. In reality, this just calls
 *      Gtk::Entry::set_text(), but is provided to be consistent with
 *      our GetText(), which we do have to provide.
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
FieldEntry::SetText(const Glib::ustring& text) // IN: The new text.
{
   set_text(text);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::GetText --
 *
 *      Returns the normalized text of the entry. This is needed because
 *      Gtk::Entry::get_text() will actually return our marked up text,
 *      but as that's not a virtual function, we can't do much to prevent
 *      that.
 *
 * Results:
 *      The normalized text in the entry.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

Glib::ustring
FieldEntry::GetText(void)
   const
{
   return get_chars(0, -1);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::SetFieldText --
 *
 *      Sets the text in the specified field.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      May emit fieldTextChanged.
 *
 *      The position may change if the current field is the one being
 *      modified. The cursor is also guaranteed to be in a field by the
 *      end of this.
 *
 *-----------------------------------------------------------------------------
 */

void
FieldEntry::SetFieldText(size_t field,              // IN: The field to set
                         const Glib::ustring& text) // IN: The new text
{
   g_return_if_fail(field < GetFieldCount());
   g_return_if_fail(text.length() <= mMaxFieldWidth);

   SetField(field, text);
   ComputeLayout();

   size_t savedField;
   size_t savedPosInField;

   savedField = GetCurrentField(&savedPosInField);

   ApplyLayout();

   SetCurrentField(savedField, savedPosInField);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::GetFieldText --
 *
 *      Returns the text contained in the specified field.
 *
 * Results:
 *      The field's text.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

Glib::ustring
FieldEntry::GetFieldText(size_t field) // IN:
   const
{
   g_return_val_if_fail(field < GetFieldCount(), "");

   return mFields[field].val;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::SetCurrentField --
 *
 *      Sets the current field that the cursor should be on, and optionally,
 *      the position in the field. Value < 0 for posInField will set the
 *      cursor on the end of the field.
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
FieldEntry::SetCurrentField(size_t field,   // IN: The field to jump to
                            int posInField) // IN: The position in the field
{
   g_return_if_fail(field < GetFieldCount());

   if (posInField < 0) {
      posInField = mFields[field].val.length();
   }

   posInField = MIN(posInField, mFields[field].val.length());

   set_position(Field2Position(field) + posInField);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::GetCurrentField --
 *
 *      Returns the field that the cursor is on.
 *
 * Results:
 *      The current field position and, optionally, the position in the field.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

size_t
FieldEntry::GetCurrentField(size_t* posInField) // OUT: Position in the field
   const
{
   size_t field;
   size_t myPosInField;

   Position2Field(get_position(), field, myPosInField);

   if (posInField != NULL) {
      *posInField = myPosInField;
   }

   return field;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::GetFieldCount --
 *
 *      Returns the number of fields in the entry.
 *
 * Results:
 *      The number of fields.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

size_t
FieldEntry::GetFieldCount(void)
   const
{
   return mFields.size();
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::GetIsFieldValid --
 *
 *      Virtual function to determine if the specified text is valid for
 *      a field.
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
FieldEntry::GetIsFieldValid(const Glib::ustring& str) // IN: New data
   const
{
   return true;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::GetAllowedFieldChars --
 *
 *      Returns a list of allowed field characters, or "" if any character
 *      can be used. This is intended for subclasses to override. By default,
 *      any character is allowed (aside from tabs and delimiters).
 *
 * Results:
 *      An empty string.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

Glib::ustring
FieldEntry::GetAllowedFieldChars(size_t field) // IN:
   const
{
   return "";
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::get_chars_vfunc --
 *
 *      Overridden virtual function handler that returns the normalized
 *      string within the range passed.
 *
 * Results:
 *      The normalized entry text.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

Glib::ustring
FieldEntry::get_chars_vfunc(int startPos, // IN:
                            int endPos)   // IN:
   const
{
   Glib::ustring newStr;
   bool empty = true;

   if (endPos < 0) {
      endPos = mMarkedUp.length();
   }

   for (int i = startPos; i < endPos; i++) {
      Glib::ustring::value_type c = mMarkedUp[i];
      if (c != sTabChar) {
         newStr += c;

         if (c != mDelim) {
            empty = false;
         }
      }
   }

   return empty ? "" : newStr;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::on_expose_event --
 *
 *      Expose event handler.
 *
 *      XXX GtkEntry deficiency: we need to set our tab stops in the GtkEntry's
 *          PangoLayout. GtkEntry destroys and re-creates the PangoLayout all
 *          the time, but there is no signal to notify us when it happens. So
 *          as it is, the get_layout() API is pretty incomplete. To workaround
 *          that, we set our tab stops in the GtkEntry's PangoLayout everytime
 *          the entry is exposed. --hpreg
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

bool
FieldEntry::on_expose_event(GdkEventExpose* event) // IN: Event
{
   ComputeLayout();
   ApplyLayout();

   return DeadEntry::on_expose_event(event);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::insert_text_vfunc --
 *
 *      Overridden virtual function handler to insert text at the
 *      specified position. This will insert across fields, taking into
 *      account field widths, the delimiters, and whether the fields
 *      are valid. This is the same function that handles user input, and
 *      thus treats a SetText() and clipboard pastes with the exact same
 *      logic as the user typing. D&D text drops are also handled by this.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Emits fieldTextChanged for every updated field.
 *
 *-----------------------------------------------------------------------------
 */

void
FieldEntry::insert_text_vfunc(const Glib::ustring& text, // IN:
                              int& position)             // IN/OUT:
{
   size_t field;
   size_t posInField;

   /* Determine the field location. */

   Position2Field(position, field, posInField);

   /*
    * Iterate through all input characters, as if the user had typed them one
    * by one.
    */

   for (int i = 0; i < text.length(); i++) {
      if (text[i] == sTabChar) {
         /* Tabs in the input would conflict with our tab stops hack. */
         break;
      }

      size_t candidateField;
      size_t candidatePosInField;

      if (text[i] == mDelim || mFields[field].val.length() == mMaxFieldWidth) {
         if (   posInField != mFields[field].val.length()
             || field == GetFieldCount() - 1) {
            break;
         }

         /* Try to apply operation at the beginning of the next field. */
         candidateField = field + 1;
         candidatePosInField = 0;
      } else {
         /* Try to apply operation at the current field location. */
         candidateField = field;
         candidatePosInField = posInField;
      }

      if (text[i] == mDelim) {
         /* The operation is a no-op, which always succeeds. */
         field = candidateField;
         posInField = candidatePosInField;
      } else {
         /* The operation is an insertion of the character. */
         Glib::ustring temp = mFields[candidateField].val;
         temp.insert(candidatePosInField, 1, text[i]);
         if (!GetIsFieldValid(temp)) {
            break;
         }

         SetField(candidateField, temp);
         ComputeLayout();
         field = candidateField;
         posInField = candidatePosInField + 1;
      }
   }

   ApplyLayout();

   /* Make sure 'currentFieldChanged' will be emitted if needed. */
   set_position(Field2Position(field) + posInField);

   /*
    * Firing the 'currentFieldChanged' signal can modify the content of the
    * entry and the current position of the entry. So re-read the current
    * position from the entry to return the correct value for 'position'.
    */
   position = get_position();
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::delete_text_vfunc --
 *
 *      Overridden virtual function to handle text deletion. Moves to
 *      the previous field if the user pressed backspace at the beginning
 *      of a field. Otherwise, deletes the text across all fields that are
 *      passed.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Emits fieldTextChanged for every affected field.
 *
 *-----------------------------------------------------------------------------
 */

void
FieldEntry::delete_text_vfunc(int startPos, // IN:
                              int endPos)   // IN:
{
   /* Normalize the end position. < 0 means the end of the string. */

   if (endPos < 0) {
      endPos = mMarkedUp.length();
   }

   size_t startField;
   size_t startPosInField;
   size_t endField;
   size_t endPosInField;

   /* Determine the start field location. */

   Position2Field(startPos, startField, startPosInField);
   size_t inFieldStartPos = Field2Position(startField) + startPosInField;
   if (inFieldStartPos > startPos) {
      /*
       * The left edge of the start field is being deleted.
       *
       * If the field is not the first one, move the start location to the end
       * of the previous field.
       */

      if (startField > 0) {
         startField--;
         startPosInField = mFields[startField].val.length();
      }
   }

   /* Determine the end field location. */

   Position2Field(endPos, endField, endPosInField);

   /* Delete all content between both field locations. */

   if (startField == endField) {
      SetField(startField,   mFields[startField].val.substr(0, startPosInField)
                           + mFields[startField].val.substr(endPosInField));
   } else {
      SetField(startField, mFields[startField].val.substr(0, startPosInField));
      for (size_t i = startField + 1; i < endField; i++) {
         SetField(i, "");
      }
      SetField(endField, mFields[endField].val.substr(endPosInField));
   }

   /* This invalidates all marked up positions. */
   ComputeLayout();
   ApplyLayout();

   set_position(Field2Position(startField) + startPosInField);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::set_position_vfunc --
 *
 *      Overridden virtual function to set the position in the entry.
 *      This takes into account the field separators and the direction
 *      the user may be moving the cursor and intelligently places the
 *      cursor inside of a field.
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
FieldEntry::set_position_vfunc(int position) // IN: The new position
{
   /* Determine the old field location. */

   size_t oldField;
   size_t oldPosInField;

   Position2Field(get_position(), oldField, oldPosInField);

   /* Normalize the new position. < 0 means the end of the string. */

   if (position < 0) {
      position = mMarkedUp.length();
   }

   /* Determine the new field location. */

   size_t field;
   size_t posInField;

   Position2Field(position, field, posInField);

   size_t inFieldNewPos = Field2Position(field) + posInField;
   if (inFieldNewPos > position) {
      /*
       * The position is on the left edge of the field.
       *
       * If the user was already at the beginning of the field, and the field
       * is not the first one, jump to the end of the previous field.
       *
       * If the user was not already at the beginning of the field, then jump
       * to the beginning of the field.
       */

      if (oldField == field && oldPosInField == 0 && field > 0) {
         field--;
         posInField = mFields[field].val.length();
      }
   } else if (inFieldNewPos < position) {
      /*
       * The position is on the right edge of the field.
       *
       * If the user was already at the end of the field, and the field is not
       * the last one, jump to the beginning of the next field.
       *
       * If the user was not already at the end of the field, then jump to the
       * end of the field.
       */

     if (   oldField == field && oldPosInField == mFields[field].val.length()
         && field < GetFieldCount() - 1) {
         field++;
         posInField = 0;
      }
   } else {
      /* The position is inside a field. Just jump to it. */
   }

   DeadEntry::set_position_vfunc(Field2Position(field) + posInField);

   if (oldField != field) {
      size_t savedField;
      size_t savedPosInField;

      savedField = GetCurrentField(&savedPosInField);

      currentFieldChanged.emit(oldField);

      SetCurrentField(savedField, savedPosInField);
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::ComputeLayout --
 *
 *      Compute the entire layout state: marked up string and associated
 *      meta-data, tab stops positions, ...
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Updates mMarkedUp. Consequently, all previously computed marked up
 *      positions become stale.
 *
 *-----------------------------------------------------------------------------
 */

void
FieldEntry::ComputeLayout()
{
   /* Use the max size initially. */
   mTabs.resize(2 * GetFieldCount());

   /*
    * We can't cache these values, as the theme or font might change.
    * We could listen for when the theme or font properties change, but
    * that's a bit more work than we really should need to do right now.
    */
   Glib::RefPtr<Pango::Layout> layout =
      create_pango_layout(Glib::ustring(1, mDelim));
   int delimWidth;
   int height;
   layout->get_pixel_size(delimWidth, height);

   int offset = 0;
   mMarkedUp = "";
   size_t tabIndex = 0;

   for (size_t i = 0; i < GetFieldCount(); i++) {
      int textWidth;
      layout->set_text(mFields[i].val);
      layout->get_pixel_size(textWidth, height);

      Glib::ustring allowedChars = GetAllowedFieldChars(i);
      if (allowedChars == "") {
         allowedChars = "W";
      }

      int maxTextWidth = utils::GetLargestCharStrWidth(*this, allowedChars,
                                                       mMaxFieldWidth);

      int fieldOffset = offset + (maxTextWidth - textWidth) / 2;

      if (fieldOffset != offset) {
         mMarkedUp += sTabChar;
         mTabs.set_tab(tabIndex, Pango::TAB_LEFT, fieldOffset);
         tabIndex++;
      }

      mFields[i].pos = mMarkedUp.length();
      mMarkedUp += mFields[i].val;
      offset += maxTextWidth;

      /*
       * Always put a tab stop after the last field. This way selecting the
       * beginning of the entry is symetrical with slecting the end of the
       * entry.
       */

      if (offset != fieldOffset + textWidth) {
         mMarkedUp += sTabChar;
         mTabs.set_tab(tabIndex, Pango::TAB_LEFT, offset);
         tabIndex++;
      }

      if (i != GetFieldCount() - 1) {
         mMarkedUp += mDelim;
         offset += delimWidth;
      }
   }

   /* Resize in case we have used less tab stops than the max. */
   mTabs.resize(tabIndex);
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::ApplyLayout --
 *
 *      Apply the layout state previously computed in ComputeLayout() to the
 *      DeadEntry. Emit all 'fieldTextChanged' signals whose emission has been
 *      delayed in SetField().
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      May emit signals, modified the current position in the entry.
 *
 *-----------------------------------------------------------------------------
 */

void
FieldEntry::ApplyLayout()
{
   get_layout()->set_tabs(mTabs);
   get_layout()->context_changed();

   if (get_text() != mMarkedUp) {
      DeadEntry::delete_text_vfunc(0, -1);
      /* We can't just pass 0, because insert_text_vfunc takes a int&. */
      int pos = 0;
      DeadEntry::insert_text_vfunc(mMarkedUp, pos);
   }

   for (size_t i = 0; i < GetFieldCount(); i++) {
      if (mFields[i].dirty) {
         mFields[i].dirty = false;
         fieldTextChanged.emit(i);
      }
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::SetField --
 *
 *      Set the value of a field. The firing of the corresponding
 *      'fieldTextChanged' signal, if needed, is delayed until you call
 *      ApplyLayout().
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
FieldEntry::SetField(size_t field,              // IN
                     const Glib::ustring& text) // IN
{
   Field &f = mFields[field];

   if (f.val == text) {
      return;
   }

   f.val = text;
   f.dirty = true;
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::Position2Field --
 *
 *      Retrieve the nearest field location corresponding to a position in the
 *      marked up string.
 *
 *      XXX Can be done in O(1) time if needed, by building an
 *          array[position] = field_location at the time we build the marked up
 *          string. --hpreg
 *
 * Results:
 *      Field location.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

void
FieldEntry::Position2Field(size_t position,    // IN
                           size_t& field,      // OUT
                           size_t& posInField) // OUT
   const
{
   /*
    *  mMarkedUp:  t   f f f   t   d   t   f f f   t   d   t   f f f   t
    *             |  |  | |  |   |   |   |  | |  |   |   |   |  | |  |  |
    *   position: 0                                                     length
    *      field: 0  0  0 0  0   0   1   1  1 1  1   1   2   2  2 2  2  2
    * posInField: 0  0  1 2  3   3   0   0  1 2  3   3   0   0  1 2  3  3
    */

   field = 0;
   posInField = 0;

   g_return_if_fail(position <= mMarkedUp.length());

   for (size_t i = 1; i <= position; i++) {
      Glib::ustring::value_type c = mMarkedUp[i - 1];
      if (c == mDelim) {
         field++;
         posInField = 0;
      } else if (c == sTabChar) {
         /* Ignored. This is good, because they are optional. */
      } else {
         /* Non-special character. */
         posInField++;
      }
   }
}


/*
 *-----------------------------------------------------------------------------
 *
 * view::FieldEntry::Field2Position --
 *
 *      Returns the position in the marked up string at which the
 *      field begins.
 *
 * Results:
 *      The position at which the field begins.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */

size_t
FieldEntry::Field2Position(size_t field) // IN: The field
   const
{
   return mFields[field].pos;
}


} /* namespace view */
