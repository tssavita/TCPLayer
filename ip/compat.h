/*
 * libip_udp -- Computer Networks Practicum IP Layer Emulation
 *
 * Copyright (C) 2004 Marten Klencke & Erik Bosman
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __COMPAT_H
#define __COMPAT_H

#include <sys/types.h>
#include <sys/time.h>

typedef u_int8_t u8_t;
typedef u_int16_t u16_t;
typedef u_int32_t u32_t;

typedef void (*sighandler_t)(int);
sighandler_t fake_signal(int signum, sighandler_t handler);

#ifndef __DONT_WRAP_FUNCTIONS
#define signal fake_signal
#endif

#endif

