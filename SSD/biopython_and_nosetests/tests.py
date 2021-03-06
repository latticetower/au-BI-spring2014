from nose.tools import with_setup, raises, assert_true, assert_equals

from dna_processing_script import *


@raises(ValueError)
def get_file_format_test():
    assert get_file_format("gene1.fas"), "fasta"
    assert get_file_format("gene2.gbk"), "genebank"
    get_file_format("file.ab1")

@raises(IOError)
def test_file_existance():
    assert_true(load_from_file("../../hometask-files/2-bio_toolkits/gene1.fasta"))
    load_from_file("gene1.fas")

def get_info_test():
    assert_equals(get_info("AAAAAA"), (0.0, "AAAAAA", "KK"))
    assert_equals(get_info("GCC"), (100.0, "GCC", "A"))
    assert_equals(get_info("GCT"), (200.0/3, "GCU", "A"))

@raises(ValueError)
def load_from_file_test():
    filename = "../../hometask-files/2-bio_toolkits/gene1.fasta"
    sequences = load_from_file(filename)
    result_sequences = print_information(0, filename, sequences)
    assert_equals(len(result_sequences), 1)
    align(result_sequences)

def load_from_file_2_test():
    '''test should not raise any errors'''
    filenames = ["../../hometask-files/2-bio_toolkits/gene1.fasta", "../../hometask-files/2-bio_toolkits/gene2.fasta"]
    results = []
    for filename in filenames:
        results += print_information(0, filename, load_from_file(filename))
    assert align(results)
