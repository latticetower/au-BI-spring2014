from Bio import SeqIO
from Bio.SeqUtils import GC
from Bio.Seq import transcribe, translate
import os
import sys


def get_file_format(filename):
    if os.path.splitext(filename)[1][1: ] in [
            "fasta", "fa", "seq", "fsa", "fas",
            "fna", "fnn", "faa", "frn"
        ]:
        return "fasta"
    if os.path.splitext(filename)[1][1: ] in ["gb", "gbk"]:
        return "genbank"
    raise ValueError("Invalid file extension")


def load_from_file(filename):
    return [x for x in SeqIO.parse(open(filename, "rU"), get_file_format(filename))]


def get_info(seq):
    return GC(seq), transcribe(seq), translate(seq)


def print_information(file_id, filename, sequences_list):
    print "...processing file N {0} ({1})...".format(file_id, filename)
    print "sequences found: {0}".format(len(sequences_list))
    from Bio.Seq import Seq
    from Bio.SeqRecord import SeqRecord
    from Bio.Alphabet import IUPAC
    processed_data = []
    for i in range(len(sequences_list)):
        print "processing sequence {0}:".format(i)
        print sequences_list[i]
        sequence_info = get_info(sequences_list[i].seq)
        print " GC: {0}\n transcribed:\n {1} \n translated:\n {2}".format(*sequence_info)
        processed_data.append(SeqRecord(
            sequence_info[2],
            id="file_{0}.seq_{1}".format(file_id, i),
            name=sequences_list[i].name,
            description=sequences_list[i].description
            ))
    return processed_data


def align(data):
    from Bio.Align.Applications import MuscleCommandline
    muscle_cline = MuscleCommandline(clw = True)
    import subprocess
    import sys
    child = subprocess.Popen(
        str(muscle_cline) + " -matrix BLOSUM62",
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        shell=(sys.platform != "win32")
        )

    SeqIO.write(data, child.stdin, "fasta")
    child.stdin.close()

    from Bio import AlignIO
    return AlignIO.read(child.stdout, "clustal")


def print_to_phylip(alignment, filename):
    from Bio import AlignIO
    AlignIO.write(alignment, filename, "phylip-relaxed")


def main(filenames):
    sequences = {}
    for filename in filenames:
        try:
            sequences[filename] = load_from_file(filename)
        except ValueError:
            print >> sys.stderr, "file {0} has invalid extension and will be ignored".format(filename)
        except IOError:
            print >> sys.stderr, "file {0} doesn't exist".format(filename)
    processed_data = []
    index = 0
    for k, v in sequences.items():
        processed_data += print_information(index, k, v)
        index += 1
    try:
        alignment = align(processed_data)
    except ValueError:
        print >> sys.stderr, "...zero or one sequences found - nothing to align"
        return
    print "..got aligned data:\n\t{0}".format(alignment)
    print_to_phylip(alignment, "resulting_alignment.phy")
    print "all done (alignment can be found at file named 'resulting_alignment.phy')"


if __name__ == "__main__":
    filenames = sys.argv[1 : ]
    information_message = "Usage: provide one of more fasta or genbank files " + \
        "with nucleotide sequences"
    if len(filenames) < 1:
        print >> sys.stderr, information_message
        exit(1)
    if filenames[0] in ["help", "--help", "-h", "/?"]:
        print >> sys.stderr, information_message
        exit(1)
    main(filenames)
