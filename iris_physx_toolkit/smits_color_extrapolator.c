/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    smits_color_extrapolator.c

Abstract:

    Implements an RGB color extrapolator.

--*/

#include "iris_physx_toolkit/smits_color_extrapolator.h"

#include <stdalign.h>
#include <stdlib.h>
#include <string.h>

#include "iris_physx_toolkit/interpolated_spectrum.h"

//
// Defines
//

#define NUM_SPECTRAL_SAMPLES 32

//
// Types
//

typedef struct _RGB_COLOR_EXTRAPOLATOR {
    _Field_size_full_(num_samples) float_t *wavelengths;
    _Field_size_full_(num_samples) float_t *spectrum_white;
    _Field_size_full_(num_samples) float_t *spectrum_cyan;
    _Field_size_full_(num_samples) float_t *spectrum_magenta;
    _Field_size_full_(num_samples) float_t *spectrum_yellow;
    _Field_size_full_(num_samples) float_t *spectrum_red;
    _Field_size_full_(num_samples) float_t *spectrum_green;
    _Field_size_full_(num_samples) float_t *spectrum_blue;
    _Field_size_full_(num_samples) float_t *reflector_white;
    _Field_size_full_(num_samples) float_t *reflector_cyan;
    _Field_size_full_(num_samples) float_t *reflector_magenta;
    _Field_size_full_(num_samples) float_t *reflector_yellow;
    _Field_size_full_(num_samples) float_t *reflector_red;
    _Field_size_full_(num_samples) float_t *reflector_green;
    _Field_size_full_(num_samples) float_t *reflector_blue;
    size_t num_samples;
} RGB_COLOR_EXTRAPOLATOR, *PRGB_COLOR_EXTRAPOLATOR;

typedef const RGB_COLOR_EXTRAPOLATOR *PCRGB_COLOR_EXTRAPOLATOR;

//
// Static Data
//

static const float_t sample_wavelengths[NUM_SPECTRAL_SAMPLES] = {
    (float_t)380.000000,
    (float_t)390.967743,
    (float_t)401.935486,
    (float_t)412.903229,
    (float_t)423.870972,
    (float_t)434.838715,
    (float_t)445.806458,
    (float_t)456.774200,
    (float_t)467.741943,
    (float_t)478.709686,
    (float_t)489.677429,
    (float_t)500.645172,
    (float_t)511.612915,
    (float_t)522.580627,
    (float_t)533.548340,
    (float_t)544.516052,
    (float_t)555.483765,
    (float_t)566.451477,
    (float_t)577.419189,
    (float_t)588.386902,
    (float_t)599.354614,
    (float_t)610.322327,
    (float_t)621.290039,
    (float_t)632.257751,
    (float_t)643.225464,
    (float_t)654.193176,
    (float_t)665.160889,
    (float_t)676.128601,
    (float_t)687.096313,
    (float_t)698.064026,
    (float_t)709.031738,
    (float_t)720.000000
};

static const float_t spectrum_white[NUM_SPECTRAL_SAMPLES] = {
    (float_t)1.1565232050369776e+00,
    (float_t)1.1567225000119139e+00,
    (float_t)1.1566203150243823e+00,
    (float_t)1.1555782088080084e+00,
    (float_t)1.1562175509215700e+00,
    (float_t)1.1567674012207332e+00,
    (float_t)1.1568023194808630e+00,
    (float_t)1.1567677445485520e+00,
    (float_t)1.1563563182952830e+00,
    (float_t)1.1567054702510189e+00,
    (float_t)1.1565134139372772e+00,
    (float_t)1.1564336176499312e+00,
    (float_t)1.1568023181530034e+00,
    (float_t)1.1473147688514642e+00,
    (float_t)1.1339317140561065e+00,
    (float_t)1.1293876490671435e+00,
    (float_t)1.1290515328639648e+00,
    (float_t)1.0504864823782283e+00,
    (float_t)1.0459696042230884e+00,
    (float_t)9.9366687168595691e-01,
    (float_t)9.5601669265393940e-01,
    (float_t)9.2467482033511805e-01,
    (float_t)9.1499944702051761e-01,
    (float_t)8.9939467658453465e-01,
    (float_t)8.9542520751331112e-01,
    (float_t)8.8870566693814745e-01,
    (float_t)8.8222843814228114e-01,
    (float_t)8.7998311373826676e-01,
    (float_t)8.7635244612244578e-01,
    (float_t)8.8000368331709111e-01,
    (float_t)8.8065665428441120e-01,
    (float_t)8.8304706460276905e-01
};

static const float_t spectrum_cyan[NUM_SPECTRAL_SAMPLES] = {
    (float_t)1.1334479663682135e+00,
    (float_t)1.1266762330194116e+00,
    (float_t)1.1346827504710164e+00,
    (float_t)1.1357395805744794e+00,
    (float_t)1.1356371830149636e+00,
    (float_t)1.1361152989346193e+00,
    (float_t)1.1362179057706772e+00,
    (float_t)1.1364819652587022e+00,
    (float_t)1.1355107110714324e+00,
    (float_t)1.1364060941199556e+00,
    (float_t)1.1360363621722465e+00,
    (float_t)1.1360122641141395e+00,
    (float_t)1.1354266882467030e+00,
    (float_t)1.1363099407179136e+00,
    (float_t)1.1355450412632506e+00,
    (float_t)1.1353732327376378e+00,
    (float_t)1.1349496420726002e+00,
    (float_t)1.1111113947168556e+00,
    (float_t)9.0598740429727143e-01,
    (float_t)6.1160780787465330e-01,
    (float_t)2.9539752170999634e-01,
    (float_t)9.5954200671150097e-02,
    (float_t)-1.1650792030826267e-02,
    (float_t)-1.2144633073395025e-02,
    (float_t)-1.1148167569748318e-02,
    (float_t)-1.1997606668458151e-02,
    (float_t)-5.0506855475394852e-03,
    (float_t)-7.9982745819542154e-03,
    (float_t)-9.4722817708236418e-03,
    (float_t)-5.5329541006658815e-03,
    (float_t)-4.5428914028274488e-03,
    (float_t)-1.2541015360921132e-02
};

