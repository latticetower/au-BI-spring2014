MAX_EDIT_DISTANCE = 100
MAX_ARRAY_SIZE = 101
GAP_COST = 1
MISMATCH_COST = 1

input_filename = "input.txt"

scoring_filename = "scoring.txt"

class Scorings
  attr_reader :gap_cost
  attr_reader :alignment_cost
  
  def initialize(sourcefile)
    File.open(sourcefile) do |f|
      @gap_cost = f.gets.to_f
      @alignment_cost = Hash[["A", "C", "G", "T"].zip(f.gets.split, 
                              f.gets.split, 
                              f.gets.split, 
                              f.gets.split).map{|x| [x[0], Hash[ ["A", "C", "G", "T"].zip([x[1].to_f, x[2].to_f, x[3].to_f, x[4].to_f ])] ]}
                              ]
                              #{|y| y[0] => y[1]}
                         
   
      
    end
  end
  
  def [](a, b)
    return 0 if a == '-' && b == '-'
    return @gap_cost if a == '-' || b == '-'
    @alignment_cost[a.upcase][b.upcase]
  end
  
  def cost(a, b, c, d)
    return 0 if a == '-' && b == '-' && c == '-' && d == '-'
    res = [a, b, c, d].count{|x| x == '-'} * gap_cost
    res += [a, b, c, d].map{|x| x.upcase }.select{|x| x != '-'}.combination(2).map{|x| x.size > 0 ? @alignment_cost[x[0]][x[1]] : 0}.inject(:+) || 0
  end
  
  def print_alignment
    puts "#{@alignment_cost.to_s}"
  end
end

scorings = Scorings.new(scoring_filename)
scorings.print_alignment

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

@lines_array = Array.new
read_fasta(input_filename) do |str|
  @lines_array << str
end

@data_array = Array.new(@lines_array[0].size + 1) {
  Array.new(@lines_array[1].size + 1) {
    Array.new(@lines_array[2].size + 1) {
      Array.new(@lines_array[3].size + 1) { 0 }
    }
  }
}

# 1. initialize bound values
#-----------------

@lines_array[0].size.times do |i|
  @data_array[i + 1][0][0][0] = @data_array[i][0][0][0] + scorings.gap_cost
end

@lines_array[1].size.times do |i|
  @data_array[0][i + 1][0][0] = @data_array[0][i][0][0] + scorings.gap_cost
end

@lines_array[0].size.times do |i|
  @data_array[0][0][i + 1][0] = @data_array[0][0][i][0] + scorings.gap_cost
end

@lines_array[0].size.times do |i|
  @data_array[0][0][0][i + 1] = @data_array[0][0][0][i] + scorings.gap_cost
end

# 2. compute pair alignments
# ---------------------------
# with line 0
@lines_array[0].size.times do |i|
  @lines_array[1].size.times do |j|
    @data_array[i + 1][j + 1][0][0] = [
        @data_array[i][j + 1][0][0] + scorings.gap_cost,
        @data_array[i + 1][j][0][0] + scorings.gap_cost, 
        @data_array[i][j + 1][0][0] + scorings[@lines_array[0][i], @lines_array[1][j]]].min
  end
  
  @lines_array[2].size.times do |k|
    @data_array[i + 1][0][k + 1][0] = [
        @data_array[i][0][k + 1][0] + scorings.gap_cost,
        @data_array[i + 1][0][k][0] + scorings.gap_cost, 
        @data_array[i][0][k + 1][0] + scorings[@lines_array[0][i], @lines_array[2][k]]].min
  end
  
  @lines_array[3].size.times do |s|
    @data_array[i + 1][0][0][s + 1] = [
        @data_array[i][0][0][s + 1] + scorings.gap_cost,
        @data_array[i + 1][0][0][s] + scorings.gap_cost, 
        @data_array[i][0][0][s + 1] + scorings[@lines_array[0][i], @lines_array[3][s]]].min
  end
end

