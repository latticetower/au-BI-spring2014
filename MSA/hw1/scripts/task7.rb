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
    w += @pssm[str[i].chr][i]
  end
  w.round
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
  p[key] = (100000*@probabilities[key]).round
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
@outputfile.puts("{:type=>'plot', :filename => 'cds_scores.svg', :plot_data => { :name => 'CDS scores in learning set', :x_name => 'score (rounded)', :y_name => 'Number of CDSs', :values => [" + 
    results_array1.map{|i| (i.nil? ? "" : "[#{i[0]}, #{i[1] || 0}, 1]" )}.join(', ') + "] }}")
    
@outputfile.puts("{:type=>'plot', :filename => 'random_scores.svg', :plot_data => { :name => 'scores with given probabilities distribution', :x_name => 'score (rounded)', :y_name => 'Number of strings', :values => [" + 
    results_array2.map{|i| (i.nil? ? "" : "[#{i[0]}, #{i[1] || 0}, 1]" )}.join(', ') + "] }}")
#@outputfile.puts("{:coverage_percentage =>  #{ (100.0*1000.0*(results_array1.count{|x| !x.nil? && x[1] > 0 } || 0)/total_length)} }")
#@outputfile.puts("{:coverage_avg => #{(results_array1.map{|x| x[1] }.reduce(&:+)||0)/(results_array1.count || 1)} }")
@outputfile.close

# serialize to json
=begin
file1 = File.new('../output/data/task4.txt', 'w')
file1.puts data.nucleotide_positions.to_s, 
           data.normalized_positions.to_s
file1.close

file2 = File.new('../output/data/task6.txt', 'w')
file2.puts data.pa, data.pc, data.pg, data.pt, data.pssm.to_s
file2.close


filename2 = File.new('..\templates\hometask.txt', 'w')
filename2.puts data.render
filename2.close
=end