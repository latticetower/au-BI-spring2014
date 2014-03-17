LINE_SIZE = 75
MAX_LENGTH = 100000

#generates sequence of chars with specified length
def generate_line
  string_array = "ACGT".chars.to_a
  Enumerator.new do |yielder|
    loop do
      yielder.yield string_array.sample
    end
  end
end

output_filename = (ARGV.size > 0 ? ARGV[0] : "input.txt")
f = File.new(output_filename, 'w')

replacement = {'A' => 'T', 'T' => 'A', 'C' => 'G', 'G' => 'C'}
str = ""
chars_printed = MAX_LENGTH
f.puts ">line 1" 
while chars_printed > 0 do 
  min_chars = [chars_printed, LINE_SIZE].min
  str += generate_line.take(min_chars).to_a.join
  f.puts generate_line.take(min_chars).to_a.join
  chars_printed -= min_chars
end
r = Random.new
100.times do |i|
  str[r.rand(0..str.length)] = replacement[str[r.rand(0..str.length)]]
end

f.puts ">line 2" 
chars_printed = MAX_LENGTH

while chars_printed > 0 do
  min_chars = [chars_printed, LINE_SIZE].min
  # puts str.slice(MAX_LENGTH - chars_printed, min_chars)
  f.puts str.slice(MAX_LENGTH - chars_printed, min_chars)
  chars_printed -= min_chars
end
f.close