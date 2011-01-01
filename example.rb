# -*- encoding : utf-8 -*-

#!/usr/bin/env ruby

# Stream all the files given on the commandline to the Icecast server on localhost. 
# 
# Use the mp3 in the test directory:
# 
#   ./example.rb spec/test.mp3

BASE_DIR = File.expand_path File.dirname(__FILE__)
VERSION = File.open(File.join(BASE_DIR, 'VERSION')).readline.strip
require File.join(BASE_DIR, "spec/test_gem_installation/gems/ruby-shout-#{VERSION}/lib/shout")

BLOCKSIZE = 16384

s = Shout.new
s.mount = "/example"
# s.charset = "UTF-8"
# s.mount = "/utf8"
s.host = "192.168.178.34"
s.user = "source"
s.pass = "hackme"
s.format = Shout::MP3
s.description ='çaffé düdeldø … dikşîne ΞŁΞϾТЯФЛłϾ MUSłϾ  ☼ ☺'

s.connect

puts "open VLC and open network -> http://#{s.host}:#{s.port}/example"

ARGV.each do |filename|
  File.open(filename) do |file|
    puts "sending data from #{filename}"
    m = ShoutMetadata.new
    m.add 'filename', filename
    m.add 'title', 'title ☼ ☺'
    s.metadata = m

    while data = file.read(BLOCKSIZE)
      s.send data
      s.sync
    end
  end
end

s.disconnect
