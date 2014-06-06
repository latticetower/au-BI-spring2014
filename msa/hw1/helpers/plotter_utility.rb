require "rubyvis"

module PlotterUtility
  def self.generate_plot(filename, data)
  #:name => data[:name],
    #:x_name => data[:x_name], :y_name => data[:y_name], :bar => data[:bar], :width => data[:width], :height => data[:height], :barw=> data[:barw]
    self.create_plot(to_openstruct(data[:values]), filename, data)
  #gc_distribution, ARGV[1], 
  #:name => 'Insertion distance between pair reads', 
  #:x_name => 'Distance between pair reads', 
  #:y_name => 'Number of reads')
  end
  
  def self.generate_plot2(filename, data_array)
    self.create_plot2(data_array.map{|data| to_openstruct(data[:values])}, filename, data_array)
  end
  
  # let's assume that data_values is array of arrays: [[]] each subarray contains 3 elements - x, y and z coordinate. array is unsorted
  def self.to_openstruct(data_values, size =nil)
      data_values.map{|point| OpenStruct.new({:x => point[0], :y => point[1], :z => (size || point[2]) })}.sort{|d1, d2| d1.x <=> d2.x }
  end
  
  def self.to_openstruct2(data_values, text_x)
      data_values.zip(text_x).map{|point| OpenStruct.new({:x => point[0], :y => point[1], :z => point[2], :text_x =>  point[3] })}.sort{|d1, d2| d1.x <=> d2.x }
  end
  
  def self.create_plot(data_values, filename, options)
    w = 800
    h = 700
    w = options[:width] if options[:width]
    h = options[:height] if options[:height]
    
    # /* The root panel. */
    vis1 = pv.Panel.new().width(w + 40).height(h + 40).bottom(40).left(100).right(40).top(5)
      

    x = pv.Scale.linear(data_values, lambda {|d| d.x}).range(0, w)
    y = pv.Scale.linear(data_values, lambda {|d| d.y}).range(0, h)
    c = pv.Scale.log(1, 100).range("orange", "brown")
    
    panel = vis1.add(Rubyvis::Panel).left(10).top(20).width(w).height(h)

    
  if options[:bar]
    bar_width = 40
    bar_width = options[:barw] if options[:barw]
    panel.bar.data(data_values).line_width(2).width(bar_width).left(lambda {|d| x.scale(d.x)-bar_width/2}).top(lambda {|d| h - y.scale(d.y)})
  else
    panel.add(Rubyvis::Line).data(data_values).line_width(2).left(lambda {|d| x.scale(d.x)}).bottom(lambda {|d| y.scale(d.y)}).
      interpolate("linear").stroke_style("lightblue")
    
  end      
  panel.add(Rubyvis::Dot).data(data_values).stroke_style(lambda {|d| c.scale(d.z)}).
    fill_style(lambda {|d| c.scale(d.z).alpha(0.5)}).shape_size(lambda {|d| d.z}).
  line_width(2).left(lambda {|d| x.scale(d.x)}).bottom(lambda {|d| y.scale(d.y)}).
      interpolate("linear") #.stroke_style("green")
    
    
    
    panel.anchor('top').add(Rubyvis::Label).top(5).left(100).text(options[:name])
    panel.anchor('left').add(Rubyvis::Label).left(5).top(100).text(options[:y_name])
    panel.anchor('bottom').add(Rubyvis::Label).right(5).bottom(5).text(options[:x_name])
    #/* X-axis and ticks. */
    xRule = panel.add(pv.Rule).data(x.ticks()).visible(lambda {|d| !d.nil? && d > 0}).strokeStyle(lambda {|d|  d!=0 ? "rgba(128,128,128,.2)" : "#000"}).left(x).bottom(-10).anchor("bottom")
    
    xRule.add(pv.Label).text(x.tick_format)
    
    panel.add(pv.Rule).data(x.ticks()).left(x).bottom(-10).stroke_style(lambda {|d|  d!=0 ? "rgba(255,255,255,.3)" : "#000"})
    #/* Y-axis and ticks. */
    panel.add(pv.Rule).data(y.ticks(10)).visible(lambda {|d| !d.nil? && d > 0}).bottom(y).strokeStyle(lambda {|d|  d!=0 ? "rgba(128,128,128,.2)" : "#000"}).anchor("left").add(pv.Label).text(y.tick_format)
    panel.add(pv.Rule).data(y.ticks(10)).left(0).bottom(y).stroke_style(lambda {|d|  d!=0 ? "rgba(255,255,255,.3)" : "#000"})
    
    vis1.render()

    @outfile1 = File.new(filename, 'w')
    @outfile1.puts vis1.to_svg
    @outfile1.close
  end
  
  
  def self.create_plot2(data_values_arr, filename, options_arr)
    w = 900
    h = 600
    xmax = data_values_arr.flatten.max{|x, y| (x.x ||0) <=> (y.x||0)}.x
    xmin = data_values_arr.flatten.min{|x, y| (x.x||0) <=> (y.x||0)}.x
    ymax = data_values_arr.flatten.max{|x, y| (x.y||0) <=> (y.y||0)}.y
    ymin = data_values_arr.flatten.min{|x, y| (x.y||0) <=> (y.y||0)}.y
    
    puts xmax, xmin, ymax, ymin
    #w = options_arr[0][:width] if options_arr[0][:width]
    #h = options_arr[0][:height] if options_arr[0][:height]
    
    # /* The root panel. */
    vis1 = pv.Panel.new().width(w + 40).height(h + 40).bottom(40).left(100).right(40).top(5)
    x = pv.Scale.linear(data_values_arr.flatten, lambda {|d| d.x.to_f ||0}).nice().range(0, w)
    y = pv.Scale.linear(data_values_arr.flatten, lambda {|d| d.y.to_f ||0}).nice().range(0, h) 
    c = pv.Scale.linear(0, (data_values_arr.size > 1 ? data_values_arr.size-1 : 1)).range( "green", "red")
    
      panel = vis1.add(Rubyvis::Panel).left(10).top(20).width(w).height(h)
