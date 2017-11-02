#ifndef SAKI_GEN_H
#define SAKI_GEN_H

#include "../form/form.h"
#include "../util/rand.h"



namespace saki
{



class Gen
{
public:
    /*
    static Gen genForm4Point(int point, int selfWind, int roundWind,
                             const Rule &rule, bool ron);
                             */
    static Gen genForm4FuHan(util::Rand &rand, int fu, int han, int selfWind, int roundWind,
                             const Rule &rule, bool ron);
    static Gen genForm4Mangan(util::Rand &rand, int han, int selfWind, int roundWind,
                              const Rule &rule, bool ron);
    static Gen genForm4(util::Rand &rand, int triCent, int quadCent, int openCent,
                        int selfWind, int roundWind, const Rule &rule, bool ron);
    static Gen genForm4F110(util::Rand &rand, int point, int selfWind, const Rule &rule, bool ron);
    static Gen genForm4F110Han(util::Rand &rand, int han, int selfWind, const Rule &rule, bool ron);
    static Gen genForm4F110Horse(util::Rand &rand, int selfWind, const Rule &rule, bool ron);

private:
    Gen(const Form &form, const Hand &hand, const T37 &pick);

    static Hand genFormal4(util::Rand &rand, int triCent, int quadCent, int openCent);
    static Hand genWild4(util::Rand &rand, int triCent, int quadCent, int openCent);
    static void genInfo(util::Rand &rand, FormCtx &ctx, T34 pick, const Hand &h,
                        bool ron, bool f110);

public:
    Form form;
    Hand hand;
    T37 pick;
};



} // namespace saki



#endif // SAKI_GEN_H


