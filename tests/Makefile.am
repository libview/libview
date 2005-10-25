noinst_PROGRAMS = \
	test-dead-entry \
	test-ovBox \
	test-drawer \
	test-header-bgbox \
	test-ip-entry \
	test-wrap-label


common_ldflags = \
	$(top_builddir)/libview/libview.la \
	$(VIEW_LIBS)


test_dead_entry_SOURCES = test-dead-entry.cc
test_dead_entry_LDADD   = $(common_ldflags)


test_ovBox_SOURCES = test-ovBox.cc
test_ovBox_LDADD   = $(common_ldflags)


test_drawer_SOURCES = test-drawer.cc
test_drawer_LDADD   = $(common_ldflags)


test_header_bgbox_SOURCES = test-header-bgbox.cc
test_header_bgbox_LDADD   = $(common_ldflags)


test_ip_entry_SOURCES = test-ip-entry.cc
test_ip_entry_LDADD   = $(common_ldflags)


test_wrap_label_SOURCES = test-wrap-label.cc
test_wrap_label_LDADD   = $(common_ldflags)


INCLUDES = \
	-I$(top_srcdir) \
	$(VIEW_CFLAGS)
