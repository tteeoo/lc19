# lc19

A simple (<500 SLOC) server for the Gemini protocol written in C.
The name is an allusion to Launch Complex 19, where NASA launched the Gemini spaceflights.
Tested on GNU/Linux (probably works on other Unix-like systems). Created by Theo Henson <theodorehenson at protonmail dot com>.

## Installation

The requirements are minimal and should already be on most GNU/Linux distributions:

* glibc
* openssl > 1.0.1

```
$ make
# make install
```

### macOS compatibility

The server should also work on macOS, but you will need to `brew install argp-standalone` and at the `-I`, `-L`, and `-l` options to the Makefile for the installed location.

## Usage

Create the file "endpoints" in the root of the specified directory, with tab separated values for each endpoint:

```
<mimetype>	<file path>	[url path]
...
```

Example:

```
text/gemini	index.gmi
text/gemini	index.gmi	/
text/gemini	about.gmi
image/png	images/cats.png	/cats.png
```

(By default files will be served at `/<file path>`, but if `[url path]` is specified then that will be used.)

Run the server with the following syntax:

```
lc19 [-p PORT] [-d DIR] [-c CERT] [-k KEY]
```

Example:

```
$ lc19 -d gemini/ -c cert.pem -k key.pem
```

(The port defaults to 1965.)

Reload the endpoints file without restarting the server by sending a SIGHUP signal, e.g.:

```
$ pkill -HUP lc19
```

Creating the endpoints file may seem cumbersome, but its generation can be automated and it provides many benefits:

* Security — Only the files listed are able to be accessed.
* Customization — You are able to directly change the URL path that files are found at.
* Simplicity — Specifying the MIME types of files yourself is the most portable (and simple) method, as you needn't rely on having everything built into the code, or having external files/programs like `/etc/mime.types` or `file`.

## License

All files are released under the Unlicense, a public domain equivalent license.
