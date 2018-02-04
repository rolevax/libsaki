#include "senriyama_sera.h"
#include "../table/table.h"
#include "../ai/ai.h"
#include "../form/form.h"
#include "../util/string_enum.h"
#include "../util/misc.h"



namespace saki
{



void Sera::onDraw(const Table &table, Mount &mount, Who who, bool rinshan)
{
    if (who != mSelf || rinshan || table.inIppatsuCycle())
        return;

    const Hand &hand = table.getHand(mSelf);
    const FormCtx &ctx = table.getFormCtx(mSelf);
    const Rule &rule = table.getRule();
    const auto &drids = mount.getDrids();
    if (hand.ready()) {
        for (T34 t : hand.effA()) {
            Form f(hand, T37(t.id34()), ctx, rule, drids);
            int ronHan = f.han();
            int tsumoHan = hand.isMenzen() ? ronHan + 1 : ronHan;
            bool pinfu = f.yakus().test(Yaku::PF);
            bool ok = tsumoHan >= (4 + pinfu);
            bool modest = tsumoHan <= 7;
            mount.lightA(t, ok ? (modest ? 400 : 100) : -200);
        }
    } else {
        accelerate(mount, hand, table.getRiver(mSelf), 30);
        if (hand.ctAka5() + drids % hand < 2) {
            for (const T37 &t : drids)
                mount.lightA(t.dora(), 80);

            mount.lightA(T37(Suit::M, 0), 30);
            mount.lightA(T37(Suit::P, 0), 30);
            mount.lightA(T37(Suit::S, 0), 30);
        }
    }
}



} // namespace saki
