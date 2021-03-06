/*
 * Interface wrapper code.
 *
 * Generated by SIP 4.5.2 (4.5.2) on Mon Dec  8 15:20:56 2008
 */

#include "sipAPIpy_sems_lib.h"
#include "sippy_sems_libPySemsB2ABCalleeDialog.h"

#include "sippy_sems_libAmB2ABCalleeSession.h"
#include "sippy_sems_libstring.h"
#include "sippy_sems_libAmSessionAudioConnector.h"
#include "sippy_sems_libPySemsB2ABCalleeDialog.h"
#include "sippy_sems_libPySemsB2ABEvent.h"
#include "sippy_sems_libAmAudioFile.h"


sipPySemsB2ABCalleeDialog::sipPySemsB2ABCalleeDialog(const string& a0,AmSessionAudioConnector *a1): PySemsB2ABCalleeDialog(a0,a1), sipPySelf(0)
{
    sipTrace(SIP_TRACE_CTORS,"sipPySemsB2ABCalleeDialog::sipPySemsB2ABCalleeDialog(const string&,AmSessionAudioConnector *) (this=0x%08x)\n",this);

    sipCommonCtor(sipPyMethods,1);
}

sipPySemsB2ABCalleeDialog::~sipPySemsB2ABCalleeDialog()
{
    sipTrace(SIP_TRACE_DTORS,"sipPySemsB2ABCalleeDialog::~sipPySemsB2ABCalleeDialog() (this=0x%08x)\n",this);

    sipCommonDtor(sipPySelf);
}

void sipPySemsB2ABCalleeDialog::onPyB2ABEvent(PySemsB2ABEvent *a0)
{
    sipTrace(SIP_TRACE_CATCHERS,"void sipPySemsB2ABCalleeDialog::onPyB2ABEvent(PySemsB2ABEvent *) (this=0x%08x)\n",this);

    extern void sipVH_py_sems_lib_0(sip_gilstate_t,PyObject *,PySemsB2ABEvent *);

    sip_gilstate_t sipGILState;
    PyObject *meth;

    meth = sipIsPyMethod(&sipGILState,&sipPyMethods[0],sipPySelf,NULL,sipNm_py_sems_lib_onPyB2ABEvent);

    if (!meth)
    {
        PySemsB2ABCalleeDialog::onPyB2ABEvent(a0);
        return;
    }

    sipVH_py_sems_lib_0(sipGILState,meth,a0);
}

void sipPySemsB2ABCalleeDialog::sipProtectVirt_onPyB2ABEvent(bool sipSelfWasArg,PySemsB2ABEvent *a0)
{
    (sipSelfWasArg ? PySemsB2ABCalleeDialog::onPyB2ABEvent(a0) : onPyB2ABEvent(a0));
}


