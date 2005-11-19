#!/usr/bin/ruby

# Stream all the files given on the commandline to the Icecast server on
# localhost. 

require 'rubygems'
require 'shout'

BLOCKSIZE = 16384

s = Shout.new
s.host = "localhost"
s.port = 8000
s.mount = "/example.ogg"
s.user = "source"
s.pass = "hackme"

s.connect

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
