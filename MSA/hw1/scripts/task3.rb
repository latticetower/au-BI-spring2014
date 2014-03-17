require 'bio'

COMPL = {'a' => 't', 't' => 'a', 'g' => 'c', 'c' => 'g', 'n' => 'n',}

outputfile = File.new("../output/data/task3.fasta", 'w')
learning_set_file = File.new("../output/data/learning_set.fasta", 'w')
test_set_file = File.new("../output/data/test_set.fasta", 'w')

File.open("../input/NC_004317.gbk") do |file|
  entry = file.gets(Bio::GenBank::DELIMITER)
  gb = Bio::GenBank.new(entry)
  seq1 = gb.seq
  
  gb.each_cds() do |cds|
    # puts "#{loc.strand}, #{loc.from}"
    loc = cds.locations.first
    position = cds.locations.absolute(1) - 1
    result_sequence = [seq1[position]] # let's collect all letters to array - to avoid 'n' letters and keep 21

    # process preceding nucleotides
    count = 0
    while count < 10 and position > 0 do
      position = loc.strand == 1 ? position - 1 : position + 1
      if seq1[position].downcase.chr != 'n'
        result_sequence.unshift(seq1[position]) 
        count = count + 1
      else
        puts seq1[position].downcase
      end
    end
    
    # process following nucleotides
    count = 0
    position = 1
    while count < 10 do
      position = position + 1
      if seq1[cds.locations.absolute(position) - 1].downcase.chr != 'n' 
        result_sequence.push(seq1[cds.locations.absolute(position) - 1]) 
        count = count + 1
      else
        puts seq1[position].downcase
      end
    end
   
    outputfile.puts ">cds from position #{cds.locations.absolute(1)}#{', at 2nd strand' if cds.locations.first.strand == -1}"
    outputfile.puts  result_sequence.map{|l| (loc.strand == 1 ? l : COMPL[l])}.join('')
    
    if cds.locations.absolute(1) <= 1645935
      learning_set_file.puts ">cds from position #{cds.locations.absolute(1)}#{', at 2nd strand' if cds.locations.first.strand == -1}",
          result_sequence.map{|l| (loc.strand == 1 ? l : COMPL[l])}.join('')
    else
      test_set_file.puts ">cds from position #{cds.locations.absolute(1)}#{', at 2nd strand' if cds.locations.first.strand == -1}",
          result_sequence.map{|l| (loc.strand == 1 ? l : COMPL[l])}.join('')
    end
    
  end
end

outputfile.close
learning_set_file.close
test_set_file.close