static const float_t spectrum_magenta[NUM_SPECTRAL_SAMPLES] = {
    (float_t)1.0371892935878366e+00,
    (float_t)1.0587542891035364e+00,
    (float_t)1.0767271213688903e+00,
    (float_t)1.0762706844110288e+00,
    (float_t)1.0795289105258212e+00,
    (float_t)1.0743644742950074e+00,
    (float_t)1.0727028691194342e+00,
    (float_t)1.0732447452056488e+00,
    (float_t)1.0823760816041414e+00,
    (float_t)1.0840545681409282e+00,
    (float_t)9.5607567526306658e-01,
    (float_t)5.5197896855064665e-01,
    (float_t)8.4191094887247575e-02,
    (float_t)8.7940070557041006e-05,
    (float_t)-2.3086408335071251e-03,
    (float_t)-1.1248136628651192e-03,
    (float_t)-7.7297612754989586e-11,
    (float_t)-2.7270769006770834e-04,
    (float_t)1.4466473094035592e-02,
    (float_t)2.5883116027169478e-01,
    (float_t)5.2907999827566732e-01,
    (float_t)9.0966624097105164e-01,
    (float_t)1.0690571327307956e+00,
    (float_t)1.0887326064796272e+00,
    (float_t)1.0637622289511852e+00,
    (float_t)1.0201812918094260e+00,
    (float_t)1.0262196688979945e+00,
    (float_t)1.0783085560613190e+00,
    (float_t)9.8333849623218872e-01,
    (float_t)1.0707246342802621e+00,
    (float_t)1.0634247770423768e+00,
    (float_t)1.0150875475729566e+00
};

static const float_t spectrum_yellow[NUM_SPECTRAL_SAMPLES] = {
    (float_t)2.7756958965811972e-03,
    (float_t)3.9673820990646612e-03,
    (float_t)-1.4606936788606750e-04,
    (float_t)3.6198394557748065e-04,
    (float_t)-2.5819258699309733e-04,
    (float_t)-5.0133191628082274e-05,
    (float_t)-2.4437242866157116e-04,
    (float_t)-7.8061419948038946e-05,
    (float_t)4.9690301207540921e-02,
    (float_t)4.8515973574763166e-01,
    (float_t)1.0295725854360589e+00,
    (float_t)1.0333210878457741e+00,
    (float_t)1.0368102644026933e+00,
    (float_t)1.0364884018886333e+00,
    (float_t)1.0365427939411784e+00,
    (float_t)1.0368595402854539e+00,
    (float_t)1.0365645405660555e+00,
    (float_t)1.0363938240707142e+00,
    (float_t)1.0367205578770746e+00,
    (float_t)1.0365239329446050e+00,
    (float_t)1.0361531226427443e+00,
    (float_t)1.0348785007827348e+00,
    (float_t)1.0042729660717318e+00,
    (float_t)8.4218486432354278e-01,
    (float_t)7.3759394894801567e-01,
    (float_t)6.5853154500294642e-01,
    (float_t)6.0531682444066282e-01,
    (float_t)5.9549794132420741e-01,
    (float_t)5.9419261278443136e-01,
    (float_t)5.6517682326634266e-01,
    (float_t)5.6061186014968556e-01,
    (float_t)5.8228610381018719e-01
};

static const float_t spectrum_red[NUM_SPECTRAL_SAMPLES] = {
    (float_t)5.4711187157291841e-02,
    (float_t)5.5609066498303397e-02,
    (float_t)6.0755873790918236e-02,
    (float_t)5.6232948615962369e-02,
    (float_t)4.6169940535708678e-02,
    (float_t)3.8012808167818095e-02,
    (float_t)2.4424225756670338e-02,
    (float_t)3.8983580581592181e-03,
    (float_t)-5.6082252172734437e-04,
    (float_t)9.6493871255194652e-04,
    (float_t)3.7341198051510371e-04,
    (float_t)-4.3367389093135200e-04,
    (float_t)-9.3533962256892034e-05,
    (float_t)-1.2354967412842033e-04,
    (float_t)-1.4524548081687461e-04,
    (float_t)-2.0047691915543731e-04,
    (float_t)-4.9938587694693670e-04,
    (float_t)2.7255083540032476e-02,
    (float_t)1.6067405906297061e-01,
    (float_t)3.5069788873150953e-01,
    (float_t)5.7357465538418961e-01,
    (float_t)7.6392091890718949e-01,
    (float_t)8.9144466740381523e-01,
    (float_t)9.6394609909574891e-01,
    (float_t)9.8879464276016282e-01,
    (float_t)9.9897449966227203e-01,
    (float_t)9.8605140403564162e-01,
    (float_t)9.9532502805345202e-01,
    (float_t)9.7433478377305371e-01,
    (float_t)9.9134364616871407e-01,
    (float_t)9.8866287772174755e-01,
    (float_t)9.9713856089735531e-01
};

