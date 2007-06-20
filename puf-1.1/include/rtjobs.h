/*-------------------------------------------------------------------------
  rtjobs.h - Real Time jobs

             (c) 2006 Pierre Gaufillet <pierre.gaufillet@magic.fr> 

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
-------------------------------------------------------------------------*/

/* $Id: rtjobs.h,v 1.1 2006/11/18 19:52:06 gaufille Exp $ */

#ifndef RTJOBS_H_
#define RTJOBS_H_

void rt_base_job(void);
void rt_basex2_job(void);
void rt_basex4_job(void);
void rt_basex8_job(void);
void rt_basex16_job(void);
void rt_basex32_job(void);
void rt_basex64_job(void);
void rt_basex128_job(void);
void rt_basex256_job(void);
void rt_basex512_job(void);
void rt_basex1024_job(void);
void rt_basex2048_job(void);
void rt_basex4096_job(void);
void rt_basex8192_job(void);
void rt_basex16384_job(void);
void rt_basex32768_job(void);
void rt_basex65536_job(void);

#endif // RTJOBS_H_
