<?php

// ODBC Compatibility script for DB2
//
// Use require() to include this file into your scripts.  This allows you
// to write applications using uodbc_* functions, then include the apropriate
// file to map those functions to the database you use
//
//

function uodbc_setoption(){return db2_setoption();}
function uodbc_autocommit($a,$b){return db2_autocommit($a,$b);}
function uodbc_close($a){return db2_close($a);}
function uodbc_close_all(){return db2_close_all();}
function uodbc_commit($a){return db2_commit($a);}
function uodbc_connect($a,$b,$c){return db2_connect($a,$b,$c);}
function uodbc_pconnect($a,$b,$c){return db2_pconnect($a,$b,$c);}
function uodbc_cursor($a){return db2_cursor($a);}
function uodbc_do($a,$b){return db2_do($a,$b);}
function uodbc_exec($a,$b){return db2_exec($a,$b);}
function uodbc_prepare($a,$b){return db2_prepare($a,$b);}
function uodbc_execute($a,$b){return db2_execute($a,$b);}
function uodbc_fetch_row($a,$b){return db2_fetch_row($a,$b);}
function uodbc_fetch_into($a,$b,&$c){return db2_fetch_into($a,$b,&$c);}
function uodbc_field_len(){return db2_field_len();}
function uodbc_field_name($a,$b){return db2_field_name($a,$b);}
function uodbc_field_type($a,$b){return db2_field_type($a,$b);}
function uodbc_field_num($a,$b){return db2_field_num($a,$b);}
function uodbc_free_result($a){return db2_free_result($a);}
function uodbc_num_fields($a){return db2_num_fields($a);}
function uodbc_num_rows($a){return db2_num_rows($a);}
function uodbc_result($a,$b){return db2_result($a,$b);}
function uodbc_result_all($a,$b){return db2_result_all($a,$b);}
function uodbc_rollback($a){return db2_rollback($a);}
function uodbc_binmode($a,$b){return db2_binmode($a,$b);}
function uodbc_longreadlen($a,$b){return db2_longreadlen($a,$b);}
