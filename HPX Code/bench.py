#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os

output = 'hpx_gemres_sparse.log'

os.system('echo \' \' > ' + output)

for thread in range(1,7):
  if thread == 1: os.system('echo \' \' > ' + output)
  cmd = './bin/hpx_gmres --hpx:threads '+ str(thread) + ' --Mfilename "../../Example Matrices/add20.mtx" --m 1000 --max_it 30 --Nblocs ' + str(thread) +' >> ' + output
  print cmd
  os.system(cmd)
  os.system('cat ' + output)
  print ''

# End
sys.exit(0)

