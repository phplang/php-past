<?php

// ODBC Compatibility script for opnlnk
//
// Use require() to include this file into your scripts.  This allows you
// to write applications using uodbc_* functions, then include the apropriate
// file to map those functions to the database you use
//
//

function uodbc_setoption(){return opnlnk_setoption();}
function uodbc_autocommit($a,$b){return opnlnk_autocommit($a,$b);}
function uodbc_close($a){return opnlnk_close($a);}
function uodbc_close_all(){return opnlnk_close_all();}
function uodbc_commit($a){return opnlnk_commit($a);}
function uodbc_connect($a,$b,$c){return opnlnk_connect($a,$b,$c);}
function uodbc_pconnect($a,$b,$c){return opnlnk_pconnect($a,$b,$c);}
function uodbc_cursor($a){return opnlnk_cursor($a);}
function uodbc_do($a,$b){return opnlnk_do($a,$b);}
function uodbc_exec($a,$b){return opnlnk_exec($a,$b);}
function uodbc_prepare($a,$b){return opnlnk_prepare($a,$b);}
function uodbc_execute($a,$b){return opnlnk_execute($a,$b);}
function uodbc_fetch_row($a,$b){return opnlnk_fetch_row($a,$b);}
function uodbc_fetch_into($a,$b,&$c){return opnlnk_fetch_into($a,$b,&$c);}
function uodbc_field_len(){return opnlnk_field_len();}
function uodbc_field_name($a,$b){return opnlnk_field_name($a,$b);}
function uodbc_field_type($a,$b){return opnlnk_field_type($a,$b);}
function uodbc_field_num($a,$b){return opnlnk_field_num($a,$b);}
function uodbc_free_result($a){return opnlnk_free_result($a);}
function uodbc_num_fields($a){return opnlnk_num_fields($a);}
function uodbc_num_rows($a){return opnlnk_num_rows($a);}
function uodbc_result($a,$b){return opnlnk_result($a,$b);}
function uodbc_result_all($a,$b){return opnlnk_result_all($a,$b);}
function uodbc_rollback($a){return opnlnk_rollback($a);}
function uodbc_binmode($a,$b){return opnlnk_binmode($a,$b);}
function uodbc_longreadlen($a,$b){return opnlnk_longreadlen($a,$b);}
