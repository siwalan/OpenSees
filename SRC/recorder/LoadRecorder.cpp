/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// Written: @siwalan
// Created On: 2022-07-06 (YYYY-MM-DD)
// Last Revised: 2022-07-06 (YYYY-MM-DD)
// 
// Description: This file contain the definitions for the LoadRecorder recorder
// At this time the LoadRecorder can read two types of LoadPattern, that is UniformExcitation and Nodal Load
// Input: recorder LoadRecorder -file filename.txt -time -dt $double -pattern $integer
// Options: -time and -dt are optional
// 
// Note: At this current time: -pattern can only take a single load Pattern
// 
// If the pattern contain multiple nodal load. The result will be outputed as
// time (If selected) NodalLoad1_DOF1 NodalLoad1_DOF2 ... NodalLoad1_DOFn NodalLoad2_DOF1 ... NodalLoadJ_DOFN
// 

#include <LoadRecorder.h> // This is Me

// OpenSees Library
#include <Domain.h>
#include <OPS_Globals.h>
#include <Vector.h> 
#include <ID.h>
#include <FE_Datastore.h>
#include <NodalLoad.h>
#include <NodalLoadIter.h>
#include <MapOfTaggedObjects.h>

// LoadPatern Library
#include <LoadPattern.h>

// C++ Library
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <iomanip>
using std::ios;


LoadRecorder::LoadRecorder(int loadID, Domain &domainHandler, OPS_Stream &outputHandler, double deltaT, double relDeltaTol, bool echoTimeFlag) :Recorder(RECORDER_TAGS_LoadRecorder), loadID(loadID), theDomain(&domainHandler), theOutput(&outputHandler), deltaT(deltaT), relDeltaTTol(relDeltaTol), nextTimeStampToRecord(0.0), echoTimeFlag(echoTimeFlag), data(0), loadType(0)

{
    LoadPattern *theLoadPattern = domainHandler.getLoadPattern(loadID);
    if (theLoadPattern == NULL) {
        opserr << "WARNING LoadRecorder::LoadRecorder() - No load pattern with tag: " << loadID << " exist in the Domain\n";
        exit(-1);
    }

    int numDbColumns = 0;
    if (echoTimeFlag == true) {
        theOutput->tag("TimeOutput");
        theOutput->attr("ResponseType", "time");
        theOutput->endTag();
        numDbColumns += 1;
    }

    NodalLoad* theNodalLoad = 0;
    NodalLoadIter &theNodalIter = theLoadPattern->getNodalLoads();

    int numberOfNodalLoad = 0;

    while ((theNodalLoad = theNodalIter()) != 0) {
        numberOfNodalLoad += (theNodalLoad->getNodalLoadDOF());
    }

    if (numberOfNodalLoad > 0) {
        numDbColumns += numberOfNodalLoad;
        loadType = 0;
    } else {
        // If there is no nodal load, assume that this load Pattern is only Excitation Load.
        // Thus only one Result.
        numDbColumns += 1;
        loadType = 1;
    }

    data = new Vector(numDbColumns);
    theOutput->tag("Data");
}

LoadRecorder::~LoadRecorder()
{
    if (data != 0)
        delete data;

    theOutput->endTag(); // Data
    theOutput->endTag(); // OpenSeesOutput

    if (theOutput != 0)
        delete theOutput;
}

int LoadRecorder::record(int commitTag, double timeStamp)
{
    int result = 0;
    // where 1.0e-5 is the maximum reliable ratio between analysis time step and deltaT
    // and provides tolerance for floating point precision (see floating-point-tolerance-for-recorder-time-step.md)
    if (deltaT == 0.0 || timeStamp - nextTimeStampToRecord >= -deltaT * relDeltaTTol) {

        if (deltaT != 0.0)
            nextTimeStampToRecord = timeStamp + deltaT;

        // print out the pseudo time if requested
        int counter = 0;
        if (echoTimeFlag == true) {
            (*data)(counter++) = timeStamp;
        }

        LoadPattern* theLoadPattern = theDomain->getLoadPattern(loadID);

        if (loadType == 1) {
            (*data)(counter++) = theLoadPattern->getLoadFactor();
        }
        else if (loadType == 0) {
            double loadPatternLoadFactor = theLoadPattern->getLoadFactor();

            NodalLoad* theNodalLoad = 0;
            NodalLoadIter& theNodalIter = theLoadPattern->getNodalLoads();

            while ((theNodalLoad = theNodalIter()) != 0) {
                int nDOF = (theNodalLoad->getNodalLoadDOF());
                for (int dofIterator = 0; dofIterator < nDOF; dofIterator++) {
                    (*data)(counter++) = theNodalLoad->getNodalLoad(dofIterator) * loadPatternLoadFactor;
                }
            }

        }

    }

    theOutput->write(*data);

    // successful completion - return 0
    return result;
}

int LoadRecorder::restart(void)
{
    return 0;
}
