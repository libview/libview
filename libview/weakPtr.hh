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
 * weakPtr.hh --
 *
 *      The WeakPtr is a simple weak pointer implementation for classes
 *      that inherit from sigc::trackable. It will automatically set itself to
 *      NULL when the object it tracks is destroyed.
 */

#ifndef LIBVIEW_WEAKPTR_HH
#define LIBVIEW_WEAKPTR_HH


namespace view {


template<class T>
class WeakPtr
{
public:
   /*
    *------------------------------------------------------------------------
    *
    * view::WeakPtr::WeakPtr --
    *
    *      Constructors.
    *
    *      The default constructor creates a weak pointer that just points
    *      to NULL. The typical constructor takes a raw trackable pointer
    *      and is explicit to avoid ambiguity problems.
    *      Copy constructor is standard fair.
    *
    * Results:
    *      None.
    *
    * Side effects:
    *      None.
    *
    *------------------------------------------------------------------------
    */

   inline WeakPtr() : mTrackable(0) {}
   inline explicit WeakPtr(T *trackable) : mTrackable(trackable) { Init(); }
   inline WeakPtr(const WeakPtr<T> &src) : mTrackable(src.mTrackable) { Init(); }


   /*
    *------------------------------------------------------------------------
    *
    * view::WeakPtr::~WeakPtr --
    *
    *      Destructor.
    *
    * Results:
    *      None.
    *
    * Side effects:
    *      None.
    *
    *------------------------------------------------------------------------
    */

   inline ~WeakPtr()
   {
      if (mTrackable) {
         mTrackable->remove_destroy_notify_callback(&mTrackable);
      }
   }


   /*
    *------------------------------------------------------------------------
    *
    * view::WeakPtr::operator= --
    *
    *      Assignment operators. Allows assignment from a raw trackable or
    *      another weak pointer.
    *
    * Results:
    *      The WeakPtr.
    *
    * Side effects:
    *      None.
    *
    *------------------------------------------------------------------------
    */

   inline WeakPtr<T> &operator=(T *trackable)
   {
      mTrackable = trackable;
      Init();
      return *this;
   }

   inline WeakPtr<T> &operator=(const WeakPtr<T> &src)
   {
      mTrackable = src.mTrackable;
      Init();
      return *this;
   }


   /*
    *------------------------------------------------------------------------
    *
    * view::WeakPtr::operator== --
    *
    *      Equality operator.
    *
    * Results:
    *      true if equal, false if not.
    *
    * Side effects:
    *      None.
    *
    *------------------------------------------------------------------------
    */

   inline bool operator==(const WeakPtr<T> &src) const 
   {
      return (mTrackable == src.mTrackable);
   }


   /*
    *------------------------------------------------------------------------
    *
    * view::WeakPtr::operator!= --
    *
    *      Inequality operator.
    *
    * Results:
    *      true if not equal, false if equal.
    *
    * Side effects:
    *      None.
    *
    *------------------------------------------------------------------------
    */

   inline bool operator!=(const WeakPtr<T> &src) const
   {
      return (mTrackable != src.mTrackable);
   }


   /*
    *------------------------------------------------------------------------
    *
    * view::WeakPtr::operator T* --
    *
    *      Cast operator to use a WeakPtr<T> when a T* is expected.
    *
    * Results:
    *      The raw T*.
    *
    * Side effects:
    *      None.
    *
    *------------------------------------------------------------------------
    */

   inline operator T*() const { return mTrackable; }


   /*
    *------------------------------------------------------------------------
    *
    * view::WeakPtr::operator-> --
    *
    *      Operator to allow calling T* methods on a WeakPtr<T>.
    *
    * Results:
    *      The raw T*.
    *
    * Side effects:
    *      None.
    *
    *------------------------------------------------------------------------
    */

   inline T *operator->() const { return mTrackable; }


   /*
    *------------------------------------------------------------------------
    *
    * view::WeakPtr::operator* --
    *
    *      Dereference operator to return the trackable as expected.
    *
    * Results:
    *      Reference to the raw trackable.
    *
    * Side effects:
    *      None.
    *
    *------------------------------------------------------------------------
    */

   inline T &operator*() const { return *mTrackable; }


   /*
    *------------------------------------------------------------------------
    *
    * view::WeakPtr::operator bool --
    *
    *      Cast operator to check if a WeakPtr is pointing to anything.
    *
    * Results:
    *      true if it points to something, false if not.
    *
    * Side effects:
    *      None.
    *
    *------------------------------------------------------------------------
    */

   inline operator bool() const { return (mTrackable != NULL); }
   inline bool operator!() const { return !operator bool(); }


private:
   T *mTrackable;


   /*
    *------------------------------------------------------------------------
    *
    * view::WeakPtr::OnDestroyNotify --
    *
    *      Notification callback for destruction of trackable object that
    *      WeakPtr is pointing to. The WeakPtr will be reset to point to NULL.
    *
    * Results:
    *      None.
    *
    * Side effects:
    *      None.
    *
    *------------------------------------------------------------------------
    */

   inline static void *OnDestroyNotify(void *data)
   {
      T **trackable = reinterpret_cast<T **>(data);

      *trackable = NULL;
      return NULL;
   }


   /*
    *------------------------------------------------------------------------
    *
    * view::WeakPtr::Init --
    *
    *      Initialise the WeakPtr by attaching a destroy notify callback to the
    *      pointed-to trackable object.
    *
    * Results:
    *      None.
    *
    * Side effects:
    *      None.
    *
    *------------------------------------------------------------------------
    */

   inline void Init()
   {
      if (mTrackable) {
         mTrackable->add_destroy_notify_callback(&mTrackable,
                                                 &WeakPtr::OnDestroyNotify);
      }
   }
};


} // namespace view


#endif // LIBVIEW_WEAKPTR_HH
