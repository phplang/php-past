<?php

// ODBC Compatibility script for iodbc
//
// Use require() to include this file into your scripts.  This allows you
// to write applications using uodbc_* functions, then include the apropriate
// file to map those functions to the database you use
//
//

function uodbc_setoption(){return iodbc_setoption();}
function uodbc_autocommit($a,$b){return iodbc_autocommit($a,$b);}
function uodbc_close($a){return iodbc_close($a);}
function uodbc_close_all(){return iodbc_close_all();}
function uodbc_commit($a){return iodbc_commit($a);}
function uodbc_connect($a,$b,$c){return iodbc_connect($a,$b,$c);}
function uodbc_pconnect($a,$b,$c){return iodbc_pconnect($a,$b,$c);}
function uodbc_cursor($a){return iodbc_cursor($a);}
function uodbc_do($a,$b){return iodbc_do($a,$b);}
function uodbc_exec($a,$b){return iodbc_exec($a,$b);}
function uodbc_prepare($a,$b){return iodbc_prepare($a,$b);}
function uodbc_execute($a,$b){return iodbc_execute($a,$b);}
function uodbc_fetch_row($a,$b){return iodbc_fetch_row($a,$b);}
function uodbc_fetch_into($a,$b,&$c){return iodbc_fetch_into($a,$b,&$c);}
function uodbc_field_len(){return iodbc_field_len();}
function uodbc_field_name($a,$b){return iodbc_field_name($a,$b);}
function uodbc_field_type($a,$b){return iodbc_field_type($a,$b);}
function uodbc_field_num($a,$b){return iodbc_field_num($a,$b);}
function uodbc_free_result($a){return iodbc_free_result($a);}
function uodbc_num_fields($a){return iodbc_num_fields($a);}
function uodbc_num_rows($a){return iodbc_num_rows($a);}
function uodbc_result($a,$b){return iodbc_result($a,$b);}
function uodbc_result_all($a,$b){return iodbc_result_all($a,$b);}
function uodbc_rollback($a){return iodbc_rollback($a);}
function uodbc_binmode($a,$b){return iodbc_binmode($a,$b);}
function uodbc_longreadlen($a,$b){return iodbc_longreadlen($a,$b);}
