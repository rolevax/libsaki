#ifndef SAKI_GEN_H
#define SAKI_GEN_H

#include "form.h"
#include "rand.h"



namespace saki
{



class Gen
{
public:
    /*
    static Gen genForm4Point(int point, int selfWind, int roundWind,
                             const RuleInfo &rule, bool ron);
                             */
    static Gen genForm4FuHan(Rand &rand, int fu, int han, int selfWind, int roundWind,
                             const RuleInfo &rule, bool ron);
    static Gen genForm4Mangan(Rand &rand, int han, int selfWind, int roundWind,
                              const RuleInfo &rule, bool ron);
    static Gen genForm4(Rand &rand, int triCent, int quadCent, int openCent,
                        int selfWind, int roundWind, const RuleInfo &rule, bool ron);
    static Gen genForm4F110(Rand &rand, int point, int selfWind, const RuleInfo &rule, bool ron);
    static Gen genForm4F110Han(Rand &rand, int han, int selfWind, const RuleInfo &rule, bool ron);
    static Gen genForm4F110Horse(Rand &rand, int selfWind, const RuleInfo &rule, bool ron);

private:
    Gen(const Form &form, const Hand &hand, const T37 &pick);

    static Hand genFormal4(Rand &rand, int triCent, int quadCent, int openCent);
    static Hand genWild4(Rand &rand, int triCent, int quadCent, int openCent);
    static void genInfo(Rand &rand, PointInfo &info, T34 pick, const Hand &h,
                        bool ron, bool f110);

public:
    Form form;
    Hand hand;
    T37 pick;
};



} // namespace saki



#endif // SAKI_GEN_H


