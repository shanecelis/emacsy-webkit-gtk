.SECONDARY:

EXTRA_DIST = emacsy-webkit-gtk.scm 

CLEANFILES=emacsy-webkit-gtk.c.x emacsy-webkit-gtk-w-buffers.c.x emacsy-webkit-gtk-w-windows.c.x

if ENABLE_WEBKIT
bin_PROGRAMS = emacsy-webkit-gtk emacsy-webkit-gtk-w-buffers emacsy-webkit-gtk-w-windows 

run-local: emacsy-webkit-gtk
	echo "Running demo 1."
	$(ENV) ./emacsy-webkit-gtk
	echo "Running demo 2. buffers!"
	$(ENV) ./emacsy-webkit-gtk-w-buffers
	echo "Running demo 3. windows!"
	$(ENV) ./emacsy-webkit-gtk-w-windows

else
		$(warning Warning: WEBKIT not found; can't build demo.)
bin_PROGRAMS = 
endif

AM_CFLAGS  = $(EMACSY_CFLAGS) $(GUILE_CFLAGS) $(WEBKIT_CFLAGS)
emacsy_webkit_gtk_LDADD   = $(EMACSY_LIBS) $(GUILE_LIBS) $(WEBKIT_LIBS)
emacsy_webkit_gtk_SOURCES = emacsy-webkit-gtk.c

emacsy_webkit_gtk_w_buffers_LDADD   = $(EMACSY_LIBS) $(GUILE_LIBS) $(WEBKIT_LIBS)
emacsy_webkit_gtk_w_buffers_SOURCES = emacsy-webkit-gtk-w-buffers.c

emacsy_webkit_gtk_w_windows_LDADD   = $(EMACSY_LIBS) $(GUILE_LIBS) $(WEBKIT_LIBS)
emacsy_webkit_gtk_w_windows_SOURCES = emacsy-webkit-gtk-w-windows.c

emacsy-webkit-gtk.o : emacsy-webkit-gtk.c.x

emacsy-webkit-gtk-w-buffers.o : emacsy-webkit-gtk-w-buffers.c.x

emacsy-webkit-gtk-w-windows.o : emacsy-webkit-gtk-w-windows.c.x

include $(top_srcdir)/support/automake/guile.am

MAINTAINERCLEANFILES = Makefile.in config.h.in 


