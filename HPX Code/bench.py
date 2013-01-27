#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os

output = 'add20.log'
os.system('echo \' \' > ' + output)
for thread in range(1,12):
  if thread == 1: os.system('echo \' \' > ' + output)
  cmd = './bin/hpx_gmres --hpx:threads '+ str(thread) + ' --Mfilename "../../Example Matrices/add20.mtx" --m 30 --max_it 1000 --Nblocs ' + str(thread) +' >> ' + output
  print cmd
  os.system(cmd)
  os.system('cat ' + output)
  print ''
  
 
output = 'orsirr_1.log'
os.system('echo \' \' > ' + output)
for thread in range(1,12):
    if thread == 1: os.system('echo \' \' > ' + output)
    cmd = './bin/hpx_gmres --hpx:threads '+ str(thread) + ' --Mfilename "../../Example Matrices/orsirr_1.mtx" --m 30 --max_it 1000 --Nblocs ' + str(thread) +' >> ' + output
    print cmd
    os.system(cmd)
    os.system('cat ' + output)
    print ''

output = 'orsreg_1.log'
os.system('echo \' \' > ' + output)
for thread in range(1,12):
    if thread == 1: os.system('echo \' \' > ' + output)
    cmd = './bin/hpx_gmres --hpx:threads '+ str(thread) + ' --Mfilename "../../Example Matrices/orsreg_1.mtx" --m 30 --max_it 1000 --Nblocs ' + str(thread) +' >> ' + output
    print cmd
    os.system(cmd)
    os.system('cat ' + output)
    print ''

output = 'pde900.log'
os.system('echo \' \' > ' + output)
for thread in range(1,12):
    if thread == 1: os.system('echo \' \' > ' + output)
    cmd = './bin/hpx_gmres --hpx:threads '+ str(thread) + ' --Mfilename "../../Example Matrices/pde900.mtx" --m 30 --max_it 1000 --Nblocs ' + str(thread) +' >> ' + output
    print cmd
    os.system(cmd)
    os.system('cat ' + output)
    print ''

output = 'sherman1.log'
os.system('echo \' \' > ' + output)
for thread in range(1,12):
    if thread == 1: os.system('echo \' \' > ' + output)
    cmd = './bin/hpx_gmres --hpx:threads '+ str(thread) + ' --Mfilename "../../Example Matrices/sherman1.mtx" --m 30 --max_it 1000 --Nblocs ' + str(thread) +' >> ' + output
    print cmd
    os.system(cmd)
    os.system('cat ' + output)
    print ''

# End
sys.exit(0)

