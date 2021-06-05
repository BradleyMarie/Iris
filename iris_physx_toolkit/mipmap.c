/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    mipmap.h

Abstract:

    Creates a mipmap.

--*/

#include "iris_physx_toolkit/mipmap.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "common/safe_math.h"

//
// Definitions
//

#define EWA_LUT_SIZE 128

//
// Static Data
//

static const float_t ewa_lookup_table[EWA_LUT_SIZE] = {
    (float_t)0.8646647167633873080905271280016677337698638439178466796875000000,
    (float_t)0.8490400371500144327714665326567455849726684391498565673828125000,
    (float_t)0.8336594881496621073650905853202885964492452330887317657470703125,
    (float_t)0.8185192552997180695300780128853546102618565782904624938964843750,
    (float_t)0.8036155837373262705923375459615698446214082650840282440185546875,
    (float_t)0.7889447772681597798928067322155044394094147719442844390869140625,
    (float_t)0.7745031974497438136953095244585654199909185990691184997558593750,
    (float_t)0.7602872626891015486435068670978409954841481521725654602050781250,
    (float_t)0.7462934473544989295965175479796727131542866118252277374267578125,
    (float_t)0.7325182809010681808268780623727423062518937513232231140136718750,
    (float_t)0.7189583470100931693585327719731026263616513460874557495117187500,
    (float_t)0.7056102827417431590890581349917454190290300175547599792480468750,
    (float_t)0.6924707777010448282077950676871580526494653895497322082519531250,
    (float_t)0.6795365732168857066325742821710775842802831903100013732910156250,
    (float_t)0.6668044615338454220859754073558178788516670465469360351562500000,
    (float_t)0.6542712850166543237222932516417017723142635077238082885742187500,
    (float_t)0.6419339353670821847475114196868162252940237522125244140625000000,
    (float_t)0.6297893528530627685660912840059921791180386207997798919677734375,
    (float_t)0.6178345255498630762608022748416658487258246168494224548339843750,
    (float_t)0.6060664885931090815982674735451496417226735502481460571289062500,
    (float_t)0.5944823234434826996755094064006641474406933411955833435058593750,
    (float_t)0.5830791571629076296004803858785692227684194222092628479003906250,
    (float_t)0.5718541617020445618586529934468387637025443837046623229980468750,
    (float_t)0.5608045531989190443308834765900883212452754378318786621093750000,
    (float_t)0.5499275912885080636375266516235882363616838119924068450927734375,
    (float_t)0.5392205784231141146812735231996782658825395628809928894042968750,
    (float_t)0.5286808592033582076684557204959702403357368893921375274658203125,
    (float_t)0.5183058197196258950903771656193441685900324955582618713378906250,
    (float_t)0.5080928869038029933652075853522944726137211546301841735839843750,
    (float_t)0.4980395278911402265407774858640976844981196336448192596435546875,
    (float_t)0.4881432493920885301455850346918907689541811123490333557128906250,
    (float_t)0.4784015970739492270994222544722163092956179752945899963378906250,
    (float_t)0.4688121549521857218457455973581460284549393691122531890869140625,
    (float_t)0.4593725447912457539247250981606640607424196787178516387939453125,
    (float_t)0.4500804255147456121887751534504928940805257298052310943603515625,
    (float_t)0.4409334926248700313802797445816850085975602269172668457031250000,
    (float_t)0.4319294776308437793193965859028793374818633310496807098388671875,
    (float_t)0.4230661474863331929945932385894735716647119261324405670166015625,
    (float_t)0.4143413040356381344341517303231370306093594990670680999755859375,
    (float_t)0.4057527834685370208200536068510189124936005100607872009277343750,
    (float_t)0.3972984557836497256349400641894931140996050089597702026367187500,
    (float_t)0.3889762242601852628570702208321563375648111104965209960937500000,
    (float_t)0.3807840249379432443627147009124200849328190088272094726562500000,
    (float_t)0.3727198261054401478317536777762342126152361743152141571044921875,
    (float_t)0.3647816277960334477672467434761216509286896325647830963134765625,
    (float_t)0.3569674612919186456249862060552402454050024971365928649902343750,
    (float_t)0.3492753886358761872092670852651963286916725337505340576171875000,
    (float_t)0.3417035021506471783236599804034483440773328766226768493652343750,
    (float_t)0.3342499239658187006767921245486974157756776548922061920166015625,
    (float_t)0.3269128055521013934072056816848572680100915022194385528564453125,
    (float_t)0.3196903272628837976761934092095884807349648326635360717773437500,
    (float_t)0.3125806978829497682751716036708700130475335754454135894775390625,
    (float_t)0.3055821541842470309788964977215641738439444452524185180664062500,
    (float_t)0.2986929604885967135191951471462346034968504682183265686035156250,
    (float_t)0.2919114082372354005258732745353711379721062257885932922363281250,
    (float_t)0.2852358155670829556005245464245234643385629169642925262451171875,
    (float_t)0.2786645268936310229266305193984720744992955587804317474365234375,
    (float_t)0.2721959125003487627893206113327551065594889223575592041015625000,
    (float_t)0.2658283681345039902972973711037241173471556976437568664550781250,
    (float_t)0.2595603146092994799429450047956180469554965384304523468017578125,
    (float_t)0.2533901974122257624898527095247402485256316140294075012207031250,
    (float_t)0.2473164863195332841336445892088669040731474524363875389099121094,
    (float_t)0.2413376750167283146401896523625119073130917968228459358215332031,
    (float_t)0.2354522807249984852636217128041273838334745960310101509094238281,
    (float_t)0.2296588438334753080888187351527029989028960699215531349182128906,
    (float_t)0.2239559275372424753514479303651008024189650313928723335266113281,
    (float_t)0.2183421174810001637599287185875773786847275914624333381652832031,
    (float_t)0.2128160214082969690230088058435775622001528972759842872619628906,
    (float_t)0.2073762688162424798113793802945004074445023434236645698547363281,
    (float_t)0.2020215106156148566373580502397722113983036251738667488098144531,
    (float_t)0.1967504187962791211021331208574913773645675973966717720031738281,
    (float_t)0.1915616860978331771543951497771551828463998390361666679382324219,
    (float_t)0.1864540256853998827912951771756944197022676235064864158630371094,
    (float_t)0.1814261708304847666302058242782102581713843392208218574523925781,
    (float_t)0.1764768745968202404784995820929971444002148928120732307434082031,
    (float_t)0.1716049095311183956424514948646553591515839798375964164733886719,
    (float_t)0.1668090673586556872454210800893470434402843238785862922668457031,
    (float_t)0.1620881586836140108204328338925570562878419877961277961730957031,
    (float_t)0.1574410126941038532110946415731866920850734459236264228820800781,
    (float_t)0.1528664768717963625132031865727633146434527589008212089538574219,
    (float_t)0.1483634167060923235626932956132684182648517889901995658874511719,
    (float_t)0.1439307154127571519887504244983311707528628176078200340270996094,
    (float_t)0.1395672736569521260623820760327085110930056544020771980285644531,
    (float_t)0.1352720092805931668473356024873233138805517228320240974426269531,
    (float_t)0.1310438570339695502222450082530080806009209481999278068542480469,
    (float_t)0.1268817683115559903558053692074203411266353214159607887268066406,
    (float_t)0.1227847108919525747798085282824764874476386466994881629943847656,
    (float_t)0.1187516686818880548001447702843869080879812827333807945251464844,
    (float_t)0.1147816414642230021825883637398568737353343749418854713439941406,
    (float_t)0.1108736446498903361658223271080103700114705134183168411254882812,
    (float_t)0.1070267090337116998907240200678003816392447333782911300659179688,
    (float_t)0.1032398805540291279709370561579184766287653474137187004089355469,
    (float_t)0.0995122200560923921372090059089821068027958972379565238952636719,
    (float_t)0.0958428030591433438003991923670454866623913403600454330444335938,
    (float_t)0.0922307195271394891794179346500293092958600027486681938171386719,
    (float_t)0.0886750736430599350760368634882802041374816326424479484558105469,
    (float_t)0.0851749835867377319228478660395609267652616836130619049072265625,
    (float_t)0.0817295813161635154514084514176808227148285368457436561584472656,
    (float_t)0.0783380123522062085471675055292628542247257428243756294250488281,
    (float_t)0.0749994355666973935441209711538768090122175635769963264465332031,
    (float_t)0.0717130229738267980691508382551369038537814049050211906433105469,
    (float_t)0.0684779595247971596694031709562366927457333076745271682739257812,
    (float_t)0.0652934429056875424170723609673672171993530355393886566162109375,
    (float_t)0.0621586833384749743006180214277556217439268948510289192199707031,
    (float_t)0.0590729033851650579811914248962168016987561713904142379760742188,
    (float_t)0.0560353377549829778186919401750110125703940866515040397644042969,
    (float_t)0.0530452331145770857316786017765508631782722659409046173095703125,
    (float_t)0.0501018479011879952131607723897488426700874697417020797729492188,
    (float_t)0.0472044521387368484012557814932797839446720900014042854309082031,
    (float_t)0.0443523272567871449577293560539992256508412538096308708190917969,
    (float_t)0.0415447659123352345872035029650248816324165090918540954589843750,
    (float_t)0.0387810718143852757225755259473665148561849491670727729797363281,
    (float_t)0.0360605595512651545012051609861991607886011479422450065612792969,
    (float_t)0.0333825544206405371515898605760952477794489823281764984130859375,
    (float_t)0.0307463922621848982289069138196246910865738755092024803161621094,
    (float_t)0.0281514192928640261565391075815334431808878434821963310241699219,
    (float_t)0.0255969919447941555854441827988488000755751272663474082946777344,
    (float_t)0.0230824767056335144614674995122705070116353454068303108215332031,
    (float_t)0.0206072499614677020265138610022326304260786855593323707580566406,
    (float_t)0.0181706978421499324897287960811631535307242302224040031433105469,
    (float_t)0.0157722160690577877759392319556575046135549200698733329772949219,
    (float_t)0.0134112098052287223088523474001654278708883794024586677551269531,
    (float_t)0.0110870935078371525111780651617898740823875414207577705383300781,
    (float_t)0.0087992907829765444296292800641090536828414769843220710754394531,
    (float_t)0.0065472342427104847958473782254529282909061294049024581909179688,
    (float_t)0.0043303653643572833183876882867657798215077491477131843566894531,
    (float_t)0.0021481343519732079062369012323330252911546267569065093994140625,
    (float_t)0.0000000000000000000000000000000000000000000000000000000000000000
};

