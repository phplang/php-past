<?php

// ODBC Compatibility script for velocis
//
// Use require() to include this file into your scripts.  This allows you
// to write applications using uodbc_* functions, then include the apropriate
// file to map those functions to the database you use
//
//

function uodbc_setoption(){return velocis_setoption();}
function uodbc_autocommit($a,$b){return velocis_autocommit($a,$b);}
function uodbc_close($a){return velocis_close($a);}
function uodbc_close_all(){return velocis_close_all();}
function uodbc_commit($a){return velocis_commit($a);}
function uodbc_connect($a,$b,$c){return velocis_connect($a,$b,$c);}
function uodbc_pconnect($a,$b,$c){return velocis_pconnect($a,$b,$c);}
function uodbc_cursor($a){return velocis_cursor($a);}
function uodbc_do($a,$b){return velocis_do($a,$b);}
function uodbc_exec($a,$b){return velocis_exec($a,$b);}
function uodbc_prepare($a,$b){return velocis_prepare($a,$b);}
function uodbc_execute($a,$b){return velocis_execute($a,$b);}
function uodbc_fetch_row($a,$b){return velocis_fetch_row($a,$b);}
function uodbc_fetch_into($a,$b,&$c){return velocis_fetch_into($a,$b,&$c);}
function uodbc_field_len(){return velocis_field_len();}
function uodbc_field_name($a,$b){return velocis_field_name($a,$b);}
function uodbc_field_type($a,$b){return velocis_field_type($a,$b);}
function uodbc_field_num($a,$b){return velocis_field_num($a,$b);}
function uodbc_free_result($a){return velocis_free_result($a);}
function uodbc_num_fields($a){return velocis_num_fields($a);}
function uodbc_num_rows($a){return velocis_num_rows($a);}
function uodbc_result($a,$b){return velocis_result($a,$b);}
function uodbc_result_all($a,$b){return velocis_result_all($a,$b);}
function uodbc_rollback($a){return velocis_rollback($a);}
function uodbc_binmode($a,$b){return velocis_binmode($a,$b);}
function uodbc_longreadlen($a,$b){return velocis_longreadlen($a,$b);}
