<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY docbook.dsl SYSTEM "/usr/lib/sgml/docbook/html/docbook.dsl" CDATA DSSSL>
<!ENTITY common.dsl SYSTEM "common.dsl">
]>

<!--

  $Id: phpweb.dsl,v 1.3 1998/01/26 07:14:46 ssb Exp $

  This file contains HTML-specific stylesheet customization
  for use on the network of PHP3 web sites.

-->

<style-specification id="local-docbook" use="docbook">

(declare-flow-object-class processing-instruction
  "UNREGISTERED::James Clark//Flow Object Class::processing-instruction")

(define %html-ext% ".php3")
(define %gentext-nav-tblwidth% "100%")
(define %gentext-nav-use-ff% #f)
(define %legalnotice-link-file% (string-append "copyright" %html-ext%))
(define %generate-legalnotice-link% #t)

;; Returns the depth of the auto-generated TOC (table of contents) that
;; should be made at the nd-level
(define (toc-depth nd)
  (if (string=? (gi nd) "BOOK")
      2 ; the depth of the top-level TOC
      1 ; the depth of all other TOCs
      ))

(define (phpweb-include-header-text title)
  (make processing-instruction
	data: (string-append " chdir(\"..\"); "
			     "$title = \"" title "\"; "
			     "$head_closed = 1; "
			     "include(\"include/header.inc\"); "
			     "?")))

(define (phpweb-include-header titlenode)
  (phpweb-include-header-text (data titlenode)))

(define (phpweb-include-footer)
  (make processing-instruction
	data: " include(\"include/footer.inc\"); ?"))

(element BOOK 
  (let* ((btitle (select-elements (children (current-node)) "TITLE"))
	 (binfo  (select-elements (children (current-node)) "BOOKINFO"))
	 (title  (if (node-list-empty? btitle)
		     (select-elements (children binfo) "TITLE")
		     btitle)))
    (if (chunk?)
	(make entity
	  system-id: (html-file)
	  (make element gi: "HTML"
	    (make element gi: "HEAD"
	      (make element gi: "TITLE"
		    (with-mode title-mode
			       (process-node-list (node-list-first title))))
	      ($html-header-meta$))
	    (phpweb-include-header (node-list-first title))
	    (header-navigation (current-node))
	    ($book-body$)
	    (footer-navigation (current-node))
	    (phpweb-include-footer)))
	($book-body$))))

(element PART
  (let* ((title (select-elements (children (current-node)) "TITLE")))
    (if (chunk?)
	(make entity
	  system-id: (html-file)
	  (make element gi: "HTML"
	    (make element gi: "HEAD"
	      (make element gi: "TITLE"
		    (with-mode title-mode
			       (process-node-list (node-list-first title))))
	      ($html-header-meta$))
	    (phpweb-include-header (node-list-first title))
	    (header-navigation (current-node))
	    ($part-body$)
	    (footer-navigation (current-node))
	    (phpweb-include-footer)))
	($part-body$))))

(define ($component$)
  (if (chunk?)
      (make entity
	 system-id: (html-file)
	 (let* ((title (select-elements (children (current-node)) "TITLE")))
	   (make element gi: "HTML"
	     (make element gi: "HEAD"
	       (make element gi: "TITLE"
		     (with-mode title-mode
				(process-node-list (node-list-first title))))
	       ($html-header-meta$))
	     (phpweb-include-header title)
	     (header-navigation (current-node))
	     ($component-body$)
	     (footer-navigation (current-node))
	     (phpweb-include-footer))))
      ($component-body$)))

(element GLOSSARY
  (if (chunk?)
      (make entity
	 system-id: (html-file)
	 (let* ((title (select-elements (children (current-node)) "TITLE")))
	   (make element gi: "HTML"
	     (make element gi: "HEAD"
	       (make element gi: "TITLE"
		     (with-mode title-mode
				(process-node-list (node-list-first title))))
	       ($html-header-meta$))
	     (phpweb-include-header (node-list-first title))
	     (header-navigation (current-node))
	     ($glossary-body$)
	     (footer-navigation (current-node))
	     (phpweb-include-footer))))
      ($glossary-body$)))

(element REFENTRY
  (if (chunk?) 
      (make entity
	 system-id: (html-file)
	 (let* ((title (select-elements (children (current-node)) "REFNAME")))
	   (make element gi: "HTML"
	     (make element gi: "HEAD"
	       (make element gi: "TITLE"
		     (with-mode title-mode
				(process-node-list (node-list-first title))))
	       ($html-header-meta$))
	     (phpweb-include-header (node-list-first title))
	     (header-navigation (current-node))
	     ($refentry-body$)
	     (footer-navigation (current-node))
	     (phpweb-include-footer))))
      ($refentry-body$)))

(define ($section$)
  (if (chunk?)
      (make entity
	 system-id: (html-file)
	 (let* ((title (select-elements (children (current-node)) "TITLE")))
	   (make element gi: "HTML"
	     (make element gi: "HEAD"
	       (make element gi: "TITLE"
		     (with-mode title-mode
				(process-node-list (node-list-first title))))
	       ($html-header-meta$))
	     (phpweb-include-header title)
	     (header-navigation (current-node))
	     ($section-body$)
	     (footer-navigation (current-node))
	     (phpweb-include-footer))))
      ($section-body$)))

(mode titlepage-mode
  (element LEGALNOTICE 
    (let ((notices (select-elements 
		     (descendants (parent (current-node)))
		     "LEGALNOTICE")))
      (if %generate-legalnotice-link%
	   ;; Divert the contents of legal to another file.  It will be xref'd
	   ;; from the Copyright.
	   (if (first-sibling? (current-node))
	       (make entity
		 system-id: %legalnotice-link-file%
		 (make element gi: "HTML"
		   (make element gi: "HEAD"
			 ($html-header-meta$))
		   (phpweb-include-header-text "Copyright")
		   (header-navigation (current-node))
		   ($semiformal-object$)
		   (with-mode legal-notice-link-mode
			      (process-node-list (node-list-rest notices)))
		   (footer-navigation (current-node))
		   (phpweb-include-footer)))
	       (empty-sosofo))
	   (make element gi: "DIV"
		 attributes: '(("CLASS" "LEGALNOTICE"))
		 ($semiformal-object$))))))

&common.dsl;

</style-specification>

<external-specification id="docbook" document="docbook.dsl">
