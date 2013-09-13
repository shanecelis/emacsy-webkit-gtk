
/* main.c

   This example program demonstrates how to embed Emacsy into a
   minimal WebKit GTK browser with multiple buffers.
*/

/*
 * Copyright (C) 2006, 2007 Apple Inc.
 * Copyright (C) 2007 Alp Toker <alp@atoker.com>
 * Copyright (C) 2011 Lukasz Slachciak
 * Copyright (C) 2011 Bob Murphy
 * Copyright (C) 2013 Shane Celis
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef SCM_MAGIC_SNARFER
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <webkit/webkit.h>
#include <JavaScriptCore/JavaScript.h>
#include <emacsy.h>
#endif
#include <libguile.h>

/* Event Handlers */
static void destroy_window(GtkWidget* widget, GtkWidget* window);
static gboolean close_window(WebKitWebView* webView, GtkWidget* window);
static gboolean key_press(GtkWidget* widget, GdkEventKey* event, gpointer user_data);
static gboolean process_and_update_emacsy(void *user_data);

/* Registers the Scheme primitive procedures */
static void init_primitives(void); 

/* Scheme Primitives */
SCM scm_webkit_load_url(SCM url);
SCM scm_webkit_forward();
SCM scm_webkit_backward();
SCM scm_webkit_reload();
SCM scm_webkit_find_next(SCM text);
SCM scm_webkit_find_previous(SCM text);
SCM scm_webkit_find_finish();
SCM scm_webkit_zoom_in();
SCM scm_webkit_zoom_out();
SCM scm_current_web_view();
WebKitWebView *scm_c_current_web_view();
//SCM scm_webkit_eval_javascript(SCM script, SCM when_finished_proc);

/* Global state */
GtkWidget *label;               /* Shows Emacsy's echo area or minibuffer */
GtkWidget *modeline;            /* Shows Emacsy's modeline */
WebKitWebView *web_view;        /* The WebKit browser */
WebKitWebView *web_view_2;        /* The WebKit browser */
GtkWidget *scrolled_window;
/*
  Create a minimal web browser that has Emacsy integrated into it.
 */
