#include <Python.h>
#include <stdlib.h>
//
// Copyright 2021 Sergey Pechenko
//
// Minimal C glue code for Python  interaction
//

int
main(int argc, char *argv[])
{
    char hook_file_path[] = "./";
    char hook_file[] = "ldap_hooks";
    char *hook_functions[] = {
      "add_hook","bind_hook","unbind_hook","compare_hook","delete_hook","modify_hook",
      "modrdn_hook","search_hook","abandon_hook","extended_hook","response_hook",NULL };
    PyObject *pName, *pModule, *pFunc, *pValue, *sys, *path, *newPaths;
    int i;
    Py_Initialize();
    // credits to https://stackoverflow.com/questions/50198057/python-c-api-free-errors-after-using-py-setpath-and-py-getpath
    // get handle to python sys.path object
    sys = PyImport_ImportModule("sys");
    path = PyObject_GetAttrString(sys, "path");
    // make a list of paths to add to sys.path
    newPaths = PyUnicode_Split(PyUnicode_FromString(hook_file_path), PyUnicode_FromWideChar(L":", 1), -1);
    // iterate through list and add all paths
    for(i=0; i<PyList_Size(newPaths); i++) {
        PyList_Append(path, PyList_GetItem(newPaths, i));
    }
    // print out sys.path after appends
    // PyObject *newlist = PyUnicode_Join(PyUnicode_FromWideChar(L":", -1), path);
    // printf("newlist = %ls\n", PyUnicode_AsWideCharString(newlist, NULL));
    // Py_XDECREF(newlist);
    Py_XDECREF(newPaths);
    Py_XDECREF(path);
    Py_XDECREF(sys);

    pName = PyUnicode_DecodeFSDefault(hook_file);
    /* Error checking of pName left out */

    if (pName == NULL){
      fprintf(stderr,"No Python hook file found\n");
    }

    pModule = PyImport_Import(pName);
    Py_DECREF(pName);
    i = 0;
    if (pModule != NULL) {
        while(hook_functions[i]!=NULL) {
          pFunc = PyObject_GetAttrString(pModule,hook_functions[i]);
          /* pFunc is a new reference */
          if (pFunc && PyCallable_Check(pFunc)) {
              fprintf(stderr,"\nfunction %s exists and can be called\n", hook_functions[i]);
              fprintf(stderr, "Calling %s ==>\n", hook_functions[i]);
              pValue = PyObject_CallFunction(pFunc, "s", hook_functions[i]);
              if (pValue != NULL) {
                if (pValue == Py_None) {
                 fprintf(stderr,"==> Дружище, это None, тут правда ничего нет\n");
                }
                else if ((pValue == Py_False) || (pValue == Py_True)) {
                 fprintf(stderr,"==> Bool:\n");
                 if (pValue == Py_False) {
                   fprintf(stderr, "✗ False\n");
                 } else {
                   fprintf(stderr, "✓ True \n");
                 }
                } else if (PyUnicode_Check(pValue)) {
                 fprintf(stderr,"==> String:\n");
                 const char* newstr = PyUnicode_AsUTF8(pValue);
                 fprintf(stderr,"\"%s\"\n", newstr);
                } else if (PyDict_Check(pValue)) {
                 PyObject *key, *value;
                 Py_ssize_t pos =0;
                 fprintf(stderr,"==> Dict:\n");
                 while (PyDict_Next(pValue, &pos, &key, &value)) {
                  fprintf(stderr, "%s: %s\n", PyUnicode_AsUTF8(key), PyUnicode_AsUTF8(value));
                 }
                } else if (PyList_Check(pValue)) {
                 fprintf(stderr,"==> List:\n");
                 Py_ssize_t i, seq_len;
                 PyObject *item;
                 seq_len = PyList_Size(pValue);
                 for (i=0; i<seq_len; i++) {
                   item = PyList_GetItem(pValue, i);
                   fprintf(stderr, "● %s\n", PyUnicode_AsUTF8(item));
                   // !!!--> NOT NEEDED <--!!!  Py_DECREF(item);
                   }
                } else if (PyTuple_Check(pValue)) {
                 fprintf(stderr,"==> Tuple:\n");
                 Py_ssize_t i, seq_len;
                 PyObject *item;
                 seq_len = PyTuple_Size(pValue);
                 for (i=0; i<seq_len; i++) {
                   item = PyTuple_GetItem(pValue, i);
                   fprintf(stderr, "► %s\n", PyUnicode_AsUTF8(item));
                   // !!!--> NOT NEEDED <--!!! Py_DECREF(item);
                   }
                } else if (PyFloat_Check(pValue)) {
                  fprintf(stderr, "==> Float: %f\n", PyFloat_AsDouble(pValue));
                } else if (PyLong_Check(pValue)) {
                  fprintf(stderr, "==> Long: %ld\n", PyLong_AsLong(pValue));
                } else if (PySet_Check(pValue)) {
                 fprintf(stderr,"==> Set:\n");
                 PyObject *str_repr = PyObject_Repr(pValue);
                 fprintf(stderr, "☼ %s\n", PyUnicode_AsUTF8(str_repr));
                 Py_XDECREF(str_repr);
                } else {
                  fprintf(stderr, "==> Какая-то дичь! Проверь-ка тип результата функции %s\n", hook_functions[i]);
                }
                Py_XDECREF(pValue);
              } else {
                fprintf(stderr, "WTF");
              }
              //else {
              //  Py_DECREF(pFunc);
              //  Py_DECREF(pModule);
              //  PyErr_Print();
              //  fprintf(stderr,"Call failed\n");
              //  return 1;
              //}
          }
          else {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", hook_functions[i]);
          }
          Py_XDECREF(pFunc);
          i++;
        }
        Py_XDECREF(pModule);
    }
    else {
        fprintf(stderr, "Failed to load \"%s\"\n", hook_file_path);
        PyErr_Print();
        return 1;
    }
    if (Py_FinalizeEx() < 0) {
        return 120;
    }
    return 0;
}
