/*
 * Interface header file.
 *
 * Generated by SIP 4.5.2 (4.5.2) on Mon Feb 18 16:33:22 2008
 */

#ifndef _py_sems_libPySemsB2ABDialog_h
#define _py_sems_libPySemsB2ABDialog_h

#line 5 "PySemsB2ABDialog.sip"
#include "../PySemsB2ABDialog.h"
#include "../../../core/AmB2ABSession.h"
#line 14 "sippy_sems_libPySemsB2ABDialog.h"

#define sipClass_PySemsB2ABDialog             sipModuleAPI_py_sems_lib.em_types[3]
#define sipCast_PySemsB2ABDialog              sipType_py_sems_lib_PySemsB2ABDialog.td_cast
#define sipForceConvertTo_PySemsB2ABDialog    sipType_py_sems_lib_PySemsB2ABDialog.td_fcto

extern sipTypeDef sipType_py_sems_lib_PySemsB2ABDialog;


class sipPySemsB2ABDialog : public PySemsB2ABDialog
{
public:
    sipPySemsB2ABDialog();
    virtual ~sipPySemsB2ABDialog();

    /*
     * There is a public method for every protected method visible from
     * this class.
     */
    AmB2ABCalleeSession * sipProtectVirt_createCalleeSession(bool);
    void sipProtectVirt_onB2ABEvent(bool,PySemsB2ABEvent *);
    void sipProtectVirt_relayEvent(bool,PySemsB2ABEvent *);

    /*
     * There is a protected method for every virtual method visible from
     * this class.
     */
protected:
    void onInvite(const AmSipRequest&);
    void onSessionStart(const AmSipRequest&);
    void onCancel();
    void onBye(const AmSipRequest&);
    void onDtmf(int,int);
    void process(AmEvent *);
    AmB2ABCalleeSession * createCalleeSession();
    void onB2ABEvent(PySemsB2ABEvent *);
    void relayEvent(PySemsB2ABEvent *);

public:
    sipWrapper *sipPySelf;

private:
    sipPySemsB2ABDialog(const sipPySemsB2ABDialog &);
    sipPySemsB2ABDialog &operator = (const sipPySemsB2ABDialog &);

    sipMethodCache sipPyMethods[9];
};

#endif
