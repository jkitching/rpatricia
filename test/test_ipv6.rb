require 'test/unit'
require 'rpatricia'

class TestIPv6 < Test::Unit::TestCase
  def test_ipv6
    t = Patricia.new :AF_INET6
    assert_equal :AF_INET6, t.family
    node = t.add('1234:567::/35', 'hello world')
    assert_kind_of Patricia::Node, node
    assert_equal 'hello world', node.data
    assert_equal 'hello world', t.match_best('1234:567::/128').data
  end

  def test_ipv4_mapped_ipv6
    t = Patricia.new :AF_INET6
    assert_equal :AF_INET6, t.family
    node = t.add('::ffff:10.0.0.0/8', 'hello world')
    assert_kind_of Patricia::Node, node
    assert_equal 'hello world', node.data
    assert_equal 'hello world', t.match_best('::ffff:10.0.0.1').data
  end
end
