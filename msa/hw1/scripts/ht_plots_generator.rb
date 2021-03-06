# this codefile uses ruby 1.9.3 and is executed locally, with previously generated data hashes as an input.
# data hashes are generated from sam files
# example input file line:
# {:type=>"plot", :filename => 'ff.svg', :plot_data => {:values => [[0,1,1],[1,2,1]], :name => 'Insertion distance between pair reads', :x_name => 'Distance between pair reads', :y_name => 'Number of reads'}}
## ARGV - must contain data files like "bwa_data125", "bowtie_data"
## 
# - this line produces svg image
require '../helpers/plotter_utility.rb'


@@data_hash = Hash.new

if ARGV.size < 1
  puts "print filenames with data"
  exit
end
# process all data generated previously
ARGV.each do |filename|
  File.open(filename) do |datafile|
    data_array = {}
    while not datafile.eof? do
      inputline = eval(datafile.gets)
      if inputline.has_key?(:type) and inputline[:type] == "plot" #if data string describes plot, get plot name, and plot data and output somewhere...
        data_array['../output/img/' + inputline[:filename]] ||= []
        data_array['../output/img/' + inputline[:filename]] << inputline[:plot_data]
      end
    end
    data_array.each_pair do |filename, data|
      PlotterUtility::generate_plot2(filename, data)
    end
  end
end
