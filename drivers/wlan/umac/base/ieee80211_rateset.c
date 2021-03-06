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

#include <ieee80211_rateset.h>
#include <ieee80211_channel.h>

/*
 * Check if the specified rate set supports ERP.
 * NB: the rate set is assumed to be sorted.
 */
int
ieee80211_iserp_rateset(struct ieee80211com *ic, struct ieee80211_rateset *rs)
{
#define N(a)    (sizeof(a) / sizeof(a[0]))
    static const int rates[] = { 2, 4, 11, 22, 12, 24, 48 };
    int i, j;

    if (rs->rs_nrates < N(rates))
        return 0;
    for (i = 0; i < N(rates); i++) {
        for (j = 0; j < rs->rs_nrates; j++) {
            int r = rs->rs_rates[j] & IEEE80211_RATE_VAL;
            if (rates[i] == r)
                goto next;
            if (r > rates[i])
                return 0;
        }
        return 0;
next:
        ;
    }
    return 1;
#undef N
}

static const struct ieee80211_rateset basic11g[IEEE80211_MODE_MAX] = {
    { 0 },                          /* IEEE80211_MODE_AUTO */
    { 3, { 12, 24, 48     } },      /* IEEE80211_MODE_11A */
    { 2, {  2,  4         } },      /* IEEE80211_MODE_11B */
    { 4, {  2,  4, 11, 22 } },      /* IEEE80211_MODE_11G (mixed b/g) */
    { 0 },                          /* IEEE80211_MODE_FH */
    { 3, { 12, 24, 48     } },      /* IEEE80211_MODE_TURBO_A */
    { 4, {  2,  4, 11, 22 } },      /* IEEE80211_MODE_TURBO_G (mixed b/g) */
    { 3, { 12, 24, 48     } },      /* IEEE80211_MODE_11NA_HT20 */
    { 4, {  2,  4, 11, 22 } },      /* IEEE80211_MODE_11NG_HT20 (mixed b/g) */
    { 3, { 12, 24, 48     } },      /* IEEE80211_MODE_11NA_HT40PLUS */
    { 3, { 12, 24, 48     } },      /* IEEE80211_MODE_11NA_HT40MINUS */
    { 4, {  2,  4, 11, 22 } },      /* IEEE80211_MODE_11NG_HT40PLUS (mixed b/g) */
    { 4, {  2,  4, 11, 22 } },      /* IEEE80211_MODE_11NG_HT40MINUS (mixed b/g) */
};

static const struct ieee80211_rateset basicpureg[] = {
    { 7, {2, 4, 11, 22, 12, 24, 48 } },
};

static const struct ieee80211_rateset pureg[] = {
    {8, {12, 18, 24, 36, 48, 72, 96, 108} },
};

static const struct ieee80211_rateset basic_half_rates[] = {
    {3, {6, 12, 24} },
};

static const struct ieee80211_rateset basic_quarter_rates[] = {
    {3, {3, 6, 12} },
};

/*
 * Search for 11g only rate
 */
int
ieee80211_find_puregrate(u_int8_t rate)
{
    int i;
    for (i = 0; i < pureg[0].rs_nrates; i++) {
        if (pureg[0].rs_rates[i] == IEEE80211_RV(rate))
            return 1;
    }
    return 0;
}

/*
 * Mark basic rates for the 11g rate table based on the pureg setting
 */
void
ieee80211_setpuregbasicrates(struct ieee80211_rateset *rs)
{
    int i, j;

    for (i = 0; i < rs->rs_nrates; i++) {
        rs->rs_rates[i] &= IEEE80211_RATE_VAL;
        for (j = 0; j < basicpureg[0].rs_nrates; j++) {
            if (basicpureg[0].rs_rates[j] == rs->rs_rates[i]) {
                rs->rs_rates[i] |= IEEE80211_RATE_BASIC;
                break;
            }
        }
    }
}

