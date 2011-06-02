require 'test/unit'
require 'rpatricia'
require 'stringio'

class TestDuplicate < Test::Unit::TestCase
  def test_dup
    tmp = {}
    t = Patricia.new
    t.add('127.0.0.0/8', tmp)
    t2 = t.dup
    assert_equal :AF_INET, t2.family
    assert_equal 1, t2.num_nodes
    assert_equal tmp.object_id, t2.match_best('127.0.0.1').data.object_id
    t2.add('10.0.0.0/8', zz = [])
    assert_equal 2, t2.num_nodes
    assert_equal 1, t.num_nodes

    tio = StringIO.new
    t.show_nodes(tio)
    assert_equal "node: 127.0.0.0/8\n", tio.string

    t2io = StringIO.new
    t2.show_nodes(t2io)
    assert_equal("node: 10.0.0.0/8\nnode: 127.0.0.0/8\n", t2io.string)
  end

  def test_dup_ipv6
    tmp = {}
    t = Patricia.new :AF_INET6
    t.add('1234:4321::/32', tmp)
    t.add('2600:0102:a100::/43', tmp)
    t2 = t.dup
    assert_equal :AF_INET6, t2.family
    assert_equal 2, t2.num_nodes
    t2.add('::1/128', zz = [])
    assert_equal 3, t2.num_nodes
    assert_equal 2, t.num_nodes

    tio = StringIO.new
    t.show_nodes(tio)
    expect = "node: 1234:4321::/32\nnode: 2600:102:a100::/43\n"
    assert_equal expect, tio.string

    t2io = StringIO.new
    t2.show_nodes(t2io)
    expect = "node: ::1/128\nnode: 1234:4321::/32\nnode: 2600:102:a100::/43\n"
    assert_equal expect, t2io.string
  end
end
