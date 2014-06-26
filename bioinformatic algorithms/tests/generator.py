from itertools import combinations
import os
import sys

def print_to_fasta(list_of_files, output_folder_name, id, len, seq, fname2):
    output_file_name = "test_{0}_{1}.fasta".format(id, len)

    if output_folder_name:
      dir_path = os.path.join(output_folder_name)
    else:
      dir_path = os.getcwd()
    if not os.path.isdir(dir_path):
      os.makedirs(dir_path)
    g = open(os.path.join(dir_path, output_file_name), 'w')
    g.write(">seq_{0}\n{1}\n".format(id, seq))
    g.close()
    list_of_files.write(os.path.join(dir_path, output_file_name))
    list_of_files.write(" ")
    list_of_files.write(os.path.join(fname2, "test_{0}_{1}.out.txt".format(id, len)))
    list_of_files.write("\n")

def generate(folder, outfolder_name, max_len):
	alphabet = ['a', 'c', 'g', 't', 'A', 'C', 'G', 'T']
	counter = 0
	list_of_files = open("test_files_{0}_{1}.txt".format(folder, outfolder_name), 'w')
	for len in range(0, max_len):
		for subset in combinations(alphabet, len):
			counter += 1
			print_to_fasta(list_of_files, folder, counter, len, "".join(subset), outfolder_name)
	list_of_files.close()
			

generate(sys.argv[1], sys.argv[2], int(sys.argv[3]))

 

