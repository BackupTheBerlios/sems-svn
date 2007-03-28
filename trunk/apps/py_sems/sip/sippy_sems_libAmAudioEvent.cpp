/*
 * Interface wrapper code.
 *
 * Generated by SIP 4.5.2 (4.5.2) on Thu Mar 29 00:29:43 2007
 */

#include "sipAPIpy_sems_lib.h"
#include "sippy_sems_libAmAudioEvent.h"

#include "sippy_sems_libAmEvent.h"


sipAmAudioEvent::sipAmAudioEvent(int a0): AmAudioEvent(a0), sipPySelf(0)
{
    sipTrace(SIP_TRACE_CTORS,"sipAmAudioEvent::sipAmAudioEvent(int) (this=0x%08x)\n",this);

    sipCommonCtor(NULL,0);
}

sipAmAudioEvent::sipAmAudioEvent(const AmAudioEvent& a0): AmAudioEvent(a0), sipPySelf(0)
{
    sipTrace(SIP_TRACE_CTORS,"sipAmAudioEvent::sipAmAudioEvent(const AmAudioEvent&) (this=0x%08x)\n",this);

    sipCommonCtor(NULL,0);
}

sipAmAudioEvent::~sipAmAudioEvent()
{
    sipTrace(SIP_TRACE_DTORS,"sipAmAudioEvent::~sipAmAudioEvent() (this=0x%08x)\n",this);

    sipCommonDtor(sipPySelf);
}


/* Cast a pointer to a type somewhere in its superclass hierarchy. */
extern "C" {static void *cast_AmAudioEvent(void *, sipWrapperType *);}
static void *cast_AmAudioEvent(void *ptr,sipWrapperType *targetClass)
{
    void *res;

    if (targetClass == sipClass_AmAudioEvent)
        return ptr;

    if ((res = sipCast_AmEvent((AmEvent *)(AmAudioEvent *)ptr,targetClass)) != NULL)
        return res;

    return NULL;
}


/* Call the instance's destructor. */
extern "C" {static void release_AmAudioEvent(void *, int);}
static void release_AmAudioEvent(void *ptr,int state)
{
    if (state & SIP_DERIVED_CLASS)
        delete reinterpret_cast<sipAmAudioEvent *>(ptr);
    else
        delete reinterpret_cast<AmAudioEvent *>(ptr);
}


extern "C" {static void dealloc_AmAudioEvent(sipWrapper *);}
static void dealloc_AmAudioEvent(sipWrapper *sipSelf)
{
    sipTrace(SIP_TRACE_DEALLOCS,"dealloc_AmAudioEvent()\n");

    if (sipIsDerived(sipSelf))
        reinterpret_cast<sipAmAudioEvent *>(sipSelf->u.cppPtr)->sipPySelf = NULL;

    if (sipIsPyOwned(sipSelf))
    {
        release_AmAudioEvent(sipSelf->u.cppPtr,sipSelf->flags);
    }
}


extern "C" {static void *init_AmAudioEvent(sipWrapper *, PyObject *, sipWrapper **, int *);}
static void *init_AmAudioEvent(sipWrapper *sipSelf,PyObject *sipArgs,sipWrapper **,int *sipArgsParsed)
{
    sipAmAudioEvent *sipCpp = 0;

    sipTrace(SIP_TRACE_INITS,"init_AmAudioEvent()\n");

    if (!sipCpp)
    {
        int a0;

        if (sipParseArgs(sipArgsParsed,sipArgs,"i",&a0))
        {
            sipCpp = new sipAmAudioEvent(a0);
        }
    }

    if (!sipCpp)
    {
        const AmAudioEvent * a0;

        if (sipParseArgs(sipArgsParsed,sipArgs,"JA",sipClass_AmAudioEvent,&a0))
        {
            sipCpp = new sipAmAudioEvent(*a0);
        }
    }

    if (sipCpp)
        sipCpp->sipPySelf = sipSelf;

    return sipCpp;
}


extern "C" {static void *forceConvertTo_AmAudioEvent(PyObject *, int *);}
static void *forceConvertTo_AmAudioEvent(PyObject *valobj,int *iserrp)
{
    if (*iserrp || valobj == NULL)
        return NULL;

    if (valobj == Py_None || sipIsSubClassInstance(valobj,sipClass_AmAudioEvent))
        return sipConvertToCpp(valobj,sipClass_AmAudioEvent,iserrp);

    sipBadClass(sipNm_py_sems_lib_AmAudioEvent);

    *iserrp = 1;

    return NULL;
}


/* Define this type's super-types. */
static sipEncodedClassDef supers_AmAudioEvent[] = {{6, 255, 1}};

static sipEnumMemberDef enummembers_AmAudioEvent[] = {
    {sipNm_py_sems_lib_cleared, AmAudioEvent::cleared, 1},
    {sipNm_py_sems_lib_noAudio, AmAudioEvent::noAudio, 1},
};


sipTypeDef sipType_py_sems_lib_AmAudioEvent = {
    0,
    SIP_TYPE_SCC,
    "py_sems_lib.AmAudioEvent",
    0,
    {0, 0, 1},
    supers_AmAudioEvent,
    0,
    0, 0,
    2, enummembers_AmAudioEvent,
    0,
    init_AmAudioEvent,
    0,
    0,
    0,
    0,
    0,
    0,
    dealloc_AmAudioEvent,
    cast_AmAudioEvent,
    release_AmAudioEvent,
    forceConvertTo_AmAudioEvent,
    0,
    0,
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    0
};
