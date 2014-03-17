require 'bio'

outputfile = File.new("../output/data/task2.txt", 'w')
File.open("../input/NC_004317.gbk") do |file|
  entry = file.gets(Bio::GenBank::DELIMITER)
  gb = Bio::GenBank.new(entry)
  puts "Processing #{gb.definition}"
  gb.each_cds() do |cds|
    outputfile.puts cds.locations.absolute(1) # same as (loc.strand == 1 ? loc.from : loc.to ), where  loc = cds.locations.first
  end
end
outputfile.close