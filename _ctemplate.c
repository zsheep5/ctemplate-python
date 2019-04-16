/*
 * C Template Library 1.0
 *
 * Copyright 2009 Stephen C. Losen.  Distributed under the terms
 * of the GNU General Public License (GPL)
 * 
 * Python port written by Marek Lipert (marek.lipert@gmail.com)
 * 
 * NO LONGER supports python 2.X
 * Python >=3.5  update 03/23/2019 Justin G  zsheep  and renamed module
 * pyctemplate
 * added the optional argument file_or_string  if set to file tries to
 * open a file,  if string is set then process the string as the template
 * removed the error file creation now sends errors to PyErr_*** c api 
 */
 
#include <Python.h>
#include <datetime.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include "ctemplate.h"


static char module_docstring[] =
    "This module is a port of C Template Library 1.0 for Python";
static char ctemplate_docstring[] =
    "This function takes three arguments: a template string or file path, "  
    "A dictionary key = the template var name then the value of the var "
    "then a text var 'string' or 'file', is descripes the first arguement a string or open a file."
    "It returns compiled template as a string. "
    "Dictionary values are casted to string using python's built in str(). "
    "if the resulting values format is not the desired result must be casted to string"
    "prior to being passed into this function.";

static PyObject *ctemplate_skompiluj_wzorzec(PyObject *self, PyObject *args);
static TMPL_varlist *addDictionaryToVarlist(TMPL_varlist *list, const char *prefix, PyObject *dictionary);
//defines the python function anem mapped to function and how arguements are accessed, then the function description
static PyMethodDef module_methods[] ={
        {"compile_template", ctemplate_skompiluj_wzorzec, METH_VARARGS, ctemplate_docstring},
        {NULL, NULL, 0, NULL}};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "pyctemplate",               /* m_name */
    PyDoc_STR(module_docstring), /* m_doc */
    -1,                          /* m_size */
    module_methods,              /* m_methods */
    NULL,                        /* m_reload */
    NULL,                        /* m_traverse */
    NULL,                        /* m_clear */
    NULL,                        /* m_free */
};

PyMODINIT_FUNC PyInit_pyctemplate(void)
{
  PyObject *m = PyModule_Create(&moduledef);
  PyDateTime_IMPORT; //this has to be here after the create out is Init fails for the module
  if (m == NULL)
    return NULL;
  return m;
}

static PyObject *ctemplate_skompiluj_wzorzec(PyObject *self, PyObject *args)
{
  //declare the first set of variables we need 
  char *file_or_string = "string";
  char *template = NULL;
  char *temppath = "";
  PyObject *main_dictionary = NULL;
  
  //step 1 get the passed variable
  if (!PyArg_ParseTuple(args, "sO!|ss", &template,
                        &PyDict_Type, &main_dictionary,
                        &file_or_string, &temppath))
    return NULL; /* this returns borrowed references */

  //step2 break the python dictionary down to be processed by ctemplate 
  TMPL_varlist *globals = addDictionaryToVarlist(NULL, NULL, main_dictionary);

  if ((!globals) || (PyErr_Occurred() != NULL)) //test to see if we got the globals or if a error was set 
  { 
    if (globals)
       TMPL_free_varlist(globals);
    return NULL;
  }

  PyObject *retVal = NULL;

  FILE *mainFile;
  char *result = NULL ;
  int position = 0, freadPosition = -1, temp_fd;
  char temporaryFileName[PATH_MAX] = "" ;

  strcat(temporaryFileName, temppath);
  strcat(temporaryFileName, "/ctemplate_XXXXXX");
  temp_fd = mkstemp(temporaryFileName); /* we need temporary file name for file operations forced by ctemplate */

  if ((mainFile = fdopen(temp_fd, "wr")) == NULL)
  {
    PyErr_Format(PyExc_IOError, "Failed to create the temporary File %s most likely a permission error", temporaryFileName);
    goto OutFileFailed;
  }

  if (strcmp(file_or_string, "file") == 0 )
  {
    TMPL_write(template, NULL, NULL, globals, mainFile, NULL);
  }
  else
  {
    TMPL_write(NULL, template, NULL, globals, mainFile, NULL);
  }

  if (PyErr_Occurred()!=NULL) //test to see if the parse engine failed
  {
    goto onParseFail;
  }

  fseek(mainFile, 0L, SEEK_END);
  position = ftell(mainFile);

  if (position)
  {
    result = malloc(position + 1);
    if ((mainFile = fopen(temporaryFileName, "r")) == NULL)
    {
      PyErr_SetFromErrno(PyExc_IOError);
      goto FileProcessError;
    }

    freadPosition = fread(result, 1, position, mainFile);
    if (ferror(mainFile))
    {
      PyErr_SetFromErrno(PyExc_IOError);
      goto FileProcessError;
    }
    else
    {
      result[position] = '\0';
      retVal = PyUnicode_FromString(result);
      if (!retVal)
        goto FileProcessError;
      goto CleanUpReturn;
    }
  }
  PyErr_SetString(PyExc_IOError, "Empty results file");
FileProcessError:
  free(result);
  fclose(mainFile);
  remove(temporaryFileName);
  return NULL;
OutFileFailed: //this 
  TMPL_free_varlist(globals);
  remove(temporaryFileName);
  return NULL;
onParseFail:
  fclose(mainFile);
  remove(temporaryFileName);
  if (globals)
  TMPL_free_varlist(globals);
  return NULL;
CleanUpReturn:
  TMPL_free_varlist(globals);
  fclose(mainFile);
  remove(temporaryFileName);
  return retVal;
}

