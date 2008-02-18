/*
 * Interface wrapper code.
 *
 * Generated by SIP 4.5.2 (4.5.2) on Mon Feb 18 16:33:22 2008
 */

#include "sipAPIpy_sems_lib.h"
#include "sippy_sems_libAmSipRequest.h"

#include "sippy_sems_libstring.h"



/* Cast a pointer to a type somewhere in its superclass hierarchy. */
extern "C" {static void *cast_AmSipRequest(void *, sipWrapperType *);}
static void *cast_AmSipRequest(void *ptr,sipWrapperType *targetClass)
{
    if (targetClass == sipClass_AmSipRequest)
        return ptr;

    return NULL;
}


/* Call the instance's destructor. */
extern "C" {static void release_AmSipRequest(void *, int);}
static void release_AmSipRequest(void *ptr,int)
{
    delete reinterpret_cast<AmSipRequest *>(ptr);
}


extern "C" {static void dealloc_AmSipRequest(sipWrapper *);}
static void dealloc_AmSipRequest(sipWrapper *sipSelf)
{
    sipTrace(SIP_TRACE_DEALLOCS,"dealloc_AmSipRequest()\n");

    if (sipIsPyOwned(sipSelf))
    {
        release_AmSipRequest(sipSelf->u.cppPtr,0);
    }
}


extern "C" {static void *init_AmSipRequest(sipWrapper *, PyObject *, sipWrapper **, int *);}
static void *init_AmSipRequest(sipWrapper *,PyObject *sipArgs,sipWrapper **,int *sipArgsParsed)
{
    AmSipRequest *sipCpp = 0;

    sipTrace(SIP_TRACE_INITS,"init_AmSipRequest()\n");

    if (!sipCpp)
    {
        if (sipParseArgs(sipArgsParsed,sipArgs,""))
        {
            sipCpp = new AmSipRequest();
        }
    }

    if (!sipCpp)
    {
        const AmSipRequest * a0;

        if (sipParseArgs(sipArgsParsed,sipArgs,"JA",sipClass_AmSipRequest,&a0))
        {
            sipCpp = new AmSipRequest(*a0);
        }
    }

    return sipCpp;
}


extern "C" {static PyObject *var_AmSipRequest_next_hop(PyObject *, PyObject *);}
static PyObject *var_AmSipRequest_next_hop(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipRequest *sipCpp = reinterpret_cast<AmSipRequest *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipRequest));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->next_hop;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->next_hop = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipRequest_route(PyObject *, PyObject *);}
static PyObject *var_AmSipRequest_route(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipRequest *sipCpp = reinterpret_cast<AmSipRequest *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipRequest));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->route;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->route = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipRequest_body(PyObject *, PyObject *);}
static PyObject *var_AmSipRequest_body(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipRequest *sipCpp = reinterpret_cast<AmSipRequest *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipRequest));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->body;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->body = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipRequest_hdrs(PyObject *, PyObject *);}
static PyObject *var_AmSipRequest_hdrs(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipRequest *sipCpp = reinterpret_cast<AmSipRequest *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipRequest));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->hdrs;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->hdrs = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipRequest_cseq(PyObject *, PyObject *);}
static PyObject *var_AmSipRequest_cseq(PyObject *sipSelf,PyObject *sipPy)
{
   unsigned sipVal;
    AmSipRequest *sipCpp = reinterpret_cast<AmSipRequest *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipRequest));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = sipCpp->cseq;

        sipPy = PyLong_FromUnsignedLong(sipVal);

        return sipPy;
    }

	sipVal = (unsigned)sipLong_AsUnsignedLong(sipPy);

    if (PyErr_Occurred() != NULL)
    {
        sipBadSetType(sipNm_py_sems_lib_AmSipRequest,sipNm_py_sems_lib_cseq);
        return NULL;
    }

    sipCpp->cseq = sipVal;

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipRequest_to_tag(PyObject *, PyObject *);}
static PyObject *var_AmSipRequest_to_tag(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipRequest *sipCpp = reinterpret_cast<AmSipRequest *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipRequest));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->to_tag;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->to_tag = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipRequest_from_tag(PyObject *, PyObject *);}
static PyObject *var_AmSipRequest_from_tag(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipRequest *sipCpp = reinterpret_cast<AmSipRequest *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipRequest));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->from_tag;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->from_tag = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipRequest_callid(PyObject *, PyObject *);}
static PyObject *var_AmSipRequest_callid(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipRequest *sipCpp = reinterpret_cast<AmSipRequest *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipRequest));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->callid;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->callid = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipRequest_to(PyObject *, PyObject *);}
static PyObject *var_AmSipRequest_to(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipRequest *sipCpp = reinterpret_cast<AmSipRequest *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipRequest));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->to;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->to = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipRequest_from(PyObject *, PyObject *);}
static PyObject *var_AmSipRequest_from(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipRequest *sipCpp = reinterpret_cast<AmSipRequest *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipRequest));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->from;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->from = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipRequest_from_uri(PyObject *, PyObject *);}
static PyObject *var_AmSipRequest_from_uri(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipRequest *sipCpp = reinterpret_cast<AmSipRequest *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipRequest));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->from_uri;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->from_uri = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipRequest_r_uri(PyObject *, PyObject *);}
static PyObject *var_AmSipRequest_r_uri(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipRequest *sipCpp = reinterpret_cast<AmSipRequest *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipRequest));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->r_uri;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->r_uri = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipRequest_port(PyObject *, PyObject *);}
static PyObject *var_AmSipRequest_port(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipRequest *sipCpp = reinterpret_cast<AmSipRequest *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipRequest));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->port;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->port = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipRequest_dstip(PyObject *, PyObject *);}
static PyObject *var_AmSipRequest_dstip(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipRequest *sipCpp = reinterpret_cast<AmSipRequest *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipRequest));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->dstip;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->dstip = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipRequest_domain(PyObject *, PyObject *);}
static PyObject *var_AmSipRequest_domain(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipRequest *sipCpp = reinterpret_cast<AmSipRequest *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipRequest));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->domain;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->domain = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipRequest_user(PyObject *, PyObject *);}
static PyObject *var_AmSipRequest_user(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipRequest *sipCpp = reinterpret_cast<AmSipRequest *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipRequest));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->user;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->user = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipRequest_method(PyObject *, PyObject *);}
static PyObject *var_AmSipRequest_method(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipRequest *sipCpp = reinterpret_cast<AmSipRequest *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipRequest));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->method;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->method = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipRequest_cmd(PyObject *, PyObject *);}
static PyObject *var_AmSipRequest_cmd(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipRequest *sipCpp = reinterpret_cast<AmSipRequest *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipRequest));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->cmd;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->cmd = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}

