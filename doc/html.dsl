<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY docbook.dsl SYSTEM "/usr/lib/sgml/docbook/html/docbook.dsl" CDATA DSSSL>
<!ENTITY common.dsl SYSTEM "common.dsl">
]>

<!--

  $Id: html.dsl,v 1.3 1998/01/26 04:04:35 ssb Exp $

  This file contains HTML-specific stylesheet customization.

-->

<style-specification id="local-docbook" use="docbook">

(define %gentext-nav-tblwidth% "100%")

(define %legalnotice-link-file% (string-append "copyright" %html-ext%))
(define %generate-legalnotice-link% #t)

;; Returns the depth of the auto-generated TOC (table of contents) that
;; should be made at the nd-level
(define (toc-depth nd)
  (if (string=? (gi nd) "BOOK")
      2 ; the depth of the top-level TOC
      1 ; the depth of all other TOCs
      ))

&common.dsl;

</style-specification>

<external-specification id="docbook" document="docbook.dsl">
