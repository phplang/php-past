/*
 * ROOTDIR
 *
 * Set this to the full path of your HTML base directory for your documents.
 * If left undefined, it will be set to your home directory as specified in
 * /etc/passwd with /public_html appended.  If your /etc/passwd file does not
 * contain your home directory, or if your base html document directory is
 * not ~/public_html, you need to define this.  Note that this is not the
 * system-wide html root directory, only your personal root directory.
 */
/* #define ROOTDIR "/usr/somewhere/user_id/public_html" */

/*
 * HTML_DIR
 *
 * If you do not use ~/public_html as the HTML directory on your system, 
 * you can set this to the actual name.  It defaults to "public_html", but
 * if set to "html" for example, then the system will look for html files
 * in ~/html, or if ROOTDIR is defined in ROOTDIR/html.  If this is confusing,
 * see common.c for the conditions.
 * 
 * Generally you should not define both ROOTDIR and HTML_DIR.   If you define
 * ROOTDIR, then give the full path to your html directory in the ROOTDIR
 * variable.  If you do not define ROOTDIR, then your home directory will
 * be read from the /etc/passwd file and if HTML_DIR is defined, HTML_DIR
 * will be appended to this directory.  By default "/public_html" is
 * appended if HTML_DIR is not defined.
 */
/* #define HTML_DIR "html" */

/*
 * LOGDIR
 * This determines where your log files will be kept.  It is relative to
 * your base directory (usually ~/public_html) and is by default set to
 * "logs/".  If you don't want your log files to be placed in
 * ~/public_html/logs then define this:
 */
#define LOGDIR "logs/"

/*
 * ACCDIR
 * This determines where access restriction definition files are stored.
 */
#define ACCDIR "logs/"

/*
 * NOACCESS
 * This is the name of the HTML file to display when access is denied
 */
#define NOACCESS "NoAccess.html"
