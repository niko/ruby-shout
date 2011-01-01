# Generated by jeweler
# DO NOT EDIT THIS FILE DIRECTLY
# Instead, edit Jeweler::Tasks in Rakefile, and run the gemspec command
# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = %q{ruby-shout}
  s.version = "2.2.0"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Jared Jennings", "Niko Dittmann"]
  s.date = %q{2011-01-01}
  s.description = %q{Ruby bindings for libshout 2, a "Library which can be used to write a source client like ices" for Icecast (http://www.icecast.org/download.php).}
  s.email = %q{mail@niko-dittmann.com}
  s.extensions = ["ext/extconf.rb"]
  s.extra_rdoc_files = [
    "README.textile"
  ]
  s.files = [
    "README.textile",
     "Rakefile",
     "VERSION",
     "ext/extconf.rb",
     "ext/shout_ext.c",
     "lib/shout.rb"
  ]
  s.homepage = %q{http://github.com/niko/ruby-shout}
  s.rdoc_options = ["--charset=UTF-8"]
  s.require_paths = ["lib"]
  s.rubyforge_project = %q{ruby-shout}
  s.rubygems_version = %q{1.3.7}
  s.summary = %q{Send audio over the network to an Icecast server}
  s.test_files = [
    "spec/accessors_spec.rb",
     "spec/build_spec.rb",
     "spec/integration_spec.rb",
     "spec/spec_helper.rb"
  ]

  if s.respond_to? :specification_version then
    current_version = Gem::Specification::CURRENT_SPECIFICATION_VERSION
    s.specification_version = 3

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
    else
    end
  else
  end
end

