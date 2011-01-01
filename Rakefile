require 'rubygems'
require 'rake/gempackagetask'
require 'spec/rake/spectask'

$TESTING_MINIUNIT = true

desc "Run spec with specdoc output"
Spec::Rake::SpecTask.new do |t|
  spec_files = Dir.glob('spec/*_spec.rb')
  t.spec_files = spec_files
  t.spec_opts << '--format specdoc -c'
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
    s.files       = FileList["[A-Z]*", "{ext}/*", "{lib}/*"]
    
  end
rescue LoadError
  puts "Jeweler not available. Install it with: gem install jeweler"
end
