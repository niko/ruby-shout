require 'rspec'

BASE_DIR = File.expand_path File.join(File.dirname(__FILE__), '..')
VERSION = File.open(File.join(BASE_DIR, 'VERSION')).readline.strip
require File.join(BASE_DIR, "spec/test_gem_installation/gems/ruby-shout-#{VERSION}/lib/shout")

# Note that the specs are (sadly) interdependant:
# All tests use the version of the gem build by RubyShoutBuildTest (build_test.rb).
# I _could_ build the gem before each run, but that takes too much time.
# So when changing C-stuff, be sure to run the build specs first.
