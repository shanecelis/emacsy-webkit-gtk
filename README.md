Emacsy WebKit GTK Browser Example
=================================

This program contains an example program for
[Emacsy](https://github.com/shanecelis/emacsy).  It is a barebones
browser with an echo-area/minibuffer any Emacsian will love.  

Running
-------

I recommend grabbing the [latest Emacsy
release](https://github.com/shanecelis/emacsy/releases) which has this
project included as one of its demos.

Demos
-----

This example includes three different demos that showcase the three
levels of integration with Emacsy.

1. `emacsy-webkit-gtk` is the easiest level of integration.  It allows
for one to have Emacs-like commands available, but there is no support
for separate buffers besides the minibuffer.  This can still be very
useful for applications which aren't based on having multiple
documents open.

2. `emacsy-webkit-gtk-w-buffers` is the middle level of integration.
It does everything demo 1 does and it allows one to switch between
multiple buffers, but there is only one fixed window that displays one
buffer at a time.

3. `emacsy-webkit-gtk-w-windows` is the highest level of integration.
It does everything demo 1 and 2 does, and it allows one to manage an
Emacs-like tiling window system.  This demo is the trickiest to
integrate because the application must deal with changing window
configurations.  GTK provides a number of useful layout primitives to
make this task much easier.  

Screenshot
----------

### Demo 1

![Obligatory screenshot](https://raw.github.com/shanecelis/emacsy-webkit-gtk/master/support/image/emacsy-webkit-gtk-screenshot-1.png)

### Demo 2

![Obligatory screenshot](https://raw.github.com/shanecelis/emacsy-webkit-gtk/master/support/image/emacsy-webkit-gtk-w-windows-screenshot-1.png)

