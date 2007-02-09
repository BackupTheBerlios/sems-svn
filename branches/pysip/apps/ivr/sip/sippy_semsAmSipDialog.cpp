/*
 * Interface wrapper code.
 *
 * Generated by SIP 4.1.1 (4.1.1-255) on Fri Feb  9 10:28:23 2007
 */

#include "sipAPIpy_sems.h"
#include "sippy_semsAmSipDialog.h"


static PyObject *meth_AmSipDialog_getRoute(PyObject *sipSelf,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
		AmSipDialog *sipCpp;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"m",sipSelf,sipClass_AmSipDialog,&sipCpp))
		{
			string *sipRes;

			sipRes = new string(sipCpp -> AmSipDialog::getRoute());

			PyObject *sipResObj = sipConvertFrom_string(sipRes);
			delete sipRes;

			return sipResObj;
		}
	}

	/* Raise an exception if the arguments couldn't be parsed. */
	sipNoMethod(sipArgsParsed,sipNm_py_sems_AmSipDialog,sipNm_py_sems_getRoute);

	return NULL;
}


static PyObject *meth_AmSipDialog_setRoute(PyObject *sipSelf,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
		const string * a0;
		int a0IsTemp = 0;
		AmSipDialog *sipCpp;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"mM1",sipSelf,sipClass_AmSipDialog,&sipCpp,sipConvertTo_string,&a0,&a0IsTemp))
		{
			sipCpp -> AmSipDialog::setRoute(*a0);

			if (a0IsTemp)
				delete const_cast<string *>(a0);

			Py_INCREF(Py_None);
			return Py_None;
		}
	}

	/* Raise an exception if the arguments couldn't be parsed. */
	sipNoMethod(sipArgsParsed,sipNm_py_sems_AmSipDialog,sipNm_py_sems_setRoute);

	return NULL;
}


static PyObject *meth_AmSipDialog_getUACTransPending(PyObject *sipSelf,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
		AmSipDialog *sipCpp;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"m",sipSelf,sipClass_AmSipDialog,&sipCpp))
		{
			bool sipRes;

			sipRes = sipCpp -> AmSipDialog::getUACTransPending();

			return PyBool_FromLong(sipRes);
		}
	}

	/* Raise an exception if the arguments couldn't be parsed. */
	sipNoMethod(sipArgsParsed,sipNm_py_sems_AmSipDialog,sipNm_py_sems_getUACTransPending);

	return NULL;
}


static PyObject *meth_AmSipDialog_getStatus(PyObject *sipSelf,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
		AmSipDialog *sipCpp;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"m",sipSelf,sipClass_AmSipDialog,&sipCpp))
		{
			int sipRes;

			sipRes = sipCpp -> AmSipDialog::getStatus();

			return PyInt_FromLong((long)sipRes);
		}
	}

	/* Raise an exception if the arguments couldn't be parsed. */
	sipNoMethod(sipArgsParsed,sipNm_py_sems_AmSipDialog,sipNm_py_sems_getStatus);

	return NULL;
}


static PyObject *meth_AmSipDialog_getContactHdr(PyObject *sipSelf,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
		AmSipDialog *sipCpp;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"m",sipSelf,sipClass_AmSipDialog,&sipCpp))
		{
			string *sipRes;

			sipRes = new string(sipCpp -> AmSipDialog::getContactHdr());

			PyObject *sipResObj = sipConvertFrom_string(sipRes);
			delete sipRes;

			return sipResObj;
		}
	}

	/* Raise an exception if the arguments couldn't be parsed. */
	sipNoMethod(sipArgsParsed,sipNm_py_sems_AmSipDialog,sipNm_py_sems_getContactHdr);

	return NULL;
}


static PyObject *meth_AmSipDialog_updateStatus(PyObject *sipSelf,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
		const AmSipRequest * a0;
		AmSipDialog *sipCpp;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"mJ1",sipSelf,sipClass_AmSipDialog,&sipCpp,sipClass_AmSipRequest,&a0))
		{
			sipCpp -> AmSipDialog::updateStatus(*a0);

			Py_INCREF(Py_None);
			return Py_None;
		}
	}

	{
		const AmSipReply * a0;
		AmSipDialog *sipCpp;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"mJ1",sipSelf,sipClass_AmSipDialog,&sipCpp,sipClass_AmSipReply,&a0))
		{
			sipCpp -> AmSipDialog::updateStatus(*a0);

			Py_INCREF(Py_None);
			return Py_None;
		}
	}

	/* Raise an exception if the arguments couldn't be parsed. */
	sipNoMethod(sipArgsParsed,sipNm_py_sems_AmSipDialog,sipNm_py_sems_updateStatus);

	return NULL;
}


