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
                                                                        
                                                                       
#ifndef LoadRecorder_h
#define LoadRecorder_h

#include <Recorder.h>
#include <fstream>

#include <Information.h>
#include <ID.h>

#include <Vector.h>
#include <TimeSeries.h>

class Domain;
class Vector;
class FE_Datastore;
class LoadPattern;

class LoadRecorder: public Recorder
{
  public:
    LoadRecorder(
        ID &loadIDs,
        Domain &theDomain,
        OPS_Stream &theOutput,
        double deltaT,
        double relDeltaTTol,
        bool echoTimeFlag
    );

    ~LoadRecorder();

    int record(int commitTag, double timeStamp);
    int restart(void);

  protected:

  private:

     ID loadIDs;
     int loadIDSize;
     Vector *loadTypes;
     Vector *data;

    // loadType
    // loadType = 0 (Nodal Load)
    // loadType = 1 (Excitation Load)

    Domain *theDomain; // This is logical because there is only one single domain
    OPS_Stream *theOutput; // For this... cannot comment much.

    bool echoTimeFlag;   // flag indicating whether time to be included in o/p

    double deltaT;
    double relDeltaTTol;
    double nextTimeStampToRecord;
    
};

#endif