//
// Static Functions
//

static
inline
size_t
SizeTLog2(
    _In_ size_t value
    )
{
    assert(value != 0 && (value & (value - 1)) == 0);

    value >>= 1;

    size_t result = 0;
    while (value != 0)
    {
        value >>= 1;
        result += 1;
    }

    return result;
}

static
inline
float_t
FloatTLog2(
    _In_ float_t value
    )
{
    float_t inv_log2 = (float_t)1.442695040888963387004650940071;
    return log(value) * inv_log2;
}

_Ret_writes_maybenull_(width * height / 4)
static
PCOLOR3
DownsampleColors(
    _In_reads_(width * height) PCCOLOR3 texels,
    _In_ size_t width,
    _In_ size_t height,
    _In_ size_t *new_width,
    _In_ size_t *new_height
    )
{
    assert(texels != NULL);
    assert(width != 0 && (width & (width - 1)) == 0);
    assert(height != 0 && (height & (height - 1)) == 0);

    *new_width = width / 2;
    *new_height = height / 2;

    PCOLOR3 colors =
        (PCOLOR3)calloc(*new_width * *new_height, sizeof(COLOR3));

    if (colors == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < *new_height; i++)
    {
        for (size_t j = 0; j < *new_width; j++)
        {
            size_t source_row = i * 2;
            size_t source_column = j * 2;

            COLOR3 color = texels[source_row * width + source_column];

            color = ColorAdd(color,
                             texels[source_row * width + source_column + 1],
                             color.color_space);

            source_row += 1;

            color = ColorAdd(color,
                             texels[source_row * width + source_column],
                             color.color_space);

            color = ColorAdd(color,
                             texels[source_row * width + source_column + 1],
                             color.color_space);

            colors[i * *new_width + j] = ColorScale(color, (float_t)0.25);
        }
    }

    return colors;
}

//
// Spectrum Mipmap Types
//

typedef struct _SPECTRUM_MIPMAP_LEVEL {
    _Field_size_(width * height) PSPECTRUM *texels;
    size_t width;
    size_t height;
    float_t width_fp;
    float_t height_fp;
    float_t texel_width;
    float_t texel_height;
} SPECTRUM_MIPMAP_LEVEL, *PSPECTRUM_MIPMAP_LEVEL;

typedef const struct SPECTRUM_MIPMAP_LEVEL *PCSPECTRUM_MIPMAP_LEVEL;

struct _SPECTRUM_MIPMAP {
    _Field_size_(num_levels) PSPECTRUM_MIPMAP_LEVEL levels;
    size_t num_levels;
    TEXTURE_FILTERING_ALGORITHM texture_filtering;
    WRAP_MODE wrap_mode;
    float_t max_anisotropy;
    float_t last_level_index_fp;
};

//
// Spectrum Mipmap Static Functions
//

static
PCSPECTRUM
SpectrumMipmapLookupTexel(
    _In_ PCSPECTRUM_MIPMAP mipmap,
    _In_ size_t level,
    _In_ float_t s,
    _In_ float_t t
    )
{
    if (mipmap->wrap_mode == WRAP_MODE_REPEAT)
    {
#if FLT_EVAL_METHOD	== 0
        float s_intpart, t_intpart;
        s = modff(s, &s_intpart);
        t = modff(t, &t_intpart);
#elif FLT_EVAL_METHOD == 1
        double s_intpart, t_intpart;
        s = modf(s, &s_intpart);
        t = modf(t, &t_intpart);
#elif FLT_EVAL_METHOD == 2
        long double s_intpart, t_intpart;
        s = modfl(s, &s_intpart);
        t = modfl(t, &t_intpart);
#endif

        if (s < (float_t)0.0)
        {
            s = (float_t)1.0 + s;
        }

        if (t < (float_t)0.0)
        {
            t = (float_t)1.0 + t;
        }
    }
    else if (mipmap->wrap_mode == WRAP_MODE_CLAMP)
    {
        s = IMin(IMax((float_t)0.0, s), (float_t)1.0);
        t = IMin(IMax((float_t)0.0, t), (float_t)1.0);
    }
    else if (s < (float_t)0.0 || (float_t)1.0 < s ||
             t < (float_t)0.0 || (float_t)1.0 < t)
    {
        assert(mipmap->wrap_mode == WRAP_MODE_BLACK);
        return NULL;
    }

    size_t x = (size_t)floor(mipmap->levels[level].width_fp * s);

    if (x == mipmap->levels[level].width)
    {
        x -= 1;
    }

    size_t y = (size_t)floor(mipmap->levels[level].height_fp * t);

    if (y == mipmap->levels[level].height)
    {
        y -= 1;
    }

    return mipmap->levels[level].texels[y * mipmap->levels[level].width + x];
}

