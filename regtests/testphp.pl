#!/bin/perl -w
$vcid='$Id: testphp.pl,v 1.4 1998/08/23 12:05:14 rocky Exp $';
sub usage {
  my($terse)=@_;
  print "
  usage: $program [OPTIONS] server *php-test-control-file*

  Used in regression testing to run a php3 program and compare its output 
  against previously saved expected results.
";
  if (!$terse) { 
    print " 
  A URL is constructed and requested. For example it might be something
  like  myserver.org:${serverport}/regtests/basic/001.phtml.

  The output is compared against the results found in php-control-file.

  The format of php-test-control-file is a sequence of sections: 
  a TITLE section, a URL section, an optional POST section, an
  optional HEAD section and an EXPECT section. 

  The start of each section is indicated in the 
  file by surrounding it in dashes. Here is a template: 

  -- A comment can go after any line starting with: --
  -- But should not be in the EXPECT or HEAD sections
  --TITLE--
  Description of test
  [line2]...
  --URL--
  test-program[?args1=value[&arg2=value...]
  [--POST--
  [post argument line 1]
  [post argument line 2...] 
  [--HEAD--
  pattern,...]
  --EXPECT--
  [expect output]


  The TITLE section can contain a number of lines and merely describes the 
  name of the test. It is not used in validation, but is manditory.
  It is printed before the test is run. It can be
  several lines long.

  The URL section gives the name of the test to run, and is manditory
  unless the command-line option --url is specified. If there are any
  GET parameters to pass along, it would be done here. 
 
  The servername, port and an optional prefix are all prepended 
  to the URL. Only the first line after --URL-- is used. Subsequent
  lines are tacitly ignored. 

  The POST section is optional, but if given, a POST method will be used.
  It is followed by a number of lines, possibly empty which contain
  the POST arguments. 

  If there is no POST section, only a GET method will be used.

  Note that both the POST section and the parameter part of the URL section
  need to have their data in url-encoded format.  

  The HEAD section is optional too, but if given the header from the
  HTTP server will be checked againt the perl pattern that is in 
  this section.

  The EXPECT section contains the desired output to check against. The
  section is manditory, although there my be no lines (no output).

A simple example which takes one argument through the POST method
and one through the GET and displays these would be:

Here is a complete test file which uses the GET method but sends 
no data.

  --TITLE--
  Trivial 'Hello World' test                                    ...
  --URL--
  basic/001.phtml
  --EXPECT--
  Hello World

=============
";
  }
  print "
  OPTIONS:
    --help              display detailed help
    --version           print version string and exit
    --quiet             don't print the Description seciton.
    --diff *prog*       specify diff program and options for use in 
                        comparison. Default is: $diff
    --port *n*          specify port number to connect to in HTTP server
                        default is: $serverport
    --prefix *str*      Specify URL prefix. default is: $prefix
    --url *str*         Specify the URL suffix. Note that you don't 
                        put a hostname and port in here or http://,
                        as these are added automatically. If --prefix
                        is given, it to is prepended.
                         
";
  if ($terse) {
    print "
For a detailed description of the test-control file format, type:
$program --help
"
  }

 exit 100;
}
 
init();
process_options();
read_test_info($php_testfile);
run_test($serverhost, $serverport, $url);
$rc=compare_results($http_out, $expect_out, $http_header);
exit $rc;

sub init {
    ($program = $0) =~ s,.*/,,;   # Who am I today, anyway? 
    $expect_out  = 'expect.out';
    $http_out = 'http.out';

    $serverport= 80;
    $prefix='';
    $diff='diff -c -w';
    $debug=0;
    $quiet=0;
}    

# Grok program options -- the bane of programming.
sub process_options {
    use Getopt::Long;
    $Getopt::Long::autoabbrev = 1;
    $result = &GetOptions
	(
	 'help'           => \$help,
	 'version'        => \$show_version,
	 'debug'          => \$debug, 
	 'quiet'          => \$quiet, 
	 'diff=s'         => \$diff, 
	 'port=i'         => \$serverport, 
	 'prefix=s'       => \$prefix, 
	 'url=s'          => \$url, 
	 );
    show_version() if $show_version;
    usage(0) if $help || !$result;
    if (@ARGV!=2) {
	print STDERR "Expecting 2 arguments: server & php-test-program name\n";
	usage(1);
    }
    $serverhost  =$ARGV[0];
    $php_testfile=$ARGV[1];
}

# Reads the PHP test file spec, and creates the expected output file
# to compare results against. Save the output in string $Expect too
# in case we want to use an internally-defined "diff".
sub read_test_info {
    my($test_control_file) = @_;

    my($desc);
    # Name of section we are processing:  TITLE, EXPECT, POST, URL, etc.
    my($in_section)='';
    undef($Post);      # String part of the POST   section
    undef($Expect);    # String part of the EXPECT section
    undef($Head_pat);  # String part of the HEAD section


    # get rid of temp files left laying around
    unlink $expect_out, $http_out;

    open(EXPECT_OUT, ">$expect_out") 
	|| die "Cannot create file $expect_out for writing: $!";
    open(CONTROL_FILE, "<$test_control_file") 
	|| die "Can't open $test_control_file for reading: $!";

    my($lineno);
    for ($lineno=1; <CONTROL_FILE>; $lineno++) {
      # See if we've hit a new section header.
      if (/^--TITLE--/i) {
	$in_section='title';
	  next;
      } elsif (/^--URL--/i) {
	$in_section='url';
	  next;
      } elsif (/^--POST--/i) {
	$in_section='post';
	  $Post = '';
	next;
      } elsif (/^--HEAD--/i) {
	$in_section='head';
	  next;
      } elsif (/^--EXPECT--/i) {
	$in_section='expect';
	  $Expect='';
	next;
      } elsif (/^--/) {
	# Comment.
	next if $in_section !~ /(head|expect)/;
      }
      
      # Above we handled switching between sections. We get here only
      # if in the middle of some section. Process the line of that section.
      if ($in_section eq 'expect') {
	$Expect .= $_;
      } elsif ($in_section eq 'url') {
	chomp;
	# --url command-line option takes precidence over section 
	# Also we take only the first line. Subsequent lines are ignored
	# silently. 
	# Perhaps we should give a warning?
	$url = "${prefix}$_" if !defined($url);
      } elsif ($in_section eq 'title') {
	$desc .= $_;
      } elsif ($in_section eq 'post') {
	chomp;
	$Post .= $_;
      } elsif ($in_section eq 'head') {
	chomp;
	$Head_pat .= $_;
      } elsif ($in_section eq '') {
	print STDERR "$program: line $lineno not in any section--ignored.\n"
	  if !$quiet;
      } else {
	myerr("$program internal error in value \$in_section; is $in_section",
              11);
      }
    }
    
    if ($quiet) { 
      printf "%s: ", $test_control_file;
    } else {
      $desc = "No description supplied.\n" if !defined($desc);
      printf "%s\n%s:\n%s", '-' x 50, $test_control_file, $desc 
    }
    
    # Now do some basic error checking of the test file.
    if (!$url) {
      myerr("URL section not found and command option override not given", 
	    20);
    } elsif (!defined($Expect)) {
      myerr("Didn't see --EXPECT--", 30);
    }
    print EXPECT_OUT $Expect ;
    close(EXPECT_OUT); 
    close(CONTROL_FILE);
    
}

# Connects to HTTP server which should have the PHP3 test program
# and fetches the output ignoring the header.
sub run_test {
    my($serverhost, $serverport, $url) = @_;
    
    use IO::Socket;
    $remote = IO::Socket::INET->new (Proto => "tcp",
				     PeerAddr => $serverhost,
				     PeerPort => $serverport,
				    );
    
    unless ($remote) {
	die "cannot connect to http daemon on ${serverhost}:$serverport" 
	}
    $remote->autoflush(1);

    # send the request to the server to retrieve the document
    if ($Post) {
	printf $remote "POST $url HTTP/1.0
Content-type: application/x-www-form-urlencoded
Content-length: %d

$Post", length($Post);
    } else {
	print $remote "GET $url HTTP/1.0\n\n";
    }
     
    open(HTTP_OUT, ">$http_out") 
	|| die "Cannot create file $http_out for writing: $!";

    # Skip until after the HTTP header. It should be after \n\n or \r\n\r\n
    # If we have a http header pattern, save the header.
    # The below may not be quite perfect in finding the end of header.
    while ( defined($line=<$remote>) && ($line !~ /^\s*$/) ) { 
      $http_header .= $line if defined($Head_pat);
    }

    while ( defined($line=<$remote>) ) {  
        next if $line =~ /^\s*$/;
	# print File $line;
	print HTTP_OUT $line;
    }
    close ($remote);
    close(HTTP_OUT);

}

# 
# Compare what we got back from PHP on the server with 
# what we're supposed to see. 
# Right now this uses Unix diff, perhaps we'll expand to remove
# this program dependency and add pattern matching.
sub compare_results {
    my($http_file, $expect_file, $http_header)=@_;

    if (defined($Head_pat)) {
      if (!compare_with_pattern($http_header, $Head_pat)) {
	print "Header match failed:\n$Head_pat\n---\n$http_header";
	return 1;
      }
    }
    my(@output)=`$diff $http_file $expect_file 2>&1`;
    my($ret)   = $? >> 8;
    
    if ( $ret )  {
	print " FAILED\n";
	print @output;
    } else {
	print " passed\n";
    }
    return $ret;
}

# Print version and exit. 
sub show_version {
    print "$program: $vcid\n";
    exit 10;
}

sub myerr {
    my($msg, $rc) = @_;
    print STDERR "$msg\n" if !$quiet;
    print " FAILED\n";
    exit $rc;
}

# Compare output string with a perl Pattern and return 1 if it matches,
# or 0 if not.
sub compare_with_pattern {
  my($http_output, $expect_pat) = @_;
  return $http_output =~ /$expect_pat/;
}
