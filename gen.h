#ifndef GEN_H
#define GEN_H

#include "form.h"

/*
class Gen
{
public:
    static Gen genForm4Point(int point, int selfWind, int roundWind,
                             const RuleInfo &rule, bool ron);
    static Gen genForm4FuHan(int fu, int han, int selfWind, int roundWind,
                             const RuleInfo &rule, bool ron);
    static Gen genForm4Mangan(int han, int selfWind, int roundWind,
                              const RuleInfo &rule, bool ron);
    static Gen genForm4(int triCent, int quadCent, int openCent,
                                          int selfWind, int roundWind, const RuleInfo &rule, bool ron);
    static Gen genForm4F110(int point, int selfWind, const RuleInfo &rule, bool ron);
    static Gen genForm4F110Han(int han, int selfWind, const RuleInfo &rule, bool ron);
    static Gen genForm4F110Horse(int selfWind, const RuleInfo &rule, bool ron);

private:
    Gen(std::unique_ptr<const Form> &form, const Hand &hand, Tile pick);

    static Hand genFormal4(int triCent, int quadCent, int openCent);
    static Hand genWild4(int triCent, int quadCent, int openCent);
    static void genInfo(PointInfo &info, Tile &pick, const Hand &h,
                        bool ron, bool f110);

public:
    std::unique_ptr<const Form> form;
    Hand hand;
    Tile pick;
};
*/



#endif // GEN_H