static
ISTATUS
SpectrumMipmapLookupWithTriangleFilter(
    _In_ PCSPECTRUM_MIPMAP mipmap,
    _In_ size_t level,
    _In_ float_t s,
    _In_ float_t t,
    _In_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    if (mipmap->num_levels <= level)
    {
        level = mipmap->num_levels - 1;
    }

    float_t scaled_s = s * mipmap->levels[level].width_fp;
    float_t scaled_t = t * mipmap->levels[level].height_fp;

    float_t scaled_s0 = floor(scaled_s - (float_t)0.5) + (float_t)0.5;
    float_t scaled_t0 = floor(scaled_t - (float_t)0.5) + (float_t)0.5;

    float_t s0 = scaled_s0 * mipmap->levels[level].texel_width;
    float_t t0 = scaled_t0 * mipmap->levels[level].texel_height;

    float_t ds = scaled_s - scaled_s0;
    float_t dt = scaled_t - scaled_t0;

    ds = IMax((float_t)0.0, IMin(ds, (float_t)1.0));
    dt = IMax((float_t)0.0, IMin(dt, (float_t)1.0));

    float_t one_minus_ds = (float_t)1.0 - ds;
    float_t one_minus_dt = (float_t)1.0 - dt;

    float_t s1 = s0 + mipmap->levels[level].texel_width;
    float_t t1 = t0 + mipmap->levels[level].texel_height;

    PCSPECTRUM texel = SpectrumMipmapLookupTexel(mipmap, level, s0, t0);

    PCSPECTRUM result;
    ISTATUS status =
        SpectrumCompositorAttenuateSpectrum(compositor,
                                            texel,
                                            one_minus_ds * one_minus_dt,
                                            &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    texel = SpectrumMipmapLookupTexel(mipmap, level, s0, t1);

    status =
        SpectrumCompositorAttenuatedAddSpectra(compositor,
                                               result,
                                               texel,
                                               one_minus_ds * dt,
                                               &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    texel = SpectrumMipmapLookupTexel(mipmap, level, s1, t0);

    status =
        SpectrumCompositorAttenuatedAddSpectra(compositor,
                                               result,
                                               texel,
                                               ds * one_minus_dt,
                                               &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    texel = SpectrumMipmapLookupTexel(mipmap, level, s1, t1);

    status =
        SpectrumCompositorAttenuatedAddSpectra(compositor,
                                               result,
                                               texel,
                                               ds * dt,
                                               spectrum);

    return status;
}

ISTATUS
SpectrumMipmapLookupTextureFilteringNone(
    _In_ PCSPECTRUM_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    *spectrum = SpectrumMipmapLookupTexel(mipmap, 0, s, t);
    return ISTATUS_SUCCESS;
}

static
ISTATUS
SpectrumMipmapLookupTextureFilteringTrilinear(
    _In_ PCSPECTRUM_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ float_t dsdx,
    _In_ float_t dsdy,
    _In_ float_t dtdx,
    _In_ float_t dtdy,
    _In_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    dsdx = fabs(dsdx);
    dsdy = fabs(dsdy);
    dtdx = fabs(dtdx);
    dtdy = fabs(dtdy);

    float_t max = IMax(dsdx, IMax(dsdy, IMax(dtdx, dtdy)));
    float_t level =
        mipmap->last_level_index_fp + FloatTLog2(IMax(max, (float_t)1e-8));

    if (level < (float_t)0.0)
    {
        ISTATUS status =
            SpectrumMipmapLookupWithTriangleFilter(mipmap,
                                                   0,
                                                   s,
                                                   t,
                                                   compositor,
                                                   spectrum);

        return status;
    }

    if (level >= mipmap->last_level_index_fp)
    {
        ISTATUS status =
            SpectrumMipmapLookupWithTriangleFilter(mipmap,
                                                   mipmap->num_levels - 1,
                                                   s,
                                                   t,
                                                   compositor,
                                                   spectrum);

        return status;
    }

#if FLT_EVAL_METHOD	== 0
    float delta, level0;
    delta = modff(level, &level0);
#elif FLT_EVAL_METHOD == 1
    double delta, level0;
    delta = modf(level, &level0);
#elif FLT_EVAL_METHOD == 2
    long double delta, level0;
    delta = modfl(level, &level0);
#endif

    PCSPECTRUM spectrum0;
    ISTATUS status =
        SpectrumMipmapLookupWithTriangleFilter(mipmap,
                                               (size_t)level0,
                                               s,
                                               t,
                                               compositor,
                                               &spectrum0);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PCSPECTRUM spectrum1;
    status =
        SpectrumMipmapLookupWithTriangleFilter(mipmap,
                                               (size_t)level0 + 1,
                                               s,
                                               t,
                                               compositor,
                                               &spectrum1);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 spectrum0,
                                                 delta,
                                                 &spectrum0);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = SpectrumCompositorAttenuatedAddSpectra(compositor,
                                                    spectrum0,
                                                    spectrum1,
                                                    (float_t)1.0 - delta,
                                                    spectrum);

    return status;
}

static
ISTATUS
SpectrumMipmapEwa(
    _In_ PCSPECTRUM_MIPMAP mipmap,
    _In_ size_t level,
    _In_ float_t s,
    _In_ float_t t,
    _In_ const float_t cdst0[2],
    _In_ const float_t cdst1[2],
    _In_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    if (mipmap->num_levels <= level)
    {
        *spectrum = SpectrumMipmapLookupTexel(mipmap,
                                              mipmap->num_levels - 1,
                                              s,
                                              t);

        return ISTATUS_SUCCESS;
    }

    float_t dst0[2] = { cdst0[0], cdst0[1] };
    float_t dst1[2] = { cdst1[0], cdst1[1] };

    s = s * mipmap->levels[level].width_fp - (float_t)0.5;
    t = t * mipmap->levels[level].height_fp - (float_t)0.5;
    dst0[0] *= mipmap->levels[level].width_fp;
    dst0[1] *= mipmap->levels[level].height_fp;
    dst1[0] *= mipmap->levels[level].width_fp;
    dst1[1] *= mipmap->levels[level].height_fp;

    float_t a = dst0[1] * dst0[1] + dst1[1] * dst1[1] + (float_t)1.0;
    float_t b = (float_t)-2.0 * (dst0[0] * dst0[1] + dst1[0] * dst1[1]);
    float_t c = dst0[0] * dst0[0] + dst1[0] * dst1[0] + (float_t)1.0;
    float_t inv_f = (float_t)1.0 / (a * c - b * b * (float_t)0.25);

    a *= inv_f;
    b *= inv_f;
    c *= inv_f;

    float_t det = -b * b + (float_t)4.0 * a * c;
    float_t inv_det = (float_t)1.0 / det;

    float_t u_sqrt = sqrt(det * c);
    float_t v_sqrt = sqrt(a * det);

    int s0 = (int)ceil(s - (float_t)2.0 * inv_det * u_sqrt);
    int s1 = (int)floor(s + (float_t)2.0 * inv_det * u_sqrt);
    int t0 = (int)ceil(t - (float_t)2.0 * inv_det * v_sqrt);
    int t1 = (int)floor(t + (float_t)2.0 * inv_det * v_sqrt);

    PCSPECTRUM sum = NULL;
    float_t sum_weights = (float_t)0.0;
    for (int it = t0; it <= t1; it += 1)
    {
        float_t tt = (float_t)(it - t);
        for (int is = s0; is <= s1; is += 1)
        {
            float_t ss = (float_t)(is - s);
            float_t r2 = a * ss * ss + b * ss * tt + c * tt * tt;

            if (r2 < (float_t)1.0)
            {
                assert ((float_t)0.0 <= r2);

                size_t index = (size_t)(r2 * (float_t)EWA_LUT_SIZE);
                if (index == EWA_LUT_SIZE)
                {
                    index = EWA_LUT_SIZE - 1;
                }

                float_t sample_s =
                    (float_t)is * mipmap->levels[level].texel_width;
                float_t sample_t =
                    (float_t)it * mipmap->levels[level].texel_height;

                PCSPECTRUM value = SpectrumMipmapLookupTexel(mipmap,
                                                             level,
                                                             sample_s,
                                                             sample_t);

                float_t weight = ewa_lookup_table[index];

                ISTATUS status =
                    SpectrumCompositorAttenuatedAddSpectra(compositor,
                                                           sum,
                                                           value,
                                                           weight,
                                                           &sum);

                if (status != ISTATUS_SUCCESS)
                {
                    return status;
                }

                sum_weights += weight;
            }
        }
    }

    ISTATUS status =
        SpectrumCompositorAttenuateSpectrum(compositor,
                                            sum,
                                            (float_t)1.0 / sum_weights,
                                            spectrum);

    return status;
}

static
ISTATUS
SpectrumMipmapLookupTextureFilteringEwa(
    _In_ PCSPECTRUM_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ float_t dsdx,
    _In_ float_t dsdy,
    _In_ float_t dtdx,
    _In_ float_t dtdy,
    _In_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    float_t dst0[2] = { dsdx, dtdx };
    float_t dst1[2] = { dsdy, dtdy };

    float_t len_dst0_sq = dst0[0] * dst0[0] + dst0[1] * dst0[1];
    float_t len_dst1_sq = dst1[0] * dst1[0] + dst1[1] * dst1[1];

    if (len_dst0_sq < len_dst1_sq)
    {
        float_t temp;
        temp = dst0[0];
        dst0[0] = dst1[0];
        dst1[0] = temp;

        temp = dst0[1];
        dst0[1] = dst1[1];
        dst1[1] = temp;

        temp = len_dst0_sq;
        len_dst0_sq = len_dst1_sq;
        len_dst1_sq = temp;
    }

    float_t major_length = sqrt(len_dst0_sq);
    float_t minor_length = sqrt(len_dst1_sq);

    float_t scaled_minor_length = minor_length * mipmap->max_anisotropy;
    if (scaled_minor_length < major_length && (float_t)0.0 < minor_length)
    {
        float_t scale = major_length / scaled_minor_length;
        dsdx *= scale;
        dsdy *= scale;
        minor_length *= scale;
    }

    if (minor_length == (float_t)0.0)
    {
        *spectrum = SpectrumMipmapLookupTexel(mipmap, 0, s, t);
        return ISTATUS_SUCCESS;
    }

    float_t lod = IMax((float_t)0.0,
                        mipmap->last_level_index_fp + FloatTLog2(minor_length));
    float_t lod_floor = floor(lod);
    size_t level = (size_t)lod_floor;

    PCSPECTRUM v0;
    ISTATUS status = SpectrumMipmapEwa(mipmap,
                                       level,
                                       s,
                                       t,
                                       dst0,
                                       dst1,
                                       compositor,
                                       &v0);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PCSPECTRUM v1;
    status = SpectrumMipmapEwa(mipmap,
                               level + 1,
                               s,
                               t,
                               dst0,
                               dst1,
                               compositor,
                               &v1);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t delta = lod - lod_floor;

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 v0,
                                                 (float_t)1.0 - delta,
                                                 &v0);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = SpectrumCompositorAttenuatedAddSpectra(compositor,
                                                    v0,
                                                    v1,
                                                    delta,
                                                    spectrum);

    return status;
}

static
bool
SpectrumMipmapAllocateInternal(
    _In_ size_t width,
    _In_ size_t height,
    _In_ TEXTURE_FILTERING_ALGORITHM texture_filtering,
    _In_ float_t max_anisotropy,
    _In_ WRAP_MODE wrap_mode,
    _Out_ PSPECTRUM_MIPMAP *mipmap
    )
{
    assert(width != 0);
    assert(height != 0);
    assert(mipmap != NULL);
    assert(texture_filtering == TEXTURE_FILTERING_ALGORITHM_NONE ||
           texture_filtering == TEXTURE_FILTERING_ALGORITHM_TRILINEAR ||
           texture_filtering == TEXTURE_FILTERING_ALGORITHM_EWA);
    assert(isfinite(max_anisotropy) && (float_t)0.0 < max_anisotropy);
    assert(wrap_mode == WRAP_MODE_REPEAT ||
           wrap_mode == WRAP_MODE_BLACK ||
           wrap_mode == WRAP_MODE_CLAMP);

    size_t num_pixels;
    bool success = CheckedMultiplySizeT(width, height, &num_pixels);

    if (!success)
    {
        return false;
    }

    PSPECTRUM_MIPMAP result =
        (PSPECTRUM_MIPMAP)malloc(sizeof(SPECTRUM_MIPMAP));

    if (result == NULL)
    {
        return false;
    }

    size_t width_log_2 = SizeTLog2(width);
    size_t height_log_2 = SizeTLog2(height);

    size_t num_levels = 1;
    if (width_log_2 < height_log_2)
    {
        num_levels += width_log_2;
    }
    else
    {
        num_levels += height_log_2;
    }

    PSPECTRUM_MIPMAP_LEVEL levels =
        (PSPECTRUM_MIPMAP_LEVEL)calloc(num_levels, sizeof(SPECTRUM_MIPMAP_LEVEL));

    if (levels == NULL)
    {
        free(result);
        return false;
    }

    result->levels = levels;
    result->num_levels = num_levels;
    result->texture_filtering = texture_filtering;
    result->wrap_mode = wrap_mode;
    result->max_anisotropy = max_anisotropy;
    result->last_level_index_fp = num_levels - 1;

    for (size_t i = 0; i < num_levels; i++)
    {
        PSPECTRUM *texels =
            (PSPECTRUM*)calloc(width * height, sizeof(PSPECTRUM));

        if (texels == NULL)
        {
            SpectrumMipmapFree(result);
            return false;
        }

        levels[i].texels = texels;
        levels[i].width = width;
        levels[i].height = height;
        levels[i].width_fp = (float_t)width;
        levels[i].height_fp = (float_t)height;
        levels[i].texel_width = (float_t)1.0 / (float_t)width;
        levels[i].texel_height = (float_t)1.0 / (float_t)height;

        width /= 2;
        height /= 2;
    }

    *mipmap = result;

    return true;
}

//
// Spectrum Mipmap Functions
//

ISTATUS
SpectrumMipmapAllocate(
    _In_reads_(height * width) const COLOR3 texels[],
    _In_ size_t width,
    _In_ size_t height,
    _In_ TEXTURE_FILTERING_ALGORITHM texture_filtering,
    _In_ float_t max_anisotropy,
    _In_ WRAP_MODE wrap_mode,
    _Inout_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _Out_ PSPECTRUM_MIPMAP *mipmap
    )
{
    if (texels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (width == 0 || (width & (width - 1)) != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (height == 0 || (height & (height - 1)) != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (texture_filtering != TEXTURE_FILTERING_ALGORITHM_NONE &&
        texture_filtering != TEXTURE_FILTERING_ALGORITHM_TRILINEAR &&
        texture_filtering != TEXTURE_FILTERING_ALGORITHM_EWA)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(max_anisotropy) || max_anisotropy <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (wrap_mode != WRAP_MODE_REPEAT &&
        wrap_mode != WRAP_MODE_BLACK &&
        wrap_mode != WRAP_MODE_CLAMP)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (color_extrapolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    PSPECTRUM_MIPMAP result;
    bool success = SpectrumMipmapAllocateInternal(width,
                                                  height,
                                                  texture_filtering,
                                                  max_anisotropy,
                                                  wrap_mode,
                                                  &result);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    ISTATUS status =
        ColorExtrapolatorPrepareToComputeSpectra(color_extrapolator,
                                                 width * height);

    if (status != ISTATUS_SUCCESS)
    {
        SpectrumMipmapFree(result);
        return status;
    }

    for (size_t i = 0; i < width * height; i++)
    {
        if (!ColorValidate(texels[i]))
        {
            SpectrumMipmapFree(result);
            return ISTATUS_INVALID_ARGUMENT_00;
        }

        status = ColorExtrapolatorComputeSpectrum(color_extrapolator,
                                                  texels[i],
                                                  result->levels[0].texels + i);

        if (status != ISTATUS_SUCCESS)
        {
            SpectrumMipmapFree(result);
            return status;
        }
    }

    PCOLOR3 working = NULL;
    PCCOLOR3 working_const = texels;
    for (size_t i = 1; i < result->num_levels; i++)
    {
        PCOLOR3 new_working = DownsampleColors(working_const,
                                               result->levels[i - 1].width,
                                               result->levels[i - 1].height,
                                               &result->levels[i].width,
                                               &result->levels[i].height);

        free(working);

        if (new_working == NULL)
        {
            SpectrumMipmapFree(result);
            return ISTATUS_ALLOCATION_FAILED;
        }

        working_const = new_working;
        working = new_working;

        size_t num_samples = result->levels[i].height * result->levels[i].width;

        status =
            ColorExtrapolatorPrepareToComputeSpectra(color_extrapolator,
                                                     num_samples);

        if (status != ISTATUS_SUCCESS)
        {
            free(working);
            SpectrumMipmapFree(result);
            return status;
        }

        for (size_t j = 0; j < num_samples; j++)
        {
            status =
                ColorExtrapolatorComputeSpectrum(color_extrapolator,
                                                 working[j],
                                                 result->levels[i].texels + j);

            if (status != ISTATUS_SUCCESS)
            {
                free(working);
                SpectrumMipmapFree(result);
                return status;
            }
        }
    }

    free(working);

    *mipmap = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
SpectrumMipmapLookup(
    _In_ PCSPECTRUM_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(s))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(t))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    ISTATUS status =
        SpectrumMipmapLookupTextureFilteringNone(mipmap,
                                                 s,
                                                 t,
                                                 compositor,
                                                 spectrum);

    return status;
}

ISTATUS
SpectrumMipmapFilteredLookup(
    _In_ PCSPECTRUM_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ float_t dsdx,
    _In_ float_t dsdy,
    _In_ float_t dtdx,
    _In_ float_t dtdy,
    _In_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(s))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(t))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!isfinite(dsdx))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(dsdy))
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (!isfinite(dtdx))
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (!isfinite(dtdy))
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }

    if (mipmap->texture_filtering == TEXTURE_FILTERING_ALGORITHM_NONE)
    {
        ISTATUS status =
            SpectrumMipmapLookupTextureFilteringNone(mipmap,
                                                     s,
                                                     t,
                                                     compositor,
                                                     spectrum);

        return status;
    }
    else if (mipmap->texture_filtering == TEXTURE_FILTERING_ALGORITHM_TRILINEAR)
    {
        ISTATUS status =
            SpectrumMipmapLookupTextureFilteringTrilinear(mipmap,
                                                          s,
                                                          t,
                                                          dsdx,
                                                          dsdy,
                                                          dtdx,
                                                          dtdy,
                                                          compositor,
                                                          spectrum);

        return status;
    }

    assert(mipmap->texture_filtering == TEXTURE_FILTERING_ALGORITHM_EWA);
    ISTATUS status =
        SpectrumMipmapLookupTextureFilteringEwa(mipmap,
                                                s,
                                                t,
                                                dsdx,
                                                dsdy,
                                                dtdx,
                                                dtdy,
                                                compositor,
                                                spectrum);

    return status;
}

