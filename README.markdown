# Notice #
This is a work-in-progress re-factoring of JSi.  It may not compile at this time.

# JSi: JavaScript Interpreter #
Author: Joshaven Potter [email me](mailto:yourtech@gmail.com)  
Date:   Aug - 11 - 2009  
Updated: Sat Aug 11 -- Joshaven  
Credits: This project was forked from [JSi](http://github.com/sebastien/JSi) and 
inspired by [jsext](http://jsext.sourceforge.net)

## Project Description ##
This project aims at creating a high-quality standards library which is engine 
and OS agnostic.

The main goal of this project is to facilitate server-side JavaScript through 
providing a JavaScript Interpreter that connects to a server-side JavaScript 
Engine (ie V8, TraceMonkey, SquirrelFish, etc) through an engine specific 
driver.

## JSi Extension: Modules & Macros ##
JSi Modules will be removed in favor of jsPods
JSi Macros will be removed in favor of Connectors

JSi facilitates JavaScript interpreter extension through encouraging 
Modules (standardized building blocks) that are plugged into JSi and loaded 
on demand by JSi.  The creation of modules is simplified through the use of 
Macros (single instructions that expand into a set of instructions). Standard 
Macros are are provided by the JavaScript Engine Driver.

### Drivers ###
JSi drivers provide a standard API (C or C++ to JavaScript crossover language) 
for interacting with the JavaScript Engine.  This API standardizes creation 
and of manipulation of JavaScript functions, objects, etc.

### Macros ###
Extending JavaScript with new or existing C libraries becomes easy using the 
Macros provided the driver API.



## Composition ##
The JSi source code is separated in two primary folders:

* `src/` which contains the JSi infrastructure source code.
* `lib/` which contains the extension modules. The directory defines the 
  module hierarchy. For instance, The source code for the 'system.posix' 
  extension module is located in 'lib/system/posix/'.

## Getting Started ##

### Prerequisites ###
POSIX (Unix/Linux/Mac) environment, gcc, git, python scons & subversion


### Obtaining Source ###
Create a ~/src directory and check out JSi out into it ~/src/JSi

> `mkdir -p ~/src`  
  `cd ~/src`  
  `git clone git://github.com/sebastien/JSi.git`  
  `cd JSi`

### Compiling (A.K.A. Installation) ###
Building and downloading dependencies, namely the V8 source, is as simple as 
typing `make` as root.

> `sudo make`  
> `sudo make install`  

You may also wish to take advantage of other make options:
You have a couple of options that you can activate:

>   `make compact`    # Builds a compressed JSi executable  
>   `make DEBUG=1`    # Enables debugging mode  
>   `make STATIC=1`   # Embeds JSi modules within the executable  
>   `make LIBTASK=1`  # Embeds libtask and provides a 'yield' feature  

## Usage ##
To confirm your installation you can type `which JSi` which should yield 
"/usr/local/bin/JSi" or "/usr/bin/JSi" depending upon where you installed JSi.

To execute a JavaScript file simply type `JSi` followed by a reference to the 
script, for example:

`JSi examples/intro-helloworld.js`

