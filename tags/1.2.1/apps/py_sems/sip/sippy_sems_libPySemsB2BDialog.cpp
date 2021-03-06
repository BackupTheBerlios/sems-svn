/*
 * Interface wrapper code.
 *
 * Generated by SIP 4.5.2 (4.5.2) on Mon Dec  8 15:20:56 2008
 */

#include "sipAPIpy_sems_lib.h"
#include "sippy_sems_libPySemsB2BDialog.h"

#include "sippy_sems_libAmSipDialog.h"
#include "sippy_sems_libPySemsB2BDialog.h"
#include "sippy_sems_libAmEvent.h"
#include "sippy_sems_libAmAudioFile.h"
#include "sippy_sems_libAmSipRequest.h"
#include "sippy_sems_libstring.h"


sipPySemsB2BDialog::sipPySemsB2BDialog(): PySemsB2BDialog(), sipPySelf(0)
{
    sipTrace(SIP_TRACE_CTORS,"sipPySemsB2BDialog::sipPySemsB2BDialog() (this=0x%08x)\n",this);

    sipCommonCtor(sipPyMethods,6);
}

sipPySemsB2BDialog::~sipPySemsB2BDialog()
{
    sipTrace(SIP_TRACE_DTORS,"sipPySemsB2BDialog::~sipPySemsB2BDialog() (this=0x%08x)\n",this);

    sipCommonDtor(sipPySelf);
}

void sipPySemsB2BDialog::onInvite(const AmSipRequest& a0)
{
    sipTrace(SIP_TRACE_CATCHERS,"void sipPySemsB2BDialog::onInvite(const AmSipRequest&) (this=0x%08x)\n",this);

    extern void sipVH_py_sems_lib_4(sip_gilstate_t,PyObject *,const AmSipRequest&);

    sip_gilstate_t sipGILState;
    PyObject *meth;

    meth = sipIsPyMethod(&sipGILState,&sipPyMethods[0],sipPySelf,NULL,sipNm_py_sems_lib_onInvite);

    if (!meth)
    {
        PySemsB2BDialog::onInvite(a0);
        return;
    }

    sipVH_py_sems_lib_4(sipGILState,meth,a0);
}

void sipPySemsB2BDialog::onSessionStart(const AmSipRequest& a0)
{
    sipTrace(SIP_TRACE_CATCHERS,"void sipPySemsB2BDialog::onSessionStart(const AmSipRequest&) (this=0x%08x)\n",this);

    extern void sipVH_py_sems_lib_4(sip_gilstate_t,PyObject *,const AmSipRequest&);

    sip_gilstate_t sipGILState;
    PyObject *meth;

    meth = sipIsPyMethod(&sipGILState,&sipPyMethods[1],sipPySelf,NULL,sipNm_py_sems_lib_onSessionStart);

    if (!meth)
    {
        PySemsB2BDialog::onSessionStart(a0);
        return;
    }

    sipVH_py_sems_lib_4(sipGILState,meth,a0);
}

void sipPySemsB2BDialog::onCancel()
{
    sipTrace(SIP_TRACE_CATCHERS,"void sipPySemsB2BDialog::onCancel() (this=0x%08x)\n",this);

    extern void sipVH_py_sems_lib_5(sip_gilstate_t,PyObject *);

    sip_gilstate_t sipGILState;
    PyObject *meth;

    meth = sipIsPyMethod(&sipGILState,&sipPyMethods[2],sipPySelf,NULL,sipNm_py_sems_lib_onCancel);

    if (!meth)
    {
        PySemsB2BDialog::onCancel();
        return;
    }

    sipVH_py_sems_lib_5(sipGILState,meth);
}