#1&2,3
@lines_array[1].size.times do |j|
  @lines_array[2].size.times do |k|
    @data_array[0][j + 1][k + 1][0] = [
        @data_array[0][j][k + 1][0] + scorings.gap_cost,
        @data_array[0][j + 1][k][0] + scorings.gap_cost, 
        @data_array[0][j][k + 1][0] + scorings[@lines_array[1][j], @lines_array[2][k]]].min
  end
  
  @lines_array[3].size.times do |s|
    @data_array[0][j + 1][0][s + 1] = [
        @data_array[0][j][0][s + 1] + scorings.gap_cost,
        @data_array[0][j][0][s] + scorings.gap_cost, 
        @data_array[0][j][0][s + 1] + scorings[@lines_array[1][j], @lines_array[3][s]]].min
  end
end

#2,3
@lines_array[2].size.times do |k|
  @lines_array[3].size.times do |s|
    @data_array[0][0][k + 1][s + 1] = [
        @data_array[0][0][k][s + 1] + scorings.gap_cost,
        @data_array[0][0][k][s] + scorings.gap_cost, 
        @data_array[0][0][k][s + 1] + scorings[@lines_array[2][k], @lines_array[3][s]]].min
  end
end

# 0 & 1 & 3
@lines_array[0].size.times do |i|
  @lines_array[1].size.times do |j|
    @lines_array[2].size.times do |k|
      @data_array[i + 1][j + 1][k + 1][0] = [
        @data_array[i][j + 1][k + 1][0] + scorings.gap_cost,
        @data_array[i + 1][j][k + 1][0] + scorings.gap_cost,
        @data_array[i + 1][j + 1][k][0] + scorings.gap_cost,
        @data_array[i + 1][j][k][0] + scorings.gap_cost,
        @data_array[i][j + 1][k][0] + scorings.gap_cost,
        @data_array[i][j][k + 1][0] + scorings.gap_cost,
        @data_array[i][j][k][0] + scorings[@lines_array[0][i], @lines_array[1][j]] + 
                  scorings[@lines_array[1][j], @lines_array[2][k]] + 
                  scorings[@lines_array[0][i], @lines_array[2][k]] 
                  ].min
    end
    
    @lines_array[3].size.times do |s|
      @data_array[i + 1][j + 1][0][s + 1] = [
        @data_array[i][j + 1][0][s + 1] + scorings.gap_cost,
        @data_array[i + 1][j][0][s + 1] + scorings.gap_cost,
        @data_array[i + 1][j + 1][0][s] + scorings.gap_cost,
        @data_array[i + 1][j][0][s] + scorings.gap_cost,
        @data_array[i][j + 1][0][s] + scorings.gap_cost,
        @data_array[i][j][0][s + 1] + scorings.gap_cost,
        @data_array[i][j][0][s] + scorings[@lines_array[0][i], @lines_array[1][j]] + 
                  scorings[@lines_array[1][j], @lines_array[3][s]] + 
                  scorings[@lines_array[0][i], @lines_array[3][s]] 
                  ].min
    end
  end
  
  #
  
  @lines_array[2].size.times do |j|
    @lines_array[3].size.times do |s|
      @data_array[i + 1][j + 1][0][s + 1] = [
        @data_array[i][j + 1][0][s + 1] + scorings.gap_cost,
        @data_array[i + 1][j][0][s + 1] + scorings.gap_cost,
        @data_array[i + 1][j + 1][0][s] + scorings.gap_cost,
        @data_array[i + 1][j][0][s] + scorings.gap_cost,
        @data_array[i][j + 1][0][s] + scorings.gap_cost,
        @data_array[i][j][0][s + 1] + scorings.gap_cost,
        @data_array[i][j][0][s] + scorings[@lines_array[0][i], @lines_array[1][j]] + 
                  scorings[@lines_array[1][j], @lines_array[3][s]] + 
                  scorings[@lines_array[0][i], @lines_array[3][s]] 
                  ].min
    end
  end
end


# 0 & 1 & 3
@lines_array[1].size.times do |i|
  @lines_array[2].size.times do |j|
    @lines_array[3].size.times do |k|
      @data_array[0][i + 1][j + 1][k + 1] = [
        @data_array[0][i][j + 1][k + 1] + scorings.gap_cost*2,
        @data_array[0][i + 1][j][k + 1] + scorings.gap_cost*2,
        @data_array[0][i + 1][j + 1][k] + scorings.gap_cost*2,
        @data_array[0][i + 1][j][k] + scorings.gap_cost + scorings[@lines_array[2][j], @lines_array[3][k]],
        @data_array[0][i][j + 1][k] + scorings.gap_cost + scorings[@lines_array[1][i], @lines_array[3][k]],
        @data_array[0][i][j][k + 1] + scorings.gap_cost + scorings[@lines_array[1][i], @lines_array[2][j]],
        @data_array[0][i][j][k] + scorings[@lines_array[1][i], @lines_array[2][j]] + 
                  scorings[@lines_array[2][j], @lines_array[3][k]] + 
                  scorings[@lines_array[1][i], @lines_array[3][k]] 
                  ].min
    end
  end
