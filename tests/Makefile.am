noinst_PROGRAMS = \
	test-drawer


common_ldflags = \
	$(top_builddir)/libview/libview.la \
	$(VIEW_LIBS)


test_drawer_SOURCES = test-drawer.cc
test_drawer_LDADD   = $(common_ldflags)


INCLUDES = \
	-I$(top_srcdir) \
	$(VIEW_CFLAGS)
