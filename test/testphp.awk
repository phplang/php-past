BEGIN		{
				file=0;
				expect=0;
				get=0;
				post=0;
			}
/--TEST--/	{
				file=0;
				expect=0;
				get=0;
				post=0;
				testcase=1;
				getline;
			}
/--POST--/	{
				file=0;
				expect=0;
				get=0;
				post=1;
				testcase=0;
				getline;
			}
/--GET--/	{
				get=1;
				post=0;
				file=0;
				expect=0;
				testcase=0;
				getline;
			}
/--FILE--/	{
				file=1;
				expect=0;
				post=0;
				get=0;
				testcase=0;
				getline;
			}
/--EXPECT--/{
				expect=1;
				post=0;
				get=0;
				file=0;
				testcase=0;
				getline;
			}
			{
				if(file) {
					if(file==1) {
						file=2;
						print $0 >"phpfi.in";
					} else {
						print $0 >>"phpfi.in";
					}
				} else if(expect) {
					if(expect==1) {
						expect=2;
						print $0 >"phpfi.out";
					} else {
						print $0 >>"phpfi.out";
					}
				} else if(testcase) {
					if(expect==1) {
						expect=2;
						printf $0 >"phpfi.desc";
					} else {
						printf $0 >>"phpfi.desc";
					}
				} else if(post) {
					post=0;
					Post=$0;
				} else if(get) {
					get=0;
					Get=$0;
				}
			}
END			{
				p = "REQUEST_METHOD=POST CONTENT_TYPE=application/x-www-form-urlencoded CONTENT_LENGTH";
				if(length(Post) && length(Get)) {
					cmd = sprintf("echo %s | (%s=%d QUERY_STRING=%s ../src/php.cgi ./phpfi.in) >phpfi.test", Post,p,length(Post),Get);
				} else if(length(Post) && !length(Get)) {
					cmd = sprintf("echo %s | (%s=%d ../src/php.cgi ./phpfi.in) >phpfi.test",Post,p,length(Post));
				} else if(!length(Post) && length(Get)) {
					cmd = sprintf("(QUERY_STRING=%s ../src/php.cgi ./phpfi.in) >phpfi.test",Get);
				} else {
					cmd = "../src/php.cgi ./phpfi.in >phpfi.test";
				}
				print cmd;
			}
