LINE_SIZE = 75
MAX_LENGTH = 50

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

4.times do |i|
  chars_printed = MAX_LENGTH
  f.puts ">line #{i + 1}" 
  while chars_printed > 0 do 
    min_chars = [chars_printed, LINE_SIZE].min
    f.puts generate_line.take(min_chars).to_a.join
    chars_printed -= min_chars
  end
end

f.close