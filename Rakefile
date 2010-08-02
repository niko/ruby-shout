require 'rubygems'
require 'rake/gempackagetask'

begin
  require 'jeweler'
  Jeweler::Tasks.new do |s|
    s.name = "ruby-shout"
    s.summary = "Send audio over the network to an Icecast server"
    s.email = "mail@niko-dittmann.com"
    s.homepage = "http://github.com/niko/ruby-shout"
    s.authors = ["Niko Dittmann", "Jared Jennings"]
    s.files =  FileList["[A-Z]*", "{lib,test}/**/*"]
  end
rescue LoadError
  puts "Jeweler, or one of its dependencies, is not available. Install it with: sudo gem install technicalpickles-jeweler -s http://gems.github.com"
end
