.SECONDARY:

# BUILT_SOURCES =
# CLEANFILES=hello-emacsy.c.x 

# dist-hook:
# 	cp $(distdir)/*.pdf $(top_distdir)

# WARN_NOTANGLE = $(top_builddir)/bin/warn-notangle

# NOWEB_DOCS = hello-emacsy-paper.pdf
# NOWEB_FILES = hello-emacsy.nw hello-emacsy-paper.nw
# NOWEB_PRODUCTS = hello-emacsy.c .hello-emacsy.scm
# EXTRA_DIST = $(NOWEB_DOCS)

# doc-local : 	hello-emacsy-paper.pdf


# hello-emacsy-paper.pdf : hello-emacsy.tex
# # hello-emacsy.c.x
# hello-emacsy.c.x : hello-emacsy.c
# hello-emacsy.o : hello-emacsy.c.x

# .hello-emacsy.scm : hello-emacsy.nw
# 	$(AM_V_GEN) $(WARN_NOTANGLE) -o $@ $(WARN_NOTANGLE_FLAGS) -- $(NOTANGLE_LISP_FLAGS)  -R"file:$@" $^

bin_PROGRAMS = emacsy-webkit-gtk

AM_CFLAGS  = $(EMACSY_CFLAGS) $(GUILE_CFLAGS) $(WEBKIT_CFLAGS)
emacsy_webkit_gtk_LDADD   = $(EMACSY_LIBS) $(GUILE_LIBS) $(WEBKIT_LIBS)
emacsy_webkit_gtk_LDFLAGS = 
emacsy_webkit_gtk_SOURCES = main.c

main.o : main.c.x

MAINTAINERCLEANFILES = Makefile.in config.h.in 

#include $(top_srcdir)/support/automake/noweb.am
include $(top_srcdir)/support/automake/guile.am

run-local: emacsy-webkit-gtk
	$(ENV) ./emacsy-webkit-gtk
