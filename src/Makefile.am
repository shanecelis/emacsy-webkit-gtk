.SECONDARY:

BUILT_SOURCES =
CLEANFILES=hello-emacsy.c.x 

WARN_NOTANGLE = $(top_builddir)/bin/warn-notangle
dist-hook:
	cp $(distdir)/*.pdf $(top_distdir)

NOWEB_DOCS = hello-emacsy-paper.pdf
NOWEB_FILES = hello-emacsy.nw hello-emacsy-paper.nw
NOWEB_PRODUCTS = hello-emacsy.c .hello-emacsy.scm
EXTRA_DIST = $(NOWEB_DOCS)

doc-local : 	hello-emacsy-paper.pdf


hello-emacsy-paper.pdf : hello-emacsy.tex
# hello-emacsy.c.x
hello-emacsy.c.x : hello-emacsy.c
hello-emacsy.o : hello-emacsy.c.x

.hello-emacsy.scm : hello-emacsy.nw
	$(AM_V_GEN) $(WARN_NOTANGLE) -o $@ $(WARN_NOTANGLE_FLAGS) -- $(NOTANGLE_LISP_FLAGS)  -R"file:$@" $^

bin_PROGRAMS = hello-emacsy

AM_CFLAGS  = $(EMACSY_CFLAGS) $(GUILE_CFLAGS)
hello_emacsy_LDADD   = $(EMACSY_LIBS) $(GUILE_LIBS) $(GLUT_LIBS)
hello_emacsy_LDFLAGS = $(GUILE_LDFLAGS) $(GLUT_LDFLAGS) $(GL_LDFLAGS) $(GLU_LDFLAGS)
hello_emacsy_SOURCES = hello-emacsy.c

MAINTAINERCLEANFILES = Makefile.in config.h.in 

include $(top_srcdir)/support/automake/noweb.am
include $(top_srcdir)/support/automake/guile.am

run-local: hello-emacsy
	$(ENV) ./hello-emacsy
