/************************************************************************************/
/* Copyright (c) 2008-2011 The Department of Arts and Culture,                      */
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
/* DATE    : 1 November 2008                                                        */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* SObject functions for four primitive data types: ints, floats, strings and void* */
/*                                                                                  */
/* Loosely based on cst_val of Flite, http://cmuflite.org (1.3)                     */
/* Note that this is a derived work with no verbatim source code from above         */
/* mentioned project.                                                               */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*  Flite license, cst_val                                                          */
/*                                                                                  */
/*                       Language Technologies Institute                            */
/*                          Carnegie Mellon University                              */
/*                             Copyright (c) 1999                                   */
/*                             All Rights Reserved.                                 */
/*                                                                                  */
/*       Permission is hereby granted, free of charge, to use and distribute        */
/*       this software and its documentation without restriction, including         */
/*       without limitation the rights to use, copy, modify, merge, publish,        */
/*       distribute, sublicense, and/or sell copies of this work, and to            */
/*       permit persons to whom this work is furnished to do so, subject to         */
/*       the following conditions:                                                  */
/*        1. The code must retain the above copyright notice, this list of          */
/*           conditions and the following disclaimer.                               */
/*        2. Any modifications must be clearly marked as such.                      */
/*        3. Original authors' names are not deleted.                               */
/*        4. The authors' names are not used to endorse or promote products         */
/*           derived from this software without specific prior written              */
/*           permission.                                                            */
/*                                                                                  */
/*       CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK               */
/*       DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING            */
/*       ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT         */
/*       SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE            */
/*       FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES          */
/*       WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN         */
/*       AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,                */
/*       ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF             */
/*       THIS SOFTWARE.                                                             */
/*                                                                                  */
/*       Author:  Alan W Black                                                      */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Modules used                                                                     */
/*                                                                                  */
/************************************************************************************/

#include "base/utils/smath.h"
#include "base/utils/alloc.h"
#include "base/strings/strings.h"
#include "base/strings/sprint.h"
#include "base/objsystem/class.h"
#include "base/objsystem/object.h"
#include "base/objsystem/object_macros.h"
#include "base/objsystem/primitives.h"
#include "serialization/serialized_object.h"


/************************************************************************************/
/*                                                                                  */
/* SInt definition (signed)                                                         */
/*                                                                                  */
/************************************************************************************/

/*
 * Definition of the integer class (signed). This class adds no class methods to the
 * #SObjectClass and is therefore exactly the same.
 */
typedef SObjectClass SIntClass;


/*
 * The integer object structure.
 */
typedef struct
{
	SObject     obj;       /*!< Inherit from SObject.      */
	sint32      i;         /*!< The integer data.          */
} SInt;


/************************************************************************************/
/*                                                                                  */
/* SFloat definition                                                                */
/*                                                                                  */
/************************************************************************************/

/*
 * Definition of the float class. This class adds no class methods to the
 * #SObjectClass and is therefore exactly the same.
 */
typedef SObjectClass SFloatClass;


/*
 * The float object structure.
 */
typedef struct
{
	SObject     obj;       /*!< Inherit from SObject.      */
	float       f;         /*!< The float data.            */
} SFloat;


/************************************************************************************/
/*                                                                                  */
/* SString definition                                                               */
/*                                                                                  */
/************************************************************************************/

/*
 * Definition of the string class. This class adds no class methods to the
 * #SObjectClass and is therefore exactly the same.
 */
typedef SObjectClass SStringClass;


/*
 * The string object structure.
 */
typedef struct
{
	SObject     obj;       /*!< Inherit from SObject.      */
	char       *s;         /*!< The string data.           */
} SString;


/************************************************************************************/
/*                                                                                  */
/* SVoid definition                                                                 */
/*                                                                                  */
/************************************************************************************/

/*
 * Definition of the void class. This class adds no class methods to the
 * #SObjectClass and is therefore exactly the same.
 */
