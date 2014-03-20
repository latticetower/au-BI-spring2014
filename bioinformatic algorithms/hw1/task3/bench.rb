require 'benchmark'

File.open('solution.rb') do |file|
  puts Benchmark.measure{eval(file.lines.to_a.join)}
end