#ifndef SAKI_AI_SENRIYAMA_H
#define SAKI_AI_SENRIYAMA_H

#include "ai.h"



namespace saki
{



class AiToki : public Ai
{
public:
    AI_CTORS(AiToki)
    Action forward(const TableView &view) override;
};

// the old impl was stateful
// that not good. make all ai classes stateless, such that they can replace
// a human player in network mode anytime in future vision
/*
class AiToki : public Ai
{
public:
    void readLog(const AiView &view, const SkillExpr &expr) override;

protected:
    bool toAttack(const AiView &view, std::vector<Rival> &readyers) override;
    bool toRiichi(const AiView &view) override;
    AiResult thinkAttack(const AiView &view) override;
    int discardAttack(const AiView &view) override;

private:
    void checkRound(const AiView &view);

private:
    int round = -1; // to trigger checkRound() when round-0
    int extra = 0;
    std::array<bool, 34> safes;
    bool sawIppatsu;
};
*/



} // namespace saki



#endif // SAKI_AI_SENRIYAMA_H