#the main cycle
    data_values_arr.size.times do |i|
      options = options_arr[i]
      data_values = data_values_arr[i]
    
      if options[:bar]
        bar_width = 40
        bar_width = options[:barw] if options[:barw]
        panel.bar.data(data_values).line_width(0).width(bar_width).
          left(lambda{|d| x.scale(d.x) - bar_width/2}).top(lambda {|d| h - y.scale(d.y)}).fill_style(c.scale(i).alpha(0.5))#(options[:color] || "lightblue", 0.5)
      else
        panel.add(Rubyvis::Line).data(data_values).line_width(2).left(lambda {|d| x.scale(d.x)}).bottom(lambda {|d| y.scale(d.y)}).
          interpolate("linear").stroke_style(c.scale(i))
      end
      if options[:show_points]
        panel.add(Rubyvis::Dot).data(data_values).stroke_style(lambda {|d| c.scale(d.z)}).
          fill_style(lambda {|d| c.scale(d.z).alpha(0.3)}).shape_size(lambda {|d| d.z}).
          line_width(2).left(lambda {|d| x.scale(d.x)}).bottom(lambda {|d| y.scale(d.y)}).
        interpolate("linear") #.stroke_style("green")
      end
      
      panel.anchor('top').add(Rubyvis::Label).top(5+i*10).left(100).text(options[:name])
      panel.anchor('left').add(Rubyvis::Label).left(5).top(100+i*10).text(options[:y_name])
      panel.anchor('bottom').add(Rubyvis::Label).right(5).bottom(5+6*i).text(options[:x_name])
    end
    x_label_ticks = x.ticks.each_slice(1).map(&:first)
    y_label_ticks = y.ticks.each_slice(1).map(&:first)
          #/* X-axis and ticks. */
      xRule = panel.add(pv.Rule).data(x.ticks()).visible(lambda {|d| x_label_ticks.member?(d)}).
        strokeStyle( "rgba(128,128,128,.2)" ).left(x).bottom(-10).anchor("bottom")
    
      xRule.add(pv.Label).text(x.tick_format)
    
      panel.add(pv.Rule).data(x.ticks()).left(x).bottom(-10).stroke_style("rgba(255,255,255,.3)")
      #/* Y-axis and ticks. */
      panel.add(pv.Rule).data(y.ticks()).visible(lambda {|d| y_label_ticks.member?(d)}).bottom(y).strokeStyle(lambda {|d|  d!=0 ? "rgba(128,128,128,.2)" : "#000"}).anchor("left").add(pv.Label).text(y.tick_format)
      panel.add(pv.Rule).data(y.ticks()).left(0).bottom(y).stroke_style(lambda {|d|  d!=0 ? "rgba(255,255,255,.3)" : "#000"})  

    vis1.render()

    @outfile1 = File.new(filename, 'w')
    @outfile1.puts vis1.to_svg
    @outfile1.close
  end
  
  def self.print_data(data)
   data.each do |d|
    puts "x: #{d.x}, y:#{d.y}"
   end
  end
  
  def self.save_to_file(data, filename)
  
    @outfile1 = File.new(filename, 'w')
    @outfile1.puts "[" + data.map{|element|  yield(element) }.join(',') + "]" if block_given? 
    @outfile1.close
  end
end