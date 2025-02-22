# 2.11BSD-Dr.Nim

The game of Dr. Nim implemented as a CGI application for 2.11 BSD running on the PiDP-11

It was written as a proof-of-concept for an update to the 2.11BSD-httpd web server that forwards request headers to the CGI program via the environment and leaves POST content available via stdin. 

Beyond installing the drnim_webcgi program in the /home/www/cgi-bin directory, the marble.png image is expected to be in the /home/www/images directory.

