;; .emacsy-webkit-gtk.scm
;;
;; Here's where the fun begins.

(define-interactive 
  (load-url #:optional 
        (url (read-from-minibuffer "URL: "))) 
  (webkit-load-url url))

;; Load-url is all right, but it requires an actual URL.
;; Let's fix that with a new command: GOTO.
(define-interactive 
  (goto #:optional
        (urlish (read-from-minibuffer "GOTO: ")))
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

;; Now let's bind these to some keys.

(define-key global-map (kbd "M-g") 'goto)
(define-key global-map (kbd "s-g") 'goto)
;; Let's use the super key to go forward and backward.
(define-key global-map (kbd "s-f") 'go-forward)
(define-key global-map (kbd "s-b") 'go-back)
(define-key global-map (kbd "C-s") 'search-forward)
(define-key global-map (kbd "C-r") 'search-backward)
