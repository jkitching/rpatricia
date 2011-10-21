# to build a gem:
#   gem build rpatricia.gemspec

Gem::Specification.new do |s|
  s.name = %q{rpatricia}
  s.version = %q{0.09} # remember to update Changes if this is changed

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
ext/rpatricia/extconf.rb
ext/rpatricia/patricia.c
ext/rpatricia/patricia.h
ext/rpatricia/rpatricia.c
rpatricia.gemspec
test.rb
)
  s.extensions = %w(ext/rpatricia/extconf.rb)
end