void sipPySemsB2BDialog::onBye(const AmSipRequest& a0)
{
    sipTrace(SIP_TRACE_CATCHERS,"void sipPySemsB2BDialog::onBye(const AmSipRequest&) (this=0x%08x)\n",this);

    extern void sipVH_py_sems_lib_4(sip_gilstate_t,PyObject *,const AmSipRequest&);

    sip_gilstate_t sipGILState;
    PyObject *meth;

    meth = sipIsPyMethod(&sipGILState,&sipPyMethods[3],sipPySelf,NULL,sipNm_py_sems_lib_onBye);

    if (!meth)
    {
        PySemsB2BDialog::onBye(a0);
        return;
    }

    sipVH_py_sems_lib_4(sipGILState,meth,a0);
}

void sipPySemsB2BDialog::onDtmf(int a0,int a1)
{
    sipTrace(SIP_TRACE_CATCHERS,"void sipPySemsB2BDialog::onDtmf(int,int) (this=0x%08x)\n",this);

    extern void sipVH_py_sems_lib_3(sip_gilstate_t,PyObject *,int,int);

    sip_gilstate_t sipGILState;
    PyObject *meth;

    meth = sipIsPyMethod(&sipGILState,&sipPyMethods[4],sipPySelf,NULL,sipNm_py_sems_lib_onDtmf);

    if (!meth)
    {
        PySemsB2BDialog::onDtmf(a0,a1);
        return;
    }

    sipVH_py_sems_lib_3(sipGILState,meth,a0,a1);
}

void sipPySemsB2BDialog::process(AmEvent *a0)
{
    sipTrace(SIP_TRACE_CATCHERS,"void sipPySemsB2BDialog::process(AmEvent *) (this=0x%08x)\n",this);

    extern void sipVH_py_sems_lib_2(sip_gilstate_t,PyObject *,AmEvent *);

    sip_gilstate_t sipGILState;
    PyObject *meth;

    meth = sipIsPyMethod(&sipGILState,&sipPyMethods[5],sipPySelf,NULL,sipNm_py_sems_lib_process);

    if (!meth)
    {
        PySemsB2BDialog::process(a0);
        return;
    }

    sipVH_py_sems_lib_2(sipGILState,meth,a0);
}


