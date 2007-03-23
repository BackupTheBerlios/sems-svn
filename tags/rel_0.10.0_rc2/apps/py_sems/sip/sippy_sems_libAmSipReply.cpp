/*
 * Interface wrapper code.
 *
 * Generated by SIP 4.5.2 (4.5.2) on Tue Feb 20 15:24:56 2007
 */

#include "sipAPIpy_sems_lib.h"
#include "sippy_sems_libAmSipReply.h"

#include "sippy_sems_libstring.h"



/* Cast a pointer to a type somewhere in its superclass hierarchy. */
extern "C" {static void *cast_AmSipReply(void *, sipWrapperType *);}
static void *cast_AmSipReply(void *ptr,sipWrapperType *targetClass)
{
    if (targetClass == sipClass_AmSipReply)
        return ptr;

    return NULL;
}


/* Call the instance's destructor. */
extern "C" {static void release_AmSipReply(void *, int);}
static void release_AmSipReply(void *ptr,int)
{
    delete reinterpret_cast<AmSipReply *>(ptr);
}


extern "C" {static void dealloc_AmSipReply(sipWrapper *);}
static void dealloc_AmSipReply(sipWrapper *sipSelf)
{
    sipTrace(SIP_TRACE_DEALLOCS,"dealloc_AmSipReply()\n");

    if (sipIsPyOwned(sipSelf))
    {
        release_AmSipReply(sipSelf->u.cppPtr,0);
    }
}


extern "C" {static void *init_AmSipReply(sipWrapper *, PyObject *, sipWrapper **, int *);}
static void *init_AmSipReply(sipWrapper *,PyObject *sipArgs,sipWrapper **,int *sipArgsParsed)
{
    AmSipReply *sipCpp = 0;

    sipTrace(SIP_TRACE_INITS,"init_AmSipReply()\n");

    if (!sipCpp)
    {
        if (sipParseArgs(sipArgsParsed,sipArgs,""))
        {
            sipCpp = new AmSipReply();
        }
    }

    if (!sipCpp)
    {
        const AmSipReply * a0;

        if (sipParseArgs(sipArgsParsed,sipArgs,"JA",sipClass_AmSipReply,&a0))
        {
            sipCpp = new AmSipReply(*a0);
        }
    }

    return sipCpp;
}


extern "C" {static PyObject *var_AmSipReply_cseq(PyObject *, PyObject *);}
static PyObject *var_AmSipReply_cseq(PyObject *sipSelf,PyObject *sipPy)
{
   unsigned sipVal;
    AmSipReply *sipCpp = reinterpret_cast<AmSipReply *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipReply));

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
        sipBadSetType(sipNm_py_sems_lib_AmSipReply,sipNm_py_sems_lib_cseq);
        return NULL;
    }

    sipCpp->cseq = sipVal;

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipReply_local_tag(PyObject *, PyObject *);}
static PyObject *var_AmSipReply_local_tag(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipReply *sipCpp = reinterpret_cast<AmSipReply *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipReply));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->local_tag;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->local_tag = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipReply_remote_tag(PyObject *, PyObject *);}
static PyObject *var_AmSipReply_remote_tag(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipReply *sipCpp = reinterpret_cast<AmSipReply *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipReply));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->remote_tag;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->remote_tag = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipReply_body(PyObject *, PyObject *);}
static PyObject *var_AmSipReply_body(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipReply *sipCpp = reinterpret_cast<AmSipReply *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipReply));

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


extern "C" {static PyObject *var_AmSipReply_hdrs(PyObject *, PyObject *);}
static PyObject *var_AmSipReply_hdrs(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipReply *sipCpp = reinterpret_cast<AmSipReply *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipReply));

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


extern "C" {static PyObject *var_AmSipReply_route(PyObject *, PyObject *);}
static PyObject *var_AmSipReply_route(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipReply *sipCpp = reinterpret_cast<AmSipReply *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipReply));

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


