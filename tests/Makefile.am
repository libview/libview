noinst_PROGRAMS = \
	test-auto-drawer \
	test-content-box \
	test-dead-entry \
	test-drawer \
	test-field-entry \
	test-header-bgbox \
	test-ip-entry \
	test-ovBox \
	test-wrap-label


common_ldflags = \
	$(top_builddir)/libview/libview.la \
	$(VIEW_LIBS)


test_auto_drawer_SOURCES = test-auto-drawer.cc
test_auto_drawer_LDADD   = $(common_ldflags)


test_content_box_SOURCES = test-content-box.cc
test_content_box_LDADD   = $(common_ldflags)


test_dead_entry_SOURCES = test-dead-entry.cc
test_dead_entry_LDADD   = $(common_ldflags)


test_drawer_SOURCES = test-drawer.cc
test_drawer_LDADD   = $(common_ldflags)


test_field_entry_SOURCES = test-field-entry.cc
test_field_entry_LDADD   = $(common_ldflags)


test_header_bgbox_SOURCES = test-header-bgbox.cc
test_header_bgbox_LDADD   = $(common_ldflags)


test_ip_entry_SOURCES = test-ip-entry.cc
test_ip_entry_LDADD   = $(common_ldflags)


test_ovBox_SOURCES = test-ovBox.cc
test_ovBox_LDADD   = $(common_ldflags)


test_wrap_label_SOURCES = test-wrap-label.cc
test_wrap_label_LDADD   = $(common_ldflags)


INCLUDES = \
	-I$(top_srcdir) \
	$(VIEW_CFLAGS) \
	$(DEBUG_CFLAGS)
