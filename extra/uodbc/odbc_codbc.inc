<?php

// ODBC Compatibility script for codbc
//
// Use require() to include this file into your scripts.  This allows you
// to write applications using uodbc_* functions, then include the apropriate
// file to map those functions to the database you use
//
//

function uodbc_setoption(){return codbc_setoption();}
function uodbc_autocommit($a,$b){return codbc_autocommit($a,$b);}
function uodbc_close($a){return codbc_close($a);}
function uodbc_close_all(){return codbc_close_all();}
function uodbc_commit($a){return codbc_commit($a);}
function uodbc_connect($a,$b,$c){return codbc_connect($a,$b,$c);}
function uodbc_pconnect($a,$b,$c){return codbc_pconnect($a,$b,$c);}
function uodbc_cursor($a){return codbc_cursor($a);}
function uodbc_do($a,$b){return codbc_do($a,$b);}
function uodbc_exec($a,$b){return codbc_exec($a,$b);}
function uodbc_prepare($a,$b){return codbc_prepare($a,$b);}
function uodbc_execute($a,$b){return codbc_execute($a,$b);}
function uodbc_fetch_row($a,$b){return codbc_fetch_row($a,$b);}
function uodbc_fetch_into($a,$b,&$c){return codbc_fetch_into($a,$b,&$c);}
function uodbc_field_len(){return codbc_field_len();}
function uodbc_field_name($a,$b){return codbc_field_name($a,$b);}
function uodbc_field_type($a,$b){return codbc_field_type($a,$b);}
function uodbc_field_num($a,$b){return codbc_field_num($a,$b);}
function uodbc_free_result($a){return codbc_free_result($a);}
function uodbc_num_fields($a){return codbc_num_fields($a);}
function uodbc_num_rows($a){return codbc_num_rows($a);}
function uodbc_result($a,$b){return codbc_result($a,$b);}
function uodbc_result_all($a,$b){return codbc_result_all($a,$b);}
function uodbc_rollback($a){return codbc_rollback($a);}
function uodbc_binmode($a,$b){return codbc_binmode($a,$b);}
function uodbc_longreadlen($a,$b){return codbc_longreadlen($a,$b);}