int main(int argc, char* argv[])
{
  int err;
  // Initialize GNU Guile.
  scm_init_guile();
  // Initialize Emacsy.
  err = emacsy_initialize();
  if (err) 
    return err;

  // Register the primitive procedures that control the browser.
  init_primitives();  

  // You can evaluate S-expressions here.
  scm_c_eval_string("(use-modules (system repl error-handling))"
                    "(define (safe-load filename)              "
                    "  (call-with-error-handling               "
                    "    (lambda () (load filename))))         ");
  
  scm_c_eval_string("(use-modules (emacsy window))");

  // But to make the application easy to mold, it's best to load the
  // Scheme code from a file.
  const char *startup_script = "emacsy-webkit-gtk-w-windows.scm";
  if (access(startup_script, R_OK) != -1) {
    printf("Loading '%s'.\n", startup_script);

    // We could load the file like this:
    //scm_c_primitive_load(".emacy-webkit-gtk.scm");

    // But this will drop us into a REPL if anything goes wrong.
    scm_call_1(scm_c_private_ref("guile-user", "safe-load"),
               scm_from_locale_string(startup_script));
  } else {
    printf("Did not find '%s'.\n", startup_script);
  }

  // Initialize GTK+.
  gtk_init(&argc, &argv);

  // Create an 800x600 window that will contain the browser instance.
  GtkWidget *main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(main_window), 800, 600);
  //gtk_window_set_size(GTK_WINDOW(main_window), 800, 600);
  
  GdkGeometry geom_struct;
  geom_struct.max_width = 800;
  geom_struct.max_height = 600;
  gtk_window_set_geometry_hints(GTK_WINDOW(main_window),
                                NULL,
                                &geom_struct,
                                GDK_HINT_MAX_SIZE);
  /* you might need to use GTK_STATE_ACTIVE or GTK_STATE_PRELIGHT */
  GdkColor black = {0, 0x0, 0x0, 0x0};
  GdkColor white = {0, 0xFFFF, 0xFFFF, 0xFFFF};
  gtk_widget_modify_bg(GTK_WINDOW(main_window), GTK_STATE_NORMAL, &black);
  gtk_widget_modify_fg(GTK_WINDOW(main_window), GTK_STATE_NORMAL, &white);

  // Create a browser instance
  /* web_view = WEBKIT_WEB_VIEW(webkit_web_view_new()); */
  /* webkit_web_view_set_highlight_text_matches(web_view, TRUE); */
  web_view = NULL;

  // Create a scrollable area, and put the browser instance into it
  scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  GtkWidget *old_scrolled_window;

  old_scrolled_window = gtk_scrolled_window_new(NULL, NULL);

  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  // This is where the web_view is set.
  //scm_c_eval_string("(new-tab)");
  //web_view_2 = web_view;
  //scm_c_eval_string("(new-tab)");

  scrolled_window = gtk_scrolled_window_new(NULL, NULL);

  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(old_scrolled_window),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  //scm_c_eval_string("(new-tab)");

  //gtk_widget_reparent(GTK_WIDGET(web_view), GTK_CONTAINER(scrolled_window)); 
  //gtk_scrolled_window_add_with_viewport(GTK_CONTAINER(scrolled_window), GTK_WIDGET(web_view));
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(web_view));
  gtk_container_add(GTK_CONTAINER(old_scrolled_window), GTK_WIDGET(web_view_2));
  gtk_widget_show_all(GTK_WIDGET(web_view));
  gtk_widget_show_all(GTK_WIDGET(web_view_2));
  // Set up callbacks so that if either the main window or the browser
  // instance is closed, the program will exit.
  g_signal_connect(main_window, "destroy", G_CALLBACK(destroy_window), NULL);
  //g_signal_connect(web_view, "close-web-view", G_CALLBACK(close_window), main_window);


  // This label will be where we display Emacsy's echo-area.
  label = gtk_label_new("label");
  gtk_misc_set_alignment(GTK_MISC(label), 0.0f, 0.0f);
  gtk_label_set_use_underline(GTK_LABEL(label), FALSE);
  gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
  gtk_label_set_single_line_mode(GTK_LABEL(label), TRUE);
  gtk_label_set_max_width_chars(GTK_LABEL(label), 160);

  modeline = gtk_label_new("modeline");
  gtk_misc_set_alignment(GTK_MISC(modeline), 0.0f, 0.0f);
  gtk_label_set_use_underline(GTK_LABEL(modeline), FALSE);
  gtk_label_set_line_wrap(GTK_LABEL(modeline), TRUE);
  gtk_label_set_single_line_mode(GTK_LABEL(modeline), TRUE);
  gtk_label_set_max_width_chars(GTK_LABEL(modeline), 160);


  // While idle, process events in Emacsy and upate the echo-area.
  g_idle_add((GSourceFunc) process_and_update_emacsy, NULL);

  // Handle key press and release events.
  g_signal_connect(main_window, "key_press_event", G_CALLBACK(key_press), NULL);
  g_signal_connect(main_window, "key_release_event", G_CALLBACK(key_press), NULL);
    
  GtkWidget *vbox;
  vbox = gtk_vbox_new(FALSE, 1);
  /* gtk_container_add(GTK_CONTAINER(vbox), old_scrolled_window); */
  /* gtk_container_add(GTK_CONTAINER(vbox), scrolled_window); */

  /* gtk_box_pack_start(GTK_BOX(vbox), modeline, FALSE, FALSE, 0); */
  SCM widget_pointer = scm_call_0(scm_c_public_ref("guile-user", "instantiate-root-window"));
  gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(scm_to_pointer(widget_pointer)));

  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

  // Put the scrollable area into the main window.
  gtk_container_add(GTK_CONTAINER(main_window), vbox);


  // Load a web page into the browser instance.
  /* webkit_web_view_load_uri(web_view,  */
  /*                          "http://shanecelis.github.io/2013/06/15/the-garden/"); */

  // Make sure that when the browser area becomes visible, it will get mouse
  // and keyboard events.
  //gtk_widget_grab_focus(GTK_WIDGET(web_view));

  // Make sure the main window and all its contents are visible.
  gtk_widget_show_all(main_window);
  gtk_window_set_resizable(GTK_WINDOW(main_window), FALSE);

  // Run the main GTK+ event loop.
  gtk_main();

  return 0;
}

/*
  Event Handlers
  ==============
*/

static void destroy_window(GtkWidget* widget, GtkWidget* window)
{
  gtk_main_quit();
}

static gboolean close_window(WebKitWebView* web_view, GtkWidget* window)
{
  gtk_widget_destroy(window);
  return TRUE;
}

static int scm_c_char_to_int(const char *char_name) {
  /* I should put a regex in here to validate it's a char */
  return scm_to_int(scm_char_to_integer(scm_c_eval_string(char_name)));
}

