;; .emacsy-webkit-gtk.scm
;;
;; 

(define-interactive (goto #:optional 
                          (url (read-from-minibuffer \"URL: \"))) 
  (load-url url))