/*
 * Mark the basic rates for the 11g rate table based on the
 * specified mode.  For 11b compatibility we mark only 11b
 * rates.  There's also a pseudo 11a-mode used to mark only
 * the basic OFDM rates; this is used to exclude 11b stations
 * from an 11g bss.
 */
void
ieee80211_set11gbasicrates(struct ieee80211_rateset *rs, enum ieee80211_phymode mode)
{
    int i, j;

    KASSERT(mode < IEEE80211_MODE_MAX, ("invalid mode %u", mode));
    for (i = 0; i < rs->rs_nrates; i++) {
        rs->rs_rates[i] &= IEEE80211_RATE_VAL;
        for (j = 0; j < basic11g[mode].rs_nrates; j++) {
            if (basic11g[mode].rs_rates[j] == rs->rs_rates[i]) {
                rs->rs_rates[i] |= IEEE80211_RATE_BASIC;
                break;
            }
        }
    }
}

/* Set basic rate based on supported value from HAL
 * XXX: Is there a better way to do it? */
void
ieee80211_setbasicrates(struct ieee80211_rateset *rs, struct ieee80211_rateset *rs_sup)
{
    int i, j;
    u_int8_t rate;
    
    for (i = 0; i < rs->rs_nrates; i++) {
        for (j = 0; j < rs_sup->rs_nrates; j++) {
            rate = rs_sup->rs_rates[j];
            if ((rs->rs_rates[i] == (rate & IEEE80211_RATE_VAL)) &&
                (rate & IEEE80211_RATE_BASIC)) {
                /* find the corresponding basic rate */
                rs->rs_rates[i] |= IEEE80211_RATE_BASIC;
            }
        }
    }
}

void
ieee80211_setbasic_half_rates(struct ieee80211_rateset *rs)
{
    int i, j;

    for (i = 0; i < rs->rs_nrates; i++) {
        rs->rs_rates[i] &= IEEE80211_RATE_VAL;
        for (j = 0; j < basic_half_rates->rs_nrates; j++) {
            if (basic_half_rates->rs_rates[j] == rs->rs_rates[i]) {
                rs->rs_rates[i] |= IEEE80211_RATE_BASIC;
                break;
            }
        }
    }
}

void
ieee80211_setbasic_quarter_rates(struct ieee80211_rateset *rs)
{
    int i, j;

    for (i = 0; i < rs->rs_nrates; i++) {
        rs->rs_rates[i] &= IEEE80211_RATE_VAL;
        for (j = 0; j < basic_quarter_rates->rs_nrates; j++) {
            if (basic_quarter_rates->rs_rates[j] == rs->rs_rates[i]) {
                rs->rs_rates[i] |= IEEE80211_RATE_BASIC;
                break;
            }
        }
    }
}

static void
ieee80211_sort_rate(struct ieee80211_rateset *rs)
{
#define RV(v)   ((v) & IEEE80211_RATE_VAL)
    int i, j;
    u_int8_t r;
    for (i = 0; i < rs->rs_nrates;i++ ) {
        for (j = i + 1; j < rs->rs_nrates; j++) {
            if (RV(rs->rs_rates[i]) > RV(rs->rs_rates[j])) {
                r = rs->rs_rates[i];
                rs->rs_rates[i] = rs->rs_rates[j];
                rs->rs_rates[j] = r;
            }
        }
    }
#undef RV
}

static void
ieee80211_xsect_rate(struct ieee80211_rateset *rs1, struct ieee80211_rateset *rs2, 
             struct ieee80211_rateset *srs)
{
#define RV(v)   ((v) & IEEE80211_RATE_VAL)
    int i, j;
    int k;
    
    k = 0;
    for (i = 0; i < rs1->rs_nrates; i++) {
        for (j = 0; j < rs2->rs_nrates; j++) {
            if (RV(rs1->rs_rates[i]) == RV(rs2->rs_rates[j])) {
                srs->rs_rates[k++] = rs2->rs_rates[j];
                break;
            }
        }
    }
    srs->rs_nrates = k;
#undef RV
}

