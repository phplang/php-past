;; -*- Scheme -*-
;;
;; $Id: common.dsl,v 1.4 1998/02/06 11:48:31 ssb Exp $
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