static PyObject *meth_AmSipDialog_reply(PyObject *sipSelf,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
		const AmSipRequest * a0;
		unsigned a1;
		const string * a2;
		int a2IsTemp = 0;
		const string * a3;
		int a3IsTemp = 0;
		const string * a4;
		int a4IsTemp = 0;
		const string * a5;
		int a5IsTemp = 0;
		AmSipDialog *sipCpp;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"mJ1iM1M1M1M1",sipSelf,sipClass_AmSipDialog,&sipCpp,sipClass_AmSipRequest,&a0,&a1,sipConvertTo_string,&a2,&a2IsTemp,sipConvertTo_string,&a3,&a3IsTemp,sipConvertTo_string,&a4,&a4IsTemp,sipConvertTo_string,&a5,&a5IsTemp))
		{
			int sipRes;

			sipRes = sipCpp -> AmSipDialog::reply(*a0,a1,*a2,*a3,*a4,*a5);

			if (a2IsTemp)
				delete const_cast<string *>(a2);

			if (a3IsTemp)
				delete const_cast<string *>(a3);

			if (a4IsTemp)
				delete const_cast<string *>(a4);

			if (a5IsTemp)
				delete const_cast<string *>(a5);

			return PyInt_FromLong((long)sipRes);
		}
	}

	/* Raise an exception if the arguments couldn't be parsed. */
	sipNoMethod(sipArgsParsed,sipNm_py_sems_AmSipDialog,sipNm_py_sems_reply);

	return NULL;
}


static PyObject *meth_AmSipDialog_sendRequest(PyObject *sipSelf,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
		const string * a0;
		int a0IsTemp = 0;
		const string * a1;
		int a1IsTemp = 0;
		const string * a2;
		int a2IsTemp = 0;
		const string * a3;
		int a3IsTemp = 0;
		AmSipDialog *sipCpp;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"mM1M1M1M1",sipSelf,sipClass_AmSipDialog,&sipCpp,sipConvertTo_string,&a0,&a0IsTemp,sipConvertTo_string,&a1,&a1IsTemp,sipConvertTo_string,&a2,&a2IsTemp,sipConvertTo_string,&a3,&a3IsTemp))
		{
			int sipRes;

			sipRes = sipCpp -> AmSipDialog::sendRequest(*a0,*a1,*a2,*a3);

			if (a0IsTemp)
				delete const_cast<string *>(a0);

			if (a1IsTemp)
				delete const_cast<string *>(a1);

			if (a2IsTemp)
				delete const_cast<string *>(a2);

			if (a3IsTemp)
				delete const_cast<string *>(a3);

			return PyInt_FromLong((long)sipRes);
		}
	}

	/* Raise an exception if the arguments couldn't be parsed. */
	sipNoMethod(sipArgsParsed,sipNm_py_sems_AmSipDialog,sipNm_py_sems_sendRequest);

	return NULL;
}


static PyObject *meth_AmSipDialog_bye(PyObject *sipSelf,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
		AmSipDialog *sipCpp;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"m",sipSelf,sipClass_AmSipDialog,&sipCpp))
		{
			int sipRes;

			sipRes = sipCpp -> AmSipDialog::bye();

			return PyInt_FromLong((long)sipRes);
		}
	}

	/* Raise an exception if the arguments couldn't be parsed. */
	sipNoMethod(sipArgsParsed,sipNm_py_sems_AmSipDialog,sipNm_py_sems_bye);

	return NULL;
}


static PyObject *meth_AmSipDialog_cancel(PyObject *sipSelf,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
		AmSipDialog *sipCpp;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"m",sipSelf,sipClass_AmSipDialog,&sipCpp))
		{
			int sipRes;

			sipRes = sipCpp -> AmSipDialog::cancel();

			return PyInt_FromLong((long)sipRes);
		}
	}

	/* Raise an exception if the arguments couldn't be parsed. */
	sipNoMethod(sipArgsParsed,sipNm_py_sems_AmSipDialog,sipNm_py_sems_cancel);

	return NULL;
}


