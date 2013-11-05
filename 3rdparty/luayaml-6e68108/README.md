lua-yaml
========
Lua [YAML] serialization using [LibYAML].

Installation
------------

The following will build and install to the default location (`/usr/local`):

    make
    sudo make install

To change the installation prefix to `/usr` use:

    make
    sudo make install PREFIX=/usr

The Makefile also supports the [DESTDIR] convention, which packagers may
find useful for installing into a "buildroot".

[DESTDIR]: http://www.gnu.org/prep/standards/html_node/DESTDIR.html

Usage
-----

The module defines the functions `dump`, `load`, and `configure` within the
global `yaml` table.

    require 'yaml'

    serialized = yaml.dump{1, 2, 3, 4}
    content = yaml.load(serialized)

Note that lua-yaml only supports YAML 1.1 (not 1.2). This is a limitation of
the underlying [LibYAML] library rather than lua-yaml itself.

Todo
----

* Make compatible with Lua 5.2
* Allow creating dump/load objects with internal configuration settings
* Better error checking when using LibYAML dump functions

History
-------

#### v0.3.1 - June 22 2012

* Added rockspec
* Added basic tests
* Improved README

#### v0.3 - June 01 2012

* Removed bundled copy of LibYAML
* Fixed Makefile to link against system copy of LibYAML and support `DESTDIR`
* Cleaned up README and reformated as Markdown

#### v0.2 - November 23 2009

* Updated LibYAML to version 0.1.3
* Now properly dumps and loads strings containing binary data using base64
* Dumped strings are quoted when they could be loaded as numbers
* Nulls are loaded as `yaml.null`, a function that returns itself and can
  be used to test for equality
* Load now also recognizes `yes` as a boolean truth value
* Zero length scalars are not converted to `nil`

#### v0.1 - May 12 2009

* Initial release

[License]
---------

Copyright (c) 2009-2012, Andrew Danforth.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

Thanks
------

Thanks to the following people for suggestions and patches:

* Peter Mawhorter
* Cyril Romain
* Adrian Sampson
* Stepan Bakshaev

Portions of this software were inspired by Ingy döt Net's [YAML::LibYAML].

[YAML]: http://yaml.org/ "YAML Ain't Markup Language"
[LibYAML]: http://pyyaml.org/wiki/LibYAML "YAML library written in C"
[License]: http://en.wikipedia.org/wiki/MIT_License "MIT license"
[YAML::LibYAML]: http://search.cpan.org/~nuffin/YAML-LibYAML "YAML Perl Module"
