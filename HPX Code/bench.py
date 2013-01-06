#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os

output = 'hpx_gemres_sparse.log'

os.system('echo \' \' > ' + output)

for thread in range(1,13):
  if thread == 1: os.system('echo \' \' > ' + output)
  cmd = 'numactl --localalloc ./bin/hpx_gmres --hpx:threads '+ str(thread) + ' --Mfilename ../mahindas.mtx --m 1000 --Nblocs ' + str(thread) +' >> ' + output
  print cmd
  os.system(cmd)
  os.system('cat ' + output)
  print ''

# End
sys.exit(0)