static int
ieee80211_brs_rate_check(struct ieee80211_rateset *srs, struct ieee80211_rateset *nrs)
{
#define RV(v)   ((v) & IEEE80211_RATE_VAL)
    int i, j;
    for (i = 0; i < srs->rs_nrates; i++) {
        if (srs->rs_rates[i] & IEEE80211_RATE_BASIC) {
            for (j = 0; j < nrs->rs_nrates; j++) {
                if (RV(srs->rs_rates[i]) == RV(nrs->rs_rates[j])) {
                    break;
                }
            }
            if (j == nrs->rs_nrates) {
                return 0;
            }
        }
    }
    return 1;
#undef RV
}

static int
ieee80211_fixed_rate_check(struct ieee80211_node *ni, struct ieee80211_rateset *nrs)
{
#define RV(v)   ((v) & IEEE80211_RATE_VAL)
#if tbd
    struct ieee80211vap *vap = ni->ni_vap;
    int i;

    if ((vap->iv_fixed_rate.mode == IEEE80211_FIXED_RATE_NONE) || 
        (vap->iv_fixed_rate.mode == IEEE80211_FIXED_RATE_MCS))
        return 1;
    for (i = 0; i < nrs->rs_nrates;i++) {
        if (RV(nrs->rs_rates[i]) == vap->iv_fixed_rate.series)
            return 1;
    }
    return 0;
#else
    return 1;
#endif
#undef RV
}
    
int
ieee80211_fixed_htrate_check(struct ieee80211_node *ni, struct ieee80211_rateset *nrs)
{
#define RV(v)   ((v) & IEEE80211_RATE_VAL)
#if tbd
    struct ieee80211vap *vap = ni->ni_vap;
    int i;

    if ((vap->iv_fixed_rate.mode == IEEE80211_FIXED_RATE_NONE) || 
        !(vap->iv_fixed_rate.mode == IEEE80211_FIXED_RATE_MCS))
        return 1;
    for (i = 0; i < nrs->rs_nrates;i++) {
        if (RV(nrs->rs_rates[i]) == (vap->iv_fixed_rate.series & IEEE80211_RATE_MCS_VAL))
            return 1;
    }
    return 0;
#else
    return 1;
#endif
#undef RV
}

/* get num of spatial streams from mcs rate */
static INLINE int 
ieee80211_mcs_to_numstreams(int mcs)
{
   int numstreams = 0;
   /* single stream mcs rates */
   if ((mcs <= 7) || (mcs == 32))
       numstreams = 1;
   /* two streams mcs rates */
   if (((mcs >= 8) && (mcs <= 15)) || ((mcs >= 33) && (mcs <= 38)))
       numstreams = 2;
   /* three streams mcs rates */
   if (((mcs >= 16) && (mcs <= 23)) || ((mcs >= 39) && (mcs <= 52)))
       numstreams = 3;
   /* four streams mcs rates */
   if (((mcs >= 24) && (mcs <= 31)) || ((mcs >= 53) && (mcs <= 76)))
       numstreams = 4;
   return numstreams;
}

/*
 * Install received rate set information in the node's state block.
 */
