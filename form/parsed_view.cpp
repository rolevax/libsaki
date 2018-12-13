#include "parsed_view.h"

namespace saki
{



ParsedView4Ready::ParsedView4Ready(const Parsed4 &parsed)
    : mParsed(parsed)
{
    assert(parsed.step4() == 0);
}

///
/// \brief Get the isorider of this waiting hand
/// \return The isorider if this hand is in isoride form;
///         nullopt otherwise
///
std::optional<T34> ParsedView4Ready::getIsorider() const
{
    std::optional<T34> res;

    const C34 &last = mParsed.heads().back();
    if (last.type() == C34::Type::FREE)
        res = last.head();

    return res;
}

///
/// \brief Get 2-tile comelds in this parse result
/// \return Empty stacktor if the parse result in in isoride form;
///         Size-1 stactor if in sequence-waiting form;
///         Size-2 stactor if in bi-bump form.
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



} // namespace saki