PyMethodDef variables_AmSipRequest[] = {
    {sipNm_py_sems_lib_next_hop, var_AmSipRequest_next_hop, 0, NULL},
    {sipNm_py_sems_lib_route, var_AmSipRequest_route, 0, NULL},
    {sipNm_py_sems_lib_body, var_AmSipRequest_body, 0, NULL},
    {sipNm_py_sems_lib_hdrs, var_AmSipRequest_hdrs, 0, NULL},
    {sipNm_py_sems_lib_cseq, var_AmSipRequest_cseq, 0, NULL},
    {sipNm_py_sems_lib_to_tag, var_AmSipRequest_to_tag, 0, NULL},
    {sipNm_py_sems_lib_from_tag, var_AmSipRequest_from_tag, 0, NULL},
    {sipNm_py_sems_lib_callid, var_AmSipRequest_callid, 0, NULL},
    {sipNm_py_sems_lib_to, var_AmSipRequest_to, 0, NULL},
    {sipNm_py_sems_lib_from, var_AmSipRequest_from, 0, NULL},
    {sipNm_py_sems_lib_from_uri, var_AmSipRequest_from_uri, 0, NULL},
    {sipNm_py_sems_lib_r_uri, var_AmSipRequest_r_uri, 0, NULL},
    {sipNm_py_sems_lib_port, var_AmSipRequest_port, 0, NULL},
    {sipNm_py_sems_lib_dstip, var_AmSipRequest_dstip, 0, NULL},
    {sipNm_py_sems_lib_domain, var_AmSipRequest_domain, 0, NULL},
    {sipNm_py_sems_lib_user, var_AmSipRequest_user, 0, NULL},
    {sipNm_py_sems_lib_method, var_AmSipRequest_method, 0, NULL},
    {sipNm_py_sems_lib_cmd, var_AmSipRequest_cmd, 0, NULL},
    {0, 0, 0, 0}
};


extern "C" {static void *forceConvertTo_AmSipRequest(PyObject *, int *);}
static void *forceConvertTo_AmSipRequest(PyObject *valobj,int *iserrp)
{
    if (*iserrp || valobj == NULL)
        return NULL;

    if (valobj == Py_None || sipIsSubClassInstance(valobj,sipClass_AmSipRequest))
        return sipConvertToCpp(valobj,sipClass_AmSipRequest,iserrp);

    sipBadClass(sipNm_py_sems_lib_AmSipRequest);

    *iserrp = 1;

    return NULL;
}


sipTypeDef sipType_py_sems_lib_AmSipRequest = {
    0,
    0,
    "py_sems_lib.AmSipRequest",
    0,
    {0, 0, 1},
    0,
    0,
    0, 0,
    0, 0,
    variables_AmSipRequest,
    init_AmSipRequest,
    0,
    0,
    0,
    0,
    0,
    0,
    dealloc_AmSipRequest,
    cast_AmSipRequest,
    release_AmSipRequest,
    forceConvertTo_AmSipRequest,
    0,
    0,
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    0
};