int
ieee80211_setup_rates(struct ieee80211_node *ni,
                      const u_int8_t *rates,
                      const u_int8_t *xrates,
                      int flags)
{
    struct ieee80211_rateset *rs = &ni->ni_rates;
    struct ieee80211_rateset rrs, *irs;
    struct ieee80211vap *vap = ni->ni_vap;
    struct ieee80211com *ic = ni->ni_ic;

    memset(&rrs, 0, sizeof(rrs));
    memset(rs, 0, sizeof(*rs));
    rrs.rs_nrates = rates[1];
    OS_MEMCPY(rrs.rs_rates, rates + 2, rrs.rs_nrates);
    if (xrates != NULL) {
        u_int8_t nxrates;
        /*
         * Tack on 11g extended supported rate element.
         */
        nxrates = xrates[1];
        if (rrs.rs_nrates + nxrates > IEEE80211_RATE_MAXSIZE) {

            nxrates = IEEE80211_RATE_MAXSIZE - rrs.rs_nrates;
            IEEE80211_NOTE(vap, IEEE80211_MSG_XRATE, ni,
                "extended rate set too large;"
                " only using %u of %u rates",
                nxrates, xrates[1]);
            vap->iv_stats.is_rx_rstoobig++;
        }
        OS_MEMCPY(rrs.rs_rates + rrs.rs_nrates, xrates+2, nxrates);
        rrs.rs_nrates += nxrates;
    }
    if (IEEE80211_IS_CHAN_HALF(ni->ni_chan)) {
        irs =  IEEE80211_HALF_RATES(ic);
    } else if (IEEE80211_IS_CHAN_QUARTER(ni->ni_chan)) {
        irs =  IEEE80211_QUARTER_RATES(ic);
    } else
        irs =  &vap->iv_op_rates[ieee80211_chan2mode(ni->ni_chan)];

    if (flags & IEEE80211_F_DOSORT)
        ieee80211_sort_rate(&rrs);

    if (flags & IEEE80211_F_DOXSECT)
        ieee80211_xsect_rate(irs, &rrs, rs);
    else
        OS_MEMCPY(rs, &rrs, sizeof(rrs));

    if (flags & IEEE80211_F_DOFRATE)
        if (!ieee80211_fixed_rate_check(ni, rs))
            return 0;

    if (flags & IEEE80211_F_DOBRS)
        if (!ieee80211_brs_rate_check(irs, rs))
            return 0;

    /* ieee80211_xsect_rate() called above alters the already sorted
     * rate set. So do the sorting again. */
    if (flags & IEEE80211_F_DOSORT)
        ieee80211_sort_rate(rs);

    return 1;
}

/*
 * Install received ht rate set information in the node's state block.
 */
int
ieee80211_setup_ht_rates(struct ieee80211_node *ni,
                         u_int8_t *ie,
                         int flags)
{
    struct ieee80211_ie_htcap_cmn *htcap = (struct ieee80211_ie_htcap_cmn *)ie;
    struct ieee80211_rateset *rs = &ni->ni_htrates;
    struct ieee80211_rateset rrs, *irs;
    struct ieee80211com *ic = ni->ni_ic;
    int i,j;
    int numstreams, max_numstreams;
    

    memset(&rrs, 0, sizeof(rrs));
    memset(rs, 0, sizeof(*rs));
    j = 0;
    max_numstreams = 0;
    

    if (htcap != NULL) {
        for (i=0; i < IEEE80211_HT_RATE_SIZE; i++) {
            if (htcap->hc_mcsset[i/8] & (1<<(i%8))) {
                rrs.rs_rates[j++] = i;
                /* update the num of streams supported */
                numstreams = ieee80211_mcs_to_numstreams(i);
                if (max_numstreams < numstreams)
                    max_numstreams = numstreams;
            }
            if (j == IEEE80211_RATE_MAXSIZE) {
                IEEE80211_NOTE(ni->ni_vap, IEEE80211_MSG_XRATE, ni,
                    "ht extended rate set too large;"
                    " only using %u rates",j);
                ni->ni_vap->iv_stats.is_rx_rstoobig++;
                break;
            }
        }
    }

    ni->ni_streams = max_numstreams;

    rrs.rs_nrates = j;
    irs =  &ic->ic_sup_ht_rates[ieee80211_chan2mode(ni->ni_chan)];
    if (flags & IEEE80211_F_DOSORT)
        ieee80211_sort_rate(&rrs);

    if (flags & IEEE80211_F_DOXSECT)
        ieee80211_xsect_rate(irs, &rrs, rs);
    else
        OS_MEMCPY(rs, &rrs, sizeof(rrs));

    if (flags & IEEE80211_F_DOFRATE)
        if (!ieee80211_fixed_rate_check(ni, rs))
            return 0;

    if (flags & IEEE80211_F_DOBRS)
        if (!ieee80211_brs_rate_check(irs, rs))
            return 0;

    return 1;
}

