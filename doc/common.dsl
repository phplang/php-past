;; -*- Scheme -*-
;;
;; $Id: common.dsl,v 1.6 1998/04/27 12:35:49 ssb Exp $
;;
;; This file contains stylesheet customization common to the HTML
;; and print versions.
;;

(define %use-id-as-filename% #t)
(define %gentext-nav-tblwidth% "100%")
(define %refentry-function% #t)
(define %refentry-generate-name% #f)
(define %funcsynopsis-style% 'ansi)
(define %legalnotice-link-file% (string-append "copyright" %html-ext%))
(define %generate-legalnotice-link% #t)
(define newline "\U-000D")

(define (php3-code code)
  (make processing-instruction
    data: (string-append "php " code "?")))
