/*-------------------------------------------------------------------------
  sequencer.c - Real Time sequencer

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

#include "common_types.h"
#include "rtjobs.h"


/******************************************************************/

void rt_sequencer(uint counter) 
{
    rt_base_job();
    
    if(counter & 0x0001)
    {
        rt_basex2_job();
        return;
    }
    if(counter & 0x0002)
    {
        rt_basex4_job();
        return;
    }
    if(counter & 0x0004)
    {
        rt_basex8_job();
        return;
    }
    if(counter & 0x0008)
    {
        rt_basex16_job();
        return;
    }
    if(counter & 0x0010)
    {
        rt_basex32_job();
        return;
    }
    if(counter & 0x0020)
    {
        rt_basex64_job();
        return;
    }
    if(counter & 0x0040)
    {
        rt_basex128_job();
        return;
    }
    if(counter & 0x0080)
    {
        rt_basex256_job();
        return;
    }
    if(counter & 0x0100)
    {
        rt_basex512_job();
        return;
    }
    if(counter & 0x0200)
    {
        rt_basex1024_job();
        return;
    }
    if(counter & 0x0400)
    {
        rt_basex2048_job();
        return;
    }
    if(counter & 0x0800)
    {
        rt_basex4096_job();
        return;
    }
    if(counter & 0x1000)
    {
        rt_basex8192_job();
        return;
    }
    if(counter & 0x2000)
    {
        rt_basex16384_job();
        return;
    }
    if(counter & 0x4000)
    {
        rt_basex32768_job();
        return;
    }
    if(counter & 0x8000)
    {
        rt_basex65536_job();
        return;
    }
}
