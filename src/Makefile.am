.SECONDARY:

EXTRA_DIST = emacsy-webkit-gtk.scm 

CLEANFILES=main.c.x 

if ENABLE_WEBKIT
bin_PROGRAMS = emacsy-webkit-gtk

run-local: emacsy-webkit-gtk
	$(ENV) ./emacsy-webkit-gtk

else
		$(warning Warning: WEBKIT not found; can't build demo.)
bin_PROGRAMS = 
endif

AM_CFLAGS  = $(EMACSY_CFLAGS) $(GUILE_CFLAGS) $(WEBKIT_CFLAGS)
emacsy_webkit_gtk_LDADD   = $(EMACSY_LIBS) $(GUILE_LIBS) $(WEBKIT_LIBS)
emacsy_webkit_gtk_LDFLAGS = 
emacsy_webkit_gtk_SOURCES = main.c

main.o : main.c.x

include $(top_srcdir)/support/automake/guile.am

MAINTAINERCLEANFILES = Makefile.in config.h.in 


