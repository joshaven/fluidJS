# WARNING! #
This readme is a recommended rewrite that has not been reviewed or approved 
by the the project authors.  I had some questions and after studying the code 
and documentation provided, I decided to attempt to rewrite this document to 
clarify the subject matter presented.  Although I have carefully worded and
researched the following content, the information herein may not accurately 
represent or explain K7 or V8.

# K7 - A standard library for V8 #
Author: Sebastien Pierre <sebastien@type-z.org>  
Date:   28-Sep-2008  

## Project Description ##
The K7 projects aims at creating a high-quality standard library for [Google's 
open source V8 JavaScript Engine](http://code.google.com/p/v8/).

The main goal of K7 is to facilitate server-side JavaScript through providing 
a standard library that simplifies the use and extension of V8.

## K7 Extension: Modules & Macros ##
K7 facilitates JavaScript interpreter extension through encouraging 
Modules (standardized building blocks) that are plugged into K7 and loaded 
on demand by K7.  The creation of modules is simplified through the use of 
Macros (single instructions that expand into a set of instructions). 
The process of working with the V8 JavaScript Engine is simplified 
through K7 Macros.

### Macros ###
K7 simplifies the creation of extensions through simple macros that aid in 
tying your into modules into the V8 JavaScript Engine.

### Modules ###
The K7 module system is organized in a module tree consisting of API 
documentation, C and/or JavaScript files which are loaded upon demand.  The 
module tree structure is presented in doc/API.yaml but built by convention 
through crawling the lib/ directory structure.

Please keep these principals in mind when writing extensions:

* Keep It Short and Simple -- Avoid writing complex extension modules with 
  lots of functions and classes. "Everything should be made as simple as 
  possible, but no simpler" - Albert Einstein
* Stick to Standards -- Provide binding to standard, well-known API (like 
  POSIX). This allows to keep the extensions simple, make maintenance and 
  usage easier.
* Use JavaScript for JavaScript objects -- Write JavaScript objects in pure 
  JavaScript. For example, a module providing a 'File' object should construct 
  the file object in pure JavaScript. In this example, the JavaScript should 
  in turn, take advantage of the 'posix' C module for the actual I/O.
* Choose Dependencies Carefully -- When you want to start writing an extension,
  pick a library that follows the UNIX philosophy 
  ["do one thing and do it well."](http://en.wikipedia.org/wiki/Unix_philosophy)
  A good example of a clean & functional library is 'libcurl'.

## Composition ##
The k7 source code is separated in two primary folders:

* `src/` which contains the K7 infrastructure source code.
* `lib/` which contains the extension modules. The directory defines the 
  module hierarchy. For instance, The source code for the 'system.posix' 
  extension module is located in 'lib/system/posix/'.

## Getting Started ##

### Prerequisites ###
POSIX (Unix/Linux/Mac) environment, git, GPP, subversion & scons.

### Obtaining Source ###
Create a ~/src directory and check out k7 out into it ~/src/k7

> `mkdir -p ~/src`  
  `cd ~/src`  
  `git clone git://github.com/sebastien/k7.git`  
  `cd k7`

### Compiling (A.K.A. Installation) ###
Building and downloading dependencies, namely the V8 source, is as simple as 
typing `make` as root.

> `sudo make`

You may also wish to take advantage of other make options:
You have a couple of options that you can activate:

>   `make compact`    # Builds a compressed K7 executable  
>   `make DEBUG=1`    # Enables debugging mode  
>   `make STATIC=1`   # Embeds K7 modules within the executable  
>   `make LIBTASK=1`  # Embeds libtask and provides a 'yield' feature  

## Usage ##

Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.