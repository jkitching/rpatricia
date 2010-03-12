# to build a gem:
#   gem build rpatricia.gemspec

Gem::Specification.new do |s|
  s.name = %q{rpatricia}
  s.version = %q{0.04} # remember to update Changes if this is changed

  s.homepage = "http://www.goto.info.waseda.ac.jp/~tatsuya/rpatricia/"

  # Tatsuya wrote the extension, Eric gemified this
  s.authors = [ "Tatsuya Mori", "Eric Wong" ]

  s.date = Time.now.utc.strftime('%Y-%m-%d')
  s.summary = %q{module for fast IP address/prefix lookups}
  s.description = %q{
This is a ruby wrapper of Net::Patricia, which has been developed by
Dave Plonka. The original Net::Patricia and its C API are available
from:
http://net.doit.wisc.edu/~plonka/Net-Patricia/

Net::Patricia is a module for fast IP address/prefix lookups.
I have modified some interfaces for the Ruby wrapper version.
}.strip

  # bother Eric for all gem issues
  s.email = %q{normalperson@yhbt.net}

  # generated using "git ls-files" from Eric's git repo
  s.files = %w(
Changes
README
TODO
copyright
credits.txt
extconf.rb
patricia.c
patricia.h
rpatricia.c
test.rb
) - %w(test.rb)
# intentionally omitting test.rb to avoid being accidentally required
# in case somebody does "require 'test'" in their code.  We
# still need to hope nobody calls "require 'extconf'"
#
# See below for plans...

  # the path layout is unchanged from the current tarballs
  # We should move files currently in the top level into the more
  # traditional ext/rpatricia/ directory in the future to avoid
  # require conflicts like we have with test.rb
  s.require_paths = %w(.)
  s.extensions = %w(extconf.rb)
end
