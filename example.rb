#!/usr/bin/env ruby

# Stream all the files given on the commandline to the Icecast server on localhost. 
# 
# Use the mp3 in the test directory:
# 
#   ./example.rb test/test.mp3

require 'rubygems'
require 'shout'

BLOCKSIZE = 16384

s = Shout.new
s.host = "localhost"
s.port = 8000
s.mount = "/example"
s.user = "source"
s.pass = "hackme"
s.format = Shout::MP3

s.connect

puts "open VLC and open network -> http://localhost:8000/example"

ARGV.each do |filename|
  File.open(filename) do |file|
    puts "sending data from #{filename}"
    m = ShoutMetadata.new
    m.add 'filename', filename
    s.metadata = m

    while data = file.read(BLOCKSIZE)
      s.send data
      s.sync
    end
  end
end

s.disconnect