typedef SObjectClass SVoidClass;


/*
 * The void object structure.
 * SVoid is used to encapsulate simple structures in a SObject
 * type, without the need to define a new object.
 */
typedef struct
{
	SObject         obj;        /*!< Inherit from SObject.                     */
	void           *ptr;        /*!< Pointer to the void data.                 */
	s_svoid_free_fp free_func;  /*!< Callback function to free void data.      */
	char           *type_name;  /*!< An identifier for this void object type.  */
} SVoid;


/************************************************************************************/
/*                                                                                  */
/* Static variables                                                                 */
/*                                                                                  */
/************************************************************************************/

static SIntClass        IntClass;
static SFloatClass      FloatClass;
static SStringClass     StringClass;
static SVoidClass       VoidClass;


/************************************************************************************/
/*                                                                                  */
/* Function implementations                                                         */
/*                                                                                  */
/************************************************************************************/


/****  SInt ****/

S_API SObject *SObjectSetInt(sint32 i, s_erc *error)
{
	SInt *self;


	S_CLR_ERR(error);

	self = S_NEW(SInt, error);
	if (S_CHK_ERR(error, S_FAILURE,
				  "SObjectSetInt",
				  "Failed to create new SInt object"))
		return NULL;

	self->i = i;

	return S_OBJECT(self);
}


S_API void SObjectResetInt(SObject *self, sint32 i, s_erc *error)
{
	s_bool type_is_good;


	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "SObjectResetInt",
				  "Argument \"self\" is NULL");
		return;
	}

	type_is_good = SObjectIsType(self, "SInt", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "SObjectResetInt",
				  "Call to \"SObjectIsType\" failed"))
		return;

	if (type_is_good == FALSE)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "SObjectResetInt",
				  "Given object \"self\" is not of type SInt");
		return;
	}

	((SInt*)self)->i = i;
}


S_API sint32 SObjectGetInt(const SObject *self, s_erc *error)
{
	s_bool type_is_good;


	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "SObjectGetInt",
				  "Argument \"self\" is NULL");
		return 0;
	}

	type_is_good = SObjectIsType(self, "SInt", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "SObjectGetInt",
				  "Call to \"SObjectIsType\" failed"))
		return 0;

	if (type_is_good == FALSE)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "SObjectGetInt",
				  "Given object \"self\" is not of type SInt");
		return 0;
	}

	return ((SInt*)self)->i;
}


/****  SFloat ****/

S_API SObject *SObjectSetFloat(float f, s_erc *error)
{
	SFloat *self;


	S_CLR_ERR(error);

	self = S_NEW(SFloat, error);
	if (S_CHK_ERR(error, S_FAILURE,
				  "SObjectSetFloat",
				  "Failed to create new SFloat object"))
		return NULL;

	self->f = f;

	return S_OBJECT(self);
}


S_API void SObjectResetFloat(SObject *self, float f, s_erc *error)
{
	s_bool type_is_good;


	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "SObjectResetFloat",
				  "Argument \"self\" is NULL");
		return;
	}

	type_is_good = SObjectIsType(self, "SFloat", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "SObjectResetFloat",
				  "Call to \"SObjectIsType\" failed"))
		return;

	if (type_is_good == FALSE)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "SObjectResetFloat",
				  "Given object \"self\" is not of type SFloat");
		return;
	}

	((SFloat*)self)->f = f;
}


S_API float SObjectGetFloat(const SObject *self, s_erc *error)
{
	s_bool type_is_good;


	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "SObjectGetFloat",
				  "Argument \"self\" is NULL");
		return 0.0;
	}

	type_is_good = SObjectIsType(self, "SFloat", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "SObjectGetFloat",
				  "Call to \"SObjectIsType\" failed"))
		return 0.0;

	if (type_is_good == FALSE)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "SObjectGetFloat",
				  "Given object \"self\" is not of type SFloat");
		return 0.0;
	}

	return ((SFloat*)self)->f;
}


