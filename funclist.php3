#! ./php -q
<?

  /* To be started in your /php3 source direcory,
     using a CGI PHP interpreter within this directory.
     
     max_execution_time on my system (P1, 133 MHz): 300 Sec.
   
   */

  function scanforfunction($file) {
    global $func, $module;
    
    $size   = filesize($file);
    $tstart = time();
    print "processing $file ($size bytes): "; flush();

    $cfile = basename($file);
    $line = implode("", @file($file));

    
    print "names "; flush();
    # Scan for "function_entry" block
    if (preg_match("!function_entry\s+(\w+)_functions\[\]\s*=\s*\{(.*)\};!sUS", $line, $m)) :
      
      # save module name
      $module[$m[1]] = $m[1];
      $cmodule       = $m[1];
      
      # split function block into function names
      # Regexp ist an Alternative (alternative1|alternative2)
      # Alternative 1 matches '{ "func_name", php3_func_name,'
      # Alternative 2 matches 'PHP_FE(func_name,'
      if (preg_match_all(
        "!(\{\s*\"(\w+)\"\s*,\s*(\w+)\s*,|PHP_FE\(\s*(\w+)\s*,)!sUS",
        $m[2], 
        $m)) :
          $fnames = $m[2];
          $cnames = $m[3];
          $xnames = $m[4];

          reset($xnames);
          while(list($k, $v) = each($xnames)) :
            if ($v) :
              $fnames[] = $v;
              $cnames[] = "php3_$v";
            endif;
          endwhile;
          
          reset($fnames);
          while (list($k, $v) = each($fnames)) :
            if ($v) :
              $func[$fnames[$k]]["name"]  = $fnames[$k];
              $func[$fnames[$k]]["cfunc"] = $cnames[$k];
              $func[$fnames[$k]]["module"]= $cmodule;
              $func[$fnames[$k]]["file"]  = $cfile;
            endif;
          endwhile;
      endif;
    endif;

    print "descriptions "; flush();
    # Scan for '/* {{{ proto ... */ ... }}} */' blocks
    if (preg_match_all(
      "!/\*\s*\{\{\{(.*)\*/.*\}\}\}\s*\*/!sUS",
      $line,
      $m,
      PREG_SET_ORDER)) :
        reset($m);
        while(list($k, $v) = each($m)) :
          preg_match("!proto\s+(\w+)\s+(\w+)\((.*)\).*\n(.*)$!sUS", $v[1], $n);
          $func[$n[2]]["return"]    = $n[1];
          $func[$n[2]]["parameter"] = $n[3];
          $func[$n[2]]["desc"]      = $n[4];
        endwhile;
    endif;

    $tstop = time();
    $tuse  = $tstop - $tstart;
    $tfunc = count($func);
    $tmod  = count($module);
    print "($tuse sec, $tfunc functions in $tmod modules).\n"; flush();
  }

  function scandir($dirname = ".") {
    $dp = opendir($dirname);
    if (!$dp)
      die("Cannot open $dirname");
    
    while($name = readdir($dp)) {
      # skip . and ..
      if ("." == $name or ".." == $name)
        continue;
        
      # recurse subdirectories
      if ("dir" == filetype("$dirname/$name"))
        scandir("$dirname/$name");
        
      # skip all non-.c files
      if (!preg_match("/\.c$/", $name))
        continue;
      
      # scan the file for functions
      scanforfunction("$dirname/$name");
    }
    closedir($dp);
  }

scandir();

reset($func);
while(list($k, $v) = each($func)) :
  printf("Function %s (Module %s, %s:%s()):\n", 
    $k, $v["module"], $v["file"], $v["cfunc"]);
  printf("%s %s(%s)\n", $v["return"], $k, $v["parameter"]);
  printf("%s\n\n", $v["desc"]);
endwhile;
exit;
?>
