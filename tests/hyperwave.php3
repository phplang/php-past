<?
  /* Change the following variables if necessary. */
  $hwhost = "localhost";

  /* Lists an object array
  */
  function list_attr($attributes) {
    for($i=0; $i<count($attributes); $i++) {
      $key = key($attributes);
      switch($key) {
        case "Title":
        case "Description":
          $title_arr = $attributes[$key];
          $cj = count($title_arr);
          for($j=0; $j<$cj; $j++) {
            $tkey = key($title_arr);
            switch($tkey) {
              case "en":
                $language = "English";
                break;
              case "ge":
                $language = "German";
                break;
              default:
                $language = "---"; 
            }
            printf("  %s = %s: %s\n", $key, $language, $title_arr[$tkey]);

            next($title_arr);
          }
          break;
        default:
          printf("  %s = %s\n", $key, $attributes[$key]);
        }
      next($attributes);
      }
  }

  function list_children($children) {
    $c_children = count($children) - 1;
    for($i=0; $i<$c_children; $i++) {
      $arr = hw_objrec2array($children[$i]);
      list_attr($arr);
    }  
  }    

  echo "This test performs:\n";
  echo "1. Connecting to Hyperwave server\n";
  echo "2. output the object record of the Hyperroot\n";
  echo "3. output the children of the Hyperroot\n";
  echo "4. map rootcollection on remote server and list children\n";
  echo "\n";

  echo "Test 1: Connecting to Hyperwave server ($hwhost).\n";
  $connect = hw_connect($hwhost, 418);
  if($connect == FALSE) {
     echo "Test 1 failed!\n";
     echo "Could not connect to $hwhost!\n";
     exit;
  }
  echo hw_info($connect);
  echo "\n";
  echo "Test 1: successful\n";
  echo "\n";

  echo "Test 2: Object Record of Hyperroot.\n";
  $rootid = hw_root($connect);
  $rootobjrec = hw_getobject($connect, $rootid);
  $rootobjarr = hw_objrec2array($rootobjrec);
  list_attr($rootobjarr);
  echo "Test 2: successful\n";
  echo "\n";

  echo "Test 3: Children of Hyperroot.\n";
  $rootchildobjrecs = hw_childrenobj($connect, $rootid);
  list_children($rootchildobjrecs);
  echo "Test 3: successful\n";
  echo "\n";

  echo "Test 4: Map rootcollection on remote server and list children.\n";
  $rootid = hw_mapid($connect, hexdec("0x82d85c03"), hexdec("0x00000002"));
  $rootchildobjrecs = hw_childrenobj($connect, $rootid);
  list_children($rootchildobjrecs);
  echo "Test 4: successful\n";
  echo "\n";

?>