static const float_t spectrum_green[NUM_SPECTRAL_SAMPLES] = {
    (float_t)2.5168388755514630e-02,
    (float_t)3.9427438169423720e-02,
    (float_t)6.2059571596425793e-03,
    (float_t)7.1120859807429554e-03,
    (float_t)2.1760044649139429e-04,
    (float_t)7.3271839984290210e-12,
    (float_t)-2.1623066217181700e-02,
    (float_t)1.5670209409407512e-02,
    (float_t)2.8019603188636222e-03,
    (float_t)3.2494773799897647e-01,
    (float_t)1.0164917292316602e+00,
    (float_t)1.0329476657890369e+00,
    (float_t)1.0321586962991549e+00,
    (float_t)1.0358667411948619e+00,
    (float_t)1.0151235476834941e+00,
    (float_t)1.0338076690093119e+00,
    (float_t)1.0371372378155013e+00,
    (float_t)1.0361377027692558e+00,
    (float_t)1.0229822432557210e+00,
    (float_t)9.6910327335652324e-01,
    (float_t)-5.1785923899878572e-03,
    (float_t)1.1131261971061429e-03,
    (float_t)6.6675503033011771e-03,
    (float_t)7.4024315686001957e-04,
    (float_t)2.1591567633473925e-02,
    (float_t)5.1481620056217231e-03,
    (float_t)1.4561928645728216e-03,
    (float_t)1.6414511045291513e-04,
    (float_t)-6.4630764968453287e-03,
    (float_t)1.0250854718507939e-02,
    (float_t)4.2387394733956134e-02,
    (float_t)2.1252716926861620e-02
};

static const float_t spectrum_blue[NUM_SPECTRAL_SAMPLES] = {
    (float_t)1.0570490759328752e+00,
    (float_t)1.0538466912851301e+00,
    (float_t)1.0550494258140670e+00,
    (float_t)1.0530407754701832e+00,
    (float_t)1.0579930596460185e+00,
    (float_t)1.0578439494812371e+00,
    (float_t)1.0583132387180239e+00,
    (float_t)1.0579712943137616e+00,
    (float_t)1.0561884233578465e+00,
    (float_t)1.0571399285426490e+00,
    (float_t)1.0425795187752152e+00,
    (float_t)3.2603084374056102e-01,
    (float_t)-1.9255628442412243e-03,
    (float_t)-1.2959221137046478e-03,
    (float_t)-1.4357356276938696e-03,
    (float_t)-1.2963697250337886e-03,
    (float_t)-1.9227081162373899e-03,
    (float_t)1.2621152526221778e-03,
    (float_t)-1.6095249003578276e-03,
    (float_t)-1.3029983817879568e-03,
    (float_t)-1.7666600873954916e-03,
    (float_t)-1.2325281140280050e-03,
    (float_t)1.0316809673254932e-02,
    (float_t)3.1284512648354357e-02,
    (float_t)8.8773879881746481e-02,
    (float_t)1.3873621740236541e-01,
    (float_t)1.5535067531939065e-01,
    (float_t)1.4878477178237029e-01,
    (float_t)1.6624255403475907e-01,
    (float_t)1.6997613960634927e-01,
    (float_t)1.5769743995852967e-01,
    (float_t)1.9069090525482305e-01
};

static const float_t reflector_white[NUM_SPECTRAL_SAMPLES] = {
    (float_t)1.0618958571272863e+00,
    (float_t)1.0615019980348779e+00,
    (float_t)1.0614335379927147e+00,
    (float_t)1.0622711654692485e+00,
    (float_t)1.0622036218416742e+00,
    (float_t)1.0625059965187085e+00,
    (float_t)1.0623938486985884e+00,
    (float_t)1.0624706448043137e+00,
    (float_t)1.0625048144827762e+00,
    (float_t)1.0624366131308856e+00,
    (float_t)1.0620694238892607e+00,
    (float_t)1.0613167586932164e+00,
    (float_t)1.0610334029377020e+00,
    (float_t)1.0613868564828413e+00,
    (float_t)1.0614215366116762e+00,
    (float_t)1.0620336151299086e+00,
    (float_t)1.0625497454805051e+00,
    (float_t)1.0624317487992085e+00,
    (float_t)1.0625249140554480e+00,
    (float_t)1.0624277664486914e+00,
    (float_t)1.0624749854090769e+00,
    (float_t)1.0625538581025402e+00,
    (float_t)1.0625326910104864e+00,
    (float_t)1.0623922312225325e+00,
    (float_t)1.0623650980354129e+00,
    (float_t)1.0625256476715284e+00,
    (float_t)1.0612277619533155e+00,
    (float_t)1.0594262608698046e+00,
    (float_t)1.0599810758292072e+00,
    (float_t)1.0602547314449409e+00,
    (float_t)1.0601263046243634e+00,
    (float_t)1.0606565756823634e+00
};

