<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY docbook.dsl SYSTEM "/usr/lib/sgml/docbook/html/docbook.dsl" CDATA DSSSL>
<!ENTITY html-common.dsl SYSTEM "html-common.dsl">
<!ENTITY common.dsl SYSTEM "common.dsl">
]>

<!--

  $Id: html.dsl,v 1.6 1998/03/11 03:02:59 ssb Exp $

  HTML-specific stylesheet customization.

-->

<style-specification id="docbook-php-html" use="docbook">

(define %html-ext% ".html")

&html-common.dsl;
&common.dsl;

</style-specification>

<external-specification id="docbook" document="docbook.dsl">
