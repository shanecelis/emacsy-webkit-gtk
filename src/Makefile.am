.SECONDARY:

EXTRA_DIST = emacsy-webkit-gtk.scm 

CLEANFILES=main.c.x 

if ENABLE_WEBKIT
bin_PROGRAMS = emacsy-webkit-gtk emacsy-webkit-gtk-w-buffers

run-local: emacsy-webkit-gtk
	$(ENV) ./emacsy-webkit-gtk

else
		$(warning Warning: WEBKIT not found; can't build demo.)
bin_PROGRAMS = 
endif

AM_CFLAGS  = $(EMACSY_CFLAGS) $(GUILE_CFLAGS) $(WEBKIT_CFLAGS)
emacsy_webkit_gtk_LDADD   = $(EMACSY_LIBS) $(GUILE_LIBS) $(WEBKIT_LIBS)
emacsy_webkit_gtk_SOURCES = main.c

emacsy_webkit_gtk_w_buffers_LDADD   = $(EMACSY_LIBS) $(GUILE_LIBS) $(WEBKIT_LIBS)
emacsy_webkit_gtk_w_buffers_SOURCES = emacsy-webkit-gtk-w-buffers.c

main.o : main.c.x

emacsy-webkit-gtk-w-buffers.o : emacsy-webkit-gtk-w-buffers.c.x

include $(top_srcdir)/support/automake/guile.am

MAINTAINERCLEANFILES = Makefile.in config.h.in 


