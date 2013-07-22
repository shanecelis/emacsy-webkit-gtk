/*
 * Copyright (C) 2006, 2007 Apple Inc.
 * Copyright (C) 2007 Alp Toker <alp@atoker.com>
 * Copyright (C) 2011 Lukasz Slachciak
 * Copyright (C) 2011 Bob Murphy
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
#include <gdk/gdkkeysyms.h>
#include <webkit/webkit.h>
#include <JavaScriptCore/JavaScript.h>
#include <emacsy.h>
#endif
#include <libguile.h>

static void init_primitives(void);
static void destroyWindowCb(GtkWidget* widget, GtkWidget* window);
static gboolean closeWebViewCb(WebKitWebView* webView, GtkWidget* window);
static gboolean keyPressWindowCb(GtkWidget* widget, GdkEventKey* event, gpointer userData);
static gboolean idling(void *userData);

GtkWidget *label;
WebKitWebView *webView;

int main(int argc, char* argv[])
{
  int err = 0;
  scm_init_guile();
  err = emacsy_initialize();
  if (err) 
    return err;

  init_primitives();  

  scm_c_eval_string("(define-interactive (goto #:optional (url (read-from-minibuffer \"URL: \"))) (load-url url))");

  // Initialize GTK+
  gtk_init(&argc, &argv);


  // Create an 800x600 window that will contain the browser instance
  GtkWidget *main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(main_window), 800, 600);

  GdkColor black = {0, 0x0, 0x0, 0x0};
  GdkColor white = {0, 0xFFFF, 0xFFFF, 0xFFFF};
  /* you might need to use GTK_STATE_ACTIVE or GTK_STATE_PRELIGHT */
  gtk_widget_modify_bg(GTK_WINDOW(main_window), GTK_STATE_NORMAL, &black);
  gtk_widget_modify_fg(GTK_WINDOW(main_window), GTK_STATE_NORMAL, &white);

  // Create a browser instance
  webView = WEBKIT_WEB_VIEW(webkit_web_view_new());
  webkit_web_view_set_highlight_text_matches(webView, TRUE);

  // Create a scrollable area, and put the browser instance into it
  GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(webView));

  // Set up callbacks so that if either the main window or the browser instance is
  // closed, the program will exit
  g_signal_connect(main_window, "destroy", G_CALLBACK(destroyWindowCb), NULL);
  g_signal_connect(webView, "close-web-view", G_CALLBACK(closeWebViewCb), main_window);

  label = gtk_label_new("label");
  gtk_misc_set_alignment(GTK_MISC(label), 0.0f, 0.0f);
  gtk_label_set_use_underline(GTK_LABEL(label), FALSE);
  gtk_label_set_single_line_mode(GTK_LABEL(label), TRUE);

  g_idle_add((GSourceFunc) idling, NULL);

  g_signal_connect(main_window, "key_press_event", G_CALLBACK(keyPressWindowCb), NULL);
  g_signal_connect(main_window, "key_release_event", G_CALLBACK(keyPressWindowCb), NULL);
    
  GtkWidget *vbox;
  vbox = gtk_vbox_new(FALSE, 1);
  gtk_container_add(GTK_CONTAINER(vbox), scrolledWindow);
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

  // Put the scrollable area into the main window
  gtk_container_add(GTK_CONTAINER(main_window), /*scrolledWindow*/
                    vbox);

  // Load a web page into the browser instance
  //webkit_web_view_load_uri(webView, "http://www.webkitgtk.org/");
  webkit_web_view_load_uri(webView, "http://google.com/");
  //webkit_web_view_load_uri(webView, "http://slashdot.org");

  // Make sure that when the browser area becomes visible, it will get mouse
  // and keyboard events
  gtk_widget_grab_focus(GTK_WIDGET(webView));

  // Make sure the main window and all its contents are visible
  gtk_widget_show_all(main_window);

  // Run the main GTK+ event loop
  gtk_main();

  return 0;
}


static void destroyWindowCb(GtkWidget* widget, GtkWidget* window)
{
  gtk_main_quit();
}

static gboolean closeWebViewCb(WebKitWebView* webView, GtkWidget* window)
{
  gtk_widget_destroy(window);
  return TRUE;
}

static int scm_c_char_to_int(const char *char_name) {
  /* I should put a regex in here to validate it's a char */
  return scm_to_int(scm_char_to_integer(scm_c_eval_string(char_name)));
}

