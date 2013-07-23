.SECONDARY:

if ENABLE_WEBKIT
CLEANFILES=main.c.x 

bin_PROGRAMS = emacsy-webkit-gtk

EXTRA_DIST = emacsy-webkit-gtk.scm

AM_CFLAGS  = $(EMACSY_CFLAGS) $(GUILE_CFLAGS) $(WEBKIT_CFLAGS)
emacsy_webkit_gtk_LDADD   = $(EMACSY_LIBS) $(GUILE_LIBS) $(WEBKIT_LIBS)
emacsy_webkit_gtk_LDFLAGS = 
emacsy_webkit_gtk_SOURCES = main.c

main.o : main.c.x

MAINTAINERCLEANFILES = Makefile.in config.h.in 

include $(top_srcdir)/support/automake/guile.am

run-local: emacsy-webkit-gtk
	$(ENV) ./emacsy-webkit-gtk
else
		$(warning Warning: WEBKIT not found; can't build demo.)
endif
