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
 * defines.h --
 *
 *      Common definitions and macros.
 */

#ifndef LIBVIEW_DEFINES_H
#define LIBVIEW_DEFINES_H


#include <glib.h>


G_BEGIN_DECLS


/*
 * Compare compile-time GTKMM versions. Contrast with GTK_CHECK_VERSION() which
 * compares compile-time GTK versions.
 */
#ifndef GTKMM_CHECK_VERSION
# define	GTKMM_CHECK_VERSION(major, minor, micro) \
   (   GTKMM_MAJOR_VERSION > (major)                     \
    || (   GTKMM_MAJOR_VERSION == (major)                \
        && (   GTKMM_MINOR_VERSION > (minor)             \
            || (   GTKMM_MINOR_VERSION == (minor)        \
                && GTKMM_MICRO_VERSION >= (micro)))))
#endif


G_END_DECLS


#endif /* LIBVIEW_DEFINES_H */