void
ieee80211_setup_basic_ht_rates(struct ieee80211_node *ni, u_int8_t *ie)
{
#define RV(v)   ((v) & IEEE80211_RATE_VAL)
    struct ieee80211_ie_htinfo_cmn *htinfo = (struct ieee80211_ie_htinfo_cmn *)ie;
    struct ieee80211_rateset *rs = &ni->ni_htrates;
    int i,j;

    if (rs->rs_nrates) {
        for (i=0; i < IEEE80211_HT_RATE_SIZE; i++) {
            if (htinfo->hi_basicmcsset[i/8] & (1<<(i%8))) {
                for (j = 0; j < rs->rs_nrates; j++) {
                    if (RV(rs->rs_rates[j]) == i) {
                        rs->rs_rates[j] |= IEEE80211_RATE_BASIC;
                    }
                }
            }
        }
    } else {
        IEEE80211_NOTE(ni->ni_vap, IEEE80211_MSG_XRATE, ni,
            "ht rate set %s;", "empty");
    }
#undef RV
}

void
ieee80211_init_rateset(struct ieee80211com *ic)
{

    /*
     * When 11g is supported, force the rate set to
     * include basic rates suitable for a mixed b/g bss.
     */
    if (IEEE80211_SUPPORT_PHY_MODE(ic, IEEE80211_MODE_11G))
        ieee80211_set11gbasicrates(
            &ic->ic_sup_rates[IEEE80211_MODE_11G],
            IEEE80211_MODE_11G);

    if (IEEE80211_SUPPORT_PHY_MODE(ic, IEEE80211_MODE_11NG_HT20))
        ieee80211_set11gbasicrates(
            &ic->ic_sup_rates[IEEE80211_MODE_11NG_HT20],
            IEEE80211_MODE_11NG_HT20);

    if (IEEE80211_SUPPORT_PHY_MODE(ic, IEEE80211_MODE_11NG_HT40PLUS))
        ieee80211_set11gbasicrates(
            &ic->ic_sup_rates[IEEE80211_MODE_11NG_HT40PLUS],
            IEEE80211_MODE_11NG_HT40PLUS);

    if (IEEE80211_SUPPORT_PHY_MODE(ic, IEEE80211_MODE_11NG_HT40MINUS))
        ieee80211_set11gbasicrates(
            &ic->ic_sup_rates[IEEE80211_MODE_11NG_HT40MINUS],
            IEEE80211_MODE_11NG_HT40MINUS);

    if (IEEE80211_SUPPORT_PHY_MODE(ic, IEEE80211_MODE_11A)) {
        ieee80211_setbasic_half_rates(IEEE80211_HALF_RATES(ic));
    }

    if (IEEE80211_SUPPORT_PHY_MODE(ic, IEEE80211_MODE_11A)) {
        ieee80211_setbasic_quarter_rates(IEEE80211_QUARTER_RATES(ic));
    }
}

void
ieee80211_rateset_vattach(struct ieee80211vap *vap)
{
    int i;
    struct ieee80211com *ic = vap->iv_ic;
    /*
     * XXX: copy supported rateset to operational rateset
     */
    for (i = 0; i < IEEE80211_MODE_MAX; i++) {
        OS_MEMCPY(&vap->iv_op_rates[i],
                  &ic->ic_sup_rates[i],
                  sizeof(struct ieee80211_rateset));
    }
}

