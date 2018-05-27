#!/usr/bin/perl -wT
#
# URLblocked.cgi - explain to the user that the URL is blocked and by which rule set.
#
# Currently the error messages supports 
# en (English), 
# de (German), 
# pl (Polish)
# sv (Swedisk)
# it (Italian)
# pt (Portuguese)
# fr (French)
# tr (Turkish)
# nl (Dutch).
# You can add a language yourself: search for all occurences of "NEWLANGUAGE"
# and add your language text.

use strict;

use Socket;

# This CGI script uses fastcgi and also requires Fcgid configured in Apache.
# Comment out the next line and the line with "while (new CGI::Fast)" (approximately at line 423)
# to revert this script to a regular CGI script.
### use CGI::Fast;

use constant {
   CT_IMAGE  => 1,
   CT_JAVA   => 2,
   CT_HTML   => 3,
   CT_XML    => 4,
   CT_CSS    => 5,
   CT_TEXT   => 6,
   CT_JSON   => 7,
   CT_STREAM => 8,
   CT_204    => 9
};

use vars qw( $admin $clientaddr $clientname $clientuser $clientgroup $category $targetgroup );
use vars qw( $color $size $mode $textcolor $bgcolor $titlesize $textsize $httpcode $url $origurl );
use vars qw( $ufdbhost $ufdbscripturi $ufdbredirscripturi $ufdbsni $ufdbservername $ufdbrequesturi $ufdbrefurl );
use vars qw( $escaped_ufdbrequesturi $escaped_url );
use vars qw( @day @month @languages $lang $protocol $address $port $path );

local $admin;
local $clientaddr;
local $clientname;
local $clientuser;
local $clientgroup;
local $targetgroup;
local $color;
local $size;
local $mode;
local $textcolor;
local $bgcolor;
local $titlesize;
local $textsize;
local $httpcode;
local $url;
local $origurl;
local $ufdbhost;
local $ufdbscripturi;
local $ufdbredirscripturi;
local $ufdbsni;
local $ufdbservername;
local $ufdbrequesturi;
local $ufdbrefurl;
local $lang;
local $protocol;
local $address;
local $port;
local $path;
local @day = ("Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday");
local @month = ("Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec");
local @languages = (
		"de (German),",
		"nl (Dutch),",
		"pl (Polish),",
		"sv (Swedish),",
		"es (Spanish),",
		"it (Italian),",
		"pt (Portuguese),",
		"fr (French),",
		"tr (Turkish),",
		"NEW (NEWLANGUAGE),",
		"en (English),",
	       );

my $html_comment = "<!-- 
generated by URLblocked.cgi :\n
this is a very long comment to signal
to MSIE and other browsers not to display their own 'user-friendly'
error message, but to display the one that is produced by this
program.\n
The rest of this message is bogus to make it longer 
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  -->\n";

sub init();
sub session_reinit();
sub getPreferedLanguage(@);
sub parseURL($);
sub parseQuery($);


sub session_reinit ()
{
   $admin = 'unknown';
   $clientaddr = 'unknown';
   $clientname = 'unknown';
   $clientuser = 'unknown';
   $clientgroup = 'unknown';
   $targetgroup = 'unknown';
   $color = 'orange';
   $size = 'normal';
   $mode = 'default';
   $httpcode = '200';
   $url = 'unknown';
   $origurl = 'unknown';
   $lang = 'unknown';
   $ufdbrequesturi = '';
}


sub init () 
{
   $lang = getPreferedLanguage( @languages );
   ( $httpcode, $admin, $clientaddr, $clientname, $clientuser, $clientgroup, $category, $url )  =
      parseQuery( $ENV{"QUERY_STRING"} );
}


#
# Find the first supported language of the client.
#
sub getPreferedLanguage (@) 
{
  my @supported = @_;
  my @clientLanguages = split(/\s*,\s*/,$ENV{"HTTP_ACCEPT_LANGUAGE"}) if(defined($ENV{"HTTP_ACCEPT_LANGUAGE"}));
  my $lang;
  my $supp;

  ### NO!  push(@clientLanguages,$supported[0]);
  for $lang (@clientLanguages) 
  {
    $lang =~ s/\s.*//;
    $lang =~ s/-.*//;
    for $supp (@supported) 
    {
      $supp =~ s/\s.*//;
      return($lang) if ($lang eq $supp);
    }
  }
  return 'en';   # default language is 'en'
}


