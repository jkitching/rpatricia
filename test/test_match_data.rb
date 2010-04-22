require 'test/unit'
require 'rpatricia'

class TestMatchData < Test::Unit::TestCase

  def test_match_data_non_string
    ary = []
    t = Patricia.new
    t.add('10.0.0.0/8', ary)

    z = t.match_best '10.1.1.1'
    assert_equal ary.object_id, z.data.object_id
    z.data << '10.1.1.1'

    t.match_best('10.2.2.2').data.include?('10.1.1.1')
  end

  # backwards compatibility, we always return a new string
  def test_match_data_string
    data = "hello"
    t = Patricia.new
    t.add('10.0.0.0/8', data)

    z = t.match_best '10.1.1.1'
    assert_equal "hello", z.data
    assert(data.object_id != z.data.object_id)

    y = t.match_best '10.1.1.1'
    assert_equal "hello", y.data
    assert(y.data.object_id != z.data.object_id)
  end

  def test_match_data_nothing
    t = Patricia.new
    t.add('10.0.0.0/8')

    z = t.match_best '10.1.1.1'
    assert_equal "", z.data
    z_object_id = z.data

    y = t.match_best '10.1.1.1'
    assert_equal "", y.data
    assert(y.data.object_id != z_object_id)
  end

end
