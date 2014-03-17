MAX_EDIT_DISTANCE = 100
MAX_ARRAY_SIZE = 101
GAP_COST = 1
MISMATCH_COST = 1

input_filename = "input.txt"

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

@data_arrays = Array.new(@lines_array[0].size){ Array.new(2*MAX_ARRAY_SIZE) {[0, -1]} }

# returns nil if coordinate lies outside of main diagonal with specified interval
def get_arr_x(x, y) 
  x # this coordinate doesn't change
end

# returns nil if coordinate lies outside of main diagonal with specified interval
def get_arr_y(x, y)
  return nil if (x - y).abs >= MAX_ARRAY_SIZE
  y0 = y - (x / MAX_ARRAY_SIZE)*MAX_ARRAY_SIZE
  if y0 < 0
    y0 = MAX_ARRAY_SIZE*2 + y0
  end
  y0
end

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
  n.times do |j|
    n.times do |i|
      x, y = get_arr_x(i, j), get_arr_y(i, j)
      #print (y.nil? ? "-" : arr[x][y][0])
    end
    #puts ''
  end
end

def collect_square(startx, arr)
  if (startx > 0)
    # corner:
    arr[startx][0].replace [
                    [arr[startx - 1][MAX_ARRAY_SIZE - 1][0] + (get_match?(startx, 0) ? 0 : MISMATCH_COST), [startx - 1, MAX_ARRAY_SIZE - 1]],
                    [arr[startx - 1][MAX_ARRAY_SIZE][0] + GAP_COST, [startx - 1, MAX_ARRAY_SIZE]],
                    [arr[startx][MAX_ARRAY_SIZE * 2 - 1][0] + GAP_COST, [startx, MAX_ARRAY_SIZE * 2 - 1]]
                  ].min{|x, y| x[0] <=> y[0]}
    # 1st row:
    1.upto(MAX_ARRAY_SIZE - 2) do |i|
      break if startx + i >= arr.size
    # puts startx+i
      arr[startx + i][0].replace [
                    [arr[startx + i - 1][MAX_ARRAY_SIZE * 2 - 1][0] + (get_match?(startx + i, 0) ? 0 : MISMATCH_COST), [startx + i - 1, MAX_ARRAY_SIZE * 2 - 1]],
                    [arr[startx + i - 1][0][0] + GAP_COST, [startx + i - 1, 0]],
                    [arr[startx + i][MAX_ARRAY_SIZE * 2 - 1][0] + GAP_COST, [startx + i, MAX_ARRAY_SIZE * 2 - 1]]
                  ].min{|x, y| x[0] <=> y[0]}
    end
    if startx + MAX_ARRAY_SIZE - 1 < arr.size
      arr[startx + MAX_ARRAY_SIZE - 1][0].replace [
                    [arr[startx + MAX_ARRAY_SIZE - 1 - 1][MAX_ARRAY_SIZE * 2 - 1][0] + (get_match?(startx + MAX_ARRAY_SIZE - 1, 0) ? 0 : MISMATCH_COST), [startx + MAX_ARRAY_SIZE - 1 - 1, MAX_ARRAY_SIZE * 2 - 1]],
                    [arr[startx + MAX_ARRAY_SIZE - 1 - 1][0][0] + GAP_COST, [startx + MAX_ARRAY_SIZE - 1 - 1, 0]]
                  ].min{|x, y| x[0] <=> y[0]}
    end
    #1st column:
    1.upto(MAX_ARRAY_SIZE - 2) do |j|
      break if j >= arr[startx].size
      arr[startx][j].replace [
                          [arr[startx][j - 1][0] + GAP_COST, [j - 1, 0]],
                          [arr[startx - 1][j + MAX_ARRAY_SIZE - 1][0] + (get_match?(startx, j) ? 0 : MISMATCH_COST), [startx - 1, j + MAX_ARRAY_SIZE - 1]],
                          [arr[startx - 1][j + MAX_ARRAY_SIZE][0] + GAP_COST, [startx - 1, j + MAX_ARRAY_SIZE]]
                        ].min{|x, y| x[0] <=> y[0]}
    end
    if MAX_ARRAY_SIZE - 1 < arr[startx].size
      arr[startx][MAX_ARRAY_SIZE - 1].replace [
                          [arr[startx][MAX_ARRAY_SIZE - 2][0] + GAP_COST, [startx, MAX_ARRAY_SIZE - 2]],
                          [arr[startx - 1][MAX_ARRAY_SIZE - 1 + MAX_ARRAY_SIZE - 1][0] + (get_match?(startx, MAX_ARRAY_SIZE - 1) ? 0 : MISMATCH_COST), [startx - 1, MAX_ARRAY_SIZE - 1 + MAX_ARRAY_SIZE - 1]]
                        ].min{|x, y| x[0] <=> y[0]}
    end
    # unimpl
  else
    # 0,0
    arr[0][0] = [
                  [(get_match?(0, 0) ? 0 : MISMATCH_COST), [-1, -1]], 
                  [GAP_COST, [-1, -1]]
                ].min{|x, y| x[0] <=> y[0]}
    
    1.upto(MAX_ARRAY_SIZE - 1) do |j|
      # puts (get_match?(0, j) ? 0 : MISMATCH_COST)
      arr[0][j].replace [
                    [(j + 1)*GAP_COST, [0, j - 1]], 
                    [(j )*GAP_COST + (get_match?(0, j) ? 0 : MISMATCH_COST), [-1, -1]],
                    [arr[0][j - 1][0] + GAP_COST, [0, j - 1]]
                  ].min{|x, y| x[0] <=> y[0]}
    end
    # puts '--------'
    1.upto(MAX_ARRAY_SIZE - 1) do |i|
      # puts (get_match?(i, 0) ? 0 : MISMATCH_COST)
      arr[i][0].replace [
                    [(i + 1)*GAP_COST, [i - 1, 0]], 
                    [(i)*GAP_COST + (get_match?(i, 0) ? 0 : MISMATCH_COST), [-1, -1]],
                    [arr[i - 1][0][0] + GAP_COST, [i - 1, 0]]
                  ].min{|x, y| x[0] <=> y[0]}
    end
  end
  # main part
  (MAX_ARRAY_SIZE - 1).times do |i|
    break if startx + i + 1 >= arr.size
    (MAX_ARRAY_SIZE - 1).times do |j|
      arr[startx + i + 1][j + 1].replace [
                            [arr[startx + i][j + 1][0] + GAP_COST, [startx + i, j + 1]], 
                            [arr[startx + i + 1][j][0] + GAP_COST, [startx + i + 1, j]], 
                            [arr[startx + i][j][0] + (get_match?(startx + i + 1, j + 1) ? 0 : MISMATCH_COST), [startx + i,j]]
                            ].min{|x, y| x[0] <=> y[0]}
    end
  end
