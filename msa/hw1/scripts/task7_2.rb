require 'bio'

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
#puts @probabilities
#puts @pssm
results_array1 = Array.new
#
read_fasta("../output/data/learning_set.fasta") do  |fasta_string|
  score = wscore(fasta_string)
  i = results_array1.index{|x| x[0] == score} 
  if i
    results_array1[i][1] += 1
  else
    results_array1 << [score, 1.0]
  end
end

results_array2 = Array.new
p = {}
File.open("../input/NC_004317.gbk") do |file|
  entry = file.gets(Bio::GenBank::DELIMITER)
  gb = Bio::GenBank.new(entry)
  puts "Processing #{gb.definition}"
  gb.seq.window_search(21) do |str|
    score = wscore str
    i = results_array2.index{|x| x[0] == score} 
    if i
      results_array2[i][1] += 1
    else
      results_array2 << [score, 1.0]
    end
  end
end


puts 'printing output'

sum1 = results_array1.map{|x| x[1] || 0 }.inject(:+)
puts sum1
results_array1.size.times do |i|
  results_array1[i][1] ||= 0
  results_array1[i][1] = 100.0*results_array1[i][1]/ sum1  
end

#the same for results_array2

sum1 = results_array2.map{|x| x[1] || 0 }.inject(:+)
puts sum1
results_array2.size.times do |i|
  results_array2[i][1] ||= 0
  results_array2[i][1] = 100.0*results_array2[i][1]/ sum1  
end

puts 'printing normalized output'
@outputfile = File.new("../output/data/task7_2.txt", 'w')
@outputfile.puts("{:type=>'plot', :filename => 'cds_scores_random_scores2.svg', :plot_data => {:bar=>true, :color=>'lightgreen', :width => 350, :height => 300, :barw => 8," + 
":name => 'CDS scores in learning set', :x_name => 'score (rounded)', :y_name => 'Percentage of CDSs', :values => [" + 
    results_array1.map{|i| (i.nil? ? "" : "[#{i[0]}, #{i[1] || 0}, 1]" )}.join(', ') + "] }}")
    
@outputfile.puts("{:type=>'plot', :filename => 'cds_scores_random_scores2.svg', :plot_data => {:bar=>true, :color=>'orange', :width => 700, :height => 300, :barw=>6 ," + 
":name => 'scores with given probabilities distribution', :x_name => 'score (rounded)', :y_name => 'Percentage of strings', :values => [" + 
    results_array2.map{|i| (i.nil? ? "" : "[#{i[0]}, #{i[1] || 0}, 1]" )}.join(', ') + "] }}")
#@outputfile.puts("{:coverage_percentage =>  #{ (100.0*1000.0*(results_array1.count{|x| !x.nil? && x[1] > 0 } || 0)/total_length)} }")
#@outputfile.puts("{:coverage_avg => #{(results_array1.map{|x| x[1] }.reduce(&:+)||0)/(results_array1.count || 1)} }")
@outputfile.close



outputfile1 = File.new("../output/data/task7_2_cds.csv", 'w')
results_array1.each do |arr|
  outputfile1.puts arr.join("\t").gsub(".", ",")
end
outputfile1.close()

outputfile2 = File.new("../output/data/task7_2_all.csv", 'w')
results_array2.each do |arr|
  outputfile2.puts arr.join("\t").gsub(".", ",")
end
outputfile2.close()