static gboolean key_press(GtkWidget* widget, GdkEventKey* event, gpointer user_data)
{
  static guint32 last_unichar = 0;
  guint32 unichar;
  GdkModifierType modifiers;
  int mod_flags = 0;

  modifiers = gtk_accelerator_get_default_mod_mask();
  if (event->state & modifiers & GDK_CONTROL_MASK)
    mod_flags |= EY_MODKEY_CONTROL;

  if (event->state & modifiers & GDK_SHIFT_MASK) 
    mod_flags |= EY_MODKEY_SHIFT;

  if (event->state & modifiers & GDK_SUPER_MASK) 
    mod_flags |= EY_MODKEY_SUPER;

  if (event->state & modifiers & GDK_MOD1_MASK)
    mod_flags |= EY_MODKEY_META;

  unichar = gdk_keyval_to_unicode(event->keyval);

  // Fix up any key values that don't translate perfectly.
  if (event->keyval == GDK_KEY_BackSpace)
    unichar = scm_c_char_to_int("#\\del");

  // If unichar is 0 then it's not a regular key, e.g., Control, Meta, etc.

  if (event->type == GDK_KEY_PRESS) {
    printf("Key press %d %s (unicode %d last_unichar %d)\n", 
           event->keyval, event->string, unichar, last_unichar);
    // Fix up some keys.
    if (unichar) {
      // Register the key event with Emacsy.
      emacsy_key_event(unichar, mod_flags);
      /* 
         One can do the event handling and the actual processing
         separately in Emacsy.  However, in this case, it's convenient
         to do some processing in the event handling here so we know
         whether or not to pass the event on to the browser.
       */
      int flags = emacsy_tick();

      printf("flags = %d\n", flags);
      if (flags & EY_RAN_UNDEFINED_COMMAND) {
        printf("Passing to browser.\n");
        return FALSE; // Pass the event through to the web browser.
      } else {
        printf("Emacsy handled it.\n");
        last_unichar = unichar;
        return TRUE; // Emacsy handled it. Don't pass the event through.
      }
    }
  } else if (event->type == GDK_KEY_RELEASE) {
    /* 
       We receive both key presses and key releases.  If we decide not
       to pass a key event when pressed, then we remember it
       (last_unichar) such that we squelch the key release event too.
     */
    printf("Key release %d %s (unicode %d last_unichar %d)\n", 
           event->keyval, event->string, unichar, last_unichar);
    if (last_unichar && last_unichar == unichar) {
      last_unichar = 0;
      return TRUE; // Don't pass event to the browser.
    }
  }
  return FALSE; // Pass the event to the browser.
}

/*
  Process events in Emacsy then update the echo area at the bottom of the
  screen.
 */
static gboolean process_and_update_emacsy(void *user_data)
{
  // Process events and any background coroutines.
  int flags = emacsy_tick();

  // If there's been a request to quit, quit.
  if (flags & EY_QUIT_APPLICATION)
    gtk_main_quit();

  // Update the status line. 
  const char *modeline_string = emacsy_mode_line();
  const char *status = emacsy_message_or_echo_area();
  // Use markup to style the status line.
  char *markup = g_markup_printf_escaped ("<span foreground=\"white\" background=\"black\" underline=\"single\"><tt>%s </tt></span>", status);
  gtk_label_set_markup(GTK_LABEL(label), markup);
  g_free(markup);

  markup = g_markup_printf_escaped ("<span foreground=\"white\" background=\"black\" underline=\"none\"><tt>%s </tt></span>", modeline_string);
  gtk_label_set_markup(GTK_LABEL(modeline), markup);
  g_free(markup);

  // Show the cursor.  Exercise for the reader: Make it blink.
  char message[255];
  memset(message, ' ', 254);
  message[255] = NULL;
  message[emacsy_minibuffer_point() - 1] = '_';
  gtk_label_set_pattern(GTK_LABEL(label), message);

  return TRUE;                  
}

/*
  Scheme Primitives
  =================
  
  These C functions are exposed as callable procedures in Scheme.
*/

SCM_DEFINE(scm_set_web_view_x, "set-web-view!", 1, 0, 0,
           (SCM web_view_pointer),
           "Set the current web view to the given pointer.")
{
  if (scm_is_true(scm_pointer_p(web_view_pointer))) {
    // Remove the current one from the window.
    if (web_view)
      gtk_container_remove(GTK_CONTAINER(scrolled_window), GTK_WIDGET(web_view));
    web_view = WEBKIT_WEB_VIEW(scm_to_pointer(web_view_pointer));
    gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(web_view));
    gtk_widget_show_all(GTK_WIDGET(web_view));
    //gtk_widget_grab_focus(GTK_WIDGET(web_view));
  } else {
    fprintf(stderr, "error: not given a pointer in set-web-view!\n");
  }
  return SCM_UNSPECIFIED;
}

