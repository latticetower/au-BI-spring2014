require 'bio'
require 'mustache'

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

class HtData < Mustache  
  PRECISION = 3
  attr_accessor :nucleotide_positions, :a, :c, :g, :t
  @normalized_positions
  
  def initialize()
    @nucleotide_positions = {
      'a' => Array.new(21) { 0 }, 
      'c' => Array.new(21) { 0 }, 
      'g' => Array.new(21) { 0 }, 
      't' => Array.new(21) { 0 },
      'sum' => Array.new(21) {0},
      'pos' => Array.new(21) {|i| i + 1}
    }
    @a = 0
    @c = 0
    @g = 0
    @t = 0
    
  end
  
  def rounded(data_hash)
    pc = {}
    data_hash.keys.each do |key|
      pc[key] = data_hash[key].map{|value| value.round(PRECISION) }
    end
    pc
  end
  
  def normalized_positions
    @normalized_positions ||= {
      'a' => Array.new(21) { |i| (1.0 * nucleotide_positions['a'][i]/nucleotide_positions['sum'][i]) }, 
      'c' => Array.new(21) { |i| (1.0 * nucleotide_positions['c'][i]/nucleotide_positions['sum'][i]) }, 
      'g' => Array.new(21) { |i| (1.0 * nucleotide_positions['g'][i]/nucleotide_positions['sum'][i]) }, 
      't' => Array.new(21) { |i| (1.0 * nucleotide_positions['t'][i]/nucleotide_positions['sum'][i]) },
      'sum' => Array.new(21) {|i| 1.0 },
      'pos' => Array.new(21) {|i| i + 1}
    }
    
    rounded(@normalized_positions)
  end
  
  def pseudo_counts
    normalized_positions if @normalized_positions.nil?
    k = 1.0
    @pseudo_counts ||= {
      'a' => Array.new(21) { |i| ((pa*k + @normalized_positions['a'][i])/(@normalized_positions['sum'][i] + k)) }, 
      'c' => Array.new(21) { |i| ((pc*k + @normalized_positions['c'][i])/(@normalized_positions['sum'][i] + k)) }, 
      'g' => Array.new(21) { |i| ((pg*k + @normalized_positions['g'][i])/(@normalized_positions['sum'][i] + k)) }, 
      't' => Array.new(21) { |i| ((pt*k + @normalized_positions['t'][i])/(@normalized_positions['sum'][i] + k)) },
      'pos' => Array.new(21) {|i| i + 1}
    }
    rounded(@pseudo_counts)
  end
  
  def pssm
    pseudo_counts unless @pseudo_counts
    @pssm ||= {
      'a' => Array.new(21) { |i| (Math.log(@pseudo_counts['a'][i] / @pa)) }, 
      'c' => Array.new(21) { |i| (Math.log(@pseudo_counts['c'][i] / @pc)) }, 
      'g' => Array.new(21) { |i| (Math.log(@pseudo_counts['g'][i] / @pg)) }, 
      't' => Array.new(21) { |i| (Math.log(@pseudo_counts['t'][i] / @pt)) },
      'sum' => Array.new(21) {|i| (Math.log(@pseudo_counts['a'][i] / @pa) + 
                                  Math.log(@pseudo_counts['c'][i] / @pc) + 
                                  Math.log(@pseudo_counts['g'][i] / @pg) + 
                                  Math.log(@pseudo_counts['t'][i] / @pt))
                              },
      'pos' => Array.new(21) {|i| i + 1}
    }
    rounded(@pssm)
  end
  
  def psum
    @psum ||= @pa + @pc + @pg + @pt
    @psum.round(PRECISION)
  end
  
  def sum1
    @sum ||= a + c + g + t
    @sum
  end
  
  ['a', 'c', 'g', 't'].each do |letter|     
    define_method "p#{letter}".to_sym do
      unless instance_variable_defined? "@p#{letter}"
        instance_variable_set("@p#{letter}", (1.0*send(letter) / sum1) )
      end
      instance_variable_get("@p#{letter}").round(PRECISION)
    end
  end
end

# ---------------------------
# main code
# ---------------------------

data = HtData.new
data.template_file = '../templates/hometask.txt.mustache'
#0.prepare background
data.a = 1338715
data.c = 301268
data.g = 305650
data.t = 1346201
#values generated with script prepare_background.rb
#
read_fasta("../output/data/learning_set.fasta") do  |fasta_string|
  21.times do |i|
    current_char = fasta_string[i].chr
    if data.nucleotide_positions.has_key? current_char
      data.nucleotide_positions[current_char][i] += 1
      data.nucleotide_positions['sum'][i] += 1
    end
  end
end

# serialize to json
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