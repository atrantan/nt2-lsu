#!/usr/bin/python

import os
import re

CSI="\x1B["
reset=CSI+"m"
bold='\033[1m'

#parses a file fi
def parse_file(f, value_name):
	content = f.read()
	items=re.findall("{0}.*$".format(value_name), content, re.MULTILINE)
	#print content
	value_s = '0'
	for item in items:
		value_s = item.partition(':')[2]
		value_s = value_s.partition('ms')[0]
	return float(value_s)

def valid_line(f, col):
	content = f.read()
	elems = content.split(' ')
	if(elems[col].split('\n')[0] == '0'):
		return False;
	return True;

#function to plot scalability graphs
def plot_scalability(apps, title,  xlabel, xlabel_range, llabel, llabel_range, dump_file_format):
	for app in apps:
		for l in llabel_range:
			datafile_n = ""
			if(len(apps) == 1):
				datafile_n = "perfs/{0}/{0}_{1}={3}_scale_over_{2}.dat".format(apps[0], llabel, xlabel,l)
			else:
				datafile_n = "perfs/{0}_{1}={3}_scale_over_{2}_{3}.dat".format(apps[-1], llabel, xlabel,l)
			datafile = open(datafile_n, 'w')
			for x in xlabel_range:
				datafile.write("{0}".format(x))
				value_v = [];
				total_time = 0;
				try:
					dumpfile_n = dump_file_format.replace("#0", x)
					dumpfile_n = dumpfile_n.replace("#1", l)
					dumpfile_n = "perfs/{0}/{0}_{1}.dump".format(app, dumpfile_n)
					#get value
					dumpfile = open(dumpfile_n)
					total_time = parse_file(dumpfile, "total time")
				except IOError:
					total_time = 0
				datafile.write(" {0}".format(total_time))
				datafile.write("\n")
	datafile.close();

# ncores = ['12']
ncores = ['1', '4', '8', '12', '16', '20','24','28','32','36','40','44','48']
#ncores = ['12']
psize = ['1000','2000','4000']
# psize = ['400', '1200', '2000']
bsize = ['200']
# bsize = ['50', '100', '200']
iterations = 10

#create lu plasma dump directory
dumpdir = 'perfs/lu_plasma';
if not os.path.exists(dumpdir):
	print "creating "+dumpdir+" directory"
	os.makedirs(dumpdir)

# plasma version, has fixed block size
for n in ncores:
	for s in psize:
		for b in bsize:
			dumpfile = dumpdir+"/lu_plasma_problem_size={0}_block_size={1}_ncores={2}.dump".format(s, b, n)
			print "running ./src/lu_plasma -n{0} -b{1} -a{2} > {3}".format(s, b, n, dumpfile)
			os.system("srun -p lyra -N 1 numactl --interleave=all ./src/lu_plasma -n{0} -b{1} -a{2} > {3}".format(s, b, n, dumpfile))

#create lu dataflow dump directory
dumpdir = 'perfs/lu_dataflow';
if not os.path.exists(dumpdir):
	print "creating "+dumpdir+" directory"
	os.makedirs(dumpdir)

#dataflow version
for n in ncores:
	for s in psize:
		for b in bsize:
			dumpfile = dumpdir+"/lu_dataflow_problem_size={0}_block_size={1}_ncores={2}.dump".format(s, b, n)
			print "running ./bin/lu_dataflow -n{0} -b{1} on {2} cores > {3}".format(s, b, n, dumpfile)
			os.system("srun -p lyra -N 1 -c {2} -n 1 numactl --interleave=all ./bin/lu_dataflow --s {0} --b {1} > {3}".format(s, b, n, dumpfile))

#create lu hpx dump directory
dumpdir = 'perfs/lu_hpx'
if not os.path.exists(dumpdir):
 	print "creating "+dumpdir+" directory"
 	os.makedirs(dumpdir)

#hpx version
for n in ncores:
	for s in psize:
 		for b in bsize:
			dumpfile = dumpdir+"/lu_hpx_problem_size={0}_block_size={1}_ncores={2}.dump".format(s, b, n)
			print "running ./bin/lu_hpx -n{0} -b{1} on {2} cores > {3}".format(s, b, n, dumpfile)
			os.system("srun -p lyra -N 1 -c {2} -n 1 numactl --interleave=all ./bin/lu_hpx --s {0} --b {1} > {3}".format(s, b, n, dumpfile))

# plot_scalability(["lu_hpx"], "Scalability of lu hpx version, for different block sizes over problem size",
# 								"problem_size", psize, "block_size", bsize,
# 								"problem_size=#0_block_size=#1_ncores=6")

#plot_scalability(["lu_dataflow"], "Scalability of lu dataflow version, for different block sizes over problem size",
#								"problem_size", psize, "block_size", bsize,
#								"problem_size=#0_block_size=#1_ncores=6")

#plot_scalability(["lu_plasma"], "Scalability of lu plasma version, for different block sizes over problem size",
# 								"problem_size", psize, "block_size", bsize,
# 								"problem_size=#0_block_size=#1_ncores=6")

plot_scalability(["lu_hpx"], "Scalability of lu hpx version",
 								"ncores", ncores, "problem_size", psize,
 								"problem_size=#1_block_size=200_ncores=#0")

plot_scalability(["lu_dataflow"], "Scalability of lu dataflow version",
								"ncores", ncores, "problem_size", psize,
								"problem_size=#1_block_size=200_ncores=#0")

plot_scalability(["lu_plasma"], "Scalability of lu plasma version",
 								"ncores", ncores, "problem_size", psize,
 								"problem_size=#1_block_size=200_ncores=#0")

