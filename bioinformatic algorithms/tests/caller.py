from itertools import combinations
import os
import sys
import subprocess

def call1(exec_name, test_file_list):
	with open(test_file_list) as f:
		for line in f:
			print line
			subprocess.call("{0} {1}".format(exec_name, line.strip()))
			

call1(sys.argv[1], sys.argv[2])

 

