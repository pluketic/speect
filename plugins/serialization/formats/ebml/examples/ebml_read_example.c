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
/* DATE    : 18 November 2009                                                       */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* Simple EBML reading example.                                                     */
/*                                                                                  */
/*                                                                                  */
/************************************************************************************/

#include <stdio.h>
#include "speect.h"
#include "ebml_read.h"


int main()
{
	s_erc error;
	SDatasource *ds = NULL;
	SPlugin *plugin = NULL;
	const char *doc_type = "spct_ebml_test";
	SEbmlRead *ebmlReader = NULL;
	int rv;


	S_CLR_ERR(&error);

	/*
	 * initialize speect
	 */
	error = speect_init(NULL);
	if (error != S_SUCCESS)
	{
		printf("Failed to initialize Speect\n");
		return 1;
	}

	/*
	 * load the ebml plug-in
	 */
	plugin = s_pm_load_plugin("ebml.spi", &error);
	if (S_CHK_ERR(&error, S_CONTERR,
				  "main",
				  "Failed to load plug-in at 'ebml.spi'"))
	{
		printf("failed to load plug-in\n");
		goto quit;
	}
	else
	{
		printf("plug-in loaded\n");
	}


	/*
	 * create a data source, "rb" = read, binary
	 */
	ds = SFilesourceOpenFile("ebml_test.bin", "rb", &error);
	if (S_CHK_ERR(&error, S_CONTERR,
				  "main",
				  "Failed to create data source 'ebml_test.bin'"))
	{
		printf("failed to create data source\n");
		goto quit;
	}
	else
	{
		printf("data source created\n");
	}

	/*
	 * create ebml reader object
	 */
	ebmlReader = S_NEW(SEbmlRead, &error);
	if (S_CHK_ERR(&error, S_CONTERR,
				  "main",
				  "Failed to create new SEbmlRead object"))
	{
		printf("failed to create SEbmlRead object\n");
		goto quit;
	}
	else
	{
		printf("created SEbmlRead object\n");
	}

	/*
	 * initialize ebml reader object
	 */
	if (S_EBMLREAD_METH_VALID(ebmlReader, read_init))
	{
		S_EBMLREAD_CALL(ebmlReader, read_init)(&ebmlReader, ds, &error);
		if (S_CHK_ERR(&error, S_CONTERR,
					  "main",
					  "Failed to initialize SEbmlRead object"))
		{
			printf("failed to initialize SEbmlRead object\n");
			goto quit;
		}
		else
		{
			printf("initialized SEbmlRead object\n");

			/*
			 * ebml reader takes hold of the data source, we dont want
			 * a reference to it anymore (for quit to work).
			 */
			ds = NULL;
		}
	}
	else
	{
		S_CTX_ERR(&error, S_METHINVLD,
				  "main",
				  "EbmlRead method \"read_init\" not implemented");
		printf("cannot do read_init\n");
		goto quit;
	}

	/*
	 * verify doc_type
	 */
	rv = s_strcmp(ebmlReader->header->doctype, doc_type, &error);
	if (S_CHK_ERR(&error, S_CONTERR,
				  "main",
				  "Failed to compare doc_type's"))
	{
		printf("failed to compare document types\n");
		goto quit;
	}

	if (rv != 0)
	{
		printf("document types differ\n");
		goto quit;
	}
	else
	{
		printf("document types are good\n");
	}

	/*
	 * Open container
	 */
	if (S_EBMLREAD_METH_VALID(ebmlReader, container))
	{
		uint32 id;

		id = S_EBMLREAD_CALL(ebmlReader, container)(ebmlReader, &error);
		if (S_CHK_ERR(&error, S_CONTERR,
					  "main",
					  "Failed to open ebml container"))
		{
			printf("failed to open container\n");
			goto quit;
		}
		else
		{
			printf("container opened with id %d, should be %d\n", id, 129);
		}
	}


	/*
	 * Read a few unsigned int values
	 */
	if (S_EBMLREAD_METH_VALID(ebmlReader, read_uint))
	{
		uint32 val;
		uint32 id;

		val = S_EBMLREAD_CALL(ebmlReader, read_uint)(ebmlReader, &id, &error);
		if (S_CHK_ERR(&error, S_CONTERR,
					  "main",
					  "Failed to read uint [%d]", 234))
		{
			printf("failed to read uint [%d]\n", 234);
			goto quit;
		}
		else
		{
			printf("read uint [%d] with id %d (should be %d and id %d)\n",
				   val, id, 234, 130);
		}

		val = S_EBMLREAD_CALL(ebmlReader, read_uint)(ebmlReader, &id, &error);
		if (S_CHK_ERR(&error, S_CONTERR,
					  "main",
					  "Failed to read uint [%d]", 31234))
		{
			printf("failed to read uint [%d]\n", 31234);
			goto quit;
		}
		else
		{
			printf("read uint [%d] with id %d (should be %d and id %d)\n",
				   val, id, 31234, 130);
		}
	}

	/*
	 * Read a few signed int values
	 */
	if (S_EBMLREAD_METH_VALID(ebmlReader, read_sint))
	{
		sint32 val;
		uint32 id;

		val = S_EBMLREAD_CALL(ebmlReader, read_sint)(ebmlReader, &id, &error);
		if (S_CHK_ERR(&error, S_CONTERR,
					  "main",
					  "Failed to read sint [%d]", -69))
		{
			printf("failed to read sint [%d]\n", -69);
			goto quit;
		}
		else
		{
			printf("read sint [%d] with id %d (should be %d, and id %d)\n",
				   val, id, -69, 131);
		}

		val = S_EBMLREAD_CALL(ebmlReader, read_sint)(ebmlReader, &id, &error);
		if (S_CHK_ERR(&error, S_CONTERR,
					  "main",
					  "Failed to read sint [%d]", -132769))
		{
			printf("failed to read sint [%d]\n", -132769);
			goto quit;
		}
		else
		{
			printf("read sint [%d] with id %d (should be %d, and id %d)\n",
				   val, id, -132769, 131);
		}
	}

	/*
	 * Open container
	 */
	if (S_EBMLREAD_METH_VALID(ebmlReader, container))
	{
		uint32 id;

		id = S_EBMLREAD_CALL(ebmlReader, container)(ebmlReader, &error);
		if (S_CHK_ERR(&error, S_CONTERR,
					  "main",
					  "Failed to open ebml container"))
		{
			printf("failed to open container\n");
			goto quit;
		}
		else
		{
			printf("container opened with id %d, should be %d\n", id, 132);
		}
	}

	/*
	 * Read some floats
	 */
	if (S_EBMLREAD_METH_VALID(ebmlReader, read_float))
	{
		float val;
		uint32 id;

		val = S_EBMLREAD_CALL(ebmlReader, read_float)(ebmlReader, &id, &error);
		if (S_CHK_ERR(&error, S_CONTERR,
					  "main",
					  "Failed to read float [%f]", 20.343))
		{
			printf("failed to read float [%f]\n", 20.343);
			goto quit;
		}
		else
		{
			printf("read float [%f] with id %d (should be %f and id %d)\n",
				   val, id, 20.343, 133);
		}

		val = S_EBMLREAD_CALL(ebmlReader, read_float)(ebmlReader, &id, &error);
		if (S_CHK_ERR(&error, S_CONTERR,
					  "main",
					  "Failed to read float [%f]", -12320.7603))
		{
			printf("failed to read float [%f]\n", -12320.7603);
			goto quit;
		}
		else
		{
			printf("read float [%f] with id %d (should be %f and id %d)\n",
				   val, id, -12320.7603, 133);
		}
	}

	/*
	 * Read some doubles
	 */
	if (S_EBMLREAD_METH_VALID(ebmlReader, read_double))
	{
		double val;
		uint32 id;

		val = S_EBMLREAD_CALL(ebmlReader, read_double)(ebmlReader, &id, &error);
		if (S_CHK_ERR(&error, S_CONTERR,
					  "main",
					  "Failed to read double [%f]", -3.145343234223321))
		{
			printf("failed to read double [%f]\n", -3.145343234223321);
			goto quit;
		}
		else
		{
			printf("read double [%f] with id %d (should be %f and id %d)\n",
				   val, id, -3.145343234223321, 134);
		}

		val = S_EBMLREAD_CALL(ebmlReader, read_double)(ebmlReader, &id, &error);
		if (S_CHK_ERR(&error, S_CONTERR,
					  "main",
					  "Failed to read double [%f]", 533.145343234223321145343234223321))
		{
			printf("failed to read double [%f]\n", 533.145343234223321145343234223321);
			goto quit;
		}
		else
		{
			printf("read double [%f] with id %d (should be %f and id %d)\n",
				   val, id, 533.145343234223321145343234223321, 134);
		}
	}

	/*
	 * Check that container is at end
	 */
	if (S_EBMLREAD_METH_VALID(ebmlReader, container_at_end))
	{
		s_bool at_end;

		at_end = S_EBMLREAD_CALL(ebmlReader, container_at_end)(ebmlReader, &error);
		if (S_CHK_ERR(&error, S_CONTERR,
					  "main",
					  "Failed to check if container is at end"))
		{
			printf("failed to check if container is at end\n");
			goto quit;
		}

		if (at_end)
		{
			printf("found container at end (correct)\n");
		}
		else
		{
			printf("container not at end, error\n");
			goto quit;
		}
	}


	/*
	 * Read some ascii
	 */
	if (S_EBMLREAD_METH_VALID(ebmlReader, read_ascii))
	{
		char *val;
		uint32 id;

		val = S_EBMLREAD_CALL(ebmlReader, read_ascii)(ebmlReader, &id, &error);
		if (S_CHK_ERR(&error, S_CONTERR,
					  "main",
					  "Failed to read ascii [%s]", "hello world"))
		{
			printf("Failed to read ascii [%s]\n", "hello world");
			goto quit;
		}
		else
		{
			printf("read ascii [%s] with id %d (should be 'hello world' and id %d)\n",
				   val, id, 135);
			S_FREE(val);
		}
	}


	/*
	 * Read some utf8
	 */
	if (S_EBMLREAD_METH_VALID(ebmlReader, read_utf8))
	{
		char *val;
		uint32 id;

		val = S_EBMLREAD_CALL(ebmlReader, read_utf8)(ebmlReader, &id, &error);
		if (S_CHK_ERR(&error, S_CONTERR,
					  "main",
					  "Failed to read utf8 [%s]", "hello world in Japanese"))
		{
			printf("Failed to read utf8 [%s]", "hello world in Japanese");
			goto quit;
		}
		else
		{
			printf("read utf8 [%s] with id %d (should be 'hello world in Japanese: ハロー世界' and id %d)\n",
				   val, id, 136);
			S_FREE(val);
		}
	}

	/*
	 * Check that container is at end
	 */
	if (S_EBMLREAD_METH_VALID(ebmlReader, container_at_end))
	{
		s_bool at_end;

		at_end = S_EBMLREAD_CALL(ebmlReader, container_at_end)(ebmlReader, &error);
		if (S_CHK_ERR(&error, S_CONTERR,
					  "main",
					  "Failed to check if container is at end"))
		{
			printf("failed to check if container is at end\n");
			goto quit;
		}

		if (at_end)
		{
			printf("found container at end (correct)\n");
		}
		else
		{
			printf("container not at end, error\n");
			goto quit;
		}
	}


	/*
	 * quit
	 */
quit:
	if (ds != NULL)
		S_DELETE(ds, "main", &error);

	if (ebmlReader != NULL)
		S_DELETE(ebmlReader, "main", &error);

	/* must be after ebmlReader */
	if (plugin != NULL)
		S_DELETE(plugin, "main", &error);

	/*
	 * quit speect
	 */
	error = speect_quit();
	if (error != S_SUCCESS)
	{
		printf("Call to 'speect_quit' failed\n");
		return 1;
	}


	return 0;
}
