
<title> Imap Example </title>
</head>
<body>
<?
    dl("/your/path/to/dl/imap.so");
    $mbox=imap_open("{Imap host}INBOX","?user?","?password?");
    $check=imap_check($mbox);
    echo  
    echo $check->Date,"<br>\n";
    echo "Connection Type: ",$check->Driver,"<br>\n";
    echo "Mbox: ",$check->Mailbox,"<br>\n";
    echo "Number Messages: ",$check->Nmsgs;
    echo " Recent: ",$check->Recent,"<br>\n";
    $num=imap_num_msg($mbox);
    for ( $i=1 ; $i <= $num ; $i++){
        echo"<hr>\n";
        $h=imap_header($mbox,$i);
        if ($h->Date[0] == "D") echo $h->Date,"<br>\n";
        if ($h->To[0] == "T") echo $h->To,"<br>\n";
        if ($h->From[0] == "F") echo $h->From,"<br>\n";
        if ($h->cc[0] == "C") echo $h->cc,"<br>\n";
        if ($h->ReplyTo[0] == "R") echo $h->ReplyTo,"<br>\n";
        if ($h->Subject) echo $h->Subject,"<br>\n";
        echo "<pre>\n",imap_body($mbox,$i),"\n</pre>";
    }
    imap_close($mbox);
?>
</body>

