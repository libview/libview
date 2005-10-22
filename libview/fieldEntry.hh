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
 * fieldEntry.hh --
 *
 *      An entry widget that separates data by fields. Such uses could be
 *      an IP entry widget or a serial number entry widget.
 */


#ifndef LIBVIEW_FIELDENTRY_HH
#define LIBVIEW_FIELDENTRY_HH


#include <libview/deadEntry.hh>


namespace view {


class FieldEntry
   : public DeadEntry
{
public:
   FieldEntry(size_t fieldCount, size_t maxFieldWidth,
              Glib::ustring::value_type delim);

   void SetText(const Glib::ustring& text);
   Glib::ustring GetText(void) const;

   void SetFieldText(size_t field, const Glib::ustring& text);
   Glib::ustring GetFieldText(size_t field) const;

   void SetCurrentField(size_t field, int posInField = 0);
   size_t GetCurrentField(size_t* posInField = NULL) const;

   size_t GetFieldCount(void) const;

   sigc::signal<void, size_t /* field */> fieldTextChanged;
   sigc::signal<void, size_t /* oldField */> currentFieldChanged;

protected:
   virtual bool GetIsFieldValid(const Glib::ustring& str) const;
   virtual Glib::ustring GetAllowedFieldChars(size_t field) const;

   virtual Glib::ustring get_chars_vfunc(int startPos, int endPos) const;
   virtual bool on_expose_event(GdkEventExpose* event);
   virtual void insert_text_vfunc(const Glib::ustring& text, int& position);
   virtual void delete_text_vfunc(int startPos, int endPos);
   virtual void set_position_vfunc(int position);

private:
   static const Glib::ustring::value_type sTabChar = '\t';

   struct Field {
      size_t pos;
      Glib::ustring val;
      bool dirty;
   };

   void OnScrollOffsetChanged();
   void SetField(size_t field, const Glib::ustring& text);
   void ComputeLayout();
   void ApplyLayout();
   void Position2Field(size_t position, size_t &field,
                       size_t &posInField) const;
   size_t Field2Position(size_t field) const;

   size_t mMaxFieldWidth;
   Glib::ustring::value_type mDelim;
   std::vector<Field> mFields;
   Pango::TabArray mTabs;
   Glib::ustring mMarkedUp;
};


} /* namespace view */


#endif /* LIBVIEW_FIELDENTRY_HH */
