<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY docbook.dsl SYSTEM "/usr/lib/sgml/docbook/html/docbook.dsl" CDATA DSSSL>
<!ENTITY common.dsl SYSTEM "common.dsl">
]>

<!--

  $Id: print.dsl,v 1.3 1998/03/11 03:13:57 ssb Exp $

  This file contains printout-specific stylesheet customization.

-->

<style-specification id="local-docbook" use="docbook">

(element FUNCTION
  (cond
   ;; function names should be plain in FUNCDEF
   ((equal? (gi (parent)) "FUNCDEF")
    (process-children))
   
   ;; else make the function name bold and add "()"
   ;; we should add some cross-reference here later
   (else
    ($bold-seq$
     (make sequence
	   (process-children)
	   (literal "()"))))))

&common.dsl;

</style-specification>

<external-specification id="docbook" document="docbook.dsl">
