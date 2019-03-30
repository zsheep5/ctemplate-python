#!/usr/bin/python
# *
# * C Template Library 1.0
# *
# * Copyright 2009 Stephen C. Losen.  Distributed under the terms
# * of the GNU General Public License (GPL)
# * 
# * Python port written by Marek Lipert (marek.lipert@gmail.com)
# *   
# *

from distutils.core import setup, Extension
#from setuptools import setup, find_packages


""" for a debug build run this command and 
comment out the below command
python builder even with the --debug option
does not always create the symbols... 

run this command for python3 on debian some linux distros 
name python3 or python, other name python 2.7 python or python2
so check to see what you got.  :-)

python3 setup.py build_ext --debug --inplace install

setup(name="pyctemplate",version="2.0",   
        ext_modules=
        [
            Extension("pyctemplate", 
                ["_ctemplate.c", "ctemplate.c"],
                extra_compile_args=['-O0']
            )    
        ]
    )
"""

with open("README", "r") as fh:
    long_description = fh.read()
"""
setuptools.setup(
    name="pyctemplate",
    version="2.0",
    author="Justin G zsheep",
    author_email="somewhere@nowhere",
    description="HTML Template Engine using ctemplate",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="",
    packages=setuptools.find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
)"""

setup(name="pyctemplate",
    version="2.0",
    author="Justin G zsheep",
    author_email="somewhere@nowhere",
    url="http://hi_hoooo.nowhere",
    description="HTML Template Engine using ctemplate",
    long_description=long_description,
    license= "GPL",
    classifiers=[
        "Programming Language :: C :: 99 ",
        "License :: OSI Approved :: GPL",
        "Operating System :: OS Independent",
    ],
    ext_modules=[
        Extension("pyctemplate", 
            ["_ctemplate.c", "ctemplate.c"]
        )
    ]
)

#run the test file
#import useTemplate