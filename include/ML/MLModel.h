/*******************************************************************************
 * Author: Mingxu Hu
 * Dependency:
 * Test:
 * Execution:
 * Description:
 *
 * Manual:
 * ****************************************************************************/

#ifndef ML_MODEL_H
#define ML_MODEL_H

#include <armadillo>

#include "Typedef.h"

#include "Image.h"
#include "Volume.h"
#include "Parallel.h"
#include "Filter.h"
#include "Spectrum.h"
#include "Projector.h"
#include "Reconstructor.h"

using namespace std;
using namespace arma;

#define FOR_EACH_CLASS \
    for (int i = 0; i < size(); i++)

class MLModel : public Parallel
{
    private:

        vector<Volume> _ref;
        /* references in Fourier space */

        vector<vec> _FSC;

        vector<vec> _SNR;

        vector<Projector> _proj;

        vector<Reconstructor> _reco;

        int _r;
        /* radius of calculating FSC and SNR */

        int _pf;
        /* padding factor */

        double _pixelSize;
        /* pixel size of 2D images */

        double _a;

        double _alpha;

    public:

        MLModel();

        void appendRef(const Volume& ref);

        int K() const;

        int size() const;

        int r() const;

        void setR(const int r);

        Reconstructor& reco(const int i = 0);

        void BcastFSC();

        void lowPassRef(const double thres,
                        const double ew);
        /* perform a low pass filtering on each reference */

        void refreshSNR();

        int resolutionP(const int i) const;
        /* get the resolution of _ref[i] */

        int resolutionP() const;
        /* get the highest resolution among all references */

        double resolutionA(const int i) const;
        /* get the resolution of _ref[i] */
        
        double resolutionA() const;
        /* get the highest resolution among all references */

        void refreshProjector();

        void updateR();
        /* increase _r according to wether FSC is high than 0.2 at current _r */
};

#endif // ML_MODEL_H