static const float_t reflector_cyan[NUM_SPECTRAL_SAMPLES] = {
    (float_t)1.0414628021426751e+00,
    (float_t)1.0328661533771188e+00,
    (float_t)1.0126146228964314e+00,
    (float_t)1.0350460524836209e+00,
    (float_t)1.0078661447098567e+00,
    (float_t)1.0422280385081280e+00,
    (float_t)1.0442596738499825e+00,
    (float_t)1.0535238290294409e+00,
    (float_t)1.0180776226938120e+00,
    (float_t)1.0442729908727713e+00,
    (float_t)1.0529362541920750e+00,
    (float_t)1.0537034271160244e+00,
    (float_t)1.0533901869215969e+00,
    (float_t)1.0537782700979574e+00,
    (float_t)1.0527093770467102e+00,
    (float_t)1.0530449040446797e+00,
    (float_t)1.0550554640191208e+00,
    (float_t)1.0553673610724821e+00,
    (float_t)1.0454306634683976e+00,
    (float_t)6.2348950639230805e-01,
    (float_t)1.8038071613188977e-01,
    (float_t)-7.6303759201984539e-03,
    (float_t)-1.5217847035781367e-04,
    (float_t)-7.5102257347258311e-03,
    (float_t)-2.1708639328491472e-03,
    (float_t)6.5919466602369636e-04,
    (float_t)1.2278815318539780e-02,
    (float_t)-4.4669775637208031e-03,
    (float_t)1.7119799082865147e-02,
    (float_t)4.9211089759759801e-03,
    (float_t)5.8762925143334985e-03,
    (float_t)2.5259399415550079e-02
};

static const float_t reflector_magenta[NUM_SPECTRAL_SAMPLES] = {
    (float_t)9.9422138151236850e-01,
    (float_t)9.8986937122975682e-01,
    (float_t)9.8293658286116958e-01,
    (float_t)9.9627868399859310e-01,
    (float_t)1.0198955019000133e+00,
    (float_t)1.0166395501210359e+00,
    (float_t)1.0220913178757398e+00,
    (float_t)9.9651666040682441e-01,
    (float_t)1.0097766178917882e+00,
    (float_t)1.0215422470827016e+00,
    (float_t)6.4031953387790963e-01,
    (float_t)2.5012379477078184e-03,
    (float_t)6.5339939555769944e-03,
    (float_t)2.8334080462675826e-03,
    (float_t)-5.1209675389074505e-11,
    (float_t)-9.0592291646646381e-03,
    (float_t)3.3936718323331200e-03,
    (float_t)-3.0638741121828406e-03,
    (float_t)2.2203936168286292e-01,
    (float_t)6.3141140024811970e-01,
    (float_t)9.7480985576500956e-01,
    (float_t)9.7209562333590571e-01,
    (float_t)1.0173770302868150e+00,
    (float_t)9.9875194322734129e-01,
    (float_t)9.4701725739602238e-01,
    (float_t)8.5258623154354796e-01,
    (float_t)9.4897798581660842e-01,
    (float_t)9.4751876096521492e-01,
    (float_t)9.9598944191059791e-01,
    (float_t)8.6301351503809076e-01,
    (float_t)8.9150987853523145e-01,
    (float_t)8.4866492652845082e-01
};

static const float_t reflector_yellow[NUM_SPECTRAL_SAMPLES] = {
    (float_t)5.5740622924920873e-03,
    (float_t)-4.7982831631446787e-03,
    (float_t)-5.2536564298613798e-03,
    (float_t)-6.4571480044499710e-03,
    (float_t)-5.9693514658007013e-03,
    (float_t)-2.1836716037686721e-03,
    (float_t)1.6781120601055327e-02,
    (float_t)9.6096355429062641e-02,
    (float_t)2.1217357081986446e-01,
    (float_t)3.6169133290685068e-01,
    (float_t)5.3961011543232529e-01,
    (float_t)7.4408810492171507e-01,
    (float_t)9.2209571148394054e-01,
    (float_t)1.0460304298411225e+00,
    (float_t)1.0513824989063714e+00,
    (float_t)1.0511991822135085e+00,
    (float_t)1.0510530911991052e+00,
    (float_t)1.0517397230360510e+00,
    (float_t)1.0516043086790485e+00,
    (float_t)1.0511944032061460e+00,
    (float_t)1.0511590325868068e+00,
    (float_t)1.0516612465483031e+00,
    (float_t)1.0514038526836869e+00,
    (float_t)1.0515941029228475e+00,
    (float_t)1.0511460436960840e+00,
    (float_t)1.0515123758830476e+00,
    (float_t)1.0508871369510702e+00,
    (float_t)1.0508923708102380e+00,
    (float_t)1.0477492815668303e+00,
    (float_t)1.0493272144017338e+00,
    (float_t)1.0435963333422726e+00,
    (float_t)1.0392280772051465e+00
};

