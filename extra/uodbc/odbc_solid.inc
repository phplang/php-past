<?php

// ODBC Compatibility script for solid
//
// Use require() to include this file into your scripts.  This allows you
// to write applications using uodbc_* functions, then include the apropriate
// file to map those functions to the database you use
//
//

function uodbc_setoption(){return solid_setoption();}
function uodbc_autocommit($a,$b){return solid_autocommit($a,$b);}
function uodbc_close($a){return solid_close($a);}
function uodbc_close_all(){return solid_close_all();}
function uodbc_commit($a){return solid_commit($a);}
function uodbc_connect($a,$b,$c){return solid_connect($a,$b,$c);}
function uodbc_pconnect($a,$b,$c){return solid_pconnect($a,$b,$c);}
function uodbc_cursor($a){return solid_cursor($a);}
function uodbc_do($a,$b){return solid_do($a,$b);}
function uodbc_exec($a,$b){return solid_exec($a,$b);}
function uodbc_prepare($a,$b){return solid_prepare($a,$b);}
function uodbc_execute($a,$b){return solid_execute($a,$b);}
function uodbc_fetch_row($a,$b){return solid_fetch_row($a,$b);}
function uodbc_fetch_into($a,$b,&$c){return solid_fetch_into($a,$b,&$c);}
function uodbc_field_len(){return solid_field_len();}
function uodbc_field_name($a,$b){return solid_field_name($a,$b);}
function uodbc_field_type($a,$b){return solid_field_type($a,$b);}
function uodbc_field_num($a,$b){return solid_field_num($a,$b);}
function uodbc_free_result($a){return solid_free_result($a);}
function uodbc_num_fields($a){return solid_num_fields($a);}
function uodbc_num_rows($a){return solid_num_rows($a);}
function uodbc_result($a,$b){return solid_result($a,$b);}
function uodbc_result_all($a,$b){return solid_result_all($a,$b);}
function uodbc_rollback($a){return solid_rollback($a);}
function uodbc_binmode($a,$b){return solid_binmode($a,$b);}
function uodbc_longreadlen($a,$b){return solid_longreadlen($a,$b);}
