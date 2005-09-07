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
 * ovBox.h --
 *
 *      Declarations for the ViewOvBox GTK+ widget.
 */

#ifndef LIBVIEW_OVBOX_H
#define LIBVIEW_OVBOX_H


#include <gtk/gtk.h>


#define VIEW_TYPE_OV_BOX ViewOvBox_GetType()
#define VIEW_OV_BOX(obj) GTK_CHECK_CAST((obj), VIEW_TYPE_OV_BOX, ViewOvBox)
#define VIEW_OV_BOX_CLASS(klass) GTK_CHECK_CLASS_CAST((klass), VIEW_TYPE_OV_BOX, \
                                                   ViewOvBoxClass)
#define VIEW_IS_OV_BOX(obj) GTK_CHECK_TYPE((obj), VIEW_TYPE_OV_BOX)
#define VIEW_IS_OV_BOX_CLASS(klass) GTK_CHECK_CLASS_TYPE((klass), VIEW_TYPE_OV_BOX)


typedef struct _ViewOvBox {
   /* Must come first. */
   GtkBox parent;

   GdkWindow *underWin;
   GtkWidget *under;
   GdkWindow *overWin;
   GtkWidget *over;
   GtkRequisition overR;
   unsigned int min;
   double fraction;
} ViewOvBox;


typedef struct _ViewOvBoxClass {
   /* Must come first. */
   GtkBoxClass parent;
} ViewOvBoxClass;


G_BEGIN_DECLS


GtkType
ViewOvBox_GetType(void);

GtkWidget *
ViewOvBox_New(void);

void
ViewOvBox_SetUnder(ViewOvBox *that,
                 GtkWidget *widget);

void
ViewOvBox_SetOver(ViewOvBox *that,
                GtkWidget *widget);

void
ViewOvBox_SetMin(ViewOvBox *that,
               unsigned int min);

void
ViewOvBox_SetFraction(ViewOvBox *that,
                    double fraction);

double
ViewOvBox_GetFraction(ViewOvBox *that);


G_END_DECLS


#endif /* LIBVIEW_OVBOX_H */
