viewincdir = $(includedir)/libview

lib_LTLIBRARIES = libview.la

viewinc_HEADERS = \
	actionGroup.hh \
	autoDrawer.h \
	baseBGBox.hh \
	contentBox.hh \
	deadEntry.hh \
	defines.h \
	drawer.h \
	fieldEntry.hh \
	header.hh \
	ipEntry.hh \
	menuToggleAction.hh \
	motionTracker.hh \
	ovBox.h \
	reparenter.hh \
	spinnerAction.hh \
	spinner.hh \
	toolTip.hh \
	uiGroup.hh \
	undoableTextView.hh \
	utils.hh \
	view.hh \
	viewport.hh \
	weakPtr.hh \
	widthHeight.hh \
	wrapLabel.hh

libview_la_SOURCES = \
	autoDrawer.c \
	baseBGBox.cc \
	contentBox.cc \
	deadEntry.cc \
	drawer.c \
	fieldEntry.cc \
	header.cc \
	ipEntry.cc \
	menuToggleAction.cc \
	motionTracker.cc \
	ovBox.c \
	reparenter.cc \
	spinnerAction.cc \
	spinner.cc \
	toolTip.cc \
	uiGroup.cc \
	undoableTextView.cc \
	utils.cc \
	viewport.cc \
	widthHeight.cc \
	wrapLabel.cc

libview_la_LIBADD = $(VIEW_LIBS)
libview_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)


INCLUDES = \
	-I$(top_srcdir) \
	$(VIEW_CFLAGS)