static TMPL_varlist *addDictionaryToVarlist(TMPL_varlist *list, const char *prefix, PyObject *dictionary)
{
  TMPL_varlist *retVal = list;
  TMPL_varlist *loopList = NULL;
  TMPL_loop *aLoop = NULL;
  PyObject *keys = PyDict_Keys(dictionary); /* new reference */
  Py_ssize_t i = 0, dsize = -1, j = 0, lsize = -1;
  PyObject *key = NULL, *value = NULL;
  PyObject *nestedDict = NULL;
  char *keyChar = NULL, *valChar = NULL;
  char *keyCharTmp = NULL;

  if (!keys)
    goto onError;
  dsize = PyList_Size(keys);
  for (i = 0; i < dsize; i++)
  {                                          /* items numbered from 0 to dsize */
    key = PyList_GetItem(keys, i);           /* borrowed reference */
    value = PyDict_GetItem(dictionary, key); /* borrowed reference */
    if (!value || !key)
      goto onError;
    keyCharTmp = PyUnicode_AsUTF8(key); //python 2.0 PyString_AsString(key);
    if (!keyCharTmp)
      goto onError;
    if (prefix && strlen(prefix))
    { /* have prefix */
      keyChar = malloc(strlen(prefix) + /* . */ 1 + strlen(keyCharTmp) + /* \0 */ 1);
      sprintf(keyChar, "%s.%s", prefix, keyCharTmp);
    }
    else
    { /* no prefix */
      keyChar = malloc(strlen(keyCharTmp) + 1);
      strcpy(keyChar, keyCharTmp);
    }
    if (PyObject_TypeCheck(value, &PyList_Type))
    { /* loop */
      lsize = PyList_Size(value);
      for (j = 0; j < lsize; j++)
      {                                        /* items numbered from 0 to lsize */
        nestedDict = PyList_GetItem(value, j); /* borrowed reference */
        if (!nestedDict)
          goto onError;
        if (!PyObject_TypeCheck(nestedDict, &PyDict_Type))
        { /* array does not contain a dictionary */
          PyErr_SetString(PyExc_AttributeError, "Lists can only contain dictionaries");
          goto onError;
        }
        /* recurrence */
        loopList = addDictionaryToVarlist(NULL, NULL, nestedDict);
        if (!loopList)
          goto onError;
        aLoop = TMPL_add_varlist(aLoop, loopList);
      }
      if (!aLoop)
        goto onError;
      retVal = TMPL_add_loop(retVal, keyChar, aLoop);
      aLoop = NULL;
    }
    else
    {
      if (PyObject_TypeCheck(value, &PyDict_Type))
      { /* single subdictionary */
        retVal = addDictionaryToVarlist(retVal, keyChar, value);
      }
      else
      { /* string 
        */
        if (PyObject_TypeCheck(value, &PyUnicode_Type))
        {
          valChar = PyUnicode_AsUTF8(value);
        }
        else if (PyObject_TypeCheck(value, &PyLong_Type))
        {
          valChar = PyUnicode_AsUTF8(PyObject_Str(value));
        }
        else if (PyObject_TypeCheck(value, &PyFloat_Type))
        {
          valChar = PyUnicode_AsUTF8(PyObject_Str(value));
        }
        else if (PyObject_TypeCheck(value, &PyBool_Type))
        {
          valChar = PyUnicode_AsUTF8(PyObject_Str(value));
        }
        else if (PyDate_Check(value))
        {
          valChar = PyUnicode_AsUTF8(PyObject_Str(value));
        }
        else if (PyTime_Check(value))
        {
          valChar = PyUnicode_AsUTF8(PyObject_Str(value));
        }
        else if (value == Py_None)
        {
          char *None = "None";
          valChar = None;
        }
        else
        { // not a list nor a string - error!
          PyErr_SetString(PyExc_AttributeError, "Entries in dictionaries can be either strings in Unicode, bools, interger, long, float, double, lists or other dictionaries. No other types are permitted");
          goto onError;
        }
        if (!valChar)
          goto onError;
        retVal = TMPL_add_var(retVal, keyChar, valChar, NULL);
        free(keyChar);
        keyChar = NULL;
      }
    }
  }
  if (keys)
    Py_DECREF(keys);
  return retVal;
onError:
  if (loopList)
    TMPL_free_varlist(loopList);
  if (keys)
    Py_DECREF(keys);
  if (keyChar)
    free(keyChar);
  if (retVal)
    TMPL_free_varlist(retVal);
  return NULL;
}
