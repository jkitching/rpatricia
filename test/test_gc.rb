require 'test/unit'
require 'rpatricia'

# this takes a while, watch memory usage not grow in top(1) or similar

class TestGc < Test::Unit::TestCase

  def setup
    @arrays = Patricia.new
    @arrays.add('10.0.0.0/8', [])
    @arrays.add('127.0.0.0/24', [])

    @strings = Patricia.new
    @strings.add('10.0.0.0/8', "big lan")
    @strings.add('127.0.0.0/24', "localhost")
  end

  def test_gc
    assert_nothing_raised do
      5_000_000.times do
        t = Patricia.new
        t.add('10.0.0.0/8', {})
        t.add('127.0.0.0/24', "home sweet home")
      end
    end

    # ensure what we created originally didn't get GC-ed'
    5_000_000.times do
      assert_equal [], @arrays.match_best('127.0.0.1').data
      assert_equal "localhost", @strings.match_best('127.0.0.1').data
    end
  end

  def test_destroy
    assert @strings.destroy
    assert @strings.destroy
  end

end