/****  SString ****/

S_API SObject *SObjectSetString(const char *s, s_erc *error)
{
	SString *self;


	S_CLR_ERR(error);

	if (s == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "SObjectSetString",
				  "Argument \"s\" is NULL");
		return NULL;
	}

	self = S_NEW(SString, error);
	if (S_CHK_ERR(error, S_FAILURE,
				  "SObjectSetString",
				  "Failed to create new SString object"))
		return NULL;

	self->s = s_strdup(s, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "SObjectSetString",
				  "Failed to copy string"))
	{
		S_DELETE(self, "SObjectSetString", error);
		return NULL;
	}

	return S_OBJECT(self);
}


S_API void SObjectResetString(SObject *self, const char *s, s_erc *error)
{
	char *tmp;
	s_bool type_is_good;


	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "SObjectResetString",
				  "Argument \"self\" is NULL");
		return;
	}

	type_is_good = SObjectIsType(self, "SString", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "SObjectResetString",
				  "Call to \"SObjectIsType\" failed"))
		return;

	if (type_is_good == FALSE)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "SObjectResetString",
				  "Given object \"self\" is not of type SString");
		return;
	}

	tmp = ((SString*)self)->s;
	S_FREE(tmp); /* checks !NULL */

	if (s != NULL)
	{
		((SString*)self)->s = s_strdup(s, error);
		S_CHK_ERR(error, S_CONTERR,
				  "SObjectResetString",
				  "Failed to copy new string");
	}
	else
	{
		((SString*)self)->s = NULL;
	}
}


S_API const char *SObjectGetString(const SObject *self, s_erc *error)
{
	s_bool type_is_good;


	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "SObjectGetString",
				  "Argument \"self\" is NULL");
		return NULL;
	}

	type_is_good = SObjectIsType(self, "SString", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "SObjectGetString",
				  "Call to \"SObjectIsType\" failed"))
		return NULL;

	if (type_is_good == FALSE)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "SObjectGetString",
				  "Given object not of type SString");
		return NULL;
	}

	return ((SString*)self)->s;
}


/****  SVoid ****/

S_API SObject *SObjectSetVoid(void *ptr,
							  const char *type_name,
							  s_svoid_free_fp free_func,
							  s_erc *error)
{
	SVoid *self;


	S_CLR_ERR(error);

	if (ptr == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "SObjectSetVoid",
				  "Argument \"ptr\" is NULL");
		return NULL;
	}

	if (type_name == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "SObjectSetVoid",
				  "Argument \"type_name\" is NULL");
		return NULL;
	}


	self = S_NEW(SVoid, error);
	if (S_CHK_ERR(error, S_FAILURE,
				  "SObjectSetVoid",
				  "Failed to create new 'SVoid' object"))
		return NULL;

	self->ptr = ptr;
	self->free_func = free_func;
	self->type_name = s_strdup(type_name, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "SObjectSetVoid",
				  "Failed to copy type name"))
	{
		S_DELETE(self, "SObjectSetVoid", error);
		return NULL;
	}

	return S_OBJECT(self);
}


S_API void SObjectResetVoid(SObject *self, void *ptr,
							const char *type_name,
							s_svoid_free_fp free_func,
							s_erc *error)
{
	s_bool type_is_good;


	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "SObjectResetVoid",
				  "Argument \"self\" is NULL");
		return;
	}

	if (ptr == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "SObjectResetVoid",
				  "Argument \"ptr\" is NULL");
		return;
	}

	if (type_name == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "SObjectResetVoid",
				  "Argument \"type_name\" is NULL");
		return;
	}

	type_is_good = SObjectIsType(self, "SVoid", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "SObjectResetVoid",
				  "Call to \"SObjectIsType\" failed"))
		return;

	if (type_is_good == FALSE)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "SObjectResetVoid",
				  "Given object \"self\" is not of type SVoid");
		return;
	}

	if (((SVoid*)self)->free_func != NULL)
	{
		((SVoid*)self)->free_func(((SVoid*)self)->ptr, error);
		if (S_CHK_ERR(error, S_FAILURE,
					  "SObjectResetVoid",
					  "Call to SVoid object free function failed"))
			return;
	}

	S_FREE(((SVoid*)self)->type_name);
	((SVoid*)self)->ptr = ptr;
	((SVoid*)self)->free_func = free_func;
	((SVoid*)self)->type_name = s_strdup(type_name, error);
	S_CHK_ERR(error, S_CONTERR,
			  "SObjectResetVoid",
			  "Failed to copy type name");
}