sub parseQuery ($) 
{
  my $query       = shift;
  my $admin       = 'The system administrator.';
  my $clientaddr  = '';
  my $clientname  = '';
  my $clientuser  = '';
  my $clientgroup = '';
  my $category    = '';
  my $httpcode    = '200';
  my $url         = 'undefined';
  my $therequest  = '';
  my $dummy;

  $ufdbservername = '';
  $ufdbhost = '';
  $ufdbrequesturi = '';

  if (defined($query))
  {
    while ($query =~ /^\&?([^\&=]+)=\"([^\"]*)\"(.*)/  || 
           $query =~ /^\&?([^\&=]+)=([^\&=]*)(.*)/)
    {
      my $key = $1;
      my $value = $2;
      $value = '??' unless(defined($value) && $value && $value ne '??');
      $query = $3;

      if ($key =~ /^(admin|clientaddr|clientname|clientuser|clientident|clientgroup|category|targetgroup|color|size|source|srcclass|targetclass|mode|httpcode|ufdbhost|ufdbscripturi|ufdbredirscripturi|ufdbsni|ufdbservername|ufdbrequesturi|ufdbrefurl|url)$/) 
      {
	$escaped_ufdbrequesturi = $value  if ($key eq 'ufdbrequesturi');
	$escaped_url = $value             if ($key eq 'url');

	$value =~ s/%20/ /g;
	$value =~ s/%22/"/g;
	$value =~ s/%23/#/g;
	$value =~ s/%24/\$/g;
	$value =~ s/%26/\&/g;
	$value =~ s/%27/\?/g;
	$value =~ s/%2B/\+/ig;
        $value =~ s/%2C/,/ig;
	$value =~ s/%2F/\//ig;
	$value =~ s/%3A/:/ig;
	$value =~ s/%3B/;/ig;
	$value =~ s/%3C/</ig;
	$value =~ s/%3D/=/ig;
	$value =~ s/%3E/>/ig;
	$value =~ s/%3F/?/ig;
	$value =~ s/%40/\@/ig;
	$value =~ s/%5C/\\/ig;
	$value =~ s/%25/%/g;            # must be last
	$key = 'clientgroup' if ($key eq 'source'  ||  $key eq 'srcclass');
	$key = 'clientuser'  if ($key eq 'clientident');
	$key = 'category'    if ($key eq 'targetgroup'  ||  $key eq 'targetclass');
	eval "\$$key = \$value";
      }

      if ($query =~ /^url=(.*)/) 
      {
	$url = $1;
	last;
      }
    }
  }

  $dummy = $ENV{'HTTP_X_BLOCKED_URL'};
  $url = $dummy  if defined $dummy;

  $dummy = $ENV{'HTTP_X_BLOCKED_CATEGORY'};
  $targetgroup = $dummy  if defined $dummy;

  if ($url eq 'undefined')
  {
     if ($ufdbservername ne '')
     {
        $url = 'https://' . $ufdbservername;
     }
     elsif ($ufdbhost ne '')
     {
        $url = 'https://' . $ufdbhost;
     }

     if ($url ne 'undefined'  &&  $ufdbrequesturi ne '')
     {
	$ufdbrequesturi =~ s/\?.*//;
        $url .= $ufdbrequesturi;
     }
  }

  $origurl = $url;
  $url = substr( $url, 0, 120 ) . '...'  if (length($url) > 120);

  $ufdbrefurl = $ENV{'HTTP_REFERER'};

  if (1)
  {
    my $dbgf = '/tmp/debug-cgibin-cgi';     # '/local/websites/logs/cgidebug';
    open DEBUG, ">> $dbgf";

    print DEBUG "\n\nurl = $url\n";
    print DEBUG "lang = $lang\n";
    foreach my $key ('HTTP_HOST', 'HTTP_REFERER', 'HTTP_USER_AGENT', 'REMOTE_ADDR', 'REQUEST_METHOD', 
		     'HTTP_ACCEPT_CHARSET', 'HTTP_ACCEPT_LANGUAGE', 'HTTP_CONTENT_TYPE', 'QUERY_STRING') {
       my $value = $ENV{$key};
       $value = 'undefined'  if !defined($value);
       print DEBUG "$key = $value\n";
    }

    if (0) 
    {
       print DEBUG ":\n";
       foreach my $key (sort(keys(%ENV))) {
	  print DEBUG "$key = $ENV{$key}\n";
       }
       close DEBUG;
    }
  }

  return ( $httpcode, $admin, $clientaddr, $clientname, $clientuser, $clientgroup, $category, $url );
}


