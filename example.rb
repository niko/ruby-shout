#!/usr/bin/ruby

# example.rb
# jared jennings
# 29 aug 2001

require 'shout'

BLOCKSIZE = 16384

s = Shout.new
s.host = "localhost"
s.port = 8000
s.user = "random"
s.pass = "hackme"

s.connect

ARGV.each do |filename|
	File.open(filename) do |file|
		puts "sending data from #{filename}"

		while data = file.read(BLOCKSIZE)
			s.send data
			s.sync
		end
	end
end

s.disconnect