S_API const void *SObjectGetVoid(const SObject *self,
								 const char *type_name,
								 s_erc *error)
{
	s_bool type_is_good;
	int rv;


	S_CLR_ERR(error);

	if (self == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
				  "SObjectGetVoid",
				  "Argument \"self\" is NULL");
		return NULL;
	}

	type_is_good = SObjectIsType(self, "SVoid", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "SObjectGetVoid",
				  "Call to \"SObjectIsType\" failed"))
		return NULL;

	if (type_is_good == FALSE)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "SObjectGetVoid",
				  "Given object not of type SVoid");
		return NULL;
	}

	rv = s_strcmp(((SVoid*)self)->type_name, type_name, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "SObjectGetVoid",
				  "Call to \"s_strcmp\" failed"))
		return NULL;

	if (rv != 0)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "SObjectGetVoid",
				  "Type mismatch, object is of type \'%s\' and not \'%s\'",
				  ((SVoid*)self)->type_name, type_name);
		return NULL;
	}

	return ((SVoid*)self)->ptr;
}


/************************************************************************************/
/*                                                                                  */
/* Class registration                                                               */
/*                                                                                  */
/************************************************************************************/

S_LOCAL void _s_primitive_class_add(s_erc *error)
{
	S_CLR_ERR(error);

	s_class_add(S_OBJECTCLASS(&IntClass), error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "_s_primitive_class_add",
				  "Failed to add SIntClass"))
		return;

	s_class_add(S_OBJECTCLASS(&FloatClass), error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "_s_primitive_class_add",
				  "Failed to add SFloatClass"))
		return;

	s_class_add(S_OBJECTCLASS(&StringClass), error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "_s_primitive_class_add",
				  "Failed to add SStringClass"))
		return;

	s_class_add(S_OBJECTCLASS(&VoidClass), error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "_s_primitive_class_add",
				  "Failed to add SVoidClass"))
		return;
}


/************************************************************************************/
/*                                                                                  */
/* Static class function implementations                                            */
/*                                                                                  */
/************************************************************************************/


/****  SInt ****/

static void DisposeInt(void *obj, s_erc *error)
{
	S_CLR_ERR(error);
	SObjectDecRef(obj);
}


static s_bool CompareInt(const SObject *Sv1, const SObject *Sv2, s_erc *error)
{
	sint32 ss1;
	sint32 ss2;


	S_CLR_ERR(error);

	/*
	 * we can do unsafe cast as to get here these
	 * objects must be SInt
	 */
	ss1 = ((SInt*)Sv1)->i;
	ss2 = ((SInt*)Sv2)->i;

	if (ss1 != ss2)
		return FALSE;

	return TRUE;
}


static char *PrintInt(const SObject *self, s_erc *error)
{
	const char *type = "[SInt] %d";
	sint32 i;
	char *buf;


	S_CLR_ERR(error);
	i = ((SInt*)self)->i;
	s_asprintf(&buf, error, type, i);
	if (S_CHK_ERR(error, S_FAILURE,
				  "PrintInt",
				  "Call to \"s_asprintf\" failed"))
	{
		if (buf != NULL)
			S_FREE(buf);
		return NULL;
	}

	return buf;
}


