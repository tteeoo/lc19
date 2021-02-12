# lc19

A simple (<500 SLOC) server for the Gemini protocol written in C.
The name is an allusion to Launch Complex 19, where NASA launched all of the Gemini manned spaceflights.
Tested on GNU/Linux. Created by Theo Henson <theodorehenson at protonmail dot com>.

## Installation

Requirements:

* glibc
* openssl > 1.0.1

```
$ make
# make install
```

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

(By default files will be served at /<file path>, but if [url path] is specified then that will be used.)

Then run the server with the following syntax:

```
lc19 [-p PORT] [-d DIR] [-c CERT] [-k KEY]
```

Example:

```
$ lc19 -d gemini/ -c cert.pem -k key.pem
```

(The port defaults to 1965.)

## License

All files are released under the Unlicense, a public domain equivalent license.