static PyObject *meth_AmSipDialog_update(PyObject *sipSelf,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
		const string * a0;
		int a0IsTemp = 0;
		AmSipDialog *sipCpp;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"mM1",sipSelf,sipClass_AmSipDialog,&sipCpp,sipConvertTo_string,&a0,&a0IsTemp))
		{
			int sipRes;

			sipRes = sipCpp -> AmSipDialog::update(*a0);

			if (a0IsTemp)
				delete const_cast<string *>(a0);

			return PyInt_FromLong((long)sipRes);
		}
	}

	/* Raise an exception if the arguments couldn't be parsed. */
	sipNoMethod(sipArgsParsed,sipNm_py_sems_AmSipDialog,sipNm_py_sems_update);

	return NULL;
}


static PyObject *meth_AmSipDialog_reinvite(PyObject *sipSelf,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
		const string * a0;
		int a0IsTemp = 0;
		const string * a1;
		int a1IsTemp = 0;
		const string * a2;
		int a2IsTemp = 0;
		AmSipDialog *sipCpp;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"mM1M1M1",sipSelf,sipClass_AmSipDialog,&sipCpp,sipConvertTo_string,&a0,&a0IsTemp,sipConvertTo_string,&a1,&a1IsTemp,sipConvertTo_string,&a2,&a2IsTemp))
		{
			int sipRes;

			sipRes = sipCpp -> AmSipDialog::reinvite(*a0,*a1,*a2);

			if (a0IsTemp)
				delete const_cast<string *>(a0);

			if (a1IsTemp)
				delete const_cast<string *>(a1);

			if (a2IsTemp)
				delete const_cast<string *>(a2);

			return PyInt_FromLong((long)sipRes);
		}
	}

	/* Raise an exception if the arguments couldn't be parsed. */
	sipNoMethod(sipArgsParsed,sipNm_py_sems_AmSipDialog,sipNm_py_sems_reinvite);

	return NULL;
}


/* Cast a pointer to a type somewhere in its superclass hierarchy. */
static void *cast_AmSipDialog(void *ptr,sipWrapperType *targetClass)
{
	if (targetClass == sipClass_AmSipDialog)
		return ptr;

	return NULL;
}


static void dealloc_AmSipDialog(sipWrapper *sipSelf)
{
	if (sipIsPyOwned(sipSelf))
	{
		delete reinterpret_cast<AmSipDialog *>(sipSelf -> u.cppPtr);
	}
}


static void *init_AmSipDialog(sipWrapper *,PyObject *sipArgs,int *)
{
	int sipArgsParsed = 0;
	AmSipDialog *sipCpp = 0;

	if (!sipCpp)
	{
		if (sipParseArgs(&sipArgsParsed,sipArgs,""))
		{
			sipCpp = new AmSipDialog();
		}
	}

	if (!sipCpp)
	{
		const AmSipDialog * a0;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"J1",sipClass_AmSipDialog,&a0))
		{
			sipCpp = new AmSipDialog(*a0);
		}
	}

	if (!sipCpp)
	{
		sipNoCtor(sipArgsParsed,sipNm_py_sems_AmSipDialog);
		return 0;
	}

	return sipCpp;
}


