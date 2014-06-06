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
=begin

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
    results_array1 << [score, 1]
  end
end

results_array2 = Array.new
p = {}
@probabilities.keys.each do |key|
  p[key] = (100500*@probabilities[key]).round
end
test_seq = Bio::Sequence::NA.randomize(p)

test_seq.window_search(21) do |str|
  score = wscore str
  i = results_array2.index{|x| x[0] == score} 
  if i
    results_array2[i][1] += 1
  else
    results_array2 << [score, 1]
  end
end

puts 'printing output'
@outputfile = File.new("../output/data/task7.txt", 'w')
@outputfile.puts("{:type=>'plot', :filename => 'cds_scores.svg', :plot_data => {:bar=>true, :width => 350, :barw => 5, :height => 300, :name => 'CDS scores in learning set', :x_name => 'score (rounded)', :y_name => 'Number of CDSs', :values => [" + 
    results_array1.map{|i| (i.nil? ? "" : "[#{i[0]}, #{i[1] || 0}, 1]" )}.join(', ') + "] }}")
    
@outputfile.puts("{:type=>'plot', :filename => 'random_scores.svg', :plot_data => {:bar=>true, :width => 500, :barw=>5, :height => 300, :name => 'scores with given probabilities distribution', :x_name => 'score (rounded)', :y_name => 'Number of strings', :values => [" + 
    results_array2.map{|i| (i.nil? ? "" : "[#{i[0]}, #{i[1] || 0}, 1]" )}.join(', ') + "] }}")
#@outputfile.puts("{:coverage_percentage =>  #{ (100.0*1000.0*(results_array1.count{|x| !x.nil? && x[1] > 0 } || 0)/total_length)} }")
#@outputfile.puts("{:coverage_avg => #{(results_array1.map{|x| x[1] }.reduce(&:+)||0)/(results_array1.count || 1)} }")
@outputfile.close

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

=end
#-------------------

cds_hash = {}
#
read_fasta("../output/data/task3.fasta") do  |fasta_string|
  score = wscore(fasta_string)
  if cds_hash.has_key? fasta_string
    cds_hash[fasta_string][1] += 1 
  else
    cds_hash[fasta_string] = [score, 1]
  end
end

trash_hash = {}

outputfile = File.new("../output/data/task8.txt", 'w')
File.open("../input/NC_004317.gbk") do |file|
  entry = file.gets(Bio::GenBank::DELIMITER)
  gb = Bio::GenBank.new(entry)
  puts "Processing #{gb.definition}"
  gb.seq.window_search(21) do |str|
    unless cds_hash.has_key? str
      score = wscore(str)
      if trash_hash.has_key? str
        trash_hash[str][1] += 1
      else
        trash_hash[str] = [score, 1]
      end
    end
  end
end
puts "processed"
a1 = cds_hash.values.sort_by{|x| x[0]}.group_by{|x| x[0]}
res_array1 = []
a1.each_pair do |k, v|
  res_array1 << [k, v.map{|x| x[1]}.inject(:+) || 0]
end

(res_array1.size - 1).times do |i|
  res_array1[i + 1][1] = res_array1[i + 1][1] + res_array1[i][1]
end

#processing rest
a2 = trash_hash.values.sort{|x, y| y[0] <=> x[0]}.group_by{|x| x[0]}
res_array2 = []
a2.each_pair do |k, v|
  res_array2 << [k, v.map{|x| x[1]}.inject(:+) || 0]
end
res_array2
(res_array2.size - 1).times do |i|
  res_array2[i + 1][1] = res_array2[i + 1][1] + res_array2[i][1]
end

outputfile = File.new("../output/data/task8.txt", 'w')
#printing result_array1
outputfile.puts("{:type=>'plot', :filename => 'fp_fn_plot1.svg', :plot_data => {:show_points=>false, :width => 350, :height => 300," + 
":name => 'False negatives', :x_name => 'score (rounded to .1)', :y_name => 'Count of CDS strings with FN score', :values => [" + 
    res_array1.map{|i| (i.nil? ? "" : "[#{i[0]}, #{i[1] || 0}, 1]" )}.join(', ') + "] }}")

#printing resuls_array2    
outputfile.puts("{:type=>'plot', :filename => 'fp_fn_plot1.svg', :plot_data => {:show_points=>false, :width => 700, :height => 300 ," + 
":name => 'False positives', :x_name => 'score (rounded to .1)', :y_name => 'Count of non-CDS strings with FP score', :values => [" + 
    res_array2.reverse.map{|i| (i.nil? ? "" : "[#{i[0]}, #{i[1] || 0}, 1]" )}.join(', ') + "] }}")

outputfile.close

##normaalization

sum1 = res_array1.map{|x| x[1].to_f || 0 }.inject(:+)
puts sum1
res_array1.size.times do |i|
  res_array1[i][1] ||= 0
  res_array1[i][1] = 100.0*res_array1[i][1]/ sum1  
end

#the same for results_array2

sum1 = res_array2.map{|x| x[1].to_f || 0 }.inject(:+)
puts sum1
res_array2.size.times do |i|
  res_array2[i][1] ||= 0
  res_array2[i][1] = 100.0*res_array2[i][1]/ sum1  
end

outputfile = File.new("../output/data/task8_normalized.txt", 'w')
#printing result_array1
outputfile.puts("{:type=>'plot', :filename => 'fp_fn_plot2.svg', :plot_data => {:show_points=>true, :width => 350, :height => 300," + 
":name => 'False negatives', :x_name => 'score (rounded to .1)', :y_name => 'Count of CDS strings with FN score', :values => [" + 
    res_array1.map{|i| (i.nil? ? "" : "[#{i[0]}, #{i[1] || 0}, 1]" )}.join(', ') + "] }}")

#printing resuls_array2    
outputfile.puts("{:type=>'plot', :filename => 'fp_fn_plot2.svg', :plot_data => {:show_points=>true, :width => 700, :height => 300 ," + 
":name => 'False positives', :x_name => 'score (rounded to .1)', :y_name => 'Count of non-CDS strings with FP score', :values => [" + 
    res_array2.reverse.map{|i| (i.nil? ? "" : "[#{i[0]}, #{i[1] || 0}, 1]" )}.join(', ') + "] }}")

outputfile.close