ISTATUS
SpectrumMipmapGetDimensions(
    _In_ PCSPECTRUM_MIPMAP mipmap,
    _Out_ size_t* levels,
    _Out_ size_t* width,
    _Out_ size_t* height
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (levels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (width == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (height == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    *levels = mipmap->num_levels;
    *width = mipmap->levels[0].width;
    *height = mipmap->levels[0].height;

    return ISTATUS_SUCCESS;
}

ISTATUS
SpectrumMipmapTexelLookup(
    _In_ PCSPECTRUM_MIPMAP mipmap,
    _In_ size_t level,
    _In_ size_t x,
    _In_ size_t y,
    _Out_ PCSPECTRUM* spectrum
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (mipmap->num_levels < level)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (mipmap->levels[level].width < x)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (mipmap->levels[level].height < y)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    size_t level_width = mipmap->levels[level].width;
    *spectrum = mipmap->levels[level].texels[x + level_width * y];

    return ISTATUS_SUCCESS;
}

void
SpectrumMipmapFree(
    _In_opt_ _Post_invalid_ PSPECTRUM_MIPMAP mipmap
    )
{
    if (mipmap == NULL)
    {
        return;
    }

    for (size_t i = 0; i < mipmap->num_levels; i++)
    {
        for (size_t j = 0;
             j < mipmap->levels[i].height * mipmap->levels[i].width;
             j++)
        {
            SpectrumRelease(mipmap->levels[i].texels[j]);
        }

        free(mipmap->levels[i].texels);
    }

    free(mipmap->levels);
    free(mipmap);
}

//
// Reflector Mipmap Types
//

typedef struct _REFLECTOR_MIPMAP_LEVEL {
    _Field_size_(width * height) PREFLECTOR *texels;
    size_t width;
    size_t height;
    float_t width_fp;
    float_t height_fp;
    float_t texel_width;
    float_t texel_height;
} REFLECTOR_MIPMAP_LEVEL, *PREFLECTOR_MIPMAP_LEVEL;

typedef const struct REFLECTOR_MIPMAP_LEVEL *PCREFLECTOR_MIPMAP_LEVEL;

struct _REFLECTOR_MIPMAP {
    _Field_size_(num_levels) PREFLECTOR_MIPMAP_LEVEL levels;
    size_t num_levels;
    TEXTURE_FILTERING_ALGORITHM texture_filtering;
    WRAP_MODE wrap_mode;
    float_t max_anisotropy;
    float_t last_level_index_fp;
};

//
// Reflector Static Functions
//

static
PCREFLECTOR
ReflectorMipmapLookupTexel(
    _In_ PCREFLECTOR_MIPMAP mipmap,
    _In_ size_t level,
    _In_ float_t s,
    _In_ float_t t
    )
{
    if (mipmap->wrap_mode == WRAP_MODE_REPEAT)
    {
#if FLT_EVAL_METHOD	== 0
        float s_intpart, t_intpart;
        s = modff(s, &s_intpart);
        t = modff(t, &t_intpart);
#elif FLT_EVAL_METHOD == 1
        double s_intpart, t_intpart;
        s = modf(s, &s_intpart);
        t = modf(t, &t_intpart);
#elif FLT_EVAL_METHOD == 2
        long double s_intpart, t_intpart;
        s = modfl(s, &s_intpart);
        t = modfl(t, &t_intpart);
#endif

        if (s < (float_t)0.0)
        {
            s = (float_t)1.0 + s;
        }

        if (t < (float_t)0.0)
        {
            t = (float_t)1.0 + t;
        }
    }
    else if (mipmap->wrap_mode == WRAP_MODE_CLAMP)
    {
        s = IMin(IMax((float_t)0.0, s), (float_t)1.0);
        t = IMin(IMax((float_t)0.0, t), (float_t)1.0);
    }
    else if (s < (float_t)0.0 || (float_t)1.0 < s ||
             t < (float_t)0.0 || (float_t)1.0 < t)
    {
        assert(mipmap->wrap_mode == WRAP_MODE_BLACK);
        return NULL;
    }

    size_t x = (size_t)floor(mipmap->levels[level].width_fp * s);

    if (x == mipmap->levels[level].width)
    {
        x -= 1;
    }

    size_t y = (size_t)floor(mipmap->levels[level].height_fp * t);

    if (y == mipmap->levels[level].height)
    {
        y -= 1;
    }

    return mipmap->levels[level].texels[y * mipmap->levels[level].width + x];
}

static
ISTATUS
ReflectorMipmapLookupWithTriangleFilter(
    _In_ PCREFLECTOR_MIPMAP mipmap,
    _In_ size_t level,
    _In_ float_t s,
    _In_ float_t t,
    _In_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    if (mipmap->num_levels <= level)
    {
        level = mipmap->num_levels - 1;
    }

    float_t scaled_s = s * mipmap->levels[level].width_fp;
    float_t scaled_t = t * mipmap->levels[level].height_fp;

    float_t scaled_s0 = floor(scaled_s - (float_t)0.5) + (float_t)0.5;
    float_t scaled_t0 = floor(scaled_t - (float_t)0.5) + (float_t)0.5;

    float_t s0 = scaled_s0 * mipmap->levels[level].texel_width;
    float_t t0 = scaled_t0 * mipmap->levels[level].texel_height;

    float_t ds = scaled_s - scaled_s0;
    float_t dt = scaled_t - scaled_t0;

    ds = IMax((float_t)0.0, IMin(ds, (float_t)1.0));
    dt = IMax((float_t)0.0, IMin(dt, (float_t)1.0));

    float_t one_minus_ds = (float_t)1.0 - ds;
    float_t one_minus_dt = (float_t)1.0 - dt;

    float_t s1 = s0 + mipmap->levels[level].texel_width;
    float_t t1 = t0 + mipmap->levels[level].texel_height;

    PCREFLECTOR texel = ReflectorMipmapLookupTexel(mipmap, level, s0, t0);

    PCREFLECTOR result;
    ISTATUS status =
        ReflectorCompositorAttenuateReflector(compositor,
                                              texel,
                                              one_minus_ds * one_minus_dt,
                                              &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    texel = ReflectorMipmapLookupTexel(mipmap, level, s0, t1);

    status =
        ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                   result,
                                                   texel,
                                                   one_minus_ds * dt,
                                                   &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    texel = ReflectorMipmapLookupTexel(mipmap, level, s1, t0);

    status =
        ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                   result,
                                                   texel,
                                                   ds * one_minus_dt,
                                                   &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    texel = ReflectorMipmapLookupTexel(mipmap, level, s1, t1);

    status =
        ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                   result,
                                                   texel,
                                                   ds * dt,
                                                   reflector);

    return status;
}

ISTATUS
ReflectorMipmapLookupTextureFilteringNone(
    _In_ PCREFLECTOR_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    *reflector = ReflectorMipmapLookupTexel(mipmap, 0, s, t);
    return ISTATUS_SUCCESS;
}

static
ISTATUS
ReflectorMipmapLookupTextureFilteringTrilinear(
    _In_ PCREFLECTOR_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ float_t dsdx,
    _In_ float_t dsdy,
    _In_ float_t dtdx,
    _In_ float_t dtdy,
    _In_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    dsdx = fabs(dsdx);
    dsdy = fabs(dsdy);
    dtdx = fabs(dtdx);
    dtdy = fabs(dtdy);

    float_t max = IMax(dsdx, IMax(dsdy, IMax(dtdx, dtdy)));
    float_t level =
        mipmap->last_level_index_fp + FloatTLog2(IMax(max, (float_t)1e-8));

    if (level < (float_t)0.0)
    {
        ISTATUS status =
            ReflectorMipmapLookupWithTriangleFilter(mipmap,
                                                    0,
                                                    s,
                                                    t,
                                                    compositor,
                                                    reflector);

        return status;
    }

    if (level >= mipmap->last_level_index_fp)
    {
        ISTATUS status =
            ReflectorMipmapLookupWithTriangleFilter(mipmap,
                                                    mipmap->num_levels - 1,
                                                    s,
                                                    t,
                                                    compositor,
                                                    reflector);

        return status;
    }

#if FLT_EVAL_METHOD	== 0
    float delta, level0;
    delta = modff(level, &level0);
#elif FLT_EVAL_METHOD == 1
    double delta, level0;
    delta = modf(level, &level0);
#elif FLT_EVAL_METHOD == 2
    long double delta, level0;
    delta = modfl(level, &level0);
#endif

    PCREFLECTOR reflector0;
    ISTATUS status =
        ReflectorMipmapLookupWithTriangleFilter(mipmap,
                                                (size_t)level0,
                                                s,
                                                t,
                                                compositor,
                                                &reflector0);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PCREFLECTOR reflector1;
    status =
        ReflectorMipmapLookupWithTriangleFilter(mipmap,
                                                (size_t)level0 + 1,
                                                s,
                                                t,
                                                compositor,
                                                &reflector1);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   reflector0,
                                                   delta,
                                                   &reflector0);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                        reflector0,
                                                        reflector1,
                                                        (float_t)1.0 - delta,
                                                        reflector);

    return status;
}

static
ISTATUS
ReflectorMipmapEwa(
    _In_ PCREFLECTOR_MIPMAP mipmap,
    _In_ size_t level,
    _In_ float_t s,
    _In_ float_t t,
    _In_ const float_t cdst0[2],
    _In_ const float_t cdst1[2],
    _In_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    if (mipmap->num_levels <= level)
    {
        *reflector = ReflectorMipmapLookupTexel(mipmap,
                                                mipmap->num_levels - 1,
                                                s,
                                                t);

        return ISTATUS_SUCCESS;
    }

    float_t dst0[2] = { cdst0[0], cdst0[1] };
    float_t dst1[2] = { cdst1[0], cdst1[1] };

    s = s * mipmap->levels[level].width_fp - (float_t)0.5;
    t = t * mipmap->levels[level].height_fp - (float_t)0.5;
    dst0[0] *= mipmap->levels[level].width_fp;
    dst0[1] *= mipmap->levels[level].height_fp;
    dst1[0] *= mipmap->levels[level].width_fp;
    dst1[1] *= mipmap->levels[level].height_fp;

    float_t a = dst0[1] * dst0[1] + dst1[1] * dst1[1] + (float_t)1.0;
    float_t b = (float_t)-2.0 * (dst0[0] * dst0[1] + dst1[0] * dst1[1]);
    float_t c = dst0[0] * dst0[0] + dst1[0] * dst1[0] + (float_t)1.0;
    float_t inv_f = (float_t)1.0 / (a * c - b * b * (float_t)0.25);

    a *= inv_f;
    b *= inv_f;
    c *= inv_f;

    float_t det = -b * b + (float_t)4.0 * a * c;
    float_t inv_det = (float_t)1.0 / det;

    float_t u_sqrt = sqrt(det * c);
    float_t v_sqrt = sqrt(a * det);

    int s0 = (int)ceil(s - (float_t)2.0 * inv_det * u_sqrt);
    int s1 = (int)floor(s + (float_t)2.0 * inv_det * u_sqrt);
    int t0 = (int)ceil(t - (float_t)2.0 * inv_det * v_sqrt);
    int t1 = (int)floor(t + (float_t)2.0 * inv_det * v_sqrt);

    PCREFLECTOR sum = NULL;
    float_t sum_weights = (float_t)0.0;
    for (int it = t0; it <= t1; it += 1)
    {
        float_t tt = (float_t)(it - t);
        for (int is = s0; is <= s1; is += 1)
        {
            float_t ss = (float_t)(is - s);
            float_t r2 = a * ss * ss + b * ss * tt + c * tt * tt;

            if (r2 < (float_t)1.0)
            {
                assert ((float_t)0.0 <= r2);

                size_t index = (size_t)(r2 * (float_t)EWA_LUT_SIZE);
                if (index == EWA_LUT_SIZE)
                {
                    index = EWA_LUT_SIZE - 1;
                }

                float_t sample_s =
                    (float_t)is * mipmap->levels[level].texel_width;
                float_t sample_t =
                    (float_t)it * mipmap->levels[level].texel_height;

                PCREFLECTOR value = ReflectorMipmapLookupTexel(mipmap,
                                                               level,
                                                               sample_s,
                                                               sample_t);

                float_t weight = ewa_lookup_table[index];

                ISTATUS status =
                    ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                               sum,
                                                               value,
                                                               weight,
                                                               &sum);

                if (status != ISTATUS_SUCCESS)
                {
                    return status;
                }

                sum_weights += weight;
            }
        }
    }

    ISTATUS status =
        ReflectorCompositorAttenuateReflector(compositor,
                                              sum,
                                              (float_t)1.0 / sum_weights,
                                              reflector);

    return status;
}

