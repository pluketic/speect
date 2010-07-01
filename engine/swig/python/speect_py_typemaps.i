/************************************************************************************/
/* Copyright (c) 2009 The Department of Arts and Culture,                           */
/* The Government of the Republic of South Africa.                                  */
/*                                                                                  */
/* Contributors:  Meraka Institute, CSIR, South Africa.                             */
/*                                                                                  */
/* Permission is hereby granted, free of charge, to any person obtaining a copy     */
/* of this software and associated documentation files (the "Software"), to deal    */
/* in the Software without restriction, including without limitation the rights     */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        */
/* copies of the Software, and to permit persons to whom the Software is            */
/* furnished to do so, subject to the following conditions:                         */
/* The above copyright notice and this permission notice shall be included in       */
/* all copies or substantial portions of the Software.                              */
/*                                                                                  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR       */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,         */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE      */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER           */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,    */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN        */
/* THE SOFTWARE.                                                                    */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* AUTHOR  : Richard Carlson, Aby Louw                                              */
/* DATE    : March 2009                                                             */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* Typemaps to convert types between Python and C, for the Speect Engine.           */
/*                                                                                  */
/*                                                                                  */
/************************************************************************************/

/*** sint32 ***/
%typemap(in) sint32
{
	$1 = (sint32) PyLong_AsLong($input);
}

%typemap(out) sint32
{
	$result = PyLong_FromLong((long) $1);
}


/*** uint32 ***/
%typemap(in) uint32
{
	$1 = (uint32) PyLong_AsUnsignedLong($input);
}

%typemap(out) uint32
{
	$result = PyLong_FromLong((ulong) $1);
}


/*** s_bool ***/
%typemap(in) s_bool
{
	if ($input == Py_False)
		$1 = 0;
	else
		$1 = 1;
}

%typemap(out) s_bool
{
	if ($1 == 0)
		$result = Py_False;
	else
		$result = Py_True;

	Py_XINCREF($result);
}


/*** version ***/
%typemap(out) s_version*
{
	PyObject *versionTuple;
	PyObject *versionMajor;
	PyObject *versionMinor;
	s_version *output = $1;


	versionTuple = PyTuple_New(2);
	versionMajor = PyLong_FromLong((long)output->major);
	versionMinor = PyLong_FromLong((long)output->minor);

	PyTuple_SetItem(versionTuple, 0, versionMajor);
	PyTuple_SetItem(versionTuple, 1, versionMinor);

	$result = versionTuple;
}

/*** Speect Engine library version ***/
%typemap(out) s_lib_version
{
	PyObject *versionTuple;
	PyObject *versionMajor;
	PyObject *versionMinor;
	PyObject *versionPatchLevel;
	PyObject *versionRelease;
	s_lib_version output = $1;
	s_erc local_err = S_SUCCESS;


	versionTuple = PyTuple_New(4);
	versionMajor = PyLong_FromLong((long)output.major);
	versionMinor = PyLong_FromLong((long)output.minor);
	versionPatchLevel = PyLong_FromLong((long)output.patch);
	versionRelease = s_set_pyobject_str(output.release, &local_err);


	PyTuple_SetItem(versionTuple, 0, versionMajor);
	PyTuple_SetItem(versionTuple, 1, versionMinor);
	PyTuple_SetItem(versionTuple, 2, versionPatchLevel);
	PyTuple_SetItem(versionTuple, 3, versionRelease);

	$result = versionTuple;
}


%typemap(in) SList*
{
	SListPy *pyList = NULL;
	SList *list;
	s_erc error;


	S_CLR_ERR(&error);
	if (PyList_Check($input))
	{
		/* create SListPy wrapper */
		pyList = (SListPy*)S_NEW("SListPy", &error);
		if (S_CHK_ERR(&error, S_CONTERR,
					  "%typemap(in) const SList* uttType",
					  "Failed to create new 'SListPy' object"))
			goto slist_fail;

		/* initialize with Python list */
		SListPyInit(&pyList, $input, &error);
		if (S_CHK_ERR(&error, S_CONTERR,
					  "%typemap(in) const SList* uttType",
					  "Call to \"SListPyInit\" failed"))
			goto slist_fail;

		/* we are finished with the Python list, it is
		 * in the wrapper now.
		 */
		Py_CLEAR($input);
	}
	else
	{
		PyErr_SetString(PyExc_TypeError,"input is not a list");
		goto slist_fail;
	}

	goto slist_good;


slist_fail:
	list = NULL;
slist_good:
	list = (SList*)pyList;
	$1 = list;
}


%typemap(out) const SList*
{
	s_erc error;
	PyObject *pyList;


	S_CLR_ERR(&error);
	pyList = s_sobject_2_pyobject(S_OBJECT($1), FALSE, &error);
	if (S_CHK_ERR(&error, S_CONTERR,
				  "%typemap(out) const SList*",
				  "Call to \"s_sobject_2_pyobject\" failed"))
		$result = NULL;
	else
		$result = pyList;
}


%typemap(out) SList*
{
	s_erc error;
	PyObject *pyList;


	S_CLR_ERR(&error);
	pyList = s_sobject_2_pyobject(S_OBJECT($1), TRUE, &error);
	if (S_CHK_ERR(&error, S_CONTERR,
				  "%typemap(out) SList*",
				  "Call to \"s_sobject_2_pyobject\" failed"))
		$result = NULL;
	else
		$result = pyList;
}
