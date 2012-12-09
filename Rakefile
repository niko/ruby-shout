require 'rubygems'
require 'rake/gempackagetask'
require 'rspec/core/rake_task'

desc "Run spec with documentation output"
RSpec::Core::RakeTask.new do |t|
  t.pattern = Dir.glob('spec/*_spec.rb')
  t.rspec_opts = '--format documentation -c'
end

desc "Run the build spec"
RSpec::Core::RakeTask.new 'spec:build' do |t|
  t.pattern = ['spec/build_spec.rb']
  t.rspec_opts = '--format documentation -c'
end

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
    s.files       = FileList["[A-Z]*", "{ext}/*", "{lib}/**/*"]
    
  end
rescue LoadError
  puts "Jeweler not available. Install it with: gem install jeweler"
end