static PyObject *var_AmSipDialog_cseq(PyObject *sipSelf,PyObject *valobj)
{
	int val;
	AmSipDialog *sipCpp = reinterpret_cast<AmSipDialog *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipDialog));

	if (!sipCpp)
		return NULL;

	if (valobj == NULL)
	{
		val = sipCpp -> cseq;

		valobj = PyInt_FromLong((long)val);

		return valobj;
	}

	val = (int)PyInt_AsLong(valobj);

	if (PyErr_Occurred() != NULL)
	{
		sipBadSetType(sipNm_py_sems_AmSipDialog,sipNm_py_sems_cseq);
		return NULL;
	}

	sipCpp -> cseq = val;

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *var_AmSipDialog_next_hop(PyObject *sipSelf,PyObject *valobj)
{
	int iserr = 0;
	string *val;
	AmSipDialog *sipCpp = reinterpret_cast<AmSipDialog *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipDialog));

	if (!sipCpp)
		return NULL;

	if (valobj == NULL)
	{
		val = &sipCpp -> next_hop;

		valobj = sipConvertFrom_string(val);

		return valobj;
	}

	val = reinterpret_cast<string *>(sipForceConvertTo_string(valobj,&iserr));

	if (iserr)
	{
		sipBadSetType(sipNm_py_sems_AmSipDialog,sipNm_py_sems_next_hop);
		return NULL;
	}

	sipCpp -> next_hop = *val;

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *var_AmSipDialog_local_party(PyObject *sipSelf,PyObject *valobj)
{
	int iserr = 0;
	string *val;
	AmSipDialog *sipCpp = reinterpret_cast<AmSipDialog *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipDialog));

	if (!sipCpp)
		return NULL;

	if (valobj == NULL)
	{
		val = &sipCpp -> local_party;

		valobj = sipConvertFrom_string(val);

		return valobj;
	}

	val = reinterpret_cast<string *>(sipForceConvertTo_string(valobj,&iserr));

	if (iserr)
	{
		sipBadSetType(sipNm_py_sems_AmSipDialog,sipNm_py_sems_local_party);
		return NULL;
	}

	sipCpp -> local_party = *val;

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *var_AmSipDialog_remote_party(PyObject *sipSelf,PyObject *valobj)
{
	int iserr = 0;
	string *val;
	AmSipDialog *sipCpp = reinterpret_cast<AmSipDialog *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipDialog));

	if (!sipCpp)
		return NULL;

	if (valobj == NULL)
	{
		val = &sipCpp -> remote_party;

		valobj = sipConvertFrom_string(val);

		return valobj;
	}

	val = reinterpret_cast<string *>(sipForceConvertTo_string(valobj,&iserr));

	if (iserr)
	{
		sipBadSetType(sipNm_py_sems_AmSipDialog,sipNm_py_sems_remote_party);
		return NULL;
	}

	sipCpp -> remote_party = *val;

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *var_AmSipDialog_local_tag(PyObject *sipSelf,PyObject *valobj)
{
	int iserr = 0;
	string *val;
	AmSipDialog *sipCpp = reinterpret_cast<AmSipDialog *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipDialog));

	if (!sipCpp)
		return NULL;

	if (valobj == NULL)
	{
		val = &sipCpp -> local_tag;

		valobj = sipConvertFrom_string(val);

		return valobj;
	}

	val = reinterpret_cast<string *>(sipForceConvertTo_string(valobj,&iserr));

	if (iserr)
	{
		sipBadSetType(sipNm_py_sems_AmSipDialog,sipNm_py_sems_local_tag);
		return NULL;
	}

	sipCpp -> local_tag = *val;

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *var_AmSipDialog_remote_tag(PyObject *sipSelf,PyObject *valobj)
{
	int iserr = 0;
	string *val;
	AmSipDialog *sipCpp = reinterpret_cast<AmSipDialog *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipDialog));

	if (!sipCpp)
		return NULL;

	if (valobj == NULL)
	{
		val = &sipCpp -> remote_tag;

		valobj = sipConvertFrom_string(val);

		return valobj;
	}

	val = reinterpret_cast<string *>(sipForceConvertTo_string(valobj,&iserr));

	if (iserr)
	{
		sipBadSetType(sipNm_py_sems_AmSipDialog,sipNm_py_sems_remote_tag);
		return NULL;
	}

	sipCpp -> remote_tag = *val;

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *var_AmSipDialog_callid(PyObject *sipSelf,PyObject *valobj)
{
	int iserr = 0;
	string *val;
	AmSipDialog *sipCpp = reinterpret_cast<AmSipDialog *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipDialog));

	if (!sipCpp)
		return NULL;

	if (valobj == NULL)
	{
		val = &sipCpp -> callid;

		valobj = sipConvertFrom_string(val);

		return valobj;
	}

	val = reinterpret_cast<string *>(sipForceConvertTo_string(valobj,&iserr));

	if (iserr)
	{
		sipBadSetType(sipNm_py_sems_AmSipDialog,sipNm_py_sems_callid);
		return NULL;
	}

	sipCpp -> callid = *val;

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *var_AmSipDialog_contact_uri(PyObject *sipSelf,PyObject *valobj)
{
	int iserr = 0;
	string *val;
	AmSipDialog *sipCpp = reinterpret_cast<AmSipDialog *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipDialog));

	if (!sipCpp)
		return NULL;

	if (valobj == NULL)
	{
		val = &sipCpp -> contact_uri;

		valobj = sipConvertFrom_string(val);

		return valobj;
	}

	val = reinterpret_cast<string *>(sipForceConvertTo_string(valobj,&iserr));

	if (iserr)
	{
		sipBadSetType(sipNm_py_sems_AmSipDialog,sipNm_py_sems_contact_uri);
		return NULL;
	}

	sipCpp -> contact_uri = *val;

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *var_AmSipDialog_remote_uri(PyObject *sipSelf,PyObject *valobj)
{
	int iserr = 0;
	string *val;
	AmSipDialog *sipCpp = reinterpret_cast<AmSipDialog *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipDialog));

	if (!sipCpp)
		return NULL;

	if (valobj == NULL)
	{
		val = &sipCpp -> remote_uri;

		valobj = sipConvertFrom_string(val);

		return valobj;
	}

	val = reinterpret_cast<string *>(sipForceConvertTo_string(valobj,&iserr));

	if (iserr)
	{
		sipBadSetType(sipNm_py_sems_AmSipDialog,sipNm_py_sems_remote_uri);
		return NULL;
	}

	sipCpp -> remote_uri = *val;

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *var_AmSipDialog_local_uri(PyObject *sipSelf,PyObject *valobj)
{
	int iserr = 0;
	string *val;
	AmSipDialog *sipCpp = reinterpret_cast<AmSipDialog *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipDialog));

	if (!sipCpp)
		return NULL;

	if (valobj == NULL)
	{
		val = &sipCpp -> local_uri;

		valobj = sipConvertFrom_string(val);

		return valobj;
	}

	val = reinterpret_cast<string *>(sipForceConvertTo_string(valobj,&iserr));

	if (iserr)
	{
		sipBadSetType(sipNm_py_sems_AmSipDialog,sipNm_py_sems_local_uri);
		return NULL;
	}

	sipCpp -> local_uri = *val;

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *var_AmSipDialog_sip_port(PyObject *sipSelf,PyObject *valobj)
{
	int iserr = 0;
	string *val;
	AmSipDialog *sipCpp = reinterpret_cast<AmSipDialog *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipDialog));

	if (!sipCpp)
		return NULL;

	if (valobj == NULL)
	{
		val = &sipCpp -> sip_port;

		valobj = sipConvertFrom_string(val);

		return valobj;
	}

	val = reinterpret_cast<string *>(sipForceConvertTo_string(valobj,&iserr));

	if (iserr)
	{
		sipBadSetType(sipNm_py_sems_AmSipDialog,sipNm_py_sems_sip_port);
		return NULL;
	}

	sipCpp -> sip_port = *val;

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *var_AmSipDialog_sip_ip(PyObject *sipSelf,PyObject *valobj)
{
	int iserr = 0;
	string *val;
	AmSipDialog *sipCpp = reinterpret_cast<AmSipDialog *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipDialog));

	if (!sipCpp)
		return NULL;

	if (valobj == NULL)
	{
		val = &sipCpp -> sip_ip;

		valobj = sipConvertFrom_string(val);

		return valobj;
	}

	val = reinterpret_cast<string *>(sipForceConvertTo_string(valobj,&iserr));

	if (iserr)
	{
		sipBadSetType(sipNm_py_sems_AmSipDialog,sipNm_py_sems_sip_ip);
		return NULL;
	}

	sipCpp -> sip_ip = *val;

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *var_AmSipDialog_domain(PyObject *sipSelf,PyObject *valobj)
{
	int iserr = 0;
	string *val;
	AmSipDialog *sipCpp = reinterpret_cast<AmSipDialog *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipDialog));

	if (!sipCpp)
		return NULL;

	if (valobj == NULL)
	{
		val = &sipCpp -> domain;

		valobj = sipConvertFrom_string(val);

		return valobj;
	}

	val = reinterpret_cast<string *>(sipForceConvertTo_string(valobj,&iserr));

	if (iserr)
	{
		sipBadSetType(sipNm_py_sems_AmSipDialog,sipNm_py_sems_domain);
		return NULL;
	}

	sipCpp -> domain = *val;

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *var_AmSipDialog_user(PyObject *sipSelf,PyObject *valobj)
{
	int iserr = 0;
	string *val;
	AmSipDialog *sipCpp = reinterpret_cast<AmSipDialog *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipDialog));

	if (!sipCpp)
		return NULL;

	if (valobj == NULL)
	{
		val = &sipCpp -> user;

		valobj = sipConvertFrom_string(val);

		return valobj;
	}

	val = reinterpret_cast<string *>(sipForceConvertTo_string(valobj,&iserr));

	if (iserr)
	{
		sipBadSetType(sipNm_py_sems_AmSipDialog,sipNm_py_sems_user);
		return NULL;
	}

	sipCpp -> user = *val;

	Py_INCREF(Py_None);
	return Py_None;
}