static
ISTATUS
ReflectorMipmapLookupTextureFilteringEwa(
    _In_ PCREFLECTOR_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ float_t dsdx,
    _In_ float_t dsdy,
    _In_ float_t dtdx,
    _In_ float_t dtdy,
    _In_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    float_t dst0[2] = { dsdx, dtdx };
    float_t dst1[2] = { dsdy, dtdy };

    float_t len_dst0_sq = dst0[0] * dst0[0] + dst0[1] * dst0[1];
    float_t len_dst1_sq = dst1[0] * dst1[0] + dst1[1] * dst1[1];

    if (len_dst0_sq < len_dst1_sq)
    {
        float_t temp;
        temp = dst0[0];
        dst0[0] = dst1[0];
        dst1[0] = temp;

        temp = dst0[1];
        dst0[1] = dst1[1];
        dst1[1] = temp;

        temp = len_dst0_sq;
        len_dst0_sq = len_dst1_sq;
        len_dst1_sq = temp;
    }

    float_t major_length = sqrt(len_dst0_sq);
    float_t minor_length = sqrt(len_dst1_sq);

    float_t scaled_minor_length = minor_length * mipmap->max_anisotropy;
    if (scaled_minor_length < major_length && (float_t)0.0 < minor_length)
    {
        float_t scale = major_length / scaled_minor_length;
        dsdx *= scale;
        dsdy *= scale;
        minor_length *= scale;
    }

    if (minor_length == (float_t)0.0)
    {
        *reflector = ReflectorMipmapLookupTexel(mipmap, 0, s, t);
        return ISTATUS_SUCCESS;
    }

    float_t lod = IMax((float_t)0.0,
                        mipmap->last_level_index_fp + FloatTLog2(minor_length));
    float_t lod_floor = floor(lod);
    size_t level = (size_t)lod_floor;

    PCREFLECTOR v0;
    ISTATUS status = ReflectorMipmapEwa(mipmap,
                                        level,
                                        s,
                                        t,
                                        dst0,
                                        dst1,
                                        compositor,
                                        &v0);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PCREFLECTOR v1;
    status = ReflectorMipmapEwa(mipmap,
                                level + 1,
                                s,
                                t,
                                dst0,
                                dst1,
                                compositor,
                                &v1);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t delta = lod - lod_floor;

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   v0,
                                                   (float_t)1.0 - delta,
                                                   &v0);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                        v0,
                                                        v1,
                                                        delta,
                                                        reflector);

    return status;
}

static
bool
ReflectorMipmapAllocateInternal(
    _In_ size_t width,
    _In_ size_t height,
    _In_ TEXTURE_FILTERING_ALGORITHM texture_filtering,
    _In_ float_t max_anisotropy,
    _In_ WRAP_MODE wrap_mode,
    _Out_ PREFLECTOR_MIPMAP *mipmap
    )
{
    assert(width != 0);
    assert(height != 0);
    assert(mipmap != NULL);
    assert(texture_filtering == TEXTURE_FILTERING_ALGORITHM_NONE ||
           texture_filtering == TEXTURE_FILTERING_ALGORITHM_TRILINEAR ||
           texture_filtering == TEXTURE_FILTERING_ALGORITHM_EWA);
    assert(isfinite(max_anisotropy) && (float_t)0.0 < max_anisotropy);
    assert(wrap_mode == WRAP_MODE_REPEAT ||
           wrap_mode == WRAP_MODE_BLACK ||
           wrap_mode == WRAP_MODE_CLAMP);

    size_t num_pixels;
    bool success = CheckedMultiplySizeT(width, height, &num_pixels);

    if (!success)
    {
        return false;
    }

    PREFLECTOR_MIPMAP result =
        (PREFLECTOR_MIPMAP)malloc(sizeof(REFLECTOR_MIPMAP));

    if (result == NULL)
    {
        return false;
    }

    size_t width_log_2 = SizeTLog2(width);
    size_t height_log_2 = SizeTLog2(height);

    size_t num_levels = 1;
    if (width_log_2 < height_log_2)
    {
        num_levels += width_log_2;
    }
    else
    {
        num_levels += height_log_2;
    }

    PREFLECTOR_MIPMAP_LEVEL levels =
        (PREFLECTOR_MIPMAP_LEVEL)calloc(num_levels, sizeof(REFLECTOR_MIPMAP_LEVEL));

    if (levels == NULL)
    {
        free(result);
        return false;
    }

    result->levels = levels;
    result->num_levels = num_levels;
    result->texture_filtering = texture_filtering;
    result->wrap_mode = wrap_mode;
    result->max_anisotropy = max_anisotropy;
    result->last_level_index_fp = num_levels - 1;

    for (size_t i = 0; i < num_levels; i++)
    {
        PREFLECTOR *texels =
            (PREFLECTOR*)calloc(width * height, sizeof(PREFLECTOR));

        if (texels == NULL)
        {
            ReflectorMipmapFree(result);
            return false;
        }

        levels[i].texels = texels;
        levels[i].width = width;
        levels[i].height = height;
        levels[i].width_fp = (float_t)width;
        levels[i].height_fp = (float_t)height;
        levels[i].texel_width = (float_t)1.0 / (float_t)width;
        levels[i].texel_height = (float_t)1.0 / (float_t)height;

        width /= 2;
        height /= 2;
    }

    *mipmap = result;

    return true;
}

