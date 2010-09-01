require 'test/unit'
require 'rpatricia'

class TestInvalidInput < Test::Unit::TestCase

  def setup
    @t = Patricia.new
  end

  def test_invalid_add
    assert_raises(ArgumentError) { @t.add('1000.0.0.0/8') }
    assert_raises(ArgumentError) { @t.add("\0") }
    assert_raises(ArgumentError) { @t.add("\0\0\0\0\0\0" * 1024 * 1024) }
  end

  def test_invalid_remove
    assert_raises(ArgumentError) { @t.remove('1000.0.0.0/8') }
  end

  def test_invalid_match_best
    assert_raises(ArgumentError) { @t.match_best('1000.0.0.0/8') }
  end

  def test_invalid_match_exact
    assert_raises(ArgumentError) { @t.match_exact('1000.0.0.0/8') }
  end
end
