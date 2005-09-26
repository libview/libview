viewincdir = $(includedir)/libview

lib_LTLIBRARIES = libview.la

viewinc_HEADERS = \
	actionGroup.hh \
	baseBGBox.hh \
	contentBox.hh \
	deadEntry.hh \
	defines.h \
	drawer.h \
	header.hh \
	menuToggleAction.hh \
	motionTracker.hh \
	ovBox.h \
	reparenter.hh \
	spinnerAction.hh \
	spinner.hh \
	toolTip.hh \
	uiGroup.hh \
	undoableTextView.hh \
	view.hh \
	viewport.hh \
	weakPtr.hh \
	widthHeight.hh \
	wrapLabel.hh

libview_la_SOURCES = \
	baseBGBox.cc \
	contentBox.cc \
	deadEntry.cc \
	drawer.c \
	header.cc \
	menuToggleAction.cc \
	motionTracker.cc \
	ovBox.c \
	reparenter.cc \
	spinnerAction.cc \
	spinner.cc \
	toolTip.cc \
	uiGroup.cc \
	undoableTextView.cc \
	viewport.cc \
	widthHeight.cc \
	wrapLabel.cc

libview_la_LIBADD = $(VIEW_LIBS)


INCLUDES = \
	-I$(top_srcdir) \
	$(VIEW_CFLAGS)