static const float_t reflector_red[NUM_SPECTRAL_SAMPLES] = {
    (float_t)1.6575604867086180e-01,
    (float_t)1.1846442802747797e-01,
    (float_t)1.2408293329637447e-01,
    (float_t)1.1371272058349924e-01,
    (float_t)7.8992434518899132e-02,
    (float_t)3.2205603593106549e-02,
    (float_t)-1.0798365407877875e-02,
    (float_t)1.8051975516730392e-02,
    (float_t)5.3407196598730527e-03,
    (float_t)1.3654918729501336e-02,
    (float_t)-5.9564213545642841e-03,
    (float_t)-1.8444365067353252e-03,
    (float_t)-1.0571884361529504e-02,
    (float_t)-2.9375521078000011e-03,
    (float_t)-1.0790476271835936e-02,
    (float_t)-8.0224306697503633e-03,
    (float_t)-2.2669167702495940e-03,
    (float_t)7.0200240494706634e-03,
    (float_t)-8.1528469000299308e-03,
    (float_t)6.0772866969252792e-01,
    (float_t)9.8831560865432400e-01,
    (float_t)9.9391691044078823e-01,
    (float_t)1.0039338994753197e+00,
    (float_t)9.9234499861167125e-01,
    (float_t)9.9926530858855522e-01,
    (float_t)1.0084621557617270e+00,
    (float_t)9.8358296827441216e-01,
    (float_t)1.0085023660099048e+00,
    (float_t)9.7451138326568698e-01,
    (float_t)9.8543269570059944e-01,
    (float_t)9.3495763980962043e-01,
    (float_t)9.8713907792319400e-01
};

static const float_t reflector_green[NUM_SPECTRAL_SAMPLES] = {
    (float_t)2.6494153587602255e-03,
    (float_t)-5.0175013429732242e-03,
    (float_t)-1.2547236272489583e-02,
    (float_t)-9.4554964308388671e-03,
    (float_t)-1.2526086181600525e-02,
    (float_t)-7.9170697760437767e-03,
    (float_t)-7.9955735204175690e-03,
    (float_t)-9.3559433444469070e-03,
    (float_t)6.5468611982999303e-02,
    (float_t)3.9572875517634137e-01,
    (float_t)7.5244022299886659e-01,
    (float_t)9.6376478690218559e-01,
    (float_t)9.9854433855162328e-01,
    (float_t)9.9992977025287921e-01,
    (float_t)9.9939086751140449e-01,
    (float_t)9.9994372267071396e-01,
    (float_t)9.9939121813418674e-01,
    (float_t)9.9911237310424483e-01,
    (float_t)9.6019584878271580e-01,
    (float_t)6.3186279338432438e-01,
    (float_t)2.5797401028763473e-01,
    (float_t)9.4014888527335638e-03,
    (float_t)-3.0798345608649747e-03,
    (float_t)-4.5230367033685034e-03,
    (float_t)-6.8933410388274038e-03,
    (float_t)-9.0352195539015398e-03,
    (float_t)-8.5913667165340209e-03,
    (float_t)-8.3690869120289398e-03,
    (float_t)-7.8685832338754313e-03,
    (float_t)-8.3657578711085132e-06,
    (float_t)5.4301225442817177e-03,
    (float_t)-2.7745589759259194e-03
};

static const float_t reflector_blue[NUM_SPECTRAL_SAMPLES] = {
    (float_t)9.9209771469720676e-01,
    (float_t)9.8876426059369127e-01,
    (float_t)9.9539040744505636e-01,
    (float_t)9.9529317353008218e-01,
    (float_t)9.9181447411633950e-01,
    (float_t)1.0002584039673432e+00,
    (float_t)9.9968478437342512e-01,
    (float_t)9.9988120766657174e-01,
    (float_t)9.8504012146370434e-01,
    (float_t)7.9029849053031276e-01,
    (float_t)5.6082198617463974e-01,
    (float_t)3.3133458513996528e-01,
    (float_t)1.3692410840839175e-01,
    (float_t)1.8914906559664151e-02,
    (float_t)-5.1129770932550889e-06,
    (float_t)-4.2395493167891873e-04,
    (float_t)-4.1934593101534273e-04,
    (float_t)1.7473028136486615e-03,
    (float_t)3.7999160177631316e-03,
    (float_t)-5.5101474906588642e-04,
    (float_t)-4.3716662898480967e-05,
    (float_t)7.5874501748732798e-03,
    (float_t)2.5795650780554021e-02,
    (float_t)3.8168376532500548e-02,
    (float_t)4.9489586408030833e-02,
    (float_t)4.9595992290102905e-02,
    (float_t)4.9814819505812249e-02,
    (float_t)3.9840911064978023e-02,
    (float_t)3.0501024937233868e-02,
    (float_t)2.1243054765241080e-02,
    (float_t)6.9596532104356399e-03,
    (float_t)4.1733649330980525e-03
};

//
// Static Functions
//

static
float_t
SmitsColorExtrapolatorResample(
    _In_reads_(num_samples) const float_t wavelengths[],
    _In_reads_(num_samples) const float_t sampled_values[],
    _In_ size_t num_samples,
    _In_ float_t target_wavelength
    )
{
    const float_t *lower_bound = wavelengths;
    size_t num_wavelengths = num_samples;

    while (num_wavelengths > 0)
    {
        size_t step = num_wavelengths >> 1;
        const float_t *pivot = lower_bound + step;
        if (*pivot < target_wavelength)
        {
            lower_bound = pivot + 1;
            num_wavelengths -= step + 1;
        }
        else
        {
            num_wavelengths = step;
        }
    }

    size_t result_index = lower_bound - wavelengths;

    if (result_index == num_samples)
    {
        return sampled_values[num_samples - 1];
    }

    float_t higher_wavelength = *lower_bound;

    if (higher_wavelength == target_wavelength)
    {
        return sampled_values[result_index];
    }

    if (result_index == 0)
    {
        return sampled_values[0];
    }

    float_t lower_wavelength = lower_bound[-1];
    float_t parameter =
        (target_wavelength - lower_wavelength) / 
        (higher_wavelength - lower_wavelength);

    float_t higher_value = sampled_values[result_index];
    float_t lower_value = sampled_values[result_index - 1];

    return fma(parameter, higher_value - lower_value, lower_value);
}