extern "C" {static PyObject *meth_PySemsB2BDialog_acceptAudio(PyObject *, PyObject *);}
static PyObject *meth_PySemsB2BDialog_acceptAudio(PyObject *sipSelf,PyObject *sipArgs)
{
    sipTrace(SIP_TRACE_METHODS,"meth_PySemsB2BDialog_acceptAudio()\n");

    int sipArgsParsed = 0;

    {
        const string * a0;
        int a0State = 0;
        const string * a1;
        int a1State = 0;
        string * a2;
        PySemsB2BDialog *sipCpp;

        if (sipParseArgs(&sipArgsParsed,sipArgs,"BM1M1",&sipSelf,sipClass_PySemsB2BDialog,&sipCpp,sipMappedType_string,&a0,&a0State,sipMappedType_string,&a1,&a1State))
        {
            int sipRes;
            a2 = new string();

            sipRes = sipCpp->acceptAudio(*a0,*a1,a2);

            sipReleaseMappedType(const_cast<string *>(a0),sipMappedType_string,a0State);
            sipReleaseMappedType(const_cast<string *>(a1),sipMappedType_string,a1State);

            return sipBuildResult(0,"(iD)",sipRes,a2,sipMappedType_string,NULL);
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipArgsParsed,sipNm_py_sems_lib_PySemsB2BDialog,sipNm_py_sems_lib_acceptAudio);

    return NULL;
}


extern "C" {static PyObject *meth_PySemsB2BDialog_setStopped(PyObject *, PyObject *);}
static PyObject *meth_PySemsB2BDialog_setStopped(PyObject *sipSelf,PyObject *sipArgs)
{
    sipTrace(SIP_TRACE_METHODS,"meth_PySemsB2BDialog_setStopped()\n");

    int sipArgsParsed = 0;

    {
        PySemsB2BDialog *sipCpp;

        if (sipParseArgs(&sipArgsParsed,sipArgs,"B",&sipSelf,sipClass_PySemsB2BDialog,&sipCpp))
        {
            sipCpp->setStopped();

            Py_INCREF(Py_None);
            return Py_None;
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipArgsParsed,sipNm_py_sems_lib_PySemsB2BDialog,sipNm_py_sems_lib_setStopped);

    return NULL;
}


extern "C" {static PyObject *meth_PySemsB2BDialog_onInvite(PyObject *, PyObject *);}
static PyObject *meth_PySemsB2BDialog_onInvite(PyObject *sipSelf,PyObject *sipArgs)
{
    sipTrace(SIP_TRACE_METHODS,"meth_PySemsB2BDialog_onInvite()\n");

    int sipArgsParsed = 0;
    bool sipSelfWasArg = !sipSelf;

    {
        const AmSipRequest * a0;
        PySemsB2BDialog *sipCpp;

        if (sipParseArgs(&sipArgsParsed,sipArgs,"BJA",&sipSelf,sipClass_PySemsB2BDialog,&sipCpp,sipClass_AmSipRequest,&a0))
        {
            (sipSelfWasArg ? sipCpp->PySemsB2BDialog::onInvite(*a0) : sipCpp->onInvite(*a0));

            Py_INCREF(Py_None);
            return Py_None;
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipArgsParsed,sipNm_py_sems_lib_PySemsB2BDialog,sipNm_py_sems_lib_onInvite);

    return NULL;
}


extern "C" {static PyObject *meth_PySemsB2BDialog_onSessionStart(PyObject *, PyObject *);}
static PyObject *meth_PySemsB2BDialog_onSessionStart(PyObject *sipSelf,PyObject *sipArgs)
{
    sipTrace(SIP_TRACE_METHODS,"meth_PySemsB2BDialog_onSessionStart()\n");

    int sipArgsParsed = 0;
    bool sipSelfWasArg = !sipSelf;

    {
        const AmSipRequest * a0;
        PySemsB2BDialog *sipCpp;

        if (sipParseArgs(&sipArgsParsed,sipArgs,"BJA",&sipSelf,sipClass_PySemsB2BDialog,&sipCpp,sipClass_AmSipRequest,&a0))
        {
            (sipSelfWasArg ? sipCpp->PySemsB2BDialog::onSessionStart(*a0) : sipCpp->onSessionStart(*a0));

            Py_INCREF(Py_None);
            return Py_None;
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipArgsParsed,sipNm_py_sems_lib_PySemsB2BDialog,sipNm_py_sems_lib_onSessionStart);

    return NULL;
}


extern "C" {static PyObject *meth_PySemsB2BDialog_onCancel(PyObject *, PyObject *);}
static PyObject *meth_PySemsB2BDialog_onCancel(PyObject *sipSelf,PyObject *sipArgs)
{
    sipTrace(SIP_TRACE_METHODS,"meth_PySemsB2BDialog_onCancel()\n");

    int sipArgsParsed = 0;
    bool sipSelfWasArg = !sipSelf;

    {
        PySemsB2BDialog *sipCpp;

        if (sipParseArgs(&sipArgsParsed,sipArgs,"B",&sipSelf,sipClass_PySemsB2BDialog,&sipCpp))
        {
            (sipSelfWasArg ? sipCpp->PySemsB2BDialog::onCancel() : sipCpp->onCancel());

            Py_INCREF(Py_None);
            return Py_None;
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipArgsParsed,sipNm_py_sems_lib_PySemsB2BDialog,sipNm_py_sems_lib_onCancel);

    return NULL;
}


extern "C" {static PyObject *meth_PySemsB2BDialog_onBye(PyObject *, PyObject *);}
static PyObject *meth_PySemsB2BDialog_onBye(PyObject *sipSelf,PyObject *sipArgs)
{
    sipTrace(SIP_TRACE_METHODS,"meth_PySemsB2BDialog_onBye()\n");

    int sipArgsParsed = 0;
    bool sipSelfWasArg = !sipSelf;

    {
        const AmSipRequest * a0;
        PySemsB2BDialog *sipCpp;

        if (sipParseArgs(&sipArgsParsed,sipArgs,"BJA",&sipSelf,sipClass_PySemsB2BDialog,&sipCpp,sipClass_AmSipRequest,&a0))
        {
            (sipSelfWasArg ? sipCpp->PySemsB2BDialog::onBye(*a0) : sipCpp->onBye(*a0));

            Py_INCREF(Py_None);
            return Py_None;
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipArgsParsed,sipNm_py_sems_lib_PySemsB2BDialog,sipNm_py_sems_lib_onBye);

    return NULL;
}


extern "C" {static PyObject *meth_PySemsB2BDialog_onDtmf(PyObject *, PyObject *);}
static PyObject *meth_PySemsB2BDialog_onDtmf(PyObject *sipSelf,PyObject *sipArgs)
{
    sipTrace(SIP_TRACE_METHODS,"meth_PySemsB2BDialog_onDtmf()\n");

    int sipArgsParsed = 0;
    bool sipSelfWasArg = !sipSelf;

    {
        int a0;
        int a1;
        PySemsB2BDialog *sipCpp;

        if (sipParseArgs(&sipArgsParsed,sipArgs,"Bii",&sipSelf,sipClass_PySemsB2BDialog,&sipCpp,&a0,&a1))
        {
            (sipSelfWasArg ? sipCpp->PySemsB2BDialog::onDtmf(a0,a1) : sipCpp->onDtmf(a0,a1));

            Py_INCREF(Py_None);
            return Py_None;
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipArgsParsed,sipNm_py_sems_lib_PySemsB2BDialog,sipNm_py_sems_lib_onDtmf);

    return NULL;
}


extern "C" {static PyObject *meth_PySemsB2BDialog_setInput(PyObject *, PyObject *);}
static PyObject *meth_PySemsB2BDialog_setInput(PyObject *sipSelf,PyObject *sipArgs)
{
    sipTrace(SIP_TRACE_METHODS,"meth_PySemsB2BDialog_setInput()\n");

    int sipArgsParsed = 0;

    {
        AmAudioFile * a0;
        PySemsB2BDialog *sipCpp;

        if (sipParseArgs(&sipArgsParsed,sipArgs,"BJ@",&sipSelf,sipClass_PySemsB2BDialog,&sipCpp,sipClass_AmAudioFile,&a0))
        {
            sipCpp->setInput(a0);

            Py_INCREF(Py_None);
            return Py_None;
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipArgsParsed,sipNm_py_sems_lib_PySemsB2BDialog,sipNm_py_sems_lib_setInput);

    return NULL;
}


extern "C" {static PyObject *meth_PySemsB2BDialog_setOutput(PyObject *, PyObject *);}
static PyObject *meth_PySemsB2BDialog_setOutput(PyObject *sipSelf,PyObject *sipArgs)
{
    sipTrace(SIP_TRACE_METHODS,"meth_PySemsB2BDialog_setOutput()\n");

    int sipArgsParsed = 0;

    {
        AmAudioFile * a0;
        PySemsB2BDialog *sipCpp;

        if (sipParseArgs(&sipArgsParsed,sipArgs,"BJ@",&sipSelf,sipClass_PySemsB2BDialog,&sipCpp,sipClass_AmAudioFile,&a0))
        {
            sipCpp->setOutput(a0);

            Py_INCREF(Py_None);
            return Py_None;
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipArgsParsed,sipNm_py_sems_lib_PySemsB2BDialog,sipNm_py_sems_lib_setOutput);

    return NULL;
}


extern "C" {static PyObject *meth_PySemsB2BDialog_setInOut(PyObject *, PyObject *);}
static PyObject *meth_PySemsB2BDialog_setInOut(PyObject *sipSelf,PyObject *sipArgs)
{
    sipTrace(SIP_TRACE_METHODS,"meth_PySemsB2BDialog_setInOut()\n");

    int sipArgsParsed = 0;

    {
        AmAudioFile * a0;
        AmAudioFile * a1;
        PySemsB2BDialog *sipCpp;

        if (sipParseArgs(&sipArgsParsed,sipArgs,"BJ@J@",&sipSelf,sipClass_PySemsB2BDialog,&sipCpp,sipClass_AmAudioFile,&a0,sipClass_AmAudioFile,&a1))
        {
            sipCpp->setInOut(a0,a1);

            Py_INCREF(Py_None);
            return Py_None;
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipArgsParsed,sipNm_py_sems_lib_PySemsB2BDialog,sipNm_py_sems_lib_setInOut);

    return NULL;
}


extern "C" {static PyObject *meth_PySemsB2BDialog_process(PyObject *, PyObject *);}
static PyObject *meth_PySemsB2BDialog_process(PyObject *sipSelf,PyObject *sipArgs)
{
    sipTrace(SIP_TRACE_METHODS,"meth_PySemsB2BDialog_process()\n");

    int sipArgsParsed = 0;
    bool sipSelfWasArg = !sipSelf;

    {
        AmEvent * a0;
        PySemsB2BDialog *sipCpp;

        if (sipParseArgs(&sipArgsParsed,sipArgs,"BJ@",&sipSelf,sipClass_PySemsB2BDialog,&sipCpp,sipClass_AmEvent,&a0))
        {
            (sipSelfWasArg ? sipCpp->PySemsB2BDialog::process(a0) : sipCpp->process(a0));

            Py_INCREF(Py_None);
            return Py_None;
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipArgsParsed,sipNm_py_sems_lib_PySemsB2BDialog,sipNm_py_sems_lib_process);

    return NULL;
}


/* Cast a pointer to a type somewhere in its superclass hierarchy. */
extern "C" {static void *cast_PySemsB2BDialog(void *, sipWrapperType *);}
static void *cast_PySemsB2BDialog(void *ptr,sipWrapperType *targetClass)
{
    if (targetClass == sipClass_PySemsB2BDialog)
        return ptr;

    return NULL;
}


/* Call the instance's destructor. */
extern "C" {static void release_PySemsB2BDialog(void *, int);}
static void release_PySemsB2BDialog(void *ptr,int state)
{
    if (state & SIP_DERIVED_CLASS)
        delete reinterpret_cast<sipPySemsB2BDialog *>(ptr);
    else
        delete reinterpret_cast<PySemsB2BDialog *>(ptr);
}


extern "C" {static void dealloc_PySemsB2BDialog(sipWrapper *);}
static void dealloc_PySemsB2BDialog(sipWrapper *sipSelf)
{
    sipTrace(SIP_TRACE_DEALLOCS,"dealloc_PySemsB2BDialog()\n");

    if (sipIsDerived(sipSelf))
        reinterpret_cast<sipPySemsB2BDialog *>(sipSelf->u.cppPtr)->sipPySelf = NULL;

    if (sipIsPyOwned(sipSelf))
    {
#line 17 "PySemsB2BDialog.sip"
    fprintf(stderr,"------------- dealloc_PySemsB2BDialog ----------\n");
#line 500 "sippy_sems_libPySemsB2BDialog.cpp"

        release_PySemsB2BDialog(sipSelf->u.cppPtr,sipSelf->flags);
    }
}


extern "C" {static void *init_PySemsB2BDialog(sipWrapper *, PyObject *, sipWrapper **, int *);}
static void *init_PySemsB2BDialog(sipWrapper *sipSelf,PyObject *sipArgs,sipWrapper **,int *sipArgsParsed)
{
    sipPySemsB2BDialog *sipCpp = 0;

    sipTrace(SIP_TRACE_INITS,"init_PySemsB2BDialog()\n");

    if (!sipCpp)
    {
        if (sipParseArgs(sipArgsParsed,sipArgs,""))
        {
            sipCpp = new sipPySemsB2BDialog();
        }
    }

    if (sipCpp)
        sipCpp->sipPySelf = sipSelf;

    return sipCpp;
}


extern "C" {static PyObject *var_PySemsB2BDialog_dlg(PyObject *, PyObject *);}
static PyObject *var_PySemsB2BDialog_dlg(PyObject *sipSelf,PyObject *sipPy)
{
    int sipIsErr = 0;
   AmSipDialog *sipVal;
    PySemsB2BDialog *sipCpp = reinterpret_cast<PySemsB2BDialog *>(sipGetCppPtr((sipWrapper *)sipSelf,sipClass_PySemsB2BDialog));

    if (!sipCpp)
        return NULL;

    if (sipPy == NULL)
    {
        sipVal = &sipCpp->dlg;

        sipPy = sipConvertFromInstance(sipVal,sipClass_AmSipDialog,NULL);

        return sipPy;
    }

    sipVal = reinterpret_cast<AmSipDialog *>(sipForceConvertToInstance(sipPy,sipClass_AmSipDialog,NULL,SIP_NOT_NONE,NULL,&sipIsErr));

    if (sipIsErr)
        return NULL;

    sipCpp->dlg = *sipVal;

    Py_INCREF(Py_None);
    return Py_None;
}

PyMethodDef variables_PySemsB2BDialog[] = {
    {sipNm_py_sems_lib_dlg, var_PySemsB2BDialog_dlg, 0, NULL},
    {0, 0, 0, 0}
};


extern "C" {static void *forceConvertTo_PySemsB2BDialog(PyObject *, int *);}
static void *forceConvertTo_PySemsB2BDialog(PyObject *valobj,int *iserrp)
{
    if (*iserrp || valobj == NULL)
        return NULL;

    if (valobj == Py_None || sipIsSubClassInstance(valobj,sipClass_PySemsB2BDialog))
        return sipConvertToCpp(valobj,sipClass_PySemsB2BDialog,iserrp);

    sipBadClass(sipNm_py_sems_lib_PySemsB2BDialog);

    *iserrp = 1;

    return NULL;
}


static PyMethodDef methods_PySemsB2BDialog[] = {
    {sipNm_py_sems_lib_acceptAudio, meth_PySemsB2BDialog_acceptAudio, METH_VARARGS, NULL},
    {sipNm_py_sems_lib_onBye, meth_PySemsB2BDialog_onBye, METH_VARARGS, NULL},
    {sipNm_py_sems_lib_onCancel, meth_PySemsB2BDialog_onCancel, METH_VARARGS, NULL},
    {sipNm_py_sems_lib_onDtmf, meth_PySemsB2BDialog_onDtmf, METH_VARARGS, NULL},
    {sipNm_py_sems_lib_onInvite, meth_PySemsB2BDialog_onInvite, METH_VARARGS, NULL},
    {sipNm_py_sems_lib_onSessionStart, meth_PySemsB2BDialog_onSessionStart, METH_VARARGS, NULL},
    {sipNm_py_sems_lib_process, meth_PySemsB2BDialog_process, METH_VARARGS, NULL},
    {sipNm_py_sems_lib_setInOut, meth_PySemsB2BDialog_setInOut, METH_VARARGS, NULL},
    {sipNm_py_sems_lib_setInput, meth_PySemsB2BDialog_setInput, METH_VARARGS, NULL},
    {sipNm_py_sems_lib_setOutput, meth_PySemsB2BDialog_setOutput, METH_VARARGS, NULL},
    {sipNm_py_sems_lib_setStopped, meth_PySemsB2BDialog_setStopped, METH_VARARGS, NULL}
};


sipTypeDef sipType_py_sems_lib_PySemsB2BDialog = {
    0,
    0,
    "py_sems_lib.PySemsB2BDialog",
    0,
    {0, 0, 1},
    0,
    0,
    11, methods_PySemsB2BDialog,
    0, 0,
    variables_PySemsB2BDialog,
    init_PySemsB2BDialog,
    0,
    0,
    0,
    0,
    0,
    0,
    dealloc_PySemsB2BDialog,
    cast_PySemsB2BDialog,
    release_PySemsB2BDialog,
    forceConvertTo_PySemsB2BDialog,
    0,
    0,
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    0
};
