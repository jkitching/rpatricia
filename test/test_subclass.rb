require 'test/unit'
require 'rpatricia'

class SubPatricia < Patricia
end

class TestSubclass < Test::Unit::TestCase
  def test_new
    assert_equal SubPatricia, SubPatricia.new.class
  end
end