void
ieee80211_init_node_rates(struct ieee80211_node *ni, struct ieee80211_channel *chan)
{
    struct ieee80211vap *vap = ni->ni_vap;
    struct ieee80211com *ic = vap->iv_ic;

    if (IEEE80211_IS_CHAN_HALF(chan)) {
        OS_MEMCPY(&ni->ni_rates, IEEE80211_HALF_RATES(ic),
                  sizeof(struct ieee80211_rateset));
        return;
    } else if (IEEE80211_IS_CHAN_QUARTER(chan)) {
        OS_MEMCPY(&ni->ni_rates, IEEE80211_QUARTER_RATES(ic),
                  sizeof(struct ieee80211_rateset));
        return;
    } else {
        OS_MEMCPY(&ni->ni_rates,
                  &vap->iv_op_rates[ieee80211_chan2mode(chan)],
                  sizeof(struct ieee80211_rateset));
    }
    OS_MEMCPY(&ni->ni_htrates,
              &ic->ic_sup_ht_rates[ieee80211_chan2mode(chan)],
              sizeof(struct ieee80211_rateset));
}

int
ieee80211_check_node_rates(struct ieee80211_node *ni)
{
    struct ieee80211vap *vap = ni->ni_vap;
    struct ieee80211_rateset *srs;

    srs = &vap->iv_op_rates[ieee80211_chan2mode(ni->ni_chan)];
    if (!ieee80211_brs_rate_check(srs, &ni->ni_rates))
        return 0;
    if (!ieee80211_fixed_rate_check(ni, &ni->ni_rates))
        return 0;

    return 1;
}

/*
 * Check rate set suitability
 */
int
ieee80211_check_rate(struct ieee80211vap *vap, struct ieee80211_channel *chan, struct ieee80211_rateset *rrs)
{
#define	RV(v)	((v) & IEEE80211_RATE_VAL)
    struct ieee80211com *ic = vap->iv_ic;
    struct ieee80211_rateset *srs;
    int i,j;

    if (IEEE80211_IS_CHAN_HALF(chan)) {
        srs = &ic->ic_sup_half_rates;
    } else if (IEEE80211_IS_CHAN_QUARTER(chan)) {
        srs = &ic->ic_sup_quarter_rates;
    } else {
        srs = &vap->iv_op_rates[ieee80211_chan2mode(chan)];
    }

    if (!(vap->iv_fixed_rate.mode & IEEE80211_FIXED_RATE_MCS) &&
        (vap->iv_fixed_rate.mode != IEEE80211_FIXED_RATE_NONE)) {
        for (i = 0; i < rrs->rs_nrates; i++) {
            if (RV(rrs->rs_rates[i]) == RV(vap->iv_fixed_rate.series))
                break;
        }
        if (i == rrs->rs_nrates)
            return 0;
    }

    for (i = 0; i < rrs->rs_nrates; i++) {
        if (rrs->rs_rates[i] & IEEE80211_RATE_BASIC) {
            for (j = 0; j < srs->rs_nrates; j++) {
                if (RV(rrs->rs_rates[i]) == RV(srs->rs_rates[j]))
                    break;
            }
            if (j == srs->rs_nrates)
                return 0;
        }
    }
    return 1;
#undef RV
}

/*
 * Check HT rate set suitability
 */
int
ieee80211_check_ht_rate(struct ieee80211vap *vap, struct ieee80211_channel *chan, struct ieee80211_rateset *rrs)
{
#define	RV(v)	((v) & IEEE80211_RATE_VAL)
    struct ieee80211com *ic = vap->iv_ic;
    struct ieee80211_rateset *srs;
    int i,j;

    srs = &ic->ic_sup_ht_rates[ieee80211_chan2mode(chan)];

    if ((vap->iv_fixed_rate.mode == IEEE80211_FIXED_RATE_MCS) &&
        (vap->iv_fixed_rate.mode != IEEE80211_FIXED_RATE_NONE)) {
        for (i = 0; i < rrs->rs_nrates; i++) {
            if (RV(rrs->rs_rates[i]) == RV(vap->iv_fixed_rate.series))
                break;
        }
        if (i == rrs->rs_nrates)
            return 0;
    }
    for (i = 0; i < rrs->rs_nrates; i++)
    {
        if (rrs->rs_rates[i] & IEEE80211_RATE_BASIC) {
            for (j = 0; j < srs->rs_nrates; j++) {
                if (RV(rrs->rs_rates[i]) == RV(srs->rs_rates[j]))
                    break;
            }
            if (j == srs->rs_nrates)
                return 0;
        }
    }
    return 1;
#undef RV
}


