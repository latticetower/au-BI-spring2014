# HMM description
# ---------------
@@states_by_index = ['A+T', 'G+C']
@@state_chr = ['1', '2']
@@chr_state = {'1' => 0, '2' => 1}
indices_by_state = {'A+T' => 0, 'G+C' => 1}
states = {'A+T' => ['A', 'T'], 'G+C' => ['G', 'C']}
@@pi = [0.996, 0.004]
@@a = [[0.999, 0.001], 
     [0.01,  0.99]]
@@e = [{'A' => 0.291, 'T' => 0.291, 'C' => 0.209, 'G' => 0.209}, 
     {'A' => 0.169, 'T' => 0.169, 'C' => 0.331, 'G' => 0.331}]
# ---------------
#   main code
# ---------------
#viterbi algo implementation
class Viterbi
  def self.forward_init(chr, e, pi)
    @@values = []
     
    @@x = [Array.new(pi.count){|i| i}]
    vnew = Array.new(pi.count){|i| e[i][chr]*pi[i] }
    @@values << vnew
    vnew
  end
  
  def self.forward_step(chr, e, pi, a, v_prev = [])
    return self.forward_init(chr, e, pi) if v_prev.size == 0
    x_new = Array.new(v_prev.count) {|i| (Array.new(v_prev.count){|j| [j, a[j][i]*v_prev[j]]}).max_by{|x| x[1]} }
    @@x << x_new.map{|x| x[0] }
    v_new =  Array.new(v_prev.count){|i| 
      e[i][chr]*x_new[i][1]
    }
    @@values << v_new
    return v_new
  end
  def self.backward_init()
    t = @@values.size - 1
    x = Array.new(@@values[t].size){|i| [i, @@values[t][i] ]}.max_by{|x| x[1]}
    return [x[0], t]
  end
  def self.backward_step(x_next)
    return self.backward_init() if x_next.size == 0
    t_1 = x_next[1] - 1
    return [self.ptr(x_next), t_1]
  end
  def self.ptr(x_next)
    @@x[x_next[1]][x_next[0]]
  end
  
  def self.print_values
    puts @@values
  end
  def self.print_x
    puts @@x
  end
end

def get_state_sequence(str)
  v = []
  str.chars.each do |chr|
    v = Viterbi.forward_step(chr, @@e, @@pi, @@a, v)
    puts v.join ', '
  end
  x_values = []
  x = []
  str2 = ""
  puts 'backward'
  (str.size).times do |i|
    x = Viterbi.backward_step(x)
    puts x.join ','
    str2 = (@@state_chr[x[0]]) + str2
  end
  return str2
end

def update_emissions(input_str, states_str)
  # 1. clear emissions
  @@e.each do |ei|
    ei.each_key do |k|
      ei[k] = 0.0
    end
  end
  # 2. process string
  input_str.size.times do |i|
    @@e[@@chr_state[states_str[i]]][input_str[i]] += 1.0
  end
  @@e.each do |ei|
    s = ei.values.inject(:+)
    ei.each_key do |k|
      ei[k] = (ei[k] / s) if s > 0
    end
  end
end

def update_transitions(states_string)
  # clear transitions
  @@chr_state.values.each do |i|
    @@chr_state.values.each do |j|
      @@a[i][j] = 0.0
    end
  end
  # process states_string
  (states_string.size - 1).times do |i|
    state1 = @@chr_state[states_string[i]]
    state2 = @@chr_state[states_string[i + 1]]
    @@a[state1][state2] += 1.0
  end
  # normalize
  @@a.each do |ai|
    aa = ai.inject(:+)
    ai.size.times do |i|
      ai[i] = (ai[i] / aa) if aa > 0
    end
  end
end

def compute_values(input_str, states_str)
  state1 = ((input_str[0] == 'A' || input_str[0] == 'T') ? 1 : 0)
  state2 = ((input_str[0] == 'G' || input_str[0] == 'C') ? 1 : 0)
  seg1 = (states_str[0] == '1' ? 1 : 0)
  seg2 = (states_str[0] == '2' ? 1 : 0)
  current_seg = states_str[0]
  
  (input_str.size - 1).times do |i|
    if ['A', 'T'].include? input_str[i + 1]
      state1 += 1
    else 
      state2 += 1
    end
    if (states_str[i + 1] != current_seg)
      current_seg = states_str[i + 1]
      if states_str[i + 1] == '1'
        seg1 += 1
      else
        seg2 += 1
      end
    end
  end
  yield [state1, state2, seg1, seg2] if block_given?
  [state1, state2, seg1, seg2]
end


# Viterbi.print_values

def read_fasta(filename)
  File.open(filename) do |file|
    s = ""
    buffer = ""
    desc = ""
    while not file.eof?
      s = file.gets.strip
      if s.index('>')
        yield [desc, buffer] if block_given? and buffer.size > 0
        desc = s
        buffer = ""
      else
        buffer = buffer + s
      end
      
    end
    if buffer.size > 0
      yield [desc, buffer] if block_given?
    end
  end
end

def find_islands(input_str, states_str)
  start_pos = 0
  end_pos = 0
  in_island = false
  states_str.size.times do |i|
    if states_str[i] == '2'
      if not in_island
        start_pos = i
        in_island = true
      end
    else
      if in_island
        end_pos = i - 1
        yield [start_pos, end_pos, input_str[start_pos..end_pos] ] if block_given?
        in_island = false
      end
    end
  end
  if in_island
    end_pos = states_str.size - 1
    yield [start_pos, end_pos, input_str[start_pos..end_pos] ] if block_given?
  end
end
## --------------
## MAIN PART
#------------------


read_fasta(ARGV[0]) do |desc, input_str|
  @@file1 = File.new('viterbi_results.txt', 'w')
  @@file1.puts ARGV[0]
  @@file1.puts desc # first line of .fna file
  states_str = ""
  10.times do |i|
    puts "iteration #{i}"
    @@file1.puts "-----------------------", "Start of Iteration #{i + 1}"
    states_str = get_state_sequence(input_str)
    compute_values(input_str, states_str) do |state1, state2, seg1, seg2|
      @@file1.puts " found #{state1} A+T states, #{state2} G+C states,\n  #{seg1} A+T segments, #{seg2} G+C segments"
    end
    update_transitions(states_str)
    @@file1.puts " new transition probabilities are:", "  a_11=#{@@a[0][0].round(4)}, a_12=#{@@a[0][1].round(4)}, a_21=#{@@a[1][0].round(4)}, a_22=#{@@a[1][1].round(4)}"
    @@file1.puts "end of Iteration #{i + 1}"
  end
  puts "printing islands"
  @@file2 = File.new('viterbi_gc_islands.fasta', 'w')
  island = 0
  find_islands(input_str, states_str) do |start_pos, end_pos, subseq|
  puts "island #{island}"
    island +=1
    @@file1.puts "Island #{island}: from #{start_pos} to #{end_pos}"
    @@file2.puts ">island_#{island}_____#{start_pos}__#{end_pos}", subseq
  end
  #find list of GC-rich segments by position
  @@file2.close
  @@file1.close
end


puts "finished"