require 'test/unit'
require 'rpatricia'
require 'stringio'

class TestShowNodes < Test::Unit::TestCase

  def test_show_nodes
    t = Patricia.new
    assert_nothing_raised do
      t.add("127.0.0.0/24")
      t.add("192.168.1.0/24")
      t.add("192.168.2.0/24")
      t.add("192.168.3.100")
      t.add("10.0.0.0/8", "pref_10")
    end
    begin
      oldout = $stdout
      $stdout = tmpout = StringIO.new
      assert_nothing_raised { t.show_nodes }
    ensure
      $stdout = oldout
    end
    expect = [
      "node: 10.0.0.0/8",
      "node: 127.0.0.0/24",
      "node: 192.168.1.0/24",
      "node: 192.168.2.0/24",
      "node: 192.168.3.100/32"
    ].join("\n") << "\n"
    assert_equal expect, tmpout.string
  end
end