static SObject *CopyInt(const SObject *self, s_erc *error)
{
	SObject *newInt;
	sint32 i;


	S_CLR_ERR(error);
	i = ((SInt*)self)->i;
	newInt = SObjectSetInt(i, error);
	if (S_CHK_ERR(error, S_FAILURE,
				  "CopyInt",
				  "Call to \"SObjectSetInt\" failed"))
	{
		if (newInt != NULL)
			S_DELETE(newInt, "CopyInt", error);
		return NULL;
	}

	return newInt;
}


/****  SFloat ****/

static void DisposeFloat(void *obj, s_erc *error)
{
	S_CLR_ERR(error);
	SObjectDecRef(obj);
}


static s_bool CompareFloat(const SObject *Sv1, const SObject *Sv2, s_erc *error)
{
	float f1;
	float f2;


	S_CLR_ERR(error);

	f1 = ((SFloat*)Sv1)->f;
	f2 = ((SFloat*)Sv2)->f;

	return s_float_equal(f1,f2);
}


static char *PrintFloat(const SObject *self, s_erc *error)
{
	const char *type = "[SFloat] %f";
	float f;
	char *buf;


	S_CLR_ERR(error);
	f = ((SFloat*)self)->f;
	s_asprintf(&buf, error, type, f);
	if (S_CHK_ERR(error, S_FAILURE,
				  "PrintFloat",
				  "Call to \"s_asprintf\" failed"))
	{
		if (buf != NULL)
			S_FREE(buf);
		return NULL;
	}

	return buf;
}


static SObject *CopyFloat(const SObject *self, s_erc *error)
{
	SObject *newFloat;
	float f;


	S_CLR_ERR(error);
	f = ((SFloat*)self)->f;
	newFloat = SObjectSetFloat(f, error);
	if (S_CHK_ERR(error, S_FAILURE,
				  "CopyFloat",
				  "Call to \"SObjectSetFloat\" failed"))
	{
		if (newFloat != NULL)
			S_DELETE(newFloat, "CopyFloat", error);
		return NULL;
	}

	return newFloat;
}


/****  SString ****/

static void DisposeString(void *obj, s_erc *error)
{
	S_CLR_ERR(error);
	SObjectDecRef(obj);
}


static void InitString(void *obj, s_erc *error)
{
	SString *self = obj;


	S_CLR_ERR(error);
	self->s = NULL;
}


static void DestroyString(void *obj, s_erc *error)
{
	SString *self = obj;


	S_CLR_ERR(error);
	S_FREE(self->s);
}


/*
 * named "CompareStrings" because win32 makes it "CompareStringA"
 * which clashes with a winnls.h function.
 */
static s_bool CompareStrings(const SObject *Sv1, const SObject *Sv2, s_erc *error)
{
	const char *s1;
	const char *s2;
	int rv;


	S_CLR_ERR(error);

	s1 = ((SString*)Sv1)->s;
	s2 = ((SString*)Sv2)->s;

	rv = s_strcmp(s1, s2, error);
	if (S_CHK_ERR(error, S_FAILURE,
				  "CompareString",
				  "Call to \"s_strcmp\" failed"))
		return FALSE;

	if (rv != 0)
		return FALSE;

	return TRUE;
}


static char *PrintString(const SObject *self, s_erc *error)
{
	const char *type = "[SString] \"%s\"";
	const char *s;
	char *buf;


	S_CLR_ERR(error);
	s = ((SString*)self)->s;
	s_asprintf(&buf, error, type, s);
	if (S_CHK_ERR(error, S_FAILURE,
				  "PrintString",
				  "Call to \"s_asprintf\" failed"))
	{
		if (buf != NULL)
			S_FREE(buf);
		return NULL;
	}

	return buf;
}


