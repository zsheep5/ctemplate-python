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

import pyctemplate
import os
cwd = os.getcwd()
import time , datetime 
gg = time.localtime()
gg = datetime.date.today()
d = {"varName":"haha1",
     "aLoop":[
        {"loopVar":"bool","loopVar2":True},
        {"loopVar":"float","loopVar2":'Unicode Test'} ],
     "sweet":{"name":"æ  ñ Ꮉ ⻋ ⻇",
              "price":None, "isBig": 44
             }
    }
e = {"varName":"file template test",
     "aLoop":[
        {"loopVar":"bool","loopVar2":False},
        {"loopVar":"float","loopVar2":98} ],
     "sweet":{"name":"it works with files",
              "price":gg,"isBig": 0
             }
    }


print ( pyctemplate.compile_template(
  """<* This is a comment *>
  varName: <TMPL_var name="varName"> 
  <TMPL_loop name="aLoop"> 
  Loop has var <TMPL_var name="loopVar"> 
      value <TMPL_var name="loopVar2"> 
  </TMPL_loop> 
  Привет: <TMPL_var name="sweet.name">
  Sweet price: <TMPL_var name="sweet.price">
  <TMPL_if name="sweet.isBig" value="44">
  Sweet is big
  </TMPL_IF>
    """,d ))


print(pyctemplate.compile_template( cwd + '/ctemplate_test', e, 'file'))

#throw a parse error
print ( pyctemplate.compile_template(
  """<* This is a comment *>
  varName: <TMPL_var name="varName"> 
  <TMPL_loop name="aLoop"> 
  Loop has var <TMPL_var name="loopVar"> 
      value <TMPL_var name="loopVar2"> 
  </TMPL_loo> 
  Sweet name: <TMPL_var name="sweet.name">
  Sweet price: <TMPL_var name="sweet.price">
  <TMPL_if name="sweet.isBig" value="0">
  Sweet is big
  </TMPL_IF>
    """,d, 'string' ))
