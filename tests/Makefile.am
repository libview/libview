noinst_PROGRAMS = \
	test-dead-entry \
	test-drawer \
	test-wrap-label


common_ldflags = \
	$(top_builddir)/libview/libview.la \
	$(VIEW_LIBS)


test_dead_entry_SOURCES = test-dead-entry.cc
test_dead_entry_LDADD   = $(common_ldflags)


test_drawer_SOURCES = test-drawer.cc
test_drawer_LDADD   = $(common_ldflags)


test_wrap_label_SOURCES = test-wrap-label.cc
test_wrap_label_LDADD   = $(common_ldflags)


INCLUDES = \
	-I$(top_srcdir) \
	$(VIEW_CFLAGS)
