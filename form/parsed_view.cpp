#include "parsed_view.h"

namespace saki
{



ParsedView4Ready::ParsedView4Ready(const Parsed4 &parsed)
    : mParsed(parsed)
{
    assert(parsed.step4() == 0);
}

const Parsed4 &ParsedView4Ready::getParsed() const
{
    return mParsed;
}

///
/// \brief Get the possible isorider of this waiting hand
/// \return One isorider tile if the hand is in 13 - 3k isoride form;
///         Two isorider candidates if the hand is in 14 - 3k isoride form;
///         Empty if the hand is not in isoride form.
///
util::Stactor<T34, 2> ParsedView4Ready::getIsoriders() const
{
    util::Stactor<T34, 2> res;

    // An isoride ready hand must have no 2-tile comeld
    if (get2s().empty()) {
        auto isFree = [](const C34 &c) { return c.type() == C34::Type::FREE; };
        const auto &heads = mParsed.heads();
        if (isFree(heads.back())) {
            if (heads.size() >= 2 && isFree(heads[heads.size() - 2])) {
                res.pushBack(heads[heads.size() - 2].head());
                res.pushBack(heads.back().head());
            } else {
                res.pushBack(heads.back().head());
            }
        }
    }

    return res;
}

///
/// \brief Get 2-tile comelds in this parse result
/// \return Empty stacktor if the parse result in in isoride form;
///         Size-2 stactor if in sequence-waiting or bi-bump form.
///
util::Stactor<C34, 2> ParsedView4Ready::get2s() const
{
    util::Stactor<C34, 2> res;

    const auto &heads = mParsed.heads();
    auto is2 = [](const C34 &c) { return c.is2(); };
    std::copy_if(heads.begin(), heads.end(), std::back_inserter(res), is2);

    return res;
}

ParsedView4Step1::ParsedView4Step1(const Parsed4 &parsed)
    : mParsed(parsed)
{
    assert(parsed.step4() == 1);
}

util::Stactor<T34, 2> ParsedView4Step1::getFrees() const
{
    util::Stactor<T34, 2> res;

    for (const C34 &c : mParsed.heads())
        if (c.type() == C34::Type::FREE)
            res.pushBack(c.head());

    return res;
}



} // namespace saki