sub parseURL ($) 
{
  my $url      = shift;
  my $protocol = "";
  my $address  = "";
  my $port     = "";
  my $path     = "";

  $url =~ /^([^\/:]+):\/\/([^\/:]+)(:\d*)?(.*)/;
  $protocol = $1 if(defined($1));
  $address  = $2 if(defined($2));
  $port     = $3 if(defined($3));
  $path     = $4 if(defined($4));

  return ( $protocol, $address, $port, $path );
}


sub getContentType( $ )
{
   my $url = shift;
   my $suffix;
   my $path;

   $url =~ s/[;\?\&].*//;
   $url =~ s/^(ftp|http|https):\/\///;

   $path = $url;
   $path =~ s/^[^\/]*//;

   $suffix = $path;
   $suffix =~ s/.*\././;

   return CT_IMAGE  if ($suffix =~ /\.(bmp|gif|ico|jpg|jpeg|jpe|png|webp|tiff)$/i);
   return CT_CSS    if ($suffix =~ /\.css$/i);
   return CT_JSON   if ($suffix =~ /\.json$/i);
   return CT_JAVA   if ($suffix =~ /\.(js|jar)$/i);
   return CT_TEXT   if ($suffix =~ /\.(csv|txt)$/i);
   return CT_HTML   if ($suffix =~ /\.(htm|html|dhtml|shtml)$/i);
   return CT_XML    if ($suffix =~ /\.(xml|sxml|rss)$/i);
   return CT_STREAM if ($suffix =~ /\.(bin|bz2|cab|class|dat|doc|gz|h264|mp3|mpg|mpeg|msi|mst|ppt|pdf|rar|tar|ttf|xls|zip|ogv|divx|xvid|qt|ra|ram|rv|wmv|avi|mov|swf|mp4|mv4|flv)$/i);

   # no suffix found, now we start with the guesswork


   return CT_HTML   if ($path eq '/' || $path eq '');
   return CT_IMAGE  if ($url =~ /^googleadservices\.com\/pagead\/conversion\// );
   return CT_TEXT   if ($url =~ /^googleads\.g\.doubleclick\.net\/pagead\/ads\// );
   return CT_JAVA   if ($url =~ /^pubads\.g\.doubleclick\.net\/pagead\/ads\// );
   return CT_JAVA   if ($url eq 'a.analytics.yahoo.com/fpc.pl' );
   return CT_IMAGE  if ($url eq 'a.analytics.yahoo.com/p.pl' );
   return CT_IMAGE  if ($url eq 'ping.chartbeat.net/ping' );

   if ($url =~ '^www\.youtube\.com')
   {
      return CT_STREAM  if ($path =~ /^\/cp\//  ||  $path =~ /^\/p\//  ||
                            $path =~ /^\/v\//   ||  $path =~ /^\/videoplayback/ );
   }

   return CT_204    if ($url =~ /^s\.youtube\.com\/api\/stats\//);

   return CT_IMAGE  if ($url =~ /^b\.scorecardresearch\.com\// );

   return CT_JAVA   if ($url =~ /\.doubleclick\.net\/adj\//   ||
                        $url =~ /\.doubleclick\.net\/pfadj\// );
   return CT_IMAGE  if ($url =~ /\.doubleclick\.net\/imp/ );

   return CT_IMAGE  if ($url =~ /\.tradedoubler\.com\/imp/ );

   if ($url =~ /^view\.atdmt\.com\//)
   {
      return CT_IMAGE  if ($path =~ /^\/action\// );
      return CT_JAVA   if ($path =~ /^\/jview\// );
   }

   return CT_JAVA   if ($url eq 'static.ak.connect.facebook.com/connect.php' );

   return CT_IMAGE  if ($url eq 'secure-us.imrworldwide.com/cgi-bin/m' );

   return CT_IMAGE  if ($url =~ /ftjcfx\.com\/image-/ );
   return CT_IMAGE  if ($url =~ /lduhtrp\.net\/image-/ );
   return CT_IMAGE  if ($url =~ /img\.pheedo\.com\/img\.phdo/ );
      
   if ($path =~ /\/realmedia\/ads\//i )
   {
      return CT_JAVA   if ($path =~ /\/adstream_jx/  ||  $path =~ /\/adstream_mjx/ );
      return CT_IMAGE  if ($path =~ /\/adstream_lx/  ||  $path =~ /\/adstream_nx/ );
      return CT_IMAGE  if ($path =~ /\/ads\/cap\.cgi/  );
   }

   return CT_JAVA   if ($url =~ /overture\.com\/ls_js_/ );

   return CT_IMAGE  if ($path =~ /\/scripts\/beacon\.dll/  ||  $path =~ /\/scripts\/beacon2\.dll/ );

   return CT_IMAGE  if ($url eq 'rtd.tubemogul.com/upi/');

   return CT_JAVA   if ($path =~ /\/javascript\//  ||  $path =~ /\/ajaxpro\// );

   return CT_JAVA   if ($path =~ /\/js\.php$/  ||  $path =~ /\/javascript\.php$/ );

   return CT_CSS    if ($path =~ /\/css\.php$/ );

   return CT_IMAGE  if ($path =~ /\/image\.php$/  || $path =~ /\/image\.php\// );

   return CT_JAVA   if ($path =~ /\/js\.ng\//  ||  $path =~ /\/js\// );

   return CT_JAVA   if ($path =~ /\/scripts\//  ||  $path =~ /\/script\// );

   return CT_XML    if ($url =~ /^xml\./ );

   if ($path =~ /\/b\/ss\// )
   {
      return CT_IMAGE  if ($path =~ /\/FAS/i  ||  $path =~ /\/H\./i  ||  $path =~ /\/G\./i );
   }
  
   return CT_JAVA   if ($url =~ /\.channel\.facebook\.com\/x\// );
   return CT_TEXT   if ($url =~ /\.channel\.facebook\.com\/p/ );
   return CT_IMAGE  if ($url eq 'www.facebook.com/fr/u.php' );

   return CT_IMAGE  if ($url eq 'pixel.mathtag.com/event/img' );
   return CT_JAVA   if ($url eq 'pixel.mathtag.com/event/js' );

   return CT_IMAGE  if ($url eq 'x.bidswitch.net/ul_cb/sync' );

   return CT_XML    if ($path =~ /\/xml-rpc/ );

   return CT_STREAM if ($path eq 'open/1');

   return CT_IMAGE  if ($url =~ /^pixel\./  ||  $path =~ /\/pixel$/ );

   return CT_TEXT   if ($url =~ /heatmap/ );

   return CT_204    if ($url eq 'analytics.livestream.com/track');

   return CT_HTML;
}


# comment out the next line if fastcgi is not configured
### while (new CGI::Fast)
{
   my $time = time;
   my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst);
   my $root;
   my $buffer;

   session_reinit();
   init();

   # the default is english
   my $forbidden = 'no access';
   my $title = $forbidden;
   my $explanation_prefix = 'URL blocked because it is';
   my $explanation_suffix = '';
   my $go_back = 'back';
   my $more_info = 'More information about ufdbGuard is <a href="https://www.urlfilterdb.com">here</a>.';

   my $contentType = getContentType( $origurl );

   $contentType = CT_204  if ($httpcode eq '204');

   if ($contentType == CT_IMAGE)
   {
      print "Content-Type: image/png\n";
      ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime( $time + 180 );
      printf "Expires: %s, %02d %s %04d %02d:%02d:%02d GMT\n",
	     $day[$wday], $mday, $month[$mon], $year+1900, $hour, $min, $sec;
      print "\n";

      my $imgfile;

      if ($category eq 'ads')
      {
         $imgfile = "transparent.png";
         $imgfile = "no-ads.png"      if ($mode eq 'noads');
         $imgfile = "smallcross.png"  if ($mode eq 'cross');
         $imgfile = "square.png"      if ($mode eq 'square');
      }
      else
      {
	 if ($mode eq 'cross') {
	    $imgfile = "smallcross.png"  if ($mode eq 'cross');
	 }
	 elsif ($mode eq 'square') {
	    $imgfile = "square.png"      if ($mode eq 'square');
	 }
	 elsif ($mode eq 'simple-red'  ||  $mode eq 'transparent'  ||  $mode eq 'transparant') {
	    $imgfile = "transparent.png";
	 }
	 else  {
	    $imgfile = "forbidden-normal-" . $lang . ".png";
	 }
      }
      $root = $ENV{'DOCUMENT_ROOT'};
      open( BLOCKEDPNG, "$root/images/$imgfile" )  ||  print "failed to open $root/images/$imgfile\n";
      print $buffer while (read (BLOCKEDPNG,$buffer,8192));
      close( BLOCKEDPNG );
   }
   elsif ($contentType == CT_204)
   {
      print "Status: 204 No Content\n";
      ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime( $time + 180 );
      print "Content-Type: text/plain\n";
      printf "Expires: %s, %02d %s %04d %02d:%02d:%02d GMT\n",
             $day[$wday], $mday, $month[$mon], $year+1900, $hour, $min, $sec;
      printf "X-blocked-category: %s\n", $category;
      printf "X-blocked-URL: %s\n", $url;
      print "\n";
   }
   elsif ($contentType == CT_STREAM)
   {
      if (1)
      {
	 print "Status: 204 no content\n";
         print "Content-Type: text/plain\n";
	 ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime( $time + 180 );
	 printf "Expires: %s, %02d %s %04d %02d:%02d:%02d GMT\n",
		$day[$wday], $mday, $month[$mon], $year+1900, $hour, $min, $sec;
	 printf "X-blocked-category: %s\n", $category;
	 printf "X-blocked-URL: %s\n", $url;
      }
      else
      {
	 print "Content-Type: application/octet-stream\n";
	 ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime( $time + 180 );
	 printf "Expires: %s, %02d %s %04d %02d:%02d:%02d GMT\n",
		$day[$wday], $mday, $month[$mon], $year+1900, $hour, $min, $sec;
      }
      print "\n";
   }
   elsif ($contentType == CT_JAVA)
   {
      print "Content-Type: application/x-javascript\n";
      ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime( $time + 180 );
      printf "Expires: %s, %02d %s %04d %02d:%02d:%02d GMT\n",
	     $day[$wday], $mday, $month[$mon], $year+1900, $hour, $min, $sec;
      print "\n";

      print "\n";
   }
   elsif ($contentType == CT_JSON)
   {
      print "Content-Type: application/json\n";
      ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime( $time + 180 );
      printf "Expires: %s, %02d %s %04d %02d:%02d:%02d GMT\n",
	     $day[$wday], $mday, $month[$mon], $year+1900, $hour, $min, $sec;
      print "\n";

      print "\n";
   }
   elsif ($contentType == CT_CSS)
   {
      print "Content-Type: text/css\n";
      ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime( $time + 180 );
      printf "Expires: %s, %02d %s %04d %02d:%02d:%02d GMT\n",
	     $day[$wday], $mday, $month[$mon], $year+1900, $hour, $min, $sec;
      print "\n";

      print "\n";
   }
   elsif ($contentType == CT_TEXT)
   {
      print "Content-Type: text/plain\n";
      ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime( $time + 180 );
      printf "Expires: %s, %02d %s %04d %02d:%02d:%02d GMT\n",
	     $day[$wday], $mday, $month[$mon], $year+1900, $hour, $min, $sec;
      print "\n";

      print "\n";
   }
   elsif ($contentType == CT_XML)
   {
      print "Content-Type: text/xml\n";
      ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime( $time + 180 );
      printf "Expires: %s, %02d %s %04d %02d:%02d:%02d GMT\n",
	     $day[$wday], $mday, $month[$mon], $year+1900, $hour, $min, $sec;
      print "\n";

      print "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
      print "<cross-domain-policy>\n";
      print "   <allow-access-from domain=\"*\" />\n";
      print "</cross-domain-policy>\n";
   }
   else 	# CT_HTML
   {
      if ($category eq 'fatal-error')
      {
	 print "Content-Type: text/html\n";
	 ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime( $time + 180 );
	 printf "Expires: %s, %02d %s %04d %02d:%02d:%02d GMT\n",
		$day[$wday], $mday, $month[$mon], $year+1900, $hour, $min, $sec;
	 print "Content-Language: $lang\n";
	 print "\n";

	 print "<html lang=\"$lang\">\n";
	 print "<head>\n";
	 print "   <title>The URL filter has a fatal error</title>\n";
	 print "   <meta name=\"description\" content=\"All URLs are blocked because the URL filter has a fatal error\" />\n";
	 print "</head>\n";
	 print "<body bgcolor=\"#e0e0e0\">\n";
	 print "<center>\n";
	 print "<font color=red><b>\n" .
	       "Access to the internet is blocked because<br>\n" .
	       "the URL filter has a fatal error. <br>\n" . 
	       "Ask your helpdesk or web proxy administrator for assistance." .
	       "</b></font>\n";
	 print "</center>\n";
	 print "$html_comment";
	 print "</body>\n";
	 print "</html>\n";
      }
      elsif ($category eq 'loading-database')
      {
	 print "Content-Type: text/html\n";
	 ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime( $time + 180 );
	 printf "Expires: %s, %02d %s %04d %02d:%02d:%02d GMT\n",
		$day[$wday], $mday, $month[$mon], $year+1900, $hour, $min, $sec;
	 print "Content-Language: $lang\n";
	 print "\n";

	 print "<html lang=\"$lang\">\n";
	 print "<head>\n";
	 print "   <title>a new URL database is being loaded</title>\n";
	 print "   <meta name=\"description\" content=\"All URLs are blocked because the URL filter is loading a fresh database\" />\n";
	 print "</head>\n";
	 print "<body bgcolor=\"#e0e0e0\">\n";
	 print "<center>\n";
	 print "<font color=red><b>\n" .
	       "Access to the internet is temporarily blocked because<br>\n" .
	       "a new URL database is being loaded by the URL filter. <br>\n" . 
	       "Wait one minute and try again." .
	       "</b></font>\n";
	 print "</center>\n";
	 print "$html_comment";
	 print "</body>\n";
	 print "</html>\n";
      }
      else
      {
	 if ($lang eq 'de') {
	    $forbidden = 'Verboten';
	    $title = "zugriff verweigert ($category)";
	    $explanation_prefix = 'Zugriff verweigert weil die URL die Klassifizierung';
	    $explanation_suffix = 'hat.';
	    $more_info = 'Mehr Informationen &uuml;ber ufdbGuard ist <a href="https://www.urlfilterdb.com/blocking/">hier</a>.';
	    $go_back = 'Klicken Sie hier um zur&uuml;ck zu gehen.';
	 }
	 elsif ($lang eq 'pl') {
	    $forbidden = 'Pobranie tej strony jest zabronione!';
	    $title = "Cenzura, zakaz pobrania ($category)";
	    $explanation_prefix = 'Nie otworzysz tej strony bo jest ona sklasyfikowana jako';
	    $explanation_suffix = 'przez program kontroli ufdbGuard';
	    $more_info = 'Informacja (po angielsku) o tym programie kontroli jest na <a href="https://www.urlfilterdb.com/blocking/">stronie</a>.';
	    $go_back = 'Wycofaj do poprzedniej strony';
	 }
	 elsif ($lang eq 'sv') {
	    $forbidden = 'Sidan stoppad enligt landstingets riktlinjer';
	    $title = "F&ouml;rbjuden ($category)";
	    $explanation_prefix = 'Access till denna sida &auml;r stoppad:';
	    $explanation_suffix = '';
	    $more_info = 'Mer information om ufdbGuard &auml;r <a href="https://www.urlfilterdb.com/blocking/">h&auml;r</a>.';
	    $go_back = 'Klicka h&auml;r f&ouml;r att komma tillbaks';
	 }
	 elsif ($lang eq 'nl') {
	    $forbidden = 'Geen Toegang';
	    $title = "geen toegang ($category)";
	    $explanation_prefix = 'De toegang is geblokkeerd omdat de URL in de categorie';
	    $explanation_suffix = 'valt.';
	    $more_info = 'Meer informatie over ufdbGuard is <a href="https://www.urlfilterdb.com/blocking/">hier</a>.';
	    $go_back = 'Klik hier om terug te gaan';
	 }
	 elsif ($lang eq 'es') {
	    $forbidden = 'Ning&uacute;n acceso';
	    $title = "ning&uacute;n acceso ($category)";
	    $explanation_prefix = 'Se bloquea el acceso puesto que el URL se considera ser';
	    $explanation_suffix = '';
	    $more_info = 'M&aacute;s informaci&oacute;n sobre ufdbGuard est&aacute; <a href="https://www.urlfilterdb.com/blocking/">aqu&iacute;</a>.';
	    $go_back = 'ir detr&aacute;s';
	 }
	 elsif ($lang eq 'it') {
	    $forbidden = 'Accesso negato';
	    $title = "accesso negato ($category)";
	    $explanation_prefix = "L'accesso &egrave; negato poich&eacute; l'URL appartiene a none";
	    $explanation_suffix = '';
	    $more_info = 'Maggiori informazioni su ufdbGuard sono disponibili <a href="https://www.urlfilterdb.com/blocking">qui</a>.';
	    $go_back = 'tornare indietro';
	 }
	 elsif ($lang eq 'pt') {
	    $forbidden = 'Proibido';
	    $title = "Proibido ($category)";
	    $explanation_prefix = "O acesso a este site foi bloqueado porque o conte&uacute;do est&aacute;";
	    $explanation_suffix = '';
	    $more_info = 'Mais informa&ccedil;&atilde;o sobre ufdbGuard est&aacute; <a href="https://www.urlfilterdb.com/blocking">aqui</a>.';
	    $go_back = 'volte';
	 }
	 elsif ($lang eq 'fr') {
	    $forbidden = 'Interdit';
	    $title = "Interdit ($category)";
	    $explanation_prefix = "L'access est inderdit parce que le site est";
	    $explanation_suffix = '';
	    $more_info = "Plus d'information de ufdbGuard est <a href=\"https://www.urlfilterdb.com/blocking\">ici</a>.";
	    $go_back = 'rentrer';
	 }
	 elsif ($lang eq 'tr') {
	    $forbidden = 'Eri&#351;im engellendi';
	    $title = "Eri&#351;im engellendi ($category)";
	    $explanation_prefix = "Ula&#351;mak istedi&#287;iniz sayfaya eri&#351;im kapal&#305;d&#305;r. S&#305;n&#305;f&#305;:";
	    $explanation_suffix = '';
	    $more_info = "ufdbGuard hakk&#305;nda bilgi i&ccedil;in <a href=\"https://www.urlfilterdb.com/blocking\">t&#305;klay&#305;n&#305;z</a>.";
	    $go_back = '&Ouml;nceki sayfa';
	 }
	 elsif ($lang eq 'NEWLANGUAGE') {
	    $forbidden = 'Forbidden';
	    $title = "Forbidden ($category)";
	    $explanation_prefix = 'Access is blocked since the URL is considered to be';
	    $explanation_suffix = '';
	    $more_info = 'More information about ufdbGuard is <a href="https://www.urlfilterdb.com/blocking">here</a>.';
	    $go_back = 'Click here to go back';
	 }
	 else {   # default (matches 'en')
	    $forbidden = 'Forbidden';
	    $title = "Forbidden ($category)";
	    $explanation_prefix = 'Access is blocked since the URL is considered to be';
	    $explanation_suffix = '';
	    $more_info = 'More information about ufdbGuard is <a href="https://www.urlfilterdb.com/blocking">here</a>.';
	    $go_back = 'Click here to go back';
	    $lang = 'en';
	 }

	 if ($color eq 'orange')
	 {
	    $textcolor = 'white';
	    $bgcolor = '#ee8811';
	 }
	 elsif ($color eq 'white')
	 {
	    $textcolor = '#3f003f';
	    $bgcolor = 'white';
	 }
	 elsif ($color eq 'black')
	 {
	    $textcolor = '#f0f0f0';
	    $bgcolor = 'black';
	 }
	 elsif ($color eq 'red')
	 {
	    $textcolor = '#f0f0f0';
	    $bgcolor = 'red';
	 }
	 elsif ($color eq 'grey'  ||  $color eq 'gray')
	 {
	    $textcolor = '#111111';
	    $bgcolor = '#c2c2c2';
	 }
	 else	# default color: orange
	 {
	    $textcolor = 'white';
	    $bgcolor = '#ee8811';
	 }

	 if ($size eq 'normal')
	 {
	    $titlesize = '+2';
	    $textsize = '+0';
	 }
	 elsif ($size eq 'small')
	 {
	    $titlesize = '+1';
	    $textsize = '-1';
	 }
	 elsif ($size eq 'large')
	 {
	    $titlesize = '+3';
	    $textsize = '+1';
	 }
	 else    # default size: normal
	 {
	    $titlesize = '+2';
	    $textsize = '+0';
	    $size = 'normal';
	 }

	 $url =~ s/[?;&].*//;

	 print "Content-Type: text/html\n";
	 ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime( $time + 180 );
	 printf "Expires: %s, %02d %s %04d %02d:%02d:%02d GMT\n",
		$day[$wday], $mday, $month[$mon], $year+1900, $hour, $min, $sec;
	 print "Content-Language: $lang\n";
	 print "\n";

	 if ($url eq 'https://blockedhttps.urlfilterdb.com')
	 {
	    print "<html lang=\"$lang\">\n";
	    print "<head>\n";
	    print "   <title>$title</title>\n";
	    print "   <meta name=\"description\" content=\"$title\" />\n";
	    print "</head>\n";
	    print "<body bgcolor=\"ffcccc\" link=\"red\" alink=\"red\" vlink=\"red\" text=\"red\">\n";
	    print "<p align=center>\n";
	    print "https://blockedhttps.urlfilterdb.com is used by ufdbGuard<br>\n";
	    print "to display messages about blocked URLs.<br>\n";
	    print "</p>\n";
	    print "$html_comment";
	    print "</body>\n";
	    print "</html>\n";
	 }
	 elsif ($category eq 'ads')
	 {
	    my $text;
	    $text = " ";	 # transparent
	    $text = " no ads " if $mode eq 'noads';
	    $text = " [] "     if $mode eq 'square';
	    $text = " x "      if $mode eq 'cross';
	    $text = "<font color=red><i>ads</i></font>"  if $mode eq 'simple-red';

	    print "<html lang=\"$lang\">\n";
	    print "<head>\n";
	    print "   <title>$title</title>\n";
	    print "   <meta name=\"description\" content=\"The ad is blocked by the URL filter.\nURL: $ufdbhost$ufdbrequesturi\" />\n";
	    print "</head>\n";
	    print "<body>\n";
	    print "<font size=\"$textsize\">$text</font>\n";
	    print "$html_comment";
	    print "</body>\n";
	    print "</html>\n";
	 }
	 else      	# no ads
	 {
	    if ($mode eq 'simple-red')
	    {
	       my $whyblocked = "$explanation_prefix $category $explanation_suffix.  URL = $url";
	       print "<html lang=\"$lang\">\n";
	       print "<head>\n";
	       print "   <title>$title</title>\n";
	       print "   <meta name=\"description\" content=\"The URL is blocked by the URL filter ($ufdbhost$escaped_ufdbrequesturi)\" />\n";
	       print "</head>\n";
	       print "<body bgcolor=\"ffe6e6\" link=\"blue\" alink=\"red\" vlink=\"black\" text=\"red\">\n";
	       print "<p align=center>\n";
	       print "<a title=\"$whyblocked\">$forbidden<br><i>$category</i></a>\n";
	       print "<a href=\"/cgi-bin/show_url_details.cgi?mode=$mode&url=$ufdbhost$escaped_ufdbrequesturi\">why is this URL blocked?</a>\n";
	       print "</p>\n";
	       print "$html_comment";
	       print "</body>\n";
	       print "</html>\n";
	    }
	    elsif ($category eq 'social-bdg'  ||  $category eq 'socbadges'  ||  $category eq 'social-badges'  ||  $category eq 'social_badges')
	    {
	       print "<html lang=\"$lang\">\n";
	       print "<head>\n";
	       print "   <title>block social networking badge</title>\n";
	       print "   <meta name=\"description\" content=\"social networking badge is blocked by the URL filter\" />\n";
	       print "</head>\n";
	       # print "<body width=30 height=30 bgcolor=\"transparent\">\n";
	       print "<body bgcolor=#fafafa>\n";
	       print "<center>\n";
	       print "<font size=\"-1\" color=\"#1f1f1f\">\n" .
		     "<a title=\"The social networking badge is blocked.\"> B </a>\n" .
		     "</font>\n";
	       print "</center>\n";
	       print "$html_comment";
	       print "</body>\n";
	       print "</html>\n";
	    }
	    else
	    {
	       print "<html lang=\"$lang\">\n";
	       print "<head>\n";
	       print "   <title>$title</title>\n";
	       print "   <meta name=\"description\" content=\"The URL is blocked by the URL filter\" />\n";
	       print "</head>\n";
	       print "<body bgcolor=\"$bgcolor\" text=\"$textcolor\">\n";
	       print "<font size=\"$titlesize\">$forbidden</font> <br>\n";
	       print "<font size=\"$textsize\">\n";
	       print "$explanation_prefix <i>$category</i> $explanation_suffix <br>\n";
	       print "URL: $url <br>\n";
	       print "<p>\n";
	       print "<a href=\"javascript:history.go(-1);\">$go_back</a>. <br>\n";
	       print "$admin\n";
	       print "<p>\n";
	       print "$more_info\n";
	       print "<br>\n&nbsp;<p />\n";
	       print "</font>\n";
	       print "<font size=\"-3\">";
	       print "user=$clientuser &nbsp; "      if (defined($clientuser)  &&  length($clientuser)>0);
	       print "client=$clientaddr &nbsp; "    if (defined($clientaddr)  &&  length($clientaddr)>0);
	       print "group=$clientgroup &nbsp; "    if (defined($clientgroup)  &&  length($clientgroup)>0);
	       print "source=$clientname &nbsp; "    if (defined($clientname)  &&  length($clientname)>0);
	       print "</font>\n";
	       print "$html_comment";
	       print "<!-- color:$color size:$size mode:$mode lang:$lang category:$category -->\n";
	       print "</body>\n";
	       print "</html>\n";
	    }
	 }
      }
   }
}

exit 0;