/* Check for the 11b cck rates only */

int 
ieee80211_check_11b_rates(struct ieee80211vap *vap, struct ieee80211_rateset *rrs)
{
    int i;

#define	RV(v)	((v) & IEEE80211_RATE_VAL)
    for (i = 0; i < rrs->rs_nrates; i++) {
      if (RV(rrs->rs_rates[i]) != 0x2 && RV(rrs->rs_rates[i]) != 0x4 &&
          RV(rrs->rs_rates[i]) != 0xb && RV(rrs->rs_rates[i]) != 0x16) {
         return 0;
      }
    }
#undef RV
    return 1;
}



/*
 * Set multicast rate
 */
void
ieee80211_set_mcast_rate(struct ieee80211vap *vap)
{
#define IEEE80211_NG_BASIC_RATE	11000
#define IEEE80211_NA_BASIC_RATE	6000
    vap->iv_mcast_rate = 0;
    if (vap->iv_mcast_fixedrate != 0) {
        vap->iv_mcast_rate = vap->iv_mcast_fixedrate;
    } else {
        if (IEEE80211_IS_CHAN_2GHZ(vap->iv_bsschan))
            vap->iv_mcast_rate = IEEE80211_NG_BASIC_RATE;
        if (IEEE80211_IS_CHAN_5GHZ(vap->iv_bsschan))
            vap->iv_mcast_rate = IEEE80211_NA_BASIC_RATE;
    }
#undef IEEE80211_NG_BASIC_RATE
#undef IEEE80211_NA_BASIC_RATE
}

/*
 * Public Rateset APIs
 */

/*
 * Query supported data rates
 */
int
wlan_get_supported_rates(wlan_dev_t devhandle,
                         enum ieee80211_phymode mode,
                         u_int8_t *rates, u_int32_t len,
                         u_int32_t *nrates)
{
    struct ieee80211com *ic = devhandle;
    struct ieee80211_rateset *rs;
    int i;

    KASSERT(mode != IEEE80211_MODE_AUTO, ("Unsupported PHY mode\n"));

    rs = &ic->ic_sup_rates[mode];
    if (rs->rs_nrates > len)
        return -EINVAL;

    for (i = 0; i < len; i++) {
        rates[i] = IEEE80211_RV(rs->rs_rates[i]);
    }
    *nrates = rs->rs_nrates;

    return 0;
}

/*
 * Query operational data rates
 */
int
wlan_get_operational_rates(wlan_if_t vaphandle,
                           enum ieee80211_phymode mode,
                           u_int8_t *rates, u_int32_t len,
                           u_int32_t *nrates)
{
    struct ieee80211vap *vap = vaphandle;
    struct ieee80211_rateset *rs;
    int i;

    KASSERT(mode != IEEE80211_MODE_AUTO, ("Unsupported PHY mode\n"));

    rs = &vap->iv_op_rates[mode];
    if (rs->rs_nrates > len)
        return -EINVAL;

    for (i = 0; i < len; i++) {
        rates[i] = IEEE80211_RV(rs->rs_rates[i]);
    }
    *nrates = rs->rs_nrates;

    return 0;
}

/*
 * Set operational data rates
 */