static gboolean keyPressWindowCb(GtkWidget* widget, GdkEventKey* event, gpointer userData)
{
  static guint32 last_unichar = 0;
  GdkModifierType modifiers;

  modifiers = gtk_accelerator_get_default_mod_mask();
  int mod_flags = 0;
  if (event->state & modifiers & GDK_CONTROL_MASK)
    mod_flags |= EY_MODKEY_CONTROL;

  if (event->state & modifiers & GDK_SHIFT_MASK) 
    mod_flags |= EY_MODKEY_SHIFT;

  if (event->state & modifiers & GDK_META_MASK)
    printf("META key is held.\n");

  if (event->state & modifiers & GDK_HYPER_MASK)
    printf("HYPER key is held.\n");

  if (event->state & modifiers & GDK_SUPER_MASK)
    printf("SUPER key is held.\n");

  if (event->state & modifiers & GDK_MOD1_MASK)
    mod_flags |= EY_MODKEY_META;

  guint32 unichar;
  unichar = gdk_keyval_to_unicode(event->keyval);
  if (event->type == GDK_KEY_PRESS) {
    printf("Key press %d %s (unicode %d last_unichar %d)\n", event->keyval, event->string, unichar, last_unichar);
    // Fix up some keys.
    if (event->keyval == GDK_KEY_BackSpace)
      unichar = scm_c_char_to_int("#\\del");
    if (unichar) {
      emacsy_key_event(unichar, mod_flags);
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
    printf("Key release %d %s (unicode %d last_unichar %d)\n", event->keyval, event->string, unichar, last_unichar);
    if (last_unichar && last_unichar == unichar) {
      last_unichar = 0;
      return TRUE;
    }
  }
  return FALSE;
}

static gboolean idling(void *userData)
{
  int flags = emacsy_tick();
  if (flags & EY_QUIT_APPLICATION)
    gtk_main_quit();

  const char *status = emacsy_message_or_echo_area();
  //printf("status: %s\n", status);
  char *markup = g_markup_printf_escaped ("<span foreground=\"white\" background=\"black\" underline=\"single\"><tt>%s </tt></span>", status);
  gtk_label_set_markup(GTK_LABEL(label), markup);
  g_free(markup);

  // Show the cursor.  Make it blink?
  char message[255];
  memset(message, ' ', 254);
  message[255] = NULL;
  message[emacsy_minibuffer_point() - 1] = '_';
  gtk_label_set_pattern(GTK_LABEL(label), message);
  return TRUE;
}

SCM_DEFINE(scm_load_url, "load-url", 1, 0, 0,
           (SCM scm_url),
           "Loads a given URL into the WebView.")
{
    const char *c_url = scm_to_locale_string(scm_url);
    webkit_web_view_load_uri(webView, c_url);
    return SCM_BOOL_F;
}

SCM_DEFINE(scm_webkit_forward, "webkit-forward", 0, 0, 0,
           (),
           "Move WebKit forward.")
{
  if (webkit_web_view_can_go_forward(webView)) {
    webkit_web_view_go_forward(webView);
    return SCM_BOOL_T;
  }
  return SCM_BOOL_F;
}

SCM_DEFINE(scm_webkit_backward, "webkit-backward", 0, 0, 0,
           (),
           "Move WebKit backward.")
{
  if (webkit_web_view_can_go_back(webView)) {
    webkit_web_view_go_back(webView);
    return SCM_BOOL_T;
  }
  return SCM_BOOL_F;
}

SCM_DEFINE(scm_webkit_reload, "webkit-reload", 0, 0, 0,
           (),
           "Reload.")
{
  webkit_web_view_reload(webView);
  return SCM_UNSPECIFIED;
}

/*
SCM_DEFINE(scm_webkit_find, "webkit-find", 1, 0, 0,
           (SCM text),
           "Find this text.")
{
  const char *c_text = scm_to_locale_string(text);
  WebKitFindController *find = webkit_web_view_get_find_controller(webView);
  webkit_find_controller_search(find, c_text, WEBKIT_FIND_OPTIONS_CASE_INSENSITIVE | WEBKIT_FIND_OPTIONS_WRAP_AROUND, 10);
  return SCM_UNSPECIFIED;
}
*/

SCM_DEFINE(scm_webkit_find_next, "webkit-find-next", 1, 0, 0,
           (SCM text),
           "Find next.")
{
  const char *c_text = scm_to_locale_string(text);
  return scm_from_bool(webkit_web_view_search_text(webView, c_text, FALSE, TRUE, TRUE)); 
  //return scm_from_bool(webkit_web_view_mark_text_matches(webView, c_text, FALSE, 0));
  
}

SCM_DEFINE(scm_webkit_find_previous, "webkit-find-previous", 1, 0, 0,
           (SCM text),
           "Find previous.")
{
  const char *c_text = scm_to_locale_string(text);
  webkit_web_view_mark_text_matches(webView, c_text, FALSE, 0);
  return scm_from_bool(webkit_web_view_search_text(webView, c_text, FALSE, FALSE, TRUE)); 
  /* WebKitFindController *find = webkit_web_view_get_find_controller(webView); */
  /* webkit_find_controller_search_previous(find); */
  /* return SCM_UNSPECIFIED; */
}

SCM_DEFINE(scm_webkit_find_finish, "webkit-find-finish", 0, 0, 0,
           (),
           "Find previous.")
{
  /* WebKitFindController *find = webkit_web_view_get_find_controller(webView); */
  /* webkit_find_controller_search_finish(find); */
  webkit_web_view_unmark_text_matches(webView);
  return SCM_UNSPECIFIED;
}

SCM_DEFINE(scm_webkit_zoom_in, "webkit-zoom-in", 0, 0, 0,
           (),
           "Zoom in.")
{
  webkit_web_view_zoom_in(webView);
  return SCM_UNSPECIFIED;
}

SCM_DEFINE(scm_webkit_zoom_out, "webkit-zoom-out", 0, 0, 0,
           (),
           "Zoom out.")
{
  webkit_web_view_zoom_out(webView);
  return SCM_UNSPECIFIED;
}



#if 0
static void
web_view_javascript_finished (GObject      *object,
                              GAsyncResult *result,
                              gpointer      user_data)
{
    WebKitJavascriptResult *js_result;
    JSValueRef              value;
    JSGlobalContextRef      context;
    GError                 *error = NULL;

    js_result = webkit_web_view_run_javascript_finish (WEBKIT_WEB_VIEW (object), result, &error);
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
  webkit_web_view_run_javascript(webView, c_script, NULL, web_view_javascript_finished, scm_is_true(proc) ? proc : NULL);
  return SCM_UNSPECIFIED;
}
#endif // 0

static void init_primitives(void)
{
#ifndef SCM_MAGIC_SNARFER
#include "main.c.x"
#endif
}

