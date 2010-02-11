require 'rpatricia'

def my_test condition
  puts (condition)? "ok!" : "error!"
end

puts "test: creating Patricia object"
my_test(t = Patricia.new) 

puts "test: adding 127.0.0.0/24"
my_test(n = t.add("127.0.0.0/24"))

puts "  data of added node = #{n.data}"
puts "  prefix of added node = #{n.prefix}"
puts "  network of added node = #{n.network}"
puts "  prefixlen of added node = #{n.prefixlen}"

puts "adding 192.168.1.0/24, 192.168.2.0/24, 192.168.3.100"
t.add("192.168.1.0/24")
t.add("192.168.2.0/24")
t.add("192.168.3.100")

puts "test: match_best 127.0.0.1"
my_test(n = t.match_best("127.0.0.1"))

puts "test: match_exact 192.168.3.100"
my_test(n = t.match_best("192.168.3.100"))

puts "  data of matched node = #{n.data}"
puts "  prefix of matched node = #{n.prefix}"
puts "  network of matched node = #{n.network}"
puts "  prefixlen of matched node = #{n.prefixlen}"

puts "test: adding prefix 10.0.0.0/8 with user data of 'pref_10.0.0.0/8'"
my_test(n = t.add("10.0.0.0/8", "pref_10"))

puts "  data of added node = #{n.data}"
puts "  prefix of added node = #{n.prefix}"
puts "  network of added node = #{n.network}"
puts "  prefixlen of added node = #{n.prefixlen}"

puts "test: match string 10.0.0.1"
my_test(n = t.match_best("10.0.0.1"))

puts "  data of matched node = #{n.data}"
puts "  prefix of matched node = #{n.prefix}"
puts "  network of matched node = #{n.network}"
puts "  prefixlen of matched node = #{n.prefixlen}"

puts "test: remove '42.0.0.0/8'; should return nil"
my_test(!t.remove("42.0.0.0/8"))

puts "test: remove '10.0.0.0/8'"
my_test(n= t.remove("10.0.0.0/8"))


puts "test: number of nodes in the tree should be '4'"
my_test(4 == t.num_nodes)

puts "test: showing all nodes"
t.show_nodes

t.destroy