//
// Reflector Mipmap Functions
//

ISTATUS
ReflectorMipmapAllocate(
    _In_reads_(height * width) const COLOR3 texels[],
    _In_ size_t width,
    _In_ size_t height,
    _In_ TEXTURE_FILTERING_ALGORITHM texture_filtering,
    _In_ float_t max_anisotropy,
    _In_ WRAP_MODE wrap_mode,
    _Inout_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _Out_ PREFLECTOR_MIPMAP *mipmap
    )
{
    if (texels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (width == 0 || (width & (width - 1)) != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (height == 0 || (height & (height - 1)) != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (texture_filtering != TEXTURE_FILTERING_ALGORITHM_NONE &&
        texture_filtering != TEXTURE_FILTERING_ALGORITHM_TRILINEAR &&
        texture_filtering != TEXTURE_FILTERING_ALGORITHM_EWA)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(max_anisotropy) || max_anisotropy <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (wrap_mode != WRAP_MODE_REPEAT &&
        wrap_mode != WRAP_MODE_BLACK &&
        wrap_mode != WRAP_MODE_CLAMP)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (color_extrapolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    PREFLECTOR_MIPMAP result;
    bool success = ReflectorMipmapAllocateInternal(width,
                                                   height,
                                                   texture_filtering,
                                                   max_anisotropy,
                                                   wrap_mode,
                                                   &result);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    ISTATUS status =
        ColorExtrapolatorPrepareToComputeReflectors(color_extrapolator,
                                                    width * height);

    if (status != ISTATUS_SUCCESS)
    {
        ReflectorMipmapFree(result);
        return status;
    }

    for (size_t i = 0; i < width * height; i++)
    {
        if (!ColorValidate(texels[i]))
        {
            ReflectorMipmapFree(result);
            return ISTATUS_INVALID_ARGUMENT_00;
        }

        status = ColorExtrapolatorComputeReflector(color_extrapolator,
                                                   texels[i],
                                                   result->levels[0].texels + i);

        if (status != ISTATUS_SUCCESS)
        {
            ReflectorMipmapFree(result);
            return status;
        }
    }

    PCOLOR3 working = NULL;
    PCCOLOR3 working_const = texels;
    for (size_t i = 1; i < result->num_levels; i++)
    {
        PCOLOR3 new_working = DownsampleColors(working_const,
                                               result->levels[i - 1].width,
                                               result->levels[i - 1].height,
                                               &result->levels[i].width,
                                               &result->levels[i].height);

        free(working);

        if (new_working == NULL)
        {
            ReflectorMipmapFree(result);
            return ISTATUS_ALLOCATION_FAILED;
        }

        working_const = new_working;
        working = new_working;

        size_t num_samples = result->levels[i].height * result->levels[i].width;

        status =
            ColorExtrapolatorPrepareToComputeReflectors(color_extrapolator,
                                                        num_samples);

        if (status != ISTATUS_SUCCESS)
        {
            free(working);
            ReflectorMipmapFree(result);
            return status;
        }

        for (size_t j = 0; j < num_samples; j++)
        {
            status =
                ColorExtrapolatorComputeReflector(color_extrapolator,
                                                  working[j],
                                                  result->levels[i].texels + j);

            if (status != ISTATUS_SUCCESS)
            {
                free(working);
                ReflectorMipmapFree(result);
                return status;
            }
        }
    }

    free(working);

    *mipmap = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
ReflectorMipmapLookup(
    _In_ PCREFLECTOR_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(s))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(t))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    ISTATUS status =
        ReflectorMipmapLookupTextureFilteringNone(mipmap,
                                                  s,
                                                  t,
                                                  compositor,
                                                  reflector);

    return status;
}

ISTATUS
ReflectorMipmapFilteredLookup(
    _In_ PCREFLECTOR_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ float_t dsdx,
    _In_ float_t dsdy,
    _In_ float_t dtdx,
    _In_ float_t dtdy,
    _In_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(s))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(t))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!isfinite(dsdx))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(dsdy))
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (!isfinite(dtdx))
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (!isfinite(dtdy))
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }

    if (mipmap->texture_filtering == TEXTURE_FILTERING_ALGORITHM_NONE)
    {
        ISTATUS status =
            ReflectorMipmapLookupTextureFilteringNone(mipmap,
                                                      s,
                                                      t,
                                                      compositor,
                                                      reflector);

        return status;
    }
    else if (mipmap->texture_filtering == TEXTURE_FILTERING_ALGORITHM_TRILINEAR)
    {
        ISTATUS status =
            ReflectorMipmapLookupTextureFilteringTrilinear(mipmap,
                                                           s,
                                                           t,
                                                           dsdx,
                                                           dsdy,
                                                           dtdx,
                                                           dtdy,
                                                           compositor,
                                                           reflector);

        return status;
    }

    assert(mipmap->texture_filtering == TEXTURE_FILTERING_ALGORITHM_EWA);
    ISTATUS status =
        ReflectorMipmapLookupTextureFilteringEwa(mipmap,
                                                 s,
                                                 t,
                                                 dsdx,
                                                 dsdy,
                                                 dtdx,
                                                 dtdy,
                                                 compositor,
                                                 reflector);

    return status;
}

