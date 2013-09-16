;; .emacsy-webkit-gtk.scm
;;
;; Here's where the fun begins.

(use-modules (oop goops)
             (emacsy window)
             (srfi srfi-9) ;; record
             )

(format #t "current module ~a~%" (current-module))
(message "Here I am!")
(set! current-window (make <window> #:window-buffer messages))
(set! root-window (make <internal-window> #:window-children (list current-window)))

(define-interactive (new-tab)
  (define (on-enter)
    (when (local-var 'web-view)
      (format #t "Setting web-view to ~a~%" (local-var 'web-view))
      (set-web-view! (local-var 'web-view))))
  (let ((buffer (switch-to-buffer "*new-tab*")))
    (set! (local-var 'web-view) (make-web-view))
    (add-hook! (buffer-enter-hook buffer)
               on-enter)
    (on-enter)
    (load-url "http://google.com")))

(define-interactive 
  (load-url #:optional 
        (url (read-from-minibuffer "URL: "))) 
  (webkit-load-url url))

;; Load-url is all right, but it requires an actual URL.
;; Let's fix that with a new command: GOTO.
(define-interactive 
  (goto #:optional
        (urlish (read-from-minibuffer "GOTO: ")))
  (set-buffer-name! urlish)
  (cond
   ((string-prefix? "http://" urlish)
    (load-url urlish))
   ((string-contains urlish " ")
    ;; It contains spaces. It's probably a search.
    (load-url
     (format #f "http://www.google.com/search?q=~a"
             (string-map (lambda (c) (if (eq? c #\space) #\+ c)) urlish)))
    )
   (else
    ;; It's just one word.  Let's try adding a .com and http:// if it
    ;; needs it.
    (load-url (format #f "http://~a~a" urlish 
                      (if (string-suffix? ".com" urlish) "" ".com"))))))

(define-interactive (go-forward)
  (webkit-forward))

(define-interactive (go-back)
  (webkit-backward))

(define-interactive (reload)
  (webkit-reload))

(define-interactive (reload-script)
  (load ".emacsy-webkit-gtk.scm"))

(define find-text #f)

;; These aren't as good as Emacs' isearch-forward, but they're not
;; a bad start.
(define-interactive 
  (search-forward #:optional
                   (text (or find-text (read-from-minibuffer "Search: "))))
  (set! find-text text)
  (webkit-find-next text))

(define-interactive 
  (search-backward #:optional
                  (text (or find-text (read-from-minibuffer "Search: "))))
  (set! find-text text)
  (webkit-find-previous text))


(define-record-type <window-user-data>
  (make-window-user-data widget web-view modeline)
  window-user-data?
  (widget wud-widget)
  (web-view wud-web-view)
  (modeline wud-modeline))

;; For some reason, I can't refer to record-types in C without first
;; referring to them in Scheme. The record accesses are inlined
;; apparently.  Seems like a Guile bug to me.
(define make-window-user-data2 make-window-user-data)
(define wud-widget2 wud-widget)
(define wud-modeline2 wud-modeline)

(define-method (redisplay (window <window>))
  (let* ((buffer (window-buffer window))
         (userdata (user-data window)))
    (when (and buffer (window-user-data? userdata))
      ;(format #t "redisplaying window ~a with buffer ~a~%" window buffer)
      (web-view-load-string (wud-web-view userdata)
                            (buffer-string buffer))
      (update-label! (wud-modeline userdata)
                     (emacsy-mode-line buffer)))))

(define-method (redisplay (window <internal-window>))
  (for-each redisplay (window-children window)))

(define (redisplay-windows)
  (redisplay root-window)
  #;(for-each redisplay
            (window-list)))

(define (instantiate-root-window)
  ;(set! root-window (make <window> #:window-buffer messages))
  ;(instantiate-window root-window)
  (instantiate-window current-window)
  )

(define-method (instantiate-window (window <window>))
  (let ((buffer (window-buffer window)))
    (create-web-view-window window buffer (is-a? buffer <text-buffer>))))

(define-method (instantiate-window (window <internal-window>))
  (create-vertical-window (map instantiate-window (window-children window)))
  #;(if (eq? (orientation window) 'vertical)
      (create-vertical-window (map instantiate-window (window-children window)))
      (create-horizontal-window (map instantiate-window (window-children window)))))

;; Now let's bind these to some keys.

(define-key global-map (kbd "M-g") 'goto)
(define-key global-map (kbd "s-g") 'goto)
;; Let's use the super key to go forward and backward.
(define-key global-map (kbd "s-f") 'go-forward)
(define-key global-map (kbd "s-b") 'go-back)
(define-key global-map (kbd "C-s") 'search-forward)
(define-key global-map (kbd "C-r") 'search-backward)

(export instantiate-window instantiate-root-window <window-user-data> make-window-user-data2 wud-widget2 wud-web-view wud-modeline window-user-data? redisplay redisplay-windows)