end

def collect_bottom(startx, arr)
  1.upto(MAX_ARRAY_SIZE - 1) do |i|
    MAX_ARRAY_SIZE.upto(i - 2 + MAX_ARRAY_SIZE) do |j|
      break if startx + i >= arr.size
    #  puts startx + i,j
      arr[startx + i][j].replace [
                              [arr[startx + i - 1][j][0] + GAP_COST, [startx + i - 1, j]],
                              [arr[startx + i][j - 1][0] + GAP_COST, [startx + i, j - 1]],
                              [arr[startx + i - 1][j - 1][0] + (get_match?(startx + i, j) ? 0 : MISMATCH_COST ), [startx + i - 1, j - 1] ]
                            ].min{|x, y| x[0] <=> y[0]}
    end
    
    #puts '---'
    if startx + i < arr.size
      arr[startx + i][i + MAX_ARRAY_SIZE - 1].replace [
                              [arr[startx + i][i + MAX_ARRAY_SIZE - 2][0] + GAP_COST, [startx + i, i + MAX_ARRAY_SIZE - 2]],
                              [arr[startx + i - 1][i + MAX_ARRAY_SIZE - 2][0] + (get_match?(startx + i, i + MAX_ARRAY_SIZE) ? 0 : MISMATCH_COST ), [startx + i - 1, i + MAX_ARRAY_SIZE - 2]]
                            ].min{|x, y| x[0] <=> y[0]}
    end
  end