static
void
SmitsColorExtrapolatorScale(
    _In_reads_(num_samples) const float_t input[],
    _In_ size_t num_samples,
    _In_ float_t scalar,
    _Out_writes_(num_samples) float_t output[]
    )
{
    for (size_t i = 0; i < num_samples; i++)
    {
        output[i] = input[i] * scalar;
    }
}

static
void
SmitsColorExtrapolatorScaledAdd(
    _In_reads_(num_samples) const float_t addend[],
    _In_reads_(num_samples) const float_t scaled[],
    _In_ size_t num_samples,
    _In_ float_t scalar,
    _Out_writes_(num_samples) float_t output[]
    )
{
    for (size_t i = 0; i < num_samples; i++)
    {
        output[i] = fma(scalar, scaled[i], addend[i]);
    }
}

static
ISTATUS
SmitsColorExtrapolatorCreateSpd(
    _In_ float_t r,
    _In_ float_t g,
    _In_ float_t b,
    _In_reads_(num_samples) const float_t white[],
    _In_reads_(num_samples) const float_t cyan[],
    _In_reads_(num_samples) const float_t magenta[],
    _In_reads_(num_samples) const float_t yellow[],
    _In_reads_(num_samples) const float_t red[],
    _In_reads_(num_samples) const float_t green[],
    _In_reads_(num_samples) const float_t blue[],
    _In_ size_t num_samples,
    _In_ float_t scalar,
    _Outptr_result_buffer_(num_samples) float_t **output
    )
{
    float_t *temp = calloc(num_samples, sizeof(float_t));

    if (temp == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (r <= g && r <= b)
    {
        SmitsColorExtrapolatorScale(white, num_samples, r, temp);
        if (g <= b)
        {
            SmitsColorExtrapolatorScaledAdd(temp, cyan, num_samples, g - r, temp);
            SmitsColorExtrapolatorScaledAdd(temp, blue, num_samples, b - g, temp);
        }
        else
        {
            SmitsColorExtrapolatorScaledAdd(temp, cyan, num_samples, b - r, temp);
            SmitsColorExtrapolatorScaledAdd(temp, green, num_samples, g - b, temp);
        }
    }
    else if (g <= r && g <= b)
    {
        SmitsColorExtrapolatorScale(white, num_samples, g, temp);
        if (r <= b)
        {
            SmitsColorExtrapolatorScaledAdd(temp, magenta, num_samples, r - g, temp);
            SmitsColorExtrapolatorScaledAdd(temp, blue, num_samples, b - r, temp);
        }
        else
        {
            SmitsColorExtrapolatorScaledAdd(temp, magenta, num_samples, b - g, temp);
            SmitsColorExtrapolatorScaledAdd(temp, red, num_samples, r - b, temp);
        }
    }
    else
    {
        SmitsColorExtrapolatorScale(white, num_samples, b, temp);
        if (r <= g)
        {
            SmitsColorExtrapolatorScaledAdd(temp, yellow, num_samples, r - b, temp);
            SmitsColorExtrapolatorScaledAdd(temp, green, num_samples, g - r, temp);
        }
        else
        {
            SmitsColorExtrapolatorScaledAdd(temp, yellow, num_samples, g - b, temp);
            SmitsColorExtrapolatorScaledAdd(temp, red, num_samples, r - g, temp);
        }
    }

    SmitsColorExtrapolatorScale(temp, num_samples, scalar, temp);

    for (size_t i = 0; i < num_samples; i++)
    {
        temp[i] = fmax((float_t)0.0, temp[i]);
    }

    *output = temp;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
SmitsColorExtrapolatorComputeSpectrum(
    _In_ const void *context,
    _In_ COLOR3 color,
    _Out_ PSPECTRUM *spectrum
    )
{
    PCRGB_COLOR_EXTRAPOLATOR extrapolator = (PCRGB_COLOR_EXTRAPOLATOR)context;

    color = ColorConvert(color, COLOR_SPACE_LINEAR_SRGB);

    float_t *spd;
    ISTATUS status = SmitsColorExtrapolatorCreateSpd(color.values[0],
                                                     color.values[1],
                                                     color.values[2],
                                                     extrapolator->spectrum_white,
                                                     extrapolator->spectrum_cyan,
                                                     extrapolator->spectrum_magenta,
                                                     extrapolator->spectrum_yellow,
                                                     extrapolator->spectrum_red,
                                                     extrapolator->spectrum_green,
                                                     extrapolator->spectrum_blue,
                                                     extrapolator->num_samples,
                                                     (float_t)0.86445,
                                                     &spd);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = InterpolatedSpectrumAllocate(extrapolator->wavelengths,
                                          spd,
                                          extrapolator->num_samples,
                                          spectrum);

    free(spd);

    return ISTATUS_SUCCESS;
}

static
ISTATUS
SmitsColorExtrapolatorComputeReflector(
    _In_ const void *context,
    _In_ COLOR3 color,
    _Out_ PREFLECTOR *reflector
    )
{
    PCRGB_COLOR_EXTRAPOLATOR extrapolator = (PCRGB_COLOR_EXTRAPOLATOR)context;

    color = ColorConvert(color, COLOR_SPACE_LINEAR_SRGB);
    color = ColorClamp(color, 1.0f);

    float_t *spd;
    ISTATUS status =
        SmitsColorExtrapolatorCreateSpd(color.values[0],
                                        color.values[1],
                                        color.values[2],
                                        extrapolator->reflector_white,
                                        extrapolator->reflector_cyan,
                                        extrapolator->reflector_magenta,
                                        extrapolator->reflector_yellow,
                                        extrapolator->reflector_red,
                                        extrapolator->reflector_green,
                                        extrapolator->reflector_blue,
                                        extrapolator->num_samples,
                                        (float_t)0.94,
                                        &spd);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = InterpolatedReflectorAllocate(extrapolator->wavelengths,
                                           spd,
                                           extrapolator->num_samples,
                                           reflector);

    free(spd);

    return status;
}

static
void
SmitsColorExtrapolatorFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PCRGB_COLOR_EXTRAPOLATOR extrapolator = (PCRGB_COLOR_EXTRAPOLATOR)context;

    free(extrapolator->wavelengths);
    free(extrapolator->spectrum_white);
    free(extrapolator->spectrum_cyan);
    free(extrapolator->spectrum_magenta);
    free(extrapolator->spectrum_yellow);
    free(extrapolator->spectrum_red);
    free(extrapolator->spectrum_green);
    free(extrapolator->spectrum_blue);
    free(extrapolator->reflector_white);
    free(extrapolator->reflector_cyan);
    free(extrapolator->reflector_magenta);
    free(extrapolator->reflector_yellow);
    free(extrapolator->reflector_red);
    free(extrapolator->reflector_green);
    free(extrapolator->reflector_blue);
}

//
// Static Data
//

static const COLOR_EXTRAPOLATOR_VTABLE rgb_color_extrapolator_vtable = {
    SmitsColorExtrapolatorComputeSpectrum,
    SmitsColorExtrapolatorComputeReflector,
    SmitsColorExtrapolatorFree
};

//
// Functions
//

ISTATUS
SmitsColorExtrapolatorAllocate(
    _In_reads_(num_wavelengths) float_t wavelengths[],
    _In_ size_t num_wavelengths,
    _Out_ PCOLOR_EXTRAPOLATOR* extrapolator
    )
{
    if (wavelengths == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (num_wavelengths == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(wavelengths[0]) || wavelengths[0] <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    for (size_t i = 1; i < num_wavelengths; i++)
    {
        if (!isfinite(wavelengths[i]) || wavelengths[i] <= wavelengths[i - 1])
        {
            return ISTATUS_INVALID_ARGUMENT_01;
        }
    }

    if (extrapolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    RGB_COLOR_EXTRAPOLATOR rgb_color_extrapolator;
    memset(&rgb_color_extrapolator, 0, sizeof(RGB_COLOR_EXTRAPOLATOR));

    rgb_color_extrapolator.wavelengths = calloc(num_wavelengths,
                                                sizeof(float_t));

    if (rgb_color_extrapolator.wavelengths == NULL)
    {
        SmitsColorExtrapolatorFree(&rgb_color_extrapolator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    rgb_color_extrapolator.spectrum_white = calloc(num_wavelengths,
                                                   sizeof(float_t));

    if (rgb_color_extrapolator.spectrum_white == NULL)
    {
        SmitsColorExtrapolatorFree(&rgb_color_extrapolator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    rgb_color_extrapolator.spectrum_cyan = calloc(num_wavelengths,
                                                  sizeof(float_t));

    if (rgb_color_extrapolator.spectrum_cyan == NULL)
    {
        SmitsColorExtrapolatorFree(&rgb_color_extrapolator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    rgb_color_extrapolator.spectrum_magenta = calloc(num_wavelengths,
                                                     sizeof(float_t));

    if (rgb_color_extrapolator.spectrum_magenta == NULL)
    {
        SmitsColorExtrapolatorFree(&rgb_color_extrapolator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    rgb_color_extrapolator.spectrum_yellow = calloc(num_wavelengths,
                                                    sizeof(float_t));

    if (rgb_color_extrapolator.spectrum_yellow == NULL)
    {
        SmitsColorExtrapolatorFree(&rgb_color_extrapolator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    rgb_color_extrapolator.spectrum_red = calloc(num_wavelengths,
                                                 sizeof(float_t));

    if (rgb_color_extrapolator.spectrum_red == NULL)
    {
        SmitsColorExtrapolatorFree(&rgb_color_extrapolator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    rgb_color_extrapolator.spectrum_green = calloc(num_wavelengths,
                                                   sizeof(float_t));

    if (rgb_color_extrapolator.spectrum_green == NULL)
    {
        SmitsColorExtrapolatorFree(&rgb_color_extrapolator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    rgb_color_extrapolator.spectrum_blue = calloc(num_wavelengths,
                                                  sizeof(float_t));

    if (rgb_color_extrapolator.spectrum_blue == NULL)
    {
        SmitsColorExtrapolatorFree(&rgb_color_extrapolator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    rgb_color_extrapolator.reflector_white = calloc(num_wavelengths,
                                                    sizeof(float_t));

    if (rgb_color_extrapolator.reflector_white == NULL)
    {
        SmitsColorExtrapolatorFree(&rgb_color_extrapolator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    rgb_color_extrapolator.reflector_cyan = calloc(num_wavelengths,
                                                   sizeof(float_t));

    if (rgb_color_extrapolator.reflector_cyan == NULL)
    {
        SmitsColorExtrapolatorFree(&rgb_color_extrapolator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    rgb_color_extrapolator.reflector_magenta = calloc(num_wavelengths,
                                                      sizeof(float_t));

    if (rgb_color_extrapolator.reflector_magenta == NULL)
    {
        SmitsColorExtrapolatorFree(&rgb_color_extrapolator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    rgb_color_extrapolator.reflector_yellow = calloc(num_wavelengths,
                                                     sizeof(float_t));

    if (rgb_color_extrapolator.reflector_yellow == NULL)
    {
        SmitsColorExtrapolatorFree(&rgb_color_extrapolator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    rgb_color_extrapolator.reflector_red = calloc(num_wavelengths,
                                                  sizeof(float_t));

    if (rgb_color_extrapolator.reflector_red == NULL)
    {
        SmitsColorExtrapolatorFree(&rgb_color_extrapolator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    rgb_color_extrapolator.reflector_green = calloc(num_wavelengths,
                                                    sizeof(float_t));

    if (rgb_color_extrapolator.reflector_green == NULL)
    {
        SmitsColorExtrapolatorFree(&rgb_color_extrapolator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    rgb_color_extrapolator.reflector_blue = calloc(num_wavelengths,
                                                   sizeof(float_t));

    if (rgb_color_extrapolator.reflector_blue == NULL)
    {
        SmitsColorExtrapolatorFree(&rgb_color_extrapolator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < num_wavelengths; i++)
    {
        rgb_color_extrapolator.wavelengths[i] = wavelengths[i];

        rgb_color_extrapolator.spectrum_white[i] =
            SmitsColorExtrapolatorResample(sample_wavelengths,
                                           spectrum_white,
                                           NUM_SPECTRAL_SAMPLES,
                                           wavelengths[i]);

        rgb_color_extrapolator.spectrum_cyan[i] =
            SmitsColorExtrapolatorResample(sample_wavelengths,
                                           spectrum_cyan,
                                           NUM_SPECTRAL_SAMPLES,
                                           wavelengths[i]);

        rgb_color_extrapolator.spectrum_magenta[i] =
            SmitsColorExtrapolatorResample(sample_wavelengths,
                                           spectrum_magenta,
                                           NUM_SPECTRAL_SAMPLES,
                                           wavelengths[i]);

        rgb_color_extrapolator.spectrum_yellow[i] =
            SmitsColorExtrapolatorResample(sample_wavelengths,
                                           spectrum_yellow,
                                           NUM_SPECTRAL_SAMPLES,
                                           wavelengths[i]);

        rgb_color_extrapolator.spectrum_red[i] =
            SmitsColorExtrapolatorResample(sample_wavelengths,
                                           spectrum_red,
                                           NUM_SPECTRAL_SAMPLES,
                                           wavelengths[i]);

        rgb_color_extrapolator.spectrum_green[i] =
            SmitsColorExtrapolatorResample(sample_wavelengths,
                                           spectrum_green,
                                           NUM_SPECTRAL_SAMPLES,
                                           wavelengths[i]);

        rgb_color_extrapolator.spectrum_blue[i] =
            SmitsColorExtrapolatorResample(sample_wavelengths,
                                           spectrum_blue,
                                           NUM_SPECTRAL_SAMPLES,
                                           wavelengths[i]);

        rgb_color_extrapolator.reflector_white[i] =
            SmitsColorExtrapolatorResample(sample_wavelengths,
                                           reflector_white,
                                           NUM_SPECTRAL_SAMPLES,
                                           wavelengths[i]);

        rgb_color_extrapolator.reflector_cyan[i] =
            SmitsColorExtrapolatorResample(sample_wavelengths,
                                           reflector_cyan,
                                           NUM_SPECTRAL_SAMPLES,
                                           wavelengths[i]);

        rgb_color_extrapolator.reflector_magenta[i] =
            SmitsColorExtrapolatorResample(sample_wavelengths,
                                           reflector_magenta,
                                           NUM_SPECTRAL_SAMPLES,
                                           wavelengths[i]);

        rgb_color_extrapolator.reflector_yellow[i] =
            SmitsColorExtrapolatorResample(sample_wavelengths,
                                           reflector_yellow,
                                           NUM_SPECTRAL_SAMPLES,
                                           wavelengths[i]);

        rgb_color_extrapolator.reflector_red[i] =
            SmitsColorExtrapolatorResample(sample_wavelengths,
                                           reflector_red,
                                           NUM_SPECTRAL_SAMPLES,
                                           wavelengths[i]);

        rgb_color_extrapolator.reflector_green[i] =
            SmitsColorExtrapolatorResample(sample_wavelengths,
                                           reflector_green,
                                           NUM_SPECTRAL_SAMPLES,
                                           wavelengths[i]);

        rgb_color_extrapolator.reflector_blue[i] =
            SmitsColorExtrapolatorResample(sample_wavelengths,
                                           reflector_blue,
                                           NUM_SPECTRAL_SAMPLES,
                                           wavelengths[i]);
    }

    rgb_color_extrapolator.num_samples = num_wavelengths;

    ISTATUS status = ColorExtrapolatorAllocate(&rgb_color_extrapolator_vtable,
                                               &rgb_color_extrapolator,
                                               sizeof(RGB_COLOR_EXTRAPOLATOR),
                                               alignof(RGB_COLOR_EXTRAPOLATOR),
                                               extrapolator);

    return status;
}