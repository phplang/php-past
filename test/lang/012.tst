--TEST--
Testing stack after early function return                     ...
--POST--
--GET--
--FILE--
<?
Function F ( 
	if(1);
		return("Hello");
	endif;
);

$i=0;
while($i<2);
	echo F();
	$i++;
endwhile;
>
--EXPECT--
Content-type: text/html
HelloHello
