ctemplate-python
================

This is a python port of an excellent C Template Library 1.0 by Stephen C. Losen (http://libctemplate.sourceforge.net). Distributed under GPL. To install type sudo python setup.py install. For usage case, see useTemplate.py.

Why should you use it?

1. It is superfast and solid, written in pure c. It outpaces all python-based competition.
2. It is really easy to use - has only one function to call and fits comfortably in one .h file and two .c files!
3. Its syntax reasembles HTML and is easy to learn. It has only three sigificant 'tags': loop tag, variable substitution tag and conditional tag.

See original documentation for explanations of usage.

Send me an email if you see it fit:  marek.lipert at gmail.com


4. Justin G zsheep changes

4.1. ported the code to work with python 3.5 did not keep support for python2
it should compile to lower versions of 3 

4.2. the function now takes in three arguements  
    1. HTML Template as a string or full path to file 
    2. dictionary that maps to the var names in the template 
    3. optional arguement 'file' or 'string' (default = string) 
    if the 'file' is passed the extension will try to open a file 
    locate the template text  

4.3. It no longer writes error messages to a log file it uses
    the python c-api pyErr methods to create exceptions and 
    passed back up to the calling function

4.4. renamed the function pyctemplate

4.5. Added basic support for type conversion to string.  
    1. Float and Double does not support rounding it uses str(float)
    2. Boolean uses str(bool) so true turns into True etc..
    4. Integer and Long  str(0)
    5. Other datatypes/objects have to be casted to string before
    being passed into function or exception will be thrown as
    before.. 

4.6. Tested on Debian 9

4.7. Unicode support has not been tested probably will not work  

to install  download the code 
python3 setup.py build_ext install

for Debug  modifey the setup then run
python3 setup.py build_ext --debug --inplace install

