<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY docbook.dsl SYSTEM "/usr/lib/sgml/docbook/html/docbook.dsl" CDATA DSSSL>
<!ENTITY html-common.dsl SYSTEM "html-common.dsl">
<!ENTITY common.dsl SYSTEM "common.dsl">
]>

<!--

  $Id: html.dsl,v 1.7 1998/10/23 11:36:33 ssb Exp $

  HTML-specific stylesheet customization.

-->

<style-specification id="docbook-php-html" use="docbook">
<style-specification-body>

(define %html-ext% ".html")

&html-common.dsl;
&common.dsl;

</style-specification-body>
</style-specification>

<external-specification id="docbook" document="docbook.dsl">