extern "C" {static PyObject *meth_PySemsB2ABCalleeDialog_onPyB2ABEvent(PyObject *, PyObject *);}
static PyObject *meth_PySemsB2ABCalleeDialog_onPyB2ABEvent(PyObject *sipSelf,PyObject *sipArgs)
{
    sipTrace(SIP_TRACE_METHODS,"meth_PySemsB2ABCalleeDialog_onPyB2ABEvent()\n");

    int sipArgsParsed = 0;
    bool sipSelfWasArg = !sipSelf;

    {
        PySemsB2ABEvent * a0;
        sipPySemsB2ABCalleeDialog *sipCpp;

        if (sipParseArgs(&sipArgsParsed,sipArgs,"pJD",&sipSelf,sipClass_PySemsB2ABCalleeDialog,&sipCpp,sipClass_PySemsB2ABEvent,&a0))
        {
            sipCpp->sipProtectVirt_onPyB2ABEvent(sipSelfWasArg,a0);

            Py_INCREF(Py_None);
            return Py_None;
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipArgsParsed,sipNm_py_sems_lib_PySemsB2ABCalleeDialog,sipNm_py_sems_lib_onPyB2ABEvent);

    return NULL;
}


/* Cast a pointer to a type somewhere in its superclass hierarchy. */
extern "C" {static void *cast_PySemsB2ABCalleeDialog(void *, sipWrapperType *);}
static void *cast_PySemsB2ABCalleeDialog(void *ptr,sipWrapperType *targetClass)
{
    void *res;

    if (targetClass == sipClass_PySemsB2ABCalleeDialog)
        return ptr;

    if ((res = sipCast_AmB2ABCalleeSession((AmB2ABCalleeSession *)(PySemsB2ABCalleeDialog *)ptr,targetClass)) != NULL)
        return res;

    return NULL;
}


/* Call the instance's destructor. */
extern "C" {static void release_PySemsB2ABCalleeDialog(void *, int);}
static void release_PySemsB2ABCalleeDialog(void *ptr,int state)
{
    if (state & SIP_DERIVED_CLASS)
        delete reinterpret_cast<sipPySemsB2ABCalleeDialog *>(ptr);
    else
        delete reinterpret_cast<PySemsB2ABCalleeDialog *>(ptr);
}


extern "C" {static void dealloc_PySemsB2ABCalleeDialog(sipWrapper *);}
static void dealloc_PySemsB2ABCalleeDialog(sipWrapper *sipSelf)
{
    sipTrace(SIP_TRACE_DEALLOCS,"dealloc_PySemsB2ABCalleeDialog()\n");

    if (sipIsDerived(sipSelf))
        reinterpret_cast<sipPySemsB2ABCalleeDialog *>(sipSelf->u.cppPtr)->sipPySelf = NULL;

    if (sipIsPyOwned(sipSelf))
    {
        release_PySemsB2ABCalleeDialog(sipSelf->u.cppPtr,sipSelf->flags);
    }
}


extern "C" {static void *init_PySemsB2ABCalleeDialog(sipWrapper *, PyObject *, sipWrapper **, int *);}
static void *init_PySemsB2ABCalleeDialog(sipWrapper *sipSelf,PyObject *sipArgs,sipWrapper **,int *sipArgsParsed)
{
    sipPySemsB2ABCalleeDialog *sipCpp = 0;

    sipTrace(SIP_TRACE_INITS,"init_PySemsB2ABCalleeDialog()\n");

    if (!sipCpp)
    {
        const string * a0;
        int a0State = 0;
        AmSessionAudioConnector * a1;

        if (sipParseArgs(sipArgsParsed,sipArgs,"M1J@",sipMappedType_string,&a0,&a0State,sipClass_AmSessionAudioConnector,&a1))
        {
            sipCpp = new sipPySemsB2ABCalleeDialog(*a0,a1);

            sipReleaseMappedType(const_cast<string *>(a0),sipMappedType_string,a0State);
        }
    }

    if (sipCpp)
        sipCpp->sipPySelf = sipSelf;

    return sipCpp;
}


extern "C" {static void *forceConvertTo_PySemsB2ABCalleeDialog(PyObject *, int *);}
static void *forceConvertTo_PySemsB2ABCalleeDialog(PyObject *valobj,int *iserrp)
{
    if (*iserrp || valobj == NULL)
        return NULL;

    if (valobj == Py_None || sipIsSubClassInstance(valobj,sipClass_PySemsB2ABCalleeDialog))
        return sipConvertToCpp(valobj,sipClass_PySemsB2ABCalleeDialog,iserrp);

    sipBadClass(sipNm_py_sems_lib_PySemsB2ABCalleeDialog);

    *iserrp = 1;

    return NULL;
}


/* Define this type's super-types. */
static sipEncodedClassDef supers_PySemsB2ABCalleeDialog[] = {{1, 255, 1}};


static PyMethodDef methods_PySemsB2ABCalleeDialog[] = {
    {sipNm_py_sems_lib_onPyB2ABEvent, meth_PySemsB2ABCalleeDialog_onPyB2ABEvent, METH_VARARGS, NULL}
};


sipTypeDef sipType_py_sems_lib_PySemsB2ABCalleeDialog = {
    0,
    0,
    "py_sems_lib.PySemsB2ABCalleeDialog",
    0,
    {0, 0, 1},
    supers_PySemsB2ABCalleeDialog,
    0,
    1, methods_PySemsB2ABCalleeDialog,
    0, 0,
    0,
    init_PySemsB2ABCalleeDialog,
    0,
    0,
    0,
    0,
    0,
    0,
    dealloc_PySemsB2ABCalleeDialog,
    cast_PySemsB2ABCalleeDialog,
    release_PySemsB2ABCalleeDialog,
    forceConvertTo_PySemsB2ABCalleeDialog,
    0,
    0,
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    0
};
