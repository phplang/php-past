<?php

// ODBC Compatibility script for odbc
//
// Use require() to include this file into your scripts.  This allows you
// to write applications using uodbc_* functions, then include the apropriate
// file to map those functions to the database you use
//
//

function uodbc_setoption(){return odbc_setoption();}
function uodbc_autocommit($a,$b){return odbc_autocommit($a,$b);}
function uodbc_close($a){return odbc_close($a);}
function uodbc_close_all(){return odbc_close_all();}
function uodbc_commit($a){return odbc_commit($a);}
function uodbc_connect($a,$b,$c){return odbc_connect($a,$b,$c);}
function uodbc_pconnect($a,$b,$c){return odbc_pconnect($a,$b,$c);}
function uodbc_cursor($a){return odbc_cursor($a);}
function uodbc_do($a,$b){return odbc_do($a,$b);}
function uodbc_exec($a,$b){return odbc_exec($a,$b);}
function uodbc_prepare($a,$b){return odbc_prepare($a,$b);}
function uodbc_execute($a,$b){return odbc_execute($a,$b);}
function uodbc_fetch_row($a,$b){return odbc_fetch_row($a,$b);}
function uodbc_fetch_into($a,$b,&$c){return odbc_fetch_into($a,$b,&$c);}
function uodbc_field_len(){return odbc_field_len();}
function uodbc_field_name($a,$b){return odbc_field_name($a,$b);}
function uodbc_field_type($a,$b){return odbc_field_type($a,$b);}
function uodbc_field_num($a,$b){return odbc_field_num($a,$b);}
function uodbc_free_result($a){return odbc_free_result($a);}
function uodbc_num_fields($a){return odbc_num_fields($a);}
function uodbc_num_rows($a){return odbc_num_rows($a);}
function uodbc_result($a,$b){return odbc_result($a,$b);}
function uodbc_result_all($a,$b){return odbc_result_all($a,$b);}
function uodbc_rollback($a){return odbc_rollback($a);}
function uodbc_binmode($a,$b){return odbc_binmode($a,$b);}
function uodbc_longreadlen($a,$b){return odbc_longreadlen($a,$b);}
