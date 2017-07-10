notes = []
mids = []
num = 1023/28.0
(1..28).each do |v|
  v % 2 == 0 ? notes.push((num*v).round) : mids.push((num*v).round)
end

print notes
puts ''
print mids