ISTATUS
ReflectorMipmapGetDimensions(
    _In_ PCREFLECTOR_MIPMAP mipmap,
    _Out_ size_t* levels,
    _Out_ size_t* width,
    _Out_ size_t* height
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (levels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (width == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (height == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    *levels = mipmap->num_levels;
    *width = mipmap->levels[0].width;
    *height = mipmap->levels[0].height;

    return ISTATUS_SUCCESS;
}

ISTATUS
ReflectorMipmapTexelLookup(
    _In_ PCREFLECTOR_MIPMAP mipmap,
    _In_ size_t level,
    _In_ size_t x,
    _In_ size_t y,
    _Out_ PCREFLECTOR* reflector
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (mipmap->num_levels < level)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (mipmap->levels[level].width < x)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (mipmap->levels[level].height < y)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    size_t level_width = mipmap->levels[level].width;
    *reflector = mipmap->levels[level].texels[x + level_width * y];

    return ISTATUS_SUCCESS;
}

void
ReflectorMipmapFree(
    _In_opt_ _Post_invalid_ PREFLECTOR_MIPMAP mipmap
    )
{
    if (mipmap == NULL)
    {
        return;
    }

    for (size_t i = 0; i < mipmap->num_levels; i++)
    {
        for (size_t j = 0;
             j < mipmap->levels[i].height * mipmap->levels[i].width;
             j++)
        {
            ReflectorRelease(mipmap->levels[i].texels[j]);
        }

        free(mipmap->levels[i].texels);
    }

    free(mipmap->levels);
    free(mipmap);
}

//
// Float Mipmap Types
//

typedef struct _FLOAT_MIPMAP_LEVEL {
    _Field_size_(width * height) float_t *texels;
    size_t width;
    size_t height;
    float_t width_fp;
    float_t height_fp;
    float_t texel_width;
    float_t texel_height;
} FLOAT_MIPMAP_LEVEL, *PFLOAT_MIPMAP_LEVEL;

typedef const struct FLOAT_MIPMAP_LEVEL *PCFLOAT_MIPMAP_LEVEL;

struct _FLOAT_MIPMAP {
    _Field_size_(num_levels) PFLOAT_MIPMAP_LEVEL levels;
    size_t num_levels;
    TEXTURE_FILTERING_ALGORITHM texture_filtering;
    WRAP_MODE wrap_mode;
    float_t max_anisotropy;
    float_t last_level_index_fp;
};

//
// Float Mipmap Static Functions
//

static
bool
FloatMipmapAllocate(
    _In_ size_t width,
    _In_ size_t height,
    _In_ TEXTURE_FILTERING_ALGORITHM texture_filtering,
    _In_ float_t max_anisotropy,
    _In_ WRAP_MODE wrap_mode,
    _Out_ PFLOAT_MIPMAP *mipmap
    )
{
    assert(width != 0);
    assert(height != 0);
    assert(mipmap != NULL);
    assert(texture_filtering == TEXTURE_FILTERING_ALGORITHM_NONE ||
           texture_filtering == TEXTURE_FILTERING_ALGORITHM_TRILINEAR ||
           texture_filtering == TEXTURE_FILTERING_ALGORITHM_EWA);
    assert(isfinite(max_anisotropy) && (float_t)0.0 < max_anisotropy);
    assert(wrap_mode == WRAP_MODE_REPEAT ||
           wrap_mode == WRAP_MODE_BLACK ||
           wrap_mode == WRAP_MODE_CLAMP);

    size_t num_pixels;
    bool success = CheckedMultiplySizeT(width, height, &num_pixels);

    if (!success)
    {
        return false;
    }

    PFLOAT_MIPMAP result =
        (PFLOAT_MIPMAP)malloc(sizeof(FLOAT_MIPMAP));

    if (result == NULL)
    {
        return false;
    }

    size_t width_log_2 = SizeTLog2(width);
    size_t height_log_2 = SizeTLog2(height);

    size_t num_levels = 1;
    if (width_log_2 < height_log_2)
    {
        num_levels += width_log_2;
    }
    else
    {
        num_levels += height_log_2;
    }

    PFLOAT_MIPMAP_LEVEL levels =
        (PFLOAT_MIPMAP_LEVEL)calloc(num_levels, sizeof(FLOAT_MIPMAP_LEVEL));

    if (levels == NULL)
    {
        free(result);
        return false;
    }

    result->levels = levels;
    result->num_levels = num_levels;
    result->texture_filtering = texture_filtering;
    result->wrap_mode = wrap_mode;
    result->max_anisotropy = max_anisotropy;
    result->last_level_index_fp = num_levels - 1;

    for (size_t i = 0; i < num_levels; i++)
    {
        float_t *texels =
            (float_t*)calloc(width * height, sizeof(float_t));

        if (texels == NULL)
        {
            FloatMipmapFree(result);
            return false;
        }

        levels[i].texels = texels;
        levels[i].width = width;
        levels[i].height = height;
        levels[i].width_fp = (float_t)width;
        levels[i].height_fp = (float_t)height;
        levels[i].texel_width = (float_t)1.0 / (float_t)width;
        levels[i].texel_height = (float_t)1.0 / (float_t)height;

        width >>= 1;
        height >>= 1;
    }

    *mipmap = result;

    return true;
}

_Ret_writes_maybenull_(width * height / 4)
static
float_t*
DownsampleFloats(
    _In_reads_(width * height) const float_t *texels,
    _In_ size_t width,
    _In_ size_t height,
    _Out_ size_t* new_width,
    _Out_ size_t* new_height
    )
{
    assert(texels != NULL);
    assert(width != 0 && (width & (width - 1)) == 0);
    assert(height != 0 && (height & (height - 1)) == 0);

    *new_width = width / 2;
    *new_height = height / 2;

    float_t *values = 
        (float_t*)calloc(*new_width * *new_height, sizeof(float_t));

    if (values == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < *new_height; i++)
    {
        for (size_t j = 0; j < *new_width; j++)
        {
            size_t source_row = i * 2;
            size_t source_column = j * 2;

            float_t value =
                texels[source_row * width + source_column] +
                texels[source_row * width + source_column + 1] +
                texels[(source_row + 1) * width + source_column] +
                texels[(source_row + 1) * width + source_column + 1];

            values[i * *new_width + j] = value * (float_t)0.25;
        }
    }

    return values;
}

static
float_t
FloatMipmapLookupTexel(
    _In_ PCFLOAT_MIPMAP mipmap,
    _In_ size_t level,
    _In_ float_t s,
    _In_ float_t t
    )
{
    if (mipmap->wrap_mode == WRAP_MODE_REPEAT)
    {
#if FLT_EVAL_METHOD	== 0
        float s_intpart, t_intpart;
        s = modff(s, &s_intpart);
        t = modff(t, &t_intpart);
#elif FLT_EVAL_METHOD == 1
        double s_intpart, t_intpart;
        s = modf(s, &s_intpart);
        t = modf(t, &t_intpart);
#elif FLT_EVAL_METHOD == 2
        long double s_intpart, t_intpart;
        s = modfl(s, &s_intpart);
        t = modfl(t, &t_intpart);
#endif

        if (s < (float_t)0.0)
        {
            s = (float_t)1.0 + s;
        }

        if (t < (float_t)0.0)
        {
            t = (float_t)1.0 + t;
        }
    }
    else if (mipmap->wrap_mode == WRAP_MODE_CLAMP)
    {
        s = IMin(IMax((float_t)0.0, s), (float_t)1.0);
        t = IMin(IMax((float_t)0.0, t), (float_t)1.0);
    }
    else if (s < (float_t)0.0 || (float_t)1.0 < s ||
             t < (float_t)0.0 || (float_t)1.0 < t)
    {
        assert(mipmap->wrap_mode == WRAP_MODE_BLACK);
        return (float_t)0.0;
    }

    size_t x = (size_t)floor(mipmap->levels[level].width_fp * s);

    if (x == mipmap->levels[level].width)
    {
        x -= 1;
    }

    size_t y = (size_t)floor(mipmap->levels[level].height_fp * t);

    if (y == mipmap->levels[level].height)
    {
        y -= 1;
    }

    return mipmap->levels[level].texels[y * mipmap->levels[level].width + x];
}

static
float_t
FloatMipmapLookupWithTriangleFilter(
    _In_ PCFLOAT_MIPMAP mipmap,
    _In_ size_t level,
    _In_ float_t s,
    _In_ float_t t
    )
{
    if (mipmap->num_levels <= level)
    {
        level = mipmap->num_levels - 1;
    }

    float_t scaled_s = s * mipmap->levels[level].width_fp;
    float_t scaled_t = t * mipmap->levels[level].height_fp;

    float_t scaled_s0 = floor(scaled_s - (float_t)0.5) + (float_t)0.5;
    float_t scaled_t0 = floor(scaled_t - (float_t)0.5) + (float_t)0.5;

    float_t s0 = scaled_s0 * mipmap->levels[level].texel_width;
    float_t t0 = scaled_t0 * mipmap->levels[level].texel_height;

    float_t ds = scaled_s - scaled_s0;
    float_t dt = scaled_t - scaled_t0;

    ds = IMax((float_t)0.0, IMin(ds, (float_t)1.0));
    dt = IMax((float_t)0.0, IMin(dt, (float_t)1.0));

    float_t one_minus_ds = (float_t)1.0 - ds;
    float_t one_minus_dt = (float_t)1.0 - dt;

    float_t s1 = s0 + mipmap->levels[level].texel_width;
    float_t t1 = t0 + mipmap->levels[level].texel_height;

    float_t result =
        (one_minus_ds * one_minus_dt * FloatMipmapLookupTexel(mipmap, level, s0, t0)) +
        (one_minus_ds * dt * FloatMipmapLookupTexel(mipmap, level, s0, t1)) +
        (ds * one_minus_dt * FloatMipmapLookupTexel(mipmap, level, s1, t0)) +
        (ds * dt * FloatMipmapLookupTexel(mipmap, level, s1, t1));

    return result;
}

static
void
FloatMipmapLookupTextureFilteringNone(
    _In_ PCFLOAT_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _Out_ float_t *value
    )
{
    *value = FloatMipmapLookupTexel(mipmap, 0, s, t);
}

static
void
FloatMipmapLookupTextureFilteringTrilinear(
    _In_ PCFLOAT_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ float_t dsdx,
    _In_ float_t dsdy,
    _In_ float_t dtdx,
    _In_ float_t dtdy,
    _Out_ float_t *value
    )
{
    dsdx = fabs(dsdx);
    dsdy = fabs(dsdy);
    dtdx = fabs(dtdx);
    dtdy = fabs(dtdy);

    float_t max = IMax(dsdx, IMax(dsdy, IMax(dtdx, dtdy)));
    float_t level =
        mipmap->last_level_index_fp + FloatTLog2(IMax(max, (float_t)1e-8));

    if (level < (float_t)0.0)
    {
        *value = FloatMipmapLookupWithTriangleFilter(mipmap, 0, s, t);
    }
    else if (level >= mipmap->last_level_index_fp)
    {
        *value = FloatMipmapLookupWithTriangleFilter(mipmap,
                                                     mipmap->num_levels - 1,
                                                     s,
                                                     t);
    }
    else
    {
#if FLT_EVAL_METHOD	== 0
        float delta, level0;
        delta = modff(level, &level0);
#elif FLT_EVAL_METHOD == 1
        double delta, level0;
        delta = modf(level, &level0);
#elif FLT_EVAL_METHOD == 2
        long double delta, level0;
        delta = modfl(level, &level0);
#endif

        float_t value0 =
            FloatMipmapLookupWithTriangleFilter(mipmap,
                                                (size_t)level0,
                                                s,
                                                t);

        float_t value1 =
            FloatMipmapLookupWithTriangleFilter(mipmap,
                                                (size_t)level0 + 1,
                                                s,
                                                t);

        *value = delta * value0 + ((float_t)1.0 - delta) * value1;
    }
}

static
float_t
FloatMipmapEwa(
    _In_ PCFLOAT_MIPMAP mipmap,
    _In_ size_t level,
    _In_ float_t s,
    _In_ float_t t,
    _In_ const float_t cdst0[2],
    _In_ const float_t cdst1[2]
    )
{
    if (mipmap->num_levels <= level)
    {
        return FloatMipmapLookupTexel(mipmap, mipmap->num_levels - 1, s, t);
    }

    float_t dst0[2] = { cdst0[0], cdst0[1] };
    float_t dst1[2] = { cdst1[0], cdst1[1] };

    s = s * mipmap->levels[level].width_fp - (float_t)0.5;
    t = t * mipmap->levels[level].height_fp - (float_t)0.5;
    dst0[0] *= mipmap->levels[level].width_fp;
    dst0[1] *= mipmap->levels[level].height_fp;
    dst1[0] *= mipmap->levels[level].width_fp;
    dst1[1] *= mipmap->levels[level].height_fp;

    float_t a = dst0[1] * dst0[1] + dst1[1] * dst1[1] + (float_t)1.0;
    float_t b = (float_t)-2.0 * (dst0[0] * dst0[1] + dst1[0] * dst1[1]);
    float_t c = dst0[0] * dst0[0] + dst1[0] * dst1[0] + (float_t)1.0;
    float_t inv_f = (float_t)1.0 / (a * c - b * b * (float_t)0.25);

    a *= inv_f;
    b *= inv_f;
    c *= inv_f;

    float_t det = -b * b + (float_t)4.0 * a * c;
    float_t inv_det = (float_t)1.0 / det;

    float_t u_sqrt = sqrt(det * c);
    float_t v_sqrt = sqrt(a * det);

    int s0 = (int)ceil(s - (float_t)2.0 * inv_det * u_sqrt);
    int s1 = (int)floor(s + (float_t)2.0 * inv_det * u_sqrt);
    int t0 = (int)ceil(t - (float_t)2.0 * inv_det * v_sqrt);
    int t1 = (int)floor(t + (float_t)2.0 * inv_det * v_sqrt);

    float_t sum = (float_t)0.0;
    float_t sum_weights = (float_t)0.0;
    for (int it = t0; it <= t1; it += 1)
    {
        float_t tt = (float_t)(it - t);
        for (int is = s0; is <= s1; is += 1)
        {
            float_t ss = (float_t)(is - s);
            float_t r2 = a * ss * ss + b * ss * tt + c * tt * tt;

            if (r2 < (float_t)1.0)
            {
                assert ((float_t)0.0 <= r2);

                size_t index = (size_t)(r2 * (float_t)EWA_LUT_SIZE);
                if (index == EWA_LUT_SIZE)
                {
                    index = EWA_LUT_SIZE - 1;
                }

                float_t sample_s =
                    (float_t)is * mipmap->levels[level].texel_width;
                float_t sample_t =
                    (float_t)it * mipmap->levels[level].texel_height;

                float_t value = FloatMipmapLookupTexel(mipmap,
                                                       level,
                                                       sample_s,
                                                       sample_t);

                float_t weight = ewa_lookup_table[index];
                sum += value * weight;
                sum_weights += weight;
            }
        }
    }

    return sum / sum_weights;
}

static
void
FloatMipmapLookupTextureFilteringEwa(
    _In_ PCFLOAT_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ float_t dsdx,
    _In_ float_t dsdy,
    _In_ float_t dtdx,
    _In_ float_t dtdy,
    _Out_ float_t *value
    )
{
    float_t dst0[2] = { dsdx, dtdx };
    float_t dst1[2] = { dsdy, dtdy };

    float_t len_dst0_sq = dst0[0] * dst0[0] + dst0[1] * dst0[1];
    float_t len_dst1_sq = dst1[0] * dst1[0] + dst1[1] * dst1[1];

    if (len_dst0_sq < len_dst1_sq)
    {
        float_t temp;
        temp = dst0[0];
        dst0[0] = dst1[0];
        dst1[0] = temp;

        temp = dst0[1];
        dst0[1] = dst1[1];
        dst1[1] = temp;

        temp = len_dst0_sq;
        len_dst0_sq = len_dst1_sq;
        len_dst1_sq = temp;
    }

    float_t major_length = sqrt(len_dst0_sq);
    float_t minor_length = sqrt(len_dst1_sq);

    float_t scaled_minor_length = minor_length * mipmap->max_anisotropy;
    if (scaled_minor_length < major_length && (float_t)0.0 < minor_length)
    {
        float_t scale = major_length / scaled_minor_length;
        dsdx *= scale;
        dsdy *= scale;
        minor_length *= scale;
    }

    if (minor_length == (float_t)0.0)
    {
        *value = FloatMipmapLookupTexel(mipmap, 0, s, t);
        return;
    }

    float_t lod = IMax((float_t)0.0,
                        mipmap->last_level_index_fp + FloatTLog2(minor_length));
    float_t lod_floor = floor(lod);
    size_t level = (size_t)lod_floor;

    float_t v0 = FloatMipmapEwa(mipmap, level, s, t, dst0, dst1);
    float_t v1 = FloatMipmapEwa(mipmap, level + 1, s, t, dst0, dst1);

    float_t delta = lod - lod_floor;

    *value = ((float_t)1.0 - delta) * v0 + delta * v1;
}

//
// Float Mipmap Functions
//

ISTATUS
FloatMipmapAllocateFromFloats(
    _In_reads_(height * width) const float_t texels[],
    _In_ size_t width,
    _In_ size_t height,
    _In_ TEXTURE_FILTERING_ALGORITHM texture_filtering,
    _In_ float_t max_anisotropy,
    _In_ WRAP_MODE wrap_mode,
    _Out_ PFLOAT_MIPMAP *mipmap
    )
{
    if (texels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (width == 0 || (width & (width - 1)) != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (height == 0 || (height & (height - 1)) != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (texture_filtering != TEXTURE_FILTERING_ALGORITHM_NONE &&
        texture_filtering != TEXTURE_FILTERING_ALGORITHM_TRILINEAR &&
        texture_filtering != TEXTURE_FILTERING_ALGORITHM_EWA)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(max_anisotropy) || max_anisotropy <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (wrap_mode != WRAP_MODE_REPEAT &&
        wrap_mode != WRAP_MODE_BLACK &&
        wrap_mode != WRAP_MODE_CLAMP)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    PFLOAT_MIPMAP result;
    bool success = FloatMipmapAllocate(width,
                                       height,
                                       texture_filtering,
                                       max_anisotropy,
                                       wrap_mode,
                                       &result);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < width * height; i++)
    {
        if (!isfinite(texels[i]) || texels[i] < (float_t)0.0)
        {
            FloatMipmapFree(result);
            return ISTATUS_INVALID_ARGUMENT_00;
        }

        result->levels[0].texels[i] = texels[i];
    }

    float_t *working = NULL;
    const float_t *working_const = texels;
    for (size_t i = 1; i < result->num_levels; i++)
    {
        float_t *new_working = DownsampleFloats(working_const, 
                                                result->levels[i - 1].width,
                                                result->levels[i - 1].height,
                                                &result->levels[i].width,
                                                &result->levels[i].height);

        free(working);

        if (new_working == NULL)
        {
            FloatMipmapFree(result);
            return ISTATUS_ALLOCATION_FAILED;
        }

        working_const = new_working;
        working = new_working;

        for (size_t j = 0;
             j < result->levels[i].height * result->levels[i].width;
             j++)
        {
            result->levels[i].texels[j] = working[j];
        }
    }

    free(working);

    *mipmap = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
FloatMipmapAllocateFromLuma(
    _In_reads_(height * width) const COLOR3 texels[],
    _In_ size_t width,
    _In_ size_t height,
    _In_ TEXTURE_FILTERING_ALGORITHM texture_filtering,
    _In_ float_t max_anisotropy,
    _In_ WRAP_MODE wrap_mode,
    _Out_ PFLOAT_MIPMAP *mipmap
    )
{
    if (texels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (width == 0 || (width & (width - 1)) != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (height == 0 || (height & (height - 1)) != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (texture_filtering != TEXTURE_FILTERING_ALGORITHM_NONE &&
        texture_filtering != TEXTURE_FILTERING_ALGORITHM_TRILINEAR &&
        texture_filtering != TEXTURE_FILTERING_ALGORITHM_EWA)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(max_anisotropy) || max_anisotropy <= (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (wrap_mode != WRAP_MODE_REPEAT &&
        wrap_mode != WRAP_MODE_BLACK &&
        wrap_mode != WRAP_MODE_CLAMP)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    PFLOAT_MIPMAP result;
    bool success = FloatMipmapAllocate(width,
                                       height,
                                       texture_filtering,
                                       max_anisotropy,
                                       wrap_mode,
                                       &result);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < width * height; i++)
    {
        if (!ColorValidate(texels[i]))
        {
            FloatMipmapFree(result);
            return ISTATUS_INVALID_ARGUMENT_00;
        }

        result->levels[0].texels[i] = ColorToLuma(texels[i]);
    }

    float_t *working = NULL;
    const float_t *working_const = result->levels[0].texels;
    for (size_t i = 1; i < result->num_levels; i++)
    {
        float_t *new_working = DownsampleFloats(working_const, 
                                                result->levels[i - 1].width,
                                                result->levels[i - 1].height,
                                                &result->levels[i].width,
                                                &result->levels[i].height);

        free(working);

        if (new_working == NULL)
        {
            FloatMipmapFree(result);
            return ISTATUS_ALLOCATION_FAILED;
        }

        working_const = new_working;
        working = new_working;

        for (size_t j = 0;
             j < result->levels[i].height * result->levels[i].width;
             j++)
        {
            result->levels[i].texels[j] = working[j];
        }
    }

    free(working);

    *mipmap = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
FloatMipmapLookup(
    _In_ PCFLOAT_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _Out_ float_t *value
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(s))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(t))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (value == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    FloatMipmapLookupTextureFilteringNone(mipmap,
                                          s,
                                          t,
                                          value);

    return ISTATUS_SUCCESS;
}

ISTATUS
FloatMipmapFilteredLookup(
    _In_ PCFLOAT_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ float_t dsdx,
    _In_ float_t dsdy,
    _In_ float_t dtdx,
    _In_ float_t dtdy,
    _Out_ float_t *value
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(s))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(t))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!isfinite(dsdx))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(dsdy))
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (!isfinite(dtdx))
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (!isfinite(dtdy))
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (value == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (mipmap->texture_filtering == TEXTURE_FILTERING_ALGORITHM_NONE)
    {
        FloatMipmapLookupTextureFilteringNone(mipmap,
                                              s,
                                              t,
                                              value);
    }
    else if (mipmap->texture_filtering == TEXTURE_FILTERING_ALGORITHM_TRILINEAR)
    {
        FloatMipmapLookupTextureFilteringTrilinear(mipmap,
                                                   s,
                                                   t,
                                                   dsdx,
                                                   dsdy,
                                                   dtdx,
                                                   dtdy,
                                                   value);
    }
    else
    {
        assert(mipmap->texture_filtering == TEXTURE_FILTERING_ALGORITHM_EWA);
        FloatMipmapLookupTextureFilteringEwa(mipmap,
                                             s,
                                             t,
                                             dsdx,
                                             dsdy,
                                             dtdx,
                                             dtdy,
                                             value);
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
FloatMipmapGetDimensions(
    _In_ PCFLOAT_MIPMAP mipmap,
    _Out_ size_t* levels,
    _Out_ size_t* width,
    _Out_ size_t* height
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (levels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (width == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (height == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    *levels = mipmap->num_levels;
    *width = mipmap->levels[0].width;
    *height = mipmap->levels[0].height;

    return ISTATUS_SUCCESS;
}

ISTATUS
FloatMipmapTexelLookup(
    _In_ PCFLOAT_MIPMAP mipmap,
    _In_ size_t level,
    _In_ size_t x,
    _In_ size_t y,
    _Out_ float_t* value
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (mipmap->num_levels < level)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (mipmap->levels[level].width < x)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (mipmap->levels[level].height < y)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (value == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    size_t level_width = mipmap->levels[level].width;
    *value = mipmap->levels[level].texels[x + level_width * y];

    return ISTATUS_SUCCESS;
}

void
FloatMipmapFree(
    _In_opt_ _Post_invalid_ PFLOAT_MIPMAP mipmap
    )
{
    if (mipmap == NULL)
    {
        return;
    }

    for (size_t i = 0; i < mipmap->num_levels; i++)
    {
        free(mipmap->levels[i].texels);
    }

    free(mipmap->levels);
    free(mipmap);
}