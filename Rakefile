require 'rubygems'
require 'rake/gempackagetask'

begin
  require 'jeweler'
  Jeweler::Tasks.new do |s|
    s.name        = "ruby-shout"
    s.summary     = "Send audio over the network to an Icecast server"
    s.description = %q{Ruby bindings for libshout 2, a "Library which can be used to write a source client like ices" for Icecast (http://www.icecast.org/download.php).}
    s.email       = "mail@niko-dittmann.com"
    s.homepage    = "http://github.com/niko/ruby-shout"
    s.authors     = ["Jared Jennings", "Niko Dittmann"]
    s.require_paths = ["lib"]
    s.rubyforge_project = 'ruby-shout'
    s.files       = FileList["[A-Z]*", "{ext}/*"]
    
  end
rescue LoadError
  puts "Jeweler, or one of its dependencies, is not available. Install it with: sudo gem install technicalpickles-jeweler -s http://gems.github.com"
end