PyMethodDef variables_AmSipDialog[] = {
	{sipNm_py_sems_cseq, var_AmSipDialog_cseq, 0, NULL},
	{sipNm_py_sems_next_hop, var_AmSipDialog_next_hop, 0, NULL},
	{sipNm_py_sems_local_party, var_AmSipDialog_local_party, 0, NULL},
	{sipNm_py_sems_remote_party, var_AmSipDialog_remote_party, 0, NULL},
	{sipNm_py_sems_local_tag, var_AmSipDialog_local_tag, 0, NULL},
	{sipNm_py_sems_remote_tag, var_AmSipDialog_remote_tag, 0, NULL},
	{sipNm_py_sems_callid, var_AmSipDialog_callid, 0, NULL},
	{sipNm_py_sems_contact_uri, var_AmSipDialog_contact_uri, 0, NULL},
	{sipNm_py_sems_remote_uri, var_AmSipDialog_remote_uri, 0, NULL},
	{sipNm_py_sems_local_uri, var_AmSipDialog_local_uri, 0, NULL},
	{sipNm_py_sems_sip_port, var_AmSipDialog_sip_port, 0, NULL},
	{sipNm_py_sems_sip_ip, var_AmSipDialog_sip_ip, 0, NULL},
	{sipNm_py_sems_domain, var_AmSipDialog_domain, 0, NULL},
	{sipNm_py_sems_user, var_AmSipDialog_user, 0, NULL},
	{0, 0, 0, 0}
};


