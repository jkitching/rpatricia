require 'test/unit'
require 'rpatricia'
require 'stringio'

class TestDuplicate < Test::Unit::TestCase
  def test_dup
    tmp = {}
    t = Patricia.new
    t.add('127.0.0.0/8', tmp)
    t2 = t.dup
    assert_equal 1, t2.num_nodes
    assert_equal tmp.object_id, t2.match_best('127.0.0.1').data.object_id
    t2.add('10.0.0.0/8', zz = [])
    assert_equal 2, t2.num_nodes
    assert_equal 1, t.num_nodes

    oldout = $stdout
    begin
      $stdout = stringio = StringIO.new
      t2.show_nodes
      puts "--"
      t.show_nodes
    ensure
      $stdout = oldout
    end
    p stringio.string
  end
end
