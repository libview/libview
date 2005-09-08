SUBDIRS = docs libview tests

pcdata_DATA = libview.pc
pcdatadir = $(libdir)/pkgconfig


EXTRA_DIST = \
	AUTHORS NEWS INSTALL COPYING README \
	ChangeLog \
	autogen.sh \
	configure.ac \
	libview.pc.in


$(OBJECTS): libtool
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

doxygen:
	doxygen