SCM_DEFINE(scm_make_web_view, "make-web-view", 0, 0, 0,
           (),
           "Creates and returns a pointer to a new webkit view.")
{
  WebKitWebView *a_web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
  a_web_view = g_object_ref(a_web_view);
  return scm_from_pointer(a_web_view, /*g_free*/ NULL);
}

SCM_DEFINE(scm_webkit_load_url, "webkit-load-url", 1, 0, 0,
           (SCM scm_url),
           "Loads a given URL into the WebView.")
{
    const char *c_url = scm_to_locale_string(scm_url);
    webkit_web_view_load_uri(scm_c_current_web_view(), c_url);
    return SCM_UNSPECIFIED;
}

SCM_DEFINE(scm_webkit_forward, "webkit-forward", 0, 0, 0,
           (),
           "Move browser forward.")
{
  if (webkit_web_view_can_go_forward(scm_c_current_web_view())) {
    webkit_web_view_go_forward(scm_c_current_web_view());
    return SCM_BOOL_T;
  }
  return SCM_BOOL_F;
}

SCM_DEFINE(scm_webkit_backward, "webkit-backward", 0, 0, 0,
           (),
           "Move browser backward.")
{
  if (webkit_web_view_can_go_back(scm_c_current_web_view())) {
    webkit_web_view_go_back(scm_c_current_web_view());
    return SCM_BOOL_T;
  }
  return SCM_BOOL_F;
}

SCM_DEFINE(scm_webkit_reload, "webkit-reload", 0, 0, 0,
           (),
           "Reload browser.")
{
  webkit_web_view_reload(scm_c_current_web_view());
  return SCM_UNSPECIFIED;
}

SCM_DEFINE(scm_webkit_find_next, "webkit-find-next", 1, 0, 0,
           (SCM text),
           "Find next.")
{
  const char *c_text = scm_to_locale_string(text);
  return scm_from_bool(webkit_web_view_search_text(scm_c_current_web_view(), c_text, FALSE, TRUE, TRUE)); 
  //return scm_from_bool(webkit_web_view_mark_text_matches(scm_c_current_web_view(), c_text, FALSE, 0));
}

SCM_DEFINE(scm_webkit_find_previous, "webkit-find-previous", 1, 0, 0,
           (SCM text),
           "Find previous.")
{
  const char *c_text = scm_to_locale_string(text);
  webkit_web_view_mark_text_matches(scm_c_current_web_view(), c_text, FALSE, 0);
  return scm_from_bool(webkit_web_view_search_text(scm_c_current_web_view(), c_text, FALSE, FALSE, TRUE)); 
}

SCM_DEFINE(scm_webkit_find_finish, "webkit-find-finish", 0, 0, 0,
           (),
           "Find previous.")
{
  webkit_web_view_unmark_text_matches(scm_c_current_web_view());
  return SCM_UNSPECIFIED;
}

SCM_DEFINE(scm_webkit_zoom_in, "webkit-zoom-in", 0, 0, 0,
           (),
           "Zoom in.")
{
  webkit_web_view_zoom_in(scm_c_current_web_view());
  return SCM_UNSPECIFIED;
}

SCM_DEFINE(scm_webkit_zoom_out, "webkit-zoom-out", 0, 0, 0,
           (),
           "Zoom out.")
{
  webkit_web_view_zoom_out(scm_c_current_web_view());
  return SCM_UNSPECIFIED;
}

SCM_DEFINE(scm_current_web_view, "webkit-zoom-out", 0, 0, 0,
           (),
           "Returns the web_view associated with the current buffer if it exists.")
{
  SCM scm_web_view = scm_call_1(scm_c_public_ref("emacsy emacsy", "local-var"),
                                scm_string_to_symbol(scm_from_locale_string("web-view")));
  return scm_web_view;
}

WebKitWebView *scm_c_current_web_view()
{
  SCM web_view = scm_current_web_view();
  if (! scm_is_false(web_view)) {
    return (WebKitWebView *) scm_to_pointer(web_view);
  }
  return SCM_BOOL_F;
}

