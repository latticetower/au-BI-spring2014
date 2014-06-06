require 'bio'

selected_score = ARGV[0].to_f
start_pos = 1645936 #one-based
@probabilities = {}
@pssm = {}

def read_fasta(filename)
  File.open(filename) do |file|
    s = ""
    buffer = ""
    while not file.eof?
      s = file.gets.strip
      if s.index('>')
        yield buffer if block_given? and buffer.size > 0
        buffer = ""
      else
        buffer = buffer + s
      end
      
    end
    if buffer.size > 0
      yield buffer if block_given?
    end
  end
end


def wscore str
  w = 0
  21.times do |i|
    #puts str[i].chr unless @pssm.has_key? str[i].chr
    w+= 'acgt'.chars.map{ |c| @pssm[c][i] }.inject(:+)  if (str[i].chr == 'n')
    
    w += @pssm[str[i].chr][i] if @pssm.has_key? str[i].chr
  end
  
  w.round(1)
end

# ---------------------------
# main code
# ---------------------------


# 1. load previously computed data from file
File.open("../output/data/task6.txt") do |file|
  @probabilities['a'] = file.gets.to_f
  @probabilities['c'] = file.gets.to_f
  @probabilities['g'] = file.gets.to_f
  @probabilities['t'] = file.gets.to_f
  @pssm = eval(file.gets)
end
#-------------------

fn = 0
tp = 0
cds_hash = {}
#
read_fasta("../output/data/task3.fasta") do  |fasta_string|
  score = wscore(fasta_string)
  if score > selected_score
    tp += 1
  else
    fn +=1
  end
  if cds_hash.has_key? fasta_string
    cds_hash[fasta_string][1] += 1 
  else
    cds_hash[fasta_string] = [score, 1]
  end
end

trash_hash = {}

fp = 0
tn = 0

outputfile = File.new("../output/data/task9.txt", 'w')
File.open("../input/NC_004317.gbk") do |file|
  entry = file.gets(Bio::GenBank::DELIMITER)
  gb = Bio::GenBank.new(entry)
  puts "Processing #{gb.definition}"
  gb.seq.subseq(start_pos, gb.seq.length).window_search(21) do |str|
    score = wscore(str)
    if score > selected_score
      fp +=1
    else
      tn += 1
    end
    unless cds_hash.has_key? str
      
      if trash_hash.has_key? str
        trash_hash[str][1] += 1
      else
        trash_hash[str] = [score, 1]
      end
    end
  end
end
puts "processed"

outputfile = File.new("../output/data/task9.txt", 'w')
# printing result_array1
outputfile.puts("FP=#{fp}, FN=#{fn}, TP=#{tp}, TN=#{tn}")
outputfile.puts("Sensitivity=#{1.0*tp/(tp+fn)}")
outputfile.puts("Specificity=#{1.0*tn/(tn+fp)}")
outputfile.puts("PPV=#{1.0*tp/(tp+fp)}")
outputfile.close
