#ifndef SAKI_AI_H
#define SAKI_AI_H

#include "../table/table_tester.h"



namespace saki
{



/// should give same output when given same input
/// need to be reproducible for implementing prediction-related skills
class Ai : public TableDecider
{
public:
    class Limits
    {
    public:
        bool noBark() const;
        bool noRiichi() const;
        bool noAnkan() const;
        bool noOut(const T37 &t) const;

        void addNoBark();
        void addNoRiichi();
        void addNoAnkan();
        void addNoOutAka5();
        void addNoOut(T34 t);

    private:
        bool mNoBark = false;
        bool mNoRiichi = false;
        bool mNoAnkan = false;
        bool mNoOutAka5 = false;
        std::bitset<34> mNoOut34s;
    };

    static std::unique_ptr<Ai> create(Girl::Id id);
    static Action thinkStdDrawnAttack(const TableView &view);

    Ai() = default;
    virtual ~Ai() = default;

    Ai(const Ai &copy) = delete;
    Ai &operator=(const Ai &assign) = delete;

    Decision decide(const TableView &view) final;

protected:
    Action maxHappy(const TableView &view);
    virtual Action thinkIrs(const TableView &view);
    virtual Action think(const TableView &view, Limits &limits);

    Action placeHolder(const TableView &view);

    void antiHatsumi(const TableView &view, Limits &limits);
    void antiToyone(const TableView &view, Limits &limits);

    Action thinkChoices(const TableView &view, Limits &limits);

    Action thinkDrawn(const TableView &view, Limits &limits);
    Action thinkDrawnAggress(const TableView &view, Limits &limits);
    Action thinkDrawnAttack(const TableView &view, Limits &limits);
    Action thinkDrawnDefend(const TableView &view, Limits &limits, const util::Stactor<Who, 3> &threats);

    Action thinkBark(const TableView &view, Limits &limits);
    Action thinkBarkAttack(const TableView &view, Limits &limits);
    Action thinkBarkDefend(const TableView &view, Limits &limits, const util::Stactor<Who, 3> &threats);

    Action thinkAttackStep(const TableView &view, const util::Range<Action> &outs);
    Action thinkAttackEff(const TableView &view, const util::Range<Action> &outs);
    Action thinkDefendChance(const TableView &view, const util::Range<Action> &outs,
                             const util::Stactor<Who, 3> &threats);

    bool afraid(const TableView &view, util::Stactor<Who, 3> &threats);
    bool testRiichi(const TableView &view, Limits &limits, Action &riichi);

    int chance(const TableView &view, Who tar, T34 t);
    int ruleChance(const TableView &view, Who tar, T34 t);
    int logicChance(const TableView &view, T34 t);

    util::Stactor<Action, 14> listOuts(const TableView &view, const Limits &limits);
    util::Stactor<Action, 14> listRiichisAsOut(const Hand &hand, const Choices::ModeDrawn &mode,
                                               const Limits &limits);
    util::Stactor<Action, 44> listCp(const Hand &hand, const Choices::ModeBark &mode,
                                     const T37 &pick, bool noChii = false);
};



} // namespace saki



#endif // SAKI_AI_H