SCM_DEFINE(scm_create_web_view_window, "create-web-view-window", 3, 0, 0,
           (SCM window, SCM buffer, SCM plain_text_p),
           "Returns a pointer to a GtkWidget* that contains a webkit in a scrolled window with a modeline.")
{
  GtkWidget *scrolled_window;
  GtkWidget *modeline;
  SCM scm_web_view = scm_make_web_view();
  WebKitWebView *web_view = (WebKitWebView *) scm_to_pointer(scm_web_view);
  printf("Calling create_web_view_window\n");
  if (scm_is_true(plain_text_p)) {
    printf("Loading string\n");
    webkit_web_view_load_string(web_view,
                                scm_to_locale_string(
                                  scm_call_1(scm_c_public_ref("emacsy emacsy",
                                                              "buffer-string"),
                                             buffer)),
                                "text/plain",
                                "utf-8",
                                "file://something");
  }

  scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(web_view));
  gtk_widget_show_all(GTK_WIDGET(scrolled_window));

  modeline = gtk_label_new("modeline");
  gtk_misc_set_alignment(GTK_MISC(modeline), 0.0f, 0.0f);
  gtk_label_set_use_underline(GTK_LABEL(modeline), FALSE);
  gtk_label_set_line_wrap(GTK_LABEL(modeline), TRUE);
  gtk_label_set_single_line_mode(GTK_LABEL(modeline), TRUE);
  gtk_label_set_max_width_chars(GTK_LABEL(modeline), 160);

  GtkWidget *vbox;
  vbox = gtk_vbox_new(FALSE, 1);
  gtk_container_add(GTK_CONTAINER(vbox), scrolled_window);
  gtk_box_pack_start(GTK_BOX(vbox), modeline, FALSE, FALSE, 0);
  SCM widget_pointer = scm_from_pointer(vbox, NULL);
  
  return widget_pointer;
}

SCM_DEFINE(scm_create_vertical_window, "create-vertical-window", 1, 0, 0,
           (SCM list),
           "Returns a pointer to a GtkWidget* that contains a vertical window with the list of other widgets as its children.")
{
  GtkWidget *vbox;
  vbox = gtk_vbox_new(FALSE, 1);

  for (; ! scm_null_p(list); list = scm_cdr(list)) {
    SCM pointer = scm_car(list);
    if (scm_is_true(scm_pointer_p(pointer))) {
      gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(scm_to_pointer(pointer)));
    } else {
      fprintf(stderr, "warning: got a non-pointer for create-vertical-window.");
    }
  }
  return scm_from_pointer(vbox, NULL);
}

/* 
   I was going to try and get fancy with some javascript evaluation,
   but I didn't succeed.
 */
/*
static void
web_view_javascript_finished (GObject      *object,
                              GAsyncResult *result,
                              gpointer      user_data)
{
    WebKitJavascriptResult *js_result;
    JSValueRef              value;
    JSGlobalContextRef      context;
    GError                 *error = NULL;

    js_result = webkit_web_view_run_javascript_finish (WEBKIT_WEB_VIEW (object), 
                                                       result, &error);
    if (!js_result) {
        g_warning ("Error running javascript: %s", error->message);
        g_error_free (error);
        return;
    }

    context = webkit_javascript_result_get_global_context (js_result);
    value = webkit_javascript_result_get_value (js_result);
    if (JSValueIsString (context, value)) {
        JSStringRef js_str_value;
        gchar      *str_value;
        gsize       str_length;

        js_str_value = JSValueToStringCopy (context, value, NULL);
        str_length = JSStringGetMaximumUTF8CStringSize (js_str_value);
        str_value = (gchar *)g_malloc (str_length);
        JSStringGetUTF8CString (js_str_value, str_value, str_length);
        JSStringRelease (js_str_value);
        if (user_data) {
          SCM proc = (SCM) user_data;
          scm_call_1(proc, scm_from_locale_string(str_value));
        }
        g_print ("Script result: %s\n", str_value);
        g_free (str_value);
    } else {
        g_warning ("Error running javascript: unexpected return value");
    }
    webkit_javascript_result_unref (js_result);
}

SCM_DEFINE(scm_webkit_eval_javascript, "webkit-eval-javascript", 2, 0, 0,
           (SCM script, SCM proc),
           "Reload.")
{
  const char *c_script = scm_to_locale_string(script);
  webkit_web_view_run_javascript(web_view, c_script, NULL, 
                                 web_view_javascript_finished, 
                                 scm_is_true(proc) ? proc : NULL);
  return SCM_UNSPECIFIED;
}
*/

static void init_primitives(void)
{
/*
  We use guile-snarf to generate main.c.x that helps us register the C
  functions as Scheme procedures.
*/
#ifndef SCM_MAGIC_SNARFER
#include "emacsy-webkit-gtk-w-windows.c.x"
#endif
}

