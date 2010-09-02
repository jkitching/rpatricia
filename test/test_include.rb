require 'test/unit'
require 'rpatricia'

class TestInclude < Test::Unit::TestCase

  def setup
    @t = Patricia.new
  end

  def test_include_exact
    @t.add '127.0.0.1'
    assert_equal true, @t.include?('127.0.0.1')
    assert_equal false, @t.include?('127.0.0.2')
    @t.clear
    assert_equal false, @t.include?('127.0.0.1')
  end

  def test_include_match_prefix
    @t.add '127.0.0.0/8'
    assert_equal true, @t.include?('127.0.0.32')
    assert_equal false, @t.include?('12.0.0.32')
    @t.clear
    assert_equal false, @t.include?('127.0.0.32')
  end
end