int
wlan_set_operational_rates(wlan_if_t vaphandle,
                           enum ieee80211_phymode mode,
                           u_int8_t *rates, u_int32_t nrates)
{
    struct ieee80211vap *vap = vaphandle;
    struct ieee80211com *ic = vap->iv_ic;
    struct ieee80211_rateset *rs, *rs_sup;
    int i;

    KASSERT(mode != IEEE80211_MODE_AUTO, ("Unsupported PHY mode\n"));

    if (nrates > IEEE80211_RATE_MAXSIZE)
        return -EINVAL;

    rs = &vap->iv_op_rates[mode];
    rs_sup = &ic->ic_sup_rates[mode];

    for (i = 0; i < nrates; i++) {
        rs->rs_rates[i] = rates[i];
    }
    rs->rs_nrates = (u_int8_t)nrates;

    /* set basic rate */
    ieee80211_setbasicrates(rs, rs_sup);
    
    return 0;
}

/*
 * Query operational data rates
 */
int
wlan_get_bss_rates(wlan_if_t vaphandle,
                           u_int8_t *rates, u_int32_t len,
                           u_int32_t *nrates)
{
    struct ieee80211vap *vap = vaphandle;
    struct ieee80211_node *ni;
    struct ieee80211_rateset *rs;
    int i;

    ni = ieee80211_ref_bss_node(vap);

    rs = &ni->ni_rates;
    if (rs->rs_nrates > len){
        ieee80211_free_node(ni);
        return -EINVAL;
    }

    for (i = 0; i < len; i++) {
        rates[i] = IEEE80211_RV(rs->rs_rates[i]);
    }
    *nrates = rs->rs_nrates;
    ieee80211_free_node(ni);

    return 0;
}

/*
 * Query operational data rates
 */
int
wlan_get_bss_ht_rates(wlan_if_t vaphandle,
                           u_int8_t *rates, u_int32_t len,
                           u_int32_t *nrates)
{
    struct ieee80211vap *vap = vaphandle;
    struct ieee80211_node *ni;
    struct ieee80211_rateset *rs;
    int i;

    ni = ieee80211_ref_bss_node(vap);

    rs = &ni->ni_htrates;
    if (rs->rs_nrates > len){
        ieee80211_free_node(ni);
        return -EINVAL;
    }

    for (i = 0; i < len; i++) {
        rates[i] = IEEE80211_RV(rs->rs_rates[i]);
    }
    *nrates = rs->rs_nrates;
    ieee80211_free_node(ni);

    return 0;
}

/*
 * Query multiple stream from data rates
 */
u_int8_t
ieee80211_is_multistream(struct ieee80211_node *ni)
{
    struct ieee80211com *ic = ni->ni_ic;
    u_int8_t             rate;
    u_int8_t             has_multistream;

    rate = ni->ni_htrates.rs_rates[ni->ni_htrates.rs_nrates - 1];
    if  (rate > IEEE80211_RATE_SINGLE_STREAM_MCS_MAX)
        has_multistream = (u_int8_t) (ic->ic_rx_chainmask > 1);
    else
        has_multistream = 0;

    return has_multistream;
}

/*
 * Get number of usable streams.  It is a function of number
 * of spatial streams and configured chain mask.
 */
u_int8_t
ieee80211_getstreams(struct ieee80211com *ic, u_int8_t chainmask)
{
    u_int8_t streams;

    /* REVISIT: add more cases if we have more than 3 chains */
    switch (chainmask) {
    case 1:
    case 2:
    case 4:
        streams = 1;
        break;
    case 3:
    case 5:
    case 6:
        streams = 2;
        break;
    case 7:
        switch (ic->ic_spatialstreams) {
        default:
        case 1:
            streams = 1;
            break;
        case 2:
            streams = 2;
            break;
        case 3:
        case 4:
            /* 4 streams limited to 3 by chainmask */
            streams = 3;
            break;
        }
        break;
    default:
        /* Default to max 1 tx spatial stream */
        streams = 1;
        break;
    }

    return streams;
}