end

def collect_top(startx, arr)
  return if startx >= arr.size
  arr[startx][1 + MAX_ARRAY_SIZE].replace [ 
                [arr[startx - 1][1][0] + GAP_COST, [startx - 1, 1]], 
                [arr[startx - 1][0][0] + (get_match?(startx, 1 + MAX_ARRAY_SIZE) ? 0 : MISMATCH_COST ), [startx - 1, 0]]
              ].min{|x, y| x[0] <=> y[0]}
  # init
  (2).upto(MAX_ARRAY_SIZE - 1) do |j|
    break if j + MAX_ARRAY_SIZE >= arr[startx].size
    # puts startx, j
    arr[startx][j + MAX_ARRAY_SIZE].replace [ 
                                        [arr[startx - 1][j][0] + GAP_COST, [startx - 1, j]],
                                        [arr[startx][j - 1 + MAX_ARRAY_SIZE][0] + GAP_COST, [startx, j - 1 + MAX_ARRAY_SIZE]],
                                        [arr[startx - 1][j - 1][0] + (get_match?(startx, j + MAX_ARRAY_SIZE) ? 0 : MISMATCH_COST ), [startx - 1, j - 1]]
                                      ].min{|x, y| x[0] <=> y[0]}
  end

  1.upto(MAX_ARRAY_SIZE - 2) do |i|  
    break if startx + i >= arr.size
      
      arr[startx + i][i + 1 + MAX_ARRAY_SIZE].replace [ 
                    [arr[startx + i - 1][i + MAX_ARRAY_SIZE + 1][0] + GAP_COST, [startx + i - 1, i + MAX_ARRAY_SIZE + 1]],
                    [arr[startx + i - 1][i + MAX_ARRAY_SIZE][0] + (get_match?(startx + i, i + 1 + MAX_ARRAY_SIZE) ? 0 : MISMATCH_COST ), [startx + i - 1, i + MAX_ARRAY_SIZE]] 
                  ].min{|x, y| x[0]<=>y[0]}
    # init
    (i + 2).upto(MAX_ARRAY_SIZE - 1) do |j|
      
      arr[startx + i][j + MAX_ARRAY_SIZE].replace [ 
                                          [arr[startx + i - 1][j + MAX_ARRAY_SIZE][0] + GAP_COST, [startx + i - 1, j + MAX_ARRAY_SIZE]],
                                          [arr[startx + i][j - 1 + MAX_ARRAY_SIZE][0] + GAP_COST, [startx + i, j - 1 + MAX_ARRAY_SIZE]],
                                          [arr[startx + i - 1][j - 1 + MAX_ARRAY_SIZE][0] + (get_match?(startx + i, j + MAX_ARRAY_SIZE) ? 0 : MISMATCH_COST ), [startx + i - 1, j - 1 + MAX_ARRAY_SIZE]]
                                          ].min{|x, y| x[0] <=> y[0]}
    end
  end

end

# print_all_data(@lines_array[0].size, @data_arrays)

(@lines_array[0].size / MAX_ARRAY_SIZE + 1).times do |i|
  collect_top(MAX_ARRAY_SIZE * i, @data_arrays)
  collect_square(MAX_ARRAY_SIZE * i, @data_arrays)
  collect_bottom(MAX_ARRAY_SIZE * i, @data_arrays)
end

#print_all_data(@lines_array[0].size, @data_arrays)

# output min distance
g = File.new("output1.txt", 'w')
g.puts "#{@data_arrays[@lines_array[0].size - 1][get_arr_y(@lines_array[0].size - 1, @lines_array[0].size - 1)][0] > MAX_EDIT_DISTANCE ? 'not similar' : 
     @data_arrays[@lines_array[0].size - 1][get_arr_y(@lines_array[0].size - 1, @lines_array[0].size - 1)][0]}"
g.close

#g = File.new("output2.txt", 'w')

# g.puts "#{@data_arrays[@lines_array[0].size - 1][get_arr_y(@lines_array[0].size - 1, @lines_array[0].size - 1)][0]}"
#g.close