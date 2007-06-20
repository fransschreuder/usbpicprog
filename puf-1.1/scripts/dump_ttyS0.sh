#!/bin/bash
# $Id: dump_ttyS0.sh,v 1.1 2006/03/21 21:58:58 gaufille Exp $

stty 115200 -cstopb cs8 < /dev/ttyS0
cat /dev/ttyS0

