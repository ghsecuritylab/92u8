/*
 * Copyright (c) 2010, Atheros Communications Inc.
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

/**
 * @ingroup adf_os_public
 * @file adf_os_pseudo.h
 * This file abstracts "pseudo module" semantics.
 */
#ifndef __ADF_OS_PSEUDO_H
#define __ADF_OS_PSEUDO_H

#include <adf_os_pseudo_pvt.h>

/**
 * @brief Specify the module's entry point.
 */ 
#define adf_os_pseudo_module_init(_fn)     __adf_os_pseudo_module_init(_fn)

/**
 * @brief Specify the module's exit point.
 */ 
#define adf_os_pseudo_module_exit(_fn)     __adf_os_pseudo_module_exit(_fn)

/**
 * @brief Setup the following driver information: name, pseudo IDs of devices
 * supported and some device handlers.
 */ 
#define adf_os_pseudo_set_drv_info(_name, _ifname, _pseudo_ids, _attach, _detach,  \
        _suspend, _resume) \
    __adf_os_pseudo_set_drv_info(_name, _ifname, _pseudo_ids, \
                                _attach, _detach, \
                                _suspend, _resume)
#endif
