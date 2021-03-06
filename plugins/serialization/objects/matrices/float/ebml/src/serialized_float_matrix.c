/************************************************************************************/
/* Copyright (c) 2009-2011 The Department of Arts and Culture,                      */
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
/* AUTHOR  : Aby Louw                                                               */
/* DATE    : December 2009                                                          */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* SSerializedObjectClass implementation for floating point matrix data             */
/* types in EBML format.                                                            */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Modules used                                                                     */
/*                                                                                  */
/************************************************************************************/

#include "serialized_float_matrix.h"


/************************************************************************************/
/*                                                                                  */
/* Typedefs                                                                         */
/*                                                                                  */
/************************************************************************************/

typedef SSerializedObject SEbmlMatrixFloat;

typedef SSerializedObjectClass SEbmlMatrixFloatClass;


/************************************************************************************/
/*                                                                                  */
/* Static variables                                                                 */
/*                                                                                  */
/************************************************************************************/

static SEbmlMatrixFloatClass EbmlMatrixFloatClass; /* SEbmlMatrixFloat class declaration. */

static const char *object_types[] =
{
	"SMatrixFloat",
	NULL
};


/************************************************************************************/
/*                                                                                  */
/* Plug-in class registration/free                                                  */
/*                                                                                  */
/************************************************************************************/

S_LOCAL void _s_serialized_ebml_matrix_float_reg(s_erc *error)
{
	S_CLR_ERR(error);

	s_class_reg(S_OBJECTCLASS(&EbmlMatrixFloatClass), error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "_s_serialized_ebml_matrix_float_reg",
				  "Failed to register SEbmlMatrixFloatClass"))
		return;

	SSerializedObjectRegister(&EbmlMatrixFloatClass, error);
	S_CHK_ERR(error, S_CONTERR,
			  "_s_serialized_ebml_matrix_float_reg",
			  "Failed to add serialized object class SEbmlMatrixFloatClass");
}


S_LOCAL void _s_serialized_ebml_matrix_float_free(s_erc *error)
{
	s_erc local_err;


	S_CLR_ERR(&local_err);
	S_CLR_ERR(error);

	SSerializedObjectFree(&EbmlMatrixFloatClass, &local_err);
	S_CHK_ERR(&local_err, S_CONTERR,
			  "_s_serialized_ebml_matrix_float_free",
			  "Failed to remove serialized object class SEbmlMatrixFloatClass");

	s_class_free(S_OBJECTCLASS(&EbmlMatrixFloatClass), error);
	S_CHK_ERR(error, S_CONTERR,
			  "_s_serialized_ebml_matrix_float_free",
			  "Failed to free SEbmlMatrixFloatClass");

	if ((local_err != S_SUCCESS)
		&& (error != NULL)
		&& (*error == S_SUCCESS))
		*error = local_err;
}


/************************************************************************************/
/*                                                                                  */
/* Static class function implementations                                            */
/*                                                                                  */
/************************************************************************************/

static void Dispose(void *obj, s_erc *error)
{
	S_CLR_ERR(error);
	SObjectDecRef(obj);
}


static void Read(SDataReader *reader, SObject *object, s_erc *error)
{
	SEbmlRead *ebmlReader;


	S_CLR_ERR(error);

	ebmlReader = S_CAST(reader, SEbmlRead, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Read",
				  "Call to \"S_CAST (SEbmlRead)\" failed for SDataReader object"))
		return;

	read_float_matrix(ebmlReader, object, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Read",
				  "Call to \"read_float_matrix\" failed"))
		return;
}


static void Write(SDataWriter *writer, const SObject *object, s_erc *error)
{
	SEbmlWrite *ebmlWriter;


	S_CLR_ERR(error);

	ebmlWriter = S_CAST(writer, SEbmlWrite, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Write",
				  "Call to \"S_CAST (SEbmlWrite)\" failed for SDataWriter object"))
		return;

	write_float_matrix(ebmlWriter, object, error);
	S_CHK_ERR(error, S_CONTERR,
			  "Write",
			  "Call to \"write_float_matrix\" failed");
}


/************************************************************************************/
/*                                                                                  */
/* SEbmlMatrixFloat class initialization                                            */
/*                                                                                  */
/************************************************************************************/

static SEbmlMatrixFloatClass EbmlMatrixFloatClass =
{
	/* SObjectClass */
	{
		"SSerializedObject:SEbmlMatrixFloat",
		sizeof(SEbmlMatrixFloat),
		{ 0, 1},
		NULL,                    /* init    */
		NULL,                    /* destroy */
		Dispose,                 /* dispose */
		NULL,                    /* compare */
		NULL,                    /* print   */
		NULL,                    /* copy    */
	},
	/* SSerializedObjectClass */
	"spct_ebml",               /* format       */
	object_types,              /* object_types */
	Read,                      /* read         */
	Write                      /* write        */
};