end
# all
#------------------------

# 0 & 1 & 3

@lines_array[0].size.times do |i|
  @lines_array[1].size.times do |j|
    @lines_array[2].size.times do |k|
      @lines_array[3].size.times do |s|
      
        @data_array[i + 1][j + 1][k + 1][s + 1] = [
        @data_array[i][j + 1][k + 1][s + 1] + scorings.cost('-', '-', '-', @lines_array[0][i]),
        @data_array[i + 1][j][k + 1][s + 1] + scorings.cost('-', '-', '-', @lines_array[1][j]),
        @data_array[i + 1][j + 1][k][s + 1] + scorings.cost('-', '-', '-', @lines_array[2][k]),
        @data_array[i + 1][j + 1][k + 1][s] + scorings.cost('-', '-', '-', @lines_array[3][s]),
        
        @data_array[i][j][k + 1][s + 1] + scorings.cost('-', '-', @lines_array[1][j], @lines_array[0][i]),
        @data_array[i + 1][j][k][s + 1] + scorings.cost('-', '-', @lines_array[2][k], @lines_array[1][j]),
        @data_array[i + 1][j + 1][k][s] + scorings.cost('-', '-', @lines_array[3][s], @lines_array[2][k]),
        @data_array[i][j + 1][k + 1][s] + scorings.cost('-', '-', @lines_array[0][i], @lines_array[3][s]),
        @data_array[i + 1][j][k + 1][s] + scorings.cost('-', '-', @lines_array[3][s], @lines_array[1][j]),
        @data_array[i][j + 1][k][s + 1] + scorings.cost('-', '-', @lines_array[0][i], @lines_array[2][k]),
        
        @data_array[i + 1][j][k][s] + scorings.cost('-', @lines_array[1][j], @lines_array[2][k], @lines_array[3][s]),
        @data_array[i][j + 1][k][s] + scorings.cost('-', @lines_array[0][i], @lines_array[2][k], @lines_array[3][s]),
        @data_array[i][j][k + 1][s] + scorings.cost('-', @lines_array[0][i], @lines_array[1][j], @lines_array[3][s]),
        @data_array[i][j][k][s + 1] + scorings.cost('-', @lines_array[0][i], @lines_array[1][j], @lines_array[2][k]),
        
        @data_array[i][j][k][s] + scorings.cost(@lines_array[3][s], @lines_array[0][i], @lines_array[1][j], @lines_array[2][k])
                  ].min
      end
    end
  end
end

#
#

def get_match?(i, j)
  s1 = @lines_array[0][i]
  s2 = ""
  if (j - 2*MAX_ARRAY_SIZE - (i % MAX_ARRAY_SIZE)).abs < MAX_ARRAY_SIZE
    s2 = @lines_array[1][j - 2 * MAX_ARRAY_SIZE + (i / MAX_ARRAY_SIZE)* MAX_ARRAY_SIZE]
  else
    s2 = @lines_array[1][j + (i / MAX_ARRAY_SIZE) * MAX_ARRAY_SIZE]
  end
  s1 == s2
end

def print_all_data(n, arr)
  #@data_array.map do |x| x.map{ |xx| }}
  # i don't want to do it now it scares me
end

# output min distance
g = File.new("output1.txt", 'w')
#g.puts "#{@data_arrays[@lines_array[0].size - 1][get_arr_y(@lines_array[0].size - 1, @lines_array[0].size - 1)][0] > MAX_EDIT_DISTANCE ? 'not similar' : 
#     @data_arrays[@lines_array[0].size - 1][get_arr_y(@lines_array[0].size - 1, @lines_array[0].size - 1)][0]}"
g.close

#g = File.new("output2.txt", 'w')

# g.puts "#{@data_arrays[@lines_array[0].size - 1][get_arr_y(@lines_array[0].size - 1, @lines_array[0].size - 1)][0]}"
#g.close