static SObject *CopyString(const SObject *self, s_erc *error)
{
	SObject *newString;
	const char *s;


	S_CLR_ERR(error);
	s = ((SString*)self)->s;
	newString = SObjectSetString(s, error);
	if (S_CHK_ERR(error, S_FAILURE,
				  "CopyString",
				  "Call to \"SObjectSetString\" failed"))
	{
		if (newString != NULL)
			S_DELETE(newString, "CopyString", error);
		return NULL;
	}

	return newString;
}


/****  SVoid ****/

static void DisposeVoid(void *obj, s_erc *error)
{
	S_CLR_ERR(error);
	SObjectDecRef(obj);
}


static void InitVoid(void *obj, s_erc *error)
{
	SVoid *self = obj;


	S_CLR_ERR(error);
	self->ptr = NULL;
	self->free_func = NULL;
	self->type_name = NULL;
}


static void DestroyVoid(void *obj, s_erc *error)
{
	SVoid *self = obj;


	S_CLR_ERR(error);

	if (self->free_func != NULL)
		self->free_func(self->ptr, error);

	S_CHK_ERR(error, S_FAILURE,
			  "DestroyVoid",
			  "Call to SVoid object free function failed");

	if (self->type_name != NULL)
		S_FREE(self->type_name);
}


static s_bool CompareVoid(const SObject *Sv1, const SObject *Sv2, s_erc *error)
{
	const SVoid *s1;
	const SVoid *s2;
	int rv;


	S_CLR_ERR(error);

	/* type is checked by SObjectCompare */
	s1 = (SVoid*)Sv1;
	s2 = (SVoid*)Sv2;

	/* check type_name */
	rv = s_strcmp(s1->type_name, s2->type_name, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "CompareVoid",
				  "Call to \"s_strcmp\" failed"))
		return FALSE;

	if (rv != 0)
		return FALSE;

	if (s1->ptr != s2->ptr)
		return FALSE;

	return TRUE;
}


static char *PrintVoid(const SObject *self, s_erc *error)
{
	const char *type = "[SVoid] %p";
	char *buf;


	S_CLR_ERR(error);

	s_asprintf(&buf, error, type, ((SVoid*)self)->ptr);
	if (S_CHK_ERR(error, S_CONTERR,
				  "PrintVoid",
				  "Call to \"s_asprintf\" failed"))
	{
		if (buf != NULL)
			S_FREE(buf);
		return NULL;
	}

	return buf;
}


/************************************************************************************/
/*                                                                                  */
/* SInt, SFloat, SString and SVoid class initialization                             */
/*                                                                                  */
/************************************************************************************/

/****  SInt ****/

static SIntClass IntClass =
{
	/* SObjectClass */
	"SInt",
	sizeof(SInt),
	{ 0, 1},
	NULL,       /* init    */
	NULL,       /* destroy */
	DisposeInt, /* dispose */
	CompareInt, /* compare */
	PrintInt,   /* print   */
	CopyInt     /* copy    */
};


/****  SFloat ****/

static SFloatClass FloatClass =
{
	/* SObjectClass */
	"SFloat",
	sizeof(SFloat),
	{ 0, 1},
	NULL,         /* init    */
	NULL,         /* destroy */
	DisposeFloat, /* dispose */
	CompareFloat, /* compare */
	PrintFloat,   /* print   */
	CopyFloat     /* copy    */
};


/****  SString ****/

static SStringClass StringClass =
{
	/* SObjectClass */
	"SString",
	sizeof(SString),
	{ 0, 1},
	InitString,     /* init    */
	DestroyString,  /* destroy */
	DisposeString,  /* dispose */
	CompareStrings, /* compare */
	PrintString,    /* print   */
	CopyString      /* copy    */
};


/****  SVoid ****/

static SVoidClass VoidClass =
{
	/* SObjectClass */
	"SVoid",
	sizeof(SVoid),
	{ 0, 1},
	InitVoid,    /* init    */
	DestroyVoid, /* destroy */
	DisposeVoid, /* dispose */
	CompareVoid, /* compare */
	PrintVoid,   /* print   */
	NULL         /* copy    */
};

