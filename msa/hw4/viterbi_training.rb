# --------------
#   main code
# --------------

class HmmState
  attr_accessor :name, :emissions, :id, :transitions, :v
  def initialize(name, id, emissions, transitions)
    @name = name
    @id = id
    @emissions = emissions
    @transitions = transitions
    @v = 0
  end
end


hmm_states = [
  HmmState.new("StartState", 0, {'A' => 0, 'C' => 0, 'G' => 0, 'T' => 0}, [0, 0.996, 0.004]),
  HmmState.new("A+T rich state", 1, 
    {'A' => 0.291, 'C' => 0.209, 'G' => 0.209, 'T' => 0.291},
    [0, 0.999, 0.001]),
  HmmState.new("C+G rich state", 2, 
    {'A' => 0.169, 'C' => 0.331, 'G' => 0.331, 'T' => 0.169},
    [0, 0.01, 0.99])
]

def compute_forward(states, sequence)
  #initialization
  Enumerator.new do |yielder|
  states.size.times { |i| states[i].v = states[0].transitions[i]*states[i].emissions[sequence[0]] }
  (sequence.size).times do |i|
    x = Array.new(states.size)
    states.size.times do |k|
      x[k] = 0
      states.size.times do |j|
        a1 = states[j].transitions[k]*states[j].v
        a2 = states[x[k]].transitions[k]*states[x[k]].v
        x[k] = j if a1 > a2
      end            
    end
    yielder.yield x
    break if i + 1>sequence.size - 1
    states.size.times do |k|
      states[k].v = states[k].emissions[sequence[i + 1]]*states[x[k]].transitions[k]*states[x[k]].v
    end
  end
  end
end

forward_results = compute_forward(hmm_states, "ACGA").to_a

def compute_backward(states, vv)
Enumerator.new do |yielder|
  counter = vv.size - 1
  x = vv[counter]
  xmax = x.max_by{|xi| states[xi].v }
  yielder.yield xmax
  counter.times do |i|
    xmax = vv[counter - i - 1][xmax]
    yielder.yield xmax
  end
end
end

puts compute_backward(hmm_states, forward_results).to_a.reverse.join('')