extern "C" {static PyObject *var_AmSipReply_next_hop(PyObject *, PyObject *);}
static PyObject *var_AmSipReply_next_hop(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipReply *sipCpp = reinterpret_cast<AmSipReply *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipReply));

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


extern "C" {static PyObject *var_AmSipReply_next_request_uri(PyObject *, PyObject *);}
static PyObject *var_AmSipReply_next_request_uri(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipReply *sipCpp = reinterpret_cast<AmSipReply *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipReply));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->next_request_uri;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->next_request_uri = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipReply_reason(PyObject *, PyObject *);}
static PyObject *var_AmSipReply_reason(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
    int sipValState;
   string *sipVal;
    AmSipReply *sipCpp = reinterpret_cast<AmSipReply *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipReply));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->reason;

        sipPy = sipConvertFromMappedType(sipVal,sipMappedType_string,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<string *>(sipForceConvertToMappedType(sipPy,sipMappedType_string,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->reason = *sipVal;

    sipReleaseMappedType(sipVal,sipMappedType_string,sipValState);

    Py_INCREF(Py_None);
    return Py_None;
}


extern "C" {static PyObject *var_AmSipReply_code(PyObject *, PyObject *);}
static PyObject *var_AmSipReply_code(PyObject *sipSelf,PyObject *sipPy)
{
   unsigned sipVal;
    AmSipReply *sipCpp = reinterpret_cast<AmSipReply *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_AmSipReply));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = sipCpp->code;

        sipPy = PyLong_FromUnsignedLong(sipVal);

        return sipPy;
    }

	sipVal = (unsigned)sipLong_AsUnsignedLong(sipPy);

    if (PyErr_Occurred() != NULL)
    {
        sipBadSetType(sipNm_py_sems_lib_AmSipReply,sipNm_py_sems_lib_code);
        return NULL;
    }

    sipCpp->code = sipVal;

    Py_INCREF(Py_None);
    return Py_None;
}

PyMethodDef variables_AmSipReply[] = {
    {sipNm_py_sems_lib_cseq, var_AmSipReply_cseq, 0, NULL},
    {sipNm_py_sems_lib_local_tag, var_AmSipReply_local_tag, 0, NULL},
    {sipNm_py_sems_lib_remote_tag, var_AmSipReply_remote_tag, 0, NULL},
    {sipNm_py_sems_lib_body, var_AmSipReply_body, 0, NULL},
    {sipNm_py_sems_lib_hdrs, var_AmSipReply_hdrs, 0, NULL},
    {sipNm_py_sems_lib_route, var_AmSipReply_route, 0, NULL},
    {sipNm_py_sems_lib_next_hop, var_AmSipReply_next_hop, 0, NULL},
    {sipNm_py_sems_lib_next_request_uri, var_AmSipReply_next_request_uri, 0, NULL},
    {sipNm_py_sems_lib_reason, var_AmSipReply_reason, 0, NULL},
    {sipNm_py_sems_lib_code, var_AmSipReply_code, 0, NULL},
    {0, 0, 0, 0}
};


extern "C" {static void *forceConvertTo_AmSipReply(PyObject *, int *);}
static void *forceConvertTo_AmSipReply(PyObject *valobj,int *iserrp)
{
    if (*iserrp || valobj == NULL)
        return NULL;

    if (valobj == Py_None || sipIsSubClassInstance(valobj,sipClass_AmSipReply))
        return sipConvertToCpp(valobj,sipClass_AmSipReply,iserrp);

    sipBadClass(sipNm_py_sems_lib_AmSipReply);

    *iserrp = 1;

    return NULL;
}


sipTypeDef sipType_py_sems_lib_AmSipReply = {
    0,
    0,
    "py_sems_lib.AmSipReply",
    0,
    {0, 0, 1},
    0,
    0,
    0, 0,
    0, 0,
    variables_AmSipReply,
    init_AmSipReply,
    0,
    0,
    0,
    0,
    0,
    0,
    dealloc_AmSipReply,
    cast_AmSipReply,
    release_AmSipReply,
    forceConvertTo_AmSipReply,
    0,
    0,
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    0
};