static void *forceConvertTo_AmSipDialog(PyObject *valobj,int *iserrp)
{
	if (*iserrp || valobj == NULL)
		return NULL;

	if (valobj == Py_None || sipIsSubClassInstance(valobj,sipClass_AmSipDialog))
		return sipConvertToCpp(valobj,sipClass_AmSipDialog,iserrp);

	sipBadClass(sipNm_py_sems_AmSipDialog);

	*iserrp = 1;

	return NULL;
}


static PyMethodDef methods_AmSipDialog[] = {
	{sipNm_py_sems_bye, meth_AmSipDialog_bye, METH_VARARGS, NULL},
	{sipNm_py_sems_cancel, meth_AmSipDialog_cancel, METH_VARARGS, NULL},
	{sipNm_py_sems_getContactHdr, meth_AmSipDialog_getContactHdr, METH_VARARGS, NULL},
	{sipNm_py_sems_getRoute, meth_AmSipDialog_getRoute, METH_VARARGS, NULL},
	{sipNm_py_sems_getStatus, meth_AmSipDialog_getStatus, METH_VARARGS, NULL},
	{sipNm_py_sems_getUACTransPending, meth_AmSipDialog_getUACTransPending, METH_VARARGS, NULL},
	{sipNm_py_sems_reinvite, meth_AmSipDialog_reinvite, METH_VARARGS, NULL},
	{sipNm_py_sems_reply, meth_AmSipDialog_reply, METH_VARARGS, NULL},
	{sipNm_py_sems_sendRequest, meth_AmSipDialog_sendRequest, METH_VARARGS, NULL},
	{sipNm_py_sems_setRoute, meth_AmSipDialog_setRoute, METH_VARARGS, NULL},
	{sipNm_py_sems_update, meth_AmSipDialog_update, METH_VARARGS, NULL},
	{sipNm_py_sems_updateStatus, meth_AmSipDialog_updateStatus, METH_VARARGS, NULL}
};

static sipEnumValueInstanceDef enums_AmSipDialog[] = {
	{sipNm_py_sems_Connected, AmSipDialog::Connected},
	{sipNm_py_sems_Disconnected, AmSipDialog::Disconnected},
	{sipNm_py_sems_Disconnecting, AmSipDialog::Disconnecting},
	{sipNm_py_sems_Pending, AmSipDialog::Pending}
};


/* The main type data structure. */
sipTypeDef sipType_AmSipDialog = {
	0,
	"py_sems.AmSipDialog",
	-1,
	0,
	0,
	12, methods_AmSipDialog,
	4, enums_AmSipDialog,
	variables_AmSipDialog,
	init_AmSipDialog,
	dealloc_AmSipDialog,
	cast_AmSipDialog,
	forceConvertTo_AmSipDialog,
	0,
	0,
	0,
	{0, 0, 0, 0, 0, 0}
};
