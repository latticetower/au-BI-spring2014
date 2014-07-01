#include <iostream>
#include <fstream>
#include "fasta_data.h"
#include "bwt_tools.h"

int main(int argc, char** argv) {
    size_t start_time = time(NULL);
    size_t end_time = 0;
    if (argc < 4) {
        std::cout << "Please, provide input file name as a first parameter, and output file name as a second parameter" << std::endl;
        return 0;
    }
    std::ifstream inputStream(argv[1]);
    if (inputStream.fail()) {
        std::cout << "Please, provide _valid_ file name as first parameter" << std::endl;
        return 0;
    }
    FastaData text_fasta;
    text_fasta.loadFromStream(inputStream);
    inputStream.close();

    std::ifstream inputStream2(argv[2]);
    if (inputStream2.fail()) {
        std::cout << "Please, provide _valid_ file name as second parameter" << std::endl;
        return 0;
    }
    FastaData strings_fasta;
    strings_fasta.loadFromStream(inputStream2);
    inputStream2.close();
    //now we have 2 fasta objects with given text and set of given substrings

    BWTBuilder builder;
    builder.Build(text_fasta.data[0].second + "$");
    std::ofstream output_file(argv[3]);
    for (std::vector<std::pair<std::string, std::string> >::iterator iter = strings_fasta.data.begin(); iter != strings_fasta.data.end(); ++iter) {
        output_file << ">" << iter->first << std::endl;
        std::set<size_t> positions = builder.find(iter->second);
        if (positions.size() == 0) {
            output_file << "no occurences were found";
        }
        for (std::set<size_t>::iterator iter2 = positions.begin(); iter2 != positions.end(); ++iter2) {
            output_file << *iter2 << '\t';
        }
        output_file << std::endl;
    }
    end_time = time(NULL);
    std::cout << "total time: " << (end_time - start_time) << " seconds" << std::endl;
    output_file.close();

    return 0;
}
