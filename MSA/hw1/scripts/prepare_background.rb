require 'bio'

data = {'a' => 0, 'c' => 0, 'g' => 0, 't' => 0}
#0.prepare background
File.open("../input/NC_004317.gbk") do |file|
  entry = file.gets(Bio::GenBank::DELIMITER)
  gb = Bio::GenBank.new(entry)
  seq1 = gb.seq
  seq1.chars.each do |ch|
    if data.has_key? ch
      data[ch] += 1
    end
  end
end
puts data.to_s

file1 = File.new('../output/data/background.txt', 'w')
file1.puts data.to_s
file1.close