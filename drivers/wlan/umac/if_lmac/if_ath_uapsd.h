/*
 *  Copyright (c) 2008 Atheros Communications Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _ATH_UAPSD_H_
#define _ATH_UAPSD_H_

#ifdef ATH_SUPPORT_UAPSD
/*
 * External Definitions
 */
wbuf_t ath_net80211_uapsd_allocqosnullframe(ieee80211_handle_t ieee);
wbuf_t ath_net80211_uapsd_getqosnullframe(ieee80211_node_t node, wbuf_t wbuf, int ac);
void ath_net80211_uapsd_retqosnullframe(ieee80211_node_t node, wbuf_t wbuf);
void ath_net80211_uapsd_eospindicate(ieee80211_node_t node, wbuf_t wbuf, int txok);
bool ath_net80211_check_uapsdtrigger(ieee80211_handle_t ieee, struct ieee80211_qosframe *qwh, u_int16_t keyix, bool isr_context);
void ath_net80211_uapsd_deliverdata(ieee80211_handle_t ieee, struct ieee80211_qosframe *qwh, u_int16_t keyix, u_int8_t is_trig, bool isr_context);

static INLINE void
ath_uapsd_txq_update(struct ath_softc_net80211 *scn, HAL_TXQ_INFO *qi, int ac)
{
    /*
    * set VO parameters in UAPSD queue
    */
    if (ac == WME_AC_VO)
        scn->sc_ops->txq_update(scn->sc_dev, scn->sc_uapsd_qnum, qi);
}

static INLINE void
ath_uapsd_txctl_update(struct ath_softc_net80211 *scn, wbuf_t wbuf, ieee80211_tx_control_t *txctl)
{
    txctl->isuapsd = wbuf_is_uapsd(wbuf);
    /*
     * UAPSD frames go to a dedicated hardware queue.
     */
    if (txctl->isuapsd) {
        txctl->qnum = scn->sc_uapsd_qnum;
    }
}

static INLINE void
ath_uapsd_attach(struct ath_softc_net80211 *scn)
{
    struct ieee80211com     *ic = &scn->sc_ic;
    scn->sc_uapsd_qnum = scn->sc_ops->tx_get_qnum(scn->sc_dev, HAL_TX_QUEUE_UAPSD, 0);
    /*
     * UAPSD capable
     */
    if (scn->sc_ops->have_capability(scn->sc_dev, ATH_CAP_UAPSD)) {
        ieee80211com_set_cap(ic, IEEE80211_C_UAPSD);
        IEEE80211_UAPSD_ENABLE(ic);
    }
}

static INLINE void
ath_uapsd_pwrsave_check(wbuf_t wbuf, struct ieee80211_node *ni)
{
    wlan_if_t vap = ni->ni_vap;
    if (WME_UAPSD_AC_ISDELIVERYENABLED(wbuf_get_priority(wbuf), ni))
    {
        /* U-APSD power save queue for delivery enabled AC */
        wbuf_set_uapsd(wbuf);
        wbuf_set_moredata(wbuf);
        IEEE80211_NODE_STAT(ni, tx_uapsd);

        if ((vap->iv_set_tim != NULL) && IEEE80211_NODE_UAPSD_USETIM(ni)) {
            vap->iv_set_tim(ni, 1);
        }
    }
}
void ath_net80211_uapsd_pause_control(struct ieee80211_node *ni, bool pause);
#else

#define ath_uapsd_txq_update(_scn, _qi, _ac)
#define ath_uapsd_txctl_update(_scn, _wbuf, _txctl)
#define ath_uapsd_attach(_scn)
#define ath_uapsd_pwrsave_check(_wbuf, _ni)
#define ath_net80211_uapsd_pause_control(ni,pause)

#endif /* ATH_SUPPORT_UAPSD */

#endif /* _ATH_UAPSD_H_ */
